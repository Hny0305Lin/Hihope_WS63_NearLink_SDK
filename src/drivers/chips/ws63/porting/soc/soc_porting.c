/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides tcxo port \n
 *
 * History: \n
 * 2023-09-14， Create file. \n
 */
#include "soc_porting.h"
#include "chip_io.h"
#include "watchdog_porting.h"
#include "tcxo_porting.h"
#include "uart_porting.h"
#include "tcxo.h"


#define REFCLK_FREQ_STATUS_MASK 0x1
#define HW_CTL              0x40000014
uint32_t get_tcxo_freq(void)
{
    uint32_t hw_ctl;
    hw_ctl = readl(HW_CTL);
    if ((hw_ctl & REFCLK_FREQ_STATUS_MASK) == CLK24M_TCXO) {
        return CLK24M_TCXO;
    } else {
        return CLK40M_TCXO;
    }
}

#define REQ_24M       24000000
#define REQ_40M       40000000
#define USEC_PER_SEC  1000000
void boot_clock_adapt(void)
{
    if (get_tcxo_freq() == CLK24M_TCXO) {
        uart_port_set_clock_value(UART_BUS_0, REQ_24M);
        tcxo_porting_ticks_per_usec_set(REQ_24M / USEC_PER_SEC);
        watchdog_port_set_clock(REQ_24M);
    } else {
        uart_port_set_clock_value(UART_BUS_0, REQ_40M);
        tcxo_porting_ticks_per_usec_set(REQ_40M / USEC_PER_SEC);
        watchdog_port_set_clock(REQ_40M);
    }
}

bool is_tcxo_24mhz(void)
{
    return (get_tcxo_freq() == CLK24M_TCXO);
}

#define CLDO_CRG_CLK_SEL    0x44001134
#define CMU_NEW_CFG1        0x400034A4
#define DELAY_1_US         1
#define CPU_DIV_FLASH_RSTN_SYNC 0x1
#define CPU_DIV_FLASH_RSTN 0x3
void switch_flash_clock_to_pll(void)
{
    writel(CMU_NEW_CFG1, CPU_DIV_FLASH_RSTN_SYNC);
    uapi_tcxo_delay_us(DELAY_1_US);
    writel(CMU_NEW_CFG1, CPU_DIV_FLASH_RSTN);
    reg_setbit(CLDO_CRG_CLK_SEL, 0, POS_18);
}

#define PAD_SFC_CLK_CTRL 0x4400d868
#define PAD_SFC_CSN_CTRL 0x4400d86C
#define PAD_SFC_IO0_CTRL 0x4400d870
#define PAD_SFC_IO1_CTRL 0x4400d874
#define PAD_SFC_IO2_CTRL 0x4400d878
#define PAD_SFC_IO3_CTRL 0x4400d87C
#define SFC_CLK_DS_VALUE 0x3
#define SFC_CSN_DS_VALUE 0x2
#define SFC_DATA_DS_VALUE 0x2
#define DTRL_DS_LEN 0x3
void config_sfc_ctrl_ds(void)
{
    reg_setbits(PAD_SFC_CLK_CTRL, 0, POS_4, DTRL_DS_LEN, SFC_CLK_DS_VALUE);
    reg_setbits(PAD_SFC_CSN_CTRL, 0, POS_4, DTRL_DS_LEN, SFC_CSN_DS_VALUE);
    reg_setbits(PAD_SFC_IO0_CTRL, 0, POS_4, DTRL_DS_LEN, SFC_DATA_DS_VALUE);
    reg_setbits(PAD_SFC_IO1_CTRL, 0, POS_4, DTRL_DS_LEN, SFC_DATA_DS_VALUE);
    reg_setbits(PAD_SFC_IO2_CTRL, 0, POS_4, DTRL_DS_LEN, SFC_DATA_DS_VALUE);
    reg_setbits(PAD_SFC_IO3_CTRL, 0, POS_4, DTRL_DS_LEN, SFC_DATA_DS_VALUE);
}
