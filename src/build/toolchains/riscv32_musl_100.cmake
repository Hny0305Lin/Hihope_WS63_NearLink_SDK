#===============================================================================
# @brief    cmake toolchains
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

function(find_env_exe var full_path exe)
    if(USE_LOCAL_COMPILER)
        file(TO_CMAKE_PATH ${full_path} full_path)
        set(${var} ${full_path}/${exe} CACHE PATH "" FORCE)
    else()
        find_program(${var} ${exe})
    endif()
endfunction(find_env_exe)

set(COMPILER_ROOT ${PROJECT_SOURCE_DIR}/tools/bin/compiler/riscv/cc_riscv32_musl_100/cc_riscv32_musl)
set(COMPILER_ROOT_WIN ${PROJECT_SOURCE_DIR}/tools/bin/compiler/riscv/cc_riscv32_musl_100/cc_riscv32_musl_win)
set(OBJDUMP_PATH ${COMPILER_ROOT}/bin)

if(EXISTS ${COMPILER_ROOT})
    set(USE_LOCAL_COMPILER TRUE)
else()
    set(USE_LOCAL_COMPILER FALSE)
endif()

if(${BUILD_PLATFORM} MATCHES "linux")
    MESSAGE(STATUS "BUILD_PLATFORM: " ${BUILD_PLATFORM})
    find_env_exe(CMAKE_C_COMPILER           ${COMPILER_ROOT}/bin         riscv32-linux-musl-gcc)
    find_env_exe(CMAKE_ASM_COMPILER         ${COMPILER_ROOT}/bin         riscv32-linux-musl-gcc)
    find_env_exe(CMAKE_CXX_COMPILER         ${COMPILER_ROOT}/bin         riscv32-linux-musl-g++)
    find_env_exe(CMAKE_LINKER               ${COMPILER_ROOT}/bin         riscv32-linux-musl-ld)
    find_env_exe(CMAKE_AR                   ${COMPILER_ROOT}/bin         riscv32-linux-musl-ar)
    find_env_exe(CMAKE_C_COMPILER_AR        ${COMPILER_ROOT}/bin         riscv32-linux-musl-ar)
    find_env_exe(CMAKE_ASM_COMPILER_AR      ${COMPILER_ROOT}/bin         riscv32-linux-musl-ar)

    find_env_exe(CMAKE_RANLIB               ${COMPILER_ROOT}/bin         riscv32-linux-musl-ranlib)
    find_env_exe(CMAKE_C_COMPILER_RANLIB    ${COMPILER_ROOT}/bin         riscv32-linux-musl-ranlib)
    find_env_exe(CMAKE_ASM_COMPILER_RANLIB  ${COMPILER_ROOT}/bin         riscv32-linux-musl-ranlib)

    find_env_exe(CMAKE_STRIP                ${COMPILER_ROOT}/bin         riscv32-linux-musl-strip)
    find_env_exe(CMAKE_NM                   ${COMPILER_ROOT}/bin         riscv32-linux-musl-nm)
    find_env_exe(CMAKE_OBJCOPY              ${COMPILER_ROOT}/bin         riscv32-linux-musl-objcopy)
    find_env_exe(CMAKE_OBJDUMP              ${COMPILER_ROOT}/bin         riscv32-linux-musl-objdump)
    find_env_exe(CMAKE_READELF              ${COMPILER_ROOT}/bin         riscv32-linux-musl-readelf)

elseif(${BUILD_PLATFORM} MATCHES "windows")
    MESSAGE(STATUS "BUILD_PLATFORM: " ${BUILD_PLATFORM})
    set(COMPILER_ROOT  ${COMPILER_ROOT_WIN})
    find_env_exe(CMAKE_C_COMPILER           ${COMPILER_ROOT}/bin         riscv32-linux-musl-gcc.exe)
    find_env_exe(CMAKE_ASM_COMPILER         ${COMPILER_ROOT}/bin         riscv32-linux-musl-gcc.exe)
    find_env_exe(CMAKE_CXX_COMPILER         ${COMPILER_ROOT}/bin         riscv32-linux-musl-g++.exe)
    find_env_exe(CMAKE_LINKER               ${COMPILER_ROOT}/bin         riscv32-linux-musl-ld.exe)
    find_env_exe(CMAKE_AR                   ${COMPILER_ROOT}/bin         riscv32-linux-musl-ar.exe)
    find_env_exe(CMAKE_C_COMPILER_AR        ${COMPILER_ROOT}/bin         riscv32-linux-musl-ar.exe)
    find_env_exe(CMAKE_ASM_COMPILER_AR      ${COMPILER_ROOT}/bin         riscv32-linux-musl-ar.exe)

    find_env_exe(CMAKE_RANLIB               ${COMPILER_ROOT}/bin         riscv32-linux-musl-ranlib.exe)
    find_env_exe(CMAKE_C_COMPILER_RANLIB    ${COMPILER_ROOT}/bin         riscv32-linux-musl-ranlib.exe)
    find_env_exe(CMAKE_ASM_COMPILER_RANLIB  ${COMPILER_ROOT}/bin         riscv32-linux-musl-ranlib.exe)

    find_env_exe(CMAKE_STRIP                ${COMPILER_ROOT}/bin         riscv32-linux-musl-strip.exe)
    find_env_exe(CMAKE_NM                   ${COMPILER_ROOT}/bin         riscv32-linux-musl-nm.exe)
    find_env_exe(CMAKE_OBJCOPY              ${COMPILER_ROOT}/bin         riscv32-linux-musl-objcopy.exe)
    find_env_exe(CMAKE_OBJDUMP              ${COMPILER_ROOT}/bin         riscv32-linux-musl-objdump.exe)
    find_env_exe(CMAKE_READELF              ${COMPILER_ROOT}/bin         riscv32-linux-musl-readelf.exe)
endif()

if(DEFINED CMAKE_C_COMPILER)
    STRING(REGEX REPLACE "(.+)/.+" "\\1" CMAKE_C_COMPILER_PATH ${CMAKE_C_COMPILER})
    set(LIB_C  ${CMAKE_C_COMPILER_PATH}/../lib/gcc/riscv32-linux-musl/7.3.0 CACHE PATH "" FORCE)
    set(LIB_GCC  ${CMAKE_C_COMPILER_PATH}/../sysroot/usr/lib CACHE PATH "" FORCE)
endif()

set(CMAKE_C_CREATE_STATIC_LIBRARY "<CMAKE_AR> rcD <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_ASM_CREATE_STATIC_LIBRARY "<CMAKE_AR> rcD <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CNAKE_CXX_CREATE_STATIC_LIBRARY "<CMAKE_AR> rcD <TARGET> <LINK_FLAGS> <OBJECTS>")
