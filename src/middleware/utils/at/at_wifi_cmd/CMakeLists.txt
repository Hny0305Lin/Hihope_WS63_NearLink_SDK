#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "wifi_at")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/at/at_ccpriv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/at/at_mfg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/at/at_register.c
    ${CMAKE_CURRENT_SOURCE_DIR}/at/at_wifi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/at/at_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_cmd_register.c
)

if(${OS} STREQUAL "liteos")
list (APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/at/at_sendtest.c
)
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/at
    ${ROOT_DIR}/middleware/utils/at/at
    ${ROOT_DIR}/middleware/utils/at/at/include
    ${ROOT_DIR}/middleware/utils/at/at/src
    ${ROOT_DIR}/include/middleware/services/wifi
    ${ROOT_DIR}/protocol/wifi/source/host/inc/liteOS
    ${ROOT_DIR}/protocol/wifi/source/host/main/liteOS
    ${ROOT_DIR}/middleware/services/wifi_service/service
)

string(REPLACE "\${ROOT_DIR}" "${ROOT_DIR}" CONFIG_LWIP_COMPILE_PATH "${CONFIG_LWIP_COMPILE_PATH}")
if (EXISTS ${CONFIG_LWIP_COMPILE_PATH}/src/include)
    list(APPEND PRIVATE_HEADER ${CONFIG_LWIP_COMPILE_PATH}/src/include)
else()
    list(APPEND PRIVATE_HEADER ${ROOT_DIR}/open_source/lwip/lwip_v2.1.3/src/include)
endif()
if (EXISTS ${CONFIG_LWIP_COMPILE_PATH}/src/include/lwip/nettool)
    list(APPEND PRIVATE_HEADER ${CONFIG_LWIP_COMPILE_PATH}/src/include/lwip/nettool)
else()
    list(APPEND PRIVATE_HEADER ${ROOT_DIR}/open_source/lwip/lwip_v2.1.3/src/include/lwip/nettool)
endif()

set(PRIVATE_DEFINES
    LOSCFG_BASE_CORE_USE_MULTI_LIST
    CONFIG_WPS_SUPPORT
    _PRE_WLAN_FEATURE_SDP
)
# 小型化版本,裁掉wps
if(${SMALLER})
    list(REMOVE_ITEM PRIVATE_DEFINES
        CONFIG_WPS_SUPPORT
        _PRE_WLAN_FEATURE_SDP
    )
endif()

set(PUBLIC_DEFINES
    AT_COMMAND
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-unused-variable
    -Wno-char-subscripts
    -Wno-sign-compare
    -Wno-unused-function
    -Wno-unused-parameter
    -Wno-logical-op
    -Wno-incompatible-pointer-types
    -Wno-implicit-function-declaration
    -Wno-pointer-sign
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    true
)

build_component()