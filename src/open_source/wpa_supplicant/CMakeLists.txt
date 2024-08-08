#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "wpa_supplicant")


set(SOURCES
    ${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/bss.c
	${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/config.c
	${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/ctrl_iface.c
	${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/eap_register.c
	${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/events.c
	${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/scan.c
	${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/wpa_supplicant.c
	${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/wpas_glue.c
	${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/ap.c
	${ROOT_DIR}/middleware/services/wifi_service/hostapd/liteos_hostapd_src/hostapd_main_rtos.c
	${ROOT_DIR}/middleware/services/wifi_service/hostapd/liteos_hostapd_src/hostapd_ctrl_iface_rtos.c
	${ROOT_DIR}/open_source/wpa_supplicant/liteos_wpa_src/crypto_mbedtls.c
	${ROOT_DIR}/open_source/wpa_supplicant/liteos_wpa_src/ctrl_iface_rtos.c
	${ROOT_DIR}/open_source/wpa_supplicant/liteos_wpa_src/eloop_rtos.c
	${ROOT_DIR}/open_source/wpa_supplicant/liteos_wpa_src/l2_packet_rtos.c
	${ROOT_DIR}/open_source/wpa_supplicant/liteos_wpa_src/main_rtos.c
	${ROOT_DIR}/open_source/wpa_supplicant/liteos_wpa_src/os_rtos.c
	${ROOT_DIR}/open_source/wpa_supplicant/liteos_wpa_src/wpa_cli_rtos.c
	${ROOT_DIR}/middleware/services/wifi_service/wpa/liteos_wpa_api/wifi_api.c
	${ROOT_DIR}/middleware/services/wifi_service/wpa/liteos_wpa_api/wifi_softap_api.c
	${ROOT_DIR}/middleware/services/wifi_service/wpa/liteos_wpa_api/wapi_api.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/common/ieee802_11_common.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/common/wpa_common.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/common/hw_features_common.c
	#${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/aes-cbc.c
	#${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/aes-internal-dec.c
	#${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/aes-internal-enc.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/aes-internal.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/aes-unwrap.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/aes-wrap.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/aes-omac1.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/crypto_internal-modexp.c
	#${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/dh_group5.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/dh_groups.c
	#${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/md5-internal.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/md5.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/random.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/rc4.c
	#${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/sha1-internal.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/sha1-prf.c
	#${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/sha256-internal.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/sha256-prf.c
	#${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/sha256.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/drivers/drivers.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/drivers/driver_common.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/eapol_supp/eapol_supp_sm.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/eap_peer/eap.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/eap_peer/eap_methods.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/eap_common/eap_common.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/eapol_auth/eapol_auth_sm.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/eap_server/eap_server.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/eap_server/eap_server_identity.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/eap_server/eap_server_methods.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/rsn_supp/wpa.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/rsn_supp/wpa_ie.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/utils/base64.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/utils/common.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/utils/uuid.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/utils/wpa_debug.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/utils/wpabuf.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/tls/bignum.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/hostapd.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/bss_load.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/wpa_auth_glue.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/utils.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/ap_config.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/sta_info.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/ap_mlme.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/drv_callbacks.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/ap_drv_ops.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/ieee802_11_ht.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/ieee802_11.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/hw_features.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/wpa_auth.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/wpa_auth_ie.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/ieee802_1x.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/ieee802_11_he.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/eap_user_db.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/beacon.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/acs.c
	#add wpa3
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/pmksa_cache_auth.c
	${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/sme.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/common/sae.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/common/dragonfly.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/rsn_supp/pmksa_cache.c

	#add ltos_src
	${ROOT_DIR}/middleware/services/wifi_service/wpa/ltos_src/eloop_ltos.c

	#add osdep src
	${ROOT_DIR}/middleware/services/wifi_service/wpa/osdep/osdep_osal.c

	#add CONFIG_DEVICE_TYPE_SOC sources
	${ROOT_DIR}/middleware/services/wifi_service/wpa/driver_soc/driver_soc.c
	${ROOT_DIR}/middleware/services/wifi_service/wpa/driver_soc/driver_soc_ioctl.c
	# wpa 2.10 new
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/ieee802_11_shared.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/sha256-kdf.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/utils/crc32.c
	# add owe
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/sha384-prf.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/sha384-kdf.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/sha512-kdf.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto/sha512-prf.c
	# add wps
	${ROOT_DIR}/open_source/wpa_supplicant/src/wps/wps.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/wps/wps_common.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/wps/wps_attr_parse.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/wps/wps_attr_build.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/wps/wps_attr_process.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/wps/wps_dev_attr.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/wps/wps_enrollee.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/wps/wps_registrar.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/wps_hostapd.c
	${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/notify.c
	${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/wps_supplicant.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/eap_common/eap_wsc_common.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/eap_peer/eap_wsc.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/eap_server/eap_server_wsc.c
	# add wapi
	${ROOT_DIR}/middleware/services/wifi_service/wpa/wapi/wapi.c
	${ROOT_DIR}/middleware/services/wifi_service/wpa/wapi/wai_sm.c
	${ROOT_DIR}/middleware/services/wifi_service/wpa/wapi/wai_rxtx.c
	${ROOT_DIR}/middleware/services/wifi_service/wpa/wapi/wai_crypto_mbedtls.c
	# add enterprise
	${ROOT_DIR}/open_source/wpa_supplicant/src/eap_peer/eap_tls.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/eap_peer/eap_tls_common.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/eap_peer/eap_methods.c
	${ROOT_DIR}/open_source/wpa_supplicant/liteos_wpa_src/tls_mbedtls.c

	# add 11r
	${ROOT_DIR}/open_source/wpa_supplicant/src/rsn_supp/wpa_ft.c

	# add mbo
	${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/mbo.c
	${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/wnm_sta.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/mbo_ap.c
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap/wnm_ap.c

	# add scan list adapt handle code
	${ROOT_DIR}/middleware/services/wifi_service/wpa/scan_list_adapt/scan_list_adapt.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
	${ROOT_DIR}/middleware/services/wifi_service/wpa/wapi
	${ROOT_DIR}/middleware/services/wifi_service/wpa/driver_soc
	${ROOT_DIR}/middleware/services/wifi_service/wpa/liteos_wpa_api
	${ROOT_DIR}/middleware/services/wifi_service/wpa/ltos_src
	${ROOT_DIR}/middleware/services/wifi_service/wpa/osdep
	${ROOT_DIR}/open_source/wpa_supplicant
	${ROOT_DIR}/open_source/wpa_supplicant/src
	${ROOT_DIR}/open_source/wpa_supplicant/src/crypto
	${ROOT_DIR}/open_source/wpa_supplicant/src/utils
	${ROOT_DIR}/open_source/wpa_supplicant/src/tls
	${ROOT_DIR}/open_source/wpa_supplicant/src/rsn_supp
	${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant
	${ROOT_DIR}/open_source/wpa_supplicant/liteos_wpa_src
	${ROOT_DIR}/open_source/wpa_supplicant/src/ap
	${ROOT_DIR}/middleware/services/wifi_service/hostapd/hostapd
	${ROOT_DIR}/middleware/services/wifi_service/hostapd/liteos_hostapd_src
	${ROOT_DIR}/middleware/services/wifi_service/service

	# add scan list adapt handle code
	${ROOT_DIR}/middleware/services/wifi_service/wpa/scan_list_adapt

	${ROOT_DIR}/middleware/utils/common_headers/

	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0/include
	${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.1.0
	${ROOT_DIR}/drivers/drivers/driver/watchdog
	${ROOT_DIR}/include/driver/security_unified
	${ROOT_DIR}/protocol/wifi/source/host/inc/liteOS
	${ROOT_DIR}/include/middleware/services/wifi
)

if("__ALIOS__" IN_LIST DEFINES)
    list(APPEND PRIVATE_HEADER
        ${ROOT_DIR}/kernel/alios/utils/libc/musl_1.2.3/include
        ${ROOT_DIR}/kernel/alios/utils/libc/adapt/include
        ${ROOT_DIR}/kernel/alios/utils/libc/adapt/musl_1.2.3/include
    )
endif()

set(PRIVATE_DEFINES
	INCLUDE_UNUSED
	WLAN_HEADERS
	WIRELESS_EXT=0
	BT_SETUP=0
	REGCODE_REMAPPING=0
	SOFTAP_MODE
	SOFTMAC_FILE_USED
	WPA_SUPPLICANT
	HOSTAPD
	CONFIG_IEEE80211AX
	CONFIG_NO_CONFIG_WRITE
	CONFIG_WPA
	CONFIG_WPS
	CONFIG_WPS_AP
	EAP_WSC
	EAP_SERVER
	EAP_SERVER_WSC

	CONFIG_SHA256

	OS_NO_C_LIB_DEFINES
	OPENSSL_DISABLE_OLD_DES_SUPPORT
	THIRTY_TWO_BIT
	OPENSSL_NO_SOCK
	CONFIG_IEEE80211N
	NEED_AP_MLME
	CONFIG_INTERNAL_LIBTOMMATH
	LTM_FAST
	CONFIG_NO_RADIUS
	CONFIG_NO_ACCOUNTING
	CONFIG_NO_VLAN
	CONFIG_NO_CONFIG_BLOBS
	CONFIG_CTRL_IFACE
	IEEE8021X_EAPOL
	CONFIG_TI_COMPILER
	CONFIG_CRYPTO_INTERNAL
	CONFIG_LITEOS_WPA
	LOS_WPA_EVENT_CALLBAK
	EXT_CODE_CROP
	LOS_CONFIG_NO_VLAN
	LOS_CONFIG_MESH_TRIM
	CONFIG_WEP
	LOS_CONFIG_HOSTAPD_QOS
	EXT_WPA_MINI
	LOS_CONFIG_EXT_DRIVER_NOT_SUPPORT
	LOS_CONFIG_HOSTAPD_EAP_CIPHERS
	EXT_WPA_KEY_MGMT_CROP
	LOS_CONFIG_HOSTAPD_TKIP_MIC
	LOS_CONFIG_HOSTAPD_RRM
	EXT_EAP_TRIM
	CONFIG_NO_HOSTAPD_LOGGER
	LOS_CONFIG_80211_IES_CROP
	CONFIG_PRINT_NOUSE
	LOS_INLINE_FUNC_CROP
	CONFIG_SAE_NO_FFC
	CONFIG_SAE_ONE_ECC_CURVE
	CONFIG_SAE_CROP
	LOS_CONFIG_HOSTAPD_PMKSA_CROP
	LOS_HOSTAPD_HT_CONFIG_CROP
	LOS_HOSTAPD_CONFIG_CROP
	LOS_CONFIG_ACL_CROP
	LOS_WPA_PATCH
	CONFIG_NO_RANDOM_POOL
	CONFIG_NO_STDOUT_DEBUG
	CONFIG_IEEE80211W
	CONFIG_DRIVER_SOC
	CONFIG_WPA3
	CONFIG_SME
	CONFIG_SAE
	CONFIG_ECC
	LTOS_CONFIG_NO_INTERNAL_SME
	CONFIG_HOSTAPD_WPA3
	LOS_CONFIG_HOSTAPD_MGMT
	CONFIG_HOSTAPD_WPA3_PMKSA
	CONFIG_OS_OSAL
	CONFIG_ACS
	CONFIG_OWE
	MBEDTLS_ECP_RESTARTABLE
	CONFIG_SHA384
	CONFIG_SHA512
	LOS_CONFIG_WPA_ENTERPRISE
	LOS_CONFIG_EAP_TLS
	EAP_TLS
	CONFIG_IEEE80211R
	CONFIG_NO_WPA_MSG
	CONFIG_WAPI
	_PRE_WLAN_FEATURE_SDP
	CONFIG_MBO
	CONFIG_WNM
)

# 小型化版本,裁掉wps、企业级加密、WAPI、ROAM等特性
if(${SMALLER})
    list(REMOVE_ITEM PRIVATE_DEFINES
		# WPS
		CONFIG_WPS
		CONFIG_WPS_AP
		EAP_WSC
		EAP_SERVER
		EAP_SERVER_WSC
		_PRE_WLAN_FEATURE_SDP
		# 企业级
		LOS_CONFIG_WPA_ENTERPRISE
		LOS_CONFIG_EAP_TLS
		EAP_TLS
		# WAPI
		CONFIG_WAPI
		# 11r
		CONFIG_IEEE80211R
		# mbo
		CONFIG_MBO
		CONFIG_WNM
    )
    list(REMOVE_ITEM SOURCES
		# WPS
		${ROOT_DIR}/open_source/wpa_supplicant/src/wps/wps.c
		${ROOT_DIR}/open_source/wpa_supplicant/src/wps/wps_common.c
		${ROOT_DIR}/open_source/wpa_supplicant/src/wps/wps_attr_parse.c
		${ROOT_DIR}/open_source/wpa_supplicant/src/wps/wps_attr_build.c
		${ROOT_DIR}/open_source/wpa_supplicant/src/wps/wps_attr_process.c
		${ROOT_DIR}/open_source/wpa_supplicant/src/wps/wps_dev_attr.c
		${ROOT_DIR}/open_source/wpa_supplicant/src/wps/wps_enrollee.c
		${ROOT_DIR}/open_source/wpa_supplicant/src/wps/wps_registrar.c
		${ROOT_DIR}/open_source/wpa_supplicant/src/ap/wps_hostapd.c
		${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/wps_supplicant.c
		${ROOT_DIR}/open_source/wpa_supplicant/src/eap_common/eap_wsc_common.c
		${ROOT_DIR}/open_source/wpa_supplicant/src/eap_peer/eap_wsc.c
		${ROOT_DIR}/open_source/wpa_supplicant/src/eap_server/eap_server_wsc.c
		# 企业级
		${ROOT_DIR}/open_source/wpa_supplicant/src/eap_peer/eap_tls.c
		${ROOT_DIR}/open_source/wpa_supplicant/src/eap_peer/eap_tls_common.c
		${ROOT_DIR}/open_source/wpa_supplicant/liteos_wpa_src/tls_mbedtls.c
		# WAPI
		${ROOT_DIR}/middleware/services/wifi_service/wpa/wapi/wapi.c
		${ROOT_DIR}/middleware/services/wifi_service/wpa/wapi/wai_sm.c
		${ROOT_DIR}/middleware/services/wifi_service/wpa/wapi/wai_rxtx.c
		${ROOT_DIR}/middleware/services/wifi_service/wpa/wapi/wai_crypto_mbedtls.c
		${ROOT_DIR}/middleware/services/wifi_service/wpa/liteos_wpa_api/wapi_api.c
		# 11r
		${ROOT_DIR}/open_source/wpa_supplicant/src/rsn_supp/wpa_ft.c
		# mbo
		${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/mbo.c
		${ROOT_DIR}/open_source/wpa_supplicant/wpa_supplicant/wnm_sta.c
		${ROOT_DIR}/open_source/wpa_supplicant/src/ap/mbo_ap.c
		${ROOT_DIR}/open_source/wpa_supplicant/src/ap/wnm_ap.c
    )
	list(APPEND PRIVATE_DEFINES
		CONFIG_SAE_NO_PW_ID
	)
endif()

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
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

if("UPDATE_WIFI_STATIC_LIB" IN_LIST DEFINES)
    set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/wifi/${TARGET_COMMAND})
endif()

build_component()

install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/src/tls/libtommath.c "*")

set(COMPONENT_NAME "enterprise_cert")

set(SOURCES
    ${ROOT_DIR}/middleware/services/wifi_service/wpa/liteos_wpa_api/enterprise_cert.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
    LOS_CONFIG_WPA_ENTERPRISE
)

if(${SMALLER})
    list(REMOVE_ITEM PRIVATE_DEFINES
		LOS_CONFIG_WPA_ENTERPRISE
    )
    list(REMOVE_ITEM SOURCES
		${ROOT_DIR}/middleware/services/wifi_service/wpa/liteos_wpa_api/enterprise_cert.c
    )
	list(APPEND PRIVATE_DEFINES
	)
endif()

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

if("UPDATE_WIFI_STATIC_LIB" IN_LIST DEFINES)
    set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/wifi/${TARGET_COMMAND})
endif()

build_component()