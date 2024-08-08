#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(host)
add_subdirectory_if_exist(slave)
if(syschannel_host_ko IN_LIST TARGET_COMPONENT)
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/host "*")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/cfg "*")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/comm "*")
endif()