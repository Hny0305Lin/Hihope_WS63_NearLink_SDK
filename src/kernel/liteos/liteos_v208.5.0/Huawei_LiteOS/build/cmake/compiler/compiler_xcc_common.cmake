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
    set(CROSS_COMPILE xt-)
endif()

set(CC       ${CROSS_COMPILE}xcc)
set(GPP      ${CROSS_COMPILE}xc++)
set(AS       ${CROSS_COMPILE}as)
set(AR       ${CROSS_COMPILE}ar)
set(LD       ${CROSS_COMPILE}ld)
set(OBJCOPY  ${CROSS_COMPILE}objcopy)
set(OBJDUMP  ${CROSS_COMPILE}objdump)
set(READELF  ${CROSS_COMPILE}readelf)
set(SIZE     ${CROSS_COMPILE}size)
set(NM       ${CROSS_COMPILE}nm)

set(CMAKE_AR             ${CROSS_COMPILE}ar)
set(CMAKE_C_COMPILER     ${CROSS_COMPILE}xcc)
set(CMAKE_ASM_COMPILER   ${CROSS_COMPILE}xcc)
set(CMAKE_OBJCOPY        ${CROSS_COMPILE}objcopy)
set(CMAKE_OBJDUMP        ${CROSS_COMPILE}objdump)
set(CMAKE_READELF        ${CROSS_COMPILE}readelf)
set(CMAKE_CXX_COMPILER   ${CROSS_COMPILE}xc++)
set(CMAKE_LINKER         ${CROSS_COMPILE}ld)
set(STRIP                ${CROSS_COMPILE}STRIP)

if (LOSCFG_ARCH_XTENSA_XEA3)
    if(LOSCFG_XTENSA_VISIONQ6_V3)
        set(VERSION_NUM RH-2018.7-linux CACHE STRING "XCC VERSION" FORCE)
    else(LOSCFG_XTENSA_VISIONQ6_V2)
        set(VERSION_NUM RH-2017.4-linux CACHE STRING "XCC VERSION" FORCE)
    endif()
elseif(LOSCFG_ARCH_XTENSA_XEA2)
    set(VERSION_NUM RG-2017.5-linux CACHE STRING "XCC VERSION" FORCE)
endif()

# xtensa use compiler libc impl
list(APPEND LITEOS_DEP_LIBS_EXT c gloss hal)

# Check if input compiler is availible
execute_process(
    COMMAND bash -c "which ${CC}"
    RESULT_VARIABLE IS_CC_NON_AVAIL
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
if(IS_CC_NON_AVAIL)
    message(FATAL_ERROR "compiler ${COMPILE_NAME} is not in the ENV")
endif()

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

# Get LITEOS_COMPILER_GCC_INCLUDE
set(XTENSA_XTDEV_TOOLS_PATH ${LITEOS_COMPILER_PATH}/../../../..)
set(XTENSA_TOOL_DIR ${XTENSA_XTDEV_TOOLS_PATH}/install/tools/${VERSION_NUM}/XtensaTools)
set(LITEOS_COMPILER_GCC_INCLUDE
    ${XTENSA_TOOL_DIR}/xtensa-elf/include
    ${XTENSA_TOOL_DIR}/lib/xcc/include
)

# Check include path is avaiable for now.
FOREACH(CUR_DIR ${LITEOS_COMPILER_GCC_INCLUDE})
    if(NOT EXISTS ${CUR_DIR})
        message(FATAL_ERROR "xcc path not exist.")
    endif()
ENDFOREACH(CUR_DIR)


