#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "radar_sensing")
set(MODULE_NAME "wifi")
set(AUTO_DEF_FILE_ID FALSE)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/radar_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/radar_driver_mgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/radar_feature_mgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/radar_fft.c
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/radar_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/radar_interpolation.c
    ${CMAKE_CURRENT_SOURCE_DIR}/spec/ws63/radar_driver_mgr_spec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/spec/ws63/radar_event.c
    ${CMAKE_CURRENT_SOURCE_DIR}/spec/ws63/radar_mips.c
    ${CMAKE_CURRENT_SOURCE_DIR}/spec/ws63/radar_tcxo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/spec/ws63/radar_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/spec/ws63/radar_dfx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/spec/ws63/radar_tsensor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/spec/ws63/radar_alg_param.c
    ${CMAKE_CURRENT_SOURCE_DIR}/spec/ws63/radar_industry_info.c
    ${CMAKE_CURRENT_SOURCE_DIR}/spec/ws63/radar_feature_mgr_spec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mfg/radar_mfg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../preproc/hal/ws63/hal_radar_sensor_one_rx_ant.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../preproc/hal/ws63/hal_radar_sensor_rx_gain_cali.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../preproc/hal/ws63/hal_radar_subframe_data_pool_mgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../preproc/hal/ws63/hal_radar_wave.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/ppl/radar_ppl_cali_inv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/ppl/radar_ppl_data_cube_mgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/ppl/radar_ppl_data_smooth.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/ppl/radar_ppl_preproc_mgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/ppl/radar_ppl_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/ppl/radar_trx_delay_comp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/spl/proximity/radar_proximity_detection.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/spl/presence/radar_doppler_detection.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/spl/radar_spl_mgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/spl/radar_res_proc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/spl/radar_cfar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/spl/radar_plot_extraction.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/spl/radar_bitmap_proc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/fpl/radar_fpl_mgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/fpl/target_track/radar_confirmer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/fpl/target_track/radar_clean_cluster.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/fpl/target_track/radar_kalman_tracker.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/fpl/target_track/radar_multi_target_mgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/fpl/target_track/radar_target_mgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/fpl/target_track/radar_target_fsm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/adl/radar_track_proximity_detection.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/adl/radar_adl_mgr.c
)

set(PUBLIC_HEADER
    ${CMAKE_SOURCE_DIR}/middleware/utils/dfx/log/include/
    ${CMAKE_SOURCE_DIR}/middleware/utils/hcc/cfg/
    ${CMAKE_SOURCE_DIR}/middleware/utils/hcc/inc/
    ${CMAKE_SOURCE_DIR}/middleware/utils/hcc/comm/
    ${CMAKE_SOURCE_DIR}/include/middleware/services/radar/
    ${CMAKE_SOURCE_DIR}/kernel/osal/include/debug/
    ${CMAKE_SOURCE_DIR}/protocol/wifi/source/host/feature/
    ${CMAKE_SOURCE_DIR}/protocol/wifi/rom_code/ws63/source/inc/romable/
    ${CMAKE_SOURCE_DIR}/protocol/wifi/rom_code/ws63/source/device/forward/
    ${CMAKE_SOURCE_DIR}/drivers/chips/ws63/porting/arch/riscv/
    ${CMAKE_SOURCE_DIR}/drivers/chips/ws63/porting/soc/
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/spec/ws63
    ${CMAKE_CURRENT_SOURCE_DIR}/mfg
    ${CMAKE_CURRENT_SOURCE_DIR}/../preproc/hal/ws63
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/ppl
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/spl
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/spl/proximity/
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/spl/presence/
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/fpl/target_track/
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/fpl/
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg/adl/
    ${CMAKE_CURRENT_SOURCE_DIR}/../alg_ai/spec/ws63/
)

set(PRIVATE_DEFINES
    _PRE_USED_LIB_QSORT
    _PRE_USED_OPTIMIZED_FSM_API
    _PRE_FILE_ID_DEFINED
    _PRE_RADAR_PRINTK
    _PRE_RADAR_NEW_WAVE
    _PRE_RADAR_CALI_DC
    _PRE_RADAR_CALI_INV_PHASE
    _PRE_RADAR_SMOOTH_DATA
    _PRE_RADAR_BITMAP_CHECK
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
