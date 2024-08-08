/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2015 Aleph One Ltd.
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This software is the subject of a Licence Agreement between Aleph One Limited and Hi_Silicon_Huawei_DigiMedia.
 *
 */

#ifndef __YAFFS_NAND_DRV_H__
#define __YAFFS_NAND_DRV_H__

#include "yaffs_guts.h"
#include "mtd_partition.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern struct yaffs_dev *yaffsfs_FindMountPoint(const YCHAR *path);
extern void yaffs_remove_device(struct yaffs_dev *dev);
extern int nand_check_block(const struct yaffs_dev *dev, unsigned block);
extern int nand_mark_bad_block(const struct yaffs_dev *dev, unsigned block);
extern int nand_erase_block(const struct yaffs_dev *dev, unsigned block);
extern int nand_read_page(struct yaffs_dev *dev,
			 unsigned page,
			 unsigned char *data, unsigned data_len,   /* page data */
			 unsigned char *spare, unsigned spare_len, /* page spare */
			 enum yaffs_ecc_result *ecc_status);
extern int nand_write_page(const struct yaffs_dev *dev,
			  unsigned page,
			  const unsigned char *data, unsigned data_len,    /* page data */
			  const unsigned char *spare, unsigned spare_len); /* page spare */

int yaffs_nand_install_drv(const YCHAR *name, const mtd_partition *partition);
void yaffs_nand_delete_drv(const YCHAR *name);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __YAFFS_NAND_DRV_H__ */
