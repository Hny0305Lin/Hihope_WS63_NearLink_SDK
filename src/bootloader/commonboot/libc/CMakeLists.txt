#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "common_boot_libc")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/string/memset.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/string/memcpy.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/string/memcmp.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/string/strcmp.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/string/strlen.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/string/memmove.c
)

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

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()