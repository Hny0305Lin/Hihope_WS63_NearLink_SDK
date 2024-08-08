/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: KV Storage Library key access interface
 */

#ifndef NV_KEY_H
#define NV_KEY_H

#include "stdbool.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define KV_KEY_MAGIC      0xA9
#define KV_KEY_VALID      0xFF
#define KV_KEY_INVALID    0
#define KV_KEY_NO_MAGIC   0xFF

#define KV_KEY_TYPE_NORMAL     0xFF
#define KV_KEY_TYPE_PERMANENT  0

#define KV_KEY_FORCE_UPDATE    0xf0

#define KV_CRYPTO_HASH_SIZE     32
#define KV_CRYPTO_CRC_SIZE      4
#define KV_CRYPTO_CRC32_SIZE    32
#define AES_BLOCK_SIZE          16
#define NV_AES_GCM_TAG_LENGTH   16
#define MIN_AES_DECRYPT_LEN     (2 * AES_BLOCK_SIZE)
#define AES_KDFKEY_SDRK_TYPE    0x52
#define CRC_SWAP_SIZE8          8
#define CRC_SWAP_SIZE24         24
#define INVAILD_CRYPTO_HANDLE   0xFFFFFFFF

typedef enum {
    KV_KEY_FILTER_STATE_VALID    = 0x01,
    KV_KEY_FILTER_STATE_INVALID  = 0x02,
    KV_KEY_FILTER_STATE_ANY      = 0x03
} kv_key_filter_state_t;

typedef enum {
    KV_KEY_FILTER_TYPE_NORMAL    = 0x01,
    KV_KEY_FILTER_TYPE_PERMANENT = 0x02,
    KV_KEY_FILTER_TYPE_ANY       = 0x03
} kv_key_filter_type_t;

/**
 * KV key attribute flags
 */
typedef enum {
    KV_ATTRIBUTE_PERMANENT   = 0x1,    /* Key is permanent and can't be deleted or modified */
    KV_ATTRIBUTE_ENCRYPTED   = 0x2,    /* Key is encrypted in flash */
    KV_ATTRIBUTE_NON_UPGRADE = 0x4     /* key cannot be modified during the upgrade. */
} kv_attributes_t;

typedef enum {
    KV_KEY_NOT_VALIDATE,
    KV_KEY_VALIDATE_CORRECT,
    KV_KEY_VALIDATE_WRONG
} kv_key_validate_status_t;

typedef uint16_t kv_key_id;

/** Represents the location in flash of a KV key.  Provided in such a way that it can't be dereferenced,
 * as flash is not directly addressable */
typedef const void *kv_key_location;

/** Defines a set of conditions to match when searching through stores for keys */
typedef struct {
    kv_key_location      location;  /* (retained) location is out of context here */
    kv_key_id            pattern;   /* Pattern of bits matched against a key_id, after mask has been applied */
    kv_key_id            mask;      /* Mask is applied to a key id before matching pattern */
    kv_key_filter_state_t  state;     /* Include only keys of the specified state in search results */
    kv_key_filter_type_t   type;      /* Include only keys of the specified type in search results */
} kv_key_filter_t;

/** KV store key header */
typedef struct {
    uint8_t   magic;     /* Magic number to indicate the start of the item              */
    uint8_t   valid;     /* flag to indicate whether the value is valid.                */
    uint16_t  length;    /* Length of the key_data field in bytes                       */
    uint8_t   type;      /* Normal (0xFF) or permanent (0x00)                           */
    uint8_t   upgrade;   /* Indicating whether upgradeable(not.0x00)                    */
    uint16_t  key_id;    /* The Key ID                                                  */
    uint16_t  enc_key;   /* Allows some customisation of the data AES key used,         */
                       /* 0x0 - key_data is plaintext, Others - key_data is encrypted   */
    uint16_t  version;   /* Version of the key                                          */
    uint32_t  rnd;       /* align the header to be 16bytes, random number               */
} kv_key_header_t;

/** A KV key handle, which provides both key header information and the location of key data in flash */
typedef struct {
    kv_key_header_t    header;
    kv_key_location  key_location;   /* Location in flash of the KV key */
} kv_key_handle_t;

typedef struct {
    kv_key_id key_id;
    bool focre_write;
    const uint8_t *kvalue;
    uint32_t kvalue_length;
    kv_attributes_t attributes;
} kv_key_details_t;
typedef struct {
    uint32_t crypto_handle;
    uint8_t *key_data_chunk;
    uint8_t *compare_data_chunk;
} kv_helper_compare_key_data_info_t;

extern uint8_t g_nv_header_magic;

errcode_t kv_key_helper_copy_flash(uint32_t dest_location, uint32_t src_location, uint16_t length);
errcode_t kv_key_direct_write_flash(uint32_t dest, uint32_t length, const uint8_t *src);
errcode_t kv_key_direct_erase_flash(uint32_t dest, const uint32_t size);
errcode_t kv_key_write_flash(uint32_t dest, uint32_t length, const uint8_t *src);
errcode_t kv_key_erase_flash(uint32_t dest, const uint32_t size);
bool kv_key_is_erased(const kv_key_handle_t *key);
errcode_t kv_key_validate_integrity(kv_key_handle_t *key, bool ignor_invalid);
errcode_t kv_key_validation(kv_key_handle_t *key, bool ignor_invalid);
bool kv_key_is_valid(kv_key_handle_t *key);
errcode_t kv_key_does_filter_match(kv_key_handle_t *key, kv_key_filter_t *search_filter);
errcode_t kv_key_get_handle_from_location(kv_key_location key_location, kv_key_handle_t *key);
errcode_t kv_key_locations_in_same_page(kv_key_location first_key_location, kv_key_location second_key_location);
errcode_t kv_key_get_next_handle(kv_key_handle_t *key, kv_key_validate_status_t key_status);
kv_attributes_t kv_key_attributes(const kv_key_handle_t *key);
uint16_t   kv_key_padded_data_length(kv_attributes_t attributes, uint16_t unpadded_length);
uint16_t   kv_key_flash_size(kv_key_handle_t *key);
errcode_t kv_key_read_data(kv_key_handle_t *key, uint8_t *dest_location);
errcode_t kv_helper_compare_key_data(kv_key_handle_t *key, const uint8_t *compare_data, uint16_t compare_length);
void kv_key_build_from_new(kv_key_handle_t *key, const kv_key_details_t *new_key, kv_key_location key_location);
errcode_t kv_key_get_next_magic_position(kv_key_location key_location, kv_key_handle_t *key);

uint32_t kv_crc32_swap(uint32_t crc);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif  /* NV_KEY_H */
