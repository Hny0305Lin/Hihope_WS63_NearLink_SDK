#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(ate_wifi_linker)
add_subdirectory_if_exist(ate_bgle_linker)
add_subdirectory_if_exist(loaderboot_linker)
add_subdirectory_if_exist(ssb_linker)
add_subdirectory_if_exist(flashboot_linker)
add_subdirectory_if_exist(ws63_liteos_app_linker)
add_subdirectory_if_exist(ws63_liteos_hilink_linker)
add_subdirectory_if_exist(ws63_liteos_xts_linker)
add_subdirectory_if_exist(ws63_freertos_app_linker)
add_subdirectory_if_exist(ws63_alios_app_linker)
add_subdirectory_if_exist(ate_flash)