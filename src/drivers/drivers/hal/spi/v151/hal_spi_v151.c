/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V151 HAL spi \n
 *
 * History: \n
 * 2022-08-12, Create file. \n
 */
#include <stdbool.h>
#include "securec.h"
#include "common_def.h"
#include "spi_porting.h"
#include "hal_spi_v151_regs_op.h"
#include "hal_spi_v151.h"

#define SPI_MINUMUM_CLOCK_IN_MHZ    1
#define SPI_MAXIMUM_CLOCK_IN_MHZ    48
#define QSPI_MINUMUM_CLOCK_IN_MHZ   1
#define QSPI_MAXIMUM_CLOCK_IN_MHZ   96

#define SPI_MINUMUM_CLK_DIV         2
#define SPI_MAXIMUM_CLK_DIV         65534
#define spi_mhz_to_hz(x)            ((x) * 1000000)

#define SPI_FRAME_BYTES_1           0x01
#define SPI_FRAME_BYTES_4           0x04

#define QSPI_WAIT_CYCLE_MAX         0x1F

#define MAX_SPI_GET_RX_LEVEL_RETRY_TIMES         0xFFFF
#define SPI_RX_INT_WAIT_TIMES         0xF

#define hal_spi_trans_bytes_to_word(x) ((((uint32_t)(*(x))) << 24) + (((uint32_t)(*((x) + 1))) << 16) + \
                                        (((uint32_t)(*((x) + 2))) << 8) + ((uint32_t)(*((x) + 3))))
#define hal_spi_trans_word_31_24_bits_to_byte(x) ((uint8_t)(((x) & 0xff000000) >> 24))
#define hal_spi_trans_word_23_16_bits_to_byte(x) ((uint8_t)(((x) & 0xff0000) >> 16))
#define hal_spi_trans_word_15_8_bits_to_byte(x)  ((uint8_t)(((x) & 0xff00) >> 8))
#define hal_spi_trans_word_7_0_bits_to_byte(x)   ((uint8_t)((x) & 0xff))
#define hal_spi_frame_size_trans_to_frame_bytes(x)  (((x) + 1) >> 0x03)

static hal_spi_callback_t g_hal_spi_callback = NULL;
static hal_spi_attr_t g_hal_spi_attrs[SPI_BUS_MAX_NUM] = { 0 };
static hal_spi_extra_attr_t g_hal_spi_extra_attrs[SPI_BUS_MAX_NUM] = { 0 };
#if defined(CONFIG_SPI_SUPPORT_LPM)
#define SPI_BUS_CONFIG_REG_NUM   6
#define SPI_GET_MTRC             0
#define SPI_GET_TWL              1
#define SPI_GET_RWL              2
#define SPI_GET_SSEF             3
#define SPI_GET_WAITNUM          4
#define SPI_GET_NRDF             5
static uint32_t g_spi_suspend_regs[SPI_BUS_CONFIG_REG_NUM] = { 0 };
#endif

static bool hal_spi_check_timeout_by_count(uint32_t *trans_time, uint32_t timeout)
{
    (*trans_time)++;
    return ((*trans_time > timeout) ? true : false);
}

static bool hal_spi_is_attr_freq_valid(const uint32_t clk_in_mhz, const hal_spi_frame_format_t spi_frame_format)
{
    if (spi_frame_format == HAL_SPI_FRAME_FORMAT_STANDARD) {
        if ((clk_in_mhz < SPI_MINUMUM_CLOCK_IN_MHZ) || (clk_in_mhz > SPI_MAXIMUM_CLOCK_IN_MHZ)) {
            return false;
        }
    } else {
        if ((clk_in_mhz < QSPI_MINUMUM_CLOCK_IN_MHZ) || (clk_in_mhz > QSPI_MAXIMUM_CLOCK_IN_MHZ)) {
            return false;
        }
    }
    return true;
}

static bool hal_spi_is_attr_valid(spi_bus_t bus, const hal_spi_attr_t *attr, const hal_spi_extra_attr_t *extra_attr)
{
    hal_spi_frame_size_t frame_size = (hal_spi_frame_size_t)attr->frame_size;
    hal_spi_frame_format_t spi_frame_format = (hal_spi_frame_format_t)attr->spi_frame_format;
    hal_spi_trans_mode_t tmod = (hal_spi_trans_mode_t)attr->tmod;
    hal_spi_cfg_frame_format_t frame_format = (hal_spi_cfg_frame_format_t)attr->frame_format;
    hal_spi_cfg_clk_cpol_t polar = (hal_spi_cfg_clk_cpol_t)attr->clk_polarity;
    hal_spi_cfg_clk_cpha_t phase = (hal_spi_cfg_clk_cpha_t)attr->clk_phase;
    hal_spi_cfg_sste_t sste = (hal_spi_cfg_sste_t)attr->sste;

    if (frame_size != HAL_SPI_FRAME_SIZE_8 && frame_size != HAL_SPI_FRAME_SIZE_32) {
        return false;
    }
    if (spi_frame_format >= HAL_SPI_FRAME_FORMAT_MAX_NUM) {
        return false;
    }
    if (spi_frame_format == HAL_SPI_FRAME_FORMAT_QUAD && extra_attr == NULL) {
        return false;
    }
    if (frame_format >= SPI_CFG_FRAME_FORMAT_MAX) {
        return false;
    }
    if (tmod >= HAL_SPI_TRANS_MODE_MAX) {
        return false;
    }
    if (polar >= SPI_CFG_CLK_CPOL_MAX) {
        return false;
    }
    if (phase >= SPI_CFG_CLK_CPHA_MAX) {
        return false;
    }
    if (sste >= SPI_CFG_SSTE_MAX) {
        return false;
    }

    /* Only SPI master need to select slave and set clock. */
    if (attr->is_slave == 0) {
        if (attr->slave_num > spi_porting_max_slave_select_get(bus) + 1) {
            return false;
        }
        if (hal_spi_is_attr_freq_valid(attr->freq_mhz, spi_frame_format) != true) {
            return false;
        }
    }
    return true;
}

static hal_spi_frame_format_t hal_spi_get_frame_format(spi_bus_t bus)
{
    hal_spi_frame_format_t spi_frame_format = (hal_spi_frame_format_t)hal_spi_v151_spi_ctra_get_enhff(bus);
    if (spi_frame_format == HAL_SPI_FRAME_FORMAT_OCTAL) {
        spi_frame_format = g_hal_spi_attrs[bus].spi_frame_format;
    }
    return spi_frame_format;
}

static bool hal_spi_is_busy(spi_bus_t bus, uint32_t timeout)
{
    uint32_t trans_time = 0;
    while (hal_spi_v151_spi_wsr_get_tfe(bus) != 1 || hal_spi_v151_spi_wsr_get_rfne(bus) != 0 ||
           hal_spi_sr_get_busy(bus) != 0) {
        if (hal_spi_check_timeout_by_count(&trans_time, timeout)) {
            return true;
        }
    }
    return false;
}

static void hal_spi_config_qspi(spi_bus_t bus, const hal_spi_xfer_qspi_param_t *qspi_param)
{
    hal_spi_ssienr_set_ssi_en(bus, 0);
    hal_spi_v151_spi_enhctl_set_aaitf(bus, qspi_param->trans_type);
    hal_spi_v151_spi_enhctl_set_ilen(bus, qspi_param->inst_len);
    hal_spi_v151_spi_enhctl_set_addrlen(bus, qspi_param->addr_len);

    uint32_t wait_cyc = qspi_param->wait_cycles;
    if (wait_cyc > QSPI_WAIT_CYCLE_MAX) {
        wait_cyc = QSPI_WAIT_CYCLE_MAX;
    }
    hal_spi_v151_spi_enhctl_set_waitnum(bus, wait_cyc);
    hal_spi_ssienr_set_ssi_en(bus, 1);
}

#if defined(CONFIG_SPI_SUPPORT_SINGLE_SPI)
static void hal_spi_config_sspi(spi_bus_t bus, const hal_spi_xfer_sspi_param_t *sspi_param)
{
    spi_porting_set_sspi_mode(bus, true);
    hal_spi_v151_spi_rsvd_set(bus, sspi_param->wait_cycles);
}
#endif

static void hal_spi_init_config_spi_frame_format(spi_bus_t bus, hal_spi_frame_format_t spi_frame_format)
{
    if (spi_frame_format == HAL_SPI_FRAME_FORMAT_STANDARD ||
        spi_frame_format == HAL_SPI_FRAME_FORMAT_DUAL ||
        spi_frame_format == HAL_SPI_FRAME_FORMAT_QUAD) {
        hal_spi_v151_spi_ctra_set_enhff(bus, (uint32_t)spi_frame_format);
    } else {
        if (spi_frame_format == HAL_SPI_FRAME_FORMAT_OCTAL ||
            spi_frame_format == HAL_SPI_FRAME_FORMAT_DOUBLE_OCTAL||
            spi_frame_format == HAL_SPI_FRAME_FORMAT_SIXT) {
            hal_spi_v151_spi_ctra_set_enhff(bus, (uint32_t)HAL_SPI_FRAME_FORMAT_OCTAL);
        }
    }
}

static void hal_spi_init_config_freq(spi_bus_t bus, uint32_t bus_clk, uint32_t clk_in_mhz)
{
    uint32_t clk_div;
    clk_div = (uint32_t)(bus_clk / spi_mhz_to_hz(clk_in_mhz));
    if (clk_div < SPI_MINUMUM_CLK_DIV) {
        clk_div = SPI_MINUMUM_CLK_DIV;
    }

    hal_spi_baudr_set_sckdv(bus, clk_div);
}

static void hal_spi_init_config_master(spi_bus_t bus, const hal_spi_attr_t *attr,
                                       const hal_spi_extra_attr_t *extra_attr)
{
    hal_spi_ser_set_ser(bus, (attr->slave_num == 0) ? 0 : bit(attr->slave_num - 1));
    hal_spi_init_config_freq(bus, attr->bus_clk, attr->freq_mhz);
    if ((hal_spi_frame_format_t)attr->spi_frame_format == HAL_SPI_FRAME_FORMAT_QUAD) {
        hal_spi_config_qspi(bus, &extra_attr->qspi_param);
    }
#if defined(CONFIG_SPI_SUPPORT_SINGLE_SPI)
    if ((hal_spi_cfg_frame_format_t)attr->frame_format == SPI_CFG_FRAME_FORMAT_NS_MICROWIRE) {
        hal_spi_config_sspi(bus, &extra_attr->sspi_param);
    }
#endif
    /* Config the number of data frames. */
    if (attr->ndf > 0) {
        hal_spi_v151_spi_ctrb_set_nrdf(bus, attr->ndf - 1);
    } else {
        hal_spi_v151_spi_ctrb_set_nrdf(bus, 0);
    }
}

static bool hal_spi_init_config(spi_bus_t bus, const hal_spi_attr_t *attr, const hal_spi_extra_attr_t *extra_attr)
{
    if (hal_spi_is_attr_valid(bus, attr, extra_attr) != true) {
        return false;
    }

#if (CONFIG_SPI_NOT_SUPPORT_TEXAS_FORMAT)
    if (attr->frame_format == SPI_CFG_FRAME_FORMAT_TEXAS_SSP) {
        return false;
    }
#endif

    /* SPI parameter config */
    hal_spi_ssienr_set_ssi_en(bus, 0);
    hal_spi_v151_spi_ctra_set(bus, 0);
    hal_spi_v151_spi_enhctl_set(bus, 0);

    hal_spi_init_config_spi_frame_format(bus, (hal_spi_frame_format_t)attr->spi_frame_format);
    hal_spi_v151_spi_ctra_set_prs(bus, (hal_spi_cfg_frame_format_t)attr->frame_format);

    /* Config the clock phase and polarity. */
    hal_spi_v151_spi_ctra_set_scph(bus, attr->clk_phase);
    hal_spi_v151_spi_ctra_set_scpol(bus, attr->clk_polarity);

    /* Config the frame size. */
    hal_spi_v151_spi_ctra_set_dfs32(bus, attr->frame_size);
    hal_spi_v151_spi_ctra_set_cfs16(bus, attr->frame_size);
    /* Config the transfer mode and FIFO threshold. */
    hal_spi_v151_spi_ctra_set_trsm(bus, attr->tmod);
    if (attr->frame_format == SPI_CFG_FRAME_FORMAT_NS_MICROWIRE && attr->tmod == HAL_SPI_TRANS_MODE_TX) {
        hal_spi_v151_spi_mcr_set_mtrc(bus, 1);
    }
    hal_spi_txftlr_set_tft(bus, CONFIG_SPI_TXFTLR);
    hal_spi_rxftlr_set_rft(bus, CONFIG_SPI_RXFTLR);

    /* Config slave info and freq in master mode. */
    if (attr->is_slave == 0) {
        hal_spi_init_config_master(bus, attr, extra_attr);
    }

    /* Config slave select toggle enable. */
    hal_spi_v151_spi_ctra_set_ssn_te(bus, attr->sste);

    /* Enable the SPI and all of the interrupts. */
    hal_spi_ssienr_set_ssi_en(bus, 1);
    hal_spi_v151_int_set(bus, SPI_INMAR_REG, 0);

    (void)memcpy_s(&g_hal_spi_attrs[bus], sizeof(hal_spi_attr_t), attr, sizeof(hal_spi_attr_t));
    (void)memcpy_s(&g_hal_spi_extra_attrs[bus], sizeof(hal_spi_extra_attr_t), extra_attr, sizeof(hal_spi_extra_attr_t));

    return true;
}

__attribute__((weak)) void spi_porting_clock_init(uint32_t bus_clk)
{
    unused(bus_clk);
    return;
}

#pragma weak hal_spi_init = hal_spi_v151_init
errcode_t hal_spi_v151_init(spi_bus_t bus, const hal_spi_attr_t *attr,
                            const hal_spi_extra_attr_t *extra_attr, hal_spi_callback_t callback)
{
    if (hal_spi_regs_init() != 0) {
        return ERRCODE_SPI_REG_ADDR_INVALID;
    }

    spi_porting_clock_init(attr->bus_clk);

    if (attr->is_slave == 1) {
        spi_porting_set_device_mode(bus, SPI_MODE_SLAVE);
    } else {
        spi_porting_set_device_mode(bus, SPI_MODE_MASTER);
    }

    g_hal_spi_callback = callback;

    if (hal_spi_init_config(bus, attr, extra_attr) != true) {
        return ERRCODE_SPI_CONFIG_FAIL;
    }
    return ERRCODE_SUCC;
}

#pragma weak hal_spi_deinit = hal_spi_v151_deinit
errcode_t hal_spi_v151_deinit(spi_bus_t bus)
{
    g_hal_spi_callback = NULL;
    hal_spi_ssienr_set_ssi_en(bus, 0);
    return ERRCODE_SUCC;
}

static inline void hal_spi_pack_data_into_fifo(spi_bus_t bus, const uint8_t *data, uint32_t frame_bytes,
                                               hal_spi_frame_format_t spi_frame_format)
{
    if (frame_bytes == SPI_FRAME_BYTES_1) {
        hal_spi_dr_set_dr(bus, (uint8_t)(*data));
    } else {    /* SPI_FRAME_BYTES_4 */
        if (spi_frame_format == HAL_SPI_FRAME_FORMAT_STANDARD) {
            hal_spi_dr_set_dr(bus, hal_spi_trans_bytes_to_word(data));
        } else {    /* SPI_FRAME_FORMAT_QUAD */
            hal_spi_dr_set_dr(bus, *(uint32_t *)data);
        }
    }
}

#pragma weak hal_spi_write = hal_spi_v151_write
errcode_t hal_spi_v151_write(spi_bus_t bus, hal_spi_xfer_data_t *data, uint32_t timeout)
{
    uint8_t *tx_buff = data->tx_buff;
    uint32_t frame_bytes = hal_spi_frame_size_trans_to_frame_bytes(hal_spi_v151_spi_ctra_get_dfs32(bus));
    hal_spi_frame_format_t spi_frame_format = hal_spi_get_frame_format(bus);
    uint32_t trans_frames = data->tx_bytes / frame_bytes;
    uint32_t trans_time = 0;

    if (data->tx_bytes % frame_bytes != 0) {
        return ERRCODE_SPI_INVALID_BYTES;
    }

#if defined(CONFIG_SPI_SUPPORT_SINGLE_SPI)
    if (g_hal_spi_attrs[bus].frame_format == SPI_CFG_FRAME_FORMAT_NS_MICROWIRE) {
        hal_spi_ssienr_set_ssi_en(bus, 0);
        if (data->rx_bytes > 0 && data->rx_buff != NULL) {
            hal_spi_v151_spi_mcr_set_mtrc(bus, 0);
            hal_spi_v151_spi_ctrb_set_nrdf(bus, (uint32_t)(data->rx_bytes / frame_bytes) - 1);
        } else {
            hal_spi_v151_spi_mcr_set_mtrc(bus, 1);
        }
        hal_spi_ssienr_set_ssi_en(bus, 1);
    }
#endif

    if (unlikely(spi_porting_get_device_mode(bus) == SPI_MODE_MASTER &&
        spi_frame_format == HAL_SPI_FRAME_FORMAT_QUAD)) {
        /* qspi mode: send cmd and register addr */
        if (hal_spi_v151_spi_enhctl_get_ilen(bus) != 0) {
            hal_spi_dr_set_dr(bus, data->cmd);
        }
        if (hal_spi_v151_spi_enhctl_get_addrlen(bus) != 0) {
            hal_spi_dr_set_dr(bus, data->addr);
        }
    }

    if (tx_buff == NULL) {
        trans_frames = 0;
    }
    while (trans_frames > 0) {
        if (hal_spi_v151_spi_wsr_get_tfnf(bus) != 0) {
            hal_spi_pack_data_into_fifo(bus, tx_buff, frame_bytes, spi_frame_format);
            tx_buff += frame_bytes;
            trans_frames--;
        }

        if (hal_spi_check_timeout_by_count(&trans_time, timeout)) {
            return ERRCODE_SPI_TIMEOUT;
        }
    }

    return ERRCODE_SUCC;
}

static void hal_spi_pack_data_outof_fifo(spi_bus_t bus, uint8_t *data, uint32_t frame_bytes,
                                         hal_spi_frame_format_t spi_frame_format)
{
    if (frame_bytes == SPI_FRAME_BYTES_1) {
        *data = (uint8_t)hal_spi_dr_get_dr(bus);
    } else {    /* SPI_FRAME_BYTES_4 */
        if (spi_frame_format == HAL_SPI_FRAME_FORMAT_STANDARD) {
            uint32_t data_read = hal_spi_dr_get_dr(bus);
            (*(data + 0)) = hal_spi_trans_word_31_24_bits_to_byte(data_read);    /* 0: TRANS_WORD 31-24 */
            (*(data + 1)) = hal_spi_trans_word_23_16_bits_to_byte(data_read);    /* 1: TRANS_WORD 23-16 */
            (*(data + 2)) = hal_spi_trans_word_15_8_bits_to_byte(data_read);     /* 2: TRANS_WORD 15-8 */
            (*(data + 3)) = hal_spi_trans_word_7_0_bits_to_byte(data_read);      /* 3: TRANS_WORD 7-0 */
        } else {
            *(uint32_t *)data = hal_spi_dr_get_dr(bus);
        }
    }
}

static bool hal_spi_read_rx_mode_prepare(spi_bus_t bus)
{
    uint32_t wait_time = 0;
    if (spi_porting_get_device_mode(bus) == SPI_MODE_MASTER &&
        hal_spi_v151_spi_ctra_get_trsm(bus) == HAL_SPI_TRANS_MODE_RX) {
        hal_spi_dr_set_dr(bus, 0); /* 解决主机读场景时，先任意写一个数据拉低CS发出时钟信号。 */

        while (hal_spi_rxflr_get_rxtfl(bus) == 0) {
            if (hal_spi_check_timeout_by_count(&wait_time, MAX_SPI_GET_RX_LEVEL_RETRY_TIMES)) {
                return false;
            }
        }
    }

    return true;
}

#pragma weak hal_spi_read = hal_spi_v151_read
errcode_t hal_spi_v151_read(spi_bus_t bus, hal_spi_xfer_data_t *data, uint32_t timeout)
{
    uint8_t *rx_buff = data->rx_buff;
    uint32_t frame_bytes = hal_spi_frame_size_trans_to_frame_bytes(hal_spi_v151_spi_ctra_get_dfs32(bus));
    hal_spi_frame_format_t spi_frame_format = hal_spi_get_frame_format(bus);
    uint32_t trans_frames = data->rx_bytes / frame_bytes;
    uint32_t trans_time = 0;
    uint32_t timeout_tmp = (timeout == 0) ? SPI_RX_INT_WAIT_TIMES : timeout;

    if (data->rx_bytes % frame_bytes != 0) {
        return ERRCODE_SPI_INVALID_BYTES;
    }

    while (trans_frames > 0) {
        if (hal_spi_v151_spi_wsr_get_rffe(bus) == 1) {
            return ERRCODE_SPI_RX_FIFO_FULL;
        }

        if ((timeout != 0) && (!hal_spi_read_rx_mode_prepare(bus))) {
            return ERRCODE_SPI_TIMEOUT;
        }

        uint32_t rx_level = hal_spi_rxflr_get_rxtfl(bus);
        while (rx_level > 0) {
            if (timeout == 0) {
                trans_time = 0;
            }
            hal_spi_pack_data_outof_fifo(bus, rx_buff, frame_bytes, spi_frame_format);
            rx_buff += frame_bytes;
            trans_frames--;
            rx_level--;
            if (trans_frames == 0) {
                break;
            }
        }

        if (hal_spi_check_timeout_by_count(&trans_time, timeout_tmp)) {
            if (timeout == 0) {
                data->rx_bytes = trans_frames * frame_bytes;
                return ERRCODE_SUCC;
            }
            return ERRCODE_SPI_TIMEOUT;
        }
    }

    if (timeout == 0) {
        data->rx_bytes = trans_frames * frame_bytes;
    }
    return ERRCODE_SUCC;
}

static errcode_t hal_spi_ctrl_set_attr(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param)
{
    unused(id);

    hal_spi_attr_t *attr = (hal_spi_attr_t *)param;

    hal_spi_ssienr_set_ssi_en(bus, 0);
    hal_spi_init_config_spi_frame_format(bus, (hal_spi_frame_format_t)attr->spi_frame_format);

    /* Config the clock phase and polarity. */
    hal_spi_v151_spi_ctra_set_scph(bus, attr->clk_phase);
    hal_spi_v151_spi_ctra_set_scpol(bus, attr->clk_polarity);

    /* Config the frame size. */
    hal_spi_v151_spi_ctra_set_dfs32(bus, attr->frame_size);

    /* Config the transfer mode. */
    if (attr->frame_format == SPI_CFG_FRAME_FORMAT_NS_MICROWIRE) {
        if (attr->tmod == HAL_SPI_TRANS_MODE_TX) {
            hal_spi_v151_spi_mcr_set_mtrc(bus, 1);
        } else {
            hal_spi_v151_spi_mcr_set_mtrc(bus, 0);
        }
    }
    hal_spi_v151_spi_ctra_set_trsm(bus, attr->tmod);

    /* Config the number of data frames. */
    if (attr->ndf > 0) {
        hal_spi_v151_spi_ctrb_set_nrdf(bus, attr->ndf - 1);
    } else {
        hal_spi_v151_spi_ctrb_set_nrdf(bus, 0);
    }

    /* Config slave info and freq in master mode. */
    if (attr->is_slave == 0) {
        hal_spi_ser_set_ser(bus, (attr->slave_num == 0) ? 0 : bit(attr->slave_num - 1));
        hal_spi_init_config_freq(bus, attr->bus_clk, attr->freq_mhz);
    }

    /* Config slave select toggle enable. */
    hal_spi_v151_spi_ctra_set_ssn_te(bus, attr->sste);

    hal_spi_ssienr_set_ssi_en(bus, 1);

    (void)memcpy_s(&g_hal_spi_attrs[bus], sizeof(hal_spi_attr_t), attr, sizeof(hal_spi_attr_t));

    return ERRCODE_SUCC;
}

static errcode_t hal_spi_ctrl_get_attr(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param)
{
    unused(id);

    hal_spi_attr_t *attr = (hal_spi_attr_t *)param;

    (void)memcpy_s(attr, sizeof(hal_spi_attr_t), &g_hal_spi_attrs[bus], sizeof(hal_spi_attr_t));

    return ERRCODE_SUCC;
}

static errcode_t hal_spi_ctrl_set_extra_attr(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param)
{
    unused(id);

    hal_spi_extra_attr_t *extra_attr = (hal_spi_extra_attr_t *)param;

    if (hal_spi_get_frame_format(bus) != HAL_SPI_FRAME_FORMAT_QUAD) {
        return ERRCODE_SPI_MODE_MISMATCH;
    }

    hal_spi_config_qspi(bus, &extra_attr->qspi_param);

    (void)memcpy_s(&g_hal_spi_extra_attrs[bus], sizeof(hal_spi_extra_attr_t), extra_attr, sizeof(hal_spi_extra_attr_t));

    return ERRCODE_SUCC;
}

static errcode_t hal_spi_ctrl_get_extra_attr(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param)
{
    unused(id);

    hal_spi_extra_attr_t *extra_attr = (hal_spi_extra_attr_t *)param;

    (void)memcpy_s(extra_attr, sizeof(hal_spi_extra_attr_t), &g_hal_spi_extra_attrs[bus], sizeof(hal_spi_extra_attr_t));

    return ERRCODE_SUCC;
}

static errcode_t hal_spi_ctrl_select_slave(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param)
{
    unused(id);

    if (hal_spi_is_busy(bus, CONFIG_SPI_MAX_TIMEOUT)) {
        return ERRCODE_SPI_TIMEOUT;
    }
    uint32_t slave = (uint32_t)param;
    hal_spi_ser_set_ser(bus, slave);
    g_hal_spi_attrs[bus].slave_num = slave + 1;

    return ERRCODE_SUCC;
}

#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
static errcode_t hal_spi_ctrl_set_dma_cfg(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param)
{
    unused(id);

    hal_spi_dma_cfg_param_t *data = (hal_spi_dma_cfg_param_t *)param;
    hal_spi_v151_spi_dcr_set_tden(bus, (uint32_t)data->is_enable);
    hal_spi_dmacr_set_rdmae(bus, (uint32_t)data->is_enable);
    hal_spi_v151_spi_dtdl_data_set_dl(bus, data->dma_tx_level);
    hal_spi_dmardlr_set_dmardl(bus, data->dma_rx_level);

    return ERRCODE_SUCC;
}

static errcode_t hal_spi_ctrl_get_dma_data_addr(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param)
{
    unused(id);

    uint32_t *addr = (uint32_t *)param;
    *addr = (uint32_t)(uintptr_t)(&(spis_v151_regs(bus)->spi_drnm[0]));

    return ERRCODE_SUCC;
}
#endif  /* CONFIG_SPI_SUPPORT_DMA */

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
static errcode_t hal_spi_ctrl_en_rxfi_int(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param)
{
    unused(id);

    uint32_t en = (uint32_t)param;
    hal_spi_v151_spi_inmar_set_rffis(bus, en);
    return ERRCODE_SUCC;
}

static errcode_t hal_spi_ctrl_check_rx_fifo_empty(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param)
{
    unused(id);

    bool *rx_fifo_empty = (bool *)param;
    if (hal_spi_v151_spi_wsr_get_rfne(bus) == 0) {
        *rx_fifo_empty = true;
    } else {
        *rx_fifo_empty = false;
    }
    return ERRCODE_SUCC;
}

static errcode_t hal_spi_ctrl_en_txei_int(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param)
{
    unused(id);

    uint32_t en = (uint32_t)param;
    hal_spi_v151_spi_inmar_set_tfeis(bus, en);
    return ERRCODE_SUCC;
}

static errcode_t hal_spi_ctrl_check_tx_fifo_full(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param)
{
    unused(id);

    bool *tx_fifo_full = (bool *)param;
    if (hal_spi_v151_spi_wsr_get_tfnf(bus) == 0) {
        *tx_fifo_full = true;
    } else {
        *tx_fifo_full = false;
    }
    return ERRCODE_SUCC;
}
#endif  /* CONFIG_SPI_SUPPORT_SLAVE */

static errcode_t hal_spi_ctrl_check_fifo_busy(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    uint32_t timeout = (uint32_t)param;
    uint32_t trans_time = 0;

    while (hal_spi_v151_spi_wsr_get_tfe(bus) != 1 || hal_spi_sr_get_busy(bus) != 0) {
        if (hal_spi_check_timeout_by_count(&trans_time, timeout)) {
            return ERRCODE_SPI_TIMEOUT;
        }
    }

    return ERRCODE_SUCC;
}

#if defined(CONFIG_SPI_SUPPORT_LPM) && (CONFIG_SPI_SUPPORT_LPM == 1)
static errcode_t hal_spi_ctrl_suspend(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    unused(param);
    g_spi_suspend_regs[SPI_GET_MTRC] = hal_spi_v151_spi_mcr_get_mtrc(bus);
    g_spi_suspend_regs[SPI_GET_TWL] = hal_spi_v151_spi_twlr_get_twl(bus);
    g_spi_suspend_regs[SPI_GET_RWL] = hal_spi_v151_spi_rwlr_get_rwl(bus);
    if (g_hal_spi_attrs[bus].is_slave == 0) {
        g_spi_suspend_regs[SPI_GET_SSEF] = hal_spi_v151_spi_slenr_get_ssef(bus);
        if ((hal_spi_frame_format_t)g_hal_spi_attrs[bus].spi_frame_format == HAL_SPI_FRAME_FORMAT_QUAD) {
            g_spi_suspend_regs[SPI_GET_WAITNUM] = hal_spi_v151_spi_enhctl_get_waitnum(bus);
        }
        g_spi_suspend_regs[SPI_GET_NRDF] = hal_spi_v151_spi_ctrb_get_nrdf(bus);
    }
    return ERRCODE_SUCC;
}

static errcode_t hal_spi_ctrl_resume(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    unused(param);
    if (g_hal_spi_attrs[bus].is_slave == 1) {
        spi_porting_set_device_mode(bus, 1);
    } else {
        spi_porting_set_device_mode(bus, 0);
    }
    /* SPI parameter config */
    hal_spi_ssienr_set_ssi_en(bus, 0);
    hal_spi_v151_spi_ctra_set(bus, 0);
    hal_spi_v151_spi_enhctl_set(bus, 0);

    hal_spi_init_config_spi_frame_format(bus, (hal_spi_frame_format_t)g_hal_spi_attrs[bus].spi_frame_format);
    hal_spi_v151_spi_ctra_set_prs(bus, (hal_spi_cfg_frame_format_t)g_hal_spi_attrs[bus].frame_format);

    /* Config the clock phase and polarity. */
    hal_spi_v151_spi_ctra_set_scph(bus, g_hal_spi_attrs[bus].clk_phase);
    hal_spi_v151_spi_ctra_set_scpol(bus, g_hal_spi_attrs[bus].clk_polarity);

    /* Config the frame size. */
    hal_spi_v151_spi_ctra_set_dfs32(bus, g_hal_spi_attrs[bus].frame_size);
    hal_spi_v151_spi_ctra_set_cfs16(bus, g_hal_spi_attrs[bus].frame_size);
    /* Config the transfer mode and FIFO threshold. */
    hal_spi_v151_spi_ctra_set_trsm(bus, g_hal_spi_attrs[bus].tmod);
    hal_spi_v151_spi_mcr_set_mtrc(bus, g_spi_suspend_regs[SPI_GET_MTRC]);
    hal_spi_txftlr_set_tft(bus, g_spi_suspend_regs[SPI_GET_TWL]);
    hal_spi_rxftlr_set_rft(bus, g_spi_suspend_regs[SPI_GET_RWL]);

    /* Config slave info and freq in master mode. */
    if (g_hal_spi_attrs[bus].is_slave == 0) {
        hal_spi_ser_set_ser(bus, g_spi_suspend_regs[SPI_GET_SSEF]);
        hal_spi_init_config_freq(bus, g_hal_spi_attrs[bus].bus_clk, g_hal_spi_attrs[bus].freq_mhz);
        if ((hal_spi_frame_format_t)g_hal_spi_attrs[bus].spi_frame_format == HAL_SPI_FRAME_FORMAT_QUAD) {
            hal_spi_ssienr_set_ssi_en(bus, 0);
            hal_spi_v151_spi_enhctl_set_aaitf(bus, g_hal_spi_extra_attrs[bus].qspi_param.trans_type);
            hal_spi_v151_spi_enhctl_set_ilen(bus, g_hal_spi_extra_attrs[bus].qspi_param.inst_len);
            hal_spi_v151_spi_enhctl_set_addrlen(bus, g_hal_spi_extra_attrs[bus].qspi_param.addr_len);
            hal_spi_v151_spi_enhctl_set_waitnum(bus, g_spi_suspend_regs[SPI_GET_WAITNUM]);
            hal_spi_ssienr_set_ssi_en(bus, 1);
        }
#if defined(CONFIG_SPI_SUPPORT_SINGLE_SPI)
        if ((hal_spi_cfg_frame_format_t)g_hal_spi_attrs[bus].frame_format == SPI_CFG_FRAME_FORMAT_NS_MICROWIRE) {
            spi_porting_set_sspi_mode(bus, true);
            hal_spi_v151_spi_rsvd_set(bus, g_hal_spi_extra_attrs[bus].sspi_param.wait_cycles);
        }
#endif
        hal_spi_v151_spi_ctrb_set_nrdf(bus, g_spi_suspend_regs[SPI_GET_NRDF]);
    }

    /* Config slave select toggle enable. */
    hal_spi_v151_spi_ctra_set_ssn_te(bus, g_hal_spi_attrs[bus].sste);

    /* Enable the SPI and all of the interrupts. */
    hal_spi_ssienr_set_ssi_en(bus, 1);
    hal_spi_v151_int_set(bus, SPI_INMAR_REG, 0);
    return ERRCODE_SUCC;
}
#endif

static errcode_t hal_spi_ctrl_set_tmod(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param)
{
    unused(id);
#if defined(CONFIG_SPI_SUPPORT_SINGLE_SPI)
    if (spi_porting_is_sspi_mode(bus)) {
        return ERRCODE_SUCC;
    }
#endif /* SPI_SUPPORT_SINGLE_SPI */
    if (hal_spi_sr_get_busy(bus) == 1) {
        return ERRCODE_SPI_BUSY;
    }
    hal_spi_attr_t *attr = (hal_spi_attr_t *)param;
    hal_spi_ssienr_set_ssi_en(bus, 0);
    hal_spi_v151_spi_ctra_set_trsm(bus, attr->tmod);
    if (attr->tmod >= HAL_SPI_TRANS_MODE_RX) {
        /* 仅在RX Only和EEPROM模式下配置。 */
        hal_spi_v151_spi_ctrb_set_nrdf(bus, attr->ndf - 1);
    }
    hal_spi_ssienr_set_ssi_en(bus, 1);
    g_hal_spi_attrs[bus].tmod = attr->tmod;
    return ERRCODE_SUCC;
}

static const hal_spi_ctrl_t g_hal_spi_ctrl_func_array[SPI_CTRL_MAX] = {
    hal_spi_ctrl_set_attr,                   /* SPI_CTRL_SET_ATTR */
    hal_spi_ctrl_get_attr,                   /* SPI_CTRL_GET_ATTR */
    hal_spi_ctrl_set_extra_attr,             /* SPI_CTRL_SET_EXTRA_ATTR */
    hal_spi_ctrl_get_extra_attr,             /* SPI_CTRL_GET_EXTRA_ATTR */
    hal_spi_ctrl_select_slave,               /* SPI_CTRL_SELECT_SLAVE */
    hal_spi_ctrl_check_fifo_busy,            /* SPI_CTRL_CHECK_FIFO_BUSY */
#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    hal_spi_ctrl_set_dma_cfg,            /* SPI_CTRL_SET_DMA_CFG */
    hal_spi_ctrl_get_dma_data_addr,     /* SPI_CTRL_GET_DMA_DATA_ADDR */
#endif  /* CONFIG_SPI_SUPPORT_DMA */
#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
    hal_spi_ctrl_en_rxfi_int,                /* SPI_CTRL_EN_RXFI_INT */
    hal_spi_ctrl_check_rx_fifo_empty,        /* SPI_CTRL_CHECK_RX_FIFO_EMPTY */
    hal_spi_ctrl_en_txei_int,                /* SPI_CTRL_EN_TXEI_INT */
    hal_spi_ctrl_check_tx_fifo_full,         /* SPI_CTRL_CHECK_TX_FIFO_FULL */
    NULL,                                    /* SPI_CTRL_EN_MULTI_MASTER_ERR_INT */
#endif  /* CONFIG_SPI_SUPPORT_SLAVE */
#if defined(CONFIG_SPI_SUPPORT_LPM) && (CONFIG_SPI_SUPPORT_LPM == 1)
    hal_spi_ctrl_suspend,
    hal_spi_ctrl_resume,
#endif  /* CONFIG_SPI_SUPPORT_LPM */
    hal_spi_ctrl_set_tmod,                   /* SPI_CTRL_SET_TMOD */
};

#pragma weak hal_spi_ctrl = hal_spi_v151_ctrl
errcode_t hal_spi_v151_ctrl(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param)
{
    return g_hal_spi_ctrl_func_array[id](bus, id, param);
}

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
void hal_spi_v151_irq_handler(spi_bus_t bus)
{
    if (!g_hal_spi_callback) {
        return;
    }

    if (hal_spi_v151_int_get_rffis(bus, SPI_INSR_REG) == 1) {
        g_hal_spi_callback(bus, SPI_EVT_RX_FULL_ISR, 0);
    }
    if (hal_spi_v151_int_get_rfufis(bus, SPI_INSR_REG) == 1) {
        g_hal_spi_callback(bus, SPI_EVT_RX_UNDERFLOW_ISR, 0);
    }
    if (hal_spi_v151_int_get_rfofis(bus, SPI_INSR_REG) == 1) {
        g_hal_spi_callback(bus, SPI_EVT_RX_OVERFLOW_ISR, 0);
    }
    if (hal_spi_v151_int_get_tfeis(bus, SPI_INSR_REG) == 1) {
        g_hal_spi_callback(bus, SPI_EVT_TX_EMPTY_ISR, 0);
    }
    if (hal_spi_v151_int_get_tfofis(bus, SPI_INSR_REG) == 1) {
        g_hal_spi_callback(bus, SPI_EVT_TX_OVERFLOW_ISR, 0);
    }
    hal_spi_v151_icr_set_any(bus);
}
#endif  /* CONFIG_SPI_SUPPORT_SLAVE */