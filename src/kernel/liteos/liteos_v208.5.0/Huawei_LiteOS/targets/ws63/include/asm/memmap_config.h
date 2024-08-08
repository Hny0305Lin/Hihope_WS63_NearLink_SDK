/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: Memory Map Config HeadFile
 * Author: Huawei LiteOS Team
 * 2023-05-25, Create file. \n
 */

#ifndef _MEMMAP_CONFIG_H
#define _MEMMAP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern void *g_intheap_begin;
extern void *g_intheap_size;

#define OS_SYS_MEM_ADDR         (&g_intheap_begin)
#define OS_SYS_MEM_SIZE         (unsigned int)(&g_intheap_size)


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MEMMAP_CONFIG_H */
