/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description:  WS63 Application Core Platform Definitions
 *
 * Create:  2021-06-16
 */

#ifndef PLATFORM_CORE_H
#define PLATFORM_CORE_H

#include "chip_core_definition.h"
#include "platform_core_rom.h"

/**
 * @defgroup DRIVER_PLATFORM_CORE CHIP Platform CORE Driver
 * @ingroup DRIVER_PLATFORM
 * @{
 */
#ifndef YES
#define YES (1)
#endif

#ifndef NO
#define NO (0)
#endif

extern unsigned int get_hso_buff(void);
#define LOGGING_REGION_START get_hso_buff()
#define LOGGING_REGION_LENGTH 0x1000

#define GLB_CTL_M_RB_BASE        0x40002000
#define GLB_CTL_B_RB_BASE        0x57000400
#define GLB_CTL_D_RB_BASE        0x57000800
#define DISPLAY_CTL_RB_BASE      0x56000000
#define GPU_BASE_ADDR            0x56200000
#define DSS_BASE_ADDR            0x56100000

#define B_CTL_RB_BASE            0x59000000
#define M_CTL_RB_BASE            0x52000000
#define COM_CTL_RB_BASE          0x55000000
#define PMU1_CTL_RB_BASE         0x40003000
#define PMU2_CMU_CTL_RB_BASE     0x57008000
#define ULP_AON_CTL_RB_ADDR      0x5702c000
#define FUSE_CTL_RB_ADDR         0x57028000
#define XO_CORE_TRIM_REG         0x57028308
#define XO_CORE_CTRIM_REG        0x5702830c
#define XIP_CACHE_CTL            0xA3006000
#define NMI_CTL_REG_BASE_ADDR    0x52000700

#define FLASH_START_ADDR 0x200000

#define UART0_BASE     0x44010004 /* UART_L0 */
#define UART1_BASE     0x44011004 /* UART_H0 */
#define UART2_BASE     0x44012004 /* UART_H1 */
#define DMA_BASE_ADDR  0x4A000000 /* M_DMA */
#define SDMA_BASE_ADDR 0x520A0000 /* S_DMA */

/* I2C reg base addr */
#define I2C_BUS_0_BASE_ADDR 0x44018000
#define I2C_BUS_1_BASE_ADDR 0x44018100

/* spi reg base addr */
#define SPI_BUS_0_BASE_ADDR 0x44020000 // SPI_M0
#define SPI_BUS_1_BASE_ADDR 0x44021000 // QSPI_0

#define I2S_BUS_0_BASE_ADDR 0x4402503C

#define DMA_HANDSHAKE_I2C_BUS_0_TX HAL_DMA_HANDSHAKING_I2C0_TX
#define DMA_HANDSHAKE_I2C_BUS_0_RX HAL_DMA_HANDSHAKING_I2C0_RX
#define DMA_HANDSHAKE_I2C_BUS_1_TX HAL_DMA_HANDSHAKING_I2C1_TX
#define DMA_HANDSHAKE_I2C_BUS_1_RX HAL_DMA_HANDSHAKING_I2C1_RX
#define DMA_HANDSHAKE_I2C_BUS_2_TX HAL_DMA_HANDSHAKING_I2C2_TX
#define DMA_HANDSHAKE_I2C_BUS_2_RX HAL_DMA_HANDSHAKING_I2C2_RX
#define DMA_HANDSHAKE_I2C_BUS_3_TX HAL_DMA_HANDSHAKING_I2C3_TX
#define DMA_HANDSHAKE_I2C_BUS_3_RX HAL_DMA_HANDSHAKING_I2C3_RX
#define DMA_HANDSHAKE_I2C_BUS_4_TX HAL_DMA_HANDSHAKING_I2C4_TX
#define DMA_HANDSHAKE_I2C_BUS_4_RX HAL_DMA_HANDSHAKING_I2C4_RX

#define HAL_SPI_DEVICE_MODE_SET_REG   (*(volatile unsigned short *)(0x44000250))
#define HAL_SPI3_MODE_SET_REG         (M_CTL_RB_BASE + 0x950)

/* PWM reg base addr */
#define PWM_0_BASE 0x44024000

/* PWM INTR REG */
#define PWM_INTR_ENABLE_REG (*(volatile unsigned short *)0x52000900)
#define PWM_INTR_CLEAR_REG  (*(volatile unsigned short *)0x52000904)
#define PWM_INTR_STATUS_REG (*(volatile unsigned short *)0x52000908)

// GPIO regs
#define GPIO_CHANNEL_0_BASE_ADDR    0x44028000
#define GPIO_CHANNEL_1_BASE_ADDR    0x44029000
#define GPIO_CHANNEL_2_BASE_ADDR    0x4402A000

#define ULP_GPIO_BASE_ADDR 0x57030000 // ULP GPIO

// GPIO select core
#define HAL_GPIO_D_CORE_SET_REG             0x570001B0
#define HAL_GPIO_NON_D_CORE_SET_REG         0x57000180
#define HAL_GPIO_CORE_SET_CHANNEL_OFFSET    0x08
#define HAL_GPIO_CORE_SET_REG_OFFSET        2
#define HAL_GPIO_CORE_SET_GPIOS             16

// ULP GPIO int clk config
#define HAL_GPIO_ULP_AON_GP_REG                 0x5702C010
#define HAL_GPIO_ULP_AON_PCLK_INT_EN_BIT        0
#define HAL_GPIO_ULP_AON_PCLK_INT_CLK_SEL_BIT   1
#define HAL_GPIO_ULP_PCLK_INTR_STATUS_BITS      0x3

#define RTC_TIMER_BASE_ADDR 0x57024000

#define SYSTICK_BASE_ADDR 0x40005000

#define AFE_DIG_BASE_ADDRESS  0x57036000

#define HAL_SOFT_RST_CTL_BASE               (GLB_CTL_M_RB_BASE)
#define HAL_GLB_CTL_M_ATOP1_L_REG_OFFSET    0x100
#define HAL_CHIP_WDT_ATOP1_RST_BIT          4

// SEC BASE ADDR
#define SEC_CTL_RB_BASE 0x52009000
#define RSAV2_S_RB_BASE 0x52009900
#define TRNG_RB_BASE 0x52009800

// cpu trace memory
#define TRACE_MEM_REGION_START                  MCPU_TRACE_MEM_REGION_START
#define TRACE_MEM_REGION_LENGTH                 CPU_TRACE_MEM_REGION_LENGTH

/*
 * Maximum UART buses
 * Defined here rather than in the uart_bus_t enum, due to needing to use it for conditional compilation
 */
#define UART_BUS_MAX_NUMBER 3  // !< Max number of UARTS available
#define I2C_BUS_MAX_NUMBER  2  // !< Max number of I2C available

#define SPI_BUS_MAX_NUMBER  2  // !< Max number of SPI available
#define GPIO_MAX_NUMBER     3  // !< Max number of GPIO available
#define I2S_MAX_NUMBER      1

#define S_DMA_CHANNEL_MAX_NUM    4  // !< Max number of SM_DMA available
#define B_DMA_CHANNEL_MAX_NUM    8  // !< Max number of M_DMA available

#define DMA_CHANNEL_MAX_NUM      (S_DMA_CHANNEL_MAX_NUM + B_DMA_CHANNEL_MAX_NUM)

#define CHIP_BCPU_SWDDIO  0
#define CHIP_BCPU_SWDCLK  0

#define TEST_SUITE_UART_BUS CONFIG_TESTSUIT_UART // UART H0
/* Test Suite UART Transmission PIN to use */
#define TEST_SUITE_UART_TX_PIN S_AGPIO9
/* Test Suite UART Reception PIN to use */
#define TEST_SUITE_UART_RX_PIN S_AGPIO10

#ifdef PRE_ASIC
#ifdef SW_UART_DEBUG
    #define CHIP_FIXED_RX_PIN S_AGPIO14
    #define CHIP_FIXED_TX_PIN S_AGPIO15
#else
    #define CHIP_FIXED_RX_PIN ULP_GPIO1
    #define CHIP_FIXED_TX_PIN ULP_GPIO0
#endif
#else
#if defined(DEVICE_ONLY)
    #define CHIP_FIXED_TX_PIN S_AGPIO18 // S_MGPIO9
    #define CHIP_FIXED_RX_PIN S_AGPIO19 // S_MGPIO10
#else
    #define CHIP_FIXED_TX_PIN S_AGPIO9
    #define CHIP_FIXED_RX_PIN S_AGPIO10
#endif
#endif
#define SW_DEBUG_UART_BUS   CONFIG_DEBUG_UART
#define CHIP_FIXED_UART_BUS CONFIG_WVT_UART
#define CODELOADER_UART_BUS UART_BUS_0
#define CODELOADER_UART_TX_PIN S_AGPIO15
#define CODELOADER_UART_RX_PIN S_AGPIO14

#define LOG_UART_BUS        CONFIG_LOG_UART
#define LOG_UART_TX_PIN     S_AGPIO12 // S_AGPIO18
#define LOG_UART_RX_PIN     S_AGPIO13 // S_AGPIO19

#define AT_UART_BUS         CONFIG_AT_UART

#define QSPI_MAX_NUMBER    2
#define FLASH_QSPI_ID      QSPI_BUS_1
#define QSPI_0_BASE_ADDR   0xA3000000
#define QSPI_1_BASE_ADDR   0xA3002000

/* !< QSPI bus */
typedef enum {
    QSPI_BUS_0 = 0,
    QSPI_BUS_1,
    QSPI_BUS_NONE = QSPI_MAX_NUMBER
} qspi_bus_t;

/**
 * @brief  UART bus.
 */
typedef enum {       // !< Hi1132 | Hi1135
    UART_BUS_0 = 0,  // !< UART L | UART L0
#if UART_BUS_MAX_NUMBER > 1
    UART_BUS_1 = 1,  // !< UART H | UART H0
#endif
#if UART_BUS_MAX_NUMBER > 2
    UART_BUS_2 = 2,  // !< M UART | UART H1
#endif
    UART_BUS_NONE = UART_BUS_MAX_NUMBER  // !< Value used as invalid/unused UART number
} uart_bus_t;

/**
 * @brief  I2C bus.
 */
typedef enum {
    I2C_BUS_0,               // !< I2C0
    I2C_BUS_1,               // !< I2C1
#if I2C_BUS_MAX_NUMBER > 2
    I2C_BUS_2,               // !< I2C2
#if I2C_BUS_MAX_NUMBER > 3  // !< I2C3
    I2C_BUS_3 = 3,
    I2C_BUS_4 = 4,
#endif
#endif
    I2C_BUS_NONE = I2C_BUS_MAX_NUMBER
} i2c_bus_t;

/***************************************
For WS63 config:
    SPI_M0,
    SPI_MS_1,
    SPI_MS_2,
    SPI_MS_3,
    SPI_S_4,
    QSPI_0,
    QSPI_1
***************************************/
/**
 * @brief  Definition of SPI bus index.
 */
typedef enum {
    SPI_BUS_0 = 0,
    SPI_BUS_1,
    SPI_BUS_2,
    SPI_BUS_3,
    SPI_BUS_4,
    SPI_BUS_5,
    SPI_BUS_6,
    SPI_BUS_NONE = SPI_BUS_MAX_NUMBER
} spi_bus_t;


/**
 * @brief SIO(I2S/PCM) Bus.
 *
 */
typedef enum {
    SIO_BUS_0,
    SIO_NONE = I2S_MAX_NUMBER
} sio_bus_t;

/* !< SLAVE CPU */
typedef enum {
    SLAVE_CPU_BT,
    SLAVE_CPU_MAX_NUM,
} slave_cpu_t;

/**********************************************************************/
/************************* MPU config base addr ***********************/
/**********************************************************************/
// config register region
#define MPU_REG_ADDR0_BASE           0x50000000
#define MPU_REG_ADDR0_LEN            0x10000000

#define MPU_REG_ADDR1_BASE           0xA3000000
#define MPU_REG_ADDR1_LEN            0x01000000

// ROM region
#define MPU_ROM_ADDR_BASE               0x0
#define MPU_ROM_ADDR_LEN                0x8000

// ITCM region
#define MPU_ITCM_ADDR_BASE              0x80000
#define MPU_ITCM_ADDR_LEN               0x80000

// L2ram region
#define MPU_L2RAM_ADDR0_BASE            0x100000
#define MPU_L2RAM_ADDR0_LEN             0x100000
#define MPU_L2RAM_ADDR1_BASE            0x200000
#define MPU_L2RAM_ADDR1_LEN             0x100000

// XIP PSRAM read & execute region
#define MPU_XIP_PSRAM_RO_ADDR_BASE      0x08000000
#define MPU_XIP_PSRAM_RO_ADDR_LEN       0x04000000

// XIP PSRAM bypass(read/write) region
#define MPU_XIP_PSRAM_RW_ADDR_BASE      0x0C000000
#define MPU_XIP_PSRAM_RW_ADDR_LEN       0x04000000

// XIP NorFlash region
#define MPU_XIP_FLASE_RO_ADDR_BASE      0x10000000
#define MPU_XIP_FLASE_RO_ADDR_LEN       0x10000000

// Sharemem region
#define MPU_SHAREMEM_ADDR_BASE          0x87000000
#define MPU_SHAREMEM_ADDR_LEN           0x10000

// MDMA(m0) address judge
#define MEM_X2P_MEMORY_START    0xA3000000
#define MEM_X2P_MEMORY_END      0xA3008FFF
#define L2RAM_MEMORY_START      0x00100000
#define L2RAM_MEMORY_END        0x0035FFFF
#define QSPI_XIP_MEMORY_START   0x08000000
#define QSPI_XIP_MEMORY_END     0x1FFFFFFF

// CHIP RESET offset address
#define CHIP_RESET_OFF   0x110

// Power on the flash memory on the FPGA
#define PMU_RESERV1      0x570040C8
#define HI_SFC_REG_BASE             0x48000000
#define HI_SFC_FLASH1_BASE          0x48100000
#define HI_SFC_MEM_SIZE             0x00800000


// 适配旧GPIO号定义
#define S_MGPIO0    0          // GPIO0_0   C_PINMUX_CTL offset: 0x0300
#define S_MGPIO1    1          // GPIO0_1   C_PINMUX_CTL
#define S_MGPIO2    2          // GPIO0_2   C_PINMUX_CTL
#define S_MGPIO3    3          // GPIO0_3   C_PINMUX_CTL
#define S_MGPIO4    4          // GPIO0_4   C_PINMUX_CTL
#define S_MGPIO5    5          // GPIO0_5   C_PINMUX_CTL
#define S_MGPIO6    6          // GPIO0_6   C_PINMUX_CTL
#define S_MGPIO7    7          // GPIO0_7   C_PINMUX_CTL
#define S_MGPIO8    8          // GPIO0_8   C_PINMUX_CTL
#define S_MGPIO9    9          // GPIO0_9   C_PINMUX_CTL
#define S_MGPIO10   10         // GPIO0_10  C_PINMUX_CTL
#define S_MGPIO11   11         // GPIO0_11  C_PINMUX_CTL
#define S_MGPIO12   12         // GPIO0_12  C_PINMUX_CTL
#define S_MGPIO13   13         // GPIO0_13  C_PINMUX_CTL
#define S_MGPIO14   14         // GPIO0_14  C_PINMUX_CTL
#define S_MGPIO15   15         // GPIO0_15  C_PINMUX_CTL
#define S_MGPIO16   16         // GPIO0_16  C_PINMUX_CTL
#define S_MGPIO17   17         // GPIO0_17  C_PINMUX_CTL
#define S_MGPIO18   18         // GPIO0_18  C_PINMUX_CTL
#define S_MGPIO19   19         // GPIO0_19  C_PINMUX_CTL
#define S_MGPIO20   20         // GPIO0_20  C_PINMUX_CTL
#define S_MGPIO21   21         // GPIO0_21  C_PINMUX_CTL
#define S_MGPIO22   22         // GPIO0_22  C_PINMUX_CTL
#define S_MGPIO23   23         // GPIO0_23  C_PINMUX_CTL
#define S_MGPIO24   24         // GPIO0_24  C_PINMUX_CTL
#define S_MGPIO25   25         // GPIO0_25  C_PINMUX_CTL
#define S_MGPIO26   26         // GPIO0_26  C_PINMUX_CTL
#define S_MGPIO27   27         // GPIO0_27  C_PINMUX_CTL
#define S_MGPIO28   28         // GPIO0_28  C_PINMUX_CTL offset: 0x031C
#define S_MGPIO29   29         // GPIO0_29  C_PINMUX_CTL
#define S_MGPIO30   30         // GPIO0_30  C_PINMUX_CTL
#define S_MGPIO31   31         // GPIO0_31  C_PINMUX_CTL

#define S_AGPIO0    32         // GPIO1_0   PINMUX_CTL   offset: 0x0320
#define S_AGPIO1    33         // GPIO1_1   PINMUX_CTL
#define S_AGPIO2    34         // GPIO1_2   PINMUX_CTL
#define S_AGPIO3    35         // GPIO1_3   PINMUX_CTL
#define S_AGPIO4    36         // GPIO1_4   PINMUX_CTL   offset: 0x0324
#define S_AGPIO5    37         // GPIO1_5   PINMUX_CTL
#define S_AGPIO6    38         // GPIO1_6   PINMUX_CTL
#define S_AGPIO7    39         // GPIO1_7   PINMUX_CTL
#define S_AGPIO8    40         // GPIO1_8   PINMUX_CTL   offset: 0x0328
#define S_AGPIO9    41         // GPIO1_9   PINMUX_CTL
#define S_AGPIO10   42         // GPIO1_10  PINMUX_CTL
#define S_AGPIO11   43         // GPIO1_11  PINMUX_CTL
#define S_AGPIO12   44         // GPIO1_12  PINMUX_CTL   offset: 0x032C
#define S_AGPIO13   45         // GPIO1_13  PINMUX_CTL
#define S_AGPIO14   46         // GPIO1_14  PINMUX_CTL
#define S_AGPIO15   47         // GPIO1_15  PINMUX_CTL
#define S_AGPIO16   48         // GPIO1_16  PINMUX_CTL   offset: 0x0330
#define S_AGPIO17   49         // GPIO1_17  PINMUX_CTL
#define S_AGPIO18   50         // GPIO1_18  PINMUX_CTL
#define S_AGPIO19   51         // GPIO1_19  PINMUX_CTL

#define S_HGPIO0   52          // GPIO1_20  C_PINMUX_CTL offset: 0x0334
#define S_HGPIO1   53          // GPIO1_21  C_PINMUX_CTL
#define S_HGPIO2   54          // GPIO1_22  C_PINMUX_CTL
#define S_HGPIO3   55          // GPIO1_23  C_PINMUX_CTL
#define S_HGPIO4   56          // GPIO1_24  C_PINMUX_CTL offset: 0x0338
#define S_HGPIO5   57          // GPIO1_25  C_PINMUX_CTL

#define L_HGPIO0   64          // RESERVED
#define L_HGPIO11   75         // RESERVED

#define L_AGPIO0   76          // GPIO2_12  PINMUX_CTL   offset: 0x034C
#define L_AGPIO1   77          // GPIO2_13  PINMUX_CTL
#define L_AGPIO2   78          // GPIO2_14  PINMUX_CTL
#define L_AGPIO3   79          // GPIO2_15  PINMUX_CTL
#define L_AGPIO8   84          // RESERVED

#define L_MGPIO0   85          // GPIO2_21  C_PINMUX_CTL offset: 0x0354
#define L_MGPIO1   86          // GPIO2_22  C_PINMUX_CTL
#define L_MGPIO2   87          // GPIO2_23  C_PINMUX_CTL
#define L_MGPIO3   88          // GPIO2_24  C_PINMUX_CTL
#define L_MGPIO4   89          // GPIO2_25  C_PINMUX_CTL
#define L_MGPIO5   90          // GPIO2_26  C_PINMUX_CTL
#define L_MGPIO6   91          // GPIO2_27  C_PINMUX_CTL
#define L_MGPIO7   92          // GPIO2_28  C_PINMUX_CTL
#define L_MGPIO8   93          // GPIO2_29  C_PINMUX_CTL
#define L_MGPIO9   94          // GPIO2_30  C_PINMUX_CTL
#define L_MGPIO10   95         // GPIO2_31  C_PINMUX_CTL
#define L_MGPIO11   96         // GPIO3_0   C_PINMUX_CTL
#define L_MGPIO12   97         // GPIO3_1   C_PINMUX_CTL
#define L_MGPIO13   98         // GPIO3_2   C_PINMUX_CTL
#define L_MGPIO14   99         // GPIO3_3   C_PINMUX_CTL
#define L_MGPIO15   100        // GPIO3_4   C_PINMUX_CTL
#define L_MGPIO16   101        // GPIO3_5   C_PINMUX_CTL
#define L_MGPIO17   102        // GPIO3_6   C_PINMUX_CTL
#define L_MGPIO18   103        // GPIO3_7   C_PINMUX_CTL
#define L_MGPIO19   104        // GPIO3_8   C_PINMUX_CTL
#define L_MGPIO31   104        // RESERVED
#define L_MGPIO47   104        // RESERVED
#define L_MGPIO57   104        // RESERVED

#define SYS_RSTN    105
#define RTC_CLK     106
#define ULP_GPIO0   107        // ULP_GPIO0
#define ULP_GPIO1   108        // ULP_GPIO1
#define ULP_GPIO2   109        // ULP_GPIO2
#define ULP_GPIO3   110        // ULP_GPIO3
#define PWR_HOLD    111        // ULP_GPIO4
#define HRESET      112        // ULP_GPIO5
#define SLEEP_N     113        // ULP_GPIO6
#define PMUIC_IRQ   114        // ULP_GPIO7
#define PIN_NONE    115        // used as invalid/unused PIN number
#define ULP_PIN     SYS_RSTN


#define TCXO_COUNT_ENABLE                 YES

#define LOW_POWER_WR15_CS_ENHANCE         YES
#define WATCHDOG_ROM_ENABLE               YES

#define GPIO_WITH_ULP                     YES
#define GPIO_FUNC                         HAL_PIO_FUNC_GPIO

#define I2C_AUTO_SEND_STOP_CMD            NO
#define I2C_WITH_BUS_RECOVERY             YES

#define SPI_WITH_OPI                      NO
#define SPI_DMA_TRANSFER_NUM_BY_BYTE      NO

#define DMA_TRANSFER_DEBUG                YES
#define DMA_USE_HIDMA                     NO
#define DMA_WITH_MDMA                     YES // dma
#define DMA_WITH_SMDMA                    YES // Small dma
#define DMA_TRANS_BY_LLI                  NO

#define XIP_WITH_OPI                      YES
#define XIP_INT_BY_NMI                    YES
#define EFLASH_SLAVE_NOTIFY_MASTER_BOOTUP YES

#define ENABLE_CPU_TRACE                  2
#define ADC_WITH_AUTO_SCAN                YES

#define OTP_HAS_READ_PERMISSION           YES
#define OTP_HAS_WRITE_PERMISSION          YES
#define OTP_HAS_CLKLDO_VSET               NO

#define SEC_IAMGE_AES_DECRYPT_EN          NO
#define SEC_BOOT_SIGN_CHECK_EN            YES
#define SEC_SUB_RST_BY_SECURITY_CORE      NO
#define TRNG_WITH_SEC_COMMON              YES
#define IS_MAIN_CORE                      YES
#define EXTERNAL_CLOCK_CALIBRATION        YES
#define AON_SPECIAL_PIO                   YES

#define SUPPORT_HI_EMMC_PHY               NO

#define OPI_PIN_FIX_DM1_DRIVER            NO
#define OPI_USE_MCU_HS_CLK                NO
#define QSPI0_FUNC  HAL_PIO_FUNC_QSPI0
#define QSPI0_D0    S_MGPIO0
#define QSPI0_D1    S_MGPIO1
#define QSPI0_D2    S_MGPIO2
#define QSPI0_D3    S_MGPIO3
#define QSPI0_CLK   S_MGPIO4
#define QSPI0_CS    S_MGPIO5

#ifdef ATE_FLASH_CHECK
#define QSPI1_FUNC  HAL_PIO_FUNC_QSPI0
#define QSPI1_D0    S_MGPIO0
#define QSPI1_D1    S_MGPIO1
#define QSPI1_D2    S_MGPIO2
#define QSPI1_D3    S_MGPIO3
#define QSPI1_CLK   S_MGPIO4
#define QSPI1_CS    S_MGPIO5
#else
#define QSPI1_FUNC  HAL_PIO_FUNC_QSPI1
#define QSPI1_D0    S_MGPIO6
#define QSPI1_D1    S_MGPIO7
#define QSPI1_D2    S_MGPIO8
#define QSPI1_D3    S_MGPIO9
#define QSPI1_CLK   S_MGPIO10
#define QSPI1_CS    S_MGPIO11
#endif

#define XIP_EXIST                         YES
#define USE_XIP_INDEX                     1
#define BCPU_INT0_ID                      26
#define UART_BAUD_RATE_DIV_8              NO
#define FIXED_IN_ROM                      NO
#define PMU_LPM_WAKEUP_SRC_NUM            16
#define ENABLE_GPIO_INTERRUPT             YES
#define CLK_AUTO_CG_ENABLE                NO
#if defined(BUILD_APPLICATION_ROM)
#define BOOT_ROM_DFR_PRINT                YES
#else
#define BOOT_ROM_DFR_PRINT                NO
#endif
#define EFLASH_WRITE_CLK_DIV_AUTO_ADJ     NO
#define DMA_WITH_MUX_CHANNEL              YES
#define CRITICAL_INT_RESTORE              YES
#define SEC_TRNG_ENABLE                  NO
#define DCACHE_IS_ENABLE                  NO
#define CODELOADER_SINGLE_PARTITION_EXP   YES
#define AUXLDO_ENABLE_FLASH               NO
#define TCXO_CLK_DYN_ADJUST               NO
#define SUPPORT_PARTITION_FEATURE         NO
#define SUPPORT_SINGLE_DSP_DUAL_IMAGE     NO
/**
 * @}
 */
#endif
