#ifndef VENDOR_TEE_WRAPPER_H
#define VENDOR_TEE_WRAPPER_H

#if defined(VENDOR_TEE_WRAP_C)
#include "mbedtls/private_access.h"

#include <stddef.h>
#include <stdint.h>

#include "gcm.h"
#include "cipher.h"
typedef enum key_type_t {
    RAW_KEY = 0x0,
	WRAP_KEY = 0x1,
} key_type_t; 
typedef struct mbedtls_aes_context
{
    int MBEDTLS_PRIVATE(nr);                     /*!< The number of rounds. */
    uint32_t *MBEDTLS_PRIVATE(rk);               /*!< AES round keys. */
    uint32_t MBEDTLS_PRIVATE(buf)[68];           /*!< Unaligned data buffer. This buffer can
                                     hold 32 extra Bytes, which can be used for
                                     one of the following purposes:
                                     <ul><li>Alignment if VIA padlock is
                                             used.</li>
                                     <li>Simplifying key expansion in the 256-bit
                                         case by generating an extra round key.
                                         </li></ul> */
    uint32_t key_type;
}
mbedtls_aes_context;

int mbedtls_aes_set_wrap_key_enc( mbedtls_aes_context *ctx, uint32_t type,
    const unsigned char *key, unsigned int keybits );
int mbedtls_aes_set_wrap_key_dec( mbedtls_aes_context *ctx, uint32_t type,
    const unsigned char *key, unsigned int keybits );

int mbedtls_cipher_set_wrap_key( mbedtls_cipher_context_t *ctx,
                           unsigned int key_type,
                           const unsigned char *key,
                           int key_bitlen,
                           const mbedtls_operation_t operation );

int mbedtls_gcm_set_wrap_key( mbedtls_gcm_context *ctx,
                        mbedtls_cipher_id_t cipher,
                        uint32_t type,
                        const unsigned char *key,
                        unsigned int keybits );

int mbedtls_gcm_set_wrap_key_ext( mbedtls_gcm_context *ctx,
                        mbedtls_cipher_id_t cipher,
                        uint32_t type,
                        const unsigned char *key,
                        unsigned int keybits );
int mbedtls_gcm_auth_decrypt_ext( mbedtls_gcm_context *ctx,
                      size_t length,
                      const unsigned char *iv,
                      size_t iv_len,
                      const unsigned char *add,
                      size_t add_len,
                      const unsigned char *tag,
                      size_t tag_len,
                      const unsigned char *input,
                      unsigned char *output );
#endif /* VENDOR_TEE_WRAP_C */
#endif /* VENDOR_TEE_WRAPPER_H */
