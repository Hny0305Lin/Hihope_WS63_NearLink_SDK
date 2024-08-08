#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_RADAR_STA)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/radar_sta_sample/radar_sta_sample.c" PARENT_SCOPE)
    set(PUBLIC_HEADER "${PUBLIC_HEADER}" "${CMAKE_CURRENT_SOURCE_DIR}/../../../include/middleware/services/radar/radar_service.h" PARENT_SCOPE)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_RADAR_SOFTAP)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/radar_softap_sample/radar_softap_sample.c" PARENT_SCOPE)
    set(PUBLIC_HEADER "${PUBLIC_HEADER}" "${CMAKE_CURRENT_SOURCE_DIR}/../../../include/middleware/services/radar/radar_service.h" PARENT_SCOPE)
endif()