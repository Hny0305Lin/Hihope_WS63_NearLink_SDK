#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

set(WIFI_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/${CHIP})
if(USE_WS63_ROM_REPO IN_LIST DEFINES)
    set(WIFI_DEVICE_BUILD_DIR ${WIFI_BUILD_DIR}/romrepo)
    message("Build wifi device with rom repo!")
else()
    set(WIFI_DEVICE_BUILD_DIR ${WIFI_BUILD_DIR}/master)
endif()

# common basic config
include(${WIFI_DEVICE_BUILD_DIR}/basic.cmake)

# build device component
include(${WIFI_DEVICE_BUILD_DIR}/wifi_alg_device.cmake)
include(${WIFI_DEVICE_BUILD_DIR}/wifi_device.cmake)
# common basic config
include(${WIFI_DEVICE_BUILD_DIR}/basic.cmake)
# build host component
include(${WIFI_BUILD_DIR}/wifi_alg_host.cmake)
include(${WIFI_BUILD_DIR}/wifi_host.cmake)