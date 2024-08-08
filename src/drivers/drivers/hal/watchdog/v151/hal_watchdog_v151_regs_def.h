/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V151 HAL watchdog \n
 *
 * History: \n
 * 2022-11-26, Create file. \n
 */
#ifndef HAL_WATCHDOG_V151_REGS_DEF_H
#define HAL_WATCHDOG_V151_REGS_DEF_H

#include <stdint.h>
#include "watchdog_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_watchdog_v151_regs_def Watchdog V151 Regs Definition
 * @ingroup  drivers_hal_watchdog
 * @{
 */

#define WDT_MODE_RESET_BY_FIRST_INTR    0
#define WDT_MODE_RESET_BY_SECOND_INTR   1

/**
 * @brief  This union represents the bit field in the Lock Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_v151_lock_data {
    uint32_t d32;
    struct  {
        uint32_t wdt_lock                 : 32;    /*!< Write to lock register, write 32bits key (0x5A5A5A5A) to
                                                        lock register, unlock register, means all registers can
                                                        be written; write other arbitrary value to lock register,
                                                        lock register, registers within lock protection range
                                                        cannot be written, registers outside lock protection range
                                                        can be written. Read the lock register (Lock register read
                                                        value indicates the lock status of the register).
                                                        -0x0000_0000 means the register is in unlocked state;
                                                        -0x0000_0001 means that the register is in the locked state. */
    } b;
} wdt_v151_lock_data_t;


/**
 * @brief  This union represents the bit field in the Load Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_v151_load_data {
    uint32_t d32;
    struct  {
        uint32_t reserved0_7               : 8;     /*!< Low 8-bit in the initial value register of the decrement
                                                         counter in wdt. */
        uint32_t wdt_load                  : 24;    /*!< Watchdog load register */
    } b;
} wdt_v151_load_data_t;

/**
 * @brief  This union represents the bit field in the Restart Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_v151_restart_data {
    uint32_t d32;
    struct  {
        uint32_t wdt_restart               : 32;    /*!< wdt Restart Register.
                                                         Write a value other than 32'h5A5A5A5A, then restart counter,
                                                         Write a value is 32'h5A5A5A5A, then restart the wdt counter. */
    } b;
} wdt_v151_restart_data_t;

/**
 * @brief  This union represents the bit field in the Interrupt clear Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_v151_eoi_data {
    uint32_t d32;
    struct  {
        uint32_t wdt_eoi                   : 1;     /*!< Reading from this register returns zeroes(0) and clear the
                                                        interrupt. */
        uint32_t reserved1_31              : 31;
    } b;
} wdt_v151_eoi_data_t;

/**
 * @brief  This union represents the bit fields in the Control Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_v151_cr_data {
    uint32_t d32;
    struct  {
        uint32_t wdt_en                    : 1;     /*!< Enable register for counter operation.
                                                         Counter Unenable(0), Counter Enable(1). */
        uint32_t reserved1                 : 1;
        uint32_t rst_en                    : 1;     /*!< System reset signal generation enable register.
                                                         Cannot generate a reset signal(0),
                                                         Can generate a reset signal(1). */
        uint32_t rst_pl                    : 3;     /*!< System reset signal effective length configuration register.
                                                         -3'b000: Valid length of the system reset signal is 8'h2,
                                                         -3'b001: Valid length of the system reset signal is 8'h4,
                                                         -3'b010: Valid length of the system reset signal is 8'h8,
                                                         -3'b011: Valid length of the system reset signal is 8'h10,
                                                         -3'b100: Valid length of the system reset signal is 8'h20,
                                                         -3'b101: Valid length of the system reset signal is 8'h40,
                                                         -3'b110: Valid length of the system reset signal is 8'h80,
                                                         -3'b111: Valid length of the system reset signal is 8'h100. */
        uint32_t wdt_imsk                  : 1;     /*!< Interrupt block Register.
                                                         -1'b0: No interruption blocking,
                                                         -1'b1: interruption blocking. */
        uint32_t wdt_mode                  : 1;     /*!< wdt working mode configuration register.
                                                         -1'b0: One interrupt to generate a reset signal,
                                                         -1'b1: Two interrupt to generate a reset signal. */
        uint32_t reserved8_31              : 24;
    } b;
} wdt_v151_cr_data_t;

/**
 * @brief  This union represents the bit field in the Decrement counter Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_v151_cnt_data {
    uint32_t d32;
    struct  {
        uint32_t wdt_cnt                   : 32;     /*!< The current value of Decrement Counter, 32 bits, Jump to the
                                                          value of the load register immediately after the unreset. */
    } b;
} wdt_v151_cnt_data_t;

/**
 * @brief  This union represents the bit field in the Raw interrupt Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_v151_raw_intr_data {
    uint32_t d32;
    struct  {
        uint32_t wdt_raw_intr              : 1;     /*!< wdt Raw Interrupt Register.
                                                         -1'b0: No raw interruptions generated,
                                                         -1'b1: Raw interruptions generated. */
        uint32_t reserved1_31              : 31;
    } b;
} wdt_v151_raw_intr_data_t;

/**
 * @brief  This union represents the bit field in the Interrupt Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_v151_intr_data {
    uint32_t d32;
    struct  {
        uint32_t wdt_intr                  : 1;     /*!< wdt Post-mask interrupt register.
                                                         -1'b0: No post-mask interrupt generated,
                                                         -1'b1: post-mask interrupt generated. */
        uint32_t reserved1_31              : 31;
    } b;
} wdt_v151_intr_data_t;

/**
 * @brief  This union represents the bit field in the Low power state Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_v151_lpif_state_data {
    uint32_t d32;
    struct  {
        uint32_t wdt_lpif_state            : 3;      /*!< Low power status indication signal.
                                                          -000: IDLE status,
                                                          -001: Normal working condition,
                                                          -010: Receive low-power request status,
                                                          -011: Enter low-power acknowledgement state,
                                                          -100: Denial of access to low power states,
                                                          -101: Waiting to exit the low-power request state,
                                                          -110: Waiting for confirmation to exit low power state,
                                                          -111: Reserved. */
        uint32_t reserved3_31              : 29;
    } b;
} wdt_v151_lpif_state_data_t;

/**
 * @brief  This union represents the bit field in the Status Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_v151_status_data {
    uint32_t d32;
    struct  {
        uint32_t wdt_status                : 1;       /*!< wdt Working Status Register.
                                                           -1'b0: wdt Free,
                                                           -1;b1: wdt Busy. */
        uint32_t reserved1_31              : 31;
    } b;
} wdt_v151_status_data_t;

/**
 * @brief  This union represents the bit field in the wdt Current Counter Value Request Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_v151_ccvr_en_data {
    uint32_t d32;
    struct  {
        uint32_t wdt_ccvr_req             : 1;        /*!< Read enable of wdt counter.
                                                           Use the following rule: When reading wdt counter,
                                                           set the bit to 1. */
        uint32_t wdt_ccvr_lock            : 1;        /*!< Latching indication signal for wdt counter.
                                                           When a wdt counter read request is initiated and the
                                                           bit is detected as valid, the valid wdt counter data
                                                           can be read via WDT_CCVR. */
        uint32_t rsvd_wdt_eoi             : 30;
    } b;
} wdt_v151_ccvr_en_data_t;


/**
 * @brief  Registers associated with Watchdog.
 */
typedef struct watchdog_v151_regs {
    volatile uint32_t wdt_lock;                      /*!< lock register.  <i>Offset: 00h</i>. */
    volatile uint32_t wdt_load;                      /*!< load register.  <i>Offset: 04h</i>. */
    volatile uint32_t wdt_restart;                   /*!< Restart register.  <i>Offset: 08h</i>. */
    volatile uint32_t wdt_eoi;                       /*!< Interrupt clear register.  <i>Offset: 0ch</i>. */
    volatile uint32_t wdt_cr;                        /*!< Control register.  <i>Offset: 10h</i>. */
    volatile uint32_t wdt_cnt;                       /*!< Count.  <i>Offset: 14h</i>. */
    volatile uint32_t wdt_raw_intr;                  /*!< Raw interrupt register.  <i>Offset: 18h</i>. */
    volatile uint32_t wdt_intr;                      /*!< interrput register.  <i>Offset: 1ch</i>. */
    volatile uint32_t wdt_lpif_state;                /*!< low power state register.  <i>Offset: 20h</i>. */
    volatile uint32_t wdt_status;                    /*!< Status register.  <i>Offset: 24h</i>. */
    volatile uint32_t wdt_ccvr_en;                   /*!< Current counter value register.  <i>Offset: 28h</i>. */
} watchdog_v151_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif