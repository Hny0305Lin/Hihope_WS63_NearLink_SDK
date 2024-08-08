#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2024. All rights reserved.
#===============================================================================
MESSAGE(STATUS "project_name:" ${project_name})
if(CHIP STREQUAL "ws63")
    include (${CMAKE_CURRENT_SOURCE_DIR}/build/${CHIP}/radar_entry.cmake)
    return()
elseif(("${project_name}" MATCHES "1156") OR ("${project_name}" MATCHES "1155"))
    include (${CMAKE_CURRENT_SOURCE_DIR}/build/115x/radar_entry.cmake)
    return()
elseif("${project_name}" MATCHES "hiexv300")
    include (${CMAKE_CURRENT_SOURCE_DIR}/build/${project_name}/radar_entry.cmake)
    return()
endif()