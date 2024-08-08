#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(SOURCES
    "${SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/radar_softap_sample.c"
    PARENT_SCOPE)

set(PUBLIC_HEADER
    "${PUBLIC_HEADER}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../include/middleware/services/radar/radar_service.h"
    PARENT_SCOPE)
