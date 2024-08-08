/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS compat headfile for net configure
 * Author: Huawei LiteOS Team
 * Create: 2022-10-21
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

#ifndef __INCLUDE_NET_CONFIG_H
#define __INCLUDE_NET_CONFIG_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* net configure */

#ifdef LOSCFG_NET_LWIP_SACK             // enable socket and net function
#include "lwip/lwipopts.h"
#define CONFIG_NSOCKET_DESCRIPTORS LWIP_CONFIG_NUM_SOCKETS  // max numbers of socket descriptor

/* max numbers of other descriptors except socket descriptors */
#define CONFIG_NET_SENDFILE         1   // enable sendfile function
#define CONFIG_NET_TCP              1   // enable sendfile and send function
#else
#define CONFIG_NSOCKET_DESCRIPTORS  0
#define CONFIG_NET_SENDFILE         0   // disable sendfile function
#define CONFIG_NET_TCP              0   // disable sendfile and send function
#endif

#endif /* __INCLUDE_NET_CONFIG_H */

