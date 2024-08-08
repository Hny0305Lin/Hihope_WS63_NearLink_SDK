/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: define the functions, which will be registered to mbedtls.
 *
 * Create: 2023-07-11
*/

#include "mbedtls_harden_adapt.h"
#include "kapi_hash.h"
#include "kapi_km.h"
#include "kapi_pke.h"
#include "kapi_pke_cal.h"
#include "kapi_symc.h"
#include "kapi_trng.h"

#include "hash_harden_impl.h"
#include "aes_harden_impl.h"
#include "ecp_harden_impl.h"

#define MALLOC_BOUNDARY 32

typedef enum {
    KM_KEYSLOT_ENGINE_AES = 0,
    KM_KEYSLOT_ENGINE_SM4,
    KM_KEYSLOT_ENGINE_HMAC_SHA1,
    KM_KEYSLOT_ENGINE_HMAC_SHA224,
    KM_KEYSLOT_ENGINE_HMAC_SHA256,
    KM_KEYSLOT_ENGINE_HMAC_SHA384,
    KM_KEYSLOT_ENGINE_HMAC_SHA512,
    KM_KEYSLOT_ENGINE_HMAC_SM3,
} km_keyslot_engine;

/* HASH */
typedef int32_t (*func_hash_init)(void);
typedef int32_t (*func_hash_deinit)(void);
typedef int32_t (*func_hash_start)(uint32_t *uapi_hash_handle, const crypto_hash_attr *hash_attr);
typedef int32_t (*func_hash_update)(uint32_t uapi_hash_handle, const crypto_buf_attr *src_buf, const uint32_t len);
typedef int32_t (*func_hash_get)(uint32_t uapi_hash_handle, crypto_hash_clone_ctx *hash_clone_ctx);
typedef int32_t (*func_hash_set)(uint32_t uapi_hash_handle, const crypto_hash_clone_ctx *hash_clone_ctx);
typedef int32_t (*func_hash_destroy)(uint32_t uapi_hash_handle);
typedef int32_t (*func_hash_finish)(uint32_t uapi_hash_handle, uint8_t *out, uint32_t *out_len);

/* HKDF */
typedef int32_t (*func_hkdf)(crypto_hkdf_t *hkdf_param, uint8_t *okm, uint32_t okm_length);
typedef int32_t (*func_hkdf_extract)(crypto_hkdf_extract_t *extract_param, uint8_t *prk, uint32_t *prk_length);
typedef int32_t (*func_hkdf_expand)(const crypto_hkdf_expand_t *expand_param, uint8_t *okm, uint32_t okm_length);

/* TRNG */
typedef int32_t (*func_trng_get_random)(uint32_t *randnum);
typedef int32_t (*func_trng_get_multi_random)(uint32_t size, uint8_t *randnum);

/* PBKDF2 */
typedef int32_t (*func_pbkdf2)(const crypto_kdf_pbkdf2_param *param, uint8_t *out, const uint32_t out_len);

/* SYMC */
typedef int32_t (*func_symc_init)(void);
typedef int32_t (*func_symc_deinit)(void);
typedef int32_t (*func_symc_create)(uint32_t *symc_handle, const crypto_symc_attr *symc_attr);
typedef int32_t (*func_symc_destroy)(uint32_t symc_handle);
typedef int32_t (*func_symc_set_config)(uint32_t symc_handle, const crypto_symc_ctrl_t *symc_ctrl);
typedef int32_t (*func_symc_get_config)(uint32_t symc_handle, crypto_symc_ctrl_t *symc_ctrl);
typedef int32_t (*func_symc_attach)(uint32_t symc_handle, uint32_t keyslot_handle);
typedef int32_t (*func_symc_encrypt)(uint32_t symc_handle, const crypto_buf_attr *src_buf,
    const crypto_buf_attr *dst_buf, uint32_t length);
typedef int32_t (*func_symc_decrypt)(uint32_t symc_handle, const crypto_buf_attr *src_buf,
    const crypto_buf_attr *dst_buf, uint32_t length);
typedef int32_t (*func_symc_get_tag)(uint32_t symc_handle, uint8_t *tag, uint32_t tag_length);
typedef int32_t (*func_symc_mac_start)(uint32_t *symc_handle, const crypto_symc_mac_attr *mac_attr);
typedef int32_t (*func_symc_mac_update)(uint32_t symc_handle, const crypto_buf_attr *src_buf, uint32_t length);
typedef int32_t (*func_symc_mac_finish)(uint32_t symc_handle, uint8_t *mac, uint32_t *mac_length);

/* PKE */
typedef int32_t (*func_pke_init)(void);
typedef int32_t (*func_pke_deinit)(void);
typedef int32_t (*func_pke_mod)(const drv_pke_data *a, const drv_pke_data *p, const drv_pke_data *c);
typedef int32_t (*func_pke_exp_mod)(const drv_pke_data *n, const drv_pke_data *k,
    const drv_pke_data *in, const drv_pke_data *out);
typedef int32_t (*func_pke_ecc_gen_key)(drv_pke_ecc_curve_type curve_type, const drv_pke_data *input_priv_key,
    const drv_pke_data *output_priv_key, const drv_pke_ecc_point *output_pub_key);
typedef int32_t (*func_pke_ecdsa_sign)(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig);
typedef int32_t (*func_pke_ecdsa_verify)(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig);
typedef int32_t (*func_pke_eddsa_sign)(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig);
typedef int32_t (*func_pke_eddsa_verify)(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig);
typedef int32_t (*func_pke_gen_ecdh_key)(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *input_pub_key,
    const drv_pke_data *input_priv_key, const drv_pke_data *output_shared_key);
typedef int32_t (*func_pke_check_dot_on_curve)(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    uint8_t *is_on_curve);
typedef int32_t (*func_pke_rsa_sign)(const drv_pke_rsa_priv_key *priv_key, drv_pke_rsa_scheme scheme,
    drv_pke_hash_type hash_type, const drv_pke_data *input_hash,
    drv_pke_data *sign);
typedef int32_t (*func_pke_rsa_verify)(const drv_pke_rsa_pub_key *pub_key, drv_pke_rsa_scheme scheme,
    drv_pke_hash_type hash_type, drv_pke_data *input_hash, const drv_pke_data *sig);
typedef int32_t (*func_pke_rsa_public_encrypt)(drv_pke_rsa_scheme scheme, drv_pke_hash_type hash_type,
    const drv_pke_rsa_pub_key *pub_key, const drv_pke_data *input, const drv_pke_data *label,
    drv_pke_data *output);
typedef int32_t (*func_pke_rsa_private_decrypt)(drv_pke_rsa_scheme scheme, drv_pke_hash_type hash_type,
    const drv_pke_rsa_priv_key *priv_key, const drv_pke_data *input, const drv_pke_data *label,
    drv_pke_data *output);

/* KM */
typedef int32_t (*func_km_init)(void);
typedef int32_t (*func_km_deinit)(void);
typedef int32_t (*func_km_create_keyslot)(uint32_t *keyslot_handle, km_keyslot_engine key_engine);
typedef void (*func_km_destroy_keyslot)(uint32_t keyslot_handle);
typedef int32_t (*func_km_set_clear_key)(uint32_t keyslot_handle, uint8_t *key, uint32_t keylen,
    km_keyslot_engine key_engine);

/* MEM */
typedef int32_t (*func_alloc_phys_buf)(crypto_buf_attr *buf_attr, void **virt_addr, uint32_t size);
typedef void (*func_free_phys_buf)(crypto_buf_attr *buf_attr, void *virt_addr, uint32_t size);
typedef int32_t (*func_get_phys_mem_addr)(const void *virt_addr, uintptr_t *phys_addr);

typedef struct {
    func_hash_init harden_hash_init;
    func_hash_deinit harden_hash_deinit;
    func_hash_start harden_hash_start;
    func_hash_update harden_hash_update;
    func_hash_get harden_hash_get;
    func_hash_set harden_hash_set;
    func_hash_destroy harden_hash_destroy;
    func_hash_finish harden_hash_finish;
    func_hkdf harden_hkdf;
    func_hkdf_extract harden_hkdf_extract;
    func_hkdf_expand harden_hkdf_expand;
    func_trng_get_random harden_trng_get_random;
    func_trng_get_multi_random harden_trng_get_multi_random;
    func_pbkdf2 harden_pbkdf2;
    func_symc_init harden_symc_init;
    func_symc_deinit harden_symc_deinit;
    func_symc_create harden_symc_create;
    func_symc_destroy harden_symc_destroy;
    func_symc_set_config harden_symc_set_config;
    func_symc_get_config harden_symc_get_config;
    func_symc_attach harden_symc_attach;
    func_symc_encrypt harden_symc_encrypt;
    func_symc_decrypt harden_symc_decrypt;
    func_symc_get_tag harden_symc_get_tag;
    func_symc_mac_start harden_symc_mac_start;
    func_symc_mac_update harden_symc_mac_update;
    func_symc_mac_finish harden_symc_mac_finish;
    func_pke_init harden_pke_init;
    func_pke_deinit harden_pke_deinit;
    func_pke_mod harden_pke_mod;
    func_pke_exp_mod harden_pke_exp_mod;
    func_pke_ecc_gen_key harden_pke_ecc_gen_key;
    func_pke_ecdsa_sign harden_pke_ecdsa_sign;
    func_pke_ecdsa_verify harden_pke_ecdsa_verify;
    func_pke_eddsa_sign harden_pke_eddsa_sign;
    func_pke_eddsa_verify harden_pke_eddsa_verify;
    func_pke_gen_ecdh_key harden_pke_gen_ecdh_key;
    func_pke_check_dot_on_curve harden_pke_check_dot_on_curve;
    func_pke_rsa_sign harden_pke_rsa_sign;
    func_pke_rsa_verify harden_pke_rsa_verify;
    func_pke_rsa_public_encrypt harden_pke_rsa_public_encrypt;
    func_pke_rsa_private_decrypt harden_pke_rsa_private_decrypt;
} mbedtls_harden_cipher_func;

void mbedtls_cipher_adapt_register_func(mbedtls_harden_cipher_func *harden_cipher_func);

static int32_t init_adapt_func(void)
{
    return 0;
}

static int32_t deinit_adapt_func(void)
{
    return 0;
}

static mbedtls_harden_cipher_func harden_cipher_func = {
    .harden_hash_init = kapi_cipher_hash_init,
    .harden_hash_deinit = kapi_cipher_hash_deinit,
    .harden_hash_start = kapi_cipher_hash_start,
    .harden_hash_update = kapi_cipher_hash_update,
    .harden_hash_get = kapi_cipher_hash_get,
    .harden_hash_set = kapi_cipher_hash_set,
    .harden_hash_destroy = kapi_cipher_hash_destroy,
    .harden_hash_finish = kapi_cipher_hash_finish,
    .harden_hkdf = kapi_cipher_hkdf,
    .harden_hkdf_extract = kapi_cipher_hkdf_extract,
    .harden_hkdf_expand = kapi_cipher_hkdf_expand,
    .harden_trng_get_random = kapi_cipher_trng_get_random,
    .harden_trng_get_multi_random = kapi_cipher_trng_get_multi_random,
    .harden_pbkdf2 = kapi_cipher_pbkdf2,
    .harden_symc_init = kapi_cipher_symc_init,
    .harden_symc_deinit = kapi_cipher_symc_deinit,
    .harden_symc_create = kapi_cipher_symc_create,
    .harden_symc_destroy = kapi_cipher_symc_destroy,
    .harden_symc_set_config = kapi_cipher_symc_set_config,
    .harden_symc_get_config = kapi_cipher_symc_get_config,
    .harden_symc_attach = kapi_cipher_symc_attach,
    .harden_symc_encrypt = kapi_cipher_symc_encrypt,
    .harden_symc_decrypt = kapi_cipher_symc_decrypt,
    .harden_symc_get_tag= kapi_cipher_symc_get_tag,
    .harden_symc_mac_start = kapi_cipher_mac_start,
    .harden_symc_mac_update = kapi_cipher_mac_update,
    .harden_symc_mac_finish = kapi_cipher_mac_finish,
    .harden_pke_init = init_adapt_func,
    .harden_pke_deinit = deinit_adapt_func,
#ifdef CONFIG_PKE_SUPPORT_BIG_NUMBER
    .harden_pke_mod = kapi_pke_mod,
    .harden_pke_exp_mod = kapi_pke_exp_mod,
#endif
#ifdef CONFIG_CONFIG_PKE_SUPPORT_ECC_GEN_KEY
    .harden_pke_ecc_gen_key = kapi_pke_ecc_gen_key,
#endif
#ifdef CONFIG_CONFIG_PKE_SUPPORT_ECC_SIGN
    .harden_pke_ecdsa_sign = kapi_pke_ecdsa_sign,
#endif
#ifdef CONFIG_CONFIG_PKE_SUPPORT_ECC_VERIFY
    .harden_pke_ecdsa_verify = kapi_pke_ecdsa_verify,
#endif
#ifdef CONFIG_PKE_SUPPORT_EDWARD
    .harden_pke_eddsa_sign = kapi_pke_eddsa_sign,
    .harden_pke_eddsa_verify = kapi_pke_eddsa_verify,
#endif
#ifdef CONFIG_CONFIG_PKE_SUPPORT_ECC_ECDH
    .harden_pke_gen_ecdh_key = kapi_pke_ecc_gen_ecdh_key,
#endif
#ifdef CONFIG_PKE_SUPPORT_ECC_CAL
    .harden_pke_check_dot_on_curve = kapi_pke_check_dot_on_curve,
#endif
#ifdef CONFIG_PKE_SUPPORT_RSA
    .harden_pke_rsa_sign = kapi_pke_rsa_sign,
    .harden_pke_rsa_verify = kapi_pke_rsa_verify,
    .harden_pke_rsa_public_encrypt = kapi_pke_rsa_public_encrypt,
    .harden_pke_rsa_private_decrypt = kapi_pke_rsa_private_decrypt,
#endif
};

int32_t mbedtls_adapt_register_func(void)
{
    mbedtls_cipher_adapt_register_func(&harden_cipher_func);

    mbedtls_alt_hash_init();
    mbedtls_alt_aes_init();
    mbedtls_alt_ecp_init();
    return 0;
}