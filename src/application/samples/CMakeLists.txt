#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "samples")

set(SOURCES
)

set(PUBLIC_HEADER
)

if(DEFINED CONFIG_ENABLE_BT_SAMPLE)
    add_subdirectory_if_exist(bt)
endif()
if(DEFINED CONFIG_ENABLE_PERIPHERAL_SAMPLE)
    add_subdirectory_if_exist(peripheral)
endif()
if(DEFINED CONFIG_ENABLE_WIFI_SAMPLE)
    add_subdirectory_if_exist(wifi)
endif()
if(DEFINED CONFIG_ENABLE_PRODUCTS_SAMPLE)
    add_subdirectory_if_exist(products)
endif()
if(DEFINED CONFIG_ENABLE_RADAR_SAMPLE)
    add_subdirectory_if_exist(radar)
endif()
if(DEFINED CONFIG_ENABLE_NFC_SAMPLE)
    add_subdirectory_if_exist(nfc)
endif()

add_subdirectory_if_exist(custom)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/peripheral" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/products" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi/sta_sample" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi/softap_sample" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi/ohos_connect" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi/hilink_indie_upgrade" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi/ble_wifi_cfg_sample" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/radar" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/nfc" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/bt/ble" "*")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/bt/sle" "*")

build_component()
