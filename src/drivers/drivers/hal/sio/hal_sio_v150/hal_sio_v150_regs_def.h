/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides v150 sio register \n
 *
 * History: \n
 * 2023-01-15, Create file. \n
 */
#ifndef HAL_SIO_V150_REGS_DEF_H
#define HAL_SIO_V150_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_sio_v150_regs_def SIO V150 Regs Definition
 * @ingroup  drivers_hal_sio
 * @{
 */

/**
 * @brief  This union represents the bit fields in the sio_version register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_version_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t vesion                 : 8;    /*!< sio version. */
        uint32_t loop                   : 1;    /*!< This bit indicates whether enter loop mode or not.
                                                     0: standard mode
                                                     1: loop mode. */
        uint32_t reserved9_31           : 23;   /*!< Reserved */
    } b;                                        /*!< Register bits. */
} sio_v150_version_data_t;

/**
 * @brief  This union represents the bit fields in the sio_mode register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_mode_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t mode                   : 1;    /*!< This bit indicates sio mode.
                                                     0: i2s mode
                                                     1: pcm mode. */
        uint32_t pcm_mode               : 1;    /*!< This bit indicates pcm time mode.
                                                     0: i2s mode
                                                     1: self-defined mode. */
        uint32_t reserved2              : 1;    /*!< Reserved */
        uint32_t ext_rec_en             : 1;    /*!< This bit indicates sio mode.
                                                     0: standard receive mode
                                                     1: multi-channel receive mode. */
        uint32_t chn_num                : 2;    /*!< This bit indicates receive channel in multi-channel receive mode.
                                                     00: 2 channel
                                                     01: 4 channel
                                                     10: 8 channel
                                                     11: 16 channel */
        uint32_t clk_edge               : 1;    /*!< This bit indicates clock edge in pcm multi-channel receive mode.
                                                     0: rising edge
                                                     1: descending edge. */
        uint32_t reserved7_31           : 25;   /*!< Reserved */
    } b;                                        /*!< Register bits. */
} sio_v150_mode_data_t;

/**
 * @brief  This union represents the bit fields in the sio_intstatus register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_intstatus_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t rx_intr                : 1;    /*!< The interrupt when rx fifo above threshold. */
        uint32_t tx_intr                : 1;    /*!< The interrupt when tx fifo above threshold. */
        uint32_t rx_right_fifo_over     : 1;    /*!< The interrupt when right tx fifo overflow. */
        uint32_t rx_left_fifo_over      : 1;    /*!< The interrupt when left tx fifo overflow. */
        uint32_t tx_right_fifo_under    : 1;    /*!< The interrupt when right tx fifo underflow. */
        uint32_t tx_left_fifo_under     : 1;    /*!< The interrupt when left tx fifo underflow. */
        uint32_t reserved6_31           : 26;   /*!< Reserved */
    } b;                                        /*!< Register bits. */
} sio_v150_intstatus_data_t;

/**
 * @brief  This union represents the bit fields in the sio_intclr register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_intclr_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t rx_intr                : 1;    /*!< Clear the interrupt when rx fifo above threshold. */
        uint32_t tx_intr                : 1;    /*!< Clear the interrupt when tx fifo above threshold. */
        uint32_t rx_right_fifo_over     : 1;    /*!< Clear the interrupt when right tx fifo overflow. */
        uint32_t rx_left_fifo_over      : 1;    /*!< Clear the interrupt when left tx fifo overflow. */
        uint32_t tx_right_fifo_under    : 1;    /*!< Clear the interrupt when right tx fifo underflow. */
        uint32_t tx_left_fifo_under     : 1;    /*!< Clear the interrupt when left tx fifo underflow. */
        uint32_t reserved6_31           : 26;   /*!< Reserved */
    } b;                                        /*!< Register bits. */
} sio_v150_intclr_data_t;

/**
 * @brief  This union represents the bit fields in the left_tx_data register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_left_tx_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t data                   : 32;   /*!< Left tx data. */
    } b;                                        /*!< Register bits. */
} sio_v150_left_tx_data_t;

/**
 * @brief  This union represents the bit fields in the right_tx_data register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_right_tx_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t data                   : 32;   /*!< Right tx data. */
    } b;                                        /*!< Register bits. */
} sio_v150_right_tx_data_t;

/**
 * @brief  This union represents the bit fields in the left_rx_data register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_left_rx_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t data                   : 32;   /*!< Left rx data. */
    } b;                                        /*!< Register bits. */
} sio_v150_left_rx_data_t;

/**
 * @brief  This union represents the bit fields in the right_rx_data register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_right_rx_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t data                   : 32;   /*!< Right rx data. */
    } b;                                        /*!< Register bits. */
} sio_v150_right_rx_data_t;

/**
 * @brief  This union represents the bit fields in the sio_ct_set register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_ct_set_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t tx_fifo_threshold      : 4;    /*!< TX fifo threshold. */
        uint32_t rx_fifo_threshold      : 4;    /*!< RX fifo threshold. */
        uint32_t tx_data_merge_en       : 1;    /*!< TX data merge enable.
                                                     0: enable
                                                     1: disable. */
        uint32_t rx_data_merge_en       : 1;    /*!< RX data merge enable.
                                                     0: enable
                                                     1: disable. */
        uint32_t tx_fifo_disable        : 1;    /*!< TX fifo enable.
                                                     0: enable
                                                     1: disable. */
        uint32_t rx_fifo_disable        : 1;    /*!< RX fifo enable.
                                                     0: enable
                                                     1: disable. */
        uint32_t tx_enable              : 1;    /*!< TX enable.
                                                     0: enable
                                                     1: disable. */
        uint32_t rx_enable              : 1;    /*!< RX enable.
                                                     0: enable
                                                     1: disable. */
        uint32_t intr_en                : 1;    /*!< Global interrupt enable.
                                                     0: enable
                                                     1: disable. */
        uint32_t rst_n                  : 1;    /*!< SIO channel reset. */
        uint32_t reserved16_31          : 16;   /*!< Reserved */
    } b;                                        /*!< Register bits. */
} sio_v150_ct_set_data_t;

/**
 * @brief  This union represents the bit fields in the sio_ct_clr register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_ct_clr_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t tx_fifo_threshold      : 4;    /*!< TX fifo threshold. */
        uint32_t rx_fifo_threshold      : 4;    /*!< RX fifo threshold. */
        uint32_t tx_data_merge_en       : 1;    /*!< TX data merge enable.
                                                     0: enable
                                                     1: disable. */
        uint32_t rx_data_merge_en       : 1;    /*!< RX data merge enable.
                                                     0: enable
                                                     1: disable. */
        uint32_t tx_fifo_disable        : 1;    /*!< TX fifo enable.
                                                     0: enable
                                                     1: disable. */
        uint32_t rx_fifo_disable        : 1;    /*!< RX fifo enable.
                                                     0: enable
                                                     1: disable. */
        uint32_t tx_enable              : 1;    /*!< TX enable.
                                                     0: enable
                                                     1: disable. */
        uint32_t rx_enable              : 1;    /*!< RX enable.
                                                     0: enable
                                                     1: disable. */
        uint32_t intr_en                : 1;    /*!< Global interrupt enable.
                                                     0: enable
                                                     1: disable. */
        uint32_t rst_n                  : 1;    /*!< SIO channel reset. */
        uint32_t reserved16_31          : 16;   /*!< Reserved */
    } b;                                        /*!< Register bits. */
} sio_v150_ct_clr_data_t;

/**
 * @brief  This union represents the bit fields in the sio_rx_sta register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_rx_sta_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t rx_right_depth         : 5;    /*!< RX right depth. */
        uint32_t rx_left_depth          : 5;    /*!< RX left depth. */
        uint32_t reserved10_31          : 22;   /*!< Reserved */
    } b;                                        /*!< Register bits. */
} sio_v150_rx_sta_data_t;

/**
 * @brief  This union represents the bit fields in the sio_tx_sta register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_tx_sta_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t tx_right_depth         : 5;    /*!< TX right depth. */
        uint32_t tx_left_depth          : 5;    /*!< TX left depth. */
        uint32_t reserved10_31          : 22;   /*!< Reserved */
    } b;                                        /*!< Register bits. */
} sio_v150_tx_sta_data_t;

/**
 * @brief  This union represents the bit fields in the sio_data_width_set register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_data_width_set_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t tx                     : 3;    /*!< TX data width set data. */
        uint32_t rx                     : 3;    /*!< RX data width set data. */
        uint32_t reserved16_31          : 26;   /*!< Reserved */
    } b;                                        /*!< Register bits. */
} sio_v150_data_width_set_data_t;

/**
 * @brief  This union represents the bit fields in the sio_i2s_start_pos register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_i2s_start_pos_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t start_pos_read         : 1;    /*!< This bit indicates which channel start read.
                                                     0: left channel
                                                     1: right channel. */
        uint32_t start_pos_write        : 1;    /*!< This bit indicates which channel start write.
                                                     0: left channel
                                                     1: right channel. */
        uint32_t reserved16_31          : 30;   /*!< Reserved */
    } b;                                        /*!< Register bits. */
} sio_v150_i2s_start_pos_data_t;

/**
 * @brief  This union represents the bit fields in the sio_pos_flag register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_i2s_pos_flag_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t start_pos_read         : 1;    /*!< This bit indicates which channel next read.
                                                     0: left channel
                                                     1: right channel. */
        uint32_t start_pos_write        : 1;    /*!< This bit indicates which channel next write.
                                                     0: left channel
                                                     1: right channel. */
        uint32_t reserved16_31          : 30;   /*!< Reserved */
    } b;                                        /*!< Register bits. */
} sio_v150_i2s_pos_flag_data_t;

/**
 * @brief  This union represents the bit fields in the sio_signed_ext register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_signed_ext_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t signed_ext_en          : 1;    /*!< Upper bit data symbol extension enable. */
        uint32_t reserved16_31          : 31;   /*!< Reserved */
    } b;                                        /*!< Register bits. */
} sio_v150_signed_ext_data_t;

/**
 * @brief  This union represents the bit fields in the sio_i2s_pos_merge_en register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_i2s_pos_merge_en_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t merge_en               : 1;    /*!< I2S pos merge en. */
        uint32_t reserved16_31          : 31;   /*!< Reserved */
    } b;                                        /*!< Register bits. */
} sio_v150_i2s_pos_merge_en_data_t;

/**
 * @brief  This union represents the bit fields in the sio_intmask register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_intmask_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t rx_intr                : 1;    /*!< Mask the interrupt when rx fifo above threshold. */
        uint32_t tx_intr                : 1;    /*!< Mask the interrupt when tx fifo above threshold. */
        uint32_t rx_right_fifo_over     : 1;    /*!< Mask the interrupt when right tx fifo overflow. */
        uint32_t rx_left_fifo_over      : 1;    /*!< Mask the interrupt when left tx fifo overflow. */
        uint32_t tx_right_fifo_under    : 1;    /*!< Mask the interrupt when right tx fifo underflow. */
        uint32_t tx_left_fifo_under     : 1;    /*!< Mask the interrupt when left tx fifo underflow. */
        uint32_t reserved6_31           : 26;   /*!< Reserved */
    } b;                                        /*!< Register bits. */
} sio_v150_intmask_data_t;

/**
 * @brief  This union represents the bit fields in the sio_i2s_dual_rx_chn register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_i2s_dual_rx_chn_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t rx_data                : 32;   /*!< Receive data. */
    } b;                                        /*!< Register bits. */
} sio_v150_i2s_dual_rx_chn_data_t;

/**
 * @brief  This union represents the bit fields in the sio_i2s_dual_tx_chn register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v150_i2s_dual_tx_chn_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t tx_data                : 32;   /*!< Send data. */
    } b;                                        /*!< Register bits. */
} sio_v150_i2s_dual_tx_chn_data_t;

/**
 * @brief  Registers associated with Sio.
 */
typedef struct sio_v150_regs {
    volatile uint32_t version;                          /*!< sio_version. <i>Offset: 3Ch</i>. */
    volatile uint32_t mode;                             /*!< sio_mode. <i>Offset: 40h</i>. */
    volatile uint32_t intstatus;                        /*!< sio_intstatus. <i>Offset: 44h</i>. */
    volatile uint32_t intclr;                           /*!< sio_intclr. <i>Offset: 48h</i>. */
    volatile uint32_t left_tx;                          /*!< sio_i2s_left_xd. <i>Offset: 4Ch</i>. */
    volatile uint32_t right_tx;                         /*!< sio_i2s_right_xd. <i>Offset: 50h</i>. */
    volatile uint32_t left_rx;                          /*!< sio_i2s_left_rd. <i>Offset: 54h</i>. */
    volatile uint32_t right_rx;                         /*!< sio_i2s_right_rd. <i>Offset: 58h</i>. */
    volatile uint32_t ct_set;                           /*!< sio_ct_set. <i>Offset: 5Ch</i>. */
    volatile uint32_t ct_clr;                           /*!< sio_ct_clr. <i>Offset: 60h</i>. */
    volatile uint32_t reserved_reg;                     /*!< sio_reserved_reg. <i>Offset: 64h</i>. */
    volatile uint32_t rx_sta;                           /*!< sio_rx_sta. <i>Offset: 68h</i>. */
    volatile uint32_t tx_sta;                           /*!< sio_tx_sta. <i>Offset: 6Ch</i>. */
    volatile uint32_t reserved_reg1[2];                 /*!< sio_reserved_reg. <i>Offset: 70h</i>. */
    volatile uint32_t data_width_set;                   /*!< sio_data_width_set. <i>Offset: 78h</i>. */
    volatile uint32_t i2s_start_pos;                    /*!< sio_i2s_start_pos. <i>Offset: 7Ch</i>. */
    volatile uint32_t i2s_pos_flag;                     /*!< i2s_pos_flag. <i>Offset: 80h</i>. */
    volatile uint32_t signed_ext;                       /*!< sio_signed_ext. <i>Offset: 84h</i>. */
    volatile uint32_t i2s_pos_merge_en;                 /*!< sio_i2s_pos_merge_en. <i>Offset: 88h</i>. */
    volatile uint32_t intmask;                          /*!< sio_intmask. <i>Offset: 8Ch</i>. */
    volatile uint32_t reserved_reg2[4];                 /*!< sio_reserved_reg. <i>Offset: 8Ch</i>. */
    volatile uint32_t i2s_dual_rx_chn;                  /*!< sio_i2s_dual_rx_chn. <i>Offset: A0h</i>. */
    volatile uint32_t reserved_reg3[7];                 /*!< sio_reserved_reg. <i>Offset: A4h</i>. */
    volatile uint32_t i2s_dual_tx_chn;                  /*!< sio_i2s_dual_tx_chn. <i>Offset: C0h</i>. */
} sio_v150_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif