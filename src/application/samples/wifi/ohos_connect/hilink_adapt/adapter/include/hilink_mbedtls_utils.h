/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLink SDK软件适配层TLS基于mbedTLS实现内部接口（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_SAL_TLS_MBED_INNER_H
#define HILINK_SAL_TLS_MBED_INNER_H

#include <stdbool.h>
#include "mbedtls/md.h"
#include "mbedtls/bignum.h"
#include "hilink_sal_md.h"
#include "hilink_sal_mpi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 将HiLink SDK摘要类型转换为mbedTLS摘要类型
 *
 * @param type HiLink摘要类型
 * @return mbedTLS摘要类型
 */
mbedtls_md_type_t GetMbedtlsMdType(HiLinkMdType type);

/**
 * @brief 校验消息摘要长度是否合法
 *
 * @param type HiLink摘要类型
 * @param len 消息摘要缓冲区长度
 * @return true成功，false失败
 */
bool IsMdLenInvalid(HiLinkMdType type, unsigned int len);

/**
 * @brief 将HiLink SDK大数转换为mbedTLS大数指针
 *
 * @param mpi HiLink SDK大数
 * @return mbedTLS大数指针
 */
mbedtls_mpi *GetMbedtlsMpi(HiLinkMpi mpi);

#ifdef __cplusplus
}
#endif

#endif /* HILINK_SAL_TLS_MBED_INNER_H */