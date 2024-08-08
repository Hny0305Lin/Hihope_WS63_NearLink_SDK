#[[
Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
Description: CMake entries of sub-directories.
Create: 2021-03-17
]]
set(MODULE_NAME "wifi")
set(AUTO_DEF_FILE_ID FALSE)
set(WIFI_DIR ${CMAKE_CURRENT_SOURCE_DIR})
include (${CMAKE_CURRENT_SOURCE_DIR}/wifi_alg.cmake)

if(CHIP STREQUAL "ws63")
    include (${CMAKE_CURRENT_SOURCE_DIR}/build/${CHIP}/wifi_entry.cmake)
    return()
endif()

if(CHIP STREQUAL "ws53")
    include (${CMAKE_CURRENT_SOURCE_DIR}/build/${CHIP}/wifi_entry.cmake)
    return()
endif()

if(DEFINED CHIP)
    return()
endif()

get_property(BUILD_TARGETS GLOBAL PROPERTY BUILD_TARGETS)
get_property(BUILD_ROM_TARGETS GLOBAL PROPERTY BUILD_ROM_TARGETS)

if(wifi_hmac IN_LIST BUILD_TARGETS)
include (${CMAKE_CURRENT_SOURCE_DIR}/wifi_hmac.cmake)
endif()

if(wifi_dmac IN_LIST BUILD_TARGETS)
include (${CMAKE_CURRENT_SOURCE_DIR}/wifi_dmac.cmake)
endif()

if(wifi_hmac_ram IN_LIST BUILD_TARGETS)
include (${CMAKE_CURRENT_SOURCE_DIR}/wifi_hmac_ram.cmake)
endif()

if(wifi_dmac_ram IN_LIST BUILD_TARGETS)
include (${CMAKE_CURRENT_SOURCE_DIR}/wifi_dmac_ram.cmake)
endif()

if(source_host IN_LIST BUILD_TARGETS)
include (${CMAKE_CURRENT_SOURCE_DIR}/source_host.cmake)
endif()

if(source_device IN_LIST BUILD_TARGETS)
include (${CMAKE_CURRENT_SOURCE_DIR}/source_device.cmake)
endif()

if(wifi_device_ram IN_LIST BUILD_TARGETS)
include (${CMAKE_CURRENT_SOURCE_DIR}/wifi_device_ram.cmake)
endif()

if(wifi_device_flash IN_LIST BUILD_TARGETS)
include (${CMAKE_CURRENT_SOURCE_DIR}/wifi_device_flash.cmake)
endif()

if(wifi_device_rom IN_LIST BUILD_ROM_TARGETS)
include (${CMAKE_CURRENT_SOURCE_DIR}/wifi_device_rom.cmake)
endif()

if(wifi_drv IN_LIST BUILD_TARGETS)
include (${CMAKE_CURRENT_SOURCE_DIR}/wifi_drv.cmake)
endif()

if(wifi_cali IN_LIST BUILD_TARGETS)
include (${CMAKE_CURRENT_SOURCE_DIR}/wifi_cali.cmake)
endif()

add_subdirectory(source)
add_subdirectory(sample)
