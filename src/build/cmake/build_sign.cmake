#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================

if (${TARGET_NAME} STREQUAL "flashboot")
if (EXISTS ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg)
    if (NOT ${SEC_BOOT} STREQUAL "")
        add_custom_target(CONCAT_BIN ALL
            COMMAND ${Python3_EXECUTABLE} ${CONCAT_TOOL} ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${SEC_BOOT}/sec_boot.bin ${PROJECT_BINARY_DIR}/flashboot.bin ${SEC_BOOT_SIZE} ${PROJECT_BINARY_DIR}/flashboot.bin
            COMMENT "concat bin"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            DEPENDS GENERAT_BIN
        )
    endif()
    add_custom_target(GENERAT_SIGNBIN ALL
        COMMAND ${SIGN_TOOL} 0 ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg 1>nul 2>nul &&
                ${CP} ${PROJECT_BINARY_DIR}/flashboot_sign_a.bin ${PROJECT_BINARY_DIR}/flashboot_sign_b.bin
        COMMENT "sign file:gen boot sign file"
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS GENERAT_BIN
    )
    if (NOT ${SEC_BOOT} STREQUAL "")
        add_dependencies(GENERAT_SIGNBIN CONCAT_BIN)
    endif()
    if (${UPDATE_BIN})
        string(REPLACE "_" "-" TARGET_DIR ${BUILD_TARGET_NAME})
        if (NOT EXISTS ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin)
            file(MAKE_DIRECTORY ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin)
        endif()
        if (NOT EXISTS ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR})
            file(MAKE_DIRECTORY ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR})
        endif()
        add_custom_target(COPY_SIGNBIN ALL
            COMMAND ${CP} ${ROOT_DIR}/output/${CHIP}/acore/${TARGET_DIR}/flashboot_sign_a.bin ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR}/flashboot_sign_a.bin  &&
                    ${CP} ${ROOT_DIR}/output/${CHIP}/acore/${TARGET_DIR}/flashboot_sign_a.bin ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR}/flashboot_sign_b.bin
            COMMENT "copy bin file"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            DEPENDS GENERAT_SIGNBIN
        )
    endif()
endif()
elseif (${TARGET_NAME} STREQUAL "loaderboot")
if (EXISTS ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg)
    add_custom_target(GENERAT_SIGNBIN ALL
        COMMAND ${SIGN_TOOL} 0 ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg 1>nul 2>nul
        COMMENT "sign file:gen boot sign file"
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS GENERAT_BIN
    )
    if (${UPDATE_BIN})
        string(REPLACE "_" "-" TARGET_DIR ${BUILD_TARGET_NAME})
        if (NOT EXISTS ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin)
            file(MAKE_DIRECTORY ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin)
        endif()
        if (NOT EXISTS ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR})
            file(MAKE_DIRECTORY ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR})
        endif()
        add_custom_target(COPY_SIGNBIN ALL
            COMMAND ${CP} ${ROOT_DIR}/output/${CHIP}/acore/${TARGET_DIR}/loaderboot_sign.bin ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR}/loaderboot_sign.bin
            COMMENT "copy bin file"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            DEPENDS GENERAT_SIGNBIN
        )
    endif()
endif()
elseif (${TARGET_NAME} MATCHES "application*" OR ${TARGET_NAME} STREQUAL "ate_debug" OR ${TARGET_NAME} STREQUAL "ate" OR
        ${TARGET_NAME} MATCHES "protocol*" )
if (EXISTS ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg)
add_custom_target(GENERAT_SIGNBIN ALL
    COMMAND ${SIGN_TOOL} 0 ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg  1>nul 2>nul
    COMMENT "sign file:gen boot sign file"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS GENERAT_BIN
)
endif()
elseif (${TARGET_NAME} MATCHES "control_ws53*")
if (EXISTS ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg)
add_custom_target(GENERAT_SIGNBIN ALL
    COMMAND ${SIGN_TOOL} 0 ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg
    COMMENT "sign file:gen boot sign file"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS GENERAT_BIN
)
endif()
endif()
if (${CHIP} STREQUAL "ws63")
add_custom_target(WS63_GENERAT_SIGNBIN ALL
    COMMAND ${Python3_EXECUTABLE} ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/params_and_bin_sign.py ${TARGET_NAME}
    COMMENT "ws63 image sign"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS GENERAT_BIN
)

if(TARGET GENERAT_ROM_PATCH)
    add_dependencies(WS63_GENERAT_SIGNBIN GENERAT_ROM_PATCH)
endif()
endif()

if (${CHIP} STREQUAL "ws53")
add_custom_target(WS53_GENERAT_SIGNBIN ALL
    COMMAND sh ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/params_and_bin_sign.sh
    COMMENT "ws53 image sign"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS GENERAT_BIN
)

if(TARGET GENERAT_ROM_PATCH)
    add_dependencies(WS53_GENERAT_SIGNBIN GENERAT_ROM_PATCH)
endif()
endif()

if(TARGET GENERAT_ROM_PATCH AND TARGET GENERAT_SIGNBIN)
    add_dependencies(GENERAT_SIGNBIN GENERAT_ROM_PATCH)
endif()

if (${TARGET_NAME} STREQUAL "sec_boot")
    if (${UPDATE_BIN})
        string(REPLACE "_" "-" TARGET_DIR ${BUILD_TARGET_NAME})
        if (NOT EXISTS ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin)
            file(MAKE_DIRECTORY ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin)
        endif()
        if (NOT EXISTS ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR})
            file(MAKE_DIRECTORY ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR})
        endif()
        add_custom_target(COPY_SEC_BOOTBIN ALL
            COMMAND ${CP} ${ROOT_DIR}/output/${CHIP}/acore/${TARGET_DIR}/sec_boot.bin ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR}/sec_boot.bin
            COMMENT "copy bin file"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            DEPENDS GENERAT_BIN
        )
    endif()
endif()

if(${GEN_SEC_BIN} AND ${GEN_SEC_BIN} STREQUAL "True")
    if(${CORE} STREQUAL "acore")
        if (TARGET GENERAT_SIGNBIN)
            add_custom_target(GENERAT_SEC_IMAGE ALL
                COMMAND ${CMAKE_OBJCOPY} --enable_sec ${PROJECT_BINARY_DIR}/${BIN_NAME}_sign.bin
                WORKING_DIRECTORY ${COMPILER_ROOT}/bin
                DEPENDS GENERAT_SIGNBIN
            )
        else()
            add_custom_target(GENERAT_SEC_IMAGE ALL
                COMMAND ${CMAKE_OBJCOPY} --enable_sec ${PROJECT_BINARY_DIR}/${BIN_NAME}.bin
                WORKING_DIRECTORY ${COMPILER_ROOT}/bin
                DEPENDS GENERAT_BIN
            )
        endif()
    elseif(${CORE} STREQUAL "bt_core")
        add_custom_target(GENERAT_SEC_IMAGE ALL
            COMMAND ${SEC_OBJCPY_TOOL} --enable_sec ${PROJECT_BINARY_DIR}/${BIN_NAME}.bin
            WORKING_DIRECTORY ${SEC_TOOL_DIR}
            DEPENDS GENERAT_ROM_PATCH
        )
    endif()
endif()

