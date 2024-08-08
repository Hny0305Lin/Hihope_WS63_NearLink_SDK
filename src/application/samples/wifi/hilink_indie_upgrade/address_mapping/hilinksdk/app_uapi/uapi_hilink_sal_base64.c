/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Base64 encoding and decoding adaptation layer interface implementation. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"

int HILINK_SAL_Base64Encode(const unsigned char *inData, unsigned int inLen,
    unsigned char *outData, unsigned int *outLen)
{
    return app_call4(APP_CALL_HILINK_SAL_BASE64_ENCODE, int, const unsigned char *, inData, unsigned int, inLen,
        unsigned char *, outData, unsigned int *, outLen);
}

int HILINK_SAL_Base64Decode(const unsigned char *inData, unsigned int inLen,
    unsigned char *outData, unsigned int *outLen)
{
    return app_call4(APP_CALL_HILINK_SAL_BASE64_DECODE, int, const unsigned char *, inData, unsigned int, inLen,
        unsigned char *, outData, unsigned int *, outLen);
}