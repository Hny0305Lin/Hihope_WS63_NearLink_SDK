#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/gpu/CMakeLists.txt)
    add_subdirectory(gpu)
elseif(EXISTS ${ROOT_DIR}/libs_url/${CHIP}/cmake/gpu.cmake)
    include(${ROOT_DIR}/libs_url/${CHIP}/cmake/gpu.cmake)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/media/CMakeLists.txt)
    add_subdirectory(media)
elseif(EXISTS ${ROOT_DIR}/libs_url/${CHIP}/cmake/media.cmake)
    include(${ROOT_DIR}/libs_url/${CHIP}/cmake/media.cmake)
endif()

if (EXISTS ${ROOT_DIR}/protocol/wifi/CMakeLists.txt)
    add_subdirectory(wifi_service)
elseif(EXISTS ${BIN_DIR}/${CHIP}/libs/wifi_service/CMakeLists.txt)
    add_subdirectory(${BIN_DIR}/${CHIP}/libs/wifi_service wpa_supplicant)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/gui/CMakeLists.txt)
    add_subdirectory(gui)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ai/ai_toothbrush/CMakeLists.txt)
    add_subdirectory(ai/ai_toothbrush)
elseif(EXISTS ${ROOT_DIR}/libs_url/${CHIP}/cmake/ai_toothbrush.cmake)
    include(${ROOT_DIR}/libs_url/${CHIP}/cmake/ai_toothbrush.cmake)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/srv_tiot_host/CMakeLists.txt)
    add_subdirectory(srv_tiot_host)
endif()