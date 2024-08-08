/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: base64编码解码适配层接口（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_SAL_BASE64_H
#define HILINK_SAL_BASE64_H

#include "hilink_sal_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief base64编码
 *
 * @param inData [IN] 待编码的数据，有效可读长度至少为inLen
 * @param inLen [IN] 待编码的数据长度
 * @param outData [OUT] 编码后输出数据缓冲区，有效可写长度至少为*outLen
 * @param outLen [IN,OUT] 输入为缓冲区长度，输出为编码后的数据长度
 * @return 0成功，非0失败
 * @attention outData为NULL或者outLen为0时，不会实际输出编码数据，只输出编码数据后的长度
 */
int HILINK_SAL_Base64Encode(const unsigned char *inData, unsigned int inLen,
    unsigned char *outData, unsigned int *outLen);

/**
 * @brief base64解码
 *
 * @param inData [IN] 待解码的数据，有效可读长度至少为inLen
 * @param inLen [IN] 待解码的数据长度
 * @param outData [OUT] 解码后输出数据缓冲区，有效可写长度至少为*outLen
 * @param outLen [IN,OUT] 输入为缓冲区长度，输出为解码后的数据长度
 * @return 0成功，非0失败
 * @attention outData为NULL或者outLen为0时，不会实际输出解码数据，只输出解码数据后的长度
 */
int HILINK_SAL_Base64Decode(const unsigned char *inData, unsigned int inLen,
    unsigned char *outData, unsigned int *outLen);

#ifdef __cplusplus
}
#endif

#endif /* HILINK_SAL_BASE64_H */