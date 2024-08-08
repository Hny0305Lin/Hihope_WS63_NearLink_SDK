#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "at")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_cmd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_msg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_parse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_channel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_report.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_base.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_zdiag.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_HEADER
)

set(PUBLIC_DEFINES
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
