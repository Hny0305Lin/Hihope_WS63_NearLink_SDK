/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  LOG BUFFER READER MODULE.
 * Author:
 * Create:  2018-10-15
 */
#include "panic.h"
#include "securec.h"
#include "log_memory_section.h"
#include "stdint.h"
#include "diag_log.h"
#include "log_buffer_reader.h"
/*
 *  Private type definitions
 */
typedef struct {
    uint8_t *log_buffer_start;
    uint32_t log_buffer_size;
    volatile uint32_t lb_next_to_consume;  // == not disdarded yet
} log_buffer_reader_control_t;

/*
 *  Private variable declarations
 */
/** Array of pointers to the section control structures for the different regions in the shared memory */
static log_memory_section_control_t *g_section_control[LOG_MEMORY_REGION_MAX_NUMBER];

/** Internal control for the different log regions */
static log_buffer_reader_control_t g_buffer_reader_control[LOG_MEMORY_REGION_MAX_NUMBER];

/*
 *  Private function definitions
 */
static uint32_t circled_index(const log_buffer_reader_control_t *lmscontrol, uint32_t index_in)
{
    uint32_t index_out;

    if (index_in < lmscontrol->log_buffer_size) {
        index_out = index_in;
    } else {
        index_out = index_in - lmscontrol->log_buffer_size;
    }
    if (index_out >= lmscontrol->log_buffer_size) {
        panic(PANIC_LOG, __LINE__);
    }
    return index_out;
}

static void log_buffer_copy_consecutive(const log_buffer_reader_control_t *lmscontrol,
                                        uint8_t *destination_p, uint32_t index_in, uint32_t len)
{
    if (destination_p == NULL) {
        panic(PANIC_LOG, __LINE__);
        return;
    }
    if (len >= lmscontrol->log_buffer_size) {
        panic(PANIC_LOG, __LINE__);
        return;
    }
    errno_t sec_ret;

    if (index_in + len <= lmscontrol->log_buffer_size) {
        sec_ret = memcpy_s((void *)destination_p, len, (void *)(lmscontrol->log_buffer_start + index_in), len);
    } else {
        uint32_t l1;
        l1 = lmscontrol->log_buffer_size - index_in;
        // copy first part
        sec_ret = memcpy_s(destination_p, l1, (void *)(lmscontrol->log_buffer_start + index_in), l1);
        if (sec_ret != EOK) {
            return;
        }
        // copy second part
        sec_ret = memcpy_s((void *)(destination_p + l1), len - l1, (void *)lmscontrol->log_buffer_start, len - l1);
    }
    if (sec_ret != EOK) {
        return;
    }
}

static uint32_t log_buffer_get_used(const log_buffer_reader_control_t *lmscontrol, uint32_t read_i, uint32_t write_i)
{
    uint32_t in_use;
    /* Check how much space is available. Use -1 to ensure the log_section_control->write ==
     * log_section_control->read means the buffer is EMPTY.
     * Without the -1, the log_section_control->write COULD wrap and catch up with log_section_control->read. */
    if (read_i <= write_i) {
        in_use = write_i - read_i;
    } else {
        /* log_section_control->write has wrapped, but log_section_control->read has not yet. */
        in_use = (lmscontrol->log_buffer_size - read_i) + write_i;
    }
    return in_use;
}

static log_reader_ret_t log_buffer_reader_get_msg_header(log_memory_region_section_t lmsec,
                                                         const log_buffer_header_t *lb_header, uint32_t m_index)
{
    const log_buffer_reader_control_t *lmsec_reader_control = &g_buffer_reader_control[lmsec];
    uint32_t in_use;
    uint32_t l_write_i;

    if (lb_header == NULL) {
        return LOG_READER_RET_ERROR_IN_PARAMS;
    }

    // Get the write index and check if there is at least a header to read
    l_write_i = g_section_control[lmsec]->write;
    if (l_write_i >= g_buffer_reader_control[lmsec].log_buffer_size) {
        return LOG_READER_RET_ERROR_CORRUPT_SHARED_MEMORY;
    }

    in_use = log_buffer_get_used(lmsec_reader_control, m_index, l_write_i);  // data between the index and the write
    if (in_use == 0) {
        return LOG_READER_RET_THERE_IS_NO_NEXT_MESSAGE;
    } else if (in_use < sizeof(log_buffer_header_t) || (in_use >= g_buffer_reader_control[lmsec].log_buffer_size)) {
        return LOG_READER_RET_ERROR_CORRUPT_SHARED_MEMORY;
    } else {
        log_buffer_copy_consecutive(lmsec_reader_control, (uint8_t *)lb_header, m_index, sizeof(log_buffer_header_t));
        return LOG_READER_RET_OK;
    }
}

/*
 *  Public function definitions
 */
log_reader_ret_t log_buffer_reader_init(void)
{
    log_memory_section_params_t ms_params;
    log_reader_ret_t ret_val;

    // Initialize the control structures for the different regions
    for (uint8_t lmsec = LOG_MEMORY_REGION_SECTION_0; lmsec < LOG_MEMORY_REGION_MAX_NUMBER; lmsec++) {
        uint32_t l_read_i;
        log_memory_section_get((log_memory_region_section_t)lmsec, &ms_params);
        g_section_control[lmsec] = log_memory_section_get_control((log_memory_region_section_t)lmsec);

        if (!pointer_in_log_memory_region((uintptr_t)ms_params.start)) {
            panic(PANIC_LOG, __LINE__);
            return LOG_READER_RET_ERROR_OVERFLOW_ON_DISCARDING;
        }
        if (!pointer_in_log_memory_region((uintptr_t)ms_params.start + ms_params.length - 1)) {
            panic(PANIC_LOG, __LINE__);
            return LOG_READER_RET_ERROR_OVERFLOW_ON_DISCARDING;
        }

        if (!pointer_in_log_memory_region((uintptr_t)ms_params.start) ||
            !pointer_in_log_memory_region((uint32_t)((uintptr_t)ms_params.start) + ms_params.length - 1)) {
            return LOG_READER_RET_ERROR_CORRUPT_SHARED_MEMORY;
        }

        g_buffer_reader_control[lmsec].log_buffer_start = ms_params.start;  // first element
        g_buffer_reader_control[lmsec].log_buffer_size = ms_params.length;  // buffer size

        l_read_i = g_section_control[lmsec]->read;

        if (l_read_i >= g_buffer_reader_control[lmsec].log_buffer_size) {
            return LOG_READER_RET_ERROR_CORRUPT_SHARED_MEMORY;
        }

        g_buffer_reader_control[lmsec].lb_next_to_consume = l_read_i;
        ret_val = LOG_READER_RET_OK;
    }
    return ret_val;
}

log_reader_ret_t log_buffer_reader_lock_next(log_memory_region_section_t *lmsec, const log_buffer_header_t *lb_header)
{
    log_reader_ret_t helper_log_ret;

    // when the log buffer is full,"read" is modified by unknown,so Synchronize to the "consume".
    for (uint8_t i_lmsec = LOG_MEMORY_REGION_SECTION_0; i_lmsec < LOG_MEMORY_REGION_MAX_NUMBER; i_lmsec++) {
        uint32_t read = g_section_control[i_lmsec]->read;
        if (g_buffer_reader_control[i_lmsec].lb_next_to_consume != read) {
            g_buffer_reader_control[i_lmsec].lb_next_to_consume = g_section_control[i_lmsec]->read;
        }
    }

    *lmsec = LOG_MEMORY_REGION_MAX_NUMBER;

    for (uint8_t i_lmsec = LOG_MEMORY_REGION_SECTION_0; i_lmsec < LOG_MEMORY_REGION_MAX_NUMBER; i_lmsec++) {
        helper_log_ret = log_buffer_reader_get_msg_header((log_memory_region_section_t)i_lmsec, lb_header,
                                                          g_buffer_reader_control[i_lmsec].lb_next_to_consume);
        if (helper_log_ret == LOG_READER_RET_OK) {
            *lmsec = i_lmsec;
            break;
        }
    }

#if CORE_NUMS > 1
    log_buffer_header_t c_lb_header = { 0 };  // candidate header
    if (*lmsec < (LOG_MEMORY_REGION_MAX_NUMBER - 1)) {
        errno_t sec_ret;
        // There is already one candidate
        for (uint32_t i_lmsec = ((uint32_t)*lmsec + 1); i_lmsec < LOG_MEMORY_REGION_MAX_NUMBER; i_lmsec++) {
            helper_log_ret = log_buffer_reader_get_msg_header((log_memory_region_section_t)i_lmsec, &c_lb_header,
                                                              g_buffer_reader_control[i_lmsec].lb_next_to_consume);
            if (helper_log_ret != LOG_READER_RET_OK) {
                continue;
            }
            if ((lb_header->time_us - c_lb_header.time_us) == 0) {
                continue;
            }
            sec_ret = memcpy_s((void *)lb_header, sizeof(log_buffer_header_t),
                               (const void *)&c_lb_header, sizeof(log_buffer_header_t));
            if (sec_ret != EOK) {
                return LOG_READER_RET_ERROR;
            }
            *lmsec = i_lmsec;
        }
    }
#endif
    if (*lmsec == LOG_MEMORY_REGION_MAX_NUMBER) {
        // no new messages found
        return LOG_READER_RET_THERE_IS_NO_NEXT_MESSAGE;
    }
    return LOG_READER_RET_OK;
}

log_reader_ret_t log_buffer_get_used_space(log_memory_region_section_t lmsec, uint32_t *used_space)
{
    log_reader_ret_t ret_value;
    uint32_t l_write_i;
    const log_buffer_reader_control_t *lmsec_reader_control;

    ret_value = LOG_READER_RET_OK;

    lmsec_reader_control = &g_buffer_reader_control[lmsec];

    l_write_i = g_section_control[lmsec]->write;
    if (l_write_i >= g_buffer_reader_control[lmsec].log_buffer_size) {
        return LOG_READER_RET_ERROR_CORRUPT_SHARED_MEMORY;
    }
    *used_space = log_buffer_get_used(lmsec_reader_control,
                                      lmsec_reader_control->lb_next_to_consume,
                                      l_write_i);  // data between the index and the write

    return ret_value;
}

log_reader_ret_t log_buffer_reader_claim_next(log_memory_region_section_t lmsec, uint8_t **r1,
                                              uint32_t *len1, uint8_t **r2, uint32_t *len2)
{
    uint32_t ret_r1;
    log_buffer_header_t lb_header = { 0 };
    const log_buffer_reader_control_t *lmsec_reader_control = &g_buffer_reader_control[lmsec];
    log_reader_ret_t ret_value;
    uint32_t l_write_i;
    uint32_t new_next_to_consume;

    // Get the next message header
    ret_value = log_buffer_reader_get_msg_header(lmsec, &lb_header, lmsec_reader_control->lb_next_to_consume);
    if (ret_value == LOG_READER_RET_OK) {
        if (lb_header.length <= sizeof(log_buffer_header_t)) {
            uapi_diag_error_log(0, "[log_reader][header]length error, len = 0x%x", lb_header.length);
            return LOG_READER_RET_ERROR;
        }
        // Get the buffer pointers
        ret_r1 = circled_index(lmsec_reader_control,
                               lmsec_reader_control->lb_next_to_consume + sizeof(log_buffer_header_t));
        if (ret_r1 + (lb_header.length - sizeof(log_buffer_header_t)) <= lmsec_reader_control->log_buffer_size) {
            /* 'Normal' case */
            *r1 = lmsec_reader_control->log_buffer_start + ret_r1;
            *len1 = lb_header.length - (uint32_t)sizeof(log_buffer_header_t);
            *r2 = NULL;
            *len2 = 0;
        } else {
            /* message divided at the end of the buffer case */
            *r1 = lmsec_reader_control->log_buffer_start + ret_r1;
            *len1 = lmsec_reader_control->log_buffer_size - ret_r1;
            *r2 = lmsec_reader_control->log_buffer_start;
            *len2 = (lb_header.length - (uint32_t)sizeof(log_buffer_header_t)) - *len1;
        }
        if ((*len1 > lmsec_reader_control->log_buffer_size) || (*len2 > lmsec_reader_control->log_buffer_size)) {
            uapi_diag_error_log(0, "[log_reader][data]length error, len1 = 0x%x, len2 = 0x%x", *len1, *len2);
            return LOG_READER_RET_ERROR_OVERFLOW_ON_DISCARDING;
        }

        // Assert if log_section_control->read tries to overflow lb_next_to_consume
        l_write_i = g_section_control[lmsec]->write;
        new_next_to_consume = circled_index(lmsec_reader_control,
                                            lmsec_reader_control->lb_next_to_consume + lb_header.length);
        // equivalent to:  if (write > next_to_consume) {
        if (((lmsec_reader_control->lb_next_to_consume >= l_write_i) ||
             ((new_next_to_consume <= l_write_i) ||
              (new_next_to_consume > lmsec_reader_control->lb_next_to_consume))) &&
            ((lmsec_reader_control->lb_next_to_consume <= l_write_i) ||
             ((new_next_to_consume <= lmsec_reader_control->lb_next_to_consume) ||
              (new_next_to_consume > l_write_i)))) {
            // Move the lb_next_to_consume data
            log_buffer_reader_control_t *writable_lmsec_reader_control = &g_buffer_reader_control[lmsec];
            writable_lmsec_reader_control->lb_next_to_consume = new_next_to_consume;
            ret_value = LOG_READER_RET_OK;
        } else {
            uapi_diag_error_log(0, "[log_reader][pointer]write_p=0x%x, read_p=0x%x, new_read_p=0x%x\r\n", l_write_i,
                lmsec_reader_control->lb_next_to_consume, new_next_to_consume);
            ret_value = LOG_READER_RET_ERROR_CORRUPT_SHARED_MEMORY;
        }
    }

    return ret_value;
}

log_reader_ret_t log_buffer_reader_discard(log_memory_region_section_t lmsec)
{
    log_buffer_header_t lb_header;
    const log_buffer_reader_control_t *lmscontrol = &g_buffer_reader_control[lmsec];
    log_reader_ret_t ret_value;
    uint32_t l_read_i;
    uint32_t new_lb_read;

    if (lmsec >= LOG_MEMORY_REGION_MAX_NUMBER) {
        return LOG_READER_RET_ERROR_IN_PARAMS;
    }

    memset_s((void *)&lb_header, sizeof(log_buffer_header_t), 0, sizeof(log_buffer_header_t));

    // Get last read pointer message header message header
    l_read_i = g_section_control[lmsec]->read;
    ret_value = log_buffer_reader_get_msg_header(lmsec, &lb_header, l_read_i);
    if (ret_value == LOG_READER_RET_OK) {
        new_lb_read = circled_index(lmscontrol, l_read_i + lb_header.length);
        if (((lmscontrol->lb_next_to_consume <= l_read_i) ||
             ((new_lb_read <= lmscontrol->lb_next_to_consume) &&
              (new_lb_read > l_read_i))) &&
            ((lmscontrol->lb_next_to_consume >= l_read_i) ||
             ((new_lb_read <= lmscontrol->lb_next_to_consume) ||
              (new_lb_read > l_read_i))) &&
            (new_lb_read < lmscontrol->log_buffer_size)) {
            g_section_control[lmsec]->read = new_lb_read;
        } else {
            return LOG_READER_RET_ERROR_CORRUPT_SHARED_MEMORY;  // Trying to overflow
        }
    }

    return LOG_READER_RET_OK;
}

void log_buffer_reader_error_recovery(log_memory_region_section_t lmsec)
{
    g_section_control[lmsec]->read = g_section_control[lmsec]->write;
}