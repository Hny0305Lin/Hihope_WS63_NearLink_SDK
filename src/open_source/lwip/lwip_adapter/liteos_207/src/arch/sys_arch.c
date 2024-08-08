/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: implementation for mbox, ticks and memory adaptor
 * Author: none
 * Create: 2020
 */

#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"

#include "los_config.h"
#include "arch/sys_arch.h"
#include "linux/wait.h"
#include "los_sem.h"
#include "los_mux.h"
#include "string.h"
#include "stdlib.h"

#ifndef UNUSED
#define UNUSED(a) ((void)(a))
#endif

#define LWIP_SEM_ID(x) (u32_t)(uintptr_t)((x)->sem)

#define MBOX_EXPAND_MULTIPLE_SIZE   2

#if defined(LOSCFG_KERNEL_SMP) && defined(YES) && (LOSCFG_KERNEL_SMP == YES)
SPIN_LOCK_INIT(arch_protect_spin);
static u32_t lwprot_thread = LOS_ERRNO_TSK_ID_INVALID;
static int lwprot_count = 0;
#endif /* LOSCFG_KERNEL_SMP == YES */

err_t sys_mbox_new_ext(struct sys_mbox **mb, int size, unsigned char is_auto_expand)
{
  struct sys_mbox *mbox = NULL;
  int ret;

  if (size <= 0) {
    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_new: mbox size must bigger than 0\n"));
    return ERR_VAL;
  }

  mbox = (struct sys_mbox *)mem_malloc(sizeof(struct sys_mbox));
  if (mbox == NULL) {
    goto err_handler;
  }

  (void)memset_s(mbox, sizeof(struct sys_mbox), 0, sizeof(struct sys_mbox));

  mbox->msgs = (void **)mem_malloc(sizeof(void *) * size);
  if (mbox->msgs == NULL) {
    goto err_handler;
  }

  (void)memset_s(mbox->msgs, (sizeof(void *) * size), 0, (sizeof(void*) * size));

  mbox->mbox_size = size;

  mbox->first = 0;
  mbox->last = 0;
  mbox->is_full = 0;
  mbox->is_empty = 1;
  mbox->is_autoexpand = is_auto_expand;

  ret = osal_mutex_init(&(mbox->mutex));
  if (ret != 0) {
    goto err_handler;
  }

  ret = osal_wait_init(&(mbox->not_empty));
  if (ret != 0) {
    (void)osal_mutex_destroy(&(mbox->mutex));
    goto err_handler;
  }

  ret = osal_wait_init(&(mbox->not_full));
  if (ret != 0) {
    (void)osal_mutex_destroy(&(mbox->mutex));
    (void)osal_wait_destroy(&(mbox->not_empty));
    goto err_handler;
  }

  SYS_STATS_INC_USED(mbox);
  *mb = mbox;
  LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_new: mbox created successfully 0x%p\n", (void *)mbox));
  return ERR_OK;

err_handler:
  if (mbox != NULL) {
    if (mbox->msgs != NULL) {
      mem_free(mbox->msgs);
      mbox->msgs = NULL;
    }
    mem_free(mbox);
  }
  return ERR_MEM;
}

/*-----------------------------------------------------------------------------------*/
void sys_mbox_free(struct sys_mbox **mb)
{
  if ((mb != NULL) && (*mb != SYS_MBOX_NULL)) {
    struct sys_mbox *mbox = *mb;
    int ret;
    SYS_STATS_DEC(mbox.used);

    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_free: going to free mbox 0x%p\n", (void *)mbox));

    ret = osal_mutex_lock(&(mbox->mutex));
    if (ret != 0) {
      return;
    }

    (void)osal_wait_destroy(&(mbox->not_empty));
    (void)osal_wait_destroy(&(mbox->not_full));

    (void)osal_mutex_unlock(&(mbox->mutex));

    (void)osal_mutex_destroy(&(mbox->mutex));

    mem_free(mbox->msgs);
    mbox->msgs = NULL;
    mem_free(mbox);
    *mb = NULL;

    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_free: freed mbox\n"));
  }
}

static unsigned char
sys_mbox_auto_expand(struct sys_mbox *mbox)
{
  unsigned char is_expanded = 0;
  void **realloc_msgs = NULL;
  if ((mbox->is_autoexpand == MBOX_AUTO_EXPAND) &&
      (MBOX_EXPAND_MULTIPLE_SIZE * (u32_t)mbox->mbox_size) <= MAX_MBOX_SIZE) {
    realloc_msgs = mem_malloc(MBOX_EXPAND_MULTIPLE_SIZE * sizeof(void *) * (u32_t)mbox->mbox_size);
    if (realloc_msgs != NULL) {
      /* memcpy_s */
      if (mbox->first > 0) {
        (void)memcpy_s(realloc_msgs,
                       sizeof(void *) * (mbox->mbox_size - mbox->first),
                       mbox->msgs + mbox->first,
                       sizeof(void *) * (mbox->mbox_size - mbox->first));
        (void)memcpy_s(realloc_msgs + (mbox->mbox_size - mbox->first),
                       sizeof(void *) * (mbox->last),
                       mbox->msgs,
                       sizeof(void *) * (mbox->last));
      } else {
        (void)memcpy_s(realloc_msgs, sizeof(void *) * mbox->mbox_size,
                       mbox->msgs, sizeof(void *) * mbox->mbox_size);
      }
      mem_free(mbox->msgs);
      mbox->msgs = realloc_msgs;
      mbox->first = 0;
      mbox->last = mbox->mbox_size;
      mbox->mbox_size *= MBOX_EXPAND_MULTIPLE_SIZE;
      LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_post : mbox 0x%p is auto expanded\n", (void *)mbox));
      is_expanded = 1;
    }
  }
  return is_expanded;
}

int lwip_wait_mbox_full_func(struct sys_mbox *mbox)
{
	int ret;
	ret = !(mbox->is_full == 1);
	return ret;
}

/*
 * Routine:  sys_mbox_post
 *
 * Description:
 *      Post the "msg" to the mailbox.
 * Inputs:
 *      sys_mbox_t mbox        -- Handle of mailbox
 *      void *msg              -- Pointer to data to post
 */
void sys_mbox_post(struct sys_mbox **mb, void *msg)
{
  struct sys_mbox *mbox = NULL;
  int ret;
  if ((mb == NULL) || (*mb == NULL)) {
    return;
  }
  mbox = *mb;
  LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_post: mbox 0x%p msg 0x%p\n", (void *)mbox, (void *)msg));

  ret = osal_mutex_lock(&(mbox->mutex));
  if (ret != 0) {
    return;
  }

  while (mbox->is_full == 1) {
    (void)osal_mutex_unlock(&(mbox->mutex));
    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_post : mbox 0x%p mbox 0x%p, queue is full\n", (void *)mbox, (void *)msg));
    ret = osal_wait_interruptible(&(mbox->not_full), (osal_wait_condition_func)lwip_wait_mbox_full_func, mbox);
    if (ret != 0) {
      return;
    }
    (void)osal_mutex_lock(&(mbox->mutex));
  }

  mbox->msgs[mbox->last] = msg;

  mbox->last++;
  if (mbox->last == mbox->mbox_size) {
    mbox->last = 0;
  }

  if (mbox->first == mbox->last) {
    if (sys_mbox_auto_expand(mbox) == 0) {
      mbox->is_full = 1;
    }
  }

  if (mbox->is_empty == 1) {
    mbox->is_empty = 0;
    (void)osal_wait_wakeup(&(mbox->not_empty)); /* if signal failed, anyway it will unlock and go out */
    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_post : mbox 0x%p msg 0x%p, signalling not empty\n", (void *)mbox, (void *)msg));
  }
  LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_post: mbox 0x%p msg 0%p posted\n", (void *)mbox, (void *)msg));
  (void)osal_mutex_unlock(&(mbox->mutex));
}

/*
 * Routine:  sys_mbox_trypost
 *
 * Description:
 *      Try to post the "msg" to the mailbox.  Returns immediately with
 *      error if cannot.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      void *msg               -- Pointer to data to post
 * Outputs:
 *      err_t                   -- ERR_OK if message posted, else ERR_MEM
 *                                  if not.
 */
err_t sys_mbox_trypost(struct sys_mbox **mb, void *msg)
{
  struct sys_mbox *mbox = NULL;
  int ret;
  mbox = *mb;

  LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_trypost: mbox 0x%p msg 0x%p \n", (void *)mbox, (void *)msg));

  ret = osal_mutex_lock(&(mbox->mutex));
  if (ret != 0) {
    return ERR_MEM;
  }

  if (mbox->is_full == 1) {
    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_trypost : mbox 0x%p msgx 0x%p,queue is full\n", (void *)mbox, (void *)msg));
    (void)osal_mutex_unlock(&(mbox->mutex));
    return ERR_MEM;
  }

  mbox->msgs[mbox->last] = msg;

  mbox->last = (mbox->mbox_size - 1 - mbox->last) ? (mbox->last + 1) : 0;

  if (mbox->first == mbox->last) {
    if (sys_mbox_auto_expand(mbox) == 0) {
      mbox->is_full = 1;
    }
  }

  if (mbox->is_empty) {
    mbox->is_empty = 0;
    (void)osal_wait_wakeup(&(mbox->not_empty));
  }
  LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_trypost: mbox 0x%p msg 0x%p posted\n", (void *)mbox, (void *)msg));
  (void)osal_mutex_unlock(&(mbox->mutex));
  return ERR_OK;
}

int lwip_wait_mbox_empty_func(struct sys_mbox *mbox)
{
	int ret;
	ret = !(mbox->is_empty == 1);
	return ret;
}

static u32_t
sys_arch_mbox_handler_timeouts(struct sys_mbox *mbox, u32_t timeout, u8_t ignore_timeout)
{
  struct timespec tmsp;
  int ret;
#if LWIP_USE_POSIX_COND_WAIT_WITH_ABS_TIMEOUT
  struct timeval tv;
#endif /* LWIP_USE_POSIX_COND_WAIT_WITH_ABS_TIMEOUT */
  if (mbox->is_empty && ignore_timeout) {
    return SYS_ARCH_TIMEOUT;
  }

  while ((mbox->is_empty == 1) && (ignore_timeout == 0)) {
    if (timeout != 0) {
#if LWIP_USE_POSIX_COND_WAIT_WITH_ABS_TIMEOUT
      gettimeofday(&tv, NULL);
      tmsp.tv_sec = tv.tv_sec + (timeout / MS_PER_SECOND);
      tmsp.tv_nsec = (tv.tv_usec * NS_PER_USECOND) + ((timeout % US_PER_MSECOND) * US_PER_MSECOND * NS_PER_USECOND);
      tmsp.tv_sec += tmsp.tv_nsec / (NS_PER_USECOND * US_PER_MSECOND * MS_PER_SECOND);
      tmsp.tv_nsec %= (NS_PER_USECOND * US_PER_MSECOND * MS_PER_SECOND);
#else
      tmsp.tv_sec = (timeout / MS_PER_SECOND);
      tmsp.tv_nsec = ((timeout % US_PER_MSECOND) * US_PER_MSECOND * NS_PER_USECOND);
#endif
      osal_mutex_unlock(&(mbox->mutex));
      ret = osal_wait_timeout_interruptible(&(mbox->not_empty), (osal_wait_condition_func)lwip_wait_mbox_empty_func,
          mbox, timeout);
      osal_mutex_lock(&(mbox->mutex));
      if (ret == 0) {
        LWIP_DEBUGF(SYS_DEBUG, ("sys_arch_mbox_fetch: mbox 0x%p,timeout in cond wait\n", (void *)mbox));
        return SYS_ARCH_TIMEOUT;
      }
    } else {
      LWIP_DEBUGF(SYS_DEBUG, ("sys_arch_mbox_fetch: cond wait\n"));
      osal_mutex_unlock(&(mbox->mutex));
      ret = osal_wait_interruptible(&(mbox->not_empty), (osal_wait_condition_func)lwip_wait_mbox_empty_func, mbox);
      osal_mutex_lock(&(mbox->mutex));
      if (ret < 0) {
        return SYS_ARCH_TIMEOUT;
      }
    }
  }
  return ERR_OK;
}

u32_t sys_arch_mbox_fetch_ext(struct sys_mbox **mb, void **msg, u32_t timeout, u8_t ignore_timeout)
{
  struct sys_mbox *mbox = NULL;
#if LWIP_USE_POSIX_COND_WAIT_WITH_ABS_TIMEOUT
  struct timeval tv;
#endif /* LWIP_USE_POSIX_COND_WAIT_WITH_ABS_TIMEOUT */
  int ret;
  mbox = *mb;

  LWIP_DEBUGF(SYS_DEBUG, ("sys_arch_mbox_fetch: mbox 0x%p msg 0x%p\n", (void *)mbox, (void *)msg));

  /* The mutex lock is quick so we don't bother with the timeout stuff here. */
  ret = osal_mutex_lock(&(mbox->mutex));
  if (ret != 0) {
    return SYS_ARCH_TIMEOUT;
  }
  u32_t timeouts = sys_arch_mbox_handler_timeouts(mbox, timeout, ignore_timeout);
  if (timeouts != ERR_OK) {
    (void)osal_mutex_unlock(&(mbox->mutex));
    return SYS_ARCH_TIMEOUT;
  }

  if (msg != NULL) {
    *msg = mbox->msgs[mbox->first];
    LWIP_DEBUGF(SYS_DEBUG, ("sys_arch_mbox_fetch: mbox 0x%p msg 0x%p\n", (void *)mbox, (void *)*msg));
  } else {
    LWIP_DEBUGF(SYS_DEBUG, ("sys_arch_mbox_fetch: mbox 0x%p, null msg\n", (void *)mbox));
  }

  mbox->first = (mbox->mbox_size - 1 - mbox->first) ? (mbox->first + 1) : 0;

  if (mbox->first == mbox->last) {
    mbox->is_empty = 1;
  }

  if (mbox->is_full) {
    mbox->is_full = 0;
    (void)osal_wait_wakeup(&(mbox->not_full));
  }
  LWIP_DEBUGF(SYS_DEBUG, ("sys_arch_mbox_fetch: mbox 0x%p msg 0x%p fetched\n", (void *)mbox, (void *)msg));
  (void)osal_mutex_unlock(&(mbox->mutex));

  return 0;
}

/*
 * Routine:  sys_init
 *
 * Description:
 *      Initialize sys arch
 */
void sys_init(void)
{
  /* set rand seed to make random sequence diff on every startup */
  extern VOID LOS_GetCpuCycle(UINT32 *puwCntHi, UINT32 *puwCntLo);
  u32_t seedhsb, seedlsb;
  LOS_GetCpuCycle(&seedhsb, &seedlsb);
  srand(seedlsb);
}


/*
 * Routine:  sys_arch_protect
 *
 * Description:
 *      This optional function does a "fast" critical region protection and
 *      returns the previous protection level. This function is only called
 *      during very short critical regions. An embedded system which supports
 *      ISR-based drivers might want to implement this function by disabling
 *      interrupts. Task-based systems might want to implement this by using
 *      a mutex or disabling tasking. This function should support recursive
 *      calls from the same task or interrupt. In other words,
 *      sys_arch_protect() could be called while already protected. In
 *      that case the return value indicates that it is already protected.
 *
 *      sys_arch_protect() is only required if your port is supporting an
 *      OS.
 * Outputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 */
sys_prot_t sys_arch_protect(void)
{
#if defined(LOSCFG_KERNEL_SMP) && defined(YES) && (LOSCFG_KERNEL_SMP == YES)
  /* Note that we are using spinlock instead of mutex for LiteOS-SMP here:
   * 1. spinlock is more effective for short critical region protection.
   * 2. this function is called only in task context, not in interrupt handler.
   *    so it's not needed to disable interrupt.
   */
  if (lwprot_thread != LOS_CurTaskIDGet()) {
    /* We are locking the spinlock where it has not been locked before
     * or is being locked by another thread */
    LOS_SpinLock(&arch_protect_spin);
    lwprot_thread = LOS_CurTaskIDGet();
    lwprot_count = 1;
  } else {
    /* It is already locked by THIS thread */
    lwprot_count++;
  }
#else
  LOS_TaskLock();
#endif /* LOSCFG_KERNEL_SMP == YES */

    return 0;
}


/*
 * Routine:  sys_arch_unprotect
 *
 * Description:
 *      This optional function does a "fast" set of critical region
 *      protection to the value specified by pval. See the documentation for
 *      sys_arch_protect() for more information. This function is only
 *      required if your port is supporting an OS.
 * Inputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 */
void sys_arch_unprotect(sys_prot_t pval)
{
  LWIP_UNUSED_ARG(pval);
#if defined(LOSCFG_KERNEL_SMP) && defined(YES) && (LOSCFG_KERNEL_SMP == YES)
  if (lwprot_thread == LOS_CurTaskIDGet()) {
    lwprot_count--;
    if (lwprot_count == 0) {
      lwprot_thread = LOS_ERRNO_TSK_ID_INVALID;
      LOS_SpinUnlock(&arch_protect_spin);
    }
  }
#else
  LOS_TaskUnlock();
#endif /* LOSCFG_KERNEL_SMP == YES */
}

u32_t sys_now(void)
{
    /* Lwip docs mentioned like wraparound is not a problem in this funtion */
    return (u32_t)((LOS_TickCountGet() * 1000) / LOSCFG_BASE_CORE_TICK_PER_SECOND);
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn function, void *arg, int stacksize, int prio)
{
  TSK_INIT_PARAM_S task;
  UINT32 taskid, ret;
  (void)memset_s(&task, sizeof(task), 0, sizeof(task));

  /* Create host Task */
  task.pfnTaskEntry = (TSK_ENTRY_FUNC)function;
  task.uwStackSize  = 4096; /* 任务栈大小设定为4096 */
  task.pcName = (char *)name;
  task.usTaskPrio = prio;
  task.uwResved   = LOS_TASK_STATUS_DETACHED;
#ifdef BR_HW_LITEOS_VERSION
  LOS_TASK_PARAM_INIT_ARG(task, arg);
#else
  task.auwArgs[0] = (UINTPTR)arg;
#endif
  ret = LOS_TaskCreate(&taskid, &task);
  if (ret != LOS_OK) {
    LWIP_DEBUGF(SYS_DEBUG, ("sys_thread_new: LOS_TaskCreate error %u\n", ret));
    return SYS_ARCH_ERROR;
  }
  (void)stacksize;
  return taskid;
}

#ifdef LWIP_DEBUG
/* \brief  Displays an error message on assertion

    This function will display an error message on an assertion
    to the dbg output.

    \param[in]    msg   Error message to display
    \param[in]    line  Line number in file with error
    \param[in]    file  Filename with error
 */
void assert_printf(char *msg, int line, char *file)
{
  if (msg != NULL) {
    LWIP_DEBUGF(LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                ("%s:%d in file %s", msg, line, file));
    return;
  } else {
    LWIP_DEBUGF(LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                ("LWIP ASSERT"));
    return;
  }
}
#endif /* LWIP_DEBUG */


/*
 * Routine:  sys_sem_new
 *
 * Description:
 *      Creates and returns a new semaphore. The "ucCount" argument specifies
 *      the initial state of the semaphore.
 *      NOTE: Currently this routine only creates counts of 1 or 0
 * Inputs:
 *      sys_sem_t sem         -- Handle of semaphore
 *      u8_t count            -- Initial count of semaphore
 * Outputs:
 *      err_t                 -- ERR_OK if semaphore created
 */
err_t sys_sem_new(sys_sem_t *sem,  u8_t count)
{
  u32_t sem_handle;
  u32_t ret;

  if (sem == NULL) {
    return -1;
  }

  LWIP_ASSERT("in sys_sem_new count exceeds the limit", (count < 0xFF));

  ret = LOS_SemCreate(count, &sem_handle);
  if (ret != ERR_OK) {
    return -1;
  }

  sem->sem = (void *)(uintptr_t)(sem_handle);

  return ERR_OK;
}

/*
 * Routine:  sys_arch_sem_wait
 *
 * Description:
 *      Blocks the thread while waiting for the semaphore to be
 *      signaled. If the "timeout" argument is non-zero, the thread should
 *      only be blocked for the specified time (measured in
 *      milliseconds).
 *
 *      If the timeout argument is non-zero, the return value is the number of
 *      milliseconds spent waiting for the semaphore to be signaled. If the
 *      semaphore wasn't signaled within the specified time, the return value is
 *      SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
 *      (i.e., it was already signaled), the function may return zero.
 *
 *      Notice that lwIP implements a function with a similar name,
 *      sys_sem_wait(), that uses the sys_arch_sem_wait() function.
 * Inputs:
 *      sys_sem_t sem           -- Semaphore to wait on
 *      u32_t timeout           -- Number of milliseconds until timeout
 * Outputs:
 *      u32_t                   -- Time elapsed or SYS_ARCH_TIMEOUT.
 */
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  u32_t retval;
  u64_t start_tick;
  u64_t end_tick;

  if ((sem == NULL) || (sem->sem == NULL)) {
    return SYS_ARCH_ERROR;
  }

  start_tick = LOS_TickCountGet();

  if (timeout == 0) {
    timeout = LOS_WAIT_FOREVER;
  } else {
    timeout = LOS_MS2Tick(timeout);
    if (!timeout) {
      timeout = 1;
    }
  }
  retval = LOS_SemPend(LWIP_SEM_ID(sem), timeout);
  if (retval == LOS_ERRNO_SEM_TIMEOUT) {
    return SYS_ARCH_TIMEOUT;
  }
  if (retval != ERR_OK) {
    return SYS_ARCH_ERROR;
  }

  end_tick = LOS_TickCountGet();
  /* Here milli second will not come more than 32 bit because timeout received as 32 bit millisecond only */
  return (u32_t)(((end_tick - start_tick) * 1000) / LOSCFG_BASE_CORE_TICK_PER_SECOND);
}


/*
 * Routine:  sys_sem_signal
 *
 * Description:
 *      Signals (releases) a semaphore
 * Inputs:
 *      sys_sem_t sem           -- Semaphore to signal
 */
void sys_sem_signal(sys_sem_t *sem)
{
  u32_t ret;

  if ((sem == NULL) || (sem->sem == NULL)) {
    return;
  }

  ret = LOS_SemPost(LWIP_SEM_ID(sem));
  LWIP_ASSERT("LOS_SemPost failed", (ret == 0));
  UNUSED(ret);

  return;
}


/*
 * Routine:  sys_sem_free
 *
 * Description:
 *      Deallocates a semaphore
 * Inputs:
 *      sys_sem_t sem           -- Semaphore to free
 */
void sys_sem_free(sys_sem_t *sem)
{
  u32_t ret;

  if ((sem == NULL) || (sem->sem == NULL)) {
    return;
  }

  ret = LOS_SemDelete(LWIP_SEM_ID(sem));
  sem->sem = NULL;
  LWIP_ASSERT("LOS_SemDelete failed", (ret == 0));
  UNUSED(ret);
  return;
}

#if !LWIP_COMPAT_MUTEX
/* Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex */
err_t sys_mutex_new(sys_mutex_t *mutex)
{
  u32_t ret;
#ifdef LWIP_LITEOS_A_COMPAT
  ret = osal_mutex_init(mutex);
#else
  ret = LOS_MuxCreate(mutex);
#endif
  LWIP_ASSERT("sys_mutex_new failed", (ret == LOS_OK));
  return ret;
}
/* Lock a mutex
 * @param mutex the mutex to lock */
void sys_mutex_lock(sys_mutex_t *mutex)
{
  u32_t ret;
#ifdef LWIP_LITEOS_A_COMPAT
  ret = osal_mutex_lock(mutex);
#else
  ret = LOS_MuxPend(*mutex, LOS_WAIT_FOREVER);
#endif
  LWIP_ASSERT("sys_mutex_lock failed", (ret == LOS_OK));
  UNUSED(ret);
  return;
}
/* Unlock a mutex
 * @param mutex the mutex to unlock */
void sys_mutex_unlock(sys_mutex_t *mutex)
{
  u32_t ret;
#ifdef LWIP_LITEOS_A_COMPAT
  ret = osal_mutex_unlock(mutex);
#else
  ret = LOS_MuxPost(*mutex);
#endif
  LWIP_ASSERT("sys_mutex_unlock failed", (ret == LOS_OK));
  UNUSED(ret);
  return;
}
/* Delete a semaphore
 * @param mutex the mutex to delete */
void sys_mutex_free(sys_mutex_t *mutex)
{
  u32_t ret;
#ifdef LWIP_LITEOS_A_COMPAT
  ret = osal_mutex_destroy(mutex);
#else
  ret = LOS_MuxDelete(*mutex);
#endif
  LWIP_ASSERT("sys_mutex_free failed", (ret == LOS_OK));
  UNUSED(ret);
  return;
}
#endif
