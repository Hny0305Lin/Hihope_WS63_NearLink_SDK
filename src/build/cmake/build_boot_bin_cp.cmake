#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

if(NOT "${FLASHBOOT_CFG}" STREQUAL "")
add_custom_target(COPY_FLASHBOOT_BIN ALL
    COMMAND ${CP} ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${FLASHBOOT_CFG}/flashboot_sign_a.bin ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/flashboot_sign_a.bin &&
            ${CP} ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${FLASHBOOT_CFG}/flashboot_sign_b.bin ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/flashboot_sign_b.bin
    COMMENT "copy bin file"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS GENERAT_SIGNBIN
)
endif()

if(NOT "${LOADERBOOT_CFG}" STREQUAL "")
add_custom_target(COPY_LOADERBOOT_BIN ALL
    COMMAND ${CP} ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${LOADERBOOT_CFG}/loaderboot_sign.bin ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/loaderboot_sign.bin
    COMMENT "copy bin file"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS GENERAT_SIGNBIN
)
endif()