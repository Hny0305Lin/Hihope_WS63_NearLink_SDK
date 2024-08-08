#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "timer_port_rom")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/timer_porting.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PUBLIC_DEFINES
    DEFAULT_TIMER=1
    TEST_TIMER_INDEX=2
)

set(WHOLE_LINK
    true
)

build_component()

install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/ "*.h")
