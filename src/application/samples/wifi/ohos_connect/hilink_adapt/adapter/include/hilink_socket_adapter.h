/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 系统适配层网络Socket接口（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_SOCKET_ADAPTER_H
#define HILINK_SOCKET_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum HiLinkSocketDomain {
    HILINK_SOCKET_DOMAIN_AF_INET = 0,
    HILINK_SOCKET_DOMAIN_AF_INET6,
    HILINK_SOCKET_DOMAIN_UNSPEC,
    /* 避免编译器优化，限制该枚举值为32-bit */
    HILINK_SOCKET_DOMAIN_RESERVED = 0x7FFFFFFF
} HiLinkSocketDomain;

typedef enum HiLinkSocketType {
    HILINK_SOCKET_TYPE_STREAM = 0,
    HILINK_SOCKET_TYPE_DGRAM,
    HILINK_SOCKET_TYPE_RAW,
    /* 避免编译器优化，限制该枚举值为32-bit */
    HILINK_SOCKET_TYPE_RESERVED = 0x7FFFFFFF
} HiLinkSocketType;

typedef enum HiLinkSocketProto {
    HILINK_SOCKET_PROTO_IP = 0,
    HILINK_SOCKET_PROTO_TCP,
    HILINK_SOCKET_PROTO_UDP,
    /* 避免编译器优化，限制该枚举值为32-bit */
    HILINK_SOCKET_PROTO_RESERVED = 0x7FFFFFFF
} HiLinkSocketProto;

typedef enum HiLinkSocketOption {
    /* 将套接字设置为阻塞模式 */
    HILINK_SOCKET_OPTION_SETFL_BLOCK = 0,
    /* 将套接字设置为非阻塞模式 */
    HILINK_SOCKET_OPTION_SETFL_NONBLOCK,
    /* 设置套接字读取超时时间，附带参数类型为unsigned int *，单位为ms */
    HILINK_SOCKET_OPTION_READ_TIMEOUT,
    /* 设置套接字发送超时时间，附带参数类型为unsigned int *，单位为ms */
    HILINK_SOCKET_OPTION_SEND_TIMEOUT,
    /* 允许套接字重复绑定地址 */
    HILINK_SOCKET_OPTION_ENABLE_REUSEADDR,
    /* 禁用套接字重复绑定地址 */
    HILINK_SOCKET_OPTION_DISABLE_REUSEADDR,
    /* 设置套接字加入组播组，附带参数类型为const char *，为组播点分ip地址字符串 */
    HILINK_SOCKET_OPTION_ADD_MULTI_GROUP,
    /* 设置套接字退出组播组，附带参数类型为const char *，为组播点分ip地址字符串 */
    HILINK_SOCKET_OPTION_DROP_MULTI_GROUP,
    /* 允许套接字发送广播 */
    HILINK_SOCKET_OPTION_ENABLE_BROADCAST,
    /* 禁用套接字发送广播 */
    HILINK_SOCKET_OPTION_DISABLE_BROADCAST,
    /* 允许套接字接收组播数据回环 */
    HILINK_SOCKET_OPTION_ENABLE_MULTI_LOOP,
    /* 禁用套接字接收组播数据回环 */
    HILINK_SOCKET_OPTION_DISABLE_MULTI_LOOP,
    /* 设置套接字发送缓冲区大小，附带参数类型为(unsigned int *) */
    HILINK_SOCKET_OPTION_SEND_BUFFER,
    /* 设置套接字读取缓冲区大小，附带参数类型为(unsigned int *) */
    HILINK_SOCKET_OPTION_READ_BUFFER,
    /* 避免编译器优化，限制该枚举值为32-bit */
    HILINK_SOCKET_OPTION_RESERVED = 0x7FFFFFFF
} HiLinkSocketOption;

typedef struct HiLinkSockaddr {
    unsigned short saFamily;
#define HILINK_SA_DATA_LEN     14
    char saData[HILINK_SA_DATA_LEN];
} HiLinkSockaddr;

typedef struct HiLinkSockaddrIn {
    unsigned short sinFamily;
    unsigned short sinPort;
    unsigned int sinAddr;
#define HILINK_SIN_ZERO_LEN    8
    char sinZero[HILINK_SIN_ZERO_LEN];
} HiLinkSockaddrIn;

typedef struct HiLinkAddrInfo HiLinkAddrInfo;

struct HiLinkAddrInfo {
    int aiFlags;
    int aiFamily;
    int aiSocktype;
    int aiProtocol;
    int aiAddrlen;
    HiLinkSockaddr *aiAddr;
    char *aiCanonname;
    HiLinkAddrInfo *aiNext;
};

typedef struct HiLinkFdSet {
    unsigned int num;
    int *fdSet;
} HiLinkFdSet;

enum HiLinkSocketErrno {
    HILINK_SOCKET_ERRNO_NO_ERROR    = 0,
    HILINK_SOCKET_ERRNO_EINTR       = 4,
    HILINK_SOCKET_ERRNO_EAGAIN      = 11,
    HILINK_SOCKET_ERRNO_EWOULDBLOCK = HILINK_SOCKET_ERRNO_EAGAIN,
    HILINK_SOCKET_ERRNO_EINPROGRESS = 115,
};

/*
 * 功能: 将主机名转换为地址信息
 * 参数: nodename，主机名
 *       servname，服务名
 *       hints，指定参数
 *       result，出参，地址信息链表
 * 返回: 0，获取成功
 *       其他，获取失败
 */
int HILINK_GetAddrInfo(const char *nodename, const char *servname,
    const HiLinkAddrInfo *hints, HiLinkAddrInfo **result);

/*
 * 功能: 释放HILINK_GetAddrInfo获取的地址信息链表
 * 参数: addrInfo，地址信息链表
 */
void HILINK_FreeAddrInfo(HiLinkAddrInfo *addrInfo);

/*
 * 功能: 获取获取INET网络套接字文件描述符
 * 参数: type，指定协议类型
 * 返回: 大于等于0，返回套接字描述符
 *       小于0，获取失败
 */
int HILINK_Socket(HiLinkSocketDomain domain, HiLinkSocketType type, HiLinkSocketProto proto);

/*
 * 功能: 关闭套接字
 * 参数: fd，指定套接字
 */
void HILINK_Close(int fd);

/*
 * 功能: 设置套接字可选字段
 * 参数: fd，指定套接字
 *       option，设定的选项
 *       value，选项参数
 *       len，选项参数长度
 * 返回: 0，设置成功
 *       -1，设置失败
 */
int HILINK_SetSocketOpt(int fd, HiLinkSocketOption option, const void *value, unsigned int len);

/*
 * 功能: 绑定套接字
 * 参数: fd，指定的套接字
 *       addr，指向包含要绑定到套接字的地址
 *       addrLen，套接字地址大小，当前不使用，为ipv6预留
 * 返回: 0，绑定成功
 *       -1，绑定失败
 */
int HILINK_Bind(int fd, const HiLinkSockaddr *addr, unsigned int addrLen);

/*
 * 功能: 连接对端IP地址
 * 参数: fd，指定的套接字文件描述符
 *       addr，指向套接字连接的地址
 * 返回: 0，设置成功
 *       其他，设置失败
 */
int HILINK_Connect(int fd, const HiLinkSockaddr *addr, unsigned int addrLen);

/*
 * 功能: 从已经连接的套接字接收消息
 * 参数: fd，指定的套接字
 *       buf，接收数据缓冲区
 *       len，缓冲区长度
 * 返回: 大于等于0，实际接收的字节数
 *       小于0，读取出错，返回错误码
 */
int HILINK_Recv(int fd, unsigned char *buf, unsigned int len);

/*
 * 功能: 传输指定长度消息到对端
 * 参数: fd，指定的套接字
 *       buf，要发送的数据
 *       len，数据长度
 * 返回: 大于等于0，实际发送的字节数
 *       小于0，发送失败，返回错误码
 */
int HILINK_Send(int fd, const unsigned char *buf, unsigned int len);

/*
 * 功能: 从套接字接收消息，并返回源地址
 * 参数: fd，指定的套接字
 *       buf，接收数据缓冲区
 *       len，缓冲区长度
 *       from，出参，数据源地址
 * 返回: 大于等于0，实际接收的字节数
 *       小于0，接收出错，返回错误码
 */
int HILINK_RecvFrom(int fd, unsigned char *buf, unsigned int len, HiLinkSockaddr *from, unsigned int *fromLen);

/*
 * 功能: 发送数据到指定地址
 * 参数: fd，指定的套接字
 *       buf，要发送的数据
 *       len，数据长度
 *       to，目标地址信息
 * 返回: 大于0，实际发送的字节数
 *       小于等于0，发送失败，返回错误码
 */
int HILINK_SendTo(int fd, const unsigned char *buf, unsigned int len, const HiLinkSockaddr *to, unsigned int toLen);

/*
 * 功能: 检视多个文件描述符
 * 参数: rs，待检查是否准备好读取的描述符集合
 *       ws，待检查是否准备好写入的描述符集合
 *       es，待检查挂起错误条件的描述符集合
 *       ms，超时时间，为HILINK_WAIT_FOREVER时不会超时
 * 返回: 大于0，与条件相符的描述符数量
 *       等于0，超时
 *       小于0，套接字出错，返回错误码
 * 注意: 返回时需将不符合要求的描述符置为-1
 */
int HILINK_Select(HiLinkFdSet *readSet, HiLinkFdSet *writeSet, HiLinkFdSet *exceptSet, unsigned int ms);

/*
 * 功能: 获取描述符错误码
 * 参数: fd，指定的套接字，小于0时返回系统errno
 * 返回: 错误码，参考HiLinkSocketErrno
 * 注意：在HiLinkSocketErrno定义范围内的错误码需严格按照定义返回
 */
int HILINK_GetSocketErrno(int fd);

int get_os_errno(void);

/*
 * 功能: 获取全局错误码
 * 返回: 全局错误码错误码
 */
typedef int (*GetErrno)(void);

/*
 * 功能: 注册获取全局错误码回调函数，不注册默认使用errno
 * 参数: cb，获取全局错误码回调函数
 * 返回: 0表示成功，其他表示失败
 */
int HILINK_RegisterErrnoCallback(GetErrno cb);

/*
 * 功能: 将32位的主机序转为网络序
 * 参数: hl，32位主机序
 * 返回: 32位网络序
 */
unsigned int HILINK_Htonl(unsigned int hl);

/*
 * 功能: 将32位的网络序转为主机序
 * 参数: netlong，32位网络序
 * 返回: 32位主机序
 */
unsigned int HILINK_Ntohl(unsigned int nl);

/*
 * 功能: 将16位的主机序转为网络序
 * 参数: hostlong，16位主机序
 * 返回: 16位网络序
 */
unsigned short HILINK_Htons(unsigned short hs);

/*
 * 功能: 将16位的网络序转为主机序
 * 参数: netlong，16位网络序
 * 返回: 16位主机序
 */
unsigned short HILINK_Ntohs(unsigned short ns);

/*
 * 功能: 将点分ip地址字符串转为32位网络序地址
 * 参数: ip，点分ip地址字符串
 *       addr，出参，32位网络序地址
 * 返回: 0，转换失败
 *       非0，转换成功
 */
unsigned int HILINK_InetAton(const char *ip, unsigned int *addr);

/*
 * 功能: 将点分ip地址字符串转为32位网络序地址
 * 参数: ip，点分ip地址字符串
 * 返回: 0xFFFFFFFF，转换失败
 *       其他，转换成功
 * 注意: 需要正确转换255.255.255.255请使用HILINK_InetAton
 */
unsigned int HILINK_InetAddr(const char *ip);

/*
 * 功能: 将32位网络序地址转为点分ip地址字符串
 * 参数: addr，32位网络序地址
 * 返回: NULL，转换失败
 *       非NULL，转换成功
 */
const char *HILINK_InetNtoa(unsigned int addr, char *buf, unsigned int buflen);

#ifdef __cplusplus
}
#endif

#endif /* HILINK_SOCKET_ADAPTER_H */
