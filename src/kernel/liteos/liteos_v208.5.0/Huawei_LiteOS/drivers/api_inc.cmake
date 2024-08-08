# Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.

## ATTENTION: api_inc.cmake is about external api header files' path. ##
## DON'T ADD OTHER VARS IN IT ##

if(LOSCFG_DRIVERS_UART_LINUX)
    set(LITEOS_UART_INCLUDE ${LITEOSTOPDIR}/drivers/uart_linux/include)
elseif(LOSCFG_DRIVERS_UART_LITEOS)
    set(LITEOS_UART_INCLUDE ${LITEOSTOPDIR}/drivers/uart/include)
endif()
if(LOSCFG_DRIVERS_IOMMU)
    set(LITEOS_IOMMU_INCLUDE ${LITEOSTOPDIR}/drivers/iommu/include)
endif()

if(LOSCFG_DRIVERS_BASE)
    set(LITEOS_DBASE_INCLUDE ${LITEOSTOPDIR}/drivers/base/include)
endif()

if(LOSCFG_DRIVERS_MMC)
    set(LITEOS_MMC_INCLUDE
	${LITEOSTOPDIR}/drivers/mmc/include
	${LITEOSTOPDIR}/${LITEOS_OPENSOURCE_PATHNAME}/freebsd/liteos/drivers/mmc/include
    )
endif()

if(LOSCFG_DRIVERS_MTD)
if(LOSCFG_DRIVERS_MTD_NAND)
    set(LITEOS_MTD_NAND_INCLUDE ${LITEOSTOPDIR}/drivers/mtd/nand/include)
endif()
if(LOSCFG_DRIVERS_MTD_SPI_NOR)
    set(LITEOS_MTD_SPI_NOR_INCLUDE ${LITEOSTOPDIR}/drivers/mtd/spi_nor/include)
endif()
endif()

if(LOSCFG_DRIVERS_CIPHER)
    set(LITEOS_CIPHER_INCLUDE
        ${LITEOSTOPDIR}/drivers/cipher/cipher/include
        ${LITEOSTOPDIR}/drivers/cipher/cipher/src/drv/cipher_v1.0/osal/include
        ${LITEOSTOPDIR}/drivers/cipher/cipher/src/drv/cipher_v1.0/drivers/core/include
        ${LITEOSTOPDIR}/drivers/cipher/cipher/src/drv/cipher_v1.0/drivers/crypto/include
        ${LITEOSTOPDIR}/drivers/cipher/osal/include
        ${LITEOSTOPDIR}/drivers/cipher/sample
        ${LITEOSTOPDIR}/drivers/cipher/hi_include
    )
endif()

if(LOSCFG_DRIVERS_HIETH_SF)
    set(LITEOS_HIETH_SF_INCLUDE ${LITEOSTOPDIR}/drivers/hieth-sf/include)
endif()

if(LOSCFG_DRIVERS_USB)
    set(LITEOS_USB_INCLUDE
        ${LITEOSTOPDIR}/drivers/usb/include
    )
endif()

if(LOSCFG_DRIVERS_VIDEO)
    set(LITEOS_VIDEO_INCLUDE ${LITEOSTOPDIR}/${LITEOS_OPENSOURCE_PATHNAME}/incubator-nuttx/liteos/drivers/video/include)
endif()

set(LITEOS_DRIVERS_INCLUDE
    ${LITEOSTOPDIR}/drivers/interrupt/include
    ${LITEOSTOPDIR}/drivers/timer/include
)

if(LOSCFG_DRIVERS_GPIO)
    set(LITEOS_GPIO_INCLUDE ${LITEOSTOPDIR}/drivers/gpio/include)
endif()

if(LOSCFG_DRIVERS_HIDMAC)
    set(LITEOS_HIDMAC_INCLUDE ${LITEOSTOPDIR}/drivers/hidmac/include)
endif()

if(LOSCFG_DRIVERS_HIEDMAC)
    set(LITEOS_HIEDMAC_INCLUDE ${LITEOSTOPDIR}/drivers/hiedmac/include)
endif()

if(LOSCFG_DRIVERS_SPI)
    set(LITEOS_SPI_INCLUDE ${LITEOSTOPDIR}/drivers/spi/include)
endif()

if(LOSCFG_DRIVERS_RANDOM)
    set(LITEOS_RANDOM_INCLUDE ${LITEOSTOPDIR}/drivers/random/include)
endif()

if(LOSCFG_DRIVERS_I2C)
    set(LITEOS_I2C_INCLUDE ${LITEOSTOPDIR}/drivers/i2c/include)
endif()

list(APPEND LITEOS_DRIVERS_INCLUDE
    ${LITEOS_GPIO_INCLUDE}
    ${LITEOS_MMC_INCLUDE}
    ${LITEOS_MTD_NAND_INCLUDE}
    ${LITEOS_MTD_SPI_NOR_INCLUDE}
    ${LITEOS_USB_INCLUDE}
    ${LITEOS_DBASE_INCLUDE}
    ${LITEOS_CIPHER_INCLUDE}
    ${LITEOS_HIETH_SF_INCLUDE}
    ${LITEOS_VIDEO_INCLUDE}
    ${LITEOS_HIDMAC_INCLUDE}
    ${LITEOS_HIEDMAC_INCLUDE}
    ${LITEOS_SPI_INCLUDE}
    ${LITEOS_RANDOM_INCLUDE}
    ${LITEOS_I2C_INCLUDE}
    ${LITEOS_IOMMU_INCLUDE}
)

