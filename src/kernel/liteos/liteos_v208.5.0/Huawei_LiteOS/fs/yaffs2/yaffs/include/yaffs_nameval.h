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

#ifndef __NAMEVAL_H__
#define __NAMEVAL_H__

#include "yportenv.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef YAFFS_EXTEND_SUPPORT

struct yaffs_dev;

int nval_del(struct yaffs_dev *dev, char *xb, int xb_size, const YCHAR * name);
int nval_set(struct yaffs_dev *dev,
	     char *xb, int xb_size, const YCHAR * name, const char *buf,
	     int bsize, int flags);
int nval_get(struct yaffs_dev *dev,
	     const char *xb, int xb_size, const YCHAR * name, char *buf,
	     int bsize);
int nval_list(struct yaffs_dev *dev,
	      const char *xb, int xb_size, char *buf, int bsize);
int nval_hasvalues(struct yaffs_dev *dev, const char *xb, int xb_size);

#endif /* YAFFS_EXTEND_SUPPORT */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NAMEVAL_H__ */
