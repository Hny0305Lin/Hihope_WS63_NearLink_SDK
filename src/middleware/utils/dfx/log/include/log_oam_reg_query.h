/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: LOG oam reg query MODULE
 * Author:
 * Create:
 */

#ifndef LOG_OAM_REG_QUERY_H
#define LOG_OAM_REG_QUERY_H
#include "product.h"

#if SYS_DEBUG_MODE_ENABLE == YES

#ifndef LIB_LOG_READER_LOG_OAM_REG_QUERY_H
#define LIB_LOG_READER_LOG_OAM_REG_QUERY_H

#include "log_oam_logger.h"
#ifdef SUPPORT_IPC
#include "ipc_actions.h"
#endif

#if USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO
typedef struct {
    om_msg_header_stru_t header;
    uint32_t reg_addr;
    uint16_t reg_len;
    uint8_t mode;
    uint8_t reserver;
    uint32_t reg_value;
    uint8_t msg_tailer;
} __attribute__((packed)) om_reg_data_stru_t;
#endif

#if MCU_ONLY
#ifdef USE_GPIO_SIMULATE_SSI
typedef struct {
    om_msg_header_stru_t  header;
    uint8_t reg_num;
    uint8_t mode;
    uint16_t reserver;
}om_msg_ssi_header_t;

typedef struct {
    uint32_t reg_addr;
    uint32_t reg_value;
} om_reg_addr_val_t;

typedef struct {
    om_msg_header_stru_t header;
    uint8_t reg_num;
    uint8_t mode;
    uint8_t reg_result;
    uint8_t reserver;
    uint8_t msg_tailer;
} __attribute__((packed)) om_msg_ssi_write_rport_t;

typedef struct {
    om_msg_header_stru_t header;
    uint8_t reg_num;
    uint8_t mode;
    uint8_t query_reg_result;
    uint8_t reserver;
    uint8_t msg_tail;
} __attribute__((packed)) om_msg_ssi_read_rport_t;

typedef struct {
    om_msg_header_stru_t header;
    uint32_t addr;
    uint16_t length;
    uint8_t operate_mode;
    uint8_t operate_result;
    uint8_t msg_tailer;
} om_ssi_block_stru_t;
#else
typedef struct {
    om_msg_header_stru_t header;
    uint32_t reg_addr;
    uint16_t output_value;
    uint8_t mode;         // unused
    uint8_t reserver;     // unused
    uint32_t reg_value;    // unused
    uint8_t msg_tailer;
} __attribute__((packed)) om_ssi_reg_output_stru_t;

struct om_rdssi_reg_data_stru_t {
    om_msg_header_stru_t header;
    uint32_t unused_bit8;
    uint32_t reg_addr;
    uint8_t msg_tailer;
};

struct om_wrssi_reg_data_stru_t {
    om_msg_header_stru_t header;
    uint32_t unused_bit8;
    uint32_t reg_addr;
    uint32_t reg_value;
    uint8_t msg_tailer;
};
#endif
#endif

enum REG_QUERY_ACTION {
    REG_ACTION_WRITE,
    REG_ACTION_READ,
};

void oml_reg_command(uint8_t *data, uint16_t length);
void oml_ssi_reg_command(uint8_t *data, uint16_t length);

#if CORE == MASTER_BY_ALL
void oml_reg_register_callback(void);
#endif

#if MCU_ONLY
#ifdef USE_GPIO_SIMULATE_SSI
void oml_ssi_block_callback(void);
void oml_ssi_reg32_register_callback(void);
#endif
void oml_ssi_reg_register_callback(void);
#endif

#endif /* end of log_oam_reg_query.h */

#endif  /* end of SYS_DEBUG_MODE_ENABLE == YES */
#endif