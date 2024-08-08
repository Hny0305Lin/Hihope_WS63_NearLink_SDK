/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: LiteOS Disk Module Inside Headfile
 * Author: Huawei LiteOS Team
 * Create: 2019-06-12
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
 * @defgroup disk Disk
 * @ingroup filesystem
 */

#ifndef _DISK_PRI_H
#define _DISK_PRI_H

#include "disk.h"
#include "los_mux.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#define DISK_MAX_SECTOR_SIZE        512

#define PAR_OFFSET           446     /* MBR: Partition table offset (2) */
#define BS_SIG55AA           510     /* Signature word (2) */
#define BS_FILSYSTEMTYPE32   82      /* File system type (1) */
#define BS_JMPBOOT           0       /* x86 jump instruction (3-byte) */
#define BS_FILSYSTYPE        0x36    /* File system type (2) */
#define BS_SIG55AA_VALUE     0xAA55

#define PAR_TYPE_OFFSET      4
#define PAR_START_OFFSET     8
#define PAR_COUNT_OFFSET     12
#define PAR_TABLE_SIZE       16
#define EXTENDED_PAR         0x0F
#define EXTENDED_8G          0x05

#define BS_FS_TYPE_MASK      0xFFFFFF
#define BS_FS_TYPE_VALUE     0x544146
#define BS_FS_TYPE_FAT       0x0B
#define BS_FS_TYPE_NTFS      0x07

#define FIRST_BYTE       1
#define SECOND_BYTE      2
#define THIRD_BYTE       3
#define FOURTH_BYTE      4

#define BIT_FOR_BYTE     8

#define LD_WORD_DISK(ptr)    (UINT16)(((UINT16)*((UINT8 *)(ptr) + FIRST_BYTE) << (BIT_FOR_BYTE * FIRST_BYTE)) | \
                                      (UINT16)*(UINT8 *)(ptr))
#define LD_DWORD_DISK(ptr)   (UINT32)(((UINT32)*((UINT8 *)(ptr) + THIRD_BYTE) << (BIT_FOR_BYTE * THIRD_BYTE)) |   \
                                      ((UINT32)*((UINT8 *)(ptr) + SECOND_BYTE) << (BIT_FOR_BYTE * SECOND_BYTE)) | \
                                      ((UINT16)*((UINT8 *)(ptr) + FIRST_BYTE) << (BIT_FOR_BYTE * FIRST_BYTE)) |   \
                                      (*(UINT8 *)(ptr)))

#define LD_QWORD_DISK(ptr)   ((UINT64)(((UINT64)LD_DWORD_DISK(&(ptr)[FOURTH_BYTE]) << (BIT_FOR_BYTE * FOURTH_BYTE)) | \
                              LD_DWORD_DISK(ptr)))

/* Check VBR string, including FAT, NTFS */
#define VERIFY_FS(ptr)       (((LD_DWORD_DISK(&(ptr)[BS_FILSYSTEMTYPE32]) & BS_FS_TYPE_MASK) == BS_FS_TYPE_VALUE) || \
                              !strncmp(&(ptr)[BS_FILSYSTYPE], "FAT", strlen("FAT")) || \
                              !strncmp(&(ptr)[BS_JMPBOOT], "\xEB\x52\x90" "NTFS    ",  \
                                       strlen("\xEB\x52\x90" "NTFS    ")))

#define PARTION_MODE_BTYE    (PAR_OFFSET + PAR_TYPE_OFFSET) /* 0xEE: GPT(GUID), else: MBR */
#define PARTION_MODE_GPT     0xEE /* 0xEE: GPT(GUID), else: MBR */
#define SIGNATURE_OFFSET     0    /* The offset of GPT partition header signature */
#define SIGNATURE_LEN        8    /* The length of GPT signature */
#define HEADER_SIZE_OFFSET   12   /* The offset of GPT header size */
#define TABLE_SIZE_OFFSET    84   /* The offset of GPT table size */
#define TABLE_NUM_OFFSET     80   /* The number of GPT table */
#define TABLE_START_SECTOR   2
#define TABLE_MAX_NUM        128
#define TABLE_SIZE           128
#define GPT_PAR_START_OFFSET      32
#define GPT_PAR_END_OFFSET        40
#define PAR_ENTRY_NUM_PER_SECTOR  4
#define HEADER_SIZE_MASK          0xFFFFFFFF
#define HEADER_SIZE               0x5C
#define HARD_DISK_GUID_OFFSET     56
#define HARD_DISK_GUID_FOR_ESP    0x0020004900460045
#define HARD_DISK_GUID_FOR_MSP    0x007200630069004D
#define PAR_VALID_OFFSET0         0
#define PAR_VALID_OFFSET1         4
#define PAR_VALID_OFFSET2         8
#define PAR_VALID_OFFSET3         12

#define VERIFY_GPT(ptr) ((!strncmp(&(ptr)[SIGNATURE_OFFSET], "EFI PART", SIGNATURE_LEN)) && \
                         ((LD_DWORD_DISK(&(ptr)[HEADER_SIZE_OFFSET]) & HEADER_SIZE_MASK) == HEADER_SIZE))

#define VERITY_PAR_VALID(ptr) ((LD_DWORD_DISK(&(ptr)[PAR_VALID_OFFSET0]) + \
                                LD_DWORD_DISK(&(ptr)[PAR_VALID_OFFSET1]) + \
                                LD_DWORD_DISK(&(ptr)[PAR_VALID_OFFSET2]) + \
                                LD_DWORD_DISK(&(ptr)[PAR_VALID_OFFSET3])) != 0)

/* ESP MSP */
#define VERITY_AVAILABLE_PAR(ptr) ((LD_QWORD_DISK(&(ptr)[HARD_DISK_GUID_OFFSET]) != HARD_DISK_GUID_FOR_ESP) && \
                                   (LD_QWORD_DISK(&(ptr)[HARD_DISK_GUID_OFFSET]) != HARD_DISK_GUID_FOR_MSP))

typedef enum _disk_status_ {
    STAT_UNUSED,
    STAT_INUSED,
    STAT_UNREADY,
    STAT_ONGOING  /* Not a state machine, represents an intermediate state when init and deinit */
} disk_status_e;

#define DISK_LOCK(disk) do {                                               \
    UINT32 ret_ = LOS_MuxPend((disk)->disk_mutex, LOS_WAIT_FOREVER);       \
    if (ret_ != LOS_OK) {                                                  \
        PRINT_ERR("%s %d, mutex lock failed, ret = 0x%x\n",                \
                  __FUNCTION__, __LINE__, ret_);                           \
    }                                                                      \
} while (0)

#define DISK_UNLOCK(disk) do {                                             \
    UINT32 ret_ = LOS_MuxPost((disk)->disk_mutex);                         \
    if (ret_ != LOS_OK) {                                                  \
        PRINT_ERR("%s %d, mutex unlock failed, ret: 0x%x\n",               \
                  __FUNCTION__, __LINE__, ret_);                           \
    }                                                                      \
} while (0)

#ifdef LOSCFG_FS_FAT_CACHE
extern UINT32 GetFatBlockNums(VOID);
extern VOID SetFatBlockNums(UINT32 blockNums);
extern UINT32 GetFatSectorsPerBlock(VOID);
extern VOID SetFatSectorsPerBlock(UINT32 sectorsPerBlock);
#endif
extern INT32 SetDiskPartName(los_part *part, const CHAR *src);
extern INT32 EraseDiskByID(UINT32 diskId, size_t startSector, UINT32 sectors);
extern BOOL IsBlockStatusReady(const struct inode *blkDriver);
extern VOID OsDiskInit(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
