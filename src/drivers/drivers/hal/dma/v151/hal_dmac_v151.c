/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V151 HAL dma \n
 *
 * History: \n
 * 2023-2-25， Create file. \n
 */
#include "securec.h"
#include "dma_porting.h"
#include "hal_dmac_v151_regs_op.h"
#include "hal_dmac_v151.h"
#ifdef CONFIG_SUPPORT_DATA_CACHE
#include "osal_adapt.h"
#endif
#include "hal_dma_mem.h"

#define HAL_DMA_CH_MAX_TRANSFER_NUM 4096

void hal_dma_v151_close(void);
bool hal_dma_v151_is_enabled(dma_channel_t ch);

/* DMA ch informations */
typedef struct hal_dma_ch_info {
    hal_dma_transfer_cb_t isr_callback;          /* ch ISR callback functions */
    uintptr_t arg;                               /* ch user arg */
    hal_dma_ch_state_t state;                    /* ch transferring state */
} hal_dma_ch_info_t;

/* DMA transfer link list buffer. */
typedef struct hal_dma_lli {
    uint32_t src_addr;
    uint32_t dst_addr;
    struct hal_dma_lli *next;
    dma_ctrl_data_t ctrl;
} hal_dma_lli_t;

static hal_dma_ch_info_t g_hal_dma_channel[DMA_CHANNEL_MAX_NUM];

static hal_dma_lli_t *g_dma_node_cfg[DMA_CHANNEL_MAX_NUM] = { NULL };
static hal_dma_lli_t *g_dma_lli_add[DMA_CHANNEL_MAX_NUM] = { NULL };

#if defined(CONFIG_DMA_SUPPORT_SMDMA)
/* pay attention please, the S_DMA_CHANNEL_MAX_NUM shouble keep same with g_sdma_burst_len_limit_ch size. */
static uint8_t g_sdma_burst_len_limit_ch[] = { HAL_DMA_BURST_TRANSACTION_LENGTH_4,
                                               HAL_DMA_BURST_TRANSACTION_LENGTH_4,
                                               HAL_DMA_BURST_TRANSACTION_LENGTH_8,
                                               HAL_DMA_BURST_TRANSACTION_LENGTH_32 };
#endif /* CONFIG_DMA_SUPPORT_SMDMA */

static dma_v151_regs_t *hal_dma_type_get(dma_channel_t ch)
{
    unused(ch);
#if defined(CONFIG_DMA_SUPPORT_SMDMA)
    if (ch >= B_DMA_CHANNEL_MAX_NUM) {
        return (dma_v151_regs_t *)g_sdma_regs;
    }
#endif /* CONFIG_DMA_SUPPORT_SMDMA */
    return (dma_v151_regs_t *)g_dma_regs;
}

static dma_channel_t hal_dma_ch_get(dma_channel_t ch)
{
    return (dma_channel_t)((uint32_t)ch % B_DMA_CHANNEL_MAX_NUM);
}

/* Get dma real channel num. */
static dma_channel_t hal_dma_get_real_channel_num(hal_dma_type_t dma_type, dma_channel_t ch)
{
    return (dma_channel_t)((dma_type == HAL_DMA_TYPE_SDMA) ? ((uint8_t)ch + B_DMA_CHANNEL_MAX_NUM) : ((uint8_t)ch));
}

/* Specific configuration to enable the DMA controller. */
static void hal_dma_enable_controller(void)
{
    hal_dma_cfg_set_en((uint32_t)1, (dma_v151_regs_t *)g_dma_regs);
#if defined(CONFIG_DMA_SUPPORT_SMDMA)
    hal_dma_cfg_set_en((uint32_t)1, (dma_v151_regs_t *)g_sdma_regs);
#endif /* CONFIG_DMA_SUPPORT_SMDMA */
}

/* Specific configuration to disable the DMA controller. */
static void hal_dma_disable_controller(void)
{
    hal_dma_cfg_set_en((uint32_t)0, (dma_v151_regs_t *)g_dma_regs);
#if defined(CONFIG_DMA_SUPPORT_SMDMA)
    hal_dma_cfg_set_en((uint32_t)0, (dma_v151_regs_t *)g_sdma_regs);
#endif /* CONFIG_DMA_SUPPORT_SMDMA */
}

static void hal_dma_reset(void)
{
    for (uint8_t ch = DMA_CHANNEL_0; ch < B_DMA_CHANNEL_MAX_NUM; ch++) {
        hal_dma_ch_cfg_set_ch_en(ch, (uint32_t)0, (dma_v151_regs_t *)g_dma_regs);
        g_hal_dma_channel[ch].state = HAL_DMA_CH_STATE_CLOSED;
    }
#if defined(CONFIG_DMA_SUPPORT_SMDMA)
    for (uint8_t ch = 0; ch < S_DMA_CHANNEL_MAX_NUM; ch++) {
        hal_dma_ch_cfg_set_ch_en(ch, (uint32_t)0, (dma_v151_regs_t *)g_sdma_regs);
        g_hal_dma_channel[ch + B_DMA_CHANNEL_MAX_NUM].state = HAL_DMA_CH_STATE_CLOSED;
    }
#endif /* CONFIG_DMA_SUPPORT_SMDMA */
    hal_dma_disable_controller();
    hal_dma_enable_controller();
}

#pragma weak hal_dma_init = hal_dma_v151_init
errcode_t hal_dma_v151_init(void)
{
    if (hal_dma_regs_init() != ERRCODE_SUCC) {
        return ERRCODE_DMA_REG_ADDR_INVALID;
    }

    hal_dma_reset();
    return hal_dma_mem_init(sizeof(hal_dma_lli_t));
}

#pragma weak hal_dma_deinit = hal_dma_v151_deinit
void hal_dma_v151_deinit(void)
{
    hal_dma_v151_close();
    hal_dma_mem_deinit();
}

#pragma weak hal_dma_open = hal_dma_v151_open
void hal_dma_v151_open(void)
{
    hal_dma_enable_controller();

    for (uint8_t ch = DMA_CHANNEL_0; ch < DMA_CHANNEL_MAX_NUM; ch++) {
        g_hal_dma_channel[ch].state = HAL_DMA_CH_STATE_CLOSED;
    }
}

#pragma weak hal_dma_close = hal_dma_v151_close
void hal_dma_v151_close(void)
{
    hal_dma_disable_controller();
}

static void hal_dma_lli_free(dma_channel_t ch)
{
    hal_dma_lli_t *dma_p = g_dma_node_cfg[ch];
    hal_dma_lli_t *dma_q;
    while (dma_p != NULL) {
        dma_q = dma_p->next;
        hal_dma_mem_free(ch, dma_p);
#if defined(CONFIG_DMA_SUPPORT_CIRCULAR_LLI)
        if (dma_q == g_dma_node_cfg[ch]) {
            break;
        }
#endif
        dma_p = dma_q;
    }
    g_dma_node_cfg[ch] = NULL;
}

#pragma weak hal_dma_enable = hal_dma_v151_enable
void hal_dma_v151_enable(dma_channel_t ch, bool en)
{
    dma_v151_regs_t *tmp_dma_regs = hal_dma_type_get(ch);
    dma_channel_t tmp_dma_ch = hal_dma_ch_get(ch);

    if (en) {
#ifdef CONFIG_SUPPORT_DATA_CACHE
        osal_dcache_flush_all();
#endif
        dma_port_add_sleep_veto();
        hal_dma_ch_cfg_set_ch_en(tmp_dma_ch, (uint32_t)1, tmp_dma_regs);
    } else {
        hal_dma_ch_cfg_set_ch_en(tmp_dma_ch, (uint32_t)0, tmp_dma_regs);
        dma_port_release_handshaking_source(ch);
        g_hal_dma_channel[ch].state = HAL_DMA_CH_STATE_CLOSED;
        hal_dma_lli_free(ch);
    }
}

#pragma weak hal_dma_get_block = hal_dma_v151_get_block
uint32_t hal_dma_v151_get_block(dma_channel_t ch)
{
    dma_v151_regs_t *tmp_dma_regs = hal_dma_type_get(ch);
    dma_channel_t tmp_dma_ch = hal_dma_ch_get(ch);
    return hal_dma_ctrl_get_tranf_size(tmp_dma_ch, tmp_dma_regs);
}

#pragma weak hal_dma_interrupt_clear = hal_dma_v151_interrupt_clear
void hal_dma_v151_interrupt_clear(dma_channel_t ch, hal_dma_interrupt_t int_type)
{
    dma_v151_regs_t *tmp_dma_regs = hal_dma_type_get(ch);
    dma_channel_t tmp_dma_ch = hal_dma_ch_get(ch);
    if (int_type == HAL_DMA_INTERRUPT_TFR) {
        hal_dma_interrupt_clear_tc(tmp_dma_ch, tmp_dma_regs);
    } else {
        hal_dma_interrupt_clear_err(tmp_dma_ch, tmp_dma_regs);
    }
}

#if defined(CONFIG_DMA_SUPPORT_SMDMA)
static dma_channel_t hal_dma_get_sdma_start_ch(hal_dma_burst_transaction_length_t burst_length)
{
    for (uint32_t i = 0; i < sizeof(g_sdma_burst_len_limit_ch); i++) {
        if (burst_length <= g_sdma_burst_len_limit_ch[i]) {
            return (dma_channel_t)(DMA_CHANNEL_8 + i);
        }
    }
    return (dma_channel_t)DMA_CHANNEL_MAX_NUM;
}
#endif /* CONFIG_DMA_SUPPORT_SMDMA */

#pragma weak hal_dma_get_idle_ch = hal_dma_v151_get_idle_ch
dma_channel_t hal_dma_v151_get_idle_ch(hal_dma_handshaking_source_t source,
                                       hal_dma_burst_transaction_length_t burst_length)
{
    uint8_t ch;
    uint8_t max_ch_num = DMA_CHANNEL_MAX_NUM;
    dma_channel_t start_ch_num = DMA_CHANNEL_0;

#if defined(CONFIG_DMA_SUPPORT_SMDMA)
    if (source < HAL_DMA_HANDSHAKING_MAX_NUM) {
        if (source >= HAL_DMA_HANDSHAKING_SDMA) {
            max_ch_num = DMA_CHANNEL_MAX_NUM;
            start_ch_num = hal_dma_get_sdma_start_ch(burst_length);
        } else {
            max_ch_num = B_DMA_CHANNEL_MAX_NUM;
            start_ch_num = DMA_CHANNEL_0;
        }
    }
#else
    unused(burst_length);
    unused(source);
#endif /* CONFIG_DMA_SUPPORT_SMDMA */
    for (ch = start_ch_num; ch < max_ch_num; ch++) {
        if (!hal_dma_v151_is_enabled((dma_channel_t)ch) &&
            (g_hal_dma_channel[ch].state == HAL_DMA_CH_STATE_CLOSED)) {
            g_hal_dma_channel[ch].state = HAL_DMA_CH_STATE_ACTIVE;
            return (dma_channel_t)ch;
        }
    }
    return DMA_CHANNEL_NONE;
}

static errcode_t hal_dma_config_periph(dma_channel_t ch, hal_dma_transfer_peri_config_t *periph_cfg)
{
    if (dma_port_set_mux_channel(ch, periph_cfg) != ERRCODE_SUCC) {
        return ERRCODE_DMA_RET_HANDSHAKING_USING;
    }
    dma_v151_regs_t *tmp_dma_regs = hal_dma_type_get(ch);
    dma_channel_t tmp_dma_ch = hal_dma_ch_get(ch);
    hal_dma_cfg_set_src_per(tmp_dma_ch, (uint32_t)periph_cfg->hs_source, tmp_dma_regs);
    hal_dma_cfg_set_dest_per(tmp_dma_ch, (uint32_t)periph_cfg->hs_dest, tmp_dma_regs);
    hal_dma_ctrl_set_protection(tmp_dma_ch, (uint32_t)periph_cfg->protection, tmp_dma_regs);
    return ERRCODE_SUCC;
}

/* Convert address increment according to the following rules:
 *  increment : 1
 *  others : 0
 */
static inline uint32_t hal_dma_inc_convert(hal_dma_address_inc_t inc_data)
{
    if (inc_data == HAL_DMA_ADDRESS_INC_INCREMENT) {
        return 1;
    }
    return 0;
}

static void hal_dma_set_base_cfg_single_block(dma_channel_t ch, dma_v151_regs_t *dma_regs,
                                              const hal_dma_transfer_base_config_t *base_cfg)
{
    /* Transfer src addr and dest addr */
    ((dma_v151_regs_t *)dma_regs)->ch_config[ch].src = base_cfg->src;
    ((dma_v151_regs_t *)dma_regs)->ch_config[ch].dest = base_cfg->dest;
    /* Transfer width */
    hal_dma_ctrl_set_src_tr_width(ch, (uint32_t)base_cfg->src_width, dma_regs);
    hal_dma_ctrl_set_dst_tr_width(ch, (uint32_t)base_cfg->dest_width, dma_regs);

    /* Address increment mode */
    hal_dma_ctrl_set_dest_inc(ch, hal_dma_inc_convert(base_cfg->dest_inc), dma_regs);
    hal_dma_ctrl_set_src_inc(ch, hal_dma_inc_convert(base_cfg->src_inc), dma_regs);

    /* Burst Transaction Length */
    hal_dma_ctrl_set_dest_bsize(ch, (uint32_t)base_cfg->dest_burst_trans_length, dma_regs);
    hal_dma_ctrl_set_src_bsize(ch, (uint32_t)base_cfg->src_burst_trans_length, dma_regs);

    hal_dma_cfg_set_lock(ch, (uint32_t)1, dma_regs);
}

#pragma weak hal_dma_config_single_block = hal_dma_v151_config_single_block
errcode_t hal_dma_v151_config_single_block(dma_channel_t ch,
                                           const hal_dma_transfer_base_config_t *base_cfg,
                                           hal_dma_transfer_peri_config_t *periph_cfg)
{
    dma_v151_regs_t *tmp_dma_regs = hal_dma_type_get(ch);
    dma_channel_t tmp_dma_ch = hal_dma_ch_get(ch);
    if (base_cfg->transfer_num >= HAL_DMA_CH_MAX_TRANSFER_NUM) {
        return ERRCODE_DMA_RET_TOO_MANY_DATA_TO_TRANSFER;
    }

    if (hal_dma_v151_is_enabled(ch)) {
        return ERRCODE_DMA_CH_BUSY;
    }
    hal_dma_set_base_cfg_single_block(tmp_dma_ch, tmp_dma_regs, base_cfg);

    /* master: only used in mdma/smdma */
    hal_dma_ctrl_set_dest_ms_sel(tmp_dma_ch, (uint32_t)dma_port_get_master_select(ch, base_cfg->dest), tmp_dma_regs);
    hal_dma_ctrl_set_src_ms_sel(tmp_dma_ch, (uint32_t)dma_port_get_master_select(ch, base_cfg->src), tmp_dma_regs);

    if (base_cfg->callback != NULL) {
        hal_dma_ctrl_set_tc_int_en(tmp_dma_ch, (uint32_t)1, tmp_dma_regs);
    }

    if (periph_cfg != NULL) {
        hal_dma_cfg_set_fc_tt(tmp_dma_ch, (uint32_t)periph_cfg->trans_type, tmp_dma_regs);
        if (hal_dma_config_periph(ch, periph_cfg) != ERRCODE_SUCC) {
            return ERRCODE_DMA_RET_HANDSHAKING_USING;
        }
    } else {
        hal_dma_cfg_set_fc_tt(tmp_dma_ch, (uint32_t)HAL_DMA_TRANS_MEMORY_TO_MEMORY_DMA, tmp_dma_regs);
    }

    /* Register callback or clear it */
    g_hal_dma_channel[ch].isr_callback = base_cfg->callback;
    g_hal_dma_channel[ch].arg = base_cfg->priv_arg;

    hal_dma_cfg_set_int_err_mask(tmp_dma_ch, (uint32_t)1, tmp_dma_regs);
    hal_dma_cfg_set_tc_int_mask(tmp_dma_ch, (uint32_t)1, tmp_dma_regs);

    /* Transfer length */
    hal_dma_ctrl_set_tranf_size(tmp_dma_ch, (uint32_t)base_cfg->transfer_num, tmp_dma_regs);

    return ERRCODE_SUCC;
}

static void hal_dma_set_base_cfg_lli(dma_channel_t ch, const hal_dma_transfer_base_config_t *base_cfg)
{
    g_dma_lli_add[ch]->src_addr = base_cfg->src;
    g_dma_lli_add[ch]->dst_addr = base_cfg->dest;

    g_dma_lli_add[ch]->ctrl.b.dwsize = base_cfg->dest_width;
    g_dma_lli_add[ch]->ctrl.b.swsize = base_cfg->src_width;

    /* Address increment mode. */
    g_dma_lli_add[ch]->ctrl.b.dest_inc = hal_dma_inc_convert(base_cfg->dest_inc);
    g_dma_lli_add[ch]->ctrl.b.src_inc = hal_dma_inc_convert(base_cfg->src_inc);

    /* Burst Transaction Length. */
    g_dma_lli_add[ch]->ctrl.b.dbsize = base_cfg->dest_burst_trans_length;
    g_dma_lli_add[ch]->ctrl.b.sbsize = base_cfg->src_burst_trans_length;

    /* Master select. */
    g_dma_lli_add[ch]->ctrl.b.dest_ms_sel = (uint32_t)dma_port_get_master_select(ch, base_cfg->dest);
    g_dma_lli_add[ch]->ctrl.b.src_ms_sel = (uint32_t)dma_port_get_master_select(ch, base_cfg->src);

    /* Transfer length. */
    g_dma_lli_add[ch]->ctrl.b.transfersize = base_cfg->transfer_num;
}

static errcode_t hal_dma_lli_get(dma_channel_t ch)
{
    if (g_dma_node_cfg[ch] == NULL) {
        g_dma_node_cfg[ch] = (hal_dma_lli_t *)hal_dma_mem_alloc(ch, sizeof(hal_dma_lli_t));
        if (g_dma_node_cfg[ch] == NULL) {
            return ERRCODE_MALLOC;
        }
        g_dma_lli_add[ch] = g_dma_node_cfg[ch];
        g_dma_lli_add[ch]->next = NULL;
    } else {
        /* llp enable */
        g_dma_lli_add[ch]->next = (hal_dma_lli_t *)hal_dma_mem_alloc(ch, sizeof(hal_dma_lli_t));
        if (g_dma_lli_add[ch]->next == NULL) {
            return ERRCODE_MALLOC;
        }
        g_dma_lli_add[ch] = g_dma_lli_add[ch]->next;
        g_dma_lli_add[ch]->next = NULL;
    }
    return ERRCODE_SUCC;
}

#pragma weak hal_dma_add_lli_transfer = hal_dma_v151_add_lli_transfer
errcode_t hal_dma_v151_add_lli_transfer(dma_channel_t ch,
                                        const hal_dma_transfer_base_config_t *base_cfg,
                                        hal_dma_transfer_peri_config_t *periph_cfg)
{
    bool dma_flag = false;

    if (g_dma_node_cfg[ch] == NULL) {
        dma_flag = true;
    }

    if (hal_dma_lli_get(ch) != ERRCODE_SUCC) {
        return ERRCODE_MALLOC;
    }

    dma_v151_regs_t *tmp_dma_regs = hal_dma_type_get(ch);
    dma_channel_t tmp_dma_ch = hal_dma_ch_get(ch);
    (void)memset_s((void *)g_dma_lli_add[ch], sizeof(hal_dma_lli_t), 0, sizeof(hal_dma_lli_t));

    hal_dma_set_base_cfg_lli(ch, base_cfg);

#if defined(CONFIG_DMA_SUPPORT_CIRCULAR_LLI)
    g_dma_lli_add[ch]->ctrl.b.tc_int_en = 1;
#endif

    if (dma_flag) {
        if (periph_cfg != NULL) {
            hal_dma_cfg_set_fc_tt(tmp_dma_ch, (uint32_t)periph_cfg->trans_type, tmp_dma_regs);
            if (hal_dma_config_periph(ch, periph_cfg) != ERRCODE_SUCC) {
                return ERRCODE_DMA_RET_HANDSHAKING_USING;
            }
        } else {
            hal_dma_cfg_set_fc_tt(ch, (uint32_t)HAL_DMA_TRANS_MEMORY_TO_MEMORY_DMA, tmp_dma_regs);
        }
        hal_dma_cfg_set_int_err_mask(tmp_dma_ch, (uint32_t)1, tmp_dma_regs);
        hal_dma_cfg_set_tc_int_mask(tmp_dma_ch, (uint32_t)1, tmp_dma_regs);
        hal_dma_cfg_set_lock(tmp_dma_ch, (uint32_t)1, tmp_dma_regs);
    }

    return ERRCODE_SUCC;
}

#pragma weak hal_dma_is_enabled = hal_dma_v151_is_enabled
bool hal_dma_v151_is_enabled(dma_channel_t ch)
{
    dma_v151_regs_t *tmp_dma_regs = hal_dma_type_get(ch);
    dma_channel_t tmp_dma_ch = hal_dma_ch_get(ch);
    return hal_dma_regs_data_get_ch_en(tmp_dma_ch, tmp_dma_regs);
}

#pragma weak hal_dma_enable_lli = hal_dma_v151_enable_lli
void hal_dma_v151_enable_lli(dma_channel_t ch, hal_dma_transfer_cb_t callback, uintptr_t arg)
{
    if (g_dma_node_cfg[ch] == NULL) { return; }
    /* Interrupt enable. */
    dma_v151_regs_t *tmp_dma_regs = hal_dma_type_get(ch);
    dma_channel_t tmp_dma_ch = hal_dma_ch_get(ch);
    g_dma_lli_add[ch]->ctrl.b.tc_int_en = 1;
#if defined(CONFIG_DMA_SUPPORT_CIRCULAR_LLI)
    g_dma_lli_add[ch]->next = g_dma_node_cfg[ch];
#endif
    ((dma_v151_regs_t *)tmp_dma_regs)->ch_config[tmp_dma_ch].src = g_dma_node_cfg[ch]->src_addr;
    ((dma_v151_regs_t *)tmp_dma_regs)->ch_config[tmp_dma_ch].dest = g_dma_node_cfg[ch]->dst_addr;

    ((dma_v151_regs_t *)tmp_dma_regs)->ch_config[tmp_dma_ch].ctrl = g_dma_node_cfg[ch]->ctrl;

    ((dma_v151_regs_t *)tmp_dma_regs)->ch_config[tmp_dma_ch].lli.d32 = (uint32_t)(uintptr_t)(g_dma_node_cfg[ch]->next);

    g_hal_dma_channel[ch].isr_callback = callback;
    g_hal_dma_channel[ch].arg = arg;

    hal_dma_v151_enable(ch, true);
}

#if defined(CONFIG_DMA_SUPPORT_SMDMA)
/* Get mdma/smdma channel max num. */
static uint8_t hal_dma_get_channel_max_num(hal_dma_type_t dma_type)
{
    return ((dma_type == HAL_DMA_TYPE_SDMA) ? (S_DMA_CHANNEL_MAX_NUM) : (B_DMA_CHANNEL_MAX_NUM));
}
#endif /* CONFIG_DMA_SUPPORT_SMDMA */

/* Transfer Complete Interrupt. */
static void hal_dma_tc_isr(hal_dma_type_t dma_type)
{
    dma_channel_t real_ch = DMA_CHANNEL_NONE;
    dma_v151_regs_t *tmp_dma_regs = (dma_v151_regs_t *)g_dma_regs;
    uint8_t ch_max = DMA_CHANNEL_MAX_NUM;
#if defined(CONFIG_DMA_SUPPORT_SMDMA)
    ch_max = hal_dma_get_channel_max_num(dma_type);
    if (dma_type == HAL_DMA_TYPE_BDMA) {
        tmp_dma_regs = (dma_v151_regs_t *)g_dma_regs;
    } else {
        tmp_dma_regs = (dma_v151_regs_t *)g_sdma_regs;
    }
#endif /* CONFIG_DMA_SUPPORT_SMDMA */

    for (uint8_t ch = DMA_CHANNEL_0; ch < ch_max; ch++) {
        if (hal_dma_regs_data_get_int_tc_st(ch, tmp_dma_regs)) {
            real_ch = hal_dma_get_real_channel_num(dma_type, ch);
            hal_dma_interrupt_clear_tc(ch, tmp_dma_regs);
            dma_port_release_handshaking_source(real_ch);
            g_hal_dma_channel[real_ch].state = HAL_DMA_CH_STATE_CLOSED;
            if (g_hal_dma_channel[real_ch].isr_callback != NULL) {
                g_hal_dma_channel[real_ch].isr_callback(HAL_DMA_INTERRUPT_TFR, real_ch, g_hal_dma_channel[real_ch].arg);
            }
        }
    }
}

/* DMA Error Interrupt. */
static void hal_dma_err_isr(hal_dma_type_t dma_type)
{
    dma_channel_t real_ch = DMA_CHANNEL_NONE;
    dma_v151_regs_t *tmp_dma_regs = (dma_v151_regs_t *)g_dma_regs;
    uint8_t ch_max = DMA_CHANNEL_MAX_NUM;
#if defined(CONFIG_DMA_SUPPORT_SMDMA)
    ch_max = hal_dma_get_channel_max_num(dma_type);
    if (dma_type == HAL_DMA_TYPE_BDMA) {
        tmp_dma_regs = (dma_v151_regs_t *)g_dma_regs;
    } else {
        tmp_dma_regs = (dma_v151_regs_t *)g_sdma_regs;
    }
#endif /* CONFIG_DMA_SUPPORT_SMDMA */

    for (uint8_t ch = DMA_CHANNEL_0; ch < ch_max; ch++) {
        if (hal_dma_regs_data_get_int_err_st(ch, tmp_dma_regs)) {
            real_ch = hal_dma_get_real_channel_num(dma_type, ch);
            hal_dma_interrupt_clear_err(ch, tmp_dma_regs);
            dma_port_release_handshaking_source(real_ch);
            g_hal_dma_channel[real_ch].state = HAL_DMA_CH_STATE_CLOSED;
            if (g_hal_dma_channel[real_ch].isr_callback != NULL) {
                g_hal_dma_channel[real_ch].isr_callback(HAL_DMA_INTERRUPT_ERR, ch, g_hal_dma_channel[real_ch].arg);
            }
        }
    }
}

void hal_dma_v151_irq_handler(void)
{
    /* Transfer Complete. */
    hal_dma_tc_isr(HAL_DMA_TYPE_BDMA);

    /* Error. */
    hal_dma_err_isr(HAL_DMA_TYPE_BDMA);

    /* remove pm veto */
    dma_port_remove_sleep_veto();
}

#if defined(CONFIG_DMA_SUPPORT_SMDMA)
void hal_sdma_v151_irq_handler(void)
{
    /* Transfer Complete. */
    hal_dma_tc_isr(HAL_DMA_TYPE_SDMA);

    /* Error. */
    hal_dma_err_isr(HAL_DMA_TYPE_SDMA);

    /* remove pm veto */
    dma_port_remove_sleep_veto();
}
#endif /* CONFIG_DMA_SUPPORT_SMDMA */

#if defined (CONFIG_DMA_SUPPORT_LPM)
#pragma weak hal_dma_suspend = hal_dma_v151_suspend
errcode_t hal_dma_v151_suspend(uintptr_t arg)
{
    unused(arg);
    return ERRCODE_SUCC;
}

#pragma weak hal_dma_resume = hal_dma_v151_resume
errcode_t hal_dma_v151_resume(uintptr_t arg)
{
    unused(arg);
    hal_dma_reset();
    return ERRCODE_SUCC;
}
#endif /* CONFIG_DMA_SUPPORT_LPM */