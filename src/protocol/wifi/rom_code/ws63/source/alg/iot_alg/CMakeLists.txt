set(
    iot_alg_device_rom_src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_main_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_txbf_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_edca_opt_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_anti_interference_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_tpc_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_tpc_config_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_tpc_probe_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_gla_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_autorate_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_autorate_if_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_autorate_basic_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_autorate_intrf_mode_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_aggr_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_aggr_probe_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_traffic_ctl_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_schedule_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_temp_protect_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_probe_common_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_tx_tb_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_rts_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable/alg_compatibility_rom.c
)

set(
    iot_alg_device_rom_inc_list
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable
)

set(
    iot_alg_device_ram_src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/device/alg_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/device/alg_ar.c
)

set(
    iot_alg_device_ram_inc_list
    ${CMAKE_CURRENT_SOURCE_DIR}/device
    ${CMAKE_CURRENT_SOURCE_DIR}/device/romable
)

set(
    iot_alg_host_src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_aggr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_gla.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_autorate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_dbac_hmac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_intf_det.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_intrf_mode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_probe_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_rts.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_schedule.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_temp_protect_basic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_tpc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_traffic_ctl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_tx_tb_hmac.c
)

set(
    iot_alg_host_inc_list
    ${CMAKE_CURRENT_SOURCE_DIR}/host
)

set(
    iot_alg_txbf_source_list
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_txbf.c
)

set(
    iot_alg_temp_protect_source_list
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_temp_protect.c
)

set(
    iot_alg_anti_interference_source_list
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_anti_interference.c
)

set(
    iot_alg_edca_opt_source_list
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_edca_opt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_edca_intrf_mode.c
)

set(
    iot_alg_cca_opt_source_list
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_cca_intrf_mode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/alg_cca_optimize.c
)

set(
    iot_alg_host_independent_src_list
    wifi_alg_txbf
    wifi_alg_temp_protect
    wifi_alg_anti_interference
    wifi_alg_edca_opt
    wifi_alg_cca_opt
)

set(
    iot_alg_device_independent_src_list
)
set(
    iot_alg_defines
    _PRE_WLAN_FEATURE_ANTI_INTERF
    _PRE_WLAN_FEATURE_AUTOAGGR
    _PRE_WLAN_FEATURE_AUTORATE
    _PRE_WLAN_FEATURE_CCA_OPT
    _PRE_WLAN_FEATURE_EDCA_OPT
    _PRE_WLAN_FEATURE_INTF_DET
    _PRE_WLAN_FEATURE_NEGTIVE_DET
    _PRE_WLAN_FEATURE_RTS
    _PRE_WLAN_FEATURE_SCHEDULE
    _PRE_WLAN_FEATURE_SCH_STRATEGY_WMM_ENSURE
    _PRE_WLAN_FEATURE_TEMP_PROTECT
    _PRE_WLAN_FEATURE_TPC
    _PRE_WLAN_FEATURE_TRAFFIC_CTL
    _PRE_WLAN_FEATURE_TXBF
    _PRE_WLAN_FEATURE_COMPATIBILITY
)
set_property(GLOBAL PROPERTY WIFI_ALG_IOT_DEVICE_ROM_SOURCES ${iot_alg_device_rom_src_list})
set_property(GLOBAL PROPERTY WIFI_ALG_IOT_DEVICE_ROM_INCLUDES ${iot_alg_device_rom_inc_list})

set_property(GLOBAL PROPERTY WIFI_ALG_IOT_DEVICE_RAM_SOURCES ${iot_alg_device_ram_src_list})
set_property(GLOBAL PROPERTY WIFI_ALG_IOT_DEVICE_RAM_INCLUDES ${iot_alg_device_ram_inc_list})

set_property(GLOBAL PROPERTY WIFI_ALG_IOT_HOST_SOURCES ${iot_alg_host_src_list})
set_property(GLOBAL PROPERTY WIFI_ALG_IOT_HOST_INCLUDES ${iot_alg_host_inc_list})

set_property(GLOBAL PROPERTY WIFI_ALG_IOT_HOST_IND_SOURCES ${iot_alg_host_independent_src_list})
set_property(GLOBAL PROPERTY WIFI_ALG_IOT_DEVICE_IND_SOURCES ${iot_alg_device_independent_src_list})

set_property(GLOBAL PROPERTY WIFI_ALG_IOT_DEFINES ${iot_alg_defines})

set_property(GLOBAL PROPERTY WIFI_ALG_TXBF ${iot_alg_txbf_source_list})
set_property(GLOBAL PROPERTY WIFI_ALG_TEMP_PROTECT ${iot_alg_temp_protect_source_list})

set_property(GLOBAL PROPERTY WIFI_ALG_ANTI_INTERFERENCE ${iot_alg_anti_interference_source_list})
set_property(GLOBAL PROPERTY WIFI_ALG_EDCA_OPT ${iot_alg_edca_opt_source_list})
set_property(GLOBAL PROPERTY WIFI_ALG_CCA_OPT ${iot_alg_cca_opt_source_list})