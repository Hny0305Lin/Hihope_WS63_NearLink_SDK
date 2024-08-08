/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag memory read and write
 * This file should be changed only infrequently and with great care.
 */

#include "diag_cmd_mem_read_write.h"
#include "securec.h"
#include "diag_cmd_mem_read_write_st.h"
#include "soc_diag_cmd_id.h"
#include "errcode.h"

#define REPORT_DATA_PER_SIZE 64

typedef struct {
    mem_read_ind_head_t head;
    uint8_t data[REPORT_DATA_PER_SIZE];
} mem_read_ind_common_t;

__attribute__((weak)) bool diag_cmd_permit_read(uintptr_t start_addr, uintptr_t end_addr)
{
    unused(start_addr);
    unused(end_addr);
    return true;
}

__attribute__((weak)) bool diag_cmd_permit_write(uintptr_t start_addr, uintptr_t end_addr)
{
    unused(start_addr);
    unused(end_addr);
    return true;
}

errcode_t diag_cmd_mem32(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_param_size);
    mem_read_cmd_t cmd;
    mem_read32_ind_t ind;
    uint32_t num = 0;
    (void)memcpy_s(&cmd, sizeof(mem_read_cmd_t), cmd_param, sizeof(mem_read_cmd_t));
    (void)memset_s(&ind, sizeof(mem_read32_ind_t), 0, sizeof(mem_read32_ind_t));
    uintptr_t end_addr = cmd.start_addr + (cmd.cnt * sizeof(uint32_t));
    if (diag_cmd_permit_read(cmd.start_addr, end_addr) == false) {
        return ERRCODE_FAIL;
    }

    ind.head.start_addr = cmd.start_addr;
    while (ind.head.start_addr < end_addr) {
        ind.head.start_addr = ind.head.start_addr + num * sizeof(uint32_t);
        ind.head.size = uapi_min(end_addr - ind.head.start_addr, REPORT_DATA_PER_SIZE);
        num = ind.head.size / (uint32_t)sizeof(uint32_t);
        for (uint32_t c = 0; c < num; c++) {
            uintptr_t addr = ind.head.start_addr + (c * sizeof(uint32_t));
            uapi_reg_read32(addr, ind.data[c]);
        }
        (void)uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ind,
                                      (uint16_t)sizeof(mem_read_ind_head_t) + (uint16_t)ind.head.size, true);
    }
    return ERRCODE_SUCC;
}

errcode_t diag_cmd_mem16(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    mem_read_cmd_t cmd;
    mem_read16_ind_t ind;
    uint32_t num = 0;
    (void)memcpy_s(&cmd, sizeof(mem_read_cmd_t), cmd_param, sizeof(mem_read_cmd_t));
    (void)memset_s(&ind, sizeof(mem_read16_ind_t), 0, sizeof(mem_read16_ind_t));
    uintptr_t end_addr = cmd.start_addr + (cmd.cnt * sizeof(uint16_t));
    if (diag_cmd_permit_read(cmd.start_addr, end_addr) == false) {
        return ERRCODE_FAIL;
    }
    unused(cmd_param_size);
    ind.head.start_addr = cmd.start_addr;
    while (ind.head.start_addr < end_addr) {
        ind.head.start_addr = ind.head.start_addr + num * sizeof(uint16_t);
        ind.head.size = uapi_min(end_addr - ind.head.start_addr, REPORT_DATA_PER_SIZE);
        num = ind.head.size / (uint32_t)sizeof(uint16_t);
        for (uint32_t c = 0; c < num; c++) {
            uintptr_t addr = ind.head.start_addr + (c * sizeof(uint16_t));
            uapi_reg_read16(addr, ind.data[c]);
        }
        (void)uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ind,
                                      (uint16_t)sizeof(mem_read_ind_head_t) + (uint16_t)ind.head.size, true);
    }
    return ERRCODE_SUCC;
}

errcode_t diag_cmd_mem8(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    mem_read_cmd_t cmd;
    mem_read8_ind_t ind;
    uint32_t num = 0;
    (void)memcpy_s(&cmd, sizeof(mem_read_cmd_t), cmd_param, sizeof(mem_read_cmd_t));
    (void)memset_s(&ind, sizeof(mem_read8_ind_t), 0, sizeof(mem_read8_ind_t));
    uintptr_t end_addr = cmd.start_addr + (cmd.cnt * sizeof(uint8_t));
    if (diag_cmd_permit_read(cmd.start_addr, end_addr) == false) {
        return ERRCODE_FAIL;
    }
    unused(cmd_param_size);
    ind.head.start_addr = cmd.start_addr;
    while (ind.head.start_addr < end_addr) {
        ind.head.start_addr = ind.head.start_addr + num * sizeof(uint8_t);
        ind.head.size = uapi_min(end_addr - ind.head.start_addr, REPORT_DATA_PER_SIZE);
        num = ind.head.size / (uint32_t)sizeof(uint8_t);
        for (uint32_t c = 0; c < num; c++) {
            uintptr_t addr = ind.head.start_addr + (c * sizeof(uint8_t));
            uapi_reg_read8(addr, ind.data[c]);
        }
        (void)uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ind,
                                      (uint16_t)sizeof(mem_read_ind_head_t) + (uint16_t)ind.head.size, true);
    }
    return ERRCODE_SUCC;
}

errcode_t diag_cmd_w1(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    mem_write_cmd_t cmd;
    mem_write_ind_t ind;
    unused(cmd_param_size);

    (void)memcpy_s(&cmd, sizeof(mem_write_cmd_t), cmd_param, sizeof(mem_read_cmd_t));
    uapi_reg_write8(cmd.start_addr, (uint8_t)cmd.val);

    ind.ret = ERRCODE_SUCC;
    uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ind, (uint16_t)sizeof(mem_write_ind_t), true);
    return ERRCODE_SUCC;
}

errcode_t diag_cmd_w2(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    mem_write_cmd_t cmd;
    mem_write_ind_t ind;
    unused(cmd_param_size);

    (void)memcpy_s(&cmd, sizeof(mem_write_cmd_t), cmd_param, sizeof(mem_write_cmd_t));
    uapi_reg_write16(cmd.start_addr, (uint16_t)cmd.val);

    ind.ret = ERRCODE_SUCC;
    uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ind, (uint16_t)sizeof(mem_write_ind_t), true);
    return ERRCODE_SUCC;
}

errcode_t diag_cmd_w4(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    mem_write_cmd_t cmd;
    mem_write_ind_t ind;
    unused(cmd_param_size);

    (void)memcpy_s(&cmd, sizeof(mem_write_cmd_t), cmd_param, sizeof(mem_write_cmd_t));
    uapi_reg_write32(cmd.start_addr, cmd.val);

    ind.ret = ERRCODE_SUCC;
    uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ind, (uint16_t)sizeof(mem_write_ind_t), true);
    return ERRCODE_SUCC;
}

errcode_t diag_cmd_mem_operate(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    switch (cmd_id) {
        case DIAG_CMD_MEM_MEM32:
            return diag_cmd_mem32(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_CMD_MEM_MEM16:
            return diag_cmd_mem16(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_CMD_MEM_MEM8:
            return diag_cmd_mem8(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_CMD_MEM_W1:
            return diag_cmd_w1(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_CMD_MEM_W2:
            return diag_cmd_w2(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_CMD_MEM_W4:
            return diag_cmd_w4(cmd_id, cmd_param, cmd_param_size, option);
        default:
            return ERRCODE_NOT_SUPPORT;
    }
}
