/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V150 tcxo register \n
 *
 * History: \n
 * 2022-08-16， Create file. \n
 */

#ifndef HAL_TCXO_V150_REGS_DEF_H
#define HAL_TCXO_V150_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_tcxo_v150_regs_def TCXO V150 Regs Definition
 * @ingroup  drivers_hal_tcxo
 * @{
 */

/**
 * @brief  This union represents the bit fields in the count register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tcxo_count_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t data                   : 16;   /*!< 16-bit count value.
                                                      count3(63~48 bits)
                                                      count2(47~32 bits)
                                                      count1(31~16 bits)
                                                      count0(15~0 bits) */
    } b;                                        /*!< Register bits. */
} tcxo_count_data_t;

/**
 * @brief  This union represents the bit fields in the count register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tcxo_status_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t refresh                  : 1;   /*!< If this bit is set to 1, set tcxo count refresh. */
        uint32_t clear                    : 1;   /*!< If this bit is set to 1, set tcxo count clear. */
        uint32_t enable                   : 1;   /*!< If this bit is set to 1, set tcxo count enable. */
        uint32_t reserved                 : 1;   /*!< tcxo count reserved. */
        uint32_t valid                    : 1;   /*!< tcxo count value. */
    } b;                                        /*!< Register bits. */
} tcxo_status_data_t;

/**
 * @brief  Registers associated with TCXO.
 */
typedef struct tcxo_regs {
    volatile uint32_t status;                   /*!< status.  <i>Offset: 00h</i>. */
    volatile uint32_t count0;                   /*!< count0.  <i>Offset: 04h</i>. */
    volatile uint32_t count1;                   /*!< count1.  <i>Offset: 08h</i>. */
    volatile uint32_t count2;                   /*!< count2.  <i>Offset: 0Ch</i>. */
    volatile uint32_t count3;                   /*!< count3.  <i>Offset: 10h</i>. */
} tcxo_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif