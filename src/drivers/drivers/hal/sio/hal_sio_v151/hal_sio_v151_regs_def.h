/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides v151 sio register \n
 *
 * History: \n
 * 2023-03-07, Create file. \n
 */
#ifndef HAL_SIO_V151_REGS_DEF_H
#define HAL_SIO_V151_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_sio_v151_regs_def SIO V151 Regs Definition
 * @ingroup  drivers_hal_sio
 * @{
 */

/**
 * @brief  This union represents the bit fields in the sio_version register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_version_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t version                    : 8;        /*!< sio_version. */
        uint32_t loop                       : 1;        /*!< This bit indicates whether enter loop mode or not.
                                                             0: standard mode.
                                                             1: loop mode. */
        uint32_t reserved9_31               : 23;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_version_data_t;

/**
 * @brief  This union represents the bit fields in the sio_mode register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_mode_data {
    uint32_t d32;                           /*!< Raw register data. */
    struct {
        uint32_t mode                       : 1;        /*!< sio mode.
                                                             0: i2s mode.
                                                             1: pcm mode. */
        uint32_t pcm_mode                   : 1;        /*!< This bit indicates pcm time mode.
                                                             0: i2s mode.
                                                             1: self-defined mode. */
        uint32_t rxws_select                : 1;        /*!< select rx mode ws. */
        uint32_t ext_rec_en                 : 1;        /*!< This bit indicates sio mode.
                                                             0: standard receive mode.
                                                             1: multi-channel receive mode. */
        uint32_t chn_num                    : 2;        /*!< This bit indicates receive channel.
                                                             00: 2 channel.
                                                             01: 4 channel.
                                                             10: 8 channel.
                                                             11: 16 channel */
        uint32_t clk_edge                   : 1;        /*!< This bit indicates clock edge.
                                                             0: rising edge.
                                                             1: descending edge. */
        uint32_t cfg_i2s_ms_mode_sel        : 1;        /*!< SIO mode is master/slave. */
        uint32_t reserved8_31               : 22;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_mode_data_t;

/**
 * @brief  This union represents the bit fields in the sio_intstatus register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_intstatus_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t rx_intr                    : 1;        /*!< The interrupt when rx fifo above threshold. */
        uint32_t tx_intr                    : 1;        /*!< The interrupt when tx fifo above threshold. */
        uint32_t rx_right_fifo_over         : 1;        /*!< The interrupt when right tx fifo overflow. */
        uint32_t rx_left_fifo_over          : 1;        /*!< The interrupt when left tx fifo overflow. */
        uint32_t tx_right_fifo_under        : 1;        /*!< The interrupt when right tx fifo underflow. */
        uint32_t tx_left_fifo_under         : 1;        /*!< The interrupt when left tx fifo underflow. */
        uint32_t reserved6_31               : 26;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_intstatus_data_t;

/**
 * @brief  This union represents the bit fields in the sio_intclr register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_intclr_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t rx_intr                    : 1;        /*!< The interrupt when rx fifo above threshold. */
        uint32_t tx_intr                    : 1;        /*!< The interrupt when tx fifo above threshold. */
        uint32_t rx_right_fifo_over         : 1;        /*!< The interrupt when right tx fifo overflow. */
        uint32_t rx_left_fifo_over          : 1;        /*!< The interrupt when left tx fifo overflow. */
        uint32_t tx_right_fifo_under        : 1;        /*!< The interrupt when right tx fifo underflow. */
        uint32_t tx_left_fifo_under         : 1;        /*!< The interrupt when left tx fifo underflow. */
        uint32_t reserved6_31               : 26;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_intclr_data_t;

/**
 * @brief  This union represents the bit fields in the sio_i2s_left_xd register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_i2s_left_xd_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t tx_left_data               : 32;       /*!< Send left channel data. */
    } b;                                                /*!< Register bits. */
} sio_v151_i2s_left_xd_data_t;

/**
 * @brief  This union represents the bit fields in the sio_i2s_right_xd register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_i2s_right_xd_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t tx_right_data               : 32;      /*!< Send right channel data. */
    } b;                                                /*!< Register bits. */
} sio_v151_i2s_right_xd_data_t;

/**
 * @brief  This union represents the bit fields in the sio_i2s_left_rd register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_i2s_left_rd_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t rx_left_data               : 32;       /*!< Recevie left channel data. */
    } b;                                                /*!< Register bits. */
} sio_v151_i2s_left_rd_data_t;

/**
 * @brief  This union represents the bit fields in the sio_i2s_right_xd register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_i2s_right_rd_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t rx_right_data               : 32;      /*!< Recevie right channel data. */
    } b;                                                /*!< Register bits. */
} sio_v151_i2s_right_rd_data_t;
/**
 * @brief  This union represents the bit fields in the sio_ct_set register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_ct_set_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t reserved0_3                : 4;        /*!< Reserved. */
        uint32_t reserved4_7                : 4;        /*!< Reserved. */
        uint32_t tx_data_merge_en           : 1;        /*!< tx data merge enable.
                                                             0: disable.
                                                             1: enable. */
        uint32_t rx_data_merge_en           : 1;        /*!< rx data merge enable.
                                                             0: disable.
                                                             1: enable. */
        uint32_t tx_fifo_disable            : 1;        /*!< tx fifo disable.
                                                             0: disable.
                                                             1: enable. */
        uint32_t rx_fifo_disable            : 1;        /*!< rx fifo disable.
                                                             0: disable.
                                                             1: enable. */
        uint32_t tx_enable                  : 1;        /*!< tx channel disable.
                                                             0: disable.
                                                             1: enable. */
        uint32_t rx_enable                  : 1;        /*!< rx channel disable.
                                                             0: disable.
                                                             1: enable. */
        uint32_t intr_en                    : 1;        /*!< Global interrupt enable.
                                                             0: disable.
                                                             1: enable. */
        uint32_t rst_n                      : 1;        /*!< SIO channel reset. */
        uint32_t reserved16_31              : 16;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_ct_set_data_t;

/**
 * @brief  This union represents the bit fields in the sio_ct_clr register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_ct_clr_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t reserved0_3                : 4;        /*!< Reserved. */
        uint32_t reserved4_7                : 4;        /*!< Reserved. */
        uint32_t tx_data_merge_en           : 1;        /*!< tx data merge enable.
                                                             0: disable.
                                                             1: enable. */
        uint32_t rx_data_merge_en           : 1;        /*!< rx data merge enable.
                                                             0: disable.
                                                             1: enable. */
        uint32_t tx_fifo_disable            : 1;        /*!< tx fifo disable.
                                                             0: disable.
                                                             1: enable. */
        uint32_t rx_fifo_disable            : 1;        /*!< rx fifo disable.
                                                             0: disable.
                                                             1: enable. */
        uint32_t tx_enable                  : 1;        /*!< tx channel disable.
                                                             0: disable.
                                                             1: enable. */
        uint32_t rx_enable                  : 1;        /*!< rx channel disable.
                                                             0: disable.
                                                             1: enable. */
        uint32_t intr_en                    : 1;        /*!< Global interrupt enable.
                                                             0: disable.
                                                             1: enable. */
        uint32_t rst_n                      : 1;        /*!< SIO channel reset. */
        uint32_t reserved16_31              : 16;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_ct_clr_data_t;

/**
 * @brief  This union represents the bit fields in the sio_fifo_threshold register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_fifo_threshold_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t tx_fifo_threshold          : 8;        /*!< Receive fifo threshold. */
        uint32_t rx_fifo_threshold          : 8;        /*!< Send fifo threshold. */
        uint32_t reserved16_31              : 16;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_fifo_threshold_data_t;

/**
 * @brief  This union represents the bit fields in the sio_rx_sta register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_rx_sta_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t rx_left_depth              : 8;        /*!< rx left depth. */
        uint32_t rx_right_depth             : 8;        /*!< rx right depth. */
        uint32_t reserved16_31              : 16;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_rx_sta_data_t;

/**
 * @brief  This union represents the bit fields in the sio_tx_sta register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_tx_sta_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t tx_left_depth              : 8;        /*!< tx left depth. */
        uint32_t tx_right_depth             : 8;        /*!< tx right depth. */
        uint32_t reserved16_31              : 16;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_tx_sta_data_t;

/**
 * @brief  This union represents the bit fields in the sio_data_width_set register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_data_width_set_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t tx_mode                    : 3;        /*!< tx data width set data. */
        uint32_t rx_mode                    : 3;        /*!< rx data width set data. */
        uint32_t reserved6_31               : 26;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_data_width_set_data_t;

/**
 * @brief  This union represents the bit fields in the sio_i2s_start_pos register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_i2s_start_pos_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t start_post_read            : 1;        /*!< This bit indicates which channel start read.
                                                             0: left channel.
                                                             1: right channel. */
        uint32_t start_pos_write            : 1;        /*!< This bit indicates which channel start write.
                                                             0: left channel.
                                                             1: right channel. */
        uint32_t reserved2_31               : 30;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_i2s_start_pos_data_t;

/**
 * @brief  This union represents the bit fields in the sio_pos_flag register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_pos_flag_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t start_post_read            : 1;        /*!< This bit indicates which channel start read.
                                                             0: left channel.
                                                             1: right channel. */
        uint32_t start_pos_write            : 1;        /*!< This bit indicates which channel start write.
                                                             0: left channel.
                                                             1: right channel. */
        uint32_t reserved2_31               : 30;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_pos_flag_data_t;

/**
 * @brief  This union represents the bit fields in the sio_signed_ext register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_signed_ext_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t signed_ext_en              : 1;        /*!< Upper bit data symbol extension enable.
                                                             0: disable.
                                                             1: enable. */
        uint32_t reserved1_31               : 31;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_signed_ext_data_t;

/**
 * @brief  This union represents the bit fields in the sio_i2s_pos_merge_en register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_i2s_pos_merge_en_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t merge_en                   : 1;        /*!< I2S pos merge enable.
                                                             0: disable.
                                                             1: enable. */
        uint32_t reserved1_31               : 31;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_i2s_pos_merge_en_data_t;

/**
 * @brief  This union represents the bit fields in the sio_intmask register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_intmask_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t rx_int                     : 1;        /*!< Mask the interrupt when rx fifo above threshold.
                                                             0: disable.
                                                             1: enable. */
        uint32_t tx_intr                    : 1;        /*!< Mask the interrupt when tx fifo above threshold.
                                                             0: disable.
                                                             1: enable. */
        uint32_t rx_right_fifo_over         : 1;        /*!< Mask the interrupt when right rx fifo overflow.
                                                             0: disable.
                                                             1: enable. */
        uint32_t rx_left_fifo_over          : 1;        /*!< Mask the interrupt when left rx fifo overflow.
                                                             0: disable.
                                                             1: enable. */
        uint32_t tx_right_fifo_under        : 1;        /*!< Mask the interrupt when right tx fifo underflow.
                                                             0: disable
                                                             1: enable. */
        uint32_t tx_left_fifo_under         : 1;        /*!< Mask the interrupt when left tx fifo underflow.
                                                             0: disable.
                                                             1: enable. */
        uint32_t reserved6_31               : 26;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_intmask_data_t;

/**
 * @brief  This union represents the bit fields in the cfg_i2s_crg register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_i2s_crg_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t i2s_bclk_div_en            : 1;        /*!< bclk enable signal. */
        uint32_t i2s_crg_clken              : 1;        /*!< bclk enable signal is invalid. */
        uint32_t i2s_bclk_sel               : 1;        /*!< bclk phase. */
        uint32_t reserved4                  : 1;        /*!< Reserved. */
        uint32_t i2s_fs_sel                 : 1;        /*!< bclk phase. */
        uint32_t reserved5_15               : 11;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_i2s_crg_data_t;

/**
 * @brief  This union represents the bit fields in the cfg_i2s_bclk_div_num register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_i2s_bclk_div_num_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t bclk_div_num               : 7;        /*!< Frequency division of the bclk frequency divider. */
        uint32_t reserved7_15               : 8;        /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_i2s_bclk_div_num_data_t;

/**
 * @brief  This union represents the bit fields in the cfg_i2s_fs_div_num register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_i2s_fs_div_num_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t fs_div_num                 : 10;       /*!< Frequency divisionof the fsclk frequency divider. */
        uint32_t reserved10_15               : 5;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_i2s_fs_div_num_data_t;

/**
 * @brief  This union represents the bit fields in the cfg_i2s_fs_div_num register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_i2s_fs_div_ratio_num_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t fs_div_ratio_num           : 11;       /*!< Frequency division ratio of the fsclk frequency divider. */
        uint32_t reserved11_15               : 4;       /*!< Reserved. */
    } b;                                                /*!< Register bits. */
} sio_v151_i2s_fs_div_ratio_num_data_t;

/**
 * @brief  This union represents the bit fields in the sio_i2s_dual_rx_chn register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_i2s_dual_rx_chn_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t rx_data                   : 32;        /*!< Receive data. */
    } b;                                                /*!< Register bits. */
} sio_v151_i2s_dual_rx_chn_data_t;

/**
 * @brief  This union represents the bit fields in the sio_i2s_dual_tx_chn register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sio_v151_i2s_dual_tx_chn_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t tx_data                  : 32;         /*!< Send data. */
    } b;                                                /*!< Register bits. */
} sio_v151_i2s_dual_tx_chn_data_t;

/**
 * @brief  Registers associated with Sio.
 */
typedef struct sio_v151_regs {
    volatile uint32_t version;                          /*!< sio_version. <i>Offset: 3Ch</i>. */
    volatile uint32_t mode;                             /*!< sio_mode. <i>Offset: 40h</i>. */
    volatile uint32_t intstatus;                        /*!< sio_intstatus. <i>Offset: 44h</i>. */
    volatile uint32_t intclr;                           /*!< sio_intclr. <i>Offset: 48h</i>. */
    volatile uint32_t left_tx;                          /*!< sio_left_tx. <i>Offset: 4Ch</i>. */
    volatile uint32_t right_tx;                         /*!< sio_right_tx. <i>Offset: 50h</i>. */
    volatile uint32_t left_rx;                          /*!< sio_left_rx. <i>Offset: 54h</i>. */
    volatile uint32_t right_rx;                         /*!< sio_right_rx. <i>Offset: 58h</i>. */
    volatile uint32_t ct_set;                           /*!< sio_ct_set. <i>Offset: 5Ch</i>. */
    volatile uint32_t ct_clr;                           /*!< sio_ct_clr. <i>Offset: 60h</i>. */
    volatile uint32_t fifo_threshold;                   /*!< sio_fifo_threshold. <i>Offset: 64h</i>. */
    volatile uint32_t rx_sta;                           /*!< sio_rx_sta. <i>Offset: 68h</i>. */
    volatile uint32_t tx_sta;                           /*!< sio_tx_sta. <i>Offset: 6Ch</i>. */
    volatile uint32_t reserved_reg[2];                  /*!< reserved_reg. <i>Offset: 70h</i>. */
    volatile uint32_t data_width_set;                   /*!< sio_data_width_set. <i>Offset: 78h</i>. */
    volatile uint32_t i2s_start_pos;                    /*!< sio_i2s_start_pos. <i>Offset: 7Ch</i>. */
    volatile uint32_t i2s_pos_flag;                     /*!< sio_i2s_pos_flag. <i>Offset:80h</i>. */
    volatile uint32_t signed_ext;                       /*!< sio_signed_ext. <i>Offset: 84h</i>. */
    volatile uint32_t i2s_pos_merge_en;                 /*!< sio_i2s_pos_merge_en. <i>Offset: 88h</i>. */
    volatile uint32_t intmask;                          /*!< sio_intmask. <i>Offset: 8Ch</i>. */
    volatile uint32_t i2s_crg;                          /*!< cfg_i2s_crg. <i>Offset: 90h</i>. */
    volatile uint32_t i2s_bclk_div_num;                 /*!< cfg_i2s_bclk_div_num. <i>Offset: 94h</i>. */
    volatile uint32_t i2s_fs_div_num;                   /*!< cfg_i2s_fs_div_num. <i>Offset: 98h</i>. */
    volatile uint32_t i2s_fs_div_ratio_num;             /*!< cfg_volatile . <i>Offset: 9Ch</i>. */
    volatile uint32_t i2s_dual_rx_chn;                  /*!< sio_i2s_dual_rx_chn. <i>Offset: A0h</i>. */
    volatile uint32_t reserved_reg1[7];                 /*!< sio_version. <i>Offset: A4h</i>. */
    volatile uint32_t i2s_dual_tx_chn;                  /*!< sio_i2s_dual_tx_chn. <i>Offset: C0h</i>. */
} sio_v151_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif