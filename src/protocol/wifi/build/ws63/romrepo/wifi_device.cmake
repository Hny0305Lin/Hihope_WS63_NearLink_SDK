#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

# Wi-Fi基础ROM组件
set(COMPONENT_NAME "wifi_driver_rom")
set(SOURCES
)
wifi_alg_compiler_entry(ws63_alg_wifi_rom OUT_SOURCES SOURCES OUT_INCLUDES Wi-Fi-ROM-PUBLIC_HEADER OUT_DEFINES Wi-Fi-PRIVATE_DEFINES
    WIFI_SOURCES ${SOURCES} WIFI_INCLUDES ${Wi-Fi-ROM-PUBLIC_HEADER} WIFI_DEFINES ${Wi-Fi-PRIVATE_DEFINES})
add_rom_base_config()
build_component()

install_sdk(${WIFI_DEVICE_DIR}/ "*.h")

set(COMPONENT_NAME "wifi_driver_dmac")
set(SOURCES
)
# ASIC和FPGA编译文件有差异
if(-DBOARD_ASIC IN_LIST CCFLAGS)
    list(APPEND SOURCES
    )
else()
    list(APPEND SOURCES
    )
endif()

add_base_config()
# 添加算法源码和头文件
wifi_alg_compiler_entry(ws63_alg_wifi_device_ram OUT_SOURCES SOURCES WIFI_SOURCES ${SOURCES})
build_component()


# FEATURE RADAR SENSOR ALG
set(COMPONENT_NAME "wifi_alg_radar_sensor_rom")
set(SOURCES
)
set(COMPONENT_PUBLIC_CCFLAGS ${Wi-Fi-COMPONENT_PUBLIC_CCFLAGS})
set(COMPONENT_CCFLAGS        ${Wi-Fi-COMPONENT_CCFLAGS})
set(WHOLE_LINK               ${Wi-Fi-WHOLE_LINK})
set(MAIN_COMPONENT           ${Wi-Fi-MAIN_COMPONENT})

set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/wifi/${TARGET_COMMAND})
build_component()

# WiFi ROM-DATA SYM
if(DEFINED ROM_SYM_PATH)
set(COMPONENT_NAME "wifi_rom_data")
set(SOURCES
    ${WIFI_DEVICE_DIR}/source/common/romable/wifi_rom_data.c
)
set(COMPONENT_PUBLIC_CCFLAGS ${Wi-Fi-COMPONENT_PUBLIC_CCFLAGS})
set(COMPONENT_CCFLAGS        ${Wi-Fi-COMPONENT_CCFLAGS})
set(WHOLE_LINK               ${Wi-Fi-WHOLE_LINK})
set(MAIN_COMPONENT           ${Wi-Fi-MAIN_COMPONENT})

set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/wifi/${TARGET_COMMAND})
build_component()
endif()
