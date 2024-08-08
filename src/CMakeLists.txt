#===============================================================================
# @brief    cmake entry
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
cmake_minimum_required(VERSION 3.14.1)
set(CMAKE_SYSTEM_NAME "Generic")

if(DEFINED ENV{build_ws63_sdk_open})
    set(build_lib true)
else()
    set(build_lib false)
endif()

if(NOT DEFINED CHIP)
    message(FATAL_ERROR "Chip is not defined ")
endif()

set(Python3_EXECUTABLE ${PY_PATH})
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif(CCACHE_FOUND)

# default hso module_name & AUTO_DEF_FILE_ID & AUTO_DEF_MODULE_ID, redefine these var in sub cmake file will
# override these value just in sub directory but not others
set(MODULE_NAME "pf")
set(AUTO_DEF_FILE_ID TRUE)
set(AUTO_DEF_MODULE_ID TRUE)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
get_filename_component(ROOT_DIR  "${PROJECT_SOURCE_DIR}" ABSOLUTE)
set(CMAKE_DIR "${ROOT_DIR}/build/cmake")
set(BIN_DIR   "${ROOT_DIR}/interim_binary")

include("${CMAKE_DIR}/build_function.cmake")
include("${CMAKE_DIR}/global_variable.cmake")
include("${CMAKE_DIR}/build_script.cmake")
include("${CMAKE_DIR}/build_command.cmake")
include("${CMAKE_DIR}/build_hso_database.cmake")
include("${CMAKE_DIR}/build_component.cmake")
include("${CMAKE_DIR}/build_sdk.cmake")

include_directories(${ROOT_DIR}/kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/targets/ws63/include)

if(EXISTS "${ROOT_DIR}/kernel/liteos/liteos_v208.5.0/${TARGET_COMMAND}")
    set(build_ws63_sdk_open true)
endif ()

if(${BUILD_TYPE} STREQUAL "UT")
    KCONFIG_GET_PARAMS("${ROOT_DIR}/build/menuconfig/test/platform/ut.config")
    include("${CMAKE_DIR}/build_ut.cmake")
    return()
endif()

if(${BUILD_TYPE} STREQUAL "FUZZ")
    KCONFIG_GET_PARAMS("${ROOT_DIR}/build/menuconfig/test/platform/ut.config")
    include("${CMAKE_DIR}/build_fuzz.cmake")
    return()
endif()

project(${CHIP}_CFBB C ASM CXX)
set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES "")

if(EXISTS ${CMAKE_BINARY_DIR}/auto_gen_libfunc.lds)
    file(REMOVE ${CMAKE_BINARY_DIR}/auto_gen_libfunc.lds)
endif()

set(TARGET_COMPONENT "${RAM_COMPONENT}" "${ROM_COMPONENT}")
set(TARGET_NAME ${BIN_NAME})
file(WRITE ${PROJECT_BINARY_DIR}/temp/__null___.c "int __null___(void) {return 0;}")
add_executable(${BIN_NAME} ${PROJECT_BINARY_DIR}/temp/__null___.c)
set_target_properties(${BIN_NAME} PROPERTIES RUNTIME_OUTPUT_NAME ${BIN_NAME}.elf)
target_compile_options(${BIN_NAME} PRIVATE "${CCFLAGS}")

include("${CMAKE_DIR}/build_rom_callback.cmake")
if(${BUILD_ROM_CALLBACK})
    build_rom_callback()
endif()

set(KCONFIG_PATH "${ROOT_DIR}/build/config/target_config/${CHIP}/menuconfig/${CORE}/${BUILD_TARGET_NAME}.config")
if(EXISTS ${KCONFIG_PATH})
    KCONFIG_GET_PARAMS(${KCONFIG_PATH})
    set(USE_KCONFIG True)
endif()

add_subdirectory_if_exist(application)
add_subdirectory_if_exist(bt)
add_subdirectory_if_exist(bootloader)
add_subdirectory_if_exist(kernel)
add_subdirectory_if_exist(drivers)
add_subdirectory_if_exist(middleware)
add_subdirectory_if_exist(open_source)
add_subdirectory_if_exist(protocol)
add_subdirectory_if_exist(test)
add_subdirectory_if_exist(include)
add_subdirectory_if_exist(vendor)

include("${CMAKE_DIR}/open_source.cmake")
include("${CMAKE_DIR}/build_linker.cmake")

if (NOT DEFINED ROM_COMPONENT)
add_custom_target(GENERAT_BIN ALL
    COMMAND ${CMAKE_OBJCOPY} --gap-fill 0xFF -O binary -R .fls_loader_ram -R .logstr -R .ARM -R .ARM ${BIN_NAME}.elf ${BIN_NAME}.bin
    COMMENT "post_build:gen bin file"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS ${TARGET_NAME}
)
else()
add_custom_target(GENERAT_BIN ALL
    COMMAND ${CMAKE_OBJCOPY} --gap-fill 0xFF -O binary -R .logstr -R .ARM.exidx -R .ARM.extab -R .*_romtext ${TARGET_NAME}.elf ${TARGET_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} --gap-fill 0xFF -O binary -j .*_romtext ${TARGET_NAME}.elf ${TARGET_NAME}_rom.bin
    COMMENT "post_build:gen rom and ram bin file"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS ${TARGET_NAME}
)
endif()

if(DEFINED CONFIG_SFC_SUPPORT_RWE_INDEPENDENT)
add_custom_target(GENERAT_FLASH_DRIVER_BIN ALL
    COMMAND ${CMAKE_OBJCOPY} -O srec --srec-len=0x20 --srec-forceS3 -S -j .fls_loader_ram ${BIN_NAME}.elf BOOTLOADERFlsDrv.signed.s19
    COMMENT "post_build:gen flash driver bin file"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS ${TARGET_NAME}
)
endif()

include("${CMAKE_DIR}/build_ssb.cmake")
include("${CMAKE_DIR}/build_elf_info.cmake")
include("${CMAKE_DIR}/build_sign.cmake")
include("${CMAKE_DIR}/build_nv_bin.cmake")
include("${CMAKE_DIR}/build_partition_bin.cmake")
include("${CMAKE_DIR}/build_boot_bin_cp.cmake")

# create_hso_db()
generate_project_file()
