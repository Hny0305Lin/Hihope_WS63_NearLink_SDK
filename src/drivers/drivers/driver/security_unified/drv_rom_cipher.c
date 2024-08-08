/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: security driver for romboot. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#include "drv_rom_cipher.h"
#include "hal_hash.h"
#include "hal_cipher_trng.h"
#include "hash_romable.h"

#include "drv_pke.h"
#include "drv_common.h"

#include "crypto_osal_adapt.h"
#include "crypto_curve_param.h"

static td_u32 g_hash_chn = 1;   /* use hash channle 1 in romboot default. */

int drv_rom_cipher_sha256(const unsigned char *data, unsigned int data_length, unsigned char *hash,
    unsigned int hash_length)
{
    int ret;

    hal_hash_lock(g_hash_chn);
    ret = drv_rom_hash(CRYPTO_HASH_TYPE_SHA256, data, data_length, hash, hash_length, g_hash_chn);
    hal_hash_unlock(g_hash_chn);

    return ret;
}

#if !defined(CONFIG_ROM_EXPEND_SUPPORT_NOT_ALIGNED)
static td_phys_addr_t inner_get_phys_addr(void *ptr)
{
    return (td_phys_addr_t)(uintptr_t)(ptr);
}

int drv_rom_cipher_init(void)
{
    crypto_drv_func drv_func_list = {0};

    /* hal trng init. */
    hal_cipher_trng_init();

    /* drv pke init. */
    drv_cipher_pke_init();

    hal_cipher_hash_init();

    drv_func_list.get_phys_addr = inner_get_phys_addr;
    (void)drv_cipher_register_func(&drv_func_list);
    crypto_curve_param_init();
    return 0;
}

int drv_rom_cipher_deinit(void)
{
    /* drv pke deinit. */
    drv_cipher_pke_deinit();

    /* hal trng deinit. */
    hal_cipher_trng_deinit();

    hal_cipher_hash_deinit();
    return 0;
}

int drv_rom_cipher_get_randnum(unsigned int *randnum)
{
    int ret = -1;

    ret = hal_cipher_trng_get_random(randnum);
    if (ret != TD_SUCCESS) {
        crypto_log_err("hal_cipher_trng_get_random failed, ret is 0x%x\n", ret);
    }

    return ret;
}

int drv_rom_cipher_pke_bp256r_verify(const drv_rom_cipher_ecc_point *pub_key, const drv_rom_cipher_data *hash,
    const drv_rom_cipher_ecc_sig *sig)
{
    int ret;

    ret = drv_cipher_pke_ecdsa_verify(
        DRV_PKE_ECC_TYPE_RFC5639_P256,
        (const drv_pke_ecc_point *)pub_key,
        (const drv_pke_data *)hash,
        (const drv_pke_ecc_sig *)sig
    );

    return ret;
}
#endif