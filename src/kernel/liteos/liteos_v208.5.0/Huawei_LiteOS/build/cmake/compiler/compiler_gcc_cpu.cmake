# Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.

if(CMAKE_TOOLCHAIN_FILE)
    message("CMAKE_TOOLCHAIN_FILE is defined ahead, skip auto-config compiler")
    return()
endif()


# Generally 32 bit and 64 bit compilers has different libgcc paths.
# FOR some history resaons.
# 32 bit : ${LITEOS_GCCLIB} can be defined as CPU level lib path,
#          if not define, use the common libs instead.
# 64 bit : Default use lib64 path, if not exist, use lib instead.
set(GCC_USE_CPU_OPT n)

if(NOT "${CROSS_COMPILE}" MATCHES "64")  # 32-bit compiler
    file(GLOB LITEOS_SRC_LIST_TEMP ${GCC_GCCLIB_PATH}/${LITEOS_GCCLIB})
    if(LITEOS_SRC_LIST_TEMP)         # LITEOS_SRC_LIST_TEMP is not null
        set(GCC_USE_CPU_OPT y)
    endif()
endif()

if(GCC_USE_CPU_OPT)
    set(LITEOS_COMPILER_GCCLIB_PATH ${GCC_GCCLIB_PATH}/${LITEOS_GCCLIB})
    set(LITEOS_COMPILER_CXXLIB_PATH ${GCC_GXXLIB_PATH}/${LITEOS_GCCLIB})
else()
    set(LITEOS_COMPILER_GCCLIB_PATH ${GCC_GCCLIB_PATH})
    set(LITEOS_COMPILER_CXXLIB_PATH ${GCC_GXXLIB_PATH})
endif()


# Check GCC lib path is avaiable for now.
# CXX paths to be added. Since LiteOS support for C++ is based on whether those libs
# are compiled for LiteOS. C++ support needs to configured with Kconfigs.
if(NOT EXISTS ${LITEOS_COMPILER_GCCLIB_PATH})
    message(FATAL_ERROR "gcc path not exist.")
endif()
