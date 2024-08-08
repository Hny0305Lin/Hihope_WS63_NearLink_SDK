/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  LOG oam message recv MODULE
 * Author:
 * Create:
 */

#ifndef LOG_OAM_MSG_H
#define LOG_OAM_MSG_H

#include "product.h"
#include "chip_definitions.h"
#include "chip_core_definition.h"
/**
 * @defgroup connectivity_libs_log_reader LOG Reader
 * @ingroup  connectivity_libs
 * @{
 */
#if SYS_DEBUG_MODE_ENABLE == YES

#ifndef LIB_LOG_READER_LOG_OAM_MSG_H
#define LIB_LOG_READER_LOG_OAM_MSG_H
#include "string.h"
#ifdef SUPPORT_IPC
#include "ipc.h"
#endif
#if CORE != GNSS
#include "uart.h"
#endif

#define LOG_OML_HEADER_PRIME_ID_MASK  (0x0F)
#ifdef USE_GPIO_SIMULATE_SSI
#define LOG_OAM_RX_BUFF_LEN           (0x420)
#else
#define LOG_OAM_RX_BUFF_LEN           (300)
#endif
#define LOG_OAM_RX_BUFF_NUM           (2)
#define REGMEM_ADDR_AND_VALUE_MAX_LEN (2)

/**
 * @brief  Definitions of the operation result
 */
typedef enum {
    OAM_RET_OK,          // !< operation successful.
    OAM_ERR_POINT_NULL,  // !< operation fail as point to NULL.
    OAM_ERR_LEN_ERR,     // !< operation fail as the len is error.
    OAM_ERR_OP_ERR,      // !< operation fail as the operation field is error.
    OAM_ERR_SIZE_ERR,    // !< operation fail as the size is error.
    OAM_ERR_VALUE_ERR,   // !< operation fail as the value is error.
} oam_ret_t;

enum OM_LOG_CONFIG_RESULT_ENUM {
    OM_LOG_CONFIG_SUCCESS = 0,
    OM_LOG_CONFIG_FAIL = 1,

    OM_LOG_CONFIG_RESULT_BUTT,
};

enum OM_IF_TYPE_ENUM {
    OM_IF_UART = 0,
    OM_IF_SDIO = 1,

    OM_IF_TYPE_BUTT,
};

enum OM_TXQUEUE_NODE_STATUS {
    OM_TXQUEUE_NODE_INVALID,
    OM_TXQUEUE_NODE_VALID,
};

typedef enum {
    OML_BUFF_NOT_USED,
    OML_BUFF_USING,
    OML_BUFF_USED, /* There is already a complete oml command in this buff */
    OML_BUFF_BUTT,
} oml_buff_status_t;

typedef void (*ramfunc_oml_csstat_cmd)(uint8_t *, uint8_t);
typedef void (*oam_cmd_handle_callback)(uint8_t *, uint16_t);

typedef struct {
    uint8_t uc_buff_state;
    uint8_t uc_reserv;
    uint16_t s_buff_used_len;
    uint8_t auc_buff[LOG_OAM_RX_BUFF_LEN];
} oml_rx_data_stru_t;

typedef struct {
    uint8_t mode;
    uint8_t reg_mem_size;
    uint32_t reg_mem_addr_value[REGMEM_ADDR_AND_VALUE_MAX_LEN];
} rw_reg_mem_cmd_t;

/**
 * @brief  Parse the opetaion in the command.
 * @param  data The AT command from App core.
 * @param  len The length of the AT command.
 * @param  reg_mem_cmd The parse result stored here.
 * @return OAM_RET.
 */
uint32_t parse_reg_or_mem_cmd_operate(const uint8_t *data, uint32_t len, rw_reg_mem_cmd_t *reg_mem_cmd);

/**
 * @brief  Parse the value size in the command.
 * @param  data The AT command from App core.
 * @param  len The length of the AT command.
 * @param  reg_mem_cmd The parse result stored here.
 * @return OAM_RET.
 */
uint32_t parse_reg_or_mem_cmd_size(const uint8_t *data, uint32_t len, rw_reg_mem_cmd_t *reg_mem_cmd);

/**
 * @brief  Parse the addr and value in the command.
 * @param  data The AT command from App core.
 * @param  len The length of the AT command.
 * @param  reg_mem_cmd The parse result stored here.
 * @return OAM_RET.
 */
uint32_t parse_reg_or_mem_cmd_addr_value(const uint8_t *data, uint32_t len, rw_reg_mem_cmd_t *reg_mem_cmd);

#ifdef SUPPORT_IPC
/**
 * @brief  The handler of IPC_ACTION_AP_BT_HOOK_DATA
 * @param  message The ipc_action_t of the received message.
 * @param  payload_p Data payload of the received mssage. Note that this is the uinion, not the individual memeber.
 * @param  src Sending core.
 * @param  id Internal message id. Can be used to detect duplicate delivery and for debug.
 * @return True if the incoming mailslot shluld be released or
 *         false if the message was not (fully) processed and should be redelivered.
 */
bool ap_bt_hook_data_action_handler(ipc_action_t message,
                                    const volatile ipc_payload *payload_p, cores_t src, uint32_t id);
#endif

#ifdef SUPPORT_IPC
/**
 * @brief  Send the result of start hooking to app core
 * @param  hook_type The type of hook that should be used by app core.
 * @param  addr The address that want to dump.
 * @param  len The length of the memory want to dump.
 * @param  value The value of the register or memory want to read.
 * @return None
 */
void send_bt_ap_hook_data(ipc_bt_ap_hook_type_e hook_type, uint32_t addr, uint32_t len, uint32_t value);
#endif
#if CORE != GNSS
void log_uart_rx_callback(const void *buffer, uint16_t length, bool remaining);
#endif
bool log_oam_register_handler_callback(uint8_t message_type, oam_cmd_handle_callback callback);
void log_oam_prase_message(void);
void oml_msg_parse(uint8_t *data, uint16_t data_len);
#ifdef SUPPORT_IPC
bool get_hci_data_action_handler(ipc_action_t message,
                                 const volatile ipc_payload *payload_p, cores_t src, uint32_t id);
#endif

#endif

#else  /* SYS_DEBUG_MODE_ENABLE == NO */

#ifndef LIB_LOG_READER_LOG_OAM_MSG_H
#define LIB_LOG_READER_LOG_OAM_MSG_H
// This branch should be deleted after the code of BTC has been finished.
#include <stdint.h>
#include <stdbool.h>
#include "log_oam_msg.h"
#ifdef SUPPORT_IPC
#include "ipc_actions.h"
#include "ipc.h"
#endif

#if (defined CONFIG_DFX_SUPPORT_CUSTOM_LOG) && (CONFIG_DFX_SUPPORT_CUSTOM_LOG == DFX_YES)
#include "dfx_adapt_typedef.h"
#endif

typedef void (*oam_cmd_handle_callback)(uint8_t *, uint16_t);

/**
 * @brief  Send the result of start hooking to app core
 * @param  hook_type The type of hook that should be used by app core.
 * @param  addr The address that want to dump.
 * @param  len The length of the memory want to dump.
 * @param  value The value of the register or memory want to read.
 * @return None
 */

#ifdef SUPPORT_IPC
void send_bt_ap_hook_data(ipc_bt_ap_hook_type_e hook_type, uint32_t addr, uint32_t len, uint32_t value);
bool get_hci_data_action_handler(ipc_action_t message,
                                 const volatile ipc_payload *payload_p, cores_t src, uint32_t id);
#endif
bool log_oam_register_handler_callback(uint8_t message_type, oam_cmd_handle_callback callback);
#endif

#endif

/**
 * @}
 */
#endif