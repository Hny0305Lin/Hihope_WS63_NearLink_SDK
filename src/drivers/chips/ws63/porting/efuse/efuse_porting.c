/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides efuse port template \n
 *
 * History: \n
 * 2023-3-4， Create file. \n
 */
#include "hal_efuse_v151.h"
#include "chip_io.h"
#include "securec.h"
#include "soc_porting.h"
#include "efuse.h"
#include "efuse_porting.h"

#define EFUSE0_BASE_ADDR             0x44008000
#define HAL_EFUSE0_WRITE_BASE_ADDR   (EFUSE0_BASE_ADDR + 0x800)
#define HAL_EFUSE0_READ_BASE_ADDR    (EFUSE0_BASE_ADDR + 0x800)
#define BIT_SIZE_OF_BYTE 8

uint32_t g_efuse_boot_done_addr = EFUSE0_BASE_ADDR + 0x2c;
uint32_t g_efuse_base_addr[1] = {EFUSE0_BASE_ADDR + 0x30};
uint32_t g_efuse_region_read_address[1] = {HAL_EFUSE0_READ_BASE_ADDR};
uint32_t g_efuse_region_write_address[1] = {HAL_EFUSE0_WRITE_BASE_ADDR};

typedef struct {
    uint16_t id_start_bit;
    uint16_t id_size;
    uint8_t attr;
} efuse_stru;

static efuse_stru g_efuse_cfg[EFUSE_IDX_MAX] = {
    { 0,    8,   EFUSE_IDX_RW }, // chip id
    { 8,    160, EFUSE_IDX_RW }, // die id
    { 624,  32,  EFUSE_IDX_RW }, // mcu ver
    { 656,  8,   EFUSE_IDX_RW }, // flashboot ver
    { 664,  8,   EFUSE_IDX_RW }, // params ver
    { 672,  256, EFUSE_IDX_RW }, // hash root public key
    { 928,  32,  EFUSE_IDX_RW }, // msid
    { 960,  1,   EFUSE_IDX_RW }, // sec verify enable
    { 1000, 8,   EFUSE_IDX_RW }, // type id
    { 1584, 48,  EFUSE_IDX_RW }, // mac_addr_01
    { 1632, 48,  EFUSE_IDX_RW }, // mac_addr_02
    { 1680, 48,  EFUSE_IDX_RW }, // mac_addr_03
    { 1728, 48,  EFUSE_IDX_RW }, // mac_addr_last
    { 1776, 128, EFUSE_IDX_RW }, // customer rsvd
    { 672, 256,  EFUSE_IDX_RW }, // customer rsvd2
    { 1135, 1,   EFUSE_IDX_RW }, // code to valtage s
    { 1120, 15,  EFUSE_IDX_RW }, // code to valtage b
    { 1136, 16,  EFUSE_IDX_RW }, // code to valtage k
};

void efuse_port_register_hal_funcs(void)
{
    hal_efuse_register_funcs(hal_efuse_funcs_get());
}

void efuse_port_unregister_hal_funcs(void)
{
    hal_efuse_unregister_funcs();
}

hal_efuse_region_t hal_efuse_get_region(uint32_t byte_addr)
{
    return (hal_efuse_region_t)(byte_addr / EFUSE_REGION_MAX_BYTES);
}

uint16_t hal_efuse_get_byte_offset(uint32_t byte_addr)
{
    return byte_addr % EFUSE_REGION_MAX_BYTES;
}

uint32_t efuse_read_item(efuse_idx efuse_id, uint8_t *data, uint16_t data_len)
{
    uint16_t item_data_len;
    uint16_t bit_index;
    uint32_t byte_index;
    uint8_t bit_offset;
    uint8_t value;
    uint16_t id_start_bit;
    uint16_t id_size;
    uint16_t result_index;
    uint32_t ret;
    if ((data == NULL) || (efuse_id >= EFUSE_IDX_MAX)) {
        return ERRCODE_FAIL;
    }
    id_start_bit = g_efuse_cfg[efuse_id].id_start_bit;
    id_size = g_efuse_cfg[efuse_id].id_size;
    item_data_len = (id_size + BIT_SIZE_OF_BYTE - 1) / BIT_SIZE_OF_BYTE;
    if (item_data_len > data_len) {
        return ERRCODE_FAIL;
    }
    memset_s(data, data_len, 0, data_len);
    if ((id_size % BIT_SIZE_OF_BYTE) == 0 && (id_start_bit % BIT_SIZE_OF_BYTE) == 0) {
        return uapi_efuse_read_buffer(data, (id_start_bit / BIT_SIZE_OF_BYTE), item_data_len);
    } else {
        for (bit_index = id_start_bit; bit_index < (id_start_bit + id_size); bit_index++) {
            byte_index = bit_index / BIT_SIZE_OF_BYTE;
            bit_offset = bit_index % BIT_SIZE_OF_BYTE;
            ret = uapi_efuse_read_bit(&value, byte_index, bit_offset);
            if (ret != ERRCODE_SUCC) {
                return ret;
            }
            if (value != 0) {
                result_index = (bit_index - id_start_bit) / BIT_SIZE_OF_BYTE;
                data[result_index] = data[result_index] |
                    (1 << ((uint16_t)((bit_index - id_start_bit) % BIT_SIZE_OF_BYTE)));
            }
        }
    }
    return ERRCODE_SUCC;
}

#define EFUSE_24M_CLK 0x29
#define EFUSE_40M_CLK 0x19
void set_efuse_period(void)
{
    if (get_tcxo_freq() == CLK24M_TCXO) {
        hal_efuse_set_clock_period(EFUSE_24M_CLK);
    } else {
        hal_efuse_set_clock_period(EFUSE_40M_CLK);
    }
}

#define FIRST_MAC_ADDR_START_BIT 1584
#define SECOND_MAC_ADDR_START_BIT 1632
#define THIRD_MAC_ADDR_START_BIT 1680
#define FORTH_MAC_ADDR_START_BIT 1728
#define FIRST_MAC_LOCK_BIT 314
#define SECOND_MAC_LOCK_BIT 315
#define THIRD_MAC_LOCK_BIT 316
#define FORTH_MAC_LOCK_BIT 317
#define EFUSE_MAC_NUM 4
#define EFUSE_MAC_LEN 6

#define BIT_TO_BYTE 8
#define SLE_MAC_ADDR_START_BIT 1904

uint32_t efuse_write_mac(uint8_t *data, uint16_t data_len)
{
    uint8_t index;
    uint32_t lock_bit;
    uint32_t mac_bit;
    uint8_t lock_data = 0;
    uint32_t ret = ERRCODE_FAIL;

    if ((data == NULL) || (data_len != EFUSE_MAC_LEN)) {
        return ERRCODE_FAIL;
    }

    for (index = 0; index < EFUSE_MAC_NUM; index++) {
        lock_bit = FIRST_MAC_LOCK_BIT + index;
        mac_bit = FIRST_MAC_ADDR_START_BIT + index * EFUSE_MAC_LEN * BIT_TO_BYTE;
        ret = uapi_efuse_read_bit(&lock_data, (lock_bit / BIT_TO_BYTE), (lock_bit % BIT_TO_BYTE));
        if ((ret == ERRCODE_SUCC) && (lock_data == 0)) {
            ret = uapi_efuse_write_buffer((mac_bit / BIT_TO_BYTE), data, data_len);
            if (ret == ERRCODE_SUCC) {
                ret = uapi_efuse_write_bit((lock_bit / BIT_TO_BYTE), (lock_bit % BIT_TO_BYTE));
            }
            return ret;
        }
    }
    return ERRCODE_FAIL;
}

uint32_t efuse_read_mac(uint8_t *data, uint16_t data_len, uint8_t *left_time)
{
    uint8_t index;
    uint32_t lock_bit;
    uint32_t mac_bit;
    uint8_t lock_data = 0;
    uint32_t ret = ERRCODE_FAIL;

    if ((data == NULL) || (data_len != EFUSE_MAC_LEN)) {
        return ERRCODE_FAIL;
    }

    for (index = 0; index < EFUSE_MAC_NUM; index++) {
        lock_bit = FIRST_MAC_LOCK_BIT + EFUSE_MAC_NUM - 1 - index;
        mac_bit = FIRST_MAC_ADDR_START_BIT + (EFUSE_MAC_NUM - 1 - index) * EFUSE_MAC_LEN * BIT_TO_BYTE;
        ret = uapi_efuse_read_bit(&lock_data, (lock_bit / BIT_TO_BYTE), (lock_bit % BIT_TO_BYTE));
        if ((ret == ERRCODE_SUCC) && (lock_data != 0)) {
            *left_time = index;
            ret = uapi_efuse_read_buffer(data, (mac_bit / BIT_TO_BYTE), data_len);
            return ret;
        }
    }
    if (index == (EFUSE_MAC_NUM - 1)) {
        *left_time = EFUSE_MAC_NUM;
        ret = uapi_efuse_read_buffer(data, (mac_bit / BIT_TO_BYTE), data_len);
        return ret;
    }
    return ERRCODE_FAIL;
}

static uint32_t check_data(const uint8_t *data, uint16_t data_len)
{
    uint8_t index;

    if ((data == NULL) || (data_len == 0)) {
        return ERRCODE_FAIL;
    }
    for (index = 0; index < data_len; index++) {
        if (data[index] != 0) {
            return ERRCODE_FAIL;
        }
    }
    return ERRCODE_SUCC;
}

uint32_t efuse_write_sle_mac(uint8_t *data, uint16_t data_len)
{
    uint32_t ret = ERRCODE_FAIL;
    uint8_t read_back_addr[EFUSE_MAC_LEN];
    if ((data == NULL) || (data_len != EFUSE_MAC_LEN)) {
        return ERRCODE_FAIL;
    }
    ret = uapi_efuse_read_buffer(read_back_addr, (SLE_MAC_ADDR_START_BIT / BIT_TO_BYTE), EFUSE_MAC_LEN);
    if ((ret == ERRCODE_SUCC) && (check_data(read_back_addr, data_len) == ERRCODE_SUCC)) {
        ret = uapi_efuse_write_buffer((SLE_MAC_ADDR_START_BIT / BIT_TO_BYTE), data, data_len);
        return ret;
    }
    return ERRCODE_FAIL;
}

uint32_t efuse_read_sle_mac(uint8_t *data, uint16_t data_len)
{
    if ((data == NULL) || (data_len != EFUSE_MAC_LEN)) {
        return ERRCODE_FAIL;
    }
    return uapi_efuse_read_buffer(data, (SLE_MAC_ADDR_START_BIT / BIT_TO_BYTE), data_len);
}

uint32_t efuse_write_customer_rsvd_efuse(uint8_t *data, uint16_t data_len)
{
    uint32_t ret = ERRCODE_FAIL;
    uint8_t read_back_data[CUSTOM_RESVED_EFUSE_BYTE_LEN] = {0};
    if ((data == NULL) || (data_len != CUSTOM_RESVED_EFUSE_BYTE_LEN)) {
        return ERRCODE_FAIL;
    }
    ret = efuse_read_item(EFUSE_CUSTOM_RESVED_ID, read_back_data, CUSTOM_RESVED_EFUSE_BYTE_LEN);
    if ((ret == ERRCODE_SUCC) && (check_data(read_back_data, CUSTOM_RESVED_EFUSE_BYTE_LEN) == ERRCODE_SUCC)) {
        ret = uapi_efuse_write_buffer(CUSTOM_RESVED_EFUSE_LEN_START_BYTE_ADDR, data, data_len);
        return ret;
    }
    return ERRCODE_FAIL;
}

uint32_t efuse_write_customer_rsvd2_efuse(uint8_t *data, uint16_t data_len)
{
    uint32_t ret = ERRCODE_FAIL;
    uint8_t read_back_data[CUSTOM_RESVED2_EFUSE_BYTE_LEN] = {0};
    if ((data == NULL) || (data_len != CUSTOM_RESVED2_EFUSE_BYTE_LEN)) {
        return ERRCODE_FAIL;
    }
    ret = uapi_efuse_read_buffer(read_back_data, CUSTOM_RESVED2_EFUSE_LEN_START_BYTE_ADDR,
        CUSTOM_RESVED2_EFUSE_BYTE_LEN);
    if ((ret == ERRCODE_SUCC) && (check_data(read_back_data, CUSTOM_RESVED2_EFUSE_BYTE_LEN) == ERRCODE_SUCC)) {
        ret = uapi_efuse_write_buffer(CUSTOM_RESVED2_EFUSE_LEN_START_BYTE_ADDR, data, data_len);
        return ret;
    } else {
        ret = uapi_efuse_read_buffer(read_back_data, CUSTOM_RESVED3_EFUSE_LEN_START_BYTE_ADDR,
            CUSTOM_RESVED2_EFUSE_BYTE_LEN);
        if ((ret == ERRCODE_SUCC) && (check_data(read_back_data, CUSTOM_RESVED2_EFUSE_BYTE_LEN) == ERRCODE_SUCC)) {
            ret = uapi_efuse_write_buffer(CUSTOM_RESVED3_EFUSE_LEN_START_BYTE_ADDR, data, data_len);
            return ret;
        }
    }
    return ERRCODE_FAIL;
}

uint32_t efuse_read_customer_rsvd2_efuse(uint8_t *data, uint16_t data_len)
{
    uint32_t ret = ERRCODE_FAIL;
    if ((data == NULL) || (data_len != CUSTOM_RESVED2_EFUSE_BYTE_LEN)) {
        return ERRCODE_FAIL;
    }
    ret = uapi_efuse_read_buffer(data, CUSTOM_RESVED3_EFUSE_LEN_START_BYTE_ADDR, CUSTOM_RESVED2_EFUSE_BYTE_LEN);
    if ((ret == ERRCODE_SUCC) && (check_data(data, CUSTOM_RESVED2_EFUSE_BYTE_LEN) != ERRCODE_SUCC)) {
        return ret;
    } else {
        ret = uapi_efuse_read_buffer(data, CUSTOM_RESVED2_EFUSE_LEN_START_BYTE_ADDR, CUSTOM_RESVED2_EFUSE_BYTE_LEN);
        if ((ret == ERRCODE_SUCC) && (check_data(data, CUSTOM_RESVED2_EFUSE_BYTE_LEN) != ERRCODE_SUCC)) {
            return ret;
        }
    }
    return ERRCODE_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_MFG_TEST
uint32_t efuse_write_jtag_ssi(void)
{
    uint32_t ret = ERRCODE_FAIL;

    ret = uapi_efuse_write_bit((EFUSE_SSI_MASK_START_BIT / BIT_TO_BYTE), (EFUSE_SSI_MASK_START_BIT % BIT_TO_BYTE));
    ret |= uapi_efuse_write_bit((EFUSE_FUNC_JTAG_MASK_START_BIT / BIT_TO_BYTE),
        (EFUSE_FUNC_JTAG_MASK_START_BIT % BIT_TO_BYTE));
    ret |= uapi_efuse_write_bit((EFUSE_DFT_JTAG_MASK_START_BIT / BIT_TO_BYTE),
        (EFUSE_DFT_JTAG_MASK_START_BIT % BIT_TO_BYTE));
    return ret;
}

uint32_t efuse_read_jtag_ssi(uint8_t *data, uint16_t data_len)
{
    uint32_t ret = ERRCODE_FAIL;
    uint8_t ssi_jtag = 0;

    if ((data == NULL) || (data_len != EFUSE_SIZE_1_BYTS)) {
        return ERRCODE_FAIL;
    }
    ret = uapi_efuse_read_buffer(&ssi_jtag, (EFUSE_SSI_MASK_START_BIT / BIT_TO_BYTE), data_len);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    if ((ssi_jtag & 0x0E) != 0 && (ssi_jtag & 0x0E) != 0x0E) {
        return ERRCODE_FAIL;
    }
    *data = (ssi_jtag == 0) ? 0 : 1;
    return ERRCODE_SUCC;
}

uint32_t efuse_write_hash_root_public_key(uint8_t *data, uint16_t data_len)
{
    uint32_t ret = ERRCODE_FAIL;

    if ((data == NULL) || (data_len != HASH_ROOT_PUBLIC_KEY_LEN)) {
        return ERRCODE_FAIL;
    }
    ret = uapi_efuse_write_buffer((EFUSE_HASH_ROOT_PUBLIC_KEY_START_BIT / BIT_TO_BYTE), data, data_len);
    return ret;
}

uint32_t efuse_read_hash_root_public_key(uint8_t *data, uint16_t data_len)
{
    uint32_t ret = ERRCODE_FAIL;

    if ((data == NULL) || (data_len != HASH_ROOT_PUBLIC_KEY_LEN)) {
        return ERRCODE_FAIL;
    }
    ret = uapi_efuse_read_buffer(data, (EFUSE_HASH_ROOT_PUBLIC_KEY_START_BIT / BIT_TO_BYTE), data_len);
    return ret;
}

uint32_t efuse_write_sec_verify(void)
{
    uint32_t ret = ERRCODE_FAIL;
    ret = uapi_efuse_write_bit((EFUSE_SEC_VERIFY_ENABLE_START_BIT / BIT_TO_BYTE),
        (EFUSE_SEC_VERIFY_ENABLE_START_BIT % BIT_TO_BYTE));
    return ret;
}

uint32_t efuse_read_sec_verify(uint8_t *data, uint16_t data_len)
{
    uint32_t ret = ERRCODE_FAIL;

    if ((data == NULL) || (data_len != EFUSE_SIZE_1_BYTS)) {
        return ERRCODE_FAIL;
    }
    ret = uapi_efuse_read_bit(data, (EFUSE_SEC_VERIFY_ENABLE_START_BIT / BIT_TO_BYTE),
        (EFUSE_SEC_VERIFY_ENABLE_START_BIT % BIT_TO_BYTE));
    return ret;
}
#endif
