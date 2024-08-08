/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
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

#ifndef __MTD_NANDCFG_H__
#define __MTD_NANDCFG_H__

#include "linux/mtd/mtd.h"
#include "mtd_partition.h"
#include "yaffs_adapt_os.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern struct mtd_info *yaffs_nand_mtd;

#define YAFFS_OOB_SIZE          (yaffs_nand_mtd->oobsize)
#define YAFFS_PAGE_DATA_SIZE    (yaffs_nand_mtd->writesize)
#define YAFFS_PAGE_PER_BLOCK    (yaffs_nand_mtd->erasesize / yaffs_nand_mtd->writesize)

#define NAND_BLOCK_ERASE_SIZE   (yaffs_nand_mtd->erasesize)
#define NAND_PAGE_SIZE          (YAFFS_PAGE_DATA_SIZE + YAFFS_OOB_SIZE)
#define NAND_BLOCK_DATA_SIZE    (YAFFS_PAGE_DATA_SIZE * YAFFS_PAGE_PER_BLOCK)

#define CONFIG_YAFFS_ECC_MODE	  1 /* 1 use ecc, 0 no ecc */
#define CONFIG_YAFFS_INBAND_TAGS  0 /* 1 use in band tags, 0-no in band tags */
#define CONFIG_YAFFS_USE_YAFFS2   1 /* 1 yaffs2, 0-yaffs1 */
#define CONFIG_YAFFS_NAND_DEFAULT_START_BLOCK 0
#define CONFIG_YAFFS_NAND_DEFAULT_END_BLOCK (yaffs_nand_mtd->size / yaffs_nand_mtd->erasesize - 1)
#define CONFIG_YAFFS_NAND_PAGE_SIZE YAFFS_PAGE_DATA_SIZE
#define CONFIG_YAFFS_NAND_OOB_SIZE YAFFS_OOB_SIZE
#define CONFIG_YAFFS_NAND_PAGES_PER_BLOCK YAFFS_PAGE_PER_BLOCK

int nf_read_page(long page,
		 unsigned char *data, unsigned long data_len,
		 unsigned char *spare, unsigned long spare_len);
int nf_write_page(long page,
		 const unsigned char *data, unsigned long data_len,
		 const unsigned char *spare, unsigned long spare_len);
int nf_move_page(long src_page, long dst_page);
int nf_erase_block(unsigned long block);
int nf_check_block(unsigned long block);
int nf_mark_badblock(unsigned long block);

int os_nand_yaffs_read(void *memaddr, unsigned long start, unsigned long size);
int os_nand_yaffs_erase(unsigned long start, unsigned long size);

int os_nand_yaffs_write(const void *memaddr, unsigned long start, unsigned long size);
int os_nand_yaffs_nand_block_isbad(loff_t ofs);
int os_nand_yaffs_nand_block_markbad(loff_t ofs);
int nand_write_yaffs_skip_bad(struct mtd_info *mtd, loff_t offset,
		 size_t *length, const char *buffer);
int nand_read_yaffs_skip_bad(struct mtd_info *mtd, loff_t offset,
		 size_t *length, char *buffer);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
