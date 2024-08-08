/*
 *  Elliptic curves over GF(p): generic functions. And adapt to the harden API.
 *
 *  Copyright The Mbed TLS Contributors
 *  Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
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
 * References:
 *
 * SEC1 http://www.secg.org/index.php?action=secg,docs_secg
 * GECC = Guide to Elliptic Curve Cryptography - Hankerson, Menezes, Vanstone
 * FIPS 186-3 http://csrc.nist.gov/publications/fips/fips186-3/fips_186-3.pdf
 * RFC 4492 for the related TLS structures and constants
 * RFC 7748 for the Curve448 and Curve25519 curve definitions
 *
 * [Curve25519] http://cr.yp.to/ecdh/curve25519-20060209.pdf
 *
 * [2] CORON, Jean-S'ebastien. Resistance against differential power analysis
 *     for elliptic curve cryptosystems. In : Cryptographic Hardware and
 *     Embedded Systems. Springer Berlin Heidelberg, 1999. p. 292-302.
 *     <http://link.springer.com/chapter/10.1007/3-540-48059-5_25>
 *
 * [3] HEDABOU, Mustapha, PINEL, Pierre, et B'EN'ETEAU, Lucien. A comb method to
 *     render ECC resistant against Side Channel Attacks. IACR Cryptology
 *     ePrint Archive, 2004, vol. 2004, p. 342.
 *     <http://eprint.iacr.org/2004/342.pdf>
 */

#include "common.h"

/**
 * \brief Function level alternative implementation.
 *
 * The MBEDTLS_ECP_INTERNAL_ALT macro enables alternative implementations to
 * replace certain functions in this module. The alternative implementations are
 * typically hardware accelerators and need to activate the hardware before the
 * computation starts and deactivate it after it finishes. The
 * mbedtls_internal_ecp_init() and mbedtls_internal_ecp_free() functions serve
 * this purpose.
 *
 * To preserve the correct functionality the following conditions must hold:
 *
 * - The alternative implementation must be activated by
 *   mbedtls_internal_ecp_init() before any of the replaceable functions is
 *   called.
 * - mbedtls_internal_ecp_free() must \b only be called when the alternative
 *   implementation is activated.
 * - mbedtls_internal_ecp_init() must \b not be called when the alternative
 *   implementation is activated.
 * - Public functions must not return while the alternative implementation is
 *   activated.
 * - Replaceable functions are guarded by \c MBEDTLS_ECP_XXX_ALT macros and
 *   before calling them an \code if( mbedtls_internal_ecp_grp_capable( grp ) )
 *   \endcode ensures that the alternative implementation supports the current
 *   group.
 */

#if defined(MBEDTLS_ECP_C)

#include "mbedtls/ecp.h"
#include "mbedtls/error.h"
#include "mbedtls/platform_util.h"

#include "mbedtls/bignum.h"

#include "dfx.h"
#include "securec.h"
#include "mbedtls_harden_adapt_api.h"

/* Parameter validation macros based on platform_util.h */
#define ECP_VALIDATE_RET( cond )    \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_ECP_BAD_INPUT_DATA )
#define ECP_VALIDATE( cond )        \
    MBEDTLS_INTERNAL_VALIDATE( cond )

#define MAX_ECC_LEN     72
/*
 * Restartable multiplication R = m * P
 */
#if defined(MBEDTLS_ECP_MUL_ALT)
typedef struct {
    mbedtls_ecp_group_id id;
    mbedtls_alt_ecp_curve_type curve_type;
    unsigned int klen;
} inner_ecp_curve_item;

static const inner_ecp_curve_item g_curve_item[] = {
    {
        .id = MBEDTLS_ECP_DP_SECP256R1, .curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_FIPS_P256R, .klen = 32,
    },
    {
        .id = MBEDTLS_ECP_DP_SECP384R1, .curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_FIPS_P384R, .klen = 48,
    },
    {
        .id = MBEDTLS_ECP_DP_BP256R1, .curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_RFC5639_P256, .klen = 32,
    },
    {
        .id = MBEDTLS_ECP_DP_BP384R1, .curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_RFC5639_P384, .klen = 48,
    },
    {
        .id = MBEDTLS_ECP_DP_BP512R1, .curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_RFC5639_P512, .klen = 64,
    },
};

static void inner_get_curve_type_and_klen(mbedtls_ecp_group_id id,
    mbedtls_alt_ecp_curve_type *curve_type, unsigned int *klen)
{
    unsigned int i;
    *curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_INVALID;
    *klen = 0;
    for (i = 0; i < sizeof(g_curve_item) / sizeof(g_curve_item[0]); i++) {
        if (g_curve_item[i].id == id) {
            *curve_type = g_curve_item[i].curve_type;
            *klen = g_curve_item[i].klen;
            return;
        }
    }
}

int mbedtls_ecp_mul_restartable(mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
    const mbedtls_mpi *m, const mbedtls_ecp_point *P,
    int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
    mbedtls_ecp_restart_ctx *rs_ctx)
{
    int ret;
    unsigned char point_buf[MAX_ECC_LEN * 2 + 1];   // 2: for x and y.
    unsigned char m_buf[MAX_ECC_LEN];
    mbedtls_alt_ecp_curve_type curve_type;
    unsigned int klen;
    size_t out_len = sizeof(point_buf);
    mbedtls_alt_ecp_data k_data;
    mbedtls_alt_ecp_point p_point;
    mbedtls_alt_ecp_point r_point;

    ECP_VALIDATE_RET(grp != NULL);
    ECP_VALIDATE_RET(R   != NULL);
    ECP_VALIDATE_RET(m   != NULL);
    ECP_VALIDATE_RET(P   != NULL);

    if(f_rng == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    inner_get_curve_type_and_klen(grp->id, &curve_type, &klen);
    if (klen == 0) {
        mbedtls_printf("invalid curve type\n");
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    ret = mbedtls_mpi_write_binary(m, m_buf, klen);
    if (ret != 0) {
        mbedtls_printf("mbedtls_mpi_write_binary failed\n");
        goto exit_clean;
    }

    ret = mbedtls_ecp_point_write_binary(grp, P, MBEDTLS_ECP_PF_UNCOMPRESSED, &out_len, point_buf, sizeof(point_buf));
    if (ret != 0) {
        mbedtls_printf("mbedtls_ecp_point_write_binary failed\n");
        goto exit_clean;
    }

    k_data.data = m_buf;
    k_data.length = klen;
    p_point.x = (unsigned char *)(point_buf + 1);
    p_point.y = (unsigned char *)(point_buf + 1 + klen);
    p_point.length = klen;
    r_point.x = (unsigned char *)(point_buf + 1);
    r_point.y = (unsigned char *)(point_buf + 1 + klen);
    r_point.length = klen;

    ret = mbedtls_alt_ecp_mul(curve_type, &k_data, &p_point, &r_point);
    if (ret != 0) {
        mbedtls_printf("mbedtls_alt_ecp_mul failed\n");
        goto exit_clean;
    }

    ret = mbedtls_ecp_point_read_binary(grp, R, point_buf, out_len);
    if (ret != 0) {
        mbedtls_printf("mbedtls_ecp_point_read_binary failed\n");
        goto exit_clean;
    }

    ret = 0;
exit_clean:
    (void)memset_s(m_buf, sizeof(m_buf), 0, sizeof(m_buf));
    (void)memset_s(point_buf, sizeof(point_buf), 0, sizeof(point_buf));
    return ret;
}
#endif

#endif