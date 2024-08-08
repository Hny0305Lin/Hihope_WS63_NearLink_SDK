/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides DMA driver api \n
 *
 * History: \n
 * 2022-10-16， Create file. \n
 */

#ifndef DMA_H
#define DMA_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_dma DMA
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Memory-to-memory type of DMA channel user configuration.
 * @else
 * @brief  内存到内存类型的DMA通道用户配置。
 * @endif
 */
typedef struct dma_ch_user_memory_config {
    /** @if Eng  The source address of this transfer.
     *  @else    传输源地址。
     *  @endif */
    uint32_t src;
    /** @if Eng  The destination address of this transfer.
     *  @else    传输目的地址。
     *  @endif */
    uint32_t dest;
    /** @if Eng  Transfer number.
     *  @else    传输数据量。
     *  @endif */
    uint16_t transfer_num;
    /** @if Eng  Transfer priority of channel(Lowest: 0 and Highest: 3).
     *  @else    传输通道优先级(最低为0以及最高为3)。
     *  @endif */
    uint8_t priority;
    /** @if Eng  Transfer data width:
     *           - 0: 1byte
     *           - 1: 2byte
     *           - 2: 4byte
     *  @else    传输数据宽度：
     *           - 0: 1字节
     *           - 1: 2字节
     *           - 2: 4字节
     *  @endif */
    uint8_t width;
} dma_ch_user_memory_config_t;

/**
 * @if Eng
 * @brief  Memory-to-periph or periph-to-memory type of DMA channel user configuration.
 * @else
 * @brief  内存到外设或外设到内存类型的DMA通道用户配置。
 * @endif
 */
typedef struct dma_ch_user_peripheral_config {
    /** @if Eng  The source address of this transfer.
     *  @else    传输源地址。
     *  @endif */
    uint32_t src;
    /** @if Eng  The destination address of this transfer.
     *  @else    传输目的地址。
     *  @endif */
    uint32_t dest;
    /** @if Eng  Transfer number.
     *  @else    传输数据量。
     *  @endif */
    uint16_t transfer_num;
    /** @if Eng  Hardware handshaking ID of the source. see @ref hal_dma_handshaking_source_t.
     *  @else    源端硬件握手号。 参考 @ref hal_dma_handshaking_source_t 。
     *  @endif */
    uint16_t src_handshaking;
    /** @if Eng  Hardware handshaking ID of the destination. see @ref hal_dma_handshaking_source_t.
     *  @else    目的端硬件握手号。 参考 @ref hal_dma_handshaking_source_t 。
     *  @endif */
    uint16_t dest_handshaking;
    /** @if Eng  Transfer type:
     *           - 0: memory to memory and DMA is flow controller
     *           - 1: memory to periph and DMA is flow controller
     *           - 2: periph to memory and DMA is flow controller
     *           - 3: periph to periph and DMA is flow controller
     *           - 4: periph to memory and periph is flow controller
     *           - 5: periph to periph and source periph is flow controller
     *           - 6: memory to periph and periph is flow controller
     *           - 7: periph to periph and destination periph is flow controller
     *  @else    传输类型：
     *           - 0: 内存到内存并且由DMA流控
     *           - 1: 内存到外设并且由DMA流控
     *           - 2: 外设到内存并且由DMA流控
     *           - 3: 外设到外设并且由DMA流控
     *           - 4: 外设到内存并且由外设流控
     *           - 5: 外设到外设并且由源端外设流控
     *           - 6: 内存到外设并且由外设流控
     *           - 7: 外设到外设并且由目的端外设流控
     *  @endif */
    uint8_t trans_type;
    /** @if Eng  Transfer direction:
     *           - 0: memory to periph
     *           - 1: periph to memory
     *           - 2: periph to periph
     *  @else    传输方向：
     *           - 0: 内存到外设
     *           - 1: 外设到内存
     *           - 2: 外设到外设
     *  @endif */
    uint8_t trans_dir;
    /** @if Eng  Transfer priority of channel(Lowest: 0 and Highest: 3).
     *  @else    传输通道优先级(最低为0以及最高为3)。
     *  @endif */
    uint8_t priority;
    /** @if Eng  Transfer data width of the source:
     *           - 0: 1byte
     *           - 1: 2byte
     *           - 2: 4byte
     *  @else    源端传输数据宽度：
     *           - 0: 1字节
     *           - 1: 2字节
     *           - 2: 4字节
     *  @endif */
    uint8_t src_width;
    /** @if Eng  Transfer data width of the destination:
     *           - 0: 1byte
     *           - 1: 2byte
     *           - 2: 4byte
     *  @else    目的端传输数据宽度：
     *           - 0: 1字节
     *           - 1: 2字节
     *           - 2: 4字节
     *  @endif */
    uint8_t dest_width;
    /** @if Eng  Transfer burst length:
     *           - 0: burst length is 1
     *           - 1: burst length is 4
     *           - 2: burst length is 8
     *           - 3: burst length is 16
     *  @else    传输burst长度：
     *           - 0: burst长度是1
     *           - 1: burst长度是4
     *           - 2: burst长度是8
     *           - 3: burst长度是16
     *  @endif */
    uint8_t burst_length;
    /** @if Eng  Source address incremental mode：
     *           - 0: increment
     *           - 1: decrement
     *           - 2: no change
     *  @else    源端地址增量模式：
     *           - 0: 递增
     *           - 1: 递减
     *           - 2: 不变
     *  @endif */
    uint8_t src_increment;
    /** @if Eng  Destination address incremental mode:
     *           - 0: increment
     *           - 1: decrement
     *           - 2: no change
     *  @else    目的端地址增量模式：
     *           - 0: 递增
     *           - 1: 递减
     *           - 2: 不变
     *  @endif */
    uint8_t dest_increment;
    /** @if Eng  DMA protection control bits used to drive the AHB HPROT[3:1] bus:
     *           - 0: HPROT[1]
     *           - 1: HPROT[2]
     *           - 2: HPROT[3]
     *  @else    保护控制位，用于驱动AHB HPRO[3:1]总线：
     *           - 0: HPROT[1]
     *           - 1: HPROT[2]
     *           - 2: HPROT[3]
     *  @endif */
    uint8_t protection;
} dma_ch_user_peripheral_config_t;

/**
 * @if Eng
 * @brief  Pointer to the DMA channel transfer done/error callback function.
 * @param  [in]  intr DMA interrupt type.
 * @param  [in]  channel DMA channel.
 * @param  [in]  arg DMA private param pointer passed by DMA caller.
 * @else
 * @brief  DMA通道传输完成/错误所触发的回调函数指针。
 * @param  [in]  intr DMA中断类型。
 * @param  [in]  channel DMA通道。
 * @param  [in]  arg 调用DMA传输时传递的私有参数指针。
 * @endif
 */
typedef void (*dma_transfer_cb_t)(uint8_t intr, uint8_t channel, uintptr_t arg);

/**
 * @if Eng
 * @brief  Initialize the DMA module.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化DMA模块。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败 参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_dma_init(void);

/**
 * @if Eng
 * @brief  Deinitialize the DMA module.
 * @else
 * @brief  去初始化DMA模块。
 * @endif
 */
void uapi_dma_deinit(void);

/**
 * @if Eng
 * @brief  Open the DMA module.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  开启DMA模块。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败 参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_dma_open(void);

/**
 * @if Eng
 * @brief  Close the DMA module.
 * @else
 * @brief  关闭DMA模块。
 * @endif
 */
void uapi_dma_close(void);

/**
 * @if Eng
 * @brief  Start DMA transfer of specific Channel.
 * @param  [in]  channel DMA channel.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  启动指定通道的DMA传输。
 * @param  [in]  channel DMA通道。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败 参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_dma_start_transfer(uint8_t channel);

/**
 * @if Eng
 * @brief  Stop DMA transfer of specific Channel.
 * @param  [in]  channel DMA channel.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  停止指定通道的DMA传输。
 * @param  [in]  channel DMA通道。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败 参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_dma_end_transfer(uint8_t channel);

/**
 * @if Eng
 * @brief  Get the number of data transferred by the DMA.
 * @param  [in]  channel DMA channel.
 * @retval The number of data transferred by the DMA.
 * @else
 * @brief  获取DMA传输的数据量。
 * @param  [in]  channel DMA通道。
 * @retval 获取DMA传输的数据量。
 * @endif
 */
uint32_t uapi_dma_get_block_ts(uint8_t channel);

/**
 * @if Eng
 * @brief  Transfer type memory-to-memory through DMA channel.
 * @param  [in]  user_cfg DMA channel transfer configuration of user. For details, see @ref dma_ch_user_memory_config_t.
 * @param  [in]  callback Channel transfer done/error callback function. For details, see @ref dma_transfer_cb_t.
 * @param  [in]  arg Private param pointer for storing self information, which can be passed to DMA callback
 *                     @ref dma_transfer_cb_t when transfer completed.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  通过DMA通道传输类型为内存到内存的数据。
 * @param  [in]  user_cfg 用户的DMA通道传输配置 For details, see @ref dma_ch_user_memory_config_t 。
 * @param  [in]  callback 通道传输完成/错误回调函数 For details, see @ref dma_transfer_cb_t 。
 * @param  [in]  arg 用于存储自定义信息的私有参数指针，传输完成时回传给dma回调函数 @ref dma_transfer_cb_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败 参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_dma_transfer_memory_single(const dma_ch_user_memory_config_t *user_cfg,
                                          dma_transfer_cb_t callback, uintptr_t arg);

/**
 * @if Eng
 * @brief  Transfer type memory-periph or periph-memory through DMA channel.
 * @param  [in]  user_cfg DMA channel transfer configuration of user.
 *                      For details, see @ref dma_ch_user_peripheral_config_t.
 * @param  [out] channel DMA channel selected.
 * @param  [in]  callback Channel transfer done/error callback function. For details, see @ref dma_transfer_cb_t.
 * @param  [in]  arg Private param pointer for storing self information, which can be passed to DMA callback
 *                     @ref dma_transfer_cb_t when transfer completed.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  通过DMA通道传输类型为内存到外设或外设到内存的数据。
 * @param  [in]  user_cfg 用户的DMA通道传输配置 For details, see @ref dma_ch_user_peripheral_config_t 。
 * @param  [out] channel 获取被选择的通道。
 * @param  [in]  callback 通道传输完成/错误回调函数 For details, see @ref dma_transfer_cb_t 。
 * @param  [in]  arg 用于存储自定义信息的私有参数指针，传输完成时回传给DMA回调函数 @ref dma_transfer_cb_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败 参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_dma_configure_peripheral_transfer_single(const dma_ch_user_peripheral_config_t *user_cfg,
                                                        uint8_t *channel, dma_transfer_cb_t callback, uintptr_t arg);

#if defined(CONFIG_DMA_SUPPORT_LLI)
/**
 * @if Eng
 * @brief  Get DMA linked list channel.
 * @param  [in]  burst_length Dma burst transfer length.
 * @param  [in]  handshaking Dma Transfer periph.
 * @retval ERRCODE_SUCC Success.
 * @retval The DMA channel.
 * @else
 * @brief  获取DMA链表传输通道。
 * @param  [in]  burst_length DMA的burst传输长度。
 * @param  [in]  handshaking DMA传输外设种类。
 * @retval DMA通道
 * @endif
 */
uint8_t uapi_dma_get_lli_channel(uint8_t burst_length, uint8_t handshaking);

/**
 * @if Eng
 * @brief  Transfer type memory-to-memory in linked list mode through DMA channel.
 * @param  [in]  channel DMA channel.
 * @param  [in]  user_cfg DMA channel transfer configuration of user. For details, see @ref dma_ch_user_memory_config_t.
 * @param  [in]  callback Channel transfer done/error callback function. For details, see @ref dma_transfer_cb_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  通过DMA通道以链表模式传输类型为内存到内存的数据。
 * @param  [in]  channel DMA通道。
 * @param  [in]  user_cfg 用户的DMA通道传输配置 For details, see @ref dma_ch_user_memory_config_t 。
 * @param  [in]  callback 通道传输完成/错误回调函数 For details, see @ref dma_transfer_cb_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败 参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_dma_transfer_memory_lli(uint8_t channel, const dma_ch_user_memory_config_t *user_cfg,
                                       dma_transfer_cb_t callback);

/**
 * @if Eng
 * @brief  Transfer type memory-periph or periph-memory in linked list mode through DMA channel.
 * @param  [in]  channel DMA channel.
 * @param  [in]  user_cfg DMA channel transfer configuration of user.
 *                      For details, see @ref dma_ch_user_peripheral_config_t.
 * @param  [in]  callback Channel transfer done/error callback function. For details, see @ref dma_transfer_cb_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  通过DMA通道以链表模式传输类型为内存到外设或外设到内存的数据。
 * @param  [in]  channel DMA通道。
 * @param  [in]  user_cfg 用户的DMA通道传输配置 For details, see @ref dma_ch_user_peripheral_config_t 。
 * @param  [in]  callback 通道传输完成/错误回调函数 For details, see @ref dma_transfer_cb_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败 参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_dma_configure_peripheral_transfer_lli(uint8_t channel, const dma_ch_user_peripheral_config_t *user_cfg,
                                                     dma_transfer_cb_t callback);

/**
 * @if Eng
 * @brief  Enable DMA linked list transfer.
 * @param  [in]  channel DMA channel.
 * @param  [in]  callback Channel transfer done/error callback function. For details, see @ref dma_transfer_cb_t.
 * @param  [in]  arg Private param pointer for storing self information, which can be passed to DMA callback
 *                     @ref dma_transfer_cb_t when transfer completed.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  启用DMA链表传输。
 * @param  [in]  channel DMA通道。
 * @param  [in]  callback 通道传输完成/错误回调函数 For details, see @ref dma_transfer_cb_t 。
 * @param  [in]  arg 用于存储自定义信息的私有参数指针，传输完成时回传给dma回调函数 @ref dma_transfer_cb_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败 参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_dma_enable_lli(uint8_t channel, dma_transfer_cb_t callback, uintptr_t arg);
#endif /* CONFIG_DMA_SUPPORT_LLI */

#ifdef CONFIG_DMA_SUPPORT_LPM
/**
 * @if Eng
 * @brief  Resume the DMA module.
 * @param  [in]  arg Argument for resume.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  恢复DMA模块。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败 参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_dma_resume(uintptr_t arg);

/**
 * @if Eng
 * @brief  suspend the DMA module.
 * @param  [in]  arg Argument for suspend.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  挂起DMA模块。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_dma_suspend(uintptr_t arg);
#endif
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
