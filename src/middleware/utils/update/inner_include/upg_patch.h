/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2021. All rights reserved.
 * @brief    FOTA patch application library public header
 */

#ifndef UPG_PATCH_H
#define UPG_PATCH_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "upg_common_porting.h"
#include "upg.h"
#include "upg_definitions.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DIFF_STATUS_FLASH_SIZE  0x800000
#define FLASH_PAGES             (DIFF_STATUS_FLASH_SIZE / UPG_FLASH_PAGE_SIZE)

#define NUM_BITS_PER_BYTE       8

typedef struct patch_description patch;

/**
 * @brief  The function to be called when a deliberate failure is injected.
 *
 * @param desc  The patch to be applied
 */
typedef void (*failfunc)(patch *desc);

/**
 * @brief  The function to be called to obtain firmware image contents.
 *
 * @param  desc      The patch description.
 * @param  size      The size of the contents.
 * @param  location  The byte position within the image to get the contents from.
 * @param  dest      The buffer to put the contents into.
 */
typedef errcode_t (*fetch_image_contents)(patch *desc, uint32_t size, int32_t location, uint8_t *dest);

/**
 * @brief  Update the image contents prior to writing to flash.
 *
 * @param  desc           The patch description.
 * @param  image_page_no  The page to be written.
 * @param  page_contents  The buffer containing the page contents that will be written.
 */
typedef errcode_t (*prep_image_contents_for_write)(patch *desc, uint32_t image_page_no, uint8_t *page_contents);

/**
* @brief    Creates a plaintext copy of the recovered 4K flash.
*             It assumes metadata pages are populated with the information about updated pages during FOTA
*             It assumes FOTA Section is populated with the right key areas.
* @param    desc           Patch description
* @param    flash_page     Number of flash pages from base
* @param    recover_buffer Buffer to decrypt and copy
* @return   ERRCODE_SUCC errcode_t or an error code.
*/
typedef errcode_t (*copy_recovered_buffer_to_flash_cache)(patch *desc, int32_t flash_page, uint8_t *recover_buffer);

/**
* @brief    Creates a plaintext version of the image flash in RAM
*             It assumes metadata pages are populated with the information about updated pages during FOTA
*             It assumes FOTA Section is populated with the right key areas.
* @param    desc                    Patch description
* @return   ERRCODE_SUCC errcode_t or an error code
*/
typedef errcode_t (*plaintext_flash_cache_init)(patch *desc);

/**
* Encrypt a UPG_FLASH_PAGE_SIZE RAM buffer and optionally specify a block of data at the start or at the end of the
* buffer that is to remain plaintext and not to be encrypted.  It can encrypt in place if required where the the output
* encrypted buffer is set to the same as the plaintext source buffer.
*
* @param aes_ctrl  aes control structure.
* @param src       UPG_FLASH_PAGE_SIZE RAM buffer where the plaintext version of the page is located.
* @param dst       UPG_FLASH_PAGE_SIZE RAM buffer where the encrypted version of the page will be produced.
* @param ptxt_len  Number of bytes in the plaintext page (#src) that is to remain plaintext and not to be encrypted.
*                  Defined either from the start (>0) or from the end (<0) of the buffer #src.
*                  Its valid range of values is:  -UPG_FLASH_PAGE_SIZE < ptxt_len < UPG_FLASH_PAGE_SIZE.
*                  If ptxt_len = 0 The whole page will be encrypted.
*                  If ptxt_len > 0 Everything except the first ptxt_len bytes will will be encrypted.
*                  If ptxt_len < 0 Everything except the last ptxt_len bytes will will be encrypted.
*
* @return ERRCODE_SUCC errcode_t or an error code.
*/
typedef errcode_t (*encrypt_one_flash_page)(upg_key_area_data_t *aes_ctrl,
                                            uint8_t *src, uint8_t *dst, int32_t ptxt_len);

struct patch_description {
    uint32_t   image_id;
    int32_t   maxsize;
    int32_t   newsize;
    int32_t   oldsize;
    uint32_t   num_new_pages;
    uint32_t   num_old_pages;
    uint32_t   num_maxsize_pages;
    uint32_t   new_sig_page;
    uint32_t   old_sig_page;

    /* If non zero, simulate a failure */
    int32_t   failpoint;
    failfunc failfn;
    uint32_t   patch_contents_ram_copy;
    uint32_t   patch_contents_flash_offset;
    uint32_t   patch_contents_len;
    bool  bottom_up;
    /* Memory layout info */
    uint32_t   image_flash_offset;
    uint32_t   image_flash_length;
    uint32_t   buffers_flash_offset;
    uint32_t   buffers_length;
    fetch_image_contents                 fetch_image_contents_fn;
    prep_image_contents_for_write        prep_image_contents_for_write_fn;
    copy_recovered_buffer_to_flash_cache copy_recovered_buffer_to_flash_cache_fn;
    plaintext_flash_cache_init           plaintext_flash_cache_init_fn;
    encrypt_one_flash_page               encrypt_flash_page_fn;
    bool  use_plain_text_cache;       /* Use a plaintext RAM cache of the flash image for patching, e.g SEMAIN */
    uint8_t    *image_cache;
    bool  image_encrypted;
    uint16_t   image_hdrs_len;        /* Length of images key area + code area hdr. */
    uint16_t   image_signature_size;  /* Length of image signature */
    upg_key_area_data_t key_area_aes_params;
};

/**
 * @brief  Process the patch provided.
 *
 * @param desc    The patch description to apply.
 * @return      errcode_t ERRCODE_SUCC or an error code.
 */
errcode_t process_patch(patch *desc);

/**
 * @brief  Apply a FOTA core task for an image.
 *
 * @param task  The FOTA code task header in SCPU RAM.
 * @return      errcode_t ERRCODE_SUCC or an error code.
 */
errcode_t fota_pkg_task_apply_code_diff(const upg_image_header_t *image);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* UPG_PATCH_H */
