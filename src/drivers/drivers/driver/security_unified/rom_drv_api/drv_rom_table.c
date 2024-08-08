/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Corresponds ROM functions to their implementation ROM addresses.
 *
 * Create: 2023-05-17
*/

#include "crypto_osal_adapt.h"
#include "hal_hash.h"
#include "hal_cipher_trng.h"
#include "drv_common.h"
#include "drv_pke.h"
#include "crypto_hash_struct.h"
#include "crypto_pke_struct.h"
#include "crypto_curve_param.h"
#include "drv_rom_cipher.h"
#include "security_rom_table.h"

/*
 * rom API
 */
int drv_rom_cipher_init(void)
{
    return ((int(*)(void))(DRV_ROM_CIPHER_INIT))();
}

#if !defined(CONFIG_ROM_EXPEND_SUPPORT_NOT_ALIGNED)
int drv_rom_cipher_sha256(const unsigned char *data, unsigned int data_length, unsigned char *hash,
    unsigned int hash_length)
{
    return ((int(*)(const unsigned char *data, unsigned int data_length, unsigned char *hash, unsigned int hash_length))
        (DRV_ROM_CIPHER_SHA256))(data, data_length, hash, hash_length);
}
#endif

int drv_rom_cipher_pke_bp256r_verify(const drv_rom_cipher_ecc_point *pub_key, const drv_rom_cipher_data *hash,
    const drv_rom_cipher_ecc_sig *sig)
{
    return ((int(*)(const drv_rom_cipher_ecc_point *pub_key, const drv_rom_cipher_data *hash,
        const drv_rom_cipher_ecc_sig *sig))(DRV_ROM_CIPHER_PKE_BP256R_VERIFY))(pub_key, hash, sig);
}
