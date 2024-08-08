#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_rtc_unified")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_rtc.c
)

if(DEFINED CONFIG_RTC_USING_V150)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v150/hal_rtc_v150.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v150/hal_rtc_v150_op.c")
else()
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_rtc_v100.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_rtc_v100_op.c")
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(DEFINED CONFIG_RTC_USING_V150)
list(APPEND PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/v150
)
else()
list(APPEND PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/v100
)
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
return_if_not_build()
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/  "*.h")
