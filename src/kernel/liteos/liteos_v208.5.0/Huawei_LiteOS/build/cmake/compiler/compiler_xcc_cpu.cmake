# Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.

if(CMAKE_TOOLCHAIN_FILE)
    message("CMAKE_TOOLCHAIN_FILE is defined ahead, skip auto-config compiler")
    return()
endif()

# Get libxcc paths.
set(XTENSA_ELF_DIR ${XTENSA_XTDEV_TOOLS_PATH}/install/builds/${VERSION_NUM}/${XTENSA_CORE}/xtensa-elf)
list(APPEND LITEOS_COMPILER_GCC_INCLUDE
    ${XTENSA_ELF_DIR}/include
    ${XTENSA_ELF_DIR}/arch/include
)
set(LITEOS_COMPILER_GCCLIB_PATH
    ${XTENSA_ELF_DIR}/arch/lib
    ${XTENSA_ELF_DIR}/lib
    ${XTENSA_ELF_DIR}/lib/xcc
)

# Check XCC lib path is avaiable for now.
FOREACH(CUR_DIR ${LITEOS_COMPILER_GCCLIB_PATH})
    if(NOT EXISTS ${CUR_DIR})
        message(FATAL_ERROR "xcc path not exist.")
    endif()
ENDFOREACH(CUR_DIR)
