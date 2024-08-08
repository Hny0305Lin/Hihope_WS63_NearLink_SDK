/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: System Console HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
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

#ifndef _CONSOLE_H
#define _CONSOLE_H

#include "los_config.h"
#ifdef LOSCFG_FS_VFS
#include "termios.h"
#ifdef LOSCFG_NET_TELNET
#include "telnet_dev.h"
#endif
#include "virtual_serial.h"
#include "los_ringbuf.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Define two fixed console id for Console ID. */
#define CONSOLE_SERIAL 1
#define CONSOLE_TELNET 2

#define STDIN  0
#define STDOUT 1
#define STDERR 2

#define CONSOLE  "/dev/console"
#define CONSOLE_NAMELEN 16

#define CONSOLE_CMD_RD_BLOCK_SERIAL    0x104
#define CONSOLE_CMD_RD_BLOCK_TELNET    101
#define CONSOLE_RD_BLOCK               1
#define CONSOLE_RD_NONBLOCK            0
#define CONSOLE_SHELL_KEY_EVENT        0x112
#define CONSOLE_SHELL_EXITED           0x400
#define CONSOLE_CONTROL_RIGHTS_CAPTURE 201
#define CONSOLE_CONTROL_RIGHTS_RELEASE 202
#define CONSOLE_CONTROL_CAPTURE_LINE   203
#define CONSOLE_CONTROL_CAPTURE_CHAR   204
#define CONSOLE_FIFO_SIZE              1024
#define CONSOLE_NUM                    2

/**
 * @ingroup console
 * @brief Create a console device.
 *
 * @par Description:
 * Create a console device specified by device name.
 * @attention
 * If a console device is already created, the same device cannot create again before
 * it destroyed.
 *
 * @param deviceName    [IN] Charactor device name, can be #SERIAL or #TELNET
 *
 * @retval #-1          Initialize failed
 * @retval #0           Initialize OK
 * @par Dependency:
 * <ul><li>console.h: the header file that contains the API declaration.</li></ul>
 */
extern INT32 system_console_init(const CHAR *deviceName);

/**
 * @ingroup console
 * @brief Destroy a console device.
 *
 * @par Description:
 * Destroy a console device specified by device name. The device shall already created by
 * #system_console_init before destroy.
 *
 * @param deviceName    [IN] Charactor device name, can be #SERIAL or #TELNET
 *
 * @retval #-1          Deinitialize failed
 * @retval #0           Deinitialize OK
 * @par Dependency:
 * <ul><li>console.h: the header file that contains the API declaration.</li></ul>
 */
extern INT32 system_console_deinit(const CHAR *deviceName);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CONSOLE_H */
