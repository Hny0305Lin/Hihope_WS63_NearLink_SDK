/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides crypto security utils \n
 *
 * History: \n
 * 2023-03-27, Create file. \n
 */
#include "crypto_security.h"

#include <securec.h>
#include "crypto_type.h"
#include "crypto_drv_common.h"

#ifdef SEC_ENHANCE
/* Secure assertions
 * \brief Secured reset on val1 != val2 assert
 * \param val1 first value to check
 * \param val2 second value to check
 */
void drv_assert_eq(unsigned int val1, unsigned int val2)
{
    unsigned int scratch;

    __asm__ __volatile__(
        ".equ SYSTEM_SOFT_RESET_REG, 0x50002100                         \n"
        ".equ SOFT_RESET_VALUE, 0x10                                    \n"
        "    li     %0, 1                                               \n"
        ".rept 2                                                        \n"
        "    li     %0, 1                                               \n"
        "    j      2f                                                  \n"
        ".endr                                                          \n"
        "1:                                                             \n"
        "    j      4f                                                  \n"
        "2:                                                             \n"
        "    beq    %1, %2, 1b                                          \n"
        "3:                                                             \n"
        ".rept 3                                                        \n"
        "    li      %0, SYSTEM_SOFT_RESET_REG                          \n"
        "    lw      %1, 0(%0)                                          \n"
        "    li      %2, SOFT_RESET_VALUE                               \n"
        "    or      %1, %1, %2                                         \n"
        "    sw      %1, 0(%0)            /* Set SC_SYSRES to 1 */      \n"
        "    fence                                                      \n"
        ".endr                                                          \n"
        "4:                                                             \n"
        : /* outputs */
        "=&r"(scratch)
        : /* inputs */
        "r"(val1),
        "r"(val2)
        : /* clobbers */
        "cc"
    );
}

/* for secure enhance. */
int memcmp_ss(const void *cs, const void *ct, unsigned int count, unsigned long check_word)
{
    int ret;
    (void)check_word;
    ret = memcmp(cs, ct, count);
    if (ret != 0) {
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

int memset_ss(void *dest, unsigned int dest_max, unsigned char c, unsigned int count, unsigned long check_word)
{
    int ret;
    (void)check_word;
    ret = memset_s(dest, dest_max, c, count);
    if (ret != EOK) {
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

int memcpy_ss(void *dest, unsigned int dest_max, const void *src, unsigned int count, unsigned long check_word)
{
    int ret;
    (void)check_word;
    ret = memcpy_s(dest, dest_max, src, count);
    if (ret != EOK) {
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

int memcmp_enhance(const void *cs, const void *ct, unsigned int count)
{
    return memcmp_ss(sec_arg_add_cs(cs, ct, count));
}

int memset_enhance(void *dest, unsigned int dest_max, unsigned char c, unsigned int count)
{
    return memset_ss(sec_arg_add_cs(dest, dest_max, c, count));
}

int memcpy_enhance(void *dest, unsigned int dest_max, const void *src, unsigned int count)
{
    return memcpy_ss(sec_arg_add_cs(dest, dest_max, src, count));
}
#else
int memcmp_enhance(const void *cs, const void *ct, unsigned int count)
{
    int ret = memcmp(cs, ct, count);
    crypto_chk_return((ret != EOK), TD_FAILURE, "memcmp failed, ret = 0x%x\r\n", ret);
    return TD_SUCCESS;
}

int memset_enhance(void *dest, unsigned int dest_max, unsigned char c, unsigned int count)
{
    int ret = memset_s(dest, dest_max, c, count);
    crypto_chk_return((ret != EOK), TD_FAILURE, "memset_s failed, ret = 0x%x\r\n", ret);
    return TD_SUCCESS;
}

int memcpy_enhance(void *dest, unsigned int dest_max, const void *src, unsigned int count)
{
    int ret = memcpy_s(dest, dest_max, src, count);
    crypto_chk_return((ret != EOK), TD_FAILURE, "memcpy_s failed, ret = 0x%x\r\n", ret);
    return TD_SUCCESS;
}
#endif