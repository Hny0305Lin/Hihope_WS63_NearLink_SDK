/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: diag mem adapt
 * This file should be changed only infrequently and with great care.
 */

#ifdef CONFIG_REG_WHITELIST
#include "diag_adapt_cmd_mem.h"
#include "soc_errno.h"
#endif
#include "diag_cmd_mem_read_write.h"

typedef struct diag_mem_config {
    uintptr_t start_addr;
    uintptr_t end_addr;
} diag_mem_config_t;

const diag_mem_config_t g_mem_config[] = {
    { 0x44210060, 0x44210064 }, // DURATION调整寄存器
    { 0x442100D0, 0x442100D4 }, // TXOP结束发送CF END的最小时间
    { 0x44210138, 0x4421013C }, // 旁路控制寄存器
    { 0x44210064, 0x4421006C }, // 接收Duration最大值
    { 0x44210098, 0x442100A0 }, // AC队列/MU时各队列的竞争窗口
    { 0x442100B0, 0x442100B8 }, // AC各队列TXOP LIMIT
    { 0x4400D868, 0x4400D880 }, // SFC
    { 0x00100000, 0x0017FFFF }, // ITCM
    { 0x00180000, 0x001C7FFF }, // DTCM
    { 0x00200000, 0x00800000 }, // FLASH
    { 0x00A00000, 0x00A987FF }  // RAM
};

static bool diag_permit_check(uintptr_t start_addr, uintptr_t end_addr)
{
    bool ret = false;
    uint32_t loop;

    for (loop = 0; loop < sizeof(g_mem_config) / sizeof(diag_mem_config_t); loop++) {
        if ((g_mem_config[loop].start_addr <= start_addr) && (g_mem_config[loop].end_addr >= end_addr)) {
            ret = true;
            break;
        }
    }
    return ret;
}

#ifdef CONFIG_REG_WHITELIST
osal_s32 reg_rw_check_addr(osal_u32 start_addr, osal_u32 bytes_cnt)
{
    bool ret = diag_permit_check(start_addr, start_addr + bytes_cnt);
    if (ret) {
        return EXT_SUCCESS;
    } else {
        return EXT_FAILURE;
    }
}
#endif

bool diag_cmd_permit_read(uintptr_t start_addr, uintptr_t end_addr)
{
    return diag_permit_check(start_addr, end_addr);
}

bool diag_cmd_permit_write(uintptr_t start_addr, uintptr_t end_addr)
{
    return diag_permit_check(start_addr, end_addr);
}
