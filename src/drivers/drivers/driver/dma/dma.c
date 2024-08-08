/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides dma driver source \n
 *
 * History: \n
 * 2022-10-16， Create file. \n
 */

#include "common_def.h"
#include "hal_dma.h"
#include "dma.h"

#define DMA_ADDR_ALIGN_MASK_2 1
#define DMA_ADDR_ALIGN_MASK_4 3

#define DMA_MEMORY_BURST_TRANS_LENGTH_BYTE (HAL_DMA_BURST_TRANSACTION_LENGTH_16)
#define DMA_MEMORY_BURST_TRANS_LENGTH_HARF (HAL_DMA_BURST_TRANSACTION_LENGTH_8)
#define DMA_MEMORY_BURST_TRANS_LENGTH_WORD (HAL_DMA_BURST_TRANSACTION_LENGTH_4)
#define dma_check_addr_align(addr, align)  (((addr) & (align)) == 0x0)

static bool g_dma_is_initialised = false;

errcode_t uapi_dma_init(void)
{
    if (g_dma_is_initialised) {
        return ERRCODE_SUCC;
    }

    errcode_t ret = hal_dma_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    g_dma_is_initialised = true;

    return ret;
}

void uapi_dma_deinit(void)
{
    if (unlikely(!g_dma_is_initialised)) {
        return;
    }

    hal_dma_deinit();

    g_dma_is_initialised = false;
}

errcode_t uapi_dma_open(void)
{
    if (unlikely(!g_dma_is_initialised)) {
        return ERRCODE_DMA_NOT_INIT;
    }

    hal_dma_open();

    dma_port_register_irq();

    return ERRCODE_SUCC;
}

void uapi_dma_close(void)
{
    if (unlikely(!g_dma_is_initialised)) {
        return;
    }
    hal_dma_close();

    dma_port_unregister_irq();
}

errcode_t uapi_dma_start_transfer(uint8_t channel)
{
    if (unlikely(!g_dma_is_initialised)) {
        return ERRCODE_DMA_NOT_INIT;
    }

    if (unlikely(channel >= DMA_CHANNEL_MAX_NUM)) {
        return ERRCODE_DMA_INVALID_PARAMETER;
    }

    hal_dma_enable((dma_channel_t)channel, true);

    return ERRCODE_SUCC;
}

errcode_t uapi_dma_end_transfer(uint8_t channel)
{
    if (unlikely(!g_dma_is_initialised)) {
        return ERRCODE_DMA_NOT_INIT;
    }

    if (unlikely(channel >= DMA_CHANNEL_MAX_NUM)) {
        return ERRCODE_DMA_INVALID_PARAMETER;
    }
    hal_dma_enable((dma_channel_t)channel, false);

    return ERRCODE_SUCC;
}

uint32_t uapi_dma_get_block_ts(uint8_t channel)
{
    if (unlikely(!g_dma_is_initialised)) {
        return ERRCODE_DMA_NOT_INIT;
    }

    if (unlikely(channel >= DMA_CHANNEL_MAX_NUM)) {
        return 0;
    }

    return hal_dma_get_block((dma_channel_t)channel);
}

static void dma_reset_channel_interrupt(dma_channel_t channel)
{
    for (uint8_t int_type = HAL_DMA_INTERRUPT_TFR; int_type <= HAL_DMA_INTERRUPT_ERR; int_type++) {
        hal_dma_interrupt_clear(channel, int_type);
    }
}

static errcode_t dma_configure_memory_transfer(dma_channel_t channel, const dma_ch_user_memory_config_t *user_cfg,
                                               dma_transfer_cb_t callback, bool lli_flag, uintptr_t arg)
{
    hal_dma_transfer_base_config_t transfer_config;

    transfer_config.src = user_cfg->src;
    transfer_config.dest = user_cfg->dest;
    transfer_config.transfer_num = user_cfg->transfer_num;
    transfer_config.priority = user_cfg->priority;

    transfer_config.src_width = user_cfg->width;
    transfer_config.dest_width = user_cfg->width;
    transfer_config.src_inc = HAL_DMA_ADDRESS_INC_INCREMENT;
    transfer_config.dest_inc = HAL_DMA_ADDRESS_INC_INCREMENT;
    transfer_config.callback = callback;
    transfer_config.priv_arg = arg;
    if (user_cfg->width == HAL_DMA_TRANSFER_WIDTH_8) {
        transfer_config.src_burst_trans_length = DMA_MEMORY_BURST_TRANS_LENGTH_BYTE;
        transfer_config.dest_burst_trans_length = DMA_MEMORY_BURST_TRANS_LENGTH_BYTE;
    } else if (user_cfg->width == HAL_DMA_TRANSFER_WIDTH_16) {
        transfer_config.src_burst_trans_length = DMA_MEMORY_BURST_TRANS_LENGTH_HARF;
        transfer_config.dest_burst_trans_length = DMA_MEMORY_BURST_TRANS_LENGTH_HARF;
        if ((!dma_check_addr_align(user_cfg->src, DMA_ADDR_ALIGN_MASK_2)) ||
            (!dma_check_addr_align(user_cfg->dest, DMA_ADDR_ALIGN_MASK_2))) {
            return ERRCODE_DMA_RET_ERROR_ADDRESS_ALIGN;
        }
    } else { /* Transfer width 32bit. */
        transfer_config.src_burst_trans_length = DMA_MEMORY_BURST_TRANS_LENGTH_WORD;
        transfer_config.dest_burst_trans_length = DMA_MEMORY_BURST_TRANS_LENGTH_WORD;
        if ((!dma_check_addr_align(user_cfg->src, DMA_ADDR_ALIGN_MASK_4)) ||
            (!dma_check_addr_align(user_cfg->dest, DMA_ADDR_ALIGN_MASK_4))) {
            return ERRCODE_DMA_RET_ERROR_ADDRESS_ALIGN;
        }
    }

    /* dma normal transfer. */
    if (!lli_flag) {
        if (hal_dma_config_single_block(channel, &transfer_config, NULL) == ERRCODE_SUCC) {
            uapi_dma_start_transfer((uint8_t)channel);
            return ERRCODE_SUCC;
        }
    } else { /* dma lli config. */
#if defined(CONFIG_DMA_SUPPORT_LLI)
        return hal_dma_add_lli_transfer(channel, &transfer_config, NULL);
#endif /* CONFIG_DMA_SUPPORT_LLI */
    }

    return ERRCODE_DMA_RET_ERROR_CONFIG;
}

errcode_t uapi_dma_transfer_memory_single(const dma_ch_user_memory_config_t *user_cfg,
                                          dma_transfer_cb_t callback, uintptr_t arg)
{
    if (unlikely(user_cfg == NULL)) {
        return ERRCODE_DMA_INVALID_PARAMETER;
    }
    if (user_cfg->priority > HAL_DMA_CH_PRIORITY_3) {
        return ERRCODE_DMA_INVALID_PARAMETER;
    }

    if (user_cfg->width > HAL_DMA_TRANSFER_WIDTH_32) {
        return ERRCODE_DMA_INVALID_PARAMETER;
    }
    if (unlikely(!g_dma_is_initialised)) {
        return ERRCODE_DMA_NOT_INIT;
    }

    dma_channel_t ch = hal_dma_get_idle_ch(HAL_DMA_HANDSHAKING_MAX_NUM, DMA_MEMORY_BURST_TRANS_LENGTH_BYTE);
    if (ch == DMA_CHANNEL_NONE) {
        return ERRCODE_DMA_RET_NO_AVAIL_CH;
    }
    dma_reset_channel_interrupt(ch);
    return dma_configure_memory_transfer(ch, user_cfg, callback, false, arg);
}

static bool dma_peripheral_cfg_param_check(const dma_ch_user_peripheral_config_t *user_cfg)
{
    if (unlikely(user_cfg == NULL)) {
        return false;
    }

    if (unlikely(user_cfg->src_handshaking >= HAL_DMA_HANDSHAKING_MAX_NUM) ||
        unlikely(user_cfg->dest_handshaking >= HAL_DMA_HANDSHAKING_MAX_NUM)) {
        return false;
    }

    if (unlikely(user_cfg->trans_type > HAL_DMA_TRANS_PERIPHERAL_TO_PERIPHERAL_DST)) {
        return false;
    }

    if (unlikely(user_cfg->trans_dir > HAL_DMA_TRANSFER_DIR_PERIPHERAL_TO_PERIPHERAL)) {
        return false;
    }

    if (unlikely(user_cfg->priority > HAL_DMA_CH_PRIORITY_3)) {
        return false;
    }

    if (unlikely((user_cfg->src_width > HAL_DMA_TRANSFER_WIDTH_256) ||
                 (user_cfg->dest_width > HAL_DMA_TRANSFER_WIDTH_256))) {
        return false;
    }

    if (unlikely(user_cfg->burst_length > HAL_DMA_BURST_TRANSACTION_LENGTH_256)) {
        return false;
    }

    if (unlikely((user_cfg->src_increment >= HAL_DMA_ADDRESS_INC_TYPES) ||
                 (user_cfg->dest_increment >= HAL_DMA_ADDRESS_INC_TYPES))) {
        return false;
    }

    if (unlikely(user_cfg->protection > HAL_DMA_PROTECTION_CONTROL_ALL)) {
        return false;
    }

    return true;
}

static void dma_peripheral_transfer_param_configure(hal_dma_transfer_base_config_t *transfer_config,
                                                    hal_dma_transfer_peri_config_t *peripheral_config,
                                                    const dma_ch_user_peripheral_config_t *user_cfg)
{
    transfer_config->src = user_cfg->src;
    transfer_config->dest = user_cfg->dest;
    transfer_config->transfer_num = user_cfg->transfer_num;
    transfer_config->src_width = user_cfg->src_width;
    transfer_config->dest_width = user_cfg->dest_width;
    transfer_config->src_burst_trans_length = user_cfg->burst_length;
    transfer_config->dest_burst_trans_length = user_cfg->burst_length;
    transfer_config->priority = user_cfg->priority;
    transfer_config->src_inc = user_cfg->src_increment;
    transfer_config->dest_inc = user_cfg->dest_increment;

    peripheral_config->trans_type = user_cfg->trans_type;
    peripheral_config->hs_source = user_cfg->src_handshaking;
    peripheral_config->hs_dest = user_cfg->dest_handshaking;
    peripheral_config->protection = user_cfg->protection;
    peripheral_config->trans_dir = user_cfg->trans_dir;
}

errcode_t uapi_dma_configure_peripheral_transfer_single(const dma_ch_user_peripheral_config_t *user_cfg,
                                                        uint8_t *channel, dma_transfer_cb_t callback, uintptr_t arg)
{
    hal_dma_transfer_base_config_t transfer_config;
    hal_dma_transfer_peri_config_t peripheral_config;

    if (unlikely(!g_dma_is_initialised)) {
        return ERRCODE_DMA_NOT_INIT;
    }

    if (!dma_peripheral_cfg_param_check(user_cfg)) {
        return ERRCODE_DMA_INVALID_PARAMETER;
    }
    if (user_cfg->trans_dir == HAL_DMA_TRANSFER_DIR_MEM_TO_PERIPHERAL) {
        *channel = hal_dma_get_idle_ch(user_cfg->dest_handshaking, user_cfg->burst_length);
    } else {
        *channel = hal_dma_get_idle_ch(user_cfg->src_handshaking, user_cfg->burst_length);
    }

    if (*channel == DMA_CHANNEL_NONE) { return ERRCODE_DMA_RET_NO_AVAIL_CH; }

    dma_reset_channel_interrupt(*channel);
    transfer_config.callback = callback;
    transfer_config.priv_arg = arg;
    dma_peripheral_transfer_param_configure(&transfer_config, &peripheral_config, user_cfg);

    return hal_dma_config_single_block(*channel, &transfer_config, &peripheral_config);
}

#if defined(CONFIG_DMA_SUPPORT_LLI)
uint8_t uapi_dma_get_lli_channel(uint8_t burst_length, uint8_t handshaking)
{
    if (burst_length > HAL_DMA_BURST_TRANSACTION_LENGTH_256) {
        return (uint8_t)DMA_CHANNEL_NONE;
    }

    if (handshaking > HAL_DMA_HANDSHAKING_MAX_NUM) {
        return (uint8_t)DMA_CHANNEL_NONE;
    }
    if (unlikely(!g_dma_is_initialised)) {
        return (uint8_t)DMA_CHANNEL_NONE;
    }

    dma_channel_t ch = hal_dma_get_idle_ch((hal_dma_handshaking_source_t)handshaking,
                                           (hal_dma_burst_transaction_length_t)burst_length);
    if (ch != DMA_CHANNEL_NONE) {
        dma_reset_channel_interrupt(ch);
    }
    return (uint8_t)ch;
}

errcode_t uapi_dma_transfer_memory_lli(uint8_t channel, const dma_ch_user_memory_config_t *user_cfg,
                                       dma_transfer_cb_t callback)
{
    if (unlikely(channel >= DMA_CHANNEL_MAX_NUM)) {
        return ERRCODE_DMA_INVALID_PARAMETER;
    }
    if (unlikely(user_cfg == NULL)) {
        return ERRCODE_DMA_INVALID_PARAMETER;
    }
    if (user_cfg->priority > HAL_DMA_CH_PRIORITY_3) {
        return ERRCODE_DMA_INVALID_PARAMETER;
    }

    if (user_cfg->width > HAL_DMA_TRANSFER_WIDTH_32) {
        return ERRCODE_DMA_INVALID_PARAMETER;
    }
    if (unlikely(!g_dma_is_initialised)) {
        return ERRCODE_DMA_NOT_INIT;
    }
    return dma_configure_memory_transfer(channel, user_cfg, callback, true, NULL);
}

errcode_t uapi_dma_configure_peripheral_transfer_lli(uint8_t channel, const dma_ch_user_peripheral_config_t *user_cfg,
                                                     dma_transfer_cb_t callback)
{
    if (unlikely(channel >= DMA_CHANNEL_MAX_NUM)) {
        return ERRCODE_DMA_INVALID_PARAMETER;
    }
    if (unlikely(!g_dma_is_initialised)) {
        return ERRCODE_DMA_NOT_INIT;
    }
    hal_dma_transfer_base_config_t transfer_config;
    hal_dma_transfer_peri_config_t peripheral_config;

    if (!dma_peripheral_cfg_param_check(user_cfg)) {
        return ERRCODE_DMA_INVALID_PARAMETER;
    }

    transfer_config.callback = callback;

    dma_peripheral_transfer_param_configure(&transfer_config, &peripheral_config, user_cfg);

    return hal_dma_add_lli_transfer(channel, &transfer_config, &peripheral_config);
}

errcode_t uapi_dma_enable_lli(uint8_t channel, dma_transfer_cb_t callback, uintptr_t arg)
{
    if (unlikely(channel >= DMA_CHANNEL_MAX_NUM)) {
        return ERRCODE_DMA_INVALID_PARAMETER;
    }
    if (unlikely(!g_dma_is_initialised)) {
        return ERRCODE_DMA_NOT_INIT;
    }
    if (hal_dma_is_enabled((dma_channel_t)channel)) {
        return ERRCODE_DMA_CH_BUSY;
    }

    hal_dma_enable_lli((dma_channel_t)channel, (hal_dma_transfer_cb_t)callback, arg);
    return ERRCODE_SUCC;
}
#endif /* CONFIG_DMA_SUPPORT_LLI */

#if defined(CONFIG_DMA_SUPPORT_LPM)
errcode_t uapi_dma_suspend(uintptr_t arg)
{
    if (unlikely(!g_dma_is_initialised)) {
        return ERRCODE_DMA_NOT_INIT;
    }
    return hal_dma_suspend(arg);
}

errcode_t uapi_dma_resume(uintptr_t arg)
{
    if (unlikely(!g_dma_is_initialised)) {
        return ERRCODE_DMA_NOT_INIT;
    }
    return hal_dma_resume(arg);
}
#endif
