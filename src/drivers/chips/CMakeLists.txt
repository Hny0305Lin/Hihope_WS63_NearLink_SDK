#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${CHIP})
    add_subdirectory_if_exist(${CHIP})
endif()

if (${CHIP} MATCHES "bs20|bs21|bs21a|bs21e|bs22|bs26")
    add_subdirectory_if_exist(bs2x)
endif()