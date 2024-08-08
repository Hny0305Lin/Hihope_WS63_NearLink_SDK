/* gcm_alt.h with dummy types for MBEDTLS_GCM_ALT */
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

#ifndef GCM_ALT_H
#define GCM_ALT_H

/**
 * \brief          The GCM context alt structure.
 */
typedef struct mbedtls_gcm_context
{
    mbedtls_cipher_context_t MBEDTLS_PRIVATE(cipher_ctx);  /*!< The cipher context used. */
    uint64_t MBEDTLS_PRIVATE(len);                         /*!< The total length of the encrypted data. */
}
mbedtls_gcm_context;


#endif /* gcm_alt.h */
