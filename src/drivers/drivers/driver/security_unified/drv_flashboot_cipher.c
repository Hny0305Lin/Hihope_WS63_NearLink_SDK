/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: flashboot API functions implemention.
 *
 * Create: 2023-05-24
*/

#include "drv_flashboot_cipher.h"
#include "hal_hash.h"
#include "hash_romable.h"

#include "drv_pke.h"
#include "drv_common.h"

#include "drv_keyslot.h"
#include "drv_klad.h"
#include "drv_fapc.h"

#include "crypto_osal_adapt.h"
#include "crypto_common_macro.h"

static td_u32 g_hash_chn = 1;   /* use hash channle 1 in boot default. */

int drv_rom_cipher_create_keyslot(uint32_t *keyslot_chn_num, const crypto_klad_effective_key *effective_key,
    crypto_kdf_hard_key_type key_type)
{
    int32_t ret;
    td_handle keyslot_handle;
    td_handle klad_handle;
    crypto_klad_attr klad_attr = {
        .klad_cfg.rootkey_type = key_type,
        .key_cfg = {
            .engine = CRYPTO_KLAD_ENGINE_AES,
            .decrypt_support = TD_TRUE,
            .encrypt_support = TD_TRUE
        },
        .key_sec_cfg = {
            .key_sec = TD_FALSE,
            .master_only_enable = TD_FALSE,
            .dest_buf_sec_support = TD_FALSE,
            .dest_buf_non_sec_support = TD_TRUE,
            .src_buf_sec_support = TD_FALSE,
            .src_buf_non_sec_support = TD_TRUE,
        }
    };

    ret = drv_keyslot_create(&keyslot_handle, CRYPTO_KEYSLOT_TYPE_MCIPHER);
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_keyslot_create failed\n");

    ret = drv_klad_create(&klad_handle);
    crypto_chk_goto(ret != TD_SUCCESS, error_keyslot_destroy, "drv_klad_create failed\n");

    ret = drv_klad_attach(klad_handle, CRYPTO_KLAD_DEST_MCIPHER, keyslot_handle);
    crypto_chk_goto(ret != TD_SUCCESS, exit_klad_destroy, "drv_klad_attach failed\n");

    ret = drv_klad_set_attr(klad_handle, &klad_attr);
    crypto_chk_goto(ret != TD_SUCCESS, exit_klad_detach, "drv_klad_set_attr failed\n");

    ret = drv_klad_set_effective_key(klad_handle, effective_key);
    crypto_chk_goto(ret != TD_SUCCESS, exit_klad_detach, "drv_klad_set_effective_key failed\n");

    *keyslot_chn_num = (uint32_t)keyslot_handle;
exit_klad_detach:
    drv_klad_detach(klad_handle, CRYPTO_KLAD_DEST_MCIPHER, keyslot_handle);
exit_klad_destroy:
    drv_klad_destroy(klad_handle);
    return ret;
error_keyslot_destroy:
    drv_keyslot_destroy(*keyslot_chn_num);
    return ret;
}

int drv_rom_cipher_destroy_keyslot(uint32_t keyslot_chn_num)
{
    return drv_keyslot_destroy((td_handle)keyslot_chn_num);
}

int drv_rom_cipher_config_odrk1(const crypto_klad_effective_key effective_key)
{
    td_s32 ret;
    td_handle klad_handle;
    crypto_klad_attr klad_attr = {
        .klad_cfg.rootkey_type = CRYPTO_KDF_HARD_KEY_TYPE_ODRK1,
        .key_cfg = {
            .engine = CRYPTO_KLAD_ENGINE_AES,
            .decrypt_support = TD_TRUE,
            .encrypt_support = TD_FALSE
        },
        .key_sec_cfg = {
            .key_sec = TD_FALSE,
            .master_only_enable = TD_FALSE,
            .dest_buf_sec_support = TD_FALSE,
            .dest_buf_non_sec_support = TD_TRUE,
            .src_buf_sec_support = TD_FALSE,
            .src_buf_non_sec_support = TD_TRUE,
        }
    };

    ret = drv_klad_create(&klad_handle);
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_klad_create failed\n");

    ret = drv_klad_attach(klad_handle, CRYPTO_KLAD_DEST_FLASH, CRYPTO_KLAD_FLASH_KEY_TYPE_REE_DEC);
    crypto_chk_goto(ret != TD_SUCCESS, exit_destroy, "drv_klad_attach failed\n");

    ret = drv_klad_set_attr(klad_handle, &klad_attr);
    crypto_chk_goto(ret != TD_SUCCESS, exit_detach, "drv_klad_set_attr failed\n");

    ret = drv_klad_set_effective_key(klad_handle, &effective_key);
    crypto_chk_goto(ret != TD_SUCCESS, exit_detach, "drv_klad_set_effective_key failed\n");

exit_detach:
    drv_klad_detach(klad_handle, CRYPTO_KLAD_DEST_FLASH, CRYPTO_KLAD_FLASH_KEY_TYPE_REE_DEC);
exit_destroy:
    drv_klad_destroy(klad_handle);
    return ret;
}

int drv_rom_cipher_config_clear_key(const crypto_klad_clear_key clear_key)
{
    td_s32 ret;
    td_handle klad_handle;
    crypto_klad_attr klad_attr = {
        .key_cfg = {
            .engine = CRYPTO_KLAD_ENGINE_AES,
            .decrypt_support = TD_TRUE,
            .encrypt_support = TD_FALSE
        },
        .key_sec_cfg = {
            .key_sec = TD_FALSE,
            .master_only_enable = TD_FALSE,
            .dest_buf_sec_support = TD_FALSE,
            .dest_buf_non_sec_support = TD_TRUE,
            .src_buf_sec_support = TD_FALSE,
            .src_buf_non_sec_support = TD_TRUE,
        }
    };

    ret = drv_klad_create(&klad_handle);
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_klad_create failed\n");

    ret = drv_klad_attach(klad_handle, CRYPTO_KLAD_DEST_FLASH, CRYPTO_KLAD_FLASH_KEY_TYPE_REE_DEC);
    crypto_chk_goto(ret != TD_SUCCESS, exit_destroy, "drv_klad_attach failed\n");

    ret = drv_klad_set_attr(klad_handle, &klad_attr);
    crypto_chk_goto(ret != TD_SUCCESS, exit_detach, "drv_klad_set_attr failed\n");

    ret = drv_klad_set_clear_key(klad_handle, &clear_key);
    crypto_chk_goto(ret != TD_SUCCESS, exit_detach, "drv_klad_set_clear_key failed\n");

exit_detach:
    drv_klad_detach(klad_handle, CRYPTO_KLAD_DEST_FLASH, CRYPTO_KLAD_FLASH_KEY_TYPE_REE_DEC);
exit_destroy:
    drv_klad_destroy(klad_handle);
    return ret;
}

int drv_rom_cipher_fapc_config(unsigned int region, unsigned int start_addr, unsigned int end_addr,
    const unsigned char *iv, unsigned int iv_length)
{
    int ret;
    crypto_fapc_config fapc_config = {
        .start_addr = start_addr,
        .end_addr = end_addr,
        .iv_start_addr = start_addr,
        .mode = CRYPTO_FAPC_REGION_MODE_DEC,
        .lock = TD_TRUE,
        .enable = TD_TRUE
    };

    crypto_chk_return((iv == TD_NULL) || (iv_length != CRYPTO_IV_LEN_IN_BYTES),
        TD_FAILURE, "param is NULL\n");

    ret = drv_fapc_set_iv(CRYPTO_FAPC_IV_TYPE_ACPU, iv, iv_length);
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_fapc_set_iv failed\n");

    ret = drv_fapc_set_config(region, &fapc_config);
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_fapc_set_config failed\n");

    return TD_SUCCESS;
}

td_s32 drv_rom_cipher_fapc_bypass_config(td_u32 region, td_u32 start_addr, td_u32 end_addr, td_bool lock)
{
    return drv_fapc_set_bypass_config(region, start_addr, end_addr, lock);
}

int drv_rom_cipher_sm3(const unsigned char *data, unsigned int data_length, unsigned char *hash,
    unsigned int hash_length)
{
    int ret;

    hal_hash_lock(g_hash_chn);
    ret = drv_rom_hash(CRYPTO_HASH_TYPE_SM3, data, data_length, hash, hash_length, g_hash_chn);
    hal_hash_unlock(g_hash_chn);

    return ret;
}

int drv_rom_cipher_pke_sm2_verify(const drv_rom_cipher_ecc_point *pub_key, const drv_rom_cipher_data *hash,
    const drv_rom_cipher_ecc_sig *sig)
{
    int ret;

    ret = drv_cipher_pke_ecdsa_verify(
        DRV_PKE_ECC_TYPE_SM2,
        (const drv_pke_ecc_point *)pub_key,
        (const drv_pke_data *)hash,
        (const drv_pke_ecc_sig *)sig
    );

    return ret;
}