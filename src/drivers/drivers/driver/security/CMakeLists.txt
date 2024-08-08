#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "sec_common")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/sec_common/sec_common.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/sec_common
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

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/sec_common" "*.h")
build_component()

set(COMPONENT_NAME "sec_rsa")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/sec_rsa/sec_rsav2.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/sec_rsa
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

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/sec_rsa" "*.h")
build_component()

set(COMPONENT_NAME "sec_sha")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/sec_sha/sec_sha.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/sec_sha
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

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/sec_sha" "*.h")

build_component()

set(COMPONENT_NAME "sec_trng")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/sec_trng/sec_trng_master/sec_trng.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/sec_trng/sec_trng_master
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

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/sec_trng" "*.h")
build_component()

add_subdirectory_if_exist(cipher)
add_subdirectory_if_exist(hash)
add_subdirectory_if_exist(km)
add_subdirectory_if_exist(pke)
add_subdirectory_if_exist(trng)
add_subdirectory_if_exist(sec_sha)
