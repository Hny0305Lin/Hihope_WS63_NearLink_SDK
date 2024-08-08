#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SUPPORT_RADAR_STA_SAMPLE)
    add_subdirectory_if_exist(sta_sample)
endif()

if(DEFINED CONFIG_SUPPORT_RADAR_SOFTAP_SAMPLE)
    add_subdirectory_if_exist(softap_sample)
endif()

if(DEFINED CONFIG_SUPPORT_RADAR_M_SAMPLE)
    add_subdirectory_if_exist(m_sample)
endif()

if(DEFINED CONFIG_SUPPORT_RADAR_STA_CONNECT_SAMPLE)
    add_subdirectory_if_exist(sta_connect_sample)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
