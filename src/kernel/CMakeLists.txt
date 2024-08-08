#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(non_os)
if(DEFINED OS AND NOT ${OS} STREQUAL "non-os")
    add_subdirectory_if_exist(${OS})
endif()
add_subdirectory_if_exist(osal)
add_subdirectory_if_exist(dpal)
add_subdirectory_if_exist(osal_adapt)