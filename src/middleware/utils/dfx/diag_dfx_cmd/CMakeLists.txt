#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

add_subdirectory_if_exist(diag_audio_proc)
add_subdirectory_if_exist(diag_audio_dump)
add_subdirectory_if_exist(diag_audio_probe)
add_subdirectory_if_exist(diag_nv)
add_subdirectory_if_exist(diag_update)
add_subdirectory_if_exist(diag_charger)
add_subdirectory_if_exist(diag_logfile)
add_subdirectory_if_exist(diag_gpu_proc)
add_subdirectory_if_exist(diag_audio_sample_data)

set(COMPONENT_NAME "diag_dfx_cmd")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_dfx_cmd_init.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

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

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/" "*.h")

build_component()
