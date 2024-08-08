#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_uart")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_uart.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(DEFINED CONFIG_UART_USING_V100)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_uart_v100.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_uart_v100_regs_op.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v100")
endif()

if(DEFINED CONFIG_UART_USING_V120)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v120/hal_uart_v120.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v120/hal_uart_v120_regs_op.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v120")
endif()

if(DEFINED CONFIG_UART_USING_V151)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_uart_v151.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_uart_v151_regs_op.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v151")
endif()

if(DEFINED CONFIG_UART_USING_LP_UART)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/lp_uart/hal_lp_uart.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/lp_uart/hal_lp_uart_regs_op.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/lp_uart")
endif()

if(NOT DEFINED CONFIG_UART_USING_V100 AND NOT DEFINED CONFIG_UART_USING_V120 AND NOT DEFINED CONFIG_UART_USING_V151)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v120/hal_uart_v120.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v120/hal_uart_v120_regs_op.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v120")
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
build_component()