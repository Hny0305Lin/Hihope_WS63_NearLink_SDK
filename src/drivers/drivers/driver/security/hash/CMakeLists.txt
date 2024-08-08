#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hash")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_hash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pbkdf2.c
)

set(PRIVATE_HEADER
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
true
)
build_component()
