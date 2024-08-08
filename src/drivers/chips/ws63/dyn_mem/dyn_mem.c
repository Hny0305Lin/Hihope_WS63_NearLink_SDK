/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description:  dyn mem driver.
 *
 * Create:  2023-04-09
 */

#include <stdint.h>

#include "chip_io.h"
#include "dyn_mem.h"

/**
 * @brief  Definition of SHARE RAM Selections.
 */
typedef enum {
    SHARE_RAM_PKT_RAM = 0,
    SHARE_RAM_BGLE = 1,
    SHARE_RAM_ITCM = 2,
    SHARE_RAM_DTCM = 3,
} pwm_action_t;

/**
 * @brief  This union represents the bit fields in the CFG_RAM_SEL register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_ram_sel {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t cfg_ram12_sel : 1;         /*!< RAM12 (size:2K) configuration:
                                                1'b0:pkt_ram
                                                1'b1:BGLE */
        uint32_t reserved0     : 1;         /*!< reserved. */
        uint32_t cfg_ram11_sel : 2;         /*!< RAM11 (size:16K) configuration
                                                2'b00:pkt_ram
                                                2'b01:BGLE
                                                2'b10:ITCM
                                                2'b11:DTCM */
        uint32_t cfg_ram10_sel : 2;         /*!< RAM10 (size:16K) configuration */
        uint32_t cfg_ram9_sel  : 2;         /*!< RAM9 (size:32K) configuration */
        uint32_t cfg_ram8_sel  : 2;         /*!< RAM8 (size:32K) configuration */
        uint32_t cfg_ram7_sel  : 2;         /*!< RAM7 (size:64K) configuration */
        uint32_t cfg_ram6_sel  : 2;         /*!< RAM6 (size:64K) configuration */
        uint32_t cfg_ram5_sel  : 2;         /*!< RAM5 (size:64K) configuration */
        uint32_t reserved1     : 16;        /*!< reserved. */
    } b;
} cfg_ram_sel_t;

#define OPEN_SHARE_RAM_CLOCK_MASK 0xFFFC00
#define CLOSE_SHARE_RAM_CLOCK_MASK 0xFF0003FF
/**
 * @brief  SHARE RAM configuration.
 */
__attribute__((section(".text.runtime.init"))) void dyn_mem_cfg(void)
{
    uint32_t reg_data;
    reg_data = readl(CFG_RAM_CKEN);
    reg_data = reg_data & CLOSE_SHARE_RAM_CLOCK_MASK;
    writel(CFG_RAM_CKEN, reg_data);
    reg_setbit(BT_EM_GT_MODE, 0, 0);

    cfg_ram_sel_t cfg;
    cfg.d32 = readl(SHARE_RAM_CTL_CFG_RAM_SEL);

    cfg.b.cfg_ram5_sel = SHARE_RAM_PKT_RAM;
    cfg.b.cfg_ram6_sel = SHARE_RAM_PKT_RAM;
    cfg.b.cfg_ram7_sel = SHARE_RAM_PKT_RAM;

#if defined(CONFIG_BGLE_RAM_SIZE_16K) && defined(WIFI_TCM_OPTIMIZE)
    // pkt_ram 512K  itcm: 64K  dtcm: 16K  bgle: 16K
    cfg.b.cfg_ram8_sel = SHARE_RAM_ITCM;
    cfg.b.cfg_ram9_sel = SHARE_RAM_ITCM;
    cfg.b.cfg_ram10_sel = SHARE_RAM_DTCM;
    cfg.b.cfg_ram11_sel = SHARE_RAM_BGLE;

#elif defined(CONFIG_BGLE_RAM_SIZE_16K)
    // pkt_ram 544K  itcm: 16K  dtcm: 32K  bgle: 16K
    cfg.b.cfg_ram8_sel = SHARE_RAM_PKT_RAM;
    cfg.b.cfg_ram9_sel = SHARE_RAM_DTCM;
    cfg.b.cfg_ram10_sel = SHARE_RAM_ITCM;
    cfg.b.cfg_ram11_sel = SHARE_RAM_BGLE;

#elif defined(CONFIG_BGLE_RAM_SIZE_32K)
    // pkt_ram 544K  itcm: 16K  dtcm: 16K  bgle: 32K
    cfg.b.cfg_ram8_sel = SHARE_RAM_PKT_RAM;
    cfg.b.cfg_ram9_sel = SHARE_RAM_BGLE;
    cfg.b.cfg_ram10_sel = SHARE_RAM_DTCM;
    cfg.b.cfg_ram11_sel = SHARE_RAM_ITCM;

#elif defined(CONFIG_BGLE_RAM_SIZE_64K) // btc-only
    // pkt_ram 448K  itcm: 32K  dtcm: 64K  bgle: 64K
    cfg.b.cfg_ram7_sel = SHARE_RAM_DTCM;
    cfg.b.cfg_ram8_sel = SHARE_RAM_BGLE;
    cfg.b.cfg_ram9_sel = SHARE_RAM_BGLE;
    cfg.b.cfg_ram10_sel = SHARE_RAM_ITCM;
    cfg.b.cfg_ram11_sel = SHARE_RAM_ITCM;

#elif defined(WIFI_TCM_OPTIMIZE) // wifi-only
    // pkt_ram 512K  itcm: 64K  dtcm: 32K  bgle: 0K
    cfg.b.cfg_ram8_sel = SHARE_RAM_ITCM;
    cfg.b.cfg_ram9_sel = SHARE_RAM_ITCM;
    cfg.b.cfg_ram10_sel = SHARE_RAM_DTCM;
    cfg.b.cfg_ram11_sel = SHARE_RAM_DTCM;

#else
    // default cfg: pkt_ram 576K  itcm: 16K  dtcm: 16K  bgle: 0K
    cfg.b.cfg_ram8_sel = SHARE_RAM_PKT_RAM;
    cfg.b.cfg_ram9_sel = SHARE_RAM_PKT_RAM;
    cfg.b.cfg_ram10_sel = SHARE_RAM_DTCM;
    cfg.b.cfg_ram11_sel = SHARE_RAM_ITCM;
#endif
    writel(SHARE_RAM_CTL_CFG_RAM_SEL, cfg.d32);

    reg_data = readl(CFG_RAM_CKEN);
    reg_data = reg_data | OPEN_SHARE_RAM_CLOCK_MASK;
    writel(CFG_RAM_CKEN, reg_data);
    reg_clrbit(BT_EM_GT_MODE, 0, 0);
    return;
}

