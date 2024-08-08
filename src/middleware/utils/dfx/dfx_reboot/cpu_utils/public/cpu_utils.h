/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: BT CPU UTILS Module Header
 * Author:
 * Create:
 */
#ifndef CPU_UTILS_H
#define CPU_UTILS_H

#include "non_os_reboot.h"
#ifdef SUPPORT_PARTITION_INFO
#include "partition.h"
#endif

/**
 * @defgroup connectivity_libs_cpu_utils CPU Utils
 * @ingroup  connectivity_libs
 * @{
 */
typedef void (*cpu_utils_reboot_cb)(int32_t);

/**
 * @brief  Resets the entire chip, and logging information
 * @param  core The core to request reboot
 * @param  cause The reboot reason
 */
void cpu_utils_reset_chip_with_log(cores_t core, reboot_cause_t cause);

/**
 * @brief  Reset the entire chip with reboot cause
 */
void cpu_utils_reset_chip_with_cause(reboot_cause_t cause);

/**
 * @brief  Registers the IPC action for the core reboot
 */
void cpu_utils_init(void);

/**
 * @brief  Return the core associated with a given core image
 * @param  cimage core image
 * @return physical core associated with core_images_e
 */
cores_t cpu_utils_core_images_to_cores(core_images_e cimage);

/**
 * @brief  set reboot cause & set reboot magic
 * @param  cause reboot cause
 */
void cpu_utils_set_system_status_by_cause(reboot_cause_t cause);

/**
 * @brief  Registers the call back for the Bt core reboot
 * @param  cb callback function
 */
void cpu_utils_set_mcu_callback(cpu_utils_reboot_cb cb);

#ifdef SUPPORT_PARTITION_INFO
/**
 * @brief  Return the partition id associated with a given core image
 * @param  cimage core image
 * @return partition id associated with core_images_e
 */
partition_ids_t cpu_utils_core_iamge_to_partition_id(core_images_e cimage);
#endif

/**
 * @}
 */
#endif
