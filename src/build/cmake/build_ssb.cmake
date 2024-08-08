#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

if(DEFINED APPLICATION)
if(${APPLICATION} STREQUAL "ssb")
add_custom_target(ADD_SHA_TO_SSB ALL
    COMMAND ${Python3_EXECUTABLE} ${BUILD_UTILS} add_len_and_sha256_info_to_ssb ${TARGET_NAME}.bin ${CHIP}
    COMMENT "add ssb length and sha256 info into ssb.bin"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS GENERAT_BIN
)
endif()
endif()
