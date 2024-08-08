/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  LOG oam message MODULE
 * Author:
 * Create:
 */

#include "product.h"
#include "non_os.h"
#include "dfx_feature_config.h"
#if SYS_DEBUG_MODE_ENABLE == YES
#include "debug_print.h"
#include "log_printf.h"
#include "log_def.h"
#include "securec.h"
#include "log_oam_reg_query.h"
#include "log_oam_mem_query.h"
#include "oal_interface.h"
#include "log_oam_msg.h"
#if CORE != CONTROL_CORE
#ifdef HSO_SUPPORT
#include "dfx_channel.h"
#endif
#endif

#if (defined CONFIG_DFX_SUPPORT_CUSTOM_LOG) && (CONFIG_DFX_SUPPORT_CUSTOM_LOG == DFX_YES)
#include "dfx_adapt_typedef.h"
#endif

#define BT_DFT_RETURN_NEWLINE_LEN     2
#define BT_DFT_CMD_COMMA_LEN          1
#define BT_DFT_CMD_WR_INDEX           0
#define BT_DFT_CMD_WR_LEN             1
#define BT_DFT_CMD_REGMEM_SIZE_INDEX  (BT_DFT_CMD_WR_INDEX + BT_DFT_CMD_WR_LEN + BT_DFT_CMD_COMMA_LEN)
#define BT_DFT_CMD_REGMEM_SIZE_LEN    1
#define BT_DFT_CMD_REGMEM_ADDR_INDEX  (BT_DFT_CMD_REGMEM_SIZE_INDEX + BT_DFT_CMD_REGMEM_SIZE_LEN + BT_DFT_CMD_COMMA_LEN)
#define BT_DFT_CMD_REGMEM_ADDR_LEN    8
#define BT_DFT_CMD_REGMEM_VALUE_INDEX (BT_DFT_CMD_REGMEM_ADDR_INDEX + BT_DFT_CMD_REGMEM_ADDR_LEN + BT_DFT_CMD_COMMA_LEN)

#ifndef HSO_SUPPORT
static oml_rx_data_stru_t g_log_oam_rx_buffer[LOG_OAM_RX_BUFF_NUM];
#endif
static oam_cmd_handle_callback g_log_oam_log_config_callback = NULL;
static oam_cmd_handle_callback g_log_oml_mem_command_callback = NULL;
static oam_cmd_handle_callback g_log_oml_reg_command_callback = NULL;
static oam_cmd_handle_callback g_log_oml_ssi_reg_command_callback = NULL;
static oam_cmd_handle_callback g_log_oml_msg_cs_stat_callback = NULL;
static oam_cmd_handle_callback g_log_oml_btc_oam_callback = NULL;
static oam_cmd_handle_callback g_log_oml_bth_oam_callback = NULL;
static oam_cmd_handle_callback g_log_oml_app_oam_callback = NULL;
static oam_cmd_handle_callback g_log_oml_hifi_oam_callback = NULL;
static oam_cmd_handle_callback g_log_oml_reg32_command_callback = NULL;
static oam_cmd_handle_callback g_log_oml_ssi_block_callback = NULL;

static uint8_t convert_hex_str_2_uint8(uint8_t data)
{
    uint8_t result;

    if ((data >= '0') && (data <= '9')) {
        result = data - '0';
    } else if ((data >= 'A') && (data <= 'F')) {
        result = data - 'A' + 10;  // Convert A-F to 10-15
    } else {
        result = 0;
    }

    return result;
}

static uint8_t get_uint8_from_at_cmd(const uint8_t *data, uint32_t data_index)
{
    uint8_t result = 0;

    result += convert_hex_str_2_uint8(data[data_index + 0]) << 4; /* Convert str[0] to bit[7:4] of uint8_t */
    result += convert_hex_str_2_uint8(data[data_index + 1]) << 0; // Convert str[1] to bit[3:0] of uint8_t

    return result;
}

static uint16_t get_uint16_from_at_cmd(const uint8_t *data, uint32_t data_index)
{
    uint16_t result = 0;

    result += convert_hex_str_2_uint8(data[data_index + 0]) << 12; /* Convert str[0] to bit[15:12] of uint16 */
    result += convert_hex_str_2_uint8(data[data_index + 1]) << 8;  // Convert str[1] to bit[11:8] of uint16
    result += convert_hex_str_2_uint8(data[data_index + 2]) << 4;  // Convert str[2] to bit[7:4] of uint16
    result += convert_hex_str_2_uint8(data[data_index + 3]) << 0;  // Convert str[3] to bit[3:0] of uint16

    return result;
}

static uint32_t get_uint32_from_at_cmd(const uint8_t *data, uint32_t data_index)
{
    uint32_t result = 0;

    result += convert_hex_str_2_uint8(data[data_index + 0]) << 28; /* Convert str[0] to bit[31:28] of uint32 */
    result += convert_hex_str_2_uint8(data[data_index + 1]) << 24; /* Convert str[1] to bit[27:24] of uint32 */
    result += convert_hex_str_2_uint8(data[data_index + 2]) << 20; // Convert str[2] to bit[23:20] of uint32
    result += convert_hex_str_2_uint8(data[data_index + 3]) << 16; // Convert str[3] to bit[19:16] of uint32
    result += convert_hex_str_2_uint8(data[data_index + 4]) << 12; // Convert str[4] to bit[15:12] of uint32
    result += convert_hex_str_2_uint8(data[data_index + 5]) << 8;  // Convert str[5] to bit[11:8] of uint32
    result += convert_hex_str_2_uint8(data[data_index + 6]) << 4;  // Convert str[6] to bit[7:4] of uint32
    result += convert_hex_str_2_uint8(data[data_index + 7]) << 0;  // Convert str[7] to bit[3:0] of uint32

    return result;
}

#if (CORE != GNSS) && (CORE != CONTROL_CORE)
#ifndef HSO_SUPPORT
static bool log_oam_store_buffer(const void *rx_buffer, uint16_t rx_buffer_length)
{
    uint8_t loop;
    uint8_t *cur_addr = NULL;
    errno_t sec_ret;

    for (loop = 0; loop < LOG_OAM_RX_BUFF_NUM; loop++) {
        // If there is a message, exit and check weather is data to store.
        if (g_log_oam_rx_buffer[loop].uc_buff_state == OML_BUFF_USED) {
            continue;
        }
        // Current buffer have space to store the data
        if (rx_buffer_length <= (LOG_OAM_RX_BUFF_LEN - g_log_oam_rx_buffer[loop].s_buff_used_len)) {
            cur_addr = g_log_oam_rx_buffer[loop].auc_buff + g_log_oam_rx_buffer[loop].s_buff_used_len;
            sec_ret = memcpy_s((void *)cur_addr, LOG_OAM_RX_BUFF_LEN - g_log_oam_rx_buffer[loop].s_buff_used_len,
                               rx_buffer, rx_buffer_length);
            if (sec_ret != EOK) {
                return false;
            }
            g_log_oam_rx_buffer[loop].s_buff_used_len += rx_buffer_length;
            g_log_oam_rx_buffer[loop].uc_buff_state = OML_BUFF_USING;
            return true;
        } else {
            g_log_oam_rx_buffer[loop].s_buff_used_len = 0;

            // Clean the buffer and check again.
            if (rx_buffer_length > (LOG_OAM_RX_BUFF_LEN - g_log_oam_rx_buffer[loop].s_buff_used_len)) {
                return false;
            }
            cur_addr = g_log_oam_rx_buffer[loop].auc_buff + g_log_oam_rx_buffer[loop].s_buff_used_len;
            sec_ret = memcpy_s((void *)cur_addr, LOG_OAM_RX_BUFF_LEN - g_log_oam_rx_buffer[loop].s_buff_used_len,
                               rx_buffer, rx_buffer_length);
            if (sec_ret != EOK) {
                return false;
            }
            g_log_oam_rx_buffer[loop].s_buff_used_len += rx_buffer_length;
            g_log_oam_rx_buffer[loop].uc_buff_state = OML_BUFF_USING;
            return true;
        }
    }
    return false;
}

static void log_recv_data_handle(void)
{
    uint8_t loop;
    om_msg_header_stru_t *pst_oml_header = NULL;
    non_os_enter_critical();
    for (loop = 0; loop < LOG_OAM_RX_BUFF_NUM; loop++) {
        // If this buff is already a complete message
        if ((g_log_oam_rx_buffer[loop].uc_buff_state == OML_BUFF_USING) &&
            (g_log_oam_rx_buffer[loop].s_buff_used_len > OM_FRAME_HEADER_LEN)) {
            pst_oml_header = (om_msg_header_stru_t *)(g_log_oam_rx_buffer[loop].auc_buff);
            if ((pst_oml_header->frame_start != OM_FRAME_DELIMITER) ||
                (pst_oml_header->func_type >= OM_MSG_TYPE_BUTT)) {
                g_log_oam_rx_buffer[loop].s_buff_used_len = 0;
                g_log_oam_rx_buffer[loop].uc_buff_state = OML_BUFF_NOT_USED;
                non_os_exit_critical();
                return;
            }
            // Check received data length
            if (pst_oml_header->frame_len > g_log_oam_rx_buffer[loop].s_buff_used_len) {
                continue;
            }
            // Check end data
            if (g_log_oam_rx_buffer[loop].auc_buff[pst_oml_header->frame_len - 1] == OM_FRAME_DELIMITER) {
                g_log_oam_rx_buffer[loop].uc_buff_state = OML_BUFF_USED;
                non_os_exit_critical();
                return;
            } else {
                g_log_oam_rx_buffer[loop].s_buff_used_len = 0;
                g_log_oam_rx_buffer[loop].uc_buff_state = OML_BUFF_NOT_USED;
            }
        }
    }
    non_os_exit_critical();
}
#endif
#endif

uint32_t parse_reg_or_mem_cmd_operate(const uint8_t *data, uint32_t len, rw_reg_mem_cmd_t *reg_mem_cmd)
{
    if (reg_mem_cmd == NULL || data == NULL) {
        return OAM_ERR_POINT_NULL;
    }

    UNUSED(len);

    /* Step1: Get the operation */
    if (data[BT_DFT_CMD_WR_INDEX] == 'R') {
        reg_mem_cmd->mode = REG_ACTION_READ;
    } else if (data[BT_DFT_CMD_WR_INDEX] == 'W') {
        reg_mem_cmd->mode = REG_ACTION_WRITE;
    } else if (data[BT_DFT_CMD_WR_INDEX] == 'B') {
        reg_mem_cmd->mode = MEM_ACTION_READ_BLOCK;
    } else {
        return OAM_ERR_OP_ERR;
    }

    return OAM_RET_OK;
}

uint32_t parse_reg_or_mem_cmd_size(const uint8_t *data, uint32_t len, rw_reg_mem_cmd_t *reg_mem_cmd)
{
    if (reg_mem_cmd == NULL) {
        return OAM_ERR_POINT_NULL;
    }

    UNUSED(len);

    /* Step2: Get the reg or mem size */
    if (data[BT_DFT_CMD_REGMEM_SIZE_INDEX] == 'L') {
        reg_mem_cmd->reg_mem_size = (uint8_t)sizeof(uint32_t);
    } else if (data[BT_DFT_CMD_REGMEM_SIZE_INDEX] == 'S') {
        reg_mem_cmd->reg_mem_size = (uint8_t)sizeof(uint16_t);
    } else if (data[BT_DFT_CMD_REGMEM_SIZE_INDEX] == 'U') {
        reg_mem_cmd->reg_mem_size = (uint8_t)sizeof(uint8_t);
    } else {
        return OAM_ERR_SIZE_ERR;
    }

    return OAM_RET_OK;
}

uint32_t parse_reg_or_mem_cmd_addr_value(const uint8_t *data, uint32_t len, rw_reg_mem_cmd_t *reg_mem_cmd)
{
    if (reg_mem_cmd == NULL) {
        return OAM_ERR_POINT_NULL;
    }

    UNUSED(len);

    /* Step3: Get the register address */
    reg_mem_cmd->reg_mem_addr_value[0] = get_uint32_from_at_cmd(data, BT_DFT_CMD_REGMEM_ADDR_INDEX);

    /* Step4: Get the value */
    if (reg_mem_cmd->reg_mem_size == sizeof(uint8_t)) {
        reg_mem_cmd->reg_mem_addr_value[1] = get_uint8_from_at_cmd(data, BT_DFT_CMD_REGMEM_VALUE_INDEX);
    } else if (reg_mem_cmd->reg_mem_size == sizeof(uint16_t)) {
        reg_mem_cmd->reg_mem_addr_value[1] = get_uint16_from_at_cmd(data, BT_DFT_CMD_REGMEM_VALUE_INDEX);
    } else if (reg_mem_cmd->reg_mem_size == sizeof(uint32_t)) {
        reg_mem_cmd->reg_mem_addr_value[1] = get_uint32_from_at_cmd(data, BT_DFT_CMD_REGMEM_VALUE_INDEX);
    } else {
        return OAM_ERR_VALUE_ERR;
    }

    return OAM_RET_OK;
}

/* The handler of IPC_ACTION_AP_BT_HOOK_DATA */
#if (CORE_NUMS  > 1) && defined IPC_NEW
#elif ((CORE_NUMS > 1) && (defined CONFIG_DFX_SUPPORT_CUSTOM_LOG) && (CONFIG_DFX_SUPPORT_CUSTOM_LOG == DFX_YES))
#elif (CORE_NUMS  > 1)
bool ap_bt_hook_data_action_handler(ipc_action_t message, const volatile ipc_payload *payload_p,
    cores_t src, uint32_t id)
{
    uint8_t *command = (uint8_t *)payload_p->ap_bt_hook_data.command;
    uint32_t command_len = payload_p->ap_bt_hook_data.command_len;

    UNUSED(message);
    UNUSED(id);
    UNUSED(src);

    oml_pf_log_print1(LOG_BCORE_PLT_LIB_LOG, LOG_NUM_LIB_LOG, LOG_LEVEL_INFO, "[INFO]:Start DFT with hook_type=%d",
                      (uint32_t)payload_p->ap_bt_hook_data.hook_type);
    switch (payload_p->ap_bt_hook_data.hook_type) {
        case HOOK_TYPE_REGS:
            oml_reg_command(command, (uint16_t)command_len);
            break;
        case HOOK_TYPE_MEMS:
            oml_mem_command(command, (uint16_t)command_len);
            break;
        case HOOK_TYPE_BLOCK_MEM:
            oml_mem_command(command, (uint16_t)command_len);
            break;
        case HOOK_TYPE_SAMPLE_DATAS:
            if (g_log_oml_btc_oam_callback) {
                g_log_oml_btc_oam_callback(command, (uint16_t)command_len);
            }
            break;
        case HOOK_TYPE_DISABLE_MEM_ACCESS:
            break;
        default:
            break;
    }

    return true;
}

/* Send the result of start hooking to app core */
void send_bt_ap_hook_data(ipc_bt_ap_hook_type_e hook_type, uint32_t addr, uint32_t len, uint32_t value)
{
    ipc_status_t ipc_returned_value;
    ipc_payload_bt_ap_hook_data ipc_bt_ap_hook_data;

    ipc_bt_ap_hook_data.hook_type = hook_type;
    ipc_bt_ap_hook_data.addr = addr;
    ipc_bt_ap_hook_data.len = len;
    ipc_bt_ap_hook_data.value = value;

    ipc_returned_value = ipc_spin_send_message_timeout(CORES_APPS_CORE,
                                                       IPC_ACTION_BT_AP_HOOK_DATA,
                                                       (ipc_payload *)&ipc_bt_ap_hook_data,
                                                       sizeof(ipc_payload_bt_ap_hook_data),
                                                       IPC_PRIORITY_LOWEST, false, IPC_SPIN_SEND_MAX_TIMEOUT);
    if (ipc_returned_value != IPC_STATUS_OK) {
        UNUSED(ipc_returned_value);
    }
}
#endif

#if (CORE != GNSS) && (CORE != CONTROL_CORE) && (CORE != BT)
#ifndef HSO_SUPPORT
static void log_uart_clear_pending(void)
{
    if (uapi_uart_rx_fifo_is_empty(LOG_UART_BUS)) {
        hal_uart_clear_pending(LOG_UART_BUS);
    }
}
#endif

void log_uart_rx_callback(const void *buffer, uint16_t length, bool remaining)
{
    UNUSED(remaining);
#ifndef HSO_SUPPORT
    if (log_oam_store_buffer((const void *)buffer, length) == true) {
        log_recv_data_handle();
        log_uart_clear_pending();
    }
#else
    diag_uart_rx_proc((uint8_t*)buffer, length);
#endif
}
#endif

bool log_oam_register_handler_callback(uint8_t message_type, oam_cmd_handle_callback callback)
{
    switch (message_type) {
        case OM_MSG_TYPE_LOG:
            g_log_oam_log_config_callback = callback;
            break;
        case OM_MSG_MEM_COMMAND:
            g_log_oml_mem_command_callback = callback;
            break;
        case OM_MSG_REG_COMMAND:
            g_log_oml_reg_command_callback = callback;
            break;
        case OM_MSG_SSI_REG_COMMAND:
            g_log_oml_ssi_reg_command_callback = callback;
            break;
        case OM_MSG_CS_STAT_CMD:
            g_log_oml_msg_cs_stat_callback = callback;
            break;
        case OM_MSG_TYPE_BTC_OAM:
            g_log_oml_btc_oam_callback = callback;
            break;
        case OM_MSG_TYPE_BTH:
            g_log_oml_bth_oam_callback = callback;
            break;
        case OM_MSG_TYPE_APP:
            g_log_oml_app_oam_callback = callback;
            break;
        case OM_MSG_TYPE_HIFI:
            g_log_oml_hifi_oam_callback = callback;
            break;
        case OM_MSG_REG32_COMMAND:
            g_log_oml_reg32_command_callback = callback;
            break;
        case OM_MSG_SSIBLOCK_COMMAND:
            g_log_oml_ssi_block_callback = callback;
            break;
        default:
            return false;
    }
    return true;
}

static void log_oam_trigger_log_cb(uint8_t prime_id, uint8_t *data, uint16_t data_len)
{
    if ((prime_id == OM_LOG_CONFIG_REQ) && (g_log_oam_log_config_callback != NULL)) {
        g_log_oam_log_config_callback(data, data_len);
    }
}

static void log_oam_trigger_mem_cb(uint8_t *data, uint16_t data_len)
{
    if (g_log_oml_mem_command_callback != NULL) {
        g_log_oml_mem_command_callback(data, (uint8_t)data_len);
    }
}

static void log_oam_trigger_reg_cb(uint8_t *data, uint16_t data_len)
{
    if (g_log_oml_reg_command_callback != NULL) {
        g_log_oml_reg_command_callback(data, (uint8_t)data_len);
    }
}

static void log_oam_trigger_reg32_cb(uint8_t *data, uint16_t data_len)
{
    if (g_log_oml_reg32_command_callback != NULL) {
        g_log_oml_reg32_command_callback(data, (uint8_t)data_len);
    }
}

static void log_oam_trigger_ssi_reg_cb(uint8_t *data, uint16_t data_len)
{
    if (g_log_oml_ssi_reg_command_callback != NULL) {
        g_log_oml_ssi_reg_command_callback(data, (uint8_t)data_len);
    }
}

static void log_oam_trigger_cs_stat_cb(uint8_t *data, uint16_t data_len)
{
    if (g_log_oml_msg_cs_stat_callback != NULL) {
        g_log_oml_msg_cs_stat_callback(data, data_len);
    }
}

static void log_oam_trigger_btc_oam_cb(uint8_t *data, uint16_t data_len)
{
    if (g_log_oml_btc_oam_callback != NULL) {
        g_log_oml_btc_oam_callback(data + sizeof(om_msg_header_stru_t),
                                   (data_len - sizeof(om_msg_header_stru_t)) - sizeof(uint8_t));
    }
}

static void log_oam_trigger_bth_cb(uint8_t *data, uint16_t data_len)
{
    if (g_log_oml_bth_oam_callback != NULL) {
        g_log_oml_bth_oam_callback(data + sizeof(om_msg_header_stru_t),
                                   (data_len - sizeof(om_msg_header_stru_t)) - sizeof(uint8_t));
    }
}

static void log_oam_trigger_app_cb(uint8_t *data, uint16_t data_len)
{
    if (g_log_oml_app_oam_callback != NULL) {
        g_log_oml_app_oam_callback(data + sizeof(om_msg_header_stru_t),
                                   (data_len - sizeof(om_msg_header_stru_t)) - sizeof(uint8_t));
    }
}

static void log_oam_trigger_hifi_cb(uint8_t *data, uint16_t data_len)
{
    if (g_log_oml_hifi_oam_callback != NULL) {
        g_log_oml_hifi_oam_callback(data + sizeof(om_msg_header_stru_t),
                                    (data_len - sizeof(om_msg_header_stru_t)) - sizeof(uint8_t));
    }
}

static void log_oam_trigger_ssi_block_cb(uint8_t *data, uint16_t data_len)
{
    if (g_log_oml_ssi_block_callback != NULL) {
        g_log_oml_ssi_block_callback(data, (uint8_t)data_len);
    }
}

static void log_oam_trigger(const om_msg_header_stru_t *pst_oml_header, uint8_t prime_id, uint8_t *data,
                            uint16_t data_len)
{
    switch (pst_oml_header->func_type) {
        case OM_MSG_TYPE_LOG:
            log_oam_trigger_log_cb(prime_id, data, data_len);
            break;
        case OM_MSG_MEM_COMMAND:
            log_oam_trigger_mem_cb(data, data_len);
            break;
        case OM_MSG_REG_COMMAND:
            log_oam_trigger_reg_cb(data, data_len);
            break;
        case OM_MSG_SSI_REG_COMMAND:
            log_oam_trigger_ssi_reg_cb(data, data_len);
            break;
        case OM_MSG_CS_STAT_CMD:
            log_oam_trigger_cs_stat_cb(data, data_len);
            break;
        case OM_MSG_TYPE_BTC_OAM:
            log_oam_trigger_btc_oam_cb(data, data_len);
            break;
        case OM_MSG_TYPE_BTH:
            log_oam_trigger_bth_cb(data, data_len);
            break;
        case OM_MSG_TYPE_APP:
            log_oam_trigger_app_cb(data, data_len);
            break;
        case OM_MSG_TYPE_HIFI:
            log_oam_trigger_hifi_cb(data, data_len);
            break;
        case OM_MSG_REG32_COMMAND:
            log_oam_trigger_reg32_cb(data, data_len);
            break;
        case OM_MSG_SSIBLOCK_COMMAND:
            log_oam_trigger_ssi_block_cb(data, data_len);
            break;
        default:
            break;
        }
}

void log_oam_prase_message(void)
{
#ifndef HSO_SUPPORT
    uint8_t loop, prime_id;
    uint8_t *data = NULL;
    uint16_t data_len;
    om_msg_header_stru_t *pst_oml_header = NULL;

    for (loop = 0; loop < LOG_OAM_RX_BUFF_NUM; loop++) {
        /* If this buff is not a complete message */
        if (g_log_oam_rx_buffer[loop].uc_buff_state != OML_BUFF_USED) { continue; }

        pst_oml_header = (om_msg_header_stru_t *)(g_log_oam_rx_buffer[loop].auc_buff);
        data = (uint8_t *)pst_oml_header;
        data_len = pst_oml_header->frame_len;
        prime_id = (pst_oml_header->prime_id & LOG_OML_HEADER_PRIME_ID_MASK);
        log_oam_trigger(pst_oml_header, prime_id, data, data_len);
#ifndef USE_GPIO_SIMULATE_SSI
        oml_pf_log_print2(LOG_BCORE_PLT_LIB_LOG, LOG_NUM_LIB_LOG, LOG_LEVEL_INFO, \
                          "[OML]recv oml msg type:%d, length:%d", pst_oml_header->func_type, data_len);
#endif

        /* Clean state and buffer */
        g_log_oam_rx_buffer[loop].uc_buff_state = OML_BUFF_NOT_USED;
        g_log_oam_rx_buffer[loop].s_buff_used_len = 0;
    }
#endif
}

void oml_msg_parse(uint8_t *data, uint16_t data_len)
{
    om_msg_header_stru_t *pst_oml_header = (om_msg_header_stru_t *)(data);
    log_oam_trigger(pst_oml_header, 0, data, data_len);
}

#if (defined CONFIG_DFX_SUPPORT_CUSTOM_LOG) && (CONFIG_DFX_SUPPORT_CUSTOM_LOG == DFX_YES)
#elif (CORE_NUMS  > 1) && !defined(IPC_NEW)
bool get_hci_data_action_handler(ipc_action_t message,
                                 const volatile ipc_payload *payload_p, cores_t src, uint32_t id)
{
    if (message != IPC_ACTION_HCI_INFORM) {
        PRINT("receive hci message fail!\r\n");
        return true;
    }

    UNUSED(message);
    UNUSED(src);
    UNUSED(id);
#ifndef DEBUG_FUNC_UNSUPPORT
    oml_pf_log_print0(LOG_BCORE_PLT_LIB_LOG, LOG_NUM_LIB_LOG, LOG_LEVEL_INFO, "receive hci message succ!");
    if (g_log_oml_btc_oam_callback != NULL) {
        oml_pf_log_print1(LOG_BCORE_PLT_LIB_LOG, LOG_NUM_LIB_LOG, LOG_LEVEL_INFO,
                          "[OML]recv hci msg length:%d", payload_p->hci_data_type.length);
        g_log_oml_btc_oam_callback((uint8_t*)payload_p->hci_data_type.data, payload_p->hci_data_type.length);
    }
#else
    UNUSED(payload_p);
    PRINT("receive hci message succ!\r\n");
#endif
    return true;
}
#endif

#elif (defined CONFIG_DFX_SUPPORT_CUSTOM_LOG) && (CONFIG_DFX_SUPPORT_CUSTOM_LOG == DFX_YES)
#else  /* SYS_DEBUG_MODE_ENABLE == NO */
// This branch should be deleted after the code of BTC has been finished.
#ifdef SUPPORT_IPC
#include "log_oam_msg.h"
#include "ipc_actions.h"

/* Send the result of start hooking to app core */
void send_bt_ap_hook_data(ipc_bt_ap_hook_type_e hook_type, uint32_t addr, uint32_t len, uint32_t value)
{
    UNUSED(hook_type);
    UNUSED(addr);
    UNUSED(len);
    UNUSED(value);
}

bool log_oam_register_handler_callback(uint8_t message_type, oam_cmd_handle_callback callback)
{
    UNUSED(message_type);
    UNUSED(callback);

    return true;
}

bool get_hci_data_action_handler(ipc_action_t message,
                                 const volatile ipc_payload *payload_p, cores_t src, uint32_t id)
{
    UNUSED(message);
    UNUSED(payload_p);
    UNUSED(src);
    UNUSED(id);
    return true;
}
#endif
#endif  /* end of SYS_DEBUG_MODE_ENABLE == NO */
