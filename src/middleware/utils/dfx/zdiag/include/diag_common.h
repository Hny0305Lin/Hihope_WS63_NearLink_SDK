/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag common header file
 * This file should be changed only infrequently and with great care.
 */

#ifndef __ZDIAG_COMMON_H__
#define __ZDIAG_COMMON_H__

#include "errcode.h"
#include "diag_config.h"

#define MUX_START_FLAG 0xDEADBEEF
#define MUX_PKT_VER 0xfd

typedef enum {
    MUX_PKT_CMD = 0,
    MUX_PKT_IND = 1,
    MUX_PKT_ACK = 3,
} mux_pkt_type_t;

typedef enum {
    DIAG_PKT_PROC_USR_ASYNC_CMD_IND,
    DIAG_PKT_PROC_USR_SYNC_CMD_IND,
    DIAG_PKT_PROC_PORT_PKT,
    DIAG_PKT_PPOC_LOCAL_EXEC_Q,
    DIAG_PKT_PPOC_OUT_PUT_Q,
} diag_pkt_proc_t;

typedef enum {
    DIAG_ADDR_ATTRIBUTE_VALID = 0x1,
    DIAG_ADDR_ATTRIBUTE_HCC = 0x2,
    DIAG_ADDR_ATTRIBUTE_HSO_CONNECT = 0x4,
} diag_addr_attribute_t;

typedef struct {
    uint32_t module;   /* source module ID */
    uint32_t dest_mod; /* destination module ID */
    uint32_t no;       /* No. */
    uint32_t id;       /* ID */
    uint32_t time;
    int8_t data[0]; /* User Data Cache */
} diag_cmd_log_layer_ind_stru_t;
#define DIAG_FULL_LOG_HEADER_SIZE sizeof(diag_cmd_log_layer_ind_stru_t)

typedef struct {
    uint16_t sn;    /* cmd sn */
    uint16_t crc16; /* crc16 */

    uint16_t cmd_id;     /* cmd id */
    uint16_t param_size; /* param size */
    uint8_t param[0];    /* data */
} msp_diag_head_req_stru_t;
#define DIAG_REQ_HEADER_SIZE sizeof(msp_diag_head_req_stru_t)

typedef struct {
    uint16_t cmd_id;     /* cmd id */
    uint16_t param_size; /* param size */
    uint8_t param[0];    /* data */
} msp_diag_head_ind_stru_t;
#define DIAG_IND_HEADER_SIZE sizeof(msp_diag_head_ind_stru_t)

typedef struct {
    uint16_t sn;  /* cmd sn */
    uint8_t ctrl; /* auto ack:0xfe,initiative ack:0 */
    uint8_t pad;
    uint16_t cmd_id;     /* cmd id */
    uint16_t param_size; /* param size */
    uint8_t param[0];    /* data */
} msp_diag_head_cnf_stru_t;
#define DIAG_CNF_HEADER_SIZE sizeof(msp_diag_head_cnf_stru_t)

typedef struct {
    uint32_t start_flag; /* start flag fixed 0xDEADBEAF */
    uint32_t au_id;      /* MSP AUID. */
    uint8_t type;        /* ind:0,ack:2,system ack:3 */
    uint8_t ver;         /* diag cmd version,0xff:old version,0xfe:crc16version,0xfd:nb version */
    uint16_t cmd_id;     /* mark unencrypted cmd id only 0x5000,0x5002,0x5003 */

    uint16_t crc16; /* crc16 */

    uint8_t dst;
    uint8_t src;

    uint8_t ctrl;
    uint8_t pad[1];
    uint16_t packet_data_size; /* puc_packet_data size, not include sizeof(MSP_MUX_PACKET_HEAD_STRU). */

    uint8_t puc_packet_data[0]; /* data:Encrypted means encrypted data, otherwise plain text */
} msp_mux_packet_head_stru_t;
#define DIAG_MUX_HEADER_SIZE sizeof(msp_mux_packet_head_stru_t)

#endif
