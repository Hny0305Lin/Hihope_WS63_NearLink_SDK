#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

if(NOT "${SECTOR_CFG}" STREQUAL "")
    SET(SECTOR_JSON ${ROOT_DIR}/build/config/target_config/${CHIP}/flash_sector_config/${SECTOR_CFG}.json)
    SET(SECTOR_BIN_PATH ${ROOT_DIR}/interim_binary/${CHIP}/bin/partition/${PKG_TARGET_NAME})
    if (NOT EXISTS ${SECTOR_BIN_PATH})
        file(MAKE_DIRECTORY ${SECTOR_BIN_PATH})
    endif()
    add_custom_target(GENERAT_PARTITION ALL
        COMMAND ${Python3_EXECUTABLE} ${ROOT_DIR}/build/script/param_packet.py ${SECTOR_JSON} ${SECTOR_BIN_PATH}/partition.bin &&
                ${CP} ${SECTOR_BIN_PATH}/partition.bin ${PROJECT_BINARY_DIR}/partition.bin
        COMMENT "update partition bin"
        WORKING_DIRECTORY ${ROOT_DIR}
        DEPENDS GENERAT_BIN
    )
endif()
