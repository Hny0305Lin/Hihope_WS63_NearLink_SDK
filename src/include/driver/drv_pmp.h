/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides PMP driver api \n
 *
 * History: \n
 * 2022-09-26， Create file. \n
 */
#ifndef PMP_H
#define PMP_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#define PMPXCFG_NUM_PER_REG                     4
#define MEMXATTR_NUM_PER_REG                    8
#define PMPADDR_RIGHT_SHIFT_BIS                 2


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_pmp PMP
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of PMP attribute.
 * @else
 * @brief  PMP属性定义。
 * @endif
 */
typedef enum pmp_attr {
    PMP_ATTR_DEVICE_NO_BUFFERABLE = 0,
    PMP_ATTR_DEVICE_BUFFERABLE = 1,
    PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE = 2,
    PMP_ATTR_NO_CACHEABLE_AND_BUFFERABLE = 3,
    PMP_ATTR_WRITETHROUGH_NO_ALLOCATE = 4,
    PMP_ATTR_WRITETHROUGH_RALLOCATE = 6,
    PMP_ATTR_WRITEBACK_RALLOCATE = 7,
    PMP_ATTR_WRITEBACK_NO_ALLOCATE = 8,
    PMP_ATTR_WRITETHROUGH_RWALLOCATE = 14,
    PMP_ATTR_WRITEBACK_RWALLOCATE = 15
} pmp_attr_t;

/**
 * @if Eng
 * @brief  Definition of PMP address match.
 * @else
 * @brief  PMP地址匹配定义。
 * @endif
 */
typedef enum addr_match {
    PMPCFG_ADDR_MATCH_OFF = 0x0,
    PMPCFG_ADDR_MATCH_TOR = 0x1,
    PMPCFG_ADDR_MATCH_NA4 = 0x2,
    PMPCFG_ADDR_MATCH_NAPOT = 0x3
} addr_match_t;

/**
 * @if Eng
 * @brief  Definition of PMP read-write-execute permission.
 * @else
 * @brief  PMP读写权限定义。
 * @endif
 */
typedef enum rwx_permission {
    PMPCFG_NO_ACCESS = 0x0,
    PMPCFG_READ_ONLY_NEXECUTE = 0x1,
    PMPCFG_RW_NEXECUTE = 0x3,
    PMPCFG_READ_ONLY_EXECUTE = 0x5,
    PMPCFG_RW_EXECUTE = 0x7
} rwx_permission_t;

/**
 * @if Eng
 * @brief  Definition of PMP region configuration parameters.
 * @else
 * @brief  PMP域配置参数。
 * @endif
 */
typedef struct pmpx_config {
    rwx_permission_t rwx_permission;  /*!< @if Eng Read-Write-Execute permission.
                                           @else   读写权限。 @endif */
    addr_match_t addr_match;          /*!< @if Eng Address matching method.
                                           @else   地址匹配方式。 @endif */
    bool lock;                        /*!< @if Eng Lock bit configuration.
                                           @else   Lock位配置。 @endif */
    pmp_attr_t pmp_attr;              /*!< @if Eng Read-Write mode configuration.
                                           @else   读写方式配置。 @endif */
} pmpx_config_t;

/**
 * @if Eng
 * @brief  Definition of PMP configuration parameters.
 * @else
 * @brief  PMP配置参数。
 * @endif
 */
typedef struct pmp_conf {
    uint32_t idx;                    /*!< @if Eng Region number.
                                          @else   域编号 @endif */
    uint32_t addr;                   /*!< @if Eng Base address, First Address in NAPOT mode and End Address in TOR mode.
                                          @else   基地址，在NAPOT/NA4模式为首地址，TOR模式为尾地址。 @endif */
    uint32_t size;                   /*!< @if Eng Used in NAPOT mode. Size of memory unit.
                                          @else   在NAPOT模式使用，内存单元的大小。 @endif */
    pmpx_config_t conf;              /*!< @if Eng PMP region configuration parameters.
                                          @else   PMP域配置参数。 @endif */
} pmp_conf_t;

/**
 * @if Eng
 * @brief  Initialize and config PMP module.
 * @param  [in]  config A set of PMP configurations.
 * @param  [in]  length Number of PMP configurations.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  初始化并配置PMP模块。
 * @param  [in]  config 一组PMP域配置。
 * @param  [in]  length PMP配置的个数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pmp_config(const pmp_conf_t *config, uint32_t length);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
