/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Implementation of the Message Digest Algorithm Adaptation Layer Interface. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_sal_md.h"

int HILINK_SAL_MdCalc(HiLinkMdType type, const unsigned char *inData, unsigned int inLen,
    unsigned char *md, unsigned int mdLen)
{
    return app_call5(APP_CALL_HILINK_SAL_MD_CALC, int, HiLinkMdType, type,
        const unsigned char *, inData, unsigned int, inLen, unsigned char *, md, unsigned int, mdLen);
}

int HILINK_SAL_HmacCalc(const HiLinkHmacParam *param, unsigned char *hmac, unsigned int hmacLen)
{
    return app_call3(APP_CALL_HILINK_SAL_HMAC_CALC, int, const HiLinkHmacParam *, param,
        unsigned char *, hmac, unsigned int, hmacLen);
}

HiLinkMdContext HILINK_SAL_MdInit(HiLinkMdType type)
{
    return app_call1(APP_CALL_HILINK_SAL_MD_INIT, HiLinkMdContext, HiLinkMdType, type);
}

int HILINK_SAL_MdUpdate(HiLinkMdContext ctx, const unsigned char *inData, unsigned int inLen)
{
    return app_call3(APP_CALL_HILINK_SAL_MD_UPDATE, int,
        HiLinkMdContext, ctx, const unsigned char *, inData, unsigned int, inLen);
}

int HILINK_SAL_MdFinish(HiLinkMdContext ctx, unsigned char *outData, unsigned int outLen)
{
    return app_call3(APP_CALL_HILINK_SAL_MD_FINISH, int,
        HiLinkMdContext, ctx, unsigned char *, outData, unsigned int, outLen);
}

void HILINK_SAL_MdFree(HiLinkMdContext ctx)
{
    app_call1(APP_CALL_HILINK_SAL_MD_FREE, void, HiLinkMdContext, ctx);
}