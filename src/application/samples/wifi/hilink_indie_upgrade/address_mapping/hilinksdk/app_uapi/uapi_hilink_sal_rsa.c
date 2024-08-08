/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Implementation of RSA Encryption and Decryption Adaptation Layer Interfaces. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_sal_rsa.h"

HiLinkRsaContext HILINK_SAL_RsaInit(HiLinkRsaPkcs1Mode padding, HiLinkMdType md)
{
    return app_call2(APP_CALL_HILINK_SAL_RSA_INIT, HiLinkRsaContext, HiLinkRsaPkcs1Mode, padding, HiLinkMdType, md);
}

void HILINK_SAL_RsaFree(HiLinkRsaContext ctx)
{
    app_call1(APP_CALL_HILINK_SAL_RSA_FREE, void, HiLinkRsaContext, ctx);
}

int HILINK_SAL_RsaParamImport(HiLinkRsaContext ctx, const HiLinkRsaParam *param)
{
    return app_call2(APP_CALL_HILINK_SAL_RSA_PARAM_IMPORT, int, HiLinkRsaContext, ctx, const HiLinkRsaParam *, param);
}

int HILINK_RsaPkcs1Verify(HiLinkRsaContext ctx, HiLinkMdType md, const unsigned char *hash,
    unsigned int hashLen, const unsigned char *sig, unsigned int sigLen)
{
    return app_call6(APP_CALL_HILINK_RSA_PKCS1_VERIFY, int, HiLinkRsaContext, ctx, HiLinkMdType, md,
        const unsigned char *, hash, unsigned int, hashLen, const unsigned char *, sig, unsigned int, sigLen);
}

int HILINK_RsaPkcs1Decrypt(const HiLinkRsaCryptParam *param, unsigned char *buf, unsigned int *len)
{
    return app_call3(APP_CALL_HILINK_RSA_PKCS1_DECRYPT, int, const HiLinkRsaCryptParam *, param,
        unsigned char *, buf, unsigned int *, len);
}

int HILINK_RsaPkcs1Encrypt(const HiLinkRsaCryptParam *param, unsigned char *buf, unsigned int len)
{
    return app_call3(APP_CALL_HILINK_RSA_PKCS1_ENCRYPT, int, const HiLinkRsaCryptParam *, param,
        unsigned char *, buf, unsigned int, len);
}