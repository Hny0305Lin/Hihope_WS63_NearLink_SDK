/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V151 HAL dma \n
 *
 * History: \n
 * 2023-2-5， Create file. \n
 */
#ifndef HAL_DMAC_V151_H
#define HAL_DMAC_V151_H

#include "hal_dma.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @if Eng
 * @brief  Initialize device for hal DMA.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层DMA的初始化接口
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_dma_v151_init(void);

/**
 * @if Eng
 * @brief  Deinitialize device for hal DMA.
 * @else
 * @brief  HAL层DMA的去初始化接口
 * @endif
 */
void hal_dma_v151_deinit(void);

 /**
 * @if Eng
 * @brief  Open interface for hal DMA.
 * @else
 * @brief  HAL层DMA开启接口
 * @endif
 */
void hal_dma_v151_open(void);

 /**
 * @if Eng
 * @brief  Close interface for hal DMA.
 * @else
 * @brief  HAL层DMA关闭接口
 * @endif
 */
void hal_dma_v151_close(void);

/**
 * @if Eng
 * @brief  Channel enable/disable interface for hal DMA.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t
 * @param  [in]  en flag of enable or disable.
 * @else
 * @brief  HAL层通道使能/去使能接口
 * @param  [in]  ch DMA通道 参考 @ref dma_channel_t
 * @param  [in]  en 使能/去使能标志
 * @endif
 */
void hal_dma_v151_enable(dma_channel_t ch, bool en);

/**
 * @if Eng
 * @brief  Get the number of data transferred by the DMA for hal DMA.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t
 * @retval  The number of data transferred by the DMA.
 * @else
 * @brief  HAL层获取DMA传输的数据量接口
 * @param  [in]  ch DMA通道 参考 @ref dma_channel_t
 * @retval  DMA传输的数据数
 * @endif
 */
uint32_t hal_dma_v151_get_block(dma_channel_t ch);

/**
 * @if Eng
 * @brief  Interrupt clear interface for hal DMA.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t
 * @param  [in]  int_type Interrupt type. For details, see @ref hal_dma_interrupt_t
 * @else
 * @brief  HAL层清除中断接口
 * @param  [in]  ch DMA通道 参考 @ref dma_channel_t
 * @param  [in]  int_type 中断类型 参考 @ref hal_dma_interrupt_t
 * @endif
 */
void hal_dma_v151_interrupt_clear(dma_channel_t ch, hal_dma_interrupt_t int_type);

/**
 * @if Eng
 * @brief  Get the idle channel interface for hal DMA.
 * @param  [in]  source DMA handshaking source select. For details, see @ref hal_dma_handshaking_source_t
 * @param  [in]  burst_length DMA transfer burst length. For details, see @ref hal_dma_burst_transaction_length_t
 * @retval The DMA channel. For details, see @ref dma_channel_t
 * @else
 * @brief  HAL层获取闲置通道接口
 * @param  [in]  source DMA握手来源选择
 * @param  [in]  burst_length DMA传输burst长度
 * @retval DMA通道 参考 @ref dma_channel_t
 * @endif
 */
dma_channel_t hal_dma_v151_get_idle_ch(hal_dma_handshaking_source_t source,
                                       hal_dma_burst_transaction_length_t burst_length);

/**
 * @if Eng
 * @brief  Configure a Single-block Transfer interface for hal DMA.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t
 * @param  [in]  base_cfg Base config for this transfer. For details, see @ref hal_dma_transfer_base_config_t
 * @param  [in]  periph_cfg Peripheral config for this transfer. For details, see @ref hal_dma_transfer_peri_config_t
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层配置单块传输
 * @param  [in]  ch DMA通道 参考 @ref dma_channel_t
 * @param  [in]  base_cfg 传输的基本配置 参考 @ref hal_dma_transfer_base_config_t
 * @param  [in]  periph_cfg 传输的外设配置 参考 @ref hal_dma_transfer_peri_config_t
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_dma_v151_config_single_block(dma_channel_t ch,
                                           const hal_dma_transfer_base_config_t *base_cfg,
                                           hal_dma_transfer_peri_config_t *periph_cfg);

/**
 * @if Eng
 * @brief  Dma lli transfer config interface for hal DMA.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t
 * @param  [in]  base_cfg Base config for this transfer. For details, see @ref hal_dma_transfer_base_config_t
 * @param  [in]  periph_cfg Peripheral config for this transfer. For details, see @ref hal_dma_transfer_peri_config_t
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层配置链表传输
 * @param  [in]  ch DMA通道 参考 @ref dma_channel_t
 * @param  [in]  base_cfg 传输的基本配置 参考 @ref hal_dma_transfer_base_config_t
 * @param  [in]  periph_cfg 传输的外设配置 参考 @ref hal_dma_transfer_peri_config_t
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_dma_v151_add_lli_transfer(dma_channel_t ch,
                                        const hal_dma_transfer_base_config_t *base_cfg,
                                        hal_dma_transfer_peri_config_t *periph_cfg);

/**
 * @if Eng
 * @brief  Check whether the DMA channel is enabled interface for hal DMA.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t
 * @retval true   enabled.
 * @retval false  disable.
 * @else
 * @brief  HAL层检查DMA通道是否使能接口
 * @param  [in]  ch DMA通道 参考 @ref dma_channel_t
 * @retval true   使能
 * @retval false  未使能
 * @endif
 */
bool hal_dma_v151_is_enabled(dma_channel_t ch);

/**
 * @if Eng
 * @brief  Enable dma lli transfer interface for hal DMA.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t
 * @param  [in]  callback Channel transfer callback function. For details, see @ref hal_dma_transfer_cb_t.
 * @param  [in]  arg Private param pointer for storing self information, which can be passed to DMA callback
 *                     @ref hal_dma_transfer_cb_t when transfer completed.
 * @else
 * @brief  HAL层使能链表传输接口
 * @param  [in]  ch DMA通道 参考 @ref dma_channel_t
 * @param  [in]  callback 通道传输回调函数 For details, see @ref hal_dma_transfer_cb_t 。
 * @param  [in]  arg 用于存储自定义信息的私有参数指针，传输完成时回传给dma回调函数 @ref hal_dma_transfer_cb_t 。
 * @endif
 */
void hal_dma_v151_enable_lli(dma_channel_t ch, hal_dma_transfer_cb_t callback, uintptr_t arg);

/**
 * @if Eng
 * @brief  Handler of the DMA interrupt request.
 * @else
 * @brief  DMA中断处理函数。
 * @endif
 */
void hal_dma_v151_irq_handler(void);

void dma_port_release_handshaking_source(dma_channel_t ch);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif