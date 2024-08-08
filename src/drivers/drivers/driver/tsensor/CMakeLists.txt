#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "tsensor")

if(DEFINED CONFIG_DRIVER_SUPPORT_TSENSOR)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tsensor.c
)
else()
set(SOURCES
)
endif()

if(DEFINED CONFIG_DRIVER_SUPPORT_TSENSOR)
set(PUBLIC_HEADER
)
else()
set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/bt_core
)
endif()

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
