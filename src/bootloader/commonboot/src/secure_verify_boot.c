/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Secure Verify for Loaderboot and Flashboot
 *
 * Create: 2023-03-09
 */

#include "boot_serial.h"
#include "securec.h"
#include "efuse_wrap.h"
#include "drv_rom_cipher.h"
#include "boot_errcode.h"
#include "secure_verify_boot.h"

#define MAINTENANCE_MODE_EN          0x2A13C812

#define OEM_ROOT_PUBLIC_KEY_IMAGE_ID 0x4BA5C31E
#define PROVISION_KEY_AREA_IMAGE_ID  0x4BD2F01E
#define PROVISION_CODE_INFO_IMAGE_ID 0x4BD2F02D
#define PARAMS_KET_AREA_IMAGE_ID     0x4B87A51E
#define PARAMS_AREA_INFO_IMAGE_ID    0x4B87A52D
#define PARAMS_AREA_IMAGE_ID         0x4B87A54B
#define FLASHBOOT_KEY_AREA_IMAGE_ID  0x4B1E3C1E
#define FLASHBOOT_CODE_INFO_IMAGE_ID 0x4B1E3C2D
#define FLASHBOOT_BACKUP_KEY_AREA_IMAGE_ID  0x4B69871E
#define FLASHBOOT_BACKUP_CODE_INFO_IMAGE_ID 0x4B69872D
#define APPBOOT_KEY_AREA_IMAGE_ID    0x4B0F2D1E
#define APPBOOT_CODE_INFO_IMAGE_ID   0x4B0F2D2D

#define boot_array_size(arr)  (sizeof(arr) / sizeof((arr)[0]))

typedef struct {
    image_type_t image_type;
    area_type_t erea_type;
    uint32_t image_id;
    efuse_idx efuse_type;
} verify_table_item_t;

uint32_t g_rootkey_status = 0;

static verify_table_item_t g_verify_table[] = {
    {
        .image_type = LOADER_BOOT_TYPE, .erea_type = KEY_EREA_TYPE,
        .image_id = PROVISION_KEY_AREA_IMAGE_ID
    },

    {
        .image_type = LOADER_BOOT_TYPE, .erea_type = CODE_INFO_TYPE,
        .image_id = PROVISION_CODE_INFO_IMAGE_ID
    },

    {
        .image_type = FLASH_BOOT_TYPE, .erea_type = KEY_EREA_TYPE,
        .image_id = FLASHBOOT_KEY_AREA_IMAGE_ID,
        .efuse_type = EFUSE_FLASHBOOT_VER_ID
    },

    {
        .image_type = FLASH_BOOT_TYPE, .erea_type = CODE_INFO_TYPE,
        .image_id = FLASHBOOT_CODE_INFO_IMAGE_ID,
        .efuse_type = EFUSE_FLASHBOOT_VER_ID
    },

    {
        .image_type = FLASH_BOOT_BACK_UP_TYPE, .erea_type = KEY_EREA_TYPE,
        .image_id = FLASHBOOT_BACKUP_KEY_AREA_IMAGE_ID,
        .efuse_type = EFUSE_FLASHBOOT_VER_ID
    },

    {
        .image_type = FLASH_BOOT_BACK_UP_TYPE, .erea_type = CODE_INFO_TYPE,
        .image_id = FLASHBOOT_BACKUP_CODE_INFO_IMAGE_ID,
        .efuse_type = EFUSE_FLASHBOOT_VER_ID
    },

    {
        .image_type = SECOND_FLASH_BOOT_TYPE, .erea_type = KEY_EREA_TYPE,
        .image_id = FLASHBOOT_KEY_AREA_IMAGE_ID,
        .efuse_type = EFUSE_FLASHBOOT_VER_ID
    },

    {
        .image_type = SECOND_FLASH_BOOT_TYPE, .erea_type = CODE_INFO_TYPE,
        .image_id = FLASHBOOT_CODE_INFO_IMAGE_ID,
        .efuse_type = EFUSE_FLASHBOOT_VER_ID
    },

    {
        .image_type = SECOND_FLASH_BOOT_BACK_UP_TYPE, .erea_type = KEY_EREA_TYPE,
        .image_id = FLASHBOOT_BACKUP_KEY_AREA_IMAGE_ID,
        .efuse_type = EFUSE_FLASHBOOT_VER_ID
    },

    {
        .image_type = SECOND_FLASH_BOOT_BACK_UP_TYPE, .erea_type = CODE_INFO_TYPE,
        .image_id = FLASHBOOT_BACKUP_CODE_INFO_IMAGE_ID,
        .efuse_type = EFUSE_FLASHBOOT_VER_ID
    },

    {
        .image_type = PARAMS_BOOT_TYPE, .erea_type = PARAMS_KEY_AREA_TYPE,
        .image_id = PARAMS_KET_AREA_IMAGE_ID,
        .efuse_type = EFUSE_PARAMS_VER_ID
    },

    {
        .image_type = PARAMS_BOOT_TYPE, .erea_type = PARAMS_AREA_IOFO_TYPE,
        .image_id = PARAMS_AREA_INFO_IMAGE_ID,
        .efuse_type = EFUSE_PARAMS_VER_ID
    },

    {
        .image_type = APP_BOOT_TYPE, .erea_type = KEY_EREA_TYPE,
        .image_id = APPBOOT_KEY_AREA_IMAGE_ID,
        .efuse_type = EFUSE_MCU_VER_ID
    },

    {
        .image_type = APP_BOOT_TYPE, .erea_type = CODE_INFO_TYPE,
        .image_id = APPBOOT_CODE_INFO_IMAGE_ID,
        .efuse_type = EFUSE_MCU_VER_ID
    },
};

static errcode_t check_image_id(image_type_t image_type, area_type_t erea_type, uint32_t image_id)
{
    uint32_t image_id_check = 0;

    for (uint32_t i = 0; i < boot_array_size(g_verify_table); i++) {
        if (image_type == g_verify_table[i].image_type && erea_type == g_verify_table[i].erea_type) {
            image_id_check = g_verify_table[i].image_id;
            break;
        }
        if (i == boot_array_size(g_verify_table)) {
            boot_msg0("get key erea iamge id failed!");
            return ERRCODE_FAIL;
        }
    }

    if (image_id != image_id_check) {
        boot_msg0("image id error!");
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

static uint32_t count_bit_num(uint32_t input)
{
    uint32_t count = 0; /* count accumulates the total bits set in input */
    uint32_t input_copy = input;

    for (count = 0; input_copy >= 1; count++) {
        input_copy &= input_copy - 1; /* clear the least significant bit set */
    }

    return count;
}

static errcode_t check_version(image_type_t image_type, uint32_t version, uint32_t version_mask)
{
    uint32_t efuse_version = 0;
    uint32_t efuse_idx = 0;
    uint32_t ret = 0;

    for (uint32_t i = 0; i < boot_array_size(g_verify_table); i++) {
        if (image_type == g_verify_table[i].image_type) {
            efuse_idx = g_verify_table[i].efuse_type;
            break;
        }
        if (i == boot_array_size(g_verify_table)) {
            boot_msg0("get key erea iamge id failed!");
            return ERRCODE_FAIL;
        }
    }

    /* read otp version */
    /* sizeof(efuse_version) = 4 */
    ret = efuse_read_item(efuse_idx, (uint8_t *)&efuse_version, (uint16_t)sizeof(efuse_version));
    if (ret != ERRCODE_SUCC) {
        boot_msg0("version efuse_read_item fail!");
        return ERRCODE_FAIL;
    }

    if (count_bit_num(version & version_mask) < count_bit_num(efuse_version & version_mask)) {
        boot_msg0("version compare fail!");
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

static errcode_t check_msid(uint32_t msid, uint32_t msid_mask)
{
    uint32_t efuse_msid = 0;
    uint32_t ret = 0;

    /* sizeof(efuse_msid) = 4 */
    ret = efuse_read_item(EFUSE_MSID_ID, (uint8_t *)&efuse_msid, (uint16_t)sizeof(efuse_msid));
    if (ret != ERRCODE_SUCC) {
        boot_msg0("version efuse_read_item fail!");
        return ERRCODE_FAIL;
    }

    if ((msid & msid_mask) != (efuse_msid & msid_mask)) {
        boot_msg0("msid compare fail!");
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

static errcode_t check_die_id(uint8_t *die_id, uint32_t die_id_length)
{
    uint8_t efuse_die_id[DIE_ID_LEN] = { 0 };
    uint32_t ret = 0;

    if (die_id_length != DIE_ID_LEN) {
        return ERRCODE_FAIL;
    }

    /* sizeof(efuse_die_id) = 16 */
    ret = efuse_read_item(EFUSE_DIE_ID, (uint8_t *)efuse_die_id, (uint16_t)sizeof(efuse_die_id));
    if (ret != ERRCODE_SUCC) {
        boot_msg0("die_id efuse_read_item fail!");
        return ERRCODE_FAIL;
    }

    ret = memcmp(efuse_die_id, die_id, die_id_length);
    if (ret != 0) {
        boot_msg0("die id memcmp fail!");
        return ERRCODE_FAIL;
    }

    return ret;
}

static errcode_t check_verify_enable(void)
{
    uint32_t ret = 0;
    uint8_t verify_enable = 0xff;

    /* sizeof(verify_enable) = 1 */
    ret = efuse_read_item(EFUSE_SEC_VERIFY_ENABLE, &verify_enable, (uint16_t)sizeof(verify_enable));
    if (ret != ERRCODE_SUCC) {
        boot_msg0("efuse_read_item verify enable fail!");
        return ERRCODE_FAIL;
    }
    if (verify_enable == 0) {
        return ERRCODE_NOT_SUPPORT;
    }

    return ERRCODE_SUCC;
}

#if defined(CONFIG_BOOT_SUPPORT_ECC_VERIFY)
static errcode_t secure_authenticate(const uint8_t *key, const uint8_t *data, uint32_t data_length,
    const uint8_t *sign_buff, uint32_t sign_length)
{
    uint8_t hash_result[HASH_LEN];
    drv_rom_cipher_ecc_point input_pub_key = { 0 };
    drv_rom_cipher_data input_hash = { 0 };
    drv_rom_cipher_ecc_sig input_sig = { 0 };
    uint32_t ret = 0;

    ret = drv_rom_cipher_sha256(data, data_length, hash_result, (uint32_t)HASH_LEN);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("secure_authenticate drv_rom_cipher_sha256 fail!");
        return ERRCODE_FAIL;
    }

    input_pub_key.x = (uint8_t *)key;
    input_pub_key.y = (uint8_t *)(key + 32);    /* 32 is ecc bp256 key length */
    input_pub_key.length = 32;                  /* 32 is ecc bp256 key length */
    input_hash.data = hash_result;
    input_hash.length = HASH_LEN;
    input_sig.r = (uint8_t *)sign_buff;
    input_sig.s = (uint8_t *)(sign_buff + sign_length / 2);    /* 2: sign =  r + s */
    input_sig.length = sign_length / 2;    /* 2: sign =  r + s */

    ret = (uint32_t)drv_rom_cipher_pke_bp256r_verify(&input_pub_key, &input_hash, &input_sig);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("secure_authenticate drv_rom_cipher_pke_bp256r_verify fail!");
        return ERRCODE_FAIL;
    }

    return ret;
}
#endif

#if defined(CONFIG_BOOT_SUPPORT_SM2_VERIFY)
static errcode_t secure_authenticate(const uint8_t *key, const uint8_t *data, uint32_t data_length,
    const uint8_t *sign_buff, uint32_t sign_length)
{
    uint8_t hash_result[HASH_LEN] = { 0 };
    drv_rom_cipher_ecc_point input_pub_key = { 0 };
    drv_rom_cipher_pke_msg input_msg = { 0 };
    drv_rom_cipher_data input_hash = { 0 };
    drv_rom_cipher_ecc_sig input_sig = { 0 };
    uint8_t sm2_id[18] = "\x42\x4c\x49\x43\x45\x31\x32\x33\x40\x59\x41\x48\x4f\x4f\x2\x43\x4f\x11";
    uint32_t ret = 0;

    input_pub_key.x = (uint8_t *)key;
    input_pub_key.y = (uint8_t *)(key + 32);    /* 32 is sm2 key length */
    input_pub_key.length = 32;      /* 32 is sm2 key length */
    input_msg.data = (uint8_t *)data;
    input_msg.length = data_length;
    input_hash.data = hash_result;
    input_hash.length = sizeof(hash_result);
    ret = drv_rom_cipher_pke_sm2_dsa_hash(sm2_id, &input_pub_key, &input_msg, &input_hash);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("secure_authenticate drv_rom_cipher_pke_sm2_dsa_hash fail!");
        return ERRCODE_FAIL;
    }

    input_sig.r = (uint8_t *)sign_buff;
    input_sig.s = (uint8_t *)(sign_buff + sign_length / 2);    /* 2: sign =  r + s */
    input_sig.length = sign_length / 2;    /* 2: sign =  r + s */
    ret = drv_rom_cipher_pke_sm2_verify(input_pub_key, input_hash, input_sig);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("secure_authenticate drv_rom_cipher_pke_sm2_verify fail!");
        return ERRCODE_FAIL;
    }

    return ret;
}
#endif

static errcode_t verify_params_key_area(const params_key_area_t *key_area, const uint8_t *public_key)
{
    uint32_t ret = 0;

    /* check verify enable */
    ret = check_verify_enable();
    if (ret == ERRCODE_NOT_SUPPORT) {
        return ERRCODE_SUCC;
    } else if (ret == ERRCODE_FAIL) {
        boot_msg0("verify_params_key_area secure verify error!");
        return ERRCODE_BOOT_VERIFY_CHECK_ENABLE;
    }

    if (g_rootkey_status != 0x5) {
        boot_msg0("verify_params_key_area rootkey_status invalid!");
        return ERRCODE_BOOT_VERIFY_INVALID_ROOT_KEY;
    }

    /* check image id */
    ret = check_image_id(PARAMS_BOOT_TYPE, PARAMS_KEY_AREA_TYPE, key_area->image_id);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_params_key_area image id error!");
        return ERRCODE_BOOT_VERIFY_INVALID_IMAGE_ID;
    }

    /* verify key area with rootkey */
    ret = secure_authenticate(public_key, (uint8_t *)key_area, KEY_AREA_STRUCTURE_LENGTH - BOOT_SIG_LEN,
        key_area->sig_params_key_area, BOOT_PUBLIC_KEY_LEN);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_params_key_area secure_authenticate fail!");
        return ERRCODE_BOOT_VERIFY_PKE_VERIFY;
    }

    /* check key version ext */
    ret = check_version(PARAMS_BOOT_TYPE,
        key_area->params_key_version_ext, key_area->mask_params_key_version_ext);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_params_key_area key version error!");
        return ERRCODE_BOOT_VERIFY_INVALID_VERSION;
    }

    /* check key msid */
    ret = check_msid(key_area->msid_ext, key_area->mask_msid_ext);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_params_key_area msid error!");
        return ERRCODE_BOOT_VERIFY_INVALID_MSID;
    }

    /* check maintention mode */
    if (key_area->maintenance_mode == MAINTENANCE_MODE_EN) {
        ret = check_die_id((uint8_t *)key_area->die_id, DIE_ID_LEN);
        if (ret != ERRCODE_SUCC) {
            boot_msg0("verify_params_key_area die id error!");
            return ERRCODE_BOOT_VERIFY_INVALID_DIE_ID;
        }
    }

    return ret;
}

static errcode_t verify_params_area_info(const params_area_info_t *area_info, const uint8_t *public_key)
{
    uint32_t ret = 0;

    /* check verify enable */
    ret = check_verify_enable();
    if (ret == ERRCODE_NOT_SUPPORT) {
        return ERRCODE_SUCC;
    } else if (ret == ERRCODE_FAIL) {
        boot_msg0("verify_params_area_info secure verify error!");
        return ERRCODE_BOOT_VERIFY_CHECK_ENABLE;
    }

    /* check image id */
    ret = check_image_id(PARAMS_BOOT_TYPE, PARAMS_AREA_IOFO_TYPE, area_info->image_id);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_params_area_info image id error!");
        return ERRCODE_BOOT_VERIFY_INVALID_IMAGE_ID;
    }

    /* verify code info with external public key */
    ret = secure_authenticate(public_key, (uint8_t *)area_info,
        CODE_INFO_STRUCTURE_LENGTH - BOOT_SIG_LEN - BOOT_EXT_SIG_LEN, area_info->sig_params_info, BOOT_PUBLIC_KEY_LEN);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_params_area_info secure_authenticate fail!");
        return ERRCODE_BOOT_VERIFY_PKE_VERIFY;
    }

    /* check version ext */
    ret = check_version(PARAMS_BOOT_TYPE,
        area_info->params_version_ext, area_info->mask_params_version_ext);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_params_area_info key version error!");
        return ERRCODE_BOOT_VERIFY_INVALID_VERSION;
    }

    /* check msid */
    ret = check_msid(area_info->msid_ext, area_info->mask_msid_ext);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_params_area_info msid error!");
        return ERRCODE_BOOT_VERIFY_INVALID_MSID;
    }

    return ret;
}

static errcode_t verify_params_area(const params_area_info_t *area_info, const uint8_t *area_addr)
{
    uint32_t area_length = 0;
    uint8_t hash_result[HASH_LEN] = { 0 };
    uint32_t ret = 0;

    /* caculate code area hash */
    area_length = area_info->params_area_len;
    ret = drv_rom_cipher_sha256((unsigned char *)(uintptr_t)area_addr, area_length, hash_result, (uint32_t)HASH_LEN);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_params_area drv_rom_cipher_sha256 fail!");
        return ERRCODE_BOOT_VERIFY_HASH_CALCULATION;
    }

    ret = memcmp(hash_result, area_info->params_area_hash, (uint32_t)HASH_LEN);
    if (ret != 0) {
        boot_msg0("verify_params_area hash memcmp fail!");
        return ERRCODE_BOOT_VERIFY_INVALID_HASH_RESULT;
    }

    return ret;
}

static errcode_t verify_image_key_area(image_type_t image_type, area_type_t erea_type,
    const image_key_area_t *key_area, const uint8_t *public_key)
{
    uint32_t ret = 0;

    /* check verify enable */
    ret = check_verify_enable();
    if (ret == ERRCODE_NOT_SUPPORT) {
        boot_msg0("verify_image_key_area secure verify disable!");
        return ERRCODE_SUCC;
    } else if (ret == ERRCODE_FAIL) {
        boot_msg0("verify_image_key_area secure verify error!");
        return ERRCODE_BOOT_VERIFY_CHECK_ENABLE;
    }

    if ((image_type == LOADER_BOOT_TYPE) || (image_type == FLASH_BOOT_TYPE) ||
        (image_type == FLASH_BOOT_BACK_UP_TYPE)) {
        if (g_rootkey_status != 0x5) {
            boot_msg0("verify_image_key_area rootkey_status invalid!");
            return ERRCODE_BOOT_VERIFY_INVALID_ROOT_KEY;
        }
    }

    /* check image id */
    ret = check_image_id(image_type, erea_type, key_area->image_id);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_image_key_area image id error!");
        return ERRCODE_BOOT_VERIFY_INVALID_IMAGE_ID;
    }

    /* verify key area with rootkey */
    ret = secure_authenticate(public_key, (uint8_t *)key_area, KEY_AREA_STRUCTURE_LENGTH - BOOT_SIG_LEN,
        key_area->sig_key_area, BOOT_PUBLIC_KEY_LEN);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_image_key_area secure_authenticate!");
        return ERRCODE_BOOT_VERIFY_PKE_VERIFY;
    }

    /* check key version ext */
    if (image_type != LOADER_BOOT_TYPE) {
        ret = check_version(image_type, key_area->key_version_ext, key_area->mask_key_version_ext);
        if (ret != ERRCODE_SUCC) {
            boot_msg0("verify_image_key_area key version error!");
            return ERRCODE_BOOT_VERIFY_INVALID_VERSION;
        }
    }

    /* check key msid */
    ret = check_msid(key_area->msid_ext, key_area->mask_msid_ext);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_image_key_area msid error!");
        return ERRCODE_BOOT_VERIFY_INVALID_MSID;
    }

    /* check maintention mode */
    if (key_area->maintenance_mode == MAINTENANCE_MODE_EN) {
        ret = check_die_id((uint8_t *)key_area->die_id, DIE_ID_LEN);
        if (ret != ERRCODE_SUCC) {
            boot_msg0("verify_image_key_area die id error!");
            return ERRCODE_BOOT_VERIFY_INVALID_DIE_ID;
        }
    }

    return ret;
}

static errcode_t verify_image_code_info(image_type_t image_type, area_type_t erea_type,
    const image_code_info_t *code_info, const uint8_t *public_key)
{
    uint32_t ret = 0;

    /* check verify enable */
    ret = check_verify_enable();
    if (ret == ERRCODE_NOT_SUPPORT) {
        boot_msg0("verify_image_code_info secure verify disable!");
        return ERRCODE_SUCC;
    } else if (ret == ERRCODE_FAIL) {
        boot_msg0("verify_image_code_info secure verify error!");
        return ERRCODE_BOOT_VERIFY_CHECK_ENABLE;
    }

    /* check image id */
    ret = check_image_id(image_type, erea_type, code_info->image_id);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_image_code_info image id error!");
        return ERRCODE_BOOT_VERIFY_INVALID_IMAGE_ID;
    }

    /* verify code info with external public key */
    ret = secure_authenticate(public_key, (uint8_t *)code_info,
        CODE_INFO_STRUCTURE_LENGTH - BOOT_SIG_LEN - BOOT_EXT_SIG_LEN, code_info->sig_code_info, BOOT_PUBLIC_KEY_LEN);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_image_code_info secure_authenticate!");
        return ERRCODE_BOOT_VERIFY_PKE_VERIFY;
    }

    /* check version ext */
    if (image_type != LOADER_BOOT_TYPE) {
        ret = check_version(image_type, code_info->version_ext, code_info->mask_version_ext);
        if (ret != ERRCODE_SUCC) {
            boot_msg0("verify_image_code_info key version error!");
            return ERRCODE_BOOT_VERIFY_INVALID_VERSION;
        }
    }

    /* check msid */
    ret = check_msid(code_info->msid_ext, code_info->mask_msid_ext);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_image_code_info msid error!");
        return ERRCODE_BOOT_VERIFY_INVALID_MSID;
    }

    return ret;
}

static errcode_t verify_image_code_area(const image_code_info_t *code_info, const uint8_t *code_addr)
{
    uint32_t code_length = 0;
    uint8_t hash_result[HASH_LEN] = { 0 };
    uint32_t ret = 0;

    /* caculate code area hash */
    code_length = code_info->code_area_len;
    ret = drv_rom_cipher_sha256((unsigned char *)(uintptr_t)code_addr, code_length, hash_result, (uint32_t)HASH_LEN);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_image_code_area drv_rom_cipher_sha256!");
        return ERRCODE_BOOT_VERIFY_HASH_CALCULATION;
    }
    ret = memcmp(hash_result, code_info->code_area_hash, (uint32_t)HASH_LEN);
    if (ret != 0) {
        boot_msg0("verify_image_code_area hash memcmp fail!");
        return ERRCODE_BOOT_VERIFY_INVALID_HASH_RESULT;
    }

    return ret;
}

errcode_t verify_public_rootkey(uint32_t rootkey_buff_addr)
{
    uint8_t hash_result[HASH_LEN] = { 0 };
    uint8_t hash_from_otp[HASH_LEN] = { 0 };
    uint32_t ret = ERRCODE_SUCC;
    const root_public_key_area_t *rootkey_buff = (root_public_key_area_t *)(uintptr_t)rootkey_buff_addr;

    if (rootkey_buff == NULL) {
        return ERRCODE_BOOT_VERIFY_PARAM_NULL;
    }

    g_rootkey_status = 0xA;

    /* check verify enable */
    ret = check_verify_enable();
    if (ret == ERRCODE_NOT_SUPPORT) {
        return ERRCODE_SUCC;
    } else if (ret == ERRCODE_FAIL) {
        boot_msg0("verify_public_rootkey secure verify error!");
        return ERRCODE_BOOT_VERIFY_CHECK_ENABLE;
    }

    boot_msg0("secure boot.");

    /* check rootkey_buff->image_id */
    if (rootkey_buff->image_id != OEM_ROOT_PUBLIC_KEY_IMAGE_ID) {
        boot_msg0("verify_public_rootkey image id error!");
        return ERRCODE_BOOT_VERIFY_INVALID_IMAGE_ID;
    }

    /* caculate rootkey hash */
    ret = drv_rom_cipher_sha256((unsigned char *)rootkey_buff, ROOT_PUBLIC_KEY_STRUCTURE_LENGTH,
        hash_result, (uint32_t)HASH_LEN);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_public_rootkey drv_rom_cipher_sha256 fail!");
        return ERRCODE_BOOT_VERIFY_HASH_CALCULATION;
    }

    /* read efuse rootkey hash */
    /* sizeof(hash_from_otp) = 32 */
    ret = efuse_read_item(EFUSE_HASH_ROOT_PUBLIC_KEY_ID, hash_from_otp, (uint16_t)sizeof(hash_from_otp));
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_public_rootkey efuse_read_item fail!");
        return ERRCODE_BOOT_VERIFY_EFUSE_READ;
    }

    /* check hash */
    ret = memcmp(hash_from_otp, hash_result, (uint32_t)HASH_LEN);
    if (ret != 0) {
        boot_msg0("verify_public_rootkey hash memcmp fail!");
        return ERRCODE_BOOT_VERIFY_INVALID_HASH_RESULT;
    }

    g_rootkey_status = 0x5;

    return ret;
}

errcode_t verify_image_head(image_type_t image_type, uint32_t public_key_addr, uint32_t boot_head_addr)
{
    uint32_t ret = 0;
    image_key_area_t *key_area = (image_key_area_t *)(uintptr_t)boot_head_addr;
    image_code_info_t *code_info = (image_code_info_t *)(uintptr_t)((uintptr_t)key_area + KEY_AREA_STRUCTURE_LENGTH);

    if (key_area == NULL || code_info == NULL) {
        return ERRCODE_BOOT_VERIFY_PARAM_NULL;
    }

    ret = verify_image_key_area(image_type, KEY_EREA_TYPE, key_area, (uint8_t *)(uintptr_t)public_key_addr);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_image_key_area fail!");
        return ret;
    }

    ret = verify_image_code_info(image_type, CODE_INFO_TYPE, code_info, key_area->ext_pulic_key_area);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_image_code_info fail!");
        return ret;
    }

    return ret;
}

errcode_t verify_image_body(uint32_t boot_head_addr, uint32_t boot_body_addr)
{
    uint32_t ret = 0;
    const image_code_info_t *code_info = (image_code_info_t *)(uintptr_t)(boot_head_addr + KEY_AREA_STRUCTURE_LENGTH);
    const uint8_t *code_addr = (uint8_t *)(uintptr_t)boot_body_addr;

    if (code_info == NULL || code_addr == NULL) {
        return ERRCODE_BOOT_VERIFY_PARAM_NULL;
    }

    ret = verify_image_code_area(code_info, code_addr);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_image_code_area fail!");
        return ret;
    }

    return ret;
}

errcode_t verify_params_head(uint32_t root_public_key_addr, uint32_t params_head_addr)
{
    uint32_t ret = 0;
    root_public_key_area_t *public_rootkey = (root_public_key_area_t *)(uintptr_t)root_public_key_addr;
    params_key_area_t *key_area = (params_key_area_t *)(uintptr_t)params_head_addr;
    params_area_info_t *area_info = (params_area_info_t *)(uintptr_t)((uintptr_t)key_area + KEY_AREA_STRUCTURE_LENGTH);

    if (public_rootkey == NULL || key_area == NULL || area_info == NULL) {
        return ERRCODE_BOOT_VERIFY_PARAM_NULL;
    }

    ret = verify_params_key_area(key_area, public_rootkey->root_key_area);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_params_key_area fail!");
        return ret;
    }

    ret = verify_params_area_info(area_info, key_area->params_ext_key_area);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_params_area_info fail!");
        return ret;
    }

    return ret;
}

errcode_t verify_params_body(uint32_t params_head_addr, uint32_t params_body_addr)
{
    uint32_t ret = 0;
    const params_area_info_t *area_info =
        (params_area_info_t *)(uintptr_t)(params_head_addr + KEY_AREA_STRUCTURE_LENGTH);
    const uint8_t *area_addr = (uint8_t *)(uintptr_t)params_body_addr;

    if (area_info == NULL || area_addr == NULL) {
        return ERRCODE_BOOT_VERIFY_PARAM_NULL;
    }

    ret = verify_params_area(area_info, area_addr);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("verify_params_area fail!");
        return ret;
    }

    return ret;
}