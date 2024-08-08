#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${CHIP}_${BOARD})
    add_subdirectory(${CHIP}_${BOARD})
endif()

add_subdirectory_if_exist(${CHIP})