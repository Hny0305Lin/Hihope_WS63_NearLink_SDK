#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_adc")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_adc.c
)

if(DEFINED CONFIG_ADC_USING_V152)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v152/hal_adc_v152.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v152/hal_adc_v152_regs_op.c")
elseif(DEFINED CONFIG_ADC_USING_V153)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v153/hal_adc_v153.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v153/hal_adc_v153_regs_op.c")
elseif(DEFINED CONFIG_ADC_USING_V154)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v154/hal_adc_v154.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v154/hal_adc_v154_regs_op.c")
else()
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_adc_v151.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_adc_v151_regs_op.c")
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(DEFINED CONFIG_ADC_USING_V152)
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v152")
elseif(DEFINED CONFIG_ADC_USING_V153)
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v153")
elseif(DEFINED CONFIG_ADC_USING_V154)
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v154")
else()
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v151")
endif()


set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
