#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "i2c_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/i2c_porting.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(WHOLE_LINK
    true
)

build_component()
