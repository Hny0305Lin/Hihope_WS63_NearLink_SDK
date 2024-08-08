#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_tcxo_rom")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_tcxo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_tcxo_v150.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_tcxo_v150_regs_op.c
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

install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/ "*.h")
