/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: UPG verification functions source file
 */

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <securec.h>
#include "common_def.h"
#include "upg_definitions.h"
#include "upg_otp_reg.h"
#include "errcode.h"
#include "upg_config.h"
#include "upg_debug.h"
#include "chip_io.h"
#include "upg_common.h"
#include "upg_common_porting.h"
#include "upg_porting.h"
#include "upg_alloc.h"
#if (UPG_CFG_VERIFICATION_MODE_RSA == YES)
#include "pke.h"
#include "cipher_api.h"
#elif ((UPG_CFG_VERIFICATION_MODE_ECC == YES) || (UPG_CFG_VERIFICATION_MODE_SM2_SM3 == YES))
#include "unified_cipher_pke.h"
#include "cipher.h"
#endif
#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_SAMPLE_VERIFY
#include "drv_flashboot_cipher.h"
#include "drv_rom_cipher.h"
#endif
#if (UPG_CFG_VERIFICATION_MODE_SW_HASH == YES)
#include "sha256/sha256.h"
#endif
#if (UPG_CFG_MEMORY_DCACHE_ENABLED == YES)
#include "soc_osal.h"
#endif
#include "upg_verify.h"
#if (UPG_CFG_VERIFICATION_SUPPORT == YES)

#define ECC_KEY_OFFSET  32
#define UPG_SM2_ID      "\x31\x32\x33\x34\x35\x36\x37\x38\x31\x32\x33\x34\x35\x36\x37\x38"
#define UPG_SM2_ID_LEN  0x10

#define RSA_N_KEY_LEN 512
#define RSA_E_KEY_LEN 512
#define RSA_SIG_LEN   512

#define ECC_X_KEY_LEN 32
#define ECC_Y_KEY_LEN 32
#define ECC_KEY_LEN   32

#define ECC_R_SIG_LEN 32
#define ECC_S_SIG_LEN 32
#define ECC_SIG_LEN   32

#if (defined(CONFIG_APPS_CORE) || defined(CONFIG_MAIN_CORE))
#define UPG_VERIFY_CIPHER_BUF_ATTR UAPI_DRV_CIPHER_BUF_NONSECURE
#else
#define UPG_VERIFY_CIPHER_BUF_ATTR UAPI_DRV_CIPHER_BUF_SECURE
#endif /* CONFIG_APPS_CORE */

STATIC uapi_upg_user_defined_check g_user_defined_check_func = NULL;
STATIC uintptr_t g_user_defined_param = 0;

errcode_t verify_hash_cmp(const uint8_t *hash, const uint8_t *hash_res, uint32_t hash_len)
{
    if (memcmp(hash, hash_res, hash_len) != 0) {
        upg_log_err("[UPG] upg_verify_hash: memcmp hash fail.\r\n");
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

#if (UPG_CFG_VERIFICATION_MODE_SW_HASH == YES)
STATIC errcode_t upg_verify_hash(uintptr_t verify_data, uint32_t verify_size, const uint8_t *hash, uint32_t hash_len)
{
    uint8_t hash_res[SHA_256_LENGTH] = {0};
    sha256_context_t md;
    (void)sha256_init(&md);
    (void)SHA256Update(&md, (unsigned char *)((uintptr_t)verify_data), verify_size);
    (void)sha256_final(&md, (unsigned char *)hash_res, SHA256_HASH_SIZE);
 
    errcode_t ret_val = verify_hash_cmp(hash, hash_res, hash_len);
    if (ret_val != ERRCODE_SUCC) {
        upg_log_err("upg_verify_hash: memcmp hash fail.");
        return ERRCODE_FAIL;
    }
 
    upg_log_err("upg_verify_hash image table OK");
    return ERRCODE_SUCC;
}
#else
STATIC errcode_t hash_operation(uapi_drv_cipher_buf_attr_t *src_buf, uint32_t src_len, uint8_t *data_sha,
    uapi_drv_cipher_hash_attr_t *hash_attr)
{
    errcode_t ret;
    uint32_t out_length = SHA_256_LENGTH;

#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_SAMPLE_VERIFY
    if (hash_attr->hash_type == UAPI_DRV_CIPHER_HASH_TYPE_SHA256) {
        ret = drv_rom_cipher_sha256((uint8_t *)src_buf->virt_addr, src_len, data_sha, out_length);
    } else if (hash_attr->hash_type == UAPI_DRV_CIPHER_HASH_TYPE_HMAC_SM3) {
        ret = drv_rom_cipher_sm3((uint8_t *)src_buf->virt_addr, src_len, data_sha, out_length);
    } else {
        upg_log_err("[UPG] hash_operation fail, no support hash_type = 0x%x\r\n", hash_attr->hash_type);
        return ERRCODE_FAIL;
    }
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] hash_operation: drv_rom_cipher_hash fail ret_val = 0x%x\r\n", ret);
    }
    return ret;
#else
    uint32_t handle;
    ret = uapi_drv_cipher_hash_start(&handle, hash_attr);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] uapi_drv_cipher_hash_start fail ret = 0x%x\r\n", ret);
        return ret;
    }

#if (UPG_CFG_MEMORY_DCACHE_ENABLED == YES)
    osal_dcache_region_wb(NULL, (unsigned long)(uintptr_t)src_buf->address, src_len);
#endif

    ret = uapi_drv_cipher_hash_update(handle, src_buf, src_len);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] uapi_drv_cipher_hash_update fail ret = 0x%x\r\n", ret);
        uapi_drv_cipher_hash_finish(handle, data_sha, &out_length);
        return ret;
    }

    return uapi_drv_cipher_hash_finish(handle, data_sha, &out_length);
#endif
}

STATIC errcode_t upg_verify_hash(uintptr_t verify_data, uint32_t verify_size, const uint8_t *hash, uint32_t hash_len)
{
    uint8_t hash_res[SHA_256_LENGTH] = {0};

    uapi_drv_cipher_hash_attr_t hash_attr;
    uapi_drv_cipher_buf_attr_t src_buf;

    hash_attr.hash_type = UAPI_DRV_CIPHER_HASH_TYPE_SHA256;
    hash_attr.keyslot_handle = 0;
#if (UPG_CFG_VERIFICATION_MODE_ECC == YES)
    src_buf.virt_addr = (void *)verify_data;
#else
    src_buf.phys_addr = verify_data;
#endif
    src_buf.buf_sec = UAPI_DRV_CIPHER_BUF_SECURE;

    errcode_t ret_val = hash_operation(&src_buf, verify_size, hash_res, &hash_attr);
    if (ret_val != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg_verify_hash: hash_operation fail ret_val = 0x%x\r\n", ret_val);
        return ERRCODE_FAIL;
    }

    ret_val = verify_hash_cmp(hash, hash_res, hash_len);
    if (ret_val != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg_verify_hash: memcmp hash fail.\r\n");
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

STATIC errcode_t calc_hash_by_type(uint32_t src_addr, uint32_t src_len, uint8_t *data_sha,
    uint32_t data_sha_len, uapi_drv_cipher_hash_type_t hash_type)
{
    uapi_drv_cipher_hash_attr_t hash_attr;
    uapi_drv_cipher_buf_attr_t src_buf;

    if (data_sha_len != SHA_256_LENGTH) {
        return ERRCODE_FAIL;
    }

    hash_attr.hash_type = hash_type;
    hash_attr.keyslot_handle = 0;

#if (UPG_CFG_VERIFICATION_MODE_ECC == YES)
    src_buf.virt_addr = (void *)src_addr;
#else
    src_buf.phys_addr = (uintptr_t)src_addr;
#endif
    src_buf.buf_sec = UAPI_DRV_CIPHER_BUF_SECURE;

    return hash_operation(&src_buf, src_len, data_sha, &hash_attr);
}

errcode_t calc_hash(uint32_t src_addr, uint32_t src_len, uint8_t *data_sha,
    uint32_t data_sha_len)
{
    return calc_hash_by_type(src_addr, src_len, data_sha, data_sha_len, UAPI_DRV_CIPHER_HASH_TYPE_SHA256);
}

#if (UPG_CFG_VERIFICATION_MODE_SM2_SM3 == YES) // SM2&SM3

STATIC errcode_t verify_signature(const uapi_drv_cipher_pke_data_t *data, uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_ecc_sig_t *sign)
{
    uint8_t data_hash[SHA_256_LENGTH];
    uapi_drv_cipher_pke_data_t hash;
    uapi_drv_cipher_pke_data_t sm2_id;
    uapi_drv_cipher_pke_msg_t pke_msg;
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type = UAPI_DRV_CIPHER_PKE_ECC_TYPE_SM2;
    int32_t ret =  ERRCODE_FAIL;
    /* Initialise hash arrays and structs */
    ret =  memset_s(data_hash, SHA_256_LENGTH, 0x5a, SHA_256_LENGTH);

    hash.data = data_hash;
    hash.length = SHA_256_LENGTH;

#if (UPG_CFG_VERIFICATION_MODE_SM3_ONLY == YES)
    uapi_drv_cipher_hash_type_t hash_type = UAPI_DRV_CIPHER_HASH_TYPE_SM3;
    ret = calc_hash_by_type((uintptr_t)data->data, data->length, data_hash, SHA_256_LENGTH, hash_type);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] sm3 calc_hash fail ret = 0x%x\r\n", ret);
        return ret;
    }
    return verify_hash_cmp((uint8_t *)sign, data_hash, SHA_256_LENGTH);
#endif
    sm2_id.data = (uint8_t *)UPG_SM2_ID;
    sm2_id.length = UPG_SM2_ID_LEN;
    pke_msg.data = data->data;
    pke_msg.length = data->length;
    pke_msg.buf_sec = UAPI_DRV_CIPHER_PKE_BUF_SECURE;
    ret = uapi_drv_cipher_pke_sm2_dsa_hash(&sm2_id, pub_key, &pke_msg, &hash);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] calc_hash fail ret = 0x%x\r\n", ret);
        return ret;
    }

    return uapi_drv_cipher_pke_ecdsa_verify(curve_type, pub_key, &hash, sign);
}

#elif (UPG_CFG_VERIFICATION_MODE_ECC == YES) // ECC
STATIC errcode_t verify_signature(const uapi_drv_cipher_pke_data_t *data, uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_ecc_sig_t *sign)
{
    uint8_t data_hash[SHA_256_LENGTH];
    uapi_drv_cipher_pke_data_t hash;
    uapi_drv_cipher_hash_type_t hash_type = UAPI_DRV_CIPHER_HASH_TYPE_SHA256;
    int32_t ret =  ERRCODE_FAIL;
    /* Initialise hash arrays and structs */
    memset_s(data_hash, SHA_256_LENGTH, 0x5a, SHA_256_LENGTH);

    hash.data = data_hash;
    hash.length = SHA_256_LENGTH;

    ret = calc_hash_by_type((uintptr_t)data->data, data->length, data_hash, SHA_256_LENGTH, hash_type);
#if (UPG_CFG_VERIFICATION_MODE_SHA256_ONLY == YES)
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] sha 256 calc_hash fail ret = 0x%x\r\n", ret);
        return ret;
    }
    return verify_hash_cmp((uint8_t *)sign, data_hash, SHA_256_LENGTH);
#endif

    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] calc_hash fail ret = 0x%x\r\n", ret);
        return ret;
    }

#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_SAMPLE_VERIFY
    return drv_rom_cipher_pke_bp256r_verify((drv_rom_cipher_ecc_point *)pub_key,
        (drv_rom_cipher_data *)&hash, (drv_rom_cipher_ecc_sig *)sign);
#else
    return uapi_drv_cipher_pke_ecdsa_verify(UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P256, pub_key, &hash, sign);
#endif
}
#elif (UPG_CFG_VERIFICATION_MODE_RSA == YES) // RSA
STATIC errcode_t verify_signature(const uapi_drv_cipher_pke_data_t *data, uapi_drv_cipher_pke_rsa_pub_key_t *pub_key,
    const uapi_drv_cipher_pke_data_t *sign)
{
    uint8_t data_hash[SHA_256_LENGTH];
    uapi_drv_cipher_pke_data_t hash;
    uapi_drv_cipher_pke_rsa_pub_key_t pub_key_tmp = {0};

    uapi_drv_cipher_pke_rsa_scheme_t scheme = UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V21;
    errcode_t ret = ERRCODE_SUCC;

    /* Initialise hash arrays and structs */
    (void)memset_s(data_hash, sizeof(data_hash), 0x5a, SHA_256_LENGTH);

    hash.data = data_hash;
    hash.length = SHA_256_LENGTH;

    ret = calc_hash((uintptr_t)data->data, data->length, data_hash, SHA_256_LENGTH);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] calc_hash fail ret = 0x%x\r\n", ret);
        return ret;
    }

    uint8_t *e_in = upg_malloc(RSA_E_KEY_LEN);
    if (e_in == NULL) {
        return ERRCODE_MALLOC;
    }
    uint8_t e_value[] = {0x0, 0x1, 0x0, 0x1};
    uint32_t offset = RSA_E_KEY_LEN - (uint32_t)sizeof(e_value);
    memset_s(e_in, RSA_N_KEY_LEN, 0, RSA_N_KEY_LEN);
    memcpy_s(e_in + offset, RSA_E_KEY_LEN - offset, e_value, sizeof(e_value));
    pub_key_tmp.len = pub_key->len;
    pub_key_tmp.n = pub_key->n;
    pub_key_tmp.e = e_in;

    ret = uapi_drv_cipher_pke_rsa_verify(&pub_key_tmp, scheme, UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA256, &hash, sign);
    if (ret != ERRCODE_SUCC) {
        upg_free(e_in);
        upg_log_err("[UPG] uapi_drv_cipher_pke_rsa_verify fail ret = 0x%x\r\n", ret);
        return ret;
    }

    upg_log_info("[UPG] verify_signature success\r\n");
    upg_free(e_in);
    return ret;
}

errcode_t secure_authenticate(const uint8_t *key, const upg_auth_data_t *data, uint8_t *sign_buff)
{
    volatile errcode_t ret = ERRCODE_FAIL;
    uapi_drv_cipher_pke_rsa_pub_key_t pub_key;
    uapi_drv_cipher_pke_data_t sign;

    if ((key == NULL) || (data == NULL) || (sign_buff == NULL)) {
        return ERRCODE_FAIL;
    }

    pub_key.n = (uint8_t *)key;
    pub_key.e = (uint8_t *)key + RSA_N_KEY_LEN;
    pub_key.len = RSA_N_KEY_LEN;
    sign.data = sign_buff;
    sign.length = RSA_SIG_LEN;

    ret = verify_signature((const uapi_drv_cipher_pke_data_t *)data, &pub_key, &sign);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] verify_signature is fail, ret = 0x%x\r\n", ret);
    }

    return ret;
}

STATIC errcode_t verify_fota_key_area(uint32_t type, upg_key_area_data_t *key_area, uint8_t *public_key)
{
    unused(type);
    if (key_area->signature_length == SHA_256_LENGTH) {
        upg_log_info("[UPG] verify_fota_key_area -> verify SHA256\r\n");
        uint32_t key_area_len = (uint32_t)sizeof(upg_key_area_data_t) - RSA_SIG_LEN;
        return upg_verify_hash((uintptr_t)key_area, key_area_len, key_area->sig_fota_key_area, SHA_256_LENGTH);
    }
    upg_log_info("[UPG] verify_fota_key_area -> verify signed\r\n");
    uapi_drv_cipher_pke_data_t data;

    /* Verify app key area with flash root public key */
    data.data = (uint8_t *)key_area;
    data.length = (uint32_t)sizeof(upg_key_area_data_t) - RSA_SIG_LEN;

    return secure_authenticate(public_key, (upg_auth_data_t *)&data, key_area->sig_fota_key_area);
}
#endif
#endif /* #if UPG_CFG_VERIFICATION_MODE_SW_HASH */

#if (UPG_CFG_VERIFICATION_MODE_ECC == YES || UPG_CFG_VERIFICATION_MODE_SM2_SM3 == YES)
errcode_t secure_authenticate(const uint8_t *key, const upg_auth_data_t *data, uint8_t *sign_buff)
{
    volatile errcode_t ret = ERRCODE_FAIL;
    uapi_drv_cipher_pke_ecc_point_t pub_key;
    uapi_drv_cipher_pke_ecc_sig_t sign;

    if ((key == NULL) || (data == NULL) || (sign_buff == NULL)) {
        return ERRCODE_FAIL;
    }
    pub_key.x = (uint8_t *)key;
    pub_key.y = (uint8_t *)key + ECC_X_KEY_LEN;
    pub_key.length = ECC_KEY_LEN;
    sign.r = sign_buff;
    sign.s = sign_buff + ECC_R_SIG_LEN;
    sign.length = ECC_SIG_LEN;

    ret = verify_signature((uapi_drv_cipher_pke_data_t *)data, &pub_key, &sign);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] verify_signature is fail, ret = 0x%x\r\n", ret);
    }
    return ret;
}

STATIC errcode_t verify_fota_key_area(uint32_t type, upg_key_area_data_t *key_area, uint8_t *public_key)
{
    unused(type);
    upg_auth_data_t data;
    if (key_area->signature_length == SHA_256_LENGTH) {
        upg_log_info("[UPG] verify_fota_key_area -> verify SHA256\r\n");
        uint32_t key_area_len = (uint32_t)sizeof(upg_key_area_data_t) - SIG_LEN;
        return upg_verify_hash((uintptr_t)key_area, key_area_len, key_area->sig_fota_key_area, SHA_256_LENGTH);
    }
    /* Verify app key area with flash root public key */
    data.data = (uint8_t *)key_area;
    data.length = sizeof(upg_key_area_data_t) - SIG_LEN;

    return secure_authenticate(public_key, &data, key_area->sig_fota_key_area);
}
#endif

STATIC errcode_t verify_fota_info(uint32_t type, upg_fota_info_data_t *fota_info, uint8_t *public_key)
{
    unused(type);
    if (fota_info->signature_length == SHA_256_LENGTH) {
        upg_log_info("[UPG] verify_fota_info -> verify SHA256\r\n");
        uint32_t fota_info_len = (uint32_t)sizeof(upg_fota_info_data_t) - SIG_LEN;
        return upg_verify_hash((uintptr_t)fota_info, fota_info_len, fota_info->sign_fota_info, SHA_256_LENGTH);
    }
#if (UPG_CFG_INTEGRITY_VERIFICATION_ONLY == YES)
    unused(public_key);
    return ERRCODE_SUCC;
#else
    upg_log_info("[UPG] verify_fota_info -> verify signed\r\n");
    upg_auth_data_t data;

    data.data = (uint8_t *)fota_info;
    data.length = (uint32_t)sizeof(upg_fota_info_data_t) - SIG_LEN;

    return secure_authenticate(public_key, &data, fota_info->sign_fota_info);
#endif
}

STATIC errcode_t upg_check_fota_image_id(const upg_key_area_data_t *upg_key_info, const upg_fota_info_data_t *fota_info)
{
    if (upg_key_info->image_id != UPG_IMAGE_ID_KEY_AREA) {
        return ERRCODE_FAIL;
    }
    if (fota_info->image_id != UPG_IMAGE_ID_FOTA_INFO_AREA) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

void uapi_upg_register_user_defined_verify_func(uapi_upg_user_defined_check func, uintptr_t param)
{
    g_user_defined_check_func = func;
    g_user_defined_param = param;
}

#if (UPG_CFG_INTEGRITY_VERIFICATION_ONLY == NO)
/*
    步骤一，使用Root_Public_Key校验Key Area的签名
    步骤二，使用存储在升级包Key Area的FOTA_External_Public_Key校验FOTA Info区签名
    步骤三，如用户注册了定义字段的校验函数，则校验自定义字段（user_defined），如未注册，此步骤默认校验通过
*/
errcode_t uapi_upg_verify_file_head(const upg_package_header_t *pkg_header)
{
    upg_key_area_data_t *key_area = (upg_key_area_data_t *)&(pkg_header->key_area);
    uint32_t type = key_area->image_id;
    uint8_t *public_key = NULL;

    if (upg_is_inited() == false) {
        return ERRCODE_UPG_NOT_INIT;
    }

    public_key = upg_get_root_public_key();
    if (public_key == NULL) {
        return ERRCODE_UPG_VERIFICATION_KEY_ERROR;
    }

    upg_fota_info_data_t *fota_info = (upg_fota_info_data_t *)&(pkg_header->info_area);

    errcode_t ret = upg_check_fota_image_id(key_area, fota_info);
    if (ret != ERRCODE_SUCC) {
        /* image id 不正确就不用校验了，直接返回失败 */
        upg_log_err("[UPG] upg verify: image ID error\r\n");
        return ret;
    }

    /* 步骤一 */
    ret = verify_fota_key_area(type, key_area, public_key);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg verify: key area error. ret = 0x%x\r\n", ret);
        return ret;
    }

    /* 步骤二 */
    ret = verify_fota_info(fota_info->image_id, fota_info, key_area->fota_external_public_key);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg verify: fota info error. ret = 0x%x\r\n", ret);
        return ret;
    }

    ret = upg_check_fota_information(pkg_header);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg verify: upg_check_fota_information. ret = 0x%x\r\n", ret);
        return ret;
    }

    /* 步骤三 */
    if (g_user_defined_check_func != NULL) {
        ret = g_user_defined_check_func(fota_info->user_defined, sizeof(fota_info->user_defined), g_user_defined_param);
    }

    return ret;
}
#endif

STATIC errcode_t uapi_upg_verify_image(const upg_package_header_t *pkg_header, upg_image_hash_node_t *img_hash_table)
{
    errcode_t ret = ERRCODE_SUCC;
    upg_image_header_t *img_header = NULL;
    upg_fota_info_data_t *fota_info = (upg_fota_info_data_t *)&(pkg_header->info_area);
    for (uint32_t i = 0; i < fota_info->image_num; i++) {
        ret = upg_get_pkg_image_header((const upg_image_hash_node_t *)&(img_hash_table[i]), &img_header);
        if (ret != ERRCODE_SUCC || img_header == NULL) {
            upg_log_err("[UPG] upg_get_pkg_image_header fail\r\n");
            break;
        }

        ret = uapi_upg_verify_file_image((const upg_image_header_t *)img_header,
                                         (const uint8_t *)img_hash_table[i].image_hash, SHA_256_LENGTH, true);
        if (ret != ERRCODE_SUCC) {
            break;
        }
        /* 步骤八:校验防回滚版本号，只有镜像中的防回滚版本号大于等于OTP中的防回滚版本号时才能升级 */
#if (UPG_CFG_ANTI_ROLLBACK_SUPPORT == YES)
        ret = upg_anti_rollback_version_verify(pkg_header, img_header);
        if (ret != ERRCODE_SUCC) {
            upg_set_temporary_result(UPG_RESULT_VERIFY_VERSION_FAILED);
            break;
        }
#endif
        upg_free(img_header);
        img_header = NULL;
    }
    upg_free(img_header);
    return ret;
}

/*
    步骤一，使用Root_Public_Key校验Key Area的签名
    步骤二，使用存储在升级包Key Area的FOTA_External_Public_Key校验FOTA Info区签名
    步骤三，如用户注册了定义字段的校验函数，则校验自定义字段（user_defined），如未注册，此步骤默认校验通过
    步骤四，使用存储在FOTA Info区的Image_Hash_Table中的HASH值，校验Image_Hash_Table
    步骤五，使用保存在Image_Hash_Table中的每个镜像Header的HASH值校验对应的Image_Header
    步骤六：使用保存在Image_Header中的新镜像HASH值校验对应的新镜像。
    步骤七，如果为差分升级，镜像的Image_Header中保存了旧版本镜像的HASH值，根据这个HASH值校验flash上的旧镜像是否是制作差分包
           时用的旧镜像。非差分镜像，可忽略此步骤
    步骤八，校验防回滚版本号，只有镜像中的防回滚版本号大于等于OTP中的防回滚版本号时才能升级
*/
errcode_t uapi_upg_verify_file(const upg_package_header_t *pkg_header)
{
    upg_image_hash_node_t *img_hash_table = NULL;

    if (upg_is_inited() == false) {
        return ERRCODE_UPG_NOT_INIT;
    }

    /* 步骤一，步骤二，步骤三 */
    errcode_t ret = uapi_upg_verify_file_head(pkg_header);
    if (ret != ERRCODE_SUCC) {
        upg_set_temporary_result(UPG_RESULT_VERIFY_HEAD_FAILED);
        return ret;
    }
    upg_log_info("[UPG] upg verify head OK\r\n");

    ret = upg_get_pkg_image_hash_table(pkg_header, &img_hash_table);
    if (ret != ERRCODE_SUCC || img_hash_table == NULL) {
        upg_log_err("[UPG] upg_get_pkg_image_hash_table fail\r\n");
        return ret;
    }

    upg_fota_info_data_t *fota_info = (upg_fota_info_data_t *)&(pkg_header->info_area);
    /* 步骤四 使用存储在FOTA Info区的Image_Hash_Table中的HASH值，校验Image_Hash_Table */
    /* 由于ImageHashTable为了16字节对齐有填充字段，此处的长度不能使用image_num * sizeof(upg_image_hash_node_t)) */
    ret = upg_verify_hash((uintptr_t)img_hash_table, fota_info->image_hash_table_length,
                          fota_info->image_hash_table_hash, sizeof(fota_info->image_hash_table_hash));
    if (ret != ERRCODE_SUCC) {
        upg_set_temporary_result(UPG_RESULT_VERIFY_HASH_TABLE_FAILED);
        upg_free(img_hash_table);
        return ret;
    }
    upg_log_info("[UPG] upg_verify_hash image table OK\r\n");
    /* 循环遍历校验每个升级image
       步骤五至八
    */
    ret = uapi_upg_verify_image(pkg_header, img_hash_table);
    upg_free(img_hash_table);
    return ret;
}

#if (UPG_CFG_DIRECT_FLASH_ACCESS == NO)
#if (UPG_CFG_VERIFICATION_MODE_SW_HASH == NO)
STATIC errcode_t upg_verify_image_multi_segment_data(const upg_image_header_t *img_header)
{
    errcode_t ret_val;
    uint32_t handle;
    uint32_t out_length = SHA_256_LENGTH;
    uapi_drv_cipher_hash_attr_t hash_attr = {NULL, 0, 0, UAPI_DRV_CIPHER_HASH_TYPE_SHA256, 0, 0};
    uapi_drv_cipher_buf_attr_t src_buf;
    uint32_t read_len;
    uint32_t image_offset = 0;
    uint8_t data_sha[SHA_256_LENGTH] = {0};
    uint32_t aligned_image_len = upg_aligned(img_header->image_len, 16); /* 16-byte alignment */

    ret_val = uapi_drv_cipher_hash_start(&handle, &hash_attr);
    if (ret_val != ERRCODE_SUCC) {
        upg_log_err("[UPG] uapi_drv_cipher_hash_start fail\r\n");
        return ret_val;
    }

    uint8_t *image_data = (uint8_t *)upg_malloc(VERIFY_BUFF_LEN);
    if (image_data == NULL) {
        return ERRCODE_MALLOC;
    }

    do {
        read_len = (image_offset + VERIFY_BUFF_LEN < aligned_image_len) ?
            VERIFY_BUFF_LEN : (aligned_image_len - image_offset);

        ret_val = upg_copy_pkg_image_data(img_header, image_offset, &read_len, image_data);
        if (ret_val != ERRCODE_SUCC) {
            upg_free(image_data);
            return ret_val;
        }

        src_buf.phys_addr = (uintptr_t)image_data;
        src_buf.buf_sec = UAPI_DRV_CIPHER_BUF_SECURE;

        ret_val = uapi_drv_cipher_hash_update(handle, &src_buf, read_len);
        if (ret_val != ERRCODE_SUCC) {
            upg_free(image_data);
            return ret_val;
        }
        image_offset += read_len;
    } while (image_offset < aligned_image_len);

    upg_free(image_data);

    ret_val = uapi_drv_cipher_hash_finish(handle, data_sha, &out_length);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    return verify_hash_cmp(img_header->image_hash, data_sha, SHA_256_LENGTH);
}
#else
STATIC errcode_t upg_verify_image_multi_segment_data(const upg_image_header_t *img_header)
{
    errcode_t ret_val;
    uint32_t out_length = SHA_256_LENGTH;
    uint32_t read_len;
    uint32_t image_offset = 0;
    uint8_t data_sha[SHA_256_LENGTH] = {0};
    sha256_context_t md;
    (void)sha256_init(&md);
    uint32_t aligned_image_len = upg_aligned(img_header->image_len, 16); /* 16-byte alignment */
    uint8_t *image_data = (uint8_t *)upg_malloc(VERIFY_BUFF_LEN);
 
    if (image_data == NULL) {
        return ERRCODE_MALLOC;
    }
 
    do {
        read_len = (image_offset + VERIFY_BUFF_LEN < aligned_image_len) ?
            VERIFY_BUFF_LEN : (aligned_image_len - image_offset);
 
        ret_val = upg_copy_pkg_image_data(img_header, image_offset, &read_len, image_data);
        if (ret_val != ERRCODE_SUCC) {
            upg_free(image_data);
            return ret_val;
        }
        (void)SHA256Update(&md, (unsigned char *)((uintptr_t)image_data), read_len);
        image_offset += read_len;
    } while (image_offset < aligned_image_len);
    upg_free(image_data);
    (void)sha256_final(&md, (unsigned char *)data_sha, out_length);
    return verify_hash_cmp(img_header->image_hash, data_sha, SHA_256_LENGTH);
}
#endif /* #if UPG_CFG_VERIFICATION_MODE_SW_HASH */
#endif /* #if UPG_CFG_DIRECT_FLASH_ACCESS */

STATIC errcode_t upg_verify_image_data(const upg_image_header_t *img_header)
{
#if (UPG_CFG_DIRECT_FLASH_ACCESS == NO)
    return upg_verify_image_multi_segment_data(img_header);
#else
    uint8_t *image_data = NULL;
    errcode_t ret;
    uint32_t aligned_image_len = upg_aligned(img_header->image_len, 16); /* 16-byte alignment */
    ret = upg_get_pkg_image_data(img_header, 0, &aligned_image_len, &image_data);
    if (ret != ERRCODE_SUCC || image_data == NULL) {
        return ret;
    }

    ret = upg_verify_hash((uintptr_t)image_data, aligned_image_len,
                          img_header->image_hash, sizeof(img_header->image_hash));
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
#endif /* #if UPG_CFG_DIRECT_FLASH_ACCESS */
}

#if (UPG_CFG_DIFF_UPGRADE_SUPPORT == YES)
STATIC errcode_t upg_verify_old_image(const upg_image_header_t *img_header)
{
    errcode_t ret;
    uint32_t handle;
    uint32_t out_length = SHA_256_LENGTH;
    uapi_drv_cipher_hash_attr_t hash_attr = { .hash_type = UAPI_DRV_CIPHER_HASH_TYPE_SHA256 };
    uapi_drv_cipher_buf_attr_t src_buf;
    uint32_t read_len;
    uint32_t image_offset = 0;
    uint8_t data_sha[SHA_256_LENGTH] = {0};
    uint32_t image_len = img_header->old_image_len;

    ret = uapi_drv_cipher_hash_start(&handle, &hash_attr);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] uapi_drv_cipher_hash_start fail\r\n");
        return ret;
    }

    uint8_t *old_image_data = (uint8_t *)upg_malloc(VERIFY_BUFF_LEN);
    if (old_image_data == NULL) {
        return ERRCODE_MALLOC;
    }

    do {
        read_len = (image_offset + VERIFY_BUFF_LEN < image_len) ?
            VERIFY_BUFF_LEN : (image_len - image_offset);

        ret = upg_read_old_image_data(image_offset, old_image_data, &read_len, img_header->image_id);
        if (ret != ERRCODE_SUCC) {
            upg_free(old_image_data);
            return ret;
        }

        src_buf.phys_addr = (uintptr_t)old_image_data;
        src_buf.buf_sec = UPG_VERIFY_CIPHER_BUF_ATTR;

        ret = uapi_drv_cipher_hash_update(handle, &src_buf, read_len);
        if (ret != ERRCODE_SUCC) {
            upg_free(old_image_data);
            return ret;
        }
        image_offset += read_len;
    } while (image_offset < image_len);

    upg_free(old_image_data);

    ret = uapi_drv_cipher_hash_finish(handle, data_sha, &out_length);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return verify_hash_cmp(img_header->old_image_hash, data_sha, SHA_256_LENGTH);
}
#endif
/*
    步骤五，使用保存在Image_Hash_Table中的每个镜像Header的HASH值校验对应的Image_Header
    步骤六：使用保存在Image_Header中的新镜像HASH值校验对应的新镜像。
    步骤七，如果为差分升级，镜像的Image_Header中保存了旧版本镜像的HASH值，根据这个HASH值校验flash上的旧镜像是否是制作差分包
           时用的旧镜像。非差分镜像，可忽略此步骤
    步骤八，校验防回滚版本号，只有镜像中的防回滚版本号大于等于OTP中的防回滚版本号时才能升级
*/
errcode_t uapi_upg_verify_file_image(const upg_image_header_t *img_header,
                                     const uint8_t *hash, uint32_t hash_len, bool verify_old)
{
    uint32_t ret;
    UNUSED(verify_old);
    /* 校验HeaderMagic有效性 */
    if (img_header->header_magic != UPG_IMAGE_HRADER_MAGIC) {
        upg_log_err("[UPG] upg verify: file image check error. header_magic = 0x%x\r\n", img_header->header_magic);
        upg_set_temporary_result(UPG_RESULT_VERIFY_IMAGE_FAILED);
        return ERRCODE_FAIL;
    }
    /* 步骤五: 使用保存在Image_Hash_Table中的每个镜像Header的HASH值校验对应的Image_Header */
    ret = upg_verify_hash((uintptr_t)img_header, sizeof(upg_image_header_t), hash, hash_len);
    if (ret != ERRCODE_SUCC) {
        upg_set_temporary_result(UPG_RESULT_VERIFY_IMAGE_FAILED);
        return ret;
    }

    /* 步骤六: 使用保存在Image_Header中的新镜像HASH值校验对应的新镜像。 */
    ret = upg_verify_image_data(img_header);
    if (ret != ERRCODE_SUCC) {
        upg_set_temporary_result(UPG_RESULT_VERIFY_IMAGE_FAILED);
        return ret;
    }

    /* 步骤七:如果为差分升级，镜像的Image_Header中保存了旧版本镜像的HASH值，根据这个HASH值校验flash上的旧镜像是否是制作差分包
       时用的旧镜像。非差分镜像，可忽略此步骤
     */
#if (UPG_CFG_DIFF_UPGRADE_SUPPORT == YES)
    if (verify_old && img_header->decompress_flag == DECOMPRESS_FLAG_DIFF) {
        ret = upg_verify_old_image(img_header);
        if (ret != ERRCODE_SUCC) {
            upg_set_temporary_result(UPG_RESULT_VERIFY_OLD_IMAGE_FAILED);
            return ret;
        }
    }
#endif
    upg_log_info("[UPG] upg verify: image check OK. image_id = 0x%x\r\n", img_header->image_id);
    /* 步骤八:校验防回滚版本号，只有镜像中的防回滚版本号大于等于OTP中的防回滚版本号时才能升级 */
    return ERRCODE_SUCC;
}

errcode_t uapi_upg_check_head_integrity(const upg_package_header_t *pkg_header)
{
    upg_key_area_data_t *key_area = (upg_key_area_data_t *)&(pkg_header->key_area);
    upg_fota_info_data_t *fota_info = (upg_fota_info_data_t *)&(pkg_header->info_area);
    upg_image_hash_node_t *img_hash_table = NULL;

    if (upg_is_inited() == false) {
        return ERRCODE_UPG_NOT_INIT;
    }

    errcode_t ret = upg_check_fota_image_id(key_area, fota_info);
    if (ret != ERRCODE_SUCC) {
        /* image id 不正确就不用校验了，直接返回失败 */
        upg_log_err("[UPG] upg verify: image ID error\r\n");
        return ret;
    }

    /* 校验FOTA info */
    ret = verify_fota_info(fota_info->image_id, fota_info, key_area->fota_external_public_key);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg verify: fota info error. ret = 0x%x\r\n", ret);
        return ret;
    }

    ret = upg_get_pkg_image_hash_table(pkg_header, &img_hash_table);
    if (ret != ERRCODE_SUCC || img_hash_table == NULL) {
        upg_log_err("[UPG] upg_get_pkg_image_hash_table fail\r\n");
        return ret;
    }

    /* 步骤四 使用存储在FOTA Info区的Image_Hash_Table中的HASH值，校验Image_Hash_Table */
    /* 由于ImageHashTable为了16字节对齐有填充字段，此处的长度不能使用image_num * sizeof(upg_image_hash_node_t)) */
    ret = upg_verify_hash((uintptr_t)img_hash_table,
                          fota_info->image_hash_table_length,
                          fota_info->image_hash_table_hash,
                          sizeof(fota_info->image_hash_table_hash));
    if (ret != ERRCODE_SUCC) {
        upg_free(img_hash_table);
        upg_log_err("[UPG] upg verify: verify hash table failed. ret = 0x%x\r\n", ret);
        return ret;
    }

    upg_free(img_hash_table);
    return ERRCODE_SUCC;
}
#endif
