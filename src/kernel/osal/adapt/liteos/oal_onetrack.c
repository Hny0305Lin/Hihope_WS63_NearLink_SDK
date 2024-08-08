/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: OAL OS
 * Author:
 * Create: 2020-11-17
 */
#include "cmsis_os2.h"
#include "oal_interface.h"
#include "los_memory.h"
#include "los_hwi.h"
#include "non_os.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

uint32_t oal_int_create(uint32_t int_num, uint32_t int_prio, oal_int_func func, uint32_t param)
{
    uint32_t ret;
    HWI_IRQ_PARAM_S par;

    par.swIrq = (int)int_num;
    par.pDevId = (VOID *)(uintptr_t)param;
    par.pName = NULL;
    ret = LOS_HwiCreate(int_num, (uint16_t)(uintptr_t)int_prio, 0, func, &par);
    ret = LOS_HwiEnable(int_num);
    return ret;
}

uint32_t oal_int_delete(uint32_t int_num)
{
    return LOS_HwiDelete(int_num, 0);
}
void oal_int_enable_all(void)
{
    non_os_exit_critical();
}

void oal_int_disable_all(void)
{
    non_os_enter_critical();
}

void oal_os_delay(uint32_t delay_ms)
{
    uint32_t delay_tick;
    delay_tick = (uint32_t)osMs2Tick((uint64_t)delay_ms);
    LOS_TaskDelay(delay_tick); // osDelay use ms on HiAudio // LOS_TaskDelay(ticks);
}

void oal_os_delay_tick(uint32_t delay_tick)
{
    LOS_TaskDelay(delay_tick);
}

oal_os_status_t oal_mem_init(void *pool, uint32_t size)
{
    uint32_t ret;
    ret = LOS_MemInit(pool, size);
    if (ret == LOS_OK) {
        return OAL_OS_STATUS_OK;
    } else {
        return OAL_OS_STATUS_ERROR;
    }
}

void *oal_mem_alloc(uint32_t size)
{
    void *ret;
    ret = (oal_mem_pool_id)LOS_MemAlloc(m_aucSysMem0, size);
    return ret;
}

uint32_t oal_mem_free(void *ptr)
{
    uint32_t ret;
    ret = (uint32_t)LOS_MemFree(m_aucSysMem0, ptr);
    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
