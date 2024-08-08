/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: rsa sign/verify/crypto algorithm implementation.
 *
 * Create: 2023-05-27
*/

#include "hal_pke.h"
#include "drv_pke_inner.h"
#include "drv_pke.h"
#include "crypto_hash_struct.h"
#include "drv_hash.h"
#include "drv_trng.h"
#include "crypto_drv_common.h"
#include "drv_common_pke.h"
#include "drv_pke_cal.h"
#include "rsa.h"

#define RANDOM_TRY_TIME                 100
#define RSA_GEN_KEY_WAIT_TIMES          10
#define RSA_GEN_KEY_TRY_TIMES           10

#define pke_null_ptr_chk(ptr)   \
    crypto_chk_return((ptr) == TD_NULL, PKE_COMPAT_ERRNO(ERROR_PARAM_IS_NULL), #ptr" is NULL\n")

typedef struct {
    drv_pke_hash_type hash_type;
    td_u32 hash_len;
    const td_u8 *asn1_data;
    td_u32 asn1_len;
    const td_u8 *lhash_data;
} pkcs1_hash_t;

// hash ASN.1
static const td_u8 g_asn1_sha1[] = {
    0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e,
    0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14,
};

static const td_u8 g_asn1_sha224[] = {
    0x30, 0x2d, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86,
    0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x04, 0x05,
    0x00, 0x04, 0x1c
};

static const td_u8 g_asn1_sha256[] = {
    0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86,
    0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05,
    0x00, 0x04, 0x20,
};

static const td_u8 g_asn1_sha384[] = {
    0x30, 0x41, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86,
    0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02, 0x05,
    0x00, 0x04, 0x30,
};

static const td_u8 g_asn1_sha512[] = {
    0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86,
    0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03, 0x05,
    0x00, 0x04, 0x40,
};

static const td_u8 g_empty_l_sha1[] = {
    0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d,
    0x32, 0x55, 0xbf, 0xef, 0x95, 0x60, 0x18, 0x90,
    0xaf, 0xd8, 0x07, 0x09,
};

static const td_u8 g_empty_l_sha224[] = {
    0xd1, 0x4a, 0x02, 0x8c, 0x2a, 0x3a, 0x2b, 0xc9,
    0x47, 0x61, 0x02, 0xbb, 0x28, 0x82, 0x34, 0xc4,
    0x15, 0xa2, 0xb0, 0x1f, 0x82, 0x8e, 0xa6, 0x2a,
    0xc5, 0xb3, 0xe4, 0x2f
};

static const td_u8 g_empty_l_sha256[] = {
    0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
    0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
    0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
    0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55,
};

static const td_u8 g_empty_l_sha384[] = {
    0x38, 0xb0, 0x60, 0xa7, 0x51, 0xac, 0x96, 0x38,
    0x4c, 0xd9, 0x32, 0x7e, 0xb1, 0xb1, 0xe3, 0x6a,
    0x21, 0xfd, 0xb7, 0x11, 0x14, 0xbe, 0x07, 0x43,
    0x4c, 0x0c, 0xc7, 0xbf, 0x63, 0xf6, 0xe1, 0xda,
    0x27, 0x4e, 0xde, 0xbf, 0xe7, 0x6f, 0x65, 0xfb,
    0xd5, 0x1a, 0xd2, 0xf1, 0x48, 0x98, 0xb9, 0x5b,
};

static const td_u8 g_empty_l_sha512[] = {
    0xcf, 0x83, 0xe1, 0x35, 0x7e, 0xef, 0xb8, 0xbd,
    0xf1, 0x54, 0x28, 0x50, 0xd6, 0x6d, 0x80, 0x07,
    0xd6, 0x20, 0xe4, 0x05, 0x0b, 0x57, 0x15, 0xdc,
    0x83, 0xf4, 0xa9, 0x21, 0xd3, 0x6c, 0xe9, 0xce,
    0x47, 0xd0, 0xd1, 0x3c, 0x5d, 0x85, 0xf2, 0xb0,
    0xff, 0x83, 0x18, 0xd2, 0x87, 0x7e, 0xec, 0x2f,
    0x63, 0xb9, 0x31, 0xbd, 0x47, 0x41, 0x7a, 0x81,
    0xa5, 0x38, 0x32, 0x7a, 0xf9, 0x27, 0xda, 0x3e,
};

static const td_s8 g_bits[] = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};

static td_u8 g_l_hash[HASH_SIZE_SHA_512] = {0x0};

static const pke_hash_properties_type g_pke_hash_properties[] = {
    {DRV_PKE_HASH_TYPE_SHA1, SHA1_BLOCK_LENGTH, SHA1_RESULT_LENGTH},
    {DRV_PKE_HASH_TYPE_SHA224, SHA224_BLOCK_LENGTH, SHA224_RESULT_LENGTH},
    {DRV_PKE_HASH_TYPE_SHA256, SHA256_BLOCK_LENGTH, SHA256_RESULT_LENGTH},
    {DRV_PKE_HASH_TYPE_SHA384, SHA384_BLOCK_LENGTH, SHA384_RESULT_LENGTH},
    {DRV_PKE_HASH_TYPE_SHA512, SHA512_BLOCK_LENGTH, SHA512_RESULT_LENGTH},
    {DRV_PKE_HASH_TYPE_SM3, SM3_BLOCK_LENGTH, SM3_RESULT_LENGTH}
};

static const pkcs1_hash_t g_pke_pkcs1_hash[] = {
    {DRV_PKE_HASH_TYPE_SHA1, HASH_SIZE_SHA_1, g_asn1_sha1, sizeof(g_asn1_sha1), g_empty_l_sha1},
    {DRV_PKE_HASH_TYPE_SHA224, HASH_SIZE_SHA_224, g_asn1_sha224, sizeof(g_asn1_sha224), g_empty_l_sha224},
    {DRV_PKE_HASH_TYPE_SHA256, HASH_SIZE_SHA_256, g_asn1_sha256, sizeof(g_asn1_sha256), g_empty_l_sha256},
    {DRV_PKE_HASH_TYPE_SHA384, HASH_SIZE_SHA_384, g_asn1_sha384, sizeof(g_asn1_sha384), g_empty_l_sha384},
    {DRV_PKE_HASH_TYPE_SHA512, HASH_SIZE_SHA_512, g_asn1_sha512, sizeof(g_asn1_sha512), g_empty_l_sha512}
};

CRYPTO_STATIC td_s32 pke_dh_check_pub_key(const drv_pke_data *mod_n, const drv_pke_data *input_pub_key);

static td_u32 drv_get_hash_len(const drv_pke_hash_type hash_type)
{
    td_u32 i = 0;

    for (i = 0; i < sizeof(g_pke_hash_properties) / sizeof(pke_hash_properties_type); i++) {
        if (g_pke_hash_properties[i].hash_type == hash_type) {
            break;
        }
    }
    return g_pke_hash_properties[i].result_len;
}

static td_s32 get_random(td_u8 *rand, const td_u32 size)
{
    td_s32 ret;
    td_u32 i;
    td_u32 try_times = 0;

    ret = drv_cipher_trng_get_multi_random(size, rand);
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_pke_get_multi_random failed\n");

    for (i = 0; i < size; i++) {
        try_times = 0;
        while (rand[i] == 0 && try_times < RANDOM_TRY_TIME) {
            ret = drv_cipher_trng_get_multi_random(1, &rand[i]);
            crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_pke_get_multi_random failed\n");
            try_times++;
        }
        if (try_times >= RANDOM_TRY_TIME) {
            return TD_FAILURE;
        }
    }
    return TD_SUCCESS;
}

static td_s32 get_limit_random(const drv_pke_data *limit_value, const drv_pke_data *output_random)
{
    td_s32 ret = TD_FAILURE;
    td_u8 *tmp = TD_NULL;
    td_u32 tmp_len = 2 * output_random->length; /* 2: x2 is for security. */
    td_u32 i;
    drv_crypto_pke_check_param(limit_value->length != output_random->length);
    drv_crypto_pke_check_param(limit_value->length > DRV_PKE_LEN_4096);

    tmp = crypto_malloc(tmp_len);    /* 2: x2 is for security. */
    crypto_chk_return(tmp == TD_NULL, ret, "crypto_malloc failed\n");
    drv_pke_data tmp_data = {.data = tmp, .length = tmp_len};
    /* genarate random. */
    for (i = 0; i < PKE_MAX_TIMES; i++) {
        ret = drv_cipher_trng_get_multi_random(tmp_len, tmp); /* 2: x2 is for security. */
        crypto_chk_goto(ret != TD_SUCCESS, __EXIT, "get random data failed, ret = 0x%x", ret);
        /* make sure ouput data is smaller than limit_value, output_random = tmp mod n. */
        ret = drv_cipher_pke_mod(&tmp_data, limit_value, output_random);
        crypto_chk_goto(ret != TD_SUCCESS, __EXIT, "drv_cipher_pke_mod, ret = 0x%x", ret);
        if (crypto_drv_pke_common_is_zero(output_random->data, output_random->length) != TD_TRUE) {
            break;
        }
    }
    if (i >= PKE_MAX_TIMES) {
        crypto_log_err("Generate limit random value timeout!\n");
        ret = TD_FAILURE;
    }
__EXIT:
    if (tmp != TD_NULL) {
        (void)memset_enhance(tmp, tmp_len, 0x00, tmp_len);  /* clear the generate random data. */
        crypto_free(tmp);
        tmp = TD_NULL;
    }
    return ret;
}

static td_u32 rsa_get_bit_num(const td_u8 *big_num, td_u32 num_len)
{
    td_u32 i = 0;
    td_s8 num = 0;

    for (i = 0; i < num_len; i++) {
        num = g_bits[(big_num[i] & (MAX_LOW_8BITS - MAX_LOW_4BITS)) >> SHIFT_4BITS];
        if (num > 0) {
            return (num_len - i - BOUND_VALUE_1) * BYTE_BITS + num + DRV_WORD_WIDTH;
        }

        num = g_bits[big_num[i] & MAX_LOW_4BITS];
        if (num > 0) {
            return (num_len - i - BOUND_VALUE_1) * BYTE_BITS + num;
        }
    }
    return 0;
}

static td_s32 pkcs1_mgf(const rsa_pkcs1_hash_info *hash_info, const td_u8 *seed, const td_u32 seed_len,
    td_u8 *mask, const td_u32 mask_len)
{
    td_s32 ret = TD_FAILURE;
    td_u32 i = 0;
    td_u32 j = 0;
    td_u32 out_len = 0;
    td_u8 cnt[DRV_WORD_WIDTH];
    td_u8 md[HASH_SIZE_SHA_MAX];
    drv_pke_data hash = {0};
    td_u8 seed_buf[DRV_PKE_LEN_4096];
    drv_pke_data arr[2];

    crypto_log_dbg("before memset\n");
    (void)memset_s(cnt, DRV_WORD_WIDTH, 0x00, DRV_WORD_WIDTH);
    crypto_log_dbg("after memset\n");
    crypto_log_dbg("before memset\n");
    (void)memset_s(md, HASH_SIZE_SHA_MAX, 0x00, HASH_SIZE_SHA_MAX);
    crypto_log_dbg("after memset\n");
    ret = memcpy_s(seed_buf, DRV_PKE_LEN_4096, seed, seed_len);
    crypto_chk_return(ret != TD_SUCCESS, ret, "memcpy_s failed, ret is 0x%x\n", ret);

    hash.length = HASH_SIZE_SHA_MAX;
    hash.data = md;

    arr[0].data = seed_buf;
    arr[0].length = seed_len;
    arr[1].data = cnt;
    arr[1].length = (uintptr_t)sizeof(cnt);

    for (i = 0; out_len < mask_len; i++) {
        cnt[WORD_INDEX_0] = (td_u8)((i >> SHIFT_24BITS) & MAX_LOW_8BITS);
        cnt[WORD_INDEX_1] = (td_u8)((i >> SHIFT_16BITS) & MAX_LOW_8BITS);
        cnt[WORD_INDEX_2] = (td_u8)((i >> SHIFT_8BITS)) & MAX_LOW_8BITS;
        cnt[WORD_INDEX_3] = (td_u8)(i & MAX_LOW_8BITS);

        /* H = Hash(seedbuf || cnt) */
        ret = crypto_drv_pke_common_calc_hash(arr, sizeof(arr) / sizeof(arr[0]), hash_info->hash_type, &hash);
        crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_calc_hash failed, ret is 0x%x\n", ret);

        for (j = 0; (j < hash_info->hash_len) && (out_len < mask_len); j++) {
            mask[out_len++] ^= md[j];
        }
    }
    return TD_SUCCESS;
}

static td_s32 pkcs1_get_hash(const drv_pke_hash_type hash_type, const drv_pke_data *label,
    rsa_pkcs1_hash_info *hash_info)
{
    td_s32 ret = TD_FAILURE;
    drv_pke_data h_hash = {0};

    for (td_u32 i = 0; i < sizeof(g_pke_pkcs1_hash) / sizeof(pkcs1_hash_t); i++) {
        if (g_pke_pkcs1_hash[i].hash_type == hash_type) {
            hash_info->hash_type = g_pke_pkcs1_hash[i].hash_type;
            hash_info->hash_len = g_pke_pkcs1_hash[i].hash_len;
            hash_info->asn1_data = (td_u8 *)g_pke_pkcs1_hash[i].asn1_data;
            hash_info->asn1_len = g_pke_pkcs1_hash[i].asn1_len;
            hash_info->lhash_data = (td_u8 *)g_pke_pkcs1_hash[i].lhash_data;
        }
    }

    if (label != TD_NULL && label->data != TD_NULL && label->length != 0) {
        hash_info->lhash_data = g_l_hash;
        h_hash.data = hash_info->lhash_data;
        h_hash.length = hash_info->hash_len;

        ret = crypto_drv_pke_common_calc_hash(label, 1, hash_info->hash_type, &h_hash);
        crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_calc_hash failed, ret is 0x%x\n", ret);
    }

    return TD_SUCCESS;
}

/* H = Hash( M' ) = Hash( Padding1 || mHash || salt ) */
static td_s32 pkcs1_pss_hash(const rsa_pkcs1_hash_info *hash_info, const td_u8 *m_hash, const td_u32 klen,
    const drv_pke_data *salt, drv_pke_data *hash)
{
    volatile td_s32 ret = TD_FAILURE;
    td_u8 ps[8];
    drv_pke_data arr[3]; // 3 elements in total

    crypto_param_check(m_hash == TD_NULL);
    crypto_param_check(salt == TD_NULL);
    crypto_param_check(salt->data == TD_NULL);
    crypto_param_check(hash == TD_NULL);

    crypto_log_dbg("before memset\n");
    (void)memset_s(ps, RSA_PADLEN_8, 0x00, RSA_PADLEN_8);
    crypto_log_dbg("after memset\n");

    /* *** H = Hash(PS || MHash || SALT) *** */
    arr[0].data = ps; // 0 th element is ps
    arr[0].length = RSA_PADLEN_8; // 0 element is PS
    arr[1].data = (td_u8 *)m_hash; // 1 element is MHash
    arr[1].length = klen; // 1 element is MHash
    arr[2].data = (td_u8 *)salt->data; // 2 element is SALT
    arr[2].length = salt->length; // 2 element is SALT
    ret = crypto_drv_pke_common_calc_hash(arr, sizeof(arr) / sizeof(arr[0]), hash_info->hash_type, hash);
    crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_calc_hash failed, ret is 0x%x\n", ret);

    return TD_SUCCESS;
}

/* EM = 00 || 01 || PS || 00 || T */
static td_s32 pkcs1_v15_sign(const drv_pke_hash_type hash_type, const rsa_pkcs1_pack *pack)
{
    td_s32 ret = TD_FAILURE;
    rsa_pkcs1_hash_info hash_info = {0};
    td_u8 *p = TD_NULL;
    td_u32 idx = 0;

    crypto_param_check(pack == TD_NULL);
    crypto_param_check(pack->em == TD_NULL);
    crypto_param_check(pack->hash == TD_NULL);
    crypto_param_check(pack->em_len != pack->klen);

    ret = pkcs1_get_hash(hash_type, TD_NULL, &hash_info);
    crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_get_hash failed, ret is 0x%x\n", ret);

    p = pack->em;
    p[idx++] = 0x00;
    p[idx++] = 0x01;

    /* PS */
    while (idx < (pack->em_len - hash_info.hash_len - hash_info.asn1_len - 1)) {
        p[idx++] = 0xFF;
    }

    p[idx++] = 0x00;

    /* T */
    (void)memcpy_s(&p[idx], hash_info.asn1_len, hash_info.asn1_data, hash_info.asn1_len);
    idx += hash_info.asn1_len;
    (void)memcpy_s(&p[idx], hash_info.hash_len, pack->hash, hash_info.hash_len);

    return TD_SUCCESS;
}

/* check EM = 00 || 01 || PS || 00 || T */
static td_s32 pkcs1_v15_verify(const drv_pke_hash_type hash_type, const rsa_pkcs1_pack *pack)
{
    td_s32 ret = TD_FAILURE;
    rsa_pkcs1_hash_info hash_info = {0};
    td_u8 *p = TD_NULL;
    td_u32 idx = 0;

    crypto_param_check(pack == TD_NULL);
    crypto_param_check(pack->em == TD_NULL);
    crypto_param_check(pack->hash == TD_NULL);
    crypto_param_check(pack->em_len != pack->klen);

    ret = pkcs1_get_hash(hash_type, TD_NULL, &hash_info);
    crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_get_hash failed, ret is 0x%x\n", ret);

    p = pack->em;
    if (p[idx++] != 0x00) {
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }
    if (p[idx++] != 0x01) {
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }

    /* PS */
    while (idx < (pack->em_len - hash_info.hash_len - hash_info.asn1_len - 1)) {
        if (p[idx++] != 0xFF) {
            return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
        }
    }
    if (p[idx++] != 0x00) {
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }

    /* T */
    if (memcmp(&p[idx], hash_info.asn1_data, hash_info.asn1_len) != 0) {
        crypto_log_err("check v15 asn1 failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }
    if (memcmp(hash_info.asn1_data, &p[idx], hash_info.asn1_len) != 0) {
        crypto_log_err("check v15 asn1 failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }

    idx += hash_info.asn1_len;
    if (memcmp(&p[idx], pack->hash, hash_info.hash_len) != 0) {
        crypto_log_err("check v15 hash failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }
    if (memcmp(pack->hash, &p[idx], hash_info.hash_len) != 0) {
        crypto_log_err("check v15 hash failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }
    return TD_SUCCESS;
}

static td_s32 pkcs1_pss_sign(const drv_pke_hash_type hash_type, const rsa_pkcs1_pack *pack)
{
    td_s32 ret = TD_FAILURE;
    rsa_pkcs1_hash_info hash_info = {0};
    td_u8 salt_data[HASH_SIZE_SHA_MAX];
    td_u32 salt_len = 0;
    td_u32 tmp_len = 0;
    td_u32 ms_bits = 0;
    td_u8 *masked_db = TD_NULL;
    drv_pke_data masked_seed;
    drv_pke_data salt = {0};

    crypto_param_check(pack == TD_NULL);
    crypto_param_check(pack->em == TD_NULL);
    crypto_param_check(pack->hash == TD_NULL);

    ret = pkcs1_get_hash(hash_type, TD_NULL, &hash_info);
    crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_get_hash failed, ret is 0x%x\n", ret);

    salt_len = hash_info.hash_len;

    crypto_param_check(pack->em_len != pack->klen);
    crypto_param_check(pack->klen < (pack->hash_len + salt_len + RSA_PADLEN_2));

    ms_bits = (pack->em_bit - BOUND_VALUE_1) & MAX_LOW_3BITS;

    ret = get_random((td_u8 *)salt_data, salt_len);
    crypto_chk_return(ret != TD_SUCCESS, ret, "get_random failed, ret is 0x%x\n", ret);

    masked_db = pack->em;
    masked_seed.data = pack->em + pack->em_len - pack->hash_len - 1; /* 1 byte for bound. */
    masked_seed.length = pack->hash_len;

    salt.data = salt_data;
    salt.length = salt_len;
    ret = pkcs1_pss_hash(&hash_info, pack->hash, pack->hash_len, &salt, &masked_seed);
    crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_pss_hash failed, ret is 0x%x\n", ret);

    tmp_len = pack->em_len - pack->hash_len - salt_len - RSA_PADLEN_2; /* padding2.length - 1 */
    crypto_log_dbg("before memset\n");
    (void)memset_s(masked_db, tmp_len, 0x00, tmp_len);
    crypto_log_dbg("after memset\n");
    masked_db[tmp_len++] = 0x01;
    (void)memcpy_s(masked_db + tmp_len, salt_len, salt_data, salt_len);

    tmp_len = pack->em_len - pack->hash_len - 1;
    ret = pkcs1_mgf(&hash_info, masked_seed.data, pack->hash_len, masked_db, tmp_len);
    crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_mgf failed, ret is 0x%x\n", ret);

    pack->em[0] &= MAX_LOW_8BITS >> (BYTE_BITS - ms_bits);
    pack->em[pack->em_len - 1] = RSA_PAD_XBC; /* 1 byte for bound. */
    return TD_SUCCESS;
}

static td_s32 pkcs1_pss_verify(const drv_pke_hash_type hash_type, const rsa_pkcs1_pack *pack)
{
    td_s32 ret = TD_FAILURE;
    rsa_pkcs1_hash_info hash_info = {0};
    td_u8 em[DRV_PKE_LEN_4096];
    td_u8 hash[HASH_SIZE_SHA_MAX];
    td_u8 *phash = hash;
    td_u32 salt_len = 0;
    td_u32 tmp_len = 0;
    td_u32 ms_bits = 0;
    td_u8 *masked_db = TD_NULL;
    td_u8 *masked_seed = TD_NULL;
    drv_pke_data hash_data = { .data = hash, .length = sizeof(hash) };
    drv_pke_data salt = {0};

    ret = pkcs1_get_hash(hash_type, TD_NULL, &hash_info);
    crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_get_hash failed, ret is 0x%x\n", ret);

    if (pack->em[pack->em_len - 1] != RSA_PAD_XBC) {
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_PSS_CHECK);
    }

    ms_bits = (pack->em_bit - BOUND_VALUE_1) & MAX_LOW_3BITS;
    if ((pack->em[0] & (MAX_LOW_8BITS << ms_bits)) != 0) {
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_PSS_CHECK);
    }

    ret = memcpy_s(em, DRV_PKE_LEN_4096, pack->em, pack->em_len);
    crypto_chk_return(ret != TD_SUCCESS, ret, "memcpy_s failed, ret is 0x%x\n", ret);

    masked_db = em;
    masked_seed = em + pack->em_len - pack->hash_len - 1; /* 1 byte for bound. */
    tmp_len = pack->em_len - pack->hash_len - 1;
    ret = pkcs1_mgf(&hash_info, masked_seed, pack->hash_len, masked_db, tmp_len);
    crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_mgf failed, ret is 0x%x\n", ret);

    em[0] &= MAX_LOW_8BITS >> (BYTE_BITS - ms_bits);

    while (masked_db < masked_seed - 1 && *masked_db == 0) {
        masked_db++;
    }

    if (*masked_db++ != 0x01) {
        crypto_log_err("pss padding check failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_PSS_CHECK);
    }

    salt_len = masked_seed - masked_db;
    salt.length = salt_len;
    salt.data = masked_db;

    ret = pkcs1_pss_hash(&hash_info, pack->hash, pack->hash_len, &salt, &hash_data);
    crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_pss_hash failed, ret is 0x%x\n", ret);

    if (memcmp(phash, masked_seed, hash_info.hash_len) != 0) {
        crypto_log_err("pss hash check failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_PSS_CHECK);
    }
    if (memcmp(masked_seed, hash, hash_info.hash_len) != 0) { /* reduntancy check */
        crypto_log_err("pss hash check failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_PSS_CHECK);
    }

    return TD_SUCCESS;
}

/* EM = 00 || 02 || PS || 00 || M */
static td_s32 pkcs1_v15_encrypt(const rsa_pkcs1_pack *pack)
{
    td_s32 ret = TD_FAILURE;
    td_u8 *p = TD_NULL;
    td_u32 idx = 0;

    crypto_param_check(pack == TD_NULL);
    crypto_param_check(pack->em == TD_NULL);
    crypto_param_check(pack->data == TD_NULL && pack->data_len != 0);
    crypto_param_check(pack->em_len != pack->klen);
    crypto_param_check(pack->data_len > pack->klen - RSA_PADLEN_11);

    p = pack->em;
    p[idx++] = 0x00;
    p[idx++] = 0x02;

    /* PS */
    ret = get_random(&p[idx], pack->em_len - pack->data_len - RSA_PADLEN_3);
    if (ret != TD_SUCCESS) {
        crypto_log_err("get random ps failed!\n");
        return ret;
    }
    idx += pack->em_len - pack->data_len - RSA_PADLEN_3;

    p[idx++] = 0x00;
    /* M */
    if (pack->data_len != 0) {
        (void)memcpy_s(&p[idx], pack->data_len, pack->data, pack->data_len);
    }

    return TD_SUCCESS;
}

/* check EM = 00 || 02 || PS || 00 || M */
static td_s32 pkcs1_v15_decrypt(rsa_pkcs1_pack *pack)
{
    td_s32 ret = TD_FAILURE;
    td_u8 *p = TD_NULL;
    td_u32 idx = 0;
    td_u32 len = 0;

    crypto_param_check(pack == TD_NULL);
    crypto_param_check(pack->em == TD_NULL);
    crypto_param_check(pack->data == TD_NULL && pack->data_len != 0);
    crypto_param_check(pack->em_len != pack->klen);

    p = pack->em;
    if (p[idx++] != 0x00) {
        crypto_log_err("padding check failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_V15_CHECK);
    }

    if (p[idx++] != 0x02) {
        crypto_log_err("padding check failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_V15_CHECK);
    }

    /* PS */
    while ((idx < (pack->em_len - RSA_PADLEN_1)) && p[idx] != 0x00) {
        idx++;
    }
    idx++;

    len = pack->em_len - idx;
    crypto_param_check(len > pack->em_len - RSA_PADLEN_11);

    /* M */
    if (len != 0) {
        ret = memcpy_s(pack->data, pack->data_len, &p[idx], len);
        crypto_chk_return(ret != TD_SUCCESS, ret, "memcpy_s failed, ret is 0x%x\n", ret);
    }
    pack->data_len = len; // record length of plain text for return

    return TD_SUCCESS;
}

static td_s32 pkcs1_oaep_encrypt(const drv_pke_hash_type hash_type, const rsa_pkcs1_pack *pack,
    const drv_pke_data *label)
{
    td_s32 ret = TD_FAILURE;
    rsa_pkcs1_hash_info hash_info = {0};
    td_u8 *p = TD_NULL;
    td_u8 *db = TD_NULL;
    td_u8 *seed = TD_NULL;
    td_u32 idx = 0;
    td_u32 hash_len = 0;
    td_u32 tmp_len = 0;

    crypto_param_check(pack == TD_NULL);
    crypto_param_check(pack->em == TD_NULL);
    crypto_param_check(pack->data == TD_NULL && pack->data_len != 0);

    ret = pkcs1_get_hash(hash_type, label, &hash_info);
    crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_get_hash failed, ret is 0x%x\n", ret);
    hash_len = hash_info.hash_len;

    crypto_param_check(pack->klen < 2 * hash_len + 2); /* Prevent inversion. eg. RSA-1024-v21-sha512 */
    crypto_param_check(pack->data_len > pack->klen - 2 * hash_len - 2); /* 2 */

    p = pack->em;
    seed = p + 1;
    db = p + hash_len + 1;

    /* 1. set data[0] = 0 */
    p[idx++] = 0x00;

    /* 2. set data[1, hash_len + 1] = random */
    ret = get_random(&p[idx], hash_len);
    crypto_chk_return(ret != TD_SUCCESS, ret, "get_random failed, ret is 0x%x\n", ret);
    idx += hash_len;

    /* 3. set data[hash_len + 1, 2hash_len + 1] = lHash */
    (void)memcpy_s(p + idx, hash_len, hash_info.lhash_data, hash_len);
    idx += hash_len;

    /* 4. set PS with 0x00 */
    tmp_len = pack->klen - pack->data_len - 2 * hash_len - 2; /* 2 */
    crypto_log_dbg("before memset\n");
    (void)memset_s(p + idx, tmp_len, 0x00, tmp_len);
    crypto_log_dbg("after memset\n");
    idx += tmp_len;

    /* 5. set 0x01 after PS */
    p[idx++] = 0x01;

    /* 6. set M */
    if (pack->data_len != 0) {
        (void)memcpy_s(p + idx, pack->data_len, pack->data, pack->data_len);
    }
    /* 7. MGF: seed -> db */
    tmp_len = pack->klen - hash_len - 1;
    ret = pkcs1_mgf(&hash_info, seed, hash_len, db, tmp_len);
    crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_mgf failed, ret is 0x%x\n", ret);

    /* 8. MGF: db -> seed */
    ret = pkcs1_mgf(&hash_info, db, tmp_len, seed, hash_len);
    crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_mgf failed, ret is 0x%x\n", ret);

    return TD_SUCCESS;
}

static td_s32 pkcs1_oaep_decrypt(const drv_pke_hash_type hash_type, rsa_pkcs1_pack *pack,
    const drv_pke_data *label)
{
    td_s32 ret = TD_FAILURE;
    rsa_pkcs1_hash_info hash_info = {0};
    td_u32 idx = 0;
    td_u32 hash_len = 0;
    td_u32 tmp_len = 0;
    td_u8 *p = TD_NULL;
    td_u8 *db = TD_NULL;
    td_u8 *seed = TD_NULL;

    crypto_param_check(pack == TD_NULL);
    crypto_param_check(pack->em == TD_NULL);
    crypto_param_check(pack->data == TD_NULL && pack->data_len != 0);

    ret = pkcs1_get_hash(hash_type, label, &hash_info);
    crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_get_hash failed, ret is 0x%x\n", ret);

    hash_len = hash_info.hash_len;
    crypto_param_check(pack->klen < 2 * hash_len + 2); /* Prevent inversion. eg. RSA-1024-v21-sha512 */

    p = pack->em;
    seed = p + 1;
    db = p + hash_len + 1;

    /* 1. check data[0] = 0 */
    if (p[idx++] != 0x00) {
        crypto_log_err("oaep padding check failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_OAEP_CHECK);
    }

    /* 2. MGF: db -> seed */
    tmp_len = pack->klen - hash_len - 1;
    ret = pkcs1_mgf(&hash_info, db, tmp_len, seed, hash_len);
    crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_mgf failed, ret is 0x%x\n", ret);

    /* 3. MGF: seed -> db */
    ret = pkcs1_mgf(&hash_info, seed, hash_len, db, tmp_len);
    crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_mgf failed, ret is 0x%x\n", ret);

    /* 4. check data[hash + 1, 2hash + 1] */
    idx += hash_len;
    if (memcmp(p + idx, hash_info.lhash_data, hash_len) != 0) {
        crypto_log_err("oaep lhash check failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_OAEP_CHECK);
    }

    /* 5. remove PS */
    idx += hash_len;
    for (; idx < pack->klen - 1; idx++) {
        if (p[idx] != 0x00) {
            break;
        }
    }

    /* 6. check 0x01 */
    if (p[idx++] != 0x01) {
        crypto_log_err("oaep check failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_OAEP_CHECK);
    }

    /* 7. check data length */
    tmp_len = pack->klen - idx;

    if (tmp_len != 0) {
        ret = memcpy_s(pack->data, pack->data_len, p + idx, tmp_len);
        crypto_chk_return(ret != TD_SUCCESS, ret, "memcpy_s failed, ret is 0x%x\n", ret);
    }
    pack->data_len = tmp_len; // record length of plain text for return

    return TD_SUCCESS;
}

static td_s32 rsa_sign(
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    td_u8 *em,
    const td_u32 klen,
    const td_u32 em_bit,
    const drv_pke_data *msg)
{
    td_s32 ret = TD_FAILURE;
    rsa_pkcs1_pack sg_pack = {
        .klen = klen,
        .em_bit = em_bit,
        .em = em,
        .em_len = klen,
        .hash = msg->data,
        .hash_len = msg->length,
        .data = TD_NULL,
        .data_len = 0
    };

    if (scheme == DRV_PKE_RSA_SCHEME_PKCS1_V15) {
        ret = pkcs1_v15_sign(hash_type, &sg_pack);
        crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_v15_sign failed, ret is 0x%x\n", ret);
    } else if (scheme == DRV_PKE_RSA_SCHEME_PKCS1_V21) {
        ret = pkcs1_pss_sign(hash_type, &sg_pack);
        crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_pss_sign failed, ret is 0x%x\n", ret);
    }
    return TD_SUCCESS;
}

static td_s32 rsa_verify(
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    td_u8 *em,
    const td_u32 klen,
    const td_u32 em_bit,
    const drv_pke_data *msg)
{
    td_s32 ret = TD_FAILURE;
    rsa_pkcs1_pack vf_pack = {
        .klen = klen,
        .em_bit = em_bit,
        .em = em,
        .em_len = klen,
        .hash = msg->data,
        .hash_len = msg->length,
        .data = TD_NULL,
        .data_len = 0
    };

    if (scheme == DRV_PKE_RSA_SCHEME_PKCS1_V15) {
        ret = pkcs1_v15_verify(hash_type, &vf_pack);
        crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_v15_verify failed, ret is 0x%x\n", ret);
    } else if (scheme == DRV_PKE_RSA_SCHEME_PKCS1_V21) {
        ret = pkcs1_pss_verify(hash_type, &vf_pack);
        crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_pss_verify failed, ret is 0x%x\n", ret);
    }
    return TD_SUCCESS;
}

static td_s32 rsa_encrypt(
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    td_u8 *em,
    const td_u32 klen,
    const drv_pke_data *msg,
    const drv_pke_data *label)
{
    td_s32 ret = TD_FAILURE;
    rsa_pkcs1_pack en_pack = {
        .klen = klen,
        .em_bit = 0,
        .em = em,
        .em_len = klen,
        .hash = TD_NULL,
        .hash_len = 0,
        .data = msg->data,
        .data_len = msg->length
    };

    if (scheme == DRV_PKE_RSA_SCHEME_PKCS1_V15) {
        ret = pkcs1_v15_encrypt(&en_pack);
        crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_v15_encrypt failed, ret is 0x%x\n", ret);
    } else if (scheme == DRV_PKE_RSA_SCHEME_PKCS1_V21) {
        ret = pkcs1_oaep_encrypt(hash_type, &en_pack, label);
        crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_oaep_encrypt failed, ret is 0x%x\n", ret);
    }
    return TD_SUCCESS;
}

static td_s32 rsa_decrypt(const drv_pke_rsa_scheme scheme, const drv_pke_hash_type hash_type, td_u8 *em,
    const td_u32 klen, const drv_pke_data *label, drv_pke_data *msg)
{
    td_s32 ret = TD_FAILURE;
    rsa_pkcs1_pack de_pack = {
        .klen = klen,
        .em_bit = 0,
        .em = em,
        .em_len = klen,
        .hash = TD_NULL,
        .hash_len = 0,
        .data = msg->data,
        .data_len = msg->length
    };

    if (scheme == DRV_PKE_RSA_SCHEME_PKCS1_V15) {
        ret = pkcs1_v15_decrypt(&de_pack);
        crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_v15_decrypt failed, ret is 0x%x\n", ret);
        msg->length = de_pack.data_len; // record length of plain text for return
    } else if (scheme == DRV_PKE_RSA_SCHEME_PKCS1_V21) {
        ret = pkcs1_oaep_decrypt(hash_type, &de_pack, label);
        crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_oaep_decrypt failed, ret is 0x%x\n", ret);
        msg->length = de_pack.data_len; // record length of plain text for return
    }
    return TD_SUCCESS;
}

static td_s32 pke_rsa_sign(
    const drv_pke_rsa_priv_key *priv_key,
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    const drv_pke_data *input_hash,
    const drv_pke_data *sign)
{
    td_s32 ret = TD_FAILURE;
    td_u8 *em;
    td_u32 klen = priv_key->n_len;
    td_u32 em_bit = 0;
    drv_pke_data in = {0};
    drv_pke_data n_data = {0};
    drv_pke_data d_data = {0};

    em = crypto_malloc(DRV_PKE_LEN_4096);
    if (em == TD_NULL) {
        crypto_log_err("%s:%d Error! Malloc memory failed!\n", __FUNCTION__, __LINE__);
        return PKE_COMPAT_ERRNO(ERROR_MALLOC);
    }
    crypto_log_dbg("before memset\n");
    (void)memset_s(em, DRV_PKE_LEN_4096, 0x00, DRV_PKE_LEN_4096);
    crypto_log_dbg("after memset\n");

    em_bit = rsa_get_bit_num((const td_u8 *)priv_key->n, priv_key->n_len);
    ret = rsa_sign(scheme, hash_type, em, klen, em_bit, input_hash);
    crypto_chk_goto_with_ret(ret, ret != TD_SUCCESS, exit__, ret, "rsa_sign failed, ret is 0x%x\n", ret);

    n_data = (drv_pke_data) {.length = priv_key->n_len, .data = priv_key->n};
    d_data = (drv_pke_data) {.length = priv_key->d_len, .data = priv_key->d};
    in = (drv_pke_data) {.length = klen, .data = em};
    ret = drv_cipher_pke_exp_mod(&n_data, &d_data, &in, sign);
    crypto_chk_goto_with_ret(ret, ret != TD_SUCCESS, exit__, ret, "drv_cipher_pke_exp_mod failed, ret is 0x%x\n", ret);

    if (memcmp(input_hash->data, sign->data, input_hash->length) == 0) {
        ret = PKE_COMPAT_ERRNO(ERROR_PKE_RSA_SAME_DATA);
    }

exit__:
    if (em != TD_NULL) {
        crypto_log_dbg("before memset\n");
        (void)memset_s(em, DRV_PKE_LEN_4096, 0, DRV_PKE_LEN_4096);
        crypto_log_dbg("after memset\n");
        crypto_free(em);
    }
    return ret;
}

static td_s32 pke_rsa_verify(
    const drv_pke_rsa_pub_key *pub_key,
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    const drv_pke_data *input_hash,
    const drv_pke_data *sign)
{
    td_s32 ret = TD_FAILURE;
    td_u8 *em;
    td_u8 *pem;
    td_u32 klen = pub_key->len;
    td_u32 em_bit = 0;
    drv_pke_data out = {0};
    drv_pke_data n_data = {0};
    drv_pke_data e_data = {0};

    em = crypto_malloc(DRV_PKE_LEN_4096);
    if (em == TD_NULL) {
        crypto_log_err("%s:%d Error! Malloc memory failed!\n", __FUNCTION__, __LINE__);
        return PKE_COMPAT_ERRNO(ERROR_MALLOC);
    }
    crypto_log_dbg("before memset\n");
    (void)memset_s(em, DRV_PKE_LEN_4096, 0x00, DRV_PKE_LEN_4096);
    crypto_log_dbg("after memset\n");
    pem = em;

    n_data = (drv_pke_data) {.length = pub_key->len, .data = pub_key->n};
    e_data = (drv_pke_data) {.length = pub_key->len, .data = pub_key->e};
    out = (drv_pke_data) {.length = klen, .data = em};
    ret = drv_cipher_pke_exp_mod(&n_data, &e_data, sign, &out);
    crypto_chk_goto_with_ret(ret, ret != TD_SUCCESS, exit__, ret, "drv_cipher_pke_exp_mod failed, ret is 0x%x\n", ret);

    if (memcmp(sign->data, pem, pub_key->len) == 0) {
        ret = PKE_COMPAT_ERRNO(ERROR_PKE_RSA_SAME_DATA);
        goto exit__;
    }

    em_bit = rsa_get_bit_num((const td_u8 *)pub_key->n, pub_key->len);
    ret = rsa_verify(scheme, hash_type, em, klen, em_bit, input_hash);
    crypto_chk_goto_with_ret(ret, ret != TD_SUCCESS, exit__, ret, "rsa_verify failed, ret is 0x%x\n", ret);

exit__:
    if (em != TD_NULL) {
        crypto_log_dbg("before memset\n");
        (void)memset_s(em, DRV_PKE_LEN_4096, 0, DRV_PKE_LEN_4096);
        crypto_log_dbg("after memset\n");
        crypto_free(em);
    }
    return ret;
}

static td_s32 pke_rsa_encrypt(
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    const drv_pke_rsa_pub_key *pub_key,
    const drv_pke_data *input,
    const drv_pke_data *label,
    const drv_pke_data *output)
{
    td_s32 ret = TD_FAILURE;
    td_u8 em[DRV_PKE_LEN_4096];
    td_u32 klen = pub_key->len;
    drv_pke_data n_data = {.data = pub_key->n, .length = klen};
    drv_pke_data e_data = {.data = pub_key->e, .length = klen};
    drv_pke_data em_data = {.data = em, .length = klen};

    crypto_log_dbg("before memset\n");
    (void)memset_s(em, sizeof(em), 0x00, sizeof(em));
    crypto_log_dbg("after memset\n");
    ret = rsa_encrypt(scheme, hash_type, em, klen, input, label);
    crypto_chk_return(ret != TD_SUCCESS, ret, "rsa_encrypt failed, ret is 0x%x\n", ret);
    ret = drv_cipher_pke_exp_mod(&n_data, &e_data, &em_data, output);
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_pke_exp_mod failed, ret is 0x%x\n", ret);
    if (memcmp(input->data, output->data, input->length) == 0) {
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_SAME_DATA);
    }

    return TD_SUCCESS;
}

static td_s32 pke_rsa_decrypt(
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    const drv_pke_rsa_priv_key *priv_key,
    const drv_pke_data *input,
    const drv_pke_data *label,
    drv_pke_data *output)
{
    td_s32 ret = TD_FAILURE;
    td_u8 em[DRV_PKE_LEN_4096];
    td_u32 klen = priv_key->n_len;
    drv_pke_data n_data = {.data = priv_key->n, .length = klen};
    drv_pke_data d_data = {.data = priv_key->d, .length = klen};
    drv_pke_data em_data = {.data = em, .length = klen};

    (void)memset_s(em, sizeof(em), 0x00, sizeof(em));

    ret = drv_cipher_pke_exp_mod(&n_data, &d_data, input, &em_data);
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_pke_exp_mod failed, ret is 0x%x\n", ret);
    ret = rsa_decrypt(scheme, hash_type, em, klen, label, output);
    crypto_chk_return(ret != TD_SUCCESS, ret, "rsa_decrypt failed, ret is 0x%x\n", ret);
    if (memcmp(input->data, output->data, output->length) == 0) {
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_SAME_DATA);
    }

    return TD_SUCCESS;
}

// expose layer
td_s32 drv_cipher_pke_rsa_sign(
    const drv_pke_rsa_priv_key *priv_key,
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    const drv_pke_data *input_hash,
    const drv_pke_data *sign)
{
    td_s32 ret = TD_FAILURE;
    td_u32 klen = 0;
    td_u32 hash_len = 0;
    /* check ptr. */
    pke_null_ptr_chk(priv_key);
    pke_null_ptr_chk(priv_key->n);
    pke_null_ptr_chk(priv_key->d);
    pke_null_ptr_chk(input_hash);
    pke_null_ptr_chk(input_hash->data);
    pke_null_ptr_chk(sign);
    pke_null_ptr_chk(sign->data);

    /* check enum. */
    crypto_chk_return(scheme >= DRV_PKE_RSA_SCHEME_MAX, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "scheme is Invalid\n");
    crypto_chk_return(hash_type < DRV_PKE_HASH_TYPE_SHA1 || hash_type > DRV_PKE_HASH_TYPE_SHA512,
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "hash_type is Invalid\n");

    /* check length. */
    klen = priv_key->n_len;
    hash_len = drv_get_hash_len(hash_type);
    crypto_chk_return(hash_len == 0, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "hash_type is Invalid\n");

    crypto_chk_return(klen != DRV_PKE_LEN_1024 && klen != DRV_PKE_LEN_2048 && klen != DRV_PKE_LEN_3072 &&
        klen != DRV_PKE_LEN_4096, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "n_len is Invalid\n");

    /* check whether the security strength is the same. */
    crypto_chk_return((klen == DRV_PKE_LEN_2048 && hash_type < DRV_PKE_HASH_TYPE_SHA224) ||
        (klen == DRV_PKE_LEN_3072 && hash_type < DRV_PKE_HASH_TYPE_SHA256) ||
        (klen == DRV_PKE_LEN_4096 && hash_type < DRV_PKE_HASH_TYPE_SHA384),
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "the security strength of the hash doesn't match the rsa!\n");

    if ((priv_key->d_len != klen) || (input_hash->length != hash_len)) {
        ret = PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
        crypto_log_err("d_len or hash_len is Invalid!\n");
        return ret;
    }
    crypto_chk_return(sign->length < klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "sign_len is Invalid\n");
    crypto_chk_return(crypto_rsa_support(klen, scheme) == TD_FALSE, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT),
        "alg is unsupport\n");
    ret = crypto_drv_pke_common_resume();
    crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_resume failed, ret is 0x%x\n", ret);
    ret = pke_rsa_sign(priv_key, scheme, hash_type, input_hash, sign);
    (void)crypto_drv_pke_common_suspend();
    return ret;
}

td_s32 drv_cipher_pke_rsa_verify(
    const drv_pke_rsa_pub_key *pub_key,
    drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    const drv_pke_data *input_hash,
    const drv_pke_data *sign)
{
    td_s32 ret = TD_FAILURE;
    td_u32 klen = 0;
    td_u32 hash_len = 0;
    /* check ptr. */
    pke_null_ptr_chk(pub_key);
    pke_null_ptr_chk(pub_key->n);
    pke_null_ptr_chk(pub_key->e);
    pke_null_ptr_chk(input_hash);
    pke_null_ptr_chk(input_hash->data);
    pke_null_ptr_chk(sign);
    pke_null_ptr_chk(sign->data);

    /* check enum. */
    crypto_chk_return(scheme >= DRV_PKE_RSA_SCHEME_MAX, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "scheme is Invalid\n");
    crypto_chk_return(hash_type < DRV_PKE_HASH_TYPE_SHA1 || hash_type > DRV_PKE_HASH_TYPE_SHA512,
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "hash_type is Invalid\n");

    /* check length. */
    klen = pub_key->len;
    hash_len = drv_get_hash_len(hash_type);
    crypto_chk_return(hash_len == 0, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "hash_type is Invalid\n");

    if ((klen != DRV_PKE_LEN_1024 && klen != DRV_PKE_LEN_2048 && klen != DRV_PKE_LEN_3072 &&
        klen != DRV_PKE_LEN_4096) || (input_hash->length != hash_len)) {
        ret = PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
        crypto_log_err("k_len or hash_len is Invalid!\n");
        return ret;
    }
    /* check whether the security strength is the same. */
    crypto_chk_return((klen == DRV_PKE_LEN_2048 && hash_type < DRV_PKE_HASH_TYPE_SHA224) ||
        (klen == DRV_PKE_LEN_3072 && hash_type < DRV_PKE_HASH_TYPE_SHA256) ||
        (klen == DRV_PKE_LEN_4096 && hash_type < DRV_PKE_HASH_TYPE_SHA384),
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "the security strength of the hash doesn't match the rsa!\n");

    crypto_chk_return(sign->length < klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "sign_len is Invalid\n");
    crypto_chk_return(crypto_rsa_support(klen, scheme) == TD_FALSE, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT),
        "alg is unsupport\n");
    ret = crypto_drv_pke_common_resume();
    crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_resume failed, ret is 0x%x\n", ret);
    ret = pke_rsa_verify(pub_key, scheme, hash_type, input_hash, sign);
    (void)crypto_drv_pke_common_suspend();
    return ret;
}

td_s32 drv_cipher_pke_rsa_public_encrypt(
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    const drv_pke_rsa_pub_key *pub_key,
    const drv_pke_data *input,
    const drv_pke_data *label,
    drv_pke_data *output)
{
    td_s32 ret = TD_FAILURE;
    td_u32 klen = 0;
    td_u32 hash_len = 0;
    td_u32 input_len = 0;
    /* check ptr. */
    pke_null_ptr_chk(pub_key);
    pke_null_ptr_chk(pub_key->n);
    pke_null_ptr_chk(pub_key->e);
    pke_null_ptr_chk(input);
    pke_null_ptr_chk(input->data);
    pke_null_ptr_chk(output);
    pke_null_ptr_chk(output->data);

    /* check enum. */
    crypto_chk_return(scheme >= DRV_PKE_RSA_SCHEME_MAX, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "scheme is Invalid\n");
    crypto_chk_return(scheme == DRV_PKE_RSA_SCHEME_PKCS1_V21 &&
        (hash_type < DRV_PKE_HASH_TYPE_SHA1 || hash_type > DRV_PKE_HASH_TYPE_SHA512),
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "hash_type is Invalid\n");

    /* check length. */
    klen = pub_key->len;
    input_len = input->length;
    hash_len = drv_get_hash_len(hash_type);
    crypto_chk_return(hash_len == 0, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "hash_type is Invalid\n");

    crypto_chk_return(klen != DRV_PKE_LEN_1024 && klen != DRV_PKE_LEN_2048 && klen != DRV_PKE_LEN_3072 &&
        klen != DRV_PKE_LEN_4096, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "n_len is Invalid\n");
    /*
     * For V15, the plain_text's max length is klen - 11.
     * For OAEP, the plain_text's max length is klen - 2 * hash_len - 2.
     */
    if (scheme == DRV_PKE_RSA_SCHEME_PKCS1_V15) {
        crypto_chk_return(input_len > klen - RSA_PADLEN_11, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
            "input_len is too long for V15\n");
    } else {
        crypto_chk_return(input_len > klen - 2 * hash_len - 2,  // 2: refer to comment.
            PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "input_len is too long for V21\n");
    }
    crypto_chk_return(input_len == 0, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "input_len is zero\n");
    crypto_chk_return(output->length < klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "output_len is Invalid\n");
    crypto_chk_return(crypto_rsa_support(klen, scheme) == TD_FALSE, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT),
        "alg is unsupport\n");
    ret = crypto_drv_pke_common_resume();
    crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_resume failed, ret is 0x%x\n", ret);
    ret = pke_rsa_encrypt(scheme, hash_type, pub_key, input, label, output);
    (void)crypto_drv_pke_common_suspend();
    return ret;
}

td_s32 drv_cipher_pke_rsa_private_decrypt(
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    const drv_pke_rsa_priv_key *priv_key,
    const drv_pke_data *input,
    const drv_pke_data *label,
    drv_pke_data *output)
{
    td_s32 ret = TD_FAILURE;

    td_u32 klen = 0;
    td_u32 hash_len = 0;
    td_u32 output_len = 0;
    /* check ptr. */
    pke_null_ptr_chk(priv_key);
    pke_null_ptr_chk(priv_key->n);
    pke_null_ptr_chk(priv_key->d);
    pke_null_ptr_chk(input);
    pke_null_ptr_chk(input->data);
    pke_null_ptr_chk(output);
    pke_null_ptr_chk(output->data);

    /* check enum. */
    crypto_chk_return(scheme >= DRV_PKE_RSA_SCHEME_MAX, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "scheme is Invalid\n");
    crypto_chk_return(scheme == DRV_PKE_RSA_SCHEME_PKCS1_V21 &&
        (hash_type < DRV_PKE_HASH_TYPE_SHA1 || hash_type > DRV_PKE_HASH_TYPE_SHA512),
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "hash_type is Invalid\n");

    /* check length. */
    klen = priv_key->n_len;
    output_len = output->length;
    hash_len = drv_get_hash_len(hash_type);
    crypto_chk_return(hash_len == 0, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "hash_type is Invalid\n");

    crypto_chk_return(klen != DRV_PKE_LEN_1024 && klen != DRV_PKE_LEN_2048 && klen != DRV_PKE_LEN_3072 &&
        klen != DRV_PKE_LEN_4096, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "n_len is Invalid\n");
    crypto_chk_return(priv_key->d_len != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "d_len is Invalid\n");
    crypto_chk_return(input->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "input_len is Invalid\n");

    /*
     * For V15, the plain_text's max length is klen - 11.
     * For OAEP, the plain_text's max length is klen - 2 * hash_len - 2.
     */
    if (scheme == DRV_PKE_RSA_SCHEME_PKCS1_V15) {
        crypto_chk_return(output_len < klen - RSA_PADLEN_11, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
            "output_len is not enough for V15\n");
    } else {
        crypto_chk_return(output_len < klen - 2 * hash_len - 2,  // 2: refer to comment.
            PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "input_len is not enough for V21\n");
    }
    crypto_chk_return(crypto_rsa_support(klen, scheme) == TD_FALSE, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT),
        "alg is unsupport\n");
    ret = crypto_drv_pke_common_resume();
    crypto_chk_func_return(crypto_drv_pke_common_resume, ret);
    ret = pke_rsa_decrypt(scheme, hash_type, priv_key, input, label, output);
    crypto_drv_pke_common_suspend();
    return ret;
}

#if defined(CONFIG_RSA_GEN_KEY_SUPPORT)
static td_s32 pke_rsa_gen_key(const drv_pke_data *input_e,
    drv_pke_rsa_priv_key *output_priv_key)
{
    td_s32 ret = TD_FAILURE;
    td_u32 klen = input_e->length;
    td_u32 i, j;
    ret = hal_pke_wait_free();
    crypto_chk_return(ret != TD_SUCCESS, ret, "hal_pke_wait_free failed, ret is 0x%x\n", ret);

    ret = hal_pke_set_mode(HAL_PKE_MODE_KGEN_WITH_E, klen);
    crypto_chk_return(ret != TD_SUCCESS, ret, "hal_pke_set_mode failed, ret is 0x%x\n", ret);

    hal_pke_set_ram(PKE_KRAM, input_e->data, input_e->length, input_e->length);
    for (i = 0; i < RSA_GEN_KEY_TRY_TIMES; i++) {
        hal_pke_start();
        // generate key usually slow, need more time than other interface
        for (j = 0; j < RSA_GEN_KEY_WAIT_TIMES; j++) {
            ret = hal_pke_wait_done();
            if (ret == TD_SUCCESS) {
                break;
            }
        }
        crypto_chk_goto(ret != TD_SUCCESS, exit__, "hal_pke_wait_done failed, ret is 0x%x\n", ret);
        hal_pke_get_ram(PKE_MRAM, output_priv_key->n, klen);
        if ((output_priv_key->n[0] & 0xC0) != 0) { // 0xC0 = 0b11000000, check high two bit
            break;
        }
    }
    if (i == RSA_GEN_KEY_TRY_TIMES) {
        crypto_log_err("generate n failed!\n");
        ret = PKE_COMPAT_ERRNO(ERROR_PKE_RSA_GEN_KEY);
        goto exit__;
    }
    hal_pke_get_ram(PKE_RRAM, output_priv_key->d, klen);
exit__:
    // this is to ensure the success of clean ram
    for (i = 0; i < RSA_GEN_KEY_WAIT_TIMES; i++) {
        if (hal_pke_wait_free() == TD_SUCCESS) {
            break;
        }
    }
    (void)drv_cipher_pke_clean_ram();
    return ret;
}

td_s32 drv_cipher_pke_rsa_gen_key(
    const drv_pke_data *input_e,
    drv_pke_rsa_priv_key *output_priv_key)
{
    td_s32 ret = TD_FAILURE;
    td_u32 klen = 0;
    /* check ptr. */
    pke_null_ptr_chk(input_e);
    pke_null_ptr_chk(input_e->data);
    pke_null_ptr_chk(output_priv_key);
    pke_null_ptr_chk(output_priv_key->n);
    pke_null_ptr_chk(output_priv_key->d);

    /* check length. */
    klen = input_e->length;

    crypto_chk_return(klen != DRV_PKE_LEN_1024 && klen != DRV_PKE_LEN_2048 && klen != DRV_PKE_LEN_3072 &&
        klen != DRV_PKE_LEN_4096, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "gen key len is Invalid\n");
    crypto_chk_return(output_priv_key->n_len != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "n_len is Invalid\n");
    crypto_chk_return(output_priv_key->d_len != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "d_len is Invalid\n");

    ret = crypto_drv_pke_common_resume();
    crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_resume failed, ret is 0x%x\n", ret);
    ret = pke_rsa_gen_key(input_e, output_priv_key);
    (void)crypto_drv_pke_common_suspend();
    return ret;
}
#endif

td_s32 drv_cipher_pke_dh_gen_key(const drv_pke_data *g_data, const drv_pke_data *mod_n,
    const drv_pke_data *input_priv_key, const drv_pke_data *output_priv_key, const drv_pke_data *output_pub_key)
{
    td_s32 ret = TD_FAILURE;
    /* check ptr. */
    pke_null_ptr_chk(g_data);
    pke_null_ptr_chk(g_data->data);
    pke_null_ptr_chk(mod_n);
    pke_null_ptr_chk(mod_n->data);
    pke_null_ptr_chk(output_priv_key);
    pke_null_ptr_chk(output_priv_key->data);
    pke_null_ptr_chk(output_pub_key);
    pke_null_ptr_chk(output_pub_key->data);
    drv_crypto_pke_check_param(input_priv_key != TD_NULL && input_priv_key->length != 0 &&
        input_priv_key->data == TD_NULL);
    drv_crypto_pke_check_param(mod_n->length != DRV_PKE_LEN_1024 && mod_n->length != DRV_PKE_LEN_2048 &&
        mod_n->length != DRV_PKE_LEN_3072 && mod_n->length != DRV_PKE_LEN_4096);
    /* if there is input private key, the key length should equal to the modulus length. */
    drv_crypto_pke_check_param(input_priv_key != TD_NULL && input_priv_key->length != 0 &&
        mod_n->length != input_priv_key->length);
    drv_crypto_pke_check_param(mod_n->length < g_data->length);
    drv_crypto_pke_check_param(mod_n->length != output_priv_key->length);
    drv_crypto_pke_check_param(mod_n->length != output_pub_key->length);
    /* the modulur couldn't be zero. */
    drv_crypto_pke_check_param(crypto_drv_pke_common_is_zero(mod_n->data, mod_n->length) == TD_TRUE);

    ret = crypto_drv_pke_common_resume();
    crypto_chk_func_return(crypto_drv_pke_common_resume, ret);
    /* Step 1: get private key. */
    if (input_priv_key != TD_NULL  && input_priv_key->data != TD_NULL && input_priv_key->length != 0) {
        ret = memcpy_enhance(output_priv_key->data, output_priv_key->length, input_priv_key->data,
            input_priv_key->length);
        crypto_chk_func_return(memcpy_enhance, ret);
    } else {
        /* genarate private key by random. */
        ret = get_limit_random(mod_n, output_priv_key);
        crypto_chk_func_return(get_limit_random, ret);
    }
    /* step 2: get public key. */
    ret = drv_cipher_pke_exp_mod(mod_n, output_priv_key, g_data, output_pub_key);
    crypto_drv_pke_common_suspend();
    return ret;
}

CRYPTO_STATIC td_s32 pke_dh_check_pub_key(const drv_pke_data *mod_n, const drv_pke_data *input_pub_key)
{
    td_s32 ret = TD_FAILURE;
    td_u8 *tmp = crypto_malloc(input_pub_key->length);
    crypto_chk_return(tmp == TD_NULL, TD_FAILURE, "malloc failed\n");
    /* shouldn't be 0 */
    ret = crypto_drv_pke_common_is_zero(input_pub_key->data, input_pub_key->length);
    crypto_chk_goto_with_ret(ret, (ret == TD_TRUE), exit_free,
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "invalid public key\n");
    /* shouldn't be 1 */
    tmp[input_pub_key->length - 1] = 1;
    ret = memcmp_enhance(input_pub_key->data, tmp, input_pub_key->length);
    crypto_chk_goto_with_ret(ret, (ret == TD_SUCCESS), exit_free,
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "invalid public key\n");
    /* shouldn't be p */
    ret = memcmp_enhance(input_pub_key->data, mod_n->data, input_pub_key->length);
    crypto_chk_goto_with_ret(ret, (ret == TD_SUCCESS), exit_free,
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "invalid public key\n");
    /* shouldn't be p-1 */
    memcpy_enhance_chk_goto(ret, tmp, input_pub_key->length, mod_n->data, mod_n->length, exit_free);
    crypto_drv_pke_common_array_sub_value(tmp, input_pub_key->length, 1);
    ret = memcmp_enhance(input_pub_key->data, tmp, input_pub_key->length);
    crypto_chk_goto_with_ret(ret, (ret == TD_SUCCESS), exit_free,
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "invalid public key\n");
    ret = TD_SUCCESS;

exit_free:
    crypto_free(tmp);
    return ret;
}

td_s32 drv_cipher_pke_dh_compute_key(const drv_pke_data *mod_n, const drv_pke_data  *input_priv_key,
    const drv_pke_data  *input_pub_key, const drv_pke_data  *output_shared_key)
{
    td_s32 ret = TD_FAILURE;
    /* check ptr. */
    pke_null_ptr_chk(mod_n);
    pke_null_ptr_chk(mod_n->data);
    pke_null_ptr_chk(input_priv_key);
    pke_null_ptr_chk(input_priv_key->data);
    pke_null_ptr_chk(input_pub_key);
    pke_null_ptr_chk(input_pub_key->data);
    pke_null_ptr_chk(output_shared_key);
    pke_null_ptr_chk(output_shared_key->data);
    drv_crypto_pke_check_param(mod_n->length != DRV_PKE_LEN_1024 && mod_n->length != DRV_PKE_LEN_2048 &&
        mod_n->length != DRV_PKE_LEN_3072 && mod_n->length != DRV_PKE_LEN_4096);
    drv_crypto_pke_check_param(mod_n->length != input_priv_key->length);
    drv_crypto_pke_check_param(mod_n->length != input_pub_key->length);
    drv_crypto_pke_check_param(mod_n->length != output_shared_key->length);
    /* check whether public key is legal. */
    ret = pke_dh_check_pub_key(mod_n, input_pub_key);
    crypto_chk_func_return(pke_dh_check_pub_key, ret);

    ret = crypto_drv_pke_common_resume();
    crypto_chk_func_return(crypto_drv_pke_common_resume, ret);
    /* start calculate. */
    ret = drv_cipher_pke_exp_mod(mod_n, input_priv_key, input_pub_key, output_shared_key);
    crypto_drv_pke_common_suspend();
    return ret;
}