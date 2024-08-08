/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS Remote Processor Queue Module HeaderFile
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

/**
 * @defgroup los_rpqueue
 * @ingroup kernel
 */

#ifndef LOS_RPQUEUE_H
#define LOS_RPQUEUE_H

#ifdef LOS_CONFIG_WITHOUT_OS
#include "los_typedef.h"
#include "los_toolchain.h"
#include "los_errno.h"
#include "los_rpqueue_bsp.h"
#else
#include "los_typedef.h"
#include "los_hwi.h"
#include "los_task.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LOS_RPQUEUE_OPTION_SYNC_MSG_SEND            0x0001U
#define LOS_RPQUEUE_ALIGN_SIZE                      8U

/**
 * @ingroup los_rpqueue
 * Task error code: Null parameter.
 *
 * Value: 0x02002501.
 *
 * Solution: Check the parameter.
 */
#define LOS_ERRNO_RPQUEUE_PTR_NULL                  LOS_ERRNO_OS_ERROR(LOS_MOD_RPQUEUE, 0x01)

/**
 * @ingroup los_rpqueue
 * Task error code: The rpqueue is damaged.
 *
 * Value: 0x02002502.
 *
 * Solution: Receiver: Wait for transmitter to re-initialize this queue.
 * Transmitter: Call LOS_RpqueueInit to re-initialize this queue.
 */
#define LOS_ERRNO_RPQUEUE_CB_DAMAGED                LOS_ERRNO_OS_ERROR(LOS_MOD_RPQUEUE, 0x02)

/**
 * @ingroup los_rpqueue
 * Task error code: The Rpqueue is invalid.
 *
 * Value: 0x02002503.
 *
 * Solution: Check the queue and initialize it.
 */
#define LOS_ERRNO_RPQUEUE_INVALID                   LOS_ERRNO_OS_ERROR(LOS_MOD_RPQUEUE, 0x03)

/**
 * @ingroup los_rpqueue
 * Task error code: The Rpqueue is writing.
 *
 * Value: 0x02002504.
 *
 * Solution: Waiting for queue release.
 */
#define LOS_ERRNO_RPQUEUE_WRITE_BUSY                LOS_ERRNO_OS_ERROR(LOS_MOD_RPQUEUE, 0x04)

/**
 * @ingroup los_rpqueue
 * Task error code: The Rpqueue is empty.
 *
 * Value: 0x02002505.
 *
 * Solution: None.
 */
#define LOS_ERRNO_RPQUEUE_BUF_EMPTY                 LOS_ERRNO_OS_ERROR(LOS_MOD_RPQUEUE, 0x05)

/**
 * @ingroup los_rpqueue
 * Task error code: The Rpqueue is not enough to write data.
 *
 * Value: 0x02002506.
 *
 * Solution: Wait for the receiver to read the data.
 */
#define LOS_ERRNO_RPQUEUE_BUF_NOT_ENOUGH            LOS_ERRNO_OS_ERROR(LOS_MOD_RPQUEUE, 0x06)

/**
 * @ingroup los_rpqueue
 * Task error code: Systemic failure has occurred.
 *
 * Value: 0x03002507.
 *
 * Solution: This is a major failure and requires the highest level of security checks.
 */
#define LOS_ERRNO_RPQUEUE_WRITE_ERROR               LOS_ERRNO_OS_FATAL(LOS_MOD_RPQUEUE, 0x07)

/**
 * @ingroup los_rpqueue
 * Task error code: The Rpqueue buffer size is too small to init.
 *
 * Value: 0x02002508.
 *
 * Solution: Use bigger size.
 */
#define LOS_ERRNO_RPQUEUE_SIZE_TOO_SMALL            LOS_ERRNO_OS_ERROR(LOS_MOD_RPQUEUE, 0x08)

/**
 * @ingroup los_rpqueue
 * Task error code: The Rpqueue is in read lock.
 *
 * Value: 0x02002509.
 *
 * Solution: Try again.
 */
#define LOS_ERRNO_RPQUEUE_READ_LOCK                 LOS_ERRNO_OS_ERROR(LOS_MOD_RPQUEUE, 0x09)

/**
 * @ingroup los_rpqueue
 * Task error code: The header data is damaged.
 *
 * Value: 0x0200250A.
 *
 * Solution: Waiting for transmitter to re-initialize this queue.
 */
#define LOS_ERRNO_RPQUEUE_HEADER_DAMAGED            LOS_ERRNO_OS_ERROR(LOS_MOD_RPQUEUE, 0x0A)

/**
 * @ingroup los_rpqueue
 * Task error code: This queue has been initialized.
 *
 * Value: 0x0200250B.
 *
 * Solution: Process the return value according to the actual situation.
 */
#define LOS_ERRNO_RPQUEUE_INITED                    LOS_ERRNO_OS_ERROR(LOS_MOD_RPQUEUE, 0x0B)

/**
 * @ingroup los_rpqueue
 * Task error code: The Rpqueue buffer size is too large to init.
 *
 * Value: 0x0200250C.
 *
 * Solution: Use smaller size.
 */
#define LOS_ERRNO_RPQUEUE_SIZE_TOO_LARGE            LOS_ERRNO_OS_ERROR(LOS_MOD_RPQUEUE, 0x0C)

typedef struct {
    UINT32 timeout;    /**< Packet retransmission times */
    UINT8  option;     /**< Packet send/receive option */
    UINT8  cmd;        /**< Packet command, used to identify packets */
} LosRpqueueSendCfg;

typedef VOID (*LosRpqueueCallback)(UINT8 cmd, UINT8 *data, UINT16 len);

/**
 * @ingroup los_rpqueue
 * @brief Rpqueue init.
 *
 * @par Description:
 * Initializing the Remote Processor Queue. Initialize at the transmit end.
 * @attention
 * Non-reentrant.
 *
 * @param queue   [IN] Rpqueue address.
 * @param size    [IN] Rpqueue buffer size, size range is 24 - 65543 bytes.
 * @param option  [IN] Rpqueue option.
 *
 * @retval #LOS_ERRNO_RPQUEUE_PTR_NULL        Queue address is NULL.
 * @retval #LOS_ERRNO_RPQUEUE_INITED          This queue has been initialized.
 * @retval #LOS_ERRNO_RPQUEUE_SIZE_TOO_SMALL  The Rpqueue buffer size is too small to init.
 * @retval #LOS_ERRNO_RPQUEUE_SIZE_TOO_LARGE  The Rpqueue buffer size is too large to init.
 * @retval #LOS_OK                            Initialization is successful.
 * @par Dependency:
 * <ul><li>los_rpqueue.h: the header file that contains the API declaration.</li></ul>
 */
UINT32 LOS_RpqueueInit(VOID *queue, UINT32 size, UINT32 option);

/**
 * @ingroup los_rpqueue
 * @brief get Rpqueue current max send size.
 *
 * @par Description:
 * get Rpqueue current max send size.
 * @attention
 * Non-reentrant.
 *
 * @param queue          [IN] Remote Processor address.
 * @param maxSendSize    [OUT] Max send size, the value of 0 also is useful.
 *
 * @retval #LOS_ERRNO_RPQUEUE_PTR_NULL        Pointer is NULL.
 * @retval #LOS_ERRNO_RPQUEUE_INVALID         Queue address is invalid.
 * @retval #LOS_ERRNO_RPQUEUE_BUF_NOT_ENOUGH  Rpqueue remaining space is not enough to send.
 * @retval #LOS_OK                            Get max send size successfully.
 * @par Dependency:
 * <ul><li>los_rpqueue.h: the header file that contains the API declaration.</li></ul>
 */
UINT32 LOS_RpqueueMaxSendSizeGet(VOID *queue, UINT16 *maxSendSize);

/**
 * @ingroup los_rpqueue
 * @brief Rpqueue send message.
 *
 * @par Description:
 * Rpqueue send message.
 * @attention
 * Non-reentrant.
 *
 * @param queue    [IN] Rpqueue address.
 * @param data     [IN] Send data buffer.
 * @param len      [IN] Send data size.
 * @param cfg      [IN] The config of sending.
 *
 * @retval #LOS_ERRNO_RPQUEUE_PTR_NULL          Rpqueue address is NULL.
 * @retval #LOS_ERRNO_RPQUEUE_INVALID           Rpqueue address is invalid.
 * @retval #LOS_ERRNO_RPQUEUE_WRITE_BUSY        Rpqueue write busy.
 * @retval #LOS_ERRNO_RPQUEUE_BUF_NOT_ENOUGH    Rpqueue remaining space is not enough to send.
 * @retval #LOS_ERRNO_RPQUEUE_CB_DAMAGED        The rpqueue is damaged.
 * @retval #LOS_OK                              Sending success.
 * @par Dependency:
 * <ul><li>los_rpqueue.h: the header file that contains the API declaration.</li></ul>
 */
UINT32 LOS_RpqueueSend(VOID *queue, const UINT8 *data, UINT16 len, const LosRpqueueSendCfg *cfg);

/**
 * @ingroup los_rpqueue
 * @brief Rpqueue data receive.
 *
 * @par Description:
 * Rpqueue data receive, when data is received, the callback function will be called.
 * @attention
 * Non-reentrant.
 *
 * @param queue        [IN] Rpqueue address.
 * @param recvDataCb   [IN] Receive data callback.
 *
 * @retval #LOS_ERRNO_RPQUEUE_READ_LOCK       Rpqueue is in read lock.
 * @retval #LOS_ERRNO_RPQUEUE_PTR_NULL        Rpqueue address is NULL.
 * @retval #LOS_ERRNO_RPQUEUE_INVALID         Rpqueue address is invalid.
 * @retval #LOS_ERRNO_RPQUEUE_HEADER_DAMAGED  The header data is damaged.
 * @retval #LOS_ERRNO_RPQUEUE_BUF_EMPTY       The Rpqueue is empty.
 * @retval #LOS_ERRNO_RPQUEUE_CB_DAMAGED      The rpqueue is damaged.
 * @retval #LOS_OK                            Initialization is successful.
 * @par Dependency:
 * <ul><li>los_rpqueue.h: the header file that contains the API declaration.</li></ul>
 */
UINT32 LOS_RpqueueRecv(VOID *queue, LosRpqueueCallback recvDataCb);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOS_RPQUEUE_H */