/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   LOG PRINTF MODULE
 * Author:
 * Create:
 */

#include "log_printf.h"
#include "log_common.h"
#if defined(SUPPORT_CONNECTIVITY) && defined(SUPPORT_IPC)
#include "connectivity_log.h"
#endif

#include "log_buffer.h"
#include "systick.h"
#ifdef SUPPORT_IPC
#include "ipc.h"
#endif

#define CHR_SECOND_PER_DAY  86400
#define CHR_MS_PER_SECOND   1000

#define LOG_MAX_ARGS_COUNT   7
#define LOG_MAX_ARGS_EXTEND_COUNT   7
#define LOG_SYNC_TIME_TIMEOUT_MS    5

#define LOG_CONTENT_INDEX2  2
#define LOG_OFFSET_18       18
#define LOG_OFFSET_4        4

static uint64_t g_log_basetime_ms = 0;
static uint32_t g_chr_basegmt_s = 0;

uint64_t get_log_basetime_ms(void)
{
    return g_log_basetime_ms;
}

uint32_t get_chr_basegmt_s(void)
{
    return g_chr_basegmt_s;
}

void set_chr_basegmt_s(uint32_t t)
{
    g_chr_basegmt_s = t;
    return;
}

#if CORE == APPS
#ifdef SUPPORT_IPC
void set_log_time(uint32_t rtc_time_s)
{
    uint64_t temp = rtc_time_s;
    g_log_basetime_ms = ((temp % CHR_SECOND_PER_DAY) * CHR_MS_PER_SECOND) - uapi_systick_get_ms();
    g_chr_basegmt_s = (uint32_t)(rtc_time_s - uapi_systick_get_s());

    log_set_sharemem_timestamp(g_log_basetime_ms);
    ipc_status_t ret = ipc_spin_send_message_timeout(CORES_BT_CORE,
                                                     IPC_ACTION_SET_LOG_TIME,
                                                     (ipc_payload *)(uintptr_t)&rtc_time_s,
                                                     sizeof(uint32_t),
                                                     IPC_PRIORITY_LOWEST,
                                                     false,
                                                     LOG_SYNC_TIME_TIMEOUT_MS);
    if (ret != IPC_STATUS_OK) {
        UNUSED(ret);
    }
}
#endif
#else
#ifdef IPC_NEW
#elif (defined CONFIG_DFX_SUPPORT_CUSTOM_LOG) && (CONFIG_DFX_SUPPORT_CUSTOM_LOG == DFX_YES)
#else
bool set_log_time_action_handler(ipc_action_t message, const volatile ipc_payload *payload_p, cores_t src, uint32_t id)
{
    UNUSED(message);
    UNUSED(id);
    UNUSED(src);

    uint64_t temp;

    temp = *(uint32_t *)payload_p;
    g_log_basetime_ms = (uint64_t)(((temp % CHR_SECOND_PER_DAY) * CHR_MS_PER_SECOND) - uapi_systick_get_ms());
    g_chr_basegmt_s = (uint32_t)(temp - uapi_systick_get_s());
    return true;
}
#endif
#endif

#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES)

#define LOG_HEADER_LEN_IN_UINT32_T    (sizeof(compress_log_header_t) / sizeof(uint32_t))
#define TEN_MS_AS_BASE              10
#define COMPRESS_LOG_MAGIC_HEADER   (0xA8 >> 3)
#define LOG_LIMIT_TIMEOUT_MS        5000
#define COMPRESS_LOG_COUNT_MARK     0

enum COMPRESS_LOG_CORE {
    COMPRESS_LOG_CORE_EXTEND,
    COMPRESS_LOG_CORE_BT,
    COMPRESS_LOG_CORE_HIFI,
    COMPRESS_LOG_CORE_BT_STATUS,
};

enum COMPRESS_LOG_EXTEND_CORE {
    COMPRESS_LOG_EXTEND_CORE_APP,
    COMPRESS_LOG_EXTEND_CORE_GNSS,
};

typedef struct {
    uint32_t level : 3;       // log level in enLogLevel
    uint32_t magic : 5;       // 0xA8 >> 3
    uint32_t core : 2;        // 0: EXTEND CORE, 1: BT, 2:HIFI, 3:BT STATUS
    uint32_t count : 3;       // count of variable params
    uint32_t addr : 19;       // address in .logstr
    uint32_t psn : 8;         // log sequence number
    uint32_t timestamp : 24;  // system time
    uint32_t code : 24;       // domain|module|sub module|log number
    uint32_t ext_core : 2;    // 0:APP, 1:GNSS, using with core
    uint32_t ext_count : 3;   // extended count of variable params
    uint32_t reserved : 3;
} compress_log_header_t;

typedef struct {
    uint16_t domain;
    uint16_t module;
    bool op;
} log_switch_cmd_t;

static uint8_t g_psn = 0;
static uint64_t  g_log_marktime_ms = 0;
static uint32_t  g_log_count_limit = 0;

static uint16_t g_log_switch_control[LOG_DOMAIN_MAX];

bool log_switch_is_open(uint16_t domain, uint16_t module)
{
    if ((domain >= LOG_DOMAIN_MAX) || (module >= LIB_LOG_MODULE_MAX)) {
        return true; /* default true, even if input is invalid */
    }

    if (g_log_switch_control[domain] & (1U << module)) {
        return true;
    }
    return false;
}

int16_t set_log_switch(uint16_t domain, uint16_t module, bool op)
{
    if ((domain > LOG_DOMAIN_MAX) || (module > LIB_LOG_MODULE_MAX)) {
        return ERR;
    }

    if (domain == LOG_DOMAIN_MAX) {
        reset_log_switch(op);
        return SUCC;
    }

    if (module == LIB_LOG_MODULE_MAX) {
        g_log_switch_control[domain] = op ? 0xFFFF : 0;
        return SUCC;
    }

    if (op) { /* op == true means on, otherwise off */
        g_log_switch_control[domain] |= (uint16_t)(1U << module);
    } else {
        g_log_switch_control[domain] &= (uint16_t)(~(1U << module));
    }

    return SUCC;
}

#if CORE == APPS
uint16_t remote_config_log_switch(cores_t dst, uint16_t domain, uint16_t module, bool op)
{
    log_switch_cmd_t cmd = {domain, module, op};

    return ipc_spin_send_message_timeout(dst,
                                         IPC_ACTION_SET_LOG_SWITCH,
                                         (ipc_payload *)&cmd,
                                         sizeof(log_switch_cmd_t),
                                         IPC_PRIORITY_LOWEST,
                                         false,
                                         LOG_SYNC_TIME_TIMEOUT_MS);
}
#else
bool set_log_switch_action_handler(ipc_action_t message, const volatile ipc_payload *payload_p,
                                   cores_t src, uint32_t id)
{
    UNUSED(message);
    UNUSED(id);
    UNUSED(src);

    log_switch_cmd_t *cmd = (log_switch_cmd_t *)payload_p;
    if (set_log_switch(cmd->domain, cmd->module, cmd->op) != SUCC) {
        return false;
    }

    return true;
}
#endif

void reset_log_switch(bool op)
{
    uint16_t i;
    uint16_t reset_val = op ? 0xFFFF : 0;

    for (i = 0; i < LOG_DOMAIN_MAX; i++) {
        g_log_switch_control[i] = reset_val;
    }
}

static bool check_log_switch(log_level_e log_lvl, uint32_t log_header)
{
    if (log_lvl < LOG_LEVEL_INFO) {
        return true; /* only filter log level INFO and DEBUG */
    }
    uint16_t domain = (uint16_t)get_log_domain(log_header);
    uint16_t module = (uint16_t)get_log_module(log_header);

    return log_switch_is_open(domain, module);
}

bool check_compress_log_printf_threshold(void)
{
    uint64_t log_time;
    static uint32_t  log_count = 0;
    log_count++;
    if (log_count > g_log_count_limit) {
        log_time = uapi_systick_get_ms();
        if ((log_time - g_log_marktime_ms) > LOG_LIMIT_TIMEOUT_MS) {
            g_log_marktime_ms = log_time;
            log_count = COMPRESS_LOG_COUNT_MARK;
            return true;
        } else {
            return false;
        }
    }
    return true;
}

void compress_log_init(void)
{
    g_log_count_limit = COMPRESS_LOG_COUNT_THRESHOLD;
    reset_log_switch(true);
}

void set_compress_log_count_threshold(uint32_t threshold)
{
    g_log_count_limit = threshold;
}

static void compress_printf_store_in_flash(uint32_t log_addr, uint32_t log_header, va_list args)
{
    log_level_e log_lvl = (log_level_e)get_log_lvl(log_header);
    if (log_lvl > log_get_local_log_level()) { return; }
    if (check_log_switch(log_lvl, log_header) == false) { return; }
    if (check_compress_log_printf_threshold() == false) { return; }

    uint32_t args_count = get_log_args_cnt(log_header);
    if (args_count > LOG_MAX_ARGS_COUNT + LOG_MAX_ARGS_EXTEND_COUNT) {
        args_count = LOG_MAX_ARGS_COUNT + LOG_MAX_ARGS_EXTEND_COUNT;
    }

    uint32_t log_code = get_log_code(log_header);
    uint32_t log_content_len = sizeof(compress_log_header_t) + args_count * sizeof(uint32_t);
    uint32_t log_content[LOG_HEADER_LEN_IN_UINT32_T + LOG_MAX_ARGS_COUNT + LOG_MAX_ARGS_EXTEND_COUNT];

    ((compress_log_header_t *)log_content)->level = (uint32_t)log_lvl;
    ((compress_log_header_t *)log_content)->magic = COMPRESS_LOG_MAGIC_HEADER;
#if CORE == BT
    if (log_code == BTC_MAGIC_LOG_CODE) {
        ((compress_log_header_t *)log_content)->core = COMPRESS_LOG_CORE_BT_STATUS;
    } else {
        ((compress_log_header_t *)log_content)->core = COMPRESS_LOG_CORE_BT;
    }
#elif CORE == APPS
    ((compress_log_header_t *)log_content)->core = COMPRESS_LOG_CORE_EXTEND;
    ((compress_log_header_t *)log_content)->ext_core = COMPRESS_LOG_EXTEND_CORE_APP;
#elif CORE == GNSS
    ((compress_log_header_t *)log_content)->core = COMPRESS_LOG_CORE_EXTEND;
    ((compress_log_header_t *)log_content)->ext_core = COMPRESS_LOG_EXTEND_CORE_GNSS;
#else
    ((compress_log_header_t *)log_content)->core = COMPRESS_LOG_CORE_HIFI;
#endif
    if (args_count > LOG_MAX_ARGS_COUNT) {
        ((compress_log_header_t *)log_content)->count = LOG_MAX_ARGS_COUNT;
        ((compress_log_header_t *)log_content)->ext_count = args_count - LOG_MAX_ARGS_COUNT;
    } else {
        ((compress_log_header_t *)log_content)->count = args_count;
        ((compress_log_header_t *)log_content)->ext_count = 0;
    }
    ((compress_log_header_t *)log_content)->addr = log_addr;
    ((compress_log_header_t *)log_content)->code = log_code;
    ((compress_log_header_t *)log_content)->timestamp = (uapi_systick_get_ms() + g_log_basetime_ms) / TEN_MS_AS_BASE;
    ((compress_log_header_t *)log_content)->psn = g_psn;
    g_psn++;

    if (args_count > 0) {
        for (uint32_t temp_index = 0; temp_index < args_count; temp_index++) {
            log_content[LOG_HEADER_LEN_IN_UINT32_T + temp_index] = va_arg(args, uint32_t);
        }
    }

    compress_log_write((const uint8_t *)log_content, log_content_len);
}

void compress_printf(uint32_t log_addr, uint32_t log_header, ...)
{
    va_list args;
    va_start(args, log_header);
    compress_printf_store_in_flash(log_addr, log_header, args);
    va_end(args);
}

/*
* +                     16 Bits                    + 10 Bits  +   4 Bits    +    4 Bits    +
* +--------------+--------------+------------------+----------+-------------+--------------+
* |                     log_module                 |  log_id  |             |              |
* |--------------+--------------+------------------+----------|   log_lvl   |  args_count  |
* |                     LOG_BCORE_BTC              |    0     |             |              |
* +-----------------------------------------------------------+-------------+--------------+
*/
void compress_printf_btc_info0(uint32_t log_addr, ...)
{
    va_list args;
    va_start(args, log_addr);
    compress_printf_store_in_flash(log_addr, ((uint32_t)(LOG_BCORE_BTC << LOG_OFFSET_18) |
         (uint32_t)(LOG_LEVEL_INFO << LOG_OFFSET_4) | (uint32_t)(NO_ARG)), args);
    va_end(args);
}

void compress_printf_btc_info1(uint32_t log_addr, ...)
{
    va_list args;
    va_start(args, log_addr);
    compress_printf_store_in_flash(log_addr, ((uint32_t)(LOG_BCORE_BTC << LOG_OFFSET_18) |
        (uint32_t)(LOG_LEVEL_INFO << LOG_OFFSET_4) | (uint32_t)(ONE_ARG)), args);
    va_end(args);
}

void compress_printf_btc_info2(uint32_t log_addr, ...)
{
    va_list args;
    va_start(args, log_addr);
    compress_printf_store_in_flash(log_addr, ((uint32_t)(LOG_BCORE_BTC << LOG_OFFSET_18) |
        (uint32_t)(LOG_LEVEL_INFO << LOG_OFFSET_4) | (uint32_t)(TWO_ARG)), args);
    va_end(args);
}

void compress_printf_btc_info3(uint32_t log_addr, ...)
{
    va_list args;
    va_start(args, log_addr);
    compress_printf_store_in_flash(log_addr, ((uint32_t)(LOG_BCORE_BTC << LOG_OFFSET_18) |
        (uint32_t)(LOG_LEVEL_INFO << LOG_OFFSET_4) | (uint32_t)(THREE_ARG)), args);
    va_end(args);
}

void compress_printf_btc_info4(uint32_t log_addr, ...)
{
    va_list args;
    va_start(args, log_addr);
    compress_printf_store_in_flash(log_addr, ((uint32_t)(LOG_BCORE_BTC << LOG_OFFSET_18) |
        (uint32_t)(LOG_LEVEL_INFO << LOG_OFFSET_4) | (uint32_t)(FOUR_ARG)), args);
    va_end(args);
}

void compress_printf_btc_warn0(uint32_t log_addr, ...)
{
    va_list args;
    va_start(args, log_addr);
    compress_printf_store_in_flash(log_addr, ((uint32_t)(LOG_BCORE_BTC << LOG_OFFSET_18) |
        (uint32_t)(LOG_LEVEL_WARNING << LOG_OFFSET_4) | (uint32_t)(NO_ARG)), args);
    va_end(args);
}

void compress_printf_btc_warn1(uint32_t log_addr, ...)
{
    va_list args;
    va_start(args, log_addr);
    compress_printf_store_in_flash(log_addr, ((uint32_t)(LOG_BCORE_BTC << LOG_OFFSET_18) |
        (uint32_t)(LOG_LEVEL_WARNING << LOG_OFFSET_4) | (uint32_t)(ONE_ARG)), args);
    va_end(args);
}

void compress_printf_btc_warn2(uint32_t log_addr, ...)
{
    va_list args;
    va_start(args, log_addr);
    compress_printf_store_in_flash(log_addr, ((uint32_t)(LOG_BCORE_BTC << LOG_OFFSET_18) |
        (uint32_t)(LOG_LEVEL_WARNING << LOG_OFFSET_4) | (uint32_t)(TWO_ARG)), args);
    va_end(args);
}

void compress_printf_btc_warn3(uint32_t log_addr, ...)
{
    va_list args;
    va_start(args, log_addr);
    compress_printf_store_in_flash(log_addr, ((uint32_t)(LOG_BCORE_BTC << LOG_OFFSET_18) |
        (uint32_t)(LOG_LEVEL_WARNING << LOG_OFFSET_4) | (uint32_t)(THREE_ARG)), args);
    va_end(args);
}

void compress_printf_btc_warn4(uint32_t log_addr, ...)
{
    va_list args;
    va_start(args, log_addr);
    compress_printf_store_in_flash(log_addr, ((uint32_t)(LOG_BCORE_BTC << LOG_OFFSET_18) |
        (uint32_t)(LOG_LEVEL_WARNING << LOG_OFFSET_4) | (uint32_t)(FOUR_ARG)), args);
    va_end(args);
}

#else /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO */
#include "log_def.h"
#include "log_oam_logger.h"

#ifndef CONFIG_LOG_USE_DIAG_FRAME
static void compress_printf_output_by_uart(uint32_t log_header, va_list args)
{
    log_level_e log_lvl = (log_level_e)(get_log_lvl(log_header));
    if (log_lvl > log_get_local_log_level()) {
        return;
    }

    uint32_t log_content[OML_LOG_HEADER_ARRAY_LENTH + LOG_MAX_ARGS_COUNT + OML_LOG_TAIL_LENTH];
    uint32_t log_mod;
    uint32_t uc_loop = 0;
    uint32_t file_id = get_file_id(log_header);
    uint32_t args_count = get_log_args_cnt(log_header);
    if (args_count > LOG_MAX_ARGS_COUNT) {
        args_count = LOG_MAX_ARGS_COUNT;
    }
#if CORE == BT || CHIP_BS2X
    log_content[0] = log_head_press(OM_BT);
    log_mod = LOG_BTMODULE;
#elif CORE == APPS
    log_content[0] = log_head_press(OM_IR);
    log_mod = LOG_PFMODULE;
#else
    log_mod = LOG_PFMODULE;
#endif
    log_content[1] = (uint32_t)log_lenth_and_sn_press(args_count, (uint32_t)get_log_sn_number());
    log_content[LOG_CONTENT_INDEX2] = para_press(log_mod, (uint32_t)log_lvl,
                                                 file_id, get_log_line(log_header));

    if (args_count > 0) {
        for (uc_loop = 0; uc_loop < args_count; uc_loop++) {
            log_content[OML_LOG_HEADER_ARRAY_LENTH + uc_loop] = (uint32_t)va_arg(args, uint32_t);
        }
    }

    log_content[OML_LOG_HEADER_ARRAY_LENTH + uc_loop] = OM_FRAME_DELIMITER;
    log_event((uint8_t *)log_content, (uint16_t)oal_log_lenth(args_count));
}
#else
#include "diag_rom_api.h"

static void compress_printf_output_by_uart(uint32_t log_header, va_list args)
{
    uint8_t log_lvl = get_log_lvl(log_header);
    if (log_lvl > log_get_local_log_level()) {
        return;
    }

    uint32_t log_data[OML_LOG_ALTER_PARA_MAX_NUM];
    uint8_t log_mod_id;
    uint32_t log_mod;
    uint32_t uc_loop = 0;
    uint32_t args_count = get_log_args_cnt(log_header);
    if (args_count > LOG_MAX_ARGS_COUNT) {
        args_count = LOG_MAX_ARGS_COUNT;
    }
#if CORE == BT
    log_mod_id = OM_BT;
    log_mod = LOG_BTMODULE;
#elif CORE == APPS
    log_mod_id = OM_IR;
    log_mod = LOG_PFMODULE;
#else
    log_mod = LOG_PFMODULE;
#endif
    log_lvl = sdt_to_diag_log_level(log_lvl);
    uint32_t msg_id = para_press(log_mod, log_lvl, get_file_id(log_header), get_log_line(log_header));

    if (args_count > 0) {
        for (uc_loop = 0; uc_loop < args_count; uc_loop++) {
            log_data[uc_loop] = (uint32_t)va_arg(args, uint32_t);
        }
    }

    uapi_diag_report_sys_msg(log_mod_id, msg_id, (uint8_t *)log_data, sizeof(uint32_t) * args_count, log_lvl);
}
#endif

void compress_printf(uint32_t log_header, ...)
{
    va_list args;
    va_start(args, log_header);
    compress_printf_output_by_uart(log_header, args);
    va_end(args);
}

void compress_printf_nolog(uint32_t log_header, ...)
{
    unused(log_header);
    return;
}

#endif /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG */


void compress_printf_rom_callback(uint32_t log_addr, uint32_t log_header_user, uint32_t log_header_eng, va_list args)
{
    (void)log_addr;
    (void)log_header_user;
    (void)log_header_eng;
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES)
    compress_printf_store_in_flash(log_addr, log_header_user, args);
#else
    compress_printf_output_by_uart(log_header_eng, args);
#endif
}

#if CHIP_LIBRA || CHIP_SOCMN1 || CHIP_BS25 || CHIP_BRANDY
void compress_log_no_print_rom_callback(uint32_t log_addr, uint32_t log_header_user,
                                        uint32_t log_header_eng, va_list args)
{
    (void)log_addr;
    (void)log_header_user;
    (void)log_header_eng;
    (void)args;
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
    compress_printf_output_by_uart(log_header_eng, args);
#endif
}
#endif
