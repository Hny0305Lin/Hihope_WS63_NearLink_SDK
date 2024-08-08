/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Implementation of the Network Socket Interface at the System Adaptation Layer. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_socket_adapter.h"

int HILINK_GetAddrInfo(const char *nodename, const char *servname,
    const HiLinkAddrInfo *hints, HiLinkAddrInfo **result)
{
    return app_call4(APP_CALL_HILINK_GET_ADDR_INFO, int, const char *, nodename, const char *, servname,
        const HiLinkAddrInfo *, hints, HiLinkAddrInfo **, result);
}

void HILINK_FreeAddrInfo(HiLinkAddrInfo *addrInfo)
{
    app_call1(APP_CALL_HILINK_FREE_ADDR_INFO, void, HiLinkAddrInfo *, addrInfo);
}

int HILINK_Socket(HiLinkSocketDomain domain, HiLinkSocketType type, HiLinkSocketProto proto)
{
    return app_call3(APP_CALL_HILINK_SOCKET, int,
        HiLinkSocketDomain, domain, HiLinkSocketType, type, HiLinkSocketProto, proto);
}

void HILINK_Close(int fd)
{
    app_call1(APP_CALL_HILINK_CLOSE, void, int, fd);
}

int HILINK_SetSocketOpt(int fd, HiLinkSocketOption option, const void *value, unsigned int len)
{
    return app_call4(APP_CALL_HILINK_SET_SOCKET_OPT, int, int, fd, HiLinkSocketOption, option,
        const void *, value, unsigned int, len);
}

int HILINK_Bind(int fd, const HiLinkSockaddr *addr, unsigned int addrLen)
{
    return app_call3(APP_CALL_HILINK_BIND, int, int, fd, const HiLinkSockaddr *, addr, unsigned int, addrLen);
}

int HILINK_Connect(int fd, const HiLinkSockaddr *addr, unsigned int addrLen)
{
    return app_call3(APP_CALL_HILINK_CONNECT, int, int, fd, const HiLinkSockaddr *, addr, unsigned int, addrLen);
}

int HILINK_Recv(int fd, unsigned char *buf, unsigned int len)
{
    return app_call3(APP_CALL_HILINK_RECV, int, int, fd, unsigned char *, buf, unsigned int, len);
}

int HILINK_Send(int fd, const unsigned char *buf, unsigned int len)
{
    return app_call3(APP_CALL_HILINK_SEND, int, int, fd, const unsigned char *, buf, unsigned int, len);
}

int HILINK_RecvFrom(int fd, unsigned char *buf, unsigned int len,
    HiLinkSockaddr *from, unsigned int *fromLen)
{
    return app_call5(APP_CALL_HILINK_RECV_FROM, int, int, fd, unsigned char *, buf, unsigned int, len,
        HiLinkSockaddr *, from, unsigned int *, fromLen);
}

int HILINK_SendTo(int fd, const unsigned char *buf, unsigned int len,
    const HiLinkSockaddr *to, unsigned int toLen)
{
    return app_call5(APP_CALL_HILINK_SEND_TO, int, int, fd, const unsigned char *, buf, unsigned int, len,
        const HiLinkSockaddr *, to, unsigned int, toLen);
}

int HILINK_Select(HiLinkFdSet *readSet, HiLinkFdSet *writeSet, HiLinkFdSet *exceptSet, unsigned int ms)
{
    return app_call4(APP_CALL_HILINK_SELECT, int, HiLinkFdSet *, readSet,
        HiLinkFdSet *, writeSet, HiLinkFdSet *, exceptSet, unsigned int, ms);
}

int HILINK_GetSocketErrno(int fd)
{
    return app_call1(APP_CALL_HILINK_GET_SOCKET_ERRNO, int, int, fd);
}

unsigned int HILINK_Htonl(unsigned int hl)
{
    return app_call1(APP_CALL_HILINK_HTONL, unsigned int, unsigned int, hl);
}

unsigned int HILINK_Ntohl(unsigned int nl)
{
    return app_call1(APP_CALL_HILINK_NTOHL, unsigned int, unsigned int, nl);
}

unsigned short HILINK_Htons(unsigned short hs)
{
    return app_call1(APP_CALL_HILINK_HTONS, unsigned short, unsigned short, hs);
}

unsigned short HILINK_Ntohs(unsigned short ns)
{
    return app_call1(APP_CALL_HILINK_NTOHS, unsigned short, unsigned short, ns);
}

unsigned int HILINK_InetAton(const char *ip, unsigned int *addr)
{
    return app_call2(APP_CALL_HILINK_INET_ATON, unsigned int, const char *, ip, unsigned int *, addr);
}

unsigned int HILINK_InetAddr(const char *ip)
{
    return app_call1(APP_CALL_HILINK_INET_ADDR, unsigned int, const char *, ip);
}

const char *HILINK_InetNtoa(unsigned int addr, char *buf, unsigned int buflen)
{
    return app_call3(APP_CALL_HILINK_INET_NTOA, const char *, unsigned int, addr, char *, buf, unsigned int, buflen);
}