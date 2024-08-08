/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides HAL i2c \n
 *
 * History: \n
 * 2023-03-06, Create file. \n
 */

#include <stdint.h>
#include "securec.h"
#include "common_def.h"
#include "i2c_porting.h"
#include "hal_i2c.h"
#include "hal_i2c_v150_comm.h"
#include "hal_i2c_v150_master.h"

static hal_i2c_speed_mode_t hal_i2c_v150_get_speed_mode(uint32_t baudrate)
{
    if (baudrate <= I2C_SS_MODE_BAUDRATE_HIGH_LIMIT) {
        return I2C_SPEED_MODE_SS;
    } else if (baudrate <= I2C_FS_MODE_BAUDRATE_HIGH_LIMIT) {
        return I2C_SPEED_MODE_FS;
    }
    return I2C_SPEED_MODE_HS;
}

static uint8_t hal_i2c_v150_master_get_addr_type(uint16_t addr)
{
    if ((addr | I2C_7BIT_ADDR_MASK) == I2C_7BIT_ADDR_MASK) {
        return I2C_ADDR_TYPE_7BIT;
    } else if ((addr | I2C_10BIT_ADDR_MASK) == I2C_10BIT_ADDR_MASK) {
        return I2C_ADDR_TYPE_10BIT;
    } else {
        return I2C_ADDR_TYPE_INVALID;
    }
}

/* 发送7bit地址 */
static errcode_t hal_i2c_v150_master_send_7bit_addr(i2c_bus_t bus, uint16_t addr, uint8_t is_write)
{
    uint8_t addr_byte = ((addr & I2C_7BIT_ADDR_MASK) << 1) & I2C_ADDR_WRITE_TAG;
    if (is_write) {
        addr_byte &= I2C_ADDR_WRITE_TAG;
    } else {
        addr_byte |= I2C_ADDR_READ_TAG;
    }
    return hal_i2c_v150_send_byte(bus, addr_byte, I2C_TRUE);
}

/* 发送10bit地址 */
static errcode_t hal_i2c_v150_master_send_10bit_addr(i2c_bus_t bus, uint16_t addr, uint8_t is_write)
{
    uint8_t addr_byte_h = ((addr & I2C_10BIT_ADDR_MASK_H) >> I2C_10BIT_ADDR_MASK_H_OFFSET) | I2C_10BIT_ADDR_MASK_H_TAG;
    uint8_t addr_byte_l = (addr & I2C_10BIT_ADDR_MASK_L) >> I2C_10BIT_ADDR_MASK_L_OFFSET;
    uint32_t ret;

    if (is_write) {
        addr_byte_h &= I2C_ADDR_WRITE_TAG;
    } else {
        addr_byte_h |= I2C_ADDR_READ_TAG;
    }

    ret = hal_i2c_v150_send_byte(bus, addr_byte_h, I2C_TRUE);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return hal_i2c_v150_send_byte(bus, addr_byte_l, I2C_FALSE);
}

/* 配置目标器件地址 */
static errcode_t hal_i2c_v150_master_cfg_target_addr(i2c_bus_t bus, uint16_t addr, uint8_t is_write)
{
    uint32_t ret;
    uint8_t addr_type = hal_i2c_v150_master_get_addr_type(addr);
    if (addr_type == I2C_ADDR_TYPE_7BIT) {
        ret = hal_i2c_v150_master_send_7bit_addr(bus, addr, is_write);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    } else if (addr_type == I2C_ADDR_TYPE_10BIT) {
        ret = hal_i2c_v150_master_send_10bit_addr(bus, addr, is_write);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    } else {
        return ERRCODE_I2C_ADDRESS_INVLID;
    }

    return ERRCODE_SUCC;
}

static errcode_t hal_i2c_v150_master_operate_prepare(i2c_bus_t bus)
{
    hal_i2c_ctrl_info_t *hal_i2c_ctrl_info = hal_i2c_v150_get_ctrl_info(bus);

    /* 锁上后再次检查init标志 */
    if (!hal_i2c_ctrl_info->init) {
        return ERRCODE_I2C_NOT_INIT;
    }
    return ERRCODE_SUCC;
}

/* 写操作前准备处理 */
static errcode_t hal_i2c_v150_master_write_prepeare(i2c_bus_t bus, uintptr_t param)
{
    uint32_t ret;
    hal_i2c_prepare_config_t *cfg = (hal_i2c_prepare_config_t *)param;

    ret = hal_i2c_v150_master_operate_prepare(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    hal_i2c_trans_info_t *hal_i2c_trans_info = hal_i2c_v150_get_trans_info(bus);
    hal_i2c_trans_info->trans_cnt = 0;
    hal_i2c_trans_info->total_len = cfg->total_len;
    hal_i2c_trans_info->trans_mode = I2C_TRANS_MODE_WRITE;
#endif
    ret = hal_i2c_v150_master_cfg_target_addr(bus, cfg->addr, I2C_TRUE);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    hal_i2c_v150_clear_all_int(bus);

    return ERRCODE_SUCC;
}

/* 写操作后恢复处理 */
static errcode_t hal_i2c_v150_master_write_restore(i2c_bus_t bus, uintptr_t param)
{
    unused(param);
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    hal_i2c_trans_info_t *hal_i2c_trans_info = hal_i2c_v150_get_trans_info(bus);
    hal_i2c_trans_info->trans_mode = I2C_TRANS_MODE_STOP_AFTER_WRITE;
    hal_i2c_v150_set_command(bus, I2C_OP_STOP);
#endif
    hal_i2c_v150_clear_all_int(bus);
    return ERRCODE_SUCC;
}

/* 读操作前准备处理 */
static errcode_t hal_i2c_v150_master_read_prepeare(i2c_bus_t bus, uintptr_t param)
{
    uint32_t ret;
    hal_i2c_prepare_config_t *cfg = (hal_i2c_prepare_config_t *)param;

    ret = hal_i2c_v150_master_operate_prepare(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    hal_i2c_trans_info_t *hal_i2c_trans_info = hal_i2c_v150_get_trans_info(bus);
    hal_i2c_trans_info->trans_cnt = 0;
    hal_i2c_trans_info->total_len = cfg->total_len;
    hal_i2c_trans_info->trans_mode = I2C_TRANS_MODE_WRITE_BEFORE_READ;
#endif
    ret = hal_i2c_v150_master_cfg_target_addr(bus, cfg->addr, I2C_FALSE);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    hal_i2c_v150_clear_all_int(bus);

    return ERRCODE_SUCC;
}

/* 读操作后恢复处理 */
static errcode_t hal_i2c_v150_master_read_restore(i2c_bus_t bus, uintptr_t param)
{
    unused(param);
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    hal_i2c_v150_set_command(bus, I2C_OP_STOP);
#endif
    hal_i2c_v150_clear_all_int(bus);
    return ERRCODE_SUCC;
}

static hal_i2c_inner_ctrl_t g_hal_i2c_master_ctrl_func_array[I2C_CTRL_MAX] = {
    hal_i2c_v150_master_write_prepeare,     /* I2C_CTRL_WRITE_PREPARE */
    hal_i2c_v150_master_write_restore,      /* I2C_CTRL_WRITE_RESTORE */
    hal_i2c_v150_master_read_prepeare,      /* I2C_CTRL_READ_PREPARE */
    hal_i2c_v150_master_read_restore,       /* I2C_CTRL_READ_RESTORE */
    hal_i2c_v150_get_write_num,             /* I2C_CTRL_GET_WRITE_NUM */
    hal_i2c_v150_get_read_num,              /* I2C_CTRL_GET_READ_NUM */
    hal_i2c_v150_ctrl_check_default,        /* I2C_CTRL_CHECK_TX_AVAILABLE */
    hal_i2c_v150_ctrl_check_default,        /* I2C_CTRL_CHECK_RX_AVAILABLE */
    hal_i2c_v150_ctrl_proc_default,         /* I2C_CTRL_FLUSH_RX_FIFO */
    hal_i2c_v150_ctrl_check_default,        /* I2C_CTRL_CHECK_TX_PROCESS_DONE */
    hal_i2c_v150_ctrl_check_default,        /* I2C_CTRL_CHECK_RX_PROCESS_DONE */
    hal_i2c_v150_ctrl_check_default,        /* I2C_CTRL_CHECK_RESTART_READY */
    hal_i2c_v150_ctrl_check_default_false,  /* I2C_CTRL_CHECK_TRANSMIT_ABRT */
    hal_i2c_v150_ctrl_check_default_false,  /* I2C_CTRL_GET_DMA_DATA_ADDR */
    hal_i2c_v150_ctrl_proc_default,         /* I2C_CTRL_CHECK_TX_FIFO_EMPTY */
};

#pragma weak hal_i2c_master_init = hal_i2c_v150_master_init
errcode_t hal_i2c_v150_master_init(i2c_bus_t bus, uint32_t baudrate, uint8_t hscode, hal_i2c_callback_t callback)
{
    unused(hscode);

    hal_i2c_speed_mode_t speed_mode = hal_i2c_v150_get_speed_mode(baudrate);
    if (speed_mode == I2C_SPEED_MODE_HS) {
        return ERRCODE_I2C_RATE_INVALID;
    }

    hal_i2c_ctrl_info_t *hal_i2c_ctrl_info = hal_i2c_v150_get_ctrl_info(bus);
    if (hal_i2c_ctrl_info->init) {
        // 根据驱动层逻辑, 在已初始化的情况下再次初始化视为设置波特率
        hal_i2c_v150_cfg_clk(bus, baudrate, hal_i2c_ctrl_info->cfg_scl_h, hal_i2c_ctrl_info->cfg_scl_l);
        return ERRCODE_SUCC;
    }

    hal_i2c_v150_regs_init(bus);
    hal_i2c_v150_reset_all_regs(bus);
    hal_i2c_v150_cfg_clk(bus, baudrate, hal_i2c_ctrl_info->cfg_scl_h, hal_i2c_ctrl_info->cfg_scl_l);
    hal_i2c_v150_set_ftrper(bus, I2C_FTRPER_STANDARD_VAL);
    hal_i2c_v150_unmask_all_int(bus);
    hal_i2c_v150_unmask_main_int(bus);
    hal_i2c_v150_load_ctrl_func(bus, g_hal_i2c_master_ctrl_func_array);
    hal_i2c_v150_register_callback(callback);
    hal_i2c_v150_set_i2c_enable(bus);

    hal_i2c_ctrl_info->baudrate = baudrate;
    hal_i2c_ctrl_info->init = I2C_TRUE;

    return ERRCODE_SUCC;
}

#pragma weak hal_i2c_slave_init = hal_i2c_v150_slave_init
errcode_t hal_i2c_v150_slave_init(i2c_bus_t bus, uint32_t baudrate, uint16_t addr, hal_i2c_callback_t callback)
{
    unused(bus);
    unused(baudrate);
    unused(addr);
    unused(callback);
    return ERRCODE_NOT_SUPPORT;
}