/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  LOG BUFFER INTERFACE
 * Author:
 * Create:  2018-10-15
 */

#ifndef NON_OS_LOG_BUFFER_H
#define NON_OS_LOG_BUFFER_H

#include <stdint.h>
#include "log_buffer_common.h"
#include "log_memory_section.h"

/**
 * @defgroup connectivity_drivers_non_os_log LOG
 * @ingroup  connectivity_drivers_non_os
 * @{
 */
#define CHR_VALUE_NULL        0
#define CHR_VALUE_SHFIT_24BIT 24
#define CHR_INFO1_SHFIT_16BIT 16
#define CHR_INFO2_SHFIT_8BIT  8
#define CHR_VALUE_MASK        0xff
#define CHR_BUFFER_WL_RATIO   2
#define CHR_EXTEND_PARAM_MAX_LEN            128

#define chr_tws_value_group(EVENT, INFO1, INFO2, INFO3) (((uint32_t)((uint32_t)(EVENT) & CHR_VALUE_MASK) << \
                                                        CHR_VALUE_SHFIT_24BIT) | \
                                                        ((uint32_t)((uint32_t)(INFO1) & CHR_VALUE_MASK) << \
                                                        CHR_INFO1_SHFIT_16BIT) | \
                                                        ((uint32_t)((uint32_t)(INFO2) & CHR_VALUE_MASK) << \
                                                        CHR_INFO2_SHFIT_8BIT) | \
                                                        ((INFO3) & CHR_VALUE_MASK))

#define chr_wear_value_group(EVENT, INFO1, INFO2, INFO3) (((uint32_t)((uint32_t)(EVENT) & CHR_VALUE_MASK) << \
                                                         CHR_VALUE_SHFIT_24BIT) | \
                                                         ((uint32_t)((uint32_t)(INFO1) & CHR_VALUE_MASK) << \
                                                         CHR_INFO1_SHFIT_16BIT) | \
                                                         ((uint32_t)((uint32_t)(INFO2) & CHR_VALUE_MASK) << \
                                                         CHR_INFO2_SHFIT_8BIT) | \
                                                         ((INFO3) & CHR_VALUE_MASK))

typedef enum {
    LOG_RET_OK,
    LOG_RET_ERROR_IN_PARAMETERS,
    LOG_RET_ERROR_NOT_ENOUGH_SPACE,
    LOG_RET_ERROR_CORRUPT_SHARED_MEMORY,
    LOG_RET_ERROR_OVERFLOW
} log_ret_t;

typedef enum {
    MASS_RET_OK,
    MASS_RET_ERROR_IN_PARAMETERS,
    MASS_RET_ERROR_NOT_ENOUGH_SPACE,
    MASS_RET_ERROR_CORRUPT_SHARED_MEMORY,
    MASS_MEM_COPY_FAIL,
    MASS_OVER_BUFFER_THD,
} mass_data_ret_t;

#define EVENT_ID_BEGIN          943900000
#define EVENT_ID_END            943999999
#define get_event_id(id, info1) (EVENT_ID_BEGIN + (((id) << 8) | (info1)))

typedef enum {
    MASS_EVENT_POINT,
    MASS_ERROR_POINT,
    MASS_EVENT_POINT_EXTEND,
    MASS_ERROR_POINT_EXTEND,
} mass_point_type_t;

// upload chr type
typedef enum {
    CHR_DFT = 0x0,
    CHR_UE_INT = 0x01,
    CHR_UE_STR = 0x02,
    CHR_UE_JSON = 0x03,
    CHR_UE_BETA = 0x04,
    CHR_END,
} chr_type_t;

typedef struct {
    uint32_t time_stamp;
    uint32_t event_id;       // diff from the 1 byte event ids, compose of EVENT_BEGIN | eventId<<8 | info1
    uint8_t event_info;      // info2
    uint8_t magic_number;
    uint8_t chr_up_type;     // upload chr format type
    uint8_t role;         // byte align
    uint32_t sub_event_info;  // info3
    uint32_t version;
    uint32_t psn;
} system_event_s_t, system_error_s_t;

#ifdef FEATURE_PLT_LB_CHECK
typedef enum {
    LOG_BUF_RET_OK = 0,
    LOG_BUF_RLW_MAGIC_ERROR = 1,
    LOG_RET_RGW_TOEND_MAGIC_ERROR = 2,
    LOG_RET_RGW_LASTONE_MAGIC_ERROR = 3,
    LOG_RET_MEMCPY_ERROR = 4,
} log_buffer_check_error_t;
#endif

#pragma pack(1)
typedef struct {
    uint8_t type;              // 0-event, 1-error.
    uint8_t event_id;
    uint8_t info1;
    uint8_t info2;
    uint16_t info3;
    uint8_t data_len;
    uint8_t data[CHR_EXTEND_PARAM_MAX_LEN];
} chr_extend_data_t;
#pragma pack()

/**
 * @brief  Initialize the log buffer module to log in the given log memory region section.
 * @return LOG_RET_OK or an error value
 */
void log_buffer_init(log_memory_region_section_t logsec);

#if (BTH_WITH_SMART_WEAR == YES) && defined(SUPPORT_IPC)
/**
 * @brief  record the system event.
 * @return void
 */
void massdata_record_system_event(uint8_t event_id, uint8_t info1, uint8_t info2, uint8_t info3);

/**
 * @brief  record the system error.
 * @return void
 */
void massdata_record_system_error(uint8_t event_id, uint8_t info1, uint8_t info2, uint8_t info3);

#else
#if (CORE == APPS)
/**
 * @brief  record the system event
 * @param  eid chr info1
 * @param  sub_eid chr info2
 * @param  code mass chr info3
 * @param  sub_code chr info4
 * @return void
 */
void chr_record_ue(uint8_t eid, uint8_t sub_eid, uint8_t code, uint32_t sub_code);

/**
 * @brief  record the system event.
 * @return void
 */
void massdata_record_system_event(uint8_t event_id, uint8_t info1, uint8_t info2, uint32_t info3);

/**
 * @brief  record the system error.
 * @return void
 */
void massdata_record_system_error(uint8_t event_id, uint8_t info1, uint8_t info2, uint32_t info3);

#elif (CORE == BT)

void massdata_set_role(uint8_t role);

/**
 * @brief  record the system event
 * @param  eid chr info1
 * @param  sub_eid chr info2
 * @param  code mass chr info3
 * @param  sub_code chr info4
 * @return void
 */
void chr_record_ue(uint8_t eid, uint8_t sub_eid, uint8_t code, uint32_t sub_code);

/**
 * @brief  record the system event.
 * @return void
 */
void massdata_record_system_event(uint8_t event_id, uint8_t info1, uint8_t info2, uint32_t info3);

/**
 * @brief  record the system error.
 * @return void
 */
void massdata_record_system_error(uint8_t event_id, uint8_t info1, uint8_t info2, uint32_t info3);
#endif
#endif

void log_buffer_massdata_record_system_error_wear(uint8_t event_id, uint8_t info1, uint8_t info2, uint8_t info3);

/**
 * @brief  init the mass share mem.
 * @return void
 */
void mass_buffer_init(mass_data_memory_region_section_t sec);

/**
 * @brief  write seg in the mass share mem.
 * @return LOG_RET_OK or an error value
 */
uint32_t mass_data_write_roll_buffer(const uint8_t *data, uint32_t length);

/**
 * @brief  triger mass data to flush.
 * @return LOG_RET_OK or an error value
 */
uint32_t massdata_triger_event(void);

/**
 * @brief  triger mass data to flush by queue.
 * @param  pay_i mass data payload
 * @param  type event or error
 * @return LOG_RET_OK or an error value
 */
uint32_t massdata_triger_queue(const uint8_t *pay_i, uint32_t type);

/**
 * @brief  Add data to the circular buffer with updating the write index.
 * @param  lb_header Header of the log buffer.
 * @param  buffer Pointer to the buffer.
 * @param  was_empty Was empty or not.
 */
void log_buffer_write(const log_buffer_header_t *lb_header, const uint8_t *buffer, bool *was_empty);

/**
 * @brief  Get the available space for the next message
 * @param  av available space will be stored here
 * @return LOG_RET_OK or an error code
 */
log_ret_t log_buffer_get_available_for_next_message(uint32_t *av);

#ifdef FEATURE_PLT_LB_CHECK
uint8_t log_buffer_check(void);
#endif

#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES)
log_ret_t compress_log_write(const uint8_t *data, uint32_t length);
#endif

/**
 * @brief  Record chr event
 * @param  chr_event type of chr event
 */
void log_buffer_record_system_event(uint32_t chr_event);

/**
 * @brief  Record chr error
 * @param  chr_error type of chr error
 */
void log_buffer_record_system_error(uint32_t chr_error);

/**
 * @brief  Record chr info with extend parameter.
 * @param  extend_data type of chr information
 */
void massdata_record_system_info_with_extend(chr_extend_data_t *extend_data);

/**
 * @}
 */
#endif
