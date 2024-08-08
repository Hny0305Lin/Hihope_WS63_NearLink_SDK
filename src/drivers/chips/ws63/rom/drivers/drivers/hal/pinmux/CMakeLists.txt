#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_pinctrl_rom")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/common/hal_pinctrl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${CHIP}/hal_pinctrl_${CHIP}.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/${CHIP}
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_HAL_PINCTRL
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)
build_component()

install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/ "*.h")
