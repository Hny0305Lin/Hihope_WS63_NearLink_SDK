/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: key derivation algorithm of pbkdf2 implementation.
 *
 * Create: 2023-05-26
*/

#include "drv_hash.h"
#include "drv_inner.h"
#include "drv_trng.h"

#include "hal_hash.h"
#include "hal_rkp.h"

#include "crypto_drv_common.h"

/* Current CPU ID Status .
8'h35: AIDSP;
8'h6a: PCPU;
8'ha5: TEE;
8'haa: ACPU.
*/
#define PCPU_STAT     0x6a
#define AIDSP_STAT    0x35
#define TEE_STAT      0xa5
#define ACPU_STAT     0xaa

/* KDF */

#define KM_CPU_PCPU  4
#define KM_CPU_AIDSP 5
#define KM_CPU_TEE   2
#define KM_CPU_REE   1
#define KM_CPU_IDLE  0

#define PBKDF2_ALG_HMAC_SHA1       1
#define PBKDF2_ALG_HMAC_SHA256     0
#define PBKDF2_ALG_HMAC_SHA384     3
#define PBKDF2_ALG_HMAC_SHA512     4
#define PBKDF2_ALG_HMAC_SM3        5

#define HMAC_SHA1_OUTPUT_LEN       20
#define HMAC_SHA256_OUTPUT_LEN     32
#define HMAC_SHA384_OUTPUT_LEN     48
#define HMAC_SHA512_OUTPUT_LEN     64
#define HMAC_SM3_OUTPUT_LEN        32

#define KDF_KEY_BLOCK_SIZE_512     64
#define KDF_KEY_BLOCK_SIZE_1024    128
#define KDF_KEY_CONFIG_LEN         32
#define KDF_SALT_CONFIG_LEN        32
#define KDF_VAL_CONFIG_LEN         16
#define KDF_ALG_TYPE               5
#define DRV_KDF_OTP_KEY_MRK1       0
#define DRV_KDF_OTP_KEY_USK        1
#define DRV_KDF_OTP_KEY_RUSK       2

/*
    pbkdf2_key_config
*/
#define KDF_SW_GEN              3

/*
 * rkp
 */
#define RKP_PBKDF2_VAL(b)       (0x200 + 4 * (b)) /* b 0~16 */

typedef struct {
    crypto_hash_type hash_type;
    crypto_kdf_sw_alg kdf_sw_alg;
} table_t;

static const table_t g_hash_to_kdf_sw[] = {
    {CRYPTO_HASH_TYPE_HMAC_SHA1, CRYPTO_KDF_SW_ALG_SHA1},
    {CRYPTO_HASH_TYPE_HMAC_SHA256, CRYPTO_KDF_SW_ALG_SHA256},
    {CRYPTO_HASH_TYPE_HMAC_SHA384, CRYPTO_KDF_SW_ALG_SHA384},
    {CRYPTO_HASH_TYPE_HMAC_SHA512, CRYPTO_KDF_SW_ALG_SHA512},
    {CRYPTO_HASH_TYPE_HMAC_SM3, CRYPTO_KDF_SW_ALG_SM3},
};

static td_s32 priv_kdf_param_salt_padding(td_u8 *salt_pad, td_u32 pad_len, td_u32 block_len,
    const crypto_kdf_pbkdf2_param *param, td_u32 cnt_bit)
{
    td_u32 i = 0;
    td_u32 j = 0;
    td_u32 salt_pad_total_len = 0;
    td_u32 random_val = 0;
    td_s32 ret;
    crypto_unused(pad_len);
    /* Formatting of the PBKDF2 salt Padding
     * The PBKDF2 salt Padding formatted as shown below:
     *                        big endian
     *         salt length || 32 bit(i) || 1 bit(1) || 2 word tail length
     *
     * sm3:
     * sha1:
     * sha256:   salt_len  ||   32 bit  ||   8 bit  ||  64 bit
     *
     * sha384:
     * sha512:   salt_len  ||   32 bit  ||   8 bit  ||  128 bit
     */
    ret = drv_cipher_trng_get_random(&random_val);
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_trng_get_random failed\n");

    for (i = 0, j = random_val % param->slen; i < param->slen; i++) {
        salt_pad[j] = param->salt[j]; /* copy salt */
        j++;
        j %= param->slen;
    }

    salt_pad[i + 3] = cnt_bit & 0xff;         /* 3: need fill 32bit i, big endian padding */
    salt_pad[i + 2] = (cnt_bit >> 8) & 0xff;  /* 2/8: need fill 32bit i, big endian padding */
    salt_pad[i + 1] = (cnt_bit >> 16) & 0xff; /* 16: need fill 32bit i, big endian padding */
    salt_pad[i] = (cnt_bit >> 24) & 0xff;     /* 24: need fill 32bit i, big endian padding */
    salt_pad[i + 4] = 0x80;                   /* 4: message data fill 1 */

    if (block_len == KDF_KEY_BLOCK_SIZE_512) {
        /* need to connect i_key_pad length, the i_key_pad length is block size */
        salt_pad_total_len = KDF_KEY_BLOCK_SIZE_512 * CRYPTO_BITS_IN_BYTE;
    } else {
        /* need to connect i_key_pad length, the i_key_pad length is block size */
        salt_pad_total_len = KDF_KEY_BLOCK_SIZE_1024 * CRYPTO_BITS_IN_BYTE;
    }
    salt_pad_total_len += 32 + param->slen * 8;          /* length: 32 bit i + slen * 8 */

    salt_pad[block_len - 1] = salt_pad_total_len & 0xff;
    salt_pad[block_len - 2] = (salt_pad_total_len >> 8) & 0xff;  /* 2: 2bit 8: right move 8 */
    salt_pad[block_len - 3] = (salt_pad_total_len >> 16) & 0xff; /* 3: 3bit 16: right move 16 */
    salt_pad[block_len - 4] = (salt_pad_total_len >> 24) & 0xff; /* 4: 4bit 24: right move 24 */

    return TD_SUCCESS;
}

static td_s32 priv_kdf_calculation(const crypto_kdf_pbkdf2_param *param, td_u32 block_size,
    td_u32 cnt_bit)
{
    td_u8 kdf_passwd_pad[KDF_KEY_BLOCK_SIZE_1024];
    td_u8 kdf_salt_pad[KDF_KEY_BLOCK_SIZE_1024];
    const td_u32 *sha_val = TD_NULL;
    td_u32 sha_val_len = 0;
    td_s32 ret;
    crypto_kdf_sw_alg kdf_sw_alg = CRYPTO_KDF_SW_ALG_SHA1;

    for (td_u32 i = 0; i < (sizeof(g_hash_to_kdf_sw) / sizeof(table_t)); i++) {
        if (param->hash_type == g_hash_to_kdf_sw[i].hash_type) {
            kdf_sw_alg = g_hash_to_kdf_sw[i].kdf_sw_alg;
            break;
        }
    }

    (td_void)memset_s(kdf_passwd_pad, KDF_KEY_BLOCK_SIZE_1024, 0, KDF_KEY_BLOCK_SIZE_1024);
    (td_void)memset_s(kdf_salt_pad, KDF_KEY_BLOCK_SIZE_1024, 0, KDF_KEY_BLOCK_SIZE_1024);

    /* password padding */
    ret = memcpy_s(kdf_passwd_pad, KDF_KEY_BLOCK_SIZE_1024, param->password, param->plen);
    crypto_chk_return(ret != EOK, ret, "memcpy_s failed\n");

    /* salt padding */
    ret = priv_kdf_param_salt_padding(kdf_salt_pad, KDF_KEY_BLOCK_SIZE_1024, block_size, param, cnt_bit);
    crypto_chk_goto(ret != TD_SUCCESS, exit_clean, "priv_kdf_param_salt_padding failed\n");

    /* config initial value. */
    sha_val = drv_hash_get_state_iv(param->hash_type, &sha_val_len);
    crypto_chk_goto_with_ret(ret, sha_val == TD_NULL, exit_clean, TD_FAILURE, "drv_hash_get_state_iv failed\n");

    ret = hal_rkp_kdf_set_val((const td_u8 *)sha_val, sha_val_len);
    crypto_chk_goto(ret != TD_SUCCESS, exit_clean, "hal_rkp_kdf_set_val failed\n");

    /* set padding_salt. */
    ret = hal_rkp_kdf_set_padding_salt(kdf_salt_pad, KDF_KEY_BLOCK_SIZE_1024);
    crypto_chk_goto(ret != TD_SUCCESS, exit_clean, "hal_rkp_kdf_set_padding_salt failed\n");

    /* set padding_passwd. */
    ret = hal_rkp_kdf_set_padding_key(kdf_passwd_pad, KDF_KEY_BLOCK_SIZE_1024);
    crypto_chk_goto(ret != TD_SUCCESS, exit_clean, "hal_rkp_kdf_set_padding_salt failed\n");

    /* kdf start. */
    ret = hal_rkp_kdf_sw_start(kdf_sw_alg, param->count, TD_FALSE);
    crypto_chk_goto(ret != TD_SUCCESS, exit_clean, "hal_rkp_kdf_sw_start failed\n");

    ret = hal_rkp_kdf_wait_done();
    crypto_chk_goto(ret != TD_SUCCESS, exit_clean, "hal_rkp_kdf_wait_done failed\n");

exit_clean:
    (td_void)memset_s(kdf_passwd_pad, KDF_KEY_BLOCK_SIZE_1024, 0, KDF_KEY_BLOCK_SIZE_1024);
    (td_void)memset_s(kdf_salt_pad, KDF_KEY_BLOCK_SIZE_1024, 0, KDF_KEY_BLOCK_SIZE_1024);
    return ret;
}

#define PBKDF2_ALG_HMAC_SHA1       1
#define PBKDF2_ALG_HMAC_SHA256     0
#define PBKDF2_ALG_HMAC_SHA384     3
#define PBKDF2_ALG_HMAC_SHA512     4
#define PBKDF2_ALG_HMAC_SM3        5

static td_s32 priv_get_alg_cfg(crypto_hash_type hash_type, td_u32 *alg_cfg, td_u32 *block_size, td_u32 *out_key_len)
{
    *block_size = crypto_hash_get_block_size(hash_type) / CRYPTO_BITS_IN_BYTE;
    *out_key_len = crypto_hash_get_result_size(hash_type) / CRYPTO_BITS_IN_BYTE;
    switch (hash_type) {
        case CRYPTO_HASH_TYPE_HMAC_SHA1:
            *alg_cfg = PBKDF2_ALG_HMAC_SHA1;
            break;
        case CRYPTO_HASH_TYPE_HMAC_SHA256:
            *alg_cfg = PBKDF2_ALG_HMAC_SHA256;
            break;
        case CRYPTO_HASH_TYPE_HMAC_SHA384:
            *alg_cfg = PBKDF2_ALG_HMAC_SHA384;
            break;
        case CRYPTO_HASH_TYPE_HMAC_SHA512:
            *alg_cfg = PBKDF2_ALG_HMAC_SHA512;
            break;
        case CRYPTO_HASH_TYPE_HMAC_SM3:
            *alg_cfg = PBKDF2_ALG_HMAC_SM3;
            break;
        default:
            return TD_FAILURE;
    }
    return TD_SUCCESS;
}

static td_s32 priv_kdf_output_key_remain(td_u8 *out, const td_u32 out_len, td_u32 out_key_len, td_u32 cnt_count,
    td_u32 remain_num)
{
    td_s32 ret;
    td_u32 i = 0;
    td_u32 readval[KDF_VAL_CONFIG_LEN];

    for (i = 0; i < out_key_len / CRYPTO_WORD_WIDTH; i++) {
        readval[i] = km_reg_read(RKP_PBKDF2_VAL(i));
    }

    ret = memcpy_s(out + cnt_count * out_key_len, out_len - cnt_count * out_key_len, readval, remain_num);
    crypto_chk_return(ret != EOK, TD_FAILURE, "memcpy_s failed\n");

    return TD_SUCCESS;
}

static td_s32 priv_kdf_output_key(td_u8 *out, const td_u32 out_len, td_u32 out_key_len, td_u32 cnt_count)
{
    td_u32 i = 0;
    td_u32 j = 0;
    td_u32 readval[KDF_VAL_CONFIG_LEN];
    td_u32 random_val = 0;
    td_s32 ret;

    ret = drv_cipher_trng_get_random(&random_val);
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_trng_get_random failed\n");

    for (i = 0, j = random_val % (out_key_len / CRYPTO_WORD_WIDTH); i < out_key_len / CRYPTO_WORD_WIDTH; i++) {
        readval[j] = km_reg_read(RKP_PBKDF2_VAL(j));
        j++;
        j %= (out_key_len / CRYPTO_WORD_WIDTH);
    }

    ret = memcpy_s(out + cnt_count * out_key_len, out_len - cnt_count * out_key_len, readval, out_key_len);
    crypto_chk_return(ret != EOK, TD_FAILURE, "memcpy_s failed\n");

    return TD_SUCCESS;
}

td_s32 drv_cipher_pbkdf2(const crypto_kdf_pbkdf2_param *param, td_u8 *out, const td_u32 out_len)
{
    td_s32 ret;
    td_u32 alg_cfg = 0;
    td_u32 block_size = 0;
    td_u32 out_key_len = 0;
    td_u32 cal_count = 0;
    td_u32 remain_num = 0;
    td_u32 i = 0;

    crypto_drv_func_enter();

    crypto_chk_return(param == TD_NULL || out == TD_NULL || param->password == TD_NULL || param->salt == TD_NULL,
        TD_FAILURE, "Param is NULL!\n");

    ret = priv_get_alg_cfg(param->hash_type, &alg_cfg, &block_size, &out_key_len);
    crypto_chk_return(ret != TD_SUCCESS, ret, "priv_get_alg_cfg failed\n");
    crypto_chk_return(param->plen > block_size, TD_FAILURE, "plen should not be Bigger Than Block Size!\n");
    crypto_chk_return(out_key_len == 0, TD_FAILURE, "out_key_len is Invalid!\n");

    /* Formatting of the PBKDF2 salt Padding
     * The PBKDF2 salt Padding formatted as shown below:
     *                        big endian
     *         salt length || 32 bit(i) || 1 bit(1) || 2 word tail length
     *
     * sm3:
     * sha1:
     * sha256:   salt_len  ||   4 byte  ||   1 byte  ||  8 byte
     * so salt_len smaller block_size - (4 + 1 + 8 = 13)
     * sha384:
     * sha512:   salt_len  ||   4 byte  ||   1 byte  ||  16 byte
     * so salt_len smaller block_size - (4 + 1 + 16 = 21)
     */
    if (block_size == KDF_KEY_BLOCK_SIZE_512) {
        crypto_chk_return(param->slen > block_size - 13, TD_FAILURE, "Invalid Salt Len!\n");   /* 13: byte */
    } else {
        crypto_chk_return(param->slen > block_size - 21, TD_FAILURE, "Invalid Salt Len!\n");   /* 21: byte */
    }

    remain_num = out_len % out_key_len;      /* residual processing */
    if (remain_num != 0) {
        cal_count = (out_len / out_key_len) + 1;  /* if remain_num is not 0, need to calculate one more block. */
    } else {
        cal_count = out_len / out_key_len;
    }

    ret = hal_rkp_lock();
    crypto_chk_return(ret != TD_SUCCESS, ret, "hal_rkp_lock failed\n");

    for (i = 0; i < cal_count; i++) {
        ret = priv_kdf_calculation(param, block_size, i + 1);
        crypto_chk_goto(ret != TD_SUCCESS, exit_rkp_unlock, "priv_kdf_calculation failed\n");
        if (remain_num != 0) {
            if (i == cal_count - 1) { /* calculation the last conut */
                ret = priv_kdf_output_key_remain(out, out_len, out_key_len, i, remain_num);
                crypto_chk_goto(ret != TD_SUCCESS, exit_rkp_unlock, "priv_kdf_output_key_remain failed\n");
            } else {
                ret = priv_kdf_output_key(out, out_len, out_key_len, i);
                crypto_chk_goto(ret != TD_SUCCESS, exit_rkp_unlock, "priv_kdf_output_key failed\n");
            }
        } else {
            ret = priv_kdf_output_key(out, out_len, out_key_len, i);
            crypto_chk_goto(ret != TD_SUCCESS, exit_rkp_unlock, "priv_kdf_output_key failed\n");
        }
    }

    /* clear key */
    hal_rkp_clear_reg_key();

exit_rkp_unlock:
    hal_rkp_unlock();     /* unlock rkp */
    crypto_drv_func_exit();
    return ret;
}