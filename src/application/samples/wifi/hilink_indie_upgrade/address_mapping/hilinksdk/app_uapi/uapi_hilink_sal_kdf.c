/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Key derivation algorithm adaptation layer interface implementation. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_sal_kdf.h"

int HILINK_SAL_Hkdf(const HiLinkHkdfParam *param, unsigned char *key, unsigned int keyLen)
{
    return app_call3(APP_CALL_HILINK_SAL_HKDF, int,
        const HiLinkHkdfParam *, param, unsigned char *, key, unsigned int, keyLen);
}

int HILINK_SAL_Pkcs5Pbkdf2Hmac(const HiLinkPbkdf2HmacParam *param, unsigned char *key, unsigned int keyLen)
{
    return app_call3(APP_CALL_HILINK_SAL_PKCS5_PBKDF2_HMAC, int,
        const HiLinkPbkdf2HmacParam *, param, unsigned char *, key, unsigned int, keyLen);
}