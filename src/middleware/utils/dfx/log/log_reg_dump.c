/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description:   LOG REG DUMP MODULE
 * Author:
 * Create: 2020-09-15
 */

#include "log_reg_dump.h"
#include "chip_io.h"
#include "panic.h"
#include "log_oam_logger.h"
#include "log_oml_exception.h"
#include "log_trigger.h"

#if (defined (BUILD_APPLICATION_STANDARD) || defined (TEST_SUITE) || defined(BUILD_SENSOR_STANDARD))
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
typedef struct {
    uint32_t start_addr;
    uint32_t end_addr;
} log_reg_dump_t;

// Dump register address config
static log_reg_dump_t g_dump_addr[] = {
    { 0x57004000, 0x570047e0 },   // PMU1_CTL
    { 0x57008000, 0x57008490 },   // PMU2_CTL
    { 0x5702c000, 0x5702c418 },   // ULP_AON_CTL
};

#define FLAG_COUNT_SIZE 4
#define REG_OFFSET 4
#define FLAG_COUNT_OFFSET (FLAG_COUNT_SIZE - 1)
// ram_size = reg len + g_dump_reg_separate_flag size
#define RAM_SIZE_LEN (FLAG_COUNT_SIZE + 2)
#define RAM_SIZE_CALCULATION 2
// This is the index of the log dump
static uint8_t g_dump_reg_separate_flag[FLAG_COUNT_SIZE] = { 0xAA, 0xBB, 0xCC, 0x0 };
static bool g_dump_table_check_success = false;

void log_exception_dump_reg_check(void)
{
    uint32_t start_addr_check, end_addr_check, len_check;
    uint16_t count = (uint16_t)sizeof(g_dump_addr) / (uint16_t)sizeof(g_dump_addr[0]);

    for (uint16_t i = 0; i < count; i++) {
        start_addr_check = (g_dump_addr[i].start_addr & 0xf) % REG_OFFSET;
        end_addr_check = (g_dump_addr[i].end_addr & 0xf) % REG_OFFSET;
        len_check = (g_dump_addr[i].end_addr - g_dump_addr[i].start_addr) % REG_OFFSET;
        if ((start_addr_check != 0) || (end_addr_check != 0) || (len_check != 0)) {
            panic(PANIC_LOG_DUMP, __LINE__);
        }
    }

    g_dump_table_check_success = true;
}

static void log_exception_dump_print(uint32_t start_addr, uint32_t end_addr)
{
    om_msg_header_stru_t msg_header = { 0 };
    uint8_t msg_tail = OM_FRAME_DELIMITER;
    // ram_size = reg len + g_dump_reg_separate_flag size
    uint16_t ram_size = (uint16_t)((end_addr - start_addr) / RAM_SIZE_CALCULATION + RAM_SIZE_LEN);
    uint16_t reg_value, length;
    bool is_first = true;
    uint32_t start_addr_tmp = start_addr;

    msg_header.frame_start = OM_FRAME_DELIMITER;
    msg_header.func_type = OM_MSG_TYPE_LAST;
    msg_header.prime_id = OM_LOG_SAVE_STACK;

    while (ram_size > 0) {
        length = MIN(ram_size, DUMP_MAX_LENGTH_PER_TRANS);
        msg_header.frame_len = (uint16_t)(sizeof(om_msg_header_stru_t) + length + sizeof(msg_tail));
        /* Send exception stack */
        log_exception_send_data((uint8_t *)(&msg_header), sizeof(om_msg_header_stru_t));
        if (is_first) {
            // Send g_dump_reg_separate_flag
            log_exception_send_data((uint8_t *)(uintptr_t)g_dump_reg_separate_flag, sizeof(g_dump_reg_separate_flag));
            is_first = false;
            length -= (uint16_t)sizeof(g_dump_reg_separate_flag);
            ram_size -= (uint16_t)sizeof(g_dump_reg_separate_flag);
        }
        for (uint16_t i = 0; i < length;) {
            // Send register value
            reg_value = readw(start_addr_tmp);
            log_exception_send_data((uint8_t *)&reg_value, sizeof(reg_value));
            // Next reg addr
            start_addr_tmp += REG_OFFSET;
            i += (uint16_t)sizeof(reg_value);
        }
        log_exception_send_data((uint8_t *)(&msg_tail), sizeof(msg_tail));

        ram_size -= length;
    }

    // Flush log before reboot.
    log_trigger();
}

void log_exception_dump_reg(void)
{
    if (!g_dump_table_check_success) {
        return;
    }

    uint16_t count = (uint16_t)sizeof(g_dump_addr) / (uint16_t)sizeof(log_reg_dump_t);

    for (uint8_t i = 0; i < count; i++) {
        // Count flag value
        g_dump_reg_separate_flag[FLAG_COUNT_OFFSET] = i;
        log_exception_dump_print(g_dump_addr[i].start_addr, g_dump_addr[i].end_addr);
    }
}
#endif
#endif