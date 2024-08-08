/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   log OAM pcm producer interface
 * Author:
 * Create:
 */
#ifndef LOG_OAM_STATUS_H
#define LOG_OAM_STATUS_H

#include "log_oam_logger.h"
#include "log_printf.h"

#define OML_STATUS_ADD_LENGTH   (13)
#define OM_STATUS_DATA_MAX_SIZE (60)

#if ((defined BUILD_IN_ROM) || (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO))
#ifndef SDT_LOG_BY_UART
#define OML_BT_STATE_STORE(msg_id, length, ulparam1, ulparam2, ulparam3, ulparam4) {                          \
        log_oam_status_store(OM_STATUS_STATE, msg_id, OM_BT, length, ulparam1, ulparam2, ulparam3, ulparam4); \
    }
#define OML_BT_MESSAGE_STORE(msg_id, length, ulparam1, ulparam2, ulparam3, ulparam4) {                       \
        log_oam_status_store(OM_STATUS_MESG, msg_id, OM_BT, length, ulparam1, ulparam2, ulparam3, ulparam4); \
    }
#define OML_BT_DATA_STORE(msg_id, length, data) {                          \
        log_oml_status_write(OM_STATUS_DATA, msg_id, OM_BT, length, data); \
    }
#define OML_BT_STRUCT_STORE(prime_id, msg_id, length, data) {        \
        log_oml_status_write(prime_id, msg_id, OM_BT, length, data); \
    }
#else
#define OML_BT_STATE_STORE(usmsg_id, uslength, ulparam1, ulparam2, ulparam3, ulparam4)  { \
    oml_status_store(OM_STATUS_STATE, usmsg_id, OM_BT, uslength, ulparam1, ulparam2, ulparam3, ulparam4); \
}
#define OML_BT_MESSAGE_STORE(usmsg_id, uslength, ulparam1, ulparam2, ulparam3, ulparam4) { \
    oml_status_store(OM_STATUS_MESG, usmsg_id, OM_BT, uslength, ulparam1, ulparam2, ulparam3, ulparam4); \
}
#define OML_BT_DATA_STORE(usmsg_id, uslength, puc_data)   { \
    oml_status_write(OM_STATUS_DATA, usmsg_id, OM_BT, uslength, puc_data); \
}
#define OML_BT_STRUCT_STORE(ucprime_id, usmsg_id, uslength, puc_data)   { \
    oml_status_write(ucprime_id, usmsg_id, OM_BT, uslength, puc_data); \
}
#endif
#else /* (defined BUILD_IN_ROM) || (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO) */
#define OML_BT_STATE_STORE(msg_id, length, ulparam1, ulparam2, ulparam3, ulparam4) \
    do { \
        compress_printf(msg_id, press_params(BTC_MAGIC_LOG_CODE, LOG_LEVEL_INFO, length), \
                        ulparam1, ulparam2, ulparam3, ulparam4); \
    } while (0)
#define OML_BT_MESSAGE_STORE(msg_id, length, ulparam1, ulparam2, ulparam3, ulparam4) \
    do { \
        compress_printf(msg_id, press_params(BTC_MAGIC_LOG_CODE, LOG_LEVEL_INFO, length), \
                        ulparam1, ulparam2, ulparam3, ulparam4); \
    } while (0)
#define OML_BT_DATA_STORE(msg_id, length, data)
#define OML_BT_STRUCT_STORE(prime_id, msg_id, length, data)
#endif /* (defined BUILD_IN_ROM) || (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO) */

enum OM_STATUS_PRIME_ID  {
    OM_STATUS_STATE,
    OM_STATUS_MESG,
    OM_STATUS_DATA,

    OM_STATUS_BUTT,
};

typedef struct _om_status_data_stru_t {
    om_msg_header_stru_t header;
    uint16_t msg_id;
    uint16_t data_len;
    uint8_t data[OM_STATUS_DATA_MAX_SIZE];
    uint8_t frame_tail;
} om_status_data_stru_t;

typedef void(*log_oam_status_store_handler)(uint8_t prime_id, uint16_t msg_id, uint16_t mode,
                                            uint16_t length, const uint32_t *param);
int32_t pf_feature_set(uint32_t feature, uint8_t set);
int32_t pf_feature_get(uint32_t feature);
void log_oml_status_packet(om_status_data_stru_t *status_entry, uint8_t prime_id,
                           uint16_t msg_id, uint16_t length, const uint8_t *buffer);
uint32_t log_oml_status_write(uint8_t prime_id, uint16_t msg_id, uint16_t mode, uint16_t length, const uint8_t *buffer);
void log_oam_status_store_init(void);
void log_oam_status_store(uint8_t prime_id, uint16_t msg_id, uint16_t mode, uint32_t length, ...);
void log_oam_status_store_deal(uint8_t prime_id, uint16_t msg_id, uint16_t mode, uint16_t length,
                               const uint32_t *param);
void log_oam_status_store_register_callback(log_oam_status_store_handler callback);
void log_oam_status_store_unregister_callback(void);
void log_oam_status_store_trigger_callback(uint8_t prime_id, uint16_t msg_id, uint16_t mode,
                                           uint16_t length, const uint32_t *param);
#endif
