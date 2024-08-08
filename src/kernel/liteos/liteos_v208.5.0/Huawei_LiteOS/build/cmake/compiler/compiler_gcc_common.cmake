# Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.

if(CMAKE_TOOLCHAIN_FILE)
    message("CMAKE_TOOLCHAIN_FILE is defined ahead, skip auto-config compiler")
    return()
endif()


if(NOT LOSCFG_COMPILER_THIRDPARTY_SUPPORT)
    # If thirdparty compiler is not enabled, then empty $(CROSS_COMPILE)
    # and use system chosen compiler.
    # Attention: this override only affects the export environment. While
    # 'make CROSS_COMPILE=' style's $(CROSS_COMPILE) cannot be overrided.
    # In this case thirdparty support is manually enabled which not controlled
    # by this config $(LOSCFG_COMPILER_THIRDPARTY_SUPPORT)
    set(CROSS_COMPILE)
endif()

# CROSS_COMPILE is empty
if(NOT CROSS_COMPILE)
    if(LOSCFG_COMPILER_HIMIX_32)
        set(CROSS_COMPILE arm-himix410-linux-)
        set(COMPILE_ALIAS arm-linux-musleabi)
    elseif(LOSCFG_COMPILER_HIMIX210_64)
        set(CROSS_COMPILE aarch64-himix210-linux-)
        set(COMPILE_ALIAS aarch64-linux-musl)
    elseif(LOSCFG_COMPILER_HCC_64)
        set(CROSS_COMPILE aarch64-target-linux-gnu-)
    elseif(LOSCFG_COMPILER_GCC_64)
        set(CROSS_COMPILE aarch64-linux-gnu-)
    elseif(LOSCFG_COMPILER_ARM_NONE_EABI)
        set(CROSS_COMPILE arm-none-eabi-)
    elseif(LOSCFG_COMPILER_RISCV_GCC_MUSL)
        set(CROSS_COMPILE riscv32-linux-musl-)
    elseif(LOSCFG_COMPILER_RISCV_GCC_UNKNOWN)
        set(CROSS_COMPILE riscv32-unknown-elf-)
    endif()
endif()

string(REGEX REPLACE -$ "" COMPILE_NAME "${CROSS_COMPILE}")
if(NOT COMPILE_ALIAS)
    set(COMPILE_ALIAS ${COMPILE_NAME})
endif()

set(CC       ${CROSS_COMPILE}gcc)
set(GPP      ${CROSS_COMPILE}g++)
set(AS       ${CROSS_COMPILE}as)
set(AR       ${CROSS_COMPILE}ar)
set(LD       ${CROSS_COMPILE}ld)
set(OBJCOPY  ${CROSS_COMPILE}objcopy)
set(OBJDUMP  ${CROSS_COMPILE}objdump)
set(READELF  ${CROSS_COMPILE}readelf)
set(SIZE     ${CROSS_COMPILE}size)
set(NM       ${CROSS_COMPILE}nm)

set(CMAKE_AR             ${CROSS_COMPILE}ar)
set(CMAKE_C_COMPILER     ${CROSS_COMPILE}gcc)
set(CMAKE_ASM_COMPILER   ${CROSS_COMPILE}gcc)
set(CMAKE_OBJCOPY        ${CROSS_COMPILE}objcopy)
set(CMAKE_OBJDUMP        ${CROSS_COMPILE}objdump)
set(CMAKE_READELF        ${CROSS_COMPILE}readelf)
set(CMAKE_CXX_COMPILER   ${CROSS_COMPILE}g++)
set(CMAKE_LINKER         ${CROSS_COMPILE}ld)
set(STRIP                ${CROSS_COMPILE}STRIP)

execute_process(
    COMMAND bash -c "${CC} -dumpversion"
    RESULT_VARIABLE GCC_VERISON_RET
    OUTPUT_VARIABLE GCC_VERISON_OUT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
if(GCC_VERISON_RET)
    message(FATAL_ERROR "Get gcc version failed.")
endif()
set(VERSION_NUM ${GCC_VERISON_OUT} CACHE STRING "GCC VERSION" FORCE)


# Check if input compiler is availible
execute_process(
    COMMAND bash -c "which ${CC}"
    RESULT_VARIABLE IS_CC_NON_AVAIL
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
if(IS_CC_NON_AVAIL)
    message(FATAL_ERROR "compiler ${COMPILE_NAME} is not in the ENV")
endif()

# Check objdump
execute_process(
    COMMAND bash -c "which ${OBJDUMP}"
    RESULT_VARIABLE OBJDUMP_IS_EXISTED
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Check size
execute_process(
    COMMAND bash -c "which ${SIZE}"
    RESULT_VARIABLE SIZE_IS_EXISTED
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Get compiler path
execute_process(
    COMMAND bash -c "sh ${LITEOSTOPDIR}/build/scripts/get_compiler_path.sh ${CC}"
    RESULT_VARIABLE GCC_PATH_RET
    OUTPUT_VARIABLE GCC_PATH_OUT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY ${LITEOSTOPDIR}
)
if(GCC_PATH_RET)
    message(FATAL_ERROR "Get gcc path failed.")
endif()
set(LITEOS_COMPILER_PATH ${GCC_PATH_OUT} CACHE STRING "GCC PATH" FORCE)


# Generally 32 bit and 64 bit compilers has different libgcc paths.
# FOR some history resaons.
# 32 bit : ${LITEOS_GCCLIB} can be defined as CPU level lib path,
#          if not define, use the common libs instead.
# 64 bit : Default use lib64 path, if not exist, use lib instead.
set(GCC_GCCLIB_PATH)
set(GCC_GXXLIB_PATH)

set(GCCLIB_PATH_32 ${LITEOS_COMPILER_PATH}/lib/gcc/${COMPILE_NAME}/${VERSION_NUM})
set(GCCLIB_PATH_64 ${LITEOS_COMPILER_PATH}/lib64/gcc/${COMPILE_NAME}/${VERSION_NUM})

set(CXXLIB_PATH_32 ${LITEOS_COMPILER_PATH}/${COMPILE_NAME}/lib)
set(CXXLIB_PATH_64 ${LITEOS_COMPILER_PATH}/${COMPILE_NAME}/lib64)

if(NOT "${CROSS_COMPILE}" MATCHES "64")  # 32-bit compiler
    set(GCC_GCCLIB_PATH ${GCCLIB_PATH_32})
    set(GCC_GXXLIB_PATH ${CXXLIB_PATH_32})
else()                                   # 64-bit compiler
    file(GLOB LITEOS_SRC_LIST_TEMP ${GCCLIB_PATH_64})
    if(LITEOS_SRC_LIST_TEMP)
        set(GCC_GCCLIB_PATH ${GCCLIB_PATH_64})
    else()
        set(GCC_GCCLIB_PATH ${GCCLIB_PATH_32})
    endif()
    set(GCC_GXXLIB_PATH ${CXXLIB_PATH_64})
endif()

set(LITEOS_COMPILER_GCC_INCLUDE ${GCC_GCCLIB_PATH}/include)

set(LITEOS_COMPILER_CXX_PATH ${LITEOS_COMPILER_PATH}/${COMPILE_NAME}/include)
list(APPEND LITEOS_CXXINCLUDE
    ${LITEOS_COMPILER_CXX_PATH}/c++/${VERSION_NUM}
    ${LITEOS_COMPILER_CXX_PATH}/c++/${VERSION_NUM}/ext
    ${LITEOS_COMPILER_CXX_PATH}/c++/${VERSION_NUM}/backward
    ${LITEOS_COMPILER_CXX_PATH}/c++/${VERSION_NUM}/${COMPILE_ALIAS}
)

# THIS feature whether support or not depands on if its C++ libs
# are compiled with liteos which should be controlled with KCONFIGS.
if(LOSCFG_KERNEL_CPPSUPPORT)
    list(APPEND LITEOS_CMACRO LOSCFG_KERNEL_CPP_EXCEPTIONS_SUPPORT)
    list(APPEND LITEOS_CXXMACRO LOSCFG_KERNEL_CPP_EXCEPTIONS_SUPPORT)
    # The C++ library in the compiler contains undefined symbols because the LiteOS libc is used.
    # For details about the symbols, see gcc.
    if(LOSCFG_COMPILER_ARM_NONE_EABI)
        list(APPEND LITEOS_CMACRO
            _U=1
            _L=2
            _N=4
            _S=8
            _P=16
            _C=32
            _X=64
            _B=128
        )
        list(APPEND LITEOS_CXXMACRO
            _U=1
            _L=2
            _N=4
            _S=8
            _P=16
            _C=32
            _X=64
            _B=128
        )
    endif()
endif()

# Check include path is avaiable for now.
# CXX paths to be added. Since LiteOS support for C++ is based on whether those libs
# are compiled for LiteOS. C++ support needs to configured with Kconfigs.
if(NOT EXISTS ${LITEOS_COMPILER_GCC_INCLUDE})
    message(FATAL_ERROR "gcc path not exist.")
endif()

