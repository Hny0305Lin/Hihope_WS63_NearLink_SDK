/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: the common function implementation of drv_layer.
 *
 * Create: 2022-10-27
*/

#include "crypto_osal_adapt.h"
#include "crypto_drv_common.h"
#include "drv_trng.h"
#include "hal_common.h"
#include "hal_pke_reg.h"
#include "hal_pke.h"
#include "drv_hash.h"
#include "drv_common_pke.h"

/************************************************ pke common global variable start************************************/
pke_default_parameters *g_ecc_params = TD_NULL;
td_u32 *g_ecc_num = TD_NULL;
/************************************************ pke common global variable end************************************/
typedef struct {
    drv_pke_ecc_curve_type curve_type;
    td_bool is_support;
} pke_ecc_alg_support_t;

const drv_pke_ecc_curve *crypto_drv_pke_common_get_ecc_curve(drv_pke_ecc_curve_type curve_type)
{
    td_u32 i = 0;
    for (i = 0; i < *g_ecc_num; i++) {
        if (curve_type == g_ecc_params[i].curve_param->ecc_type) {
            return g_ecc_params[i].curve_param;
        }
    }
    return TD_NULL;
}

void crypto_drv_pke_common_arry_right_shift_value(td_u8 *k, td_u32 k_len, td_u32 shift_bit)
{
    td_u32 i = 0;
    td_u8 left_mask = ((1 << shift_bit) - 1) & 0xFF;
    td_u8 right_mask = ~((1 << shift_bit) - 1) & 0xFF;

    for (i = k_len - 1; i > 0; i--) {
        k[i] = ((k[i] & right_mask) >> shift_bit) | ((k[i - 1] & left_mask) << (CRYPTO_BITS_IN_BYTE - shift_bit));
    }
    k[0] = (k[0] & right_mask) >> shift_bit;

    return;
}

CRYPTO_STATIC void inner_bn_add_one(td_u8 *data, td_u32 len)
{
    td_u32 idx = len - 1;
    td_u8 carry = 1;
    td_u32 value;
    while (carry == 1 && idx >= 1) {
        value = data[idx] + carry;
        if (value > 0xFF) {
            carry = 1;
        } else {
            carry = 0;
        }
        data[idx] = value % (0x100);
        idx--;
    }
    data[idx] += carry;
}

/* here the data must be greater than 1. */
CRYPTO_STATIC void inner_bn_sub_one(td_u8 *data, td_u32 len)
{
    td_u32 idx = len - 1;
    td_s32 carry = 1;
    while (carry == 1 && idx > 0) {
        if (data[idx] == 0) {
            carry = 1;
            data[idx] = 0xFF;
        } else {
            carry = 0;
            data[idx]--;
        }
        idx--;
    }
    if (idx == 0 && carry != 0) {
        data[idx]--;
    }
}

void crypto_drv_pke_common_array_add_plus_minus_one(td_u8 *k, td_u32 k_len, td_s32 value)
{
    crypto_chk_return_void(k_len == 0);

    if (value == 0) {
        return;
    }

    if (value == 1) {
        inner_bn_add_one(k, k_len);
    } else if (value == -1) {
        inner_bn_sub_one(k, k_len);
    }

    return;
}

td_s32 crypto_drv_pke_common_convert_normal_scalar_to_naf(const td_u8 *k, const td_u32 k_len, td_s32 *k_naf,
    td_u32 *k_naf_len)
{
    td_s32 ret = TD_FAILURE;
    td_u32 cnt = 0;

    td_u8 *tmp = crypto_malloc(k_len + 1);
    crypto_chk_return((tmp == TD_NULL), ret, "malloc failed!\n");
    memset_s(tmp, k_len + 1, 0, k_len + 1);

    ret = memcpy_s(tmp + 1, k_len, k, k_len);
    crypto_chk_goto((ret != EOK), __EXIT, "copy data failed!\n");

    while (crypto_drv_pke_common_is_zero(tmp, k_len + 1) != TD_TRUE) {
        if (tmp[k_len] % 2 == 1) {  /* 2: to check whether the byte is odd or even. */
            /* the last byte is odd, then the whole data is odd. */
            k_naf[cnt] = (int) (2 - tmp[k_len] % CRYPTO_WORD_WIDTH);    /* 2: for normal scalar to naf trans. */
            crypto_drv_pke_common_array_add_plus_minus_one(tmp, k_len + 1, -k_naf[cnt]);
        } else {
            k_naf[cnt] = 0;
        }
        crypto_drv_pke_common_arry_right_shift_value(tmp, k_len + 1, 1);  /* 1: right shift one. */
        cnt++;
    }

    *k_naf_len = cnt;
__EXIT:
    if (tmp != TD_NULL) {
        crypto_free(tmp);
        tmp = TD_NULL;
    }
    return ret;
}

td_s32 crypto_drv_pke_common_montgomery_data_config(td_u8 *rr, td_u32 rr_len, td_u8 *rrp, td_u32 rrp_len)
{
    td_s32 ret = TD_FAILURE;
    crypto_drv_func_enter();

    drv_crypto_pke_check_param(rr == TD_NULL && rrp == TD_NULL);

    if (rr != TD_NULL) {
        ret = memset_enhance(rr, rr_len, 0x00, rr_len);
        crypto_chk_func_return(memset_enhance, ret);
        rr[0] = 1; /* 2^(2*bit_len), so it need to move left one bit, which means the highest one byte value is 1. */
    }

    if (rrp != TD_NULL) {
        ret = memset_enhance(rrp, rrp_len, 0x00, rrp_len);
        crypto_chk_func_return(memset_enhance, ret);
    }

    crypto_drv_func_exit();
    return ret;
}

/* check val whether zero or not :
        TD_SUCCESS: val is zero.
        TD_FAILURE: val is not zero.
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM): null pointer of val
*/
td_bool crypto_drv_pke_common_is_zero(const td_u8 *val, td_u32 klen)
{
    volatile td_u32 i = 0;
    volatile td_u32 res = TD_FAILURE;
    volatile td_s32 ret = TD_FAILURE;

    for (i = 0; i < klen; i++) {
        res += val[i];
    }

    val_enhance_chk(i, klen);

    ret = TD_SUCCESS + res - TD_FAILURE;
    if (ret == TD_SUCCESS) {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_void crypto_drv_pke_common_array_sub_value(td_u8 *k, td_u32 k_len, td_s32 value)
{
    crypto_chk_return_void(k_len == 0);
    crypto_dump_buffer("input data k", k, k_len);

    if (k[k_len - 1] < value) {
        k[k_len - 1] = 0xFF - value + 1 + k[k_len - 1];
        crypto_drv_pke_common_array_add_plus_minus_one(k, k_len - 1, -1);
        crypto_dump_buffer("out data k", k, k_len);
        return;
    }
    k[k_len - 1] -= value;
    crypto_dump_buffer("out data k", k, k_len);

    return;
}

td_s32 crypto_drv_pke_common_little_big_endian_trans(td_u8 *output_endian, const td_u8 *input_endian, td_u32 byte_len)
{
    td_s32 ret = TD_FAILURE;
    td_u32 i = 0;
    td_u32 j = 0;

    td_u32 random_val = 0;
    ret = drv_cipher_trng_get_random(&random_val);
    ret_enhance_chk(ret, TD_SUCCESS);

    /* according to the items about x25519 in RFC7748, the point coordinate and k are all little-endian byte array,
        which need to be transfor into big-endian before be used in calculation. */
    for (i = 0, j = random_val % byte_len; i < byte_len; i++) {
        output_endian[j] = input_endian[byte_len - 1 - j];
        j++;
        j %= byte_len;
    }

    val_enhance_chk(i, byte_len);

    return ret;
}

td_s32 crypto_drv_pke_common_byte_stream_to_int_array(const td_u8 *byte_stream, const td_u32 stream_length,
    td_u32 *int_array, td_u32 array_length)
{
    td_s32 ret = TD_FAILURE;
    td_u32 i = 0;
    crypto_drv_func_enter();

    drv_crypto_pke_check_param(stream_length % sizeof(td_u32) != 0);

    ret = memset_enhance(int_array, (array_length), 0x00, (array_length));
    crypto_chk_func_return(memset_enhance, ret);

    for (i = 0; i < stream_length; i += CRYPTO_WORD_WIDTH) {
        int_array[ i / sizeof(td_u32)] |= byte_stream[i] << 24;     /* 24: get [31:24] */
        int_array[ i / sizeof(td_u32)] |= byte_stream[i + 1] << 16; /* 16: get [23:16] */
        int_array[ i / sizeof(td_u32)] |= byte_stream[i + 2] << 8;  /* 2,8: get [15:8] */
        int_array[ i / sizeof(td_u32)] |= byte_stream[i + 3];       /* 3: get[7:0] */
    }

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

/* left is high address, right is low address. And c->length is larger one than the max(a->length, b->length) */
td_void crypto_drv_pke_common_normal_add(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *c)
{
    td_s32 i = 0;
    td_s32 j = 0;
    td_s32 k = 0;
    crypto_drv_func_enter();

    (void)memset_s(c->data, c->length, 0x00, c->length);
    for (i = a->length - 1, j = b->length - 1, k = c->length - 1; i >= 0 && j >= 0 && k >= 1; i--, j--, k--) {
        td_s32 carry = 0;
        if (a->data[i] >= (BYTE_VALUE - b->data[j])) {
            carry = b->data[j] - (BYTE_VALUE - a->data[i]); /* most value is 0xff + 0xff = 0x1fe, will cause move. */
            c->data[k - 1] += 1;
        } else {
            carry = a->data[i] + b->data[j]; /* most value is 0x80 + 0x7f = 0xff. */
        }
        if ((carry == BYTE_VALUE - 1 && c->data[k] == 1) ||
            (carry == BYTE_VALUE - 2 && c->data[k] == 2)) { // 2: carry in binary.
            /* in this condition, the add process will cause carry. */
            c->data[k] = 0;
            c->data[k - 1] += 1;
        } else {
            c->data[k] += carry;
        }
    }

    /* when out the for loop, i == 0 or j == 0 */
    for (; j >= 0 && k >= 1; j--, k--) {
        if ((b->data[j] == BYTE_VALUE - 1 && c->data[k] == 1) ||
            (b->data[j] == BYTE_VALUE - 2 && c->data[k] == 2)) {  /* 2: carry in binary. */
            /* in this condition, the add process will cause carry. */
            c->data[k] = 0;
            c->data[k - 1] += 1;
        } else {
            c->data[k] += b->data[j];
        }
    }
    for (; i >= 0 && k >= 1; i--, k--) {
        if ((a->data[i] == BYTE_VALUE - 1 && c->data[k] == 1) ||
            (a->data[i] == BYTE_VALUE - 2 && c->data[k] == 2)) {  /* 2: carry in binary. */
            /* in this condition, the add process will cause carry. */
            c->data[k] = 0;
            c->data[k - 1] += 1;
        } else {
            c->data[k] += a->data[i];
        }
    }

    crypto_drv_func_exit();
}

/* int this condition b will no more than a, and the most b = a - 1, so that c->length = a.length */
td_s32 normal_sub(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *c)
{
    td_s32 ret = TD_FAILURE;
    td_s32 i = 0;
    td_s32 j = 0;

    drv_crypto_pke_check_param(a == TD_NULL);
    drv_crypto_pke_check_param(a->data == TD_NULL);
    drv_crypto_pke_check_param(b == TD_NULL);
    drv_crypto_pke_check_param(b->data == TD_NULL);
    drv_crypto_pke_check_param(c == TD_NULL);
    drv_crypto_pke_check_param(c->data == TD_NULL);
    drv_crypto_pke_check_param(a->length != c->length);

    (void)memset_s(c->data, c->length, 0x00, c->length);
    ret = memcpy_enhance(c->data, c->length, a->data, a->length);
    crypto_chk_func_return(memcpy_enhance, ret);
    td_s32 carry = 0;
    for (i = a->length - 1, j = b->length - 1; i >= 1 && j >= 0; i--, j--) {
        if (a->data[i] < b->data[j] || a->data[i] - b->data[j] < carry) {
            /* need to call one */
            c->data[i] = c->data[i] + (BYTE_VALUE - b->data[j] - carry);
            carry = 1;  /* note next byte */
        } else {
            c->data[i] -= (b->data[j] + carry);
            carry = 0;  /* note next byte */
        }
    }
    if (j == 0) {
        /* which means the b->length == a->length */
        c->data[i] -= (b->data[j] + carry);
    }

    return TD_SUCCESS;
}

/* left shift const one offset bits, while offset is less than 64. */
static td_void left_shift_const_one(const drv_pke_data *a, const td_u32 offset)
{
    td_u32 i = offset / CRYPTO_BITS_IN_BYTE;
    td_u32 e = offset % CRYPTO_BITS_IN_BYTE;
    crypto_drv_func_enter();

    (void)memset_s(a->data, a->length, 0x00, a->length);
    a->data[a->length - i - 1] = 1 << e;

    crypto_drv_func_exit();
}

/* the input p only need the low 64bit of p. which means the low 8 bytes, the mont_param will no more than 64bit */
td_s32 crypto_drv_pke_common_set_mont_param(const drv_pke_data *p)
{
    td_s32 ret = TD_FAILURE;
    td_s32 i = 0;
    td_u32 s_len = PKE_MONT_BIT_LEN / CRYPTO_BITS_IN_BYTE;
    td_u8 s[PKE_MONT_BIT_LEN / CRYPTO_BITS_IN_BYTE];
    td_u8 p_cal[PKE_MONT_BIT_LEN / CRYPTO_BITS_IN_BYTE];
    td_u32 x_len = PKE_MONT_BIT_LEN / CRYPTO_BITS_IN_BYTE + 1;
    td_u8 x[PKE_MONT_BIT_LEN / CRYPTO_BITS_IN_BYTE + 1];
    td_u8 bit_2[PKE_MONT_BIT_LEN / CRYPTO_BITS_IN_BYTE + 1];
    td_u8 tmp[PKE_MONT_BIT_LEN / CRYPTO_BITS_IN_BYTE + 1];
    td_u8 tmp_2[PKE_MONT_BIT_LEN / CRYPTO_BITS_IN_BYTE + 1];
    drv_pke_data aa = {0};
    drv_pke_data bb = {0};
    drv_pke_data cc = {0};
    drv_pke_data dd = {0};
    drv_pke_data ff = {0};
    td_u32 mont_param[PKE_MONT_PARAM_LEN] = {0};
    crypto_drv_func_enter();

    crypto_timer_start(TIMER_ID_6, __func__);

    ret = memset_enhance(x, x_len, 0x00, x_len);
    crypto_chk_func_return(memset_enhance, ret);
    x[PKE_MONT_BIT_LEN / CRYPTO_BITS_IN_BYTE] = 1;
    ret = crypto_drv_pke_common_montgomery_data_config(bit_2, x_len, tmp, x_len);
    crypto_chk_func_return(crypto_drv_pke_common_montgomery_data_config, ret);

    crypto_timer_end(TIMER_ID_6, "Step 0 crypto_drv_pke_common_montgomery_data_config");

    /* Step 1: only 64bit participate the calculation. */
    ret = memcpy_enhance(p_cal, s_len, (p->data + p->length - s_len), s_len);
    crypto_chk_func_return(memcpy_enhance, ret);
    crypto_dump_buffer("used in calculate p", p_cal, s_len);

    ret = memcpy_enhance(s, s_len, p_cal, s_len);
    crypto_chk_func_return(memcpy_enhance, ret);
    crypto_dump_buffer("copy p to s", s, s_len);

    crypto_timer_end(TIMER_ID_6, "Step 1 memcpy_enhance twice");
    /* Step 2: right shift 1 to get s = (p - 1) >> 1 */
    crypto_drv_pke_common_arry_right_shift_value(s, s_len, 1);
    crypto_dump_buffer("used in calculate s", s, s_len);

    crypto_timer_end(TIMER_ID_6, "Step 2 crypto_drv_pke_common_arry_right_shift_value");

    aa = (drv_pke_data) {.length = s_len, .data = s};
    bb = (drv_pke_data) {.length = s_len, .data = p_cal};
    cc = (drv_pke_data) {.length = x_len, .data = tmp};
    dd = (drv_pke_data) {.length = x_len, .data = x};
    ff = (drv_pke_data) {.length = x_len, .data = tmp_2};
    /* x = x + (xi << i), xi << i may largest is 64 bit, so x largest is 64 bit 1, and smallest is 1. */
    for (i = 1; i < PKE_MONT_BIT_LEN; i++) {
        td_s32 xi = s[s_len - 1] & 1;   /* xi = s & 1; */
        crypto_log_dbg("xi[%d] = %d\r\n", i, xi);
        if (xi == 1) {
            crypto_drv_pke_common_normal_add(&aa, &bb, &cc);  /* s = s + xi * p; */
            crypto_drv_pke_common_arry_right_shift_value(tmp, x_len, 1);  /* right shift 1. s = s >> 1. */
            ret = memcpy_enhance(s, s_len, tmp + 1, s_len); /* copy the result s for next calculation. */
            crypto_chk_func_return(memcpy_enhance, ret);
            left_shift_const_one(&cc, i);   /* xi << i */
            crypto_drv_pke_common_normal_add(&dd, &cc, &ff);  /* x + (xi << i) */
            ret = memcpy_enhance(x, x_len, tmp_2, x_len);   /* x = x + (xi << i) */
            crypto_chk_func_return(memcpy_enhance, ret);
            crypto_timer_end(TIMER_ID_6, "Step 2 process add + right_shift");
        } else {
            crypto_drv_pke_common_arry_right_shift_value(s, s_len, 1);    /* right shift 1. */
            crypto_timer_end(TIMER_ID_6, "Step 2 process crypto_drv_pke_common_arry_right_shift_value");
        }
    }
    crypto_dump_buffer("bit_2", bit_2, x_len);
    crypto_dump_buffer("x", x, x_len);
    cc = (drv_pke_data) {.length = x_len, .data = bit_2};
    ret = normal_sub(&cc, &dd, &ff);
    crypto_chk_func_return(normal_sub, ret);
    crypto_dump_buffer("mont_param in byte", tmp_2, x_len);
    crypto_timer_end(TIMER_ID_6, "Step 2 normal_sub");

    /* set mont_param into register. */
    ret = crypto_drv_pke_common_byte_stream_to_int_array(tmp_2 + 1, s_len, mont_param, PKE_MONT_PARAM_LEN);
    crypto_chk_func_return(crypto_drv_pke_common_byte_stream_to_int_array, ret);
    crypto_timer_end(TIMER_ID_6, "Step 2 crypto_drv_pke_common_byte_stream_to_int_array");

    crypto_log_dbg("mont_param[0] = 0x%x, mont_param[1] = 0x%x\r\n", mont_param[0], mont_param[1]);
    ret = hal_pke_set_mont_para(sec_arg_add_cs(mont_param[1], mont_param[0]));
    crypto_chk_func_return(hal_pke_set_mont_para, ret);
    crypto_timer_end(TIMER_ID_6, "Step 2 hal_pke_set_mont_para");

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

td_s32 crypto_drv_pke_common_set_ecc_mont_param(const drv_pke_data *p)
{
    td_s32 ret = TD_FAILURE;
    td_u32 i = 0;
    td_u32 mont_param[PKE_MONT_PARAM_LEN] = {0};
    crypto_drv_func_enter();

    crypto_timer_start(TIMER_ID_6, __func__);
    /* step 0: get right montogory parameter. */
    crypto_log_dbg("g_ecc_num = %u\n", *g_ecc_num);
    for (; i < *g_ecc_num; i++) {
        if (p->length == g_ecc_params[i].curve_param->ksize) {
            crypto_log_dbg("i = %u\n", i);
            crypto_log_dbg("p->length = %u\n", p->length);
            crypto_log_dbg("curve_type = %u\n", g_ecc_params[i].curve_param->ecc_type);
            if (g_ecc_params[i].curve_param->n != TD_NULL &&
                memcmp(p->data, g_ecc_params[i].curve_param->n, g_ecc_params[i].curve_param->ksize) == 0) {
                crypto_log_dbg("here\n");
                mont_param[1] = g_ecc_params[i].default_param->mont_param_n[1];
                mont_param[0] = g_ecc_params[i].default_param->mont_param_n[0];
                break;
            } else if (g_ecc_params[i].curve_param->p != TD_NULL &&
                memcmp(p->data, g_ecc_params[i].curve_param->p, g_ecc_params[i].curve_param->ksize) == 0) {
                crypto_log_dbg("here\n");
                mont_param[1] = g_ecc_params[i].default_param->mont_param_p[1];
                mont_param[0] = g_ecc_params[i].default_param->mont_param_p[0];
                break;
            }
        }
    }
    crypto_log_dbg("i = %u\n", i);
    crypto_timer_end(TIMER_ID_6, "Step 0 get right montogory parameter");
    crypto_chk_return((i == *g_ecc_num), PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "invalid curve\n");

    /* step 1: set mont_param into register. */
    crypto_log_dbg("mont_param[0] = 0x%x, mont_param[1] = 0x%x\r\n", mont_param[0], mont_param[1]);
    ret = hal_pke_set_mont_para(sec_arg_add_cs(mont_param[1], mont_param[0]));
    crypto_chk_func_return(hal_pke_set_mont_para, ret);
    crypto_timer_end(TIMER_ID_6, "Step 1 hal_pke_set_mont_para");

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

td_s32 crypto_drv_pke_common_batch_instr_process(const rom_lib *batch_instr, td_u32 work_len)
{
    td_s32 ret = TD_FAILURE;
    crypto_drv_func_enter();

    ret = hal_pke_set_mode(sec_arg_add_cs(PKE_BATCH_INSTR, 0, batch_instr, work_len));
    crypto_chk_func_return(hal_pke_set_mode, ret);

    ret = hal_pke_start(sec_arg_add_cs(PKE_BATCH_INSTR));
    crypto_chk_func_return(hal_pke_start, ret);

    ret = hal_pke_wait_done();
    crypto_chk_func_return(hal_pke_wait_done, ret);

    crypto_drv_func_exit();
    return ret;
}

td_s32 crypto_drv_pke_common_get_single_instr(td_u32 instr_cmd, const instr_data_ram *data_ram, td_u32 *out_instr)
{
    drv_crypto_pke_check_param(data_ram == TD_NULL);
    drv_crypto_pke_check_param(out_instr == TD_NULL);
    drv_crypto_pke_check_param(data_ram->ram_section_c > MAX_RAM_SECTION);
    drv_crypto_pke_check_param(data_ram->ram_section_a > MAX_RAM_SECTION);
    drv_crypto_pke_check_param(data_ram->ram_section_b > MAX_RAM_SECTION);
    drv_crypto_pke_check_param(data_ram->ram_section_tp > MAX_RAM_SECTION);

    *out_instr = instr_cmd | (data_ram->ram_section_a << APOS_OFFSET) | (data_ram->ram_section_b << BPOS_OFFSET) |
        (data_ram->ram_section_tp << TPOS_OFFSET) | (data_ram->ram_section_c << RPOS_OFFSET);

    return TD_SUCCESS;
}

td_s32 crypto_drv_pke_common_single_instr_process(td_u32 single_instr, td_u32 work_len)
{
    td_s32 ret = TD_FAILURE;
    crypto_drv_func_enter();

    ret = hal_pke_set_mode(sec_arg_add_cs(PKE_SINGLE_INSTR0, single_instr, TD_NULL, work_len));
    crypto_chk_func_return(hal_pke_set_mode, ret);

    ret = hal_pke_start(sec_arg_add_cs(PKE_SINGLE_INSTR0));
    crypto_chk_func_return(hal_pke_start, ret);

    ret = hal_pke_wait_done();
    crypto_chk_func_return(hal_pke_wait_done, ret);

    crypto_drv_func_exit();
    return ret;
}

#ifdef PKE_V5_DEBUG
/* etip: for batch instructions single process test */
td_s32 crypto_drv_pke_common_batch_instr_single_process_test(const rom_lib *signle_instr_list, td_u32 work_len)
{
    td_s32 ret = TD_FAILURE;
    td_s32 i = 0;
    crypto_drv_func_enter();

    for (; i < single_instr_list->instr_num; i++) {
        ret = hal_pke_set_mode(sec_arg_add_cs(PKE_INSTR0, signle_instr_list + i, TD_NULL, work_len));
        crypto_chk_func_return(hal_pke_set_mode, ret);

        ret = hal_pke_start(sec_arg_add_cs(PKE_INSTR0));
        crypto_chk_func_return(hal_pke_start, ret);

        ret = hal_pke_wait_done();
        crypto_chk_func_return(hal_pke_wait_done, ret);
    }

    crypto_drv_func_exit();
    return ret;
}
#endif

td_s32 crypto_drv_pke_common_jac_to_aff_cal(const rom_lib *batch_instr, const td_u32 batch_instr_num,
    const drv_pke_data *mod_p)
{
    td_s32 ret = TD_FAILURE;
    td_u32 i = 0;
    td_s32 j = 0;
    td_u8 mod_p_2[DRV_PKE_LEN_576];
    td_u8 bit_2 = 0;
    td_bool start_flag = TD_FALSE;
    crypto_drv_func_enter();
    crypto_timer_start(TIMER_ID_5, __func__);

    /* 1. set necessary data into DRAM, which should have been set by former process. */
    /* get work_len */
    td_u32 work_len = mod_p->length / ALIGNED_TO_WORK_LEN_IN_BYTE;
    drv_crypto_pke_check_param(batch_instr_num != JAC_TO_AFF_INSTR_NUM);

    crypto_timer_end(TIMER_ID_5, "Step 1");
    /* 2. start transform */
    /* 2.1 preprocess, initialize the data, use batch process step. */
    ret = crypto_drv_pke_common_batch_instr_process(&batch_instr[0], work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* 2.2 expand (p-2)'s bit length to 2n, and calculate from high bit */
    /* get p - 2 */
    ret = memcpy_enhance(mod_p_2, DRV_PKE_LEN_576, mod_p->data, mod_p->length);
    crypto_chk_return((ret != TD_SUCCESS), ret, "memcpy_enhance failed!\n");

    crypto_drv_pke_common_array_sub_value(mod_p_2, mod_p->length, 2);   /* 2: to get mod_p - 2. */

    crypto_timer_end(TIMER_ID_5, "Step 2.1~2.2");

    /*
     * 1) mod_p_2 is one byte array, mod_p->length is the byte number.
     * 2) For each byte in mod_p_2, get bit2 = byte[7:6], byte[5:4], byte[3:2], byte[1:0] in order.
     * 3) If there is one bit2 is not zero, then all the next bit2(contain this time) will call barch_process(),
            the first param depends on bit2's value.
     */
    for (i = 0; i < mod_p->length; i++) {
        for (j = 3; j >= 0; j--) {                  // 3: get byte[7:6], byte[5:4], byte[3:2], byte[1:0] in order.
            bit_2 = (mod_p_2[i] >> (j * 2)) & 0x3;   // 2, 0x3: get bit2
            if ((start_flag == TD_FALSE) && (bit_2 != 0)) {
                start_flag = TD_TRUE;
                ret = crypto_drv_pke_common_batch_instr_process(&batch_instr[bit_2 + 1], work_len);
                crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_batch_instr_process failed\n");
                crypto_timer_end(TIMER_ID_5, "Step 2.2 process");
            } else if (start_flag == TD_TRUE) {
                ret = crypto_drv_pke_common_batch_instr_process(&batch_instr[bit_2 + 1], work_len);
                crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_batch_instr_process failed\n");
                crypto_timer_end(TIMER_ID_5, "Step 2.2 process");
            }
        }
    }

    /* 2.3 postprocess, demontgomery and complete reduction */
    ret = crypto_drv_pke_common_batch_instr_process(&batch_instr[5], work_len); /* 5: instr index. */
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    crypto_timer_end(TIMER_ID_5, "Step 2.3");
    crypto_drv_func_exit();
    return TD_SUCCESS;
}

td_s32 crypto_drv_pke_common_point_mul_naf(const rom_lib *batch_instr, const td_u32 batch_instr_num,
    const drv_pke_data *k, td_u32 work_len)
{
    td_s32 ret = TD_FAILURE;
    td_s32 i = 0;
    drv_crypto_pke_check_param(batch_instr_num != POINT_NAF_INSTR_NUM);

    /* Step 1: convert k to non-adjacent-form. */
    td_u32 k_naf_len = k->length * CRYPTO_BITS_IN_BYTE + 1;
    td_s32 *k_naf = crypto_malloc(k_naf_len * sizeof(td_u32));
    crypto_chk_return((k_naf == TD_NULL), PKE_COMPAT_ERRNO(ERROR_MALLOC), "malloc failed!\n");
    ret = crypto_drv_pke_common_convert_normal_scalar_to_naf(k->data, k->length, k_naf, &k_naf_len);
    crypto_chk_goto((ret != TD_SUCCESS), __EXIT, "convert data to naf failed!\n");

    /* before calculate naf_point_mul, you need to set mont_a to ecc_addr_mont_a first.
       And keep ecc->p in the modulur. */
    /* Step 2: calculate point multiplication by double point and point plus. k_naf couldn't be infinity point. */
    /* instr_ecfp_mul_p_22_18[0,40], instr_ecfp_mul_g_22_18[40,40], instr_ecfp_mul_c_double_22[0,22] */
    for (i = k_naf_len - 2; i >= 0; i--) {  /* 2: to get the start calculate index. */
        if (k_naf[i] == 1) {
            ret = hal_pke_set_mode(sec_arg_add_cs(PKE_BATCH_INSTR, 0, &batch_instr[0], work_len));
        } else if (k_naf[i] == -1) {
            ret = hal_pke_set_mode(sec_arg_add_cs(PKE_BATCH_INSTR, 0, &batch_instr[1], work_len));
        } else {
            ret = hal_pke_set_mode(sec_arg_add_cs(PKE_BATCH_INSTR, 0, &batch_instr[2], work_len));  /* 2: instr index */
        }
        crypto_chk_goto((ret != TD_SUCCESS), __EXIT, "hal_pke_set_mode failed, ret = 0x%x", ret);
        ret = hal_pke_start(sec_arg_add_cs(PKE_BATCH_INSTR));
        crypto_chk_goto((ret != TD_SUCCESS), __EXIT, "hal_pke_start failed, ret = 0x%x", ret);
        ret = hal_pke_wait_done();
        crypto_chk_goto((ret != TD_SUCCESS), __EXIT, "hal_pke_wait_done failed, ret = 0x%x", ret);
    }

__EXIT:
    if (k_naf != TD_NULL) {
        crypto_free(k_naf);
        k_naf = TD_NULL;
    }

    return ret;
}
#if defined(CONFIG_PKE_SUPPORT_SM2) || defined(CONFIG_PKE_SUPPORT_EDWARD) || defined(CONFIG_PKE_SUPPORT_RSA)
/* Define ECC-HASH related data & types */
#define SHA1_BLOCK_LENGTH                          64
#define SHA224_BLOCK_LENGTH                        64
#define SHA256_BLOCK_LENGTH                        64
#define SHA384_BLOCK_LENGTH                        128
#define SHA512_BLOCK_LENGTH                        128
#define SM3_BLOCK_LENGTH                           64
#define PKE_MAX_HASH_BLOCK_SIZE 128

#define SHA1_RESULT_LENGTH                         20
#define SHA224_RESULT_LENGTH                       28
#define SHA256_RESULT_LENGTH                       32
#define SHA384_RESULT_LENGTH                       48
#define SHA512_RESULT_LENGTH                       64
#define SM3_RESULT_LENGTH                          32

static crypto_hash_type drv_pke_get_hash_type(drv_pke_hash_type hash_type)
{
    switch (hash_type) {
        case DRV_PKE_HASH_TYPE_SHA1:
            return CRYPTO_HASH_TYPE_SHA1;
        case DRV_PKE_HASH_TYPE_SHA224:
            return CRYPTO_HASH_TYPE_SHA224;
        case DRV_PKE_HASH_TYPE_SHA256:
            return CRYPTO_HASH_TYPE_SHA256;
        case DRV_PKE_HASH_TYPE_SHA384:
            return CRYPTO_HASH_TYPE_SHA384;
        case DRV_PKE_HASH_TYPE_SHA512:
            return CRYPTO_HASH_TYPE_SHA512;
        case DRV_PKE_HASH_TYPE_SM3:
            return CRYPTO_HASH_TYPE_SM3;
        default:
            return CRYPTO_HASH_TYPE_INVALID;
    }
}

td_s32 crypto_drv_pke_common_calc_hash(const drv_pke_data* arr, const td_u32 arr_len, const drv_pke_hash_type hash_type,
    drv_pke_data *hash)
{
    td_s32 ret = TD_FAILURE;
    td_handle h_handle = 0;
    crypto_buf_attr src_buf = {0};
    td_u32 i;
    crypto_hash_type hash_alg;
    crypto_hash_attr hash_attr;
    crypto_param_check(arr == TD_NULL);
    crypto_param_check(hash == TD_NULL);
    crypto_param_check(hash->data == TD_NULL);

    hash_alg = drv_pke_get_hash_type(hash_type);
    if (hash_alg == CRYPTO_HASH_TYPE_INVALID) {
        return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }
    hash_attr = (crypto_hash_attr){.hash_type = hash_alg};

    ret = drv_cipher_hash_start(&h_handle, &hash_attr);
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_hash_start failed, ret is 0x%x\n", ret);

    for (i = 0;i < arr_len; i++) {
        src_buf.virt_addr = arr[i].data;
        ret = drv_cipher_hash_update(h_handle, &src_buf, arr[i].length);
        crypto_chk_goto(ret != TD_SUCCESS, exit__, "drv_cipher_hash_update failed, ret is 0x%x\n", ret);
    }
    ret = drv_cipher_hash_finish(h_handle, hash->data, &(hash->length));
    crypto_chk_goto(ret != TD_SUCCESS, exit__, "drv_cipher_hash_finish failed, ret is 0x%x\n", ret);
    return TD_SUCCESS;
exit__:
    (void)drv_cipher_hash_destroy(h_handle);
    return ret;
}
#endif

td_s32 crypto_drv_pke_common_resume(void)
{
    crypto_drv_func_enter();

    td_s32 ret = TD_FAILURE;
    ret = hal_pke_lock();
    crypto_chk_func_return(hal_pke_lock, ret);

    hal_pke_enable_noise();

    ret = hal_pke_pre_process();
    crypto_chk_goto((ret != TD_SUCCESS), __EXIT, "hal_pke_pre_process failed, ret = 0x%x", ret);

    crypto_drv_func_exit();
    return ret;
__EXIT:
    hal_pke_disable_noise();
    hal_pke_unlock();
    return ret;
}

void crypto_drv_pke_common_suspend(void)
{
    crypto_drv_func_enter();
    (void)hal_pke_clean_ram();
    hal_pke_disable_noise();
    hal_pke_unlock();
    crypto_drv_func_exit();
}

td_s32 crypto_drv_pke_common_limit_value_check(const td_u8 *in_buf, const td_u8 *limit_value, const td_u32 limit_len)
{
    td_u32 i = 0;
    drv_crypto_pke_check_param(in_buf == TD_NULL);
    drv_crypto_pke_check_param(limit_value == TD_NULL);

    /* whether in_buf <= limit - 1 */
    for (; i < limit_len; i++) {
        /* if in_buf[i] == limit_value[i], then go next loop. */
        if (in_buf[i] < limit_value[i]) {
            return TD_SUCCESS;
        } else if (in_buf[i] > limit_value[i]) {
            return TD_FAILURE;
        }
    }
    /* secure enhancement, make sure the loop execute correctly. */
    val_enhance_chk(i, limit_len);

    /* if not exit in former process, means in_buf == limit_value. */
    return TD_FAILURE;
}

td_s32 crypto_drv_pke_common_range_check(const td_u8 *in_buf, const td_u8 *limit_value, const td_u32 limit_len)
{
    /* whether in_buf >= 1 */
    if (crypto_drv_pke_common_is_zero(in_buf, limit_len) == TD_TRUE) {
        return TD_FAILURE;
    }

    return crypto_drv_pke_common_limit_value_check(in_buf, limit_value, limit_len);
}

td_s32 crypto_drv_pke_common_init_param(const drv_pke_ecc_curve *ecc)
{
    td_u32 i = 0;
    td_s32 ret = TD_FAILURE;
    drv_crypto_pke_check_param(ecc == TD_NULL);

    for (; i < *g_ecc_num; i++) {
        if (ecc->ecc_type == g_ecc_params[i].curve_param->ecc_type &&
            ecc->ksize == g_ecc_params[i].curve_param->ksize) {
            ret = hal_pke_set_init_param(g_ecc_params[i].default_param, g_ecc_params[i].curve_param);
            crypto_chk_func_return(hal_pke_set_init_param, ret);
            break;
        }
    }
    return ret;
}