#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

set(WIFI_DEVICE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(Wi-Fi-ROM-PUBLIC_HEADER
    ${WIFI_DEVICE_DIR}/sample/ws63

    ${WIFI_DEVICE_DIR}/source/inc/romable
    ${WIFI_DEVICE_DIR}/source/inc/romable/msg
    ${WIFI_DEVICE_DIR}/source/inc/romable/alg
    ${WIFI_DEVICE_DIR}/source/inc/romable/device_common
    ${WIFI_DEVICE_DIR}/source/inc/romable/hal/ws63
    ${WIFI_DEVICE_DIR}/source/inc/romable/hal/ws63/dscr
    ${WIFI_DEVICE_DIR}/source/inc/romable/hal/ws63/mac
    ${WIFI_DEVICE_DIR}/source/inc/romable/hal/ws63/phy
    ${WIFI_DEVICE_DIR}/source/inc/romable/hal/ws63/rf
    ${WIFI_DEVICE_DIR}/source/inc/romable/hal/ws63/spec
    ${WIFI_DEVICE_DIR}/source/device/frw/romable

    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/fe_extern

    ${WIFI_DEVICE_DIR}/source/device/hcc/romable/inc

    ${WIFI_DEVICE_DIR}/source/device/inc/romable
    ${WIFI_DEVICE_DIR}/source/device/inc/romable/hal
    ${WIFI_DEVICE_DIR}/source/device/mac/romable
    ${WIFI_DEVICE_DIR}/source/device/forward/romable
    ${WIFI_DEVICE_DIR}/source/device/dmac/romable
    ${WIFI_DEVICE_DIR}/source/device/hal/hal_ws63/romable
)

set(Wi-Fi-PUBLIC_HEADER
    ${WIFI_DEVICE_DIR}/sample/ws63
    ${WIFI_DEVICE_DIR}/source/common
    ${WIFI_DEVICE_DIR}/source/common/cali
    ${WIFI_DEVICE_DIR}/source/common/cali/pktram
    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/calibrate/offline_cali
    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/calibrate/online_cali
    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/calibrate/outer_if
    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/calibrate/pub_if
    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/spec/ws63
    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/calibrate/common_utils
    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/calibrate/cali_comp
    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/calibrate/cali_debug
    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/fe_extern
    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/fe_hal/phy
    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/fe_hal/phy/ws63
    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/fe_hal/rf
    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/power_ctrl
    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/rf_res_manage
    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/rf_customize
    ${WIFI_DEVICE_DIR}/source/common/wifi_fe/equipment

    ${WIFI_DEVICE_DIR}/source/inc
    ${WIFI_DEVICE_DIR}/source/inc/romable
    ${WIFI_DEVICE_DIR}/source/inc/romable/msg
    ${WIFI_DEVICE_DIR}/source/inc/romable/alg
    ${WIFI_DEVICE_DIR}/source/inc/romable/device_common
    ${WIFI_DEVICE_DIR}/source/inc/romable/hal/ws63
    ${WIFI_DEVICE_DIR}/source/inc/romable/hal/ws63/dscr
    ${WIFI_DEVICE_DIR}/source/inc/romable/hal/ws63/mac
    ${WIFI_DEVICE_DIR}/source/inc/romable/hal/ws63/phy
    ${WIFI_DEVICE_DIR}/source/inc/romable/hal/ws63/rf
    ${WIFI_DEVICE_DIR}/source/inc/romable/hal/ws63/spec
    ${WIFI_DEVICE_DIR}/source/device/frw/romable
)
# ASIC和FPGA编译文件有差异
if(BOARD_ASIC IN_LIST DEFINES)
    list(APPEND Wi-Fi-PUBLIC_HEADER
        ${WIFI_DEVICE_DIR}/source/common/wifi_fe/fe_hal/rf/ws63
    )
else()
    list(APPEND Wi-Fi-PUBLIC_HEADER
        ${WIFI_DEVICE_DIR}/source/common/wifi_fe/fe_hal/rf/mpw
    )
endif()
set(Wi-Fi-PRIVATE_HEADER
    ${CMAKE_SOURCE_DIR}/middleware/utils/hcc/inc
    ${CMAKE_SOURCE_DIR}/middleware/utils/hcc/comm
    ${CMAKE_SOURCE_DIR}/middleware/utils/hcc/cfg
    ${CMAKE_SOURCE_DIR}/kernel/osal/include
    ${CMAKE_SOURCE_DIR}/kernel/osal_adapt/inc

    #watch dog
    ${CMAKE_SOURCE_DIR}/drivers/drivers/driver/watchdog

    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/common/customize
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/feature/interface
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/forward
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/frw
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/hmac
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/inc/liteOS
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/inc/frw
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/inc/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/inc/oal
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/inc/oal/liteOS
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/inc/oal/liteOS/pm
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/inc/mac
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/mac
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/oal
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/version
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/forward/liteOS
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/main/liteOS
    ${CMAKE_CURRENT_SOURCE_DIR}/source/host/wal/release/liteOS

    ${WIFI_DEVICE_DIR}/source/device/hcc/romable/inc
    ${WIFI_DEVICE_DIR}/source/device/inc
    ${WIFI_DEVICE_DIR}/source/device/inc/romable
    ${WIFI_DEVICE_DIR}/source/device/inc/romable/hal
    ${WIFI_DEVICE_DIR}/source/device/mac/romable
    ${WIFI_DEVICE_DIR}/source/device/main
    ${WIFI_DEVICE_DIR}/source/device/forward
    ${WIFI_DEVICE_DIR}/source/device/forward/romable
    ${WIFI_DEVICE_DIR}/source/device/inc
    ${WIFI_DEVICE_DIR}/source/device/dmac
    ${WIFI_DEVICE_DIR}/source/device/dmac/romable
    ${WIFI_DEVICE_DIR}/source/device/alg
    ${WIFI_DEVICE_DIR}/source/device/dmac
    ${WIFI_DEVICE_DIR}/source/device/dmac/romable
    ${WIFI_DEVICE_DIR}/source/device/mac
    ${WIFI_DEVICE_DIR}/source/device/mac/romable
    ${WIFI_DEVICE_DIR}/source/device/forward
    ${WIFI_DEVICE_DIR}/source/device/forward/romable
    ${WIFI_DEVICE_DIR}/source/device/frw/romable
    ${WIFI_DEVICE_DIR}/source/device/main
    ${WIFI_DEVICE_DIR}/source/device/frw/romable
    ${WIFI_DEVICE_DIR}/source/device/mac/romable
    ${WIFI_DEVICE_DIR}/source/device/forward/romable
    ${WIFI_DEVICE_DIR}/source/device/hal/hal_ws63/romable
    ${WIFI_DEVICE_DIR}/source/device/hal/hal_ws63/src
)

string(REPLACE "\${ROOT_DIR}" "${ROOT_DIR}" CONFIG_LWIP_COMPILE_PATH "${CONFIG_LWIP_COMPILE_PATH}")
if (EXISTS ${CONFIG_LWIP_COMPILE_PATH}/src/include)
        list(APPEND Wi-Fi-PRIVATE_HEADER ${CONFIG_LWIP_COMPILE_PATH}/src/include)
else()
        list(APPEND Wi-Fi-PRIVATE_HEADER ${ROOT_DIR}/open_source/lwip/lwip_v2.1.3/src/include)
endif()

set(Wi-Fi-PRIVATE_DEFINES
    _PRE_WLAN_FEATURE_CENTRALIZE
    _PRE_WLAN_FEATURE_WS63
    _PRE_BIG_CPU_ENDIAN=0
    _PRE_CHIP_BITS_MIPS32=0
    _PRE_CHIP_BITS_MIPS64=1
    _PRE_CHIP_BITS=_PRE_CHIP_BITS_MIPS32
    _PRE_CONFIG_CONN_SOFTWDFT
    _PRE_CONFIG_PANIC_DUMP_SUPPORT
    _PRE_CONFIG_TARGET_PRODUCT=_PRE_TARGET_PRODUCT_TYPE_1102COMMON
    _PRE_CONFIG_WLAN_THRANS_THREAD_DEBUG
    _PRE_CPU_ENDIAN=_PRE_LITTLE_CPU_ENDIAN
    _PRE_DBAC_HI_TIMERX=0
    _PRE_DBAC_MDRV_TIMER=1
    _PRE_DBAC_OFFLOAD_TIMER=2
    _PRE_DEBUG_MODE_USER_TRACK
    _PRE_FRW_FEATURE_PROCCESS_ENTITY_TASKLET=1
    _PRE_FRW_FEATURE_PROCCESS_ENTITY_THREAD=0
    _PRE_FRW_FEATURE_PROCCESS_ENTITY_TYPE=_PRE_FRW_FEATURE_PROCCESS_ENTITY_THREAD
    _PRE_HW_DEVICE_REGISTER_WITP=0
    _PRE_LITTLE_CPU_ENDIAN=1
    _PRE_MAX_WIDTH_20M=0
    _PRE_MAX_WIDTH_40M=1
    _PRE_MAX_WIDTH_80M=2
    _PRE_MEM_DEBUG
    _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC=3
    _PRE_MULTI_CORE_MODE_OFFLOAD_HMAC=2
    _PRE_MULTI_CORE_MODE_PIPELINE_AMP=1
    _PRE_MULTI_CORE_MODE_SMP=0
    _PRE_MULTI_CORE_MODE=_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC
    _PRE_OAL_FEATURE_SDT
    _PRE_PLAT_FEATURE_CUSTOMIZE
    _PRE_PM_TBTT_OFFSET_PROBE
    _PRE_PRODUCT_ID_MP13_HOST=6
    _PRE_PRODUCT_ID_WITP=0
    _PRE_PRODUCT_ID=_PRE_PRODUCT_ID_MP13_HOST
    _PRE_PSM_DEBUG_MODE
    _PRE_SUPPORT_DFS
    _PRE_TARGET_PRODUCT_TYPE_1102COMMON=2
    _PRE_TARGET_PRODUCT_TYPE_5610DMB=1
    _PRE_TARGET_PRODUCT_TYPE_5610EVB=0
    _PRE_TARGET_PRODUCT_TYPE_5630HERA=7
    _PRE_TARGET_PRODUCT_TYPE_CPE=8
    _PRE_TARGET_PRODUCT_TYPE_E5=5
    _PRE_TARGET_PRODUCT_TYPE_ONT=6
    _PRE_TARGET_PRODUCT_TYPE_VSPM310DMB=3
    _PRE_TARGET_PRODUCT_TYPE_WS835DMB=4
    _PRE_TEST_MODE_BOARD_ST=3
    _PRE_TEST_MODE_OFF=0
    _PRE_TEST_MODE_ST=2
    _PRE_TEST_MODE_UT=1
    _PRE_TEST_MODE=_PRE_TEST_MODE_OFF
    _PRE_WLAN_11AX_20M_ONLY
    _PRE_WLAN_WS86_FPGA
    _PRE_WLAN_802_11A=4
    _PRE_WLAN_802_11AC=16
    _PRE_WLAN_802_11B=1
    _PRE_WLAN_802_11G=2
    _PRE_WLAN_802_11N=8
    _PRE_WLAN_CFGID_DEBUG
    _PRE_WLAN_CHIP_ASIC=2
    _PRE_WLAN_CHIP_FPGA=3
    _PRE_WLAN_CHIP_SIM=_PRE_WLAN_REAL_CHIP
    _PRE_WLAN_CHIP_VERSION=_PRE_WLAN_CHIP_ASIC
    _PRE_WLAN_DFT_DUMP_DSCR
    _PRE_WLAN_DFT_IRQ_STAT
    _PRE_WLAN_DFT_STAT
    _PRE_WLAN_SUPPORT_CCPRIV_CMD
    _PRE_WLAN_FEATRUE_DBAC_TIMER=_PRE_DBAC_OFFLOAD_TIMER
    _PRE_WLAN_FEATURE_11AX
    _PRE_WLAN_FEATURE_11AX_ER_SU_DCM
    _PRE_WLAN_FEATURE_11D
    _PRE_WLAN_FEATURE_11K
    _PRE_WLAN_FEATURE_11R
    _PRE_WLAN_FEATURE_11V
    _PRE_WLAN_FEATURE_ACS
    _PRE_WLAN_FEATURE_ALG_CFG
    _PRE_WLAN_FEATURE_SDP
    _PRE_WLAN_FEATURE_ALWAYS_TX
    _PRE_WLAN_FEATURE_DAQ
    _PRE_WLAN_FEATURE_AMPDU
    _PRE_WLAN_FEATURE_AMPDU_TX_HW
    _PRE_WLAN_FEATURE_AMSDU
    _PRE_WLAN_FEATURE_BLACKLIST
#    _PRE_WLAN_FEATURE_BTCOEX
#    _PRE_WLAN_FEATURE_BT_SUPPORT
    _PRE_WLAN_LATENCY_STAT
    _PRE_WLAN_FEATURE_CCPRIV
    _PRE_WLAN_FEATURE_CLASSIFY
    _PRE_WLAN_FEATURE_COMP_TEMP
    _PRE_WLAN_FEATURE_CUSTOM_SECURITY
    _PRE_WLAN_FEATURE_DEV_RX_DSCR_CTL
    _PRE_WLAN_FEATURE_DYNAMIC_BANDWIDTH
    _PRE_WLAN_FEATURE_FRW_DEBUG
    _PRE_WLAN_FEATURE_HISTREAM
    _PRE_WLAN_FEATURE_INTRF_MODE
    _PRE_WLAN_FEATURE_MAC_PARSE_TIM
    _PRE_WLAN_FEATURE_MEM_OPT
    _PRE_WLAN_FEATURE_MIDDLE_GI
    _PRE_WLAN_FEATURE_MVAP_SCH
    _PRE_WLAN_FEATURE_NO_FRM_INT
    _PRE_WLAN_FEATURE_NSS_MODE=_PRE_WLAN_SUPPORT_SINGLE_NSS
    #_PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
    _PRE_WLAN_FEATURE_PM
    _PRE_WLAN_FEATURE_PMF
    _PRE_WLAN_FEATURE_PSM
    _PRE_WLAN_FEATURE_ROAM
    _PRE_WLAN_FEATURE_SCH_STRATEGY_PROPO_FAIR
    _PRE_WLAN_FEATURE_SCH_STRATEGY_ROUND_ROBIN
    _PRE_WLAN_FEATURE_SPECIAL_PKT_LOG
    _PRE_WLAN_FEATURE_STA_PM
    _PRE_WLAN_FEATURE_STA_UAPSD
    _PRE_WLAN_FEATURE_TPC_TX_DSCR_BIT20=1
    _PRE_WLAN_FEATURE_TPC_TX_DSCR_BIT8=0
    _PRE_WLAN_FEATURE_TPC_TX_DSCR_BITX=_PRE_WLAN_FEATURE_TPC_TX_DSCR_BIT20
    _PRE_WLAN_FEATURE_TWT
    _PRE_WLAN_FEATURE_UAPSD
    _PRE_WLAN_FEATURE_USER_RESP_POWER
    _PRE_WLAN_FEATURE_VOWIFI
    _PRE_WLAN_FEATURE_WPA3
    _PRE_WLAN_PHY_IRQ_ENABLE
    _PRE_WLAN_PROTOCAL_VER=_PRE_WLAN_802_11B+_PRE_WLAN_802_11G+_PRE_WLAN_802_11A+_PRE_WLAN_802_11N
    _PRE_WLAN_RATE_RTS
    _PRE_WLAN_REAL_CHIP=1
    _PRE_WLAN_REG_DEBUG
    _PRE_WLAN_SIM_CHIP=0
    _PRE_WLAN_SUPPORT_DOUBLE_NSS=1
    _PRE_WLAN_SUPPORT_FOUR_NSS=3
    _PRE_WLAN_SUPPORT_SINGLE_NSS=0
    _PRE_WLAN_SUPPORT_TRIPLE_NSS=2
    _PRE_WLAN_FEATURE_WIDTH_MODE=_PRE_MAX_WIDTH_40M
    _PRE_WLAN_FEATURE_APF
    _PRE_WLAN_FEATURE_CSA
    _PRE_WLAN_FEATURE_SLP
    _PRE_WLAN_FEATURE_MBO
    _PRE_WLAN_FEATURE_SR
    _PRE_WLAN_FEATURE_BSRP
    _PRE_WLAN_FEATURE_SINGLE_RF_RX_BCN
    _PRE_WLAN_FEATURE_SINGLE_PROXYSTA
    _PRE_WLAN_FEATURE_STA_DNB
    _PRE_WLAN_SMOOTH_PHASE
    _PRE_WLAN_FEATURE_STAT
    _PRE_WLAN_FEATURE_PK_MODE
    _PRE_WLAN_FEATURE_ANT_SWITCH
    _PRE_WLAN_FEATURE_20_40_80_COEXIST
    _PRE_WLAN_FEATURE_CSI
    _PRE_WLAN_FEATURE_DBDC
    _PRE_WLAN_FEATURE_GCMP_256_CCMP256
    _PRE_WLAN_PLAT_WS83
    _PRE_WLAN_MP13_DDC_BUGFIX
    _PRE_WLAN_MP13_TCP_SMALL_QUEUE_BUGFIX
    _PRE_WLAN_DEBUG_REG_PERIOD_REPORT
    _PRE_WLAN_FEATURE_PSD
    _PRE_WLAN_FEATURE_M2U
    CONFIG_SPIN_LOCK_MAGIC_DEBUG
    HH503_DEV_SW_VERSION_WIFI="xxx"
    HH503_WIFI
    HH503_WL_BUS=WL_BUS_SDIO
    WL_BUS_PCIE=1
    WL_BUS_SDIO=0
    WL_BUS_USB=2
    RELEASE_TYPE=DEBUG
    _PRE_DFX_USER_CONNECT_INFO_RECORD
    _PRE_FRW_EVENT_PROCESS_TRACE_DEBUG
    _PRE_FRW_TIMER_BIND_CPU
    _PRE_OAL_FEATURE_KEY_PROCESS_TRACE
    _PRE_OAL_FEATURE_TASK_NEST_LOCK
    _PRE_PRODUCT_ID_HOST
    CONFIG_LAMBORGHINI_DEVICE
    CONFIG_NON_OS
    LOSCFG_PLATFORM_BSP_RISCV_PLIC
    CONFIG_SUPPORT_TSENSOR_VERSION_D
    CONFIG_SYSERR_INFO_SUPPORT_PRINT
    CONFIG_SUPPORT_SAMPLE_DATA
    FPGA_3883
    SECUREC_ENABLE_SPRINTF_FLOAT=0
    SECUREC_HAVE_WCTOMB=0
    BOARD_ES0
    SUPPORT_CPUP_SET_CONFIG
    CONFIG_WPS_SUPPORT
    CONFIG_P2P_SUPPORT
    #DIAG_PRINT
    CONFIG_SUPPORT_NEW_DIAG
    CONFIG_IPERF_SUPPORT
    CUSTOM_AT_COMMAND
    CONFIG_AT_COMMAND
    CONFIG_SUPPORT_TSENSOR_INTERRUPT_ENABLE
    CONFIG_SUPPORT_TSENSOR_MULTILEVEL_ENABLE
    CONFIG_SUPPORT_TSENSOR_OVERTEMP_PROTECT
    CONFIG_SUPPORT_TSENSOR_CALIBRATION
    CONFIG_SUPPORT_SAMPLE_USB
    CONFIG_SUPPORT_GPIOKEYS
    CONFIG_SUPPORT_TSENSOR_VERSION_B
    CONFIG_SUPPORT_NV_REMOTE_CORE
    PBUF_ZERO_COPY_RESERVE=80
    PBUF_ZERO_COPY_TAILROOM=4
    CONFIG_HCC_SUPPORT_IPC
    CONFIG_BUS_MSG_FLOWCTRL
    DMAC_ON_HOST
    CONFIG_SPI_SUPPORT_DMA
    CONFIG_BOOT_SENSOR
    CONFIG_BOOT_PCORE
    CONFIG_SUPPORT_MOCKED_SHELL
    CONFIG_SYSERR_SUPPORT_CRASH_DUMP
    CONFIG_SOC_SENSOR_SUPPORT
    _PRE_WLAN_FEATURE_WS92_MERGE
    CUSTOMIZE_NV_WS92
    _PRE_OS_VERSION_LINUX=0
    _PRE_OS_VERSION_WIN32=1
    _PRE_OS_VERSION_WINDOWS=2
    _PRE_OS_VERSION_RAW=3
    _PRE_OS_VERSION_HiRTOS=4
    _PRE_OS_VERSION_WIN32_RAW=5
    _PRE_OS_VERSION_LITEOS=6
    _PRE_OS_VERSION=_PRE_OS_VERSION_LITEOS
    _PRE_WLAN_ONLINE_CALI
    _PRE_WLAN_PKT_DEBUG
    _PRE_WLAN_FEATURE_ANT_SEL
    _PRE_WLAN_ALG_UART_PRINT
    _PRE_WLAN_FEATURE_LDPC_TX_ONLY
    _PRE_WLAN_SMALL_MEMORY
    _PRE_WLAN_FEATURE_LOCAL_BRIDGE
    _PRE_LWIP_ZERO_COPY_MALLOC_SKB

#   以下是非重要，可关闭的宏，其它宏请加载此宏的前面
    _PRE_WIFI_DEBUG
    #_PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN
    _PRE_WIFI_PRINTK
)

if(BOARD_ASIC IN_LIST DEFINES)
    list(APPEND Wi-Fi-PRIVATE_DEFINES
        BOARD_ASIC
        BOARD_ASIC_WIFI
    )
else()
    list(APPEND Wi-Fi-PRIVATE_DEFINES
        BOARD_FPGA
        BOARD_FPGA_WIFI
        _PRE_WLAN_03_MPW_RF
    )
endif()

set(Wi-Fi-PUBLIC_DEFINES
)

if("__ALIOS__" IN_LIST DEFINES OR "__FREERTOS__" IN_LIST DEFINES)
    list (APPEND Wi-Fi-PRIVATE_DEFINES
        FREERTOS_DEFINE
    )
endif()

set(Wi-Fi-WHOLE_LINK
    true
)

set(Wi-Fi-MAIN_COMPONENT
    false
)

macro(add_base_config)
    set(PUBLIC_HEADER            ${Wi-Fi-PUBLIC_HEADER})
    set(PRIVATE_HEADER           ${Wi-Fi-PRIVATE_HEADER})
    set(PRIVATE_DEFINES          ${Wi-Fi-PRIVATE_DEFINES})
    set(PUBLIC_DEFINES           ${Wi-Fi-PUBLIC_DEFINES})
    set(COMPONENT_PUBLIC_CCFLAGS ${Wi-Fi-COMPONENT_PUBLIC_CCFLAGS})
    set(COMPONENT_CCFLAGS        ${Wi-Fi-COMPONENT_CCFLAGS})
    set(WHOLE_LINK               ${Wi-Fi-WHOLE_LINK})
    set(MAIN_COMPONENT           ${Wi-Fi-MAIN_COMPONENT})
    if("UPDATE_WIFI_STATIC_LIB" IN_LIST DEFINES)
        set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/wifi/${TARGET_COMMAND})
    endif()
endmacro()

macro(add_rom_base_config)
    set(PUBLIC_HEADER            ${Wi-Fi-ROM-PUBLIC_HEADER})
    set(PRIVATE_DEFINES          ${Wi-Fi-PRIVATE_DEFINES})
    set(PUBLIC_DEFINES           ${Wi-Fi-PUBLIC_DEFINES})
    set(WHOLE_LINK               ${Wi-Fi-WHOLE_LINK})
    set(MAIN_COMPONENT           ${Wi-Fi-MAIN_COMPONENT})
    set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/wifi/${TARGET_COMMAND})
endmacro()