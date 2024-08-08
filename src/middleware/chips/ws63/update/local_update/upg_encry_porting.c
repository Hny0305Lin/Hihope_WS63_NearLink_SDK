/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: UPG encry image functions source file
 */
#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_COMPRESS_ENCRY
#include <stddef.h>
#include <stdint.h>
#include "chip_io.h"
#include "securec.h"
#include "upg_debug.h"
#include "secure_verify_boot.h"
#include "drv_flashboot_cipher.h"
#include "upg_encry_porting.h"

static upg_cryto_cfg g_upg_cryto_cfg = { 0 };

static upg_cryto_cfg *upg_get_cryto_cfg(void)
{
    return &g_upg_cryto_cfg;
}

static errcode_t upg_init_enc_chnum(upg_cryto_cfg *cfg, crypto_klad_effective_key *effective_key)
{
    errcode_t ret = (errcode_t)drv_rom_cipher_symc_init();
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] drv_rom_cipher_symc_init fail, ret = 0x%x. \r\n", ret);
        return ret;
    }
    /* 初始化加密通道 */
    ret = (errcode_t)drv_rom_cipher_create_keyslot(&cfg->encry_cfg.chnum, effective_key, cfg->encry_cfg.key_type);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] drv_rom_cipher_create_keyslot encry failed, ret = 0x%x. \r\n", ret);
        drv_rom_cipher_symc_deinit();
        return ret;
    }
    /* 初始化解密通道 */
    ret = (errcode_t)drv_rom_cipher_create_keyslot(&cfg->decry_cfg.chnum, effective_key, cfg->decry_cfg.key_type);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] drv_rom_cipher_create_keyslot decry failed, ret = 0x%x. \r\n", ret);
        drv_rom_cipher_destroy_keyslot(cfg->encry_cfg.chnum);
        drv_rom_cipher_symc_deinit();
        return ret;
    }
    return ret;
}

errcode_t upg_cryto_init(const upg_image_header_t *image)
{
    upg_cryto_cfg *cfg = upg_get_cryto_cfg();
    crypto_klad_effective_key effective_key = {
        .kdf_hard_alg = CRYPTO_KDF_HARD_ALG_SHA256,
        .key_parity = TD_FALSE,
        .key_size = CRYPTO_KLAD_KEY_SIZE_128BIT,
        .salt = (uint8_t *)image->enc_pk_l1,
        .salt_length = OTA_IMAGE_SALT_LEN,
        .oneway = TD_TRUE
    };
    /* 镜像未加密，直接返回 */
    if (image->re_enc_flag != OTA_ENCRY_FLAG) {
        return ERRCODE_SUCC;
    }

    cfg->encry_cfg.key_type = CRYPTO_KDF_HARD_KEY_TYPE_ODRK1;
    cfg->decry_cfg.key_type = CRYPTO_KDF_HARD_KEY_TYPE_ABRK_REE;

    /* 此处使用enc_pk_l1(16byte)和enc_pk_l2(前12byte)共28byte组合作为加解密的salt */
    if (memcpy_s(cfg->encry_cfg.salt, OTA_IMAGE_SALT_LEN, image->enc_pk_l1, OTA_IMAGE_SALT_LEN) != EOK) {
        upg_log_err("[UPG] upg_init_cryto_cfg memcpy_s encry salt fail. \r\n");
        return ERRCODE_MEMCPY;
    }
    if (memcpy_s(cfg->decry_cfg.salt, OTA_IMAGE_SALT_LEN, image->enc_pk_l1, OTA_IMAGE_SALT_LEN) != EOK) {
        upg_log_err("[UPG] upg_init_cryto_cfg memcpy_s decry salt fail. \r\n");
        return ERRCODE_MEMCPY;
    }
    if (memcpy_s(cfg->encry_cfg.iv, IV_LEN, image->iv, IV_LEN) != EOK) {
        upg_log_err("[UPG] upg_init_cryto_cfg memcpy_s encry iv fail. \r\n");
        return ERRCODE_MEMCPY;
    }
    if (memcpy_s(cfg->decry_cfg.iv, IV_LEN, image->iv, IV_LEN) != EOK) {
        upg_log_err("[UPG] upg_init_cryto_cfg memcpy_s decry iv fail. \r\n");
        return ERRCODE_MEMCPY;
    }
    return upg_init_enc_chnum(cfg, &effective_key);
}

errcode_t upg_cryto_deinit(const upg_image_header_t *image)
{
    errcode_t ret = ERRCODE_SUCC;
    if (image->re_enc_flag != OTA_ENCRY_FLAG) {
        return ret;
    }

    upg_cryto_cfg *cfg = upg_get_cryto_cfg();
    ret |= (errcode_t)drv_rom_cipher_destroy_keyslot(cfg->encry_cfg.chnum);
    ret |= (errcode_t)drv_rom_cipher_destroy_keyslot(cfg->decry_cfg.chnum);
    ret |= (errcode_t)drv_rom_cipher_symc_deinit();
    return ret;
}

/* 镜像头偏移处理 */
errcode_t upg_process_cryto_info(uint32_t *head_offset, td_bool *is_image_head)
{
    if (*is_image_head == TD_FALSE) {
        *head_offset = 0;
        return ERRCODE_SUCC;
    }
    *head_offset = IMAGE_HEADER_LEN;
    *is_image_head = TD_FALSE;
    return ERRCODE_SUCC;
}

/* 升级包数据解密 */
errcode_t upg_decry_fota_pkt(uint8_t *buf, uint32_t len, const upg_image_header_t *image)
{
    errcode_t ret = ERRCODE_SUCC;
    upg_cryto_cfg *cfg = upg_get_cryto_cfg();
    /* 镜像未加密，直接返回 */
    if (image->re_enc_flag != OTA_ENCRY_FLAG) {
        return ret;
    }

    ret = (errcode_t)drv_rom_cipher_symc_decrypt(cfg->decry_cfg.chnum, (uint8_t *)cfg->decry_cfg.iv, IV_LEN,
        (uint32_t)buf, (uint32_t)buf, len);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg_decry_fota_pkt:drv_rom_cipher_symc_decrypt failed, ret = 0x%x \r\n", ret);
        return ret;
    }
    return ERRCODE_SUCC;
}

/* 镜像数据加密 */
errcode_t upg_encry_fota_pkt(uint8_t *buf, uint32_t len, const upg_image_header_t *image)
{
    errcode_t ret = ERRCODE_SUCC;
    upg_cryto_cfg *cfg = upg_get_cryto_cfg();
    /* 镜像未加密，直接返回 */
    if (image->re_enc_flag != OTA_ENCRY_FLAG) {
        return ret;
    }

    ret = (errcode_t)drv_rom_cipher_symc_decrypt(cfg->encry_cfg.chnum, (uint8_t *)cfg->encry_cfg.iv, IV_LEN,
        (uint32_t)buf, (uint32_t)buf, len);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg_encry_fota_pkt:drv_rom_cipher_symc_decrypt failed, ret = 0x%x \r\n", ret);
        return ret;
    }
    return ERRCODE_SUCC;
}

#endif
