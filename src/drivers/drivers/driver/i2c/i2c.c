/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides i2c driver source \n
 *
 * History: \n
 * 2022-08-15, Create file. \n
 */

#include <stdbool.h>
#include "soc_osal.h"
#include "securec.h"
#include "common_def.h"
#include "tcxo.h"
#include "i2c.h"
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#include "dma.h"
#include "hal_dma.h"
#endif  /* CONFIG_I2C_SUPPORT_DMA */

#define I2C_DEFAULT_DEV_ADDRESS  0x0

typedef struct i2c_ctrl_block_info {
    bool init_flag;              /* When the I2C bus is initialized, the flag is true. */
    bool master_flag;            /* when init_flag is True, the flag can be valid. */
    uint8_t hscode;              /* High speeed mode master code. */
    uint8_t operate_type;        /* I2C data operation type. */
    uint16_t addr;               /* I2C slave address, valid for slave mode. */
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    i2c_irq_callback_t  callback;
#endif
} i2c_ctrl_block_info_t;

static i2c_ctrl_block_info_t g_i2c_ctrl[I2C_BUS_MAX_NUM];

#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
typedef struct i2c_dma_trans_inf {
    bool trans_succ;
    uint8_t read_channel;
    uint8_t write_channel;
    osal_semaphore dma_sem;
} i2c_dma_trans_inf_t;

static i2c_dma_trans_inf_t g_dma_trans[I2C_BUS_NONE] = { 0 };
#endif  /* CONFIG_I2C_SUPPORT_DMA */

#if defined(CONFIG_I2C_SUPPORT_CONCURRENCY) && (CONFIG_I2C_SUPPORT_CONCURRENCY == 1)
static osal_mutex g_i2c_mutex[I2C_BUS_NONE];
#endif  /* CONFIG_I2C_SUPPORT_CONCURRENCY */

#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
static osal_semaphore g_i2c_int_sem[I2C_BUS_MAX_NUM];
static hal_i2c_buffer_wrap_t g_i2c_buffer_wrap[I2C_BUS_MAX_NUM];
#endif  /* CONFIG_I2C_SUPPORT_INT */

static errcode_t i2c_bus_init_check(i2c_bus_t bus)
{
    if (bus >= I2C_BUS_MAX_NUMBER) {
        return ERRCODE_I2C_INVALID_PARAMETER;
    }

    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];
    if (i2c_ctrl->init_flag == false) {
        return ERRCODE_I2C_NOT_INIT;
    }
    return ERRCODE_SUCC;
}

static errcode_t i2c_write_pre_check(i2c_bus_t bus, const i2c_data_t *data)
{
    if ((data == NULL) || (data->send_buf == NULL) || (data->send_len == 0)) {
        return ERRCODE_I2C_SEND_PARAM_INVALID;
    }

    errcode_t ret = i2c_bus_init_check(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
}

static errcode_t i2c_read_pre_check(i2c_bus_t bus, const i2c_data_t *data)
{
    if ((data == NULL) || (data->receive_buf == NULL) || (data->receive_len == 0)) {
        return ERRCODE_I2C_RECEIVE_PARAM_INVALID;
    }

    errcode_t ret = i2c_bus_init_check(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
}

static inline uint32_t i2c_event_get_mask(hal_i2c_evt_id_t evt_id)
{
    return (1 << (uint32_t)evt_id);
}

static inline uint32_t i2c_ctrl_get_mask(hal_i2c_ctrl_id_t ctrl_id)
{
    return (1 << (uint32_t)ctrl_id);
}

static inline bool i2c_ctrl_get_id(uint32_t ctrl_mask, hal_i2c_ctrl_id_t ctrl_id)
{
    return (ctrl_mask >> (uint32_t)ctrl_id) & 1;
}

static errcode_t i2c_comm_ctrl_check(i2c_bus_t bus, hal_i2c_ctrl_id_t id)
{
    uint32_t flag = false;
    errcode_t ret = hal_i2c_ctrl(bus, id, (uintptr_t)&flag);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    if (flag == true) {
        return ERRCODE_SUCC;
    }
    return ERRCODE_I2C_WAIT_CONTINUE;
}

static errcode_t i2c_ctrl_wait(i2c_bus_t bus, uint32_t ctrl_mask, uint32_t timeout)
{
    errcode_t ret;
    uint32_t tmp_ctrl_mask = ctrl_mask;
    uint64_t start_time = uapi_tcxo_get_ms();

    do {
        for (uint32_t loop = 0; loop <= (uint32_t)I2C_CTRL_NORMAL_MAX; loop++) {
            if (i2c_ctrl_get_id(tmp_ctrl_mask, loop) == false) {
                continue;
            }
            ret = i2c_comm_ctrl_check(bus, loop);
            if (ret == ERRCODE_SUCC) {
                tmp_ctrl_mask &= ~(1 << loop);
            } else if (ret == ERRCODE_I2C_WAIT_CONTINUE) {
                continue;
            }
        }

        ret = i2c_comm_ctrl_check(bus, I2C_CTRL_CHECK_TRANSMIT_ABRT);
        if (ret == ERRCODE_SUCC) {
            return ERRCODE_I2C_WAIT_EXCEPTION;
        }

        if ((uapi_tcxo_get_ms() - start_time) > timeout) {
            return ERRCODE_I2C_TIMEOUT;
        }
    } while (tmp_ctrl_mask != 0);
    return ERRCODE_SUCC;
}

static errcode_t i2c_wait(i2c_bus_t bus, i2c_wait_condition_t *condition, uint32_t timeout)
{
    errcode_t ret = ERRCODE_SUCC;
    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];

    if (i2c_ctrl->operate_type == I2C_DATA_OPERATION_TYPE_POLL) {
        ret = i2c_ctrl_wait(bus, condition->ctrl_mask, timeout);
    }
    return ret;
}

static errcode_t i2c_write(i2c_bus_t bus, hal_i2c_buffer_wrap_t *buffer_wrap)
{
    errcode_t ret = ERRCODE_SUCC;
    uint32_t real_write_num = 0;
    uint8_t *buffer = buffer_wrap->buffer;
    uint32_t len = buffer_wrap->len;
    i2c_wait_condition_t condition = { 0 };

    uint32_t burst_num = 0;
    hal_i2c_ctrl(bus, I2C_CTRL_GET_WRITE_NUM, (uintptr_t)&burst_num);
    if (burst_num == 0) {
        return ret;
    }

    while (real_write_num < len) {
        condition.ctrl_mask = i2c_ctrl_get_mask(I2C_CTRL_CHECK_TX_AVAILABLE);
        condition.evt_mask = i2c_event_get_mask(I2C_EVT_TX_AVAILABLE) | i2c_event_get_mask(I2C_EVT_TX_READY);
        ret = i2c_wait(bus, &condition, CONFIG_I2C_WAIT_CONDITION_TIMEOUT);
        if (ret != ERRCODE_SUCC) {
            break;
        }

        uint32_t remain_num = len - real_write_num;
        hal_i2c_buffer_wrap_t write_data = { 0 };
        if (remain_num > burst_num) {
            write_data.len = burst_num;
        } else {
            write_data.len = remain_num;
            write_data.stop_flag = buffer_wrap->stop_flag;
        }

        if (real_write_num == 0) {
            write_data.restart_flag = buffer_wrap->restart_flag;
        }

        write_data.buffer = &buffer[real_write_num];
        unsigned int irq_sts = i2c_porting_lock(bus);
        ret = hal_i2c_write(bus, &write_data);
        i2c_porting_unlock(bus, irq_sts);
        if (ret != ERRCODE_SUCC) {
            break;
        }

        real_write_num += write_data.len;
    }

    return ret;
}

static errcode_t i2c_read(i2c_bus_t bus, hal_i2c_buffer_wrap_t *buffer_wrap)
{
    uint8_t *buffer = buffer_wrap->buffer;
    uint32_t len = buffer_wrap->len;
    uint32_t real_read_num = 0;
    i2c_wait_condition_t condition = { 0 };
    errcode_t ret = ERRCODE_SUCC;

    while (real_read_num < len) {
        uint32_t burst_num = 0;
        hal_i2c_ctrl(bus, I2C_CTRL_GET_READ_NUM, (uintptr_t)&burst_num);

        uint32_t remain_num = len - real_read_num;
        hal_i2c_buffer_wrap_t read_data = { 0 };
        if (remain_num > burst_num) {
            read_data.len = burst_num;
        } else {
            read_data.len = remain_num;
            read_data.stop_flag = buffer_wrap->stop_flag;
        }

        if (real_read_num == 0) {
            read_data.restart_flag = buffer_wrap->restart_flag;
        }

        ret = hal_i2c_ctrl(bus, I2C_CTRL_FLUSH_RX_FIFO, (uintptr_t)&read_data);
        if (ret == ERRCODE_SUCC) {
            condition.ctrl_mask = i2c_ctrl_get_mask(I2C_CTRL_CHECK_RX_AVAILABLE);
            condition.evt_mask = i2c_event_get_mask(I2C_EVT_RX_READY);
            (void)i2c_wait(bus, &condition, CONFIG_I2C_WAIT_CONDITION_TIMEOUT);
        }

        read_data.buffer = &buffer[real_read_num];
        unsigned int irq_sts = i2c_porting_lock(bus);
        ret = hal_i2c_read(bus, &read_data);
        i2c_porting_unlock(bus, irq_sts);
        if (ret != ERRCODE_SUCC) {
            break;
        }
        real_read_num += read_data.len;
    }
    return ret;
}

static errcode_t i2c_poll_write(i2c_bus_t bus, uint16_t dev_addr, hal_i2c_buffer_wrap_t *data_cfg, bool end_flag)
{
    hal_i2c_prepare_config_t cfg = { 0 };
    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];

    cfg.addr = dev_addr;
    cfg.operation_type = I2C_DATA_OPERATION_TYPE_POLL;
    i2c_ctrl->operate_type = cfg.operation_type;

    errcode_t ret = hal_i2c_ctrl(bus, I2C_CTRL_WRITE_PREPARE, (uintptr_t)&cfg);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = i2c_write(bus, data_cfg);
    if (ret != ERRCODE_SUCC) {
        (void)hal_i2c_ctrl(bus, I2C_CTRL_WRITE_RESTORE, (uintptr_t)NULL);
        return ret;
    }

    i2c_wait_condition_t condition = { 0 };
    if (end_flag) {
        condition.ctrl_mask = i2c_ctrl_get_mask(I2C_CTRL_CHECK_TX_PROCESS_DONE);
    }
    ret = i2c_wait(bus, &condition, CONFIG_I2C_WAIT_CONDITION_TIMEOUT);

    (void)hal_i2c_ctrl(bus, I2C_CTRL_WRITE_RESTORE, (uintptr_t)NULL);
    return ret;
}

static errcode_t i2c_poll_read(i2c_bus_t bus, uint16_t dev_addr, hal_i2c_buffer_wrap_t *data_cfg, bool end_flag)
{
    unused(end_flag);
    hal_i2c_prepare_config_t cfg = { 0 };
    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];

    cfg.addr = dev_addr;
    cfg.operation_type = I2C_DATA_OPERATION_TYPE_POLL;
    i2c_ctrl->operate_type = cfg.operation_type;

    errcode_t ret = hal_i2c_ctrl(bus, I2C_CTRL_READ_PREPARE, (uintptr_t)&cfg);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = i2c_read(bus, data_cfg);
    if (ret != ERRCODE_SUCC) {
        (void)hal_i2c_ctrl(bus, I2C_CTRL_READ_RESTORE, (uintptr_t)NULL);
        return ret;
    }

    i2c_wait_condition_t condition = { 0 };
    condition.ctrl_mask = i2c_ctrl_get_mask(I2C_CTRL_CHECK_RX_PROCESS_DONE);

    ret = i2c_wait(bus, &condition, CONFIG_I2C_WAIT_CONDITION_TIMEOUT);

    (void)hal_i2c_ctrl(bus, I2C_CTRL_READ_RESTORE, (uintptr_t)NULL);
    return ret;
}

#if (defined(CONFIG_I2C_SUPPORT_DMA) || defined(CONFIG_I2C_SUPPORT_INT))
static int i2c_int_mode_down(osal_semaphore *sem)
{
    unused(sem);
    int osal_value = OSAL_SUCCESS;
#if !defined(CONFIG_I2C_SUPPORT_IN_CHIP_LOOPBACK)
    osal_value = osal_sem_down(sem);
#endif
    return osal_value;
}

static void i2c_int_mode_up(osal_semaphore *sem)
{
    unused(sem);
#if !defined(CONFIG_I2C_SUPPORT_IN_CHIP_LOOPBACK)
    osal_sem_up(sem);
#endif
}
#endif

#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
static void i2c_dma_isr(uint8_t int_type, uint8_t ch, uintptr_t arg)
{
    unused(arg);
    uint8_t bus = I2C_BUS_NONE;
    for (uint8_t i = I2C_BUS_0; i < I2C_BUS_NONE; i++) {
        /* channel default value is 0, means not used. channel > 0 means used.
           So ch + 1 will not misjudgment with channel value 0. */
        if (g_dma_trans[i].read_channel == ch + 1) {
            bus = i;
            break;
        }
        if (g_dma_trans[i].write_channel == ch + 1) {
            bus = i;
            break;
        }
    }

    if (bus != I2C_BUS_NONE) {
        if (int_type == 0) {
            g_dma_trans[bus].trans_succ = true;
        }
        i2c_int_mode_up(&(g_dma_trans[bus].dma_sem));
    }
}

static void i2c_write_by_dma_config(i2c_bus_t bus, uint16_t *buffer,
                                    uint32_t length,
                                    dma_ch_user_peripheral_config_t *user_cfg)
{
    uint32_t data_addr = 0;
    hal_i2c_ctrl(bus, I2C_CTRL_GET_DMA_DATA_ADDR, (uintptr_t)(&data_addr));

    user_cfg->src = (uint32_t)(uintptr_t)buffer;
    user_cfg->dest = (uint32_t)data_addr;
    user_cfg->transfer_num = (uint16_t)length;
    user_cfg->src_handshaking = 0;
    user_cfg->trans_type = HAL_DMA_TRANS_MEMORY_TO_PERIPHERAL_DMA;
    user_cfg->trans_dir = HAL_DMA_TRANSFER_DIR_MEM_TO_PERIPHERAL;
    user_cfg->priority = HAL_DMA_CH_PRIORITY_0;
    user_cfg->src_width = HAL_DMA_TRANSFER_WIDTH_8;
    user_cfg->dest_width = HAL_DMA_TRANSFER_WIDTH_8;
    user_cfg->burst_length = HAL_DMA_BURST_TRANSACTION_LENGTH_4;
    user_cfg->src_increment = HAL_DMA_ADDRESS_INC_INCREMENT;
    user_cfg->dest_increment = HAL_DMA_ADDRESS_INC_NO_CHANGE;
    user_cfg->protection = HAL_DMA_PROTECTION_CONTROL_PRIVILEGED;
    user_cfg->dest_handshaking = (uint16_t)i2c_port_get_dma_trans_dest_handshaking(bus);
}

static void i2c_read_by_dma_config(i2c_bus_t bus,
                                   uint8_t *buffer,
                                   uint32_t length,
                                   dma_ch_user_peripheral_config_t *user_cfg)
{
    uint32_t data_addr = 0;
    hal_i2c_ctrl(bus, I2C_CTRL_GET_DMA_DATA_ADDR, (uintptr_t)(&data_addr));

    user_cfg->src = (uint32_t)data_addr;
    user_cfg->dest = (uint32_t)(uintptr_t)buffer;
    user_cfg->transfer_num = (uint16_t)length;
    user_cfg->src_handshaking = (uint16_t)i2c_port_get_dma_trans_src_handshaking(bus);
    user_cfg->trans_type = HAL_DMA_TRANS_PERIPHERAL_TO_MEMORY_DMA;
    user_cfg->trans_dir = HAL_DMA_TRANSFER_DIR_PERIPHERAL_TO_MEM;
    user_cfg->priority = HAL_DMA_CH_PRIORITY_0;
    user_cfg->src_width = HAL_DMA_TRANSFER_WIDTH_8;
    user_cfg->dest_width = HAL_DMA_TRANSFER_WIDTH_8;
    user_cfg->burst_length = HAL_DMA_BURST_TRANSACTION_LENGTH_4;
    user_cfg->src_increment = HAL_DMA_ADDRESS_INC_NO_CHANGE;
    user_cfg->dest_increment = HAL_DMA_ADDRESS_INC_INCREMENT;
    user_cfg->protection = HAL_DMA_PROTECTION_CONTROL_BUFFERABLE;
    user_cfg->dest_handshaking = 0;
}

static errcode_t i2c_start_transmit_dma(i2c_bus_t bus, uint8_t dma_ch)
{
    g_dma_trans[bus].write_channel = dma_ch + 1;
    g_dma_trans[bus].trans_succ = false;
    if (uapi_dma_start_transfer(dma_ch) != ERRCODE_SUCC) {
        g_dma_trans[bus].write_channel = 0;
        return ERRCODE_I2C_DMA_CONFIG_ERROR;
    }
    return ERRCODE_SUCC;
}

static errcode_t i2c_transmit_dma(i2c_bus_t bus, hal_i2c_buffer_wrap_t *data_cfg)
{
    uint8_t dma_ch;
    dma_ch_user_peripheral_config_t user_cfg = {0};

    i2c_write_by_dma_config(bus, (uint16_t*)data_cfg->buffer, (uint16_t)data_cfg->len, &user_cfg);

    if (uapi_dma_configure_peripheral_transfer_single(&user_cfg, &dma_ch,
        i2c_dma_isr, (uintptr_t)NULL) != ERRCODE_SUCC) {
        return ERRCODE_I2C_DMA_CONFIG_ERROR;
    }

    return i2c_start_transmit_dma(bus, dma_ch);
}

static errcode_t i2c_transmit_read_cmd_by_dma(i2c_bus_t bus, hal_i2c_buffer_wrap_t *data_cfg)
{
    uint8_t dma_ch;
    static uint16_t read_cmd = 0x100;
    dma_ch_user_peripheral_config_t user_cfg = {0};

    i2c_write_by_dma_config(bus, &read_cmd, data_cfg->len, &user_cfg);
    user_cfg.src_width = HAL_DMA_TRANSFER_WIDTH_16;
    user_cfg.dest_width = HAL_DMA_TRANSFER_WIDTH_16;
    user_cfg.src_increment = HAL_DMA_ADDRESS_INC_NO_CHANGE;

    if (uapi_dma_configure_peripheral_transfer_single(&user_cfg, &dma_ch,
        i2c_dma_isr, (uintptr_t)NULL) != ERRCODE_SUCC) {
        return ERRCODE_I2C_DMA_CONFIG_ERROR;
    }

    return i2c_start_transmit_dma(bus, dma_ch);
}

static errcode_t i2c_start_recv_dma(i2c_bus_t bus, hal_i2c_buffer_wrap_t *data_cfg)
{
    uint8_t dma_ch;
    dma_ch_user_peripheral_config_t user_cfg = {0};

    i2c_read_by_dma_config(bus, data_cfg->buffer, data_cfg->len, &user_cfg);

    if (uapi_dma_configure_peripheral_transfer_single(&user_cfg, &dma_ch,
        i2c_dma_isr, (uintptr_t)NULL) != ERRCODE_SUCC) {
        return ERRCODE_I2C_DMA_CONFIG_ERROR;
    }

    g_dma_trans[bus].read_channel = dma_ch + 1;
    g_dma_trans[bus].trans_succ = false;
    if (uapi_dma_start_transfer(dma_ch) != ERRCODE_SUCC) {
        g_dma_trans[bus].read_channel = 0;
        return ERRCODE_I2C_DMA_CONFIG_ERROR;
    }
    return ERRCODE_SUCC;
}

static errcode_t i2c_write_by_dma(i2c_bus_t bus, uint16_t dev_addr, hal_i2c_buffer_wrap_t *data)
{
    hal_i2c_prepare_config_t cfg = { 0 };
    hal_i2c_buffer_wrap_t buffer_wrap;
    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];

    cfg.addr = dev_addr;
    cfg.operation_type = I2C_DATA_OPERATION_TYPE_DMA;
    i2c_ctrl->operate_type = cfg.operation_type;
    errcode_t ret = hal_i2c_ctrl(bus, I2C_CTRL_WRITE_PREPARE, (uintptr_t)&cfg);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    buffer_wrap.buffer = data->buffer;
    buffer_wrap.len = data->stop_flag ? data->len - 1 : data->len;

    /* if the lenth is 0 here, send stop byte straightforward */
    if (buffer_wrap.len != 0) {
        ret = i2c_transmit_dma(bus, &buffer_wrap);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }

        /* wait for trans */
        if (i2c_int_mode_down(&(g_dma_trans[bus].dma_sem)) != OSAL_SUCCESS) {
            g_dma_trans[bus].write_channel = 0;
            return ERRCODE_I2C_DMA_TRANSFER_ERROR;
        }
        g_dma_trans[bus].write_channel = 0;
        if (!g_dma_trans[bus].trans_succ) {
            return ERRCODE_I2C_DMA_TRANSFER_ERROR;
        }
    }

    if (data->stop_flag) {
        buffer_wrap.buffer = data->buffer + data->len - 1;
        buffer_wrap.len = 1;
        buffer_wrap.restart_flag = false;
        buffer_wrap.stop_flag = true;

        ret = i2c_write(bus, &buffer_wrap);
        if (ret != ERRCODE_SUCC) {
            (void)hal_i2c_ctrl(bus, I2C_CTRL_WRITE_RESTORE, (uintptr_t)NULL);
            return ret;
        }

        i2c_wait_condition_t condition = { 0 };
        condition.ctrl_mask = i2c_ctrl_get_mask(I2C_CTRL_CHECK_TX_PROCESS_DONE);
        ret = i2c_wait(bus, &condition, CONFIG_I2C_WAIT_CONDITION_TIMEOUT);
    }

    (void)hal_i2c_ctrl(bus, I2C_CTRL_WRITE_RESTORE, (uintptr_t)NULL);

    return ret;
}

static errcode_t i2c_read_by_dma(i2c_bus_t bus, uint16_t dev_addr, hal_i2c_buffer_wrap_t *data)
{
    hal_i2c_prepare_config_t cfg = { 0 };
    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];
    cfg.addr = dev_addr;
    cfg.operation_type = I2C_DATA_OPERATION_TYPE_DMA;
    i2c_ctrl->operate_type = cfg.operation_type;
    /* 读准备 */
    errcode_t ret = hal_i2c_ctrl(bus, I2C_CTRL_READ_PREPARE, (uintptr_t)&cfg);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    hal_i2c_buffer_wrap_t buffer_wrap;

    buffer_wrap.len = data->len;
    buffer_wrap.buffer = data->buffer;

    /* 开启一个dma接收传输 */
    ret = i2c_start_recv_dma(bus, &buffer_wrap);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* 从模式在这里接收信号量，主模式在发送读命令之后接收信号量 */
    if (!i2c_ctrl->master_flag) {
        if (i2c_int_mode_down(&(g_dma_trans[bus].dma_sem)) != OSAL_SUCCESS) {
            g_dma_trans[bus].write_channel = 0;
            return ERRCODE_I2C_DMA_TRANSFER_ERROR;
        }

        g_dma_trans[bus].write_channel = 0;
        if (!g_dma_trans[bus].trans_succ) {
            return ERRCODE_I2C_DMA_TRANSFER_ERROR;
        }

        (void)hal_i2c_ctrl(bus, I2C_CTRL_READ_RESTORE, (uintptr_t)NULL);
    }

    return ERRCODE_SUCC;
}

static errcode_t i2c_read_do_send_cmd(i2c_bus_t bus, hal_i2c_buffer_wrap_t *data)
{
    errcode_t ret;
    hal_i2c_buffer_wrap_t buffer_wrap;

    buffer_wrap.buffer = data->buffer;
    buffer_wrap.len = data->len - 1;
    buffer_wrap.stop_flag = data->stop_flag;

    /* 先通过dma发送len - 1个没有停止位的读命令 */
    if (buffer_wrap.len != 0) {
        ret = i2c_transmit_read_cmd_by_dma(bus, &buffer_wrap);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }

        /* 发送len - 1个命令的dma中断 */
        if (i2c_int_mode_down(&(g_dma_trans[bus].dma_sem)) != OSAL_SUCCESS) {
            g_dma_trans[bus].write_channel = 0;
            g_dma_trans[bus].read_channel = 0;
            return ERRCODE_I2C_DMA_TRANSFER_ERROR;
        }
    }

    /* 单独发送最后一个cmd，会包含停止位，触发数据接收流程 */
    buffer_wrap.len = 1;
    hal_i2c_ctrl(bus, I2C_CTRL_FLUSH_RX_FIFO, (uintptr_t)&buffer_wrap);
    /* 等待接收数据的dma中断释放信号量 */
    if (i2c_int_mode_down(&(g_dma_trans[bus].dma_sem)) != OSAL_SUCCESS) {
        g_dma_trans[bus].read_channel = 0;
        (void)hal_i2c_ctrl(bus, I2C_CTRL_READ_RESTORE, (uintptr_t)NULL);
        return ERRCODE_I2C_DMA_TRANSFER_ERROR;
    }

    (void)hal_i2c_ctrl(bus, I2C_CTRL_READ_RESTORE, (uintptr_t)NULL);

    return ERRCODE_SUCC;
}
#endif  /* CONFIG_I2C_SUPPORT_DMA */

#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
static void i2c_evt_busy_callback(i2c_bus_t bus)
{
    if (g_i2c_ctrl[bus].callback != NULL) {
        g_i2c_ctrl[bus].callback(bus, I2C_IRQ_EVT_I2C_BUSY);
    }
}

static void i2c_evt_err_callback(i2c_bus_t bus)
{
    if (g_i2c_ctrl[bus].callback != NULL) {
        g_i2c_ctrl[bus].callback(bus, I2C_IRQ_EVT_I2C_ERR);
    }
}

static void i2c_evt_tx_callback(i2c_bus_t bus)
{
    hal_i2c_buffer_wrap_t hal_wrap;
    uint32_t burst_len;

    /* 如果没有要写的数据了，就退出 */
    if (g_i2c_buffer_wrap[bus].len == 0) {
        (void)hal_i2c_ctrl(bus, I2C_CTRL_WRITE_RESTORE, (uintptr_t)NULL);
        if (g_i2c_ctrl[bus].callback != NULL) {
            g_i2c_ctrl[bus].callback(bus, I2C_IRQ_EVT_TX_DONE);
        }
        return;
    }

    hal_i2c_ctrl(bus, I2C_CTRL_GET_WRITE_NUM, (uintptr_t)&burst_len);
    uint32_t remain_len = g_i2c_buffer_wrap[bus].len;

    burst_len = burst_len < remain_len ? burst_len : remain_len;
    hal_wrap.len = burst_len;
    hal_wrap.buffer = g_i2c_buffer_wrap[bus].buffer;
    hal_wrap.restart_flag = g_i2c_buffer_wrap[bus].restart_flag;
    /* 只有在发送到末尾时，才发送停止位 */
    if (burst_len == g_i2c_buffer_wrap[bus].len) {
        hal_wrap.stop_flag = g_i2c_buffer_wrap[bus].stop_flag;
    } else {
        hal_wrap.stop_flag = false;
    }

    g_i2c_buffer_wrap[bus].len -= burst_len;
    g_i2c_buffer_wrap[bus].buffer += burst_len;

    hal_i2c_write(bus, &hal_wrap);
}

static void i2c_evt_rx_callback(i2c_bus_t bus)
{
    uint32_t burst_len;
    uint32_t read_len;
    uint32_t cmd_len;
    hal_i2c_buffer_wrap_t read_wrap;
    hal_i2c_buffer_wrap_t cmd_wrap;

    hal_i2c_ctrl(bus, I2C_CTRL_GET_READ_NUM, (uintptr_t)&burst_len);

    read_len = g_i2c_buffer_wrap[bus].len;
    read_len = burst_len < read_len ? burst_len : read_len;

    read_wrap.len = read_len;
    read_wrap.buffer = g_i2c_buffer_wrap[bus].buffer;
    read_wrap.restart_flag = g_i2c_buffer_wrap[bus].restart_flag;
    read_wrap.stop_flag = g_i2c_buffer_wrap[bus].stop_flag;

    hal_i2c_read(bus, &read_wrap);

    g_i2c_buffer_wrap[bus].buffer += read_len;
    g_i2c_buffer_wrap[bus].len -= read_len;

    /* 先是读取已有的数据，接下来发送读取之后的数据的命令 */
    if (g_i2c_buffer_wrap[bus].len == 0) {
        hal_i2c_ctrl(bus, I2C_CTRL_READ_RESTORE, (uintptr_t)NULL);
        i2c_int_mode_up(&g_i2c_int_sem[bus]);
        if (g_i2c_ctrl[bus].callback != NULL) {
            g_i2c_ctrl[bus].callback(bus, I2C_IRQ_EVT_RX_DONE);
        }
        return;
    }

    hal_i2c_ctrl(bus, I2C_CTRL_GET_READ_NUM, (uintptr_t)&burst_len);
    cmd_len = g_i2c_buffer_wrap[bus].len;
    cmd_len = burst_len < cmd_len ? burst_len : cmd_len;

    cmd_wrap.buffer = g_i2c_buffer_wrap[bus].buffer;
    cmd_wrap.len = cmd_len;
    cmd_wrap.restart_flag = g_i2c_buffer_wrap[bus].restart_flag;
    /* 只有在发送到末尾时，才发送停止位 */
    if (cmd_len < g_i2c_buffer_wrap[bus].len) {
        cmd_wrap.stop_flag = false;
    } else {
        cmd_wrap.stop_flag = g_i2c_buffer_wrap[bus].stop_flag;
    }

    hal_i2c_ctrl(bus, I2C_CTRL_FLUSH_RX_FIFO, (uintptr_t)&cmd_wrap);
}

static errcode_t i2c_write_int(i2c_bus_t bus, uint16_t dev_addr, hal_i2c_buffer_wrap_t *data_cfg)
{
    hal_i2c_prepare_config_t cfg = { 0 };
    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];

    cfg.addr = dev_addr;
    cfg.operation_type = I2C_DATA_OPERATION_TYPE_INT;
    cfg.total_len = data_cfg->len;
    i2c_ctrl->operate_type = cfg.operation_type;

    g_i2c_buffer_wrap[bus].buffer = data_cfg->buffer;
    g_i2c_buffer_wrap[bus].len = data_cfg->len;
    g_i2c_buffer_wrap[bus].restart_flag = data_cfg->restart_flag;
    g_i2c_buffer_wrap[bus].stop_flag = data_cfg->stop_flag;

    errcode_t ret = hal_i2c_ctrl(bus, I2C_CTRL_WRITE_PREPARE, (uintptr_t)&cfg);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if (i2c_int_mode_down(&g_i2c_int_sem[bus]) != OSAL_SUCCESS) {
        return ERRCODE_I2C_DMA_TRANSFER_ERROR;
    }

    return ret;
}

static errcode_t i2c_read_int(i2c_bus_t bus, uint16_t dev_addr, hal_i2c_buffer_wrap_t *data_cfg)
{
    hal_i2c_prepare_config_t cfg = { 0 };
    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];

    cfg.addr = dev_addr;
    cfg.operation_type = I2C_DATA_OPERATION_TYPE_INT;
    cfg.total_len = data_cfg->len;
    i2c_ctrl->operate_type = cfg.operation_type;

    errcode_t ret = hal_i2c_ctrl(bus, I2C_CTRL_READ_PREPARE, (uintptr_t)&cfg);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    uint32_t burst_num;
    uint32_t len = data_cfg->len;
    hal_i2c_buffer_wrap_t hal_read_cmd;

    hal_i2c_ctrl(bus, I2C_CTRL_GET_READ_NUM, (uintptr_t)&burst_num);
    burst_num = burst_num < len ? burst_num : len;
    g_i2c_buffer_wrap[bus].len = data_cfg->len;
    g_i2c_buffer_wrap[bus].buffer = data_cfg->buffer;
    g_i2c_buffer_wrap[bus].restart_flag = data_cfg->restart_flag;
    g_i2c_buffer_wrap[bus].stop_flag = data_cfg->stop_flag;

    hal_read_cmd.len = burst_num;
    hal_read_cmd.buffer = data_cfg->buffer;
    hal_read_cmd.restart_flag = data_cfg->restart_flag;
    if (burst_num == g_i2c_buffer_wrap[bus].len) {
        hal_read_cmd.stop_flag = data_cfg->stop_flag;
    } else {
        hal_read_cmd.stop_flag = false;
    }
    hal_i2c_ctrl(bus, I2C_CTRL_FLUSH_RX_FIFO, (uintptr_t)&hal_read_cmd);

    if (i2c_int_mode_down(&g_i2c_int_sem[bus]) != OSAL_SUCCESS) {
        return ERRCODE_I2C_DMA_TRANSFER_ERROR;
    }

    return ret;
}

#endif  /* CONFIG_I2C_SUPPORT_INT */

static errcode_t i2c_evt_callback(i2c_bus_t bus, hal_i2c_evt_id_t evt, uintptr_t param)
{
    unused(param);
    unused(evt);
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_POLL) {
        return ERRCODE_SUCC;
    }

#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    if (evt == I2C_EVT_TX_READY) {
        i2c_evt_tx_callback(bus);
    } else if (evt == I2C_EVT_RX_READY) {
        i2c_evt_rx_callback(bus);
    } else if (evt == I2C_EVT_TRANSMITION_DONE) {
        i2c_int_mode_up(&g_i2c_int_sem[bus]);
    } else if (evt == I2C_EVT_TRANSMITION_ABRT) {
        i2c_evt_err_callback(bus);
    } else if (evt == I2C_EVT_TRANSMITION_BUSY) {
        i2c_evt_busy_callback(bus);
    }

#endif  /* CONFIG_I2C_SUPPORT_INT */
    return ERRCODE_SUCC;
}

#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
errcode_t uapi_i2c_set_irq_mode(i2c_bus_t bus, bool irq_en)
{
    if (bus >= I2C_BUS_NONE) {
        return ERRCODE_I2C_INVALID_PARAMETER;
    }
    if (irq_en) {
        g_i2c_ctrl[bus].operate_type = I2C_DATA_OPERATION_TYPE_INT;
    } else {
        g_i2c_ctrl[bus].operate_type = I2C_DATA_OPERATION_TYPE_POLL;
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_i2c_register_irq_callback(i2c_bus_t bus, i2c_irq_callback_t callback)
{
    g_i2c_ctrl[bus].callback = callback;
    return ERRCODE_SUCC;
}

errcode_t uapi_i2c_unregister_irq_callback(i2c_bus_t bus)
{
    g_i2c_ctrl[bus].callback = NULL;
    return ERRCODE_SUCC;
}
#endif  /* CONFIG_I2C_SUPPORT_INT */

static void i2c_int_mode_init(i2c_bus_t bus)
{
    unused(bus);
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    (void)memset_s(&g_i2c_int_sem[bus], sizeof(g_i2c_int_sem[bus]),
                   0, sizeof(g_i2c_int_sem[bus]));
#if !defined(CONFIG_I2C_SUPPORT_IN_CHIP_LOOPBACK)
    (void)osal_sem_init(&g_i2c_int_sem[bus], 0);
#endif
#endif  /* CONFIG_I2C_SUPPORT_INT */
}

static void i2c_int_mode_sem_deinit(i2c_bus_t bus)
{
    unused(bus);
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
#if !defined(CONFIG_I2C_SUPPORT_IN_CHIP_LOOPBACK)
    osal_sem_destroy(&g_i2c_int_sem[bus]);
#endif
#endif  /* CONFIG_I2C_SUPPORT_INT */
}

#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
errcode_t uapi_i2c_set_dma_mode(i2c_bus_t bus, bool dma_en)
{
    if (bus >= I2C_BUS_NONE) {
        return ERRCODE_I2C_INVALID_PARAMETER;
    }
    if (dma_en) {
        g_i2c_ctrl[bus].operate_type = I2C_DATA_OPERATION_TYPE_DMA;
    } else {
        g_i2c_ctrl[bus].operate_type = I2C_DATA_OPERATION_TYPE_POLL;
    }
    return ERRCODE_SUCC;
}
#endif  /* CONFIG_I2C_SUPPORT_DMA */

static void i2c_dma_mode_init(i2c_bus_t bus)
{
    unused(bus);
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
    (void)memset_s(&(g_dma_trans[bus].dma_sem), sizeof(g_dma_trans[bus].dma_sem),
                   0, sizeof(g_dma_trans[bus].dma_sem));
    (void)osal_sem_init(&(g_dma_trans[bus].dma_sem), 0);
#endif  /* CONFIG_I2C_SUPPORT_DMA */
}

static void i2c_dma_mode_sem_deinit(i2c_bus_t bus)
{
    unused(bus);
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
    osal_sem_destroy(&(g_dma_trans[bus].dma_sem));
#endif  /* CONFIG_I2C_SUPPORT_DMA */
}

static void i2c_mutex_init(i2c_bus_t bus)
{
    unused(bus);
#if defined(CONFIG_I2C_SUPPORT_CONCURRENCY) && (CONFIG_I2C_SUPPORT_CONCURRENCY == 1)
    (void)memset_s(&g_i2c_mutex[bus], sizeof(g_i2c_mutex[bus]), 0, sizeof(g_i2c_mutex[bus]));
    (void)osal_mutex_init(&g_i2c_mutex[bus]);
#endif  /* CONFIG_I2C_SUPPORT_CONCURRENCY */
}

static void i2c_mutex_destroy(i2c_bus_t bus)
{
    unused(bus);
#if defined(CONFIG_I2C_SUPPORT_CONCURRENCY) && (CONFIG_I2C_SUPPORT_CONCURRENCY == 1)
    osal_mutex_destroy(&g_i2c_mutex[bus]);
#endif  /* CONFIG_I2C_SUPPORT_CONCURRENCY */
}

static void i2c_mutex_lock(i2c_bus_t bus)
{
    unused(bus);
#if defined(CONFIG_I2C_SUPPORT_CONCURRENCY) && (CONFIG_I2C_SUPPORT_CONCURRENCY == 1)
    osal_mutex_lock(&g_i2c_mutex[bus]);
#endif  /* CONFIG_I2C_SUPPORT_CONCURRENCY */
}

static void i2c_mutex_unlock(i2c_bus_t bus)
{
    unused(bus);
#if defined(CONFIG_I2C_SUPPORT_CONCURRENCY) && (CONFIG_I2C_SUPPORT_CONCURRENCY == 1)
    osal_mutex_unlock(&g_i2c_mutex[bus]);
#endif  /* CONFIG_I2C_SUPPORT_CONCURRENCY */
}

static errcode_t i2c_init_check(i2c_bus_t bus, uint32_t baudrate)
{
    if (unlikely(bus >= I2C_BUS_MAX_NUM)) {
        return ERRCODE_I2C_INVALID_PARAMETER;
    }
    if (unlikely(baudrate > I2C_HS_MODE_BAUDRATE_HIGH_LIMIT || baudrate == 0)) {
        return ERRCODE_I2C_RATE_INVALID;
    }
    if (g_i2c_ctrl[bus].init_flag == true) {
        return ERRCODE_I2C_ALREADY_INIT;
    }
    return ERRCODE_SUCC;
}

#if defined(CONFIG_I2C_SUPPORT_MASTER) && (CONFIG_I2C_SUPPORT_MASTER == 1)
static errcode_t i2c_master_init_check(i2c_bus_t bus, uint32_t baudrate, uint8_t hscode)
{
    errcode_t ret = i2c_init_check(bus, baudrate);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if (unlikely(hscode > I2C_HS_MODE_MASTER_CODE_MAX)) {
        return ERRCODE_I2C_INVALID_PARAMETER;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_i2c_master_init(i2c_bus_t bus, uint32_t baudrate, uint8_t hscode)
{
    errcode_t ret = i2c_master_init_check(bus, baudrate, hscode);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if defined(CONFIG_I2C_SUPPORT_LPC)
    i2c_port_clock_enable(bus, true);
#endif

    i2c_mutex_init(bus);
    i2c_int_mode_init(bus);
    i2c_dma_mode_init(bus);

    ret = hal_i2c_master_init(bus, baudrate, hscode, i2c_evt_callback);
    if (ret != ERRCODE_SUCC) {
        i2c_int_mode_sem_deinit(bus);
        i2c_dma_mode_sem_deinit(bus);
        i2c_mutex_destroy(bus);
        return ret;
    }

#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    i2c_port_register_irq(bus);
#endif

    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];
    i2c_ctrl->init_flag = true;
    i2c_ctrl->master_flag = true;
    i2c_ctrl->hscode = hscode;
    return ERRCODE_SUCC;
}

static errcode_t i2c_fifo_check(i2c_bus_t bus)
{
    if (hal_i2c_ctrl(bus, I2C_CTRL_CHECK_TX_FIFO_EMPTY, (uintptr_t)CONFIG_I2C_WAIT_CONDITION_TIMEOUT) != ERRCODE_SUCC) {
        return ERRCODE_I2C_TIMEOUT;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_i2c_master_write(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data)
{
    errcode_t ret = i2c_write_pre_check(bus, data);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = i2c_fifo_check(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    hal_i2c_buffer_wrap_t buffer_wrap = { 0 };
    buffer_wrap.buffer = data->send_buf;
    buffer_wrap.len = data->send_len;
    buffer_wrap.stop_flag = true;
    buffer_wrap.restart_flag = false;

    i2c_mutex_lock(bus);
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#if defined(CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    if (buffer_wrap.len > CONFIG_I2C_POLL_AND_DMA_AUTO_SWITCH_THRESHOLD) {
        uapi_i2c_set_dma_mode(bus, true);
        ret = i2c_write_by_dma(bus, dev_addr, &buffer_wrap);
        i2c_mutex_unlock(bus);
        return ret;
    }
    uapi_i2c_set_dma_mode(bus, false);
#else
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_DMA) {
        ret = i2c_write_by_dma(bus, dev_addr, &buffer_wrap);
        i2c_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
#endif  /* CONFIG_I2C_SUPPORT_DMA */
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_INT) {
        ret = i2c_write_int(bus, dev_addr, &buffer_wrap);
        i2c_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_I2C_SUPPORT_INT */
    ret = i2c_poll_write(bus, dev_addr, &buffer_wrap, true);
    i2c_mutex_unlock(bus);
    return ret;
}

errcode_t uapi_i2c_master_read(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data)
{
    errcode_t ret = i2c_read_pre_check(bus, data);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    hal_i2c_buffer_wrap_t buffer_wrap = { 0 };
    buffer_wrap.buffer = data->receive_buf;
    buffer_wrap.len = data->receive_len;
    buffer_wrap.stop_flag = true;
    buffer_wrap.restart_flag = false;

    i2c_mutex_lock(bus);
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#if defined(CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    if (buffer_wrap.len > CONFIG_I2C_POLL_AND_DMA_AUTO_SWITCH_THRESHOLD) {
        uapi_i2c_set_dma_mode(bus, true);
        errcode_t ret = i2c_read_by_dma(bus, dev_addr, &buffer_wrap);
        if (ret != ERRCODE_SUCC) {
            i2c_mutex_unlock(bus);
            return ret;
        }
        ret = i2c_read_do_send_cmd(bus, &buffer_wrap);
        i2c_mutex_unlock(bus);
        return ret;
    }
    uapi_i2c_set_dma_mode(bus, false);
#else
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_DMA) {
        errcode_t ret = i2c_read_by_dma(bus, dev_addr, &buffer_wrap);
        if (ret != ERRCODE_SUCC) {
            i2c_mutex_unlock(bus);
            return ret;
        }
        ret = i2c_read_do_send_cmd(bus, &buffer_wrap);
        i2c_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
#endif  /* CONFIG_I2C_SUPPORT_DMA */
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_INT) {
        ret = i2c_read_int(bus, dev_addr, &buffer_wrap);
        i2c_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_I2C_SUPPORT_INT */
    ret = i2c_poll_read(bus, dev_addr, &buffer_wrap, true);
    i2c_mutex_unlock(bus);
    return ret;
}

errcode_t uapi_i2c_master_writeread(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data)
{
    errcode_t ret = i2c_write_pre_check(bus, data);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = i2c_read_pre_check(bus, data);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = i2c_fifo_check(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    i2c_mutex_lock(bus);
    ret = uapi_i2c_master_write(bus, dev_addr, data);
    if (ret != ERRCODE_SUCC) {
        i2c_mutex_unlock(bus);
        return ret;
    }

    i2c_wait_condition_t condition = { 0 };
    condition.ctrl_mask = i2c_ctrl_get_mask(I2C_CTRL_CHECK_RESTART_READY);
    (void)i2c_wait(bus, &condition, CONFIG_I2C_WAIT_CONDITION_TIMEOUT);

    ret = uapi_i2c_master_read(bus, dev_addr, data);
    i2c_mutex_unlock(bus);
    return ret;
}
#endif  /* CONFIG_I2C_SUPPORT_MASTER */

#if defined(CONFIG_I2C_SUPPORT_SLAVE) && (CONFIG_I2C_SUPPORT_SLAVE == 1)
errcode_t uapi_i2c_slave_init(i2c_bus_t bus, uint32_t baudrate, uint16_t addr)
{
    errcode_t ret = i2c_init_check(bus, baudrate);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if defined(CONFIG_I2C_SUPPORT_LPC)
    i2c_port_clock_enable(bus, true);
#endif

    i2c_mutex_init(bus);
    i2c_int_mode_init(bus);
    i2c_dma_mode_init(bus);

    ret = hal_i2c_slave_init(bus, baudrate, addr, i2c_evt_callback);
    if (ret != ERRCODE_SUCC) {
        i2c_int_mode_sem_deinit(bus);
        i2c_dma_mode_sem_deinit(bus);
        i2c_mutex_destroy(bus);
        return ret;
    }

#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    i2c_port_register_irq(bus);
#endif

    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];
    i2c_ctrl->init_flag = true;
    i2c_ctrl->master_flag = false;
    i2c_ctrl->addr = addr;
    return ERRCODE_SUCC;
}

errcode_t uapi_i2c_slave_write(i2c_bus_t bus, i2c_data_t *data)
{
    errcode_t ret = i2c_write_pre_check(bus, data);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = i2c_fifo_check(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    hal_i2c_buffer_wrap_t buffer_wrap = { 0 };
    buffer_wrap.buffer = data->send_buf;
    buffer_wrap.len = data->send_len;
    buffer_wrap.stop_flag = false;

    i2c_mutex_lock(bus);
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#if defined(CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    if (buffer_wrap.len > CONFIG_I2C_POLL_AND_DMA_AUTO_SWITCH_THRESHOLD) {
        uapi_i2c_set_dma_mode(bus, true);
        ret = i2c_write_by_dma(bus, I2C_DEFAULT_DEV_ADDRESS, &buffer_wrap);
        i2c_mutex_unlock(bus);
        return ret;
    }
    uapi_i2c_set_dma_mode(bus, false);
#else
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_DMA) {
        ret = i2c_write_by_dma(bus, I2C_DEFAULT_DEV_ADDRESS, &buffer_wrap);
        i2c_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
#endif  /* CONFIG_I2C_SUPPORT_DMA */
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_INT) {
        ret = i2c_write_int(bus, I2C_DEFAULT_DEV_ADDRESS, &buffer_wrap);
        i2c_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_I2C_SUPPORT_INT */
    ret = i2c_poll_write(bus, I2C_DEFAULT_DEV_ADDRESS, &buffer_wrap, true);
    i2c_mutex_unlock(bus);
    return ret;
}

errcode_t uapi_i2c_slave_read(i2c_bus_t bus, i2c_data_t *data)
{
    errcode_t ret = i2c_read_pre_check(bus, data);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    hal_i2c_buffer_wrap_t buffer_wrap;
    buffer_wrap.buffer = data->receive_buf;
    buffer_wrap.len = data->receive_len;
    buffer_wrap.stop_flag = false;
    buffer_wrap.restart_flag = false;

    i2c_mutex_lock(bus);
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#if defined(CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    if (buffer_wrap.len > CONFIG_I2C_POLL_AND_DMA_AUTO_SWITCH_THRESHOLD) {
        uapi_i2c_set_dma_mode(bus, true);
        ret = i2c_read_by_dma(bus, I2C_DEFAULT_DEV_ADDRESS, &buffer_wrap);
        i2c_mutex_unlock(bus);
        return ret;
    }
    uapi_i2c_set_dma_mode(bus, false);
#else
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_DMA) {
        ret = i2c_read_by_dma(bus, I2C_DEFAULT_DEV_ADDRESS, &buffer_wrap);
        i2c_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
#endif  /* CONFIG_I2C_SUPPORT_DMA */

#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_INT) {
        ret = i2c_read_int(bus, I2C_DEFAULT_DEV_ADDRESS, &buffer_wrap);
        i2c_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_I2C_SUPPORT_INT */
    ret = i2c_poll_read(bus, I2C_DEFAULT_DEV_ADDRESS, &buffer_wrap, true);
    i2c_mutex_unlock(bus);
    return ret;
}
#endif  /* CONFIG_I2C_SUPPORT_SLAVE */

errcode_t uapi_i2c_deinit(i2c_bus_t bus)
{
    errcode_t ret = i2c_bus_init_check(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];

    hal_i2c_deinit(bus);
    i2c_int_mode_sem_deinit(bus);
    i2c_dma_mode_sem_deinit(bus);
    i2c_mutex_destroy(bus);
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    i2c_port_unregister_irq(bus);
#endif
#if defined(CONFIG_I2C_SUPPORT_LPC)
    i2c_port_clock_enable(bus, false);
#endif

    i2c_ctrl->init_flag = false;
    i2c_ctrl->master_flag = false;
    return ERRCODE_SUCC;
}

errcode_t uapi_i2c_set_baudrate(i2c_bus_t bus, uint32_t baudrate)
{
    if (baudrate > I2C_HS_MODE_BAUDRATE_HIGH_LIMIT || baudrate == 0) {
        return ERRCODE_I2C_RATE_INVALID;
    }
    errcode_t ret = i2c_bus_init_check(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];
    i2c_mutex_lock(bus);
    if (i2c_ctrl->master_flag == true) {
        ret = hal_i2c_master_init(bus, baudrate, i2c_ctrl->hscode, i2c_evt_callback);
    } else {
        ret = hal_i2c_slave_init(bus, baudrate, i2c_ctrl->addr, i2c_evt_callback);
    }
    i2c_mutex_unlock(bus);
    return ret;
}

#if defined(CONFIG_I2C_SUPPORT_LPM)
errcode_t uapi_i2c_suspend(uintptr_t arg)
{
    unused(arg);
    return ERRCODE_SUCC;
}

errcode_t uapi_i2c_resume(uintptr_t arg)
{
    unused(arg);
    return ERRCODE_SUCC;
}
#endif