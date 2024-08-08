# Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.

## ATTENTION: api_inc.cmake is about external api header files' path. ##
## DON'T ADD OTHER VARS IN IT ##

if(LOSCFG_FS_VFS)
    set(LITEOS_VFS_INCLUDE
        ${LITEOSTOPDIR}/fs/include
        ${LITEOSTOPDIR}/${LITEOS_OPENSOURCE_PATHNAME}/incubator-nuttx/liteos/fs/include
        ${LITEOSTOPDIR}/fs/vfs/include/driver
        ${LITEOSTOPDIR}/${LITEOS_OPENSOURCE_PATHNAME}/incubator-nuttx/liteos/fs/vfs/include/driver
        ${LITEOSTOPDIR}/fs/vfs/include/operation
    )
    set(LITEOS_VFS_MTD_INCLUDE ${LITEOSTOPDIR}/fs/vfs/include/multi_partition)
    set(LITEOS_VFS_DISK_INCLUDE ${LITEOSTOPDIR}/fs/vfs/include/disk)
endif()

if(LOSCFG_FS_FAT)
    set(LITEOS_FAT_INCLUDE
        ${LITEOSTOPDIR}/fs/fat/include
    )
endif()

if(LOSCFG_FS_FAT_CACHE)
    set(LITEOS_FAT_CACHE_INCLUDE
        ${LITEOSTOPDIR}/fs/vfs/include/bcache
    )
endif()

if(LOSCFG_FS_PROC)
    set(LITEOS_PROC_INCLUDE
        ${LITEOSTOPDIR}/fs/proc/include
    )
endif()

set(LITEOS_FS_INCLUDE
    ${LITEOS_VFS_INCLUDE}
    ${LITEOS_FAT_CACHE_INCLUDE}
    ${LITEOS_VFS_MTD_INCLUDE}
    ${LITEOS_VFS_DISK_INCLUDE}
    ${LITEOS_PROC_INCLUDE}
    ${LITEOS_FAT_VIRPART_INCLUDE}
    ${LITEOS_FAT_INCLUDE}
)
