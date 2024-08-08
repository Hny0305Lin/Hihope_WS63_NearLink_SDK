#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "radar_ai")
set(MODULE_NAME "wifi")
set(AUTO_DEF_FILE_ID FALSE)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg_ai/radar_ai_attention_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg_ai/radar_ai_conv_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg_ai/radar_ai_conv2_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg_ai/radar_ai_fc_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg_ai/radar_ai_lstm_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg_ai/radar_ai_model_para.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg_ai/radar_ai_model.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg_ai/spec/ws63/radar_ai_model_spec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg_ai/radar_ai_model_preprocess.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg_ai/radar_ai_pool_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg_ai/radar_ai_utils.c
)

set(PUBLIC_HEADER
    #${CMAKE_SOURCE_DIR}/middleware/utils/dfx/log/include/
    #${CMAKE_SOURCE_DIR}/include/middleware/services/radar/
    #${CMAKE_SOURCE_DIR}/kernel/osal/include/debug/
    #${CMAKE_SOURCE_DIR}/protocol/wifi/source/host/feature/
    #${CMAKE_SOURCE_DIR}/protocol/wifi/rom_code/ws63/source/inc/romable/
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/spec/ws63
    ${CMAKE_CURRENT_SOURCE_DIR}/../plat
    ${CMAKE_CURRENT_SOURCE_DIR}/../plat/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../plat/spec/ws63
    ${CMAKE_CURRENT_SOURCE_DIR}/../plat/mfg
    ${CMAKE_CURRENT_SOURCE_DIR}/../preproc/hal/ws63
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/ppl
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/spl
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/spl/proximity/
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/spl/presence/
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/fpl/target_track/
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/adl/
)

set(PRIVATE_DEFINES
    _PRE_FILE_ID_DEFINED
    _PRE_RADAR_PRINTK
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CXXFLAGS
    -Werror
    -Wall
    -Wframe-larger-than=2048
    -Wtrampolines
    -Wfloat-equal
    -Wformat=2
    -fno-common
    -ffunction-sections
    -fdata-sections
    -fno-strict-aliasing
    -fsigned-char
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-strict-prototypes
    -Wno-sign-compare
    -Wno-unused-but-set-variable
    -Wno-maybe-uninitialized
    -Wno-return-type
    -Wno-unused-value
    -Wno-comment
    -Wno-pointer-sign
    -Wno-attributes
    -Wno-uninitialized
    -Wno-maybe-uninitialized
    -Wno-unused-function
    -Wno-jump-misses-init
    -Wno-format
    -Wno-incompatible-pointer-types
    -Wno-empty-body
    -Wno-implicit-function-declaration
    -Wno-int-conversion
    -Wno-missing-field-initializers
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/radar/${TARGET_COMMAND})
build_component()
