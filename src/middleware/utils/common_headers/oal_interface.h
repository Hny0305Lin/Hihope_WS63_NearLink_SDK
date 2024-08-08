/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: OAL OS INTERFACE
 */

#ifndef OAL_INTERFACE_H
#define OAL_INTERFACE_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief  Definition of error code.
 */
typedef enum {
    OAL_OS_STATUS_OK                  =  0,
    OAL_OS_STATUS_ERROR               = -1,
    OAL_OS_STATUS_ERROR_TIMEOUT       = -2,
    OAL_OS_STATUS_ERROR_RESOURCE      = -3,
    OAL_OS_STATUS_ERROR_PARA          = -4,
    OAL_OS_STATUS_ERROR_NO_MEM        = -5,
    OAL_OS_STATUS_ERROR_ISR           = -6,
    OAL_OS_STATUS_STATUS_MAX          = 0x7FFFFFFF,
} oal_os_status_t;


/***************************  OS Management ************************/

/***************************  Thread Management ************************/

/***************************  Message Queue ************************/

/***************************  Interrupt ************************/
typedef void (*oal_int_func)(void);
uint32_t oal_int_create(uint32_t int_num, uint32_t int_prio, oal_int_func func, uint32_t param);
uint32_t oal_int_delete(uint32_t int_num);
void oal_int_enable_all(void);
void oal_int_disable_all(void);

/***************************  Generic Wait ************************/
#define OAL_WAIT_FOREVER         0xFFFFFFFFU ///< Wait forever timeout value.
#define OAL_NO_WAIT              0x0U

void oal_os_delay(uint32_t delay_ms);
void oal_os_delay_tick(uint32_t delay_tick);

/***************************  Mutex ************************/

/***************************  Semaphore ************************/

/***************************  Memory Management ************************/
typedef void *oal_mem_pool_id;
oal_os_status_t oal_mem_init(void *pool, uint32_t size);
void *oal_mem_alloc(uint32_t size);
uint32_t oal_mem_free(void *ptr);

/***************************  Timer Management ************************/

/***************************  file system ************************/

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
