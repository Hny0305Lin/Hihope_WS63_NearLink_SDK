/**
 * \file config-suite-b.h
 *
 * \brief Minimal configuration for TLS NSA Suite B Profile (RFC 6460)
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*
 * Minimal configuration for TLS NSA Suite B Profile (RFC 6460)
 *
 * Distinguishing features:
 * - no RSA or classic DH, fully based on ECC
 * - optimized for low RAM usage
 *
 * Possible improvements:
 * - if 128-bit security is enough, disable secp384r1 and SHA-512
 * - use embedded certs in DER format and disable PEM_PARSE_C and BASE64_C
 *
 * See README.txt for usage instructions.
 */

#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#define MBEDTLS_RSA_C
#define MBEDTLS_THREADING_C
#define MBEDTLS_THREADING_PTHREAD
#define MBEDTLS_FS_IO
#define MBEDTLS_SSL_TICKET_C
#define MBEDTLS_MD5_C
#define MBEDTLS_DEBUG_C
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_SSL_RENEGOTIATION
#define MBEDTLS_ARC4_C
#define MBEDTLS_ERROR_C
#define MBEDTLS_X509_CRL_PARSE_C
#define MBEDTLS_VERSION_C
#define MBEDTLS_PK_WRITE_C
#define MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_SESSION_TICKETS
#define MBEDTLS_PKCS5_C
#define MBEDTLS_DES_C
#define MBEDTLS_VENDOR_ARM_ALG
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_CRT_WRITE_C
#define MBEDTLS_X509_CREATE_C
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_DHM_C
#define MBEDTLS_SSL_CACHE_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_X509_CSR_WRITE_C
#define MBEDTLS_PEM_WRITE_C
#define MBEDTLS_GENPRIME

#define MBEDTLS_HAVE_ASM
#define MBEDTLS_HAVE_TIME

#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#define MBEDTLS_SSL_PROTO_TLS1_2

#define MBEDTLS_AES_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_ECDH_C
#define MBEDTLS_ECDSA_C
#define MBEDTLS_ECP_C
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_GCM_C
#define MBEDTLS_MD_C
#define MBEDTLS_NET_C
#define MBEDTLS_OID_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C

#define MBEDTLS_SHA224_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA384_C
#define MBEDTLS_SHA512_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_USE_C

#define MBEDTLS_BASE64_C
#define MBEDTLS_PEM_PARSE_C

#define MBEDTLS_AES_ROM_TABLES

#define MBEDTLS_MPI_MAX_SIZE    1024 // 384-bit EC curve = 1024 bytes

#define MBEDTLS_ECP_WINDOW_SIZE        2
#define MBEDTLS_ECP_FIXED_POINT_OPTIM  0

#define MBEDTLS_ECP_NIST_OPTIM

#define MBEDTLS_ENTROPY_MAX_SOURCES 2

#define MBEDTLS_SSL_CIPHERSUITES                        \
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,    \
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256

#define MBEDTLS_SSL_IN_CONTENT_LEN             16384
#define MBEDTLS_SSL_OUT_CONTENT_LEN             16384
