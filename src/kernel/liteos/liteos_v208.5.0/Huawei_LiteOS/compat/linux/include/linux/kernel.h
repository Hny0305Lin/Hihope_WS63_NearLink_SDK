/**
 * @defgroup linux_kernel Linux Kernel Adapt
 * @ingroup linux
 */

#ifndef _LINUX_KERNEL_H
#define _LINUX_KERNEL_H

#include "errno.h"
#include "string.h"
#include "unistd.h"
#include "sys/time.h"
#include "sys/mman.h"
#include "linux/types.h"
#include "linux/slab.h"
#include "asm/semaphore.h"
#include "asm/atomic.h"
#include "linux/spinlock.h"
#include "linux/list.h"
#include "linux/io.h"
/*
 * linux/sched/clock.h is included here because the app codes call the inner interface hi_sched_clock
 * by include the kernel.h, it will be deleted later.
 */
#include "linux/sched/clock.h"
#include "asm/io.h"
#include "asm/bug.h"
#include "mutex.h"
#ifdef LOSCFG_FS_VFS
#include "fs/fs.h"
#endif
#include "los_exc.h"
#include "los_tick.h"
#if defined(LOSCFG_ARCH_ARM_CORTEX_A) || defined(LOSCFG_ARCH_ARM_AARCH64)
#include "arch/mmu_pri.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define printk    dprintf  /* Do not modify for code check */

#define jiffies     LOS_TickCountGet()
#ifndef HZ
#define HZ          KERNEL_TICK_PER_SECOND
#endif

#define SZ_1K       (0x00000400)
#define __init
#define __exit
#define __user

#define ERR_PTR(err) ((void*)(unsigned long)(err))
#define PTR_ERR(err) ((unsigned long)(err))
#define IS_ERR(err)  ((unsigned long)(err) > (unsigned long)-1000L)

#define    COMPAT_KERN_EMERG              "<0>" /* system is unusable */
#define    COMPAT_KERN_ALERT              "<1>" /* action must be taken immediately */
#define    COMPAT_KERN_CRIT               "<2>" /* critical conditions */
#define    COMPAT_KERN_ERR                "<3>" /* error conditions */
#define    COMPAT_KERN_WARNING            "<4>" /* warning conditions */
#define    COMPAT_KERN_NOTICE             "<5>" /* normal but significant condition */
#define    COMPAT_KERN_INFO               "<6>" /* informational */
#define    COMPAT_KERN_DEBUG              "<7>" /* debug-level messages */
#define    COMPAT_KERN_CONT               "<c>"

#ifndef pr_fmt
#define pr_fmt(fmt) fmt
#endif

#define pr_emerg(fmt, ...) \
    dprintf(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__)
#define pr_alert(fmt, ...) \
    dprintf(KERN_ALERT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_crit(fmt, ...) \
    dprintf(KERN_CRIT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_err(fmt, ...) \
    dprintf(KERN_ERR pr_fmt(fmt), ##__VA_ARGS__)
#define pr_warning(fmt, ...) \
    dprintf(KERN_WARNING pr_fmt(fmt), ##__VA_ARGS__)
#define pr_warn pr_warning
#define pr_notice(fmt, ...) \
    dprintf(KERN_NOTICE pr_fmt(fmt), ##__VA_ARGS__)
#define pr_info(fmt, ...) \
    dprintf(KERN_INFO pr_fmt(fmt), ##__VA_ARGS__)
#define pr_cont(fmt, ...) \
    dprintf(KERN_CONT fmt, ##__VA_ARGS__)

/* pr_devel() should produce zero code unless DEBUG is defined */
#ifdef DEBUG
#define pr_devel(fmt, ...) \
    dprintf(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#else
#define pr_devel(fmt, ...) do {} while (0)
#endif
/* If you are writing a driver, please use dev_dbg instead */
#if defined(DEBUG)
#define pr_debug(fmt, ...) \
    dprintf(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#else
#define pr_debug(fmt, ...) do {} while (0)
#endif

#define WARN_ON(condition) do {} while (0)

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max(x, y) ((x) < (y) ? (y) : (x))
#endif

#ifndef min_t
#define min_t(t, x, y) ((t)(x) < (t)(y) ? (t)(x) : (t)(y))
#endif

#define BUG_ON(condition) do {  \
    if (condition) {            \
        BUG();                  \
    }                           \
} while (0)

/* it can not be commented, because it used for business */
#define panic  LOS_Panic

#define __setup(str, fn)

#define in_interrupt()        (0)

/**
 * @ingroup  linux_kernel
 * @brief do division implimentation.
 *
 * @par Description:
 * This API is used to do a division implimentation, and return the remainder.
 *
 * @attention
 * <ul>
 * <li>the param n should point to a valid address.</li>
 * <li>the param base should not be 0.</li>
 * </ul>
 *
 * @param  n    [IN/OUT]     the dividend as IN, the quotient as OUT.
 * @param  base [IN]         the divisor.
 * @retval remainder
 * @par Dependency:
 * <ul><li>kernel.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 do_div_imp(UINT64 *n, UINT32 base);

/**
 * @ingroup  linux_kernel
 * @brief do division implimentation.
 *
 * @par Description:
 * This API is used to do a division implimentation, and return the remainder.
 *
 * @attention
 * <ul>
 * <li>the param n should point to a valid address.</li>
 * <li>the param base should not be 0.</li>
 * </ul>
 *
 * @param  n    [IN/OUT]  the dividend as IN, the quotient as OUT.
 * @param  base [IN]      the divisor > 0.
 * @retval remainder
 * @par Dependency:
 * <ul><li>kernel.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern INT32 do_div_s64_imp(INT64 *n, INT32 base);

/**
 * @ingroup  linux_kernel
 * @brief do division implimentation.
 *
 * @par Description:
 * This API is used to do a division implimentation, and return the quotien.
 *
 * @attention
 * <ul>
 * <li>the param divisor should not be 0.</li>
 * </ul>
 *
 * @param  dividend [IN]     the dividend as IN.
 * @param  divisor  [IN]     the divisor > 0.
 * @retval quotient
 * @par Dependency:
 * <ul><li>kernel.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V200R001C00
 */
static inline UINT64 div64_u64(UINT64 dividend, UINT64 divisor)
{
    return dividend / divisor;
}

/**
 * @ingroup  linux_kernel
 * @brief do division implimentation.
 *
 * @par Description:
 * This API is used to do a division implimentation, and return the quotient.
 *
 * @attention
 * <ul>
 * <li>the param divisor should not be 0.</li>
 * </ul>
 *
 * @param  dividend [IN]     the dividend as IN.
 * @param  divisor  [IN]     the divisor not is 0.
 * @retval quotient
 * @par Dependency:
 * <ul><li>kernel.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V200R001C00
 */
static inline INT64 div64_s64(INT64 dividend, INT64 divisor)
{
    return dividend / divisor;
}

#define do_div(n, base) ({             \
    UINT32 tmpBase = (base);           \
    UINT32 rem;                        \
    rem = ((UINT64)(n)) % tmpBase;     \
    (n) = ((UINT64)(n)) / tmpBase;     \
    rem;                               \
})

/**
 * @ingroup  linux_kernel
 * @brief do division implimentation.
 *
 * @par Description:
 * This API is used to do a division implimentation, and return the quotient, and remainder as OUT.
 *
 * @attention
 * <ul>
 * <li>the param divisor should not be 0.</li>
 * <li>the param remainder should point to a valid address.</li>
 * </ul>
 *
 * @param  dividend  [IN]    the dividend as IN.
 * @param  divisor   [IN]    the divisor is not 0, and as IN.
 * @param  remainder [OUT]   the remainder should point to a valid address. remainder as OUT.
 * @retval quotient
 * @par Dependency:
 * <ul><li>kernel.h: the header file that contains the API declaration and implimentation.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
static inline INT64 div_s64_rem(INT64 dividend, INT32 divisor, INT32 *remainder)
{
    *remainder = dividend % divisor;
    return dividend / divisor;
}

/**
 * @ingroup  linux_kernel
 * @brief do division implimentation.
 *
 * @par Description:
 * This API is used to do a division implimentation, and return the quotient, and remainder as OUT.
 *
 * @attention
 * <ul>
 * <li>the param divisor should be greater than 0.</li>
 * <li>the param remainder should point to a valid address.</li>
 * </ul>
 *
 * @param  dividend  [IN]    the dividend as IN.
 * @param  divisor   [IN]    the divisor is greater than 0, and as IN.
 * @param  remainder [OUT]   the remainder should point to a valid address. remainder as OUT.
 * @retval quotient
 * @par Dependency:
 * <ul><li>kernel.h: the header file that contains the API declaration and implimentation.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
static inline UINT64 div64_u64_rem(UINT64 dividend, UINT64 divisor, UINT64 *remainder)
{
    *remainder = dividend % divisor;
    return dividend / divisor;
}

/**
 * @ingroup  linux_kernel
 * @brief do division implimentation.
 *
 * @par Description:
 * This API is used to do a division implimentation, and return the quotient, and remainder as OUT.
 *
 * @attention
 * <ul>
 * <li>the param divisor should be greater than 0.</li>
 * <li>the param remainder should point to a valid address.</li>
 * </ul>
 *
 * @param  dividend  [IN]    the dividend as IN.
 * @param  divisor   [IN]    the divisor is greater than 0, and as IN.
 * @param  remainder [OUT]   the remainder should point to a valid address. remainder as OUT.
 * @retval quotient
 * @par Dependency:
 * <ul><li>kernel.h: the header file that contains the API declaration and implimentation.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
static inline UINT64 div_u64_rem(UINT64 dividend, UINT32 divisor, UINT32 *remainder)
{
    *remainder = dividend % divisor;
    return dividend / divisor;
}

/**
 * @ingroup  linux_kernel
 * @brief do division implimentation.
 *
 * @par Description:
 * This API is used to do a division implimentation, and return the quotient.
 *
 * @attention
 * <ul>
 * <li>the param divisor should not be 0.</li>
 * </ul>
 *
 * @param dividend [IN]     the dividend as IN
 * @param divisor  [IN]     the divisor is not 0, and as IN.
 * @retval quotient
 * @par Dependency:
 * <ul><li>kernel.h: the header file that contains the API declaration and implimentation.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
static inline INT64 div_s64(INT64 dividend, INT32 divisor)
{
    INT32 remainder;
    return div_s64_rem(dividend, divisor, &remainder);
}

/**
 * @ingroup  linux_kernel
 * @brief do division implimentation.
 *
 * @par Description:
 * This API is used to do a division implimentation, and return the quotient.
 *
 * @attention
 * <ul>
 * <li>the param divisor should be greater than 0.</li>
 * </ul>
 *
 * @param dividend [IN]     the dividend as IN.
 * @param divisor  [IN]     the divisor is greater than 0, and as IN.
 * @retval quotient
 * @par Dependency:
 * <ul><li>kernel.h: the header file that contains the API declaration and implimentation.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
static inline UINT64 div_u64(UINT64 dividend, UINT32 divisor)
{
    UINT32 remainder;
    return div_u64_rem(dividend, divisor, &remainder);
}

static inline unsigned long copy_from_user(void *to, const void *from, unsigned long n)
{
    if ((to == NULL) || (from == NULL)) {
        return (unsigned long)-1;
    }
    (VOID)memcpy(to, from, n);
    return 0;
}

static inline unsigned long copy_to_user(void *to, const void *from, unsigned long n)
{
    if ((to == NULL) || (from == NULL)) {
        return (unsigned long)-1;
    }
    (VOID)memcpy(to, from, n);
    return 0;
}

static inline void *ioremap_cached(unsigned long physAddr, unsigned int size)
{
#if defined(LOSCFG_ARCH_ARM_CORTEX_A) || defined(LOSCFG_ARCH_ARM_AARCH64)
    return (void *)OsCachedRemap(physAddr, size);
#else
    (void)size;
    return (void *)(UINTPTR)physAddr;
#endif
}

static inline void *ioremap_nocache(unsigned long physAddr, unsigned long size)
{
#if defined(LOSCFG_ARCH_ARM_CORTEX_A) || defined(LOSCFG_ARCH_ARM_AARCH64)
    return (void *)OsNoCachedRemap(physAddr, size);
#else
    (void)size;
    return (void *)(UINTPTR)physAddr;
#endif
}

static inline void iounmap(void *addr)
{
}

#define likely(x)       (x)
#define unlikely(x)     (x)
#define EXPORT_SYMBOL(x)

typedef VOID (*unused_func_t)(VOID);

struct file_operations {
    struct module *owner;
    unused_func_t   llseek;
    unused_func_t   read;
    unused_func_t   write;
    unused_func_t   aio_read;
    unused_func_t   aio_write;
    unused_func_t   readdir;
    unused_func_t   poll;
    unused_func_t   unlocked_ioctl;
    unused_func_t   compat_ioctl;
    unused_func_t   mmap;
    unused_func_t   open;
    unused_func_t   flush;
    unused_func_t   release;
    unused_func_t   fsync;
    unused_func_t   aio_fsync;
    unused_func_t   fasync;
    unused_func_t   lock;
    unused_func_t   sendpage;
    unused_func_t   get_unmapped_area;
    unused_func_t   check_flags;
    unused_func_t   flock;
    unused_func_t   splice_write;
    unused_func_t   splice_read;
    unused_func_t   setlease;
    unused_func_t   fallocate;
};

#define simple_strtol       strtol
#define do_gettimeofday(a)  gettimeofday(a, NULL)
#define DEFINE_MUTEX(m)     pthread_mutex_t m
#define mutex_lock          pthread_mutex_lock
#define mutex_unlock        pthread_mutex_unlock
#define mutex_init(m)       pthread_mutex_init(m, NULL)
#define mutex_destroy(m)    pthread_mutex_destroy((m))

static inline void printtime(void)
{
    struct timeval time;

    (VOID)gettimeofday(&time, NULL);
    PRINT_INFO("[time:%ld.%03ld]", time.tv_sec, time.tv_usec / 1000); /* 1000: millisecond to microseconds */
    return;
}

#define TRACETIME() printtime()

extern void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
extern int munmap(void *addr, size_t length);

#define    KERN_EMERG              COMPAT_KERN_EMERG
#define    KERN_ALERT              COMPAT_KERN_ALERT
#define    KERN_CRIT               COMPAT_KERN_CRIT
#define    KERN_ERR                COMPAT_KERN_ERR
#define    KERN_WARNING            COMPAT_KERN_WARNING
#define    KERN_NOTICE             COMPAT_KERN_NOTICE
#define    KERN_INFO               COMPAT_KERN_INFO
#define    KERN_DEBUG              COMPAT_KERN_DEBUG
#define    KERN_CONT               COMPAT_KERN_CONT

/**
 * @ingroup linux_kernel
 * @brief Gets the address of the container structure.
 *
 * @par Description:
 * This API gets the address of the structure.
 * @attention
 * None.
 *
 * @param ptr     [IN] The pointer to the #member.
 * @param type    [IN] The type of structure.
 * @param member  [IN] The name of the #member in the structure.
 *
 * @retval The pointer to the address of the container structure.
 * @par Dependency:
 * <ul><li>kernel.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
#define container_of    LOS_DL_LIST_ENTRY

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_KERNEL_H */
