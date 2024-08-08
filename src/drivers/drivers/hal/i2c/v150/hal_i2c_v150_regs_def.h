/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V150 i2c register \n
 *
 * History: \n
 * 2023-03-06, Create file. \n
 */

#ifndef HAL_I2C_V150_REGS_DEF_H
#define HAL_I2C_V150_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_i2c_v150_regs_def I2C V150 Regs Definition
 * @ingroup  drivers_hal_i2c
 * @{
 */

/* I2C寄存器默认值定义 */
#define I2C_CTRL_DEFAULT_VAL        0x0
#define I2C_COM_DEFAULT_VAL         0x0
#define I2C_ICR_DEFAULT_VAL         0x0
#define I2C_SR_DEFAULT_VAL          0x0
#define I2C_SCL_H_DEFAULT_VAL       0x0
#define I2C_SCL_L_DEFAULT_VAL       0x0
#define I2C_TXR_DEFAULT_VAL         0x0
#define I2C_RXR_DEFAULT_VAL         0x0
#define I2C_FIFOSTATUS_DEFAULT_VAL  0x0
#define I2C_TXCOUNT_DEFAULT_VAL     0x0
#define I2C_RXCOUNT_DEFAULT_VAL     0x0
#define I2C_RXTIDE_DEFAULT_VAL      0x1
#define I2C_TXTIDE_DEFAULT_VAL      0x1
#define I2C_FTRPER_DEFAULT_VAL      0xF

/**
 * @if Eng
 * @brief  This union represents the bit fields in I2C_CTRL Register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 *         I2C control register, used to configure the I2C enable and interrupt mask.
 * @else
 * @brief  I2C_CTRL 寄存器联合体定义, 通过<i>d32</i>成员读取寄存器值, 通过<i>b</i>成员设置比特位。
 *         I2C控制寄存器, 用于配置I2C使能和中断屏蔽。
 * @endif
 */
typedef union i2c_ctrl_data {
    uint32_t d32;                       /*!< 寄存器实际数据 */
    struct {
        uint32_t int_done_mask          : 1; /*!< bit[0]
                                                  总线传输完成中断屏蔽。
                                                  0: 屏蔽
                                                  1: 不屏蔽 */
        uint32_t int_arb_loss_mask      : 1; /*!< bit[1]
                                                  总线仲裁失败中断屏蔽。
                                                  0: 屏蔽
                                                  1: 不屏蔽 */
        uint32_t int_ack_err_mask       : 1; /*!< bit[2]
                                                  从机ACK错误中断屏蔽。
                                                  0: 屏蔽
                                                  1: 不屏蔽 */
        uint32_t int_rx_mask            : 1; /*!< bit[3]
                                                  主机接收中断屏蔽。
                                                  0: 屏蔽
                                                  1: 不屏蔽 */
        uint32_t int_tx_mask            : 1; /*!< bit[4]
                                                  主机发送中断屏蔽。
                                                  0: 屏蔽
                                                  1: 不屏蔽 */
        uint32_t int_stop_mask          : 1; /*!< bit[5]
                                                  主机停止条件发送结束中断屏蔽。
                                                  0: 屏蔽
                                                  1: 不屏蔽 */
        uint32_t int_start_mask         : 1; /*!< bit[6]
                                                  主机开始条件发送结束中断屏蔽。
                                                  0: 屏蔽
                                                  1: 不屏蔽 */
        uint32_t int_mask               : 1; /*!< bit[7]
                                                  I2C中断总屏蔽。
                                                  0: 屏蔽
                                                  1: 不屏蔽 */
        uint32_t i2c_en                 : 1; /*!< bit[8]
                                                  I2C使能。
                                                  0: 不使能
                                                  1: 使能 */
        uint32_t int_rxtide_mask        : 1; /*!< bit[9]
                                                  发送FIFO溢出中断屏蔽。
                                                  0: 屏蔽
                                                  1: 不屏蔽 */
        uint32_t int_txtide_mask        : 1; /*!< bit[10]
                                                  发送FIFO溢出中断屏蔽。
                                                  0: 屏蔽
                                                  1: 不屏蔽 */
        uint32_t mode_ctrl              : 1; /*!< bit[11]
                                                  I2C工作模式选择。
                                                  0: 不使用FIFO传输模式
                                                  1: 使用FIFO传输模式 */
        uint32_t int_txfifo_over_mask   : 1; /*!< bit[12]
                                                  发送FIFO数据发送完成中断屏蔽。
                                                  0: 屏蔽
                                                  1: 不屏蔽 */
        uint32_t reserved13_31          : 19; /*!< bit[13:31] 保留位 */
    } b;                                /*!< Register bits. */
} i2c_ctrl_data_t;

/**
 * @if Eng
 * @brief  This union represents the bit fields in I2C_COM Register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 *         I2C command register, used to configure work operation of I2C module.
 *         Corresponding interrupt status should be cleared before or duration I2C initialization.
 *         I2C_COM bit[3:0] is automatically cleared after the operation is complete.
 * @else
 * @brief  I2C_COM 寄存器联合体定义, 通过<i>d32</i>成员读取寄存器值, 通过<i>b</i>成员设置比特位。
 *         I2C模块的命令寄存器, 用于配置I2C模块工作时命令。
 *         在系统初始化时配置或配置前, 需要清除对应中断标志, I2C_COM bit[3:0]在操作结束后将自动清0。
 * @endif
 */
typedef union i2c_com_data {
    uint32_t d32;                       /*!< 寄存器实际数据 */
    struct {
        uint32_t op_stop                : 1; /*!< bit[0]
                                                  产生停止条件操作。
                                                  0: 操作结束。
                                                  1: 操作有效。 */
        uint32_t op_we                  : 1; /*!< bit[1]
                                                  产生写操作。
                                                  0: 操作结束。
                                                  1: 操作有效。 */
        uint32_t op_rd                  : 1; /*!< bit[2]
                                                  产生读操作。
                                                  0: 操作结束。
                                                  1: 操作有效。 */
        uint32_t op_start               : 1; /*!< bit[3]
                                                  产生开始条件操作;
                                                  0: 操作结束。
                                                  1: 操作有效。 */
        uint32_t op_ack                 : 1; /*!< bit[4]
                                                  主机作为接收器是否发送ACK。
                                                  0: 发送。
                                                  1: 不发送。 */
        uint32_t reserved5_31           : 27; /*!< bit[5:31] 保留位 */
    } b;                                /*!< 寄存器比特位 */
} i2c_com_data_t;

/**
 * @if Eng
 * @brief  This union represents the bit fields in I2C_ICR Register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 *         I2C interrupt status clear register, used to clear interrupt status.
 *         Corresponding bit will be set to 0 when new interrupt rised.
 * @else
 * @brief  I2C_ICR 寄存器联合体定义, 通过<i>d32</i>成员读取寄存器值, 通过<i>b</i>成员设置比特位。
 *         I2C模块的中断清除寄存器。
 *         新中断到来时, I2C模块会自动将I2C_ICR相应位清0。
 * @endif
 */
typedef union i2c_icr_data {
    uint32_t d32;                       /*!< 寄存器实际数据 */
    struct {
        uint32_t clr_int_done           : 1; /*!< bit[0]
                                                  总线传输完成中断标志清除。
                                                  0: 不清除
                                                  1: 清除 */
        uint32_t clr_int_arb_loss       : 1; /*!< bit[1]
                                                  总线仲裁失败中断标志清除。
                                                  0: 不清除
                                                  1: 清除 */
        uint32_t clr_int_ack_err        : 1; /*!< bit[2]
                                                  从机ACK错误中断标志清除。
                                                  0: 不清除
                                                  1: 清除 */
        uint32_t clr_int_rx             : 1; /*!< bit[3]
                                                  主机接收中断标志清除。
                                                  0: 不清除
                                                  1: 清除 */
        uint32_t clr_int_tx             : 1; /*!< bit[4]
                                                  主机发送中断标志清除。
                                                  0: 不清除
                                                  1: 清除 */
        uint32_t clr_int_stop           : 1; /*!< bit[5]
                                                  主机停止条件发送结束中断标志清除。
                                                  0: 不清除
                                                  1: 清除 */
        uint32_t clr_int_start          : 1; /*!< bit[6]
                                                  主机开始条件发送结束中断标志清除。
                                                  0: 不清除
                                                  1: 清除 */
        uint32_t clr_int_rxtide         : 1; /*!< bit[7]
                                                  接收FIFO溢出中断标志清除。
                                                  0: 不清除
                                                  1: 清除 */
        uint32_t clr_int_txtide         : 1; /*!< bit[8]
                                                  发送FIFO溢出中断标志清除。
                                                  0: 不清除
                                                  1: 清除 */
        uint32_t clr_int_txfifo_over    : 1; /*!< bit[9]
                                                  发送FIFO数据发送完成中断标志清除。
                                                  0: 不清除
                                                  1: 清除 */
        uint32_t reserved10_31          : 22; /*!< bit[10:31] 保留位 */
    } b;                                /*!< 寄存器比特位 */
} i2c_icr_data_t;

/**
 * @if Eng
 * @brief  This union represents the bit fields in I2C_SR Register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 *         I2C status register, used to get work status of I2C module.
 *         I2C_SR bit[1] indicates I2C arbitrate failed, when this bit enabled, the last operation is failed.
 *         We should clear other interrupt flags before clear bit[1], then we clear I2C_COM or write new operation
 *         to I2C_COM, finally we can clear I2C_SR bit[1].
 * @else
 * @brief  I2C_SR 寄存器联合体定义, 通过<i>d32</i>成员读取寄存器值, 通过<i>b</i>成员设置比特位。
 *         I2C模块状态寄存器, 用于读取I2C模块工作状态。
 *         I2C_SR bit[1]表示I2C总线仲裁失败, 当I2C_SR bit[1]有效时, 当前操作失败, 在清I2C_SR bit[1]之前,
 *         需要清除其他中断标志, 然后清除I2C_COM或向I2C_COM写入新的操作命令, 最后清除I2C_SR bit[1]。
 * @endif
 */
typedef union i2c_sr_data {
    uint32_t d32;                       /*!< 寄存器实际数据 */
    struct {
        uint32_t int_done               : 1; /*!< bit[0]
                                                  总线传输完成中断标志。
                                                  0: 无中断标志产生
                                                  1: 中断标志产生 */
        uint32_t int_arb_loss           : 1; /*!< bit[1]
                                                  总线仲裁失败中断标志。
                                                  0: 无中断标志产生
                                                  1: 中断标志产生 */
        uint32_t int_ack_err            : 1; /*!< bit[2]
                                                  从机ACK错误中断标志。
                                                  0: 无中断标志产生
                                                  1: 中断标志产生 */
        uint32_t int_rx                 : 1; /*!< bit[3]
                                                  主机接收中断标志。
                                                  0: 无中断标志产生
                                                  1: 中断标志产生 */
        uint32_t int_tx                 : 1; /*!< bit[4]
                                                  主机发送中断标志。
                                                  0: 无中断标志产生
                                                  1: 中断标志产生 */
        uint32_t int_stop               : 1; /*!< bit[5]
                                                  主机停止条件发送结束中断标志。
                                                  0: 无中断标志产生
                                                  1: 中断标志产生 */
        uint32_t int_start              : 1; /*!< bit[6]
                                                  主机开始条件发送结束中断标志。
                                                  0: 无中断标志产生
                                                  1: 中断标志产生 */
        uint32_t bus_busy               : 1; /*!< bit[7]
                                                  总线忙。
                                                  0: 空闲
                                                  1: 忙 */
        uint32_t int_rxtide             : 1; /*!< bit[8]
                                                  接收FIFO溢出中断标志。
                                                  0: 无中断标志产生
                                                  1: 中断标志产生 */
        uint32_t int_txtide             : 1; /*!< bit[9]
                                                  发送FIFO溢出中断标志。
                                                  0: 无中断标志产生
                                                  1: 中断标志产生 */
        uint32_t int_txfifo_over        : 1; /*!< bit[10]
                                                  发送FIFO数据发送完成中断标志。
                                                  0: 无中断标志产生
                                                  1: 中断标志产生 */
        uint32_t reserved11_31          : 21; /*!< bit[11:31] 保留位 */
    } b;                                /*!< 寄存器比特位 */
} i2c_sr_data_t;

/**
 * @if Eng
 * @brief  This union represents the bit fields in I2C_SCL_H Register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 *         I2C serial clock high register, used to set number of clock high-level cycles.
 *         I2C_SCL_H should be configed in I2C initialization or after setting I2C_CTRL bit[7] to 0.
 * @else
 * @brief  I2C_SCL_H 寄存器联合体定义, 通过<i>d32</i>成员读取寄存器值, 通过<i>b</i>成员设置比特位。
 *         I2C总线SCL信号高电平周期数寄存器, 用于配置I2C模块工作时SCL高电平周期数。
 *         在系统初始化时配置或配置前使I2C_CTRL bit[7]=0。
 * @endif
 */
typedef union i2c_scl_h_data {
    uint32_t d32;                       /*!< 寄存器实际数据 */
    struct {
        uint32_t scl_h                  : 16; /*!< bit[0:15]
                                                  配置数值乘2等于SCL高电平周期数。 */
        uint32_t reserved16_31          : 16; /*!< bit[16:31] 保留位 */
    } b;                                /*!< 寄存器比特位 */
} i2c_scl_h_data_t;

/**
 * @if Eng
 * @brief  This union represents the bit fields in I2C_SCL_L Register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 *         I2C serial clock low register, used to set number of clock low-level cycles.
 *         I2C_SCL_L should be configed in I2C initialization or after setting I2C_CTRL bit[7] to 0.
 * @else
 * @brief  I2C_SCL_L 寄存器联合体定义, 通过<i>d32</i>成员读取寄存器值, 通过<i>b</i>成员设置比特位。
 *         I2C总线SCL信号低电平周期数寄存器, 用于配置I2C模块工作时SCL低电平周期数。
 *         在系统初始化时配置或配置前使I2C_CTRL bit[7]=0。
 * @endif
 */
typedef union i2c_scl_l_data {
    uint32_t d32;                       /*!< 寄存器实际数据 */
    struct {
        uint32_t scl_l                  : 16; /*!< bit[0:15]
                                                  配置数值乘2等于SCL底电平周期数。 */
        uint32_t reserved16_31          : 16; /*!< bit[16:31] 保留位 */
    } b;                                /*!< 寄存器比特位 */
} i2c_scl_l_data_t;

/**
 * @if Eng
 * @brief  This union represents the bit fields in I2C_TXR Register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 *         I2C tx data register, used to set data to send.
 *         When FIFO disabled, value of I2C_TXR will not be changed after transmission.
 *         When FIFO enabled, data writen to this reg will be load into fifo automatically
 *         until transmission finished.
 * @else
 * @brief  I2C_TXR 寄存器联合体定义, 通过<i>d32</i>成员读取寄存器值, 通过<i>b</i>成员设置比特位。
 *         I2C发送数据寄存器, 用于配置I2C模块工作时发送数据。
 *         不使用FIFO模式下, 发送结束后, I2C模块不会修改I2C_TXR内容。
 *         使用FIFO模式下, 写入的数据会自动载入到发送FIFO中保存直到该数据发送结束。
 * @endif
 */
typedef union i2c_txr_data {
    uint32_t d32;                       /*!< 寄存器实际数据 */
    struct {
        uint32_t i2c_txr                : 8; /*!< bit[0:7]
                                                  主机发送数据。 */
        uint32_t reserved8_31           : 24; /*!< bit[8:31] 保留位 */
    } b;                                /*!< 寄存器比特位 */
} i2c_txr_data_t;

/**
 * @if Eng
 * @brief  This union represents the bit fields in I2C_RXR Register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 *         I2C rx data register, used to get data received.
 *         When FIFO disabled, data in I2C_RXR is valid when I2C_SR bit[3]=1.
 *         When FIFO enabled, read I2C_RXR directly fetches data from RX FIFO.
 * @else
 * @brief  I2C_RXR 寄存器联合体定义, 通过<i>d32</i>成员读取寄存器值, 通过<i>b</i>成员设置比特位。
 *         I2C接收数据寄存器, 用于主机接收从机数据。
 *         不使用FIFO模式下, I2C_RXR数据在I2C_SR bit[3]=1时, 数据有效, 同时数据将保持到下一个读操作前。
 *         使用FIFO模式下, 读取I2C_RXR会直接从接收FIFO中取数据。
 * @endif
 */
typedef union i2c_rxr_data {
    uint32_t d32;                       /*!< 寄存器实际数据 */
    struct {
        uint32_t i2c_rxr                : 8; /*!< bit[0:7]
                                                  主机接收数据。 */
        uint32_t reserved8_31           : 24; /*!< bit[8:31] 保留位 */
    } b;                                /*!< 寄存器比特位 */
} i2c_rxr_data_t;

/**
 * @if Eng
 * @brief  This union represents the bit fields in I2C_FIFOSTATUS Register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 *         I2C FIFO status register, used to get fifo status.
 * @else
 * @brief  I2C_FIFOSTATUS 寄存器联合体定义, 通过<i>d32</i>成员读取寄存器值, 通过<i>b</i>成员设置比特位。
 *         FIFO状态寄存器。
 * @endif
 */
typedef union i2c_fifostatus_data {
    uint32_t d32;                       /*!< 寄存器实际数据 */
    struct {
        uint32_t txff                   : 1; /*!< bit[0]
                                                  发送FIFO满状态。
                                                  0: 未满
                                                  1: 满 */
        uint32_t txfe                   : 1; /*!< bit[1]
                                                  发送FIFO空状态。
                                                  0: 非空
                                                  1: 空 */
        uint32_t rxff                   : 1; /*!< bit[2]
                                                  接收FIFO满状态。
                                                  0: 未满
                                                  1: 满 */
        uint32_t rxfe                   : 1; /*!< bit[3]
                                                  接收FIFO空状态。
                                                  0: 非空
                                                  1: 空 */
        uint32_t reserved4_31           : 28; /*!< bit[4:31] 保留位 */
    } b;                                /*!< 寄存器比特位 */
} i2c_fifostatus_data_t;

/**
 * @if Eng
 * @brief  This union represents the bit fields in I2C_TXCOUNT Register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 *         I2C tx data cnt register.
 * @else
 * @brief  I2C_TXCOUNT 寄存器联合体定义, 通过<i>d32</i>成员读取寄存器值, 通过<i>b</i>成员设置比特位。
 *         发送FIFO数据个数寄存器。
 * @endif
 */
typedef union i2c_txcount_data {
    uint32_t d32;                       /*!< 寄存器实际数据 */
    struct {
        uint32_t txcount                : 6; /*!< bit[0:5]
                                                  读该寄存器返回发送FIFO中的字符数。
                                                  写该寄存器(任意值)将清空发送FIFO。 */
        uint32_t reserved6_31           : 26; /*!< bit[6:31] 保留位 */
    } b;                                /*!< 寄存器比特位 */
} i2c_txcount_data_t;

/**
 * @if Eng
 * @brief  This union represents the bit fields in I2C_RXCOUNT Register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 *         I2C rx data cnt register.
 * @else
 * @brief  I2C_RXCOUNT 寄存器联合体定义, 通过<i>d32</i>成员读取寄存器值, 通过<i>b</i>成员设置比特位。
 *         接收FIFO数据个数寄存器。
 * @endif
 */
typedef union i2c_rxcount_data {
    uint32_t d32;                       /*!< 寄存器实际数据 */
    struct {
        uint32_t rxcount                : 6; /*!< bit[0:5]
                                                  读该寄存器返回接收FIFO中的字符数。
                                                  写该寄存器(任意值)将清空接收FIFO。 */
        uint32_t reserved6_31           : 26; /*!< bit[6:31] 保留位 */
    } b;                                /*!< 寄存器比特位 */
} i2c_rxcount_data_t;

/**
 * @if Eng
 * @brief  This union represents the bit fields in I2C_RXTIDE Register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 *         I2C rx fifo overflow thershold register.
 * @else
 * @brief  I2C_RXTIDE 寄存器联合体定义, 通过<i>d32</i>成员读取寄存器值, 通过<i>b</i>成员设置比特位。
 *         接收FIFO的溢出阈值寄存器。
 * @endif
 */
typedef union i2c_rxtide_data {
    uint32_t d32;                       /*!< 寄存器实际数据 */
    struct {
        uint32_t rxtide                 : 6; /*!< bit[0:5]
                                                  设置int_rxtide中断的触发值。
                                                  当 RX_FIFO中的字符个数 ≥ I2C_RXTIDE[rxtide] 时
                                                  会触发接收FIFO溢出中断。 */
        uint32_t reserved6_31           : 26; /*!< bit[6:31] 保留位 */
    } b;                                /*!< 寄存器比特位 */
} i2c_rxtide_data_t;

/**
 * @if Eng
 * @brief  This union represents the bit fields in I2C_TXTIDE Register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 *         I2C tx fifo overflow thershold register. Data in TXFIFO will be removed only after successful tx.
 * @else
 * @brief  I2C_TXTIDE 寄存器联合体定义, 通过<i>d32</i>成员读取寄存器值, 通过<i>b</i>成员设置比特位。
 *         接收FIFO的溢出阈值寄存器。
 *         TXFIFO中的字符只有在成功发送后才会被移除。
 * @endif
 */
typedef union i2c_txtide_data {
    uint32_t d32;                       /*!< 寄存器实际数据 */
    struct {
        uint32_t txtide                 : 6; /*!< bit[0:5]
                                                  设置int_txtide中断的触发值。
                                                  当 TX_FIFO中的字符个数 ≤ I2C_TXTIDE[txtide] 时
                                                  会触发发送FIFO溢出中断。 */
        uint32_t reserved6_31           : 26; /*!< bit[6:31] 保留位 */
    } b;                                /*!< 寄存器比特位 */
} i2c_txtide_data_t;

/**
 * @if Eng
 * @brief  This union represents the bit fields in I2C_FTRPER Register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 *         I2C glitch filtering cycles number register.
 *         Determine high-level duration of sda signal, unit is the number of ic_clk cycles.
 *         Signal be regard as receive data when duration is longer than this number.
 * @else
 * @brief  I2C_FTRPER 寄存器联合体定义, 通过<i>d32</i>成员读取寄存器值, 通过<i>b</i>成员设置比特位。
 *         毛刺过滤寄存器。
 *         scl为高时，判断sda电平持续时间，单位为ic_clk的时钟个数，持续时间大于该值才认为该电平为接收值。
 * @endif
 */
typedef union i2c_ftrper_data {
    uint32_t d32;                       /*!< 寄存器实际数据 */
    struct {
        uint32_t ftrper                 : 4; /*!< bit[0:3]
                                                  默认15个时钟周期, 需要SDA需要再SCL为高时, 电平保持的时间。 */
        uint32_t reserved4_31           : 28; /*!< bit[6:31] 保留位 */
    } b;                                /*!< 寄存器比特位 */
} i2c_ftrper_data_t;

/**
 * @if Eng
 * @brief  I2C registers.
 * @else
 * @brief  I2C相关寄存器。
 * @endif
 */
typedef struct i2c_regs {
    volatile uint32_t i2c_ctrl;         /*!< I2C控制寄存器 <i>Offset: 0x00</i>. */
    volatile uint32_t i2c_com;          /*!< I2C命令寄存器 <i>Offset: 0x04</i>. */
    volatile uint32_t i2c_icr;          /*!< I2C中断清除寄存器 <i>Offset: 0x08</i>. */
    volatile uint32_t i2c_sr;           /*!< I2C状态寄存器 <i>Offset: 0x0C</i>. */
    volatile uint32_t i2c_scl_h;        /*!< I2C总线SCL信号高电平周期数寄存器 <i>Offset: 0x10</i>. */
    volatile uint32_t i2c_scl_l;        /*!< I2C总线SCL信号低电平周期数寄存器 <i>Offset: 0x14</i>. */
    volatile uint32_t i2c_txr;          /*!< I2C发送数据寄存器 <i>Offset: 0x18</i>. */
    volatile uint32_t i2c_rxr;          /*!< I2C接收数据寄存器 <i>Offset: 0x1C</i>. */
    volatile uint32_t i2c_fifostatus;   /*!< FIFO状态寄存器 <i>Offset: 0x20</i>. */
    volatile uint32_t i2c_txcount;      /*!< 发送FIFO数据个数寄存器 <i>Offset: 0x24</i>. */
    volatile uint32_t i2c_rxcount;      /*!< 接收FIFO数据个数寄存器 <i>Offset: 0x28</i>. */
    volatile uint32_t i2c_rxtide;       /*!< 接收FIFO的溢出阈值寄存器 <i>Offset: 0x2C</i>. */
    volatile uint32_t i2c_txtide;       /*!< 发送FIFO的溢出阈值寄存器 <i>Offset: 0x30</i>. */
    volatile uint32_t i2c_ftrper;       /*!< I2C毛刺过滤寄存器 <i>Offset: 0x34</i>. */
} i2c_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

