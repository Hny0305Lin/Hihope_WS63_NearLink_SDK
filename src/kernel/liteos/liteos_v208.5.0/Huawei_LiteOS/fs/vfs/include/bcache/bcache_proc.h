/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LiteOS Bcache Proc Headfile
 * Author: Huawei LiteOS Team
 * Create: 2021-09-28
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
#ifndef _BCACHE_PROC_H
#define _BCACHE_PROC_H

#include "linux/seq_file.h"
#include "proc_fs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BCACHE_PROC_ROOT	"fs"
#define BCACHE_PROC_NAME	"bcache_info"

#define BCACHE_STATS_START(start) UINT64 (start) = LOS_CurrNanosec()
#define BCACHE_STATS_READ_END(bc, sectors, start) UpdateReadStats(bc, sectors, start)
#define BCACHE_STATS_WRITE_END(bc, sectors, start) UpdateWriteStats(bc, sectors, start)

#ifdef LOSCFG_FS_FAT_CACHE_PROC

struct BcacheInitInfo {
    const struct inode *dev;
    UINT32 sectorSize;
    UINT32 sectorPerBlock;
    BOOL inUse; /* Init as FALSE */
};

struct BcacheIoStats {
    Atomic64 writeSectors;
    Atomic64 writeCounts;
    Atomic64 writeCosts; /* in ms */

    Atomic64 readSectors;
    Atomic64 readCounts;
    Atomic64 readCosts; /* in ms */
};

extern UINT32 BcacheProcInit(const struct inode *devNode, const OsBcache *bcache);
extern VOID BcacheProcDeinit(const OsBcache *bcache);
extern VOID UpdateReadStats(const OsBcache *bc, UINT64 sectorCount, UINT64 startTime);
extern VOID UpdateWriteStats(const OsBcache *bc, UINT64 sectorCount, UINT64 startTime);

#else /* !LOSCFG_FS_FAT_CACHE_PROC */

STATIC INLINE UINT32 BcacheProcInit(const struct inode *devNode, OsBcache *bcache)
{
    return ENOERR;
}

STATIC INLINE VOID BcacheProcDeinit(const OsBcache *bcache) {}
STATIC INLINE VOID UpdateReadStats(const OsBcache *bc, UINT64 sectorCount, UINT64 startTime) {}
STATIC INLINE VOID UpdateWriteStats(const OsBcache *bc, UINT64 sectorCount, UINT64 startTime) {}

#endif /* LOSCFG_FS_FAT_CACHE_PROC */

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  /* _BCACHE_PROC_H */
