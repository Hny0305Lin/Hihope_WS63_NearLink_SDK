#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_sio")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_sio.c
)

if(DEFINED CONFIG_SIO_USING_V150)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hal_sio_v150/hal_sio_v150.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hal_sio_v150/hal_sio_v150_regs_op.c)
endif()

if(DEFINED CONFIG_SIO_USING_V151)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hal_sio_v151/hal_sio_v151.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hal_sio_v151/hal_sio_v151_regs_op.c)
endif()

if(DEFINED CONFIG_DRIVER_SUPPORT_I2S)
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/hal_sio_v151" "*.c")
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/hal_sio_v150" "*.c")
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_sio_v150
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_sio_v151
    ${CMAKE_CURRENT_SOURCE_DIR}
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
