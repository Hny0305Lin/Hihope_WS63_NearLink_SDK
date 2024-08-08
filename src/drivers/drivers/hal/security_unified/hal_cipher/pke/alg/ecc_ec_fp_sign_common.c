/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: sign common algorithm implementation
 *
 * Create: 2023-06-05
*/

#include "ecc_ec_fp.h"
#include "crypto_drv_common.h"
#include "drv_trng.h"
#include "hal_pke_reg.h"
#include "hal_pke.h"
#include "drv_common_pke.h"

#define ECC_MAX_LEN     (DRV_PKE_LEN_576 * 2)

td_s32 get_random_key(const td_u8 *n, const td_u32 size, td_u8 *rand CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    td_u32 j = 0;
    td_u32 k = 0;
    drv_pke_data aa = {0};
    drv_pke_data bb = {0};
    drv_pke_data pp = {0};
    drv_pke_data cc = {0};
    td_u8 tmp[ECC_MAX_LEN];
    td_u8 const_0[ECC_MAX_LEN];
    crypto_drv_func_enter();

    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), n, size, rand);
    drv_crypto_pke_check_param(n == TD_NULL);
    drv_crypto_pke_check_param(rand == TD_NULL);

    if ((size > DRV_PKE_LEN_576) || (size % 4 != 0)) { // 4 = size_u32 / size_u8, 'size * u8' should be 32X
        crypto_log_err("Invalid key size!\n");
        return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }

    for (j = 0; j < PKE_MAX_TIMES; j++) {
        ret = memset_enhance(tmp, ECC_MAX_LEN, 0x00, ECC_MAX_LEN);
        crypto_chk_return((ret != TD_SUCCESS), ret, "memset_enhance failed, ret = 0x%x", ret);
        for (k = 0; k < size * 2; k += 4) { // 4 = size_u32 / size_u8
            ret = drv_cipher_trng_get_random((td_u32 *)(tmp + k));
            crypto_chk_func_return(drv_cipher_trng_get_random, ret);
        }
        val_enhance_chk(k, size * 2); // 2: x2 is for security enhancement

        /* rand = tmp mod n */
        ret = memset_enhance(const_0, ECC_MAX_LEN, 0x00, ECC_MAX_LEN);
        crypto_chk_func_return(memset_enhance, ret);
        aa = (drv_pke_data) {.length = size, .data = const_0};
        bb = (drv_pke_data) {.length = size, .data = tmp};
        pp = (drv_pke_data) {.length = size, .data = (td_u8 *)n};
        cc = (drv_pke_data) {.length = size, .data = rand};
        ret = ecc_ecfn_add_mod(sec_arg_add_cs(&aa, &bb, &pp, &cc));
        crypto_chk_func_return(ecc_ecfn_add_mod, ret);

        if (crypto_drv_pke_common_is_zero(rand, size) != TD_TRUE) {
            break;
        }
    }
    crypto_drv_func_exit();
    return TD_SUCCESS;
}

/* (ecc_addr_px, ecc_addr_py) -> (ecc_addr_cx, ecc_addr_cy, ecc_addr_cz) */
td_s32 ecc_ecfp_aff_to_jac(const drv_pke_ecc_point *in, const drv_pke_data *mod_p, pke_ecc_point_jac *out)
{
    td_s32 ret = TD_FAILURE;
    td_u32 work_len = 0;
    crypto_drv_func_enter();
    drv_crypto_pke_check_param(mod_p == TD_NULL || mod_p->data == TD_NULL);
    drv_crypto_pke_check_param(out != TD_NULL && out->x != TD_NULL && out->y != TD_NULL && out->z != TD_NULL &&
        mod_p->length != out->length);

    /* Step 1: wait for PKE free */
    ret = hal_pke_check_free();
    crypto_chk_func_return(hal_pke_check_free, ret);

    /* Step 2: set input data into DRAM. */
    if (in != TD_NULL && in->x != TD_NULL && in->y != TD_NULL) {
        hal_pke_set_ram(sec_arg_add_cs(ecc_addr_px, in->x, in->length, mod_p->length));
        hal_pke_set_ram(sec_arg_add_cs(ecc_addr_py, in->y, in->length, mod_p->length));
    }
    /* need to reset modulur. */
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_m, mod_p->data, mod_p->length, mod_p->length));

    /* Step 3: start calculate. */
    work_len = mod_p->length / ALIGNED_TO_WORK_LEN_IN_BYTE;
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ecfp_cpy_p2c_3, work_len);

    /* Step 4: get result from DRAM. */
    /* no need to get result out, only a common process. */
    if (out != TD_NULL && out->x != TD_NULL && out->y != TD_NULL && out->z != TD_NULL) {
        hal_pke_get_ram(sec_arg_add_cs(ecc_addr_cx, out->x, out->length));
        hal_pke_get_ram(sec_arg_add_cs(ecc_addr_cy, out->y, out->length));
        hal_pke_get_ram(sec_arg_add_cs(ecc_addr_cz, out->z, out->length));
    }

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

td_s32 ecc_ecfp_mul_naf_cal(td_u32 work_len, const drv_pke_data *k)
{
    volatile td_s32 ret = TD_FAILURE;
    crypto_drv_func_enter();

    /* WARN: from here, there should be no other process which is not ecc, for the result in DRAM wasn't got out, other
    process will destroy data in the DRAM. */
    /* 3. copy negative point data */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ecfp_cpy_np2g_2, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* 4. NAF point multiplication. */
    rom_lib batch_instr_block[POINT_NAF_INSTR_NUM] = {instr_ecfp_mul_p_22_18, instr_ecfp_mul_g_22_18,
        instr_ecfp_mul_c_double_22};
    ret = crypto_drv_pke_common_point_mul_naf(batch_instr_block, POINT_NAF_INSTR_NUM, k, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_point_mul_naf, ret);

    crypto_drv_func_exit();
    return ret;
}

td_s32 ecc_ecfp_mul_naf(const drv_pke_ecc_curve *ecc, const drv_pke_data *k,
    const drv_pke_ecc_point *p, const drv_pke_ecc_point *r CIPHER_CHECK_WORD)
{
    volatile td_s32 ret = TD_FAILURE;
    td_u32 klen = 0;
    crypto_drv_func_enter();

    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), ecc, k, p, r);
    drv_crypto_pke_check_param(ecc == TD_NULL);
    drv_crypto_pke_check_param(ecc->gx == TD_NULL);
    drv_crypto_pke_check_param(ecc->gy == TD_NULL);
    drv_crypto_pke_check_param(ecc->n == TD_NULL);
    drv_crypto_pke_check_param(ecc->b == TD_NULL);
    drv_crypto_pke_check_param(ecc->a == TD_NULL);
    drv_crypto_pke_check_param(ecc->p == TD_NULL);
    drv_crypto_pke_check_param(crypto_drv_pke_common_is_zero(ecc->p, ecc->ksize) == TD_TRUE);
    drv_crypto_pke_check_param(k == TD_NULL);
    drv_crypto_pke_check_param(k->data == TD_NULL);
    drv_crypto_pke_check_param(p == TD_NULL);
    drv_crypto_pke_check_param(p->x == TD_NULL);
    drv_crypto_pke_check_param(p->y == TD_NULL);
    drv_crypto_pke_check_param(r == TD_NULL);
    drv_crypto_pke_check_param(r->x == TD_NULL);
    drv_crypto_pke_check_param(r->y == TD_NULL);

    if (g_pke_initialize != TD_TRUE) {
        crypto_log_err("PKE not initialized\n");
        return PKE_COMPAT_ERRNO(ERROR_NOT_INIT);
    }

    klen = ecc->ksize;
    if ((klen != k->length) || (klen != p->length) || (klen != r->length)) {
        crypto_log_err("Key size not equal!\n");
        return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }
    if ((klen != DRV_PKE_LEN_192) && (klen != DRV_PKE_LEN_224) &&
        (klen != DRV_PKE_LEN_256) && (klen != DRV_PKE_LEN_384) &&
        (klen != DRV_PKE_LEN_512) && (klen != DRV_PKE_LEN_576)) {
        crypto_log_err("Invalid key size!\n");
        return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }

    /* set initial parameters into DRAM. */
    ret = crypto_drv_pke_common_init_param(ecc);
    crypto_chk_func_return(crypto_drv_pke_common_init_param, ret);

    /* Step 0: set montgomery param into register. */
    ret = update_modulus(sec_arg_add_cs(ecc->p, ecc->ksize));
    crypto_chk_func_return(update_modulus, ret);

    /* Step 1: montgomery point p */
    drv_pke_data mod_p = {.data = (td_u8 *)ecc->p, .length = ecc->ksize};
    ret = ecc_ecfp_montgomery_data_aff(sec_arg_add_cs(p, &mod_p, r));
    crypto_chk_func_return(ecc_ecfp_montgomery_data_aff, ret);
    /* Step 2: trans Affine coordinate system to Jacobin coordinate system */
    /* WARN: from here, there should be no other process which is not ecc, for the result in DRAM wasn't got out, other
    process will destroy data in the DRAM. */
    ret = ecc_ecfp_aff_to_jac(r, &mod_p, TD_NULL);
    crypto_chk_func_return(ecc_ecfp_aff_to_jac, ret);

    /* Step 3: start NAF-multiplication. */
    ret = ecc_ecfp_mul_naf_cal(klen / ALIGNED_TO_WORK_LEN_IN_BYTE, k);
    crypto_chk_func_return(ecc_ecfp_mul_naf_cal, ret);

    /* Step 4: trans result data from jacobin coordinate system to affine coordinate system. */
    ret = ecc_ecfp_jac_to_aff(&mod_p);
    crypto_chk_func_return(ecc_ecfp_jac_to_aff, ret);

    /* Step 5. demontgomery data. */
    ret = ecc_ecfp_demontgomery_data_aff(r, (td_u32)ecc->ksize / ALIGNED_TO_WORK_LEN_IN_BYTE);
    crypto_chk_func_return(ecc_ecfp_demontgomery_data_aff, ret);

    crypto_drv_func_exit();
    return ret;
}
