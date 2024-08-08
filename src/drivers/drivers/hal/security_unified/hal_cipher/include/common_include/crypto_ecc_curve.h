/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: crypto ecc curve header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef CRYPTO_ECC_CURVE_H
#define CRYPTO_ECC_CURVE_H

#include "crypto_pke_struct.h"

/* RFC 5639 BrainPool */
const drv_pke_ecc_curve *BrainPool_P256_curve(td_void);
const drv_pke_ecc_curve *BrainPool_P384_curve(td_void);
const drv_pke_ecc_curve *BrainPool_P512_curve(td_void);

/* NIST FIPS 186-4 */
const drv_pke_ecc_curve *fips_256K_curve(td_void);
const drv_pke_ecc_curve *fips_256R_curve(td_void);
const drv_pke_ecc_curve *fips_384R_curve(td_void);
const drv_pke_ecc_curve *fips_521R_curve(td_void);

/* RFC 7748 Curve25519 */
const drv_pke_ecc_curve *rfc7748_curve(td_void);

/* RFC8032 ED25519 */
const drv_pke_ecc_curve *rfc8032_curve(td_void);

/* GMT 0003.2-2012 */
const drv_pke_ecc_curve *sm2_curve(td_void);

const drv_pke_ecc_curve *get_ecc_curve(drv_pke_ecc_curve_type curve_type);

#endif