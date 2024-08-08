/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: hash kernel API function implementation.
 *
 * Create: 2023-05-26
*/

#include "kapi_hash.h"
#include <securec.h>
#include "kapi_inner.h"
#include "kapi_km.h"

#include "drv_hash.h"
#include "crypto_common_macro.h"
#include "crypto_common_def.h"
#include "crypto_errno.h"
#include "crypto_drv_common.h"

typedef struct {
    crypto_hash_type hmac_type;
    td_u8 *key;
    td_u32 key_length;
    td_u8 *msg;
    td_u32 msg_length;
} crypto_hkdf_hmac_param_t;

typedef struct {
    crypto_hash_type hmac_type;
    crypto_klad_hmac_type klad_hmac_type;
    crypto_hash_type hash_type;
    td_u32 block_size;
    td_u32 result_size;
} crypto_hkdf_table_item_t;

#define CRYPTO_HASH_INIT_MAX_NUM            256

static const crypto_hkdf_table_item_t g_hkdf_hmac_table[] = {
    {
        .hmac_type = CRYPTO_HASH_TYPE_HMAC_SHA1, .klad_hmac_type = CRYPTO_KLAD_HMAC_TYPE_SHA1,
        .hash_type = CRYPTO_HASH_TYPE_SHA1, .block_size = HASH_SHA1_BLOCK_SIZE,
        .result_size = HASH_SHA1_RESULT_SIZE
    },
    {
        .hmac_type = CRYPTO_HASH_TYPE_HMAC_SHA224, .klad_hmac_type = CRYPTO_KLAD_HMAC_TYPE_SHA224,
        .hash_type = CRYPTO_HASH_TYPE_SHA224, .block_size = HASH_SHA224_BLOCK_SIZE,
        .result_size = HASH_SHA224_RESULT_SIZE
    },
    {
        .hmac_type = CRYPTO_HASH_TYPE_HMAC_SHA256, .klad_hmac_type = CRYPTO_KLAD_HMAC_TYPE_SHA256,
        .hash_type = CRYPTO_HASH_TYPE_SHA256, .block_size = HASH_SHA256_BLOCK_SIZE,
        .result_size = HASH_SHA256_RESULT_SIZE
    },
    {
        .hmac_type = CRYPTO_HASH_TYPE_HMAC_SHA384, .klad_hmac_type = CRYPTO_KLAD_HMAC_TYPE_SHA384,
        .hash_type = CRYPTO_HASH_TYPE_SHA384, .block_size = HASH_SHA384_BLOCK_SIZE,
        .result_size = HASH_SHA384_RESULT_SIZE
    },
    {
        .hmac_type = CRYPTO_HASH_TYPE_HMAC_SHA512, .klad_hmac_type = CRYPTO_KLAD_HMAC_TYPE_SHA512,
        .hash_type = CRYPTO_HASH_TYPE_SHA512, .block_size = HASH_SHA512_BLOCK_SIZE,
        .result_size = HASH_SHA512_RESULT_SIZE
    },
    {
        .hmac_type = CRYPTO_HASH_TYPE_HMAC_SM3, .klad_hmac_type = CRYPTO_KLAD_HMAC_TYPE_SM3,
        .hash_type = CRYPTO_HASH_TYPE_SM3, .block_size = HASH_SM3_BLOCK_SIZE,
        .result_size = HASH_SM3_RESULT_SIZE
    }
};

#define HASH_COMPAT_ERRNO(err_code)     KAPI_COMPAT_ERRNO(ERROR_MODULE_HASH, err_code)

static const crypto_hkdf_table_item_t *priv_get_hkdf_table_item(crypto_hash_type hmac_type)
{
    td_u32 i;

    for (i = 0; i < crypto_array_size(g_hkdf_hmac_table); i++) {
        if (hmac_type == g_hkdf_hmac_table[i].hmac_type) {
            return &g_hkdf_hmac_table[i];
        }
    }
    return TD_NULL;
}

static td_s32 inner_hkdf_hash(const crypto_hash_type hash_type, td_u8 *data, td_u32 data_length,
    td_u8 *out, td_u32 *out_length)
{
    td_s32 ret = 0;
    td_handle hash_handle = 0;
    crypto_hash_attr hash_attr = { 0 };
    crypto_buf_attr src_buf = { 0 };

    hash_attr.hash_type = hash_type;
    hash_attr.is_long_term = TD_TRUE;
    ret = kapi_cipher_hash_start(&hash_handle, &hash_attr);
    crypto_chk_return((ret != TD_SUCCESS), TD_FAILURE, "kapi_cipher_hash_start failed!\n");

    src_buf.virt_addr = data;
    ret = kapi_cipher_hash_update(hash_handle,  &src_buf, data_length);
    crypto_chk_goto((ret != TD_SUCCESS), exit_hash_destroy, "kapi_cipher_hash_update failed!\n");

    ret = kapi_cipher_hash_finish(hash_handle, out, out_length);
    crypto_chk_goto((ret != TD_SUCCESS), exit_hash_destroy, "kapi_cipher_hash_finish failed!\n");
    return ret;

exit_hash_destroy:
    (void)kapi_cipher_hash_destroy(hash_handle);
    return ret;
}

static td_s32 inner_hkdf_key_config(const crypto_hkdf_hmac_param_t *hmac_param, td_handle *ks_handle)
{
    td_s32 ret = 0;
    td_handle klad_handle = 0;
    crypto_klad_clear_key clear_key = {0};
    crypto_klad_hmac_type klad_hmac_type = { 0 };
    const crypto_hkdf_table_item_t *hkdf_item = TD_NULL;
    crypto_klad_attr klad_attr = {
        .key_cfg = {
            .engine = CRYPTO_KLAD_ENGINE_SHA2_HMAC,
            .decrypt_support = TD_TRUE,
            .encrypt_support = TD_TRUE
        },
        .key_sec_cfg = {
            .key_sec = TD_FALSE,
            .master_only_enable = TD_FALSE,
            .dest_buf_sec_support = TD_FALSE,
            .dest_buf_non_sec_support = TD_TRUE,
            .src_buf_sec_support = TD_FALSE,
            .src_buf_non_sec_support = TD_TRUE,
        }
    };

    hkdf_item = priv_get_hkdf_table_item(hmac_param->hmac_type);
    crypto_chk_return(hkdf_item == TD_NULL, TD_FAILURE, "get hkdf hmac type failed!\n");
    klad_hmac_type = hkdf_item->klad_hmac_type;

    ret = kapi_keyslot_create(ks_handle, CRYPTO_KEYSLOT_TYPE_HMAC);
    crypto_chk_return(ret != TD_SUCCESS, ret, "kapi_keyslot_create failed\n");

    ret = kapi_klad_create(&klad_handle);
    crypto_chk_goto((ret != TD_SUCCESS), exit_ks_destroy, "kapi_klad_create failed!\n");

    ret = kapi_klad_attach(klad_handle, CRYPTO_KLAD_DEST_HMAC, *ks_handle);
    crypto_chk_goto((ret != TD_SUCCESS), exit_klad_destroy, "kapi_klad_attach failed!\n");

    ret = kapi_klad_set_attr(klad_handle, &klad_attr);
    crypto_chk_goto((ret != TD_SUCCESS), exit_klad_detach, "kapi_klad_set_attr failed!\n");

    clear_key.key = hmac_param->key;
    clear_key.key_length = hmac_param->key_length;
    clear_key.hmac_type = klad_hmac_type;
    ret = kapi_klad_set_clear_key(klad_handle, &clear_key);
    crypto_chk_goto((ret != TD_SUCCESS), exit_klad_detach, "kapi_klad_set_clear_key failed!\n");

    kapi_klad_detach(klad_handle, CRYPTO_KLAD_DEST_HMAC, *ks_handle);
    kapi_klad_destroy(klad_handle);
    return ret;

exit_klad_detach:
    kapi_klad_detach(klad_handle, CRYPTO_KLAD_DEST_HMAC, *ks_handle);
exit_klad_destroy:
    kapi_klad_destroy(klad_handle);
exit_ks_destroy:
    kapi_keyslot_destroy(*ks_handle);
    return ret;
}

static td_s32 inner_hkdf_hmac(const crypto_hkdf_hmac_param_t *hmac_param, td_handle ks_handle,
    td_u8 *out, td_u32 *out_length)
{
    td_s32 ret = 0;
    td_handle hash_handle = 0;
    crypto_hash_attr hash_attr = { 0 };
    crypto_buf_attr src_buf = { 0 };

    hash_attr.hash_type = hmac_param->hmac_type;
    hash_attr.drv_keyslot_handle = ks_handle;
    hash_attr.is_keyslot = TD_TRUE;
    hash_attr.is_long_term = TD_TRUE;
    ret = kapi_cipher_hash_start(&hash_handle, &hash_attr);
    crypto_chk_return((ret != TD_SUCCESS), TD_FAILURE, "kapi_cipher_hash_start failed!\n");

    src_buf.virt_addr = hmac_param->msg;
    ret = kapi_cipher_hash_update(hash_handle,  &src_buf, hmac_param->msg_length);
    crypto_chk_goto((ret != TD_SUCCESS), exit_hash_destroy, "kapi_cipher_hash_update failed!\n");

    ret = kapi_cipher_hash_finish(hash_handle, out, out_length);
    crypto_chk_goto((ret != TD_SUCCESS), exit_hash_destroy, "kapi_cipher_hash_finish failed!\n");
    return ret;

exit_hash_destroy:
    (void)kapi_cipher_hash_destroy(hash_handle);
    return ret;
}

td_s32 kapi_cipher_hkdf_extract(crypto_hkdf_extract_t *extract_param, td_u8 *prk, td_u32 *prk_length)
{
    td_u32 input_block_length = 0;
    td_handle ks_handle = 0;
    crypto_hash_type hash_type = 0;
    crypto_hkdf_hmac_param_t hmac_param = { 0 };
    const crypto_hkdf_table_item_t *hkdf_item = TD_NULL;
    td_s32 ret = 0;

    crypto_chk_return(extract_param == TD_NULL || extract_param->ikm == TD_NULL, TD_FAILURE, "Param is NULL!\n");
    crypto_chk_return(extract_param->salt_length != 0 && extract_param->salt == TD_NULL,
        TD_FAILURE, "Param is NULL!\n");
    crypto_chk_return(prk == TD_NULL || prk_length == TD_NULL, TD_FAILURE, "Param is NULL!\n");

    hkdf_item = priv_get_hkdf_table_item(extract_param->hmac_type);
    crypto_chk_return(hkdf_item == TD_NULL, TD_FAILURE, "get hkdf hmac type failed!\n");
    input_block_length = hkdf_item->block_size;
    hash_type = hkdf_item->hash_type;

    if (extract_param->salt_length > input_block_length) {
        ret = inner_hkdf_hash(hash_type, extract_param->salt, extract_param->salt_length,
            extract_param->salt, &extract_param->salt_length);
        crypto_chk_return(ret != TD_SUCCESS, ret, "inner_hkdf_hash failed\n");
    }

    hmac_param.hmac_type = extract_param->hmac_type;
    hmac_param.key = extract_param->salt;
    hmac_param.key_length = extract_param->salt_length;
    ret = inner_hkdf_key_config(&hmac_param, &ks_handle);
    crypto_chk_return(ret != TD_SUCCESS, ret, "inner_hkdf_key_config failed\n");

    hmac_param.msg = extract_param->ikm;
    hmac_param.msg_length = extract_param->ikm_length;
    ret = inner_hkdf_hmac(&hmac_param, ks_handle, prk, prk_length);

    kapi_keyslot_destroy(ks_handle);
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_cipher_hkdf_extract);

td_s32 kapi_cipher_hkdf_expand(const crypto_hkdf_expand_t *expand_param, td_u8 *okm, td_u32 okm_length)
{
    td_u32 block_num = 0;
    td_handle ks_handle = 0;
    td_u8 output_block[64] = { 0 };  /* 64 is the max hash output length */
    td_u32 output_block_length = 0;
    td_u8 *buffer = TD_NULL;
    td_u32 buffer_length = 0;
    td_u32 processing_len = 0;
    td_u32 last_len = 0;
    crypto_hkdf_hmac_param_t hmac_param = { 0 };
    const crypto_hkdf_table_item_t *hkdf_item = TD_NULL;
    td_u32 i;
    td_s32 ret = 0;

    crypto_chk_return(expand_param == TD_NULL || expand_param->prk == TD_NULL, TD_FAILURE, "Param is NULL!\n");
    crypto_chk_return(expand_param->info_length != 0 && expand_param->info == TD_NULL, TD_FAILURE, "Param is NULL!\n");
    crypto_chk_return(okm == TD_NULL, TD_FAILURE, "Param is NULL!\n");
    crypto_chk_return(expand_param->info_length > HKDF_EXPAND_INFO_MAX_LENGTH, TD_FAILURE, "Param is NULL!\n");

    hkdf_item = priv_get_hkdf_table_item(expand_param->hmac_type);
    crypto_chk_return(hkdf_item == TD_NULL, TD_FAILURE, "get hkdf hmac type failed!\n");
    output_block_length = hkdf_item->result_size;
    /* According to RFC 5869 Section2.3, the length must <= 255 * hash_length. */
    if (okm_length > 255 * output_block_length) {   // 255: refer to comment above
        crypto_log_err("okm_length is too long\n");
        return HASH_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }

    buffer_length = output_block_length + expand_param->info_length + 1;
    buffer = crypto_malloc(buffer_length);
    if (buffer == TD_NULL) {
        crypto_log_err("crypto_malloc failed\n");
        return TD_FAILURE;
    }

    hmac_param.hmac_type = expand_param->hmac_type;
    hmac_param.key = expand_param->prk;
    hmac_param.key_length = expand_param->prk_length;
    ret = inner_hkdf_key_config(&hmac_param, &ks_handle);
    crypto_chk_goto(ret != TD_SUCCESS, free_exit, "inner_hkdf_key_config failed\n");

    block_num = okm_length / output_block_length + ((okm_length % output_block_length) == 0 ? 0 : 1);
    for (i = 0; i < block_num; i++) {
        if (i == 0) {
            output_block_length = 0;
        } else {
            ret = memcpy_s(buffer, buffer_length, output_block, output_block_length);
            crypto_chk_goto(ret != TD_SUCCESS, okm_exit, "memcpy_s failed\n");
        }

        if (expand_param->info != TD_NULL) {
            ret = memcpy_s(buffer + output_block_length, buffer_length - output_block_length,
                expand_param->info, expand_param->info_length);
            crypto_chk_goto(ret != TD_SUCCESS, okm_exit, "memcpy_s failed\n");
        }
        buffer[output_block_length + expand_param->info_length] = i + 1;

        processing_len = output_block_length + expand_param->info_length + 1;
        output_block_length = hkdf_item->result_size;
        hmac_param.hmac_type = expand_param->hmac_type;
        hmac_param.msg = buffer;
        hmac_param.msg_length = processing_len;
        ret = inner_hkdf_hmac(&hmac_param, ks_handle, output_block, &output_block_length);
        crypto_chk_goto(ret != TD_SUCCESS, okm_exit, "inner_hkdf_hmac failed\n");

        if (i != block_num - 1) {
            ret = memcpy_s(okm + i * output_block_length, okm_length - i * output_block_length,
                output_block, output_block_length);
            crypto_chk_goto(ret != TD_SUCCESS, okm_exit, "memcpy_s failed\n");
        }
    }

    last_len = (okm_length % output_block_length) == 0 ? output_block_length : (okm_length % output_block_length);
    ret = memcpy_s(okm + (i - 1) * output_block_length, okm_length - (i - 1) * output_block_length,
        output_block, last_len);
    crypto_chk_goto(ret != TD_SUCCESS, okm_exit, "memcpy_s failed\n");

    goto expand_exit;

okm_exit:
    memset_s(okm, okm_length, 0x00, okm_length);
expand_exit:
    memset_s(output_block, sizeof(output_block), 0x00, sizeof(output_block));
    memset_s(buffer, buffer_length, 0x00, buffer_length);
    kapi_keyslot_destroy(ks_handle);
free_exit:
    crypto_free(buffer);
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_cipher_hkdf_expand);

td_s32 kapi_cipher_hkdf(crypto_hkdf_t *hkdf_param, td_u8 *okm, td_u32 okm_length)
{
    td_u8 prk[64] = { 0 };  /* 64 is the max hash result length */
    td_u32 prk_length = 0;
    crypto_hkdf_extract_t hkdf_extract = { 0 };
    crypto_hkdf_expand_t hkdf_expand = { 0 };
    const crypto_hkdf_table_item_t *hkdf_item = TD_NULL;
    td_s32 ret = 0;

    crypto_chk_return(hkdf_param == TD_NULL || hkdf_param->ikm == TD_NULL, TD_FAILURE, "Param is NULL!\n");
    crypto_chk_return(hkdf_param->salt_length != 0 && hkdf_param->salt == TD_NULL,
        TD_FAILURE, "Param is NULL!\n");
    crypto_chk_return(hkdf_param->info_length != 0 && hkdf_param->info == TD_NULL,
        TD_FAILURE, "Param is NULL!\n");
    crypto_chk_return(okm == TD_NULL, TD_FAILURE, "Param is NULL!\n");

    hkdf_item = priv_get_hkdf_table_item(hkdf_param->hmac_type);
    crypto_chk_return(hkdf_item == TD_NULL, TD_FAILURE, "get hkdf hmac type failed!\n");
    prk_length = hkdf_item->result_size;

    hkdf_extract.hmac_type = hkdf_param->hmac_type;
    hkdf_extract.ikm = hkdf_param->ikm;
    hkdf_extract.ikm_length = hkdf_param->ikm_length;
    hkdf_extract.salt = hkdf_param->salt;
    hkdf_extract.salt_length = hkdf_param->salt_length;
    ret = kapi_cipher_hkdf_extract(&hkdf_extract, prk, &prk_length);
    crypto_chk_return(ret != TD_SUCCESS, ret, "kapi_cipher_hkdf_extract failed\n");

    hkdf_expand.hmac_type = hkdf_param->hmac_type;
    hkdf_expand.info = hkdf_param->info;
    hkdf_expand.info_length = hkdf_param->info_length;
    hkdf_expand.prk = prk;
    hkdf_expand.prk_length = prk_length;
    ret = kapi_cipher_hkdf_expand(&hkdf_expand, okm, okm_length);

    memset_s(prk, sizeof(prk), 0x00, sizeof(prk));
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_cipher_hkdf);