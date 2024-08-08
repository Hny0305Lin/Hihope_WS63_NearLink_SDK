/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides flash OTHERS Configuration information. \n
 *
 * History: \n
 * 2022-11-15， Create file. \n
 */
#ifndef FLASH_OTHERS_CONFIG_H
#define FLASH_OTHERS_CONFIG_H

/**
 * @defgroup drivers_driver_flash_other_config Flash Other Config
 * @ingroup  drivers_driver_flash
 * @{
 */

#include "flash_common_config.h"

/* OTHERS enter qspi mode cmd config. */
/* Micro config. */
/* MT25QU128 Micro flash config. */
static flash_cmd_exe_t g_micro_mt25qu128_enter_qspi_mode_cmd[] = {
    /* Write enable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { FLASH_WREN_CMD } },
    /* Write volatile configuration register, config dummy cycle 4, XIP disable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 2, { 0x81, 0x4B } },
    /* Wait until the configuration is complete. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR1_CMD, 0, 0 } },
    /* Write disable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { FLASH_WRDI_CMD } },
    /* Wait until the write enable flag is cleared. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR1_CMD, 1, 0 } },
    /* Enable QPI mode. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x35 } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END,        HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};
static flash_cmd_exe_t g_micro_mt25qu128_enter_xip_mode_cmd[] = {
    /* Write enable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_QUAD, 1, { FLASH_WREN_CMD } },
    /* Write volatile configuration register, config dummy cycle 4, wrap 32byte,XIP enable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_QUAD, 2, { 0x81, 0x41 } },
    /* Wait until the configuration is complete. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_QUAD, 3, { FLASH_RDSR1_CMD, 0, 0 } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END,        HAL_SPI_FRAME_FORMAT_MAX_NUM,    0, { 0 } }
};

/* MT25QU256 Micro flash config. */
static flash_cmd_exe_t g_micro_mt25qu256_enter_qspi_mode_cmd[] = {
    /* Write enable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { FLASH_WREN_CMD } },
    /* Write volatile configuration register, config dummy cycle 4, XIP disable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 2, { 0x81, 0x4B } },
    /* Write enable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { FLASH_WREN_CMD } },
    /* ENTER 4-BYTE ADDRESS MODE. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0xB7 } },
    /* Wait until the configuration is complete. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR1_CMD, 0, 0 } },
    /* Write disable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { FLASH_WRDI_CMD } },
    /* Wait until the write enable flag is cleared. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR1_CMD, 1, 0 } },
    /* Enable QPI mode. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x35 } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END,        HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};
static flash_cmd_exe_t g_micro_mt25qu256_enter_xip_mode_cmd[] = {
    /* Write enable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_QUAD, 1, { FLASH_WREN_CMD } },
    /* Write volatile configuration register, config dummy cycle 4, wrap 32byte,XIP enable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_QUAD, 2, { 0x81, 0x41 } },
    /* Wait until the configuration is complete. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_QUAD, 3, { FLASH_RDSR1_CMD, 0, 0 } },
    /* Write disable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_QUAD, 1, { FLASH_WRDI_CMD } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END,        HAL_SPI_FRAME_FORMAT_MAX_NUM,    0, { 0 } }
};

/* PUYA config. */
/* P25Q32LX PUYA flash config. */
static flash_cmd_exe_t g_puya_p25q32lc_enter_qspi_mode_cmd[] = {
    /* Write enable for volatile status register. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x50 } },
    /* Write status register-2 to 0x2, enable QPI write bit. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 2, { 0x31, 0x02 } },
    /* Wait until the configuration is complete. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR1_CMD, 0, 0 } },
    /* Enable QPI mode. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x38 } },
    /* Set the dummy cycle to 6, with the maximum read frequency being 104MHZ 70MHZ. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_QUAD, 2, { 0xC0,  0x20} },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END,        HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};

/* ADESTO config. */
/* AT25SL128A PUYA flash config. */
static flash_cmd_exe_t g_adesto_at25sl128a_enter_qspi_mode_cmd[] = {
    /* Write enable for volatile status register. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x50 } },
    /* Write volatile configuration register, config XIP disable. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { 0x01, 0x00, 0x02 } },
    /* Wait until the configuration is complete. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR1_CMD, 0, 0 } },
    /* Enable QPI mode. The dummy cycle default value is 4, with the maximum read frequency being 90MHZ. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x38 } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END, HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};

/* ELITE config. */
/* EN25SX128A PUYA flash config. */
static flash_cmd_exe_t g_mxic_en25sxxa_enter_qspi_mode_cmd[] = {
    /* Write enable for volatile status register. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x50 } },
    /* Write volatile configuration register, config XIP disable. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { 0x01, 0x00, 0x02 } },
    /* Enable QPI mode. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x38 } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END, HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};

/* XTX config. */
/* XT25Q128D flash config. */
static flash_cmd_exe_t g_xtx_xt25qxxd_enter_qspi_mode_cmd[] = {
    /* Write enable for volatile status register. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x50 } },
    /* Write status register-2 to 0x2, enable QPI write bit. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 2, { 0x31, 0x02 } },
    /* Wait until the configuration is complete. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR1_CMD, 0, 0 } },
    /* Enable QPI mode. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x38 } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END,        HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};

/* OTHERS enter xip mode config. */
/* Micro config. */
/* MT25QU128 Micro flash config. */
static flash_qspi_xip_config_t g_micro_mt25qu128_config = {
    /* Enter XIP mode before config. */
    WAIT_CYCLES_2,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_32,
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
    /* Enter XIP mode after config. */
    WAIT_CYCLES_2,
    HAL_SPI_INST_LEN_0,
    HAL_SPI_ADDR_LEN_32,
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
    /* XIP 32bit addr & wrap config. */
    false,
    true,
};

/* MT25QU256 Micro flash config. */
static flash_qspi_xip_config_t g_micro_mt25qu256_config = {
    /* Enter XIP mode before config. */
    WAIT_CYCLES_2,
    HAL_SPI_INST_LEN_0,
    HAL_SPI_ADDR_LEN_32,
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
    /* Enter XIP mode after config. */
    WAIT_CYCLES_2,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_32,
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
    /* XIP 32bit addr & wrap config. */
    true,
    true,
};

/* PUYA config. */
/* P25Q32LX PUYA flash config. */
static flash_qspi_xip_config_t g_puya_p25q32lc_config = {
    /* The config that before enter XIP mode. */
    WAIT_CYCLES_6,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_24,
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
    /* The config that after enter XIP mode. */
    WAIT_CYCLES_6,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_24,
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
    /* XIP 32bit addr & wrap config. */
    false,
    false,
};

/* ADESTO config. */
/* AT25SL128A PUYA flash config. */
static flash_qspi_xip_config_t g_adesto_at25sl128a_config = {
    /* The config that before enter XIP mode. */
    WAIT_CYCLES_4,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_24,
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
    /* The config that after enter XIP mode. */
    WAIT_CYCLES_4,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_24,
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
    /* XIP 32bit addr & wrap config. */
    false,
    false,
};

/* ELITE config. */
/* EN25SX128A PUYA flash config. */
static flash_qspi_xip_config_t g_mxic_en25sxxa_config = {
    /* The config that before enter XIP mode. */
    WAIT_CYCLES_6,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_24,
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
    /* The config that after enter XIP mode. */
    WAIT_CYCLES_6,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_24,
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
    /* XIP 32bit addr & wrap config. */
    false,
    false,
};

/* XTX config. */
/* XT25QXXD flash config. */
static flash_qspi_xip_config_t g_xtx_xt25qxxd_config = {
    /* The config that before enter XIP mode. */
    WAIT_CYCLES_6,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_32,
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
    /* The config that after enter XIP mode. */
    WAIT_CYCLES_6,
    HAL_SPI_INST_LEN_0,
    HAL_SPI_ADDR_LEN_32,
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
    /* XIP 32bit addr & wrap config. */
    false,
    false,
};
/**
  * @}
  */
#endif