/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2019. All rights reserved.
 * Description: LiteOS PROC FS Headfile
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

#ifndef _PROC_FS_H
#define _PROC_FS_H
#include <sys/types.h>

#ifdef LOSCFG_FS_PROC
#include "linux/spinlock.h"
#include "asm/atomic.h"
#include "fs/fs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef unsigned short fmode_t;
#define MAX_NAMELEN 32

struct proc_file;
struct proc_dir_entry;
struct proc_file_operations;

typedef int (*read_proc_t)(struct proc_file *pf, void *buf);
typedef int (*write_proc_t)(struct proc_file *pf, const char *buf, size_t count, loff_t *ppos);

struct proc_file_operations {
	char *name;

	/* make a proc file */
	loff_t (*llseek)(struct proc_file *pf, loff_t offset, int whence);
	int (*open)(struct inode *inode, struct proc_file *pf);
	int (*release)(struct inode *inode, struct proc_file *pf);
	ssize_t (*read)(struct proc_file *pf, char *buf, size_t count, loff_t *ppos);
	ssize_t (*write)(struct proc_file *pf, const char *buf, size_t count, loff_t *ppos);
};

struct proc_dir_entry {
	mode_t mode;
	int flags;
	const struct proc_file_operations *proc_fops;
	struct proc_file *pf;
	struct proc_dir_entry *next, *parent, *subdir;
	void *data;
	read_proc_t read_proc;
	write_proc_t write_proc;
	atomic_t count; /* open file count */
	spinlock_t pde_unload_lock;

	int namelen;
	struct proc_dir_entry *pdir_current;
	char name[MAX_NAMELEN];
};

struct proc_file {
	fmode_t f_mode;
	spinlock_t f_lock;
	atomic_t f_count;
	struct proc_dir_entry *pPDE;
	void *private_data;
	unsigned long long f_version;
	loff_t f_pos;
	char name[MAX_NAMELEN];
};

/**
 * Interface for modules using proc below internal proc moudule;
 */
/**
 * @ingroup  procfs
 * @brief create a proc node
 *
 * @par Description:
 * This API is used to create the node by 'name' and parent inode
 *
 * @attention
 * <ul>
 * <li>This interface should be called after system initialization.</li>
 * <li>The parameter name should be a valid string.</li>
 * </ul>
 *
 * @param  name    [IN] Type #const char * The name of the node to be created.
 * @param  mode    [IN] Type #mode_t the mode of create's node.
 * @param  parent  [IN] Type #struct proc_dir_entry * the parent node of the node to be created,
 * if pass NULL, default parent node is "/proc".
 *
 * @retval #NULL                Create failed.
 * @retval #proc_dir_entry*     Create successfully.
 * @par Dependency:
 * <ul><li>proc_fs.h: the header file that contains the API declaration.</li></ul>
 * @see
 *
 * @since Huawei LiteOS V100R001C00
 */
extern struct proc_dir_entry *create_proc_entry(const char *name, mode_t mode,
    struct proc_dir_entry *parent);

/**
 * @ingroup  procfs
 * @brief remove a proc node
 *
 * @par Description:
 * This API is used to remove the node by 'name' and parent inode
 *
 * @attention
 * <ul>
 * <li>This interface should be called after system initialization.</li>
 * <li>The parameter name should be a valid string.</li>
 * </ul>
 *
 * @param  name   [IN] Type #const char * The name of the node to be removed.
 * @param  parent [IN] Type #struct proc_dir_entry * the parent node of the node to be remove.
 *
 * @par Dependency:
 * <ul><li>proc_fs.h: the header file that contains the API declaration.</li></ul>
 * @see
 *
 * @since Huawei LiteOS V100R001C00
 */
extern void remove_proc_entry(const char *name, struct proc_dir_entry *parent);

/**
 * @ingroup  procfs
 * @brief create a proc directory node
 *
 * @par Description:
 * This API is used to create the directory node by 'name' and parent inode
 *
 * @attention
 * <ul>
 * <li>This interface should be called after system initialization.</li>
 * <li>The parameter name should be a valid string.</li>
 * </ul>
 *
 * @param  name   [IN] Type #const char * The name of the node directory to be created.
 * @param  parent [IN] Type #struct proc_dir_entry * the parent node of the directory node to be created,
 * if pass NULL, default parent node is "/proc".
 *
 * @retval #NULL               Create failed.
 * @retval #proc_dir_entry*    Create successfully.
 * @par Dependency:
 * <ul><li>proc_fs.h: the header file that contains the API declaration.</li></ul>
 * @see
 *
 * @since Huawei LiteOS V100R001C00
 */
extern struct proc_dir_entry *proc_mkdir(const char *name, struct proc_dir_entry *parent);

/**
 * @ingroup  procfs
 * @brief create a proc  node
 *
 * @par Description:
 * This API is used to create the node by 'name' and parent inode,
 * And assignment operation function
 *
 * @attention
 * <ul>
 * <li>This interface should be called after system initialization.</li>
 * <li>The parameter name should be a valid string.</li>
 * </ul>
 *
 * @param  name      [IN] Type #const char * The name of the node to be created.
 * @param  mode      [IN] Type #mode_t the mode of create's node.
 * @param  parent    [IN] Type #struct proc_dir_entry * the parent node of the node to be created.
 * @param  proc_fops [IN] Type #const struct proc_file_operations * operation function of the node.
 *
 * @retval #NULL               Create failed.
 * @retval #proc_dir_entry*    Create successfully.
 * @par Dependency:
 * <ul><li>proc_fs.h: the header file that contains the API declaration.</li></ul>
 * @see
 *
 * @since Huawei LiteOS V100R001C00
 */
extern struct proc_dir_entry *proc_create(const char *name, mode_t mode,
    struct proc_dir_entry *parent, const struct proc_file_operations *proc_fops);

/**
 * @ingroup  procfs
 * @brief init proc fs
 *
 * @par Description:
 * This API is used to init proc fs.
 *
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param  NONE
 *
 * @retval NONE
 * @par Dependency:
 * <ul><li>proc_fs.h: the header file that contains the API declaration.</li></ul>
 * @see proc_fs_init
 *
 * @since Huawei LiteOS V100R001C00
 */
extern void proc_fs_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOSCFG_FS_PROC */
#endif /* _PROC_FS_H */
