/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS compat headfile for nuttx net
 * Author: Huawei LiteOS Team
 * Create: 2022-08-15
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#ifndef __INCLUDE_NUTTX_NET_NET_H
#define __INCLUDE_NUTTX_NET_NET_H

#define HTONL                   htonl
#define RPC_PATH_MAX            PATH_MAX
#define CONFIG_NFS_HOSTNAME_LEN ((CONFIG_NFS_MACHINE_NAME_SIZE + sizeof(uint32_t) - 1) / sizeof(uint32_t))

#ifdef LOSCFG_NET_LWIP_SACK
#include "lwip/opt.h"
#include "lwip/sockets.h"

struct socket {
    int fd;
};

void psock_init(struct socket *psock);
bool psock_is_valid(struct socket *psock);
int psock_socket(int domain, int type, int protocol, struct socket *psock);
int psock_bind(struct socket *psock, const struct sockaddr *addr, socklen_t addrlen);
int psock_setsockopt(struct socket *psock, int level, int option, FAR const void *value, socklen_t value_len);
int psock_connect(struct socket *psock, const struct sockaddr *addr, socklen_t addrlen);
ssize_t psock_sendmsg(struct socket *psock, FAR struct msghdr *msg, int flags);
ssize_t psock_send(struct socket *psock, const void *buf, size_t len, int flags);
ssize_t psock_recv(struct socket *psock, void *buf, size_t len, int flags);
void psock_close(struct socket *psock);

#endif /* LOSCFG_NET_LWIP_SACK */
#endif /* __INCLUDE_NUTTX_NET_NET_H */
