#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "dfx_port_ws63")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/dfx_channel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dfx_system_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_adapt_cmd_mem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/last_dump_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_data_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_adapt_sdt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_adapt_psd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_sample_data.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
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

set(LOG_DEF
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_bt.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_dsp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_pf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_wifi.h
)

build_component()
