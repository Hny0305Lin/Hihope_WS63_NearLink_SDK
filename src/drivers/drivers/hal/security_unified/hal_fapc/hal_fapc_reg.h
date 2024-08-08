/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: hal fapc register header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef HAL_FAPC_REG_H
#define HAL_FAPC_REG_H

#include "td_type.h"

typedef union {
    struct {
        td_u32 reserved_0 : 8;
        td_u32 flash_apc_addr : 24;
    } bits;
    td_u32 u32;
} fapc_addr_type;

typedef union {
    struct {
        td_u32 alias_cs : 1;    // [0] alias option, 0: alias_cs0, 1: alias_cs1
        td_u32 reserved : 31;
    } bits;
    td_u32 u32;
} fapc_alias_option;

typedef struct {
    td_u32 flash_size_option_bit;
    td_u32 flash_size_bit;
} flash_size_option;


typedef union {
    struct {
        td_u32 flash_size_cs0 : 4;     // [0:3] alias_cs0_size
        td_u32 reserved_0 : 4;         // [4:7] reserved
        td_u32 flash_size_cs1 : 4;     // [8:11] alias_cs1_size
        td_u32 reserved_1 : 20;        // [12:31] reserved
    } bits;
    td_u32 u32;
} fapc_alias_size;

#define FAPC_CFG_START_ADDR(n)    (0x1000 + 4 * (n))
#define FAPC_CFG_END_ADDR(n)      (0x1040 + 4 * (n))
#define FAPC_CFG_ALLOW_RD_ID(n)   (0x1080 + 4 * (n))
#define FAPC_CFG_ALLOW_WR_ID(n)   (0x10c0 + 4 * (n))
#define FAPC_CFG_APC_EN(n)        (0x1100 + 4 * (n))
#define FAPC_AUTH_MAC_SADDR(n)    (0x1140 + 4 * (n))
#define FAPC_DEC_AUTH_CFG           0x1180
#define FAPC_ONE_WAY_LOCK           0x1220
#define FAPC_IV_VALID               0x1600
#define FAPC_ACORE_IV_BASE_ADDR     0x1620
#define FAPC_PCORE_IV_BASE_ADDR     0x1630
#define FAPC_SCORE_IV_BASE_ADDR     0x1810

#define FAPC_IV_ACPU_START_ADDR(n)  (0x1640 + 4 * (n))

#define FLASH_BUS_FLASH_SIZE        0x0210
#define FLASH_BUS_BASE_ADDR_CS0     0x0214
#define FLASH_BUS_BASE_ADDR_CS1     0x0218
#define FLASH_BUS_ALIAS_CS          0x0220

#define FLASH_ADDR_MASK           0x000000ff
#define FAPC_MODE_NUM             3
#define FAPC_REGION_NUM           4
#define FAPC_ENABLE_FLAG          0x05   // Any value other than 0x0a means to enable FAPC
#define FAPC_DISABLE_FLAG         0x0a
#define FAPC_ENABLE_MASK          0x0000000f
#define FAPC_UNLOCK               0
#define FAPC_IV_SIZE              16
#define IV_MAX_SIZE_IN_WORD       4

#define FLASH_SIZE_256K_BIT 0x8000      // base size, SFC alias size default value is 0x1000000
#define FLASH_SIZE_OPTION_NO_CONNECT 0x0    // SPI FLASH not connect, SFC alias option default value is 0x9
#endif