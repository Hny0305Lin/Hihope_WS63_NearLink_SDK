#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

set(BUILT_LDS ${CMAKE_BINARY_DIR}/linker.lds)

if (DEFINED BUILD_LEVEL AND NOT ${BUILD_LEVEL} STREQUAL "release")
    set(EXTRA_DUMP_OPT -S -l)
endif()

if (DEFINED NHSO AND "${NHSO}" STREQUAL "True")
    set(GEN_PARSE_TOOL False)
endif()

if((NOT DEFINED DEBUG_FILES OR "lst" IN_LIST DEBUG_FILES OR "mem" IN_LIST DEBUG_FILES) AND NOT ${ROM_CHECK})
    add_custom_target(GENERAT_LST ALL
        COMMAND ${CMAKE_OBJDUMP} -x ${EXTRA_DUMP_OPT} ${BIN_NAME}.elf > ${BIN_NAME}.lst
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS ${TARGET_NAME}
    )
endif()

if(NOT DEFINED DEBUG_FILES OR "nm" IN_LIST DEBUG_FILES OR "${GEN_PARSE_TOOL}" STREQUAL "True")
    add_custom_target(GENERAT_NM ALL
        COMMAND ${CMAKE_NM} -S -n  --format=sysv ${BIN_NAME}.elf > ${BIN_NAME}.nm
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS ${TARGET_NAME}
    )
endif()

if(NOT DEFINED DEBUG_FILES OR "asm" IN_LIST DEBUG_FILES)
    add_custom_target(GENERAT_ASM ALL
        COMMAND ${CMAKE_OBJDUMP} -d -m ${ARCH_FAMILY} ${BIN_NAME}.elf > ${BIN_NAME}.asm
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS ${TARGET_NAME}
    )
endif()

if("${GEN_PARSE_TOOL}" STREQUAL "True")
    add_custom_target(GENERAT_INFO ALL
        COMMAND ${CMAKE_OBJDUMP} -Wi ${BIN_NAME}.elf > ${BIN_NAME}.info
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS ${TARGET_NAME}
    )
endif()

if((NOT DEFINED DEBUG_FILES OR "mem" IN_LIST DEBUG_FILES) AND NOT ${ROM_CHECK})
add_custom_target(GENERAT_MEM ALL
    COMMAND ${Python3_EXECUTABLE} ${ELF_TO_DU} ${ROOT_DIR} ${BIN_NAME}.elf ${CMAKE_NM} > ${BIN_NAME}.du
    COMMAND ${Python3_EXECUTABLE} ${MEM_STATE} ${BIN_NAME}.lst ${BIN_NAME}.du ${BUILT_LDS} ${CHIP} > ${BIN_NAME}.mem
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS "${TARGET_NAME};GENERAT_LST"
)
if(NOT "SDK_NOT_MEM_LIMIT" IN_LIST DEFINES)
    add_custom_target(GENERAT_MEM_LIMIT ALL
        COMMENT "MEM_LIMIT"
        COMMAND ${Python3_EXECUTABLE} ${PARSE_MAP_SIZE_INFO} ${BIN_NAME}.lst ${BIN_NAME}.map
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS "${TARGET_NAME};GENERAT_LST")
 
endif()
endif()

if(NOT DEFINED DEBUG_FILES OR "hex" IN_LIST DEBUG_FILES)
add_custom_target(GENERAT_STD_HEX ALL
    # 生成 intel 标准 hex 文件
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${BIN_NAME}.elf ${BIN_NAME}_std.hex
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS GENERAT_BIN
)
add_custom_target(GENERAT_HEX ALL
    COMMAND ${Python3_EXECUTABLE} ${GEN_HEX} ${BIN_NAME}.bin ${BIN_NAME}.hex
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS GENERAT_BIN
)

if(DEFINED ROM_COMPONENT)
    add_custom_target(GENERAT_ROM_HEX ALL
        COMMAND ${Python3_EXECUTABLE} ${GEN_HEX} ${BIN_NAME}_rom.bin ${BIN_NAME}_rom.hex
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS GENERAT_BIN
    )
endif()
endif()

if (DEFINED BUILD_LEVEL AND NOT ${BUILD_LEVEL} STREQUAL "release")
if("${IMAGE_ANALYSIS}" STREQUAL "True" AND ${BUILD_PLATFORM} MATCHES "linux")
    add_custom_target(GENERAT_IMAGE_ANALYSIS ALL
        COMMAND ${Python3_EXECUTABLE} ${IMAGE_ANALSIS_TOOL} ${TARGET_COMMAND} ${OBJDUMP_PATH}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS ${TARGET_NAME}
    )
endif()

if("${CODESIZE_STATISTIC}" STREQUAL "True")
    add_custom_target(GENERAT_CODESIZE_STATISTIC ALL
        COMMAND ${Python3_EXECUTABLE} ${CODESIZE_STATISTIC_TOOL} ${TARGET_COMMAND}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS ${TARGET_NAME}
    )
endif()
endif()

if("${GEN_ROM_INFO}" STREQUAL "True")
    set(REMOTE_LIB ${ROOT_DIR}/drivers/chips/${CHIP}/remote_lib/${BUILD_TARGET_NAME}.cfg)
if (${CHIP} MATCHES "bs20|bs21|bs21a|bs21e|bs22|bs26")
    set(TARGET_SYM ${ROOT_DIR}/drivers/chips/bs2x/rom/rom_config/${CORE}/${BIN_NAME}.sym)
else()
    set(TARGET_SYM ${ROOT_DIR}/drivers/chips/${CHIP}/rom_config/${CORE}/${BIN_NAME}.sym)
endif()
    add_custom_target(GENERAT_ROM_INFO ALL
        COMMAND ${Python3_EXECUTABLE} ${GEN_ROM_INFO_TOOL} ${BIN_NAME}.elf ${REMOTE_LIB} ${TARGET_SYM}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS GENERAT_BIN
    )
endif()

if(NOT DEFINED PATCH)
    set(PATCH False)
endif()

if(NOT DEFINED PATCH_DYN_TBL)
    set(PATCH_DYN_TBL False)
endif()

if(${PATCH_DYN_TBL})

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${ROOT_DIR}/build/config/target_config/${CHIP}/patch_config
    ${PROJECT_BINARY_DIR}/patch_config
)

if((${PATCH} AND ${PATCH_DYN_TBL}) AND NOT ${ROM_CHECK})
    set(PATCH_CONFIG ${PROJECT_BINARY_DIR}/patch_config)
    set(GET_TA_TOOL ${ROOT_DIR}/build/config/target_config/${CHIP}/script/patch_get_tbl_info.py)
    add_custom_target(GET_DYN_TBL_ADDR ALL
        COMMAND ${Python3_EXECUTABLE} ${GET_TA_TOOL} ${CORE} ${TARGET_COMMAND} ${BIN_NAME} ${PATCH_CONFIG} ${PROJECT_BINARY_DIR}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS GENERAT_BIN
    )
endif()

if((DEFINED ROM_COMPONENT AND ${PATCH}) AND NOT ${ROM_CHECK})
    set(PATCH_CONFIG ${PROJECT_BINARY_DIR}/patch_config)
    add_custom_target(GENERAT_ROM_PATCH ALL
        COMMAND ${Python3_EXECUTABLE} ${GEN_PATCH} ${BIN_NAME}.bin ${BIN_NAME}_rom.bin ${BIN_NAME}.nm ${PATCH_CONFIG} ${CORE} ${TARGET_COMMAND} ${PROJECT_BINARY_DIR}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS GET_DYN_TBL_ADDR GENERAT_NM
    )
endif()

else()

if(((DEFINED ROM_COMPONENT AND ${PATCH}) AND NOT ${ROM_CHECK}) OR DEFINED ROM_SYM_PATH)
    set(PATCH_CONFIG ${ROOT_DIR}/build/config/target_config/${CHIP}/patch_config)
    add_custom_target(GENERAT_ROM_PATCH ALL
        COMMAND ${Python3_EXECUTABLE} ${GEN_PATCH} ${BIN_NAME}.bin ${BIN_NAME}_rom.bin ${BIN_NAME}.nm ${PATCH_CONFIG} ${CORE} ${TARGET_COMMAND} ${PROJECT_BINARY_DIR}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
		DEPENDS GENERAT_BIN GENERAT_NM
    )
endif()

endif()
