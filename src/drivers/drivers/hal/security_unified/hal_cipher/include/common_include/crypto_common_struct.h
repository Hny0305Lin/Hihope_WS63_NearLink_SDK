/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: crypto common struct header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef CRYPTO_COMMON_STRUCT_H
#define CRYPTO_COMMON_STRUCT_H

#include "crypto_type.h"

typedef enum {
    CRYPTO_BUF_SECURE,
    CRYPTO_BUF_NONSECURE,
} crypto_buffer_secure;

typedef struct {
    unsigned long long uapi_mem_handle;     /* Handle to buffer header address */
    unsigned long long addr_offset;         /* buffer offset, unused. */
    td_void *kapi_mem_handle;

    unsigned long phys_addr;
    void *virt_addr;
    crypto_buffer_secure buf_sec;           /* NONSECURE or SECURE */
} crypto_buf_attr;

typedef enum {
    CPU_ID_AIDSP = 0x00000035,
    CPU_ID_PCPU = 0x0000006a,
    CPU_ID_SCPU = 0x000000a5,
    CPU_ID_ACPU = 0x000000aa,
    CPU_ID_INVALID = 0xffffffff
} ca_misc_cpu_id;

#endif