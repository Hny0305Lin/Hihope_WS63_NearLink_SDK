/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides dma port \n
 *
 * History: \n
 * 2022-10-16， Create file. \n
 */

#include "common_def.h"
#include "interrupt/osal_interrupt.h"
#include "platform_core.h"
#include "chip_core_irq.h"
#include "hal_dmac_v151.h"
#include "dma_porting.h"

uintptr_t g_dma_base_addr =   (uintptr_t)DMA_BASE_ADDR;
uint8_t chn_sreq_info[DMA_CHANNEL_MAX_NUM];
uint8_t chn_dreq_info[DMA_CHANNEL_MAX_NUM];

static uint32_t g_dma_handshaking_channel_status = 0;

static void irq_dma_handler(void)
{
    hal_dma_v151_irq_handler();
}

#define DMA_CLK_AUTO_CTRL_REG 0x44000244
#define DMA_CLK_ON_MASK 0x80000
void dma_port_register_irq(void)
{
    // BYPASS dma自动门控
    uint32_t val;
    uapi_reg_read32(DMA_CLK_AUTO_CTRL_REG, val);
    val |= DMA_CLK_ON_MASK;
    uapi_reg_write32(DMA_CLK_AUTO_CTRL_REG, val);

    osal_irq_request(DMA_IRQN, (osal_irq_handler)irq_dma_handler, NULL, NULL, NULL);
    osal_irq_enable(DMA_IRQN);
}

void dma_port_unregister_irq(void)
{
    osal_irq_disable(DMA_IRQN);
    osal_irq_free(DMA_IRQN, NULL);
}

static hal_dma_mux_handshaking_status_t dma_port_get_handshaking_channel_status(hal_dma_handshaking_source_t channel)
{
    uint32_t status = 0;
    if (channel >= HAL_DMA_HANDSHAKING_MAX_NUM) {
        return HAL_DMA_MUX_HANDSHAKING_USING;
    }

    if (channel == HAL_DMA_HANDSHAKING_TIE0) {
        return HAL_DMA_MUX_HANDSHAKING_IDLE;
    }

    status = g_dma_handshaking_channel_status & (1UL<<(uint8_t)channel);
    if (status != 0) {
        return HAL_DMA_MUX_HANDSHAKING_USING;
    } else {
        return HAL_DMA_MUX_HANDSHAKING_IDLE;
    }
}

/* 记录每个soure的状态, 0表示空闲， 1表示busy */
void dma_port_set_handshaking_channel_status(hal_dma_handshaking_source_t channel, bool on)
{
    if (channel >= HAL_DMA_HANDSHAKING_MAX_NUM) {
        return;
    }
    if (on) {
        g_dma_handshaking_channel_status |= (1UL << (uint8_t)channel);
    } else {
        g_dma_handshaking_channel_status &= (uint32_t)(~(1UL << (uint8_t)channel));
    }
}

static void dma_port_set_handshaking_source(dma_channel_t ch, hal_dma_handshaking_source_t source,
    hal_dma_handshaking_source_t dest)
{
    if (ch >= DMA_CHANNEL_MAX_NUM) {
        return;
    }
    chn_sreq_info[ch] = source;
    chn_dreq_info[ch] = dest;
}

void dma_port_release_handshaking_source(dma_channel_t ch)
{
    dma_port_set_handshaking_channel_status(chn_sreq_info[ch], false);
    dma_port_set_handshaking_channel_status(chn_dreq_info[ch], false);
    chn_sreq_info[ch] = 0;
    chn_sreq_info[ch] = 0;
}

errcode_t dma_port_set_mux_channel(dma_channel_t ch, hal_dma_transfer_peri_config_t *per_cfg)
{
    if ((dma_port_get_handshaking_channel_status(per_cfg->hs_source) == HAL_DMA_MUX_HANDSHAKING_USING) ||
        (dma_port_get_handshaking_channel_status(per_cfg->hs_dest) == HAL_DMA_MUX_HANDSHAKING_USING)) {
        return ERRCODE_DMA_RET_HANDSHAKING_USING;
    }

    dma_port_set_handshaking_channel_status(per_cfg->hs_source, true);
    dma_port_set_handshaking_channel_status(per_cfg->hs_dest, true);
    dma_port_set_handshaking_source(ch, per_cfg->hs_source, per_cfg->hs_dest);
    return ERRCODE_SUCC;
}

hal_dma_master_select_t dma_port_get_master_select(dma_channel_t ch, uint32_t addr)
{
    unused(ch);
    unused(addr);
    return HAL_DMA_MASTER_SELECT_0;
}

void dma_port_add_sleep_veto(void)
{
    return;
}

void dma_port_remove_sleep_veto(void)
{
    return;
}
