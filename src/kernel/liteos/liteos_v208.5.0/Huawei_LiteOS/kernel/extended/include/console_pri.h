/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: System Console Private HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2022-04-02
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

#ifndef _CONSOLE_PRI_H
#define _CONSOLE_PRI_H

#include "console.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_KERNEL_CONSOLE
typedef struct {
    Ringbuf ringbuf;           /* Ring buffer */
    EVENT_CB_S sendEvent;   /* Inform telnet send task */
} RingbufSendCB;

typedef struct {
    UINT32 consoleID;
    UINT32 consoleType;
    UINT32 consoleSem;
    UINT32 shellEntryId;
    UINT32 consoleMask;
    struct inode *devInode;
    CHAR *name;
    INT32 fd;
    UINT32 refCount;
    BOOL isNonBlock;
#ifdef LOSCFG_SHELL
    VOID *shellHandle;
#endif
    UINT32 sendTaskID;
    RingbufSendCB *ringbufSendCB;
    UINT8 fifo[CONSOLE_FIFO_SIZE];
    UINT32 fifoOut;
    UINT32 fifoIn;
    UINT32 currentLen;
    struct termios consoleTermios;
} CONSOLE_CB;

extern CONSOLE_CB *OsGetConsoleByID(INT32 consoleId);
extern CONSOLE_CB *OsGetConsoleByTaskID(UINT32 taskId);
extern UINT32 ConsoleTaskReg(INT32 consoleId, UINT32 taskId);
extern BOOL is_nonblock(const CONSOLE_CB *consoleCB);
extern BOOL IsConsoleOccupied(const CONSOLE_CB *consoleCB);

extern BOOL OsConsoleIsEnabled(VOID);

#ifdef LOSCFG_CONSOLE_UNIFIED_SERIAL_OUTPUT
extern VOID OsConsoleDisable(VOID);
#endif

extern INT32 FilepOpen(struct file *filep, const struct file_operations_vfs *fops);
extern INT32 FilepClose(struct file *filep, const struct file_operations_vfs *fops);
extern INT32 FilepRead(struct file *filep, const struct file_operations_vfs *fops, CHAR *buffer, size_t bufLen);
extern INT32 FilepWrite(struct file *filep, const struct file_operations_vfs *fops, const CHAR *buffer, size_t bufLen);
extern INT32 FilepPoll(struct file *filep, const struct file_operations_vfs *fops, poll_table *fds);
extern INT32 FilepIoctl(struct file *filep, const struct file_operations_vfs *fops, INT32 cmd, unsigned long arg);
extern INT32 GetFilepOps(const struct file *filep, struct file **privFilep, const struct file_operations_vfs **fops);

extern INT32 ConsoleUpdateFd(VOID);
#else
STATIC INLINE INT32 ConsoleUpdateFd(VOID)
{
    return -1;
}
#endif /* LOSCFG_KERNEL_CONSOLE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CONSOLE_PRI_H */
