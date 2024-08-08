/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_soc_rom.h.
 */

#ifndef HAL_SOC_ROM_H
#define HAL_SOC_ROM_H

#include "osal_types.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef BUILD_UT
#define HH503_SSI0_MST_RB_BASE_ADDR (osal_u32)g_rf_ssi0_mst_rb_base_addr
#define HH503_SSI0_MST_RB_RW_REG1_REG          (HH503_SSI0_MST_RB_BASE_ADDR + 0x1A4)
#define HH503_SSI0_MST_RB_SSI_MST_CK_CFG_REG   (HH503_SSI0_MST_RB_BASE_ADDR + 0x4C)       /* SSI0 ctrl */
#define HH503_SSI1_MST_RB_BASE_ADDR (osal_u32)g_rf_ssi0_mst_rb_base_addr + 0x400
#define HH503_SSI1_MST_RB_RW_REG1_REG          (HH503_SSI1_MST_RB_BASE_ADDR + 0x1A4)
#define HH503_SSI1_MST_RB_SSI_MST_CK_CFG_REG   (HH503_SSI1_MST_RB_BASE_ADDR + 0x4C)       /* SSI1 ctrl */
#define HH503_SPI0_0_BASE_ADDR  (osal_u32)g_rf_spi0_base_addr
#define HH503_SPI0_1_BASE_ADDR  (osal_u32)g_rf_spi0_base_addr + 0x200
#define HH503_SPI0_2_BASE_ADDR  (osal_u32)g_rf_spi0_base_addr + 0x400
#define HH503_SHARE_MEM__CTL_ID (osal_u32)g_soc_cfg_share_mem_base_addr
#define HH503_CFG_FREQ_BUSDMAC                 (HH503_SHARE_MEM__CTL_ID + 0x50)

#define HH503_CFG_RX_EN                        (HH503_SHARE_MEM__CTL_ID + 0x58)
#define HH503_CFG_RX_START_ADDR                (HH503_SHARE_MEM__CTL_ID + 0x5c)
#define HH503_CFG_RX_END_ADDR                  (HH503_SHARE_MEM__CTL_ID + 0x60)
#define HH503_CFG_RX_DONE_STS                  (HH503_SHARE_MEM__CTL_ID + 0x64)
#define HH503_CFG_RX_DONE_ADDR                 (HH503_SHARE_MEM__CTL_ID + 0x68)
#define HH503_CFG_WLPHY2PKT_RX_RAM_STS         (HH503_SHARE_MEM__CTL_ID + 0x6c)

#define HH503_CFG_TX_EN                        (HH503_SHARE_MEM__CTL_ID + 0x70)
#define HH503_CFG_TX_START_ADDR                (HH503_SHARE_MEM__CTL_ID + 0x74)
#define HH503_CFG_TX_END_ADDR                  (HH503_SHARE_MEM__CTL_ID + 0x78)
#define HH503_CFG_TX_DONE_STS                  (HH503_SHARE_MEM__CTL_ID + 0x7c)
#define HH503_CFG_TX_DONE_ADDR                 (HH503_SHARE_MEM__CTL_ID + 0x80)
#define HH503_CFG_WLPHY2PKT_TX_RAM_STS         (HH503_SHARE_MEM__CTL_ID + 0x84)
#else
#define HH503_SSI0_MST_RB_BASE_ADDR            0x44030000
#define HH503_SSI0_MST_RB_RW_REG1_REG          (HH503_SSI0_MST_RB_BASE_ADDR + 0x1A4)
#define HH503_SSI0_MST_RB_SSI_MST_CK_CFG_REG   (HH503_SSI0_MST_RB_BASE_ADDR + 0x4C)       /* SSI0 ctrl */
#define HH503_SSI1_MST_RB_BASE_ADDR            (HH503_SSI0_MST_RB_BASE_ADDR + 0x400)
#define HH503_SSI1_MST_RB_RW_REG1_REG          (HH503_SSI1_MST_RB_BASE_ADDR + 0x1A4)
#define HH503_SSI1_MST_RB_SSI_MST_CK_CFG_REG   (HH503_SSI1_MST_RB_BASE_ADDR + 0x4C)       /* SSI1 ctrl */
#define HH503_SPI0_0_BASE_ADDR                 (HH503_SSI0_MST_RB_BASE_ADDR + 0x800) /* WL0 ADDA */
#define HH503_SPI0_1_BASE_ADDR                 (HH503_SSI0_MST_RB_BASE_ADDR + 0xA00) /* WL1 ADDA */
#define HH503_SPI0_2_BASE_ADDR                 (HH503_SSI0_MST_RB_BASE_ADDR + 0xC00) /* ABB5_0 PLL */


#define HH503_SHARE_MEM__CTL_ID                0x44006c00
#define HH503_CFG_FREQ_BUSDMAC                 (HH503_SHARE_MEM__CTL_ID + 0x50)

#define HH503_CFG_RX_EN                        (HH503_SHARE_MEM__CTL_ID + 0x58)
#define HH503_CFG_RX_START_ADDR                (HH503_SHARE_MEM__CTL_ID + 0x5c)
#define HH503_CFG_RX_END_ADDR                  (HH503_SHARE_MEM__CTL_ID + 0x60)
#define HH503_CFG_RX_DONE_STS                  (HH503_SHARE_MEM__CTL_ID + 0x64)
#define HH503_CFG_RX_DONE_ADDR                 (HH503_SHARE_MEM__CTL_ID + 0x68)
#define HH503_CFG_WLPHY2PKT_RX_RAM_STS         (HH503_SHARE_MEM__CTL_ID + 0x6c)

#define HH503_CFG_TX_EN                        (HH503_SHARE_MEM__CTL_ID + 0x70)
#define HH503_CFG_TX_START_ADDR                (HH503_SHARE_MEM__CTL_ID + 0x74)
#define HH503_CFG_TX_END_ADDR                  (HH503_SHARE_MEM__CTL_ID + 0x78)
#define HH503_CFG_TX_DONE_STS                  (HH503_SHARE_MEM__CTL_ID + 0x7c)
#define HH503_CFG_TX_DONE_ADDR                 (HH503_SHARE_MEM__CTL_ID + 0x80)
#define HH503_CFG_WLPHY2PKT_TX_RAM_STS         (HH503_SHARE_MEM__CTL_ID + 0x84)
#endif

typedef enum {
    WITP_SOC_BANK_0 = 0,
    WITP_SOC_RF_W_C0_CTL_1,
    WITP_SOC_RF_W_C1_CTL_2,
    WITP_SOC_W_CTL_3,
    WITP_SOC_COEX_CTL_4,

    WITP_SOC_BANK_BUTT
} hh503_soc_bank_idx_enum;

#ifdef BOARD_ASIC_WIFI
#define HH503_SOC_CLD0_CRG_DIV_CTL10 0x44001130
/* Define the union u_cld0_cgr_div_ctl10 */
typedef union {
    /* Define the struct bits */
    struct {
        osal_u32 tcxo_120m_div1_num      : 3;  /* [0..2] */
        osal_u32 tcxo_120m_load_div_en   : 1;  /* [1] */
        osal_u32 tcxo_120m_div_en        : 1;  /* [1] */
        osal_u32 mac_main_div_num        : 6;  /* [5..10] */
        osal_u32 wtop_div_en             : 1;  /* [11] */
        osal_u32 tcxo_120m_div2_num      : 3;  /* [12..14] */
        osal_u32 reserved                : 17; /* [31..15] */
    } bits;

    /* Define an unsigned member */
    osal_u32 u32;
} u_cld0_cgr_div_ctl10;
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_soc_rom.h */
