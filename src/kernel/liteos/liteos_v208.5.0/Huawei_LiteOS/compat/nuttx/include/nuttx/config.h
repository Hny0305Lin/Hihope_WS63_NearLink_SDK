/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS compat headfile for nuttx config
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

#ifndef __INCLUDE_NUTTX_CONFIG_H
#define __INCLUDE_NUTTX_CONFIG_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "nuttx/net_config.h"
#include "nuttx/vfs_oflags.h"
#include "limits.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Do not modify the following macro definition switch. */
#ifdef LOSCFG_FS_VFS
#define CONFIG_VFS_64BIT_ACCESS /* To support access to files and devices larger than 2 ^ 32 byte size. */
#define CONFIG_VFS_FILE_FORCE_RELEASE
#define CONFIG_VFS_POSIX_OFLAGS /* LiteOS uses the POSIX standard oflags. */
#define CONFIG_VFS_BIND_WITH_PATH
#define CONFIG_VFS_UNBIND_WITHOUT_FLAGS
#define CONFIG_VFS_OPERATION_EXTEND
#define CONFIG_VFS_FILES_EXTEND_DISABLE

#define CONFIG_VFS_DIR_VERIFY
#define CONFIG_VFS_FULLPATH_ACQUIRE
#define CONFIG_VFS_DRIVER_PATH_VERIFY
#define CONFIG_VFS_DIR_POSITION_RESET
#define CONFIG_VFS_BLOCKPROXY_SEM

#define CONFIG_VFS_FILELIST_ALLOC
#define CONFIG_FILES_ALLOCATE_WITH_FILEP
#define CONFIG_VFS_MINFD_VERIFY
#define FILE_START_FD 3
#define CONFIG_VFS_SOCKET
#define CONFIG_VFS_CONSOLE
#define CONFIG_VFS_CLOSE_SOCKET
#define CONFIG_VFS_VFCNTL_SOCKET
#define CONFIG_VFS_IOCTL_EXTEND
#define CONFIG_VFS_READ_EXTEND
#define CONFIG_VFS_WRITE_EXTEND
#define CONFIG_VFS_INODE_FREE
#define CONFIG_VFS_INODE_ALLOC
#define CONFIG_VFS_INODE_PATH_VERIFY
#define CONFIG_VFS_FD_PRE_ALLOC
#define CONFIG_VFS_SUPPORT_ERROR_RETURN
#define CONFIG_VFS_SEEK_VERIFY
#define CONFIG_VFS_DISK_STATUS_VERIFY
#define CONFIG_VFS_SPECIAL_FD_VERIFY
#define CONFIG_VFS_SET_STATFSBUF_DISABLE
#define CONFIG_VFS_DISABLE_REMOVE_OLDINODE
#define CONFIG_VFS_ENABLE_SAME_DIR_RENAME
#define CONFIG_VFS_ENABLE_FILE_STRUCT_PATH
#define CONFIG_VFS_DISABLE_INODE_CHECKFLAGS
#define CONFIG_VFS_FILE_PRE_CLOSE
#ifdef LOSCFG_COMPAT_LINUX
#define CONFIG_VFS_POLL_WITHOUT_SETUP
#define CONFIG_VFS_OPTIMIZATION_ALLOC_POLLFD
#else
#define CONFIG_VFS_POLL_DISABLE
#endif
#define CONFIG_VFS_POLLSET_EVENT_EXTEND
#define CONFIG_VFS_UMOUNT_WITHOUT_FLAGS
#define CONFIG_VFS_CHANGE_REFERENCE_COUNT
#define CONFIG_VFS_INODE_UBIND
#define CONFIG_VFS_DESC_PATH_VERIFY
#define CONFIG_VFS_OFFSETOF_MARCO_DISABLE
#define CONFIG_VFS_DISABLE_INCLUDE_NUTTX_FS_H
#define CONFIG_VFS_SIMPLE_DEBUG
#define CONFIG_VFS_DIRENT_FS
#define CONFIG_VFS_RMDIR_RELPATH_VERIFY
#define CONFIG_VFS_TRUNCATE_LENGTH_VERIFY
#define CONFIG_VFS_WRITEV_SUPPORT
#define CONFIG_VFS_FILELIST_EXTERN
#define CONFIG_VFS_MOUNT_STATUS_ENABLE
#define CONFIG_VFS_READV_VERIFY
#define CONFIG_VFS_READV_ONCE
#define CONFIG_VFS_EXTEND_HEADER_FILE

#undef  OK
#define OK 0

/* POSIX-like OS return values: */
#ifndef VFS_ERROR
#define VFS_ERROR -1
#endif
#endif

#ifdef LOSCFG_FS_VFS_BLOCK_DEVICE
#define CONFIG_BCH_EXTEND_SIZE 16
#else
#define CONFIG_VFS_DISABLE_BLOCK_DEVICE
#endif

#ifdef LOSCFG_DRIVERS_VIDEO
#define CONFIG_FB_CMAP
#define CONFIG_FB_OVERLAY_BLIT
#define CONFIG_FB_OVERLAY
#define CONFIG_FB_TRANSPARENCY
#define CONFIG_FB_FUNCTIONS_EXTEND
#define CONFIG_FB_OPEN_ENABLE
#define CONFIG_FB_CLOSE_ENABLE
#define CONFIG_FB_COPY_WITHOUT_OFFSET
#define CONFIG_FB_IOCTL_DEFAULT_FUNC
#define CONFIG_FB_UNREGISTER_FUNC
#define CONFIG_FB_DISPLAY_CHECK
#define CONFIG_FB_GLOBAL_DEVICE
#define CONFIG_FB_PRIVATE_IOCTL
#define CONFIG_FB_VTABLE_EXTEND

#define FB_DEV_MAXNUM   32
#endif

#ifdef LOSCFG_FS_NFS
#define CONFIG_NFS_EXTEND
#define CONFIG_NFS_READDIR_HANEWIN_SERVER_SUPPORT
#define CONFIG_NFS_RPC_PATH_MAX
#define CONFIG_NFS_BIND_BAD_JUMP
#define CONFIG_NFS_UMOUNT_STATUS_ACQUIRE
#define CONFIG_NFS_SOCKADDR_USED
#define CONFIG_NFS_FWARN_USED
#define CONFIG_NFS_ENTRY3_USED

#define CONFIG_NFS_FILE_ATTR_UPDATE
#define CONFIG_NFS_ATTR_FIELD_PARSE
#define CONFIG_NFS_DIR_OPEN_CHECK
#define CONFIG_NFS_UDP_BIG_MSS
#define CONFIG_NFS_SEEK_ENABLE
#define CONFIG_NFS_RAND_XID
#define CONFIG_NFS_RPC_PATH_VERIFY
#define CONFIG_NFS_RPC_FHSIZIE_VERIFY
#define CONFIG_NFS_APPEND_OFLAGS_SUPPORT

#define CONFIG_NFS_PSOCK_INIT

#define CONFIG_NFS_TCP_ALIVE_CHECK
#define CONFIG_NFS_TCP_RECONNECT
#define CONFIG_NFS_TCP_PORT_BIND
#define CONFIG_NFS_TCP_RECV_RETRY

#define CONFIG_NFS_UNIX_CONFIG

#define CONFIG_NFS_PARAMS_DEFINED
#define CONFIG_NFS_UNSTABLE_WRITE /* Write to NFS server cache instead of disk */
#define CONFIG_NFS_TCP_TIMEOUT 50

#define NFS_TIMEO          10             /* 10 tick = 1 second */
#define NFS_MINTIMEO       10             /* Min timeout to use */
#define NFS_MAXTIMEO       255            /* Max timeout to backoff to */
#define NFS_MAXREXMIT      10             /* Stop counting after this many */
#define NFS_RETRANS        5              /* Num of retrans for soft mounts */
#define NFS_WSIZE          (8192 * 4)     /* Def. write data size <= 8192 * 4 */
#define NFS_RSIZE          (8192 * 4)     /* Def. read data size <= 8192 * 4 */
#define NFS_READDIRSIZE    1024           /* Def. readdir size */
#define DIRENT_NFS_MAXHANDLE    64        /* Maximum length of an NFSv3 file handle */
#define DIRENT_NFS_VERFLEN      8         /* Length of the copy verifier */
#endif

#ifdef LOSCFG_FS_ROMFS
#define CONFIG_FS_ROMFS
#define CONFIG_ROMFS_EXTEND_SIZE 8
#define CONFIG_ROMFS_DISABLE_IOCTL
#endif

#ifdef LOSCFG_FS_RAMFS
#define CONFIG_FS_TMPFS
#define TMPFS_MAGIC RAMFS_MAGIC
#define CONFIG_TMPFS_ENABLE_SYNC
#define CONFIG_TMPFS_DISABLE_IOCTL
#define CONFIG_TMPFS_DISABLE_DUP
#define CONFIG_TMPFS_TIME_EXTEND
#define CONFIG_TMPFS_NLINK_EXTEND
#define CONFIG_TMPFS_D_OFF_EXTEND
#define CONFIG_TMPFS_RETURN_VALUE_EXTEND
#define CONFIG_TMPFS_RENAME_DELETE_EXISTING_FILE
#define CONFIG_TMPFS_RENAME_CHECK_NAME_LEN_EXTEND
#define CONFIG_TMPFS_STRNDUP_NOT_SUPPORT
#define CONFIG_TMPFS_WRITE_UNIT_EXTEND
#define CONFIG_TMPFS_READ_VERIFY
#define CONFIG_TMPFS_LEN_VERIFY
#define CONFIG_TMPFS_PARAM_NEGATIVE_VERIFY
#define CONFIG_TMPFS_SEM_INIT_VERIFY
#define CONFIG_TMPFS_DYNAMIC_SEM_DESTROY
#define CONFIG_TMPFS_UNBIND_BUSY
#define CONFIG_TMPFS_STATIC_SUPER_BLOCK

#define CONFIG_FS_TMPFS_DIRECTORY_FREEGUARD	64
#define CONFIG_FS_TMPFS_DIRECTORY_ALLOCGUARD	48
#define CONFIG_FS_TMPFS_FILE_FREEGUARD  ~0UL
#define CONFIG_FS_TMPFS_FILE_ALLOCGUARD 48
#define CONFIG_FS_TMPFS_BLOCKSIZE   1
#endif

#define CONFIG_DISABLE_MQUEUE   // disable posix mqueue inode configure
#define CONFIG_DISABLE_ENVIRON  // disable obtain the current working path by environment variables
#define CONFIG_BCH
/* file system configur */

#define CONFIG_FS_WRITABLE      // enable file system can be written
#define CONFIG_FS_READABLE      // enable file system can be read
#define CONFIG_DEBUG_FS         // enable vfs debug function
#define CONFIG_DEBUG_FEATURES   // enable param check

/* pollfd count in stack, optimization in order to avoid small memory allocation */

#define POLL_STACK_CNT 5

/* nfs configure */

#define CONFIG_NFS_MACHINE_NAME "IPC"   // nfs device name is IPC
#define CONFIG_NFS_MACHINE_NAME_SIZE 3  // size of nfs machine name

/* file descriptors configure */

#define CONFIG_NFILE_STREAMS        1   // enable file stream
#define CONFIG_STDIO_BUFFER_SIZE    0
#define CONFIG_NUNGET_CHARS         0

#ifndef FD_SETSIZE
#define FD_SETSIZE  512  // total fds
#endif

#ifndef CONFIG_NFILE_DESCRIPTORS
#define CONFIG_NFILE_DESCRIPTORS    256
#endif
/* directory configure */

#define VFS_USING_WORKDIR               // enable current working directory

#ifdef LOSCFG_DRIVERS_USB_GADGET
#define CONFIG_USBDEV_COMPOSITE
#define CONFIG_USBDEV_DMA

#define CONFIG_COMPOSITE_VENDORID   0x03eb
#define CONFIG_COMPOSITE_PRODUCTID  0x2022
#define CONFIG_COMPOSITE_VENDORSTR  "NuttX"
#define CONFIG_COMPOSITE_PRODUCTSTR "Composite Device"

#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
#define CONFIG_USBDEV_EXTEND_USB3_DEVICE_CONTROLLER
#endif
#define CONFIG_USBDEV_EXTEND_EP
#define CONFIG_USBDEV_EXTEND_EP_FLUSH
#define CONFIG_USBDEV_ALT_ALLOCEP_BY_DESC
#define CONFIG_USBDEV_ALT_UNBIND_WITH_RET

#define CONFIG_COMPOSITE_DISABLE_CRITICAL
#define CONFIG_COMPOSITE_EXTEND_MKDEVDESC
#define CONFIG_COMPOSITE_EXTEND_CLASSUP
#define CONFIG_COMPOSITE_EXTEND_SPECIFIC_CTRLREQ_SIZE
#define CONFIG_COMPOSITE_EXTEND_SETUP
#define CONFIG_COMPOSITE_EXTEND_BOS_DESC
#define CONFIG_COMPOSITE_ALT_EPBUF_NONCACHE
#define CONFIG_COMPOSITE_ALT_SPEED_HIGH
#define CONFIG_COMPOSITE_ALT_REQ_TYPE_STADDARD_ALL
#define CONFIG_COMPOSITE_ALT_GET_USB_CLASS_DESC
#define CONFIG_COMPOSITE_ALT_SETCONFIG_SUBMIT
#define CONFIG_COMPOSITE_ALT_GETCONFIG_SUBMIT
#define CONFIG_COMPOSITE_ALT_RELEASE_ONCE
#endif /* LOSCFG_DRIVERS_USB_GADGET */

#if defined(LOSCFG_DRIVERS_USB_ETHERNET_GADGET) || defined(LOSCFG_DRIVERS_USB_ETH_SER_GADGET)
#define CONFIG_NET_GUARDSIZE       2
#define CONFIG_NET_ETH_PKTSIZE     1500
#define CONFIG_RNDIS_COMPOSITE
#define CONFIG_RNDIS_NWRREQS       0

#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
#define CONFIG_RNDIS_BULKIN_REQLEN 0x800
#else
#define CONFIG_RNDIS_BULKIN_REQLEN 0x600
#endif /* LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER */

#define CONFIG_RNDIS_ALT_ALLOC_DYNAMIC
#define CONFIG_RNDIS_ALT_LWIP
#define CONFIG_RNDIS_ALT_DESC
#define CONFIG_RNDIS_ALT_HANDLE_BY_CALLBACK
#define CONFIG_RNDIS_ALT_GET_CTRLREQ_BY_DEV
#define CONFIG_RNDIS_ALT_EP_CONFIG
#define CONFIG_RNDIS_ALT_SPEED_HIGH
#define CONFIG_RNDIS_ENBALE_DATA_CRITICAL
#define CONFIG_RNDIS_DISABLE_CTRL_CRITICAL
#define CONFIG_RNDIS_DISABLE_EP_CANCEL
#define CONFIG_RNDIS_EXTEND_INITIALIZE
#define CONFIG_RNDIS_EXTEND_UNINITIALIZE
#define CONFIG_RNDIS_EXTEND_BIND_DEV
#define CONFIG_RNDIS_EXTEND_RANDOM_MAC
#define CONFIG_RNDIS_EXTEND_SETINTERFACE
#endif /* LOSCFG_DRIVERS_USB_ETHERNET_GADGET || LOSCFG_DRIVERS_USB_ETH_SER_GADGET */

#ifdef CONFIG_DRIVERS_USB_DFU_GADGET
#define CONFIG_DFU_INTERFACE_NAME "DFU interface"
#define CONFIG_DFU_ALT_CONFIG_DESC
#define CONFIG_DFU_ALT_HANDLE_CLASS_REQ
#define CONFIG_DFU_ALT_GET_CTRLREQ_BY_DEV
#define CONFIG_DFU_EXTEND_SETCONFIG
#define CONFIG_DFU_EXTEND_INIT
#define CONFIG_DFU_EXTEND_UNBIND_CHECK
#endif /* CONFIG_DRIVERS_USB_DFU_GADGET */

#endif /* __INCLUDE_NUTTX_CONFIG_H */
