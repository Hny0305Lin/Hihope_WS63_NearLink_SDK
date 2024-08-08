/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: define the data store address in the PKE DRAM.
 *
 * Create: 2022-08-17
*/

#include "rom_lib.h"

/* the largest bit length to support is 521bits, so that we should allocate 3 block DRAM for one data,
to avoid data stamping */

/* store modulus */
const td_u32 ecc_addr_m  = 0;
/* store the result of point multiplication operation */
const td_u32 ecc_addr_cx = 3;
const td_u32 ecc_addr_cy = 6;
const td_u32 ecc_addr_cz = 9;
/* Temporary point coordinate storage space */
const td_u32 ecc_addr_ax = 12;
const td_u32 ecc_addr_ay = 15;
const td_u32 ecc_addr_az = 18;
/* Storage space for input point coordinates */
const td_u32 ecc_addr_px = 21;
const td_u32 ecc_addr_py = 24;
/* Storage space for input point coordinates */
const td_u32 ecc_addr_gx = 27;
const td_u32 ecc_addr_gy = 30;
/* Temporary variable storage space for operations */
const td_u32 ecc_addr_t0 = 33;
const td_u32 ecc_addr_t1 = 36;
const td_u32 ecc_addr_t2 = 39;
const td_u32 ecc_addr_t3 = 42;
const td_u32 ecc_addr_t4 = 45;
const td_u32 ecc_addr_tp = 48;

/* Storage space for constant values */
const td_u32 ecc_addr_p      = 51;
const td_u32 ecc_addr_rrp    = 54;
const td_u32 ecc_addr_n      = 57;
const td_u32 ecc_addr_rrn    = 60;
const td_u32 ecc_addr_mont_a = 63;
const td_u32 ecc_addr_mont_b = 66;
const td_u32 ecc_addr_mont_1_p = 69;
const td_u32 ecc_addr_mont_1_n = 72;
const td_u32 ecc_addr_const_1 = 75;
const td_u32 ecc_addr_const_0 = 78 ;

/* Storage space for digital signatures */
const td_u32 ecc_addr_s = 81;
const td_u32 ecc_addr_r = 84;
/* Storage space of the message hash value */
const td_u32 ecc_addr_e = 87;

/* Storage space of intermediate variables during signature and verification */
const td_u32 ecc_addr_k = 12; /* ecc_addr_k = ecc_addr_ax */
const td_u32 ecc_addr_d = 15; /* ecc_addr_d = ecc_addr_ay */

const td_u32 ecc_addr_u1 = 3; /* ecc_addr_u1 = ecc_addr_cx */
const td_u32 ecc_addr_u2 = 6; /* ecc_addr_u2 = ecc_addr_cy */

const td_u32 ecc_addr_v = 3; /* ecc_addr_v = ecc_addr_cx */
const td_u32 ecc_addr_rx = 3; /* ecc_addr_rx = ecc_addr_R = ecc_addr_cx */
const td_u32 ecc_addr_t = 3; /* ecc_addr_t = ecc_addr_cx */