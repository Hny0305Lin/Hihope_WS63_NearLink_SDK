/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides flash GIGADEVICE Configuration information. \n
 *
 * History: \n
 * 2022-11-15， Create file. \n
 */
#ifndef FLASH_GIGADEVICE_CONFIG_H
#define FLASH_GIGADEVICE_CONFIG_H

/**
 * @defgroup drivers_driver_flash_gigadevice_config Flash Gigadevice Config
 * @ingroup  drivers_driver_flash
 * @{
 */

#include "flash_common_config.h"

/* Gigadevice enter qspi mode cmd config. */
/* GD25WQ64/GD25LE64EX gigadevice flash config. */
static flash_cmd_exe_t g_gigadevice_gd25wq64_enter_qspi_mode_cmd[] = {
    /* Write enable for volatile status register. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x50 } },
    /* Write volatile configuration register, config XIP disable. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { 0x01, 0x00, 0x02 } },
    /* Wait until the configuration is complete. qspi mode, max 104MHZ. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR1_CMD, 0, 0 } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END, HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};

/* GD25LX128 gigadevice flash config. */
static flash_cmd_exe_t g_gigadevice_gd25lq128_enter_qspi_mode_cmd[] = {
    /* Write enable for volatile status register. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x50 } },
    /* Write volatile configuration register, config XIP disable. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { 0x01, 0x00, 0x02 } },
    /* Enable QPI mode. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x38 } },
    /* Set the dummy cycle to 6, with the maximum read frequency being 108MHZ. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_QUAD, 2, { 0xC0,  0x10} },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END, HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};

/* GD25LQ256 gigadevice flash config. */
static flash_cmd_exe_t g_gigadevice_gd25lq256_enter_qspi_mode_cmd[] = {
    /* Write enable for volatile status register. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x50 } },
    /* Write volatile configuration register, config XIP disable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 3, { 0x01, 0x00, 0x02 } },
    /* Write enable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { FLASH_WREN_CMD } },
    /* ENTER 4-BYTE ADDRESS MODE. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0xB7 } },
    /* Wait until the configuration is complete. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR1_CMD, 0, 0 } },
    /* Enable QPI mode. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x38 } },
    /* Set the dummy cycle to 6, with the maximum read frequency being 108MHZ. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_QUAD, 2, { 0xC0,  0x10} },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END,        HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};

/* GD25LQ32D gigadevice flash config. */
static flash_cmd_exe_t g_gigadevice_gd25lq32d_enter_qspi_mode_cmd[] = {
    /* Write enable for volatile status register. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x50 } },
    /* Write volatile configuration register, config XIP disable. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { 0x01, 0x00, 0x02 } },
    /* Enable QPI mode. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x38 } },
    /* Set the dummy cycle to 6, with the maximum read frequency being 108MHZ. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_QUAD, 2, { 0xC0,  0x20} },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END, HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};


/* Gigadevice enter xip mode config. */
/* GD25WQ64 gigadevice flash config. */
static flash_qspi_xip_config_t g_gigadevice_gd25wq64_config = {
    /* The config that before enter XIP mode. */
    WAIT_CYCLES_6,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_24,
    HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q,
    /* The config that after enter XIP mode. */
    WAIT_CYCLES_6,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_24,
    HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q,
    /* XIP 32bit addr & wrap config. */
    false,
    false,
};

/* GD25LE64EX gigadevice flash config. */
static flash_qspi_xip_config_t g_gigadevice_gd25le64_config = {
    /* The config that before enter XIP mode. */
    WAIT_CYCLES_6,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_24,
    HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q,
    /* The config that after enter XIP mode. */
    WAIT_CYCLES_6,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_24,
    HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q,
    /* XIP 32bit addr & wrap config. */
    false,
    false,
};

/* GD25LX128/GD25LQ32D gigadevice flash config. */
static flash_qspi_xip_config_t g_gigadevice_gd25lx_config = {
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

/* GD25LQ256 gigadevice flash config. */
static flash_qspi_xip_config_t g_gigadevice_gd25lq256_config = {
    /* The config that before enter XIP mode. */
    WAIT_CYCLES_4,
    HAL_SPI_INST_LEN_0,
    HAL_SPI_ADDR_LEN_32,
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
    /* The config that after enter XIP mode. */
    WAIT_CYCLES_4,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_32,
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
    /* XIP 32bit addr & wrap config. */
    true,
    false,
};

/**
  * @}
  */
#endif