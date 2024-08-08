/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides efuse driver source \n
 *
 * History: \n
 * 2023-3-4, Create file. \n
 */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "securec.h"
#include "common_def.h"
#include "hal_efuse.h"
#include "hal_efuse_v151_reg_op.h"
#include "efuse_porting.h"
#include "tcxo.h"

#define HAL_EFUSE_WRITE_MODE        0xa5a5
#define HAL_EFUSE_READ_MODE         0x5a5a
#define HAL_EFUSE_CLEAR_RESULT      0x0
#define HAL_EFUSE_REG_WIDTH         1U
#define HAL_EFUSE_REG_SHIFT         2U
#define HAL_EFUSE_REG_LENGTH        4U
#define HAL_EFUSE_REG_DATA_SHIFT    8U
#define HAL_EFUSE_REG_DATA_LENGTH   16
#define HAL_EFUSE_REG_VALID_DATA    0x1
#define HAL_EFUSE_REG_BYTE_OFFSET   1
#define HAL_EFUSE_BYTES_PRE_REG     2
#define HAL_EFUSE_BYTE_MASK         0xFF
#define HAL_EFUSE_REG_H_MASK        0xFF00
#define HAL_EFUSE_REG_L_MASK        0x00FF

#define HAL_EFUSE_POWER_ON_DELAY_US 120ULL
#define HAL_EFUSE_DELAY_US  100
#define HAL_EFUSE_CLOCK_PERIOD 0x1f
#if defined(CONFIG_EFUSE_SWITCH_EN)
#define HAL_EFUSE_SWITCH_ON_DELAY 200
#define HAL_EFUSE_SWITCH_OFF_DELAY 200
#define HAL_EFUSE_SWITCH_EN 0xa5a5
#define HAL_EFUSE_SWITCH_OFF 0x0
#endif

#define HAL_EFUSE0_BOOT_DONE 0x4

static uint32_t g_efuse_clock_period = HAL_EFUSE_CLOCK_PERIOD;
static bool g_efuse_is_init = false;

void hal_efuse_set_clock_period(uint32_t clock_period)
{
    g_efuse_clock_period = clock_period;
}

static errcode_t hal_efuse_init(void)
{
    hal_efuse_regs_init(0);
    hal_efuse_clock_period_set(0, g_efuse_clock_period);
#if CONFIG_EFUSE_REGION_NUM > EFUSE_REGION_INDEX1
    hal_efuse_regs_init(EFUSE_REGION_INDEX1);
    hal_efuse_clock_period_set(EFUSE_REGION_INDEX1, g_efuse_clock_period);
#endif
#if CONFIG_EFUSE_REGION_NUM > EFUSE_REGION_INDEX2
    hal_efuse_regs_init(EFUSE_REGION_INDEX2);
    hal_efuse_clock_period_set(EFUSE_REGION_INDEX2, g_efuse_clock_period);
#endif
    g_efuse_is_init = true;
    return ERRCODE_SUCC;
}

static void hal_efuse_deinit(void)
{
    g_efuse_is_init = false;
    return ;
}

static uint32_t hal_efuse_get_writeread_addr(uint32_t byte_addr)
{
    hal_efuse_region_t region = hal_efuse_get_region(byte_addr);
    uint16_t offset = hal_efuse_get_byte_offset(byte_addr);
    return g_efuse_region_read_address[region] + ((offset >> 1U) << HAL_EFUSE_REG_SHIFT);
}

static errcode_t hal_efuse_read_byte(uint32_t byte_address, uint8_t *value)
{
    if (g_efuse_is_init == false) {
        return ERRCODE_EFUSE_NOT_INIT;
    }

    if ((byte_address >= EFUSE_MAX_BYTES) || (value == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }
    volatile uint32_t *efuse_byte = (volatile uint32_t *)((uintptr_t)hal_efuse_get_writeread_addr(byte_address));
    hal_efuse_ctl_set(hal_efuse_get_region(byte_address), HAL_EFUSE_READ_MODE);
    if ((byte_address % HAL_EFUSE_BYTES_PRE_REG) != 0) {
        *value = (uint8_t)((*efuse_byte >> HAL_EFUSE_REG_DATA_SHIFT) & HAL_EFUSE_BYTE_MASK);
    } else {
        *value = (uint8_t)(*efuse_byte & HAL_EFUSE_BYTE_MASK);
    }
    return ERRCODE_SUCC;
}

static errcode_t hal_efuse_write_operation(uint32_t address, uint8_t value, hal_efuse_region_t region)
{
    unused(region);
    if (g_efuse_is_init == false) {
        return ERRCODE_EFUSE_NOT_INIT;
    }

    if (address >= EFUSE_MAX_BYTES) {
        return ERRCODE_INVALID_PARAM;
    }
#if defined(CONFIG_EFUSE_SWITCH_EN)
    hal_efuse_switch_en_set(HAL_EFUSE_SWITCH_EN);
    uapi_tcxo_delay_us(HAL_EFUSE_SWITCH_ON_DELAY);
#endif

    hal_efuse_ctl_set(hal_efuse_get_region(address), HAL_EFUSE_WRITE_MODE);
    hal_efuse_avdd_ctl_set(hal_efuse_get_region(address), 1);
    uapi_tcxo_delay_us(HAL_EFUSE_DELAY_US);
    volatile uint32_t *efuse_byte = (volatile uint32_t *)((uintptr_t)hal_efuse_get_writeread_addr(address));
    if ((address % HAL_EFUSE_BYTES_PRE_REG) != 0) {
        *efuse_byte = (uint16_t)(value) << HAL_EFUSE_REG_DATA_SHIFT;
    } else {
        *efuse_byte = (uint32_t)(value);
    }
    hal_efuse_avdd_ctl_set(hal_efuse_get_region(address), 0);
    uapi_tcxo_delay_us(HAL_EFUSE_DELAY_US);

#if defined(CONFIG_EFUSE_SWITCH_EN)
    uapi_tcxo_delay_us(HAL_EFUSE_SWITCH_OFF_DELAY);
    hal_efuse_switch_en_set(0);
#endif
    return ERRCODE_SUCC;
}

static errcode_t hal_efuse_write_buffer_operation(uint32_t address, const uint8_t *buffer, uint16_t length)
{
    if (g_efuse_is_init == false) {
        return ERRCODE_EFUSE_NOT_INIT;
    }

    for (uint32_t i = 0; i < length; i++) {
        if (hal_efuse_write_operation(address + i, buffer[i], 0) != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        }
    }

    return ERRCODE_SUCC;
}

static errcode_t hal_efuse_get_die_id(uint8_t *buffer, uint16_t length)
{
    if (g_efuse_is_init == false) {
        return ERRCODE_EFUSE_NOT_INIT;
    }
    for (uint32_t i = 0; i < length; i++) {
        if (hal_efuse_read_byte(EFUSE_DIE_ID_BASE_BYTE_ADDR + i, &buffer[i]) != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        }
    }
    return ERRCODE_SUCC;
}

hal_efuse_funcs_t g_hal_efuse_funcs = {
    .init = hal_efuse_init,
    .deinit = hal_efuse_deinit,
    .flush_write = NULL,
    .refresh_read = NULL,
    .read_byte = hal_efuse_read_byte,
    .write_byte = NULL,
    .clear = NULL,
    .write_op = hal_efuse_write_operation,
    .write_buffer_op = hal_efuse_write_buffer_operation,
    .get_die_id = hal_efuse_get_die_id,
    .get_chip_id = NULL
};

hal_efuse_funcs_t *hal_efuse_funcs_get(void)
{
    return &g_hal_efuse_funcs;
}
