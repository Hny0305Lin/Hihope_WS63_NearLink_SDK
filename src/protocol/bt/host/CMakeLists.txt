#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2023. All rights reserved.
#===============================================================================
set(AUTO_DEF_FILE_ID FALSE)
if(${CHIP} MATCHES "bs20|bs21|bs21e|bs21a|bs22|bs26")
    set(BTH_ONETRACK TRUE)
elseif(${CHIP} MATCHES "ws63")
    set(BTH_ONETRACK TRUE)
else()
    set(BTH_ONETRACK FALSE)
endif()

# 通过定义cmake变量和宏适配没有Kconfig项目
include(KconfigAdapter.cmake)

add_subdirectory_if_exist(bg_common)
add_subdirectory_if_exist(bt)
add_subdirectory_if_exist(gle)