/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2021. All rights reserved.
 * @brief    FOTA patch application program
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "securec.h"
#include "common_def.h"
#include "upg_patch_info.h"
#include "upg_alloc.h"
#include "upg_debug.h"
#include "upg_porting.h"
#include "errcode.h"
#include "LzmaDec.h"
#include "upg_common_porting.h"
#include "upg_common.h"
#include "upg_config.h"

#include "upg_patch.h"

#define Z_OK                           SZ_OK
#define ZIP_STREAM_END                 99
#define WORD_WIDTH                     4

/* Code style related constants */
#define PATCH_KNVD_STR                 "KNVD"
#define PATCH_KNVD_STR_LEN             4
#define PAGE_STATUS_BIT_SEARCH_START   1
#define PAGE_STATUS_BIT_SEARCH_END     5
#define PAGE_STATUS_BIT_SEARCH_INC     2

STATIC void fota_patch_free(void *mem)
{
    if (mem == NULL) {
        return;
    }

    /* first words of the allocated memory is an int32_t indicating how much memory is allocated... */
    int32_t *q = ((int32_t *)mem) - 1;
    upg_free(q);
}

STATIC void* fota_patch_alloc(size_t size)
{
    volatile int32_t *p = NULL;
    uint32_t len = size + (uint32_t)sizeof(int32_t);
    p = (int32_t*)upg_malloc(len);
    if (p == NULL) {
        upg_log_err("[UPG] upg_malloc failure for requested size = 0x%x\r\n", (uint32_t)size);
        return NULL;
    }
    /* This will panic if it fails so no need to check the return value. */
    (void)memset_s((int32_t *)p, len, 0, len);

    *p = (int32_t)size;

    return (void*)(p + 1);
}

STATIC void* lzma_malloc(ISzAllocPtr p, size_t size)
{
    unused(p);
    return fota_patch_alloc(size);
}

STATIC void lzma_free(ISzAllocPtr p, void *address)
{
    unused(p);
    fota_patch_free(address);
}

STATIC errcode_t fota_pkg_flash_read_image(patch *desc, uint32_t length, int32_t location, uint8_t *dest)
{
    if (desc->use_plain_text_cache) {
        /* Read from the plain text cache. */
        uint8_t *src = NULL;

        if (desc->image_cache == NULL) {
            return ERRCODE_FAIL;
        }
        src = desc->image_cache + location;
        /* This will panic if it fails so no need to check the return value. */
        (void)memcpy_s(dest, length, src, length);
    } else {
        errcode_t ret = ERRCODE_SUCC;
        ret = upg_read_old_image_data((uint32_t)location, dest, &length, desc->image_id);
        if (ret != ERRCODE_SUCC) {
            upg_log_err("[UPG] fota_pkg_flash_read_image read flash error. ret = 0x%x\r\n", ret);
            return ret;
        }
    }
    return ERRCODE_SUCC;
}

STATIC errcode_t fota_pkg_flash_prep_page_contents_for_write(patch *desc, uint32_t image_page_no,
                                                             uint8_t *page_contents)
{
    errcode_t ret = ERRCODE_SUCC;
    uint8_t *dest = NULL;

    if (desc->use_plain_text_cache) {
        /* Update the plaintext cache. */
        if (desc->image_cache == NULL) {
            return ERRCODE_FAIL;
        }
        dest = desc->image_cache + image_page_no * UPG_FLASH_PAGE_SIZE;
        /* This will panic if it fails so no need to check the return value. */
        (void)memcpy_s(dest, UPG_FLASH_PAGE_SIZE, page_contents, UPG_FLASH_PAGE_SIZE);
    }

    return ret;
}

STATIC errcode_t fota_pkg_plaintext_flash_cache_init(patch *desc)
{
    errcode_t ret = ERRCODE_SUCC;

    if (!desc->use_plain_text_cache) {
        return ret;
    }

    /* 以下为加解密时使用 */
    uint32_t image_len = desc->image_flash_length;
    upg_log_info("[UPG] cache_init: flash_offset = 0x%x, len = 0x%x\r\n", desc->image_flash_offset, image_len);

    desc->image_cache = (uint8_t*)upg_malloc(image_len);
    upg_log_info("[UPG] cache_init: desc->image_cache = 0x%x\r\n", (uint32_t)(uintptr_t)desc->image_cache);
    if (desc->image_cache == NULL) {
        upg_log_err("[UPG] cache_init: malloc failure\r\n");
        return ERRCODE_MALLOC;
    }

    /* Cache the Plain Text region. */
    uint8_t *dst_addr = desc->image_cache;
    uint32_t src_offset = desc->image_flash_offset;
    ret = upg_read_old_image_data(src_offset, dst_addr, &image_len, desc->image_id);

    return ret;
}

STATIC bool read_diff_data_from_ram(const uint8_t *addr, uint8_t *data, uint32_t length)
{
    size_t copy_size = length;
    /* This will panic if it fails so no need to check the return value. */
    (void)memcpy_s(data, length, addr, copy_size);
    return true;
}

STATIC bool read_diff_data_to_ram(uint32_t offsets, uint8_t *data, uint32_t length, patch_state_t *state)
{
    uint32_t read_len = length;
    bool success = true;

    memset_s(data, DECOMPRESSION_SIZE, 0, DECOMPRESSION_SIZE);
    if (state->desc->patch_contents_ram_copy != 0) {
        success = read_diff_data_from_ram((uint8_t *)(uintptr_t)(state->desc->patch_contents_ram_copy + offsets),
                                          data, read_len);
    } else {
        if (upg_read_fota_pkg_data((uint32_t)state->desc->patch_contents_flash_offset + offsets,
                                   (uint8_t *)data, &read_len) != ERRCODE_SUCC) {
            success = false;
        }
    }
    if (!success) {
        upg_log_err("[UPG] read diff to ram error");
        return false;
    }

    return true;
}

STATIC errcode_t zip_init_helper(zip_context_t *z, patch_state_t *state)
{
    SRes sres;
    bool success;
    uint32_t i;
    uint8_t cdata[LZMA_PROPS_SIZE + HN_LZMA_SIZEOF_IMGSIZE];

    /* Configure compressed patch data pointer and copy routine */
    z->cdata = (unsigned char *)fota_patch_alloc(DECOMPRESSION_SIZE);
    if (z->cdata == NULL) {
        state->err_code = ERRCODE_MALLOC;
        upg_log_err("[UPG] z->cdata alloc failure, err_code = 0x%x\r\n", state->err_code);
        return state->err_code;
    }
    read_diff_data_to_ram(0, z->cdata, DECOMPRESSION_SIZE, state);

    /* Decode the properties from the header */
    success = read_diff_data_from_ram(z->cdata, cdata, LZMA_PROPS_SIZE + HN_LZMA_SIZEOF_IMGSIZE);
    if (!success) {
        state->err_code = ERRCODE_FAIL;
        fota_patch_free(z->cdata);
        upg_log_err("[UPG] zip_init_helper Decode the properties from the header false!");
        return state->err_code;
    }

    sres = LzmaDec_Allocate(z->dec, cdata, LZMA_PROPS_SIZE, &state->lzma_alloc);
    if (sres != SZ_OK) {
        state->err_code = ERRCODE_FAIL;
        fota_patch_free(z->cdata);
        upg_log_err("[UPG] zip_init_helper LzmaDec_Allocate is false!");
        return state->err_code;
    }
    LzmaDec_Init(z->dec);
    z->offset = LZMA_PROPS_SIZE;
    z->cdata_len = state->desc->patch_contents_len - (LZMA_PROPS_SIZE + HN_LZMA_SIZEOF_IMGSIZE);
    z->unpacked_len = 0;
    for (i = 0; i < HN_LZMA_SIZEOF_IMGSIZE; i++) {
        z->unpacked_len += ((uint32_t)(cdata[(uint32_t)z->offset + i])) << (i * NUM_BITS_PER_BYTE);
    }
    z->unpacked_so_far = 0;
    z->offset += HN_LZMA_SIZEOF_IMGSIZE;
    return ERRCODE_SUCC;
}

STATIC zip_context_t *zip_init(patch_state_t *state)
{
    errcode_t ret;
    zip_context_t *z = fota_patch_alloc(sizeof(zip_context_t));
    if (z == NULL) {
        state->err_code = ERRCODE_MALLOC;
        upg_log_err("[UPG] zip_init z alloc failure, err_code = 0x%x\r\n", state->err_code);
        return z;
    }

    z->dec = fota_patch_alloc(sizeof(CLzmaDec));
    if (z->dec == NULL) {
        state->err_code = ERRCODE_MALLOC;
        fota_patch_free(z);
        z = NULL;
        upg_log_err("[UPG] zip_init z->dec alloc failure, err_code = 0x%x\r\n", state->err_code);
        return z;
    }
    LzmaDec_Construct(z->dec);
    /* LZMA is absolute trash.  Required to prevent a segfault */
    z->dec->probs = NULL;

    ret = zip_init_helper(z, state);
    if (ret != ERRCODE_SUCC) {
        fota_patch_free(z->dec);
        fota_patch_free(z);
        z = NULL;
    }
    return z;
}

STATIC int32_t zip_mem_read(patch_state_t *state, int32_t *error, zip_context_t *z, unsigned char *dest, int32_t len)
{
    SizeT src_len, dest_len;
    dest_len = (SizeT)len;
    uint32_t done_len = 0;

    while (done_len != (uint32_t)len) {
        src_len = uapi_min(state->desc->patch_contents_len - (uint32_t)(z->offset), DECOMPRESSION_SIZE);
        if (!read_diff_data_to_ram((uint32_t)z->offset, (uint8_t *)z->cdata, (uint32_t)src_len, state)) {
            state->err_code = ERRCODE_UPG_FILE_READ_FAIL;
            upg_log_err("[UPG] LZMA zip_mem_read: read diff error, err_code = 0x%x\r\n", state->err_code);
            return 0;
        }

        ELzmaStatus status = 0;
        SRes ret;
        ret = LzmaDec_DecodeToBuf(z->dec, dest + done_len, &dest_len, z->cdata, &src_len, LZMA_FINISH_ANY, &status);
        if (ret != SZ_OK) {
            state->err_code = (errcode_t)ret;
            upg_log_err("[UPG] LZMA zip_mem_read: Decode error, err_code = 0x%x\r\n", state->err_code);
            return 0;
        }

        z->offset += (int32_t)src_len;
        *error = (status == LZMA_STATUS_NOT_FINISHED) ? Z_OK : ZIP_STREAM_END;
        z->unpacked_so_far += dest_len;
        done_len += dest_len;
        dest_len = (uint32_t)len - done_len;

        if (status == LZMA_STATUS_FINISHED_WITH_MARK) {
            break;
        }
    }

    return (int32_t)done_len;
}

STATIC void zip_end(patch_state_t *state, zip_context_t *z)
{
    LzmaDec_Free(z->dec, &state->lzma_alloc);
    fota_patch_free(z->cdata);
    fota_patch_free(z->dec);
    fota_patch_free(z);
}

STATIC void read_image_block(patch_state_t *state, uint32_t size, int32_t location, uint8_t *dest)
{
    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] read_image_block state err_code = 0x%x\r\n", state->err_code);
        /* Do not process anything if an error occurred */
        return;
    }

    /* Perform flash reads of the image using injected function to handle any decryption required. */
    state->err_code = fota_pkg_flash_read_image(state->desc, size, location, dest);
    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] ERROR read_image_block, err_code = 0x%x\r\n", state->err_code);
        return;
    }
}

void write_image_block(patch_state_t *state, uint32_t size, int32_t location, const uint8_t *source)
{
    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] write_image_block state err_code = 0x%x\r\n", state->err_code);
        /* Do not process anything if an error occurred */
        return;
    }

    if (upg_write_new_image_data((uint32_t)location, (uint8_t *)source, &size, state->desc->image_id, true) !=
        ERRCODE_SUCC) {
        state->err_code = ERRCODE_FAIL;
        upg_log_err("[UPG] write_image_block write err_code = 0x%x\r\n", state->err_code);
        return;
    }

    upg_calculate_and_notify_process((uint32_t)(state->page_last_written + 1));
}

/*
 * buffer file contains
 *  1.  a page of buffer_size
 *  2.  a set of bytes, one byte per page.
 * **N.B.** in a real, flash-based implementation, these bits would have
 * to be spaced out in accordance with the flash's access rules.
 */
STATIC bool fota_buffers_has_contents(patch_state_t *state)
{
    fota_buffers_t *buffer = NULL;
    uint32_t   i;
    bool  buffers_set = true;
    errcode_t ret;

    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] fota_buffers_has_contents state err_code = 0x%x\r\n", state->err_code);
        /* Do not process anything if an error occurred */
        return buffers_set;
    }
    buffer = fota_patch_alloc(sizeof(fota_buffers_t));
    if (buffer == NULL) {
        state->err_code = ERRCODE_MALLOC;
        upg_log_err("[UPG] fota_buffers_has_contents: cannot allocate read buffer, err_code = 0x%x\r\n",
            state->err_code);
        goto ret_free;
    }

    ret = upg_flash_read(state->desc->buffers_flash_offset, sizeof(fota_buffers_t), (uint8_t*)(uintptr_t)buffer);
    if (ret != ERRCODE_SUCC) {
        state->err_code = ret;
        upg_log_err("[UPG] fota_buffers_has_contents: cannot read buffers, err_code = 0x%x\r\n", state->err_code);
        goto ret_free;
    }

    for (i = 0; i < (sizeof(fota_buffers_t) / WORD_WIDTH); i++) {
        if (*((uint32_t *)(uintptr_t)buffer + i) != 0xffffffff) {
            /* The buffer was not erased */
            upg_log_err("[UPG] fota_buffers_has_contents: buffers are not erased  i = %d, val = 0x%x\r\n",
                i, *(((uint32_t*)(uintptr_t)buffer) + i));
            goto ret_free;
        }
    }
    /* The buffer has been erased */
    buffers_set = false;

ret_free:
    fota_patch_free(buffer);
    return buffers_set;
}

STATIC void read_page_buffer(patch_state_t *state, uint8_t *dest)
{
    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] read_page_buffer state err_code = 0x%x\r\n", state->err_code);
        /* Do not process anything if an error occurred */
        return;
    }

    if (upg_flash_read(state->desc->buffers_flash_offset, UPG_FLASH_PAGE_SIZE, dest) != ERRCODE_SUCC) {
        state->err_code = ERRCODE_FAIL;
        upg_log_err("[UPG] read_page_buffer: cannot read buffer, err_code = 0x%x\r\n", state->err_code);
        return;
    }
}

STATIC void replace_page_buffer(patch_state_t *state, uint8_t *source)
{
    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] replace_page_buffer state err_code = 0x%x\r\n", state->err_code);
        /* Do not process anything if an error occurred */
        return;
    }

    if (upg_flash_write(state->desc->buffers_flash_offset, UPG_FLASH_PAGE_SIZE, source, true) != ERRCODE_SUCC) {
        state->err_code = ERRCODE_FAIL;
        upg_log_err("[UPG] replace_page_buffer: cannot write buffer, err_code = 0x%x\r\n", state->err_code);
        return;
    }
}

STATIC uint8_t read_page_status(patch_state_t *state, int32_t page_no)
{
    uint32_t  val = 0;
    uint32_t  page_num = state->desc->image_flash_offset / UPG_FLASH_PAGE_SIZE + (uint32_t)page_no;
    uint32_t offset =
        state->desc->buffers_flash_offset + state->desc->buffers_length + ((uint32_t)sizeof(uint8_t) * page_num);

    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] read_page_status state err_code = 0x%x\r\n", state->err_code);
        /* Do not process anything if an error occurred */
        return 0x00;
    }

    if (upg_flash_read(offset, sizeof(uint8_t), (uint8_t *)&val) != ERRCODE_SUCC) {
        state->err_code = ERRCODE_FAIL;
        upg_log_err("[UPG] read_page_status: cannot read status, err_code = 0x%x\r\n", state->err_code);
        return 0x00;
    }
    return (uint8_t)val;
}

STATIC void write_page_status(patch_state_t *state, int32_t page_no, uint8_t val)
{
    uint32_t page_num = state->desc->image_flash_offset / UPG_FLASH_PAGE_SIZE + (uint32_t)page_no;
    uint32_t offset =
        state->desc->buffers_flash_offset + state->desc->buffers_length + ((uint32_t)sizeof(val) * page_num);
    uint32_t old = 0;

    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] write_page_status state err_code = 0x%x\r\n", state->err_code);
        /* Do not process anything if an error occurred */
        return;
    }

    if (upg_flash_read(offset, sizeof(val), (uint8_t *)&old) != ERRCODE_SUCC) {
        state->err_code = ERRCODE_FAIL;
        upg_log_err("[UPG] write_page_status: cannot read old status, err_code = 0x%x\r\n", state->err_code);
        return;
    }
    if ((val & ~((uint8_t)old)) != 0) {
        upg_log_info("[UPG] write_page_status: Trying to set bits 0->1 at offset = 0x%x, with 0x%x -> 0x%x\r\n",
            offset, (uint32_t)old, (uint32_t)val);
    }
    if (upg_flash_write(offset, sizeof(val), &val, false) != ERRCODE_SUCC) {
        state->err_code = ERRCODE_FAIL;
        upg_log_err("[UPG] write_page_status: cannot write status, err_code = 0x%x\r\n", state->err_code);
        return;
    }
}

STATIC void erase_fota_buffers(patch_state_t *state)
{
    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] erase_fota_buffers state err_code = 0x%x\r\n", state->err_code);
        /* Do not process anything if an error occurred */
        return;
    }

    if (upg_flash_erase(state->desc->buffers_flash_offset, state->desc->buffers_length) != ERRCODE_SUCC) {
        state->err_code = ERRCODE_FAIL;
        upg_log_err("[UPG] erase_fota_buffers: cannot erase the fota buffers, err_code = 0x%x\r\n",
            state->err_code);
        return;
    }
}

STATIC void erase_image_flash_page(patch_state_t *state, int32_t image_page)
{
    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] erase_image_flash_page state err_code = 0x%x\r\n", state->err_code);
        return;
    }

    errcode_t ret;
    ret = upg_flash_erase(state->desc->image_flash_offset + (image_page * UPG_FLASH_PAGE_SIZE), UPG_FLASH_PAGE_SIZE);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] read_byte state err_code = 0x%x\r\n", state->err_code);
        state->err_code = ERRCODE_FAIL;
        return;
    }
}

/*
 * In applying an in-place patch, the flash writes get applied a page at a time.
 * Sometimes the page gets written to more than once.  In fact, an individual page can get
 * written to up to 3 times.    (Ok, there may be several updates within a page.  But there
 * will only be a maximum of 3 writes.
 *
 * here's a page:
 *                    d  d  d  d  d  d  d  d  d  d  d  d  d  d  d  d  d  d  d

 * here's the 3 types of write operations, a b and c
 *                    a5 a4 a3 a2 a1 b1 b2 b3 b4 b5 b6 b7 b8 b9 c4 c3 c2 c2 c1
 *         (<--------) <----------a  b---->----->->---------->  <-----------c (<--------)
 *
 * so this function page_write_type() spots these different operations based on the first and
 * last value used.
 */
STATIC unsigned char page_write_type(const patch_state_t *state, int32_t first, int32_t last)
{
    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] page_write_type state err_code = 0x%x\r\n", state->err_code);
        /* Do not process anything if an error occurred */
        return 0xff;
    }

    const int32_t limit = UPG_FLASH_PAGE_SIZE - 1;
    if (((first == 0) && (last == limit)) || ((first == limit) && (last == 0))) {
        /* it's a full page. */
        upg_log_info("[UPG] STANDARD A\r\n");
        return PAGE_WRITE_STANDARD;
    }

    if ((first > 0) && (first < limit) && (last > 0) && (last < limit)) {
        /* it's a section from the middle */
        upg_log_info("[UPG] STANDARD B\r\n");
        return PAGE_WRITE_STANDARD;
    }

    if ((first == 0) || (last == 0)) {
        upg_log_info("[UPG] PARTIAL_START\r\n");
        return PAGE_WRITE_PARTIAL_START;
    }
    return PAGE_WRITE_PARTIAL_END;
}

STATIC void write_flash_page_skip_done(patch_state_t *state)
{
    if (state == NULL || state->err_code != ERRCODE_SUCC || state->desc == NULL) {
        upg_log_err("[UPG] write_flash_page_skip_done state error");
        /* Do not process anything if an error occurred */
        return;
    }

    if (state->local_buffer_page != -1) {
        const int32_t r = read_page_status(state, state->local_buffer_page);
        const int32_t t = page_write_type(state, state->page_first_written, state->page_last_written);
        if (((uint32_t)r & (uint32_t)t) == 0) {
            upg_calculate_and_notify_process(UPG_FLASH_PAGE_SIZE);

            upg_log_info("[UPG] Skipping! page = 0x%x\r\n", (uint32_t)state->local_buffer_page);
            upg_log_info("[UPG]   type = 0x%x permitted = 0x%x (first = 0x%x last = 0x%x)\r\n",
                (uint32_t)t, (uint32_t)r, (uint32_t)state->page_first_written, (uint32_t)state->page_last_written);
            if (state->done_skipping) {
                upg_log_err("[UPG] Corrupt flash! We have written valid pages, but now we're back to skipping!");
            }
        } else if (((uint32_t)r & ((uint32_t)t >> 1)) == 0) {
            /* So this means the W bit for the operation was set, but B was cleared.  We should never be */
            /* in this state; the recovery operation should mark this as completed. */
            upg_log_err("[UPG] Corrupt flash!Page write bits are not as expected.page = 0x%x, status = 0x%x\r\n",
                (uint32_t)state->local_buffer_page, (uint32_t)read_page_status(state, state->local_buffer_page));
        } else {
            const uint32_t unsigned_t = (uint32_t) t;
            const uint32_t unsigned_r = (uint32_t) r;
            const uint32_t val = unsigned_r & ~(unsigned_t>>1);
            /* Prepare the image page contents for writing performing any encryption necessary. */
            /* Any error returned put into the state err_code to be handled. */
            state->err_code = fota_pkg_flash_prep_page_contents_for_write(state->desc,
                                                                          (uint32_t)state->local_buffer_page,
                                                                          state->local_buffer);
            state->done_skipping = 1;
            replace_page_buffer(state, state->local_buffer);
            /* mark the bits to show the buffer is good.  (Clear B) */

            write_page_status(state, state->local_buffer_page, (uint8_t)val);

            /* write the whole UPG_FLASH_PAGE_SIZE block */
            write_image_block(state, UPG_FLASH_PAGE_SIZE,
                              state->local_buffer_page * UPG_FLASH_PAGE_SIZE, state->local_buffer);
            if (state->err_code == ERRCODE_SUCC) {
                /* mark the bits to show it's written.  (Clear W (and B)) */
                write_page_status(state, state->local_buffer_page,
                    (uint8_t)(unsigned_r & ~(unsigned_t | (unsigned_t >> 1))));
            }
        }
    }
}

/*
 * Note, in order to do the recovery thing, write_byte will SKIP those pages that
 * have already been written, in an 'restartable' manner
 * to permit recovery from an incomplete update operation.
 */
STATIC void write_byte(patch_state_t *state, int32_t dest_offset, unsigned char val)
{
    int32_t dest_offset_page;
    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] write_byte state err_code = 0x%x\r\n", state->err_code);
        /* Do not process anything if an error occurred */
        return;
    }

    dest_offset_page = dest_offset / UPG_FLASH_PAGE_SIZE;
    if (dest_offset_page != state->local_buffer_page) {
        /* write back the old page */
        write_flash_page_skip_done(state);
        /* check to see if we have a page to write back, AND if we have written this already. */
         /* now populate state->local_buffer with the next page's values. */
        read_image_block(state, UPG_FLASH_PAGE_SIZE, dest_offset_page * UPG_FLASH_PAGE_SIZE, state->local_buffer);
        state->local_buffer_page = dest_offset_page;
        state->page_first_written = dest_offset % UPG_FLASH_PAGE_SIZE;
    }
    state->local_buffer[dest_offset % UPG_FLASH_PAGE_SIZE] = val;
    state->page_last_written = dest_offset % UPG_FLASH_PAGE_SIZE;
}

STATIC unsigned char read_byte(patch_state_t *state, int32_t dest_offset)
{
    int32_t dest_offset_page;

    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] read_byte state err_code = 0x%x\r\n", state->err_code);
        /* Do not process anything if an error occurred */
        return 0xff;
    }

    dest_offset_page = dest_offset / UPG_FLASH_PAGE_SIZE;
    if (dest_offset_page == state->local_buffer_page) {
        return state->local_buffer[dest_offset % UPG_FLASH_PAGE_SIZE];
    } else {
        uint32_t b = 0;
        read_image_block(state, 1, dest_offset, (uint8_t *)&b);
        return (uint8_t)b;
    }
}


STATIC void finish_write(patch_state_t *state)
{
    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] finish_write state err_code = 0x%x\r\n", state->err_code);
        /* Do not process anything if an error occurred */
        return;
    }

    write_flash_page_skip_done(state);
    state->local_buffer_page = -1;
}

/**
 * @brief  Sanity check the diff after obtaining a control block.
 *
 * @param aps    apply_patch(...) function state.
 * @param state  fota patching state.
 * @return       true if processing should proceed with this control block or false if processing should stop.
 */
STATIC bool apply_patch_get_control_block_sanity_check(const apply_patch_state_t *aps, patch_state_t *state)
{
    if (aps->newpos + aps->cb.copy > (uint32_t)(state->desc->newsize)) {
        state->err_code = ERRCODE_UPG_CHECK_FOTA_ERROR;
        upg_log_err("[UPG] Corrupt patch 4, err_code = 0x%x\r\n", state->err_code);
        return false;
    }
    if (aps->oldpos + aps->cb.copy > (uint32_t)(state->desc->maxsize)) {
        state->err_code = ERRCODE_UPG_CHECK_FOTA_ERROR;
        upg_log_err("[UPG] Corrupt patch 4a, err_code = 0x%x\r\n", state->err_code);
        return false;
    }
#if (UPG_CFG_DEBUG_PRINT_ENABLED == YES)
    if (state->desc->bottom_up) {
        if (aps->newpos <= aps->oldpos) {
            upg_log_info("[UPG] NCkA\r\n");
        } else {
            upg_log_info("[UPG] RCkB\r\n");
        }
    } else {
        if (aps->newpos <= aps->oldpos) {
            upg_log_info("[UPG] NCkC\r\n");
        } else {
            upg_log_info("[UPG] RCkD\r\n");
        }
    }
#endif
    return true;
}

/**
 * @brief  Fetch the control block from the diff.
 *
 * @param aps    apply_patch(...) function state.
 * @param state  fota patching state.
 * @return       true if processing should proceed with this control block or false if processing should stop.
 */
STATIC bool apply_patch_get_control_block(apply_patch_state_t *aps, patch_state_t *state)
{
    /* Read control data */
    aps->lenread =
        (uint32_t)zip_mem_read(state, &(aps->cerror), aps->zcontext, (unsigned char *)&(aps->cb), sizeof(aps->cb));
    if ((aps->lenread < sizeof(aps->cb)) || ((aps->cerror != Z_OK) && (aps->cerror != ZIP_STREAM_END))) {
        state->err_code = ERRCODE_UPG_CHECK_FOTA_ERROR;
        upg_log_err("[UPG] Corrupt patch 3a (data exhausted), err_code = 0x%x\r\n", state->err_code);
        return false;
    }
    if (memcmp((unsigned char *)&(aps->cb), PATCH_KNVD_STR, PATCH_KNVD_STR_LEN) != 0) {
        state->err_code = ERRCODE_UPG_CHECK_FOTA_ERROR;
        upg_log_err("[UPG] Corrupt patch 3b (not magic number), err_code = 0x%x\r\n", state->err_code);
        return false;
    }
    aps->blockcount++;

    /* Sanity-check */
    return apply_patch_get_control_block_sanity_check(aps, state);
}

/**
 * @brief Apply a delta chunk for a bottom up patch
 *
 * @param aps    apply_patch(...) function state.
 * @param state  fota patching state.
 */
STATIC void apply_patch_delta_chunk_bottom_up(apply_patch_state_t *aps, patch_state_t *state)
{
    int32_t newsize = state->desc->newsize;
    int32_t chunk_offset;
    int32_t rpos;
    int32_t wpos;

    if (aps->newpos <= aps->oldpos) {
        for (chunk_offset = 0; (chunk_offset < aps->chunk_size) && (state->err_code == ERRCODE_SUCC);
             chunk_offset++) {
            rpos = newsize - 1 - (aps->oldpos + chunk_offset + aps->chunk_start);
            wpos = newsize - 1 - (aps->newpos + chunk_offset + aps->chunk_start);
            write_byte(state, wpos, read_byte(state, rpos) + aps->unpackedbuf[chunk_offset]);
        }
    } else {
        for (chunk_offset = 0; (chunk_offset < aps->chunk_size) && (state->err_code == ERRCODE_SUCC);
             chunk_offset++) {
            rpos = newsize - 1 - (aps->oldpos + ((int32_t)aps->cb.copy - 1 - (chunk_offset + aps->chunk_start)));
            wpos = newsize - 1 - (aps->newpos + ((int32_t)aps->cb.copy - 1 - (chunk_offset + aps->chunk_start)));
            write_byte(state, wpos, read_byte(state, rpos) + aps->unpackedbuf[chunk_offset]);
        }
    }
}

/**
 * @brief Apply a delta chunk for a top down patch
 *
 * @param aps    apply_patch(...) function state.
 * @param state  fota patching state.
 */
STATIC void apply_patch_delta_chunk_top_down(apply_patch_state_t *aps, patch_state_t *state)
{
    int32_t chunk_offset;
    int32_t rpos;
    int32_t wpos;

    if (aps->newpos <= aps->oldpos) {
        for (chunk_offset = 0; (chunk_offset < aps->chunk_size) && (state->err_code == ERRCODE_SUCC);
             chunk_offset++) {
            rpos = aps->oldpos + chunk_offset + aps->chunk_start;
            wpos = aps->newpos + chunk_offset + aps->chunk_start;
            write_byte(state, wpos, read_byte(state, rpos) + aps->unpackedbuf[chunk_offset]);
        }
    } else {
        for (chunk_offset = 0; (chunk_offset < aps->chunk_size) && (state->err_code == ERRCODE_SUCC);
             chunk_offset++) {
            rpos = aps->oldpos + ((int32_t)aps->cb.copy - 1 - (chunk_offset + aps->chunk_start));
            wpos = aps->newpos + ((int32_t)aps->cb.copy - 1 - (chunk_offset + aps->chunk_start));
            write_byte(state, wpos, read_byte(state, rpos) + aps->unpackedbuf[chunk_offset]);
        }
    }
}

/**
 * @brief  Process the code deltas within the diff for a section of the image.
 *
 * @param aps    apply_patch(...) function state.
 * @param state  fota patching state.
 * @return       true if processing should be continue onto the next chunk or false if processing should stop.
 */
STATIC bool apply_patch_delta_chunk(apply_patch_state_t *aps, patch_state_t *state, int32_t *cctotal)
{
    aps->chunk_size = uapi_min((int32_t)DECOMPRESSION_SIZE, (int32_t)aps->cb.copy - aps->chunk_start);
    *cctotal += aps->chunk_size;

    aps->unpackedbuf = fota_patch_alloc((uint32_t)aps->chunk_size);
    if (aps->unpackedbuf == NULL) {
        state->err_code = ERRCODE_MALLOC;
        upg_log_err("[UPG] malloc failure, err_code = 0x%x\r\n", state->err_code);
        /* Exit loop to immediately handle the error */
        return false;
    }

    aps->lenread = (uint32_t)zip_mem_read(state, (int32_t *)&(aps->cerror), aps->zcontext, aps->unpackedbuf,
                                          aps->chunk_size);
    if (((int32_t)aps->lenread < aps->chunk_size) ||
        ((aps->cerror != Z_OK) && (aps->cerror != ZIP_STREAM_END))) {
        state->err_code = ERRCODE_UPG_CHECK_FOTA_ERROR;
        fota_patch_free(aps->unpackedbuf);
        aps->unpackedbuf = NULL;
        upg_log_err("[UPG] Corrupt patch 5 (data exhausted), err_code = 0x%x\r\n", state->err_code);
        /* Exit loop to immediately handle the error */
        return false;
    }

    /* add diff data to previous data.   It's IN PLACE!!!!! */
    if (state->desc->bottom_up) {
        apply_patch_delta_chunk_bottom_up(aps, state);
    } else {
        apply_patch_delta_chunk_top_down(aps, state);
    }

    /* Free any previously allocated memory */
    fota_patch_free(aps->unpackedbuf);
    aps->unpackedbuf = NULL;
    upg_watchdog_kick();

    return true;
}


/**
 * @brief  Process the code deltas within the diff for a section of code.
 *
 * @param aps    apply_patch(...) function state.
 * @param state  fota patching state.
 * @return       true if processing should be continued or false if processing should stop.
 */
STATIC bool apply_patch_deltas(apply_patch_state_t *aps, patch_state_t *state)
{
    int32_t cctotal = 0;

    for (aps->chunk_start = 0; (aps->chunk_start < (int32_t)(aps->cb.copy)) && (state->err_code == ERRCODE_SUCC);
         aps->chunk_start += (int32_t)DECOMPRESSION_SIZE) {
        if (!apply_patch_delta_chunk(aps, state, &cctotal)) {
            break;
        }
    }
    /* Ensure that any allocated memory has been freed even if there has been an error */
    fota_patch_free(aps->unpackedbuf);
    aps->unpackedbuf = NULL;
    /* return if there has been an error */
    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] apply_patch b state err_code = 0x%x\r\n", state->err_code);
        return false;
    }
    /* Adjust pointers */
    aps->newpos += (int32_t)aps->cb.copy;
    aps->oldpos += (int32_t)aps->cb.copy;

    /* Sanity-check */
    if ((aps->newpos + (int32_t)aps->cb.extra) > state->desc->newsize) {
        state->err_code = ERRCODE_UPG_CHECK_FOTA_ERROR;
        upg_log_err("[UPG] Corrupt patch 6, err_code = 0x%x\r\n", state->err_code);
        return false;
    }

    return true;
}

/**
 * @brief Apply an extra chunk for a bottom up patch
 *
 * @param aps    apply_patch(...) function state.
 * @param state  fota patching state.
 */
STATIC void apply_patch_extra_chunk_bottom_up(apply_patch_state_t *aps, patch_state_t *state)
{
    int32_t newsize = state->desc->newsize;
    int32_t chunk_offset;
    int32_t wpos;

    for (chunk_offset = 0; (chunk_offset < aps->chunk_size) && (state->err_code == ERRCODE_SUCC);
         chunk_offset++) {
        wpos = newsize - 1 - (aps->newpos + chunk_offset + aps->chunk_start);
        write_byte(state, wpos, aps->unpackedbuf[chunk_offset]);
    }
}

/**
 * @brief Apply an extra chunk for a top down patch
 *
 * @param aps    apply_patch(...) function state.
 * @param state  fota patching state.
 */
STATIC void apply_patch_extra_chunk_top_down(apply_patch_state_t *aps, patch_state_t *state)
{
    int32_t chunk_offset;
    int32_t wpos;

    for (chunk_offset = 0; (chunk_offset < aps->chunk_size) && (state->err_code == ERRCODE_SUCC);
         chunk_offset++) {
        wpos = aps->newpos + chunk_offset + aps->chunk_start;
        write_byte(state, wpos, aps->unpackedbuf[chunk_offset]);
    }
}

/**
 * @brief  Process the code extras within the diff for a section of code.
 *
 * @param aps    apply_patch(...) function state.
 * @param state  fota patching state.
 * @return       true if processing should be continued or false if processing should stop.
 */
STATIC bool apply_patch_extra_chunk(apply_patch_state_t *aps, patch_state_t *state)
{
    aps->chunk_size = uapi_min((int32_t)DECOMPRESSION_SIZE, (int32_t)aps->cb.extra - aps->chunk_start);

    aps->unpackedbuf = fota_patch_alloc((uint32_t)aps->chunk_size);
    if (aps->unpackedbuf == NULL) {
        state->err_code = ERRCODE_MALLOC;
        upg_log_err("[UPG] malloc failure, err_code = 0x%x\r\n", state->err_code);
        return false;
    }

    aps->lenread = (uint32_t)zip_mem_read(state, &(aps->cerror), aps->zcontext, aps->unpackedbuf, aps->chunk_size);
    if ((int32_t)aps->lenread < aps->chunk_size) {
        state->err_code = ERRCODE_UPG_CHECK_FOTA_ERROR;
        fota_patch_free(aps->unpackedbuf);
        aps->unpackedbuf = NULL;
        upg_log_err("[UPG] Corrupt patch 7a (data exhausted), err_code = 0x%x\r\n", state->err_code);
        return false;
    }
    if (((aps->cerror != Z_OK) && (aps->cerror != ZIP_STREAM_END))) {
        state->err_code = ERRCODE_UPG_CHECK_FOTA_ERROR;
        upg_log_err("[UPG] Corrupt patch 7, returned(data corrupted) = 0x%x, err_code = 0x%x\r\n",
            (uint32_t)aps->cerror, state->err_code);
    }
    if (state->desc->bottom_up) {
        apply_patch_extra_chunk_bottom_up(aps, state);
    } else {
        apply_patch_extra_chunk_top_down(aps, state);
    }
    fota_patch_free(aps->unpackedbuf);
    aps->unpackedbuf = NULL;

    return true;
}

/**
 * @brief  Process the code extras within the diff for a section of code.
 *
 * @param aps    apply_patch(...) function state.
 * @param state  fota patching state.
 * @return       true if processing should be continued or false if processing should stop.
 */
STATIC bool apply_patch_extras(apply_patch_state_t *aps, patch_state_t *state)
{
    for (aps->chunk_start = 0; (aps->chunk_start < (int32_t)(aps->cb.extra)) && (state->err_code == ERRCODE_SUCC);
         aps->chunk_start += (int32_t)DECOMPRESSION_SIZE) {
        if (!apply_patch_extra_chunk(aps, state)) {
            break;
        }
    }
    /* Ensure that any allocated memory has been freed even if there has been an error */
    fota_patch_free(aps->unpackedbuf);
    aps->unpackedbuf = NULL;

    /* return if there has been an error */
    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] apply_patch c state err_code = 0x%x\r\n", state->err_code);
        return false;
    }
    /* Adjust pointers */
    aps->newpos += (int32_t)aps->cb.extra;
    aps->oldpos += aps->cb.seek;

    /* more sanity */
    if (aps->newpos > state->desc->newsize) {
        state->err_code = ERRCODE_UPG_CHECK_FOTA_ERROR;
        upg_log_err("[UPG] Corrupt patch 8 (overran destination), err_code = 0x%x\r\n", state->err_code);
        return false;
    }
    if (aps->oldpos >= (state->desc->maxsize)) {
        state->err_code = ERRCODE_UPG_CHECK_FOTA_ERROR;
        upg_log_err("[UPG] Corrupt patch 9 (overran source), err_code = 0x%x\r\n", state->err_code);
        return false;
    }

    return true;
}

STATIC void apply_patch(patch_state_t *state)
{
    apply_patch_state_t aps;

    if (state == NULL || state->err_code != ERRCODE_SUCC || state->desc == NULL) {
        upg_log_err("[UPG] apply_patch state error");
        /* Do not process anything if an error occurred */
        return;
    }

    aps.zcontext = zip_init(state);
    if ((aps.zcontext == NULL) || (state->err_code != ERRCODE_SUCC)) {
        upg_log_err("[UPG] aps.zcontext init state err_code = 0x%x\r\n", state->err_code);
        return;
    }
    aps.oldpos = 0;
    aps.newpos = 0;
    aps.blockcount = 0;
    aps.unpackedbuf = NULL;

    /* main patching loop */
    while ((aps.newpos < state->desc->newsize) && (state->err_code == ERRCODE_SUCC)) {
        /* Get the next control block */
        if (!apply_patch_get_control_block(&aps, state)) {
            break;
        }

        /* Apply diff deltas */
        if (!apply_patch_deltas(&aps, state)) {
            break;
        }

        /* Read extra string */
        if (!apply_patch_extras(&aps, state)) {
            break;
        }
    };
    /* complete the write */
    finish_write(state);
    /* Ensure that any allocated memory has been freed even if there has been an error */
    fota_patch_free(aps.unpackedbuf);
    if (aps.zcontext != NULL) {
        /* Clean up the compressed reads */
        zip_end(state, aps.zcontext);
    }
}

STATIC void erase_redundant_pages(patch_state_t *state)
{
    /* Erase any old image pages if new image is smaller */
    uint32_t num_old_pages = state->desc->num_old_pages;
    uint32_t num_new_pages = state->desc->num_new_pages;
    if (num_old_pages > num_new_pages) {
        for (uint32_t page = num_new_pages; page < num_old_pages; page++) {
            erase_image_flash_page(state, (int32_t)page);
        }
    }
}

STATIC void process_patch_init(patch_state_t *state, patch *desc, process_patch_state_t *pps)
{
    state->desc = desc;
    state->lzma_alloc.Alloc = lzma_malloc;
    state->lzma_alloc.Free = lzma_free;
    state->local_buffer = fota_patch_alloc(UPG_FLASH_PAGE_SIZE);
    if (state->local_buffer == NULL) {
        upg_log_err("[UPG] local_buffer malloc error");
        state->err_code = ERRCODE_MALLOC;
    } else {
        state->err_code = ERRCODE_SUCC;
    }
    state->local_buffer_page = -1;
    state->page_first_written = -1;
    state->page_last_written = -1;
    state->done_skipping = false;

    pps->recovery_buffer = NULL;
    pps->recovery_found = false;
#if (UPG_CFG_DEBUG_PRINT_ENABLED == YES)
    upg_log_info("[UPG] Starting patching process\r\n");
    upg_log_info("[UPG] desc->bottom_up = 0x%x\r\n", (uint32_t)desc->bottom_up);
    upg_log_info("[UPG] desc->buffers_flash_offset = 0x%x\r\n", desc->buffers_flash_offset);
    upg_log_info("[UPG] desc->buffers_length = 0x%x\r\n", desc->buffers_length);
    upg_log_info("[UPG] desc->failpoint = 0x%x\r\n", (uint32_t)desc->failpoint);
    upg_log_info("[UPG] desc->image_flash_offset = 0x%x\r\n", desc->image_flash_offset);
    upg_log_info("[UPG] desc->maxsize = 0x%x\r\n", (uint32_t)desc->maxsize);
    upg_log_info("[UPG] desc->newsize = 0x%x\r\n", (uint32_t)desc->newsize);
    upg_log_info("[UPG] desc->oldsize = 0x%x\r\n", (uint32_t)desc->oldsize);
    upg_log_info("[UPG] desc->patch_contents_flash_offset = 0x%x\r\n", desc->patch_contents_flash_offset);
    upg_log_info("[UPG] desc->patch_contents_len = 0x%x\r\n", desc->patch_contents_len);
#endif
}

STATIC void process_patch_recover_page(patch_state_t *state, process_patch_state_t *pps, uint32_t page_no,
                                       int32_t page_status, int32_t status_bit)
{
    if (pps->recovery_found) {
        state->err_code = ERRCODE_FAIL;
        upg_log_err("[UPG] Corrupt flash recovery - we can't have more than one page in progress!!!\r\n");
        upg_log_err("[UPG]     page = 0x%x, bit = 0x%x, err_code = 0x%x\r\n",
            page_no, (uint32_t)status_bit, state->err_code);
        return;
    }
    pps->recovery_buffer = fota_patch_alloc(UPG_FLASH_PAGE_SIZE);
    if (pps->recovery_buffer == NULL) {
        state->err_code = ERRCODE_MALLOC;
        upg_log_err("[UPG] err_code = 0x%x\r\n", state->err_code);
        return;
    }
    /* extract contents to recovery_buffer */
    read_page_buffer(state, pps->recovery_buffer);
    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] process_patch read_page_buffer err_code = 0x%x\r\n", state->err_code);
        fota_patch_free(pps->recovery_buffer);
        pps->recovery_buffer = NULL;
        return;
    }

    /* write page */
    write_image_block(state, UPG_FLASH_PAGE_SIZE, (int32_t)page_no * UPG_FLASH_PAGE_SIZE, pps->recovery_buffer);

    fota_patch_free(pps->recovery_buffer);
    pps->recovery_buffer = NULL;

    const uint32_t page_status_unsigned = (uint32_t) page_status;
    const uint32_t status_bit_unsigned  = (uint32_t) status_bit;

    /* it's the new page now, so clear down this operation. */
    write_page_status(state, (int32_t)page_no,
        (uint8_t)(page_status_unsigned & ~((1UL << status_bit_unsigned) | (1UL << (status_bit_unsigned - 1)))));
    pps->recovery_found = true;
}

STATIC void process_patch_check_page_buffer_recovery(patch_state_t *state, process_patch_state_t *pps)
{
    int32_t page_no;
    int32_t status_bit;

    /* pass through the bits, and if there's a buffer page written, with the corresponding page deleted,
     * then write the buffer to the duff page. */
    for (page_no = 0; (page_no < (int32_t)state->desc->num_maxsize_pages) && (state->err_code == ERRCODE_SUCC);
         page_no++) {
        const int32_t page_status = read_page_status(state, page_no);
        const uint32_t page_status_unsigned = (uint32_t)page_status;
        for (status_bit = PAGE_STATUS_BIT_SEARCH_START;
             (status_bit <= PAGE_STATUS_BIT_SEARCH_END) && (state->err_code == ERRCODE_SUCC);
             status_bit += PAGE_STATUS_BIT_SEARCH_INC) {
            const uint32_t status_bit_unsigned = (uint32_t)status_bit;
            if (((page_status_unsigned & (1UL << status_bit_unsigned)) != 0) &&
                ((page_status_unsigned & (1UL << (status_bit_unsigned - 1))) == 0)) {
                process_patch_recover_page(state, pps, (uint32_t)page_no, page_status, status_bit);
            }
        }
    }
}

STATIC void process_patch_perform_recovery(patch_state_t *state, process_patch_state_t *pps)
{
    /* Cache the recovery image in RAM if required, determine if old signature page is encrypted */
    state->err_code = fota_pkg_plaintext_flash_cache_init(state->desc);
    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] process_patch - caching recovery flash failure err_code = 0x%x\r\n", state->err_code);
        return;
    }

    /* Check if there is a page buffer written that needs to be written back to flash. */
    process_patch_check_page_buffer_recovery(state, pps);
    if (state->err_code != ERRCODE_SUCC) {
        upg_log_err("[UPG] process_patch state err_code = 0x%x\r\n", state->err_code);
        return;
    }
#if (UPG_CFG_DEBUG_PRINT_ENABLED == YES)
    if (!pps->recovery_found) {
        upg_log_info("[UPG] No buffer page required recovery.\r\n");
        upg_log_info("[UPG] (This implies that the failure must have occurred\r\n");
        upg_log_info("[UPG] while writing to the buffer, not while erasing or writing the target flash.\r\n");
    } else {
        upg_log_info("[UPG] Ready to continue patching\r\n");
    }
    upg_log_info("[UPG] Recovery complete\r\n");
#endif
}

errcode_t process_patch(patch *desc)
{
    patch_state_t state;
    process_patch_state_t pps;
    process_patch_init(&state, desc, &pps);
    if (state.err_code != ERRCODE_SUCC) {
        return state.err_code;
    }

    /* check if there is a patch in progress and continue it if so. */
    if (fota_buffers_has_contents(&state)) {
        process_patch_perform_recovery(&state, &pps);
        if (state.err_code != ERRCODE_SUCC) {
            goto ret_free;
        }
    } else {
        /* Cache non-recovery SEMAIN as required */
        state.err_code = fota_pkg_plaintext_flash_cache_init(desc);
        if (state.err_code != ERRCODE_SUCC) {
            upg_log_err("[UPG] process_patch - caching flash failure err_code = 0x%x\r\n", state.err_code);
            goto ret_free;
        }

        erase_fota_buffers(&state);
    }
    /* apply the zipped patch.  This will in turn use the write_byte_skip_done functions
     * which means an interrupted write should resume and (potentially) complete ok. */
    apply_patch(&state);

    erase_redundant_pages(&state);
    erase_fota_buffers(&state);

ret_free:
    fota_patch_free(state.local_buffer);
    fota_patch_free(pps.recovery_buffer);
    return state.err_code;
}

STATIC void fota_pkg_task_code_diff_cleanup_actions(patch *patch_desc)
{
    /* Clean allocated cache, Freeing a NULL is benign so no check for FOTA_PKG_TASK_ID_SEMAIN_CODE needed */
    upg_free(patch_desc->image_cache);
    patch_desc->image_cache = NULL;
}

STATIC void init_patch_description_with_task_info(const upg_image_header_t *image, patch *patch_desc)
{
    /* This will panic if it fails so no need to check the return value. */
    (void)memset_s(patch_desc, sizeof(patch), 0, sizeof(patch));

    patch_desc->image_id = image->image_id;
    patch_desc->maxsize = uapi_max((int32_t)image->new_image_len, (int32_t)image->old_image_len);
    patch_desc->newsize = (int32_t)image->new_image_len;
    patch_desc->oldsize = (int32_t)image->old_image_len;
    patch_desc->num_new_pages = (uint32_t)(patch_desc->newsize + (UPG_FLASH_PAGE_SIZE - 1)) / UPG_FLASH_PAGE_SIZE;
    patch_desc->num_old_pages = (uint32_t)(patch_desc->oldsize + (UPG_FLASH_PAGE_SIZE - 1)) / UPG_FLASH_PAGE_SIZE;
    patch_desc->num_maxsize_pages = uapi_max(patch_desc->num_new_pages, patch_desc->num_old_pages);
    patch_desc->new_sig_page = patch_desc->num_new_pages - 1;
    patch_desc->old_sig_page = patch_desc->num_old_pages - 1;

    patch_desc->patch_contents_ram_copy = 0;
    patch_desc->patch_contents_flash_offset = (uint32_t)image->image_offset;
    patch_desc->patch_contents_len = image->image_len;
    patch_desc->bottom_up = image->old_image_len < image->new_image_len;
    patch_desc->image_encrypted = false;
    /* No failure injection */
    patch_desc->failpoint = 0;
    patch_desc->failfn = NULL;
    /* Image read and prepare for writing functions. */
    patch_desc->fetch_image_contents_fn = fota_pkg_flash_read_image;
    patch_desc->prep_image_contents_for_write_fn = fota_pkg_flash_prep_page_contents_for_write;
    patch_desc->plaintext_flash_cache_init_fn = fota_pkg_plaintext_flash_cache_init;
    patch_desc->copy_recovered_buffer_to_flash_cache_fn = NULL;
    patch_desc->encrypt_flash_page_fn = NULL;
}

errcode_t fota_pkg_task_apply_code_diff(const upg_image_header_t *image)
{
    errcode_t ret;
    patch     *desc;
    uint32_t   fota_data_addr;
    uint32_t   fota_data_len;
    if (image == NULL) {
        upg_log_err("[UPG] task_apply_code_diff image is NULL");
        return ERRCODE_FAIL;
    }

    desc = fota_patch_alloc(sizeof(patch));
    if (desc == NULL) {
        upg_log_err("[UPG] patch *desc malloc failed");
        return ERRCODE_MALLOC;
    }
    init_patch_description_with_task_info(image, desc);

    ret = upg_get_partition_info(image->image_id, &desc->image_flash_offset, &desc->image_flash_length);
    if (ret != ERRCODE_SUCC) {
        fota_patch_free(desc);
        return ret;
    }

    /* Get the metadata buffer area */
    ret = upg_get_progress_status_start_addr(&fota_data_addr, &fota_data_len);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] fota_pkg_task_apply_code_diff get area addr faile, ret = 0x%x\r\n", ret);
        fota_patch_free(desc);
        return ret;
    }
    desc->buffers_flash_offset = fota_data_addr;
    desc->buffers_length = FOTA_DATA_BUFFER_AREA_LEN;

    /* Update patch description structure with task specific parameters and run through
     * any required initialisation functions done on a per task basis */
    if (ret == ERRCODE_SUCC) {
        /* Attempt to apply the patch */
        ret = process_patch(desc);
    }

    fota_pkg_task_code_diff_cleanup_actions(desc);
    fota_patch_free(desc);
    return ret;
}