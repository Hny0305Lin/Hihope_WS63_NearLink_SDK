/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides flash MXIC Configuration information. \n
 *
 * History: \n
 * 2022-11-15， Create file. \n
 */
#ifndef FLASH_MXIC_CONFIG_H
#define FLASH_MXIC_CONFIG_H

/**
 * @defgroup drivers_driver_flash_mxic_config Flash Mxic Config
 * @ingroup  drivers_driver_flash
 * @{
 */

#include "flash_common_config.h"

/* Mxic enter qspi mode cmd config. */
/* MX25R64/MX25R32 mxic flash config. */
static flash_cmd_exe_t g_mxic_mx25rxx_enter_qspi_mode_cmd[] = {
    /* Write enable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { FLASH_WREN_CMD } },
    /* Write configuration register, enable QPI & enable High performance mode. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 4, { 0x01, 0x40, 0x00, 0x02 } },
    /* Wait until the configuration is complete. qspi mode 6 dummy cycle, max 80MHZ. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR1_CMD, 0, 0 } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END,        HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};

/* MX25U64/MX25U128/MX25U12843 mxic falsh config. */
static flash_cmd_exe_t g_mxic_mx25uxx_enter_qspi_mode_cmd[] = {
    /* Enable QPI mode, MX25U64 dummy cycle 6, max 84MHZ; MX25U128/MX25U12843, dummy cycle 6, max 104MHZ. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 1, { 0x35 } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END, HAL_SPI_FRAME_FORMAT_MAX_NUM, 0, { 0 } }
};

/* MX25U32 mxic falsh config. */
static flash_cmd_exe_t g_mxic_mx25u32_enter_qspi_mode_cmd[] = {
    /* Write enable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { FLASH_WREN_CMD } },
    /* Write configuration register, Enable QPI & enable High performance mode. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 2, { 0x01, 0x40 } },
    /* Wait until the configuration is complete. The dummy cycle default va 4, max 84MHZ. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR1_CMD, 0, 0 } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END,        HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};


/* Mxic enter xip mode config. */
/* MX25R64/MX25R32 mxic flash config. */
static flash_qspi_xip_config_t g_mxic_mx25rxx_config = {
    /* Enter XIP mode before config. */
    WAIT_CYCLES_4, HAL_SPI_INST_LEN_8, HAL_SPI_ADDR_LEN_32, HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q,
    /* Enter XIP mode after config. */
    WAIT_CYCLES_4, HAL_SPI_INST_LEN_0, HAL_SPI_ADDR_LEN_32, HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q,
    /* XIP 32bit addr & wrap config. */
    false,
    false,
};

/* MX25U64/MX25U128/MX25U12843 mxic falsh config. */
static flash_qspi_xip_config_t g_mxic_mx25uxx_config = {
    /* Enter XIP mode before config. */
    WAIT_CYCLES_4,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_32,
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
    /* Enter XIP mode after config. */
    WAIT_CYCLES_4,
    HAL_SPI_INST_LEN_0,
    HAL_SPI_ADDR_LEN_32,
    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,
    /* XIP 32bit addr & wrap config. */
    false,
    false,
};

/* MX25U32 mxic falsh config. */
static flash_qspi_xip_config_t g_mxic_mx25u32_config = {
    /* Enter XIP mode before config. */
    WAIT_CYCLES_6,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_24,
    HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q,
    /* Enter XIP mode after config. */
    WAIT_CYCLES_6,
    HAL_SPI_INST_LEN_8,
    HAL_SPI_ADDR_LEN_24,
    HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q,
    /* XIP 32bit addr & wrap config. */
    false,
    false,
};

/**
  * @}
  */
#endif