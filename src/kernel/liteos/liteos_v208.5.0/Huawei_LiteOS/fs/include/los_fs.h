/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LiteOS FS Headfile
 * Author: Huawei LiteOS Team
 * Create: 2021-07-16
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

#ifndef _INCLUDE_LOS_FS_H
#define _INCLUDE_LOS_FS_H

#include "errno.h"
#include "compiler.h"
#include "sys/statfs.h"
#include "sys/types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
#define _MAX_ENTRYLENGTH  16                                       /* MAX virtual partition name length */
#define _MAX_VIRVOLUMES   5                                        /* MAX virtual partition number */
typedef struct virtual_partition_info
{
  char *devpartpath;                                             /* partition name need set virtual partition,e.g. /dev/mmcblk0p0 */
  int  virpartnum;                                               /* virtual partition numbers, MAX number is 5 */
  double virpartpercent[_MAX_VIRVOLUMES];                        /* every virtual partition percent,e.g 0.6,0.3,0.1 */
  char virpartname[_MAX_VIRVOLUMES][_MAX_ENTRYLENGTH + 1];       /* every virtual partition name, MAX length is 16 */
} virpartinfo;
#endif

struct fsmap_t
{
  const char                      *fs_filesystemtype;
  const struct mountpt_operations *fs_mops;
  const BOOL                      is_mtd_support;
  const BOOL                      is_bdfs;
};

#define FSMAP_WOW_ENTRY(_l, _name, _mop, _is_mtd_support, _is_bdfs) \
struct fsmap_t _l LOS_HAL_TABLE_WOW_ENTRY(fsmap) =                  \
{                                                                   \
  _name,                                                            \
  &_mop,                                                            \
  _is_mtd_support,                                                  \
  _is_bdfs                                                          \
}

#define FSMAP_SCATTER_ENTRY(_l, _name, _mop, _is_mtd_support, _is_bdfs) \
struct fsmap_t _l LOS_HAL_TABLE_SCATTER_ENTRY(fsmap) =                  \
{                                                                       \
  _name,                                                                \
  &_mop,                                                                \
  _is_mtd_support,                                                      \
  _is_bdfs                                                              \
}

#define FSMAP_ENTRY(_l, _name, _mop, _is_mtd_support, _is_bdfs) \
struct fsmap_t _l LOS_HAL_TABLE_ENTRY(fsmap) =                  \
{                                                               \
  _name,                                                        \
  &_mop,                                                        \
  _is_mtd_support,                                              \
  _is_bdfs                                                      \
}

typedef enum mount_status
{
  STAT_UNMOUNTED = 0,
  STAT_MOUNTED,
} MOUNT_STATE;

/**
 * @ingroup  fs
 * @brief Initializes the vfs filesystem
 *
 * @par Description:
 * This API is used to initializes the vfs filesystem
 *
 * @attention
 * <ul>
 * <li>Called only once, multiple calls will cause file system error.</li>
 * </ul>
 *
 * @param none
 *
 * @retval none
 * @par Dependency:
 * <ul><li>fs.h: the header file that contains the API declaration.</li></ul>
 * @see NULL
 * @since Huawei LiteOS V100R001C00
 */

void los_vfs_init(void);

/**
 * @ingroup  fs
 * @brief   list directory contents.
 *
 * @par Description:
 * List information about the FILEs (the current directory by default).
 *
 * @attention
 * <ul>
 * <li>The total length of parameter pathname must be less than the value defined by PATH_MAX.</li>
 * </ul>
 *
 * @param pathname [IN]  Type #const char*  The file pathname.
 *
 * @retval
 * <ul>None.</ul>
 *
 * @par Dependency:
 * <ul><li>fs.h: the header file that contains the API declaration.</li></ul>
 * @see ls
 * @since Huawei LiteOS V100R001C00
 */

extern void ls(const char *pathname);

/**
 * @ingroup  fs
 * @brief    locate character in string.
 *
 * @par Description:
 * The API function returns a pointer to the last occurrence of the character c in the string s.
 *
 * @attention
 * <ul>
 * <li>The parameter s must point a valid string, which end with the terminating null byte.</li>
 * </ul>
 *
 * @param s [IN]  Type #const char*  A pointer to string.
 * @param c [IN]  Type #int          The character.
 *
 * @retval #char*        a pointer to the matched character or NULL if the character is not found.
 *
 * @par Dependency:
 * <ul><li>fs.h: the header file that contains the API declaration.</li></ul>
 * @see rindex
 * @since Huawei LiteOS V100R001C00
 */

extern char *rindex(const char *s, int c);

/**
 * @ingroup  fs
 * @brief   list directory contents.
 *
 * @par Description:
 * Get the volume label of the FAT partition.
 *
 * @attention
 * <ul>
 * <li>The function support FAT filesystem only.</li>
 * <li>The label must allocated more than 11 charactors space first</li>
 * </ul>
 *
 * @param target   [IN]   Type #const char*  The file pathname.
 * @param label    [OUT]  Type #const char*  The string pointer transform the label massge back.
 *
 * @retval #int  Point the status which is successed or failed.
 *
 * @par Dependency:
 * <ul><li>fs.h: the header file that contains the API declaration.</li></ul>
 */

int getlabel(const char *target, char *label);

/**
 * @ingroup fs
 *
 * @par Description:
 * The set_label() function shall set the value of a global variable,
 * the value will be used to set the label of SD card in format().
 *
 * @param name  [IN] label to set, the length must be less than 12
 *
 * @attention
 * <ul>
 * <li>The function must be called before format().</li>
 * </ul>
 *
 * @retval #void   None.
 *
 * @par Dependency:
 * <ul><li>fs.h</li></ul>
 * @see format
 * @since Huawei LiteOS V100R001C00
 */

extern void set_label(const char *name);

/**
 * @ingroup  fs
 * @brief formatting sd card
 *
 * @par Description:
 * formatting sd card.
 *
 * @attention
 * <ul>
 * <li>The prefix of the parameter dev must be "/dev", and the length must be less than the value defined by PATH_MAX.
 *     There are four kind of format option: FMT_FAT16, FMT_FAT32, FMT_ANY, FMT_ERASE. If users input anything else,
 *     the default format option is FMT_ANY. Format option is decided by the number of clusters. Choosing the wrong
 *     option will cause error of format. The detailed information of (FAT16,FAT32) is ff.h.
 * </li>
 * </ul>
 *
 * @param  dev          [IN] Type #const char*  path of the block device to format, which must be a really existing block device node.
 * @param  sectors      [IN] Type #int number of sectors per cluster.
 * @param  option       [IN] Type #int option of format.
 *
 * @retval #0      Format success.
 * @retval #-1     Format failed.
 *
 * @par Dependency:
 * <ul><li>unistd.h: the header file that contains the API declaration.</li></ul>
 * @see
 *
 * @since Huawei LiteOS V100R001C00
 */

extern int format(const char *dev, int sectors, int option);

/**
 * @ingroup  fs
 * @brief set current system time is valid or invalid for FAT file system.
 *
 * @par Description:
 * The function is used for setting current system time is valid or invalid for FAT file system.
 * The value can be set as FAT_SYSTEM_TIME_ENABLE/FAT_SYSTEM_TIME_DISABLE.
 *
 * @attention
 * <ul>
 * <li>When the system time is valid, it should set FAT_SYSTEM_TIME_ENABLE.</li>
 * <li>When the system time is invalid, it should set FAT_SYSTEM_TIME_DISABLE.</li>
 * </ul>
 *
 * @param  b_status             [IN] Type #BOOL    system time status.
 *
 * @retval #0      set status success
 * @retval #-22    Invalid argument
 *
 * @par Dependency:
 * <ul><li>fs.h: the header file that contains the API declaration.</li></ul>
 * @see
 *
 * @since Huawei LiteOS V200R001C00
 */

extern int los_set_systime_status(BOOL b_status);

/**
 * @ingroup  fs
 * @check the three latest files in path
 *
 * @par Description:
 * The fscheck() function check the latest three files in path and subdirectories.
 * The function will fix the FAT when the file size info of directory is not matched with FAT.
 *
 * @attention
 * <ul>
 * <li>This function only support for FAT32.</li>
 * </ul>
 *
 * @param  path     [IN] Type #const char *   The path of the directory to be checked.
 *
 * @retval #0      truncate success.
 * @retval #-1     truncate failed.
 *
 * @par Dependency:
 * <ul><li>fs.h: the header file that contains the API declaration.</li></ul>
 * @see
 *
 * @since Huawei LiteOS V200R001C00
 */

FAR int fscheck(FAR const char *path);

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
/**
 * @ingroup  fs
 * @get the virtual partitions' or free space information in virtual parition feature.
 *
 * @par Description:
 * The virstatfs() function returns the information about the a virtual partition or the free space
 * outside the virtual partition.
 *
 * @attention
 * <ul>
 * <li>This function only support for FAT32.</li>
 * <li>This function only support for the virtual partition feature.</li>
 * <li>The parameter 'buf' need to be allocate enough memory space outside the function first.</li>
 * </ul>
 *
 * @param  path     [IN]  Type #const char *   The path which virtual partition or free space to be checked.
 * @param  buf      [OUT] Type #struct statfs *   The statfs buffer saving the information.
 *
 * @retval #0      virstatfs success.
 * @retval #-1     virstatfs failed.
 *
 * @par Dependency:
 * <ul><li>fs.h: the header file that contains the API declaration.</li></ul>
 * @see
 *
 * @since Huawei LiteOS V200R002C00
 */

extern int virstatfs(FAR const char *path, FAR struct statfs *buf);

/**
 * @ingroup  fs
 * @set the virtual partition information.
 *
 * @par Description:
 * The los_set_virpartparam() function use for set virtual partition parameter.
 * The parameter include virtual partition number, virtual partition percent, virtual partition name
 * and the partition path which need mount virtual partition.
 *
 * @attention
 * <ul>
 * <li>This function only support for FAT32.</li>
 * <li>This function only support for the virtual partition feature.</li>
 * <li>This function only can be used before mount function.</li>
 * <li>The function can be invoked once before umount partition.</li>
 * <li>Now support set single partition,the partition information will be replaced if it used for set another partition name.</li>
 * <li>The function has no effert if virtual partition information is already in the partition.</li>
 * </ul>
 *
 * @param  virtualinfo  [IN] Type #virpartinfo   The struct which include virtual partition information.
 *
 * @retval #0      los_set_virpartparam success.
 * @retval #-1     los_set_virpartparam failed.
 *
 * @par Dependency:
 * <ul><li>fs.h: the header file that contains the API declaration.</li></ul>
 * @see
 *
 * @since Huawei LiteOS V200R002C00
 */

int los_set_virpartparam(virpartinfo virtualinfo);

#endif

/**
 * @ingroup fs
 *
 * @par Description:
 * The chattr() function shall change the mode of file named by the pathname pointed to by the path argument.
 *
 * @attention
 * <ul>
 * <li>Now only fat filesystem support this function.</li>
 * </ul>
 *
 * @retval #0  On success.
 * @retval #-1 On failure with errno set.
 *
 * @par Errors
 * <ul>
 * <li><b>EINVAL</b>: The path is a null pointer or points to an empty string.</li>
 * <li><b>ENAMETOOLONG</b>: The length of a component of a pathname is longer than {NAME_MAX}.</li>
 * <li><b>ENOENT</b>: A component of the path does not exist.</li>
 * <li><b>EPERM</b>: The entry represented by the path is a mount point.</li>
 * <li><b>ENOSYS</b>: The file system doesn't support this function.</li>
 * <li><b>EACCES</b>: It is a read-only file system.</li>
 * <li><b>ENOMEM</b>: Out of memory.</li>
 * <li><b>EIO</b>: A hard error occurred in the low level disk I/O layer or the physical drive cannot work.</li>
 * <li><b>ENODEV</b>: The device is not existed.</li>
 * </ul>
 *
 * @par Dependency:
 * <ul><li>fs.h</li></ul>
 * @see None
 * @since Huawei LiteOS V100R001C00
 */

int chattr(const char *path, mode_t mode);

/**
 * @ingroup fs
 *
 * @par Description:
 * The LOS_BcacheSyncByName() function shall sync all the data in the cache corresponding to the disk name to the disk.
 *
 * @param name  [IN] name of the disk
 *
 * @attention
 * <ul>
 * <li>Now only fat filesystem support this function.</li>
 * </ul>
 *
 * @retval #0      On success.
 * @retval #INT32  On failure.
 *
 * @par Dependency:
 * <ul><li>fs.h</li></ul>
 * @see None
 * @since Huawei LiteOS V200R002C10
 */

extern INT32 LOS_BcacheSyncByName(const CHAR *name);

/**
 * @ingroup fs
 *
 * @par Description:
 * The LOS_GetDirtyRatioByName() function shall return the percentage of dirty blocks in the cache corresponding
 * to the disk name.
 *
 * @param name  [IN] name of the disk
 *
 * @attention
 * <ul>
 * <li>Now only fat filesystem support this function.</li>
 * </ul>
 *
 * @retval #INT32  the percentage of dirty blocks.
 * @retval #-1     On failure.
 *
 * @par Dependency:
 * <ul><li>fs.h</li></ul>
 * @see None
 * @since Huawei LiteOS V200R002C10
 */

extern INT32 LOS_GetDirtyRatioByName(const CHAR *name);

#ifdef LOSCFG_FS_FAT_CACHE_SYNC_THREAD
/**
 * @ingroup fs
 *
 * @par Description:
 * The LOS_SetDirtyRatioThreshold() function shall set the dirty ratio threshold of bcache. When the percentage
 * of dirty blocks in the cache is greater than the threshold, write back data to disk.
 *
 * @param dirtyRatio  [IN] Threshold of the percentage of dirty blocks, expressed in %.
 *
 * @attention
 * <ul>
 * <li>The dirtyRatio must be less than or equal to 100, or the setting is invalid.</li>
 * </ul>
 *
 * @retval #VOID  None.
 *
 * @par Dependency:
 * <ul><li>fs.h</li></ul>
 * @see LOS_SetSyncThreadInterval | LOS_SetSyncThreadPrio
 * @since Huawei LiteOS V200R002C10
 */

extern VOID LOS_SetDirtyRatioThreshold(UINT32 dirtyRatio);

/**
 * @ingroup fs
 * @brief Obtain the dirty ratio threshold of bcache.
 *
 * @par Description:
 * This API is used to obtain the dirty ratio threshold of bcache.
 *
 * @attention None.
 *
 * @retval #UINT32  Dirty ratio.
 *
 * @par Dependency:
 * <ul><li>fs.h</li></ul>
 * @see LOS_SetDirtyRatioThreshold
 * @since Huawei LiteOS 206.1.0
 */
extern UINT32 LOS_GetDirtyRatioThreshold(VOID);

/**
 * @ingroup fs
 *
 * @par Description:
 * The LOS_SetSyncThreadInterval() function shall set the interval for the sync thread to wake up.
 *
 * @param interval [IN] the interval time for the sync thread to wake up, in milliseconds, accuracy is 10ms.
 *
 * @attention
 * <ul>
 * <li>None</li>
 * </ul>
 *
 * @retval #VOID  None.
 *
 * @par Dependency:
 * <ul><li>fs.h</li></ul>
 * @see LOS_SetDirtyRatioThreshold | LOS_SetSyncThreadPrio
 * @since Huawei LiteOS V200R002C10
 */

extern VOID LOS_SetSyncThreadInterval(UINT32 interval);

/**
 * @ingroup fs
 * @brief Obtain the interval for the sync thread to wake up.
 *
 * @par Description:
 * This API is used to obtain the interval for the sync thread to wake up.
 *
 * @attention None.
 *
 * @retval #UINT32  The interval time for the sync thread to wake up, in milliseconds.
 *
 * @par Dependency:
 * <ul><li>fs.h</li></ul>
 * @see LOS_SetSyncThreadInterval
 * @since Huawei LiteOS 206.1.0
 */
extern UINT32 LOS_GetSyncThreadInterval(VOID);

/**
 * @ingroup fs
 *
 * @par Description:
 * The LOS_SetSyncThreadPrio() function shall set the priority of the sync thread.
 *
 * @param prio  [IN] priority of sync thread to be set
 * @param name  [IN] name of the disk
 *
 * @attention
 * <ul>
 * <li>The prio must be less than 31 and be greater than 0, or the setting is invalid.</li>
 * <li>If the parameter name is NULL, it only set the value of a global variable, and take effect the next time the
 * thread is created. If name is not NULL and can't find the disk corresponding to name, it shall return an error.</li>
 * </ul>
 *
 * @retval #INT32  On failure.
 * @retval 0       On success.
 *
 * @par Dependency:
 * <ul><li>fs.h</li></ul>
 * @see LOS_SetDirtyRatioThreshold | LOS_SetSyncThreadInterval | LOS_TaskPriSet
 * @since Huawei LiteOS V200R002C10
 */

extern INT32 LOS_SetSyncThreadPrio(UINT32 prio, const CHAR *name);

#endif

/**
 * @ingroup fs
 *
 * @par Description:
 * The LOS_SetBlockExpireInterval() function shall set the max expire interval for block.
 * If the expire interval of blocks is reached, the current block will be synchronized
 *
 * @param interval [IN] the expire interval for the block
 *
 * @attention
 * <ul>
 * <li>None</li>
 * </ul>
 *
 * @retval #VOID  None.
 *
 * @par Dependency:
 * <ul><li>fs.h</li></ul>
 * @see LOS_SetBlockExpireInterval
 * @since Huawei LiteOS V200R006C00
 */

extern VOID LOS_SetBlockExpireInterval(UINT32 interval);

/**
 * @ingroup fs
 * @brief Obtain the max expire interval for block.
 *
 * @par Description:
 * This API is used to obtain the max expire interval for block.
 *
 * @attention None.
 *
 * @retval #UINT32  The expire interval for the block, in milliseconds.
 *
 * @par Dependency:
 * <ul><li>fs.h</li></ul>
 * @see LOS_SetBlockExpireInterval
 * @since Huawei LiteOS 206.1.0
 */
extern UINT32 LOS_GetBlockExpireInterval(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _INCLUDE_LOS_FS_H */
