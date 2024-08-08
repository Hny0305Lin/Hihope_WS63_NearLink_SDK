/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: flashboot API header file.
 *
 * Create: 2023-05-26
*/

#ifndef DRV_FLASHBOOT_CIPHER_H
#define DRV_FLASHBOOT_CIPHER_H

#include <stdint.h>
#include <stdbool.h>
#include "drv_klad.h"
#include "drv_rom_cipher.h"

int drv_rom_cipher_config_odrk1(const crypto_klad_effective_key effective_key);

int drv_rom_cipher_config_clear_key(const crypto_klad_clear_key clear_key);

int drv_rom_cipher_fapc_config(unsigned int region, unsigned int start_addr, unsigned int end_addr,
    const unsigned char *iv, unsigned int iv_length);

int drv_rom_cipher_fapc_bypass_config(td_u32 region, td_u32 start_addr, td_u32 end_addr, td_bool lock);

/* support data_length not 64 bytes aligned. */
int drv_rom_cipher_sm3(const unsigned char *data, unsigned int data_length, unsigned char *hash,
    unsigned int hash_length);

int drv_rom_cipher_pke_sm2_verify(const drv_rom_cipher_ecc_point *pub_key, const drv_rom_cipher_data *hash,
    const drv_rom_cipher_ecc_sig *sig);

int32_t drv_rom_cipher_symc_init(void);

int32_t drv_rom_cipher_symc_deinit(void);

int drv_rom_cipher_create_keyslot(uint32_t *keyslot_chn_num, const crypto_klad_effective_key *effective_key,
    crypto_kdf_hard_key_type key_type);

int drv_rom_cipher_destroy_keyslot(uint32_t keyslot_chn_num);

int32_t drv_rom_cipher_symc_encrypt(uint32_t keyslot_chn_num, uint8_t *iv, uint32_t iv_length,
    uint64_t src, uint64_t dst, uint32_t length);

int32_t drv_rom_cipher_symc_decrypt(uint32_t keyslot_chn_num, uint8_t *iv, uint32_t iv_length,
    uint64_t src, uint64_t dst, uint32_t length);

#endif