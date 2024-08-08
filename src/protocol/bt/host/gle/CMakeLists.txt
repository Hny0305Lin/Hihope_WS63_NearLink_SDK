#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(MODULE_NAME "bth")
if((${CHIP} MATCHES "bs25") AND (${CORE} MATCHES "bt_core"))
set(MODULE_NAME "bt")
endif()

if("gle_app" IN_LIST TARGET_COMPONENT)
    include(gle_app.cmake)
endif()
if("bth_gle" IN_LIST TARGET_COMPONENT)
    include(bth_gle.cmake)
endif()
if("gle_host" IN_LIST TARGET_COMPONENT)
    include(gle_host.cmake)
endif()
if(${CHIP} MATCHES "GLE_LINUX_CONFIG")
    include(gle_linux.cmake)
endif()
