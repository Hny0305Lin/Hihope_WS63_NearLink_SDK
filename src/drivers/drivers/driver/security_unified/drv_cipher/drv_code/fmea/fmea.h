/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: cipher driver fmea. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef FMEA_H
#define FMEA_H

#include "td_type.h"
 
#define CIPHER_INVALID_PARAM_FAULT_CODE 955051801
#define SYMC_INVALID_PARAM_FAULT_CODE 955051802
#define SYMC_TIMEOUT_PARAM_FAULT_CODE 955051803
#define PKE_INVALID_PARAM_FAULT_CODE 955051804
 
/* symc fmea parameter error. symc fmea 参数参数错误信息 */
typedef struct {
    td_char *func_name_1;           /* Top-of-Stack Function. 故障发生时栈顶函数 */
    td_char *func_name_2;           /* The second function in the stack when the fault occurs. 故障发生时栈里第二个函数 */
    td_s32 alg;                     /* Algorithm configured. 故障发生时配置的算法 */
    td_s32 mode;                    /* Indicates the mode configured. 故障发生时配置的模式 */
    td_s32 bit_width;               /* Algorithm bit width configured. 故障发生时配置的算法位宽 */
    td_s32 error_code;              /* Error code. 故障发生时的错误码 */
} fmea_cipher_symc_invalid_param;
 
/* symc fmea timeout error. symc fmea 超时错误信息 */
typedef struct {
    td_char *func_name_1;           /* Top-of-Stack Function. 故障发生时栈顶函数 */
    td_char *func_name_2;           /* The second function in the stack when the fault occurs. 故障发生时栈里第二个函数 */
    td_s32 alg;                     /* Algorithm configured. 故障发生时配置的算法 */
    td_s32 mode;                    /* Indicates the mode configured. 故障发生时配置的模式 */
    td_s32 bit_width;               /* Algorithm bit width configured. 故障发生时配置的算法位宽 */
    td_u32 data_len;                /* Length of decrypted data. 故障发生时配置的解密数据长度 */
    td_u32 timeout;                 /* Timeout interval configured. 故障发生时配置的超时时间 */
    td_s32 error_code;              /* Error code. 故障发生时的错误码 */
} fmea_cipher_symc_timeout_param;
 
/* pke fmea parameter error. pke fmea 参数参数错误信息 */
typedef struct {
    const td_char *func_name_1;     /* Top-of-Stack Function. 故障发生时栈顶函数 */
    const td_char *func_name_2;     /* The second function in the stack when the fault occurs. 故障发生时栈里第二个函数 */
    td_u32 keylen;                  /* Key length when the fault occurs. 故障发生时秘钥长度 */
    td_u32 msglen;                  /* Data length when the fault occurs. 故障发生时数据长度 */
    td_s32 error_code;              /* Error code. 故障发生时的错误码 */
} fmea_cipher_pke_invalid_param;
 
/* Indicates the failure mode function of reporting invalid symc parameters. 上报symc参数无效故障模式函数 */
td_s32 fmea_cipher_symc_invalid_parameter(fmea_cipher_symc_invalid_param *symc_info);
 
/* Function for reporting the symc timeout fault mode. 上报symc超时故障模式函数 */
td_s32 fmea_cipher_symc_timeout(fmea_cipher_symc_timeout_param *symc_timeout_info);
 
/*
 * The function is used to optimize the parameter passing of the fmea_cipher_symc_timeout function.
 * 用于优化fmea_cipher_symc_timeout函数的参数传递。
 */
td_s32 fmea_cipher_symc_timeout_set_func(fmea_cipher_symc_timeout_param *symc_timeout_info,
    td_char *func_name_1, td_char *func_name_2, td_s32 error_code, td_s32 timeout);
 
/*
 * The function is used to optimize the parameter passing of the fmea_cipher_symc_timeout function.
 * 用于优化fmea_cipher_symc_timeout函数的参数传递。
 */
td_s32 fmea_cipher_symc_timeout_set_alg(fmea_cipher_symc_timeout_param *symc_timeout_info,
    td_s32 alg, td_s32 mode, td_s32 bit_width, td_s32 data_len);
 
/* Function for reporting invalid pke parameters. 上报pke参数无效故障模式函数 */
td_s32 fmea_cipher_pke_invalid_parameter(fmea_cipher_pke_invalid_param *pke_info);
 
/*
 * The cipher fmea module can be tailored based on the CONFIG_SOCT_CIPHER_FMEA_SUPPORT configuration item.
 * 可通过CONFIG_SOCT_CIPHER_FMEA_SUPPORT配置项对cipher fmea模块进行裁剪。
 */
#ifdef CONFIG_SOCT_CIPHER_FMEA_SUPPORT
#define fmea_cipher_symc_invalid_param(fmea_func1, fmea_func2, fmea_alg, fmea_mode, fmea_bitwidth, fmea_errcode) do { \
        fmea_cipher_symc_invalid_param _symc_param;                 \
        _symc_param.func_name_1 = fmea_func1;                       \
        _symc_param.func_name_2 = fmea_func2;                       \
        _symc_param.alg = fmea_alg;                                 \
        _symc_param.mode = fmea_mode;                               \
        _symc_param.bit_width = fmea_bitwidth;                      \
        _symc_param.error_code = fmea_errcode;                      \
        (td_void)fmea_cipher_symc_invalid_parameter(&_symc_param);  \
    } while (0)
 
#define fmea_cipher_symc_time_out(fmea_func1, fmea_func2, fmea_alg, fmea_mode, fmea_bitwidth, fmea_datalen,           \
                                  fmea_timeout, fmea_errcode)                                                         \
    do {                                                                                                              \
        fmea_cipher_symc_timeout_param _symc_param ;                                                                  \
                                                                                                                      \
        (td_void)fmea_cipher_symc_timeout_set_func(&_symc_param, fmea_func1, fmea_func2, fmea_errcode, fmea_timeout); \
        (td_void)fmea_cipher_symc_timeout_set_alg(&_symc_param, fmea_alg, fmea_mode, fmea_bitwidth, fmea_datalen);    \
        (td_void)fmea_cipher_symc_timeout(&_symc_param);                                                              \
    } while (0)
 
#define fmea_cipher_pke_invalid_param(fmea_func_1, fmea_func2, fmea_keylen, fmea_msglen, fmea_error_code)    \
    do {                                                            \
        fmea_cipher_pke_invalid_param _pke_param;                   \
                                                                    \
        _pke_param.func_name_1 = fmea_func_1;                       \
        _pke_param.func_name_2 = fmea_func2;                        \
        _pke_param.keylen = fmea_keylen;                            \
        _pke_param.msglen = fmea_msglen;                            \
        _pke_param.error_code = fmea_error_code;                    \
        (td_void)fmea_cipher_pke_invalid_parameter(&_pke_param);    \
    } while (0)
 
#else
#define fmea_cipher_symc_invalid_param(fmea_func_1, fmea_func_2, fmea_alg, fmea_mode, fmea_bit_width, fmea_error_code)
#define fmea_cipher_symc_time_out(fmea_func_1, fmea_func2, fmea_alg, fmea_mode, fmea_bit_width, fmea_data_len,  \
                                  fmea_timeout, fmea_error_code)
#define fmea_cipher_pke_invalid_param(fmea_func_1, fmea_func2, fmea_keylen, fmea_msglen, fmea_error_code)
#endif /* end for CONFIG_SOCT_CIPHER_FMEA_SUPPORT */
 
#endif /* FMEA_H */

