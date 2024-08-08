/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2023. All rights reserved.
 * Description: oal_mm.h 的头文件
 * Create: 2012年12月10日
 */

#ifndef __OAL_MM_H__
#define __OAL_MM_H__

#if defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/slab.h>
#include <linux/hardirq.h>
#include <linux/vmalloc.h>
#elif defined(_PRE_OS_VERSION_LITEOS) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include <string.h>
#endif
#include <malloc.h>
#include <stdlib.h>
#include "oal_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
 功能描述  : 申请核心态的内存空间，并填充0。对于Linux操作系统而言，
             需要考虑中断上下文和内核上下文的不同情况(GFP_KERNEL和GFP_ATOMIC)。
 输入参数  : size: alloc mem size
 返 回 值  : alloc mem addr
*****************************************************************************/
OAL_STATIC OAL_INLINE osal_void* oal_memalloc(osal_u32 size)
{
#if defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    td_s32   l_flags = GFP_KERNEL;
    td_void   *mem_space = OSAL_NULL;

    /* 不睡眠或在中断程序中标志置为GFP_ATOMIC */
    if (in_interrupt() || irqs_disabled()) {
        l_flags = GFP_ATOMIC;
    }

    if (size == 0) {
        return OSAL_NULL;
    }

    mem_space = kmalloc(size, l_flags);
    if (mem_space == OSAL_NULL) {
        return OSAL_NULL;
    }

    return mem_space;
#elif defined(_PRE_OS_VERSION_LITEOS) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return malloc(size);
#endif
}

/*****************************************************************************
 函 数 名  : oal_free
 功能描述  : 释放核心态的内存空间。
 输入参数  : pbuf:需释放的内存地址
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月17日
    修改内容   : 新生成函数

*****************************************************************************/
OAL_STATIC OAL_INLINE osal_void  oal_free(osal_void *p_buf)
{
    if (p_buf != NULL) {
        free(p_buf);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_mm.h */

