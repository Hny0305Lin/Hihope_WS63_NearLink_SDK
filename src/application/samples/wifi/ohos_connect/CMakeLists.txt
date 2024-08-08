#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
if (NOT DEFINES MATCHES "CONFIG_SUPPORT_HILINK_INDIE_UPGRADE")
set(COMPONENT_NAME "hilinkdevicesdk")
set(LIBS ${ROOT_DIR}/application/samples/wifi/libhilink/lib${COMPONENT_NAME}.a)
set(WHOLE_LINK
    true
)
build_component()

set(COMPONENT_NAME "hilinkota")
set(LIBS ${ROOT_DIR}/application/samples/wifi/libhilink/lib${COMPONENT_NAME}.a)
set(WHOLE_LINK
    true
)
build_component()

set(COMPONENT_NAME "hilinkbtsdk")
set(LIBS ${ROOT_DIR}/application/samples/wifi/libhilink/lib${COMPONENT_NAME}.a)
set(WHOLE_LINK
    true
)
build_component()
endif()

set(COMPONENT_NAME "hilink")

set(CMAKE_HILINK_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/product/hilink_device.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_kv_adapter.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_open_ota_adapter.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_sal_base64.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_sal_md.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_socket_adapter.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_str_adapter.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_time_adapter.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_mem_adapter.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_open_ota_mcu_adapter.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_sal_drbg.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_sal_mpi.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_softap_adapter.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_sys_adapter.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_tls_client.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_network_adapter.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_sal_aes.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_sal_kdf.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_sal_rsa.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_stdio_adapter.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_thread_adapter.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/hilink_ble_adapter.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/entry/hilink_indie_upgrade_main.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/entry/hilink_wifi_main.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/entry/hilink_ble_main.c
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/entry/hilink_entry.c
)

set(PUBLIC_HEADER
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/include/
)

set(PRIVATE_HEADER
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/product/
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/include/
    ${CMAKE_HILINK_SOURCE_DIR}/hilink_adapt/adapter/include/
    ${ROOT_DIR}/protocol/wifi/source/host/inc/liteOS
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/src/internal_include
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error=logical-op
    -Wno-error=sign-compare
    -Wno-error=jump-misses-init
    -Wno-sign-compare
    -Wno-jump-misses-init
    -Wno-error=unused-parameter
    -Wno-unused-parameter
    -Wno-unused-but-set-variable
    -Wno-error=unused-variable
)

set(PRIVATE_DEFINES
    MBEDTLS_SHA256_ALT
)

set(PUBLIC_DEFINES
)
set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/wifi/${TARGET_COMMAND})

build_component()