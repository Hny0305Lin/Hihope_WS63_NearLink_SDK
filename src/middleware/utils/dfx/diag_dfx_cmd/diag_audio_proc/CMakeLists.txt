#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "audio_proc")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/zdiag_audio_proc.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_HEADER
    ${CMAKE_SOURCE_DIR}/drivers/drivers/driver/audio/source/include
    ${CMAKE_SOURCE_DIR}/drivers/drivers/driver/audio/source/drv/include
    ${CMAKE_SOURCE_DIR}/drivers/drivers/driver/audio/source/core/common/include
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
