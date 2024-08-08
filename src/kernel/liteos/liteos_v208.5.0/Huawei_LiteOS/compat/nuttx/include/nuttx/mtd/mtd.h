/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS compat headfile for nuttx mtd
 * Author: Huawei LiteOS Team
 * Create: 2022-07-15
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

#ifndef __INCLUDE_NUTTX_MTD_H
#define __INCLUDE_NUTTX_MTD_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#ifdef ROMFS_MOUNT_ON_FLASH
#include "mtd_partition.h"
#include "linux/mtd/mtd.h"
#endif

#include "sys/types.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define MTD_BREAD(d, s, n, b) ((inode->u.i_bops->read != NULL) ? inode->u.i_bops->read(inode, b, s, n) : (-ENOSYS))
#define MTD_IOCTL(d, c, a)    ((inode->u.i_bops->ioctl != NULL) ? inode->u.i_bops->ioctl(inode, c, a) : (-ENOSYS))

struct mtd_geometry_s {
    uint32_t blocksize;
    uint32_t erasesize;
    uint32_t neraseblocks;
};

#endif /* __INCLUDE_NUTTX_MTD_H */