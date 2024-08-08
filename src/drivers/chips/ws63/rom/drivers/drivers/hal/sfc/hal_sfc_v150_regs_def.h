/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides v150 SFC register \n
 *
 * History: \n
 * 2022-12-01, Create file. \n
 */
#ifndef HAL_SFC_V150_REGS_DEF_H
#define HAL_SFC_V150_REGS_DEF_H

#include <stdint.h>

#define MAX_DATABUF_NUM                   16
#define MAX_BYTE_PER_DATABUF              4

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_sfc_v150_regs_def SFC v150 Regs Definition
 * @ingroup  drivers_hal_sfc
 * @{
 */

/**
 * @brief  This union represents the bit fields in the SFC global config Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union global_config {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t mode                    : 1;    /*!< SPI Mode. \n
                                                      Values:
                                                        - 0x0 SPI Mode0.
                                                        - 0x1 SPI Mode3. */
        uint32_t wp_en                   : 1;    /*!< Hardware write protection enable. \n
                                                      Values:
                                                        - 0x0 disabled.
                                                        - 0x1 enabled. */
        uint32_t flash_addr_mode         : 1;    /*!< SPI address mode. \n
                                                      Values:
                                                        - 0x0 3-byte addressing mode (default value)
                                                        - 0x1 4-byte addressing mode. */
        uint32_t rd_delay                : 3;    /*!< Number of delay cycles for reading data. \n
                                                      Values:
                                                        - 000: 0.5–1 clock cycle (default value)
                                                        - 001: 1–1.5 clock cycles;
                                                        - 010: 1.5–2 clock cycles;
                                                        - 011: 2–2.5 clock cycles;
                                                        - 100: 2.5–3 clock cycles;
                                                        - 101: 3–3.5 clock cycles;
                                                        - 110: 3.5–4 clock cycles;
                                                        - 111: not supported (processed according to "110"). */
        uint32_t sample_test             : 4;    /*!< In DDR mode, the sampling point is selected based on the delay
                                                         parameters of different components. */
        uint32_t reserved                : 22;
    } b;                                         /*!< Register bits. */
} global_config_t;

/**
 * @brief  This union represents the bit fields in the SFC timing Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union timing {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t tshsl                   : 4;    /*!< Sets the interval between two flash operations.
                                                         Time interval = tshsl + 2. */
        uint32_t reserved0               : 4;
        uint32_t tcss                    : 3;    /*!<  CS setup time.
                                                         Time = tcss + 1. */
        uint32_t reserved1               : 1;
        uint32_t tcsh                    : 3;    /*!<  CS hold time.
                                                         Time = tcss + 1. */
    } b;                                         /*!< Register bits. */
} timing_t;

/**
 * @brief  This union represents the bit fields in the SFC bus config Register 1. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union bus_config1 {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t rd_mem_if_type          : 3;    /*!< SPI interface type of bus read
                                                         value:
                                                            000：Standard SPI;
                                                            001：Dual-Input/Dual-Output SPI(based on read/write);
                                                            010：Dual-I/O SPI;
                                                            101：Quad-Input/Dual-Output SPI(based on read/write);
                                                            110：Quad-I/O SPI;
                                                            other: reserved. */
        uint32_t rd_dummy_bytes          : 3;    /*!< Number of dummy bytes in the read operation. */
        uint32_t rd_prefetch_cnt         : 2;    /*!< Prefetch cycle when the bus accesses the flash memory. */
        uint32_t rd_ins                  : 8;    /*!< Read command of bus mode. */
        uint32_t wr_mem_if_type          : 3;    /*!< SPI interface type of bus read.
                                                         value:
                                                            000：Standard SPI;
                                                            001：Dual-Input/Dual-Output SPI(based on read/write);
                                                            010：Dual-I/O SPI;
                                                            101：Quad-Input/Dual-Output SPI(based on read/write);
                                                            110：Quad-I/O SPI;
                                                            other: reserved. */
        uint32_t wr_dummy_bytes          : 3;    /*!< Number of dummy bytes in the write operation. */
        uint32_t wr_ins                  : 8;    /*!< Write command of bus mode. */
        uint32_t wr_enable               : 1;    /*!< Bus mode write enable. */
        uint32_t rd_enable               : 1;    /*!< Bus mode read enable. */
    } b;                                         /*!< Register bits. */
} bus_config1_t;

/**
 * @brief  This union represents the bit fields in the SFC bus config Register 2. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union bus_config2 {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t wip_locate              : 3;    /*!< Write In Progress is located in the flash status register. */
        uint32_t bus_ddr_mode            : 1;    /*!< DDR mode enable. */
        uint32_t ddr_rd_dummy_bytes      : 4;    /*!< Number of dummy bytes in the read operation in DDR mode. */
        uint32_t ddr_wr_dummy_bytes      : 4;    /*!< Number of dummy bytes in the write operation in DDR mode. */
        uint32_t sfc_ddr_tx_delay_enable : 1;    /*!< Phase relationship between CLK and TX_DATA in DDR mode:
                                                      value :0: align;
                                                             1: CLK is 1/4 SFC_CLK cycle ahead of TX_DATA. */
        uint32_t reserved                : 19;
    } b;                                         /*!< Register bits. */
} bus_config2_t;

/**
 * @brief  This union represents the bit fields in the SFC bus flash size Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union bus_flash_size {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t flash_size_cs0          : 4;    /*!< Capacity of the SPI flash connected to CS0.
                                                      value:
                                                        0x0 No flash;
                                                        0x1 64KB;
                                                        0x2 128KB;
                                                        0x3 256KB;
                                                        0x4 512KB;
                                                        0x5 1MB;
                                                        0x6 2MB;
                                                        0x7 4MB;
                                                        0x8 8MB;
                                                        0x9 16MB(default);
                                                        0xA 32MB;
                                                        0xB 64MB;
                                                        0xC 128MB;
                                                        0xD 256MB;
                                                        0xE 512MB;
                                                        0xF 1GB. */
        uint32_t reserved0               : 4;
        uint32_t flash_size_cs1          : 4;    /*!< Capacity of the SPI flash connected to CS0.
                                                      The value is the same as that of flash_size_cs0. */
        uint32_t reserved1               : 20;
    } b;                                         /*!< Register bits. */
} bus_flash_size_t;

/**
 * @brief  This union represents the bit fields in the SFC bus base addr Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union bus_base_addr {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t reserved                : 16;
        uint32_t bus_base_addr_high_cs   : 16;   /*!< Base address of the flash to be mapped to the system space. */
    } b;                                         /*!< Register bits. */
} bus_base_addr_t;

/**
 * @brief  This union represents the bit fields in the SFC bus dma ctrl Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union bus_dma_ctrl {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t dma_start               : 1;    /*!< DMA transfer enable control.
                                                      Automatic return to 0 after the DMA transfer is complete. */
        uint32_t dma_rw                  : 1;    /*!< DMA read/write indicator. 0: write; 1: read. */
        uint32_t reserved0               : 2;
        uint32_t dma_sel_cs              : 1;    /*!< CS specified by the DMA operation. */
        uint32_t reserved1               : 27;
    } b;                                         /*!< Register bits. */
} bus_dma_ctrl_t;

/**
 * @brief  This union represents the bit fields in the SFC bus dma len Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union bus_dma_len {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t dma_len                 : 30;   /*!< Data transfer length of the DMA operation.
                                                      Data length = (dma_len + 1) Byte. */
        uint32_t reserved                : 2;
    } b;                                         /*!< Register bits. */
} bus_dma_len_t;

/**
 * @brief  This union represents the bit fields in the SFC bus dma ahb ctrl Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union bus_dma_ahb_ctrl {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t incr4_en                : 1;    /*!< INC4 burst type enable. */
        uint32_t incr8_en                : 1;    /*!< INC8 burst type enable. */
        uint32_t incr16_en               : 1;    /*!< INC16 burst type enable. */
        uint32_t reserved                : 29;
    } b;                                         /*!< Register bits. */
} bus_dma_ahb_ctrl_t;

/**
 * @brief  This union represents the bit fields in the SFC command config Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cmd_config {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t start                   : 1;    /*!< SPI transfer enable control.
                                                      Automatic return to 0 after the SPI transfer is complete. */
        uint32_t sel_cs                  : 1;    /*!< CS select operation. */
        uint32_t reserved0               : 1;
        uint32_t addr_en                 : 1;    /*!< Indicates whether an address is available for this operation. */
        uint32_t dummy_byte_cnt          : 3;    /*!< Number of dummy bytes in the operation. */
        uint32_t data_en                 : 1;    /*!< Indicates whether data is available for this operation. */
        uint32_t rw                      : 1;    /*!< SPI read/write indicator. 0: write; 1: read. */
        uint32_t data_cnt                : 6;    /*!< Data transfer length of the SPI operation.
                                                      Data length = (data_cnt + 1) Byte. */
        uint32_t reserved                : 2;
        uint32_t mem_if_type             : 3;    /*!< SPI interface type.
                                                         value:
                                                            000：Standard SPI;
                                                            001：Dual-Input/Dual-Output SPI(based on read/write);
                                                            010：Dual-I/O SPI;
                                                            101：Quad-Input/Dual-Output SPI(based on read/write);
                                                            110：Quad-I/O SPI;
                                                            other: reserved. */
        uint32_t reserved1               : 12;
    } b;                                         /*!< Register bits. */
} cmd_config_t;

/**
 * @brief  This union represents the bit fields in the SFC command ins Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cmd_ins {
    uint32_t d32;                                /*!< Register bits. */
    struct {
        uint32_t reg_ins                 : 8;    /*!< SPI command. */
        uint32_t cmd_ddr_mode            : 1;    /*!< Using DDR mode when the register accesses the flash memory. */
        uint32_t reserved0               : 3;
        uint32_t ddr_dummy_byte_cnt      : 4;    /*!< Register operation DummyByte in DDR mode. */
        uint32_t reserved1               : 16;
    } b;                                         /*!< Register bits. */
} cmd_ins_t;

/**
 * @brief  This union represents the bit fields in the SFC command address Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cmd_addr {
    uint32_t d32;                                /*!< Register bits. */
    struct {
        uint32_t cmd_addr                : 30;   /*!< Operation address when the register accesses the flash memory. */
        uint32_t reserved                : 2;
    } b;                                         /*!< Register bits. */
} cmd_addr_t;

/**
 * @brief  This union represents the bit fields in the SFC command data buffer Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cmd_databuf {
    uint32_t d32;                                /*!< Register bits. */
    struct {
        uint8_t databyte[MAX_BYTE_PER_DATABUF];  /*!< 1Byte of data. */
    } b;                                         /*!< Register bits. */
} cmd_databuf_t;

/**
 * @brief  Global Config Registers associated with SFC.
 */
typedef struct global_conf_regs {
    volatile uint32_t global_config;             /*!< global config register. <i>offset 0x100</i> */
    volatile uint32_t reserved[3];
    volatile uint32_t timing;                    /*!< timing register. <i>offset 0x110</i> */
} global_conf_regs_t;

/**
 * @brief  Bus mode Registers associated with SFC.
 */
typedef struct bus_regs {
    volatile uint32_t bus_config1;               /*!< bus config1 register. <i>offset 0x200</i> */
    volatile uint32_t bus_config2;               /*!< bus config1 register. <i>offset 0x204</i> */
    volatile uint32_t reserved[2];
    volatile uint32_t bus_flash_size;            /*!< bus flash size register. <i>offset 0x210</i> */
    volatile uint32_t bus_base_addr_cs0;         /*!< bus base addr cs0 register. <i>offset 0x214</i> */
    volatile uint32_t bus_base_addr_cs1;         /*!< bus base addr cs1 register. <i>offset 0x218</i> */
} bus_regs_t;

/**
 * @brief  DMA mode Registers associated with SFC.
 */
typedef struct bus_dma_regs {
    volatile uint32_t bus_dma_ctrl;              /*!< bus dma control register. <i>offset 0x240</i> */
    volatile uint32_t bus_dma_mem_saddr;         /*!< bus dma memory start address register. <i>offset 0x244</i> */
    volatile uint32_t bus_dma_flash_saddr;       /*!< bus dma flash start address register. <i>offset 0x248</i> */
    volatile uint32_t bus_dma_len;               /*!< bus dma data length address register. <i>offset 0x24C</i> */
    volatile uint32_t bus_dma_ahb_ctrl;          /*!< bus dma data ahb control address register. <i>offset 0x250</i> */
} bus_dma_regs_t;

/**
 * @brief  SPI mode Registers associated with SFC.
 */
typedef struct cmd_regs {
    volatile uint32_t cmd_config;                /*!< command config register. <i>offset 0x300</i> */
    volatile uint32_t reserved;
    volatile uint32_t cmd_ins;                   /*!< command ins register. <i>offset 0x308</i> */
    volatile uint32_t cmd_addr;                  /*!< command address register. <i>offset 0x30C</i> */
} cmd_regs_t;

/**
 * @brief  SPI data buffer Registers associated with SFC.
 */
typedef struct cmd_databufs {
    volatile uint32_t cmd_databuf[MAX_DATABUF_NUM]; /*!< command data buffer register. <i>offset (0x400 + 4 * n)</i> */
} cmd_databufs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif