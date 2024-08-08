/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V150 i2c common function. \n
 *
 * History: \n
 * 2023-03-06, Create file. \n
 */

#include "common_def.h"
#include "hal_i2c.h"
#include "hal_i2c_v150_regs_op.h"
#include "hal_i2c_v150_comm.h"


// 当前的传输信息，包含传输模式、已传输数据长度，剩余数据长度
static hal_i2c_trans_info_t g_hal_i2c_trans_info[I2C_BUS_MAX_NUM] = {
#if I2C_BUS_MAX_NUM > 1
    {I2C_TRANS_MODE_INVALID, 0, I2C_V150_DEFAULT_WRITE_NUM},
#if I2C_BUS_MAX_NUM > 2
    {I2C_TRANS_MODE_INVALID, 0, I2C_V150_DEFAULT_WRITE_NUM},
#if I2C_BUS_MAX_NUM > 3
    {I2C_TRANS_MODE_INVALID, 0, I2C_V150_DEFAULT_WRITE_NUM},
#endif
#endif
#endif
    {I2C_TRANS_MODE_INVALID, 0, I2C_V150_DEFAULT_WRITE_NUM}
};

/* I2C控制函数表 */
static hal_i2c_inner_ctrl_t *g_hal_i2c_ctrl_func[I2C_BUS_MAX_NUM];

/* I2C控制信息 */
static hal_i2c_ctrl_info_t g_hal_i2c_ctrl_info[I2C_BUS_MAX_NUM] = {
#if I2C_BUS_MAX_NUM > 1
    {I2C_FALSE, I2C_CFG_SCL_H_DEFAULT_VAL, I2C_CFG_SCL_L_DEFAULT_VAL, I2C_WORK_TYPE_POLL_NOFIFO,
        CONFIG_I2C_WAIT_CONDITION_TIMEOUT, I2C_SS_MODE_BAUDRATE_HIGH_LIMIT, NULL},
#if I2C_BUS_MAX_NUM > 2
    {I2C_FALSE, I2C_CFG_SCL_H_DEFAULT_VAL, I2C_CFG_SCL_L_DEFAULT_VAL, I2C_WORK_TYPE_POLL_NOFIFO,
        CONFIG_I2C_WAIT_CONDITION_TIMEOUT, I2C_SS_MODE_BAUDRATE_HIGH_LIMIT, NULL},
#if I2C_BUS_MAX_NUM > 3
    {I2C_FALSE, I2C_CFG_SCL_H_DEFAULT_VAL, I2C_CFG_SCL_L_DEFAULT_VAL, I2C_WORK_TYPE_POLL_NOFIFO,
        CONFIG_I2C_WAIT_CONDITION_TIMEOUT, I2C_SS_MODE_BAUDRATE_HIGH_LIMIT, NULL},
#endif
#endif
#endif
    {I2C_FALSE, I2C_CFG_SCL_H_DEFAULT_VAL, I2C_CFG_SCL_L_DEFAULT_VAL, I2C_WORK_TYPE_POLL_NOFIFO,
        CONFIG_I2C_WAIT_CONDITION_TIMEOUT, I2C_SS_MODE_BAUDRATE_HIGH_LIMIT, NULL}
};

/* I2C中断处理回调 */
hal_i2c_callback_t g_hal_i2c_callback = NULL;

/* 获取控制信息 */
hal_i2c_ctrl_info_t *hal_i2c_v150_get_ctrl_info(i2c_bus_t bus)
{
    return &g_hal_i2c_ctrl_info[bus];
}

/* 加载控制函数表 */
void hal_i2c_v150_load_ctrl_func(i2c_bus_t bus, hal_i2c_inner_ctrl_t *func_table)
{
    g_hal_i2c_ctrl_func[bus] = func_table;
}

/* 卸载控制函数表 */
void hal_i2c_v150_unload_ctrl_func(i2c_bus_t bus)
{
    g_hal_i2c_ctrl_func[bus] = NULL;
}

/* 加载中断处理回调 */
void hal_i2c_v150_register_callback(hal_i2c_callback_t callback)
{
    g_hal_i2c_callback = callback;
}

/* 获取传输信息 */
hal_i2c_trans_info_t *hal_i2c_v150_get_trans_info(i2c_bus_t bus)
{
    return &g_hal_i2c_trans_info[bus];
}

/* 框架适配用函数, 入参指针写入True */
errcode_t hal_i2c_v150_ctrl_check_default(i2c_bus_t bus, uintptr_t param)
{
    unused(bus);
    *(uint32_t *)param = I2C_TRUE;
    return ERRCODE_SUCC;
}

/* 框架适配用函数, 入参指针写入False */
errcode_t hal_i2c_v150_ctrl_check_default_false(i2c_bus_t bus, uintptr_t param)
{
    unused(bus);
    *(uint32_t *)param = I2C_FALSE;
    return ERRCODE_SUCC;
}

/* 框架适配用函数, 不处理参数 */
errcode_t hal_i2c_v150_ctrl_proc_default(i2c_bus_t bus, uintptr_t param)
{
    unused(bus);
    unused(param);
    return ERRCODE_SUCC;
}

/* 获取可发送字节数, 不使用FIFO默认全部发送 */
errcode_t hal_i2c_v150_get_write_num(i2c_bus_t bus, uintptr_t param)
{
    unused(bus);
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    *(uint32_t *)param = 1;  // 中断模式，每次写一个字节
#else
    *(uint32_t *)param = I2C_V150_DEFAULT_WRITE_NUM;
#endif
    return ERRCODE_SUCC;
}

/* 获取可读取字节数, 不使用FIFO默认全部读取 */
errcode_t hal_i2c_v150_get_read_num(i2c_bus_t bus, uintptr_t param)
{
    unused(bus);
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    *(uint32_t *)param = 1;  // 中断模式，每次读一个字节
#else
    *(uint32_t *)param = I2C_V150_DEFAULT_READ_NUM;
#endif
    return ERRCODE_SUCC;
}

#if !(defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1))
/* 进行操作后等待 I2C_INT_DONE 完成中断上报 */
static errcode_t hal_i2c_v150_wait(i2c_bus_t bus)
{
    uint32_t int_state;
    uint32_t time_out = 0;

    hal_i2c_ctrl_info_t *hal_i2c_ctrl_info = hal_i2c_v150_get_ctrl_info(bus);
    int_state = hal_i2c_v150_get_int_state(bus);
    while (((int_state & I2C_INT_TYPE_DONE) != I2C_INT_TYPE_DONE) && (time_out < hal_i2c_ctrl_info->timeout_us)) {
        time_out++;
        int_state = hal_i2c_v150_get_int_state(bus);
        osal_udelay(1);
    }
    if (time_out >= hal_i2c_ctrl_info->timeout_us) {
        hal_i2c_v150_clear_all_int(bus);
        return ERRCODE_I2C_TIMEOUT;
    }
    
    if (int_state & I2C_INT_TYPE_ACK_ERR) {
        hal_i2c_v150_clear_all_int(bus);
        return ERRCODE_I2C_ACK_ERR;
    }
    return ERRCODE_SUCC;
}

/* 发送停止信号, 并等待操作完成 */
static errcode_t hal_i2c_v150_stop(i2c_bus_t bus)
{
    uint32_t ret;

    hal_i2c_v150_set_command(bus, I2C_OP_STOP);

    ret = hal_i2c_v150_wait(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    hal_i2c_v150_clear_all_int(bus);

    return ERRCODE_SUCC;
}
#endif

/* 发送一个字节 */
errcode_t hal_i2c_v150_send_byte(i2c_bus_t bus, uint8_t data, uint8_t should_start)
{
    // 发送字节
    hal_i2c_v150_set_tx_data(bus, data);
    hal_i2c_v150_set_command(bus, ((should_start ? I2C_OP_START : 0) | I2C_OP_WRITE));

#if !(defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1))
    uint32_t ret;
    // 等待中断状态
    ret = hal_i2c_v150_wait(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    // 清除中断状态
    hal_i2c_v150_clear_int(bus, ((should_start ? I2C_INT_TYPE_START : 0) | I2C_INT_TYPE_TX | I2C_INT_TYPE_DONE));
#endif  /* CONFIG_I2C_SUPPORT_INT */

    return ERRCODE_SUCC;
}

/* 接收一个字节 */
errcode_t hal_i2c_v150_receive_byte(i2c_bus_t bus, uint8_t *data, uint32_t remain_len)
{
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    if (remain_len == 2) {  // 中断模式2表示即将收最后一个数据
        hal_i2c_v150_disable_ack(bus);
    }
    // 读取接收字节
    *data = hal_i2c_v150_get_rx_data(bus);
    if (remain_len > 1) {
        g_hal_i2c_trans_info[bus].trans_mode = I2C_TRANS_MODE_READ;
        hal_i2c_v150_set_command(bus, I2C_OP_READ);
    }

    // 最后一个字节结束后, 恢复ACK设置
    if (remain_len == 1) {
        hal_i2c_v150_enable_ack(bus);
    }
#else
    uint32_t ret;
    // 最后一个字节不发送ACK, 代表读取结束
    if (remain_len == 1) {
        hal_i2c_v150_disable_ack(bus);
    }

    // 接收字节
    hal_i2c_v150_set_command(bus, I2C_OP_READ);
    // 等待中断状态
    ret = hal_i2c_v150_wait(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    // 读取接收字节
    *data = hal_i2c_v150_get_rx_data(bus);
    // 清除中断状态
    hal_i2c_v150_clear_int(bus, (I2C_INT_TYPE_RX | I2C_INT_TYPE_DONE));

    // 最后一个字节结束后, 恢复ACK设置
    if (remain_len == 1) {
        hal_i2c_v150_enable_ack(bus);
    }
#endif  /* CONFIG_I2C_SUPPORT_INT */
    return ERRCODE_SUCC;
}

/* 循环发送, 每次发送一个字节 */
#pragma weak hal_i2c_write = hal_i2c_v150_write
errcode_t hal_i2c_v150_write(i2c_bus_t bus, hal_i2c_buffer_wrap_t *data)
{
    if (data->len == 0) {
        return ERRCODE_I2C_SEND_PARAM_INVALID;
    }

    uint8_t should_start;
    uint32_t ret;

#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    should_start = (data->restart_flag && (g_hal_i2c_trans_info[bus].trans_cnt == 0)) ? I2C_TRUE : I2C_FALSE;
    g_hal_i2c_trans_info[bus].trans_mode = I2C_TRANS_MODE_WRITE;
    (g_hal_i2c_trans_info[bus].trans_cnt)++;
    ret = hal_i2c_v150_send_byte(bus, data->buffer[0], should_start);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#else
    uint32_t byte_cnt;
    for (byte_cnt = 0; byte_cnt < data->len; byte_cnt++) {
        // restart_flag置位时, 在首个字节发送时产生start信号
        should_start = (data->restart_flag && (byte_cnt == 0)) ? I2C_TRUE : I2C_FALSE;

        // 发送字节
        ret = hal_i2c_v150_send_byte(bus, data->buffer[byte_cnt], should_start);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }
    // 发送完成后检查是否需要产生stop信号
    if (data->stop_flag) {
        ret = hal_i2c_v150_stop(bus);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }
#endif  /* CONFIG_I2C_SUPPORT_INT */
    return ERRCODE_SUCC;
}

/* 循环读取, 每次读取一个字节 */
#pragma weak hal_i2c_read = hal_i2c_v150_read
errcode_t hal_i2c_v150_read(i2c_bus_t bus, hal_i2c_buffer_wrap_t *data)
{
    // 不使用FIFO模式, 默认读一个字节
    if (data->len == 0) {
        return ERRCODE_I2C_RECEIVE_PARAM_INVALID;
    }

    uint32_t ret;
    uint32_t remain_len;
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    remain_len = g_hal_i2c_trans_info[bus].total_len - g_hal_i2c_trans_info[bus].trans_cnt;
    (g_hal_i2c_trans_info[bus].trans_cnt)++;
    ret = hal_i2c_v150_receive_byte(bus, &(data->buffer[0]), remain_len);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#else
    uint32_t byte_cnt;
    for (byte_cnt = 0; byte_cnt < data->len; byte_cnt++) {
        remain_len = data->len - byte_cnt;
        // 接收字节
        ret = hal_i2c_v150_receive_byte(bus, &(data->buffer[byte_cnt]), remain_len);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }
    // 读取完成后默认产生stop信号
    ret = hal_i2c_v150_stop(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#endif  /* CONFIG_I2C_SUPPORT_INT */
    return ERRCODE_SUCC;
}

/* i2c去初始化 */
#pragma weak hal_i2c_deinit = hal_i2c_v150_deinit
errcode_t hal_i2c_v150_deinit(i2c_bus_t bus)
{
    hal_i2c_ctrl_info_t *hal_i2c_ctrl_info = hal_i2c_v150_get_ctrl_info(bus);
    if (!hal_i2c_ctrl_info->init) {
        return ERRCODE_I2C_NOT_INIT;
    }

    hal_i2c_ctrl_info->init = I2C_FALSE;

    hal_i2c_v150_set_i2c_disable(bus);
    hal_i2c_v150_mask_main_int(bus);
    hal_i2c_v150_mask_all_int(bus);
    hal_i2c_v150_clear_all_int(bus);
    hal_i2c_v150_unload_ctrl_func(bus);
    hal_i2c_v150_regs_deinit(bus);

    return ERRCODE_SUCC;
}

void hal_i2c_v150_cfg_clk(i2c_bus_t bus, uint32_t baudrate, uint8_t scl_h, uint8_t scl_l)
{
    uint16_t clk_val;
    uint32_t i2c_clk = i2c_port_get_clock_value(bus);

    hal_i2c_v150_mask_main_int(bus);

    clk_val = (i2c_clk / (baudrate * 2)) * scl_h / (scl_l + scl_h) - 1; // 计算时波特率乘2
    hal_i2c_v150_set_scl_h(bus, clk_val);
    clk_val = (i2c_clk / (baudrate * 2)) * scl_l / (scl_l + scl_h) - 1; // 计算时波特率乘2
    hal_i2c_v150_set_scl_l(bus, clk_val);

    hal_i2c_v150_unmask_main_int(bus);
}

/* 初始化配置控制参数 */
void hal_i2c_v150_init_comp_param(i2c_bus_t bus)
{
    hal_i2c_ctrl_info_t *hal_i2c_ctrl_info = &g_hal_i2c_ctrl_info[bus];
    hal_i2c_ctrl_info->init = I2C_FALSE;
    hal_i2c_ctrl_info->cfg_scl_h = I2C_CFG_SCL_H_DEFAULT_VAL;
    hal_i2c_ctrl_info->cfg_scl_l = I2C_CFG_SCL_L_DEFAULT_VAL;
    hal_i2c_ctrl_info->work_type = I2C_WORK_TYPE_POLL_NOFIFO;
    hal_i2c_ctrl_info->timeout_us = CONFIG_I2C_WAIT_CONDITION_TIMEOUT;
    hal_i2c_ctrl_info->baudrate = I2C_SS_MODE_BAUDRATE_HIGH_LIMIT;
    hal_i2c_ctrl_info->ext = NULL;
}

#pragma weak hal_i2c_ctrl = hal_i2c_v150_ctrl
errcode_t hal_i2c_v150_ctrl(i2c_bus_t bus, hal_i2c_ctrl_id_t id, uintptr_t param)
{
    if (g_hal_i2c_ctrl_func[bus] == NULL) {
        return ERRCODE_I2C_NOT_INIT;
    }
    if (g_hal_i2c_ctrl_func[bus][id] != NULL) {
        return g_hal_i2c_ctrl_func[bus][id](bus, param);
    }
    return ERRCODE_I2C_NOT_IMPLEMENT;
}

/* 中断回调函数 */
void hal_i2c_v150_irq_handler(i2c_bus_t bus)
{
    unused(bus);
    if (!g_hal_i2c_callback) {
        return;
    }
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    volatile i2c_sr_data_t i2c_int_reg = (i2c_sr_data_t)g_i2c_regs[bus]->i2c_sr;
    g_i2c_regs[bus]->i2c_icr = i2c_int_reg.d32;  // 清除已产生中断

    if ((i2c_int_reg.d32 & I2C_INT_TYPE_DONE) == I2C_INT_TYPE_DONE) {
        if (g_hal_i2c_trans_info[bus].trans_mode == I2C_TRANS_MODE_WRITE_BEFORE_READ) {
            // 触发rx 读数据,只有一个字节时需要发disable ack？
            g_hal_i2c_trans_info[bus].trans_mode = I2C_TRANS_MODE_READ;
            if (g_hal_i2c_trans_info[bus].total_len == 1) {
                hal_i2c_v150_disable_ack(bus);
            }
            hal_i2c_v150_set_command(bus, I2C_OP_READ);
        } else if (g_hal_i2c_trans_info[bus].trans_mode == I2C_TRANS_MODE_WRITE) {
            g_hal_i2c_trans_info[bus].trans_mode = I2C_TRANS_MODE_INVALID;
            g_hal_i2c_callback(bus, I2C_EVT_TX_READY, NULL);
        } else if (g_hal_i2c_trans_info[bus].trans_mode == I2C_TRANS_MODE_READ) {
            g_hal_i2c_trans_info[bus].trans_mode = I2C_TRANS_MODE_INVALID;
            g_hal_i2c_callback(bus, I2C_EVT_RX_READY, NULL);
        }
    }
    
    if ((i2c_int_reg.d32 & I2C_INT_TYPE_STOP) == I2C_INT_TYPE_STOP &&
        g_hal_i2c_trans_info[bus].trans_mode == I2C_TRANS_MODE_STOP_AFTER_WRITE) {
        g_hal_i2c_trans_info[bus].trans_mode = I2C_TRANS_MODE_INVALID;
        g_hal_i2c_callback(bus, I2C_EVT_TRANSMITION_DONE, NULL);
    }

    if (hal_i2c_v150_is_bus_busy(bus) == 1) {
        g_hal_i2c_callback(bus, I2C_EVT_TRANSMITION_BUSY, NULL);
    }
    if ((i2c_int_reg.d32 & I2C_INT_TYPE_ACK_ERR) == I2C_INT_TYPE_ACK_ERR) {
        g_hal_i2c_callback(bus, I2C_EVT_TRANSMITION_ABRT, NULL);
    }
#endif
}