/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   PANIC module implementation
 * Author:
 * Create:
 */
#include "systick.h"
#include "non_os.h"
#ifdef SUPPORT_DFX_PRESERVE
#include "preserve.h"
#endif
#include "soc_osal.h"
#include "arch_barrier.h"
#ifdef SUPPORT_CPU_TRACE
#include "cpu_trace.h"
#endif
#ifdef SUPPORT_DFX_LOG
#include "log_oml_exception.h"
#include "log_def.h"
#include "log_printf.h"
#include "diag_log.h"
#endif
#include "debug_print.h"

#ifdef USE_CMSIS_OS
#ifdef __LITEOS__
#include "los_task_pri.h"
#endif
#endif
#ifdef SUPPORT_CPU_UTILS
#include "cpu_utils.h"
#endif
#ifdef SUPPORT_CRASH_DATA_RAM
#include "crash_data.h"
#endif

#ifdef SUPPORT_WATCHDOG
#include "watchdog.h"
#endif
#include "panic.h"

#define NON_OS_ENTER_MAX_NUM 8
#define MAX_STR_LEN  64

static bool g_already_panicking = false;

static lib_panic_dump_callback g_panic_dump_callback = NULL;
#ifdef SUPPORT_CPU_TRACE
static uint32_t g_panic_cpu_trace_pc = 0x0;
static uint32_t g_panic_cpu_trace_lr = 0x0;
static uint32_t g_panic_cpu_trace_sp = 0x0;
#endif

#if defined(__ICCARM__)
static lib_panic_dump_callback_with_param g_panic_dump_wear_callback = NULL;
#endif

#if (CORE == BT) && (NON_OS_CRITICAL_RECORD == YES)
static volatile uint32_t g_non_os_critical_enter_lr[NON_OS_ENTER_MAX_NUM] = { 0 };
static volatile uint32_t g_non_os_critical_exit_lr[NON_OS_ENTER_MAX_NUM] = { 0 };
#endif

void register_panic_dump_callback(lib_panic_dump_callback callback)
{
    if (callback != NULL) {
        g_panic_dump_callback = callback;
    }
}

#if defined(__ICCARM__)
void register_panic_dump_callback_with_param(lib_panic_dump_callback_with_param callback)
{
    if (callback != NULL) {
        g_panic_dump_wear_callback = callback;
    }
}
#endif

static bool panic_trigger_callback_with_param(panic_id_t origin, uint32_t code, uint32_t caller)
{
    UNUSED(origin);
    UNUSED(code);
    UNUSED(caller);
#if defined(__ICCARM__)
    char str[MAX_STR_LEN] = { 0 };
    int ret = sprintf_s(str, sizeof(str), "panic:origin:0x%X, code:0x%X, caller:0x%X", origin, code, caller);
    if (ret < 0) {
        return false;
    }
    PRINT("set info err: ret:%d, id:%d, code:0x%x, call:0x%x", ret, origin, code, caller);
    if (g_panic_dump_wear_callback != NULL) {
        g_panic_dump_wear_callback(str);
    }
    return true;
#else
    return false;
#endif
}

#if (CORE == BT) && (NON_OS_CRITICAL_RECORD == YES)
static void panic_critical_record(critical_statistic_mode_e mode, uint32_t address, uint16_t nestings)
{
    UNUSED(mode);
    UNUSED(address);
    UNUSED(nestings);
    if (mode == CRITICAL_ENTER) {
        if ((get_already_panicking() == false) && (nestings < NON_OS_ENTER_MAX_NUM)) {
            g_non_os_critical_enter_lr[nestings] = address;
        }
    } else {
        if ((get_already_panicking() == false) && (nestings < NON_OS_ENTER_MAX_NUM)) {
            g_non_os_critical_exit_lr[nestings] = address;
        }
    }
}
#endif

void panic_init(void)
{
    panic_register_deal_callback(panic_deal);
#if (CORE == BT) && (NON_OS_CRITICAL_RECORD == YES)
    non_os_register_critical_record(panic_critical_record);
#endif
}

void panic_deinit(void) {}

void panic_wait_forever(void)
{
    // unlike the hardfault handler 'i' does not need to be static, since we trust our stack
    volatile uint8_t i = 1;
    // Loop forever - or until jlink changes i
    while (i != 0) {
#ifdef SUPPORT_WATCHDOG
        uapi_watchdog_kick();
#endif
    }
}

bool get_already_panicking(void)
{
    return g_already_panicking;
}

static void panic_deal_log(panic_id_t origin, uint32_t code, uint32_t caller)
{
    UNUSED(origin);
    UNUSED(code);
    UNUSED(caller);
#ifdef SUPPORT_DFX_LOG
    uapi_diag_error_log3(0, "[panic]id:%d,code:0x%x,call:0x%x", origin, code, caller);
#endif
    PRINT("[panic]id:%d,code:0x%x,call:0x%x", origin, code, caller); // Caller need +5 maybe.

#ifdef SUPPORT_CPU_TRACE
    hal_cpu_trace_traced_cpu_t cpu;
    cpu = HAL_CPU_TRACE_TRACED_BCPU;
#if CORE == BT
    cpu = HAL_CPU_TRACE_TRACED_MCPU;
#endif
    cpu_trace_get_locked_regs(cpu, &g_panic_cpu_trace_pc, &g_panic_cpu_trace_lr, &g_panic_cpu_trace_sp);
#ifdef SUPPORT_DFX_LOG
    uapi_diag_error_log3(0, "[panic]core:%d,pc:0x%x,lr:0x%x", cpu, g_panic_cpu_trace_pc, g_panic_cpu_trace_lr);
#endif
    PRINT("[panic]core:%d,pc:0x%x,lr:0x%x", cpu, g_panic_cpu_trace_pc, g_panic_cpu_trace_lr);
#endif
}

#ifdef SUPPORT_DFX_PRESERVE
static void panic_set_desc(panic_id_t origin, uint32_t code, uint32_t caller)
{
    panic_desc_t panic_desc;
    panic_desc.origin = origin;
    panic_desc.code = code;
    panic_desc.timestamp_ms = (uint32_t)uapi_systick_get_us();
    panic_desc.caller = caller;
    set_last_panic(&panic_desc);
}

static void panic_set_exception_info(exception_info_t *exception_info)
{
#ifdef USE_CMSIS_OS
#ifdef __LITEOS__
    TSK_INFO_S task_info;
    uint32_t task_id;

    task_id = LOS_CurTaskIDGet();
    if (LOS_TaskInfoGet(task_id, &task_info) != LOS_OK) {
        return;
    }
    exception_info->sp_bottom = task_info.uwBottomOfStack;
    exception_info->exp_task_id = task_id;
    exception_info->task_array = (unsigned long)(uintptr_t)OS_TCB_FROM_TID(0);
    exception_info->task_max_num = g_taskMaxNum + 1;  // an additional task saves the running task information.
#endif
#endif
    set_exception_info(exception_info);
}
#endif

#if defined(SUPPORT_CPU_UTILS) && defined(SUPPORT_DFX_PRESERVE)
__attribute__((unused)) static void panic_set_cause(void)
{
    reboot_cause_t cause = REBOOT_CAUSE_APPLICATION_PANIC;
    cpu_utils_set_system_status_by_cause(cause);
}
#endif

static void panic_deal_wait(void)
{
#ifndef NO_TIMEOUT
#if CORE == APPS && defined(SUPPORT_CPU_UTILS)
    cpu_utils_reset_chip_with_log((cores_t)APPS, REBOOT_CAUSE_APPLICATION_PANIC);
#elif CORE == BT
    reboot_system(REBOOT_CAUSE_BT_PANIC);
    panic_wait_forever(); // Need request apps core to deal reboot.
#elif CORE == GNSS
    reboot_system(REBOOT_CAUSE_GNSS_PANIC);
    panic_wait_forever(); // Need request apps core to deal reboot.
#endif
#else
    panic_wait_forever();
#endif
}

void panic_deal(panic_id_t origin, uint32_t code, uint32_t caller)
{
    if (panic_trigger_callback_with_param(origin, code, caller) == true) { return; }

#ifdef SUPPORT_CPU_TRACE
    cpu_trace_disable();
#endif

    panic_deal_log(origin, code, caller);

    uint32_t irq = osal_irq_lock();

    if (!g_already_panicking) {
        g_already_panicking = true;
#ifdef SUPPORT_DFX_PRESERVE
#if (defined(CHIP_SW39) && (CHIP_SW39 == 1))
        duplicate_app_preserve_mem();
#endif
#ifdef SUPPORT_CRASH_DATA_RAM
        crash_data_clear();
        crash_data_set_time_s();
#endif
        panic_set_desc(origin, code, caller);

        exception_info_t exception_info = { 0 };
        exception_stack_frame_t exception_stack_frame = { 0 };
        exception_stack_frame.stacked_lr = caller;
        get_temp_pc(exception_stack_frame.stacked_pc);
        get_temp_sp(exception_info.sp); // Backtracking parsing from the panic function
#if (ARCH == CM3) || (ARCH == CM7)
        /* // the m3 core r7 is usually saved as the SP reference and is used for sp recovery. */
        __asm volatile("MOV %0, R7" : "=r"(exception_info.regs[3]));
#endif
        panic_set_exception_info(&exception_info);
        set_exception_stack_frame(&exception_stack_frame);
#if (defined BUILD_APPLICATION_STANDARD) && (CORE == MASTER_BY_ALL)
        set_exception_time_stamp();
        /* set reboot cause before memory dump */
        panic_set_cause();
#if defined(SUPPORT_CRASH_DATA_RAM) && defined(SAVE_EXC_INFO)
        crash_data_save();
#endif
#endif
#endif
    }

    if (g_panic_dump_callback != NULL) { g_panic_dump_callback(); }

#ifdef SUPPORT_DFX_LOG
    // Dump whole memory
    log_oml_memory_dump();
#endif
    panic_deal_wait();
    osal_irq_restore(irq);
}
