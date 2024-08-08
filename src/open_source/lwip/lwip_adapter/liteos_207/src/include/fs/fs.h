/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: filesystem(fs) adapter.
 */

#ifndef FS_H
#define FS_H

#if LWIP_LITEOS_COMPAT

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

struct inode;

#ifndef FAR
#define FAR
#endif

/* file，待适配 */
struct file {
    int                f_oflags;    /* Open mode flags */
    FAR struct inode   *f_inode;    /* Driver or file system interface */
    long               f_pos;       /* File position */
    char               *f_path;     /* File fullpath */
    void               *f_priv;     /* Per file driver private data */
    const char         *f_relpath;  /* realpath */
};

/* 解决 iperf 编译报错 */
#if !LWIP_CMSIS_SUPPORT
#ifndef UNUSED
#define UNUSED(a) ((void)(a))
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* LWIP_LITEOS_COMPAT */

#endif /* FS_H */