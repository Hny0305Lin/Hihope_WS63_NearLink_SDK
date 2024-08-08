#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_timer")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_drv_timer.c
)

set(PRIVATE_HEADER
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(DEFINED CONFIG_TIMER_USING_V100)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_timer_v100.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_timer_v100_regs_op.c")
    list(APPEND PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/v100)
endif()

if(DEFINED CONFIG_TIMER_USING_V150)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v150/hal_timer_v150.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v150/hal_timer_v150_regs_op.c")
    list(APPEND PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/v150)
endif()

if(NOT DEFINED CONFIG_TIMER_USING_V100 AND NOT DEFINED CONFIG_TIMER_USING_V150)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_timer_v100.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_timer_v100_regs_op.c")
    list(APPEND PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/v100)
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
