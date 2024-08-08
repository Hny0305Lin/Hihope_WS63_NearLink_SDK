#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
if(CHIP STREQUAL "ws63")
    include (${CMAKE_CURRENT_SOURCE_DIR}/build/${CHIP}/radar_ai_entry.cmake)
    return()
elseif(("${project_name}" MATCHES "1156") OR ("${project_name}" MATCHES "1155"))
    include (${CMAKE_CURRENT_SOURCE_DIR}/build/115x/radar_ai_entry.cmake)
    return()
elseif(CHIP STREQUAL "2831")
    include (${CMAKE_CURRENT_SOURCE_DIR}/build/${CHIP}/radar_ai_entry.cmake)
    return()
endif()
