/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Provides efuse port template
 *
 * Create: 2023-03-04
 */
#ifndef EFUSE_PORTING_H
#define EFUSE_PORTING_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_efuse Efuse
 * @ingroup  drivers_port
 * @{
 */

#define EFUSE_REGION_NUM               1
#define EFUSE_REGION_MAX_BITS          2048
#define EFUSE_REGION_MAX_BYTES         (EFUSE_REGION_MAX_BITS >> 3)  // MAX_BIT / 8
#define EFUSE_MAX_BITS                 (EFUSE_REGION_MAX_BITS * EFUSE_REGION_NUM)
#define EFUSE_MAX_BYTES                (EFUSE_MAX_BITS >> 3)  // MAX_BIT / 8
#define EFUSE_MAX_BIT_POS              8U

#define EFUSE_PMU_CLKLDO_VSET_MASK       0x78
#define EFUSE_PMU_CLKLDO_OFFSET          3
#define EFUSE_TRIM_VERSION_DEFAULT_VALUE 1
#define EFUSE_CHAR_BIT_WIDE              8
#define EFUSE_CALC_CRC_MAX_LEN          32
#define EFUSE_DIE_ID_BASE_BYTE_ADDR      0

#define EFUSE_TRIM_FLAG    50
#define EFUSE_IS_CHIP_TRIMED 0
// 7-0 -> Version of the trim
#define EFUSE_TRIM_VERSION 51
// 2-0 -> vset_bbldo[3:1]
// 6-3 -> vset_clkldo[3:0]
#define EFUSE_PMU_TRIM_VSET 25

typedef enum {
    HAL_EFUSE_REGION_0,
    HAL_EFUSE_REGION_MAX,
} hal_efuse_region_t;

#define EFUSE_IDX_NRW 0x0
#define EFUSE_IDX_RO  0x1
#define EFUSE_IDX_WO  0x2
#define EFUSE_IDX_RW  0x3

#define EFUSE_FLASHBOOT_ROLLBACK_VERSION 656
#define EFUSE_APP_ROLLBACK_VERSION 624
#define MAC_ADDR_0_EFUSE_START_BIT 1584
#define MAC_ADDR_1_EFUSE_START_BIT 1632
#define MAC_ADDR_2_EFUSE_START_BIT 1680
#define MAC_ADDR_LAST_EFUSE_START_BIT 1728
#define EFUSE_MAC_NUM 4
#define EFUSE_SIZE_1_BYTS 1
#define HASH_ROOT_PUBLIC_KEY_LEN 32
#define EFUSE_SSI_MASK_START_BIT 193
#define EFUSE_FUNC_JTAG_MASK_START_BIT 194
#define EFUSE_DFT_JTAG_MASK_START_BIT 195
#define EFUSE_HASH_ROOT_PUBLIC_KEY_START_BIT 672
#define EFUSE_SEC_VERIFY_ENABLE_START_BIT 960

#define CUSTOM_RESVED_EFUSE_BYTE_LEN 16
#define CUSTOM_RESVED_EFUSE_LEN_START_BYTE_ADDR 222

#define CUSTOM_RESVED2_EFUSE_BYTE_LEN 16
#define CUSTOM_RESVED2_EFUSE_LEN_START_BYTE_ADDR 84
#define CUSTOM_RESVED3_EFUSE_LEN_START_BYTE_ADDR 100

#define BURN_EFUSE_IDX_MAX           115
#define EFUSE_BIT_NUM 2048
#define EFUSE_READ_MAX_BYTE 32
#define BURN_EFUSE_BIN_ADDR 0xA30000
#define EFUSE_CFG_MAX_LEN       1320
#define EFUSE_CFG_MIN_LEN       48
#define CUSTOMER_RSVD_EFUSE_START_BIT  1776
#define CUSTOMER_RSVD_EFUSE_BIT_LEN    128

typedef enum {
    EFUSE_CHIP_ID = 0,
    EFUSE_DIE_ID = 1,
    EFUSE_MCU_VER_ID = 2,
    EFUSE_FLASHBOOT_VER_ID = 3,
    EFUSE_PARAMS_VER_ID = 4,
    EFUSE_HASH_ROOT_PUBLIC_KEY_ID = 5,
    EFUSE_MSID_ID = 6,
    EFUSE_SEC_VERIFY_ENABLE = 7,
    EFUSE_TYPE_ID = 8,
    EFUSE_MAC_ADDR_01_ID = 9,
    EFUSE_MAC_ADDR_02_ID = 10,
    EFUSE_MAC_ADDR_03_ID = 11,
    EFUSE_MAC_ADDR_LAST_ID = 12,
    EFUSE_CUSTOM_RESVED_ID = 13,
    EFUSE_CUSTOM_RESVED2_ID = 14,
    EFUSE_GET_S_ID = 15,
    EFUSE_GET_B_ID = 16,
    EFUSE_GET_K_ID = 17,
    EFUSE_IDX_MAX,
} efuse_idx;

/**
 * @if Eng
 * @brief  Base address for the EFUSE boot done.
 * @else
 * @brief  EFUSE的上电完成地址
 * @endif
 */
extern uint32_t g_efuse_boot_done_addr;

/**
 * @if Eng
 * @brief  Base address for the IP.
 * @else
 * @brief  IP的基地址
 * @endif
 */
extern uint32_t g_efuse_base_addr[1];

/**
 * @if Eng
 * @brief  Base read address for the IP.
 * @else
 * @brief  IP的读基地址
 * @endif
 */
extern uint32_t g_efuse_region_read_address[1];

/**
 * @if Eng
 * @brief  Base write address for the IP.
 * @else
 * @brief  IP的写基地址
 * @endif
 */
extern uint32_t g_efuse_region_write_address[1];

/**
 * @if Eng
 * @brief  Register hal funcs objects into hal_efuse module.
 * @else
 * @brief  将hal funcs对象注册到hal_efuse模块中
 * @endif
 */
void efuse_port_register_hal_funcs(void);

/**
 * @if Eng
 * @brief  Unregister hal funcs objects from hal_efuse module.
 * @else
 * @brief  从hal_efuse模块注销hal funcs对象
 * @endif
 */
void efuse_port_unregister_hal_funcs(void);

/**
 * @brief  Get the region of a otp byte address
 * @param  byte_addr the addr of the byte to get register
 * @retval region The region of otp
 * @else
 * @brief  获取otp字节地址的区域
 * @param  byte_addr 要获取寄存器的字节的地址
 * @retval 区域OTP的区域
 */
hal_efuse_region_t hal_efuse_get_region(uint32_t byte_addr);

/**
 * @brief  Get the offset addr of a otp byte address
 * @param  byte_addr the addr of the byte to get register
 * @retval address
 * @else
 * @brief  获取otp字节地址的偏移地址
 * @param  byte_addr 要获取寄存器的字节的地址
 * @retval 偏移地址
 */
uint16_t hal_efuse_get_byte_offset(uint32_t byte_addr);

/**
 * @brief  Get the value from efuse
 * @param  efuse_idx the index of the byte to get in map
 * @param  data the value of the byte to get
 * @param  data_len the length of the byte to get
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  从efuse中读取指定字节
 * @param  efuse_idx 要获取字节在表中的序号
 * @param  data 要获取的字节返回的值
 * @param  data_len 要获取的字节的长度
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 */
uint32_t efuse_read_item(efuse_idx efuse_id, uint8_t *data, uint16_t data_len);

void set_efuse_period(void);

uint32_t efuse_write_mac(uint8_t *data, uint16_t data_len);
uint32_t efuse_read_mac(uint8_t *data, uint16_t data_len, uint8_t *left_time);
uint32_t efuse_write_sle_mac(uint8_t *data, uint16_t data_len);
uint32_t efuse_read_sle_mac(uint8_t *data, uint16_t data_len);
uint32_t efuse_write_customer_rsvd_efuse(uint8_t *data, uint16_t data_len);
uint32_t efuse_write_customer_rsvd2_efuse(uint8_t *data, uint16_t data_len);
uint32_t efuse_read_customer_rsvd2_efuse(uint8_t *data, uint16_t data_len);
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
uint32_t efuse_write_jtag_ssi(void);
uint32_t efuse_read_jtag_ssi(uint8_t *data, uint16_t data_len);
uint32_t efuse_write_hash_root_public_key(uint8_t *data, uint16_t data_len);
uint32_t efuse_read_hash_root_public_key(uint8_t *data, uint16_t data_len);
uint32_t efuse_write_sec_verify(void);
uint32_t efuse_read_sec_verify(uint8_t *data, uint16_t data_len);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
