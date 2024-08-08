#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_pmp")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_pmp.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

if(DEFINED CONFIG_PMP_USING_RISCV_31)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/v31/hal_pmp_riscv31.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/v31/hal_pmp_riscv31_regs_op.c)
    list(APPEND PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/v31)
endif()

if(DEFINED CONFIG_PMP_USING_RISCV_70)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/v70/hal_pmp_riscv70.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/v70/hal_pmp_riscv70_regs_op.c)
    list(APPEND PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/v70)
endif()

if(NOT DEFINED CONFIG_PMP_USING_RISCV_70 AND NOT DEFINED CONFIG_PMP_USING_RISCV_31)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/v70/hal_pmp_riscv70.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/v70/hal_pmp_riscv70_regs_op.c)
    list(APPEND PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/v70)
endif()

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