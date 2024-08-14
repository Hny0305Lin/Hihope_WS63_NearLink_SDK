#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================

if(DEFINED CONFIG_SAMPLE_SUPPORT_SPI_MASTER)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/ssd1306_fonts.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ssd1306.c
    ${CMAKE_CURRENT_SOURCE_DIR}/spi_master_demo.c
)
set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
endif()
