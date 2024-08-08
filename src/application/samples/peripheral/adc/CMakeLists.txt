#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

if((DEFINED CONFIG_ADC_USING_V154) OR (DEFINED CONFIG_ADC_USING_V155))
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/adc_demo_inc.c" PARENT_SCOPE)
else()
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/adc_demo.c" PARENT_SCOPE)
endif()