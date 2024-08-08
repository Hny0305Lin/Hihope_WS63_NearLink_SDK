/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: addr
 *
 * Create: 2021-12-16
 */

#include <string.h>
#include <los_memory.h>
#include <los_config.h>
#include <linux/kernel.h>

#ifdef HW_LITEOS_OPEN_VERSION_NUM
#include <securec.h>
#endif
#include "soc_osal.h"
#include "osal_inner.h"

// osal_gfp_flag is not valid in liteos
void *osal_kmalloc(unsigned long size, unsigned int osal_gfp_flag)
{
    osal_unused(osal_gfp_flag);
    return LOS_MemAlloc((void*)m_aucSysMem0, size);
}

void *osal_kzalloc(unsigned long size, unsigned int osal_gfp_flag)
{
    osal_unused(osal_gfp_flag);
    void *addr = LOS_MemAlloc((void*)m_aucSysMem0, size);
    if (addr != NULL) {
        memset_s(addr, size, 0, size);
    }
    return addr;
}

void *osal_kmalloc_align(unsigned int size, unsigned int osal_gfp_flag, unsigned int boundary)
{
    osal_unused(osal_gfp_flag);
    return LOS_MemAllocAlign((void*)m_aucSysMem0, size, boundary);
}

void *osal_kzalloc_align(unsigned int size, unsigned int osal_gfp_flag, unsigned int boundary)
{
    osal_unused(osal_gfp_flag);
    void *addr = LOS_MemAllocAlign((void*)m_aucSysMem0, size, boundary);
    if (addr != NULL) {
        memset_s(addr, size, 0, size);
    }
    return addr;
}

void osal_kfree(void *addr)
{
    (void)LOS_MemFree((void*)m_aucSysMem0, (void*)addr);
}

int osal_pool_mem_init(void *pool, unsigned int size)
{
    unsigned int ret = LOS_MemInit(pool, size);
    if (ret != LOS_OK) {
        return OSAL_FAILURE;
    }
    return OSAL_SUCCESS;
}

void *osal_pool_mem_alloc(void *pool, unsigned int size)
{
    return LOS_MemAlloc(pool, size);
}

void *osal_pool_mem_alloc_align(void *pool, unsigned int size, unsigned int boundary)
{
    return LOS_MemAllocAlign(pool, size, boundary);
}

void osal_pool_mem_free(void *pool, const void *addr)
{
    (void)LOS_MemFree(pool, (void*)addr);
}

int osal_pool_mem_deinit(void *pool)
{
#ifndef TINY_KERNEL
#ifdef LOSCFG_MEM_MUL_POOL
    unsigned int ret = LOS_MemDeInit(pool);
    if (ret != LOS_OK) {
        osal_log("LOS_MemDeInit failed! ret = %#x.\n", ret);
        return OSAL_FAILURE;
    }
#endif
#endif
    return OSAL_SUCCESS;
}

void *osal_vmalloc(unsigned long size)
{
#ifdef HW_LITEOS_OPEN_VERSION_NUM
    return LOS_MemAlloc((void*)m_aucSysMem0, size);
#else
    return LOS_VMalloc(size);
#endif
}

void *osal_vzalloc(unsigned long size)
{
#ifdef HW_LITEOS_OPEN_VERSION_NUM
    void *addr = LOS_MemAlloc((void*)m_aucSysMem0, size);
#else
    void *addr = LOS_VMalloc(size);
#endif
    if (addr != NULL) {
        memset_s(addr, size, 0, size);
    }
    return addr;
}

void osal_vfree(void *addr)
{
#ifdef HW_LITEOS_OPEN_VERSION_NUM
    (void)LOS_MemFree((void*)m_aucSysMem0, (void*)addr);
#else
    LOS_VFree((void *)addr);
#endif
}

void *osal_ioremap(unsigned long phys_addr, unsigned long size)
{
#ifdef HW_LITEOS_OPEN_VERSION_NUM
    return ioremap_nocache(phys_addr, size);
#else
    return ioremap(phys_addr, size);
#endif
}

void *osal_ioremap_nocache(unsigned long phys_addr, unsigned long size)
{
    return ioremap_nocache(phys_addr, size);
}
void *osal_ioremap_wc(unsigned long phys_addr, unsigned long size)
{
    return ioremap_nocache(phys_addr, size);
}

void *osal_ioremap_cached(unsigned long phys_addr, unsigned long size)
{
    return ioremap_cached(phys_addr, size);
}

void osal_iounmap(void *addr, unsigned long size)
{
    return iounmap(addr);
}

/*
* copy success: return 0;
* copy fail : return remain bytes
*/
unsigned long osal_copy_from_user(void *to, const void *from, unsigned long n)
{
    if (to == NULL || from == NULL) {
        return n;
    }
#ifdef HW_LITEOS_OPEN_VERSION_NUM
    errno_t ret = memcpy_s(to, n, from, n);
    return (ret == EOK) ? 0 : n;
#else
    int ret = LOS_CopyToKernel(to, n, from, n);
    if (ret != 0) {
        return n;
    }
    return 0;
#endif
}
/*
* copy success: return 0;
* copy fail : return remain bytes
*/
unsigned long osal_copy_to_user(void *to, const void *from, unsigned long n)
{
    if (to == NULL || from == NULL) {
        return n;
    }
#ifdef HW_LITEOS_OPEN_VERSION_NUM
    errno_t ret = memcpy_s(to, n, from, n);
    return (ret == EOK) ? 0 : n;
#else
    int ret = LOS_CopyFromKernel(to, n, from, n);
    if (ret != 0) {
        return n;
    }
    return 0;
#endif
}

void *osal_phys_to_virt(unsigned long addr)
{
    return (void *)(UINTPTR)addr;
}

unsigned long osal_virt_to_phys(const void *virt_addr)
{
    return (unsigned long)(UINTPTR)virt_addr;
}

int osal_access_ok(int type, const void *addr, unsigned long size)
{
    osal_log("Do not support in liteos!\n");
    return 1;
}
