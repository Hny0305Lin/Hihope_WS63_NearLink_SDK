/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  log OAM logger producer interface
 * Author:
 * Create:
 */

#ifndef LOG_OAM_LOGGER_H
#define LOG_OAM_LOGGER_H

#include "log_types.h"
#include "log_trigger.h"
#include "log_module_id.h"
#include "log_printf.h"

#define OM_FRAME_DELIMITER              (0x7E)
#define OM_FRAME_DELIMITER_LEN          (1)
#define OM_FRAME_HEADER_LEN             (8)
#define OM_LOG_SEND_MSG_LENGTH          (29)
#define OM_LOG_HEAD_PRESS_MASK          (0xFF)
#define OM_MSG_TYPE_LOG_OFFSET          (8)
#define OM_LOG_RPT_IND_MODULE_ID_OFFSET (16)
#define OM_LOG_SN_OFFSET                (16)

#define OM_FRAME_DUMP_DELIMITER         (0x1)

#define OML_LOG_FILEID_HIGH_BIT_OFFSET (2)
#define OML_LOG_FILEID_LOW_BIT_MASK    (0x3)
#define OML_LOG_LINENO_HIGH_BIT_OFFSET (8)
#define OML_LOG_LINENO_LOW_BIT_MASK    (0xFF)
#define OML_LOG_MODULE_MASK            (0x3F)
#define OML_LOG_LEVEL_MASK             (0x3)
#define OML_LOG_PARA_MAX_NUM           (4)
#define OML_LOG_ALTER_PARA_MAX_NUM     (12)
#define OML_LOG_HEADER_ARRAY_LENTH     (3)
#define OML_LOG_LEVEL_OFFSET           (6)
#define OML_LOG_TAIL_LENTH             (1)
#define OML_LOG_ZERO_ARG_SEND          (0)
#define OML_LOG_ONE_ARG_SEND           (1)
#define OML_LOG_TWO_ARG_SEND           (2)
#define OML_LOG_THREE_ARG_SEND         (3)
#define OML_LOG_FOUR_ARG_SEND          (4)
#define OML_LOG_INT_2_DOUBLE           (2)
#define OML_LOG_INT_BIT                (32)

#define OML_DATA_MSG_TYPE     (0x0E)
#define OML_MSG_HEADER_LENGTH (4)
#define OML_MSG_TAIL_LENGTH   (1)

#define FEATURE_PF_PM               (0x00000001)
#define FEATURE_PF_PMU_ERR_CHECK    (0x00000002)
#define FEATURE_PF_STATUS           (0x00000004)
#define FEATURE_PF_LOG_TEST_LOW_POW (0x00000008)
#define FEATURE_PF_BFGNUART_USE_DMA (0x00000010)
#define FEATURE_PF_RESTRACE         (0x00000020)
#define FEATURE_PF_HEARTBEAT        (0x00000040)

#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
#ifndef CONFIG_LOG_USE_DIAG_FRAME
#define getlog_level(ulPressPara) (((ulPressPara) >> OML_LOG_LEVEL_OFFSET) & OML_LOG_LEVEL_MASK)
#define get_module_id(ulPressPara) ((ulPressPara) & OML_LOG_MODULE_MASK)

#define log_head_press(log_mid) ((OM_FRAME_DELIMITER & OM_LOG_HEAD_PRESS_MASK) |  \
                                 ((OM_MSG_TYPE_LOG & OM_LOG_HEAD_PRESS_MASK) << OM_MSG_TYPE_LOG_OFFSET) |  \
                                 ((OM_LOG_RPT_IND & OM_LOG_HEAD_PRESS_MASK) << OM_LOG_RPT_IND_MODULE_ID_OFFSET) |  \
                                 (((log_mid) & OM_LOG_HEAD_PRESS_MASK) << OM_LOG_RPT_IND_MODULE_ID_OFFSET))
/**
 * @brief  Fill this 32 bit value as little ending format
 * Last by will be send as the first byte by log system.
 * |-------------------------------------------------------------------------------------------------------|
 * |-H--------------------------------------------- 32 bit ----------------------------------------------L-|
 * |      8 bit       |                 8 bit                 |       8 bit        |         8 bit         |
 * | lineno low 8 bit | lineno high 6 bit + file_id low 2 bit | file_id high 8 bit | lev 2 bit + mid 6 bit |
 * |-------------------------------------------------------------------------------------------------------|
 */
#define para_press(mid, lev, fileid, lineno) (((uint32_t)(((uint32_t)(lineno) & 0xFF) << 0x18)) |  \
                                              ((uint32_t)((((uint32_t)(fileid) & 0x3) << 0x10) |   \
                                              ((uint32_t)(lineno) & 0x3F00) << 0xA))  |          \
                                              ((uint32_t)(((uint32_t)(fileid) & 0x3FC) << 0x6)) |  \
                                              ((uint32_t)(((mid) & 0x3F) | (((lev) & 0x3) << 0x6))))

#define oal_log_lenth(arg_num)              (sizeof(om_log_header_t) + sizeof(uint32_t) * (arg_num) + sizeof(uint8_t))
#define log_lenth_and_sn_press(arg_num, SN) (oal_log_lenth(arg_num) | ((uint32_t)(SN) << OM_LOG_SN_OFFSET))
#else
#include "soc_diag_log.h"
#include "diag_common.h"

#define getlog_level(ulPressPara) ((ulPressPara) & OAM_LOG_LEVEL_MASK)
#define get_module_id(ulPressPara) (((ulPressPara) >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK)

#define log_head_press(log_mid) diag_log_msg_mk_mod_id(0)
#define para_press(mid, lev, fileid, lineno) diag_log_msg_mk_id(mid, fileid, lineno, lev)
#define oal_log_lenth(arg_num) (DIAG_FRAME_HEADER_SIZE + DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE + \
                                sizeof(uint32_t) * (arg_num) + sizeof(uint8_t))

#define sdt_to_diag_log_level(sdt_level) ((sdt_level) == LOG_LEVEL_NONE ? DIAG_LOG_LEVEL_ALERT : \
                                         ((sdt_level) == LOG_LEVEL_ERROR ? DIAG_LOG_LEVEL_ERROR : \
                                         ((sdt_level) == LOG_LEVEL_WARNING ? DIAG_LOG_LEVEL_WARN : \
                                         ((sdt_level) == LOG_LEVEL_INFO ? DIAG_LOG_LEVEL_INFO : \
                                         ((sdt_level) == LOG_LEVEL_DBG ? DIAG_LOG_LEVEL_DBG : \
                                         (LOG_LEVEL_NONE))))))
#endif

#define oml_wifi_log_print0(mid, lvl, fmt) \
    log_event_wifi_print0(para_press(LOG_WIFIMODULE, lvl, THIS_FILE_ID, __LINE__))
#define oml_wifi_log_print1(mid, lvl, fmt, p1) \
    log_event_wifi_print1(para_press(LOG_WIFIMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1))
#define oml_wifi_log_print2(mid, lvl, fmt, p1, p2) \
    log_event_wifi_print2(para_press(LOG_WIFIMODULE, lvl, THIS_FILE_ID, __LINE__), \
                          (uint32_t)(p1), (uint32_t)(p2))
#define oml_wifi_log_print3(mid, lvl, fmt, p1, p2, p3) \
    log_event_wifi_print3(para_press(LOG_WIFIMODULE, lvl, THIS_FILE_ID, __LINE__), \
                          (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3))
#define oml_wifi_log_print4(mid, lvl, fmt, p1, p2, p3, p4) \
    log_event_wifi_print4(para_press(LOG_WIFIMODULE, lvl, THIS_FILE_ID, __LINE__), \
                          (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3), (uint32_t)(p4))
#define oml_wifi_log_alter(mid, lognum, lvl, fmt, num, arg...) \
    log_event_print_alterable_para_press(log_head_press(OM_WIFI), \
                                         para_press(LOG_WIFIMODULE, lvl, THIS_FILE_ID, __LINE__), num, ##arg)

#ifdef BT_USER_RELEASE
#define oml_bt_log_print0(mid, lvl, fmt) \
    do { compress_printf_nolog(0); \
    } while (0)
#define oml_bt_log_print1(mid, lvl, fmt, p1) \
    do { compress_printf_nolog(0, (uint32_t)(p1)); \
    } while (0)
#define oml_bt_log_print2(mid, lvl, fmt, p1, p2) \
    do { compress_printf_nolog(0, (uint32_t)(p1), (uint32_t)(p2)); \
    } while (0)
#define oml_bt_log_print3(mid, lvl, fmt, p1, p2, p3) \
    do { compress_printf_nolog(0, (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3)); \
    } while (0)
#define oml_bt_log_print4(mid, lvl, fmt, p1, p2, p3, p4) \
    do { compress_printf_nolog(0, (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3), (uint32_t)(p4)); \
    } while (0)
#define oml_bt_log_alter(mid, lognum, lvl, fmt, num, arg...) \
    do { compress_printf_nolog(0, ##arg); \
    } while (0)

#define oml_bth_log_print0(mid, lvl, fmt) \
    do { compress_printf_nolog(0); \
    } while (0)
#define oml_bth_log_print1(mid, lvl, fmt, p1) \
    do { compress_printf_nolog(0, (uint32_t)(p1)); \
    } while (0)
#define oml_bth_log_print2(mid, lvl, fmt, p1, p2) \
    do { compress_printf_nolog(0, (uint32_t)(p1), (uint32_t)(p2)); \
    } while (0)
#define oml_bth_log_print3(mid, lvl, fmt, p1, p2, p3) \
    do { compress_printf_nolog(0, (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3)); \
    } while (0)
#define oml_bth_log_print4(mid, lvl, fmt, p1, p2, p3, p4) \
    do { compress_printf_nolog(0, (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3), (uint32_t)(p4)); \
    } while (0)
#define oml_bth_log_print5(mid, lvl, fmt, num, arg...) \
    do { compress_printf_nolog(0, ##arg); \
    } while (0)
#define oml_bth_log_print6(mid, lvl, fmt, num, arg...) \
    do { compress_printf_nolog(0, ##arg); \
    } while (0)
#define oml_bth_log_alter(mid, lognum, lvl, fmt, num, arg...) \
    do { compress_printf_nolog(0, ##arg); \
    } while (0)
#else
#define oml_bt_log_print0(mid, lvl, fmt) \
    log_event_print0(log_head_press(OM_BT), para_press(LOG_BTMODULE, lvl, THIS_FILE_ID, __LINE__))
#define oml_bt_log_print1(mid, lvl, fmt, p1) \
    log_event_print1(log_head_press(OM_BT), para_press(LOG_BTMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1))
#define oml_bt_log_print2(mid, lvl, fmt, p1, p2) \
    log_event_print2(log_head_press(OM_BT), para_press(LOG_BTMODULE, lvl, THIS_FILE_ID, __LINE__), \
                     (uint32_t)(p1), (uint32_t)(p2))
#define oml_bt_log_print3(mid, lvl, fmt, p1, p2, p3) \
    log_event_print3(log_head_press(OM_BT), para_press(LOG_BTMODULE, lvl, THIS_FILE_ID, __LINE__), \
                     (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3))
#define oml_bt_log_print4(mid, lvl, fmt, p1, p2, p3, p4) \
    log_event_print4(log_head_press(OM_BT), para_press(LOG_BTMODULE, lvl, THIS_FILE_ID, __LINE__), \
                    (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3), (uint32_t)(p4))
#define oml_bt_log_alter(mid, lognum, lvl, fmt, num, arg...) \
    log_event_print_alterable_para_press(log_head_press(OM_BT), \
                                         para_press(LOG_BTMODULE, lvl, THIS_FILE_ID, __LINE__), num, ##arg)

#define oml_bth_log_print0(mid, lvl, fmt) \
    log_event_print0(log_head_press(OM_BTH), para_press(LOG_BTHMODULE, lvl, THIS_FILE_ID, __LINE__))
#define oml_bth_log_print1(mid, lvl, fmt, p1) \
    log_event_print1(log_head_press(OM_BTH), para_press(LOG_BTHMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1))
#define oml_bth_log_print2(mid, lvl, fmt, p1, p2) \
    log_event_print2(log_head_press(OM_BTH), para_press(LOG_BTHMODULE, lvl, THIS_FILE_ID, __LINE__), \
                     (uint32_t)(p1), (uint32_t)(p2))
#define oml_bth_log_print3(mid, lvl, fmt, p1, p2, p3) \
    log_event_print3(log_head_press(OM_BTH), para_press(LOG_BTHMODULE, lvl, THIS_FILE_ID, __LINE__), \
                     (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3))
#define oml_bth_log_print4(mid, lvl, fmt, p1, p2, p3, p4) \
    log_event_print4(log_head_press(OM_BTH), para_press(LOG_BTHMODULE, lvl, THIS_FILE_ID, __LINE__), \
                    (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3), (uint32_t)(p4))
#define oml_bth_log_print5(mid, lvl, fmt, num, arg...) \
    log_event_print_alterable_para_press(log_head_press(OM_BTH), \
                                         para_press(LOG_BTHMODULE, lvl, THIS_FILE_ID, __LINE__), num, ##arg)
#define oml_bth_log_print6(mid, lvl, fmt, num, arg...) \
    log_event_print_alterable_para_press(log_head_press(OM_BTH), \
                                         para_press(LOG_BTHMODULE, lvl, THIS_FILE_ID, __LINE__), num, ##arg)
#define oml_bth_log_alter(mid, lognum, lvl, fmt, num, arg...) \
    log_event_print_alterable_para_press(log_head_press(OM_BTH), \
                                         para_press(LOG_BTHMODULE, lvl, THIS_FILE_ID, __LINE__), num, ##arg)
#endif

#ifdef CONFIG_DFX_SUPPORT_USERS_PRINT
typedef int (*log_other_print_t)(uint32_t log_module_type, uint32_t level, const char *fmt, uint32_t paranum, ...);
void log_other_print_register(log_other_print_t users_print_func);
void log_other_print0(uint32_t log_header, uint32_t log_level, const char *fmt);
void log_other_print1(uint32_t log_header, uint32_t log_level, const char *fmt, uint32_t p0);
void log_other_print2(uint32_t log_header, uint32_t log_level, const char *fmt, uint32_t p0, uint32_t p1);
void log_other_print3(uint32_t log_header, uint32_t log_level, const char *fmt, uint32_t p0, uint32_t p1, uint32_t p2);
void log_other_print4(uint32_t log_header, uint32_t log_level, const char *fmt,
                      uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3);

#define oml_pf_log_print0(mid, lognum, lvl, fmt) do {    \
    log_event_print0(log_head_press(OM_PF), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__));    \
    log_other_print0(mid, lvl, fmt);    \
} while (0)
#define oml_pf_log_print1(mid, lognum, lvl, fmt, p1) do {    \
    log_event_print1(log_head_press(OM_PF), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1));    \
    log_other_print1(mid, lvl, fmt, (uint32_t)(p1));    \
} while (0)
#define oml_pf_log_print2(mid, lognum, lvl, fmt, p1, p2) do {    \
    log_event_print2(log_head_press(OM_PF), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__),    \
                     (uint32_t)(p1), (uint32_t)(p2));    \
    log_other_print2(mid, lvl, fmt, (uint32_t)(p1), (uint32_t)(p2));    \
} while (0)
#define oml_pf_log_print3(mid, lognum, lvl, fmt, p1, p2, p3) do {    \
    log_event_print3(log_head_press(OM_PF), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__),    \
                     (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3));    \
    log_other_print3(mid, lvl, fmt, (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3));    \
} while (0)
#define oml_pf_log_print4(mid, lognum, lvl, fmt, p1, p2, p3, p4) do {    \
    log_event_print4(log_head_press(OM_PF), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__),    \
                     (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3), (uint32_t)(p4));    \
    log_other_print4(mid, lvl, fmt, (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3), (uint32_t)(p4));    \
} while (0)

#define oml_app_log_print0(mid, lvl, fmt) do {    \
    log_event_print0(log_head_press(OM_IR), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__));    \
    log_other_print0(mid, lvl, fmt);    \
} while (0)
#define oml_app_log_print1(mid, lvl, fmt, p1) do {    \
    log_event_print1(log_head_press(OM_IR), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1));    \
    log_other_print1(mid, lvl, fmt, (uint32_t)(p1));    \
} while (0)
#define oml_app_log_print2(mid, lvl, fmt, p1, p2) do {    \
    log_event_print2(log_head_press(OM_IR), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__),    \
                     (uint32_t)(p1), (uint32_t)(p2));    \
    log_other_print2(mid, lvl, fmt, (uint32_t)(p1), (uint32_t)(p2));   \
} while (0)
#define oml_app_log_print3(mid, lvl, fmt, p1, p2, p3) do {    \
    log_event_print3(log_head_press(OM_IR), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__),    \
                     (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3));    \
    log_other_print3(mid, lvl, fmt, (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3));    \
} while (0)
#define oml_app_log_print4(mid, lvl, fmt, p1, p2, p3, p4) do {    \
    log_event_print4(log_head_press(OM_IR), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__),    \
                     (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3), (uint32_t)(p4));    \
    log_other_print4(mid, lvl, fmt, (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3), (uint32_t)(p4));    \
} while (0)
#else
#define oml_pf_log_print0(mid, lognum, lvl, fmt) \
    log_event_print0(log_head_press(OM_PF), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__))
#define oml_pf_log_print1(mid, lognum, lvl, fmt, p1) \
    log_event_print1(log_head_press(OM_PF), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1))
#define oml_pf_log_print2(mid, lognum, lvl, fmt, p1, p2) \
    log_event_print2(log_head_press(OM_PF), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1), \
                     (uint32_t)(p2))
#define oml_pf_log_print3(mid, lognum, lvl, fmt, p1, p2, p3) \
    log_event_print3(log_head_press(OM_PF), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1), \
                     (uint32_t)(p2), (uint32_t)(p3))
#define oml_pf_log_print4(mid, lognum, lvl, fmt, p1, p2, p3, p4) \
    log_event_print4(log_head_press(OM_PF), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1), \
                     (uint32_t)(p2), (uint32_t)(p3), (uint32_t)(p4))

#define oml_app_log_print0(mid, lvl, fmt) \
    log_event_print0(log_head_press(OM_IR), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__))
#define oml_app_log_print1(mid, lvl, fmt, p1) \
    log_event_print1(log_head_press(OM_IR), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1))
#define oml_app_log_print2(mid, lvl, fmt, p1, p2) \
    log_event_print2(log_head_press(OM_IR), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1), \
                     (uint32_t)(p2))
#define oml_app_log_print3(mid, lvl, fmt, p1, p2, p3) \
    log_event_print3(log_head_press(OM_IR), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1), \
                     (uint32_t)(p2), (uint32_t)(p3))
#define oml_app_log_print4(mid, lvl, fmt, p1, p2, p3, p4) \
    log_event_print4(log_head_press(OM_IR), para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1), \
                     (uint32_t)(p2), (uint32_t)(p3), (uint32_t)(p4))
#endif
#define oml_pf_log_print_alter(mid, lognum, lvl, fmt, num, arg...) \
    log_event_print_alterable_para_press(log_head_press(OM_PF), \
                                         para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__), num, ##arg)
#define pf_log_alter(mid, lognum, lvl, fmt, num, arg...) \
    oml_pf_log_print_alter(LOG_PFMODULE, lognum, lvl, fmt, num, ##arg)
#define oml_app_log_print_alter(mid, lvl, fmt, num, arg...) \
    log_event_print_alterable_para_press(log_head_press(OM_IR), \
                                         para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__), num, ##arg)
#define app_log_alter(mid, lvl, fmt, num, arg...) oml_app_log_print_alter(LOG_PFMODULE, lvl, fmt, num, ##arg)

#define oml_nfc_log_print0(lvl, fmt) \
    log_event_print0(log_head_press(OM_NFC), para_press(LOG_NFCMODULE, lvl, THIS_FILE_ID, __LINE__))
#define oml_nfc_log_print1(lvl, fmt, p1) \
    log_event_print1(log_head_press(OM_NFC), para_press(LOG_NFCMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1))
#define oml_nfc_log_print2(lvl, fmt, p1, p2) \
    log_event_print2(log_head_press(OM_NFC), para_press(LOG_NFCMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1), \
                     (uint32_t)(p2))
#define oml_nfc_log_print3(lvl, fmt, p1, p2, p3) \
    log_event_print3(log_head_press(OM_NFC), para_press(LOG_NFCMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1), \
                     (uint32_t)(p2), (uint32_t)(p3))
#define oml_nfc_log_print4(lvl, fmt, p1, p2, p3, p4) \
    log_event_print4(log_head_press(OM_NFC), para_press(LOG_NFCMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1), \
                     (uint32_t)(p2), (uint32_t)(p3), (uint32_t)(p4))
#define oml_nfc_log_print_alter(lvl, fmt, num, arg...) \
    log_event_print_alterable_para_press(log_head_press(OM_NFC), \
                                         para_press(LOG_NFCMODULE, lvl, THIS_FILE_ID, __LINE__), num, ##arg)
#define nfc_log_alter(lvl, fmt, num, arg...) oml_app_log_print_alter(LOG_NFCMODULE, lvl, fmt, num, ##arg)


#define oml_glp_log_print0(lvl, fmt) \
    log_event_print0(log_head_press(OM_SLP), para_press(LOG_SLPMODULE, lvl, THIS_FILE_ID, __LINE__))
#define oml_glp_log_print1(lvl, fmt, p1) \
    log_event_print1(log_head_press(OM_SLP), para_press(LOG_SLPMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1))
#define oml_glp_log_print2(lvl, fmt, p1, p2) \
    log_event_print2(log_head_press(OM_SLP), para_press(LOG_SLPMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1), \
                     (uint32_t)(p2))
#define oml_glp_log_print3(lvl, fmt, p1, p2, p3) \
    log_event_print3(log_head_press(OM_SLP), para_press(LOG_SLPMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1), \
                     (uint32_t)(p2), (uint32_t)(p3))
#define oml_glp_log_print4(lvl, fmt, p1, p2, p3, p4) \
    log_event_print4(log_head_press(OM_SLP), para_press(LOG_SLPMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1), \
                     (uint32_t)(p2), (uint32_t)(p3), (uint32_t)(p4))
#define oml_glp_log_print_alter(lvl, fmt, num, arg...) \
    log_event_print_alterable_para_press(log_head_press(OM_SLP), \
                                         para_press(LOG_SLPMODULE, lvl, THIS_FILE_ID, __LINE__), num, ##arg)
#define glp_log_alter(lvl, fmt, num, arg...) oml_app_log_print_alter(LOG_SLPMODULE, lvl, fmt, num, ##arg)


#if CORE == CORE_LOGGING
#define pf_write_fifo_log_alter(mid, lognum, lvl, fmt, num, arg...) \
    log_event_print_alterable_para_press_by_write_uart_fifo(log_head_press(OM_PF), \
                                                            para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__), \
                                                            num, ##arg)
#else
#define pf_write_fifo_log_alter(mid, lognum, lvl, fmt, num, arg...) \
    log_event_print_alterable_para_press(log_head_press(OM_PF), \
                                         para_press(LOG_PFMODULE, lvl, THIS_FILE_ID, __LINE__), num, ##arg)
#endif
#ifdef SDT_LOG_BY_UART
void oml_log_print_alterable_para_press(uint32_t press_log_head, uint32_t press_para, uint16_t para_num, ...);
#define oml_bt_log_print_alter(mid, lvl, fmt, num, arg...) \
    oml_log_print_alterable_para_press(LOG_HEAD_PRESS(OM_BT), PARA_PRESS(mid, lvl, THIS_FILE_ID, __LINE__), num, ##arg)

#define oml_gnss_log_print0(mid, lvl, fmt) \
    oml_log_print0_press(LOG_HEAD_PRESS(OM_GNSS), PARA_PRESS(mid, lvl, THIS_FILE_ID, __LINE__))
#define oml_gnss_log_print1(mid, lvl, fmt, p1) \
    oml_log_print1_press(LOG_HEAD_PRESS(OM_GNSS), PARA_PRESS(mid, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1))
#define oml_gnss_log_print2(mid, lvl, fmt, p1, p2) \
    oml_log_print2_press(LOG_HEAD_PRESS(OM_GNSS), PARA_PRESS(mid, lvl, THIS_FILE_ID, __LINE__), \
                         (uint32_t)(p1), (uint32_t)(p2))
#define oml_gnss_log_print3(mid, lvl, fmt, p1, p2, p3) \
    oml_log_print3_press(LOG_HEAD_PRESS(OM_GNSS), PARA_PRESS(mid, lvl, THIS_FILE_ID, __LINE__), \
                         (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3))
#define oml_gnss_log_print4(mid, lvl, fmt, p1, p2, p3, p4) \
    oml_log_print4_press(LOG_HEAD_PRESS(OM_GNSS), PARA_PRESS(mid, lvl, THIS_FILE_ID, __LINE__), \
                         (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3), (uint32_t)(p4))
#define oml_gnss_log_print_alter(mid, lvl, fmt, num, arg...) \
    oml_log_print_alterable_para_press(LOG_HEAD_PRESS(OM_GNSS), PARA_PRESS(mid, lvl, THIS_FILE_ID, __LINE__), \
                                       num, ##arg)

#define oml_gnss_log_no_num_print_alter(mid, lvl, fmt, arg...)         \
    oml_log_print_alterable_para_press_no_num(LOG_HEAD_PRESS(OM_GNSS), \
    PARA_PRESS(mid, lvl, THIS_FILE_ID, __LINE__), (int8_t *)fmt, ##arg)

#define oml_gnss_double_log_print_alter(mid, lvl, fmt, float_num, int_num, arg...) \
    oml_log_print_double_alterable_para(LOG_HEAD_PRESS(OM_GNSS),                   \
    PARA_PRESS(mid, lvl, THIS_FILE_ID, __LINE__), float_num, int_num, ##arg)
#else
#define oml_bt_log_print_alter(mid, lvl, fmt, num, arg...) \
    log_event_print_alterable_para_press(log_head_press(OM_BT), \
                                         para_press(LOG_BTMODULE, lvl, THIS_FILE_ID, __LINE__), num, ##arg)
#define oml_gnss_log_print0(mid, lvl, fmt) \
    log_event_print0(log_head_press(OM_GNSS), para_press(LOG_GNSSMODULE, lvl, THIS_FILE_ID, __LINE__))
#define oml_gnss_log_print1(mid, lvl, fmt, p1) \
    log_event_print1(log_head_press(OM_GNSS), para_press(LOG_GNSSMODULE, lvl, THIS_FILE_ID, __LINE__), (uint32_t)(p1))
#define oml_gnss_log_print2(mid, lvl, fmt, p1, p2) \
    log_event_print2(log_head_press(OM_GNSS), para_press(LOG_GNSSMODULE, lvl, THIS_FILE_ID, __LINE__), \
                     (uint32_t)(p1), (uint32_t)(p2))
#define oml_gnss_log_print3(mid, lvl, fmt, p1, p2, p3) \
    log_event_print3(log_head_press(OM_GNSS), para_press(LOG_GNSSMODULE, lvl, THIS_FILE_ID, __LINE__), \
                     (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3))
#define oml_gnss_log_print4(mid, lvl, fmt, p1, p2, p3, p4) \
    log_event_print4(log_head_press(OM_GNSS), para_press(LOG_GNSSMODULE, lvl, THIS_FILE_ID, __LINE__), \
                     (uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3), (uint32_t)(p4))
#define oml_gnss_log_print_alter(mid, lvl, fmt, num, arg...) \
    log_event_print_alterable_para_press(log_head_press(OM_GNSS), \
                                         para_press(LOG_GNSSMODULE, lvl, THIS_FILE_ID, __LINE__), num, ##arg)
#endif
#else /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES */
#include "log_printf.h"
#include "log_num.h"
#define LOG_NUM_MASK 0xFFF
#define connection(text1, text2) (text1##text2)
#define contect(text1, text2)    (connection(text1, text2))
#define var_name(text)           (contect(text, __LINE__))

#if (CORE == BT)
#include "ipc.h"
#define NO_USE   0
#define oml_bt_log_print0(mid, lvl, fmt)   BASE_PRINT(NO_USE, lvl, fmt, NO_ARG)
#define oml_bt_log_print1(mid, lvl, fmt, p1)   BASE_PRINT(NO_USE, lvl, fmt, ONE_ARG, p1)
#define oml_bt_log_print2(mid, lvl, fmt, p1, p2)   BASE_PRINT(NO_USE, lvl, fmt, TWO_ARG, p1, p2)
#define oml_bt_log_print3(mid, lvl, fmt, p1, p2, p3)   BASE_PRINT(NO_USE, lvl, fmt, THREE_ARG, p1, p2, p3)
#define oml_bt_log_print4(mid, lvl, fmt, p1, p2, p3, p4)   BASE_PRINT(NO_USE, lvl, fmt, FOUR_ARG, p1, p2, p3, p4)
#define oml_bt_log_print_alter(mid, lvl, fmt, num, arg...)   BASE_PRINT(NO_USE, lvl, fmt, num, ##arg)

#elif (CORE == GNSS)

#define connection(text1, text2) (text1##text2)
#define contect(text1, text2)    (connection(text1, text2))
#define var_name(text)           (contect(text, __LINE__))

#define NO_USE   0
#define oml_gnss_log_print0(mid, lvl, fmt)   BASE_PRINT(NO_USE, lvl, fmt, NO_ARG)
#define oml_gnss_log_print1(mid, lvl, fmt, p1)   BASE_PRINT(NO_USE, lvl, fmt, ONE_ARG, p1)
#define oml_gnss_log_print2(mid, lvl, fmt, p1, p2)   BASE_PRINT(NO_USE, lvl, fmt, TWO_ARG, p1, p2)
#define oml_gnss_log_print3(mid, lvl, fmt, p1, p2, p3)   BASE_PRINT(NO_USE, lvl, fmt, THREE_ARG, p1, p2, p3)
#define oml_gnss_log_print4(mid, lvl, fmt, p1, p2, p3, p4)   BASE_PRINT(NO_USE, lvl, fmt, FOUR_ARG, p1, p2, p3, p4)
#define oml_gnss_log_print_alter(mid, lvl, fmt, num, arg...)   BASE_PRINT(NO_USE, lvl, fmt, num, ##arg)

#elif (CORE == APPS)
#define oml_app_log_print0(mid, lvl, fmt)                                                    \
    do {                                                                                     \
        __attribute__((used, section(".logstr"))) static char var_name(LOGSTR)[] = fmt;      \
        (LOS_Printf(MODULE_EXTERNAL, LOG_LEVEL_DEBUG, (uint32_t)var_name(LOGSTR), 0));         \
    } while (0)
#define oml_app_log_print1(mid, lvl, fmt, p1)                                                \
    do {                                                                                     \
        __attribute__((used, section(".logstr"))) static char var_name(LOGSTR)[] = fmt;      \
        (LOS_Printf(MODULE_EXTERNAL, LOG_LEVEL_DEBUG, (uint32_t)var_name(LOGSTR), 1, p1));     \
    } while (0)
#define oml_app_log_print2(mid, lvl, fmt, p1, p2)                                            \
    do {                                                                                     \
        __attribute__((used, section(".logstr"))) static char var_name(LOGSTR)[] = fmt;      \
        (LOS_Printf(MODULE_EXTERNAL, LOG_LEVEL_DEBUG, (uint32_t)var_name(LOGSTR), 2, p1, p2)); \
    } while (0)
#define oml_app_log_print3(mid, lvl, fmt, p1, p2, p3)                                                \
    do {                                                                                             \
        __attribute__((used, section(".logstr"))) static char var_name(LOGSTR)[] = fmt;              \
        (LOS_Printf(MODULE_EXTERNAL, LOG_LEVEL_DEBUG, (uint32_t)var_name(LOGSTR), 3, p1, p2, p3));     \
    } while (0)
#define oml_app_log_print4(mid, lvl, fmt, p1, p2, p3, p4)                                            \
    do {                                                                                             \
        __attribute__((used, section(".logstr"))) static char var_name(LOGSTR)[] = fmt;              \
        (LOS_Printf(MODULE_EXTERNAL, LOG_LEVEL_DEBUG, (uint32_t)var_name(LOGSTR), 4, p1, p2, p3, p4)); \
    } while (0)
#define oml_app_log_print_alter(mid, lvl, fmt, num, arg...)                                   \
    do {                                                                                      \
        __attribute__((used, section(".logstr"))) static char var_name(LOGSTR)[] = fmt;       \
        (LOS_Printf(MODULE_EXTERNAL, LOG_LEVEL_DEBUG, (uint32_t)var_name(LOGSTR), num, ##arg)); \
    } while (0)

#endif /* CORE == BT */
#define oml_pf_log_print0(mid, lognum, lvl, fmt) BASE_PRINT(CONNECT(mid, ((lognum) & LOG_NUM_MASK)), lvl, fmt, NO_ARG)
#define oml_pf_log_print1(mid, lognum, lvl, fmt, p1) BASE_PRINT(CONNECT(mid, ((lognum) & LOG_NUM_MASK)), lvl, fmt, \
                                                                ONE_ARG, p1)
#define oml_pf_log_print2(mid, lognum, lvl, fmt, p1, p2) BASE_PRINT(CONNECT(mid, ((lognum) & LOG_NUM_MASK)), lvl, fmt, \
                                                                    TWO_ARG, p1, p2)
#define oml_pf_log_print3(mid, lognum, lvl, fmt, p1, p2, p3) \
    BASE_PRINT(CONNECT(mid, ((lognum) & LOG_NUM_MASK)), lvl, fmt, THREE_ARG, p1, p2, p3)
#define oml_pf_log_print4(mid, lognum, lvl, fmt, p1, p2, p3, p4) \
    BASE_PRINT(CONNECT(mid, ((lognum) & LOG_NUM_MASK)), lvl, fmt, FOUR_ARG, p1, p2, p3, p4)
#define oml_pf_log_print_alter(mid, lognum, lvl, fmt, num, arg...) \
    BASE_PRINT(CONNECT(mid, ((lognum) & LOG_NUM_MASK)), lvl, fmt, num, ##arg)
#define pf_log_alter(mid, lognum, lvl, fmt, num, arg...) oml_pf_log_print_alter(mid, lognum, lvl, fmt, num, ##arg)
#define pf_write_fifo_log_alter(mid, lognum, lvl, fmt, num, arg...) \
        oml_pf_log_print_alter(mid, lognum, lvl, fmt, num, ##arg)
#endif /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG */

#ifndef FEATURE_ON
#define FEATURE_ON 1
#endif
#ifndef FEATURE_OFF
#define FEATURE_OFF 0
#endif

enum OM_MSG_TYPE_ENUM {
    OM_MSG_TYPE_LOG = 1,
    OM_MSG_TYPE_OTA = 2,
    OM_MSG_TYPE_TRACE = 3,
    OM_MSG_TYPE_EVENT = 4,
    OM_MSG_TYPE_ALARM = 5,
    OM_MSG_TYPE_CATCH = 6,
    OM_MSG_TYPE_COLLECT = 7,
    OM_MSG_TYPE_DUMP = 8,
    OM_MSG_MEM_COMMAND = 9,
    OM_MSG_SSI_REG_COMMAND = 10,
    OM_MSG_ICONFIG_COMMAND = 11,
    OM_MSG_TYPE_BTC_OAM = 12,
    OM_MSG_TYPE_WARNING = 13,
    OM_MSG_TYPE_STATISTIC = 15,
    OM_MSG_TYPE_LAST = 16,
    OM_MSG_NFC_COMMAND = 17,
    OM_MSG_TYPE_EDITION = 18,
    OM_MSG_TYPE_COVERAGE = 19,

    OM_MSG_GNSS_HOST_CMD_TYPE = 20, /* Add for GNSS, should modified later */
    OM_MSG_REG_COMMAND = 21,
    OM_MSG_CS_STAT_CMD = 22,
    OM_MSG_GNSS_SDT_CMD = 23,
    OM_MSG_TRSSI_FSM = 25,
    OM_MSG_TRSSI_VAL_CFG = 26,
    OM_MSG_TYPE_CALIRESULT = 27,
    OM_MSG_SSIBLOCK_COMMAND = 28,   /* Used by testfpga */
    OM_MSG_DBGUART_SWITCH_TO_BT = 29,
    OM_MSG_DBGUART_SWITCH_TO_WIFI = 30,

    OM_MSG_TYPE_STATUS = 31,
    OM_PCM_REPORT_BT = 32,
    OM_MSG_TYPE_DSCR = 33,
    OM_BT_SAMPLE_DATA = 41,

    OM_MSG_REG32_COMMAND = 65,
    OM_MSG_TYPE_BTH = 70,
    OM_MSG_TYPE_APP = 71,
    OM_MSG_TYPE_HIFI = 72,

    OM_MSG_TYPE_LOG_CUSTOM = 73,    /* Used for the log with customized structure. */

    OM_MSG_TYPE_BTC_WVT = 100, /* 0x64 */
    OM_MSG_TYPE_BUTT,
};

enum OM_LOG_ERROR_TYPE {
    OM_LOG_RET_OK,
    OM_LOG_RET_PARA_INVALID,
    OM_LOG_RET_POINTER_NULL,
};

enum OM_LOG_MSG_ENUM {
    OM_LOG_CONFIG_REQ = 1,
    OM_LOG_CONFIG_ACK = 2,
    OM_LOG_RPT_IND = 3,
    OM_LOG_SAVE_STACK = 8,
    OM_LOG_CONFIG_BUTT,
};

enum LOG_OAM_ENTRY_INDEX {
    LOG_OAM_INDEX_0,
    LOG_OAM_INDEX_1,
    LOG_OAM_INDEX_2,
    LOG_OAM_INDEX_3,
    LOG_OAM_INDEX_4,
    LOG_OAM_INDEX_5,
    LOG_OAM_INDEX_6,
    LOG_OAM_INDEX_7,
};

typedef struct {
    uint8_t module_id;
    uint8_t print_level;
} om_log_module_lev_t;

typedef struct {
    uint8_t frame_start;
    uint8_t func_type;
    uint8_t prime_id;
    uint8_t arr_reserver[1];
    uint16_t frame_len;
    uint16_t sn;
} __attribute__((packed)) om_msg_header_stru_t;

typedef struct {
    uint16_t count;
    uint8_t end_flag;
    uint8_t arr_reserve;
} om_msg_dump_header_stru_t;

/* Log message struct */
typedef struct {
    om_msg_header_stru_t header;
    uint8_t mod_print_lev_info; /* log_level_e */
    uint8_t file_idx_high;      /* file idx high */
    uint8_t bit2_file_idx_low : 2; /* file idx low */
    uint8_t bit6_line_no_high : 6; /* line No. high */
    uint8_t line_no_low;        /* line No. low */
} om_log_header_t;

#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
void log_event_print0(uint32_t log_header, uint32_t presspara);

void log_event_print1(uint32_t log_header, uint32_t presspara, uint32_t para1);

void log_event_print2(uint32_t log_header, uint32_t presspara, uint32_t para1, uint32_t para2);

void log_event_print3(uint32_t log_header, uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3);

void log_event_print4(uint32_t log_header, uint32_t presspara, uint32_t para1, uint32_t para2,
                      uint32_t para3, uint32_t para4);

void log_event_print_alterable_para_press(uint32_t log_header, uint32_t presspara, uint32_t paranum, ...);

void log_event_wifi_print0(uint32_t presspara);
void log_event_wifi_print1(uint32_t presspara, uint32_t para1);
void log_event_wifi_print2(uint32_t presspara, uint32_t para1, uint32_t para2);
void log_event_wifi_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3);
void log_event_wifi_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4);

#if CORE == CORE_LOGGING
void log_event_print_alterable_para_press_by_write_uart_fifo(
    uint32_t log_header, uint32_t presspara, uint32_t paranum, ...);
#endif

#endif /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG */

#endif
