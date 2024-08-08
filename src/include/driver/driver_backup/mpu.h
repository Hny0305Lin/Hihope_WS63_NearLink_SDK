/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides MPU driver api \n
 *
 * History: \n
 * 2023-01-16， Create file. \n
 */

#ifndef MPU_H
#define MPU_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_mpu MPU
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  MPU configuration.
 * @else
 * @brief  MPU配置信息定义。
 * @endif
 */
typedef struct mpu_config {
    /**
    * @if Eng
    * @brief  MPU region addr. Needs to be divisible by mpu_region_len.
    * @else
    * @brief  MPU域地址，需要被mpu_region_len整除。
    * @endif
    */
    uint32_t mpu_region_addr;
    /**
     * @if Eng MPU region length.
               -b00000~b00011: reserved.
               -b00100: 32B
               -b00101: 64B
               -b00110: 128B
               -b00111: 256B
               -b01000: 512B
               -b01001: 1KB
               -b01010: 2KB
               -b01011: 4KB
               -b01100: 8KB
               -b01101: 16KB
               -b01110: 32KB
               -b01111: 64B
               -b10000: 128KB
               -b10001: 256KB
               -b10010: 512KB
               -b10011: 1MB
               -b10100: 2MB
               -b10101: 4MB
               -b10110: 8MB
               -b10111: 16MB
               -b11000: 32MB
               -b11001: 64MB
               -b11010: 128MB
               -b11011: 256MB
               -b11100: 512MB
               -b11101: 1GM
               -b11110: 2GB
               -b11111: 4GB
     * @else   MPU域长度。
               -b00000~b00011: reserved.
               -b00100: 32B
               -b00101: 64B
               -b00110: 128B
               -b00111: 256B
               -b01000: 512B
               -b01001: 1KB
               -b01010: 2KB
               -b01011: 4KB
               -b01100: 8KB
               -b01101: 16KB
               -b01110: 32KB
               -b01111: 64B
               -b10000: 128KB
               -b10001: 256KB
               -b10010: 512KB
               -b10011: 1MB
               -b10100: 2MB
               -b10101: 4MB
               -b10110: 8MB
               -b10111: 16MB
               -b11000: 32MB
               -b11001: 64MB
               -b11010: 128MB
               -b11011: 256MB
               -b11100: 512MB
               -b11101: 1GM
               -b11110: 2GB
               -b11111: 4GB
     * @endif
     */
    uint32_t mpu_region_len;
    /**
    * @if Eng
    * @brief  MPU access permit.
    * @else
    * @brief  MPU数据访问的权限。
    * @endif
    */
    uint8_t mpu_rasr_ap;
    /**
    * @if Eng
    * @brief  MPU enable buffur bit.
    * @else
    * @brief  MPU缓冲使能位。
    * @endif
    */
    bool mpu_rasr_buffer_en;
    /**
    * @if Eng
    * @brief  MPU enable indicates access bit.
    * @else
    * @brief  MPU指令访问使能位。
    * @endif
    */
    bool mpu_rasr_exec_en;
    /**
    * @if Eng
    * @brief  MPU type extension bit.
    * @else
    * @brief  MPU类型展开域。
    * @endif
    */
    uint8_t mpu_rasr_tex;
    /**
    * @if Eng
    * @brief  MPU enable cache bit.
    * @else
    * @brief  MPU缓存使能位。
    * @endif
    */
    bool mpu_rasr_cache_en;
    /**
    * @if Eng
    * @brief  MPU enable share bit.
    * @else
    * @brief  MPU共用使能位。
    * @endif
    */
    bool mpu_rasr_share_en;
   /**
    * @if Eng
    * @brief  MPU disable subregion.
    * @else
    * @brief  MPU子域禁止。
    * @endif
    */
    uint8_t mpu_rasr_srd;
} mpu_config_t;

/**
 * @if Eng
 * @brief  Initialize MPU.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化MPU。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_mpu_init(void);

/**
 * @if Eng
 * @brief  Deinitialize MPU.
 * @else
 * @brief  去初始化MPU。
 * @endif
 */
void uapi_mpu_deinit(void);

/**
 * @if Eng
 * @brief  Config MPU.
 * @param  [in]  mpu_cfg MPU config table.
 * @param  [in]  mpu_cfg_size lenth of MPU config table.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  配置MPU。
 * @param  [in]  mpu_cfg MPU配置项。
 * @param  [in]  mpu_cfg_size MPU配置项长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_mpu_config(const mpu_config_t *mpu_cfg, uint8_t mpu_cfg_size);

/**
 * @if Eng
 * @brief  Enable MPU.
 * @param  [in]  open_privdef Enables the default memory map as a background region for privileged access.
 * @param  [in]  enable_hfnmi Enable mpu hfnmi.
 *                            Controls whether handlers executing with priority less than 0 access memory
 *                            with the MPU enabled or with the MPU disabled.This applies to HardFaults, NMIs,
 *                            and exception handlers when FAULTMASK is set to 1.
 *                            -true:  Disables the MPU, use these handlers.
 *                            -false: Use the MPU for memory accesses by these handlers.
 * @else
 * @brief  使能MPU。
 * @param  [in]  open_privdef 启用默认内存映射作为特权访问的背景域。
 * @param  [in]  enable_hfnmi 使能mpu hfnmi。
 *                            控制以小于0的优先级执行的处理程序是在MPU启用还是MPU禁用的情况下访问内存。
 *                            当故障掩码设置为1时，这适用于HardFaults、NMI和异常处理程序。
 *                            -true：禁用MPU，使用这些处理程序。
 *                            -false：使用MPU进行这些处理程序的内存访问。
 * @endif
 */
void uapi_mpu_enable(bool open_privdef, bool enable_hfnmi);

/**
 * @if Eng
 * @brief  Disable MPU.
 * @else
 * @brief  去使能MPU。
 * @endif
 */
void uapi_mpu_disable(void);

/**
 * @if Eng
 * @brief  Enable MPU region.
 * @param  [in]  mpu_region_num Enable which one region.
 * @param  [in]  mpu_cfg Region config. mpu_region_len size need be 2^n, minimum is 32. And region_addr needs
 *                       to be divisible by region_len.The region_addr is low than 5 is required to be 0.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  使能PMU域功能。
 * @param  [in]  mpu_region_num 使能哪一个域。
 * @param  [in]  mpu_cfg 域配置。
 *                       mpu_region_len大小需要为2^n，最小为32。
 *                       并且region_addr需要被region_len整除，region_addr低于5则视为0。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_mpu_region_enable(uint8_t mpu_region_num, const mpu_config_t *mpu_cfg);

/**
 * @if Eng
 * @brief  Disable MPU region.
 * @param  [in]  mpu_region_num Enable which one region.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去使能PMU域功能。
 * @param  [in]  mpu_region_num 使能哪一个域。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_mpu_region_disable(uint8_t mpu_region_num);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
