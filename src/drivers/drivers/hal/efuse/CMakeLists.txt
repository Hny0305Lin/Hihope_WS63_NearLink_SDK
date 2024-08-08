#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_efuse_v100")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_efuse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_efuse_v100.c
    ${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_efuse_v100_reg_op.c
)

set(PRIVATE_HEADER
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/v100
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_V100_EFUSE
)

set(COMPONENT_CCFLAGS
)
set(WHOLE_LINK
true
)
build_component()

#===============================================================================
set(COMPONENT_NAME "hal_efuse_v120")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_efuse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/v120/hal_efuse_v120.c
    ${CMAKE_CURRENT_SOURCE_DIR}/v120/hal_efuse_v120_reg_op.c
)

set(PRIVATE_HEADER
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/v120
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_V120_EFUSE
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

build_component()

#===============================================================================
set(COMPONENT_NAME "hal_efuse_v151")

if(DEFINED CONFIG_EFUSE_USING_V151)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_efuse.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_efuse_v151.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_efuse_v151_reg_op.c")
endif()

set(PRIVATE_HEADER
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/v151
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

#===============================================================================
set(COMPONENT_NAME "hal_efuse_v130")

if(DEFINED CONFIG_EFUSE_USING_V151)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_efuse.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v130/hal_efuse_v130.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v130/hal_efuse_v130_reg_op.c")
endif()

set(PRIVATE_HEADER
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/v130
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

