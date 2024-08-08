#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_watchdog")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_watchdog.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(DEFINED CONFIG_WATCHDOG_USING_V151)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_watchdog_v151.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v151")
endif()

if(DEFINED CONFIG_WATCHDOG_USING_V100)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_watchdog_v100.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v100")
endif()

if(DEFINED CONFIG_WATCHDOG_USING_V152)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v152/hal_watchdog_v152.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v152")
endif()

if(NOT DEFINED CONFIG_WATCHDOG_USING_V100 AND NOT DEFINED CONFIG_WATCHDOG_USING_V151)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_watchdog_v100.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v100")
endif()

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
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
