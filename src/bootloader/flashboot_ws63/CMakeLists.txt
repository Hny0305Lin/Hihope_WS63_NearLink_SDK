#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "flashboot_common")

if("flashboot" IN_LIST BIN_NAME)
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/startup/main.c
        ${CMAKE_CURRENT_SOURCE_DIR}/startup/main.c
        ${CMAKE_CURRENT_SOURCE_DIR}/startup/riscv_init.S
        ${CMAKE_CURRENT_SOURCE_DIR}/../commonboot/src/secure_verify_boot.c
    )

    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../commonboot/include
    )
elseif ("ssb" IN_LIST BIN_NAME)
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ssb/main.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ssb/riscv_init.S
        ${CMAKE_CURRENT_SOURCE_DIR}/../commonboot/src/secure_verify_boot.c
    )

    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/../commonboot/include
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