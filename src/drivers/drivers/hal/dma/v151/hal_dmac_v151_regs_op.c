/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V151 dma register operation api \n
 *
 * History: \n
 * 2023-2-25， Create file. \n
 */
#include "hal_dmac_v151_regs_op.h"
#include "debug_print.h"
#include "debug_print.h"

#define DMAC_INT_TC_ST_MASK 0xff00
#define DMAC_INT_TC_ST_BITS 8
#define DMAC_INT_ERR_ST_MASK 0xff0000
#define DMAC_INT_ERR_ST_BITS 16
#define INT_ERR_CLR_BITS 8

static void hal_dma_reg_ch_set(dma_channel_t channel, dam_ch_int_data_t *ch_data, uint32_t val)
{
    switch (channel) {
        case DMA_CHANNEL_0:
            ch_data->b.ch_0 = val;
            break;
        case DMA_CHANNEL_1:
            ch_data->b.ch_1 = val;
            break;
        case DMA_CHANNEL_2:
            ch_data->b.ch_2 = val;
            break;
        case DMA_CHANNEL_3:
            ch_data->b.ch_3 = val;
            break;
        case DMA_CHANNEL_4:
            ch_data->b.ch_4 = val;
            break;
        case DMA_CHANNEL_5:
            ch_data->b.ch_5 = val;
            break;
        case DMA_CHANNEL_6:
            ch_data->b.ch_6 = val;
            break;
        case DMA_CHANNEL_7:
            ch_data->b.ch_7 = val;
            break;
        default:
            break;
    }
}

static uint32_t hal_dma_reg_ch_get(dma_channel_t channel, dam_ch_int_data_t ch_data)
{
    switch (channel) {
        case DMA_CHANNEL_0:
            return ch_data.b.ch_0;
        case DMA_CHANNEL_1:
            return ch_data.b.ch_1;
        case DMA_CHANNEL_2:
            return ch_data.b.ch_2;
        case DMA_CHANNEL_3:
            return ch_data.b.ch_3;
        case DMA_CHANNEL_4:
            return ch_data.b.ch_4;
        case DMA_CHANNEL_5:
            return ch_data.b.ch_5;
        case DMA_CHANNEL_6:
            return ch_data.b.ch_6;
        case DMA_CHANNEL_7:
            return ch_data.b.ch_7;
        default:
            return 0;
    }
}

bool hal_dma_regs_data_get_int_tc_st(dma_channel_t channel, dma_v151_regs_t *dma_regs)
{
    dam_ch_int_data_t ch_data;
    uint32_t int_st_data = (((dma_v151_regs_t *)dma_regs)->int_st.d32) & DMAC_INT_TC_ST_MASK;
    ch_data.d32 = (int_st_data >> DMAC_INT_TC_ST_BITS);
    return (bool)hal_dma_reg_ch_get(channel, ch_data);
}

bool hal_dma_regs_data_get_int_err_st(dma_channel_t channel, dma_v151_regs_t *dma_regs)
{
    dam_ch_int_data_t ch_data;
    uint32_t int_st_data = (((dma_v151_regs_t *)dma_regs)->int_st.d32) & DMAC_INT_ERR_ST_MASK;
    ch_data.d32 = (int_st_data >> DMAC_INT_ERR_ST_BITS);
    return (bool)hal_dma_reg_ch_get(channel, ch_data);
}

void hal_dma_interrupt_clear_tc(dma_channel_t channel, dma_v151_regs_t *dma_regs)
{
    dam_ch_int_data_t ch_data;
    ch_data.d32 = 0;
    uint32_t int_trans_clr_data = ((dma_v151_regs_t *)dma_regs)->int_clr.d32;
    hal_dma_reg_ch_set(channel, &ch_data, 1);
    int_trans_clr_data = (int_trans_clr_data | ch_data.d32);
    ((dma_v151_regs_t *)dma_regs)->int_clr.d32 = int_trans_clr_data;
}

void hal_dma_interrupt_clear_err(dma_channel_t channel, dma_v151_regs_t *dma_regs)
{
    dam_ch_int_data_t ch_data;
    ch_data.d32 = 0;
    uint32_t int_trans_clr_data = ((dma_v151_regs_t *)dma_regs)->int_clr.d32;
    hal_dma_reg_ch_set(channel, &ch_data, 1);
    uint32_t cur_data = (ch_data.d32 << INT_ERR_CLR_BITS);
    int_trans_clr_data = (int_trans_clr_data | cur_data);
    ((dma_v151_regs_t *)dma_regs)->int_clr.d32 = int_trans_clr_data;
}

bool hal_dma_regs_data_get_ch_en(dma_channel_t channel, dma_v151_regs_t *dma_regs)
{
    dam_ch_int_data_t ch_data;
    ch_data.d32 = (((dma_v151_regs_t *)dma_regs)->en_chns.d32) & 0xff;
    return (bool)hal_dma_reg_ch_get(channel, ch_data);
}