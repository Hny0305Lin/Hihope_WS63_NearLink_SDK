/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V151 dma register \n
 *
 * History: \n
 * 2022-2-25， Create file. \n
 */
#ifndef HAL_DMAC_V151_REGS_DEF_H
#define HAL_DMAC_V151_REGS_DEF_H

#include <stdint.h>
#include "dma_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_dma_v151_regs_def DMA V151 Regs Definition
 * @ingroup  drivers_hal_dma
 * @{
 */


/**
 * @brief  This union represents the bit fields in the DMA channel interrupt.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dam_ch_int_data {
    uint32_t d32;                         /*!< Raw register data. */
    struct {
        uint32_t ch_0 : 1;                /*!< Channel 0 interrupt register. */
        uint32_t ch_1 : 1;                /*!< Channel 1 interrupt register. */
        uint32_t ch_2 : 1;                /*!< Channel 2 interrupt register. */
        uint32_t ch_3 : 1;                /*!< Channel 3 interrupt register. */
        uint32_t ch_4 : 1;                /*!< Channel 4 interrupt register. */
        uint32_t ch_5 : 1;                /*!< Channel 5 interrupt register. */
        uint32_t ch_6 : 1;                /*!< Channel 6 interrupt register. */
        uint32_t ch_7 : 1;                /*!< Channel 7 interrupt register. */
    } b;                                  /*!< Register bits. */
} dam_ch_int_data_t;

/**
 * @brief Interrupt Status Register
 */
typedef union {
    struct {
        uint32_t    int_st        : 8;
        uint32_t    int_trans_st  : 8;
        uint32_t    int_err_st    : 8;
        uint32_t    reserved      : 8;
    } b;

    uint32_t    d32;
} dma_int_st_t;

/**
 * @brief Transfer Interrupt Register
 */
typedef union {
    struct {
        uint32_t    int_trans_clr   : 8;
        uint32_t    int_err_clr     : 8;
        uint32_t    reserved        : 16;
    } b;

    uint32_t    d32;
} dma_int_clr_t;

/**
 * @brief Raw Interrupt Status Register
 */
typedef union {
    struct {
        uint32_t    ori_int_trans_st  : 8;
        uint32_t    ori_int_err_st    : 8;
        uint32_t    reserved          : 16;
    } b;

    uint32_t    d32;
} dma_ori_int_st_t;

/**
 * @brief Channel Enable Query Register
 */
typedef union {
    struct {
        uint32_t    en_chns   : 8;
        uint32_t    reserved  : 24;
    } b;

    uint32_t    d32;
} dma_en_chns_t;

/**
 * @brief BURST software configuration register
 */
typedef union {
    struct {
        uint32_t    last_burst_req  : 16;
        uint32_t    burst_req       : 16;
    } b;

    uint32_t    d32;
} dma_burst_req_t;

/**
 * @brief SINGLE software configuration register
 */
typedef union {
    struct {
        uint32_t    last_single_req  : 16;
        uint32_t    single_req       : 16;
    } b;

    uint32_t    d32;
} dma_single_req_t;
/**
 * @brief configuration register
 */
typedef union {
    struct {
        uint32_t    dma_enable  : 1;
        uint32_t    master1     : 1;
        uint32_t    master2     : 1;
        uint32_t    reserved    : 29;
    } b;

    uint32_t    d32;
} dma_config_t;

typedef dma_config_t dma_v151_cfg_reg_data_t;

/**
 * @brief
 */
typedef union {
    struct {
        uint32_t    damc_sync  : 16;
        uint32_t    reserved   : 16;
    } b;

    uint32_t    d32;
} dma_sync_t;

/**
 * @brief Channel linked list register
 */
typedef union {
    struct {
        uint32_t    dmac_lm   : 1;
        uint32_t    reserved  : 1;
        uint32_t    dmac_lli  : 30;
    } b;

    uint32_t    d32;
} dma_lli_t;

/**
 * @brief Channel Configuration Register
 */
typedef union {
    struct {
        uint32_t    ch_enable     : 1;
        uint32_t    src_per       : 4;
        uint32_t    dest_per      : 4;
        uint32_t    fc_tt         : 3;
        uint32_t    int_err_mask  : 1;
        uint32_t    tc_int_mask   : 1;
        uint32_t    lock          : 1;
        uint32_t    active        : 1;
        uint32_t    halt          : 1;
        uint32_t    reserved      : 15;
    } b;

    uint32_t    d32;
} dma_chn_config_t;

typedef dma_chn_config_t dma_cfg_data_t;

/**
 * @brief Channel Control Register
 */
typedef union {
    struct {
        uint32_t    transfersize  : 12;
        uint32_t    sbsize        : 3;
        uint32_t    dbsize        : 3;
        uint32_t    swsize        : 3;
        uint32_t    dwsize        : 3;
        uint32_t    src_ms_sel    : 1;
        uint32_t    dest_ms_sel   : 1;
        uint32_t    src_inc       : 1;
        uint32_t    dest_inc      : 1;
        uint32_t    protection    : 3;
        uint32_t    tc_int_en     : 1;
    } b;

    uint32_t    d32;
} dma_chn_control_t;

typedef dma_chn_control_t dma_ctrl_data_t;

#define DMA_REGISTER_PAD_LEN 55

/**
 * @brief DMA Channel configuration.
 */
typedef struct {
    volatile dma_lli_t              lli; /* 00 */
    volatile uint32_t               dest; /* 04 */
    volatile dma_chn_config_t       cfg; /* 08 */
    volatile uint32_t               rsv1; /* 0c */
    volatile uint32_t               src; /* 10 */
    volatile dma_chn_control_t      ctrl; /* 14 */
    volatile uint32_t               rsv2; /* 18 */
    volatile uint32_t               rsv3; /* 1c */
} dma_chn_arg_t;

/**
 * @brief Registers associated with DMA.
 */
typedef struct {
    volatile uint32_t              reserve;
    volatile dma_int_st_t          int_st;
    volatile dma_int_clr_t         int_clr;
    volatile dma_ori_int_st_t      ori_int;
    volatile dma_en_chns_t         en_chns;
    volatile dma_burst_req_t       burst_req;
    volatile dma_single_req_t      single_req;
    volatile dma_config_t          configuration;
    volatile dma_sync_t            sync;
    volatile uint32_t              pad[DMA_REGISTER_PAD_LEN]; /* 55*4 */
    volatile dma_chn_arg_t ch_config[B_DMA_CHANNEL_MAX_NUM];
} dma_v151_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif