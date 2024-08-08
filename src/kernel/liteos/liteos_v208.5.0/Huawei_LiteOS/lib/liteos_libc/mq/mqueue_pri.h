/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS mqueue private header
 * Author: Huawei LiteOS Team
 * Create: 2022-02-22
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

#ifndef _MQUEUE_PRI_H
#define _MQUEUE_PRI_H

#include "los_typedef.h"
#include "limits.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup mqueue
 * Maximum number of messages in a message queue
 */
#define MQ_MAX_MSG_NUM    16

/**
 * @ingroup mqueue
 * Maximum size of a single message in a message queue
 */
#define MQ_MAX_MSG_LEN    64

#define MQ_USE_MAGIC      0x89abcdef

struct mqarray {
    UINT32 mq_id;
    void *mqcb;
    struct mqpersonal *mq_personal;
    char mq_name[PATH_MAX];
    BOOL unlinkflag;
};

struct mqpersonal {
    struct mqarray *mq_posixdes;
    struct mqpersonal *mq_next;
    int mq_flags;
    UINT32 mq_status;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MQUEUE_PRI_H */
