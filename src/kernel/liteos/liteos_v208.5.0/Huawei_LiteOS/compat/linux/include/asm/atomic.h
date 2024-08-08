/**
 * @defgroup atomic Atomic
 * @ingroup linux
 */

#ifndef _ASM_ATOMIC_H
#define _ASM_ATOMIC_H

#include "los_atomic.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef Atomic atomic_t;
typedef Atomic64 atomic64_t;

/**
 * @ingroup  atomic
 * @brief Atomic init.
 *
 * @par Description:
 * This API is used to initialize the atomic variable based on the value of #x.
 * @attention
 * None.
 *
 * @param  x      [IN] The value to be set.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>atomic.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
#define ATOMIC_INIT(x)                  (x)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_AtomicRead.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic_read(v)                  LOS_AtomicRead(v)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_AtomicSet.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic_set(v, i)                LOS_AtomicSet(v, i)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_AtomicInc.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic_inc(v)                   LOS_AtomicInc(v)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_AtomicDec.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic_dec(v)                   LOS_AtomicDec(v)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_AtomicAdd.
 * @attention
 * <ul>
 * <li>The parameters of this API comply with Linux, but the input parameter sequence
 * is different from that of Linux.</li>
 * </ul>
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic_add(i, v)                LOS_AtomicAdd(v, i)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_AtomicSub.
 * @attention
 * <ul>
 * <li>The parameters of this API comply with Linux, but the input parameter sequence
 * is different from that of Linux.</li>
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic_sub(i, v)                LOS_AtomicSub(v, i)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_AtomicAdd.
 * @attention
 * <ul>
 * <li>The parameters of this API comply with Linux, but the input parameter sequence
 * is different from that of Linux.</li>
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic_add_return(i, v)         LOS_AtomicAdd(v, i)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_AtomicIncRet.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic_inc_return(v)            LOS_AtomicIncRet(v)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_AtomicDecRet.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic_dec_return(v)            LOS_AtomicDecRet(v)
#define atomic_dec_and_test(v)          (atomic_dec_return(v) == 0)

/**
 * @ingroup  atomic
 * @brief Atomic init.
 *
 * @par Description:
 * This API is used to initialize the atomic variable based on the value of #x.
 * @attention
 * None.
 *
 * @param  x      [IN] The value to be set.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>atomic.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
#define ATOMIC64_INIT(x)                (x)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_Atomic64Read.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic64_read(v)                LOS_Atomic64Read(v)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_Atomic64Set.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic64_set(v, i)              LOS_Atomic64Set(v, i)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_Atomic64Inc.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic64_inc(v)                 LOS_Atomic64Inc(v)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_Atomic64Dec.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic64_dec(v)                 LOS_Atomic64Dec(v)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_Atomic64Add.
 * @attention
 * <ul>
 * <li>The parameters of this API comply with Linux, but the input parameter sequence
 * is different from that of Linux.</li>
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic64_add(i, v)              LOS_Atomic64Add(v, i)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_Atomic64Sub.
 * @attention
 * <ul>
 * <li>The parameters of this API comply with Linux, but the input parameter sequence
 * is different from that of Linux.</li>
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic64_sub(i, v)              LOS_Atomic64Sub(v, i)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_Atomic64Add.
 * @attention
 * <ul>
 * <li>The parameters of this API comply with Linux, but the input parameter sequence
 * is different from that of Linux.</li>
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic64_add_return(i, v)       LOS_Atomic64Add(v, i)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_Atomic64IncRet.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic64_inc_return(v)          LOS_Atomic64IncRet(v)

/**
 * @ingroup  atomic
 * @brief It is same with #LOS_Atomic64DecRet.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 */
#define atomic64_dec_return(v)          LOS_Atomic64DecRet(v)
#define atomic64_dec_and_test(v)        (atomic64_dec_return(v) == 0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASM_ATOMIC_H */
