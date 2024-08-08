/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:LOG oam reg query MODULE
 * Author:
 * Create:
 */

#include "product.h"
#if SYS_DEBUG_MODE_ENABLE == YES
#include "chip_core_definition.h"
#include "securec.h"
#if CORE == MASTER_BY_ALL
#include "log_oam_msg.h"
#include "log_common.h"
#ifdef USE_GPIO_SIMULATE_SSI
#include "ssi.h"
#include "tcxo.h"
#endif
#include "log_oam_reg_query.h"

#define write_u8(addr, d)  (*(volatile uint8_t *)(uintptr_t)(addr) = (uint8_t)(d))
#ifdef USE_GPIO_SIMULATE_SSI
#define ssi_write_u32(addr, d) ssi_write32_etc((uint32_t)addr, (uint32_t)d)
#define ssi_write_u16(addr, d) ssi_write16_etc((uint16_t)addr, (uint16_t)d)
#define ssi_read_u32(addr) ssi_read32_etc((uint32_t)addr)
#define ssi_read_u16(addr) ssi_read16_etc((uint16_t)addr)
#endif
#define write_u32(addr, d)  (*(volatile uint32_t *)(uintptr_t)(addr) = (uint32_t)(d))
#define write_u16(addr, d) (*(volatile uint16_t *)(uintptr_t)(addr) = (uint16_t)(d))
#define read_u32(addr)     (*(volatile uint32_t *)(uintptr_t)(addr))
#define read_u16(addr)     (*(volatile uint16_t *)(uintptr_t)(addr))

#define REG_METHOD_REQ 0x01
#define REG_METHOD_CNF 0x02
#define LOG_OAM_REG_ADDR_SHIFT  16

#if MCU_ONLY
/* SSI REG begin */
#define RDSSI_CMD_LEN     (17)
#define WRSSI_CMD_LEN     (21)
#define BT_SSI_BASE_ADDR  (0x59003800)
#define BT_SSI_ADDR       (0x8000)
#define BT_SSI_ADDR_REG   (BT_SSI_BASE_ADDR + 0x30)
#define BT_SSI_RW_REG     (BT_SSI_BASE_ADDR + 0x34)
#define BT_SSI_WDATA_REG  (BT_SSI_BASE_ADDR + 0x38)
#define BT_SSI_RDATA_REG  (BT_SSI_BASE_ADDR + 0x3c)
#define BT_SSI_TRANS_DONE (BT_SSI_BASE_ADDR + 0x40)
#define BT_SSI_RD_ERR     (BT_SSI_BASE_ADDR + 0x48)
#define BT_CFG_SUCCESS (0)
#define BT_CFG_FAIL    (1)
/* SSI read write mode */
#define BT_SSIREAD  (1)
#define BT_SSIWRITE (0)
/* set read write work flag */
#define BT_SSIWORKFLAG     (1)
#define BT_SSIWORKDONEFLAG (0)
/* SSI REG end */
#ifdef USE_GPIO_SIMULATE_SSI
#define SSI_FUNC_TYPE                       0xa
#define SSI_WRITE_MODE                      0x2
#define SSI_READ_MODE                       0x3
#define SSI_32B_WRITE_MODE                  0x4
#define SSI_32B_READ_MODE                   0x5
#define SIZEOF_OM_MSG_SSI_WRITE_RPORT_T     13
#define SIZEOF_OM_MSG_SSI_READ              12
#define SIZEOF_OM_SSI_BLOCK_STRU_T          17
#define BITS_OF_ONE_BYTE                    8
#define CLEAR_LOW_EIGHT_BITS                0xFF00
#define CLEAR_HIGH_EIGHT_BITS               0x00FF
#define CLEAR_LOW_TWO_BITS                  0xFFFFFFFC
#define SSI_WORD_MASK                       0xFFFFFFFF
#define SSI_WRITE_32BIT_PAGE_DELAY          100ULL
#define SSI_READ_32BIT_PAGE_DELAY           30ULL
#define REG_VALUE_MAX                       256
#define SSI_0X_FLAG                         0x1100
#define SSI_3X_FLAG                         0x1130
#define SSI_5X_FLAG                         0x1150
#define SSI_PIN_MODE_FLAG                   0x75000000
#define OPERATION_FOR_MASTER_BEGIN          0x75000010
#define OPERATION_FOR_MASTER_END            0x75000020

bool g_wr_device_instead_of_master_flag = true;
#endif
#endif

#if USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO
static void oml_reg_write(const uint8_t *data, uint16_t length)
{
    uint32_t qury_addr;
    uint32_t reg_value;
    uint32_t reg_len;
    om_reg_data_stru_t *reg_request = NULL;

    UNUSED(length);

    reg_request = (om_reg_data_stru_t *)data;
    qury_addr = reg_request->reg_addr;
    reg_value = reg_request->reg_value;
    reg_len = reg_request->reg_len;

    if (reg_len == sizeof(uint8_t)) {
        write_u8(qury_addr, (uint8_t)reg_value);
    } else if (reg_len == sizeof(uint16_t)) {
        write_u16(qury_addr, (uint16_t)reg_value);
    } else if (reg_len == sizeof(uint32_t)) {
        write_u32(qury_addr, reg_value);
    } else {
    }
}

static void oml_reg_read(const uint8_t *data, uint16_t length)
{
    if (log_get_local_log_level() == LOG_LEVEL_NONE) {
        return;
    }

    uint32_t qury_addr;
    uint8_t reg_len;
    om_reg_data_stru_t *reg_request = NULL;
    om_reg_data_stru_t reg_read_output;
    UNUSED(length);

    memset_s((void *)&reg_read_output, sizeof(reg_read_output), 0, sizeof(reg_read_output));

    reg_request = (om_reg_data_stru_t *)data;
    qury_addr = reg_request->reg_addr;
    reg_len = (uint8_t)reg_request->reg_len;

    reg_read_output.header.frame_start = OM_FRAME_DELIMITER;
    reg_read_output.header.func_type = OM_MSG_REG_COMMAND;
    reg_read_output.header.prime_id = REG_METHOD_CNF;
    reg_read_output.header.frame_len = (uint16_t)sizeof(om_reg_data_stru_t);
    reg_read_output.reg_addr = qury_addr;
    reg_read_output.reg_len = reg_len;
    reg_read_output.mode = REG_ACTION_READ;

    if (reg_len == sizeof(uint8_t)) {
        reg_read_output.reg_value = *(uint8_t *)(uintptr_t)qury_addr;
    } else if (reg_len == sizeof(uint16_t)) {
        reg_read_output.reg_value = read_u16((uintptr_t)qury_addr);
    } else {
        reg_read_output.reg_value = read_u32((uintptr_t)qury_addr);
    }
    reg_read_output.msg_tailer = OM_FRAME_DELIMITER;

    log_event((uint8_t *)&reg_read_output, reg_read_output.header.frame_len);
}

void oml_reg_command(uint8_t *data, uint16_t length)
{
    if (data == NULL) {
        return;
    }
    om_reg_data_stru_t *reg_request;

    reg_request = (om_reg_data_stru_t *)data;

    if (reg_request->mode == REG_ACTION_WRITE) {
        oml_reg_write(data, length);
    } else if (reg_request->mode == REG_ACTION_READ) {
        oml_reg_read(data, length);
    } else {
        return;
    }
}
#else
static void oml_reg_write(const uint8_t *data, uint16_t length)
{
    rw_reg_mem_cmd_t reg_request;

    /* Parse the size in AT command */
    if (parse_reg_or_mem_cmd_size(data, length, &reg_request) != OAM_RET_OK) {
        return;
    }

    /* Parse the AT command */
    if (parse_reg_or_mem_cmd_addr_value(data, length, &reg_request) != OAM_RET_OK) {
        return;
    }

    if (reg_request.reg_mem_size == sizeof(uint8_t)) {
        write_u8((void *)(uintptr_t)reg_request.reg_mem_addr_value[0], (uint8_t)reg_request.reg_mem_addr_value[1]);
    } else if (reg_request.reg_mem_size == sizeof(uint16_t)) {
        write_u16((void *)(uintptr_t)reg_request.reg_mem_addr_value[0], (uint16_t)reg_request.reg_mem_addr_value[1]);
    } else if (reg_request.reg_mem_size == sizeof(uint32_t)) {
        write_u32((void *)((uintptr_t)reg_request.reg_mem_addr_value[0]), reg_request.reg_mem_addr_value[1]);
    } else {
    }

    send_bt_ap_hook_data(HOOK_TYPE_REGS, 0, 0, reg_request.reg_mem_addr_value[1]);
}

static void oml_reg_read(const uint8_t *data, uint16_t length)
{
    uint32_t value;
    rw_reg_mem_cmd_t reg_request;

    /* Parse the size in AT command */
    if (parse_reg_or_mem_cmd_size(data, length, &reg_request) != OAM_RET_OK) {
        return;
    }

    /* Parse the addr and value in AT command */
    if (parse_reg_or_mem_cmd_addr_value(data, length, &reg_request) != OAM_RET_OK) {
        return;
    }

    /* Read the value of specified register or memory */
    if (reg_request.reg_mem_size == sizeof(uint8_t)) {
        value = *(uint8_t *)(uintptr_t)reg_request.reg_mem_addr_value[0];
    } else if (reg_request.reg_mem_size == sizeof(uint16_t)) {
        value = read_u16((void *)(uintptr_t)reg_request.reg_mem_addr_value[0]);
    } else {
        value = read_u32((void *)((uintptr_t)reg_request.reg_mem_addr_value[0]));
    }

    send_bt_ap_hook_data(HOOK_TYPE_REGS, 0, 0, value);
}

void oml_reg_command(uint8_t *data, uint16_t length)
{
    rw_reg_mem_cmd_t reg_request;

    /* Parse the AT command */
    if (parse_reg_or_mem_cmd_operate(data, length, &reg_request) != OAM_RET_OK) {
        return;
    }

    if (reg_request.mode == REG_ACTION_WRITE) {
        oml_reg_write(data, length);
    } else if (reg_request.mode == REG_ACTION_READ) {
        oml_reg_read(data, length);
    } else {
        return;
    }
}
#endif
void oml_reg_register_callback(void)
{
    log_oam_register_handler_callback(OM_MSG_REG_COMMAND, oml_reg_command);
}
#else
// add rpc rpc interface for APP and HIFI
#endif

#if MCU_ONLY
#ifdef USE_GPIO_SIMULATE_SSI
uint32_t g_reg_value[REG_VALUE_MAX] = { 0 };
static void oml_ssi_write_reg(uint32_t ul_add, uint16_t ul_value)
{
    ssi_write_u32(ul_add, ul_value);
}

static uint16_t oml_ssi_read_reg(uint32_t ul_add)
{
    return ssi_read_u32(ul_add);
}

static void ssi_write_reg(uint32_t ul_add, uint32_t ul_value)
{
    ssi_write32(ul_add, ul_value);
}

static uint32_t ssi_read_reg(uint32_t ul_add)
{
    return ssi_read32(ul_add);
}

static void ssi_select_ssi16_mode(uint32_t addr, uint32_t value)
{
    if ((addr == SSI_PIN_MODE_FLAG) && (value == SSI_3X_FLAG)) {
        ssi_init_bs3x();
    }
}

static void oml_ssi_reg_write(uint8_t *data, uint16_t length)
{
    uint32_t qury_addr;
    uint32_t reg_value;
    uint8_t reg_num;
    UNUSED(length);
    om_msg_ssi_header_t *ssi_header = (om_msg_ssi_header_t*)data;
    om_reg_addr_val_t *ssi_reg_addr_val = (om_reg_addr_val_t*)(data + sizeof(om_msg_ssi_header_t));
    reg_num = ssi_header->reg_num;

    for (int i = 0; i < reg_num; i++) {
        qury_addr = ssi_reg_addr_val->reg_addr;
        reg_value = ssi_reg_addr_val->reg_value;
        ssi_select_ssi16_mode(qury_addr, reg_value);
        oml_ssi_write_reg(qury_addr, (uint16_t)reg_value);
        ssi_reg_addr_val++;
    }
    om_msg_ssi_write_rport_t reg_ssi_input;

    memset_s((void *)&reg_ssi_input, sizeof(om_msg_ssi_write_rport_t), 0, sizeof(om_msg_ssi_write_rport_t));
    reg_ssi_input.header.frame_start = OM_FRAME_DELIMITER;
    reg_ssi_input.header.func_type = ssi_header->header.func_type;
    reg_ssi_input.header.prime_id = REG_METHOD_CNF;
    reg_ssi_input.header.frame_len = sizeof(om_msg_ssi_write_rport_t);
    reg_ssi_input.header.sn = ssi_header->header.sn;
    reg_ssi_input.reg_result = 0;
    reg_ssi_input.reg_num = reg_num;
    reg_ssi_input.mode = SSI_WRITE_MODE;
    reg_ssi_input.msg_tailer = OM_FRAME_DELIMITER;

    log_event((uint8_t *)&reg_ssi_input, reg_ssi_input.header.frame_len);
}

static void oml_ssi_reg_read(uint8_t *data, uint16_t length)
{
    uint32_t qury_addr;
    uint8_t reg_num_temp;
    UNUSED(length);
    uint32_t *ssi_reg_addr_val;
    om_msg_ssi_header_t *ssi_read = (om_msg_ssi_header_t*)data;
    ssi_reg_addr_val = (uint32_t *)(data + sizeof(om_msg_ssi_header_t));
    reg_num_temp = ssi_read->reg_num;
    om_msg_ssi_read_rport_t reg_read_output;
    memset_s((void *)&reg_read_output, sizeof(om_msg_ssi_read_rport_t), 0, sizeof(om_msg_ssi_read_rport_t));

    reg_read_output.header.frame_start = OM_FRAME_DELIMITER;
    reg_read_output.header.func_type = ssi_read->header.func_type;
    reg_read_output.header.prime_id = REG_METHOD_CNF;
    reg_read_output.header.frame_len = SIZEOF_OM_MSG_SSI_WRITE_RPORT_T + sizeof(uint32_t) * reg_num_temp;
    reg_read_output.header.sn = ssi_read->header.sn;
    reg_read_output.reg_num = reg_num_temp;
    reg_read_output.mode = SSI_READ_MODE;
    reg_read_output.query_reg_result = 0;

    for (int i = 0; i < reg_num_temp; i++) {
        qury_addr = ssi_reg_addr_val[i];
        g_reg_value[i] = oml_ssi_read_reg(qury_addr);
    }
    uint8_t msg_tail = OM_FRAME_DELIMITER;
    log_event((uint8_t*)(&reg_read_output), SIZEOF_OM_MSG_SSI_READ);
    log_event((uint8_t*)(g_reg_value), sizeof(uint32_t)* reg_num_temp);
    log_event((uint8_t*)(&msg_tail), 1);
}

static void ssi_select_ssi32_mode(uint32_t addr, uint32_t value)
{
    if (addr == SSI_PIN_MODE_FLAG) {
        if (value == SSI_0X_FLAG) {
            ssi_init_bs0x();
        } else if (value == SSI_3X_FLAG) {
            ssi_init_bs3x();
        } else if (value == SSI_5X_FLAG) {
            ssi_init_bs5x();
        }
    }
}

static void ssi_reg_write(uint8_t *data, uint16_t length)
{
    uint32_t qury_addr;
    uint32_t reg_value;
    uint8_t reg_num;
    UNUSED(length);
    om_msg_ssi_header_t *ssi_header = (om_msg_ssi_header_t*)data;
    om_reg_addr_val_t *ssi_reg_addr_val = (om_reg_addr_val_t*)(data + sizeof(om_msg_ssi_header_t));
    reg_num = ssi_header->reg_num;
    for (int i = 0; i < reg_num; i++) {
        qury_addr = ssi_reg_addr_val->reg_addr;
        reg_value = ssi_reg_addr_val->reg_value;
        ssi_select_ssi32_mode(qury_addr, reg_value);
        ssi_reg_addr_val++;
        if (qury_addr == OPERATION_FOR_MASTER_BEGIN) {
            g_wr_device_instead_of_master_flag = false;
            continue;
        } else if (qury_addr == OPERATION_FOR_MASTER_END) {
            g_wr_device_instead_of_master_flag = true;
            continue;
        }
        if (g_wr_device_instead_of_master_flag == true) {
            ssi_write_reg(qury_addr, reg_value);
        } else {
            write_u32(qury_addr, reg_value);
        }
    }

    om_msg_ssi_write_rport_t reg_ssi_input;

    memset_s((void *)&reg_ssi_input, sizeof(om_msg_ssi_write_rport_t), 0, sizeof(om_msg_ssi_write_rport_t));
    reg_ssi_input.header.frame_start = OM_FRAME_DELIMITER;
    reg_ssi_input.header.func_type = ssi_header->header.func_type;
    reg_ssi_input.header.prime_id = REG_METHOD_CNF;
    reg_ssi_input.header.frame_len = sizeof(om_msg_ssi_write_rport_t);
    reg_ssi_input.header.sn = ssi_header->header.sn;
    reg_ssi_input.reg_result = 0;
    reg_ssi_input.reg_num = reg_num;
    reg_ssi_input.mode = SSI_WRITE_MODE;
    reg_ssi_input.msg_tailer = OM_FRAME_DELIMITER;

    log_event((uint8_t *)&reg_ssi_input, reg_ssi_input.header.frame_len);
}

static void ssi_reg_read(uint8_t *data, uint16_t length)
{
    uint32_t qury_addr;
    uint8_t reg_num_temp;
    UNUSED(length);
    uint32_t *ssi_reg_addr_val;
    om_msg_ssi_header_t *ssi_read = (om_msg_ssi_header_t*)data;
    ssi_reg_addr_val = (uint32_t *)(data + sizeof(om_msg_ssi_header_t));
    reg_num_temp = ssi_read->reg_num;
    om_msg_ssi_read_rport_t reg_read_output;
    memset_s((void *)&reg_read_output, sizeof(om_msg_ssi_read_rport_t), 0, sizeof(om_msg_ssi_read_rport_t));

    reg_read_output.header.frame_start = OM_FRAME_DELIMITER;
    reg_read_output.header.func_type = ssi_read->header.func_type;
    reg_read_output.header.prime_id = REG_METHOD_CNF;
    reg_read_output.header.frame_len = SIZEOF_OM_MSG_SSI_WRITE_RPORT_T + sizeof(uint32_t) * reg_num_temp;
    reg_read_output.header.sn = ssi_read->header.sn;
    reg_read_output.reg_num = reg_num_temp;
    reg_read_output.mode = SSI_READ_MODE;
    reg_read_output.query_reg_result = 0;

    for (int i = 0; i < reg_num_temp; i++) {
        qury_addr = ssi_reg_addr_val[i];
        if (qury_addr == OPERATION_FOR_MASTER_BEGIN) {
            g_wr_device_instead_of_master_flag = false;
            continue;
        } else if (qury_addr == OPERATION_FOR_MASTER_END) {
            g_wr_device_instead_of_master_flag = true;
            continue;
        }
        if (g_wr_device_instead_of_master_flag == true) {
            g_reg_value[i] = ssi_read_reg(qury_addr);
        } else {
            g_reg_value[i] = read_u32(qury_addr);
        }
    }
    uint8_t msg_tail = OM_FRAME_DELIMITER;
    log_event((uint8_t*)(&reg_read_output), SIZEOF_OM_MSG_SSI_READ);
    log_event((uint8_t*)(g_reg_value), sizeof(uint32_t)* reg_num_temp);
    log_event((uint8_t*)(&msg_tail), 1);
}

void oml_ssi_reg_command(uint8_t *data, uint16_t length)
{
    om_msg_ssi_header_t *ssi_head = (om_msg_ssi_header_t*)data;
    uint8_t mode = ssi_head->mode;
    if (mode == SSI_WRITE_MODE) {
        oml_ssi_reg_write(data, length);
    } else if (mode == SSI_READ_MODE) {
        oml_ssi_reg_read(data, length);
    } else {
        return;
    }
}

void oml_ssi_reg32_command(uint8_t *data, uint16_t length)
{
    om_msg_ssi_header_t *ssi_head = (om_msg_ssi_header_t*)data;
    uint8_t mode = ssi_head->mode;
    if (mode == SSI_WRITE_MODE) {
        ssi_reg_write(data, length);
    } else if (mode == SSI_READ_MODE) {
        ssi_reg_read(data, length);
    } else {
        return;
    }
}

static void oml_ssi_write_one_data(uint16_t usaddr, uint16_t uldata)
{
    ssi_write_u16(BT_SSI_ADDR_REG, usaddr);
    ssi_write_u16(BT_SSI_RW_REG, BT_SSIWRITE);
    ssi_write_u16(BT_SSI_WDATA_REG, uldata);
    ssi_write_u16(BT_SSI_TRANS_DONE, BT_SSIWORKFLAG);
}

static uint16_t ssi_write_page(uint32_t dest_addr, uint16_t len, uint16_t *data)
{
    uint16_t write_len = 0;
    uint16_t write_data;
    while (len > 0) {
        if (len >= sizeof(uint16_t)) {
            ssi_write_u32(dest_addr, *data);
            data++;
            len -= sizeof(uint16_t);
            dest_addr += sizeof(uint16_t);
            write_len += sizeof(uint16_t);
        } else {
            write_data = ssi_read_u32(dest_addr);
            write_data = ((*data) & CLEAR_HIGH_EIGHT_BITS) | (write_data & CLEAR_LOW_EIGHT_BITS);
            ssi_write_u32(dest_addr, write_data);
            len = 0;
            write_len += 1;
        }
    }
    return write_len;
}

static uint16_t ssi_32b_write_page(uint32_t dest_addr, uint16_t len, uint32_t *data)
{
    uint16_t write_length = 0;
    uint32_t reg_value;

    oml_ssi_write_one_data(AHB_SIZE_REG, SSI_32BIT_MODE);
    while (len > 0) {
        reg_value = *data;
        if (len >= sizeof(uint32_t)) {
            ssi_write32(dest_addr, reg_value);
            len -= sizeof(uint32_t);
            dest_addr += sizeof(uint32_t);
            write_length += sizeof(uint32_t);
            data++;
        } else {
            reg_value = ssi_read32(dest_addr);
            uapi_tcxo_delay_ms(SSI_WRITE_32BIT_PAGE_DELAY);

            // The result is composed by the high (32 - len*8) bits of dest_addr and the low len*8 bits of
            // input data while length < 4.
            reg_value = (reg_value & (uint32_t)(SSI_WORD_MASK << (len * BITS_OF_ONE_BYTE))) | \
                        ((*data) & (uint32_t)~(SSI_WORD_MASK << (len * BITS_OF_ONE_BYTE)));
            ssi_write32(dest_addr, reg_value);
            uapi_tcxo_delay_ms(SSI_WRITE_32BIT_PAGE_DELAY);

            write_length += len;
            len  = 0;
        }
    }

    oml_ssi_write_one_data(AHB_SIZE_REG, SSI_16BIT_MODE);
    return write_length;
}

static uint16_t ssi_write_block(uint32_t dest_addr, uint16_t len, uint8_t mode, uint16_t *data)
{
    int write_len;
    while (len > 0) {
        if (mode == SSI_WRITE_BLOCK) {
            write_len = ssi_write_page(dest_addr, len, data);
        } else if (mode == SSI_32BWRITE_BLOCK) {
            write_len = ssi_32b_write_page(dest_addr, len, (uint32_t *)data);
        } else { return BT_CFG_FAIL; }

        if (write_len == 0) {
            return BT_CFG_FAIL;
        }
        dest_addr += write_len;
        data += write_len / sizeof(uint16_t);
        if (len > write_len) {
            len -= write_len;
        } else if (len == write_len) {
            return BT_CFG_SUCCESS;
        } else {
            return BT_CFG_FAIL;
        }
    }
    return BT_CFG_SUCCESS;
}

static void ssi_write_block_handle(uint8_t *data, uint16_t len, uint8_t mode)
{
    om_ssi_block_stru_t ssi_block;
    memcpy_s(&ssi_block, SIZEOF_OM_SSI_BLOCK_STRU_T, data, SIZEOF_OM_SSI_BLOCK_STRU_T);
    uint32_t dest_addr = ssi_block.addr;
    uint16_t length = ssi_block.length;
    uint8_t result;

    result = ssi_write_block(dest_addr, length, mode, (uint16_t*)(data + SIZEOF_OM_SSI_BLOCK_STRU_T -1));
    ssi_block.header.frame_start = OM_FRAME_DELIMITER;
    ssi_block.header.func_type = OM_MSG_SSIBLOCK_COMMAND;
    ssi_block.header.frame_len = SIZEOF_OM_SSI_BLOCK_STRU_T;
    ssi_block.operate_result = result;
    ssi_block.msg_tailer = OM_FRAME_DELIMITER;
    UNUSED(len);
    log_event((uint8_t*)(&ssi_block), ssi_block.header.frame_len);
}


static uint16_t ssi_read_page(uint32_t dest_addr, uint16_t len)
{
    uint16_t read_length = 0;
    uint16_t read_data;
    while (len > 0) {
        read_data = ssi_read_u32(dest_addr);
        if (len >= sizeof(uint16_t)) {
            len -= sizeof(uint16_t);
            dest_addr += sizeof(uint16_t);
            read_length += sizeof(uint16_t);
            log_event((uint8_t*)(&read_data), sizeof(uint16_t));
        } else {
            log_event((uint8_t*)(&read_data), len);
            read_length += len;
            len = 0;
        }
    }
    return read_length;
}

static uint16_t ssi_32b_read_page(uint32_t dest_addr, uint16_t len)
{
    uint32_t read_data = 0;
    uint32_t addr_temp;
    uint16_t read_length = 0;

    addr_temp = dest_addr;
    addr_temp = addr_temp & CLEAR_LOW_TWO_BITS;
    oml_ssi_write_one_data(AHB_SIZE_REG, SSI_32BIT_MODE);
    while (len > 0) {
        read_data = ssi_read32(addr_temp);
        uapi_tcxo_delay_ms(SSI_READ_32BIT_PAGE_DELAY);
        if (len >= sizeof(uint32_t)) {
            len -= sizeof(uint32_t);
            addr_temp += sizeof(uint32_t);
            read_length += sizeof(uint32_t);
            log_event((uint8_t *)&read_data, sizeof(uint32_t));
        } else {
            log_event((uint8_t *)&read_data, len);
            read_length += len;
            len = 0;
        }
    }
    oml_ssi_write_one_data(AHB_SIZE_REG, SSI_16BIT_MODE);

    return read_length;
}

static uint16_t ssi_read_block(uint32_t dest_addr, uint16_t len, uint8_t mode)
{
    uint16_t read_length;
    while (len > 0) {
        if (mode == SSI_READ_BLOCK) {
            read_length = ssi_read_page(dest_addr, len);
        } else {
            read_length = ssi_32b_read_page(dest_addr, len);
        }

        if (read_length == 0) {
            return BT_CFG_FAIL;
        }
        dest_addr += read_length;
        if (len > read_length) {
            len -= read_length;
        } else if (len == read_length) {
            return BT_CFG_SUCCESS;
        } else {
            return BT_CFG_FAIL;
        }
    }
    return BT_CFG_SUCCESS;
}

static void ssi_read_block_handle(uint8_t *data, uint16_t len, uint8_t mode)
{
    om_ssi_block_stru_t ssi_block;
    memcpy_s(&ssi_block, SIZEOF_OM_SSI_BLOCK_STRU_T, data, SIZEOF_OM_SSI_BLOCK_STRU_T);
    uint32_t dest_addr = ssi_block.addr;
    uint16_t read_len = ssi_block.length;
    uint8_t result = 0;
    if ((data == NULL) || (len == 0)) {
        result = 1;
    }
    ssi_block.header.frame_len = read_len + SIZEOF_OM_SSI_BLOCK_STRU_T;
    ssi_block.operate_result = result;
    log_event((uint8_t*)(&ssi_block), SIZEOF_OM_SSI_BLOCK_STRU_T - 1);
    result = ssi_read_block(dest_addr, read_len, mode);
    uint8_t msg_tail = OM_FRAME_DELIMITER;
    log_event((uint8_t*)(&msg_tail), 1);
}

void oml_ssi_block_command(uint8_t *data, uint16_t length)
{
    if ((data == NULL) || (length == 0)) {
        return;
    }
    uint8_t ssi_mode;
    om_ssi_block_stru_t *ssi_request = NULL;

    ssi_request = (om_ssi_block_stru_t *)data;
    ssi_mode = ssi_request->operate_mode;
    if ((ssi_mode == SSI_WRITE_MODE) || (ssi_mode == SSI_32B_WRITE_MODE)) {
        ssi_write_block_handle(data, length, ssi_mode);
    } else if ((ssi_mode == SSI_READ_MODE) || (ssi_mode == SSI_32B_READ_MODE)) {
        ssi_read_block_handle(data, length, ssi_mode);
    }
}

void oml_ssi_block_callback(void)
{
    log_oam_register_handler_callback(OM_MSG_SSIBLOCK_COMMAND, oml_ssi_block_command);
}

void oml_ssi_reg32_register_callback(void)
{
    log_oam_register_handler_callback(OM_MSG_REG32_COMMAND, oml_ssi_reg32_command);
}
#else
static void oml_ssi_write_one_data(uint16_t usaddr, uint16_t uldata)
{
    write_u16(BT_SSI_ADDR_REG, usaddr);
    write_u16(BT_SSI_RW_REG, BT_SSIWRITE);
    write_u16(BT_SSI_WDATA_REG, uldata);
    write_u16(BT_SSI_TRANS_DONE, BT_SSIWORKFLAG);
}

static void oml_ssi_set_baseaddr(uint32_t uladdr)
{
    uint16_t ul_base_addr;

    ul_base_addr = (uint16_t)(uladdr >> LOG_OAM_REG_ADDR_SHIFT);
    oml_ssi_write_one_data(BT_SSI_ADDR, ul_base_addr);
}

static void oml_ssi_write_reg(uint32_t ul_add, uint16_t ul_value)
{
    uint16_t us_ssi_regaddr;
    uint8_t  uc_ret;

    if (ul_add < 0x36) {
        us_ssi_regaddr = (uint16_t)ul_add;
        us_ssi_regaddr = ((us_ssi_regaddr >> 1) | BT_SSI_ADDR);
        write_u16(BT_SSI_ADDR_REG, us_ssi_regaddr);
        write_u16(BT_SSI_RW_REG, BT_SSIWRITE);
        write_u16(BT_SSI_WDATA_REG, (uint16_t)ul_value);
        write_u16(BT_SSI_TRANS_DONE, BT_SSIWORKFLAG);

        uc_ret = read_u16(BT_SSI_TRANS_DONE) & BIT(0);
        while (uc_ret == BT_SSIWORKFLAG) {
            uc_ret = read_u16(BT_SSI_TRANS_DONE) & BIT(0);
        }
    } else {
        oml_ssi_set_baseaddr(ul_add);
        oml_ssi_write_one_data(((uint16_t)(ul_add & 0x0000FFFF)) >> 1, ul_value);
    }
}

static uint8_t oml_ssi_read_one_data(uint16_t usaddr, uint16_t *uldata)
{
    uint8_t uc_ret;

    write_u16(BT_SSI_ADDR_REG, usaddr);
    write_u16(BT_SSI_RW_REG, BT_SSIREAD);
    write_u16(BT_SSI_TRANS_DONE, BT_SSIWORKFLAG);

    while ((read_u16(BT_SSI_TRANS_DONE) & BIT(0)) == BT_SSIWORKFLAG) {
    }

    *uldata = read_u16(BT_SSI_RDATA_REG);

    if (((read_u16(BT_SSI_RD_ERR)) & (BIT(0))) == 0) {
        uc_ret = BT_CFG_SUCCESS;
    } else {
        uc_ret = BT_CFG_FAIL;
    }

    return uc_ret;
}

static uint16_t oml_ssi_read_reg(uint32_t ul_add)
{
    uint16_t ul_value = 0;
    uint16_t us_ssi_regaddr;

    if (ul_add < 0x36) {
        us_ssi_regaddr = (uint16_t)ul_add;
        us_ssi_regaddr = ((us_ssi_regaddr >> 1) | (BIT(15)));  // Read register bit 15
        write_u16(BT_SSI_ADDR_REG, us_ssi_regaddr);
        write_u16(BT_SSI_RW_REG, BT_SSIREAD);
        write_u16(BT_SSI_TRANS_DONE, BT_SSIWORKFLAG);

        while ((read_u16(BT_SSI_TRANS_DONE) & BIT(0)) == BT_SSIWORKFLAG) { }

        ul_value = read_u16(BT_SSI_RDATA_REG);
    } else if ((ul_add >= 0x30080000) && (ul_add <= 0x3008FFFF)) {
        ul_value = read_u16(ul_add);
    } else {
        oml_ssi_set_baseaddr(ul_add);
        oml_ssi_read_one_data(((uint16_t)(ul_add & 0x0000FFFF)) >> 1, &ul_value);
    }

    return ul_value;
}

static void oml_ssi_reg_write(uint8_t *data, uint16_t length)
{
    uint32_t qury_addr;
    uint32_t reg_value;
    struct om_wrssi_reg_data_stru_t *reg_request = NULL;
    UNUSED(length);

    reg_request = (struct om_wrssi_reg_data_stru_t *)data;
    qury_addr = reg_request->reg_addr;
    reg_value = reg_request->reg_value;

    oml_ssi_write_reg(qury_addr, (uint16_t)reg_value);
}

static void oml_ssi_reg_read(uint8_t *data, uint16_t length)
{
    uint32_t qury_addr;
    struct om_rdssi_reg_data_stru_t *reg_request = NULL;
    om_ssi_reg_output_stru_t reg_read_output;
    UNUSED(length);

    memset_s((void *)&reg_read_output, sizeof(reg_read_output), 0, sizeof(reg_read_output));

    reg_request = (struct om_rdssi_reg_data_stru_t *)data;
    qury_addr = reg_request->reg_addr;

    reg_read_output.header.frame_start = OM_FRAME_DELIMITER;
    reg_read_output.header.func_type = OM_MSG_SSI_REG_COMMAND;
    reg_read_output.header.prime_id = REG_METHOD_CNF;
    reg_read_output.header.frame_len = (uint16_t)sizeof(om_ssi_reg_output_stru_t);

    reg_read_output.output_value = oml_ssi_read_reg(qury_addr);
    reg_read_output.msg_tailer = OM_FRAME_DELIMITER;

    log_event((uint8_t *)&reg_read_output, reg_read_output.header.frame_len);
}

void oml_ssi_reg_command(uint8_t *data, uint16_t length)
{
    if (data == NULL) {
        return;
    }
    if (length == WRSSI_CMD_LEN) {
        oml_ssi_reg_write(data, length);
    } else if (length == RDSSI_CMD_LEN) {
        oml_ssi_reg_read(data, length);
    } else {
        return;
    }
}
#endif

void oml_ssi_reg_register_callback(void)
{
    log_oam_register_handler_callback(OM_MSG_SSI_REG_COMMAND, oml_ssi_reg_command);
}
#endif

#endif
