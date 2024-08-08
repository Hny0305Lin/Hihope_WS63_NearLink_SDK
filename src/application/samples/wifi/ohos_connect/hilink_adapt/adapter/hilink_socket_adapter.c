/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 系统适配层网络Socket接口实现（此文件为DEMO，需集成方适配修改）
 */

#include "hilink_socket_adapter.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "securec.h"
#ifdef HILINK_SDK_BUILD_IN
#ifndef _LINUX_OS_
#include "lwip.h"
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif
#else
/* 在此处引入平台socket接口头文件 */
#include "lwip/pbuf.h"
#include "lwip/dns.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#endif /* HILINK_SDK_BUILD_IN */
#include "hilink_network_adapter.h"
#include "hilink_str_adapter.h"
#include "hilink_sal_defines.h"
#include "hilink_mem_adapter.h"

#define MS_PER_SEC  1000
#define US_PER_MS   1000
#define MAX_IP_LEN  40
#define MAX(a, b)   (((a) > (b)) ? (a) : (b))
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define IPV6_ADDR_LEN 28
#define MAX_ADDR_LEN IPV6_ADDR_LEN
#define MAX_DNS_RES_NUM 10

typedef struct AddrInfoInner {
    HiLinkAddrInfo hiAddr;
    struct addrinfo *addr;
} AddrInfoInner;

typedef struct {
    HiLinkSocketOption option;
    int (*setOptionFunc)(int fd, const void *value, unsigned int len);
} OptionItem;

typedef struct {
    int hiType;
    int sockType;
} HiLinkSocketTypePair;

static const HiLinkSocketTypePair AF_MAP[] = {
    {HILINK_SOCKET_DOMAIN_AF_INET, AF_INET},
    {HILINK_SOCKET_DOMAIN_AF_INET6, AF_INET6},
    {HILINK_SOCKET_DOMAIN_UNSPEC, AF_UNSPEC},
};

static const HiLinkSocketTypePair AP_MAP[] = {
    {HILINK_SOCKET_PROTO_IP, IPPROTO_IP},
    {HILINK_SOCKET_PROTO_TCP, IPPROTO_TCP},
    {HILINK_SOCKET_PROTO_UDP, IPPROTO_UDP},
};

static const HiLinkSocketTypePair AS_MAP[] = {
    {HILINK_SOCKET_TYPE_STREAM, SOCK_STREAM},
    {HILINK_SOCKET_TYPE_DGRAM, SOCK_DGRAM},
    {HILINK_SOCKET_TYPE_RAW, SOCK_RAW},
};

static int HiLinkAiFamily2Socket(int af)
{
    for (unsigned int i = 0; i < ARRAY_SIZE(AF_MAP); ++i) {
        if (AF_MAP[i].hiType == af) {
            return AF_MAP[i].sockType;
        }
    }
    return af;
}

static int SocketAiFamily2HiLink(int af)
{
    for (unsigned int i = 0; i < ARRAY_SIZE(AF_MAP); ++i) {
        if (AF_MAP[i].sockType == af) {
            return AF_MAP[i].hiType;
        }
    }
    return af;
}

static int HiLinkAiProtocal2Socket(int ap)
{
    for (unsigned int i = 0; i < ARRAY_SIZE(AP_MAP); ++i) {
        if (AP_MAP[i].hiType == ap) {
            return AP_MAP[i].sockType;
        }
    }
    return ap;
}

static int SocketAiProtocal2HiLink(int ap)
{
    for (unsigned int i = 0; i < ARRAY_SIZE(AP_MAP); ++i) {
        if (AP_MAP[i].sockType == ap) {
            return AP_MAP[i].hiType;
        }
    }
    return ap;
}

static int HiLinkAiSocktype2Socket(int as)
{
    for (unsigned int i = 0; i < ARRAY_SIZE(AS_MAP); ++i) {
        if (AS_MAP[i].hiType == as) {
            return AS_MAP[i].sockType;
        }
    }
    return as;
}

static int SocketAiSocktype2HiLink(int as)
{
    for (unsigned int i = 0; i < ARRAY_SIZE(AS_MAP); ++i) {
        if (AS_MAP[i].sockType == as) {
            return AS_MAP[i].hiType;
        }
    }
    return as;
}

static void GetHiLinkAddrInfo(HiLinkAddrInfo *out, struct addrinfo *in, int level, int maxLevel)
{
    if ((level > maxLevel) || (in == NULL) || (out == NULL)) {
        return;
    }

    out->aiFlags = in->ai_flags;
    out->aiFamily = SocketAiFamily2HiLink(in->ai_family);
    out->aiProtocol = SocketAiProtocal2HiLink(in->ai_protocol);
    out->aiSocktype = SocketAiSocktype2HiLink(in->ai_socktype);
    out->aiAddrlen = in->ai_addrlen;
    out->aiAddr = (HiLinkSockaddr *)in->ai_addr;
    out->aiCanonname = in->ai_canonname;

    if (in->ai_next != NULL) {
        out->aiNext = (HiLinkAddrInfo *)HILINK_Malloc(sizeof(HiLinkAddrInfo));
        if (out->aiNext == NULL) {
            HILINK_SAL_WARN("malloc error\r\n");
        } else {
            (void)memset_s(out->aiNext, sizeof(HiLinkAddrInfo), 0, sizeof(HiLinkAddrInfo));
            /* 递归函数，+1表示递归深度+1 */
            GetHiLinkAddrInfo(out->aiNext, in->ai_next, level + 1, maxLevel);
        }
    }

    return;
}

static void FreeHiLinkAddrInfo(HiLinkAddrInfo *addr)
{
    if (addr->aiNext != NULL) {
        FreeHiLinkAddrInfo(addr->aiNext);
        addr->aiNext = NULL;
    }
    HILINK_Free(addr);
}

static void FreeAddrInfoInner(AddrInfoInner *addrInner)
{
    if (addrInner->addr != NULL) {
        freeaddrinfo(addrInner->addr);
        addrInner->addr = NULL;
    }

    FreeHiLinkAddrInfo((HiLinkAddrInfo *)addrInner);
}

static HiLinkAddrInfo *GetAddrInfoInner(struct addrinfo *addr)
{
    if (addr == NULL) {
        return NULL;
    }
    AddrInfoInner *addrInner = (AddrInfoInner *)HILINK_Malloc(sizeof(AddrInfoInner));
    if (addrInner == NULL) {
        HILINK_SAL_WARN("malloc error\r\n");
        freeaddrinfo(addr);
        return NULL;
    }
    (void)memset_s(addrInner, sizeof(AddrInfoInner), 0, sizeof(AddrInfoInner));
    /* 递归函数，1代表第一层 */
    GetHiLinkAddrInfo(&addrInner->hiAddr, addr, 1, MAX_DNS_RES_NUM);
    addrInner->addr = addr;
    return (HiLinkAddrInfo *)addrInner;
}

int HILINK_GetAddrInfo(const char *nodename, const char *servname,
    const HiLinkAddrInfo *hints, HiLinkAddrInfo **result)
{
    if ((nodename == NULL) || (result == NULL)) {
        HILINK_SAL_WARN("invalid param");
        return HILINK_SAL_PARAM_INVALID;
    }

    struct addrinfo *resInfo = NULL;
    struct addrinfo *hintsInfoP = NULL;
    struct addrinfo hintsInfo;

    HILINK_SAL_DEBUG_LIMITED("get addrinfo %s\r\n", nodename);
    if (hints != NULL) {
        if (memcpy_s(&hintsInfo, sizeof(struct addrinfo), hints, sizeof(HiLinkAddrInfo)) != EOK) {
            HILINK_SAL_ERROR("memcpy error");
            return HILINK_SAL_MEMCPY_ERR;
        }
        hintsInfo.ai_family = HiLinkAiFamily2Socket(hintsInfo.ai_family);
        hintsInfo.ai_protocol = HiLinkAiProtocal2Socket(hintsInfo.ai_protocol);
        hintsInfo.ai_socktype = HiLinkAiSocktype2Socket(hintsInfo.ai_socktype);
        hintsInfoP = &hintsInfo;
    }

    int ret = getaddrinfo(nodename, servname, hintsInfoP, &resInfo);
    if ((ret != 0) || (resInfo == NULL)) {
        HILINK_SAL_ERROR_LIMITED("getaddrinfo failed, ret %d\r\n", ret);
        return HILINK_SAL_DNS_ERR;
    }

    *result = GetAddrInfoInner(resInfo);
    return HILINK_SAL_OK;
}

void HILINK_FreeAddrInfo(HiLinkAddrInfo *addrInfo)
{
    if (addrInfo == NULL) {
        HILINK_SAL_WARN("invalid param");
        return;
    }
    FreeAddrInfoInner((AddrInfoInner *)addrInfo);
    return;
}

int HILINK_Socket(HiLinkSocketDomain domain, HiLinkSocketType type, HiLinkSocketProto proto)
{
    int af = HiLinkAiFamily2Socket(domain);
    int st = HiLinkAiSocktype2Socket(type);
    int ap = HiLinkAiProtocal2Socket(proto);

    return socket(af, st, ap);
}

void HILINK_Close(int fd)
{
    (void)lwip_close(fd);
    return;
}

static int SetFcntl(int fd, bool isBlock)
{
    int flags = lwip_fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        HILINK_SAL_WARN("fcntl get failed, ret %d\r\n", flags);
        return HILINK_SAL_FCNTL_ERR;
    }
    if (isBlock) {
        flags &= (~O_NONBLOCK);
    } else {
        flags |= O_NONBLOCK;
    }
    if (lwip_fcntl(fd, F_SETFL, flags) < 0) {
        HILINK_SAL_WARN("fcntl set failed\r\n");
        return HILINK_SAL_FCNTL_ERR;
    }
    return HILINK_SAL_OK;
}

static int SetSocketOptionNonblock(int fd, const void *value, unsigned int len)
{
    (void)value;
    (void)len;

    return SetFcntl(fd, false);
}

static int SetSocketOptionBlock(int fd, const void *value, unsigned int len)
{
    (void)value;
    (void)len;

    return SetFcntl(fd, true);
}

static int SetSocketTimeout(int fd, unsigned int timeout, bool isRead)
{
    struct timeval tv;
    int flag = isRead ? SO_RCVTIMEO : SO_SNDTIMEO;

    tv.tv_sec = timeout / MS_PER_SEC;
    tv.tv_usec = timeout % MS_PER_SEC * US_PER_MS;
    if (setsockopt(fd, SOL_SOCKET, flag, &tv, sizeof(struct timeval)) != 0) {
        HILINK_SAL_WARN("set [%d][%u] failed\r\n", flag, timeout);
        return HILINK_SAL_SET_SOCK_OPT_ERR;
    }
    return HILINK_SAL_OK;
}

static int SetSocketOptionReadTimeouot(int fd, const void *value, unsigned int len)
{
    if (len < sizeof(unsigned int)) {
        HILINK_SAL_WARN("invalid param");
        return HILINK_SAL_PARAM_INVALID;
    }
    unsigned int timeout = *(const unsigned int *)value;

    return SetSocketTimeout(fd, timeout, true);
}

static int SetSocketOptionSendTimeouot(int fd, const void *value, unsigned int len)
{
    if (len < sizeof(unsigned int)) {
        HILINK_SAL_WARN("invalid param");
        return HILINK_SAL_PARAM_INVALID;
    }
    unsigned int timeout = *(const unsigned int *)value;

    return SetSocketTimeout(fd, timeout, false);
}

static int SetSocketOptionEnableReuseAddr(int fd, const void *value, unsigned int len)
{
    (void)value;
    (void)len;
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) != 0) {
        HILINK_SAL_WARN("set reuse addr failed\r\n");
        return HILINK_SAL_SET_SOCK_OPT_ERR;
    }
    return HILINK_SAL_OK;
}

static int SetSocketOptionDisableReuseAddr(int fd, const void *value, unsigned int len)
{
    (void)value;
    (void)len;
    int opt = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) != 0) {
        HILINK_SAL_WARN("close reuse addr failed\r\n");
        return HILINK_SAL_SET_SOCK_OPT_ERR;
    }
    return HILINK_SAL_OK;
}

static int SetSocketMultiGroup(int fd, const char *multicastIp, bool isAdd)
{
    struct ip_mreq group;
    (void)memset_s(&group, sizeof(struct ip_mreq), 0, sizeof(struct ip_mreq));
    group.imr_multiaddr.s_addr = HILINK_InetAddr(multicastIp);
    char localIp[MAX_IP_LEN] = {0};
    if ((HILINK_GetLocalIp(localIp, sizeof(localIp)) != 0) ||
        (HILINK_Strlen(localIp) == 0) || (HILINK_Strcmp(localIp, "0.0.0.0") == 0)) {
        group.imr_interface.s_addr = HILINK_Htonl(INADDR_ANY);
        HILINK_SAL_NOTICE("use any addr\r\n");
    } else {
        group.imr_interface.s_addr = HILINK_InetAddr(localIp);
    }

    HILINK_SAL_NOTICE("SetSocketMultiGroup get ip[%s]\r\n", localIp);
    int flag = isAdd ? IP_ADD_MEMBERSHIP : IP_DROP_MEMBERSHIP;
    if (setsockopt(fd, IPPROTO_IP, flag, (void *)&group, sizeof(group)) < 0) {
        HILINK_SAL_WARN("set opt %d failed\r\n", flag);
        return HILINK_SAL_SET_SOCK_OPT_ERR;
    }
    return HILINK_SAL_OK;
}

static int SetSocketOptionAddMultiGroup(int fd, const void *value, unsigned int len)
{
    if (HILINK_Strlen((const char *)value) > len) {
        HILINK_SAL_WARN("invalid param");
        return HILINK_SAL_PARAM_INVALID;
    }
    return SetSocketMultiGroup(fd, (const char *)value, true);
}

static int SetSocketOptionDropMultiGroup(int fd, const void *value, unsigned int len)
{
    if (HILINK_Strlen((const char *)value) > len) {
        HILINK_SAL_WARN("invalid param");
        return HILINK_SAL_PARAM_INVALID;
    }
    return SetSocketMultiGroup(fd, (const char *)value, false);
}

static int SetSocketOptionEnableBroadcast(int fd, const void *value, unsigned int len)
{
    (void)value;
    (void)len;
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (const char *)&opt, sizeof(opt)) != 0) {
        HILINK_SAL_WARN("set broadcast failed\r\n");
        return HILINK_SAL_SET_SOCK_OPT_ERR;
    }
    return HILINK_SAL_OK;
}

static int SetSocketOptionDisableBroadcast(int fd, const void *value, unsigned int len)
{
    (void)value;
    (void)len;
    int opt = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (const char *)&opt, sizeof(opt)) != 0) {
        HILINK_SAL_WARN("close broadcast failed\r\n");
        return HILINK_SAL_SET_SOCK_OPT_ERR;
    }
    return HILINK_SAL_OK;
}

static int SetSocketOptionEnableMultiLoop(int fd, const void *value, unsigned int len)
{
    (void)value;
    (void)len;
    int opt = 1;
    if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, (const char *)&opt, sizeof(opt)) != 0) {
        HILINK_SAL_WARN("set loop failed\r\n");
        return HILINK_SAL_SET_SOCK_OPT_ERR;
    }
    return HILINK_SAL_OK;
}

static int SetSocketOptionDisableMultiLoop(int fd, const void *value, unsigned int len)
{
    (void)value;
    (void)len;
    int opt = 0;
    if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, (const char *)&opt, sizeof(opt)) != 0) {
        HILINK_SAL_WARN("close loop failed\r\n");
        return HILINK_SAL_SET_SOCK_OPT_ERR;
    }
    return HILINK_SAL_OK;
}

static int SetSocketOptionSendBuffer(int fd, const void *value, unsigned int len)
{
    if (len < sizeof(unsigned int)) {
        HILINK_SAL_WARN("invalid param");
        return HILINK_SAL_PARAM_INVALID;
    }
    unsigned int bufferLen = *(unsigned int *)value;
    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void *)&bufferLen, sizeof(bufferLen)) < 0) {
        HILINK_SAL_WARN("set sendbuf %u failed\r\n", bufferLen);
        return HILINK_SAL_SET_SOCK_OPT_ERR;
    }
    return HILINK_SAL_OK;
}

static int SetSocketOptionReadBuffer(int fd, const void *value, unsigned int len)
{
    if (len < sizeof(unsigned int)) {
        HILINK_SAL_WARN("invalid param");
        return HILINK_SAL_PARAM_INVALID;
    }
    unsigned int bufferLen = *(unsigned int *)value;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (void *)&bufferLen, sizeof(bufferLen)) < 0) {
        HILINK_SAL_WARN("set recvbuf %u failed\r\n", bufferLen);
        return HILINK_SAL_SET_SOCK_OPT_ERR;
    }
    return HILINK_SAL_OK;
}

int HILINK_SetSocketOpt(int fd, HiLinkSocketOption option, const void *value, unsigned int len)
{
    static const OptionItem optionList[] = {
        {HILINK_SOCKET_OPTION_SETFL_BLOCK, SetSocketOptionBlock},
        {HILINK_SOCKET_OPTION_SETFL_NONBLOCK, SetSocketOptionNonblock},
        {HILINK_SOCKET_OPTION_READ_TIMEOUT, SetSocketOptionReadTimeouot},
        {HILINK_SOCKET_OPTION_SEND_TIMEOUT, SetSocketOptionSendTimeouot},
        {HILINK_SOCKET_OPTION_ENABLE_REUSEADDR, SetSocketOptionEnableReuseAddr},
        {HILINK_SOCKET_OPTION_DISABLE_REUSEADDR, SetSocketOptionDisableReuseAddr},
        {HILINK_SOCKET_OPTION_ADD_MULTI_GROUP, SetSocketOptionAddMultiGroup},
        {HILINK_SOCKET_OPTION_DROP_MULTI_GROUP, SetSocketOptionDropMultiGroup},
        {HILINK_SOCKET_OPTION_ENABLE_BROADCAST, SetSocketOptionEnableBroadcast},
        {HILINK_SOCKET_OPTION_DISABLE_BROADCAST, SetSocketOptionDisableBroadcast},
        {HILINK_SOCKET_OPTION_ENABLE_MULTI_LOOP, SetSocketOptionEnableMultiLoop},
        {HILINK_SOCKET_OPTION_DISABLE_MULTI_LOOP, SetSocketOptionDisableMultiLoop},
        {HILINK_SOCKET_OPTION_SEND_BUFFER, SetSocketOptionSendBuffer},
        {HILINK_SOCKET_OPTION_READ_BUFFER, SetSocketOptionReadBuffer},
    };
    for (unsigned int i = 0; i < (sizeof(optionList) / sizeof(OptionItem)); ++i) {
        if (option == optionList[i].option) {
            return optionList[i].setOptionFunc(fd, value, len);
        }
    }
    HILINK_SAL_WARN("unsupport option %d\r\n", option);
    return HILINK_SAL_NOT_SUPPORT;
}

int HILINK_Bind(int fd, const HiLinkSockaddr *addr, unsigned int addrLen)
{
    (void)addrLen;
    if (addr == NULL) {
        HILINK_SAL_WARN("invalid param");
        return HILINK_SAL_PARAM_INVALID;
    }
    struct sockaddr addrIn;
    addrIn.sa_family = HiLinkAiFamily2Socket(addr->saFamily);
    if (memcpy_s(addrIn.sa_data, sizeof(addrIn.sa_data), addr->saData, sizeof(addr->saData)) != EOK) {
        HILINK_SAL_WARN("memcpy error");
        return HILINK_SAL_MEMCPY_ERR;
    }

    return bind(fd, &addrIn, sizeof(struct sockaddr));
}


int HILINK_Connect(int fd, const HiLinkSockaddr *addr, unsigned int addrLen)
{
    (void)addrLen;
    if (addr == NULL) {
        HILINK_SAL_WARN("invalid param");
        return HILINK_SAL_PARAM_INVALID;
    }
    struct sockaddr addrIn;
    addrIn.sa_family = HiLinkAiFamily2Socket(addr->saFamily);
    if (memcpy_s(addrIn.sa_data, sizeof(addrIn.sa_data), addr->saData, sizeof(addr->saData)) != EOK) {
        HILINK_SAL_WARN("memcpy error");
        return HILINK_SAL_MEMCPY_ERR;
    }
    return connect(fd, &addrIn, sizeof(struct sockaddr));
}

int HILINK_Recv(int fd, unsigned char *buf, unsigned int len)
{
    return recv(fd, buf, len, MSG_DONTWAIT);
}

int HILINK_Send(int fd, const unsigned char *buf, unsigned int len)
{
    return send(fd, buf, len, MSG_DONTWAIT);
}

int HILINK_RecvFrom(int fd, unsigned char *buf, unsigned int len,
    HiLinkSockaddr *from, unsigned int *fromLen)
{
    if ((from == NULL) || (fromLen == NULL)) {
        HILINK_SAL_WARN("invalid param");
        return HILINK_SAL_PARAM_INVALID;
    }
    struct sockaddr addr;
    (void)memset_s(&addr, sizeof(struct sockaddr), 0, sizeof(struct sockaddr));
    int ret = recvfrom(fd, buf, len, 0, &addr, (socklen_t *)fromLen);
    from->saFamily = addr.sa_family;
    if (memcpy_s(from->saData, sizeof(from->saData), addr.sa_data, sizeof(addr.sa_data)) != EOK) {
        HILINK_SAL_WARN("memcpy error");
        return HILINK_SAL_MEMCPY_ERR;
    }
    return ret;
}

int HILINK_SendTo(int fd, const unsigned char *buf, unsigned int len,
    const HiLinkSockaddr *to, unsigned int toLen)
{
    if ((to == NULL) || (toLen == 0)) {
        HILINK_SAL_WARN("invalid param");
        return HILINK_SAL_PARAM_INVALID;
    }
    struct sockaddr addr;
    addr.sa_family = HiLinkAiFamily2Socket(to->saFamily);
    if (memcpy_s(addr.sa_data, sizeof(addr.sa_data), to->saData, sizeof(to->saData)) != EOK) {
        HILINK_SAL_WARN("memcpy error");
        return HILINK_SAL_MEMCPY_ERR;
    }
    return sendto(fd, buf, len, 0, &addr, toLen);
}

static void GetFdSet(HiLinkFdSet *set, fd_set *fdSet, int *maxfd)
{
    if ((set != NULL) && (set->fdSet != NULL)) {
        memset_s(fdSet, sizeof(fd_set), 0, sizeof(fd_set));
        for (unsigned int i = 0; i < set->num; ++i) {
            if (set->fdSet[i] >= 0) {
                FD_SET(set->fdSet[i], fdSet);
                *maxfd = MAX(*maxfd, set->fdSet[i]);
            }
        }
    }
}

static void FdIsSet(HiLinkFdSet *set, fd_set *fdSet)
{
    if ((set == NULL) || (fdSet == NULL)) {
        return;
    }
    for (unsigned int i = 0; i < set->num; ++i) {
        if (FD_ISSET(set->fdSet[i], fdSet) == 0) {
            set->fdSet[i] = -1;
        }
    }
    return;
}

int HILINK_Select(HiLinkFdSet *readSet, HiLinkFdSet *writeSet, HiLinkFdSet *exceptSet, unsigned int ms)
{
    int maxfd = -1;
    fd_set read, write, except;
    GetFdSet(readSet, &read, &maxfd);
    GetFdSet(writeSet, &write, &maxfd);
    GetFdSet(exceptSet, &except, &maxfd);

    struct timeval timeout;
    timeout.tv_sec = ms / MS_PER_SEC;
    timeout.tv_usec = ms % MS_PER_SEC * US_PER_MS;
    int ret = lwip_select(maxfd + 1, (readSet == NULL) ? NULL : &read,
        (writeSet == NULL) ? NULL : &write,
        (exceptSet == NULL) ? NULL : &except, &timeout);
    if (ret <= 0) {
        return ret;
    }
    FdIsSet(readSet, &read);
    FdIsSet(writeSet, &write);
    FdIsSet(exceptSet, &except);
    return ret;
}

int HILINK_GetSocketErrno(int fd)
{
#if defined(_LINUX_OS_) && defined(errno)
    if (fd < 0) {
        return errno;
    }
#endif
    int socketErr;
    unsigned int len = sizeof(socklen_t);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &socketErr, (socklen_t *)&len) != 0) {
        HILINK_SAL_WARN("get socket errno error\r\n");
        return HILINK_SAL_GET_SOCK_OPT_ERR;
    }

    switch (socketErr) {
        case EINTR:
            return HILINK_SOCKET_ERRNO_EINTR;
        case EAGAIN:
            return HILINK_SOCKET_ERRNO_EAGAIN;
        case EINPROGRESS:
            return HILINK_SOCKET_ERRNO_EINPROGRESS;
        default:
            break;
    }

    return socketErr;
}

int get_os_errno(void)
{
    int errCode = errno;
    return errCode;
}

unsigned int HILINK_Htonl(unsigned int hl)
{
    return htonl(hl);
}

unsigned int HILINK_Ntohl(unsigned int nl)
{
    return ntohl(nl);
}

unsigned short HILINK_Htons(unsigned short hs)
{
    return htons(hs);
}

unsigned short HILINK_Ntohs(unsigned short ns)
{
    return ntohs(ns);
}

unsigned int HILINK_InetAton(const char *ip, unsigned int *addr)
{
    return inet_aton(ip, (struct in_addr *)addr);
}

unsigned int HILINK_InetAddr(const char *ip)
{
    return inet_addr(ip);
}

const char *HILINK_InetNtoa(unsigned int addr, char *buf, unsigned int buflen)
{
    struct in_addr tempAddr;
    tempAddr.s_addr = addr;
#ifndef _LINUX_OS_
    return inet_ntoa_r(tempAddr, buf, buflen);
#else
    return inet_ntop(AF_INET, &tempAddr, buf, buflen);
#endif
}