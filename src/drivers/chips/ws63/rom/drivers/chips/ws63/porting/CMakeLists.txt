#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

add_subdirectory_if_exist(pinctrl)
add_subdirectory_if_exist(systick)
add_subdirectory_if_exist(tcxo)
add_subdirectory_if_exist(timer)
add_subdirectory_if_exist(watchdog)
add_subdirectory_if_exist(version)