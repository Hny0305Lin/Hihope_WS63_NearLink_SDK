/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: TLS 客户端常用操作，包括会话的创建、销毁等（此文件为DEMO，需集成方适配修改）
 */

#ifndef HILINK_TLS_CLIENT_H
#define HILINK_TLS_CLIENT_H

#include <stddef.h>
#include <stdbool.h>
#include "hilink_sal_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* hilink tls需要的算法套件重定义，与mbedtls中的头文件保持一致 */
#define HILINK_MBEDTLS_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 0xCCA8
#define HILINK_MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384 0xC030
#define HILINK_MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 0xC02F
#define HILINK_MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256 0xA8
#define HILINK_MBEDTLS_TLS_PSK_WITH_AES_256_GCM_SHA384 0xA9

/* hilink tls最大分片大小重定义，与mbedtls中的头文件保持一致 */
#define HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_NONE 0 /* !< don't use this extension */
#define HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_512 1 /* !< MaxFragmentLength 2^9 */
#define HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_1024 2 /* !< MaxFragmentLength 2^10 */
#define HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_2048 3 /* !< MaxFragmentLength 2^11 */
#define HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_4096 4 /* !< MaxFragmentLength 2^12 */
#define HILINK_MBEDTLS_SSL_MAX_FRAG_LEN_DEFAULT 255 /* 使用默认值，不配置 */

/** @brief TLS客户端句柄 */
typedef struct HiLinkTlsClient HiLinkTlsClient;

/**
 * @brief 获取当前实时时间
 *
 * @param timeMs [OUT] 输出时间
 * @return 0 成功; -1 失败
 */
typedef int (*HiLinkMbedtlsGetTimeCb)(unsigned long long *timeMs);

/** @brief TLS客户端host信息 */
typedef struct HiLinkTlsHost {
    /** 对端hostname */
    const char *hostname;
    /** 对端端口号，由TLS客户端创建fd时需传入 */
    unsigned short port;
} HiLinkTlsHost;

/** @brief TLS客户端使用的加密套件 */
typedef struct HiLinkTlsCiphersuites {
    /** 加密套件标识符数组 */
    const int *ciphersuites;
    /** 加密套件数量 */
    unsigned int num;
} HiLinkTlsCiphersuites;

/** @brief TLS客户端使用的证书 */
typedef struct HiLlinkTlsCerts {
    /** 证书数组，在客户端有效期内数组指针应有效 */
    const char **certs;
    /** 证书数量 */
    unsigned int num;
    /** 延迟校验证书标志位 */
    bool isDelayVerifyCert;
} HiLlinkTlsCerts;

/** @brief TLS客户端psk参数 */
typedef struct HiLlinkTlsPsk {
    /** psk */
    const unsigned char *psk;
    /** psk长度 */
    unsigned int pskLen;
    /** psk标识符 */
    const unsigned char *pskIdentity;
    /** psk标识符长度 */
    unsigned int pskIdentityLen;
} HiLlinkTlsPsk;

typedef enum {
    /* 为TLS客户端设置套接字，参数类型为int* */
    HILINK_TLS_OPTION_FD = 0,
    /* 为TLS客户端设置获取时间回调函数，参数类型为HiLinkMbedtlsGetTimeCb，对所有客户端生效 */
    HILINK_TLS_OPTION_REG_TIME_CB,
    /* 为TLS客户端设置对端地址，参数类型为HiLinkTlsHost* */
    HILINK_TLS_OPTION_HOST,
    /* 为TLS客户端设置套件白名单，参数类型为HiLinkTlsCiphersuites* */
    HILINK_TLS_OPTION_CIPHERSUITE,
    /* 为TLS客户端设置证书，参数类型为HiLlinkTlsCerts* */
    HILINK_TLS_OPTION_CERT,
    /* 为TLS客户端设置psk，参数类型为HiLlinkTlsPsk* */
    HILINK_TLS_OPTION_PSK,
    /* 为TLS客户端设置最大分片大小，参数类型为unsiged char *，值参考HILINK_MBEDTLS_SSL_MAX_FRAG_LEN */
    HILINK_TLS_OPTION_MAX_FRAG_LEN,
    HILINK_TLS_OPTION_RESERVED = 0x7FFFFFFF,
} HiLinkTlsOption;

/**
 * @brief 创建 Tls 客户端
 *
 * @param custom [IN] tls客户端标识名称
 * @return 非NULL客户端句柄，NULL失败
 */
HiLinkTlsClient *HILINK_TlsClientCreate(const char *custom);

/**
 * @brief 配置TLS客户端
 *
 * @param ctx [IN] tls客户端句柄
 * @param option [IN] 设定的选项
 * @param value [IN] 选项参数
 * @param len [IN] 选项参数长度
 * @return 0成功，非0失败
 */
int HILINK_SetTlsClientOption(HiLinkTlsClient *ctx, HiLinkTlsOption option, const void *value, unsigned int len);

/**
 * @brief TLS客户端连接对端
 *
 * @param ctx [IN] tls客户端句柄
 * @return 0成功，非0失败
 */
int HILINK_TlsClientConnect(HiLinkTlsClient *ctx);

/**
 * @brief 获取ContextFd
 *
 * @param ctx [IN] tls客户端句柄
 * @return  ContextFd
 */
int HILINK_TlsClientGetContextFd(HiLinkTlsClient *ctx);

/**
 * @brief Tls 读取数据 非阻塞
 *
 * @param ctx [IN] tls客户端句柄
 * @param buf [OUT] 数据输出buf
 * @param len [IN] buf可写长度
 * @return 0 正常在读取中暂未接到包; 小于0 失败; 其他为接收成功数据长度
 */
int HILINK_TlsClientRead(HiLinkTlsClient *ctx, unsigned char *buf, size_t len);

/**
 * @brief Tls 发送数据 非阻塞
 *
 * @param ctx [IN] tls客户端句柄
 * @param buf [IN] 发送数据
 * @param len [IN] buf 大小
 * @return 0 正常在发送; 小于0失败; 其他为发送成功数据长度
 */
int HILINK_TlsClientWrite(HiLinkTlsClient *ctx, const unsigned char *buf, size_t len);

/**
 * @brief Tls 验证证书
 *
 * @param ctx [IN] tls客户端句柄
 * @return true 有效 false 失效
 */
bool HILINK_TlsClientIsValidCert(HiLinkTlsClient *ctx);

/**
 * @brief Tls 资源释放
 *
 * @param ctx [IN] tls客户端句柄
 * @return 0 成功; 其他失败
 */
int HILINK_TlsClientFreeResource(HiLinkTlsClient *ctx);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HILINK_TLS_CLIENT_H */
