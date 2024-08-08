#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

if("liteos_207" IN_LIST TARGET_COMPONENT)
    add_subdirectory(liteos_v207 ${PROJECT_BINARY_DIR}/liteos)
elseif("liteos_207_0_0" IN_LIST TARGET_COMPONENT)
    add_subdirectory(liteos_v207.0.0)
elseif("liteos_207_1_0_b014" IN_LIST TARGET_COMPONENT)
    add_subdirectory(liteos_v207.1.0_b014)
elseif("liteos_207_1_0" IN_LIST TARGET_COMPONENT)
    add_subdirectory(liteos_v207.1.0)
elseif("liteos_208_5_0_b004" IN_LIST TARGET_COMPONENT)
    add_subdirectory(liteos_v208.5.0_b004)
elseif("liteos_208_5_0" IN_LIST TARGET_COMPONENT)
    add_subdirectory(liteos_v208.5.0)
elseif("liteos_208_6_0_b017" IN_LIST TARGET_COMPONENT)
    add_subdirectory(liteos_v208.6.0_b017)
elseif("liteos_208_6_0_b017_cat1" IN_LIST TARGET_COMPONENT)
    add_subdirectory(liteos_v208.6.0_b017_cat1)
endif()

add_subdirectory_if_exist(ohos_adapt)
add_subdirectory_if_exist(printf_adapt)
