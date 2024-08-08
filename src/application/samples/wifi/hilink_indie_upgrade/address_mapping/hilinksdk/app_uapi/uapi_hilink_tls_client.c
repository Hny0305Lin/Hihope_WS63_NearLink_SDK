/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Common operations on the TLS client, including session creation and destruction. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_tls_client.h"

HiLinkTlsClient *HILINK_TlsClientCreate(const char *custom)
{
    return app_call1(APP_CALL_HILINK_TLS_CLIENT_CREATE, HiLinkTlsClient *, const char *, custom);
}

int HILINK_SetTlsClientOption(HiLinkTlsClient *ctx, HiLinkTlsOption option, const void *value, unsigned int len)
{
    return app_call4(APP_CALL_HILINK_SET_TLS_CLIENT_OPTION, int, HiLinkTlsClient *, ctx, HiLinkTlsOption, option,
        const void *, value, unsigned int, len);
}

int HILINK_TlsClientConnect(HiLinkTlsClient *ctx)
{
    return app_call1(APP_CALL_HILINK_TLS_CLIENT_CONNECT, int, HiLinkTlsClient *, ctx);
}

int HILINK_TlsClientGetContextFd(HiLinkTlsClient *ctx)
{
    return app_call1(APP_CALL_HILINK_TLS_CLIENT_GET_CONTEXT_FD, int, HiLinkTlsClient *, ctx);
}

int HILINK_TlsClientRead(HiLinkTlsClient *ctx, unsigned char *buf, size_t len)
{
    return app_call3(APP_CALL_HILINK_TLS_CLIENT_READ, int, HiLinkTlsClient *, ctx, unsigned char *, buf, size_t, len);
}

int HILINK_TlsClientWrite(HiLinkTlsClient *ctx, const unsigned char *buf, size_t len)
{
    return app_call3(APP_CALL_HILINK_TLS_CLIENT_WRITE, int,
        HiLinkTlsClient *, ctx, const unsigned char *, buf, size_t, len);
}

bool HILINK_TlsClientIsValidCert(HiLinkTlsClient *ctx)
{
    return app_call1(APP_CALL_HILINK_TLS_CLIENT_IS_VALID_CERT, bool, HiLinkTlsClient *, ctx);
}

int HILINK_TlsClientFreeResource(HiLinkTlsClient *ctx)
{
    return app_call1(APP_CALL_HILINK_TLS_CLIENT_FREE_RESOURCE, int, HiLinkTlsClient *, ctx);
}