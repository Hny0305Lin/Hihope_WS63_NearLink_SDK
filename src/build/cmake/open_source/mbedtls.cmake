#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
if(EXISTS ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/build/conncet/mbedtls.cmake)
    include(${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/harden/build/conncet/mbedtls.cmake)
endif()

if(mbedtls IN_LIST TARGET_COMPONENT)
    install_sdk(${ROOT_DIR}/open_source/mbedtls "*")
endif()