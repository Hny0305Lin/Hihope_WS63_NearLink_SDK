#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_I2C_MASTER)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/i2c_master_demo.c" PARENT_SCOPE)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_I2C_SLAVE)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/i2c_slave_demo.c" PARENT_SCOPE)
endif()