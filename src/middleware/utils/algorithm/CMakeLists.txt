#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "algorithm")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/crc/uapi_crc16.c
    ${CMAKE_CURRENT_SOURCE_DIR}/crc/uapi_crc32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sha256/sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sec_random/sec_random.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cstl/cstl_hash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cstl/cstl_rawlist.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cstl/cstl_public.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/crc
    ${CMAKE_CURRENT_SOURCE_DIR}/sec_random
    ${CMAKE_CURRENT_SOURCE_DIR}/cstl
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

if("CONFIG_DIAG_WITH_SECURE" IN_LIST DEFINES)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/srp/private/srp.c)
    list(APPEND PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/srp/public)
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
    -Wno-implicit-function-declaration
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

set(COMPONENT_NAME "algorithm_sha256")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/sha256/sha256.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/
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