#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ohos_drv_v3.2")

set(SOURCES
if(DEFINED CONFIG_DRIVER_SUPPORT_SFC)
    ${CMAKE_CURRENT_SOURCE_DIR}/iot_sfc.c
else()
    ${CMAKE_CURRENT_SOURCE_DIR}/iot_flash.c
endif()
    ${CMAKE_CURRENT_SOURCE_DIR}/iot_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/iot_i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/iot_pwm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/iot_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/iot_watchdog.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lowpower.c
    ${CMAKE_CURRENT_SOURCE_DIR}/reset.c
)

set(PUBLIC_HEADER
    ${ROOT_DIR}/open_source/openharmony/iothardware/peripheral/interfaces/kits
)

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
