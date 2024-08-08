/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * @brief    FOTA patch application library
 */

#ifndef UPG_PATCH_INFO_H
#define UPG_PATCH_INFO_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "upg_patch.h"
#include "LzmaDec.h"
#include "upg_common_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define upg_compile_assert(expr) typedef char upg_comp_assert[(expr) ? 1 : 0]

#define uapi_max(a, b)                           (((a) > (b)) ? (a) : (b))
#define uapi_min(a, b)                           (((a) < (b)) ? (a) : (b))

#define DECOMPRESSION_SIZE 1024

#define PAGE_WRITE_PARTIAL_START (1<<1)
#define PAGE_WRITE_STANDARD  (1<<3)
#define PAGE_WRITE_PARTIAL_END (1<<5)
#define PAGE_WRITE_SIGNATURE_MASK 0xC0  /* Top 2 bits used for signature handling 1100 0000 */
#define PAGE_WRITE_SIGNATURE_B    0x80  /* Signature page has been written to flash double buffer */
#define PAGE_WRITE_SIGNATURE_W    0x40  /* Signature page has been written to flash image */

#define HN_LZMA_SIZEOF_IMGSIZE 3

typedef struct fota_buffers {
    uint8_t page_buffer[UPG_FLASH_PAGE_SIZE];
    uint8_t page_status[FLASH_PAGES];
} fota_buffers_t;

typedef struct patch_state {
    /* patch description */
    patch     *desc;
    /* patch contents moved into RAM */
    /* Error code */
    errcode_t  err_code;
    /* patch operating state */
    ISzAlloc  lzma_alloc;
    uint8_t     *local_buffer;
    int32_t    local_buffer_page;
    int32_t    page_first_written;
    int32_t    page_last_written;
    bool   done_skipping;
} patch_state_t;

#pragma pack(push, 4)
typedef struct control_block {
    uint32_t magic;
    uint32_t copy;
    uint32_t extra;
    int32_t seek;
} control_block_t;  // or could use attribute__((__packed))  instead of this pragma pack stuff

#pragma pack(pop)
upg_compile_assert(sizeof(control_block_t) == 16);

typedef struct zip_context {
    /* The compression context */
    CLzmaDec *dec;
    /* an offset into the source (why the hell this isn't stored within the context is beyond me) */
    int32_t     offset;
    /* the compressed data */
    unsigned char *cdata;
    size_t unpacked_len;
    size_t unpacked_so_far;
    size_t cdata_len;
} zip_context_t;

/* Code style related process_patch(...) recovery state */
typedef struct process_patch_state {
    uint8_t *recovery_buffer;
    bool recovery_found;
} process_patch_state_t;

/* Code style related apply_patch(...) state */
typedef struct apply_patch_state {
    zip_context_t *zcontext;
    unsigned char *unpackedbuf;
    control_block_t cb;
    int32_t cerror;
    uint32_t lenread;
    int32_t oldpos;
    int32_t newpos;
    int32_t blockcount;
    int32_t chunk_start;
    int32_t chunk_size;
} apply_patch_state_t;

void write_image_block(patch_state_t *state, uint32_t size, int32_t location, const uint8_t *source);
void fota_patch_report_mem(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* UPG_PATCH_INFO_H */

