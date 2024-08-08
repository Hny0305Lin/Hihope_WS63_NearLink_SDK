#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_spi")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_spi.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(DEFINED CONFIG_SPI_USING_V151)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_spi_v151.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_spi_v151_regs_op.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v151")
    install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/v151" "*.h")
endif()

if(DEFINED CONFIG_SPI_USING_V100)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_spi_v100.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_spi_v100_regs_op.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v100")
    install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/v100" "*.h")
endif()

if(NOT DEFINED CONFIG_SPI_USING_V100 AND NOT DEFINED CONFIG_SPI_USING_V151)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_spi_v100.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_spi_v100_regs_op.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v100")
    install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/v100" "*.h")
endif()

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}" "*.h")

build_component()
