/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: define mbox, ticks and memory adaptor
 * Author: none
 * Create: 2020
 */

#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__

#include "lwip/opt.h"
#if LWIP_LITEOS_COMPAT
#include "los_memory.h"
#endif

#if NO_SYS == 0

#define SYS_MBOX_NULL               (NULL)
#define sys_mbox_valid(mbox) (((mbox) != (void *)NULL) && (*(mbox) != (void *)NULL))
#define sys_mbox_set_invalid(mbox) do { if ((mbox) != NULL) { *(mbox) = NULL; }} while (0)

/* Note: Please make sure the mbox passed is an valid pointer */
#define sys_dual_mbox_valid(mbox) (*(mbox) != NULL)

#if (MEM_MALLOC_DMA_ALIGN != 1)
static inline void *sys_align_malloc(u16_t length)
{
#if LWIP_LITEOS_COMPAT
  return LOS_MemAllocAlign(OS_SYS_MEM_ADDR, length, MEM_MALLOC_DMA_ALIGN);
#else
  return malloc(length);
#endif
}

static inline void sys_align_free(void *mem)
{
#if LWIP_LITEOS_COMPAT
  (void)LOS_MemFree(OS_SYS_MEM_ADDR, mem);
#else
  free(mem);
#endif
}
#endif

#if LWIP_LITEOS_COMPAT
#include "los_sem.h"
#include "los_typedef.h"
#endif
#include "semaphore.h"
#include "osal_mutex.h"
#include "osal_wait.h"
#include "time.h"

/* this is temp adaptation, please optimize it later! */
#define LITEOS_RR4_DIVERSION (0x00050000)
#if defined(HW_LITEOS_OPEN_VERSION_NUM) && (HW_LITEOS_OPEN_VERSION_NUM >= LITEOS_RR4_DIVERSION) /* br_hw_liteos */
#define BR_HW_LITEOS_VERSION
#endif

#define LWIP_OFFSET_OF     LOS_OFF_SET_OF

#define MBOX_NO_EXPAND    0
#define MBOX_AUTO_EXPAND  1

/**
 * Defines the maximum mailbox size for the incoming packets on a
 * NETCONN_*. make it small to save memory.
 */
#ifndef MAX_MBOX_SIZE
#define MAX_MBOX_SIZE                   2048
#endif

#if LWIP_LITEOS_COMPAT

typedef struct posix_sem sys_sem_t;

#else
struct sys_sem {
  int *sem;
};
typedef struct sys_sem sys_sem_t;
#endif

struct sys_mbox {
  int first, last;
  void **msgs;
  int mbox_size;
  unsigned char is_full;
  unsigned char is_empty;
  unsigned char is_autoexpand;
  osal_wait not_empty;
  osal_wait not_full;
  osal_mutex mutex;
};

typedef struct sys_mbox *sys_mbox_t;

typedef unsigned int sys_thread_t;

#define sys_sem_valid(x)        (((*x).sem == NULL) ? 0 : 1)
#define sys_sem_set_invalid(x)  ((*x).sem = NULL)

// === PROTECTION ===
typedef int sys_prot_t;

#ifdef LWIP_LITEOS_A_COMPAT
typedef pthread_mutex_t sys_mutex_t;
#else
typedef u32_t sys_mutex_t;
#endif

#else  /* NO_SYS == 0 */
#ifdef __cplusplus
extern "C" {
#endif
/* \brief  Get the current systick time in milliSeconds
 *
 *  Returns the current systick time in milliSeconds. This function is only
 *  used in standalone systems.
 *
 *  /returns current systick time in milliSeconds
 */
u32_t sys_now(void);

#ifdef __cplusplus
}
#endif
#endif /* NO_SYS == 0 */

#endif /* __ARCH_SYS_ARCH_H__ */
