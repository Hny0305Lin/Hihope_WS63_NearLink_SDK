/****************************************************************************
 * include/nuttx/fs/file.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __INCLUDE_FS_FILE_H
#define __INCLUDE_FS_FILE_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <compiler.h>

#include <sys/types.h>
#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/****************************************************************************
 * Global Function Prototypes
 ****************************************************************************/

/****************************************************************************
* Name: nxsched_get_files
*
* Description:
*   Get the file list.
*
****************************************************************************/

struct filelist *nxsched_get_files(void);

/****************************************************************************
 * Name: file_pread
 *
 * Description:
 *   Equivalent to the standard pread function except that is accepts a
 *   struct file instance instead of a file descriptor.  Currently used
 *   only by aio_read().
 *
 ****************************************************************************/

ssize_t file_pread(FAR struct file *filep, FAR void *buf, size_t nbytes,
                   off_t offset);

/****************************************************************************
 * Name: file_pwrite
 *
 * Description:
 *   Equivalent to the standard pwrite function except that is accepts a
 *   struct file instance instead of a file descriptor.  Currently used
 *   only by aio_write().
 *
 ****************************************************************************/

ssize_t file_pwrite(FAR struct file *filep, FAR const void *buf,
                    size_t nbytes, off_t offset);

/****************************************************************************
 * Name: file_seek64
 *
 * Description:
 *   Equivalent to the standard lseek64() function except that is accepts a
 *   struct file instance instead of a file descriptor.  Currently used
 *   only by net_sendfile()
 *
 ****************************************************************************/

off64_t file_seek64(FAR struct file *filep, off64_t offset, int whence);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __INCLUDE_FS_FILE_H */
