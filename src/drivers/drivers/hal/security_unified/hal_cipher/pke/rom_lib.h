/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: romlib instruction functions.
 *
 * Create: 2022-08-15
*/

#ifndef ROM_LIB_H
#define ROM_LIB_H

#include "td_type.h"
/* Here the ram should have been set into the address, and the instructions have shown which DRAM will be used for
calculate. */
typedef struct {
    td_u32 instr_addr;     /* the start address for instructions, which is ROM or SRAM address. */
    td_u32 instr_num;       /* the number of instructions from the instr_addr, which will be used for calculate. */
} rom_lib;

/* etip: fromer rom lib design. */
typedef struct {
    td_u32 instr_addr;     /* the start address for instructions, which is ROM or SRAM address. */
    const td_u32 *instr1_num;       /* array to store the number of each instruction section. */
    const td_u8 *instr_description;    /* the description of instructions's function */
} rom_lib_block;

/************************************************** ecc instr start************************************/

extern const rom_lib instr_ecfp_j2a_pre_5;

extern const rom_lib instr_ecfp_j2a_exp_00;

extern const rom_lib instr_ecfp_j2a_exp_01;

extern const rom_lib instr_ecfp_j2a_exp_10;

extern const rom_lib instr_ecfp_j2a_exp_11;

extern const rom_lib instr_ecfp_j2a_post_4;

extern const rom_lib instr_ecfp_demont_c_6;

extern const rom_lib instr_ecfp_demont_cz_3;

extern const rom_lib instr_ecfp_mont_p_2;

extern const rom_lib instr_ecfp_cpy_p2c_3;

extern const rom_lib instr_ecfp_cpy_np2g_2;

extern const rom_lib instr_ecfp_mul_p_22_18;

extern const rom_lib instr_ecfp_mul_g_22_18;

extern const rom_lib instr_ecfp_mul_c_double_22;

extern const rom_lib instr_ecfp_prime_n_1;

extern const rom_lib instr_ecfn_inv_pre_5;

extern const rom_lib instr_ecfn_inv_exp_00;

extern const rom_lib instr_ecfn_inv_exp_01;

extern const rom_lib instr_ecfn_inv_exp_10;

extern const rom_lib instr_ecfn_inv_exp_11;

extern const rom_lib instr_ecfn_sign_s_12;

extern const rom_lib instr_ecfn_verify_u_10;

extern const rom_lib instr_ecfp_add_ja_verify_18;

extern const rom_lib instr_ecfp_cpy_a2c_3;

extern const rom_lib instr_ecfp_cpy_g2c_3;

extern const rom_lib instr_ecfp_mul_jj_22_23;

extern const rom_lib instr_ecfn_verify_v_4;

/************************************************** ecc instr end************************************/

/************************************************** sm2 instr start************************************/
extern const rom_lib instr_sm2_sign_r_1;

#ifdef CONFIG_PKE_SUPPORT_SM2
#ifdef CONFIG_PKE_SUPPORT_SM2_SIGN
extern const rom_lib instr_sm2_sign_s_pre_6;

extern const rom_lib instr_sm2_sign_s_post_6;
#endif
#endif

extern const rom_lib instr_sm2_verify_t_3;
/************************************************** sm2 instr end************************************/

/************************************************** curve instr start************************************/
#ifdef CONFIG_PKE_SUPPORT_CURVE
extern const rom_lib instr_curve25519_pre_ladder;

extern const rom_lib instr_curve25519_ladder_0;

extern const rom_lib instr_curve25519_ladder_1;

extern const rom_lib instr_curve25519_xz2x_pre;

extern const rom_lib instr_curve25519_xz2x_exp_00;

extern const rom_lib instr_curve25519_xz2x_exp_01;

extern const rom_lib instr_curve25519_xz2x_exp_10;

extern const rom_lib instr_curve25519_xz2x_exp_11;

extern const rom_lib instr_curve25519_xz2x_post;
#endif
/************************************************** curve instr end************************************/

/************************************************** ed instr start************************************/
#ifdef CONFIG_PKE_SUPPORT_EDWARD
extern const rom_lib instr_ed25519_mul_loop_0;

extern const rom_lib instr_ed25519_mul_a_loop_1;

extern const rom_lib instr_ed25519_mul_p_loop_1;

extern const rom_lib instr_ed25519_mul_g_loop_1;

extern const rom_lib instr_ed25519_mul_a_pre;

extern const rom_lib instr_ed_ecfp_xyz2xy_pre;

extern const rom_lib instr_ed_ecfp_xyz2xy_exp_00;

extern const rom_lib instr_ed_ecfp_xyz2xy_exp_01;

extern const rom_lib instr_ed_ecfp_xyz2xy_exp_10;

extern const rom_lib instr_ed_ecfp_xyz2xy_exp_11;

extern const rom_lib instr_ed_ecfp_xyz2xy_post;

extern const rom_lib instr_ed25519_mul_sub_pre;

extern const rom_lib instr_ed25519_calc_h;

extern const rom_lib instr_ed25519_calc_s;

extern const rom_lib instr_ed25519_recover_x_pre;

extern const rom_lib instr_ed25519_recover_x_exp_00;

extern const rom_lib instr_ed25519_recover_x_exp_01;

extern const rom_lib instr_ed25519_recover_x_exp_10;

extern const rom_lib instr_ed25519_recover_x_exp_11;

extern const rom_lib instr_ed25519_recover_x_post;

extern const rom_lib instr_ed25519_cpy_tx0_to_tx1;

extern const rom_lib instr_ed25519_cpy_tx1_to_tx0;

extern const rom_lib instr_ed25519_cpy_t2g;

extern const rom_lib instr_ed25519_cpy_t2r;

extern const rom_lib instr_ed25519_c_eq_r;
#endif
/************************************************** ed instr end************************************/

/************************************************** rsa instr start************************************/
#if defined(CONFIG_PKE_SUPPORT_RSA) || defined(CONFIG_PKE_SUPPORT_BIG_NUMBER)
extern const rom_lib instr_rsa_exp_pre_6;

extern const rom_lib instr_rsa_exp_00;

extern const rom_lib instr_rsa_exp_01;

extern const rom_lib instr_rsa_exp_10;

extern const rom_lib instr_rsa_exp_11;

extern const rom_lib instr_rsa_exp_post_3;

extern const rom_lib instr_rsa_regular_mul;

extern const rom_lib instr_rsa_rrn_add;

extern const rom_lib instr_rsa_rrn_add_3072;

extern const rom_lib instr_rsa_rrn_add_320;

extern const rom_lib instr_rsa_rrn_mul;

extern const rom_lib instr_rsa_mod;

extern const rom_lib instr_rsa_mod_add;

extern const rom_lib instr_rsa_mod_sub;

extern const rom_lib instr_rsa_mod_mul;
#endif
/************************************************** rsa instr end************************************/

/********************************************** ecc param storage space define start********************************/
extern const td_u32 ecc_addr_m;
extern const td_u32 ecc_addr_cx;
extern const td_u32 ecc_addr_cy;
extern const td_u32 ecc_addr_cz;
extern const td_u32 ecc_addr_ax;
extern const td_u32 ecc_addr_ay;
extern const td_u32 ecc_addr_az;
extern const td_u32 ecc_addr_px;
extern const td_u32 ecc_addr_py;
extern const td_u32 ecc_addr_gx;
extern const td_u32 ecc_addr_gy;
extern const td_u32 ecc_addr_t0;
extern const td_u32 ecc_addr_t1;
extern const td_u32 ecc_addr_t2;
extern const td_u32 ecc_addr_t3;
extern const td_u32 ecc_addr_t4;
extern const td_u32 ecc_addr_tp;

extern const td_u32 ecc_addr_p;
extern const td_u32 ecc_addr_rrp;
extern const td_u32 ecc_addr_n;
extern const td_u32 ecc_addr_rrn;
extern const td_u32 ecc_addr_mont_a;
extern const td_u32 ecc_addr_mont_b;
extern const td_u32 ecc_addr_mont_1_p;
extern const td_u32 ecc_addr_mont_1_n;
extern const td_u32 ecc_addr_const_1;
extern const td_u32 ecc_addr_const_0;

extern const td_u32 ecc_addr_s;
extern const td_u32 ecc_addr_r;
extern const td_u32 ecc_addr_e;

extern const td_u32 ecc_addr_k;
extern const td_u32 ecc_addr_d;

extern const td_u32 ecc_addr_u1;
extern const td_u32 ecc_addr_u2;

extern const td_u32 ecc_addr_v;
extern const td_u32 ecc_addr_rx;
extern const td_u32 ecc_addr_t;
/********************************************** ecc param storage space define end********************************/

/*************************************** curve param storage space define start************************************/
#ifdef CONFIG_PKE_SUPPORT_CURVE
extern const td_u32 curve_addr_m;
/* store results */
extern const td_u32 curve_addr_x2;
extern const td_u32 curve_addr_z2;
/* store point coordinate temporarily */
extern const td_u32 curve_addr_x3;
extern const td_u32 curve_addr_z3;
extern const td_u32 curve_addr_x1;

/* store temporary variable */
extern const td_u32 curve_addr_t0;
extern const td_u32 curve_addr_t1;
extern const td_u32 curve_addr_t2;
extern const td_u32 curve_addr_t3;
extern const td_u32 curve_addr_t4;
extern const td_u32 curve_addr_t5;
extern const td_u32 curve_addr_t6;
extern const td_u32 curve_addr_tp;

/* store the const value */
extern const td_u32 curve_addr_p;
extern const td_u32 curve_addr_rrp;
extern const td_u32 curve_addr_n;
extern const td_u32 curve_addr_rrn;
extern const td_u32 curve_addr_mont_a;
extern const td_u32 curve_addr_mont_a24;
extern const td_u32 curve_addr_mont_1_p;
extern const td_u32 curve_addr_mont_1_n;
extern const td_u32 curve_addr_const_1;
extern const td_u32 curve_addr_const_0;
#endif
/*************************************** curve param storage space define end************************************/

/*************************************** eddsa param storage space define start************************************/
#ifdef CONFIG_PKE_SUPPORT_EDWARD
/* store modulus */
extern const td_u32 ed_addr_m;
/* store the result for point mulplication. */
extern const td_u32 ed_addr_cx;
extern const td_u32 ed_addr_cy;
extern const td_u32 ed_addr_cz;
extern const td_u32 ed_addr_ct;
/* store point coordinate for temporary */
extern const td_u32 ed_addr_ax;
extern const td_u32 ed_addr_ay;
extern const td_u32 ed_addr_az;
extern const td_u32 ed_addr_at;
/* store the input point */
extern const td_u32 ed_addr_px;
extern const td_u32 ed_addr_py;
extern const td_u32 ed_addr_pz;
extern const td_u32 ed_addr_pt;
/* store the input point */
extern const td_u32 ed_addr_gx;
extern const td_u32 ed_addr_gy;
extern const td_u32 ed_addr_gz;
extern const td_u32 ed_addr_gt;

/* store the tmeporary variable */
extern const td_u32 ed_addr_tp;
extern const td_u32 ed_addr_t0;
extern const td_u32 ed_addr_t1;
extern const td_u32 ed_addr_t2;
extern const td_u32 ed_addr_t3;
extern const td_u32 ed_addr_t4;

/* store the const value */
extern const td_u32 ed_addr_p;
extern const td_u32 ed_addr_rrp;
extern const td_u32 ed_addr_n;
extern const td_u32 ed_addr_rrn;
extern const td_u32 ed_addr_mont_d;
extern const td_u32 ed_addr_mont_1_p;
extern const td_u32 ed_addr_mont_1_n;
extern const td_u32 ed_addr_const_1;
extern const td_u32 ed_addr_const_0;
extern const td_u32 ed_addr_sqrt_m1;

/* store intermediate variable for sign */
extern const td_u32 ed_addr_r;
extern const td_u32 ed_addr_h;
/* store private key for sign */
extern const td_u32 ed_addr_sk;
/* store result for signature */
extern const td_u32 ed_addr_s;

/* store temporary variable for verify */
extern const td_u32 ed_addr_u;
extern const td_u32 ed_addr_v;

extern const td_u32 ed_addr_tx0;
extern const td_u32 ed_addr_tx1;
extern const td_u32 ed_addr_ty;
extern const td_u32 ed_addr_bx;
extern const td_u32 ed_addr_by;
extern const td_u32 ed_addr_rx;
extern const td_u32 ed_addr_ry;
#endif
/*************************************** eddsa param storage space define end************************************/

/********************************************** rsa param storage space define start******************************/
#if defined(CONFIG_PKE_SUPPORT_RSA) || defined(CONFIG_PKE_SUPPORT_BIG_NUMBER)
/* modulus storage space */
extern const td_u32 rsa_addr_n;
/* storage space for base number of the modular exponentiation */
extern const td_u32 rsa_addr_a;
/* storage sapce for montgomery parameter */
extern const td_u32 rsa_addr_rr;
/* storage sapce for constant value 1 */
extern const td_u32 rsa_addr_const_1;
/* storage sapce of result */
extern const td_u32 rsa_addr_s;
/* storage sapce for intermediate variable of CRT mode */
extern const td_u32 rsa_addr_m2;
extern const td_u32 rsa_addr_q_inv;
/* storage sapce for common intermediate variable */
extern const td_u32 rsa_addr_tp;
/* storage sapce for input data */
extern const td_u32 rsa_addr_t0;
extern const td_u32 rsa_addr_t1;
#endif
/********************************************** rsa param storage space define end******************************/

#endif