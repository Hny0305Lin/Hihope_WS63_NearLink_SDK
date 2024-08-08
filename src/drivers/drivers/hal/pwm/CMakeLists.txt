#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_pwm")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_pwm.c
)

set(PRIVATE_HEADER
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(DEFINED CONFIG_PWM_USING_V150)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v150/hal_pwm_v150.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v150/hal_pwm_v150_regs_op.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v150")
endif()

if(DEFINED CONFIG_PWM_USING_V151)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_pwm_v151.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_pwm_v151_regs_op.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v151")
endif()

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)
build_component()
