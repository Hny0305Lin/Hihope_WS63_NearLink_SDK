#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

add_subdirectory_if_exist(bootloader)
add_subdirectory_if_exist(drivers)
add_subdirectory_if_exist(middleware)
add_subdirectory_if_exist(open_source)
add_subdirectory_if_exist(include)
add_subdirectory_if_exist(kernel)