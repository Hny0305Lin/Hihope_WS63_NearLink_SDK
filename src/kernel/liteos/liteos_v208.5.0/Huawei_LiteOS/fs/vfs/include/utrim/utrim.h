/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LiteOS Unite Trim Module Headfile
 * Author: Huawei LiteOS Team
 * Create: 2021-09-05
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
#ifndef _UTRIM_H
#define _UTRIM_H

#include "linux/rbtree.h"
#include "los_list.h"
#include "inode/inode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef INT32 (*UnitTrimFun)(struct inode *dev, UINT64 start, UINT32 size);

typedef struct {
    LOS_DL_LIST listNode;   /* list node */
    struct rb_node rbNode;  /* red-black tree node */

    UINT32 num : 31;        /* trim unit number */
    UINT32 trimFlag : 1;    /* the unit whether can trim */

    UINT32 bitmap[0];
} OsTrimUnit;

typedef struct {
    VOID *dev;
    UnitTrimFun trimFun;

    LOS_DL_LIST freeListHead;
    LOS_DL_LIST sortListHead;
    LOS_DL_LIST bitmapListHead;

    struct rb_root rbRoot;        /* block red-black tree root */

    UINT32 unitBits;
    UINT32 bitmapSize;
    UINT32 bit2SectorScale;
    UINT32 unit2SectorScale;
} OsTrimManage;

INT32 UniTrim(OsTrimManage *manager, UINT64 start, UINT32 trimSize);
INT32 UniUntrim(OsTrimManage *manager, UINT64 start, UINT32 trimSize);
OsTrimManage *UniTrimInit(struct inode *devNode, UINT32 bit2SectorSize, UINT64 sectorCount);
VOID UniTrimDeinit(OsTrimManage *manager);
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  /* _UTRIM_H */
