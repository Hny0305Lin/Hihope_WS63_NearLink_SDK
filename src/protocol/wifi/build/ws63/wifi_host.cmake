#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

# Wi-Fi基础组件
set(COMPONENT_NAME "wifi_driver_hmac")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/interface/hmac_feature_interface.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/interface/hmac_ccpriv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/frw/frw_util_notifier.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_beacon.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_dfr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_dfs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_dfx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_feature_dft.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_encap_frame.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_encap_frame_ap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_encap_frame_sta.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/mac/oal_fsm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/main/liteOS/wifi_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/oal/liteOS/pm_temp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/version/hmac_version.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_11i.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_11w.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_alg_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_ant_switch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_arp_offload.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_blockack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_cali_mgmt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_chan_mgmt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_chr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_keep_alive.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_al_tx_rx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_crypto_tkip.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_data_acq.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_fcs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_ftm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_isolation.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_m2s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_mfg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_mgmt_ap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_mgmt_bss_comm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_mgmt_classifier.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_mgmt_sta.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_obss_sta.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_obss_ap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_opmode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_p2p.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_power.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_protection.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_proxy_arp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_psm_ap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_psm_sta.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_radar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_reset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_resource.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_scan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_stat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_sme_sta.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_smps.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_sta_pm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_tcp_opt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_thruput_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_txopps.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_wow.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_wur_ap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_ampdu_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_wmm.c

    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_cfg80211.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_wpa_ioctl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_regdb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_ccpriv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_mfg_ccpriv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_11d.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_config_sdp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_liteos_sdp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_cfg80211_apt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_scan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_ioctl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/soc_wifi_driver_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_customize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/common/wal_ccpriv_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/common/wal_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/common/wal_common.c

    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_fsm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_rx_filter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_tid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_traffic_classify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_tx_mgmt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_user.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_vap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_hook.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_alg_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_sample_daq.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_sample_daq_phy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/mac/mac_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/mac/mac_frame.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/mac/mac_ie.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/mac/mac_regdomain.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/mac/mac_resource.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/mac/mac_user.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/mac/mac_vap.c

    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/oal/liteOS/oal_main.c

    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_event_msg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_event.c
)

if(_PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN IN_LIST Wi-Fi-PRIVATE_DEFINES)
    list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac/hmac_traffic_classify.c
    )
endif()

# 添加算法源码和头文件
wifi_alg_compiler_entry(ws63_alg_wifi_host_ram OUT_SOURCES SOURCES OUT_INCLUDES Wi-Fi-PUBLIC_HEADER OUT_DEFINES Wi-Fi-PRIVATE_DEFINES
    WIFI_SOURCES ${SOURCES} WIFI_INCLUDES ${Wi-Fi-PUBLIC_HEADER} WIFI_DEFINES ${Wi-Fi-PRIVATE_DEFINES})
add_base_config()
build_component()

# Wi-Fi TCM组件
set(COMPONENT_NAME "wifi_driver_tcm")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/forward/hmac_rx_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/forward/hmac_rx_data_event.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/forward/hmac_rx_data_feature.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/forward/hmac_rx_data_filter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/forward/hmac_tx_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/forward/hmac_tx_encap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/forward/hmac_tx_mpdu_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/forward/hmac_tx_mpdu_queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/forward/liteOS/oal_skbuff.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/forward/liteOS/oal_netbuf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/frw/frw_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/frw/frw_hcc_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/frw/frw_hmac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/frw/frw_hmac_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/frw/frw_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/frw/frw_thread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/frw/frw_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/mac/mac_mem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/oal/liteOS/oal_net.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_net.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS/wal_net_bridge.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/mac/mac_data.c
)
add_base_config()
# 添加算法源码和头文件
wifi_alg_compiler_entry(ws63_alg_wifi_tcm OUT_SOURCES SOURCES WIFI_SOURCES ${SOURCES})
build_component()

# 编译算法独立组件
wifi_alg_independent_build(ws63_alg_ind_wifi)

# FEATURE CSA
set(COMPONENT_NAME "wifi_csa")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_csa_ap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_csa_sta.c
)
add_base_config()
build_component()

# FEATURE FRAG
set(COMPONENT_NAME "wifi_frag")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_frag.c
)
add_base_config()
build_component()

# FEATURE MBO
set(COMPONENT_NAME "wifi_mbo")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_mbo.c
)
add_base_config()
build_component()

# FEATURE BSRP_NFRP
set(COMPONENT_NAME "wifi_bsrp_nfrp")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_bsrp_nfrp.c
)
add_base_config()
build_component()

# FEATURE SLP
set(COMPONENT_NAME "wifi_slp")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_slp.c
)
add_base_config()
build_component()

# FEATURE RADAR SENSOR
set(COMPONENT_NAME "wifi_radar_sensor")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_radar_sensor.c
)
add_base_config()
build_component()

# FEATURE APF
set(COMPONENT_NAME "wifi_apf")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_apf.c
)
add_base_config()
build_component()

# FEATURE 11k
set(COMPONENT_NAME "wifi_11k")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_11k.c
)
add_base_config()
build_component()

# FEATURE 11v
set(COMPONENT_NAME "wifi_11v")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_11v.c
)
add_base_config()
build_component()

# FEATURE wifi_auto_adjust_freq
set(COMPONENT_NAME "wifi_auto_adjust_freq")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_auto_adjust_freq.c
)
add_base_config()
build_component()

# FEATURE 11r
set(COMPONENT_NAME "wifi_11r")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_11r.c
)
add_base_config()
build_component()

# FEATURE REPEATER
set(COMPONENT_NAME "wifi_repeater")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_single_proxysta.c
)
add_base_config()
build_component()

# FEATURE CSI
set(COMPONENT_NAME "wifi_csi")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_csi.c
)
add_base_config()
build_component()

# FEATURE M2U
set(COMPONENT_NAME "wifi_m2u")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_m2u.c
)
add_base_config()
build_component()

# FEATURE BLACKLIST
set(COMPONENT_NAME "wifi_blacklist")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_blacklist.c
)
add_base_config()
build_component()

# FEATURE wapi
set(COMPONENT_NAME "wifi_wapi")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_wapi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_wapi_sms4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_wapi_wpi.c
)
add_base_config()
build_component()

# FEATURE sdp
set(COMPONENT_NAME "wifi_sdp")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_sdp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_sdp_test.c
)
add_base_config()
build_component()

# FEATURE WPS
set(COMPONENT_NAME "wifi_wps")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_wps.c
)
add_base_config()
build_component()

# FEATURE latency
set(COMPONENT_NAME "wifi_latency")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_latency_stat.c
)
add_base_config()
build_component()

# FEATURE promisc
set(COMPONENT_NAME "wifi_promisc")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_promisc.c
)
add_base_config()
build_component()

# FEATURE tx_amsdu
set(COMPONENT_NAME "wifi_tx_amsdu")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_tx_amsdu.c
)
add_base_config()
build_component()

# FEATURE roam
set(COMPONENT_NAME "wifi_roam")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_roam_alg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_roam_connect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_roam_main.c
)
add_base_config()
build_component()

# FEATURE uapsd_sta
set(COMPONENT_NAME "wifi_uapsd_sta")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_uapsd_sta.c
)
add_base_config()
build_component()

# FEATURE ant_sel
set(COMPONENT_NAME "wifi_ant_sel")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_ant_sel.c
)
add_base_config()
build_component()

# FEATURE PSD
set(COMPONENT_NAME "wifi_psd")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_psd.c
)
add_base_config()
build_component()

# FEATURE twt
set(COMPONENT_NAME "wifi_twt")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_twt.c
)
add_base_config()
build_component()

# FEATURE btcoex
set(COMPONENT_NAME "wifi_btcoex")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_btcoex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_btcoex_ba.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_btcoex_btsta.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_btcoex_m2s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_btcoex_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_btcoex_ps.c
)
add_base_config()
build_component()

# FEATURE uapsd_ap
set(COMPONENT_NAME "wifi_uapsd_ap")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_uapsd.c
)
add_base_config()
build_component()

# FEATURE dnb
set(COMPONENT_NAME "wifi_dnb")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_dnb_sta.c
)
add_base_config()
build_component()

# FEATURE sr
set(COMPONENT_NAME "wifi_sr")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/hmac_sr_sta.c
)
add_base_config()
build_component()
