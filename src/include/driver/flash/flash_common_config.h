/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides flash common Configuration information. \n
 *
 * History: \n
 * 2022-11-15， Create file. \n
 */
#ifndef FLASH_COMMON_CONFIG_H
#define FLASH_COMMON_CONFIG_H

/**
 * @defgroup drivers_driver_flash_common_config Flash Common Config
 * @ingroup  drivers_driver_flash
 * @{
 */
#include "stdbool.h"
#include "spi.h"

#define WAIT_CYCLES_0  0x0
#define WAIT_CYCLES_1  0x1
#define WAIT_CYCLES_2  0x2
#define WAIT_CYCLES_3  0x3
#define WAIT_CYCLES_4  0x4
#define WAIT_CYCLES_5  0x5
#define WAIT_CYCLES_6  0x6
#define WAIT_CYCLES_7  0x7
#define WAIT_CYCLES_8  0x8
#define WAIT_CYCLES_9  0x9
#define WAIT_CYCLES_10 0x0A

/* Flash cmd define. */
#define FLASH_WREN_CMD  0x06
#define FLASH_WRDI_CMD  0x04
#define FLASH_RDID_CMD  0x9F
#define FLASH_RDSR1_CMD 0x05
#define FLASH_WRSR1_CMD 0x01
#define FLASH_RDSR2_CMD 0x35
#define FLASH_WRSR2_CMD 0x31
#define FLASH_RDSR3_CMD 0x15
#define FLASH_WRSR3_CMD 0x11
#define FLASH_SE_CMD    0x20
#define FLASH_BE_CMD    0xD8
#define FLASH_BE32K_CMD 0x52
#define FLASH_CE_CMD    0xC7
#define FLASH_DUMMY_CMD 0xC0
#define FLASH_PP_CMD    0x02
#define FLASH_4PP_CMD   0x38
#define FLASH_DP_CMD    0xB9
#define FLASH_RDP_CMD   0xAB
#define FLASH_RD_CMD    0x03
#define FLASH_FRD_CMD   0x0B
#define FLASH_QRD_CMD   0xEB
#define FLASH_RSTEN_CMD 0x66
#define FLASH_RST_CMD   0x99
#define FLASH_RDB_CMD   0xAB
/* Manufacturer ID + Device ID, Device id can track manufacturer info. */
#define FLASH_MSID_CMD  0x90
#define FLASH_UQID_CMD  0x4B

#define FLASH_DATA_ONE_BYTE 0
#define FLASH_DUMMY         0x0

#define FLASH_XIP_TRANS_2_BYTE 0x1
#define FLASH_XIP_TRANS_8_BYTE 0x7

#define FLASH_QSPI_TRANSMIT_DMA_ENABLE  1
#define FLASH_QSPI_TRANSMIT_DMA_DISABLE 2

#define FLASH_WRITE_MAX_TRANS_CNT 0x100
#define FLASH_CMD_LENGTH_4_BYTE 4
#define MANUFACTURE_ID_LENGTH 3
#define DEVICE_ID_LENGTH 2

/* Define flash manufacturer id. */
#define FLASH_MANUFACTURER_MXIC_MX25R80          0x1428C2
#define FLASH_MANUFACTURER_MXIC_MX25R32          0x1628C2
#define FLASH_MANUFACTURER_MXIC_MX25R64          0x1728C2
#define FLASH_MANUFACTURER_MXIC_MX25U64          0x3725C2
#define FLASH_MANUFACTURER_MXIC_MX25U128         0x3825C2
#define FLASH_MANUFACTURER_MXIC_MX25U12843       0xC22538
/* Flash ID is same, but the unique ID is different, and unique ID(0xE9) needs to add cmd at xip mode. */
#define FLASH_MANUFACTURER_WINBOND_W25Q32        0x1660EF
/* Flash ID is same, but the unique ID is different, and unique ID(0xEC) needs to add cmd at xip mode. */
#define FLASH_MANUFACTURER_WINBOND_W25Q64        0x1760EF
/* Flash ID is same, but the unique ID is different, and unique ID(NULL) needs to add cmd at xip mode. */
#define FLASH_MANUFACTURER_WINBOND_W25Q128       0x1860EF
#define FLASH_MANUFACTURER_WINBOND_W25Q128JW     0x1880EF
#define FLASH_MANUFACTURER_WINBOND_W25Q256       0x1960EF
#define FLASH_MANUFACTURER_MICRON_MT25QU128      0x18BB20
#define FLASH_MANUFACTURER_MICRON_MT25QU256      0x19BB20
/* Need to add cmd at xip mode, so it can enter sleep and exit sleep mode. */
#define FLASH_MANUFACTURER_GIGADEVICE_GD25WQ64   0x1765C8
/* Need to add cmd at xip mode, so it can enter sleep and exit sleep mode. */
#define FLASH_MANUFACTURER_GIGADEVICE_GD25LE64EX 0x1760C8    /* EX represents E3IG and ESIG. */
/* Need to add cmd at xip mode, so it can enter sleep and exit sleep mode */
#define FLASH_MANUFACTURER_GIGADEVICE_GD25LX128  0x1860C8    /* LX represents LB and LE. */
#define FLASH_MANUFACTURER_GIGADEVICE_GD25LQ256  0x1960C8
#define FLASH_MANUFACTURER_DOSILICON_FM25M64     0x1743F8
/* Need to add cmd at xip mode, so it can enter sleep and exit sleep mode. */
#define FLASH_MANUFACTURER_GIGADEVICE_GD25LQ32D  0x1660C8
/* Need to add cmd at xip mode, so it can enter sleep and exit sleep mode. */
#define FLASH_MANUFACTURER_DOSILICON_FM25M4AA    0x1842F8
/* Need to add cmd at xip mode, so it can enter sleep and exit sleep mode. */
#define FLASH_MANUFACTURER_PUYA_P25Q32LX         0x166085    /* LX represents L and LC. */
#define FLASH_MANUFACTURER_PUYA_P25Q64SL         0x176085
/* Need to add cmd at xip mode, so it can enter sleep and exit sleep mode. */
#define FLASH_MANUFACTURER_ADESTO_AT25SL128A     0x18421F
/* Need to add cmd at xip mode, so it can enter sleep and exit sleep mode. */
#define FLASH_MANUFACTURER_MXIC_MX25U32          0x3625C2
#define FLASH_MANUFACTURER_MXIC_MX25U256         0x3925C2
#define FLASH_MANUFACTURER_ELITE_EN25SX128A      0x18781C
/* Need to add cmd at xip mode, so it can enter sleep and exit sleep mode. */
#define FLASH_MANUFACTURER_DOSILICON_DS25M4AB    0x1842E5
#define FLASH_MANUFACTURER_ELITE_EN25S32A        0x16381C
#define FLASH_MANUFACTURER_XTX_XT25Q64D          0x17600B
#define FLASH_MANUFACTURER_XTX_XT25Q128D         0x18600B

#define FLASH_CMD_LEN_MAX     4

#define addr_unalign(Addr, Boundary) ((Addr) & ((Boundary) - 1))
#define addr_unalign_4b(Addr) addr_unalign((Addr), 4)

#define FLASH_SIZE_4MB  0x400000
#define FLASH_SIZE_8MB  0x800000
#define FLASH_SIZE_16MB 0x1000000
#define FLASH_SIZE_32MB 0x2000000

#define FLASH_PAGE_SIZE 4096

#define FLASH_BLOCK_32K_SIZE 0x8000
#define FLASH_BLOCK_64K_SIZE 0x10000

#define FLASH_W25Q64JWBYIQ_UNIQUE_ID 0xEC
#define FLASH_W25Q32JWBYIQ_UNIQUE_ID 0xE9
#define FLASH_W25Q128JWBYIQ_UNIQUE_ID 0xEB


static const uint32_t g_flash_need_cmd_unique_id[] = { FLASH_W25Q32JWBYIQ_UNIQUE_ID,
                                                       FLASH_W25Q64JWBYIQ_UNIQUE_ID,
                                                       FLASH_W25Q128JWBYIQ_UNIQUE_ID };

static const uint32_t g_winbond_flash_manufact_id[] = { FLASH_MANUFACTURER_WINBOND_W25Q32,
                                                        FLASH_MANUFACTURER_WINBOND_W25Q64,
                                                        FLASH_MANUFACTURER_WINBOND_W25Q128 };

/**
 * @if Eng
 * @brief  supported flash.
 * @else
 * @brief  支持的flash。
 * @endif
 */
typedef enum flash_support_manufacturer {
    FLASH_MXIC_MX25R32,
    FLASH_MXIC_MX25R64,
    FLASH_MXIC_MX25U64,
    FLASH_MXIC_MX25U128,
    FLASH_WINBOND_W25Q32,
    FLASH_WINBOND_W25Q64,
    FLASH_WINBOND_W25Q128,
    FLASH_WINBOND_W25Q128JW,
    FLASH_WINBOND_W25Q256,
    FLASH_MICRON_MT25QU128,
    FLASH_MICRON_MT25QU256,
    FLASH_GIGADEVICE_GD25WQ64,
    FLASH_GIGADEVICE_GD25LE64E,
    FLASH_GIGADEVICE_GD25LB128,
    FLASH_GIGADEVICE_GD25LQ256,
    FLASH_DOSILICON_FM25M64,
    FLASH_GIGADEVICE_GD25LQ32D,
    FLASH_DOSILICON_FM25M4AA,
    FLASH_PUYA_P25Q32LC,
    FLASH_ADESTO_AT25SL128A,
    FLASH_MXIC_MX25U32,
    FLASH_MXIC_MX25U12843,
    FLASH_ELITE_EN25SX128A,
    FLASH_DOSILICON_DS25M4AB,
    FLASH_ELITE_EN25S32A,
    FLASH_XTX_XT25Q64D,
    FLASH_XTX_XT25Q128D,
    FLASH_PUYA_P25Q64SL,
    FLASH_MXIC_MX25U256,
    FLASH_MXIC_MX25R80,
    FLASH_MANUFACTURER_MAX
} flash_support_manufacturer_t;

/**
 * @brief  Flash cmd type.
 */
typedef enum flash_cmd_type {
    FLASH_CMD_TYPE_CMD,
    FLASH_CMD_TYPE_PROCESSING,
    FLASH_CMD_TYPE_CHECK,
    FLASH_CMD_TYPE_END
} flash_cmd_type_t;

/**
 * @if Eng
 * @brief  Flash cmd.
 * @else
 * @brief  Flash 命令。
 * @endif
 */
typedef struct flash_cmd_exe {
    flash_cmd_type_t cmd_type;                              /*!< @if Eng Flash cmd type.
                                                                 @else Flash 命令模式。@endif */
    hal_spi_frame_format_t spi_frf_mode;                    /*!< @if Eng Flash spi trans mode.
                                                                 @else Flash使用的spi传输模式。@endif */
    uint8_t cmd_len;                                        /*!< @if Eng cmd length.
                                                                 @else 命令长度。@endif */
    uint8_t cmd[FLASH_CMD_LEN_MAX];                         /*!< @if Eng cmd.
                                                                 @else 命令数组。@endif */
} flash_cmd_exe_t;

/**
 * @if Eng
 * @brief  Flash qspi xip config.
 * @else
 * @brief  Flash进出xip配置。
 * @endif
 */
typedef struct flash_qspi_xip_config {
    uint32_t enter_xip_before_wait_cycles_config;           /*!< @if Eng Before enter xip mode wait cycle.
                                                                 @else 进xip模式前的等待周期。@endif */
    uint32_t enter_xip_before_inst_l;                       /*!< @if Eng Before enter xip mode inst len.
                                                                 @else 进xip模式前的指令长度。@endif */
    uint32_t enter_xip_before_addr_l;                       /*!< @if Eng Before enter xip mode addr len.
                                                                 @else 进xip模式前的地址长度。@endif */
    uint32_t enter_xip_before_trans_type;                   /*!< @if Eng Before enter xip mode trans type.
                                                                 @else 进xip模式前的传输方式。@endif */
    uint32_t enter_xip_after_wait_cycles_config;            /*!< @if Eng After enter xip mode wait cycle.
                                                                 @else 进xip模式后的等待周期。@endif */
    uint32_t enter_xip_after_inst_l;                        /*!< @if Eng After enter xip mode inst len.
                                                                 @else 进xip模式后的指令长度。@endif */
    uint32_t enter_xip_after_addr_l;                        /*!< @if Eng After enter xip mode addr len.
                                                                 @else 进xip模式后的地址长度。@endif */
    uint32_t enter_xip_after_trans_type;                    /*!< @if Eng After enter xip mode trans type.
                                                                 @else 进xip模式后的传输方式。@endif */
    bool enter_xip_after_enable_32bit_addr;                 /*!< @if Eng Whether to enable 32bit_addr.
                                                                 @else 进xip模式后是否需要使能32位地址。@endif */
    bool enter_xip_after_enable_wrap;                       /*!< @if Eng Whether to enable wrap.
                                                                 @else 进xip模式后是否需要使能数据包装。@endif */
} flash_qspi_xip_config_t;

/**
 * @if Eng
 * @brief  Flash qspi enable config.
 * @else
 * @brief  Flash  qspi使能配置。
 * @endif
 */
typedef struct flash_qe_config {
    uint32_t manufacturer_id;                           /*!< @if Eng Flash manufacturer id.
                                                             @else Flash 制造 id。@endif */
    flash_cmd_exe_t *flash_set_qe_cmd;                  /*!< @if Eng Flash set qspi enable cmd
                                                             @else Flash qspi 使能命令。@endif */
} flash_qe_config_t;

/**
 * @if Eng
 * @brief  The basic configuration for flash.
 * @else
 * @brief  Flash基础配置。
 * @endif
 */
typedef struct flash_cfg {
    uint32_t                            isinit;                     /*!< @if Eng Flash is init.
                                                                         @else Flash 是否初始化。@endif */
    uint32_t                            qspi_isinit;                /*!< @if Eng Flash qspi is init.
                                                                         @else Flash qspi是否初始化。@endif */
    flash_support_manufacturer_t        flash_manufacturer;         /*!< @if Eng Flash manufacturer id index.
                                                                         @else Flash id 索引。@endif */
    uint32_t                            unique_id;                  /*!< @if Eng Flash unique id.
                                                                         @else Flash 唯一 id。@endif */
    spi_bus_t                           bus;                        /*!< @if Eng Flash spi bus.
                                                                         @else Flash 使用的 spi bus。@endif */
    hal_spi_frame_format_t              mode;                       /*!< @if Eng Flash trans mode.
                                                                         @else Flash 传输模式。@endif */
    bool                                is_xip;                     /*!< @if Eng Flash is use xip.
                                                                         @else Flash 使用xip。@endif */
    bool                                need_cmd_at_xip_mode;       /*!< @if Eng Flash need cmd at xip mode.
                                                                         @else Flash 需要xip命令。@endif */
    spi_attr_t                          attr;                       /*!< @if Eng Flash spi attr.
                                                                         @else Flash spi 基础配置。@endif */
    spi_extra_attr_t                    extra_attr;                 /*!< @if Eng Flash spi extra attr.
                                                                         @else Flash spi 拓展配置。@endif */
} flash_cfg_t;

/**
  * @}
  */
#endif
