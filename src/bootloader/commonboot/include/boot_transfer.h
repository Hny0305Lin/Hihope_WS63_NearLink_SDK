/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_TRANSFER_H
#define BOOT_TRANSFER_H

#include <stdint.h>
#include "securec.h"

#define MODEM_SOH                   0x01 /* Start character of data block */
#define MODEM_STX                   0x02 /* The start byte of 1024 bytes */
#define MODEM_EOT                   0x04 /* File transfer finish */
#define MODEM_ACK                   0x06 /* ACK message */
#define MODEM_NAK                   0x15 /* Error occurred */
#define MODEM_CAN                   0x18 /* Cancel transmission */
#define MODEM_EOF                   0x1A /* Data blank filler */
#define MODEM_C                     0x43 /* Capital letter C */

#define FLASH_ADDR_OFFSET           FLASH_START

#define UPLOAD_WAIT_START_C_TIME    10000000 /* 10s */
#define UPLOAD_WAIT_DEFAULT_TIME    2000000  /* 2s */

#define UPLOAD_DATA_SIZE            1024
#define UPLOAD_BUFF_LEN             1029

#define RETRY_COUNT                 0
#define CAN_COUNT                   3
#define MSG_START_LEN               3
#define SOH_MSG_LEN                 128
#define SOH_MSG_TOTAL_LEN           133
#define WAIT_ZERO_ACK_DELAY         100
#define UPLOAD_FILE_NAME            "upload.bin"
#define UPLOAD_FILE_NAME_LEN        11

enum {
    UPLOAD_NONE,
    UPLOAD_WAIT_START_C,
    UPLOAD_WAIT_INIT_ACK,
    UPLOAD_WAIT_TRANS_C,
    UPLOAD_WAIT_INTER_ACK,
    UPLOAD_WAIT_FINAL_ACK,
    UPLOAD_WAIT_EOT_C,
    UPLOAD_WAIT_ZERO_ACK,
};

typedef struct {
    uintptr_t file_addr;
    uint32_t file_length;
    char *file_name;
    uint32_t offset;
    uint8_t status;
    uint8_t seq;
    uint8_t retry : 4;
    uint8_t can_cnt : 4;
    uint8_t buffer[UPLOAD_BUFF_LEN];
} upload_context;

uint32_t download_image(uint32_t addr, uint32_t erase_size, uint32_t flash_size, uint8_t burn_efuse);
uint32_t download_factory_image(uint32_t addr, uint32_t erase_size, uint32_t flash_size, uint8_t burn_efuse);
uint32_t upload_data(uint32_t addr, uint32_t length);

#endif