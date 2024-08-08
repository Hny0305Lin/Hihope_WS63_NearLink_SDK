/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V150 i2c register operation api \n
 *
 * History: \n
 * 2023-03-06, Create file. \n
 */

#ifndef HAL_I2C_V150_REGS_OP_H
#define HAL_I2C_V150_REGS_OP_H

#include <stdint.h>
#include <stdbool.h>
#include "soc_osal.h"
#include "hal_i2c_v150_regs_def.h"
#include "i2c_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_i2c_v150_regs_op I2C V150 Regs Operation
 * @ingroup  drivers_hal_i2c
 * @{
 */

/* 操作类型定义 */
#define I2C_OP_START    (1 << 3)
#define I2C_OP_READ     (1 << 2)
#define I2C_OP_WRITE    (1 << 1)
#define I2C_OP_STOP     (1 << 0)

/*
 * 中断类型与状态定义
 * I2C中断状态/中断清除/中断屏蔽寄存器的比特位不对齐, 通过掩码将其对齐为中断类型, 方便统一处理;
 * 注意, 以下定义的"中断类型"为代码逻辑中计算使用的中断类型值, 与寄存器真实值不同。
 */
/* 中断类型 */
#define I2C_INT_TYPE_DONE           (uint32_t)(1 << 0)
#define I2C_INT_TYPE_ARB_LOSS       (uint32_t)(1 << 1)
#define I2C_INT_TYPE_ACK_ERR        (uint32_t)(1 << 2)
#define I2C_INT_TYPE_RX             (uint32_t)(1 << 3)
#define I2C_INT_TYPE_TX             (uint32_t)(1 << 4)
#define I2C_INT_TYPE_STOP           (uint32_t)(1 << 5)
#define I2C_INT_TYPE_START          (uint32_t)(1 << 6)
#define I2C_INT_TYPE_RXTIDE         (uint32_t)(1 << 7)
#define I2C_INT_TYPE_TXTIDE         (uint32_t)(1 << 8)
#define I2C_INT_TYPE_TXFIFO_OVER    (uint32_t)(1 << 9)

/* 中断有效类型掩码 */
#define I2C_INT_TYPE_MASK 0x3FF

/* I2C_CTRL寄存器 中断屏蔽状态掩码 */
#define I2C_CTRL_INT_MASK           0x167F
#define I2C_CTRL_INT_MASK_L         0x007F
#define I2C_CTRL_INT_MASK_L_OFFSET  0
#define I2C_CTRL_INT_MASK_M         0x0600
#define I2C_CTRL_INT_MASK_M_OFFSET  2
#define I2C_CTRL_INT_MASK_H         0x1000
#define I2C_CTRL_INT_MASK_H_OFFSET  3

/* I2C_ICR寄存器 中断清除状态掩码 */
#define I2C_ICR_INT_MASK            0x03FF

/* I2C_SR寄存器 中断上报状态掩码 */
#define I2C_SR_INT_MASK             0x077F
#define I2C_SR_INT_MASK_L           0x007F
#define I2C_SR_INT_MASK_L_OFFSET    0
#define I2C_SR_INT_MASK_H           0x0700
#define I2C_SR_INT_MASK_H_OFFSET    1

/* 毛刺过滤标准值, 默认使用逻辑配置参数0x8 */
#define I2C_FTRPER_STANDARD_VAL     0x8

/**
 * @if Eng
 * @brief  Bit definition of I2C registers.
 * @else
 * @brief  I2C寄存器比特位定义。
 * @endif
 */
typedef enum i2c_con_reg_config {
    I2C_BIT_FALSE,        /*!< 比特置为0 */
    I2C_BIT_TRUE,         /*!< 比特置为1 */
} i2c_bit_config_t;

extern i2c_regs_t *g_i2c_regs[I2C_BUS_MAX_NUM];

/**
 * @if Eng
 * @brief  Transform Interrupt type to I2C_CTRL Interrupt mask status bit.
 * @param  [in]  int_type Interrupt type.
 * @return I2C_CTRL Interrupt mask status bit.
 * @else
 * @brief  中断类型 转换为 I2C_CTRL 中断屏蔽状态比特。
 * @param  [in]  int_type 中断类型。
 * @return I2C_CTRL 中断屏蔽状态比特。
 * @endif
 */
static inline uint32_t hal_i2c_v150_int_type_to_ctrl(uint32_t int_type)
{
    return (((int_type & (I2C_CTRL_INT_MASK_L >> I2C_CTRL_INT_MASK_L_OFFSET)) << I2C_CTRL_INT_MASK_L_OFFSET) |
        ((int_type & (I2C_CTRL_INT_MASK_M >> I2C_CTRL_INT_MASK_M_OFFSET)) << I2C_CTRL_INT_MASK_M_OFFSET) |
        ((int_type & (I2C_CTRL_INT_MASK_H >> I2C_CTRL_INT_MASK_H_OFFSET)) << I2C_CTRL_INT_MASK_H_OFFSET));
}

/**
 * @if Eng
 * @brief  Transform I2C_CTRL Interrupt mask status bit to Interrupt type.
 * @param  [in]  int_bits Interrupt mask status bit.
 * @return Interrupt type.
 * @else
 * @brief  I2C_CTRL中断屏蔽状态比特 转换为 中断类型。
 * @param  [in]  int_bits I2C_CTRL中断屏蔽状态比特。
 * @return 中断类型。
 * @endif
 */
static inline uint32_t hal_i2c_v150_ctrl_to_int_type(uint32_t int_bits)
{
    return (((int_bits & I2C_CTRL_INT_MASK_L) >> I2C_CTRL_INT_MASK_L_OFFSET) |
        ((int_bits & I2C_CTRL_INT_MASK_M) >> I2C_CTRL_INT_MASK_M_OFFSET) |
        ((int_bits & I2C_CTRL_INT_MASK_H) >> I2C_CTRL_INT_MASK_H_OFFSET));
}

/**
 * @if Eng
 * @brief  Transform Interrupt type to I2C_ICR Interrupt clear status bit.
 * @param  [in]  int_type Interrupt type.
 * @return I2C_ICR Interrupt clear status bit.
 * @else
 * @brief  中断类型 转换为 I2C_ICR 中断清除状态比特。
 * @param  [in]  int_type 中断类型。
 * @return I2C_ICR 中断清除状态比特。
 * @endif
 */
static inline uint32_t hal_i2c_v150_int_type_to_icr(uint32_t int_type)
{
    return (int_type & I2C_ICR_INT_MASK);
}

/**
 * @if Eng
 * @brief  Transform I2C_ICR Interrupt clear status bit to Interrupt type.
 * @param  [in]  reg_icr Interrupt clear status bit.
 * @return Interrupt type.
 * @else
 * @brief  I2C_ICR 中断清除状态比特 转换为 中断类型。
 * @param  [in]  reg_icr I2C_ICR中断清除状态比特。
 * @return 中断类型。
 * @endif
 */
static inline uint32_t hal_i2c_v150_icr_to_int_type(uint32_t reg_icr)
{
    return (reg_icr & I2C_ICR_INT_MASK);
}

/**
 * @if Eng
 * @brief  Transform Interrupt type to I2C_SR Interrupt report status bit.
 * @param  [in]  int_type Interrupt type.
 * @return I2C_SR Interrupt report status bit.
 * @else
 * @brief  中断类型 转换为 I2C_SR 中断上报状态比特。
 * @param  [in]  int_type 中断类型。
 * @return I2C_SR 中断上报状态比特。
 * @endif
 */
static inline uint32_t hal_i2c_v150_int_type_to_sr(uint32_t int_type)
{
    return (((int_type & (I2C_SR_INT_MASK_L >> I2C_SR_INT_MASK_L_OFFSET)) << I2C_SR_INT_MASK_L_OFFSET) |
        ((int_type & (I2C_SR_INT_MASK_H >> I2C_SR_INT_MASK_H_OFFSET)) << I2C_SR_INT_MASK_H_OFFSET));
}

/**
 * @if Eng
 * @brief  Transform I2C_SR Interrupt report status bit to Interrupt type.
 * @param  [in]  reg_sr Interrupt report status bit.
 * @return Interrupt type.
 * @else
 * @brief  I2C_SR 中断上报状态比特 转换为 中断类型。
 * @param  [in]  reg_sr I2C_SR 中断上报状态比特。
 * @return 中断类型。
 * @endif
 */
static inline uint32_t hal_i2c_v150_sr_to_int_type(uint32_t reg_sr)
{
    return (((reg_sr & I2C_SR_INT_MASK_L) >> I2C_SR_INT_MASK_L_OFFSET) |
        ((reg_sr & I2C_SR_INT_MASK_H) >> I2C_SR_INT_MASK_H_OFFSET));
}

/**
 * @if Eng
 * @brief  Set I2C enable.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @else
 * @brief  使能I2C。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @endif
 */
static inline void hal_i2c_v150_set_i2c_enable(i2c_bus_t bus)
{
    i2c_ctrl_data_t i2c_ctrl;
    i2c_ctrl.d32 = g_i2c_regs[bus]->i2c_ctrl;
    i2c_ctrl.b.i2c_en = I2C_BIT_TRUE;
    g_i2c_regs[bus]->i2c_ctrl = i2c_ctrl.d32;
}

/**
 * @if Eng
 * @brief  Set I2C disable.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @else
 * @brief  去使能I2C。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @endif
 */
static inline void hal_i2c_v150_set_i2c_disable(i2c_bus_t bus)
{
    i2c_ctrl_data_t i2c_ctrl;
    i2c_ctrl.d32 = g_i2c_regs[bus]->i2c_ctrl;
    i2c_ctrl.b.i2c_en = I2C_BIT_FALSE;
    g_i2c_regs[bus]->i2c_ctrl = i2c_ctrl.d32;
}

/**
 * @if Eng
 * @brief  Enable FIFO transfer mode.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @else
 * @brief  使能FIFO传输模式。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @endif
 */
static inline void hal_i2c_v150_set_fifo_enable(i2c_bus_t bus)
{
    i2c_ctrl_data_t i2c_ctrl;
    i2c_ctrl.d32 = g_i2c_regs[bus]->i2c_ctrl;
    i2c_ctrl.b.mode_ctrl = I2C_BIT_TRUE;
    g_i2c_regs[bus]->i2c_ctrl = i2c_ctrl.d32;
}

/**
 * @if Eng
 * @brief  Disable FIFO transfer mode.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @else
 * @brief  去使能FIFO传输模式。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @endif
 */
static inline void hal_i2c_v150_set_fifo_disable(i2c_bus_t bus)
{
    i2c_ctrl_data_t i2c_ctrl;
    i2c_ctrl.d32 = g_i2c_regs[bus]->i2c_ctrl;
    i2c_ctrl.b.mode_ctrl = I2C_BIT_FALSE;
    g_i2c_regs[bus]->i2c_ctrl = i2c_ctrl.d32;
}

/**
 * @if Eng
 * @brief  Mask main entry of interrupt.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @else
 * @brief  屏蔽中断总入口。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @endif
 */
static inline void hal_i2c_v150_mask_main_int(i2c_bus_t bus)
{
    i2c_ctrl_data_t i2c_ctrl;
    i2c_ctrl.d32 = g_i2c_regs[bus]->i2c_ctrl;
    i2c_ctrl.b.int_mask = I2C_BIT_FALSE;
    g_i2c_regs[bus]->i2c_ctrl = i2c_ctrl.d32;
}

/**
 * @if Eng
 * @brief  Unmask main entry of interrupt.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @else
 * @brief  不屏蔽中断总入口。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @endif
 */
static inline void hal_i2c_v150_unmask_main_int(i2c_bus_t bus)
{
    i2c_ctrl_data_t i2c_ctrl;
    i2c_ctrl.d32 = g_i2c_regs[bus]->i2c_ctrl;
    i2c_ctrl.b.int_mask = I2C_BIT_TRUE;
    g_i2c_regs[bus]->i2c_ctrl = i2c_ctrl.d32;
}

/**
 * @if Eng
 * @brief  Mask specific interrupt.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @param  [in]  int_type Interrupt type.
 * @else
 * @brief  屏蔽指定中断。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @param  [in]  int_type 中断类型。
 * @endif
 */
static inline void hal_i2c_v150_mask_int(i2c_bus_t bus, uint32_t int_type)
{
    i2c_ctrl_data_t i2c_ctrl;
    i2c_ctrl.d32 = g_i2c_regs[bus]->i2c_ctrl;
    i2c_ctrl.d32 &= ~(hal_i2c_v150_int_type_to_ctrl(int_type));
    g_i2c_regs[bus]->i2c_ctrl = i2c_ctrl.d32;
}

/**
 * @if Eng
 * @brief  Unmask specific interrupt.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @param  [in]  int_type Interrupt type.
 * @else
 * @brief  不屏蔽指定中断。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @param  [in]  int_type 中断类型。
 * @endif
 */
static inline void hal_i2c_v150_unmask_int(i2c_bus_t bus, uint32_t int_type)
{
    i2c_ctrl_data_t i2c_ctrl;
    i2c_ctrl.d32 = g_i2c_regs[bus]->i2c_ctrl;
    i2c_ctrl.d32 |= hal_i2c_v150_int_type_to_ctrl(int_type);
    g_i2c_regs[bus]->i2c_ctrl = i2c_ctrl.d32;
}

/**
 * @if Eng
 * @brief  Mask all interrupt.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @else
 * @brief  屏蔽所有中断。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @endif
 */
static inline void hal_i2c_v150_mask_all_int(i2c_bus_t bus)
{
    hal_i2c_v150_mask_int(bus, I2C_INT_TYPE_DONE | I2C_INT_TYPE_ARB_LOSS | I2C_INT_TYPE_ACK_ERR |
        I2C_INT_TYPE_RX | I2C_INT_TYPE_TX | I2C_INT_TYPE_STOP | I2C_INT_TYPE_START |
        I2C_INT_TYPE_RXTIDE | I2C_INT_TYPE_TXTIDE | I2C_INT_TYPE_TXFIFO_OVER);
}

/**
 * @if Eng
 * @brief  Unask all interrupt.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @else
 * @brief  不屏蔽所有中断。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @endif
 */
static inline void hal_i2c_v150_unmask_all_int(i2c_bus_t bus)
{
    hal_i2c_v150_unmask_int(bus, I2C_INT_TYPE_DONE | I2C_INT_TYPE_ARB_LOSS | I2C_INT_TYPE_ACK_ERR |
        I2C_INT_TYPE_RX | I2C_INT_TYPE_TX | I2C_INT_TYPE_STOP | I2C_INT_TYPE_START |
        I2C_INT_TYPE_RXTIDE | I2C_INT_TYPE_TXTIDE | I2C_INT_TYPE_TXFIFO_OVER);
}

/**
 * @if Eng
 * @brief  Set I2C to send ACK when receive.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @else
 * @brief  设置I2C接收时发送ACK。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @endif
 */
static inline void hal_i2c_v150_enable_ack(i2c_bus_t bus)
{
    i2c_com_data_t i2c_com = {0};
    i2c_com.b.op_ack = I2C_BIT_FALSE;
    g_i2c_regs[bus]->i2c_com = i2c_com.d32;
}

/**
 * @if Eng
 * @brief  Set I2C NOT to send ACK when receive.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @else
 * @brief  设置I2C接收时不发送ACK。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @endif
 */
static inline void hal_i2c_v150_disable_ack(i2c_bus_t bus)
{
    i2c_com_data_t i2c_com = {0};
    i2c_com.b.op_ack = I2C_BIT_TRUE;
    g_i2c_regs[bus]->i2c_com = i2c_com.d32;
}

/**
 * @if Eng
 * @brief  Set I2C work operation.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @param  [in]  command_type I2C command(restart, stop, write, read).
 * @else
 * @brief  设置I2C工作命令。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @param  [in]  command_type I2C命令类型(重新启动, 停止, 写, 读)。
 * @endif
 */
static inline void hal_i2c_v150_set_command(i2c_bus_t bus, uint32_t command_type)
{
    i2c_com_data_t i2c_com;
    i2c_com.d32 = g_i2c_regs[bus]->i2c_com;
    i2c_com.b.op_stop = I2C_BIT_FALSE;
    i2c_com.b.op_we = I2C_BIT_FALSE;
    i2c_com.b.op_rd = I2C_BIT_FALSE;
    i2c_com.b.op_start = I2C_BIT_FALSE;
    i2c_com.d32 |= command_type;
    g_i2c_regs[bus]->i2c_com = i2c_com.d32;
}

/**
 * @if Eng
 * @brief  Clear specific interrupt.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @param  [in]  int_type Interrupt type.
 * @else
 * @brief  清除指定中断。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @param  [in]  int_type 中断类型。
 * @endif
 */
static inline void hal_i2c_v150_clear_int(i2c_bus_t bus, uint32_t int_type)
{
    i2c_icr_data_t i2c_icr;
    i2c_icr.d32 = hal_i2c_v150_int_type_to_icr(int_type);
    g_i2c_regs[bus]->i2c_icr = i2c_icr.d32;
}

/**
 * @if Eng
 * @brief  Clear all interrupt.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @else
 * @brief  清除所有中断。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @endif
 */
static inline void hal_i2c_v150_clear_all_int(i2c_bus_t bus)
{
    hal_i2c_v150_clear_int(bus, I2C_INT_TYPE_DONE | I2C_INT_TYPE_ARB_LOSS | I2C_INT_TYPE_ACK_ERR |
        I2C_INT_TYPE_RX | I2C_INT_TYPE_TX | I2C_INT_TYPE_STOP | I2C_INT_TYPE_START |
        I2C_INT_TYPE_RXTIDE | I2C_INT_TYPE_TXTIDE | I2C_INT_TYPE_TXFIFO_OVER);
}

/**
 * @if Eng
 * @brief  Get interrupt state.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @return Interrupt type.
 * @else
 * @brief  获取中断状态。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @return 中断类型。
 * @endif
 */
static inline uint32_t hal_i2c_v150_get_int_state(i2c_bus_t bus)
{
    i2c_sr_data_t i2c_sr;
    i2c_sr.d32 = g_i2c_regs[bus]->i2c_sr;
    return hal_i2c_v150_sr_to_int_type(i2c_sr.d32);
}

/**
 * @if Eng
 * @brief  Get bus busy status.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @return Bus busy status. 1: Bus busy. 0: Bus idle.
 * @else
 * @brief  获取总线忙状态。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @return 总线忙状态, 1:总线忙, 0:总线空闲。
 * @endif
 */
static inline uint8_t hal_i2c_v150_is_bus_busy(i2c_bus_t bus)
{
    i2c_sr_data_t i2c_sr;
    i2c_sr.d32 = g_i2c_regs[bus]->i2c_sr;
    return i2c_sr.b.bus_busy;
}

/**
 * @if Eng
 * @brief  Set number of clock high-level cycles scl_h.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @param  [in]  val Number of clock high-level cycles scl_h.
 * @else
 * @brief  设置时钟高周期数scl_h。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @param  [in]  val 时钟高周期数scl_h。
 * @endif
 */
static inline void hal_i2c_v150_set_scl_h(i2c_bus_t bus, uint16_t val)
{
    i2c_scl_h_data_t i2c_scl_h = {0};
    i2c_scl_h.b.scl_h = val;
    g_i2c_regs[bus]->i2c_scl_h = i2c_scl_h.d32;
}

/**
 * @if Eng
 * @brief  Set number of clock low-level cycles scl_h.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @param  [in]  val Number of clock low-level cycles scl_h.
 * @else
 * @brief  设置时钟低周期数scl_l。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @param  [in]  val 时钟低周期数scl_l。
 * @endif
 */
static inline void hal_i2c_v150_set_scl_l(i2c_bus_t bus, uint16_t val)
{
    i2c_scl_l_data_t i2c_scl_l = {0};
    i2c_scl_l.b.scl_l = val;
    g_i2c_regs[bus]->i2c_scl_l = i2c_scl_l.d32;
}

/**
 * @if Eng
 * @brief  Set data to send.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @param  [in]  data Data to send, one byte.
 * @else
 * @brief  设置发送数据。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @param  [in]  data 待发送数据, 一个字节。
 * @endif
 */
static inline void hal_i2c_v150_set_tx_data(i2c_bus_t bus, uint8_t data)
{
    i2c_txr_data_t i2c_txr = {0};
    i2c_txr.b.i2c_txr = data;
    g_i2c_regs[bus]->i2c_txr = i2c_txr.d32;
}

/**
 * @if Eng
 * @brief  Set data to receive.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @return Data to receive, one byte.
 * @else
 * @brief  获取接收数据。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @return 接收的数据, 一个字节。
 * @endif
 */
static inline uint8_t hal_i2c_v150_get_rx_data(i2c_bus_t bus)
{
    i2c_rxr_data_t i2c_rxr;
    i2c_rxr.d32 = g_i2c_regs[bus]->i2c_rxr;
    return i2c_rxr.b.i2c_rxr;
}

/**
 * @if Eng
 * @brief  Set number of glitch filtering cycles.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @param  [in]  ftrper Number of glitch filtering cycles, Signal be regard as receive data when
 *               duration is longer than this number. Width 4 bit.
 * @else
 * @brief  设置毛刺过滤周期数。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @param  [in]  ftrper 毛刺过滤周期数, 数据持续时间大于该周期数才被认为是接收值, 宽度为4bit。
 * @endif
 */
static inline void hal_i2c_v150_set_ftrper(i2c_bus_t bus, uint8_t ftrper)
{
    i2c_ftrper_data_t i2c_ftrper = {0};
    i2c_ftrper.b.ftrper = ftrper;
    g_i2c_regs[bus]->i2c_ftrper = i2c_ftrper.d32;
}

/**
 * @if Eng
 * @brief  Get number of glitch filtering cycles.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @return Number of glitch filtering cycles, Signal be regard as receive data when
 *         duration is longer than this number. Width 4 bit.
 * @else
 * @brief  获取毛刺过滤周期数。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @return 毛刺过滤周期数, 数据持续时间大于该周期数才被认为是接收值, 宽度为4bit。
 * @endif
 */
static inline uint8_t hal_i2c_v150_get_ftrper(i2c_bus_t bus)
{
    i2c_ftrper_data_t i2c_ftrper;
    i2c_ftrper.d32 = g_i2c_regs[bus]->i2c_ftrper;
    return i2c_ftrper.b.ftrper;
}

/**
 * @if Eng
 * @brief  Initialize base address of I2C registers.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @else
 * @brief  初始化I2C寄存器基地址。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @endif
 */
void hal_i2c_v150_regs_init(i2c_bus_t bus);

/**
 * @if Eng
 * @brief  Deinitialize base address of I2C registers.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @else
 * @brief  去初始化I2C寄存器基地址。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @endif
 */
void hal_i2c_v150_regs_deinit(i2c_bus_t bus);

/**
 * @if Eng
 * @brief  Reset all writeable registers to initial state.
 * @param  [in]  bus I2C bus id, see @ref i2c_bus_t.
 * @else
 * @brief  设置所有可写寄存器恢复初始状态。
 * @param  [in]  bus I2C总线id, 参考 @ref i2c_bus_t 。
 * @endif
 */
void hal_i2c_v150_reset_all_regs(i2c_bus_t bus);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif