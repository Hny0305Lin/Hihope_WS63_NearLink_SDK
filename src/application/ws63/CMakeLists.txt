#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(ws63_liteos_application)
add_subdirectory_if_exist(ws63_liteos_testsuite)
add_subdirectory_if_exist(ws63_freertos_application)
add_subdirectory_if_exist(ws63_freertos_testsuite)
add_subdirectory_if_exist(ws63_alios_application)
add_subdirectory_if_exist(ws63_alios_testsuite)
add_subdirectory_if_exist(ws63_ate)
add_subdirectory_if_exist(ws63_liteos_mfg)