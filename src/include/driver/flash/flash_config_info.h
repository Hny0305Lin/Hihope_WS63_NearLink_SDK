/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides flash common information. \n
 *
 * History: \n
 * 2022-11-15， Create file. \n
 */
#ifndef FLASH_INFORMATION_H
#define FLASH_INFORMATION_H

/**
 * @defgroup drivers_driver_flash_config_info Flash Config Info
 * @ingroup  drivers_driver_flash
 * @{
 */
#include "flash_mxic_config.h"
#include "flash_others_config.h"
#include "flash_winbond_config.h"
#include "flash_dosilicon_config.h"
#include "flash_gigadevice_config.h"

static const flash_cmd_exe_t g_w25q64_lock_image_region_cmd[] = {
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_QUAD, 1, { FLASH_WREN_CMD } },         /* Write enable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_QUAD, 2, { 0x01, 0x1C } },             /* Write status reg1. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_QUAD, 3, { FLASH_RDSR1_CMD, 0, 0 } },  /* Wait config done. */
    { FLASH_CMD_TYPE_END,        HAL_SPI_FRAME_FORMAT_MAX_NUM,    0, { 0 } }
};

static const flash_cmd_exe_t g_w25q64_unlock_image_region_cmd[] = {
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_QUAD, 1, { FLASH_WREN_CMD } },         /* Write enable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_QUAD, 2, { 0x01, 0x00 } },             /* Write status reg1. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_QUAD, 3, { FLASH_RDSR1_CMD, 0, 0 } },  /* Wait config done. */
    { FLASH_CMD_TYPE_END,        HAL_SPI_FRAME_FORMAT_MAX_NUM,    0, { 0 } }
};

/**
 * @if Eng
 * @brief  Flash adaptation table.
 * @else
 * @brief  flash 适配表。
 * @endif
 */
typedef struct flash_device_parameter {
    uint32_t manufacturer_id;                               /*!< @if Eng Flash manufacturer id.
                                                                 @else Flash 制造id。@endif */
    uint32_t flash_size;                                    /*!< @if Eng Flash size.
                                                                 @else Flash 大小。@endif */
    uint8_t enter_xip;                                      /*!< @if Eng Enter xip cmd.
                                                                 @else 进xip命令。@endif */
    uint8_t exit_xip;                                       /*!< @if Eng Exit xip cmd.
                                                                 @else 退xip命令。@endif */
    uint8_t read_dummy_clk;                                 /*!< @if Eng Read dummy cycle
                                                                 @else 读虚拟字节。@endif */
    uint8_t security_reg_cmd;                               /*!< @if Eng Security reg cmd.
                                                                 @else 安全寄存器命令。@endif */
    uint8_t suspend_cmd;                                    /*!< @if Eng Suspend cmd.
                                                                 @else 休眠命令。@endif */
    uint8_t resume_cmd;                                     /*!< @if Eng Resume cmd.
                                                                 @else 唤醒命令。@endif */
    flash_cmd_exe_t *enter_qspi_mode_cmd;                   /*!< @if Eng Enter qspi mode cmd.
                                                                 @else 进qspi模式命令。@endif */
    flash_cmd_exe_t *enter_xip_mode_cmd;                    /*!< @if Eng Enter xip mode wait cycle.
                                                                 @else 进xip模式命令。@endif */
    flash_qspi_xip_config_t *enter_xip_mode_config;         /*!< @if Eng Enter xip mode config.
                                                                 @else 进xip模式配置。@endif */
} flash_device_parameter_t;

static const flash_device_parameter_t g_flash_device_parameter[FLASH_MANUFACTURER_MAX] = {
    { FLASH_MANUFACTURER_MXIC_MX25R32,          FLASH_SIZE_4MB,  0xA5, 0xAA, WAIT_CYCLES_4, 0x2B, 0xB0, 0x30,
      g_mxic_mx25rxx_enter_qspi_mode_cmd,         NULL,                               &g_mxic_mx25rxx_config },
    { FLASH_MANUFACTURER_MXIC_MX25R64,          FLASH_SIZE_8MB,  0xA5, 0xAA, WAIT_CYCLES_4, 0x2B, 0xB0, 0x30,
      g_mxic_mx25rxx_enter_qspi_mode_cmd,         NULL,                               &g_mxic_mx25rxx_config },
    { FLASH_MANUFACTURER_MXIC_MX25U64,          FLASH_SIZE_8MB,  0xA5, 0xAA, WAIT_CYCLES_4, 0x2B, 0xB0, 0x30,
      g_mxic_mx25uxx_enter_qspi_mode_cmd,         NULL,                               &g_mxic_mx25uxx_config },
    { FLASH_MANUFACTURER_MXIC_MX25U128,         FLASH_SIZE_16MB, 0xA5, 0xAA, WAIT_CYCLES_4, 0x2B, 0xB0, 0x30,
      g_mxic_mx25uxx_enter_qspi_mode_cmd,         NULL,                               &g_mxic_mx25uxx_config },
    { FLASH_MANUFACTURER_WINBOND_W25Q32,        FLASH_SIZE_4MB,  0x20, 0xFF, WAIT_CYCLES_4, 0x05, 0x75, 0x7A,
      g_winbond_w25qxx_enter_qspi_mode_cmd,       NULL,                               &g_winbond_w25qxx_config },
    { FLASH_MANUFACTURER_WINBOND_W25Q64,        FLASH_SIZE_8MB,  0x20, 0xFF, WAIT_CYCLES_4, 0x35, 0x75, 0x7A,
      g_winbond_w25qxx_enter_qspi_mode_cmd,       NULL,                               &g_winbond_w25qxx_config },
    { FLASH_MANUFACTURER_WINBOND_W25Q128,       FLASH_SIZE_16MB, 0x20, 0xFF, WAIT_CYCLES_4, 0x05, 0x75, 0x7A,
      g_winbond_w25qxx_enter_qspi_mode_cmd,       NULL,                               &g_winbond_w25qxx_config },
    { FLASH_MANUFACTURER_WINBOND_W25Q128JW,     FLASH_SIZE_16MB, 0x20, 0xFF, WAIT_CYCLES_4, 0x05, 0x75, 0x7A,
      g_winbond_w25qxx_enter_qspi_mode_cmd,       NULL,                               &g_winbond_w25qxx_config },
    { FLASH_MANUFACTURER_WINBOND_W25Q256,       FLASH_SIZE_32MB, 0x20, 0xFF, WAIT_CYCLES_2, 0x05, 0x75, 0x7A,
      g_winbond_w25q256_enter_qspi_mode_cmd,      NULL,                               &g_winbond_w25q256_config },
    { FLASH_MANUFACTURER_MICRON_MT25QU128,      FLASH_SIZE_16MB, 0x0,  0x10, WAIT_CYCLES_2, 0x70, 0x75, 0x7A,
      g_micro_mt25qu128_enter_qspi_mode_cmd,      g_micro_mt25qu128_enter_xip_mode_cmd, &g_micro_mt25qu128_config },
    { FLASH_MANUFACTURER_MICRON_MT25QU256,      FLASH_SIZE_32MB, 0x0,  0x10, WAIT_CYCLES_4, 0x70, 0x75, 0x7A,
      g_micro_mt25qu256_enter_qspi_mode_cmd,      g_micro_mt25qu256_enter_xip_mode_cmd, &g_micro_mt25qu256_config },
    { FLASH_MANUFACTURER_GIGADEVICE_GD25WQ64,   FLASH_SIZE_8MB,  0x20, 0x0,  WAIT_CYCLES_4, 0x05, 0x75, 0x7A,
      g_gigadevice_gd25wq64_enter_qspi_mode_cmd,  NULL,                               &g_gigadevice_gd25wq64_config },
    { FLASH_MANUFACTURER_GIGADEVICE_GD25LE64EX, FLASH_SIZE_8MB,  0x20, 0x0,  WAIT_CYCLES_4, 0x05, 0x75, 0x7A,
      g_gigadevice_gd25wq64_enter_qspi_mode_cmd,  NULL,                               &g_gigadevice_gd25le64_config },
    { FLASH_MANUFACTURER_GIGADEVICE_GD25LX128,  FLASH_SIZE_16MB, 0x20, 0x0,  WAIT_CYCLES_4, 0x05, 0x75, 0x7A,
      g_gigadevice_gd25lq128_enter_qspi_mode_cmd, NULL,                               &g_gigadevice_gd25lx_config },
    { FLASH_MANUFACTURER_GIGADEVICE_GD25LQ256,  FLASH_SIZE_32MB, 0x20, 0x0,  WAIT_CYCLES_4, 0x05, 0x75, 0x7A,
      g_gigadevice_gd25lq256_enter_qspi_mode_cmd, NULL,                               &g_gigadevice_gd25lq256_config },
    { FLASH_MANUFACTURER_DOSILICON_FM25M64,     FLASH_SIZE_8MB,  0xA0, 0x00, WAIT_CYCLES_2, 0x35, 0x75, 0x7A,
      g_dosilicon_fm25m64_enter_qspi_mode_cmd,    NULL,                               &g_dosilicon_fm25m64_config },
    { FLASH_MANUFACTURER_GIGADEVICE_GD25LQ32D,  FLASH_SIZE_4MB,  0x20, 0x0,  WAIT_CYCLES_4, 0x05, 0x75, 0x7A,
      g_gigadevice_gd25lq32d_enter_qspi_mode_cmd, NULL,                               &g_gigadevice_gd25lx_config },
    { FLASH_MANUFACTURER_DOSILICON_FM25M4AA,    FLASH_SIZE_16MB, 0xA0, 0x00, WAIT_CYCLES_2, 0x35, 0x75, 0x7A,
      g_dosilicon_fm25m64_enter_qspi_mode_cmd,    NULL,                               &g_dosilicon_fm25m4aa_config },
    { FLASH_MANUFACTURER_PUYA_P25Q32LX,         FLASH_SIZE_32MB, 0x20, 0xFF, WAIT_CYCLES_4, 0x05, 0x75, 0x7A,
      g_puya_p25q32lc_enter_qspi_mode_cmd,        NULL,                               &g_puya_p25q32lc_config },
    { FLASH_MANUFACTURER_ADESTO_AT25SL128A,     FLASH_SIZE_16MB, 0xA0, 0x00, WAIT_CYCLES_2, 0x35, 0x75, 0x7A,
      g_adesto_at25sl128a_enter_qspi_mode_cmd,    NULL,                               &g_adesto_at25sl128a_config },
    { FLASH_MANUFACTURER_MXIC_MX25U32,          FLASH_SIZE_4MB,  0xA5, 0xAA, WAIT_CYCLES_4, 0x2B, 0xB0, 0x30,
      g_mxic_mx25u32_enter_qspi_mode_cmd,         NULL,                               &g_mxic_mx25u32_config },
    { FLASH_MANUFACTURER_MXIC_MX25U12843,       FLASH_SIZE_16MB, 0xA5, 0xAA, WAIT_CYCLES_4, 0x2B, 0xB0, 0x30,
      g_mxic_mx25uxx_enter_qspi_mode_cmd,         NULL,                               &g_mxic_mx25uxx_config },
    { FLASH_MANUFACTURER_ELITE_EN25SX128A,      FLASH_SIZE_16MB, 0x20, 0x0, WAIT_CYCLES_4,  0x05, 0x75, 0x7A,
      g_mxic_en25sxxa_enter_qspi_mode_cmd,        NULL,                               &g_mxic_en25sxxa_config },
    { FLASH_MANUFACTURER_DOSILICON_DS25M4AB,    FLASH_SIZE_16MB, 0x20, 0xFF, WAIT_CYCLES_4,  0x05, 0x75, 0x7A,
      g_dosilicon_ds25m4ab_enter_qspi_mode_cmd,   NULL,                               &g_dosilicon_ds25m4ab_config },
    { FLASH_MANUFACTURER_ELITE_EN25S32A,        FLASH_SIZE_4MB, 0x20, 0x0, WAIT_CYCLES_4,  0x05, 0x75, 0x7A,
      g_mxic_en25sxxa_enter_qspi_mode_cmd,        NULL,                               &g_mxic_en25sxxa_config },
    { FLASH_MANUFACTURER_XTX_XT25Q64D,         FLASH_SIZE_8MB, 0x20, 0x0, WAIT_CYCLES_6,  0x35, 0x75, 0x7A,
      g_xtx_xt25qxxd_enter_qspi_mode_cmd,         NULL,                               &g_xtx_xt25qxxd_config },
    { FLASH_MANUFACTURER_XTX_XT25Q128D,         FLASH_SIZE_16MB, 0x20, 0x0, WAIT_CYCLES_6,  0x35, 0x75, 0x7A,
      g_xtx_xt25qxxd_enter_qspi_mode_cmd,         NULL,                               &g_xtx_xt25qxxd_config },
    { FLASH_MANUFACTURER_PUYA_P25Q64SL,         FLASH_SIZE_8MB, 0x20, 0xFF, WAIT_CYCLES_4, 0x05, 0x75, 0x7A,
      g_puya_p25q32lc_enter_qspi_mode_cmd,        NULL,                               &g_puya_p25q32lc_config },
    { FLASH_MANUFACTURER_MXIC_MX25U256,         FLASH_SIZE_32MB,  0xA5, 0xAA, WAIT_CYCLES_4, 0x2B, 0xB0, 0x30,
      g_mxic_mx25uxx_enter_qspi_mode_cmd,         NULL,                               &g_mxic_mx25uxx_config },
    { FLASH_MANUFACTURER_MXIC_MX25R80,          FLASH_SIZE_8MB,  0xA5, 0xAA, WAIT_CYCLES_4, 0x2B, 0xB0, 0x30,
      g_mxic_mx25rxx_enter_qspi_mode_cmd,         NULL,                               &g_mxic_mx25rxx_config },
};

static const uint32_t g_xip_use_cmd_mode[] = {
    FLASH_MANUFACTURER_GIGADEVICE_GD25LX128,
    FLASH_MANUFACTURER_GIGADEVICE_GD25LQ32D,
    FLASH_MANUFACTURER_GIGADEVICE_GD25LE64EX,
    FLASH_MANUFACTURER_GIGADEVICE_GD25WQ64,
    FLASH_MANUFACTURER_DOSILICON_FM25M4AA,
    FLASH_MANUFACTURER_PUYA_P25Q32LX,
    FLASH_MANUFACTURER_ADESTO_AT25SL128A,
    FLASH_MANUFACTURER_MXIC_MX25U32,
    FLASH_MANUFACTURER_ELITE_EN25SX128A,
    FLASH_MANUFACTURER_DOSILICON_FM25M64,
    FLASH_MANUFACTURER_ELITE_EN25S32A,
    FLASH_MANUFACTURER_DOSILICON_DS25M4AB,
    FLASH_MANUFACTURER_PUYA_P25Q64SL,
};

static flash_cmd_exe_t g_winbond_w25qxx_set_qe_cmd[] = {
    /* Check QE bit. */
    { FLASH_CMD_TYPE_CHECK,      HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR2_CMD, 0x1, 0x2 } },
    /* Write enable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { FLASH_WREN_CMD } },
    /* Write status register-2 to 0x2, set QE bit. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 2, { 0x31, 0x02 } },
    /* Wait until the configuration is complete. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR1_CMD, 0, 0 } },
    /* Check QE bit. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR2_CMD, 0x1, 0x2 } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END,        HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};

static flash_cmd_exe_t g_dosilicon_fm25m64_set_qe_cmd[] = {
    /* Check QE bit. */
    { FLASH_CMD_TYPE_CHECK,      HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR2_CMD, 0x1, 0x2 } },
    /* Write enable. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 1, { FLASH_WREN_CMD } },
    /* Write status register-2 to 0x2, set QE bit. */
    { FLASH_CMD_TYPE_CMD, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { 0x01, 0x00, 0x02 } },
    /* Wait until the configuration is complete. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR1_CMD, 0, 0 } },
    /* Check QE bit. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR2_CMD, 0x1, 0x2 } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END, HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};

static flash_cmd_exe_t g_winbond_en25sx128a_set_qe_cmd[] = {
    /* Check QE bit. */
    { FLASH_CMD_TYPE_CHECK,      HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR2_CMD, 0x1, 0x2 } },
    /* Write enable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { FLASH_WREN_CMD } },
    /* Write status register-2 to 0x2, set QE bit. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 2, { 0x31, 0x02 } },
    /* Wait until the configuration is complete. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR1_CMD, 0, 0 } },
    /* Check QE bit. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR2_CMD, 0x1, 0x2 } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END,        HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};

static flash_cmd_exe_t g_xtx_xt25q128d_set_qe_cmd[] = {
    /* Check QE bit. */
    { FLASH_CMD_TYPE_CHECK,      HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR2_CMD, 0x1, 0x2 } },
    /* Write enable. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 1, { FLASH_WREN_CMD } },
    /* Write status register-2 to 0x2, set QE bit. */
    { FLASH_CMD_TYPE_CMD,        HAL_SPI_FRAME_FORMAT_STANDARD, 2, { 0x31, 0x02 } },
    /* Wait until the configuration is complete. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR1_CMD, 0, 0 } },
    /* Check QE bit. */
    { FLASH_CMD_TYPE_PROCESSING, HAL_SPI_FRAME_FORMAT_STANDARD, 3, { FLASH_RDSR2_CMD, 0x1, 0x2 } },
    /* The END command, will not be executed and need return. */
    { FLASH_CMD_TYPE_END,        HAL_SPI_FRAME_FORMAT_MAX_NUM,   0, { 0 } }
};

static const flash_qe_config_t g_flash_need_set_qe[] = {
    {FLASH_MANUFACTURER_WINBOND_W25Q32,      g_winbond_w25qxx_set_qe_cmd},
    {FLASH_MANUFACTURER_WINBOND_W25Q64,      g_winbond_w25qxx_set_qe_cmd},
    {FLASH_MANUFACTURER_WINBOND_W25Q128,     g_winbond_w25qxx_set_qe_cmd},
    {FLASH_MANUFACTURER_WINBOND_W25Q128JW,   g_winbond_w25qxx_set_qe_cmd},
    {FLASH_MANUFACTURER_DOSILICON_FM25M64,   g_dosilicon_fm25m64_set_qe_cmd},
    {FLASH_MANUFACTURER_DOSILICON_FM25M4AA,  g_dosilicon_fm25m64_set_qe_cmd},
    {FLASH_MANUFACTURER_DOSILICON_DS25M4AB,  g_winbond_w25qxx_set_qe_cmd},
    {FLASH_MANUFACTURER_ELITE_EN25SX128A,    g_winbond_en25sx128a_set_qe_cmd},
    {FLASH_MANUFACTURER_XTX_XT25Q128D,       g_xtx_xt25q128d_set_qe_cmd},
};

#define FLASH_WIP                (bit(0))
#define FLASH_WEL                (bit(1))
#define FLASH_MXIC_SUSPEND       (bit(2) | bit(3))
#define FLASH_MICRON_SUSPEND     (bit(7))
#define FLASH_WINBOND_SUSPEND    (bit(7))
#define FLASH_GIGADEVICE_SUSPEND (bit(0))
#define FLASH_DOSILICON_SUSPEND  (bit(7))

/**
  * @}
  */
#endif