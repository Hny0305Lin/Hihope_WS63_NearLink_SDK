/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: bs21 product fpga standard config
 *
 * Create:  2021-06-16
 */
#ifndef PRODUCT_FPGA_CONFIG_H
#define PRODUCT_FPGA_CONFIG_H

#define BTH_WITH_SMART_WEAR                 YES
#define BTH_WEAR_ENABLE_HALL_STATE          NO
#define BTH_WEAR_ALLOW_INQUIRY_SCAN         NO
#define BTH_WEAR_ENABLE_AUDIO_SOURCE        NO
#define BTH_WEAR_ENABLE_AVRCP_TARGET        NO
#define PRODUCT_SUPPORT_MODE_SWITCH         NO
#define BT_MANAGER_DEPLOYED                 2
#define BT_CODEC_TID                        2
#define APP_WRITE_OTP_ENABLE                NO
#define APP_HEAP_SIZE                       0x19000    // the minimum APP heap size (100kB)
#define USE_FLASH_SUSPEND_AND_RESUME        NO
#define WAIT_APPS_DUMP_FOREVER              NO
#define TWS_MPU_XIP_PROTECT                 NO
#define ENABLE_LOW_POWER                    NO
#define FSB_IMAGE_PAGES                     0
#define SSB_IMAGE_PAGES                     19
#define DTB_IMAGE_PAGES                     0
#define RECOVERY_IMAGE_PAGES                0
#define RESERVE_IMAGE_PAGES                 0
#define BT_IMAGE_PAGES                      336
#define HIFI0_IMAGE_PAGES                   0
#define HIFI1_IMAGE_PAGES                   0
#define APP_IMAGE_PAGES                     384
#define FOTA_IMAGE_START                    0x800000
#define SSB_FOTA_OFFSET                     0x0
#define XO_CTRIM_VALUR_DEFAULT              0x56
#define DSP1_IPC_ENABLE                     NO
#define APP_BTN_TASK_MONITOR_ENABLE         NO
#define SSB_FOTA_MODE_TWS                   YES
#define DSP_EXTEND_OCRAM_SIZE               1310720 // Unit byte, = 1280 KB
#define OSC_EN_CALLBACK_BY_PLT              YES
#define CHIP_PACKAGE_SUXUNK                 NO
#define BT_MIPS_DEBUG                       NO
/* Platform board config, need check if it support or not carefully. */
#define NON_OS_CRITICAL_RECORD              NO
#define ENABLE_MASSDATA_RECORD              NO
#define USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG NO
#define COMPRESS_LOG_TRIGGER_THRESHOLD      0
#define COMPRESS_LOG_COUNT_THRESHOLD        0xFFFFFFFF
#define EXCEPTION_TEST_ENABLE               NO
#define LOG_LEVEL_APP_DEFAULT_CONFIG        3   // Info level
#define LOG_LEVEL_BT_DEFAULT_CONFIG         3   // Info level
#define LOG_LEVEL_DSP_DEFAULT_CONFIG        3   // Info level
#define BCPU_HEAP_MININUM_SIZE              0xA000 // 40KB
#define SYS_DEBUG_MODE_ENABLE               YES
#define PROMPT_TONE_REGION_LENGTH           0
#define AUDIO_DATA_STREAM_REGION_LENGTH     0
#define WEAR_CRASH_HANDLE                   NO    // use IPC_ACTION_EXCEPTION_IND ipc info to inform M7
#define BT_MASSDATA_LENGTH                  0x400
#define APP_MASSDATA_LENGTH                 0x400
#define APP_LOGGING_LENGTH                  0x800
#define DSP_LOGGING_LENGTH                  0
#define RC_CLOCK_ON                         NO
#define SMART_WATCH_BT_COEX_PIN_SWITCH      NO   // pinmux switch in the Coexistence of BT and WiFi of smart watch
#define CAP_SENSOR_BUFF_LEN                 128

/********************Other module board config********************/
#define BTC_SWITCH_COEX                     NO
#define BTC_SMART_WEAR_WLCOEX               NO
#define BTC_WITH_SMART_WEAR                 NO
#define BTC_TWS_MONITOR                     YES
#define BTC_ICO_SUPPORT                     YES
#define BTC_DUAL_ANT_SWITCH_SUPPORT         NO
#define BTC_GET_EMC_FROM_PLATFORM           NO // 耳机产品通过HCI传递NV数据实现后，该选项需要置为NO
#define BTSNOOP_ENABLE                      YES
#define TWS_USER_CHR_RECORD                 NO
#define BTH_WEAR_ENABLE_AUDIO_SINK          NO
#define BTH_WEAR_ENABLE_AUDIO_GATEWAY       NO
#define BTH_WEAR_BREDR_DOUBLE_CONNECT       NO
#define BTC_DFX_LOG_HELP_SUPPORT            NO
#define FACTORY_CALI                        NO
#define BTH_WEAR_ENABLE_CONNECT_MANAGER     YES
#define BTH_WSTP_CMD_SUPPORT                YES
#define BTH_WEAR_ENABLE_BLE_FEATURES        YES
#define BTH_WEAR_ENABLE_HFP_FEATURES        NO
#define WEAR_USER_CONFIG                    NO
#define BTH_HIGH_POWER                      YES
#define MULTI_CONNECT                       YES
#define BTH_CONFIG_HDAP                     NO
#define DEVICE_MANAGE_FEATURE               YES
#define BTH_CALL_LC3_32K                    NO
#define BTH_ENABLE_LC3_CODEC                YES
#define ENABLE_CHANGE_DEVICE_NAME           YES
#define BTH_DIP_PRODUCT_ID                  0x4108
#define ENABLE_BT_XO_16M                    NO

/********************Other module board config********************/


#define BTH_ENABLE_L2HC_CODEC               YES


#define BTH_WEAR_ENABLE_AVRCP_CONTROLLER    NO
#define KEYBOARD_REPORT_BYPASS_GPIO         NO

#endif
