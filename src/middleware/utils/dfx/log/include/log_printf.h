/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: log printf interface
 * Author:
 * Create: 2018-12-01
 */

/*
 * log example:
 *
 * #define MUX_PRINT_INFO(log_id, fmt, count, args...) \
 *    BASE_PRINT_INFO(CONNECT(LOG_OS_MUTEX, log_id), fmt, count, ##args)
 *
 * coding: there must be a param for the count of % in fmt
 * MUX_PRINT_INFO(log_num, "this is an example %d", ONE_ARG, __LINE__);
 *
 */
#ifndef LOG_PRINTF_H
#define LOG_PRINTF_H

#include <stdarg.h>
#include <stdint.h>
#include "common_def.h"
#include "chip_definitions.h"
#include "log_types.h"
#include "platform_types.h"
#include "dfx_feature_config.h"
#if (defined (BUILD_APPLICATION_STANDARD) || defined (TEST_SUITE) || CORE == CONTROL_CORE)
#ifdef SUPPORT_IPC
#include "ipc_actions.h"
#endif
#include "core.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/**
 * @defgroup connectivity_libs_log LOG
 * @ingroup  connectivity_libs
 * @{
 */
#define BTC_MAGIC_LOG_CODE   0xCBCBA
#define CONNECT(log_mod, log_id)   (((log_mod) << 10) | (log_id))

enum log_args_number {
    NO_ARG,
    ONE_ARG,
    TWO_ARG,
    THREE_ARG,
    FOUR_ARG,
    FIVE_ARG,
    SIX_ARG,
    SEVEN_ARG,
};

typedef struct {
    uint32_t log_id : 10;
    uint32_t sub_module : 6;
    uint32_t module : 4;
    uint32_t domain : 4;
    uint32_t reserved : 8;
} compress_log_code_t;

/*
 * +   4 Bits   +   4 Bits   +     6 Bits     +
 * +------------+------------+----------------+
 * |   domain   |   module   |   sub module   |
 * +------------+------------+----------------+
 */
#define LOG_DOMAIN_MAX       12 // each core 4 domains
#define LIB_LOG_MODULE_MAX   16
#define LOG_SUB_MODULE_MAX   32
#define log_sub_module_base(domain, module)   (((uint32_t)(domain) << 10) | ((uint32_t)(module) << 6))

enum log_domain_e {
    LOG_DOMAIN_ACORE_BEGIN = 0,
    LOG_DOMAIN_ACORE_PLT = LOG_DOMAIN_ACORE_BEGIN,
    LOG_DOMAIN_ACORE_DEV,
    LOG_DOMAIN_ACORE_APP,
    LOG_DOMAIN_ACORE_END,

    LOG_DOMAIN_BCORE_BEGIN = 4,
    LOG_DOMAIN_BCORE_PLT = LOG_DOMAIN_BCORE_BEGIN,
    LOG_DOMAIN_BCORE_BTH,
    LOG_DOMAIN_BCORE_BTC,
    LOG_DOMAIN_BCORE_BTS,
    LOG_DOMAIN_BCORE_END,

    LOG_DOMAIN_DCORE_BEGIN = 8,
    LOG_DOMAIN_DCORE_END,
};

/* --------------------- module enums --------------------- */
enum log_domain_acore_plt_module_e {
    LOG_DOMAIN_ACORE_PLT_MODULE_OS,
    LOG_DOMAIN_ACORE_PLT_MODULE_BSP_COMMON,
    LOG_DOMAIN_ACORE_PLT_MODULE_BSP_DRIVER,
    LOG_DOMAIN_ACORE_PLT_MODULE_BSP_LIB,
    LOG_DOMAIN_ACORE_PLT_MODULE_FWK,
    LOG_DOMAIN_ACORE_PLT_MODULE_UTILS,
    LOG_DOMAIN_ACORE_PLT_MODULE_FS,
    LOG_DOMAIN_ACORE_PLT_MODULE_HAUI,
    LOG_DOMAIN_ACORE_PLT_MODULE_MAX,
};

enum log_domain_acore_dev_module_e {
    LOG_DOMAIN_ACORE_DEV_MODULE_AUDIO,
    LOG_DOMAIN_ACORE_DEV_MODULE_BT,
    LOG_DOMAIN_ACORE_DEV_MODULE_SENSORS,
    LOG_DOMAIN_ACORE_DEV_MODULE_MAX,
};

enum log_domain_acore_app_module_e {
    LOG_DOMAIN_ACORE_APP_MODULE_UX,
    LOG_DOMAIN_ACORE_APP_MODULE_MANU,
    LOG_DOMAIN_ACORE_APP_MODULE_FOTA,
    LOG_DOMAIN_ACORE_APP_MODULE_MT,
    LOG_DOMAIN_ACORE_APP_MODULE_POWER,
    LOG_DOMAIN_ACORE_APP_MODULE_LPC,
    LOG_DOMAIN_ACORE_APP_MODULE_SENSORS,
    LOG_DOMAIN_ACORE_APP_MODULE_MAX,
};

enum log_domain_bcore_plt_module_e {
    LOG_DOMAIN_BCORE_PLT_MODULE_DRIVER,
    LOG_DOMAIN_BCORE_PLT_MODULE_LIB,
    LOG_DOMAIN_BCORE_PLT_MODULE_INFO,
    LOG_DOMAIN_BCORE_PLT_MODULE_LPM,
    LOG_DOMAIN_BCORE_PLT_MODULE_MAX,
};

enum log_domain_bcore_bth_module_e {
    LOG_DOMAIN_BCORE_BTH_MODULE_TL_ADAPTER,
    LOG_DOMAIN_BCORE_BTH_MODULE_STACK,
    LOG_DOMAIN_BCORE_BTH_MODULE_STACK_ADAPTER,
    LOG_DOMAIN_BCORE_BTH_MODULE_SERVICE,
    LOG_DOMAIN_BCORE_BTH_MODULE_MAX,
};

enum log_domain_bcore_btc_module_e {
    LOG_DOMAIN_BCORE_BTC_MODULE_NORMAL,
    LOG_DOMAIN_BCORE_BTC_MODULE_STATE,
    LOG_DOMAIN_BCORE_BTC_MODULE_MESG,
    LOG_DOMAIN_BCORE_BTC_MODULE_MAX,
};

enum log_domain_bcore_bts_module_e {
    LOG_DOMAIN_BCORE_BTS_MODULE_WEAR,
    LOG_DOMAIN_BCORE_BTS_MODULE_MAX,
};

/* --------------------- sub module enums --------------------- */
enum log_domain_acore_plt_module_os_sub_mudole_e {
    LOG_ACORE_PLT_OS_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_PLT, LOG_DOMAIN_ACORE_PLT_MODULE_OS),
    LOG_OS_TASK = LOG_ACORE_PLT_OS_BEGIN,
    LOG_OS_TIMER,
    LOG_OS_MUTEX,
    LOG_OS_SEM,
    LOG_OS_MAIL,
    LOG_OS_MEM,
    LOG_ACORE_PLT_OS_END,
};

enum log_domain_acore_plt_module_bsp_common_sub_mudole_e {
    LOG_ACORE_PLT_BSP_COMMON_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_PLT, LOG_DOMAIN_ACORE_PLT_MODULE_BSP_COMMON),
    LOG_ACORE_PLT_BSP = LOG_ACORE_PLT_BSP_COMMON_BEGIN,
    LOG_ACORE_PLT_BSP_COMMON_END,
};

enum log_domain_acore_plt_module_bsp_driver_sub_mudole_e {
    LOG_ACORE_PLT_BSP_DRIVER_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_PLT, LOG_DOMAIN_ACORE_PLT_MODULE_BSP_DRIVER),
    LOG_ACORE_PLT_BSP_DRIVER_CLOCK = LOG_ACORE_PLT_BSP_DRIVER_BEGIN,
    LOG_ACORE_PLT_BSP_DRIVER_DMA,
    LOG_ACORE_PLT_BSP_DRIVER_EFLASH,
    LOG_ACORE_PLT_BSP_DRIVER_I2C,
    LOG_ACORE_PLT_BSP_DRIVER_SPI,
    LOG_ACORE_PLT_BSP_DRIVER_UART,
    LOG_ACORE_PLT_BSP_DRIVER_XIP,
    LOG_ACORE_PLT_BSP_DRIVER_ADC,
    LOG_ACORE_PLT_BSP_DRIVER_CAPSENSOR,
    LOG_ACORE_PLT_BSP_DRIVER_GPIO,
    LOG_ACORE_PLT_BSP_DRIVER_IPC,
    LOG_ACORE_PLT_BSP_DRIVER_LPC,
    LOG_ACORE_PLT_BSP_DRIVER_MEM,
    LOG_ACORE_PLT_BSP_DRIVER_MPU,
    LOG_ACORE_PLT_BSP_DRIVER_OTP,
    LOG_ACORE_PLT_BSP_DRIVER_PMU,
    LOG_ACORE_PLT_BSP_DRIVER_PWM,
    LOG_ACORE_PLT_BSP_DRIVER_QSPI,
    LOG_ACORE_PLT_BSP_DRIVER_REBOOT,
    LOG_ACORE_PLT_BSP_DRIVER_RTC,
    LOG_ACORE_PLT_BSP_DRIVER_SYSSTATUS,
    LOG_ACORE_PLT_BSP_DRIVER_SYSTICK,
    LOG_ACORE_PLT_BSP_DRIVER_TCXO,
    LOG_ACORE_PLT_BSP_DRIVER_TIMER,
    LOG_ACORE_PLT_BSP_DRIVER_TSENSOR,
    LOG_ACORE_PLT_BSP_DRIVER_WATCHDOG,
    LOG_ACORE_PLT_BSP_DRIVER_END,
};

enum log_domain_acore_plt_module_bsp_lib_sub_mudole_e {
    LOG_ACORE_PLT_BSP_LIB_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_PLT, LOG_DOMAIN_ACORE_PLT_MODULE_BSP_LIB),
    LOG_ACORE_PLT_BSP_LIB_LOG = LOG_ACORE_PLT_BSP_LIB_BEGIN,
    LOG_ACORE_PLT_BSP_LIB_CHR,
    LOG_ACORE_PLT_BSP_LIB_RPC,
    LOG_ACORE_PLT_BSP_LIB_UTILS,
    LOG_ACORE_PLT_BSP_LIB_CONNECTIVITY,
    LOG_ACORE_PLT_BSP_INFO_STACK,
    LOG_ACORE_PLT_BSP_INFO_HEAP,
    LOG_ACORE_PLT_BSP_LPM_SLEEP,
    LOG_ACORE_PLT_BSP_LPM_VETO,
    LOG_ACORE_PLT_BSP_LPM_SWITCH_CLK,
    LOG_ACORE_PLT_BSP_LPM_WFI,
    LOG_ACORE_PLT_BSP_LIB_END,
};

enum log_domain_acore_plt_module_fwk_sub_mudole_e {
    LOG_ACORE_PLT_FWK_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_PLT, LOG_DOMAIN_ACORE_PLT_MODULE_FWK),
    LOG_FWK_COMMON = LOG_ACORE_PLT_FWK_BEGIN,
    LOG_FWK_COMMU,
    LOG_FWK_AM,
    LOG_FWK_DM,
    LOG_FWK_MISC_DEV,
    LOG_FWK_MISC_SYS,
    LOG_FWK_FDT_GPIO,
    LOG_ACORE_PLT_FWK_END,
};

enum log_domain_acore_plt_module_utils_sub_mudole_e {
    LOG_ACORE_PLT_UTILS_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_PLT, LOG_DOMAIN_ACORE_PLT_MODULE_UTILS),
    LOG_ACORE_PLT_UTILS_COMMON = LOG_ACORE_PLT_UTILS_BEGIN,
    LOG_ACORE_PLT_UTILS_HELP_CODE,
    LOG_ACORE_PLT_UTILS_INICONFIG,
    LOG_ACORE_PLT_UTILS_PROPERTY,
    LOG_ACORE_PLT_UTILS_DAEMON,
    LOG_ACORE_PLT_UTILS_SHELL,
    LOG_ACORE_PLT_UTILS_CIRCLE_QUEUE,
    LOG_ACORE_PLT_UTILS_FILE_EXTEND,
    LOG_ACORE_PLT_UTILS_FLASH_EXTEND,
    LOG_ACORE_PLT_UTILS_EFUSE_EXTEND,
    LOG_ACORE_PLT_UTILS_SAFE_MODE,
    LOG_ACORE_PLT_UTILS_KVFS,
    LOG_ACORE_PLT_UTILS_FDT,
    LOG_ACORE_PLT_UTILS_END,
};

enum log_domain_acore_plt_module_fs_sub_mudole_e {
    LOG_ACORE_PLT_FS_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_PLT, LOG_DOMAIN_ACORE_PLT_MODULE_FS),
    LOG_ACORE_PLT_FS = LOG_ACORE_PLT_FS_BEGIN,
    LOG_ACORE_PLT_FS_END,
};

enum log_domain_acore_plt_module_haui_sub_mudole_e {
    LOG_ACORE_PLT_HAUI_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_PLT, LOG_DOMAIN_ACORE_PLT_MODULE_HAUI),
    LOG_ACORE_PLT_HAUI_SECURE = LOG_ACORE_PLT_HAUI_BEGIN,
    LOG_ACORE_PLT_HAUI_AES,
    LOG_ACORE_PLT_HAUI_END,
};

enum log_domain_acore_dev_module_audio_sub_mudole_e {
    LOG_ACORE_DEV_AUDIO_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_DEV, LOG_DOMAIN_ACORE_DEV_MODULE_AUDIO),
    LOG_ACORE_DRV = LOG_ACORE_DEV_AUDIO_BEGIN,
    LOG_ACORE_DEV_AUDIO_END,
};

enum log_domain_acore_dev_module_bt_sub_mudole_e {
    LOG_ACORE_DEV_BT_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_DEV, LOG_DOMAIN_ACORE_DEV_MODULE_BT),
    LOG_ACORE_DRV_BT,
    LOG_ACORE_DEV_BT_END,
};

enum log_domain_acore_dev_module_sensors_sub_mudole_e {
    LOG_ACORE_DEV_SENSORS_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_DEV, LOG_DOMAIN_ACORE_DEV_MODULE_SENSORS),
    LOG_ACORE_DRV_SENSORS = LOG_ACORE_DEV_SENSORS_BEGIN,
    LOG_ACORE_DRV_GSENSOR,
    LOG_ACORE_DRV_PSENSOR,
    LOG_ACORE_DRV_CAPSENSOR,
    LOG_ACORE_DRV_VKSENSOR,
    LOG_ACORE_DRV_CAPPRESSSENSOR,
    LOG_ACORE_DEV_SENSORS_END,
};

enum log_domain_acore_app_module_ux_sub_mudole_e {
    LOG_ACORE_APP_UX_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_APP, LOG_DOMAIN_ACORE_APP_MODULE_UX),
    LOG_ACORE_APP_UX_COMMON = LOG_ACORE_APP_UX_BEGIN,
    LOG_ACORE_APP_UX_AUDIO,
    LOG_ACORE_APP_UX_BT,
    LOG_ACORE_APP_UX_CONFIG,
    LOG_ACORE_APP_UX_MANAGER,
    LOG_ACORE_APP_UX_BOX,
    LOG_ACORE_APP_UX_CHARGE,
    LOG_ACORE_APP_UX_SENSOR,
    LOG_ACORE_APP_UX_SPP,
    LOG_ACORE_APP_UX_SECURE,
    LOG_ACORE_APP_UX_MULTIPLE_DEVICE,
    LOG_ACORE_APP_UX_END,
};

enum log_domain_acore_app_module_manu_sub_mudole_e {
    LOG_ACORE_APP_MANU_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_APP, LOG_DOMAIN_ACORE_APP_MODULE_MANU),
    LOG_ACORE_APP_MANU_COMMON = LOG_ACORE_APP_MANU_BEGIN,
    LOG_ACORE_APP_MANU_AT_TEST,
    LOG_ACORE_APP_MANU_AGE_TEST,
    LOG_ACORE_APP_MANU_PLAFTORM_TEST,
    LOG_ACORE_APP_MANU_END,
};

enum log_domain_acore_app_module_fota_sub_mudole_e {
    LOG_ACORE_APP_FOTA_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_APP, LOG_DOMAIN_ACORE_APP_MODULE_FOTA),
    LOG_ACORE_APP_FOTA_COMMON = LOG_ACORE_APP_FOTA_BEGIN,
    LOG_FOTA_MAIN,
    LOG_FOTA_MBB,
    LOG_FOTA_PACKAGE,
    LOG_FOTA_LINKER,
    LOG_FOTA_SIGN,
    LOG_FOTA_NOTIF,
    LOG_FOTA_PORT,
    LOG_FOTA_TRANSPROT,
    LOG_ACORE_APP_FOTA_END,
};

enum log_domain_acore_app_module_mt_sub_mudole_e {
    LOG_ACORE_APP_MT_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_APP, LOG_DOMAIN_ACORE_APP_MODULE_MT),
    LOG_ACORE_APP_MT_COMMON = LOG_ACORE_APP_MT_BEGIN,
    LOG_ACORE_APP_MT_LOG_CONFIG,
    LOG_ACORE_APP_MT_MASTER,
    LOG_ACORE_APP_MT_SLAVE,
    LOG_ACORE_APP_MT_BOX,
    LOG_ACORE_APP_MASS_UPLOAD,
    LOG_ACORE_APP_FILE_TRANS,
    LOG_ACORE_APP_MT_END,
};

enum log_domain_acore_app_module_power_sub_mudole_e {
    LOG_ACORE_APP_POWER_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_APP, LOG_DOMAIN_ACORE_APP_MODULE_POWER),
    LOG_ACORE_APP_POWER = LOG_ACORE_APP_POWER_BEGIN,
    LOG_ACORE_APP_POWER_CHARGER,
    LOG_ACORE_APP_POWER_FUELGAUGE,
    LOG_ACORE_APP_POWER_BATTERY,
    LOG_ACORE_APP_POWER_END,
};

enum log_domain_acore_app_module_lpc_sub_mudole_e {
    LOG_ACORE_APP_LPC_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_APP, LOG_DOMAIN_ACORE_APP_MODULE_LPC),
    LOG_ACORE_APP_LPC = LOG_ACORE_APP_LPC_BEGIN,
    LOG_ACORE_APP_LPC_END,
};

enum log_domain_acore_app_module_sensors_sub_mudole_e {
    LOG_ACORE_APP_SENSORS_BEGIN = log_sub_module_base(LOG_DOMAIN_ACORE_APP, LOG_DOMAIN_ACORE_APP_MODULE_SENSORS),
    LOG_ACORE_APP_BASIC_SENSORS = LOG_ACORE_APP_SENSORS_BEGIN,
    LOG_ACORE_APP_GESTURE,
    LOG_ACORE_APP_WEAR,
    LOG_ACORE_APP_PRESS,
    LOG_ACORE_APP_SENSORS_END,
};

enum log_domain_bcore_plt_module_driver_sub_mudole_e {
    LOG_BCORE_PLT_DRIVER_BEGIN = log_sub_module_base(LOG_DOMAIN_BCORE_PLT, LOG_DOMAIN_BCORE_PLT_MODULE_DRIVER),
    LOG_BCORE_PLT = LOG_BCORE_PLT_DRIVER_BEGIN,
    LOG_BCORE_PLT_DRIVER_CLOCK,
    LOG_BCORE_PLT_DRIVER_DMA,
    LOG_BCORE_PLT_DRIVER_EFLASH,
    LOG_BCORE_PLT_DRIVER_I2C,
    LOG_BCORE_PLT_DRIVER_SPI,
    LOG_BCORE_PLT_DRIVER_UART,
    LOG_BCORE_PLT_DRIVER_XIP,
    LOG_BCORE_PLT_DRIVER_ADC,
    LOG_BCORE_PLT_DRIVER_CAPSENSOR,
    LOG_BCORE_PLT_DRIVER_GPIO,
    LOG_BCORE_PLT_DRIVER_IPC,
    LOG_BCORE_PLT_DRIVER_LPC,
    LOG_BCORE_PLT_DRIVER_MEM,
    LOG_BCORE_PLT_DRIVER_MPU,
    LOG_BCORE_PLT_DRIVER_OTP,
    LOG_BCORE_PLT_DRIVER_PMU,
    LOG_BCORE_PLT_DRIVER_PWM,
    LOG_BCORE_PLT_DRIVER_QSPI,
    LOG_BCORE_PLT_DRIVER_REBOOT,
    LOG_BCORE_PLT_DRIVER_RTC,
    LOG_BCORE_PLT_DRIVER_SYSSTATUS,
    LOG_BCORE_PLT_DRIVER_SYSTICK,
    LOG_BCORE_PLT_DRIVER_TCXO,
    LOG_BCORE_PLT_DRIVER_TIMER,
    LOG_BCORE_PLT_DRIVER_TSENSOR,
    LOG_BCORE_PLT_DRIVER_WATCHDOG,
    LOG_BCORE_PLT_DRIVER_IR,
    LOG_BCORE_PLT_DRIVER_END,
};

enum log_domain_bcore_plt_module_lib_sub_mudole_e {
    LOG_BCORE_PLT_LIB_BEGIN = log_sub_module_base(LOG_DOMAIN_BCORE_PLT, LOG_DOMAIN_BCORE_PLT_MODULE_LIB),
    LOG_BCORE_PLT_LIB_LOG = LOG_BCORE_PLT_LIB_BEGIN,
    LOG_BCORE_PLT_LIB_RPC,
    LOG_BCORE_PLT_LIB_UTILS,
    LOG_BCORE_PLT_LIB_CONNECTIVITY,
    LOG_BCORE_PLT_LIB_PANIC,
    LOG_BCORE_PLT_LIB_END,
};

enum log_domain_bcore_plt_module_info_sub_mudole_e {
    LOG_BCORE_PLT_INFO_BEGIN = log_sub_module_base(LOG_DOMAIN_BCORE_PLT, LOG_DOMAIN_BCORE_PLT_MODULE_LPM),
    LOG_BCORE_PLT_INFO_STACK = LOG_BCORE_PLT_INFO_BEGIN,
    LOG_BCORE_PLT_INFO_HEAP,
    LOG_BCORE_PLT_INFO_TRACE,
    LOG_BCORE_PLT_INFO_CPU_CLK,
    LOG_BCORE_PLT_INFO_CODEC_REG,
    LOG_BCORE_PLT_INFO_RST,
    LOG_BCORE_PLT_INFO_OS,
    LOG_BCORE_PLT_INFO_SYS,
    LOG_BCORE_PLT_INFO_END,
};

enum log_domain_bcore_plt_module_lpm_sub_mudole_e {
    LOG_BCORE_PLT_LPM_BEGIN = log_sub_module_base(LOG_DOMAIN_BCORE_PLT, LOG_DOMAIN_BCORE_PLT_MODULE_LPM),
    LOG_BCORE_PLT_LPM = LOG_BCORE_PLT_LPM_BEGIN,
    LOG_BCORE_PLT_LPM_SLEEP,
    LOG_BCORE_PLT_LPM_VETO,
    LOG_BCORE_PLT_LPM_SWITCH_CLK,
    LOG_BCORE_PLT_LPM_WFI,
    LOG_BCORE_PLT_LPM_CODEC_POWER,
    LOG_BCORE_PLT_LPM_DSP_POWER,
    LOG_BCORE_PLT_LPM_CLK0_OUTPUT,
    LOG_BCORE_PLT_LPM_I2S_CLK,
    LOG_BCORE_PLT_LPM_MAX,
    LOG_BCORE_PLT_LPM_END,
};

enum log_domain_bcore_bth_module_tl_adapter_sub_mudole_e {
    LOG_BTH_TL_ADAPTER_BEGIN = log_sub_module_base(LOG_DOMAIN_BCORE_BTH, LOG_DOMAIN_BCORE_BTH_MODULE_TL_ADAPTER),
    LOG_BTH_H2C = LOG_BTH_TL_ADAPTER_BEGIN,
    LOG_BTH_C2H,
    LOG_BTH_TL_ADAPTER_END,
};

enum log_domain_bcore_bth_module_stack_sub_mudole_e {
    LOG_BTH_STACK_BEGIN = log_sub_module_base(LOG_DOMAIN_BCORE_BTH, LOG_DOMAIN_BCORE_BTH_MODULE_STACK),
    LOG_BTH_STACK_GAP = LOG_BTH_STACK_BEGIN,
    LOG_BTH_STACK_SDP,
    LOG_BTH_STACK_SMP,
    LOG_BTH_STACK_A2DP,
    LOG_BTH_STACK_HFP,
    LOG_BTH_STACK_AVRCP,
    LOG_BTH_STACK_SPP,
    LOG_BTH_STACK_GATT,
    LOG_BTH_STACK_AVDTP,
    LOG_BTH_STACK_AVCTP,
    LOG_BTH_STACK_RFCOMM,
    LOG_BTH_STACK_L2CAP,
    LOG_BTH_STACK_HCI,
    LOG_BTH_STACK_ASCP,
    LOG_BTH_STACK_ASMP,
    LOG_BTH_STACK_JITWS,
    LOG_BTH_STACK_SOCTWS,
    LOG_BTH_STACK_DADP,
    LOG_BTH_STACK_FSM,
    LOG_BTH_STACK_COMMON,
    LOG_BTH_STACK_END,
};

enum log_domain_bcore_bth_module_stack_adapter_sub_mudole_e {
    LOG_BTH_STACK_ADPATER_BEGIN = log_sub_module_base(LOG_DOMAIN_BCORE_BTH, LOG_DOMAIN_BCORE_BTH_MODULE_STACK_ADAPTER),
    LOG_BTH_GAP_ADAPTER = LOG_BTH_STACK_ADPATER_BEGIN,
    LOG_BTH_A2DP_ADAPTER,
    LOG_BTH_HFP_ADAPTER,
    LOG_BTH_AVRCP_ADAPTER,
    LOG_BTH_SPP_ADAPTER,
    LOG_BTH_SOCTWS_ADAPTER,
    LOG_BTH_JITWS_ADAPTER,
    LOG_BTH_STACK_ADPATER_END,
};

enum log_domain_bcore_bth_module_service_sub_mudole_e {
    LOG_BTH_SERVICE_BEGIN = log_sub_module_base(LOG_DOMAIN_BCORE_BTH, LOG_DOMAIN_BCORE_BTH_MODULE_SERVICE),
    LOG_BTH_SERVICE_GAP_SERVICE = LOG_BTH_SERVICE_BEGIN,
    LOG_BTH_SERVICE_PROFILE_SERVICE,
    LOG_BTH_SERVICE_CONN_MGR,
    LOG_BTH_SERVICE_SOCTWS_MGR,
    LOG_BTH_SERVICE_JITWS_MGR,
    LOG_BTH_SERVICE_TWS_MGR,
    LOG_BTH_SERVICE_DTS_MGR,
    LOG_BTH_SERVICE_DATA_MGR,
    LOG_BTH_SERVICE_UX_ADPATER,
    LOG_BTH_SERVICE_NEARBY,
    LOG_BTH_SERVICE_ICARRY,
    LOG_BTH_SERVICE_SPA,
    LOG_BTH_SERVICE_IPC,
    LOG_BTH_SERVICE_FACTORY,
    LOG_BTH_SERVICE_GATT_MGR,
    LOG_BTH_SERVICE_END,
};

enum log_domain_bcore_btc_module_normal_sub_mudole_e {
    LOG_BTC_NORMAL_BEGIN = log_sub_module_base(LOG_DOMAIN_BCORE_BTC, LOG_DOMAIN_BCORE_BTC_MODULE_NORMAL),
    LOG_BCORE_BTC = LOG_BTC_NORMAL_BEGIN,
    LOG_BTC_BT_COMMON,
    LOG_BTC_BLE,
    LOG_BTC_ACL,
    LOG_BTC_WLCOEX,
    LOG_BTC_SCO,
    LOG_BTC_WBS,
    LOG_BTC_RS,
    LOG_BTC_SNIFF,
    LOG_BTC_IRQ,
    LOG_BTC_DATA,
    LOG_BTC_SLEEP,
    LOG_BTC_CLOSEME,
    LOG_BTC_CHNLSCAN,
    LOG_BTC_EA,
    LOG_BTC_NORMAL_END,
};

enum log_domain_bcore_btc_module_state_sub_mudole_e {
    LOG_BTC_STATE_BEGIN = log_sub_module_base(LOG_DOMAIN_BCORE_BTC, LOG_DOMAIN_BCORE_BTC_MODULE_STATE),
    BT_STATE_GET_ID = LOG_BTC_STATE_BEGIN,
    BT_STATE_SET_ID,
    BT_SAVED_STATE_ID,
    LOG_BTC_STATE_END,
};

enum log_domain_bcore_btc_module_mesg_sub_mudole_e {
    LOG_BTC_MESG_BEGIN = log_sub_module_base(LOG_DOMAIN_BCORE_BTC, LOG_DOMAIN_BCORE_BTC_MODULE_MESG),
    BTC_BT_MESGID = LOG_BTC_MESG_BEGIN,
    BTC_BT_HCI_EVT,
    BTC_BT_HCI_CMD,
    BTC_BT_SC_CHK,
    BTC_BT_FEAT_GET,
    BTC_BT_LMP_RX_NOR,
    BTC_BT_LMP_RX_EXT,
    BTC_BT_LMP_TX_NOR,
    BTC_BT_LMP_TX_EXT,
    BTC_BT_ACC_GET,
    BTC_BT_ACC_NOT_GET,
    BTC_BT_ACC_EXT_GET,
    BTC_BT_ACC_NOT_EXT_GET,
    BTC_BT_TX_CFM_OP_GET,
    BTC_BT_TX_CFM_EXTOP_GET,
    BTC_BLE_HCI_EVENT,
    BTC_BLE_LLD_RX,
    BTC_BLE_LLCP_RX,
    BTC_BLE_LLCP_TX,
    BTC_BT_CIRCLE_COUNT,
    BTC_BT_CIRCLE_COUNT2,
    LOG_BTC_MESG_END,
};

enum log_domain_bcore_bts_module_wear_sub_mudole_e {
    LOG_BTS_WEAR_BEGIN = log_sub_module_base(LOG_DOMAIN_BCORE_BTS, LOG_DOMAIN_BCORE_BTS_MODULE_WEAR),
    BTS_WEAR_HFP = LOG_BTS_WEAR_BEGIN,
    BTS_WEAR_DEV,
    BTS_WEAR_GDK,
    BTS_WEAR_CONFIG,
    BTS_WEAR_COMMON,
    BTS_WEAR_MPC,
    BTS_WEAR_PBAP,
    BTS_WEAR_A2DP,
    BTS_WEAR_AVRCP,
    BTS_WEAR_SPP,
    BTS_WEAR_IAP,
    BTS_WEAR_IOT,
    BTS_WEAR_BR_SAIS,
    BTS_WEAR_VENDOR,
    BTS_WEAR_BR_CONN_MGR,
    BTS_WEAR_PERIPHERAL_MGR,
    BTS_WEAR_DATA_ROUTE,
    BTS_WEAR_ADV,
    BTS_WEAR_BLE_CONN_MGR,
    BTS_WEAR_BLE_SAIS,
    BTS_WEAR_ALIPAY,
    BTS_WEAR_ANCS,
    BTS_WEAR_AMS,
    BTS_WEAR_HID,
    BTS_WEAR_SOCECHO_SRV,
    BTS_WEAR_BTIF,
    LOG_BTS_WEAR_END,
};

uint32_t get_chr_basegmt_s(void);
void set_chr_basegmt_s(uint32_t t);
uint64_t get_log_basetime_ms(void);

#if (defined (BUILD_APPLICATION_STANDARD) || defined (TEST_SUITE))
#if CORE == APPS
void set_log_time(uint32_t rtc_time_s);
#else
#ifdef IPC_NEW
#else
bool set_log_time_action_handler(ipc_action_t message, const volatile ipc_payload *payload_p, cores_t src, uint32_t id);
#endif
#endif
#endif

#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES)
 /*
 * +    4 Bits    +    4 Bits    +      6 Bits      + 10 Bits  +   4 Bits    +    4 Bits    +
 * +--------------+--------------+------------------+----------+-------------+--------------+
 * |  log_domain  |  log_module  |  log_sub_module  |  log_id  |             |              |
 * |--------------+--------------+------------------+----------|   log_lvl   |  args_count  |
 * |                     log_code<24 Bits>                     |             |              |
 * +-----------------------------------------------------------+-------------+--------------+
 */
#define press_params(log_code, log_lvl, args_count) \
    (((uint32_t)(log_code) << 8) | ((uint32_t)(log_lvl) << 4) | (uint32_t)(args_count))
#define get_log_code(log_header)         (((log_header) >> 8) & 0xFFFFFF)
#define get_log_domain(log_header)       (((log_header) >> 28) & 0xF)
#define get_log_module(log_header)       (((log_header) >> 24) & 0xF)
#define get_log_sub_module(log_header)   (((log_header) >> 18) & 0x3F)
#define get_log_id(log_header)           (((log_header) >> 8) & 0x3FF)
#define get_log_lvl(log_header)          (((log_header) >> 4) & 0xF)
#define get_log_args_cnt(log_header)     ((log_header) & 0xF)

#define link_line(text1, text2)   (text1##text2)
#define str_name(text)           (link_line(text, __LINE__))

#ifndef BUILD_IN_ROM
#define BASE_PRINT_WITH_MODULE(log_mod, log_id, log_lvl, fmt, args_count, args...) \
    do {__attribute__((used, section(".logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf((uint32_t)(uintptr)str_name(LOGSTR), \
                        press_params(CONNECT(log_mod, log_id), log_lvl,  args_count), ##args); \
    } while (0)
#define BASE_PRINT(log_code, log_lvl, fmt, args_count, args...) \
    do {__attribute__((used, section(".logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf((uint32_t)(uintptr)str_name(LOGSTR), \
                        press_params(log_code, log_lvl,  args_count), ##args); \
    } while (0)
#define BASE_PRINT_NON_COMPRESS(log_code, log_lvl, fmt, args_count, args...)
#define BASE_PRINT_ERR(log_code, fmt, args_count, args...) \
    do {__attribute__((used, section(".logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf((uint32_t)(uintptr)str_name(LOGSTR), \
                        press_params(log_code, LOG_LEVEL_ERROR, args_count), ##args); \
    } while (0)
#define BASE_PRINT_WARN(log_code, fmt, args_count, args...) \
    do {__attribute__((used, section(".logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf((uint32_t)(uintptr)str_name(LOGSTR), \
                        press_params(log_code, LOG_LEVEL_WARNING, args_count), ##args); \
    } while (0)
#define BASE_PRINT_INFO(log_code, fmt, args_count, args...) \
    do {__attribute__((used, section(".logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf((uint32_t)(uintptr)str_name(LOGSTR), \
                        press_params(log_code, LOG_LEVEL_INFO, args_count), ##args); \
    } while (0)
#define BASE_PRINT_DEBUG(log_code, fmt, args_count, args...) \
    do {__attribute__((used, section(".logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf((uint32_t)(uintptr)str_name(LOGSTR), \
                        press_params(log_code, LOG_LEVEL_DBG, args_count), ##args); \
    } while (0)

#define BASE_PRINT_BTC_INFO0(fmt, args...) \
    do {__attribute__((used, section(".logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_btc_info0((uint32_t)(uintptr)str_name(LOGSTR), ##args); \
    } while (0)
#define BASE_PRINT_BTC_INFO1(fmt, args...) \
    do {__attribute__((used, section(".logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_btc_info1((uint32_t)(uintptr)str_name(LOGSTR), ##args); \
    } while (0)
#define BASE_PRINT_BTC_INFO2(fmt, args...) \
    do {__attribute__((used, section(".logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_btc_info2((uint32_t)(uintptr)str_name(LOGSTR), ##args); \
    } while (0)
#define BASE_PRINT_BTC_INFO3(fmt, args...) \
    do {__attribute__((used, section(".logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_btc_info3((uint32_t)(uintptr)str_name(LOGSTR), ##args); \
    } while (0)
#define BASE_PRINT_BTC_INFO4(fmt, args...) \
    do {__attribute__((used, section(".logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_btc_info4((uint32_t)(uintptr)str_name(LOGSTR), ##args); \
    } while (0)
#define BASE_PRINT_BTC_WARN0(fmt, args...) \
    do {__attribute__((used, section(".logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_btc_warn0((uint32_t)(uintptr)str_name(LOGSTR), ##args); \
    } while (0)
#define BASE_PRINT_BTC_WARN1(fmt, args...) \
    do {__attribute__((used, section(".logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_btc_warn1((uint32_t)(uintptr)str_name(LOGSTR), ##args); \
    } while (0)
#define BASE_PRINT_BTC_WARN2(fmt, args...) \
    do {__attribute__((used, section(".logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_btc_warn2((uint32_t)(uintptr)str_name(LOGSTR), ##args); \
    } while (0)
#define BASE_PRINT_BTC_WARN3(fmt, args...) \
    do {__attribute__((used, section(".logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_btc_warn3((uint32_t)(uintptr)str_name(LOGSTR), ##args); \
    } while (0)
#define BASE_PRINT_BTC_WARN4(fmt, args...) \
    do {__attribute__((used, section(".logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_btc_warn4((uint32_t)(uintptr)str_name(LOGSTR), ##args); \
    } while (0)

#endif /* BUILD_IN_ROM */
/**
 * @brief  Compression log printing function
 */
void compress_printf(uint32_t log_addr, uint32_t log_header, ...);

/*
* +                     16 Bits                    + 10 Bits  +   4 Bits    +    4 Bits    +
* +--------------+--------------+------------------+----------+-------------+--------------+
* |                     log_module                 |  log_id  |             |              |
* |--------------+--------------+------------------+----------|   log_lvl   |  args_count  |
* |                     LOG_BCORE_BTC              |    0     |             |              |
* +-----------------------------------------------------------+-------------+--------------+
*/
void compress_printf_btc_info0(uint32_t log_addr, ...);
void compress_printf_btc_info1(uint32_t log_addr, ...);
void compress_printf_btc_info2(uint32_t log_addr, ...);
void compress_printf_btc_info3(uint32_t log_addr, ...);
void compress_printf_btc_info4(uint32_t log_addr, ...);
void compress_printf_btc_warn0(uint32_t log_addr, ...);
void compress_printf_btc_warn1(uint32_t log_addr, ...);
void compress_printf_btc_warn2(uint32_t log_addr, ...);
void compress_printf_btc_warn3(uint32_t log_addr, ...);
void compress_printf_btc_warn4(uint32_t log_addr, ...);
/**
 * @brief  Initialization of the compression log threshold
 */
void compress_log_init(void);
/**
 * @brief  Set the compression log threshold
 * Used for the external interface function of the wear product
 */
void set_compress_log_count_threshold(uint32_t threshold);
/**
 * @brief  Check whether the log printing is within the threshold
 * return true:within the threshold, return false:exceeded the threshold
 */
bool check_compress_log_printf_threshold(void);

/**
 * @brief  Query the log module switch
 * @param  domain domain enum value
 * @param  module module enum value
 * @return true:on, false:off
 */
bool log_switch_is_open(uint16_t domain, uint16_t module);

/**
 * @brief  Switch log on or off
 * @param  domain domain enum value
 * @param  module module enum value
 * @param  op true:on, false:off
 * @return result SUCC or ERR
 */
int16_t set_log_switch(uint16_t domain, uint16_t module, bool op);

/**
 * @brief  Reset log switch to all on or off
 * @param  op true:all on, false:all off
 * @return None
 */
void reset_log_switch(bool op);

/**
 * @brief  Called by core APP to remote config bt and hifi.
 * @param  dst dest core enum
 * @param  domain
 * @param  module
 * @param  op true:all on, false:all off
 * @return enum ipc_status_t
 */
uint16_t remote_config_log_switch(cores_t dst, uint16_t domain, uint16_t module, bool op);

/**
 * @brief  Set the log switch action handler object
 * @param  message
 * @param  payload_p see struct log_switch_cmd_t
 * @param  src
 * @param  id
 * @return should always true
 */
bool set_log_switch_action_handler(ipc_action_t message, const volatile ipc_payload *payload_p,
                                   cores_t src, uint32_t id);

#else /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO */

/*
 * +   10 Bits   +   14 Bits   +    4 Bits   +    4 Bits    +
 * +-------------+-------------+-------------+--------------+
 * |   file_id   |   log_line  |   log_lvl   |  args_count  |
 * +-------------+-------------+-------------+--------------+
 */
#define press_params(log_lvl, file_id, log_line, args_count) \
    (((uint32_t)(file_id) << 22) | ((uint32_t)(log_line) << 8) | ((uint32_t)(log_lvl) << 4) | (uint32_t)(args_count))
#define get_file_id(log_header)        (((log_header) >> 22) & 0x3FF)
#define get_log_line(log_header)       (((log_header) >> 8) & 0x3FFF)
#define get_log_lvl(log_header)        (((log_header) >> 4) & 0xF)
#define get_log_args_cnt(log_header)   ((log_header) & 0xF)

#define get_log_domain(log_code)       (((log_code) >> 20) & 0xF)
#define get_log_module(log_code)       (((log_code) >> 16) & 0xF)
#define get_log_sub_module(log_code)   (((log_code) >> 10) & 0x3F)

#ifndef BUILD_IN_ROM
#ifdef BT_USER_RELEASE
#define BASE_PRINT_WITH_MODULE(log_mod, log_id, log_lvl, fmt, args_count, args...) \
    do { compress_printf_nolog(0, ##args); \
    } while (0)
#else
#define BASE_PRINT_WITH_MODULE(log_mod, log_id, log_lvl, fmt, args_count, args...) \
    do { compress_printf(press_params(log_lvl, THIS_FILE_ID, __LINE__, args_count), ##args); \
    } while (0)
#endif
#ifdef BT_ATE
#define BASE_PRINT(log_code, log_lvl, fmt, args_count, args...)
#else
#ifdef BT_USER_RELEASE
#define BASE_PRINT(log_code, log_lvl, fmt, args_count, args...) \
    do { compress_printf_nolog(0, ##args); \
    } while (0)
#else
#define BASE_PRINT(log_code, log_lvl, fmt, args_count, args...) \
    do { compress_printf(press_params(log_lvl, THIS_FILE_ID, __LINE__, args_count), ##args); \
    } while (0)
#endif
#endif
#ifdef BT_USER_RELEASE
#define BASE_PRINT_NON_COMPRESS(log_code, log_lvl, fmt, args_count, args...) \
    do { compress_printf_nolog(0, ##args); \
    } while (0)
#define BASE_PRINT_ERR(log_code, fmt, args_count, args...) \
    do { compress_printf_nolog(0, ##args); \
    } while (0)
#define BASE_PRINT_WARN(log_code, fmt, args_count, args...) \
    do { compress_printf_nolog(0, ##args); \
    } while (0)
#define BASE_PRINT_INFO(log_code, fmt, args_count, args...) \
    do { compress_printf_nolog(0, ##args); \
    } while (0)
#define BASE_PRINT_DEBUG(log_code, fmt, args_count, args...) \
    do { compress_printf_nolog(0, ##args); \
    } while (0)
#else
#define BASE_PRINT_NON_COMPRESS(log_code, log_lvl, fmt, args_count, args...) \
    do { compress_printf(press_params(log_lvl, THIS_FILE_ID, __LINE__, args_count), ##args); \
    } while (0)
#define BASE_PRINT_ERR(log_code, fmt, args_count, args...) \
    do { compress_printf(press_params(LOG_LEVEL_ERROR, THIS_FILE_ID, __LINE__, args_count), ##args); \
    } while (0)
#define BASE_PRINT_WARN(log_code, fmt, args_count, args...) \
    do { compress_printf(press_params(LOG_LEVEL_WARNING, THIS_FILE_ID, __LINE__, args_count), ##args); \
    } while (0)
#define BASE_PRINT_INFO(log_code, fmt, args_count, args...) \
    do { compress_printf(press_params(LOG_LEVEL_INFO, THIS_FILE_ID, __LINE__, args_count), ##args); \
    } while (0)
#define BASE_PRINT_DEBUG(log_code, fmt, args_count, args...) \
    do { compress_printf(press_params(LOG_LEVEL_DBG, THIS_FILE_ID, __LINE__, args_count), ##args); \
    } while (0)
#endif
#endif /* BUILD_IN_ROM */

void compress_printf(uint32_t log_header, ...);
void compress_printf_nolog(uint32_t log_header, ...);
#endif /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG */

#ifdef BUILD_IN_ROM
#define press_user_params(log_code, log_lvl, args_count) \
    (((uint32_t)(log_code) << 8) | ((uint32_t)(log_lvl) << 4) | (uint32_t)(args_count))

#define press_eng_params(log_lvl, file_id, log_line, args_count) \
    (((uint32_t)(file_id) << 22) | ((uint32_t)(log_line) << 8) | ((uint32_t)(log_lvl) << 4) | (uint32_t)(args_count))

#define link_line(text1, text2)   (text1##text2)
#define str_name(text)           (link_line(text, __LINE__))

#define BASE_PRINT_WITH_MODULE(log_mod, log_id, log_lvl, fmt, args_count, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                               press_user_params(CONNECT(log_mod, log_id), log_lvl,  args_count), \
                               press_eng_params(log_lvl, THIS_FILE_ID, __LINE__, args_count), \
                               ##args); \
    } while (0)
#define BASE_PRINT(log_code, log_lvl, fmt, args_count, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                               press_user_params(log_code, log_lvl,  args_count), \
                               press_eng_params(log_lvl, THIS_FILE_ID, __LINE__, args_count), \
                               ##args); \
    } while (0)
#define BASE_PRINT_NON_COMPRESS(log_code, log_lvl, fmt, args_count, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_log_no_print_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                                     press_user_params(log_code, log_lvl,  args_count), \
                                     press_eng_params(log_lvl, THIS_FILE_ID, __LINE__, args_count), \
                                     ##args); \
    } while (0)
#define BASE_PRINT_ERR(log_code, fmt, args_count, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                               press_user_params(log_code, LOG_LEVEL_ERROR,  args_count), \
                               press_eng_params(LOG_LEVEL_ERROR, THIS_FILE_ID, __LINE__, args_count), \
                               ##args); \
    } while (0)
#define BASE_PRINT_WARN(log_code, fmt, args_count, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                               press_user_params(log_code, LOG_LEVEL_WARNING,  args_count), \
                               press_eng_params(LOG_LEVEL_WARNING, THIS_FILE_ID, __LINE__, args_count), \
                               ##args); \
    } while (0)
#define BASE_PRINT_INFO(log_code, fmt, args_count, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                               press_user_params(log_code, LOG_LEVEL_INFO,  args_count), \
                               press_eng_params(LOG_LEVEL_INFO, THIS_FILE_ID, __LINE__, args_count), \
                               ##args); \
    } while (0)
#define BASE_PRINT_DEBUG(log_code, fmt, args_count, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                               press_user_params(log_code, LOG_LEVEL_DBG,  args_count), \
                               press_eng_params(LOG_LEVEL_DBG, THIS_FILE_ID, __LINE__, args_count), \
                               ##args); \
    } while (0)

#define BASE_PRINT_BTC_INFO0(fmt, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                               press_user_params(CONNECT((0 + LOG_BCORE_BTC), 0), LOG_LEVEL_INFO,  NO_ARG), \
                               press_eng_params(LOG_LEVEL_INFO, THIS_FILE_ID, __LINE__, NO_ARG), \
                               ##args); \
    } while (0)
#define BASE_PRINT_BTC_INFO1(fmt, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                               press_user_params(CONNECT((0 + LOG_BCORE_BTC), 0), LOG_LEVEL_INFO,  ONE_ARG), \
                               press_eng_params(LOG_LEVEL_INFO, THIS_FILE_ID, __LINE__, ONE_ARG), \
                               ##args); \
    } while (0)
#define BASE_PRINT_BTC_INFO2(fmt, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                               press_user_params(CONNECT((0 + LOG_BCORE_BTC), 0), LOG_LEVEL_INFO,  TWO_ARG), \
                               press_eng_params(LOG_LEVEL_INFO, THIS_FILE_ID, __LINE__, TWO_ARG), \
                               ##args); \
    } while (0)
#define BASE_PRINT_BTC_INFO3(fmt, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                               press_user_params(CONNECT((0 + LOG_BCORE_BTC), 0), LOG_LEVEL_INFO,  THREE_ARG), \
                               press_eng_params(LOG_LEVEL_INFO, THIS_FILE_ID, __LINE__, THREE_ARG), \
                               ##args); \
    } while (0)
#define BASE_PRINT_BTC_INFO4(fmt, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                               press_user_params(CONNECT((0 + LOG_BCORE_BTC), 0), LOG_LEVEL_INFO,  FOUR_ARG), \
                               press_eng_params(LOG_LEVEL_INFO, THIS_FILE_ID, __LINE__, FOUR_ARG), \
                               ##args); \
    } while (0)
#define BASE_PRINT_BTC_WARN0(fmt, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                               press_user_params(CONNECT((0 + LOG_BCORE_BTC), 0), LOG_LEVEL_WARNING,  NO_ARG), \
                               press_eng_params(LOG_LEVEL_WARNING, THIS_FILE_ID, __LINE__, NO_ARG), \
                               ##args); \
    } while (0)
#define BASE_PRINT_BTC_WARN1(fmt, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                               press_user_params(CONNECT((0 + LOG_BCORE_BTC), 0), LOG_LEVEL_WARNING,  ONE_ARG), \
                               press_eng_params(LOG_LEVEL_WARNING, THIS_FILE_ID, __LINE__, ONE_ARG), \
                               ##args); \
    } while (0)
#define BASE_PRINT_BTC_WARN2(fmt, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                               press_user_params(CONNECT((0 + LOG_BCORE_BTC), 0), LOG_LEVEL_WARNING,  TWO_ARG), \
                               press_eng_params(LOG_LEVEL_WARNING, THIS_FILE_ID, __LINE__, TWO_ARG), \
                               ##args); \
    } while (0)
#define BASE_PRINT_BTC_WARN3(fmt, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                               press_user_params(CONNECT((0 + LOG_BCORE_BTC), 0), LOG_LEVEL_WARNING,  THREE_ARG), \
                               press_eng_params(LOG_LEVEL_WARNING, THIS_FILE_ID, __LINE__, THREE_ARG), \
                               ##args); \
    } while (0)
#define BASE_PRINT_BTC_WARN4(fmt, args...) \
    do {__attribute__((used, section(".rom_logstr"))) static char str_name(LOGSTR)[] = fmt; \
        compress_printf_in_rom((uint32_t)(uintptr)str_name(LOGSTR), \
                               press_user_params(CONNECT((0 + LOG_BCORE_BTC), 0), LOG_LEVEL_WARNING,  FOUR_ARG), \
                               press_eng_params(LOG_LEVEL_WARNING, THIS_FILE_ID, __LINE__, FOUR_ARG), \
                               ##args); \
    } while (0)

#endif /* BUILD_IN_ROM */

void compress_printf_in_rom(uint32_t log_addr, uint32_t log_header_user, uint32_t log_header_eng, ...);
void compress_printf_rom_callback(uint32_t log_addr, uint32_t log_header_user, uint32_t log_header_eng, va_list args);
void compress_log_no_print_in_rom(uint32_t log_addr, uint32_t log_header_user, uint32_t log_header_eng, ...);
void compress_log_no_print_rom_callback(uint32_t log_addr, uint32_t log_header_user,
                                        uint32_t log_header_eng, va_list args);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
