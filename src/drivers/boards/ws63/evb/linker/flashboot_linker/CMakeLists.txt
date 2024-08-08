#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ws63_flashboot_lds")

if(NOT ${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
    return()
endif()

set(PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR})
set(LINK_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linker.prelds" CACHE INTERNAL "" FORCE)
build_component()