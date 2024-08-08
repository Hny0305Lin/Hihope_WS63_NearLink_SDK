/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides v151 TSENSOR register \n
 *
 * History: \n
 * 2023-02-28， Create file. \n
 */
#ifndef HAL_TSENSOR_V151_REGS_DEF_H
#define HAL_TSENSOR_V151_REGS_DEF_H

#include <stdint.h>
#include "tsensor_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_tsensor_v151_regs_def Tsensor V151 Regs Definition
 * @ingroup  drivers_hal_tsensor
 * @{
 */

#define TSENSOR_COMM_REGS_OFFSET 0x0000
#define TSENSOR_CTRL_REGS_OFFSET 0x0300

#define TEMP_CODE_MIN   114
#define TEMP_CODE_MAX   896
#define TEMP_VAL_MIN    (-40)
#define TEMP_VAL_MAX    125

/**
 * @brief  This union represents the bit fields in the tsensor_ctl_id register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tsensor_ctl_id_data {
    uint32_t d32;                       /*!< tsensor_ctl_id register data. */
    struct {
        uint32_t tsensor_ctl_id : 16;   /*!< Tsensor ctl ID. */
    } b;                                /*!< Register bits. */
} tsensor_ctl_id_data_t;


/**
 * @brief  This union represents the bit fields in the tsensor_common_reg register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tsensor_common_reg_data {
    uint32_t d32;                           /*!< tsensor_common_reg register data. */
    struct {
        uint32_t tsensor_common_reg : 16;   /*!< Tsensor common reg. */
    } b;                                    /*!< Register bits. */
} tsensor_common_reg_data_t;

/**
 * @brief  This union represents the bit fields in the tsensor_start register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tsensor_start_data {
    uint32_t d32;                       /*!< tsensor_start register data. */
    struct {
        uint32_t tsensor_start  : 1;    /*!< In automatic mode, write 1 to refresh the temperature code and
                                             read back tsensor_data_auto to obtain the current temperature.
                                             When tsensor_rdy_auto is 1, the temperature is valid.
                                             Writing 0 is invalid. */
    } b;                                /*!< Register bits. */
} tsensor_start_data_t;

/**
 * @brief  This union represents the bit fields in the tsensor_ctrl register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tsensor_ctrl_data {
    uint32_t d32;                       /*!< tsensor_ctrl register data. */
    struct {
        uint32_t tsensor_enable : 1;    /*!< If this bit is set to 1, open tsensor_ctrl */
        uint32_t tsensor_mode   : 2;    /*!< b00: 16-point average single reporting mode.
                                             b01: 16-point average cyclic reporting mode.
                                             b10/b11: Single-point cyclic reporting mode. */
    } b;                                /*!< Register bits. */
} tsensor_ctrl_data_t;

/**
 * @brief  This union represents the bit fields in the tsensor_sts register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tsensor_sts_data {
    uint32_t d32;                       /*!< tsensor_sts register data. */
    struct {
        uint32_t tsensor_clr    : 1;    /*!< Set 1 to clear the status of all modes */
        uint32_t tsensor_rdy    : 1;    /*!< In all modes:
                                             0: Detection is not started or in manual detection.
                                             1: Value of tsensor_data is valid. */
        uint32_t tsensor_data   : 10;   /*!< 10-bit temperature range code value output,
                                             linear distribution;
                                             -40C~ dec'xxx,
                                             125C~ dec'xxx,
                                             That is, the tsensor direct output code value to
                                             the temperature °C unit conversion formula T_°C =
                                             [BIN2DEC(TEMP_OUT<9:0>)-132]/(921-132)*[125-(-40)]+(-40)
                                             */
    } b;                                                /*!< Register bits. */
} tsensor_sts_data_t;

/**
 * @brief  This union represents the bit fields in the tsensor_ctrl1 register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tsensor_ctrl1_data {
    uint32_t d32;                       /*!< tsensor_ctrl1 register data. */
    struct {
        uint32_t temp_calib     : 1;    /*!< If this bit is set to 1,
                                             TSENSOR close Calibration algorithm. */
        uint32_t temp_ct_sel    : 2;    /*!< 00-----0.512ms；
                                             01-----0.256ms；
                                             10-----1.024ms；
                                             11-----2.048msstop. */
        uint32_t temp_set       : 1;
        uint32_t temp_scan_dft  : 1;    /* DFT enable
                                           0: Function output.
                                           1: DFT output. */
    } b;                                /*!< Register bits. */
} tsensor_ctrl1_data_t;

/**
 * @brief  This union represents the bit fields in the tsensor_temp_high_limit register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tsensor_temp_high_limit_data {
    uint32_t d32;                                   /*!< tsensor_temp_high_limit register data. */
    struct {
        uint32_t tsensor_temp_high_limit    : 10;   /*!< Tsensor temperature high limit code. */
    } b;                                            /*!< Register bits. */
} tsensor_temp_high_limit_data_t;

/**
 * @brief  This union represents the bit fields in the tsensor_temp_low_limit register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tsensor_temp_low_limit_data {
    uint32_t d32;                                   /*!< tsensor_temp_low_limit register data. */
    struct {
        uint32_t tsensor_temp_low_limit    : 10;   /*!< Tsensor temperature low limit code. */
    } b;                                            /*!< Register bits. */
} tsensor_temp_low_limit_data_t;

/**
 * @brief  This union represents the bit fields in the tsensor_over_temp register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tsensor_over_temp_data {
    uint32_t d32;                                   /*!< tsensor_over_temp register data. */
    struct {
        uint32_t tsensor_overtemp_thresh    : 10;   /*!< Over-temperature PA protection thershold code. */
        uint32_t tsensor_overtemp_thresh_en : 1;    /*!< Over-temperature PA protection enable:
                                                         0: Disable.
                                                         1: Enable. */
    } b;                                            /*!< Register bits. */
} tsensor_over_temp_data_t;

/**
 * @brief  This union represents the bit fields in the tsensor_temp_int_en register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tsensor_temp_int_en_data {
    uint32_t d32;                                   /*!< tsensor_temp_int_en register data. */
    struct {
        uint32_t tsensor_done_int_en        : 1;    /*!< Tsensor temperature collection completion interrupt enable.
                                                         0: Disable.
                                                         1: Enable. */
        uint32_t tsensor_out_thresh_int_en  : 1;    /*!< Tsensor temperature out-of-threshold interrupt enable.
                                                         0: Disable.
                                                         1: Enable. */
        uint32_t tsensor_overtemp_int_en    : 1;    /*!< Tsensor over-temperature interrupt enable.
                                                         0: Disable.
                                                         1: Enable. */
    } b;                                            /*!< Register bits. */
} tsensor_temp_int_en_data_t;

/**
 * @brief  This union represents the bit fields in the tsensor_temp_int_clr register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tsensor_temp_int_clr_data {
    uint32_t d32;                           /*!< tsensor_temp_int_clr register data. */
    struct {
        uint32_t tsensor_int_clr    : 1;    /*!< Write 1 to clear interrupt. */
    } b;                                    /*!< Register bits. */
} tsensor_temp_int_clr_data_t;

/**
 * @brief  This union represents the bit fields in the tsensor_temp_int_sts register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tsensor_temp_int_sts_data {
    uint32_t d32;                                   /*!< tsensor_temp_int_sts register data. */
    struct {
        uint32_t tsensor_done_int_sts       : 1;    /*!< Tsensor temperature collection completion interrupt status. */
        uint32_t tsensor_out_thresh_int_sts : 1;    /*!< Tsensor temperature out-of-threshold interrupt status. */
        uint32_t tsensor_overtemp_int_sts   : 1;    /*!< Tsensor over-temperature interrupt enable. */
    } b;                                       /*!< Register bits. */
} tsensor_temp_int_sts_data_t;

/**
 * @brief  This union represents the bit fields in the tsensor_auto_refresh_period register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tsensor_auto_refresh_period_data {
    uint32_t d32;                                       /*!< tsensor_auto_refresh_period register data. */
    struct {
        uint32_t tsensor_auto_refresh_period    : 16;   /*!< Tsensor automatic detection cycle, 32kHz clock cycles */
    } b;                                                /*!< Register bits. */
} tsensor_auto_refresh_period_data_t;

/**
 * @brief  This union represents the bit fields in the tsensor_auto_refresh_cfg register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tsensor_auto_refresh_cfg_data {
    uint32_t d32;                                       /*!< tsensor_auto_refresh_cfg register data. */
    struct {
        uint32_t tsensor_auto_refresh_enable    : 1;    /*!< periodic detection enable in
                                                            16-point average single reporting mode. */
    } b;                                                /*!< Register bits. */
} tsensor_auto_refresh_cfg_data_t;

typedef struct tsensor_comm_regs {
    volatile uint32_t tsensor_ctl_id;           /*!< tsensor_ctl_id.    <i>Offset: 00h</i>. */
    volatile uint32_t rsv0[3];                  /*!< reserve.           <i>Offset: 04h ~ 0Ch</i>. */
    volatile uint32_t tsensor_reg0;             /*!< tsensor_reg0.      <i>Offset: 10h</i>. */
    volatile uint32_t tsensor_reg1;             /*!< tsensor_reg1.      <i>Offset: 14h</i>. */
    volatile uint32_t tsensor_reg2;             /*!< tsensor_reg2.      <i>Offset: 18h</i>. */
    volatile uint32_t tsensor_reg3;             /*!< tsensor_reg3.      <i>Offset: 1Ch</i>. */
} tsensor_comm_regs_t;

typedef struct tsensor_ctrl_regs {
    volatile uint32_t tsensor_start;                /*!< tsensor_start.                 <i>Offset: 0300h</i>. */
    volatile uint32_t tsensor_ctrl;                 /*!< tsensor_ctrl.                  <i>Offset: 0304h</i>. */
    volatile uint32_t tsensor_sts;                  /*!< tsensor_sts.                   <i>Offset: 0308h</i>. */
    volatile uint32_t rsv0;                         /*!< reserve.                       <i>Offset: 030Ch</i>. */
    volatile uint32_t tsensor_ctrl1;                /*!< tsensor_ctrl1.                 <i>Offset: 0310h</i>. */
    volatile uint32_t tsensor_temp_high_limit;      /*!< tsensor_temp_high_limit.       <i>Offset: 0314h</i>. */
    volatile uint32_t tsensor_temp_low_limit;       /*!< tsensor_temp_low_limit.        <i>Offset: 0318h</i>. */
    volatile uint32_t tsensor_over_temp;            /*!< tsensor_over_temp.             <i>Offset: 031Ch</i>. */
    volatile uint32_t tsensor_temp_int_en;          /*!< tsensor_temp_int_en.           <i>Offset: 0320h</i>. */
    volatile uint32_t tsensor_temp_int_clr;         /*!< tsensor_temp_int_clr.          <i>Offset: 0324h</i>. */
    volatile uint32_t tsensor_temp_int_sts;         /*!< tsensor_temp_int_sts.          <i>Offset: 0328h</i>. */
    volatile uint32_t rsv1;                         /*!< reserve.                       <i>Offset: 032Ch</i>. */
    volatile uint32_t tsensor_auto_refresh_period;  /*!< tsensor_auto_refresh_period.   <i>Offset: 0330h</i>. */
    volatile uint32_t tsensor_auto_refresh_cfg;     /*!< tsensor_auto_refresh_cfg.      <i>Offset: 0334h</i>. */
} tsensor_ctrl_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
