/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Remote Processor Queue Private HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2022-11-11
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

#ifndef LOS_RPQUEUE_PRI_H
#define LOS_RPQUEUE_PRI_H

#include "los_rpqueue.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define RPQUEUE_LOCK()                  LOS_IntLock()
#define RPQUEUE_UNLOCK(x)               LOS_IntRestore(x)
#define RPQUEUE_DELAY(x)                LOS_TaskDelay(x)

#define OS_RPQUEUE_EMPTY_BYTE          1 /* One byte can't be written */
#define OS_RPQUEUE_SIZE_MAX            0xFFF7U

#define OS_RPQUEUE_WRITE_IDLE          0xE1U
#define OS_RPQUEUE_WRITE_BUSY          0xE2U

#define OS_RPQUEUE_DATA_OK             0xE3U
#define OS_RPQUEUE_HEADER_ERROR        0xE4U
#define OS_RPQUEUE_CB_ERROR            0xE5U

#define OS_RPQUEUE_READ_LOCK           0xE6U
#define OS_RPQUEUE_READ_UNLOCK         0xE7U

#define OS_RPQUEUE_INFO_MAGIC          0xC3C9CACEU

#define OS_RPQUEUE_HEADER_CHECK_SIZE   6U

#define RPQUEUE_TX_MAGIC_GET(obj) ((obj)->txMagic = OsRpqueueTxMagicGet(obj))
#define RPQUEUE_RX_MAGIC_GET(obj) ((obj)->rxMagic = OsRpqueueRxMagicGet(obj))

#define RPQUEUE_ADDR_ALIGN(addr) (((UINTPTR)(addr) + (LOS_RPQUEUE_ALIGN_SIZE - 1)) & (~(LOS_RPQUEUE_ALIGN_SIZE - 1)))

typedef struct {
    volatile UINT32     magic;          /* Use to check the queue is initialized or not. */
    volatile UINT16     rxMagic;        /* Verification area: queueSize, readPos. */
    volatile UINT16     txMagic;        /* Verification area: queueSize, writeEnd, writePos, writeStatus, readLock. */
    volatile UINT16     queueSize;      /* The buffer size which is used to write sending data. */
    volatile UINT16     writeEnd;       /* Tail of actual data. */
    volatile UINT16     readPos;        /* The read position. */
    volatile UINT16     writePos;       /* The write position. */
    volatile UINT8      writeStatus;    /* When this parameter is set to busy,
                                           other tasks are not allowed to send messages. */
    volatile UINT8      readLock;       /* When this parameter is set to lock,
                                           the receiver is not allowed to receive messages. */
    volatile UINT8      dataError;      /* Receiver sets this parameter to error when xor error,
                                           the transmitter will re-init the queue before sending. */
    UINT8               reserved[5];    /* Reserved for 8 bytes alignment. */
    UINT8               queue[0];       /* Queue buffer, the buffer length is 0 to facilitate
                                           the calculation of offset. */
} LosRpqueueCB;

typedef struct {
    UINT8              cmd;             /* Packet command. */
    UINT8              option;          /* Packet option. */
    UINT16             sequence;        /* Packet sequence, used for synchronous communication. */
    UINT16             len;             /* Packet payload length. */
    UINT8              magic;           /* The magic byte to check data. */
    UINT8              reserved;        /* Reserved for 8 bytes alignment. */
    UINT8              payload[0];      /* Payload buffer, the buffer length is 0 to facilitate
                                           the calculation of offset. */
} LosRpqueueHeader;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOS_RPQUEUE_PRI_H */
