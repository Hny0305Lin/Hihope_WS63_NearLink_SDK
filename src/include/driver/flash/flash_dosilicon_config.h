/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides flash DOSILICON Configuration information. \n
 *
 * History: \n
 * 2022-11-15， Create file. \n
 */
#ifndef FLASH_DOSILICON_CONFIG_H
#define FLASH_DOSILICON_CONFIG_H

/**
 * @defgroup drivers_driver_flash_dosilicon_config Flash Dosilicon Config
 * @ingroup  drivers_driver_flash
 * @{
 */

#include "flash_common_config.h"

/* DOSILICON enter qspi mode cmd config. */
/* FM25M64/FM25M4AA  DOSILICON flash config. */
static flash_cmd_exe_t g_dosilicon_fm25m64_enter_qspi_mode_cmd[] = {
    /* Write enable for volatile status register. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x50 } },
    /* Write volatile configuration register, config XIP disable. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { 0x01, 0x00, 0x02 } },
    /* Wait config done. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR1_CMD, 0, 0 } },
    /* Enable QPI mode. Set the dummy cycle to 4, with the maximum read frequency being 80MHZ. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x38 } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END, HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};

/* DS25M4AB DOSILICON flash config. */
static flash_cmd_exe_t g_dosilicon_ds25m4ab_enter_qspi_mode_cmd[] = {
    /* Write enable for volatile status register. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD,  1, { 0x50 } },
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD,  3, { 0x01, 0x00, 0x02 } },
    /* Enable QPI mode. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD,  1, { 0x38 } },
    /* Set the dummy cycle to 6, with the maximum read frequency being 108MHZ. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_QUAD, 2, { 0xC0, 0x20 } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END, HAL_SPI_FRAME_FORMAT_MAX_NUM,    0, { 0 } }
};


/* DOSILICON enter xip mode config. */
/* FM25M64 DOSILICON flash config. */
static flash_qspi_xip_config_t g_dosilicon_fm25m64_config = {
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

/* FM25M4AA DOSILICON flash config. */
static flash_qspi_xip_config_t g_dosilicon_fm25m4aa_config = {
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

/* DS25M4AB DOSILICON flash config. */
static flash_qspi_xip_config_t g_dosilicon_ds25m4ab_config = {
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

/**
  * @}
  */
#endif