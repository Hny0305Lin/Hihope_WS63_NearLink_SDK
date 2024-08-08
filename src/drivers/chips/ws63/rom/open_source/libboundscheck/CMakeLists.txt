#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "libboundscheck_rom")

if(${CHIP} MATCHES "socmn1")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/memcpy_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/memmove_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/memset_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/securecutil.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/secureprintoutput_a.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/snprintf_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sprintf_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/strcat_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/strcpy_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/strncat_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/strncpy_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vsnprintf_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vsprintf_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/strtok_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sscanf_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vsscanf_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/secureinput_a.c
    )
else()
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/memcpy_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/memmove_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/memset_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/securecutil.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/secureprintoutput_a.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/snprintf_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sprintf_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/strcat_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/strcpy_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/strncat_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/strncpy_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vsnprintf_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vsprintf_s.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/strtok_s.c
    )
endif()



set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
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
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/src/ "*.inl")
