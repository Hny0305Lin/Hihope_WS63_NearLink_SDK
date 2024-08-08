/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: hash romable functions implement.
 *
 * Create: 2024-01-25
*/

#include "crypto_osal_adapt.h"

#include "hal_hash.h"
#include "hash_romable.h"

// SHA-256, the initial hash value
static const td_u8 g_sha256_ival[32] = {
    0x6A, 0x09, 0xE6, 0x67,
    0xBB, 0x67, 0xAE, 0x85,
    0x3C, 0x6E, 0xF3, 0x72,
    0xA5, 0x4F, 0xF5, 0x3A,
    0x51, 0x0E, 0x52, 0x7F,
    0x9B, 0x05, 0x68, 0x8C,
    0x1F, 0x83, 0xD9, 0xAB,
    0x5B, 0xE0, 0xCD, 0x19
};

// SM3, the initial hash value
static const td_u8 g_sm3_ival[32] = {
    0x73, 0x80, 0x16, 0x6F,
    0x49, 0x14, 0xB2, 0xB9,
    0x17, 0x24, 0x42, 0xD7,
    0xDA, 0x8A, 0x06, 0x00,
    0xA9, 0x6F, 0x30, 0xBC,
    0x16, 0x31, 0x38, 0xAA,
    0xE3, 0x8D, 0xEE, 0x4D,
    0xB0, 0xFB, 0x0E, 0x4E
};

#define MAX_HASH_RESULT_SIZE        64
#define HASH_BLOCK_SIZE             64
#define HASH_SIZE                   32 /* for SHA256/SM3. */
#define HASH_DATA_MAX_LEN           (16 * 1024 * 1024)    /* 16MB. */
#define HASH_PADDING_ALIGNED_LEN    56

int drv_rom_hash(crypto_hash_type hash_type, const unsigned char *data, unsigned int data_length,
    unsigned char *hash, unsigned int hash_length, unsigned int hash_chn)
{
    td_s32 ret = TD_SUCCESS;
    td_u8 state[MAX_HASH_RESULT_SIZE] = {0};
    td_u8 padding[HASH_BLOCK_SIZE * 2] = {0};   /* 2: two blocks for padding. */
    td_u32 padding_len = sizeof(padding);
    td_u32 processing_padding_len = HASH_BLOCK_SIZE;
    td_u32 length = 0;
    const td_u32 *init_val = (const td_u32 *)g_sha256_ival;
    td_u32 processing_length = 0;
    td_u32 tail_length = 0;

    crypto_chk_return(data == TD_NULL, TD_FAILURE, "data is NULL\n");
    crypto_chk_return(hash == TD_NULL, TD_FAILURE, "hash is NULL\n");
    crypto_chk_return(data_length >= HASH_DATA_MAX_LEN, TD_FAILURE, "data_length is too long\n");
    crypto_chk_return(hash_length != HASH_SIZE, TD_FAILURE, "hash_length should be 32 bytes\n");
    if (hash_type == CRYPTO_HASH_TYPE_SM3) {
        init_val = (const td_u32 *)g_sm3_ival;
    }

    ret = hal_cipher_hash_config(hash_chn, hash_type, init_val);
    crypto_chk_return(ret != EOK, ret, "hal_cipher_hash_config failed\n");

    if (data_length % HASH_BLOCK_SIZE == 0) {
        processing_length = data_length;
    } else {
        tail_length = data_length % HASH_BLOCK_SIZE;
        processing_length = data_length - tail_length;
    }

    /* process 64-byte data. */
    if (processing_length != 0) {
        ret = hal_cipher_hash_add_in_node(hash_chn, (uintptr_t)data, processing_length,
            IN_NODE_TYPE_FIRST | IN_NODE_TYPE_LAST);
        crypto_chk_return(ret != EOK, ret, "hal_cipher_hash_add_in_node failed\n");

        ret = hal_cipher_hash_start(hash_chn, TD_FALSE);
        crypto_chk_return(ret != EOK, ret, "hal_cipher_hash_start failed\n");

        ret = hal_cipher_hash_wait_done(hash_chn, TD_NULL, 0);
        crypto_chk_return(ret != EOK, ret, "hal_cipher_hash_wait_done failed\n");
    }

    /* Padding. */
    (void)memset_s(padding, padding_len, 0, padding_len);
    ret = memcpy_s(padding, padding_len, data + processing_length, tail_length);
    crypto_chk_return(ret != EOK, TD_FAILURE, "memcpy_s failed\n");

    padding[tail_length] = 0x80;    /* 0x80: the padding byte for hash alg. */
    if (tail_length >= HASH_PADDING_ALIGNED_LEN) {
        processing_padding_len = 2 * HASH_BLOCK_SIZE;   /* 2: two blocks for padding. */
    }

    length = crypto_cpu_to_be32(data_length * CRYPTO_BITS_IN_BYTE);
    ret = memcpy_s(padding + processing_padding_len - sizeof(length), sizeof(length), &length, sizeof(length));
    crypto_chk_return(ret != EOK, TD_FAILURE, "memcpy_s failed\n");

    /* process padding. */
    ret = hal_cipher_hash_add_in_node(hash_chn, (uintptr_t)padding, processing_padding_len,
        IN_NODE_TYPE_FIRST | IN_NODE_TYPE_LAST);
    crypto_chk_return(ret != EOK, ret, "hal_cipher_hash_add_in_node failed\n");

    ret = hal_cipher_hash_start(hash_chn, TD_FALSE);
    crypto_chk_return(ret != EOK, ret, "hal_cipher_hash_start failed\n");

    ret = hal_cipher_hash_wait_done(hash_chn, (td_u32 *)state, sizeof(state));
    crypto_chk_return(ret != EOK, ret, "hal_cipher_hash_wait_done failed\n");

    ret = memcpy_s(hash, hash_length, state, hash_length);
    crypto_chk_return(ret != EOK, TD_FAILURE, "memcpy_s failed\n");

    return ret;
}
