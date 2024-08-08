#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

if(${NV_UPDATE})
    add_custom_target(GENERAT_NVBIN ALL
        COMMAND ${Python3_EXECUTABLE} ${ROOT_DIR}/build/config/target_config/${CHIP}/build_nvbin.py ${TARGET_NAME}
        COMMENT "update nv bin"
        WORKING_DIRECTORY ${ROOT_DIR}
        DEPENDS GENERAT_BIN
    )
endif()

if(NOT ${NV_CFG} EQUAL "")
    set(NV_CFG_JSON             ${PROJECT_BINARY_DIR}/nv_config/build_nv_config.json)
    set(OUT_BIN_DIR             ${ROOT_DIR}/interim_binary/${CHIP}/bin/nv/${PKG_TARGET_NAME})
    if (NOT EXISTS ${OUT_BIN_DIR})
        file(MAKE_DIRECTORY ${OUT_BIN_DIR})
    endif()
    set(OUT_BIN_NAME            ${CHIP}_all_nv.bin)
    set(BUILD_TEMP_PATH         ${PROJECT_BINARY_DIR}/nv_config)
    set(NV_TARGET_JSON_PATH     ${ROOT_DIR}/middleware/chips/${CHIP}/nv/nv_config/nv_target.json)
    set(NV_RELATIVE_PATH        ${PROJECT_BINARY_DIR}/nv_config)
    set(NV_DEFAULT_CFG_DIR      ${ROOT_DIR}/middleware/chips/${CHIP}/nv/nv_config/${NV_CFG}/cfg)
    set(DATABASE_TXT_FILE       ${PROJECT_BINARY_DIR}/nv_config)


    if (NOT EXISTS ${NV_CFG_JSON})
        file(WRITE ${NV_CFG_JSON} "{\"OUT_BIN_DIR\":\"${OUT_BIN_DIR}\", \
            \"OUT_BIN_NAME\":\"${OUT_BIN_NAME}\", \"BUILD_TEMP_PATH\":\"${BUILD_TEMP_PATH}\", \
            \"NV_TARGET_JSON_PATH\":\"${NV_TARGET_JSON_PATH}\", \"NV_RELATIVE_PATH\":\"${NV_RELATIVE_PATH}\", \
            \"NV_DEFAULT_CFG_DIR\":\"${NV_DEFAULT_CFG_DIR}\", \"DATABASE_TXT_FILE\":\"${DATABASE_TXT_FILE}\"}")
    endif()

    set(TARGET_INCLUDE
        -I${ROOT_DIR}/include
        -I${ROOT_DIR}/middleware/utils/common_headers/native
        -I${ROOT_DIR}/middleware/chips/${CHIP}/nv/nv_config/${NV_CFG}/include
    )

    set(TARGET_DEFINES -DCONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM)

    set(GEN_TARGET_DIR  ${PROJECT_BINARY_DIR}/nv_config/etypes)
    if (NOT EXISTS ${GEN_TARGET_DIR})
        file(MAKE_DIRECTORY ${GEN_TARGET_DIR})
    endif()
    set(GEN_TARGET_SRC ${GEN_TARGET_DIR}/${CORE}.c)
    set(PRECOMPILE_TARGET ${GEN_TARGET_DIR}/${CORE}.etypes)

    add_custom_target(GENERAT_NV_INFO ALL
        COMMAND ${Python3_EXECUTABLE} ${BUILD_NV_GEN_UTILS} NV include ${GEN_TARGET_SRC}
        COMMAND ${CMAKE_C_COMPILER} -o ${PRECOMPILE_TARGET} ${TARGET_INCLUDE} ${TARGET_DEFINES} -E ${GEN_TARGET_SRC}
        WORKING_DIRECTORY ${ROOT_DIR}/middleware/chips/${CHIP}/nv/nv_config/${NV_CFG}
        DEPENDS GENERAT_BIN
    )

    if (${NV_CRC16})
        set(CRC_FLAGS True)
    else()
        set(CRC_FLAGS False)
    endif()

    add_custom_target(GENERAT_NVBIN ALL
        COMMAND ${Python3_EXECUTABLE} ${BUILD_NV_TOOL} ${NV_CFG_JSON} ${CORE} ${CRC_FLAGS} &&
            ${CP} ${OUT_BIN_DIR}/${OUT_BIN_NAME} ${PROJECT_BINARY_DIR}/${OUT_BIN_NAME}
        COMMENT "update nv bin"
        WORKING_DIRECTORY ${ROOT_DIR}
        DEPENDS GENERAT_NV_INFO
    )
endif()
