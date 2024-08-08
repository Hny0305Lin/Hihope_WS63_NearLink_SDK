/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag securec connect cmd.
 * This file should be changed only infrequently and with great care.
 */

#include "diag_cmd_password.h"
#include "securec.h"
#include "diag.h"
#include "soc_diag_cmd_id.h"
#include "diag_adapt_layer.h"
#include "diag_cmd_password_st.h"
#include "diag_ind_src.h"
#include "errcode.h"
#include "diag_dfx.h"
#include "dfx_adapt_layer.h"
#include "diag_filter.h"
#ifdef CONFIG_DIAG_WITH_SECURE
#include "key_id.h"
#include "nv_storage.h"
#include "srp.h"
#include "diag_secure.h"
#endif
#ifdef CONFIG_DIAG_WITH_SECURE

const char *g_conn_user_name = "admin";
const uint32_t g_gn_type = SRP_GN_3072;

typedef struct {
    bignum *bn_u;
    bignum *bn_vertify;
    bignum *bn_ss;
} diag_bignum_bn;

typedef struct {
    uint8_t *byte_n;
    uint8_t *byte_g;
    uint8_t *pub_a;
    uint8_t *pub_b;
} diag_byte_pub;
typedef struct {
    diag_cmd_change_pwd_req_stru_t *req;
    diag_cmd_change_pwd_ind_stru_t ind;
    ext_nv_secure_conn_t nv_conn_old;
    ext_nv_secure_conn_t nv_conn_new;
    bignum *bn_salt_save;
    bignum *bn_salt_new;
    bignum *bn_vertify_save;
    bignum *bn_vertify_old;
    bignum *bn_vertify_new;
    srp_gn *gn;
    diag_conn_auth_ctrl_ctx_t *auth_ctrl_ctx;
    uint8_t req_pwd_old[DIAG_PWD_MAX_LEN + 1];
    uint8_t req_pwd_new[DIAG_PWD_MAX_LEN + 1];
    uint8_t vertify_new[DIAG_CONN_VERTIFY_SIZE];
    uint8_t pad[2]; /* pad(2) */
} diag_change_pwd_temp_param_t;

bool dfx_ndm_get_connect_state(void)
{
    return FALSE;
}

bool diag_get_lock_status(uint16_t *lock_res_sec)
{
    diag_secure_ctx_t *ctx = diag_get_secure_ctx();

    if (ctx->auth_ctrl_ctx.is_locked == FALSE) {
        *lock_res_sec = 0;
        return FALSE;
    }

    uint32_t cur_sec = dfx_get_cur_second();
    if (ctx->auth_ctrl_ctx.lock_sec > cur_sec) {       /* Special processing to unlock when time rolls over */
        ctx->auth_ctrl_ctx.is_locked = FALSE;
        ctx->auth_ctrl_ctx.cur_try_times = 0;
        return FALSE;
    } else if ((cur_sec - ctx->auth_ctrl_ctx.lock_sec) >= ctx->auth_ctrl_ctx.max_lock_sec) {
        ctx->auth_ctrl_ctx.is_locked = FALSE;
        ctx->auth_ctrl_ctx.cur_try_times = 0;
        return FALSE;
    } else {
        *lock_res_sec = ctx->auth_ctrl_ctx.max_lock_sec - (uint16_t)(cur_sec - ctx->auth_ctrl_ctx.lock_sec);
        return TRUE;
    }
}

STATIC uint32_t diag_nv_write(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length)
{
    nv_key_attr_t attr = {0};
    uint16_t real_len = 0;
    uint32_t ret;

    ret = uapi_nv_get_key_attr(key, &real_len, &attr);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = uapi_nv_write_with_attr(key, kvalue, kvalue_length, &attr, NULL);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
}

STATIC errcode_t diag_pub_a_phase_1(const void * cmd_param, uint16_t param_size,
    diag_cmd_conn_rsp_pub_b_stru_t *conn_rsp_pub_b, diag_option_t *option)
{
    diag_secure_ctx_t *ctx = diag_get_secure_ctx();

    diag_cmd_conn_auth_ind_stru_t *ret_ind = &conn_rsp_pub_b->ret_ind;
    uint16_t lock_res_sec = 0;
    /* check param */
    if ((cmd_param == NULL) ||
        (param_size < sizeof(diag_cmd_conn_req_pub_a_stru_t))) {
        ret_ind->err_no = EXT_DIAG_CONN_ERR_SYS_CALL;
        return ERRCODE_DIAG_INVALID_PARAMETER;
    }

    /* is lock status */
    if (diag_get_lock_status(&lock_res_sec) == TRUE) {
        ret_ind->err_no = EXT_DIAG_CONN_ERR_ACCOUNT_LOCK;
        ret_ind->res_lock_time = lock_res_sec;
        return ERRCODE_FAIL;
    }

    /* disconnect? */
    if (ctx->srp_info.conn_fsm >= CONN_FSM_CONNECTED) {
        zdiag_set_enable(FALSE, option->peer_addr);
    }
    diag_srp_connect_info_reset();
    ctx->srp_info.conn_fsm = CONN_FSM_WAIT_PUB_A;

    return ERRCODE_SUCC;
}

/* make pub_a,rand_b, */
STATIC uint32_t diag_pub_a_phase_2(const diag_cmd_conn_req_pub_a_stru_t *req_pub_a,
    bignum **bn_vertify, ext_nv_secure_conn_t *nv_conn)
{
    diag_secure_ctx_t *ctx = diag_get_secure_ctx();
    bignum *bn_temp = NULL;
    uint8_t rand_b[SRP_RANDOM_SIZE] = {0};
    uint16_t real_len = 0;
    bn_temp = bn_bin2bn(req_pub_a->pub_a, sizeof(req_pub_a->pub_a), NULL);
    if (bn_temp == NULL) {
        return ERRCODE_FAIL;
    }
    ctx->srp_info.pub_a = bn_temp;
    /* get random b */
    if (srp_get_random_data(rand_b, sizeof(rand_b)) != ERRCODE_SUCC) {
        memset_s(rand_b, sizeof(rand_b), 1, sizeof(rand_b));
    }

    bn_temp = bn_bin2bn(rand_b, sizeof(rand_b), NULL);
    if (bn_temp == NULL) {
        return ERRCODE_FAIL;
    }
    ctx->srp_info.rand_b = bn_temp;

    /* get v */
    uint32_t ret = uapi_nv_read(NV_ID_SECURE_CONN, sizeof(ext_nv_secure_conn_t), &real_len, (uint8_t *)nv_conn);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    /* make bn_vertify */
    *bn_vertify = bn_bin2bn(nv_conn->conn_vertify, sizeof(nv_conn->conn_vertify), NULL);
    if (*bn_vertify == NULL) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

STATIC uint32_t diag_pub_a_phase_3(diag_cmd_conn_rsp_pub_b_stru_t *conn_rsp_pub_b, bignum *bn_vertify,
    const ext_nv_secure_conn_t *nv_conn)
{
    srp_gn *gn = srp_get_default_gn(g_gn_type);
    diag_secure_ctx_t *ctx = diag_get_secure_ctx();
    bignum *bn_pub_b = NULL;
    diag_cmd_conn_auth_ind_stru_t *ret_ind = &conn_rsp_pub_b->ret_ind;

    /* calc PUB_B */
    bn_pub_b = srp_calc_b(ctx->srp_info.rand_b, gn->n, gn->g, bn_vertify);
    if (bn_pub_b == NULL) {
        ret_ind->err_no = EXT_DIAG_CONN_ERR_SYS_CALL;
        return ERRCODE_FAIL;
    }

    if (!srp_verify_b_mod_n(bn_pub_b, gn->n)) {
        ret_ind->err_no = EXT_DIAG_CONN_ERR_SYS_CALL;
        return ERRCODE_FAIL;
    }

    if (bn_ucmp(bn_pub_b, gn->n) >= 0) {
        zdiag_dfx_conn_bu_inc();
    }

    ctx->srp_info.pub_b = bn_pub_b;

    /* get salt */
    /* send salt and PUB_B */
    (void)memcpy_s(conn_rsp_pub_b->salt, sizeof(conn_rsp_pub_b->salt),
        nv_conn->conn_salt, sizeof(nv_conn->conn_salt));
    conn_rsp_pub_b->salt_len = nv_conn->salt_len;
    bn_bn2bin(bn_pub_b, conn_rsp_pub_b->pub_b);
    ret_ind->err_no = EXT_DIAG_CONN_ERR_OK;
    return ERRCODE_SUCC;
}
#endif
STATIC errcode_t diag_cmd_password_pub_a(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                         diag_option_t *option)
{
    diag_cmd_conn_rsp_pub_b_stru_t conn_rsp_pub_b;
    errcode_t ret;

    memset_s(&conn_rsp_pub_b, sizeof(diag_cmd_conn_rsp_pub_b_stru_t), 0, sizeof(diag_cmd_conn_rsp_pub_b_stru_t));

#ifdef CONFIG_DIAG_WITH_SECURE
    diag_secure_ctx_t *ctx = diag_get_secure_ctx();
    diag_cmd_conn_auth_ind_stru_t *ret_ind = &(conn_rsp_pub_b.ret_ind);
    ext_nv_secure_conn_t nv_conn;
    bignum *bn_vertify = NULL;
    if (diag_pub_a_phase_1(cmd_param, cmd_param_size, &conn_rsp_pub_b, option) != ERRCODE_SUCC) {
        goto final;
    }
    if (diag_pub_a_phase_2((diag_cmd_conn_req_pub_a_stru_t *)cmd_param, &bn_vertify, &nv_conn) != ERRCODE_SUCC) {
        conn_rsp_pub_b.ret_ind.err_no = EXT_DIAG_CONN_ERR_SYS_CALL;
        goto final;
    }
    ret = diag_pub_a_phase_3(&conn_rsp_pub_b, bn_vertify, &nv_conn);
final:
#else
    conn_rsp_pub_b.ret_ind.err_no = EXT_DIAG_CONN_ERR_OK;
    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
#endif
#ifndef SUPPORT_DIAG_V2_PROTOCOL
    msp_diag_ack_param_t ack;
    ack.sn = 0;
    ack.ctrl = 0;
    ack.cmd_id = DIAG_CMD_CONNECT_RANDOM;
    ack.param_size = (uint16_t)sizeof(diag_cmd_conn_rsp_pub_b_stru_t);
    ack.param = (uint8_t *)&conn_rsp_pub_b;
    ret = uapi_diag_report_ack(&ack, option);
#else
    ret = uapi_diag_report_packet_direct(DIAG_CMD_CONNECT_RANDOM, option, (uint8_t *)&conn_rsp_pub_b,
                                         (uint16_t)sizeof(diag_cmd_conn_rsp_pub_b_stru_t));
#endif
#ifdef CONFIG_DIAG_WITH_SECURE
    if (ret == ERRCODE_SUCC && ret_ind->err_no == EXT_DIAG_CONN_ERR_OK) {
        ctx->srp_info.conn_fsm = CONN_FSM_WAIT_REQ_M1;
        dfx_timer_start(&ctx->diag_secure_timer, DIAG_SECURE_SRP_TIMEOUT);
    } else {
        diag_srp_connect_info_reset();
    }

    bn_free(bn_vertify);
    if (ret_ind->err_no == EXT_DIAG_CONN_ERR_SYS_CALL) {
        zdiag_dfx_conn_except();
    }
#endif
    return ret;
}

#ifdef CONFIG_DIAG_WITH_SECURE
STATIC uint32_t diag_malloc_bn_ss(diag_bignum_bn *bignum_bn, uint8_t **ss, ext_nv_secure_conn_t *nv_conn, uint8_t *ks,
    uint32_t len)
{
    srp_gn *gn = srp_get_default_gn(g_gn_type);
    diag_secure_ctx_t *ctx = diag_get_secure_ctx();
    uint16_t real_len = 0;

    if (len != DIAG_CONN_KEY_HASH_SIZE) {
        return ERRCODE_FAIL;
    }
    /* calc u */
    /* Code commented: Hash(a, b); a = g^a; b = kv + g^b */
    bignum_bn->bn_u = srp_calc_u(ctx->srp_info.pub_a, ctx->srp_info.pub_b, gn->n);
    if (bignum_bn->bn_u == NULL) {
        return ERRCODE_FAIL;
    }

    /* get v */
    uint32_t ret = uapi_nv_read(NV_ID_SECURE_CONN, sizeof(ext_nv_secure_conn_t), &real_len, (uint8_t *)nv_conn);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    bignum_bn->bn_vertify = bn_bin2bn(nv_conn->conn_vertify, sizeof(nv_conn->conn_vertify), NULL);
    if (bignum_bn->bn_vertify == NULL) {
        return ERRCODE_FAIL;
    }

    /* calc Ss */
    /* Code commented: Ks = Hash(Ss); Ss = (a*V^u)^b */
    bignum_bn->bn_ss = srp_calc_server_key(ctx->srp_info.pub_a, bignum_bn->bn_vertify, bignum_bn->bn_u,
        ctx->srp_info.rand_b, gn->n);
    if (bignum_bn->bn_ss == NULL) {
        return ERRCODE_FAIL;
    }

    uint32_t bytes_ss = (uint32_t)bn_num_bytes(bignum_bn->bn_ss);
    *ss = dfx_malloc(0, bytes_ss);
    if (*ss == NULL) {
        return ERRCODE_FAIL;
    }

    bn_bn2bin(bignum_bn->bn_ss, *ss);

    /* calc Ks */
    ret = uapi_hash_sha256(*ss, bytes_ss, ks);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

STATIC uint32_t diag_malloc_byte_and_pub(diag_byte_pub *byte_pub, uint8_t *hash_user_name,
                                         uint8_t *xor_n_g, uint32_t len)
{
    diag_secure_ctx_t *ctx = diag_get_secure_ctx();
    srp_gn *gn = srp_get_default_gn(g_gn_type);
    uint8_t hash_g[DIAG_CONN_KEY_HASH_SIZE] = {0};
    uint8_t hash_n[DIAG_CONN_KEY_HASH_SIZE] = {0};

    if (len != DIAG_CONN_KEY_HASH_SIZE) {
        return ERRCODE_FAIL;
    }

    /* check M1, compute M2 */
    /* code: M1=H(H(n) xor H(g),H(I),s,a,b, Kc) */
    byte_pub->byte_n = (uint8_t *)dfx_malloc(0, (uint32_t)bn_num_bytes(gn->n));
    if (byte_pub->byte_n == NULL) {
        return ERRCODE_FAIL;
    }

    bn_bn2bin(gn->n, byte_pub->byte_n);

    if (uapi_hash_sha256(byte_pub->byte_n, (uint32_t)bn_num_bytes(gn->n), hash_n) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    byte_pub->byte_g = (uint8_t *)dfx_malloc(0, (uint32_t)bn_num_bytes(gn->g));
    if (byte_pub->byte_g == NULL) {
        return ERRCODE_FAIL;
    }

    bn_bn2bin(gn->g, byte_pub->byte_g);

    if (uapi_hash_sha256(byte_pub->byte_g, (uint32_t)bn_num_bytes(gn->g), hash_g) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    for (uint8_t i = 0; i < DIAG_CONN_KEY_HASH_SIZE; i++) {
        xor_n_g[i] = hash_n[i] ^ hash_g[i];
    }

    if (uapi_hash_sha256((uint8_t *)g_conn_user_name, strlen(g_conn_user_name), hash_user_name) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    byte_pub->pub_a = (uint8_t *)dfx_malloc(0, (uint32_t)bn_num_bytes(ctx->srp_info.pub_a));
    if (byte_pub->pub_a == NULL) {
        return ERRCODE_FAIL;
    }
    bn_bn2bin(ctx->srp_info.pub_a, byte_pub->pub_a);

    byte_pub->pub_b = (uint8_t *)dfx_malloc(0, (uint32_t)bn_num_bytes(ctx->srp_info.pub_b));
    if (byte_pub->pub_b == NULL) {
        return ERRCODE_FAIL;
    }
    bn_bn2bin(ctx->srp_info.pub_b, byte_pub->pub_b);

    return ERRCODE_SUCC;
}

STATIC void set_auth_ctrl_and_conn_rsp_m(diag_conn_auth_ctrl_ctx_t *auth_ctrl, diag_cmd_conn_rsp_m_stru_t *conn_rsp_m)
{
    auth_ctrl->cur_try_times++;
    if (auth_ctrl->cur_try_times >= auth_ctrl->max_try_times) {
        auth_ctrl->is_locked = TRUE;
        auth_ctrl->lock_sec = dfx_get_cur_second();
        conn_rsp_m->ret_ind.err_no = EXT_DIAG_CONN_ERR_ACCOUNT_LOCK;
        conn_rsp_m->ret_ind.res_lock_time = DIAG_CONN_AUTH_MAX_LOCK_SEC;
    } else {
        conn_rsp_m->ret_ind.err_no = EXT_DIAG_CONN_ERR_PASS_WORD_WRONG;
        conn_rsp_m->ret_ind.res_try_cnt = auth_ctrl->max_try_times - auth_ctrl->cur_try_times;
    }
    conn_rsp_m->ret = ERRCODE_FAIL;
}

STATIC uint32_t diag_host_hash_update(const diag_byte_pub *pub,
    const uint8_t *m1,
    diag_cmd_conn_rsp_m_stru_t *conn_rsp_m,
    const uint8_t *ks,
    const uint32_t len)
{
    conn_rsp_m->ret = ERRCODE_SUCC;
    conn_rsp_m->ret_ind.err_no = EXT_DIAG_CONN_ERR_OK;
    diag_secure_ctx_t *ctx = diag_get_secure_ctx();

    /* save key in context, then use as aes key */
    if (memcpy_s(ctx->srp_info.key, sizeof(ctx->srp_info.key), ks, sizeof(ctx->srp_info.key)) != EOK) {
        conn_rsp_m->ret = ERRCODE_FAIL;
        conn_rsp_m->ret_ind.err_no = EXT_DIAG_CONN_ERR_SYS_CALL;
        return ERRCODE_FAIL;
    }
    /* calc M2: M2=H(a, M1 , Ks) */
    uint8_t m2[DIAG_CONN_KEY_HASH_SIZE] = {0};
    uint32_t hash_handle = 0;

    uint32_t ret = uapi_hash_start(&hash_handle);
    ret |= uapi_hash_update(hash_handle, pub->pub_a, (uint32_t)bn_num_bytes(ctx->srp_info.pub_a));
    ret |= uapi_hash_update(hash_handle, m1, DIAG_CONN_KEY_HASH_SIZE);
    ret |= uapi_hash_update(hash_handle, ks, DIAG_CONN_KEY_HASH_SIZE);
    ret |= uapi_hash_final(hash_handle, m2, DIAG_CONN_KEY_HASH_SIZE);
    if (ret != ERRCODE_SUCC) {
        conn_rsp_m->ret = ERRCODE_FAIL;
        conn_rsp_m->ret_ind.err_no = EXT_DIAG_CONN_ERR_SYS_CALL;
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(conn_rsp_m->rsp_m2, sizeof(conn_rsp_m->rsp_m2), m2, sizeof(m2));
    return ERRCODE_SUCC;
}

STATIC void diag_check_connect_state(diag_cmd_conn_rsp_m_stru_t *conn_rsp_m)
{
#if defined(PRODUCT_CFG_PRODUCT_TYPE_CCO) || defined(PRODUCT_CFG_PRODUCT_TYPE_STA)
    if (dfx_ndm_get_connect_state() == TRUE) {
        conn_rsp_m->ret_ind.err_no = EXT_DIAG_CONN_ERR_BUSY;
    }
#endif
    return;
}

STATIC void send_ack_packet_when_faild(diag_cmd_conn_rsp_m_stru_t *conn_rsp_m,
    diag_secure_ctx_t *ctx, diag_option_t *option)
{
    uint32_t ret;
#ifndef SUPPORT_DIAG_V2_PROTOCOL
    msp_diag_ack_param_t ack;
    ack.sn = 0;
    ack.ctrl = 0;
    ack.cmd_id = DIAG_CMD_CONNECT_M_CHECK;
    ack.param_size = sizeof(diag_cmd_conn_rsp_m_stru_t);
    ack.param = (uint8_t *)conn_rsp_m;
    ret = uapi_diag_report_ack(&ack, option);
#else
    ret = uapi_diag_report_packet_direct(DIAG_CMD_CONNECT_M_CHECK, option, (uint8_t *)conn_rsp_m,
                                         (uint16_t)sizeof(diag_cmd_conn_rsp_m_stru_t));
#endif
    if ((ret == ERRCODE_SUCC) && ((conn_rsp_m->ret_ind.err_no == EXT_DIAG_CONN_ERR_OK) ||
        (conn_rsp_m->ret_ind.err_no == EXT_DIAG_CONN_ERR_BUSY))) {
        ctx->srp_info.conn_fsm = CONN_FSM_WAIT_CONN;
        diag_aes_gcm_setkey(ctx->srp_info.key, AES_KEY_LEN);
        ctx->auth_ctrl_ctx.cur_try_times = 0;
        (void)memset_s(ctx->srp_info.key, AES_KEY_LEN, 0, sizeof(ctx->srp_info.key));
    }
}

STATIC void free_byte_and_pub(uint8_t *ss, diag_byte_pub *byte_pub)
{
    dfx_free(0, ss);

    dfx_free(0, byte_pub->byte_n);
    byte_pub->byte_n = NULL;

    dfx_free(0, byte_pub->byte_g);
    byte_pub->byte_g = NULL;

    dfx_free(0, byte_pub->pub_a);
    byte_pub->pub_a = NULL;

    dfx_free(0, byte_pub->pub_b);
    byte_pub->pub_b = NULL;
}

STATIC void free_ctx_and_bn(diag_secure_ctx_t *ctx, diag_bignum_bn *bignum_bn)
{
    bn_free(ctx->srp_info.pub_a);
    ctx->srp_info.pub_a = NULL;

    bn_free(ctx->srp_info.pub_b);
    ctx->srp_info.pub_b = NULL;

    bn_free(ctx->srp_info.rand_b);
    ctx->srp_info.rand_b = NULL;

    bn_free(bignum_bn->bn_ss);
    bignum_bn->bn_ss = NULL;

    bn_free(bignum_bn->bn_vertify);
    bignum_bn->bn_vertify = NULL;

    bn_free(bignum_bn->bn_u);
    bignum_bn->bn_u = NULL;
}

STATIC void diag_host_conn_check_req_m_fail(uint8_t *ss, diag_cmd_conn_rsp_m_stru_t *conn_rsp_m,
    diag_bignum_bn *bignum_bn, diag_byte_pub *byte_pub, diag_option_t *option)
{
    diag_secure_ctx_t *ctx = diag_get_secure_ctx();
    send_ack_packet_when_faild(conn_rsp_m, ctx, option);
    free_byte_and_pub(ss, byte_pub);
    free_ctx_and_bn(ctx, bignum_bn);

    if (conn_rsp_m->ret_ind.err_no == EXT_DIAG_CONN_ERR_SYS_CALL) {
        zdiag_dfx_conn_except();
    }
}

STATIC uint32_t diag_host_conn_check_req_m(const diag_cmd_conn_req_m_stru_t *req_m,
    diag_secure_ctx_t *ctx, diag_option_t *option)
{
    diag_cmd_conn_rsp_m_stru_t conn_rsp_m = { 0 };
    diag_bignum_bn bignum_bn = { 0 };
    diag_byte_pub byte_pub = { 0 };
    uint8_t *ss = NULL;
    ext_nv_secure_conn_t nv_conn = { 0 };
    uint8_t ks[DIAG_CONN_KEY_HASH_SIZE] = {0};
    uint8_t xor_n_g[DIAG_CONN_KEY_HASH_SIZE] = {0};
    uint8_t hash_user_name[DIAG_CONN_KEY_HASH_SIZE] = {0};
    uint8_t m1[DIAG_CONN_KEY_HASH_SIZE] = {0};
    uint32_t hash_handle = 0;

    if (diag_malloc_bn_ss(&bignum_bn, &ss, &nv_conn, ks, DIAG_CONN_KEY_HASH_SIZE) != ERRCODE_SUCC) {
        conn_rsp_m.ret_ind.err_no = EXT_DIAG_CONN_ERR_SYS_CALL;
        goto final;
    }
    if (diag_malloc_byte_and_pub(&byte_pub, hash_user_name, xor_n_g, DIAG_CONN_KEY_HASH_SIZE) != ERRCODE_SUCC) {
        conn_rsp_m.ret_ind.err_no = EXT_DIAG_CONN_ERR_SYS_CALL;
        goto final;
    }
    uint32_t ret = uapi_hash_start(&hash_handle);
    ret |= uapi_hash_update(hash_handle, xor_n_g, DIAG_CONN_KEY_HASH_SIZE);
    ret |= uapi_hash_update(hash_handle, hash_user_name, DIAG_CONN_KEY_HASH_SIZE);
    ret |= uapi_hash_update(hash_handle, nv_conn.conn_salt, nv_conn.salt_len);
    ret |= uapi_hash_update(hash_handle, byte_pub.pub_a, (uint32_t)bn_num_bytes(ctx->srp_info.pub_a));
    ret |= uapi_hash_update(hash_handle, byte_pub.pub_b, (uint32_t)bn_num_bytes(ctx->srp_info.pub_b));
    ret |= uapi_hash_update(hash_handle, ks, sizeof(ks));
    ret |= uapi_hash_final(hash_handle, m1, DIAG_CONN_KEY_HASH_SIZE);
    if (ret != ERRCODE_SUCC) {
        conn_rsp_m.ret_ind.err_no = EXT_DIAG_CONN_ERR_SYS_CALL;
        goto final;
    }

    if (memcmp(m1, req_m->req_m1, DIAG_CONN_KEY_HASH_SIZE) != 0) {
        set_auth_ctrl_and_conn_rsp_m(&(ctx->auth_ctrl_ctx), &conn_rsp_m);
    } else {
        if (diag_host_hash_update(&byte_pub, m1, &conn_rsp_m, ks, DIAG_CONN_KEY_HASH_SIZE) != ERRCODE_SUCC) {
            goto final;
        }
    }
    diag_check_connect_state(&conn_rsp_m);
final:
    diag_host_conn_check_req_m_fail(ss, &conn_rsp_m, &bignum_bn, &byte_pub, option);
    ss = NULL;
    return ERRCODE_DIAG_CONSUMED;
}
#endif

STATIC errcode_t diag_cmd_password_m_check(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
    diag_option_t *option)
{
    diag_cmd_conn_rsp_m_stru_t conn_rsp_m = { 0 };
    errcode_t ret;
#ifdef CONFIG_DIAG_WITH_SECURE
    diag_cmd_conn_req_m_stru_t *req_m = (diag_cmd_conn_req_m_stru_t *)cmd_param;
    diag_secure_ctx_t *ctx = diag_get_secure_ctx();
    uint16_t lock_res_sec = 0;

    if (cmd_param == NULL || cmd_param_size < sizeof(diag_cmd_conn_req_m_stru_t)) {
        return ERRCODE_DIAG_INVALID_PARAMETER;
    }
    if (ctx->srp_info.conn_fsm != CONN_FSM_WAIT_REQ_M1) {
        return ERRCODE_FAIL;
    }
    if (diag_get_lock_status(&lock_res_sec) == TRUE) {
        conn_rsp_m.ret_ind.err_no = EXT_DIAG_CONN_ERR_ACCOUNT_LOCK;
        conn_rsp_m.ret_ind.res_lock_time = lock_res_sec;
        send_ack_packet_when_faild(&conn_rsp_m, ctx, option);
        return ERRCODE_DIAG_CONSUMED;
    }
    return diag_host_conn_check_req_m(req_m, ctx, option);
#else
    memset_s(&conn_rsp_m, sizeof(diag_cmd_conn_rsp_m_stru_t), 0, sizeof(diag_cmd_conn_rsp_m_stru_t));

    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);

    conn_rsp_m.ret = ERRCODE_SUCC;
    conn_rsp_m.ret_ind.err_no = EXT_DIAG_CONN_ERR_OK;
#ifndef SUPPORT_DIAG_V2_PROTOCOL
    msp_diag_ack_param_t ack;
    ack.sn = 0;
    ack.ctrl = 0;
    ack.cmd_id = DIAG_CMD_CONNECT_M_CHECK;
    ack.param_size = (uint16_t)sizeof(diag_cmd_conn_rsp_m_stru_t);
    ack.param = (uint8_t *)&conn_rsp_m;
    ret = uapi_diag_report_ack(&ack, option);
#else
    ret = uapi_diag_report_packet_direct(DIAG_CMD_CONNECT_M_CHECK, option, (uint8_t *)&conn_rsp_m,
                                         (uint16_t)sizeof(diag_cmd_conn_rsp_m_stru_t));
#endif
    return ret;
#endif
}

#ifdef CONFIG_DIAG_WITH_SECURE
STATIC uint32_t diag_change_pwd_check_phase1(uint16_t cmd_id, void * cmd_param, uint16_t param_size,
    diag_option_t *option)
{
    unused(cmd_id);
    uint32_t ret;
    if (param_size != sizeof(diag_cmd_change_pwd_req_stru_t)) {
        ret = ERRCODE_DIAG_INVALID_PARAMETER;
        goto failed;
    }
    if (!zdiag_is_enable()) { /* ps:why check diag connect status in cmd? */
        ret = ERRCODE_FAIL;
        goto failed;
    }
    return ERRCODE_SUCC;
failed:
    if (cmd_param != NULL && param_size != 0) {
        memset_s(cmd_param, param_size, 0x0, param_size);
    }
    return ret;
}

STATIC uint32_t diag_change_pwd_check_phase2(diag_change_pwd_temp_param_t *param)
{
    uint16_t lock_res_sec = 0;
    if (diag_get_lock_status(&lock_res_sec) == TRUE) {
        param->ind.err_no = EXT_DIAG_CONN_ERR_ACCOUNT_LOCK;
        param->ind.res_lock_time = lock_res_sec;
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/* *
 * 建议口令复杂度要求：
 * 口令长度至少8个字符；
 * 口令必须包含如下至少两种字符的组合：
 * 至少一个小写字母
 * 至少一个大写字母
 * 至少一个数字
 * 至少一个特殊字符：`~!@#$%^&*()-_=+\|[{}];:'",<.>/?和空格
 */
#define MIN_PWD_LEN 8
STATIC bool is_pwd_legal(const char *pwd)
{
    uint8_t lower_flag = 0;
    uint8_t num_flag = 0;
    uint8_t upper_flag = 0;
    uint8_t special_flag = 0;
    uint32_t pwd_len;
    uint32_t i;

    if (pwd == NULL) {
        return FALSE;
    }

    pwd_len = strlen(pwd);
    if (pwd_len < MIN_PWD_LEN) {
        return FALSE;
    }

    for (i = 0; i < pwd_len; i++) {
        if (islower(pwd[i])) {
            lower_flag = 1;
        } else if (isdigit(pwd[i])) {
            num_flag = 1;
        } else if (isupper(pwd[i])) {
            upper_flag = 1;
        } else if (ispunct(pwd[i]) || pwd[i] == ' ') {
            special_flag = 1;
        } else {
            return FALSE;
        }
    }

    if (lower_flag + num_flag + upper_flag + special_flag >= 2) { /* There needs 2 types of char. */
        return TRUE;
    }

    return FALSE;
}

STATIC uint32_t diag_change_pwd_phase1(diag_change_pwd_temp_param_t *param)
{
    diag_cmd_change_pwd_req_stru_t *req = param->req;
    diag_cmd_change_pwd_ind_stru_t *ind = &(param->ind);
    uint16_t real_len = 0;

    if (req->old_pwd_len > DIAG_PWD_MAX_LEN || req->new_pwd_len > DIAG_PWD_MAX_LEN) {
        ind->err_no = EXT_CHANGE_PWD_ERR_PASSWORD_FORMAT_ERR;
        return ERRCODE_FAIL;
    }

    if (memcpy_s(param->req_pwd_old, DIAG_PWD_MAX_LEN, req->old_pwd, req->old_pwd_len) != EOK) {
        ind->err_no = EXT_CHANGE_PWD_ERR_SYS_CALL;
        return ERRCODE_FAIL;
    }
    if (memcpy_s(param->req_pwd_new, DIAG_PWD_MAX_LEN, req->new_pwd, req->new_pwd_len) != EOK) {
        ind->err_no = EXT_CHANGE_PWD_ERR_SYS_CALL;
        return ERRCODE_FAIL;
    }

    param->req_pwd_new[DIAG_PWD_MAX_LEN] = 0;
    param->req_pwd_old[DIAG_PWD_MAX_LEN] = 0;
    if (strcmp((char*)param->req_pwd_old, (char*)param->req_pwd_new) == EOK) {
        ind->err_no = EXT_CHANGE_PWD_ERR_PASSWORD_FORMAT_ERR;
        return ERRCODE_FAIL;
    }
    if (FALSE == is_pwd_legal((const char *)param->req_pwd_new)) {
        ind->err_no = EXT_CHANGE_PWD_ERR_PASSWORD_FORMAT_ERR;
        return ERRCODE_FAIL;
    }

    uint32_t ret;
    ret = uapi_nv_read(NV_ID_SECURE_CONN, sizeof(ext_nv_secure_conn_t), &real_len, (uint8_t *) & (param->nv_conn_old));
    if (ret != ERRCODE_SUCC) {
        ind->err_no = EXT_CHANGE_PWD_ERR_SYS_CALL;
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

STATIC uint32_t diag_change_pwd_phase2(diag_change_pwd_temp_param_t *param)
{
    diag_cmd_change_pwd_ind_stru_t *ind = &(param->ind);
    param->bn_salt_save = bn_bin2bn(param->nv_conn_old.conn_salt, (int)param->nv_conn_old.salt_len, NULL);
    param->bn_vertify_save =
        bn_bin2bn(param->nv_conn_old.conn_vertify, sizeof(param->nv_conn_old.conn_vertify), NULL);
    param->bn_salt_new = bn_bin2bn(param->req->new_salt, param->req->new_salt_len, NULL);
    if (param->bn_salt_save == NULL || param->bn_vertify_save == NULL || param->bn_salt_new == NULL) {
        ind->err_no = EXT_CHANGE_PWD_ERR_SYS_CALL;
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

STATIC uint32_t diag_change_pwd_check_old_pwd(diag_change_pwd_temp_param_t *param)
{
    diag_cmd_change_pwd_ind_stru_t *ind = &(param->ind);

    /* verify old pwd. */
    if (!srp_create_verifier_bn(g_conn_user_name, (const char *)param->req_pwd_old, &param->bn_salt_save,
        &param->bn_vertify_old, param->gn)) {
        ind->err_no = EXT_CHANGE_PWD_ERR_SYS_CALL;
        return ERRCODE_FAIL;
    }
    if (bn_cmp(param->bn_vertify_save, param->bn_vertify_old) != 0) {
        param->auth_ctrl_ctx->cur_try_times++;
        if (param->auth_ctrl_ctx->cur_try_times >= param->auth_ctrl_ctx->max_try_times) {
            param->auth_ctrl_ctx->is_locked = TRUE;
            param->auth_ctrl_ctx->lock_sec = dfx_get_cur_second();
            ind->err_no = EXT_DIAG_CONN_ERR_ACCOUNT_LOCK;
            ind->res_lock_time = DIAG_CONN_AUTH_MAX_LOCK_SEC;
        } else {
            ind->err_no = EXT_CHANGE_PWD_ERR_PASSWORD_WRONG;
            ind->res_try_cnt = DIAG_CONN_AUTH_MAX_TRY_TIME - param->auth_ctrl_ctx->cur_try_times;
        }
        return ERRCODE_FAIL;
    }
    param->auth_ctrl_ctx->cur_try_times = 0;
    return ERRCODE_SUCC;
}

STATIC uint32_t diag_change_pwd_save_new_code(diag_change_pwd_temp_param_t *param)
{
    diag_cmd_change_pwd_ind_stru_t *ind = &(param->ind);

    uint8_t new_salt[DIAG_CONN_SALT_SIZE];
    if (srp_get_random_data(new_salt, DIAG_CONN_SALT_SIZE) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    param->bn_salt_new = bn_bin2bn(new_salt, DIAG_CONN_SALT_SIZE, NULL);

    /* 保存新密码 */
    if (!srp_create_verifier_bn(g_conn_user_name, (const char *)param->req_pwd_new, &param->bn_salt_new,
        &param->bn_vertify_new, param->gn)) {
        ind->err_no = EXT_CHANGE_PWD_ERR_SYS_CALL;
        return ERRCODE_FAIL;
    }

    bn_bn2bin(param->bn_vertify_new, param->vertify_new);
    if (memcpy_s(param->nv_conn_new.conn_salt, sizeof(param->nv_conn_new.conn_salt), new_salt,
        DIAG_CONN_SALT_SIZE) != EOK) {
        ind->err_no = EXT_CHANGE_PWD_ERR_SYS_CALL;
        return ERRCODE_FAIL;
    }
    param->nv_conn_new.salt_len = DIAG_CONN_SALT_SIZE;
    (void)memcpy_s(param->nv_conn_new.conn_vertify, sizeof(param->nv_conn_new.conn_vertify), param->vertify_new,
        sizeof(param->vertify_new));
    uint32_t ret = diag_nv_write(NV_ID_SECURE_CONN, (uint8_t *)&param->nv_conn_new, sizeof(param->nv_conn_new));
    if (ret != ERRCODE_SUCC) {
        ind->err_no = EXT_CHANGE_PWD_ERR_SYS_CALL;
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

STATIC uint32_t diag_change_pwd_final(diag_change_pwd_temp_param_t *param,
    void * cmd_param, uint16_t param_size, diag_option_t *option)
{
#ifndef SUPPORT_DIAG_V2_PROTOCOL
    msp_diag_ack_param_t ack;
    ack.sn = 0;
    ack.ctrl = 0;
    ack.cmd_id = DIAG_CMD_PWD_CHANGE;
    ack.param_size = (uint16_t)sizeof(diag_cmd_change_pwd_ind_stru_t);
    ack.param = (uint8_t *)&param->ind;
    uapi_diag_report_ack(&ack, option);
#else
    uapi_diag_report_packet_direct(DIAG_CMD_PWD_CHANGE, option, (uint8_t *)&param->ind,
                                   (uint16_t)sizeof(diag_cmd_change_pwd_ind_stru_t));
#endif
    bn_free(param->bn_salt_save);
    bn_free(param->bn_salt_new);
    bn_free(param->bn_vertify_save);
    bn_free(param->bn_vertify_old);
    bn_free(param->bn_vertify_new);

    memset_s(cmd_param, param_size, 0x0, param_size);
    /* 若密码修改成功，断开当前连接 */
    if (param->ind.err_no == EXT_CHANGE_PWD_ERR_OK) {
        diag_srp_connect_info_reset();
        zdiag_set_enable(false, option->peer_addr);
    }
    memset_s(param, sizeof(diag_change_pwd_temp_param_t), 0x0, sizeof(diag_change_pwd_temp_param_t));
    dfx_free(0, param);
    return ERRCODE_SUCC;
}

uint32_t diag_change_pwd(uint16_t cmd_id, void * cmd_param, uint16_t param_size, diag_option_t *option)
{
    diag_change_pwd_temp_param_t *param = NULL;
    uint32_t ret = diag_change_pwd_check_phase1(cmd_id, cmd_param, param_size, option);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    param = (diag_change_pwd_temp_param_t *)dfx_malloc(0, sizeof(diag_change_pwd_temp_param_t));
    if (param == NULL) {
        memset_s(cmd_param, param_size, 0x0, param_size);
        return ERRCODE_DIAG_NOT_ENOUGH_MEMORY;
    }
    memset_s(param, sizeof(diag_change_pwd_temp_param_t), 0, sizeof(diag_change_pwd_temp_param_t));
    param->req = cmd_param;
    param->gn = srp_get_default_gn(g_gn_type);
    param->auth_ctrl_ctx = &(diag_get_secure_ctx()->auth_ctrl_ctx);

    ret = diag_change_pwd_check_phase2(param);
    if (ret != ERRCODE_SUCC) {
        goto final;
    }
    ret = diag_change_pwd_phase1(param);
    if (ret != ERRCODE_SUCC) {
        goto final;
    }
    ret = diag_change_pwd_phase2(param);
    if (ret != ERRCODE_SUCC) {
        goto final;
    }
    ret = diag_change_pwd_check_old_pwd(param);
    if (ret != ERRCODE_SUCC) {
        goto final;
    }
    ret = diag_change_pwd_save_new_code(param);
    if (ret != ERRCODE_SUCC) {
        goto final;
    }
    /* 上报密码修改成功 */
    param->ind.err_no = EXT_CHANGE_PWD_ERR_OK;

final:
    return diag_change_pwd_final(param, cmd_param, param_size, option);
}
#endif

errcode_t diag_cmd_password(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    switch (cmd_id) {
        case DIAG_CMD_CONNECT_RANDOM:
            return diag_cmd_password_pub_a(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_CMD_CONNECT_M_CHECK:
            return diag_cmd_password_m_check(cmd_id, cmd_param, cmd_param_size, option);
#ifdef CONFIG_DIAG_WITH_SECURE
        case DIAG_CMD_PWD_CHANGE:
            return diag_change_pwd(cmd_id, cmd_param, cmd_param_size, option);
#endif
        default:
            return ERRCODE_NOT_SUPPORT;
    }
}
