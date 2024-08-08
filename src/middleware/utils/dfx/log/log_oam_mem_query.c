/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description:  LOG oam reg query MODULE
 */
#include "product.h"
#if SYS_DEBUG_MODE_ENABLE == YES
#include "non_os.h"
#include "log_oam_mem_query.h"

#if CORE == MASTER_BY_ALL
#include "securec.h"
#include "log_common.h"
#include "log_oam_msg.h"
#include "securec.h"
#ifdef HSO_SUPPORT
#include "diag_adapt_layer.h"
#include "diag_adapt_sdt.h"
#endif

#define write_u32(addr, d) (*(volatile uint32_t *)(uintptr_t)(addr) = (uint32_t)(d))
#define write_u16(addr, d) (*(volatile uint16_t *)(uintptr_t)(addr) = (uint16_t)(d))
#define write_u8(addr, d)  (*(volatile uint8_t *)(uintptr_t)(addr) = (uint8_t)(d))
#define read_u32(addr)     (*(volatile uint32_t *)(uintptr_t)(addr))
#define read_u16(addr)     (*(volatile uint16_t *)(uintptr_t)(addr))
#define read_u8(addr)      (*(volatile uint8_t *)(uintptr_t)(addr))
#define MEM_METHOD_REQ 0x01
#define MEM_METHOD_CNF 0x02

#if USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO
static void oml_mem_write(uint8_t *data, uint16_t length)
{
    om_mem_req_header_t *mem_request_header = NULL;
    uint8_t *mem_addr = NULL;
    uint32_t qury_addr;
    uint16_t qury_len;
    errno_t ret;
    UNUSED(length);

    mem_request_header = (om_mem_req_header_t *)data;
    qury_addr = mem_request_header->mem_start_addr;
    qury_len = mem_request_header->data_len;
    mem_addr = data + sizeof(om_mem_req_header_t);

    if (qury_len == ((mem_request_header->header.frame_len - sizeof(om_mem_req_header_t)) - OM_FRAME_DELIMITER_LEN)) {
        ret = memcpy_s((void *)(uintptr_t)qury_addr, qury_len, mem_addr, qury_len);
        if (ret != EOK) {
            UNUSED(ret);
        }
    }
}

static void oml_mem_read(uint8_t *data, uint16_t length)
{
    if (log_get_local_log_level() == LOG_LEVEL_NONE) {
        return;
    }

    om_mem_req_header_t *mem_request_header = NULL;
    om_msg_header_stru_t msg_header;
    uint32_t qury_addr;
    uint16_t qury_len;
    uint8_t msg_tailer;
    UNUSED(length);

    memset_s((void *)&msg_header, sizeof(msg_header), 0, sizeof(msg_header));

    /* Get the point and length */
    mem_request_header = (om_mem_req_header_t *)data;
    qury_addr = mem_request_header->mem_start_addr;
    qury_len = mem_request_header->data_len;

    /* Fill header */
    msg_header.frame_start = OM_FRAME_DELIMITER;
    msg_header.func_type = OM_MSG_MEM_COMMAND;
    msg_header.prime_id = MEM_METHOD_CNF;
    msg_header.frame_len = (uint16_t)sizeof(om_msg_header_stru_t) +
                            qury_len + (uint16_t)sizeof(msg_tailer);
    msg_tailer = OM_FRAME_DELIMITER;

    log_event((uint8_t *)&msg_header, sizeof(om_msg_header_stru_t));
    log_event((uint8_t *)(uintptr_t)qury_addr, qury_len);
    log_event(&msg_tailer, sizeof(msg_tailer));
}

void oml_mem_command(uint8_t *data, uint16_t length)
{
    if (data == NULL) {
        return;
    }

    om_mem_req_header_t *reg_request;

    reg_request = (om_mem_req_header_t *)data;

    if (reg_request->mode == MEM_ACTION_WRITE) {
        oml_mem_write(data, length);
    } else if (reg_request->mode == MEM_ACTION_READ) {
        oml_mem_read(data, length);
    } else {
        return;
    }
}

#ifdef SUPPORT_IPC
void oml_btc_command(uint8_t *data, uint16_t length)
{
    errno_t sec_ret;
    ipc_payload_hci_data_type hci_data;

    if ((data == NULL) || (length == 0) || (length > HCI_DATA_LRN)) {
        return;
    }

    sec_ret = memcpy_s(hci_data.data, HCI_DATA_LRN, data, length);
    if (sec_ret != EOK) {
        return;
    }
    hci_data.length = length;
    (void)ipc_send_message(CORES_BT_CORE, IPC_ACTION_HCI_INFORM, (ipc_payload *)&hci_data,
                           sizeof(ipc_payload_hci_data_type), IPC_PRIORITY_LOWEST, false);
}
#endif
#else  /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO */
static void oml_mem_write(const uint8_t *data, uint16_t length)
{
    rw_reg_mem_cmd_t mem_request;

    /* Parse the size in AT command */
    if (parse_reg_or_mem_cmd_size(data, length, &mem_request) != OAM_RET_OK) {
        return;
    }

    /* Parse the addr and value AT command */
    if (parse_reg_or_mem_cmd_addr_value(data, length, &mem_request) != OAM_RET_OK) {
        return;
    }

    if (mem_request.reg_mem_size == sizeof(uint8_t)) {
        write_u8((void *)(uintptr_t)mem_request.reg_mem_addr_value[0], mem_request.reg_mem_addr_value[1]);
    } else if (mem_request.reg_mem_size == sizeof(uint16_t)) {
        write_u16((void *)(uintptr_t)mem_request.reg_mem_addr_value[0], mem_request.reg_mem_addr_value[1]);
    } else if (mem_request.reg_mem_size == sizeof(uint32_t)) {
        write_u32((void *)((uintptr_t)mem_request.reg_mem_addr_value[0]), mem_request.reg_mem_addr_value[1]);
    } else { }
}

static void oml_mem_read(const uint8_t *data, uint16_t length)
{
    uint32_t value;
    rw_reg_mem_cmd_t mem_request;

    /* Parse the size in AT command */
    if (parse_reg_or_mem_cmd_size(data, length, &mem_request) != OAM_RET_OK) {
        return;
    }

    /* Parse the addr and value AT command */
    if (parse_reg_or_mem_cmd_addr_value(data, length, &mem_request) != OAM_RET_OK) {
        return;
    }

    if (mem_request.reg_mem_size == sizeof(uint8_t)) {
        value = read_u8((void *)(uintptr_t)mem_request.reg_mem_addr_value[0]);
    } else if (mem_request.reg_mem_size == sizeof(uint16_t)) {
        value = read_u16((void *)(uintptr_t)mem_request.reg_mem_addr_value[0]);
    } else if (mem_request.reg_mem_size == sizeof(uint32_t)) {
        value = read_u32((void *)((uintptr_t)mem_request.reg_mem_addr_value[0]));
    } else {
        /* unknow size of the data want to write */
        value = 0;
    }

    send_bt_ap_hook_data(HOOK_TYPE_MEMS, 0, 0, value);
}

static void oml_mem_read_block(const uint8_t *data, uint16_t length)
{
    rw_reg_mem_cmd_t mem_request;

    /* Parse the size in AT command */
    if (parse_reg_or_mem_cmd_size(data, length, &mem_request) != OAM_RET_OK) {
        return;
    }

    /* Parse the addr and value AT command */
    if (parse_reg_or_mem_cmd_addr_value(data, length, &mem_request) != OAM_RET_OK) {
        return;
    }

    /* Make sure the App core has the right to read the address of BT core */
    hal_memory_mcpu_access_enable(HAL_MEMORY_ACCESS_BSUB);

    /* Nority the App core to read the memory of BT core directly */
    send_bt_ap_hook_data(HOOK_TYPE_BLOCK_MEM, mem_request.reg_mem_addr_value[0], mem_request.reg_mem_addr_value[1], 0);
}

void oml_mem_command(uint8_t *data, uint16_t length)
{
    if (data == NULL) {
        return;
    }

    rw_reg_mem_cmd_t mem_request;

    if (parse_reg_or_mem_cmd_operate(data, length, &mem_request) != OAM_RET_OK) {
        return;
    }

    if (mem_request.mode == MEM_ACTION_WRITE) {
        oml_mem_write(data, length);
    } else if (mem_request.mode == MEM_ACTION_READ) {
        oml_mem_read(data, length);
    } else if (mem_request.mode == MEM_ACTION_WRITE_BLOCK) {
        // Not implement.
    } else if (mem_request.mode == MEM_ACTION_READ_BLOCK) {
        oml_mem_read_block(data, length);
    } else {
        return;
    }
}

void oml_btc_command(uint8_t *data, uint16_t length)
{
    UNUSED(data);
    UNUSED(length);
}
#endif  /* end of USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO */

void oml_mem_register_callback(void)
{
    log_oam_register_handler_callback(OM_MSG_MEM_COMMAND, oml_mem_command);
}

#if ((CORE == APPS) && (CHIP_LIBRA || CHIP_SOCMN1 || CHIP_BS25 || CHIP_BRANDY || CHIP_SW39 || CHIP_SW21))
void oml_btc_cmd_callback(void)
{
    log_oam_register_handler_callback(OM_MSG_TYPE_BTC_OAM, oml_btc_command);
#ifdef HSO_SUPPORT
    diag_register_debug_cmd_callback(oml_btc_command);
#endif
}
#endif

#else

#endif

#endif  /* end of SYS_DEBUG_MODE_ENABLE == YES */
