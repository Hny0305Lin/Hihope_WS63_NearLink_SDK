#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(libc)

set(COMPONENT_NAME "common_boot")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_reset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_debug.c
)

if(${APPLICATION} STREQUAL "romboot")
set(SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_uart_auth.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_verify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_ymodem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_load.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_jump.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_crc16.c
)
endif()

if(${APPLICATION} STREQUAL "loaderboot")
set(SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_transfer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_cmd_loop.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_ymodem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_crc16.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_load.c
)

if(DEFINED CONFIG_LOADERBOOT_SUPPORT_EFUSE_BURN)
list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/boot_efuse_opt.c")
endif()

endif()

if(${APPLICATION} STREQUAL "flashboot")
set(SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_jump.c
)
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
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

if (${CHIP} STREQUAL "sw39" OR ${CHIP} STREQUAL "sw21")
set(COMPONENT_CCFLAGS
)
else()
set(COMPONENT_CCFLAGS
)
endif()

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

set(COMPONENT_NAME "flashboot_with_loadboot")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_transfer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_cmd_loop.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_ymodem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_crc16.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_load.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot_uart_auth.c
)

set(PUBLIC_HEADER
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