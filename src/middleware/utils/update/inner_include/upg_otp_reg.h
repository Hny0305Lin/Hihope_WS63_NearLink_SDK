/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: header file of otp (register-op related)
 */

#ifndef UPG_OTP_REG_H
#define UPG_OTP_REG_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define REG_OTPC_BASE       0x500E0000

#define OTP_SHADOW_BASE             REG_OTPC_BASE
#define OTP_BIT_ALIGNED_LOCKABLE    (OTP_SHADOW_BASE + 0x0000)

#define OTP_REE_KEY_MASK   0xFF000000
#define OTP_REE_CODE_MASK  0x00FFFFFF

typedef union otp_bit_aligned_lockable {
    struct {
        uint32_t scs_alg_sel                 : 1; /* [0] */
        uint32_t tee_owner_sel               : 1; /* [1] */
        uint32_t rkp_deob_alg_sel            : 1; /* [2] */
        uint32_t ds_disable                  : 1; /* [3] */
        uint32_t aidsp_owner_sel             : 1; /* [4] */
        uint32_t rkp_kdf_alg_sel             : 1; /* [5] */
        uint32_t dice_cdi_enable             : 1; /* [6] */
        uint32_t reserved_0                  : 1; /* [7] */
        uint32_t provision_a_disable         : 1; /* [8] */
        uint32_t provision_b_disable         : 1; /* [9] */
        uint32_t reserved_1                  : 14; /* [23:10] */
        uint32_t sha1_disable                : 1; /* [24] */
        uint32_t sm4_disable                 : 1; /* [25] */
        uint32_t sm3_disable                 : 1; /* [26] */
        uint32_t sm2_disable                 : 1; /* [27] */
        uint32_t reserved_2                  : 4; /* [31:28] */
    } bits;
    uint32_t u32;
} otp_bit_aligned_lockable_t; /* 0x0 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* UPG_OTP_REG_H */
