/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides crypto security utils header \n
 *
 * History: \n
 * 2023-03-27, Create file. \n
 */
#ifndef CRYPTO_SECURITY_H
#define CRYPTO_SECURITY_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/************************************************** secure enhance start************************************/
/* here is the standard API define for cooperate with the secure enhance and no secure enhance version.
   you can follow the hal_pke.h to define your own version.
   eg.
   define API: void hal_pke_get_ram(td_u32 ram_section, unsigned char *data, td_u32 data_len CIPHER_CHECK_WORD);
   call API: hal_pke_get_ram(sec_arg_add_cs(ram_section, data, data_len));
   inner chk_sum == check_word: check_sum_inspect(ram_section, data, data_len);
*/

/* for boot default open secure enhance */
#ifdef SEC_ENHANCE
#define CIPHER_CHECK_WORD                  , const unsigned long check_word
#define CRYPTO_ROM_CHECK_WORD       , check_word

#ifndef SEC_CS_CONST
#define SEC_CS_CONST                        0x0
#endif

#define security_inner_sec_count_args(dummy, a, b, c, d, e, f, g, count, ...) count
#define security_sec_count_args(...) security_inner_sec_count_args(dummy, ##__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1, 0)

#define security_sec_cs_1(a) ((unsigned long) ~((unsigned long)a) ^ SEC_CS_CONST)
#define security_sec_cs_2(a, b) ((unsigned long)(a) ^ (unsigned long)(b) ^ SEC_CS_CONST)
#define security_sec_cs_3(a, b, c) ((unsigned long)(a) ^ (unsigned long)(b) ^ (unsigned long)(c) ^ SEC_CS_CONST)
#define security_sec_cs_4(a, b, c, d)   \
    ((unsigned long)(a) ^ (unsigned long)(b) ^ (unsigned long)(c) ^ (unsigned long)(d) ^ SEC_CS_CONST)
#define security_sec_cs_5(a, b, c, d, e) ((unsigned long)(a) ^ (unsigned long)(b) ^ (unsigned long)(c) ^ \
    (unsigned long)(d) ^ (unsigned long)(e) ^ SEC_CS_CONST)
#define security_sec_cs_6(a, b, c, d, e, f) \
    ((unsigned long)(a) ^ (unsigned long)(b) ^ (unsigned long)(c) ^ (unsigned long)(d) ^ (unsigned long)(e) ^ \
    (unsigned long)(f) ^ SEC_CS_CONST)
#define security_sec_cs_7(a, b, c, d, e, f, g) \
    ((unsigned long)(a) ^ (unsigned long)(b) ^ (unsigned long)(c) ^ (unsigned long)(d) ^ (unsigned long)(e) ^ \
    (unsigned long)(f) ^ (unsigned long)(g) ^ SEC_CS_CONST)
#define security_inner_sec_cs(a, ...) security_sec_cs_##a(__VA_ARGS__)
#define security_sec_cs(...) security_inner_sec_cs(__VA_ARGS__)

#define sec_cs(...) security_sec_cs(_sec_count_args(__VA_ARGS__), __VA_ARGS__)

#define check_sum_inspect(err_ret, ...) do {    \
        if (check_word != sec_cs(__VA_ARGS__)) { \
            return err_ret;                     \
        };                                      \
} while (0)

#define check_sum_assert(...) do {    \
    crypto_log_err("drv_assert_eq \n"); \
    drv_assert_eq(check_word, sec_cs(__VA_ARGS__)); \
} while (0)

#define sec_arg_add_cs(...) __VA_ARGS__, sec_cs(__VA_ARGS__)

/* call back for secure enhancement */
#define reg_callback_chk(reg_micro, expect_val) do {        \
    crypto_log_err("drv_assert_eq \n"); \
    drv_assert_eq(pke_reg_read(reg_micro), expect_val);   \
} while (0)
#define val_enhance_chk(chk_val, expect_val) do {  \
    crypto_log_err("drv_assert_eq \n"); \
    drv_assert_eq(chk_val, expect_val);    \
} while (0)
#define ret_enhance_chk(chk_val, expect_val) do {  \
    crypto_log_err("drv_assert_eq \n"); \
    drv_assert_eq(chk_val, expect_val);    \
} while (0)

/* for security functions */
#define memset_enhance_chk_return(ret, dest, destMax, c, count) do { \
    ret = memset_ss(sec_arg_add_cs(dest, destMax, c, count)); \
    crypto_chk_func_return(memset_ss, ret);    \
} while (0)
#define memcpy_enhance_chk_return(ret, dest, destMax, c, count) do { \
    ret = memcpy_ss(sec_arg_add_cs(dest, destMax, c, count)); \
    crypto_chk_func_return(memcpy_ss, ret); \
} while (0)
#define memcmp_enhance_chk_return(ret, cs, ct, count) do { \
    ret = memcmp_ss(sec_arg_add_cs(cs, ct, count)); \
    crypto_chk_func_return(memcmp_ss, ret); \
} while (0)
#define memset_enhance_chk_goto(ret, dest, destMax, c, count, label) do { \
    ret = memset_ss(sec_arg_add_cs(dest, destMax, c, count)); \
    crypto_chk_goto(((ret) != TD_SUCCESS), label, "memset_ss failed, ret = 0x%x\r\n", ret);    \
} while (0)
#define memcpy_enhance_chk_goto(ret, dest, destMax, c, count, label) do { \
    ret = memcpy_ss(sec_arg_add_cs(dest, destMax, c, count)); \
    crypto_chk_goto(((ret) != TD_SUCCESS), label, "memcpy_ss failed, ret = 0x%x\r\n", ret);    \
} while (0)
#define memcmp_enhance_chk_goto(ret, cs, ct, count, label) do { \
    ret = memcmp_ss(sec_arg_add_cs(cs, ct, count)); \
    crypto_chk_goto(((ret) != TD_SUCCESS), label, "memcmp_ss failed, ret = 0x%x\r\n", ret);    \
} while (0)

#else /* SEC_ENHANCE */
#define CIPHER_CHECK_WORD
#define CRYPTO_ROM_CHECK_WORD
#define sec_cs(...)
#define check_sum_inspect(err_ret, ...)
#define check_sum_assert(...)
#define sec_arg_add_cs(...)  __VA_ARGS__
#define reg_callback_chk(reg_micro, expect_val)
#define val_enhance_chk(chk_val, expect_val)
#define ret_enhance_chk(chk_val, expect_val) do {  \
    crypto_chk_return(((chk_val) != (expect_val)), chk_val);    \
} while (0)

/* for security functions */
#define memset_enhance_chk_return(ret, dest, destMax, c, count) do { \
    ret = memset_s(dest, destMax, c, count);  \
    crypto_chk_return(((ret) != EOK), TD_FAILURE, "memset_s failed, ret = 0x%x\r\n", ret);    \
} while (0)

#define memcpy_enhance_chk_return(ret, dest, destMax, c, count) do { \
    ret = memcpy_s(dest, destMax, c, count); \
    crypto_chk_return(((ret) != EOK), TD_FAILURE, "memcpy_s failed, ret = 0x%x\r\n", ret);    \
} while (0)

#define memcmp_enhance_chk_return(ret, cs, ct, count) do { \
    ret = memcmp(cs, ct, count); \
    crypto_chk_return(((ret) != EOK), TD_FAILURE, "memcmp failed, ret = 0x%x\r\n", ret);    \
} while (0)

#define memset_enhance_chk_goto(ret, dest, destMax, c, count, label) do { \
    ret = memset_s(dest, destMax, c, count);  \
    crypto_chk_goto_with_ret(ret, ((ret) != EOK), label, TD_FAILURE, "memset_s failed, ret = 0x%x\r\n", ret);    \
} while (0)

#define memcpy_enhance_chk_goto(ret, dest, destMax, c, count, label) do { \
    ret = memcpy_s(dest, destMax, c, count); \
    crypto_chk_goto_with_ret(ret, ((ret) != EOK), label, TD_FAILURE, "memcpy_s failed, ret = 0x%x\r\n", ret);    \
} while (0)

#define memcmp_enhance_chk_goto(ret, cs, ct, count) do { \
    ret = memcmp(cs, ct, count); \
    crypto_chk_goto_with_ret(ret, ((ret) != EOK), label, TD_FAILURE, "memcmp failed, ret = 0x%x\r\n", ret);    \
} while (0)

#endif /* SEC_ENHANCE */
/************************************************** secure enhance end************************************/

/* for secure enhance. */
int memcmp_ss(const void *cs, const void *ct, unsigned int count, unsigned long check_word);

int memset_ss(void *dest, unsigned int dest_max, unsigned char c, unsigned int count, unsigned long check_word);

int memcpy_ss(void *dest, unsigned int dest_max, const void *src, unsigned int count, unsigned long check_word);

int memcmp_enhance(const void *cs, const void *ct, unsigned int count);

int memset_enhance(void *dest, unsigned int dest_max, unsigned char c, unsigned int count);

int memcpy_enhance(void *dest, unsigned int dest_max, const void *src, unsigned int count);

void drv_assert_eq(unsigned int val1, unsigned int val2);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif