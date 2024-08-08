#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_pinctrl")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/common/hal_pinctrl.c

)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/common

)

if(DEFINED CONFIG_PINCTRL_USING_V150)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/common/hal_pinctrl_v150.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/common/hal_pinctrl_v150_regs_op.c")
else()
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${CHIP}/hal_pinctrl_${CHIP}.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/${CHIP}")
endif()

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
