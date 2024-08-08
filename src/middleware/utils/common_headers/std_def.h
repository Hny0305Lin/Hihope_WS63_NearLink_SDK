/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   his should be #included by every C source file
 */

#ifndef STD_DEF_H
#define STD_DEF_H

/**
 * @defgroup connectivity_libs_common_types COMMON Types
 * @ingroup  connectivity_libs_common
 * @{
 */
#include <stddef.h>
#include <stdbool.h>
#include "stdint.h"
#ifdef __LITEOS__
#include "los_typedef.h"
#endif
#include "product.h"

#ifndef var_segment
#if (defined __GNUC__) || (defined __ICCARM__)
#define var_segment(seg) __attribute__((section(seg)))
#else
#if (defined PC_ST)
#define var_segment(seg)
#else
#error "You need to define var_segment for your compiler."
#endif  // (defined PC_ST)
#endif  // (defined __GNUC__ )
#endif  // var_segment

#ifndef UNUSED
#define UNUSED(var) do { (void)(var); } while (0)
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

/** A compile time assertion check.
 *
 *  Validate at compile time that the predicate is true without
 *  generating code. This can be used at any point in a source file
 *  where typedef is legal.
 *
 *  On success, compilation proceeds normally.
 *
 *  On failure, attempts to typedef an array type of negative size. The
 *  offending line will look like
 *      typedef assertion_failed_file_h_42[-1]
 *  where file is the content of the second parameter which should
 *  typically be related in some obvious way to the containing file
 *  name, 42 is the line number in the file on which the assertion
 *  appears, and -1 is the result of a calculation based on the
 *  predicate failing.
 *
 *  \param predicate The predicate to test. It must evaluate to
 *  something that can be coerced to a normal C boolean.
 *
 *  \param file A sequence of legal identifier characters that should
 *  uniquely identify the source file in which this condition appears.
 */
#define cassert(predicate, file) impl_cassert_line(predicate, __LINE__, file)

#define impl_paste(a, b)                         a##b
#define impl_cassert_line(predicate, line, file) \
    typedef char impl_paste(assertion_failed_##file##_, line)[2 * !!(predicate) - 1]

#ifndef BIT
#define BIT(x) (1UL << (uint32_t)(x))
#endif

// Macro to check at compile time certain expressions not supported by the preprocessor
#define ct_assert(e) enum LENGTH_CHECK { ct_assert_value = 1 / ((!(!(e)))) }

#define __IRQ
#define __ISR __IRQ

enum ERROR_CODE_ENUM {
    ERR = -1,
    SUCC = 0
};

/**
 * @}
 */
#endif
