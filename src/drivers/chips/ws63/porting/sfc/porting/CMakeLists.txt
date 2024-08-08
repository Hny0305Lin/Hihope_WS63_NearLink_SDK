#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "sfc_port_ws63")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/sfc_porting.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sfc_protect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sfc_protect_gd25q32.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../
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

build_component()