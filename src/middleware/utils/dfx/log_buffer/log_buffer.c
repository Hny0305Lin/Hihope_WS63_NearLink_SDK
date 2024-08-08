/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  LOG BUFFER MODULE.
 * Author:
 * Create:  2018-10-15
 */

#include "securec.h"
#include "non_os.h"
#include "panic.h"
#include "soc_osal.h"
#include "log_buffer.h"

/*
 *  Private definitions
 */
#define index_in_cbuff(x) ((x) < g_log_buffer_size)

/*
 *  Private variables
 */
static uint8_t *g_log_buffer_start;
static uint32_t g_log_buffer_size;
static volatile log_memory_section_control_t *g_log_section_control;

volatile massdata_memory_section_control_t *g_mass_section_control = NULL;

/**
 * Read an index with overflow.
 * @param index_in index in the limits of the circular buffer or up to buffer_size longer
 * @return index in the circular buffer limits
 */
static uint32_t circled_index(uint32_t index_in)
{
    uint32_t index_out;

    if (index_in < g_log_buffer_size) {
        index_out = index_in;
    } else {
        index_out = index_in - g_log_buffer_size;
    }
    if (!index_in_cbuff(index_out)) {
        panic(PANIC_LOG, __LINE__);
        return 0;
    }
    return index_out;
}

static uint32_t log_buffer_get_used(uint32_t read_i, uint32_t write_i)
{
    uint32_t in_use;
    /* Check how much space is used. Use -1 to ensure the g_log_section_control->write ==
     * g_log_section_control->read means the buffer is EMPTY.
     * Without the -1, the g_log_section_control->write COULD wrap and catch up with g_log_section_control->read. */
    if (read_i <= write_i) {
        in_use = write_i - read_i;
    } else {
        /* g_log_section_control->write has wrapped, but g_log_section_control->read has not yet. */
        in_use = (g_log_buffer_size - read_i) + write_i;
    }
    return in_use;
}

/**
 * Get the available space in the buffer
 * @param av if return is LOG_RET_OK the available data will be stored in the pointer given
 * @return LOG_RET_OK or an error value
 */
static inline log_ret_t log_buffer_get_available(uint32_t *av)
{
    uint32_t l_read_i = g_log_section_control->read;
    uint32_t l_write_i = g_log_section_control->write;

    /* Check how much space is available. Use -1 to ensure the g_log_section_control->write ==
     * g_log_section_control->read means the buffer is EMPTY.
     * Without the -1, the g_log_section_control->write COULD wrap and catch up with g_log_section_control->read. */
    if (!index_in_cbuff(l_read_i) || !index_in_cbuff(l_write_i)) { return LOG_RET_ERROR_CORRUPT_SHARED_MEMORY; }

    if (l_read_i <= l_write_i) {
        // l_read_i and l_write_i has been checked to be in safe boundaries.
        *av = (g_log_buffer_size - 1u) - (l_write_i - l_read_i);
    /* g_log_section_control->write has wrapped, but g_log_section_control->read has not yet. */
    // l_read_i and l_write_i has been checked to be in safe boundaries.
    } else { *av = (l_read_i - l_write_i) - 1u; }
    return LOG_RET_OK;
}

/**
 * Add data to the circular buffer with updating the write index.
 * @param data buffer with the data to store
 * @param data_len data length in bytes
 */
static void log_buffer_helper_add(const uint8_t *data, const uint32_t data_len)
{
    errno_t sec_ret;
    uint32_t l_write = g_log_section_control->write;
    size_t to_end = g_log_buffer_size - l_write;

    if (to_end >= data_len) {
        /* Entire message fits. */
        sec_ret = memcpy_s((void *)(g_log_buffer_start + l_write), to_end, data, data_len);
    } else {
        /* Message needs to be segmented. Write to end of buffer and then the remainder from the beginning. */
        sec_ret = memcpy_s((void *)(g_log_buffer_start + l_write), to_end, data, to_end);
        if (sec_ret != EOK) {
            return;
        }
        sec_ret = memcpy_s((void *)g_log_buffer_start, g_log_section_control->read, &data[to_end], data_len - to_end);
    }
    if (sec_ret != EOK) {
        return;
    }

    g_log_section_control->write = circled_index(l_write + data_len);
}

/**
 * Scatter gather write to log_buffer.
 * @param length array of lengths for the different buffers, it admits 0 as a value of one of them
 * @param buffer array of values containig the buffers to the data to save in the log buffer
 * @param was_empty if a pointer != NULL is given it will store a return value
 *        indicating the buffer was empty when the write was done.
 * @return LOG_RET_OK or an error value
 */
void log_buffer_write(const log_buffer_header_t *lb_header, const uint8_t *buffer, bool *was_empty)
{
    uint32_t l_read_i;
    uint32_t l_write_i;
    // Add the header
    log_buffer_helper_add((const uint8_t *)lb_header, sizeof(log_buffer_header_t));

    // Add the buffer
    log_buffer_helper_add(buffer, lb_header->length - sizeof(log_buffer_header_t));

    l_read_i = g_log_section_control->read;
    l_write_i = g_log_section_control->write;

    *was_empty = (log_buffer_get_used(l_read_i, l_write_i) <= lb_header->length);
}
#ifdef FEATURE_PLT_LB_CHECK

uint8_t *g_p_log_buf_sdt_cur = NULL;
uint8_t *g_p_log_buf_sdt_beg = NULL;
uint8_t *g_p_log_buf_sdt_end = NULL;

static uint8_t log_buffer_check_rlw(uint8_t *beg, uint8_t *end)
{
    uint8_t *cur = NULL;
    log_buffer_header_t *hdr = NULL;

    if ((beg == NULL) || (end == NULL)) {
        return 0xFF;
    }

    if (beg >= end) {
        return 0xFE;
    }

    cur = beg;
    while (cur < end) {
        hdr = (log_buffer_header_t *)(cur);
        if (!lb_magic_check(hdr)) {
            g_p_log_buf_sdt_beg = beg;
            g_p_log_buf_sdt_cur = cur;
            g_p_log_buf_sdt_end = end;

            return LOG_BUF_RLW_MAGIC_ERROR;
        }

        cur += hdr->length;
    }

    return LOG_BUF_RET_OK;
}

#define MAX_ONE_LOG_CPY_LENGTH 100
#define G_LB_BUF_LEN 200
static uint8_t g_lb_buf[G_LB_BUF_LEN] = { 0 };

static uint8_t log_buffer_check_last_one(uint8_t **beg, uint8_t *end, uint8_t **cur, log_buffer_header_t *hdr)
{
    uint32_t last_len = end - (*cur);
    uint8_t *buf = g_lb_buf;
    memset_s(buf, sizeof(g_lb_buf), 0, G_LB_BUF_LEN);
    if (memcpy_s(buf, sizeof(g_lb_buf), *cur, last_len) != EOK) {
        return LOG_RET_MEMCPY_ERROR;
    }
    *beg = (uint8_t *)(g_log_buffer_start);
    if (memcpy_s(buf + last_len, sizeof(g_lb_buf) - last_len, *beg, MAX_ONE_LOG_CPY_LENGTH) != EOK) {
        return LOG_RET_MEMCPY_ERROR;
    }
    hdr = (log_buffer_header_t *)(buf);

    /* magic check */
    if (!lb_magic_check(hdr)) {
        g_p_log_buf_sdt_beg = *beg;
        g_p_log_buf_sdt_cur = *cur;
        g_p_log_buf_sdt_end = end;
        return LOG_RET_RGW_LASTONE_MAGIC_ERROR;
    }
    *cur = (*beg) + ((log_buffer_header_t)(*hdr)).length - last_len;
    return LOG_BUF_RET_OK;
}

static uint8_t log_buffer_check_rgw(uint8_t *ori_beg, uint8_t *ori_end)
{
    uint8_t *beg = ori_beg;
    uint8_t *end = ori_end;
    uint8_t *cur = NULL;
    log_buffer_header_t *hdr = NULL;
    uint8_t last_one = 0;

    if ((beg == NULL) || (end == NULL)) { return 0xFF; }
    if (beg <= end) { return 0xFE; }
    cur = beg;
    end = (uint8_t *)(g_log_buffer_start + g_log_buffer_size);
    while (cur < end) {
        if ((cur + sizeof(log_buffer_header_t)) >= end) { // judge last one
            last_one = 1;
            break;
        }
        hdr = (log_buffer_header_t *)(cur);
        if ((cur + hdr->length) > end) {
            last_one = 1;
            break;
        }
        if (!lb_magic_check(hdr)) { // magic check
            g_p_log_buf_sdt_beg = beg;
            g_p_log_buf_sdt_cur = cur;
            g_p_log_buf_sdt_end = end;
            return LOG_RET_RGW_TOEND_MAGIC_ERROR;
        }
        if ((cur + hdr->length) == end) { // to end
            cur = (uint8_t *)(g_log_buffer_start);
            break;
        }
        cur += hdr->length;
    }
    if (last_one && log_buffer_check_last_one(&beg, end, &cur, hdr) == LOG_RET_RGW_LASTONE_MAGIC_ERROR) { // last one
        return LOG_RET_RGW_LASTONE_MAGIC_ERROR;
    }
    end = ori_end; // from head
    while (cur < end) {
        hdr = (log_buffer_header_t *)(cur);
        if (!lb_magic_check(hdr)) { // magic check
            g_p_log_buf_sdt_beg = beg;
            g_p_log_buf_sdt_cur = cur;
            g_p_log_buf_sdt_end = end;
            return LOG_RET_RGW_LASTONE_MAGIC_ERROR;
        }
        cur += hdr->length;
    }
    return LOG_BUF_RET_OK;
}

uint8_t log_buffer_check(void)
{
    uint8_t *beg = (uint8_t *)(g_log_buffer_start + g_log_section_control->read);
    uint8_t *end = (uint8_t *)(g_log_buffer_start + g_log_section_control->write);

    if (beg == end) {
        return 0;
    } else if (beg < end) {
        return log_buffer_check_rlw(beg, end);
    } else {
        return log_buffer_check_rgw(beg, end);
    }
}
#endif

/*
 *  Public function definitions
 */
void log_buffer_init(log_memory_region_section_t logsec)
{
    /* Init the buffer limits */
    log_memory_section_params_t ms_params;
    log_memory_section_get(logsec, &ms_params);
    if (pointer_in_log_memory_region((uintptr_t)ms_params.start) &&
        pointer_in_log_memory_region((uint32_t)((uintptr_t)ms_params.start) + ms_params.length - 1)) {
        g_log_buffer_start = ms_params.start;  // first element
        g_log_buffer_size = ms_params.length;  // buffer size

        g_log_section_control = log_memory_section_get_control(logsec);
    } else {
        panic(PANIC_LOG, __LINE__);
    }
}

log_ret_t log_buffer_get_available_for_next_message(uint32_t *av)
{
    log_ret_t ret_val;
    uint32_t available = 0;

    ret_val = log_buffer_get_available(&available);
    if (ret_val == LOG_RET_OK) {
        if (available <= sizeof(log_buffer_header_t)) {
            *av = 0;
        } else {
            *av = available - (uint32_t)sizeof(log_buffer_header_t);
        }
    }
    return ret_val;
}

#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES)
#include "log_trigger.h"

log_ret_t compress_log_write(const uint8_t *data, uint32_t length)
{
    uint32_t available;
    log_ret_t ret_val;
    bool trigger_flag = false;

    // Check the message length is as much the buffer size
    if ((data == NULL) || (length > (g_log_buffer_size - 1))) {
        return LOG_RET_ERROR_IN_PARAMETERS;
    }

    uint32_t irq = osal_irq_lock();
    available = 0;
    // if it fits in the buffer add it
    ret_val = log_buffer_get_available(&available);
    if (ret_val == LOG_RET_OK) {
        if (available > length) {
            // Get the read and write indexes
            uint32_t l_read_i = g_log_section_control->read;
            uint32_t l_write_i = g_log_section_control->write;

            // Add the buffer & update g_log_section_control->write
            log_buffer_helper_add(data, length);
            // get the new write index
            uint32_t new_write_index = g_log_section_control->write;
            if (!((l_write_i < l_read_i) || ((new_write_index > l_write_i) || (new_write_index < l_read_i)))) {
                osal_irq_restore(irq);
                panic(PANIC_LOG, __LINE__);
                return LOG_RET_ERROR_OVERFLOW;
            }
            if (!((l_write_i >= l_read_i) || (new_write_index < l_read_i))) {
                osal_irq_restore(irq);
                panic(PANIC_LOG, __LINE__);

                return LOG_RET_ERROR_OVERFLOW;
            }
            if (available >= g_log_buffer_size) {
                osal_irq_restore(irq);
                panic(PANIC_LOG, __LINE__);
                return LOG_RET_ERROR_OVERFLOW;
            }
        } else {
            ret_val = LOG_RET_ERROR_NOT_ENOUGH_SPACE;
        }
        if ((g_log_buffer_size - available) > COMPRESS_LOG_TRIGGER_THRESHOLD) {
            trigger_flag = true;
        }
    }
    osal_irq_restore(irq);

    if (trigger_flag) {
        log_trigger();
    }
    return ret_val;
}
#endif /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES */

#ifdef BUILD_APPLICATION_STANDARD

#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES)
static uint32_t g_mass_share_mem_size;
static uint32_t g_mass_share_mem_start;
#if (BTH_WITH_SMART_WEAR == NO) && defined(SUPPORT_IPC)
#include "ipc.h"
static uint32_t g_version_number;
static bool mass_receive_remote_info(ipc_action_t message, const volatile ipc_payload *payload,
                                     cores_t src, uint32_t id)
{
    UNUSED(message);
    UNUSED(src);
    UNUSED(id);

    if (payload != NULL) {
        g_version_number = *(uint32_t *)(uintptr_t)payload;
    }
    return true;
}
#endif

void mass_buffer_init(mass_data_memory_region_section_t sec)
{
    if (sec != MASS_MEMORY_REGION_SECTION_0 && sec != MASS_MEMORY_REGION_SECTION_1) {
        return;
    }
    massdata_memory_region_control_t *mass_ctrl = (massdata_memory_region_control_t *)(uintptr_t)MASSDATA_REGION_START;
    g_mass_section_control = &(mass_ctrl->section_control[sec]);

    g_mass_share_mem_start = g_mass_section_control->region_start;
    g_mass_share_mem_size = g_mass_section_control->region_len;
    g_mass_section_control->water_line = g_mass_share_mem_size / CHR_BUFFER_WL_RATIO;
#if (BTH_WITH_SMART_WEAR == NO)
    ipc_register_handler(IPC_ACTION_MASS_DATA_INFORM, mass_receive_remote_info);
#endif
}
#endif

#if (BTH_WITH_SMART_WEAR == YES) && defined(SUPPORT_IPC)
#include "ipc.h"
#include "ipc_actions.h"

void massdata_record_system_error(uint8_t event_id, uint8_t info1, uint8_t info2, uint8_t info3)
{
#if (CORE == BT)
    ipc_payload_mass_data_type chr_info;
    chr_info.type = MASS_ERROR_POINT;
    chr_info.event_id = event_id;
    chr_info.info1 = info1;
    chr_info.info2 = info2;
    chr_info.info3 = info3;

    (void)ipc_spin_send_message_timeout(CORES_APPS_CORE,
                                        IPC_ACTION_MASS_DATA_INFORM,
                                        (ipc_payload *)((void *)&chr_info),
                                        sizeof(chr_info),
                                        IPC_PRIORITY_LOWEST,
                                        false,
                                        0);
#else
    UNUSED(event_id);
    UNUSED(info1);
    UNUSED(info2);
    UNUSED(info3);
#endif
}

void massdata_record_system_event(uint8_t event_id, uint8_t info1, uint8_t info2, uint8_t info3)
{
#if (CORE == BT)
    ipc_payload_mass_data_type chr_info;
    chr_info.type = MASS_EVENT_POINT;
    chr_info.event_id = event_id;
    chr_info.info1 = info1;
    chr_info.info2 = info2;
    chr_info.info3 = info3;

    (void)ipc_spin_send_message_timeout(CORES_APPS_CORE,
                                        IPC_ACTION_MASS_DATA_INFORM,
                                        (ipc_payload *)((void *)&chr_info),
                                        sizeof(chr_info),
                                        IPC_PRIORITY_LOWEST,
                                        false,
                                        0);
#else
    UNUSED(event_id);
    UNUSED(info1);
    UNUSED(info2);
    UNUSED(info3);
#endif
}

void massdata_record_system_info_with_extend(chr_extend_data_t *extend_data)
{
    if ((extend_data == NULL) ||
        (extend_data->type != MASS_EVENT_POINT_EXTEND && extend_data->type != MASS_ERROR_POINT_EXTEND)) {
        panic(PANIC_LOG, MASS_EVENT_POINT_EXTEND);
        return ;
    }
    ipc_payload_mass_data_extend_type chr_data = { 0 };
    chr_data.mass_data.type = extend_data->type;
    chr_data.mass_data.event_id = extend_data->event_id;
    chr_data.mass_data.info1 = extend_data->info1;
    chr_data.mass_data.info2 = extend_data->info2;
    chr_data.mass_data.info3 = extend_data->info3;
    chr_data.data_len = extend_data->data_len;
    if (memcpy_s(chr_data.data, CHR_EXTEND_PARAM_MAX_LEN, extend_data->data, extend_data->data_len) != EOK) {
        panic(PANIC_LOG, __LINE__);
        return ;
    }

    (void)ipc_spin_send_message_timeout(CORES_APPS_CORE,
                                        IPC_ACTION_MASS_DATA_INFORM_WITH_EXTEND,
                                        (ipc_payload *)((void *)&chr_data),
                                        sizeof(ipc_payload_mass_data_extend_type),
                                        IPC_PRIORITY_LOWEST,
                                        false,
                                        0);
}

#else
#include "log_trigger.h"
#include "systick.h"
#include "log_printf.h"

#define TWS_MASSDATA_MAGIC_NUMBER 0xAA

#if (CORE == APPS)
#if (ENABLE_MASSDATA_RECORD == YES)
static uint32_t g_version_number;
#endif
void chr_record_ue(uint8_t eid, uint8_t sub_eid, uint8_t code, uint32_t sub_code)
{
#if (ENABLE_MASSDATA_RECORD == YES)
#define MS_PER_SECOND 1000
    uint32_t ret;
    size_t size;
    uint8_t eid_shift = 8;

    system_event_s_t ue_point;
    ue_point.time_stamp = (uint32_t)(uapi_systick_get_s() + get_chr_basegmt_s());
    ue_point.event_id = (0xFFFF & ((eid << eid_shift) | (sub_eid)));
    ue_point.event_info = code;
    ue_point.sub_event_info = sub_code;
    ue_point.chr_up_type = CHR_UE_JSON;
    ue_point.version = g_version_number;
    ue_point.magic_number = TWS_MASSDATA_MAGIC_NUMBER;

    size = sizeof(system_event_s_t);
    ret = mass_data_write_roll_buffer((uint8_t *)&ue_point, (uint32_t)size);
    if ((ret == MASS_OVER_BUFFER_THD) || (ret == MASS_RET_ERROR_NOT_ENOUGH_SPACE)) {
        massdata_triger_queue((uint8_t *)&ue_point, MASS_EVENT_POINT);
    }
#undef MS_PER_SECOND
#else
    UNUSED(eid);
    UNUSED(sub_eid);
    UNUSED(code);
    UNUSED(sub_code);
#endif
}

uint32_t massdata_triger_queue(const uint8_t *pay_i, uint32_t type)
{
    UNUSED(pay_i);
    UNUSED(type);
    return 0;
}

void massdata_record_system_error(uint8_t event_id, uint8_t info1, uint8_t info2, uint32_t info3)
{
#if (ENABLE_MASSDATA_RECORD == YES)
#define MS_PER_SECOND 1000
    uint32_t ret;
    size_t size;

    system_error_s_t error_seg;
    error_seg.time_stamp = (uint32_t)(uapi_systick_get_s() + get_chr_basegmt_s());
    error_seg.event_id = get_event_id(event_id, info1);
    error_seg.event_info = info2;
    error_seg.sub_event_info = info3;
    error_seg.chr_up_type = CHR_DFT;
    error_seg.version = g_version_number;
    error_seg.magic_number = TWS_MASSDATA_MAGIC_NUMBER;

    size = sizeof(system_error_s_t);
    ret = mass_data_write_roll_buffer((uint8_t *)&error_seg, (uint32_t)size);
    if ((ret == MASS_OVER_BUFFER_THD) || (ret == MASS_RET_ERROR_NOT_ENOUGH_SPACE)) {
        massdata_triger_queue((uint8_t *)&error_seg, MASS_EVENT_POINT);
    }
#undef MS_PER_SECOND
    return;
#else
    UNUSED(event_id);
    UNUSED(info1);
    UNUSED(info2);
    UNUSED(info3);
#endif
}


void massdata_record_system_event(uint8_t event_id, uint8_t info1, uint8_t info2, uint32_t info3)
{
#if (ENABLE_MASSDATA_RECORD == YES)
#define MS_PER_SECOND 1000
    uint32_t ret;
    system_event_s_t event_seg;
    size_t size;

    event_seg.time_stamp = (uint32_t)(uapi_systick_get_s() + get_chr_basegmt_s());
    event_seg.event_id = get_event_id(event_id, info1);
    event_seg.event_info = info2;
    event_seg.sub_event_info = info3;
    event_seg.chr_up_type = CHR_DFT;
    event_seg.version = g_version_number;
    event_seg.magic_number = TWS_MASSDATA_MAGIC_NUMBER;

    size = sizeof(system_event_s_t);

    ret = mass_data_write_roll_buffer((uint8_t *)&event_seg, (uint32_t)size);
    if (ret == MASS_OVER_BUFFER_THD || ret == MASS_RET_ERROR_NOT_ENOUGH_SPACE) {
        massdata_triger_queue((uint8_t *)&event_seg, MASS_EVENT_POINT);
    }
#undef MS_PER_SECOND
    return;
#else
    UNUSED(event_id);
    UNUSED(info1);
    UNUSED(info2);
    UNUSED(info3);
#endif
}

#elif (CORE == BT)
static uint8_t g_role_state;
static uint32_t g_mass_write_psn = 0;

void massdata_set_role(uint8_t role)
{
    g_role_state = role;
}

void chr_record_ue(uint8_t eid, uint8_t sub_eid, uint8_t code, uint32_t sub_code)
{
#define MS_PER_SECOND 1000
    uint32_t ret;
    uint32_t size;
    uint8_t eid_shift = 8;

    system_event_s_t ue_point;
    ue_point.time_stamp = (uint32_t)(uapi_systick_get_s() + get_chr_basegmt_s());
    ue_point.event_id = (0xFFFF & ((eid << eid_shift) | (sub_eid)));
    ue_point.event_info = code;
    ue_point.sub_event_info = (uint32_t)sub_code;
    ue_point.chr_up_type = CHR_UE_JSON;
    ue_point.version = g_version_number;
    ue_point.magic_number = TWS_MASSDATA_MAGIC_NUMBER;
    ue_point.role = g_role_state;

    uint32_t irq = osal_irq_lock();
    ue_point.psn = g_mass_write_psn++;
    osal_irq_restore(irq);

    size = sizeof(system_event_s_t);
    ret = mass_data_write_roll_buffer((uint8_t *)&ue_point, size);
    if (ret == MASS_OVER_BUFFER_THD) {
        massdata_trigger((void *)&ue_point, CORE, MASS_EVENT_POINT);
    }

#undef MS_PER_SECOND
}

void massdata_record_system_error(uint8_t event_id, uint8_t info1, uint8_t info2, uint32_t info3)
{
#define MS_PER_SECOND 1000
    uint32_t ret;
    uint32_t size;

    system_error_s_t error_seg;
    error_seg.time_stamp = (uint32_t)(uapi_systick_get_s() + get_chr_basegmt_s());
    error_seg.event_id = get_event_id(event_id, info1);
    error_seg.event_info = info2;
    error_seg.sub_event_info = (uint32_t)info3;
    error_seg.chr_up_type = CHR_DFT;
    error_seg.version = g_version_number;
    error_seg.magic_number = TWS_MASSDATA_MAGIC_NUMBER;
    error_seg.role = g_role_state;

    uint32_t irq = osal_irq_lock();
    error_seg.psn = g_mass_write_psn++;
    osal_irq_restore(irq);

    size = sizeof(system_error_s_t);
    ret = mass_data_write_roll_buffer((uint8_t *)&error_seg, size);
    if (ret == MASS_OVER_BUFFER_THD) {
        massdata_trigger((void *)&error_seg, CORE, MASS_EVENT_POINT);
    }

#undef MS_PER_SECOND

    return;
}


void massdata_record_system_event(uint8_t event_id, uint8_t info1, uint8_t info2, uint32_t info3)
{
#define MS_PER_SECOND 1000
    uint32_t ret;
    uint16_t size;

    system_event_s_t event_seg;
    event_seg.time_stamp = (uint32_t)(uapi_systick_get_s() + get_chr_basegmt_s());
    event_seg.event_id = get_event_id(event_id, info1);
    event_seg.event_info = info2;
    event_seg.sub_event_info = (uint32_t)info3;
    event_seg.chr_up_type = CHR_DFT;
    event_seg.version = g_version_number;
    event_seg.magic_number = TWS_MASSDATA_MAGIC_NUMBER;
    event_seg.role = g_role_state;

    uint32_t irq = osal_irq_lock();
    event_seg.psn = g_mass_write_psn++;
    osal_irq_restore(irq);

    size = sizeof(system_event_s_t);

    ret = mass_data_write_roll_buffer((uint8_t *)&event_seg, size);
    if (ret == MASS_OVER_BUFFER_THD) {
        massdata_trigger((void *)&event_seg, CORE, MASS_EVENT_POINT);
    }

#undef MS_PER_SECOND
    return;
}

#endif
#endif

// write mass data to share mem roll buffer, index by core type
#if (ENABLE_MASSDATA_RECORD == YES)
uint32_t mass_data_write_roll_buffer(const uint8_t *data, uint32_t length)
{
    uint32_t remain_size, to_end, read, write, size, water_line;
    errno_t ret;

    // check the message length is as much the buffer size
    if ((data == NULL) || (g_mass_section_control == NULL)) { return MASS_RET_ERROR_IN_PARAMETERS; }

    uint32_t irq = osal_irq_lock();
    read = g_mass_section_control->read;
    write = g_mass_section_control->write;
    size = g_mass_section_control->region_len;
    to_end = size - write;

    // check if mem overflow
    if ((read > g_mass_share_mem_size) || (write > g_mass_share_mem_size) || (size > g_mass_share_mem_size) ||
        (g_mass_section_control->region_start != g_mass_share_mem_start)) {
        // recorvy mem offset
        g_mass_section_control->read = 0;
        g_mass_section_control->write = 0;
        g_mass_section_control->region_start = g_mass_share_mem_start;
        g_mass_section_control->region_len = g_mass_share_mem_size;
        osal_irq_restore(irq);
        return MASS_RET_ERROR_CORRUPT_SHARED_MEMORY;
    }

    remain_size = (read <= write) ? (size - (write - read + 1)) : (read - write - 1);
    water_line = g_mass_section_control->water_line;

    if (remain_size < length) {
        osal_irq_restore(irq);
        return MASS_RET_ERROR_NOT_ENOUGH_SPACE;
    }

    if (length <= to_end) {
        ret = memcpy_s((void *)((uintptr_t)(g_mass_section_control->region_start + write)), to_end, data, length);
        if (ret != EOK) {
            osal_irq_restore(irq);
            return MASS_MEM_COPY_FAIL;
        }
    } else {
        ret = memcpy_s((void *)((uintptr_t)(g_mass_section_control->region_start + write)), to_end, data, to_end);
        if (ret != EOK) {
            osal_irq_restore(irq);
            return MASS_MEM_COPY_FAIL;
        }
        ret = memcpy_s((void *)((uintptr_t)(g_mass_section_control->region_start)),
            length - to_end, data + to_end, length - to_end);
        if (ret != EOK) {
            osal_irq_restore(irq);
            return MASS_MEM_COPY_FAIL;
        }
    }

    write = (write + length) % size;

    g_mass_section_control->write = write;

    osal_irq_restore(irq);

    if (remain_size < water_line) { return MASS_OVER_BUFFER_THD; }

    return MASS_RET_OK;
}

void log_buffer_record_system_event(uint32_t chr_event)
{
    if (chr_event == CHR_VALUE_NULL) {
        return;
    }

    uint8_t event_id;
    uint8_t info1;
    uint8_t info2;
    uint8_t info3;

    event_id = (chr_event >> CHR_VALUE_SHFIT_24BIT) & CHR_VALUE_MASK;
    info1 = (chr_event >> CHR_INFO1_SHFIT_16BIT) & CHR_VALUE_MASK;
    info2 = (chr_event >> CHR_INFO2_SHFIT_8BIT) & CHR_VALUE_MASK;
    info3 = chr_event & CHR_VALUE_MASK;

    massdata_record_system_event(event_id, info1, info2, info3);
}

void log_buffer_record_system_error(uint32_t chr_error)
{
    if (chr_error == CHR_VALUE_NULL) {
        return;
    }

    uint8_t error_id;
    uint8_t info1;
    uint8_t info2;
    uint8_t info3;

    error_id = (chr_error >> CHR_VALUE_SHFIT_24BIT) & CHR_VALUE_MASK;
    info1 = (chr_error >> CHR_INFO1_SHFIT_16BIT) & CHR_VALUE_MASK;
    info2 = (chr_error >> CHR_INFO2_SHFIT_8BIT) & CHR_VALUE_MASK;
    info3 = chr_error & CHR_VALUE_MASK;

    massdata_record_system_error(error_id, info1, info2, info3);
}
#endif
#endif
