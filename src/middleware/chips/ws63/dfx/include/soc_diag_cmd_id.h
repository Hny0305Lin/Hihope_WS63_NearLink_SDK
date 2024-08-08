/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: DIAG - cmd id.
 */

#ifndef SOC_DIAG_CMD_ID_H
#define SOC_DIAG_CMD_ID_H

#ifndef SUPPORT_DIAG_V2_PROTOCOL

#define DIAG_L1_A_ID_MIN 0x0050 /* [0x0050, 0x1000 */
#define DIAG_L1_A_ID_MAX 0x1000
#define DIAG_MAC_A_ID_MIN 0x1000 /* [0x1000, 0x2000 */
#define DIAG_MAC_A_ID_MAX 0x2000

#define DIAG_APP_A_ID_MIN 0x5600 /* [0x5600, 0x5E00 */
#define DIAG_APP_A_ID_MAX 0x5E00

#define DIAG_L1_B_ID_MIN 0x2100 /* [0x2100, 0x2900 */
#define DIAG_L1_B_ID_MAX 0x2900
#define DIAG_MAC_B_ID_MIN 0x2900 /* [0x2900, 0x3000 */
#define DIAG_MAC_B_ID_MAX 0x3000

#define DIAG_APP_B_ID_MIN 0x5E00 /* [0x5E00, 0x6600 */
#define DIAG_APP_B_ID_MAX 0x6600

#define DIAG_CMD_OS_RESET 0x3009
#define DIAG_CMD_VER_QRY_1 0x3013
#define DIAG_CMD_SYS_QRY_1 0x3014
#define DIAG_CMD_TEST_CMD 0x3071
#define DIAG_CMD_TEST_CMD_2 0x3072

#define ID_DIAG_CMD_CYCLE_READ_MEM 0X30FE
#define ID_DIAG_CMD_READ_MEM 0X30FF

#define EXT_DBG_STAT_Q              0x3430  /* EXT_DBG_STAT_Q_S,,EXT_DBG_MEM_MOD_STAT_IND_S */
#define EXT_DBG_DUMP_DIAG_ITEMS     0x3431  /* EXT_DBG_DUMP_DIAG_ITEMS_REQ_S */
#define EXT_DBG_DFX_TASK_TEST       0x3432  /* EXT_DBG_DFX_TASK_TEST_S */

#define EXT_QRY_UE_API_VER 0x340A

/* *
 * First, initialization and system related DIAG commands [0x5000, 0x5200
 */
#define DIAG_CMD_HOST_CONNECT    0x5000
#define DIAG_CMD_HOST_DISCONNECT 0x5001
#define DIAG_CMD_CONNECT_RANDOM  0x5002
#define DIAG_CMD_CONNECT_M_CHECK 0x5003
#define DIAG_CMD_PWD_CHANGE      0x5004
#define DIAG_CMD_HEART_BEAT      0x5005
#define DIAG_CMD_REMOTE_TEST     0x5805 /* delete node from white list */
#define DIAG_CMD_SIMULATE_AT     0x500D
#define DIAG_CMD_MOCKED_SHELL    0x500E

#define DIAG_CMD_DUMP_SYSERR     0x7003
#define DIAG_CMD_DUMP_SYSERR_MAX 0x7005

#define DIAG_CMD_SIM_INIT_STATUS_IND 0x5050
#define DIAG_CMD_UE_POWERON_IND      0x5051
#define DIAG_CMD_UE_POWEROFF_IND     0x5052
#define DIAG_CMD_TIMER_OUT_IND       0x5053
#define DIAG_CMD_UE_SYS_STATUS_IND   0x5054
#define DIAG_CMD_PORT_TEST_IND       0x5055
#define DIAG_CMD_VCOM_SPY            0x5056
#define DIAG_CMD_LOG_GATTER          0x5060

#define DIAG_CMD_GET_CPUP            0x5070
#define DIAG_CMD_GET_MEM_INFO        0x5071
#define DIAG_CMD_GET_RESOURCE_INFO   0x5072
#define DIAG_CMD_GET_TASK_INFO       0x5073


#define DIAG_CMD_MEM_MEM32    0x5074
#define DIAG_CMD_MEM_MEM16    0x5075
#define DIAG_CMD_MEM_MEM8     0x5076
#define DIAG_CMD_MEM_W1       0x5077
#define DIAG_CMD_MEM_W2       0x5078
#define DIAG_CMD_MEM_W4       0x5079

/* *
 * Second, equipment operation instructions [0x5200, 0x5300
 */
#define DIAG_CMD_POWER_OFF      0x5200 /* Shutdown */
#define DIAG_CMD_UE_RST         0x5201    /* Restart the device */
#define DIAG_CMD_POWER_INFO_IND 0x5202
#define DIAG_CMD_SCPU_START     0x5203 /* Turn on the slave CPU including DSP */

/* UE read and write commands, such as memory, registers, Flash and other peripherals. */
#define DIAG_CMD_MEM_RD         0x5203
#define DIAG_CMD_MEM_WR         0x5204
#define DIAG_CMD_REG_RD         0x5205
#define DIAG_CMD_REG_WR         0x5206
#define DIAG_CMD_GPIO_WR        0x5208
#define DIAG_CMD_GPIO_RD        0x5207
#define DIAG_CMD_MEM_DUMP       0x5209 /* Data DUMP */
#define DIAG_CMD_M              0x5220 /* M */
#define DIAG_CMD_D              0x5221 /* D */
#define DIAG_CMD_MEM_DUMP_IND   0x520A

/* *
 * DIAG_CMD_SW_VER_QRY: Software version information observable DIAG command
 * DIAG_CMD_HW_VER_QRY: Hardware version information observable DIAG command
 * DIAG_CMD_IMEI_QRY: Query IMEI number
 * DIAG_CMD_HW_INFO_QRY: device name in product version information
 * DIAG_CMD_SOC_INFO_QRY: Query the version of the chip
 */
#define DIAG_CMD_SW_VER_QRY     0x520B
#define DIAG_CMD_HW_VER_QRY     0x520C
#define DIAG_CMD_IMEI_QRY       0x520D
#define DIAG_CMD_HW_INFO_QRY    0x520E
#define DIAG_CMD_SOC_INFO_QRY   0x520F
/* DIAG_CMD_BUF_CFG : LOG emergency and normal buffer settings, the purpose is to store buffered data in MUX */
#define DIAG_CMD_BUF_CFG        0x5210
#define DIAG_CMD_GET_UNLOCK_CODE 0x5212
#define DIAG_CMD_GETBUF_CFG     0x5211 /* DIAG_CMD_GETBUF_CFG : Get emergency and normal buffer settings */


/* *
 * Third, the log command [0x5300, 0x5400
 */
/* Message configuration and reporting commands */
#define DIAG_CMD_MSG_RPT_AIR        0x5304
#define DIAG_CMD_MSG_CFG_SET_SYS    0x5310
#define DIAG_CMD_MSG_CFG_SET_DEV    0x5311
#define DIAG_CMD_MSG_CFG_SET_USR    0x5312
#define DIAG_CMD_MSG_CFG_RST        0x5313
#define DIAG_CMD_MSG_RPT_SYS        0x5314
#define DIAG_CMD_MSG_RPT_LAYER      0x5315
#define DIAG_CMD_MSG_RPT_USR        0x5316
#define DIAG_CMD_MSG_CFG_SET_AIR    0x5512
#define DIAG_CMD_MSG_CFG_SET_LEVEL  0x5517

#define DIAG_CMD_VER_QRY            0x5318 /* ver command deleted */

#define DIAG_CMD_SYS_QRY            0x5319    /* sdm_sq */
#define DIAG_CMD_SYSE_QRY_R         0x5323 /* sdm_sq */
#define DIAG_CMD_SYS_END            0x5325
#define DIAG_CMD_CHL_SET            0x5333
#define DIAG_CMD_CHL_SET_END        0x5338

/* *
 * Fourth, the measurable command [0x5400, 0x5500
 */
#define DIAG_CMD_GET_CPU_INFO       0x5400

#define DIAG_CMD_HSO_AT             0x5451
#define DIAG_CMD_HSO_AT_SWT         0x5450
#define DIAG_CMD_MSP_SDM_QURY       0x5452
#define DIAG_CMD_MSP_SDM_QURY_IND   0x5453
#define DIAG_CMD_GTR_SET            0x5454

/* *
 * Five, NV read and write commands [0x5500, 0x5600
 */
#define DIAG_CMD_NV_RD          0x5500
#define DIAG_CMD_NV_WR          0x5501
#define DIAG_CMD_NV_RD_IND      0x5502
#define DIAG_CMD_NV_QRY         0x5503
#define DIAG_CMD_NV_EXPORT      0x5505
#define DIAG_CMD_NV_IMPORT      0x5504
#define DIAG_CMD_NV_EXPORT_IND  0x5506
#define DIAG_CMD_NV_DEL         0x5507
#define DIAG_CMD_NV_FILE_LOAD   0x5508
#define DIAG_CMD_NV_FILE_UPDATE 0x5509
#define DIAG_CMD_NV_FILE_EXPORT 0x550A
#define DIAG_CMD_NV_BACK        0x550C
#define DIAG_CMD_NV_UPDATE      0x550B
#define DIAG_CMD_NV_REFRESH     0x550D
#define DIAG_CMD_NV_CHANGESYSMODE 0x550E

#define ID_DIAG_CMD_REMOTE      0x7000
#define ID_DIAG_CMD_REMOTE_USR  0x7001
#define ID_DIAG_CMD_REMOTE_END  0x7002
#define ID_DIAG_CMD_DO_FRAME_ERR 0x7007 /* DIAG error report */

#define DIAG_CMD_CHANGE_UART_CONFIG     0x7180
#define DIAG_CMD_GET_UART_CONFIG        0x7181
#define DIAG_CMD_MOCKED_SHELL_ENABLE    0x7182

#define DIAG_CMD_TRANSMIT_ID_ST     0x7190
#define DIAG_CMD_TRANSMIT_GET_DATA  0x7190
#define DIAG_CMD_TRANSMIT_LS        0x7191
#define DIAG_CMD_TRANSMIT_DUMP_FILE 0x7192

#define DIAG_CMD_ID_TRANSMIT_REQUEST 0x7193
#define DIAG_CMD_ID_TRANSMIT_REPLY   0x7194
#define DIAG_CMD_ID_TRANSMIT_START   0x7195
#define DIAG_CMD_ID_STATE_NOTIFY     0x7196
#define DIAG_CMD_TRANSMIT_DEL_FILE   0x7198

#define DIAG_CMD_ID_LAST_DUMP_START  0x71A3
#define DIAG_CMD_ID_LAST_DUMP        0x71A4
#define DIAG_CMD_ID_LAST_DUMP_FINISH 0x71A5
#define DIAG_CMD_ID_LAST_WORD        0x71A6
#define DIAG_CMD_ID_LAST_DUMP_END    0x71A7

#define DIAG_CMD_TRANSMIT_ID_END    0x71B0

#define DIAG_CMD_UPDATE_PREPARE     0x71B1
#define DIAG_CMD_UPDATE_REQUEST     0x71B2
#define DIAG_CMD_UPDATE_START       0x71B3

#define DIAG_CMD_ID_DBG_STAT_QUERY  0x71BF
#define DIAG_CMD_ID_DIAG_DFX_REQ    0x71C0
#define DIAG_CMD_ID_DIAG_DFX_START  0x71C1
#define DIAG_CMD_ID_DIAG_DFX_END    0x71CF

/* PSD */
#define DIAG_CMD_ID_PSD_ENABLE 0x71D0
#define DIAG_CMD_ID_PSD_REPORT 0x71D1

#define DIAG_CMD_CRITICAL_DUMP      0x71D3
#define DIAG_CMD_ID_SAMPLE          0x71D2
#define DIAG_CMD_ID_BGLE_SAMPLE     0x71D4
#define DIAG_CMD_ID_WLAN_SAMPLE     0x71D5
#define DIAG_CMD_ID_SOC_SAMPLE      0x71D6
#define DIAG_CMD_ID_WLAN_PHY_SAMPLE 0x71D7
#define DIAG_CMD_ID_SAMPLE_DATA     0x7194 /* the same as DIAG_CMD_ID_TRANSMIT_REPLY */
#define DIAG_CMD_ID_SAMPLE_FINISH   0x7196 /* the same as DIAG_CMD_ID_STATE_NOTIFY */

#define DIAG_CMD_SIMULATE_AT_IND    0xA011
#define DIAG_CMD_MOCKED_SHELL_IND   0xA012

#else /* SUPPORT_DIAG_V2_PROTOCOL */

#define DIAG_MAINTENANCE_MODULE_ID      0x05

#define diag_cmd_id_comb(cmd_id)        (DIAG_MAINTENANCE_MODULE_ID << 8 | (cmd_id))

/* initialization and system related DIAG commands [0x00, 0x08] */
#define DIAG_CMD_HOST_CONNECT           diag_cmd_id_comb(0x00)
#define DIAG_CMD_HOST_DISCONNECT        diag_cmd_id_comb(0x01)
#define DIAG_CMD_CONNECT_RANDOM         diag_cmd_id_comb(0x02)
#define DIAG_CMD_CONNECT_M_CHECK        diag_cmd_id_comb(0x03)
#define DIAG_CMD_PWD_CHANGE             diag_cmd_id_comb(0x04)
#define DIAG_CMD_HEART_BEAT             diag_cmd_id_comb(0x05)
#define DIAG_CMD_REMOTE_TEST            diag_cmd_id_comb(0x06)
#define DIAG_CMD_SIMULATE_AT            diag_cmd_id_comb(0x07)
#define DIAG_CMD_MOCKED_SHELL           diag_cmd_id_comb(0x08)

/* os info cmd [0x09, 0x0C] */
#define DIAG_CMD_GET_CPUP               diag_cmd_id_comb(0x09)
#define DIAG_CMD_GET_MEM_INFO           diag_cmd_id_comb(0x0A)
#define DIAG_CMD_GET_RESOURCE_INFO      diag_cmd_id_comb(0x0B)
#define DIAG_CMD_GET_TASK_INFO          diag_cmd_id_comb(0x0C)

/* memory Operations cmd [0x0D, 0x12] */
#define DIAG_CMD_MEM_MEM32              diag_cmd_id_comb(0x0D)
#define DIAG_CMD_MEM_MEM16              diag_cmd_id_comb(0x0E)
#define DIAG_CMD_MEM_MEM8               diag_cmd_id_comb(0x0F)
#define DIAG_CMD_MEM_W1                 diag_cmd_id_comb(0x10)
#define DIAG_CMD_MEM_W2                 diag_cmd_id_comb(0x11)
#define DIAG_CMD_MEM_W4                 diag_cmd_id_comb(0x12)

/* DFX cmd 1 [0x13, 0x23] */
#define DIAG_CMD_ID_LAST_DUMP           diag_cmd_id_comb(0x13)
#define DIAG_CMD_ID_LAST_DUMP_FINISH    diag_cmd_id_comb(0x14)
#define DIAG_CMD_ID_LAST_WORD           diag_cmd_id_comb(0x15)
#define DIAG_CMD_DUMP_SYSERR            diag_cmd_id_comb(0x16)
#define DIAG_CMD_DUMP_SYSERR_MAX        diag_cmd_id_comb(0x17)
#define DIAG_CMD_ID_LAST_DUMP_START     diag_cmd_id_comb(0x18)
#define DIAG_CMD_ID_LAST_DUMP_END       diag_cmd_id_comb(0x19)

#define DIAG_CMD_TRACE_START            diag_cmd_id_comb(0x20)
#define DIAG_CMD_TRACE_STOP             diag_cmd_id_comb(0x21)
#define DIAG_CMD_GET_TIME               diag_cmd_id_comb(0x22)
#define DIAG_CMD_SET_TIME               diag_cmd_id_comb(0x23)

/* PQUI cmd [0x24, 0x2F] */
#define DIAG_CMD_AUDIO_HAID_REQ         diag_cmd_id_comb(0x24)
#define DIAG_CMD_AUDIO_SWS_EFFECT_REQ   diag_cmd_id_comb(0x25)
#define DIAG_CMD_AUDIO_SEE_EFFECT_REQ   diag_cmd_id_comb(0x26)

/* NV read and write commands [0x30, 0x33] */
#define DIAG_CMD_NV_RD                  diag_cmd_id_comb(0x30)
#define DIAG_CMD_NV_WR                  diag_cmd_id_comb(0x31)
#define DIAG_CMD_NV_RD_WITH_ATTR        diag_cmd_id_comb(0x32)
#define DIAG_CMD_NV_WR_WITH_ATTR        diag_cmd_id_comb(0x33)

/* sample related cmd [0x34, 0x3F] */
#define DIAG_CMD_ID_AUDIO_SAMPLE        diag_cmd_id_comb(0x38)
#define DIAG_CMD_ID_SAMPLE              diag_cmd_id_comb(0x39)
#define DIAG_CMD_ID_BGLE_SAMPLE         diag_cmd_id_comb(0x3A)
#define DIAG_CMD_ID_WLAN_SAMPLE         diag_cmd_id_comb(0x3B)
#define DIAG_CMD_ID_SOC_SAMPLE          diag_cmd_id_comb(0x3C)
#define DIAG_CMD_ID_SAMPLE_DATA         diag_cmd_id_comb(0x3D)
#define DIAG_CMD_ID_SAMPLE_FINISH       diag_cmd_id_comb(0x3E)
#define DIAG_CMD_ID_PHY_SAMPLE          diag_cmd_id_comb(0x3F)

/* DFX cmd 2 [0x40, 0x4F] */
#define DIAG_CMD_SIMULATE_AT_IND        diag_cmd_id_comb(0x40)
#define DIAG_CMD_MOCKED_SHELL_IND       diag_cmd_id_comb(0x41)
#define DIAG_CMD_CHANGE_UART_CONFIG     diag_cmd_id_comb(0x42)
#define DIAG_CMD_GET_UART_CONFIG        diag_cmd_id_comb(0x43)
#define DIAG_CMD_MOCKED_SHELL_ENABLE    diag_cmd_id_comb(0x44)
#define DIAG_CMD_SET_OFFLINE_LOG        diag_cmd_id_comb(0x45)
#define DIAG_CMD_GET_INIT_INFO          diag_cmd_id_comb(0x46)

/* audio cmd [0x4a, 0x55] */
#define DIAG_CMD_AUDIO_PROC_AI          diag_cmd_id_comb(0x4a)
#define DIAG_CMD_AUDIO_PROC_SEA         diag_cmd_id_comb(0x4b)
#define DIAG_CMD_AUDIO_PROC_ADEC        diag_cmd_id_comb(0x4c)
#define DIAG_CMD_AUDIO_PROC_AENC        diag_cmd_id_comb(0x4d)
#define DIAG_CMD_AUDIO_PROC_AO          diag_cmd_id_comb(0x4e)
#define DIAG_CMD_AUDIO_PROC_AB          diag_cmd_id_comb(0x4f)
#define DIAG_CMD_AUDIO_PROC_SYS         diag_cmd_id_comb(0x50)
#define DIAG_CMD_AUDIO_DUMP             diag_cmd_id_comb(0x51)
#define DIAG_CMD_AUDIO_PROBE            diag_cmd_id_comb(0x52)

/* DFX cmd 3 [0x56, 0x5F] */
#define DIAG_CMD_ID_DIAG_DFX_REQ        diag_cmd_id_comb(0x56)
#define DIAG_CMD_ID_DIAG_DFX_START      diag_cmd_id_comb(0x57)
#define DIAG_CMD_ID_DIAG_DFX_END        diag_cmd_id_comb(0x5E)
#define DIAG_CMD_ID_DBG_STAT_QUERY      diag_cmd_id_comb(0x5F)

/* MSG config [0x60, 0x69] */
#define DIAG_CMD_MSG_CFG_SET_SYS        diag_cmd_id_comb(0x60)
#define DIAG_CMD_MSG_CFG_SET_DEV        diag_cmd_id_comb(0x61)
#define DIAG_CMD_MSG_CFG_SET_USR        diag_cmd_id_comb(0x62)
#define DIAG_CMD_MSG_CFG_SET_AIR        diag_cmd_id_comb(0x63)
#define DIAG_CMD_MSG_CFG_SET_LEVEL      diag_cmd_id_comb(0x64)

/* NV cmd 2 [0x6A, 0x6F] */
#define DIAG_CMD_NV_GET_STORE_STATUS    diag_cmd_id_comb(0x6A)
#define DIAG_CMD_NV_BACKUP_ALL_KEYS     diag_cmd_id_comb(0x6B)
#define DIAG_CMD_NV_RESTORE_ALL_KEYS    diag_cmd_id_comb(0x6C)
#define DIAG_CMD_NV_GET_PAGE_USAGE      diag_cmd_id_comb(0x6D)
#define DIAG_CMD_NV_SET_RESTORE_FLAG    diag_cmd_id_comb(0x6E)

/* DFX cmd 4 [0x70, 0x7F] */
#define DIAG_CMD_UPDATE_PREPARE         diag_cmd_id_comb(0x70)
#define DIAG_CMD_UPDATE_REQUEST         diag_cmd_id_comb(0x71)
#define DIAG_CMD_UPDATE_START           diag_cmd_id_comb(0x72)
#define DIAG_CMD_UPDATE_GET_SIZE        diag_cmd_id_comb(0x74)
#define DIAG_CMD_UPDATE_WRITE           diag_cmd_id_comb(0x75)
#define DIAG_CMD_LOG_FILE_CFG           diag_cmd_id_comb(0x76)
#define DIAG_CMD_LOG_FILE_CTRL          diag_cmd_id_comb(0x77)
#define DIAG_CMD_LOG_FILE_WRITE         diag_cmd_id_comb(0x78)

/* BT */
#define DIAG_CMD_ID_BT_DFT              diag_cmd_id_comb(0x80)

/* Charger */
#define DIAG_CMD_DEV_INIT               diag_cmd_id_comb(0x82)
#define DIAG_CMD_IOCTL_INIT             diag_cmd_id_comb(0x83)
#define DIAG_CMD_GET_SYS_STATUS         diag_cmd_id_comb(0x84)
#define DIAG_CMD_GET_PARAMETER          diag_cmd_id_comb(0x85)
#define DIAG_CMD_SET_PARAMETER          diag_cmd_id_comb(0x86)
#define DIAG_CMD_GET_ID                 diag_cmd_id_comb(0x87)
#define DIAG_CMD_GET_NAME               diag_cmd_id_comb(0x88)
#define DIAG_CMD_ENTER_SHIPMODE         diag_cmd_id_comb(0x89)

/* BTH DFX [0x90,0x9F] */
#define DIAG_CMD_ID_BT_DBG_POS          diag_cmd_id_comb(0x90)

/* gpu cmd [0xa0, 0xaf] */
#define DIAG_CMD_GPU_VAU_PROC          diag_cmd_id_comb(0xa0)
#define DIAG_CMD_GPU_DPU_PROC          diag_cmd_id_comb(0xa1)
#define DIAG_CMD_GPU_GMMU_PROC         diag_cmd_id_comb(0xa2)
#define DIAG_CMD_GPU_JPEG_PROC         diag_cmd_id_comb(0xa3)
#define DIAG_CMD_GPU_DPU_PRINT         diag_cmd_id_comb(0xa4)


/* report log cmd [0xF0, 0xFF] */
#define DIAG_CMD_MSG_RPT_SYS_EXTEND_LOG diag_cmd_id_comb(0xF9)
#define DIAG_CMD_MSG_RPT_LAYER          diag_cmd_id_comb(0xFA)
#define DIAG_CMD_MSG_RPT_USR            diag_cmd_id_comb(0xFB)
#define DIAG_CMD_MSG_RPT_AIR            diag_cmd_id_comb(0xFC)
#define DIAG_CMD_MSG_RPT_SYS_NORMAL_LOG diag_cmd_id_comb(0xFD)
#define DIAG_CMD_MSG_RPT_SYS_FULLY_LOG  diag_cmd_id_comb(0xFE)
#define DIAG_CMD_MSG_RPT_SYS_MINI_LOG   diag_cmd_id_comb(0xFF)

#define DIAG_CMD_MAX                    0x100

#endif /* SUPPORT_DIAG_V2_PROTOCOL */

#endif /* SOC_DIAG_CMD_ID_H */
