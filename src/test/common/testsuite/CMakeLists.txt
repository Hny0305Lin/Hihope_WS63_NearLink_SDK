#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "testsuite")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_suite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_suite_channel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_suite_commands_processor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_suite_console.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_suite_functions_processor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_suite_ipc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_suite_log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_suite_task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_suite_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_auxiliary.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_lieyin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_crc16.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    CONFIG_TESTSUITE_UART
    SUPPORT_TEST_COMMON
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
