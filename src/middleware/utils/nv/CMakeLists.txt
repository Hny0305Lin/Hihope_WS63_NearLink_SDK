#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "nv")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_storage_app/soc_nv_storage_handle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_storage_app/soc_nv_storage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_storage_lib/nv_key.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_storage_lib/nv_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_storage_lib/nv_nvregion.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_storage_lib/nv_page.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_storage_lib/nv_reset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_storage_lib/nv_rpc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_storage_lib/nv_store.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_storage_lib/nv_update.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_storage_lib/nv_upg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_storage_lib/nv_debug.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_storage_lib/nv_async_store.c
)

set(PUBLIC_HEADER
    ${CMAKE_SOURCE_DIR}/include/middleware/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_storage_lib
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_storage_app
)

set(PRIVATE_HEADER
    ${CMAKE_SOURCE_DIR}/drivers/drivers/driver/security/hash
    ${CMAKE_SOURCE_DIR}/middleware/chips/${chip}/nv/include
    ${CMAKE_SOURCE_DIR}/middleware/utils/update/inner_include
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
build_component()