#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "ws63_mem_config")

# check this component whether on the rom.
if(NOT ${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
    return()
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/product
    ${CMAKE_CURRENT_SOURCE_DIR}/common_config
)

build_component()
