/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: TLS 客户端常用操作，包括会话的创建、销毁等（此文件为DEMO，需集成方适配修改）
 * Note: 由于此文件在采用外部mbedtls编译的时候会打包到外部，所以修改本函数代码的时候
 *       不应涉及引用hilink库编译要使用的宏以及非对外头文件函数
 */

#include "hilink_tls_client.h"

#include <stdlib.h>
#include <errno.h>

#include "securec.h"

#include "hilink_sal_defines.h"
#include "hilink_str_adapter.h"
#include "hilink_mem_adapter.h"
#include "hilink_time_adapter.h"
#include "hilink_stdio_adapter.h"
#include "hilink_stdio_adapter.h"
#include "hilink_thread_adapter.h"

#include "mbedtls/ssl.h"
#include "hilink_sal_drbg.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509.h"
#include "mbedtls/platform_time.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/platform.h"

/* 检查宏定义是否一致 */
#if (HILINK_MBEDTLS_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 != \
    MBEDTLS_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256)
#error "HILINK_MBEDTLS_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256"
#endif
#if (HILINK_MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384 != \
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384)
#error "HILINK_MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384"
#endif
#if (HILINK_MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 != \
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256)
#error "HILINK_MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256"
#endif
#if (HILINK_MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256 != \
    MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256)
#error "HILINK_MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256"
#endif
#if (HILINK_MBEDTLS_TLS_PSK_WITH_AES_256_GCM_SHA384 != \
    MBEDTLS_TLS_PSK_WITH_AES_256_GCM_SHA384)
#error "HILINK_MBEDTLS_TLS_PSK_WITH_AES_256_GCM_SHA384"
#endif

#if (HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_NONE != \
    MBEDTLS_SSL_MAX_FRAG_LEN_NONE)
#error "HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_NONE"
#endif
#if (HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_512 != \
    MBEDTLS_SSL_MAX_FRAG_LEN_512)
#error "HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_512"
#endif
#if (HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_1024 != \
    MBEDTLS_SSL_MAX_FRAG_LEN_1024)
#error "HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_1024"
#endif
#if (HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_2048 != \
    MBEDTLS_SSL_MAX_FRAG_LEN_2048)
#error "HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_2048"
#endif
#if (HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_4096 != \
    MBEDTLS_SSL_MAX_FRAG_LEN_4096)
#error "HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_4096"
#endif

#ifndef MS_PER_SECOND
#define MS_PER_SECOND 1000
#endif
#ifndef US_PER_MS
#define US_PER_MS 1000
#endif
#define MAX_PORT_STR_LEN 20
#define MAX_CUSTOM_STR_LEN 32
#define MAX_HOSTNAME_STR_LEN 64

#ifdef MBEDTLS_DEBUG_C
#undef HILINK_TLS_DEBUG /* 用于调试tls通信，默认不开 */
#endif /* MBEDTLS_DEBUG_C */

#define MBEDTLS_DEBUG_LEVEL 4
#define TLS_READ_TIMEOUT_MS 10
#define TLS_HANDSHAKE_TIMEOUT 4000
#define TLS_WRITE_TIMEOUT_MS 10000

#ifndef TCP_COAP_MAX_PDU_SIZE
#define TCP_COAP_MAX_PDU_SIZE 65536
#endif

#ifdef MBEDTLS_VERSION_3
#define HILINK_MBEDTLS_ENTROPY_MIN_HARDCLOCK 4
#endif

struct HiLinkTlsClient {
    char *custom;
    /* 算法套件白名单相关数据 */
    int *supportedCiphersuites;
    /* 证书相关数据 */
    const char **certs;
    unsigned int certsNum;
    mbedtls_x509_time validFrom;
    mbedtls_x509_time validTo;
    bool isDelayVerifyCert;
    /* mbedtls相关数据 */
    mbedtls_net_context serverFd;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    HiLinkDrbgContext drbg;
    mbedtls_x509_crt caCert;
    unsigned short port;
    char *hostName;
};

static HiLinkMbedtlsGetTimeCb g_mbedtlsGetTimeCb = NULL;

typedef struct {
    HiLinkTlsOption option;
    int (*setOptionFunc)(HiLinkTlsClient *ctx, const void *value, unsigned int len);
} OptionItem;

static unsigned int GetTlsTimeoutTime(unsigned int packetLen)
{
    return ((packetLen / TCP_COAP_MAX_PDU_SIZE) + 1) * TLS_WRITE_TIMEOUT_MS;
}

static int GetOsTime(unsigned long *ms)
{
    if (ms == NULL) {
        return HILINK_SAL_NOK;
    }
    HiLinkTimeval cur = {0, 0};

    if (HILINK_GetOsTime(&cur) != 0) {
        return HILINK_SAL_TIME_ERR;
    }

    *ms = (cur.sec * MS_PER_SECOND) + (cur.usec / US_PER_MS);

    return HILINK_SAL_OK;
}

static unsigned long DeltaTime(unsigned long timeNew, unsigned long timeOld)
{
    unsigned long deltaTime;

    if (timeNew >= timeOld) {
        deltaTime = timeNew - timeOld;
    } else {
        deltaTime = ((unsigned long)(-1) - timeOld) + timeNew + 1; /* 处理时间翻转 */
    }

    return deltaTime;
}

static mbedtls_time_t MbedtlsPlatformGetTime(mbedtls_time_t *timeNow)
{
    (void)timeNow; /* 此参数可能为空 */

    time_t rawSeconds = 0;
    unsigned long long timeMs = 0;
    if (g_mbedtlsGetTimeCb == NULL) {
        HILINK_SAL_NOTICE("no cb\r\n");
        return (mbedtls_time_t)rawSeconds;
    }
    int ret = g_mbedtlsGetTimeCb(&timeMs);
    if (ret == HILINK_SAL_OK) {
        rawSeconds = (time_t)(timeMs / MS_PER_SECOND);
    } else {
        HILINK_SAL_NOTICE("get local time failed, set rawSeconds=0\r\n");
    }

    return (mbedtls_time_t)rawSeconds;
}

static void *MbedtlsPlatformCalloc(size_t n, size_t size)
{
    if ((n == 0) || (size == 0)) {
        return NULL;
    }
    if (n > (UINT_MAX / size)) {
        return NULL;
    }

    /* 上层调用保证不会出现溢出 */
    unsigned int len = (unsigned int)(n * size);
    void *data = HILINK_Malloc(len);
    if (data != NULL) {
        (void)memset_s(data, len, 0, len);
    }

    return data;
}

static void InitMbedtls(void)
{
    static bool isInitmbedtls = false;
    if (!isInitmbedtls) {
        mbedtls_platform_set_calloc_free(MbedtlsPlatformCalloc, HILINK_Free);
        (void)mbedtls_platform_set_time(MbedtlsPlatformGetTime);
#if defined(HILINK_TLS_DEBUG)
        mbedtls_debug_set_threshold(MBEDTLS_DEBUG_LEVEL);
#endif /* HILINK_TLS_DEBUG */
        isInitmbedtls = true;
    }
}

#if defined(HILINK_TLS_DEBUG)
static void TlsDebug(void *context, int level, const char *file, int line, const char *str)
{
    (void) level;
    (void) file;
    (void) line;
    HiLinkTlsClient *ctx = (HiLinkTlsClient *)(context);
    HILINK_SAL_INFO("custom=%s,mbeddebug: %s", ctx->custom, str);
}
#endif /* HILINK_TLS_DEBUG */

static void InitTlsContextData(HiLinkTlsClient *context)
{
    mbedtls_ssl_init((mbedtls_ssl_context *)&context->ssl);
    mbedtls_x509_crt_init((mbedtls_x509_crt *)&context->caCert);
    mbedtls_net_init((mbedtls_net_context *)&context->serverFd);
    mbedtls_ssl_config_init((mbedtls_ssl_config *)&context->conf);
}

static int InitTlsContext(HiLinkTlsClient *context)
{
    InitTlsContextData(context);
#if defined(HILINK_TLS_DEBUG)
    mbedtls_ssl_conf_dbg(&(context->conf), TlsDebug, context);
#endif /* HILINK_TLS_DEBUG */

    return HILINK_SAL_OK;
}

/* 该函数为注册到mbedtls中获取随机数的回调 */
static int SslRngGenerater(void *param, unsigned char *output, size_t len)
{
    HiLinkDrbgContext drbg = (HiLinkDrbgContext)param;
    if ((drbg == NULL) || (output == NULL) || (len == 0)) {
        HILINK_SAL_ERROR("param invalid\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    return HILINK_SAL_DrbgRandom(drbg, output, len);
}

static int SetTlsRandomMember(HiLinkTlsClient *context, const char *custom)
{
    context->drbg = HILINK_SAL_DrbgInit(custom);
    if (context->drbg == NULL) {
        HILINK_SAL_ERROR("drbg init error\r\n");
        return HILINK_SAL_NOK;
    }

    return HILINK_SAL_OK;
}

static bool X509CheckTime(const mbedtls_x509_time *before, const mbedtls_x509_time *after)
{
    if (before->year > after->year) {
        return true;
    } else if (before->year < after->year) {
        return false;
    }
    if (before->mon > after->mon) {
        return true;
    } else if (before->mon < after->mon) {
        return false;
    }
    if (before->day > after->day) {
        return true;
    } else if (before->day < after->day) {
        return false;
    }
    if (before->hour > after->hour) {
        return true;
    } else if (before->hour < after->hour) {
        return false;
    }
    if (before->min > after->min) {
        return true;
    } else if (before->min < after->min) {
        return false;
    }
    if (before->sec > after->sec) {
        return true;
    } else if (before->sec < after->sec) {
        return false;
    }

    return true;
}

static int RecordVaildCertTime(HiLinkTlsClient *context, mbedtls_x509_crt *crt)
{
    HILINK_SAL_DEBUG("vaild cert validFrom=%04d/%02d/%02d %02d:%02d:%02d\r\n",
        crt->valid_from.year, crt->valid_from.mon, crt->valid_from.day,
        crt->valid_from.hour, crt->valid_from.min, crt->valid_from.sec);
    HILINK_SAL_DEBUG("vaild cert validTo=%04d/%02d/%02d %02d:%02d:%02d\r\n",
        crt->valid_to.year, crt->valid_to.mon, crt->valid_to.day,
        crt->valid_to.hour, crt->valid_to.min, crt->valid_to.sec);
    /* 取证书验证通过里面最大的有效时间 */
    if (!X509CheckTime(&context->validTo, &crt->valid_to)) {
        if ((memcpy_s(&context->validFrom, sizeof(mbedtls_x509_time),
            &crt->valid_from, sizeof(mbedtls_x509_time)) != EOK) ||
            (memcpy_s(&context->validTo, sizeof(mbedtls_x509_time),
                &crt->valid_to, sizeof(mbedtls_x509_time)) != EOK)) {
            HILINK_SAL_ERROR("memcpy_s\r\n");
            return HILINK_SAL_NOK;
        }
    }
    HILINK_SAL_DEBUG("record cert validFrom=%04d/%02d/%02d %02d:%02d:%02d\r\n",
        context->validFrom.year, context->validFrom.mon, context->validFrom.day,
        context->validFrom.hour, context->validFrom.min, context->validFrom.sec);
    HILINK_SAL_DEBUG("record cert validTo=%04d/%02d/%02d %02d:%02d:%02d\r\n",
        context->validTo.year, context->validTo.mon, context->validTo.day,
        context->validTo.hour, context->validTo.min, context->validTo.sec);

    return HILINK_SAL_OK;
}

static int VerifyCertProc(void *data, mbedtls_x509_crt *crt, uint32_t *flags)
{
    if (data == NULL) {
        HILINK_SAL_ERROR("invaild context\r\n");
        return HILINK_SAL_NOK;
    }

    int ret;
    HiLinkTlsClient *context = (HiLinkTlsClient *)data;
    for (unsigned int i = 0; i < context->certsNum; i++) {
        mbedtls_x509_crt caCert;
        mbedtls_x509_crt_init(&caCert);
        ret = mbedtls_x509_crt_parse(&caCert, (const unsigned char *)context->certs[i],
            HILINK_Strlen(context->certs[i]) + 1);
        if (ret != 0) {
            mbedtls_x509_crt_free(&caCert);
            HILINK_SAL_WARN("i=%u,ret=[-0x%04x]\r\n", i, -ret);
            continue;
        }
        ret = mbedtls_x509_crt_verify(crt, &caCert, NULL, NULL, flags, NULL, NULL);
        mbedtls_x509_crt_free(&caCert);
        HILINK_SAL_DEBUG("i=%u,flags=%08x,ret=[-0x%04x]\r\n", i, *flags, -ret);
        if (ret == 0) {
            if ((*flags) == 0) {
                if (RecordVaildCertTime(context, crt) != HILINK_SAL_OK) {
                    HILINK_SAL_ERROR("record vaild cert time\r\n");
                    return HILINK_SAL_NOK;
                }
                HILINK_SAL_DEBUG("verify cert okay\r\n");
                return HILINK_SAL_OK;
            }
        } else if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
            /* 只处理校验时间的错误 */
            if (((*flags) == MBEDTLS_X509_BADCERT_EXPIRED) || ((*flags) == MBEDTLS_X509_BADCERT_FUTURE) ||
                ((*flags) == (MBEDTLS_X509_BADCERT_EXPIRED + MBEDTLS_X509_BADCERT_FUTURE))) {
                if (RecordVaildCertTime(context, crt) != HILINK_SAL_OK) {
                    HILINK_SAL_ERROR("record vaild cert time\r\n");
                    return HILINK_SAL_NOK;
                }
                if (context->isDelayVerifyCert) {
                    /* 此时先不检查证书时间，待同步到时间后再校验 */
                    HILINK_SAL_NOTICE("delay verify cert\r\n");
                    *flags &= ~(MBEDTLS_X509_BADCERT_EXPIRED | MBEDTLS_X509_BADCERT_FUTURE);
                    return HILINK_SAL_OK;
                }
            }
        }
    }

    return HILINK_SAL_NOK;
}

static int VerifyCertCb(void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
    (void)depth;
    if ((data == NULL) || (crt == NULL) || (flags == NULL)) {
        HILINK_SAL_ERROR("param invaild\r\n");
        return HILINK_SAL_NOK;
    }

#if defined(HILINK_TLS_DEBUG)
    char buf[64] = {0}; /* 64为缓冲buf大小 */
    HILINK_SAL_NOTICE("\nVerify requested for (Depth %d):\n", depth);
    mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);
    HILINK_SAL_NOTICE("%s", buf);
    if ((*flags) == 0) {
        HILINK_SAL_NOTICE("\n  This certificate has no flags\n");
    } else {
        mbedtls_x509_crt_verify_info(buf, sizeof(buf), "  ! ", *flags);
        HILINK_SAL_NOTICE("%s\n", buf);
    }
#endif /* HILINK_TLS_DEBUG */

    if (VerifyCertProc(data, crt, flags) != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("verify cert\r\n");
    }
    return HILINK_SAL_OK;
}

static int SetTlsConfigMaxFragLen(HiLinkTlsClient *context, unsigned char maxFragLen)
{
    int ret;
    if (maxFragLen != HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_DEFAULT) {
        ret = mbedtls_ssl_conf_max_frag_len(&context->conf, maxFragLen);
        if (ret != 0) {
            HILINK_SAL_ERROR("ret=[-0x%04x]\r\n", -ret);
            return HILINK_SAL_NOK;
        }
    }
    return HILINK_SAL_OK;
}

static int SetTlsConfigBase(HiLinkTlsClient *context)
{
    int ret = mbedtls_ssl_config_defaults(&context->conf, MBEDTLS_SSL_IS_CLIENT,
        MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0) {
        HILINK_SAL_ERROR("ret=[-0x%04x]\r\n", -ret);
        return HILINK_SAL_NOK;
    }
    mbedtls_ssl_conf_rng(&context->conf, SslRngGenerater, context->drbg);
    mbedtls_ssl_conf_min_version(&context->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
    mbedtls_ssl_conf_max_version(&context->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
    mbedtls_ssl_conf_authmode(&context->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    return HILINK_SAL_OK;
}

static int SetTlsConfigCiphersuites(HiLinkTlsClient *context,
    const int *ciphersuiteWhiteList, const unsigned int ciphersuiteWhiteListNum)
{
    if (ciphersuiteWhiteList != NULL && ciphersuiteWhiteListNum != 0 && context->supportedCiphersuites == NULL) {
        /* 支持的白名单算法库肯定比白名单数量少，ciphersuites数组以0为结尾，需预留 */
        unsigned int maxCiphersuitesNum = ciphersuiteWhiteListNum;
        int maxSupportedCiphersuitesSize = (maxCiphersuitesNum + 1) * sizeof(int);
        int *supportedCiphersuites = HILINK_Malloc(maxSupportedCiphersuitesSize);
        if (supportedCiphersuites == NULL) {
            HILINK_SAL_ERROR("malloc\r\n");
            return HILINK_SAL_NOK;
        }
        (void)memset_s(supportedCiphersuites, maxSupportedCiphersuitesSize, 0, maxSupportedCiphersuitesSize);
        /* 从白名单中找到库里支持的 */
        int *supportSuite = supportedCiphersuites;
        for (unsigned int i = 0; i < maxCiphersuitesNum; i++) {
            if (mbedtls_ssl_ciphersuite_from_id(ciphersuiteWhiteList[i]) == NULL) {
                HILINK_SAL_NOTICE("i=[%u],ciphersuiteWhiteList=[%04x]\r\n", i, ciphersuiteWhiteList[i]);
                continue;
            }
            if (supportSuite < supportedCiphersuites + maxCiphersuitesNum) {
                *supportSuite = ciphersuiteWhiteList[i];
                ++supportSuite;
            }
        }
        *supportSuite = 0;
        if (supportSuite == supportedCiphersuites) {
            HILINK_Free(supportedCiphersuites);
            HILINK_SAL_ERROR("no support ciphersuites\r\n");
            return HILINK_SAL_NOK;
        }
        context->supportedCiphersuites = supportedCiphersuites;
        mbedtls_ssl_conf_ciphersuites(&context->conf, context->supportedCiphersuites);
    }

    return HILINK_SAL_OK;
}

static int SetTlsConfigCert(HiLinkTlsClient *context,
    const char **caCerts, const unsigned int certsNum, const bool isDelayVerifyCert)
{
    (void)context;
    if ((caCerts != NULL) && (certsNum != 0)) {
        /* 对于多个证书，这里只注入一个证书，其他证书在回调函数里验证 */
        int ret = mbedtls_x509_crt_parse(&context->caCert, (const unsigned char *)caCerts[0], strlen(caCerts[0]) + 1);
        if (ret < 0) {
            HILINK_SAL_ERROR("ret=[-0x%04x]\r\n", -ret);
            return HILINK_SAL_NOK;
        }
        mbedtls_ssl_conf_ca_chain(&context->conf, &context->caCert, NULL);
        mbedtls_ssl_conf_verify(&context->conf, VerifyCertCb, context);
        context->certs = caCerts;
        context->certsNum = certsNum;
        context->isDelayVerifyCert = isDelayVerifyCert;
    }

    return HILINK_SAL_OK;
}

static int SetTlsConfigPsk(HiLinkTlsClient *context,
    const unsigned char *psk, const size_t pskLen, const unsigned char *pskIdentity, const size_t pskIdentityLen)
{
    (void)context;
    if ((psk != NULL) && (pskLen != 0) && (pskIdentity != NULL) && (pskIdentityLen != 0)) {
        /*
         * 如下是配置psk加密的tls客户端，现在hilink用在与中枢的连接
         */
#if (defined(HILINK_SDK_BUILD_IN) && defined(SUPPORT_CENTRAL_TLS)) || \
    (!defined(HILINK_SDK_BUILD_IN))
        int ret = mbedtls_ssl_conf_psk(&context->conf, psk, pskLen, pskIdentity, pskIdentityLen);
        if (ret < 0) {
            HILINK_SAL_ERROR("ret=[-0x%04x]\r\n", -ret);
            return HILINK_SAL_NOK;
        }
#endif
    }
    return HILINK_SAL_OK;
}

static int InitTlsSocket(HiLinkTlsClient *context)
{
    int ret;
    if (context->serverFd.fd < 0) {
        if (context->hostName != NULL) {
            char portStr[MAX_PORT_STR_LEN];
            if (sprintf_s(portStr, sizeof(portStr), "%u", context->port) <= 0) {
                HILINK_SAL_ERROR("sprintf_s error\r\n");
                return HILINK_SAL_SPRINTF_ERR;
            }
            ret = mbedtls_net_connect(&context->serverFd, context->hostName, portStr,
                MBEDTLS_NET_PROTO_TCP);
            if (ret != 0) {
                HILINK_SAL_ERROR("ret=[-0x%04x],errno=[%d]\r\n", -ret, errno);
                return ret;
            }
        } else {
            HILINK_SAL_ERROR("no hostname\r\n");
            return HILINK_SAL_NOK;
        }
    }
    mbedtls_ssl_conf_read_timeout(&context->conf, TLS_READ_TIMEOUT_MS);
    mbedtls_ssl_set_bio(&context->ssl, &context->serverFd,
        mbedtls_net_send, NULL, mbedtls_net_recv_timeout);

    return HILINK_SAL_OK;
}

static int TlsHandshake(HiLinkTlsClient *context)
{
    int ret;
    unsigned long curTime;
    unsigned long startTime = 0;

    if (GetOsTime(&startTime) != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("GetOsTime\r\n");
        return HILINK_SAL_NOK;
    }
    do {
        ret = mbedtls_ssl_handshake(&context->ssl);
        if (GetOsTime(&curTime) != HILINK_SAL_OK) {
            HILINK_SAL_ERROR("GetOsTime\r\n");
            return HILINK_SAL_NOK;
        }
    } while (((ret == MBEDTLS_ERR_SSL_WANT_READ) || (ret == MBEDTLS_ERR_SSL_WANT_WRITE) ||
        (ret == MBEDTLS_ERR_SSL_TIMEOUT)) && (DeltaTime(curTime, startTime) < TLS_HANDSHAKE_TIMEOUT));
    if (ret != 0) {
        HILINK_SAL_ERROR("ret=[-0x%04x],errno=[%d]\r\n", -ret, errno);
        return ret;
    }

    return HILINK_SAL_OK;
}

HiLinkTlsClient *HILINK_TlsClientCreate(const char *custom)
{
    if ((custom == NULL) || (HILINK_Strlen(custom) > MAX_CUSTOM_STR_LEN)) {
        HILINK_SAL_ERROR("param invaild\r\n");
        return NULL;
    }
    HILINK_SAL_NOTICE("custom=[%s] start tls create\r\n", custom);
    HiLinkTlsClient *ctx = (HiLinkTlsClient *)HILINK_Malloc(sizeof(HiLinkTlsClient));
    if (ctx == NULL) {
        HILINK_SAL_ERROR("malloc error\r\n");
        return NULL;
    }
    (void)memset_s(ctx, sizeof(HiLinkTlsClient), 0, sizeof(HiLinkTlsClient));

    int ret;
    do {
        int mallocSize = HILINK_Strlen(custom) + 1;
        ctx->custom = (char *)HILINK_Malloc(mallocSize);
        if (ctx->custom == NULL) {
            HILINK_SAL_ERROR("malloc error\r\n");
            break;
        }
        (void)memset_s(ctx->custom, mallocSize, 0, mallocSize);

        ret = strcpy_s(ctx->custom, mallocSize, custom);
        if (ret != EOK) {
            HILINK_SAL_ERROR("strcpy error %d\r\n", ret);
            break;
        }

        InitMbedtls();
        ret = InitTlsContext(ctx);
        if (ret != HILINK_SAL_OK) {
            HILINK_SAL_ERROR("init error %d\r\n", ret);
            break;
        }

        ret = SetTlsRandomMember(ctx, custom);
        if (ret != HILINK_SAL_OK) {
            HILINK_SAL_ERROR("set random error %d\r\n", ret);
            break;
        }

        ret = SetTlsConfigBase(ctx);
        if (ret != HILINK_SAL_OK) {
            HILINK_SAL_ERROR("set config base error %d\r\n", ret);
            break;
        }

        return ctx;
    } while (0);
    (void)HILINK_TlsClientFreeResource(ctx);
    return NULL;
}

static int SetTlsClientOptionHost(HiLinkTlsClient *ctx, const void *value, unsigned int len)
{
    if ((len != sizeof(HiLinkTlsHost) || (value == NULL))) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    const HiLinkTlsHost *host = (const HiLinkTlsHost *)value;
    ctx->port = host->port;
    if (host->hostname == NULL) {
        HILINK_SAL_INFO("no host name for [%s]\r\n", ctx->custom);
        return HILINK_SAL_OK;
    }

    unsigned int hostNameLen = HILINK_Strlen(host->hostname);
    if (hostNameLen > MAX_HOSTNAME_STR_LEN) {
        HILINK_SAL_WARN("host name too long [%u]\r\n", hostNameLen);
        return HILINK_SAL_PARAM_INVALID;
    }
    int ret = mbedtls_ssl_set_hostname(&ctx->ssl, host->hostname);
    if (ret != 0) {
        HILINK_SAL_ERROR("set host name error, ret=[-0x%04x]\r\n", -ret);
        return HILINK_SAL_NOK;
    }
    if (ctx->hostName != NULL) {
        HILINK_Free(ctx->hostName);
        ctx->hostName = NULL;
    }
    ctx->hostName = (char *)HILINK_Malloc(hostNameLen + 1);
    if (ctx->hostName == NULL) {
        HILINK_SAL_ERROR("malloc error\r\n");
        return HILINK_SAL_NOK;
    }
    (void)memset_s(ctx->hostName, hostNameLen + 1, 0, hostNameLen + 1);
    ret = strcpy_s(ctx->hostName, hostNameLen + 1, host->hostname);
    if (ret != EOK) {
        HILINK_SAL_ERROR("strcpy error %d\r\n", ret);
        HILINK_Free(ctx->hostName);
        ctx->hostName = NULL;
        return HILINK_SAL_NOK;
    }

    return HILINK_SAL_OK;
}

static int SetTlsClientOptionFd(HiLinkTlsClient *ctx, const void *value, unsigned int len)
{
    if ((len != sizeof(int) || (value == NULL))) {
        HILINK_SAL_WARN("invalid param");
        return HILINK_SAL_PARAM_INVALID;
    }

    int fd = *(const int *)value;
    if (fd < 0) {
        HILINK_SAL_WARN("invalid fd");
        return HILINK_SAL_PARAM_INVALID;
    }
    ctx->serverFd.fd = fd;

    return HILINK_SAL_OK;
}

static int SetTlsClientOptiontTimeCallback(HiLinkTlsClient *ctx, const void *value, unsigned int len)
{
    (void)ctx;
    if ((len != sizeof(HiLinkMbedtlsGetTimeCb)) || (value == NULL)) {
        HILINK_SAL_WARN("invalid param");
        return HILINK_SAL_PARAM_INVALID;
    }

    g_mbedtlsGetTimeCb = (HiLinkMbedtlsGetTimeCb)value;
    return HILINK_SAL_OK;
}

static int SetTlsClientOptiontCiphersuite(HiLinkTlsClient *ctx, const void *value, unsigned int len)
{
    if ((len != sizeof(HiLinkTlsCiphersuites)) || (value == NULL)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    const HiLinkTlsCiphersuites *suites = (const HiLinkTlsCiphersuites *)value;
    if ((suites->ciphersuites == NULL) || (suites->num == 0)) {
        HILINK_SAL_WARN("invalid suites\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    return SetTlsConfigCiphersuites(ctx, suites->ciphersuites, suites->num);
}

static int SetTlsClientOptiontCert(HiLinkTlsClient *ctx, const void *value, unsigned int len)
{
    if ((len != sizeof(HiLlinkTlsCerts)) || (value == NULL)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    const HiLlinkTlsCerts *certs = (const HiLlinkTlsCerts *)value;
    if ((certs->certs == NULL) || (certs->num == 0)) {
        HILINK_SAL_WARN("invalid certs\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    return SetTlsConfigCert(ctx, certs->certs, certs->num, certs->num);
}

static int SetTlsClientOptiontPsk(HiLinkTlsClient *ctx, const void *value, unsigned int len)
{
    if ((len != sizeof(HiLlinkTlsPsk)) || (value == NULL)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    const HiLlinkTlsPsk *psk = (const HiLlinkTlsPsk *)value;
    if ((psk->psk == NULL) || (psk->pskLen == 0) || (psk->pskIdentity == NULL) ||
        (psk->pskIdentityLen == 0)) {
        HILINK_SAL_WARN("invalid psk\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    return SetTlsConfigPsk(ctx, psk->psk, psk->pskLen, psk->pskIdentity, psk->pskIdentityLen);
}

static int SetTlsClientOptiontMaxFragLen(HiLinkTlsClient *ctx, const void *value, unsigned int len)
{
    if ((len != sizeof(unsigned char)) || (value == NULL)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    unsigned char fragLenType = *(const unsigned char *)value;

    return SetTlsConfigMaxFragLen(ctx, fragLenType);
}

int HILINK_SetTlsClientOption(HiLinkTlsClient *ctx, HiLinkTlsOption option, const void *value, unsigned int len)
{
    if (ctx == NULL) {
        HILINK_SAL_ERROR("param invaild\r\n");
        return HILINK_SAL_NOK;
    }

    static const OptionItem optionList[] = {
        {HILINK_TLS_OPTION_FD, SetTlsClientOptionFd},
        {HILINK_TLS_OPTION_REG_TIME_CB, SetTlsClientOptiontTimeCallback},
        {HILINK_TLS_OPTION_HOST, SetTlsClientOptionHost},
        {HILINK_TLS_OPTION_CIPHERSUITE, SetTlsClientOptiontCiphersuite},
        {HILINK_TLS_OPTION_CERT, SetTlsClientOptiontCert},
        {HILINK_TLS_OPTION_PSK, SetTlsClientOptiontPsk},
        {HILINK_TLS_OPTION_MAX_FRAG_LEN, SetTlsClientOptiontMaxFragLen},
    };
    for (unsigned int i = 0; i < (sizeof(optionList) / sizeof(OptionItem)); ++i) {
        if (option == optionList[i].option) {
            return optionList[i].setOptionFunc(ctx, value, len);
        }
    }
    HILINK_SAL_WARN("unsupport option %d\r\n", option);
    return HILINK_SAL_NOT_SUPPORT;
}

int HILINK_TlsClientConnect(HiLinkTlsClient *ctx)
{
    if (ctx == NULL) {
        HILINK_SAL_ERROR("param invaild\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    int ret = InitTlsSocket(ctx);
    if (ret != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("init socket error\r\n");
        return ret;
    }

    ret = mbedtls_ssl_setup(&ctx->ssl, &ctx->conf);
    if (ret != 0) {
        HILINK_SAL_ERROR("ret=[-0x%04x]\r\n", -ret);
        return ret;
    }

    ret = TlsHandshake(ctx);
    if (ret != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("handshake\r\n");
        return ret;
    }

    return HILINK_SAL_OK;
}

int HILINK_TlsClientGetContextFd(HiLinkTlsClient *ctx)
{
    if (ctx == NULL) {
        HILINK_SAL_DEBUG("param invaild\r\n");
        return -1;
    }

    return ctx->serverFd.fd;
}

int HILINK_TlsClientRead(HiLinkTlsClient *ctx, unsigned char *buf, size_t len)
{
    if ((ctx == NULL) || (buf == NULL) || (len == 0)) {
        HILINK_SAL_ERROR("param invaild\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    if (ctx->serverFd.fd < 0) {
        HILINK_SAL_ERROR("context invaild\r\n");
        return HILINK_SAL_FD_INVALID;
    }

    int ret = mbedtls_ssl_read(&ctx->ssl, buf, len);
    if (ret > 0) {
        HILINK_SAL_DEBUG("read exit,custom=[%s],len=[%d]\r\n", ctx->custom, ret);
        return ret;
    } else if (ret == 0) {
        HILINK_SAL_ERROR("custom=[%s],ret=[%d],errno=[%d]\r\n", ctx->custom, ret, errno);
        return HILINK_SAL_NOK;
    } else if ((ret == MBEDTLS_ERR_SSL_TIMEOUT) ||
        (ret == MBEDTLS_ERR_SSL_WANT_WRITE) || (ret == MBEDTLS_ERR_SSL_WANT_READ)) {
        return HILINK_SAL_OK;
    }

    HILINK_SAL_ERROR("custom=[%s],ret=[-0x%04x],errno=[%d]\r\n", ctx->custom, -ret, errno);
    return ret;
}

int HILINK_TlsClientWrite(HiLinkTlsClient *ctx, const unsigned char *buf, size_t len)
{
    if ((ctx == NULL) || (buf == NULL) || (len == 0)) {
        HILINK_SAL_ERROR("param invaild\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    if (ctx->serverFd.fd < 0) {
        HILINK_SAL_ERROR("context invaild\r\n");
        return HILINK_SAL_FD_INVALID;
    }

    unsigned long curTime = 0;
    unsigned long startTime = 0;
    if (GetOsTime(&startTime) != HILINK_SAL_OK) {
        return HILINK_SAL_TIME_ERR;
    }

    int writeLen = 0;
    while ((size_t)writeLen < len) {
        if (GetOsTime(&curTime) != HILINK_SAL_OK) {
            return HILINK_SAL_TIME_ERR;
        }

        if (DeltaTime(curTime, startTime) > GetTlsTimeoutTime(len)) {
            HILINK_SAL_ERROR("tls write timeout,writeLen=[%d],len=[%u]\r\n", writeLen, len);
            return HILINK_SAL_TIMEOUT;
        }

        int ret = mbedtls_ssl_write(&ctx->ssl, buf + writeLen, len - writeLen);
        if (ret > 0) {
            writeLen += ret;
            HILINK_SAL_DEBUG("custom=[%s],writeLen=[%d]\r\n", ctx->custom, writeLen);
            continue;
        } else if (ret == 0) {
            HILINK_SAL_ERROR("custom=[%s],ret=[%d],errno=[%d]\r\n", ctx->custom, ret, errno);
            return HILINK_SAL_NOK;
        } else if ((ret == MBEDTLS_ERR_SSL_TIMEOUT) ||
            (ret == MBEDTLS_ERR_SSL_WANT_WRITE) || (ret == MBEDTLS_ERR_SSL_WANT_READ)) {
            HILINK_MilliSleep(1);
            continue;
        } else {
            HILINK_SAL_ERROR("custom=[%s],ret=[-0x%04x],errno=[%d]\r\n", ctx->custom, -ret, errno);
            return ret;
        }
    }

    return writeLen;
}

bool HILINK_TlsClientIsValidCert(HiLinkTlsClient *ctx)
{
    if (ctx == NULL) {
        HILINK_SAL_NOTICE("param invaild\r\n");
        return false;
    }

    int ret;
    ret = mbedtls_x509_time_is_future(&ctx->validFrom);
    if (ret != 0) {
        HILINK_SAL_ERROR("ret=[%d]\r\n", ret);
        return false;
    }
    ret = mbedtls_x509_time_is_past(&ctx->validTo);
    if (ret != 0) {
        HILINK_SAL_ERROR("ret=[%d]\r\n", ret);
        return false;
    }

    return true;
}

int HILINK_TlsClientFreeResource(HiLinkTlsClient *ctx)
{
    if (ctx == NULL) {
        HILINK_SAL_NOTICE("param invaild\r\n");
        return HILINK_SAL_NOK;
    }

    mbedtls_ssl_free(&ctx->ssl);
    mbedtls_net_free(&ctx->serverFd);
    mbedtls_ssl_config_free(&ctx->conf);
    mbedtls_x509_crt_free(&ctx->caCert);
    HILINK_SAL_DrbgDeinit(ctx->drbg);
    ctx->drbg = NULL;
    HILINK_Free(ctx->supportedCiphersuites);
    ctx->supportedCiphersuites = NULL;
    if (ctx->custom != NULL) {
        HILINK_SAL_NOTICE("custom=[%s] free resource success\r\n", ctx->custom);
        HILINK_Free(ctx->custom);
        ctx->custom = NULL;
    }
    if (ctx->hostName != NULL) {
        HILINK_Free(ctx->hostName);
        ctx->hostName = NULL;
    }
    HILINK_Free(ctx);
    return HILINK_SAL_OK;
}