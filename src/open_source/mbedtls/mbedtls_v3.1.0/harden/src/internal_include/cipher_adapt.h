/**
 * Copyright ( c ) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#ifndef CIPHER_ADAPT_H
#define CIPHER_ADAPT_H

#include "crypto_cipher_common_struct.h"

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
typedef td_s32 ( *func_hash_init )( td_void );
typedef td_s32 ( *func_hash_deinit )( td_void );
typedef td_s32 ( *func_hash_start )( td_handle *uapi_hash_handle, const crypto_hash_attr *hash_attr );
typedef td_s32 ( *func_hash_update )( td_handle uapi_hash_handle, const crypto_buf_attr *src_buf, const td_u32 len );
typedef td_s32 ( *func_hash_get )( td_handle uapi_hash_handle, crypto_hash_clone_ctx *hash_clone_ctx );
typedef td_s32 ( *func_hash_set )( td_handle uapi_hash_handle, const crypto_hash_clone_ctx *hash_clone_ctx );
typedef td_s32 ( *func_hash_destroy )( td_handle uapi_hash_handle );
typedef td_s32 ( *func_hash_finish )( td_handle uapi_hash_handle, td_u8 *out, td_u32 *out_len );

/* HKDF */
typedef td_s32 ( *func_hkdf )( crypto_hkdf_t *hkdf_param, td_u8 *okm, td_u32 okm_length );
typedef td_s32 ( *func_hkdf_extract )( crypto_hkdf_extract_t *extract_param, td_u8 *prk, td_u32 *prk_length );
typedef td_s32 ( *func_hkdf_expand )( const crypto_hkdf_expand_t *expand_param, td_u8 *okm, td_u32 okm_length );

/* TRNG */
typedef td_s32 ( *func_trng_get_random )( td_u32 *randnum );
typedef td_s32 ( *func_trng_get_multi_random )( td_u32 size, td_u8 *randnum );

/* PBKDF2 */
typedef td_s32 ( *func_pbkdf2 )( const crypto_kdf_pbkdf2_param *param, td_u8 *out, const td_u32 out_len );

/* SYMC */
typedef td_s32 ( *func_symc_init )( td_void );
typedef td_s32 ( *func_symc_deinit )( td_void );
typedef td_s32 ( *func_symc_create )( td_handle *symc_handle, const crypto_symc_attr *symc_attr );
typedef td_s32 ( *func_symc_destroy )( td_handle symc_handle );
typedef td_s32 ( *func_symc_set_config )( td_handle symc_handle, const crypto_symc_ctrl_t *symc_ctrl );
typedef td_s32 ( *func_symc_get_config )( td_handle symc_handle, crypto_symc_ctrl_t *symc_ctrl );
typedef td_s32 ( *func_symc_attach )( td_handle symc_handle, td_handle keyslot_handle );
typedef td_s32 ( *func_symc_encrypt )( td_handle symc_handle, const crypto_buf_attr *src_buf,
    const crypto_buf_attr *dst_buf, td_u32 length );
typedef td_s32 ( *func_symc_decrypt )( td_handle symc_handle, const crypto_buf_attr *src_buf,
    const crypto_buf_attr *dst_buf, td_u32 length );
typedef td_s32 ( *func_symc_get_tag )( td_handle symc_handle, td_u8 *tag, td_u32 tag_length );
typedef td_s32 ( *func_symc_mac_start )( td_handle *symc_handle, const crypto_symc_mac_attr *mac_attr );
typedef td_s32 ( *func_symc_mac_update )( td_handle symc_handle, const crypto_buf_attr *src_buf, td_u32 length );
typedef td_s32 ( *func_symc_mac_finish )( td_handle symc_handle, td_u8 *mac, td_u32 *mac_length );

/* PKE */
typedef td_s32 ( *func_pke_init )( td_void );
typedef td_s32 ( *func_pke_deinit )( td_void );
typedef td_s32 ( *func_pke_mod )( const drv_pke_data *a, const drv_pke_data *p, const drv_pke_data *c );
typedef td_s32 ( *func_pke_exp_mod )( const drv_pke_data *n, const drv_pke_data *k,
    const drv_pke_data *in, const drv_pke_data *out );
typedef td_s32 ( *func_pke_ecc_gen_key )( drv_pke_ecc_curve_type curve_type, const drv_pke_data *input_priv_key,
    const drv_pke_data *output_priv_key, const drv_pke_ecc_point *output_pub_key );
typedef td_s32 ( *func_pke_ecdsa_sign )( drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig );
typedef td_s32 ( *func_pke_ecdsa_verify )( drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig );
typedef td_s32 ( *func_pke_eddsa_sign )( drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig );
typedef td_s32 ( *func_pke_eddsa_verify )( drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig );
typedef td_s32 ( *func_pke_gen_ecdh_key )( drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *input_pub_key,
    const drv_pke_data *input_priv_key, const drv_pke_data *output_shared_key );
typedef td_s32 ( *func_pke_check_dot_on_curve )( drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    td_bool *is_on_curve );
typedef td_s32 ( *func_pke_rsa_sign )( const drv_pke_rsa_priv_key *priv_key, drv_pke_rsa_scheme scheme,
    drv_pke_hash_type hash_type, const drv_pke_data *input_hash,
    drv_pke_data *sign );
typedef td_s32 ( *func_pke_rsa_verify )( const drv_pke_rsa_pub_key *pub_key, drv_pke_rsa_scheme scheme,
    drv_pke_hash_type hash_type, drv_pke_data *input_hash, const drv_pke_data *sig );
typedef td_s32 ( *func_pke_rsa_public_encrypt )( drv_pke_rsa_scheme scheme, drv_pke_hash_type hash_type,
    const drv_pke_rsa_pub_key *pub_key, const drv_pke_data *input, const drv_pke_data *label,
    drv_pke_data *output );
typedef td_s32 ( *func_pke_rsa_private_decrypt )( drv_pke_rsa_scheme scheme, drv_pke_hash_type hash_type,
    const drv_pke_rsa_priv_key *priv_key, const drv_pke_data *input, const drv_pke_data *label,
    drv_pke_data *output );

/* KM */
typedef td_s32 ( *func_km_init )( td_void );
typedef td_s32 ( *func_km_deinit )( td_void );
typedef td_s32 ( *func_km_create_keyslot )( td_handle *keyslot_handle, km_keyslot_engine key_engine );
typedef td_void ( *func_km_destroy_keyslot )( td_handle keyslot_handle );
typedef td_s32 ( *func_km_set_clear_key )( td_handle keyslot_handle, td_u8 *key, td_u32 keylen,
    km_keyslot_engine key_engine );

/* MEM */
typedef td_s32 ( *func_alloc_phys_buf )( crypto_buf_attr *buf_attr, void **virt_addr, unsigned int size );
typedef td_void ( *func_free_phys_buf )( crypto_buf_attr *buf_attr, void *virt_addr, unsigned int size );
typedef td_s32 ( *func_get_phys_addr )( void *virt_addr, unsigned long *phys_addr );

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

typedef struct {
    func_km_init harden_km_init;
    func_km_deinit harden_km_deinit;
    func_km_create_keyslot harden_km_create_keyslot;
    func_km_destroy_keyslot harden_km_destroy_keyslot;
    func_km_set_clear_key harden_km_set_clear_key;
} mbedtls_harden_km_func;

typedef struct {
    func_alloc_phys_buf harden_alloc_phys_buf;
    func_free_phys_buf harden_free_phys_buf;
    func_get_phys_addr harden_get_phys_addr;
} mbedtls_harden_mem_func;

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_void mbedtls_cipher_adapt_register_func( mbedtls_harden_cipher_func *harden_cipher_func );
td_void mbedtls_km_adapt_register_func( mbedtls_harden_km_func *harden_km_func );
td_void mbedtls_mem_adapt_register_func( mbedtls_harden_mem_func *harden_mem_func );

/* HASH */
td_s32 CIPHER_HASH_INIT( td_void );
td_s32 CIPHER_HASH_DEINIT( td_void );
td_s32 CIPHER_HASH_START( td_handle *uapi_hash_handle, const crypto_hash_attr *hash_attr );
td_s32 CIPHER_HASH_UPDATE( td_handle uapi_hash_handle, const crypto_buf_attr *src_buf, const td_u32 len );
td_s32 CIPHER_HASH_GET( td_handle uapi_hash_handle, crypto_hash_clone_ctx *hash_clone_ctx );
td_s32 CIPHER_HASH_SET( td_handle uapi_hash_handle, const crypto_hash_clone_ctx *hash_clone_ctx );
td_s32 CIPHER_HASH_DESTROY( td_handle uapi_hash_handle );
td_s32 CIPHER_HASH_FINISH( td_handle uapi_hash_handle, td_u8 *out, td_u32 *out_len );

/* HKDF */
td_s32 CIPHER_HKDF( crypto_hkdf_t *hkdf_param, td_u8 *okm, td_u32 okm_length );
td_s32 CIPHER_HKDF_EXTRACT( crypto_hkdf_extract_t *extract_param, td_u8 *prk, td_u32 *prk_length );
td_s32 CIPHER_HKDF_EXPAND( const crypto_hkdf_expand_t *expand_param, td_u8 *okm, td_u32 okm_length );

/* TRNG */
td_s32 CIPHER_TRNG_GET_RANDOM( td_u32 *randnum );
td_s32 CIPHER_TRNG_GET_MULTI_RANDOM( td_u32 size, td_u8 *randnum );

/* PBKDF2 */
td_s32 CIPHER_PBKDF2( const crypto_kdf_pbkdf2_param *param, td_u8 *out, const td_u32 out_len );

/* SYMC */
td_s32 CIPHER_SYMC_INIT( td_void );
td_s32 CIPHER_SYMC_DEINIT( td_void );
td_s32 CIPHER_SYMC_CREATE( td_handle *symc_handle, const crypto_symc_attr *symc_attr );
td_s32 CIPHER_SYMC_DESTROY( td_handle symc_handle );
td_s32 CIPHER_SYMC_SET_CONFIG( td_handle symc_handle, const crypto_symc_ctrl_t *symc_ctrl );
td_s32 CIPHER_SYMC_GET_CONFIG( td_handle symc_handle, crypto_symc_ctrl_t *symc_ctrl );
td_s32 CIPHER_SYMC_ATTACH( td_handle symc_handle, td_handle keyslot_handle );
td_s32 CIPHER_SYMC_ENCRYPT( td_handle symc_handle, const crypto_buf_attr *src_buf,
    const crypto_buf_attr *dst_buf, td_u32 length );
td_s32 CIPHER_SYMC_DECRYPT( td_handle symc_handle, const crypto_buf_attr *src_buf,
    const crypto_buf_attr *dst_buf, td_u32 length );
td_s32 CIPHER_SYMC_GET_TAG( td_handle symc_handle, td_u8 *tag, td_u32 tag_length );
td_s32 CIPHER_SYMC_MAC_START( td_handle *symc_handle, const crypto_symc_mac_attr *mac_attr );
td_s32 CIPHER_SYMC_MAC_UPDATE( td_handle symc_handle, const crypto_buf_attr *src_buf, td_u32 length );
td_s32 CIPHER_SYMC_MAC_FINISH( td_handle symc_handle, td_u8 *mac, td_u32 *mac_length );

/* PKE */
td_s32 CIPHER_PKE_INIT( td_void );
td_s32 CIPHER_PKE_DEINIT( td_void );
td_s32 CIPHER_PKE_MOD( const drv_pke_data *a, const drv_pke_data *p, drv_pke_data *c );
td_s32 CIPHER_PKE_EXP_MOD( const drv_pke_data *n, const drv_pke_data *k,
    const drv_pke_data *in, const drv_pke_data *out );
td_s32 CIPHER_PKE_ECC_GEN_KEY( drv_pke_ecc_curve_type curve_type, const drv_pke_data *input_priv_key,
    const drv_pke_data *output_priv_key, const drv_pke_ecc_point *output_pub_key );
td_s32 CIPHER_PKE_ECDSA_SIGN( drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig );
td_s32 CIPHER_PKE_ECDSA_VERIFY( drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig );
td_s32 CIPHER_PKE_EDDSA_SIGN( drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig );
td_s32 CIPHER_PKE_EDDSA_VERIFY( drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig );
td_s32 CIPHER_PKE_ECC_GEN_ECDH_KEY( drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *input_pub_key,
    const drv_pke_data *input_priv_key, const drv_pke_data *output_shared_key );
td_s32 CIPHER_PKE_CHECK_DOT_ON_CURVE( drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    td_bool *is_on_curve );
td_s32 CIPHER_PKE_RSA_SIGN( const drv_pke_rsa_priv_key *priv_key, drv_pke_rsa_scheme scheme,
    drv_pke_hash_type hash_type, const drv_pke_data *input_hash,
    drv_pke_data *sign );
td_s32 CIPHER_PKE_RSA_VERIFY( const drv_pke_rsa_pub_key *pub_key, drv_pke_rsa_scheme scheme,
    drv_pke_hash_type hash_type, drv_pke_data *input_hash, const drv_pke_data *sig );
td_s32 CIPHER_PKE_RSA_PUBLIC_ENCRYPT( drv_pke_rsa_scheme scheme, drv_pke_hash_type hash_type,
    const drv_pke_rsa_pub_key *pub_key, const drv_pke_data *input, const drv_pke_data *label,
    drv_pke_data *output );
td_s32 CIPHER_PKE_RSA_PRIVATE_DECRYPT( drv_pke_rsa_scheme scheme, drv_pke_hash_type hash_type,
    const drv_pke_rsa_priv_key *priv_key, const drv_pke_data *input, const drv_pke_data *label,
    drv_pke_data *output );

/* KM */
td_s32 KM_INIT( td_void );
td_s32 KM_DEINIT( td_void );
td_s32 CIPHER_CREATE_KEYSLOT( td_handle *keyslot_handle, km_keyslot_engine key_engine );
td_void CIPHER_DESTROY_KEYSLOT( td_handle keyslot_handle );
td_s32 CIPHER_SET_CLEAR_KEY( td_handle keyslot_handle, td_u8 *key, td_u32 keylen,
    km_keyslot_engine key_engine );

/* MEM */
td_s32 CIPHER_ALLOC_BUF_ATTR( crypto_buf_attr *buf_attr, void **virt_addr, unsigned int size );
td_void CIPHER_FREE_BUF_ATTR( crypto_buf_attr *buf_attr, void *virt_addr, unsigned int size );
td_s32 CIPHER_GET_PHYS_MEM( void *virt_addr, unsigned long *phys_addr );

td_bool IS_KM_FUNC_REGISTERED(td_void);
td_bool IS_MEM_FUNC_REGISTERED(td_void);
td_bool IS_SYMC_FUNC_REGISTERED(td_void);
td_bool IS_HASH_FUNC_REGISTERED(td_void);
td_bool IS_HKDF_FUNC_REGISTERED(td_void);
td_bool IS_PBKDF2_FUNC_REGISTERED(td_void);
td_bool IS_TRNG_FUNC_REGISTERED(td_void);
td_bool IS_PKE_MOD_FUNC_REGISTERED(td_void);
td_bool IS_PKE_EXP_MOD_FUNC_REGISTERED(td_void);
td_bool IS_PKE_ECC_FUNC_REGISTERED(td_void);
td_bool IS_PKE_EDDSA_SIGN_FUNC_REGISTERED(td_void);
td_bool IS_PKE_EDDSA_VERIFY_FUNC_REGISTERED(td_void);
td_bool IS_PKE_CHECK_DOT_ON_CURVE_FUNC_REGISTERED(td_void);
td_bool IS_PKE_RSA_SIGN_FUNC_REGISTERED(td_void);
td_bool IS_PKE_RSA_VERIFY_FUNC_REGISTERED(td_void);
td_bool IS_PKE_RSA_PUBLIC_ENCRYPT_FUNC_REGISTERED(td_void);
td_bool IS_PKE_RSA_PRIVATE_DECRYPT_FUNC_REGISTERED(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* CIPHER_ADAPT_H */