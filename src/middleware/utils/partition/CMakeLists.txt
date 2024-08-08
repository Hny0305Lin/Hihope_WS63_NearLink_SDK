#===============================================================================
# @brief    cmake make file
# Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd.
#===============================================================================
set(COMPONENT_NAME "partition")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/partition.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_SOURCE_DIR}/middleware/chips/${CHIP}/partition
    ${CMAKE_CURRENT_SOURCE_DIR}
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
