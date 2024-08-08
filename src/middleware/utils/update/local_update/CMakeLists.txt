#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(MODULE_NAME "pf")
set(AUTO_DEF_FILE_ID TRUE)

set(COMPONENT_NAME "update_local")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/upg_encry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/upg_lzmadec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/upg_patch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/upg_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/upg_resource.c
    ${CMAKE_CURRENT_SOURCE_DIR}/upg_upgrade.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_SOURCE_DIR}/middleware/utils/update/inner_include
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
