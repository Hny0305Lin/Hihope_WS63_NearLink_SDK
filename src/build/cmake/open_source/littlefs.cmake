#===============================================================================
# @brief mqtt cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(CMAKE_LITTLEFS_SOURCE_DIR ${CMAKE_DIR}/../../open_source/littlefs)

set(COMPONENT_NAME "little_fs")

set(CMAKE_LITTLEFSV250_SOURCE_DIR
    ${CMAKE_LITTLEFS_SOURCE_DIR}/v2.5.0/)

set(SOURCES
    ${CMAKE_LITTLEFSV250_SOURCE_DIR}/lfs.c
    ${CMAKE_LITTLEFSV250_SOURCE_DIR}/lfs_util.c

)

set(PUBLIC_HEADER
    ${CMAKE_LITTLEFSV250_SOURCE_DIR}/
)

set(PRIVATE_HEADER
    ${CMAKE_LITTLEFSV250_SOURCE_DIR}/
)

set(PRIVATE_DEFINES
    LFS_NO_ERROR
    LFS_NO_WARN
    LFS_NO_DEBUG
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
#    -Wno-missing-prototypes
#    -Wno-unused-parameter
#    -Wno-strict-prototypes
#    -Wno-implicit-function-declaration
#    -Wno-missing-declarations
#    -Wno-sign-compare
#    -Wno-int-conversion
#    -Wno-unused-label
    -Wno-unused-function
#    -Wno-error=logical-op
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

if(${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
    build_component()
endif()