/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: hard riscv interface
 */
#include "platform_core.h"
#include "exception.h"
#include "chip_io.h"
#include "preserve.h"
#include "debug_print.h"
#include "securec.h"
#include "non_os_reboot.h"
#include "reboot_porting.h"
#include "watchdog_porting.h"
#ifdef SUPPORT_CPU_TRACE
#include "cpu_trace.h"
#include "memory_config_common.h"
#endif
#ifdef SUPPORT_WATCHDOG
#include "watchdog.h"
#endif

#if (CORE == MASTER_BY_ALL)
#include "cpu_utils.h"
#endif
#if SLAVE_BY_WS53_ONLY
#include "nonos_trace.h"
#endif

#include "non_os.h"

#include "oam_trace.h"
#include "debug_print.h"
#ifndef USE_CMSIS_OS
#include "arch_encoding.h"
#include "vectors.h"
#include "interrupt_handler.h"
#endif
#include "tcxo.h"
#if defined(__LITEOS__)
#include "los_sched_pri.h"
#include "los_task_pri.h"
#include "los_sem_pri.h"
#endif
#include "fcntl.h"
#include "unistd.h"
#if defined SAVE_EXC_INFO
#include "dfx_adapt_layer.h"
#if CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES
#include "dfx_file_operation.h"
#include "sys/stat.h"
#if defined CFG_DRIVERS_NANDFLASH
#include "nandflash_config.h"
#endif
#if defined CFG_DRIVERS_MMC
#include "block.h"
#endif
#endif
#endif
#if defined(__FREERTOS__)
#include "FreeRTOS.h"
#include "encoding.h"
#endif

#ifdef CONFIG_SUPPORT_CRASHINFO_SAVE_TO_FLASH
#include "sfc.h"
#include "exception.h"
#include "board_ws63.h"
#include "partition_resource_id.h"
#include "partition.h"
#include "oam_trace.h"
#endif

#define EXCEPT_REBOOT_DELAY_MS     3000
#define IPC_NOTIFY_APPS_TIMEOUT_MS 16000
#define SW_PRINT_DELAY             200ULL
#define USER_STACK_PUSH_LENGTH  160
#define USER_STACK_OFFSET_PER   4
#define USER_STACK_PRINT_DEPTH  30
#define MAX_CALL_STACK          20
#define CALLSTACK_RA_POS        4
#define CALLSTACK_FP_POS        8
#define EXC_STACK_INFO_SIZE     8
#if defined(SAVE_EXC_INFO) && defined(CFG_DRIVERS_MMC)
#define ONE_SECTOR_SIZE         512
#define START_SECTOR_NUM      4445186
#endif
#ifdef __FREERTOS__
#define STACK_BOTTROM_INIT_VAL  0xa5a5a5a5
#else
#define STACK_BOTTROM_INIT_VAL  0xf0f0f0f
#endif
#if (ARCH == RISCV70)
#define MCAUSE_MASK             0x80000FFF
#define NMI_INTERRUPT           0x00000FFF
#else
#define NMI_INTERRUPT           0x8000000C
#endif
#if defined(__FREERTOS__)
#define SP_OFFSET               (30 * 4)
#else
#define SP_OFFSET               0x10
#endif
#define CONTEXT_OFFSET          (4 * 4 + 33 * 4 + 4)

#ifdef CONFIG_SUPPORT_CRASHINFO_SAVE_TO_FLASH
#define STACK_DEPTH 128
#define STACK_SAVE_DEPTH 10
#define STACK_SAVE_SIZE (STACK_SAVE_DEPTH * 8 + 4) /* |4B:count|4B:sp 4B:sp_content|4B:sp 4B:sp_content|...| */
#define BASE_ADDR 0x200000
#define CRASHINFO_GROUP_CNT 10
#define INVALID_SEM_ID  0xFFFFFFFF
#define TASK_NAME_MAX 23

static uint32_t g_crashinfo_flag = 0xdeadbeef;
#endif

#if defined(USE_CMSIS_OS) && defined(__LITEOS__)
extern VOID OsExcInfoDisplay(uint32_t excType, const ExcContext *excBufAddr);
extern ExcInfo g_excInfo;
#endif
static void deal_with_exception(exc_context_t *exc_buf_addr, uint32_t exc_type, reboot_cause_t reset_cause);

static hal_exception_dump_callback g_exception_dump_callback = NULL;

nmi_proc_func g_nmi_hook = do_hard_fault_handler;

exc_info_t g_exc_info;

#if defined SAVE_EXC_INFO
/* ifdef LOSCFG_EXC_SIMPLE_INFO g_xregs_map start from X4(instead of X0) to X31 */
static const uint8_t g_xregs_map[] = {
/*  tp  t0  t1  t2  s0  s1  a0 */
    2,  30, 29, 28, 15, 14, 27,
/*  a1  a2  a3  a4  a5  a6  a7 */
    26, 25, 24, 23, 22, 21, 20,
/*  s2  s3  s4  s5  s6  s7  s8 */
    13, 12, 11, 10, 9,  8,  7,
/*  s9  s10 s11 t3  t4  t5  t6 */
    6,  5,  4,  19, 18, 17, 16
};
#if CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES
static const char *g_exc_info_path = "/user/exc/exc_info.bin";
#endif
#endif

#if (SLAVE_BY_BS25_ONLY || SLAVE_BY_WS53_ONLY) && (!defined(NO_TIMEOUT))
static volatile uint32_t g_bt_nmi_reboot = 0;
static volatile uint32_t g_bt_nmi_mask = 0;
#endif
#if ((defined(BUILD_APPLICATION_STANDARD)) && (IS_MAIN_CORE == NO))

#include "ipc.h"
#include "ipc_actions.h"

static void wait_apps_refresh_flash(void)
{
#if SYS_DEBUG_MODE_ENABLE == YES
    for (uint32_t i = 0; i < WAIT_APPS_DUMP_DELAY_TIMES; i++) {
#if CORE == MASTER_BY_ALL
        uapi_watchdog_kick();
#endif
        uapi_tcxo_delay_ms((uint64_t)WAIT_APPS_DUMP_DELAY_MS_EACH_TIME);
    }
#else /* SYS_DEBUG_MODE_ENABLE == NO */
    uapi_tcxo_delay_ms((uint64_t)WAIT_APPS_REFRESH_FLASH_MS);
#endif /* SYS_DEBUG_MODE_ENABLE == YES */
}

static void notify_apps_coredump_and_wait(void)
{
    ipc_status_t ipc_returned_value;
    ipc_returned_value = ipc_spin_send_message_timeout(CORES_APPS_CORE,
                                                       IPC_ACTION_SYS_REBOOT_REQ,
                                                       NULL,
                                                       sizeof(ipc_payload),
                                                       IPC_PRIORITY_HIGHEST, false, IPC_NOTIFY_APPS_TIMEOUT_MS);
    if (ipc_returned_value != IPC_STATUS_OK) {
        UNUSED(ipc_returned_value);
    }
}
#endif

#ifndef USE_CMSIS_OS
static void do_process_exception(exc_context_t *exc_buff_addr)
{
    g_exc_info.type = (uint16_t)(exc_buff_addr->mcause);
    g_exc_info.context = exc_buff_addr;
    exc_info_display(&g_exc_info);
}

void do_trap_unknown(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - unknown exception\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_insn_misaligned(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - instruction address misaligned\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_insn_fault(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - instruction access fault\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_insn_illegal(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - illegal instruction\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_load_misaligned(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - load address misaligned\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_load_fault(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - load access fault\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_store_misaligned(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - store (or AMO) address misaligned\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_store_fault(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - store (or AMO) access fault\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_ecall_u(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - environment call from U-mode\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_ecall_s(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - environment call from S-mode\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_ecall_m(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - environment call from M-mode\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_break(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - ebreak\r\n");
    do_process_exception(exc_buff_addr);
}

void do_insn_page_fault(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - insn page fault\r\n");
    do_process_exception(exc_buff_addr);
}

void do_load_page_fault(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - load page fault\r\n");
    do_process_exception(exc_buff_addr);
}

void do_store_page_fault(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - store page fault\r\n");
    do_process_exception(exc_buff_addr);
}

void do_hard_fault(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - hard fault\r\n");
    do_process_exception(exc_buff_addr);
}

void do_lockup(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - lock up\r\n");
    do_process_exception(exc_buff_addr);
}

void default_handler(void)
{
    uint32_t hwi_index = interrupt_number_get();
    PRINT("default_handler : interrupt idx[%d]\n", hwi_index);
    while (true) { }
    UNUSED(hwi_index);
}

void nmi_handler(void)
{
    uint32_t hwi_index = interrupt_number_get();
    PRINT("nmi_handler : interrupt idx[%d]\n", hwi_index);
    while (true) { }
    UNUSED(hwi_index);
}
#endif

#ifdef SUPPORT_CALLSTACK
static void dump_call_stack(uint32_t fp)
{
    uint32_t back_fp = fp;
    uint32_t tmp_fp;
    uint32_t back_ra;
    uint32_t count = 0;

    PRINT("*******dump call stack begin*******\n");

    while ((back_fp != 0) && (back_fp != STACK_BOTTROM_INIT_VAL)) {
        tmp_fp = back_fp;
        back_ra = *((uint32_t *)(tmp_fp - CALLSTACK_RA_POS));
        back_fp = *((uint32_t *)(tmp_fp - CALLSTACK_FP_POS));
        PRINT("call stack %d -- ra = 0x%x    fp = 0x%x\n", count, back_ra, back_fp);

        count++;
        if ((count == MAX_CALL_STACK) || (back_fp == tmp_fp)) {
            break;
        }
    }

    PRINT("*******dump call stack end*******\n");
}
#else
void back_trace(uint32_t fp)
{
    uint32_t back_fp = fp + USER_STACK_PUSH_LENGTH;
    uint32_t temp_fp;
    uint32_t back_sp;
    uint32_t count = 0;

    PRINT("*******backtrace begin*******\n");

    while (back_fp != 0) {
        temp_fp = back_fp;
        back_sp = readl(temp_fp);
        PRINT("traceback %d -- sp addr= 0x%x    sp content= 0x%x\n", count, temp_fp, back_sp);
        back_fp = back_fp + USER_STACK_OFFSET_PER;

        count++;
        if (count == USER_STACK_PRINT_DEPTH) {
            break;
        }
    }
    PRINT("*******backtrace end*******\n");
    UNUSED(back_sp);
}
#endif

void exc_info_display(const exc_info_t *exc)
{
    if (exc == NULL) {
        return;
    }
    PRINT("**************Exception Information************** \n");
    PRINT("uwExcType = 0x%x\n", exc->type);

    PRINT("mepc       = 0x%x\n", exc->context->task_context.mepc);
    PRINT("mstatus    = 0x%x\n", exc->context->task_context.mstatus);
    PRINT("mtval      = 0x%x\n", exc->context->mtval);
    PRINT("mcause     = 0x%x\n", exc->context->mcause);
    PRINT("ccause     = 0x%x\n", exc->context->ccause);
#if CORE == WIFI
    PRINT("cxcptsc    = 0x%x\n", read_custom_csr(CXCPTSC));
#endif
    PRINT("ra         = 0x%x\n", exc->context->task_context.ra);
    PRINT("sp         = 0x%x\n", exc->context->task_context.sp);
    PRINT("gp         = 0x%x\n", exc->context->gp);
    PRINT("tp         = 0x%x\n", exc->context->task_context.tp);
    PRINT("t0         = 0x%x\n", exc->context->task_context.t0);
    PRINT("t1         = 0x%x\n", exc->context->task_context.t1);
    PRINT("t2         = 0x%x\n", exc->context->task_context.t2);
    PRINT("s0         = 0x%x\n", exc->context->task_context.s0);
    PRINT("s1         = 0x%x\n", exc->context->task_context.s1);
    PRINT("a0         = 0x%x\n", exc->context->task_context.a0);
    PRINT("a1         = 0x%x\n", exc->context->task_context.a1);
    PRINT("a2         = 0x%x\n", exc->context->task_context.a2);
    PRINT("a3         = 0x%x\n", exc->context->task_context.a3);
    PRINT("a4         = 0x%x\n", exc->context->task_context.a4);
    PRINT("a5         = 0x%x\n", exc->context->task_context.a5);
    PRINT("a6         = 0x%x\n", exc->context->task_context.a6);
    PRINT("a7         = 0x%x\n", exc->context->task_context.a7);
    PRINT("s2         = 0x%x\n", exc->context->task_context.s2);
    PRINT("s3         = 0x%x\n", exc->context->task_context.s3);
    PRINT("s4         = 0x%x\n", exc->context->task_context.s4);
    PRINT("s5         = 0x%x\n", exc->context->task_context.s5);
    PRINT("s6         = 0x%x\n", exc->context->task_context.s6);
    PRINT("s7         = 0x%x\n", exc->context->task_context.s7);
    PRINT("s8         = 0x%x\n", exc->context->task_context.s8);
    PRINT("s9         = 0x%x\n", exc->context->task_context.s9);
    PRINT("s10        = 0x%x\n", exc->context->task_context.s10);
    PRINT("s11        = 0x%x\n", exc->context->task_context.s11);
    PRINT("t3         = 0x%x\n", exc->context->task_context.t3);
    PRINT("t4         = 0x%x\n", exc->context->task_context.t4);
    PRINT("t5         = 0x%x\n", exc->context->task_context.t5);
    PRINT("t6         = 0x%x\n", exc->context->task_context.t6);

    PRINT("**************Exception Information end************** \n");
    return;
}

void wait_apps_prepare_for_rebooting(void)
{
#if ((defined(BUILD_APPLICATION_STANDARD)) && (IS_MAIN_CORE == NO))
    notify_apps_coredump_and_wait();
    wait_apps_refresh_flash();
#endif
}

#if MCU_ONLY
static reboot_cause_t hal_set_nmi_cause(uint16_t status)
{
    reboot_cause_t reset_cause = REBOOT_CAUSE_APPLICATION_GLOBAL;
    switch (status) {
        case NON_OS_NMI_CWDT_INT_FLAG:
            reset_cause = REBOOT_CAUSE_APPLICATION_CHIP_WDT;
            break;
        case NON_OS_NMI_XIP_CTRL_INT_FLAG:
            reset_cause = REBOOT_CAUSE_APPLICATION_XIP_CTRL;
            break;
        case NON_OS_NMI_XIP_CACHE_INT_FLAG:
            reset_cause = REBOOT_CAUSE_APPLICATION_XIP_CACHE;
            break;
        case NON_OS_NMI_MDMA_INT_FLAG:
            reset_cause = REBOOT_CAUSE_APPLICATION_MDMA;
            break;
        case NON_OS_NMI_SMDMAINT_FLAG:
            reset_cause = REBOOT_CAUSE_APPLICATION_SMDMA;
            break;
        default:
            break;
    }
    return reset_cause;
}
#endif

#if (CORE == MASTER_BY_ALL)
static reboot_cause_t hal_set_reset_cause(uint32_t id)
{
#if CORE != WIFI
    reboot_cause_t reset_cause = REBOOT_CAUSE_UNKNOWN;

    if (id == NMI_INTERRUPT) {
        reset_cause = hal_set_nmi_cause(NON_OS_NMI_CWDT_INT_FLAG);
    } else {
        reset_cause = REBOOT_CAUSE_APPLICATION_HARDFAULT;
    }
    return reset_cause;
#else
    UNUSED(id);
    return REBOOT_CAUSE_UNKNOWN;
#endif
}
#endif

static void hal_save_reg_info(exc_context_t *exc_buf_addr)
{
    exc_buf_addr->task_context.sp += SP_OFFSET;
    set_exception_info_riscv(exc_buf_addr);
}

#if defined SAVE_EXC_INFO
#ifdef SUPPORT_CALLSTACK
static void get_stack_size(uint32_t fp, uint16_t *stack_size)
{
    uint32_t back_fp = fp;
    uint32_t tmp_fp;
    uint32_t back_ra;
    while ((back_fp != 0) && (back_fp != STACK_BOTTROM_INIT_VAL)) {
        tmp_fp = back_fp;
        back_ra = *((uint32_t *)(uintptr_t)(tmp_fp - CALLSTACK_RA_POS));
        back_fp = *((uint32_t *)(uintptr_t)(tmp_fp - CALLSTACK_FP_POS));
        (*stack_size)++;
        if ((*stack_size == MAX_CALL_STACK) || (back_fp == tmp_fp)) {
            break;
        }
    }
}

static void set_exc_stack_info(exc_info_save_t **exc_info_save, uint32_t fp)
{
    exc_stack_info_t *stack = (*exc_info_save)->stack;
    uint32_t back_fp = fp;
    uint32_t tmp_fp;
    uint32_t back_ra;
    uint32_t stack_size = (*exc_info_save)->stack_size;
    uint32_t i;
    for (i = 0; i < stack_size; i++) {
        tmp_fp = back_fp;
        back_ra = *((uint32_t *)(uintptr_t)(tmp_fp - CALLSTACK_RA_POS));
        back_fp = *((uint32_t *)(uintptr_t)(tmp_fp - CALLSTACK_FP_POS));
        stack[i].ra = back_ra;
        stack[i].fp = back_fp;
    }
}
#else
static void get_stack_size(uint32_t sp, uint16_t *stack_size)
{
    uint32_t back_sp = sp;
    while (back_sp != 0) {
        back_sp = back_sp + USER_STACK_OFFSET_PER;
        (*stack_size)++;
        if (*stack_size == USER_STACK_PRINT_DEPTH) {
            break;
        }
    }
}

static void set_exc_stack_info(exc_info_save_t **exc_info_save, uint32_t sp)
{
    exc_stack_info_t *stack = (*exc_info_save)->backtrace;
    uint32_t back_sp = sp;
    uint32_t backtrace_size = (*exc_info_save)->backtrace_size;
    uint32_t i;
    for (i = 0; i < backtrace_size; i++) {
        back_sp = back_sp + USER_STACK_OFFSET_PER;
        stack[i].sp_addr = back_sp;
        stack[i].sp_content = *((uint32_t *)(back_sp));
    }
}
#endif

static void set_exc_context_info(const task_context_t *task_context, exc_info_save_t **exc_info_save)
{
    exc_info_save_t *exc_info = *exc_info_save;
    exc_info->mstatus = task_context->mstatus;
    exc_info->mepc = task_context->mepc;
    exc_info->ra = task_context->ra;
    exc_info->sp = task_context->sp;

    uint32_t i;
    const uint32_t *context_regs = (const uint32_t *)(uintptr_t)task_context;
    uint32_t *context_exc_info = (uint32_t *)(uintptr_t)exc_info;

#ifndef LOSCFG_EXC_SIMPLE_INFO
    for (i = 0; i < sizeof(g_xregs_map) / sizeof(g_xregs_map[0]); i++) {
        context_exc_info[EXC_INFO_SAVE_TP_INDEX + i] = context_regs[g_xregs_map[i]];
    }
#else
    for (i = 0; i < sizeof(g_xregs_map) / sizeof(g_xregs_map[0]); i++) {
        context_exc_info[EXC_INFO_SAVE_X4_INDEX + i] = context_regs[g_xregs_map[i]];
    }
#endif
#ifdef SUPPORT_CALLSTACK
    set_exc_stack_info(exc_info_save, task_context->s0);
#elif defined(__LITEOS__)
    set_exc_stack_info(exc_info_save, task_context->sp);
#endif
}

static errcode_t set_exc_info(uint32_t exc_type, const exc_context_t *exc_buf_addr, exc_info_save_t **exc_info_save)
{
    unused(exc_type);
    exc_info_save_t *exc_info = *exc_info_save;
#if defined(USE_CMSIS_OS) && defined(__LITEOS__)
    uint32_t phase;
    uint32_t thrd_pid;
    if (g_excInfo.nestCnt > 1) { /* Exception nesting level is 1 */
        phase = OS_EXC_STAGE_EXC;
        thrd_pid = OS_EXC_STAGE_INIT_VALUE;
    } else if (!OS_SCHEDULER_ACTIVE) {
        phase = OS_EXC_STAGE_INIT;
        thrd_pid = OS_EXC_STAGE_INIT_VALUE;
    } else if (OS_INT_ACTIVE != 0) {
        phase = OS_EXC_STAGE_IRQ;
        thrd_pid = OS_EXC_STAGE_INIT_VALUE;
    } else {
        phase = OS_EXC_STAGE_TASK;
        thrd_pid = LOS_CurTaskIDGet();
    }
    char *task_name = OsCurTaskNameGet();
    if (strcpy_s(exc_info->task_name, sizeof(exc_info->task_name), task_name) != EOK) {
        return ERRCODE_FAIL;
    }
    exc_info->thrd_pid = thrd_pid;
    exc_info->type = (uint16_t)exc_type;
    exc_info->nest_cnt = g_excInfo.nestCnt;
#ifndef LOSCFG_EXC_SIMPLE_INFO
    if (strcpy_s(exc_info->phase, sizeof(exc_info->phase), g_excOccurStage[phase])) {
        return ERRCODE_FAIL;
    }
#else
    exc_info->phase = (uint16_t)phase;
#endif

    exc_info->ccause = exc_buf_addr->ccause;
#elif defined(__FREERTOS__)
    exc_info->uw_exc_type = (uint16_t)exc_buf_addr->mcause;
#if CORE == WIFI
    exc_info->cxcptsc = read_custom_csr(CXCPTSC);
#endif
#endif
    exc_info->mcause = exc_buf_addr->mcause;
    exc_info->mtval = exc_buf_addr->mtval;
    exc_info->gp = exc_buf_addr->gp;
    set_exc_context_info((const task_context_t *)&exc_buf_addr->task_context, exc_info_save);
    return ERRCODE_SUCC;
}

#ifdef CFG_DRIVERS_MMC
errcode_t emmc_exc_info_write(void)
{
    char *exc_info_buffer = (char *)dfx_malloc(0, ONE_SECTOR_SIZE);
    uint32_t sector_num = mmc_direct_read(0, exc_info_buffer, START_SECTOR_NUM, 1);
    if (sector_num == 0) {
        return ERRCODE_FAIL;
    }
    if (strlen(exc_info_buffer) == 0) {
        return ERRCODE_SUCC;
    }
    int32_t ret = crash_data_write(g_exc_info_path, 0, (const uint8_t *)exc_info_buffer, ONE_SECTOR_SIZE);
    if (ret < 0) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
#endif
static errcode_t exc_info_save(uint32_t exc_type, const exc_context_t *exc_buf_addr)
{
    uint16_t stack_size = 0;
#ifdef SUPPORT_CALLSTACK
    get_stack_size(exc_buf_addr->task_context.s0, &stack_size);
#elif defined(__LITEOS__)
    get_stack_size(exc_buf_addr->task_context.sp, &stack_size);
#endif
    uint32_t exc_size = (uint32_t)EXC_INFO_SAVE_SIZE + (stack_size * EXC_STACK_INFO_SIZE);
    exc_info_save_t *exc_info_save = (exc_info_save_t *)dfx_malloc(0, exc_size);
    if (exc_info_save == NULL) {
        return ERRCODE_FAIL;
    }
    memset_s(exc_info_save, exc_size, 0, exc_size);
#ifdef SUPPORT_CALLSTACK
    exc_info_save->stack_size = stack_size;
#elif defined(__LITEOS__)
    exc_info_save->backtrace_size = stack_size;
#endif
    if (set_exc_info(exc_type, (const exc_context_t *)exc_buf_addr, &exc_info_save) != ERRCODE_SUCC) {
        dfx_free(0, exc_info_save);
        return ERRCODE_FAIL;
    }
    int32_t ret;
#if CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES
#ifdef CFG_DRIVERS_NANDFLASH
    nand_driver_init(SPEED_SLOW);
    set_trans_type(TRANS_BY_CPU_SINGLE_LINE);
    ret = exc_info_write(g_exc_info_path, 0, (const uint8_t *)exc_info_save, exc_size);
#endif
#ifdef CFG_DRIVERS_MMC
    char *exc_info_buffer = (char *)dfx_malloc(0, ONE_SECTOR_SIZE);
    memset_s(exc_info_buffer, ONE_SECTOR_SIZE, 0, ONE_SECTOR_SIZE);
    memcpy_s(exc_info_buffer, ONE_SECTOR_SIZE, exc_info_save, exc_size);
    ret = (int32_t)mmc_write_in_exception(0, exc_info_buffer, START_SECTOR_NUM, 1);
    dfx_free(0, exc_info_buffer);
#endif
#else
    if (dfx_flash_erase(FLASH_OP_TYPE_DUMP_INFO, 0, exc_size) != ERRCODE_SUCC) {
        dfx_free(0, exc_info_save);
        return ERRCODE_FAIL;
    }
    ret = dfx_flash_write(FLASH_OP_TYPE_DUMP_INFO, 0, (uint8_t *)exc_info_save, exc_size, 0);
#endif
    dfx_free(0, exc_info_save);
    if (ret <= 0) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
#endif

static void exc_fault_info_display(uint32_t exc_type, const exc_context_t *exc_buf_addr)
{
#if defined(USE_CMSIS_OS) && defined(__LITEOS__)
    OsExcInfoDisplay(exc_type, (const ExcContext *)(uintptr_t)exc_buf_addr);
#if !(MASTER_BY_BRANDY_ONLY)
    exec_fault_handler(exc_type, (const ExcContext *)(uintptr_t)exc_buf_addr);
#endif
#endif

#if defined SAVE_EXC_INFO
    exc_info_save(exc_type, (const exc_context_t *)exc_buf_addr);
#endif
#if SLAVE_BY_WS53_ONLY
    syserr_backtrace_print(exc_buf_addr);
#endif
    UNUSED(exc_type);
    UNUSED(exc_buf_addr);
}

#ifdef CONFIG_SUPPORT_CRASHINFO_SAVE_TO_FLASH
static const char *g_exc_occur_stage[] = {
    "Init",
    "Task",
    "Irq",
    "Exc"
};

uint32_t read_from_flash(uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size)
{
    return uapi_sfc_reg_read(flash_addr, read_buffer, read_size);
}

static inline bool crashinfo_flash_addr_get(uint32_t *flash_save_addr, uint32_t *flash_save_size)
{
    partition_information_t info = {0};

    if (uapi_partition_get_info(PARTITION_CRASH_INFO, &info) != ERRCODE_SUCC) {
        return false;
    }
    *flash_save_addr = info.part_info.addr_info.addr;
    *flash_save_size = info.part_info.addr_info.size;
    return true;
}

static void crashinfo_back_trace_save(uint32_t addr, uint32_t sp)
{
    uint32_t back_sp = sp;
    uint32_t stack_depth = 0;
    uint32_t count = 0;
    uint32_t ptr = addr;

    while (back_sp != 0) {
        if (is_valid_txt_addr(*((uint32_t *)(back_sp)))) {
            ptr += sizeof(uint32_t);
            uapi_sfc_reg_write(ptr, (uint8_t *)(uintptr_t)&back_sp, sizeof(uint32_t));
            ptr += sizeof(uint32_t);
            uapi_sfc_reg_write(ptr, (uint8_t *)(uintptr_t)back_sp, sizeof(uint32_t));
            if (++count == STACK_SAVE_DEPTH) {
                break;
            }
        }
        back_sp = back_sp + USER_STACK_OFFSET_PER;
        if (++stack_depth == STACK_DEPTH) {
            break;
        }
    }
    uapi_sfc_reg_write(addr, (uint8_t *)(uintptr_t)&count, sizeof(uint32_t));
}

static void crashinfo_back_trace_print(uint32_t addr)
{
    uint32_t count = 0;
    uint32_t max_trace_count = 0;
    uint32_t ptr = addr;
    uint32_t back_sp = 0;
    uint32_t sp_content = 0;
    uapi_sfc_reg_read(ptr, (uint8_t *)(uintptr_t)&count, sizeof(uint32_t));
    while (count-- > 0) {
        ptr += sizeof(uint32_t);
        uapi_sfc_reg_read(ptr, (uint8_t *)(uintptr_t)&back_sp, sizeof(uint32_t));
        ptr += sizeof(uint32_t);
        uapi_sfc_reg_read(ptr, (uint8_t *)(uintptr_t)&sp_content, sizeof(uint32_t));
        PRINT("sp addr= 0x%x    sp content= 0x%x\r\n", back_sp, sp_content);
        if (++max_trace_count == STACK_SAVE_DEPTH) {
            break;
        }
    }
}

static uint32_t crashinfo_taskinfo_get_size(const LosTaskCB *all_task_array)
{
    const LosTaskCB *task_cb = NULL;
    uint32_t loop;
    uint32_t save_size = sizeof(uint32_t);
    for (loop = 0; loop < g_taskMaxNum; ++loop) {
        task_cb = all_task_array + loop;
        if ((task_cb->taskStatus & OS_TASK_STATUS_UNUSED) != 0) {
            continue;
        }
        save_size += sizeof(uint32_t);
        save_size += sizeof(uint32_t);
        save_size += strlen(task_cb->taskName) + 1;
    }
    return save_size;
}

static uint32_t crashinfo_taskinfo_get_size_from_flash(uint32_t flash_save_offset)
{
    uint32_t loop;
    uint32_t task_name_len = 0;
    uint32_t save_size = sizeof(uint32_t);
    uint32_t cnt = 0;
    uapi_sfc_reg_read(flash_save_offset, (uint8_t *)(uintptr_t)&cnt, sizeof(uint32_t));
    for (loop = 0; loop < cnt; ++loop) {
        save_size += sizeof(uint32_t);
        uapi_sfc_reg_read(flash_save_offset + save_size, (uint8_t *)(uintptr_t)&task_name_len, sizeof(uint32_t));
        save_size += sizeof(uint32_t);
        save_size += task_name_len + 1;
    }
    return save_size;
}

static void crashinfo_taskinfo_save(uint32_t flash_save_offset, const LosTaskCB *all_task_array)
{
    const LosTaskCB *task_cb = NULL;
    uint32_t loop;
    uint32_t semid = 0;
    uint32_t save_offset = flash_save_offset + sizeof(uint32_t);
    uint32_t task_name_size[g_taskMaxNum];
    uint32_t cnt = 0;
    memset_s(task_name_size, sizeof(uint32_t) * g_taskMaxNum, 0, sizeof(uint32_t) * g_taskMaxNum);
    for (loop = 0; loop < g_taskMaxNum; ++loop) {
        task_cb = all_task_array + loop;
        if ((task_cb->taskStatus & OS_TASK_STATUS_UNUSED) != 0) {
            continue;
        }
        semid = (task_cb->taskSem != NULL) ? ((LosSemCB *)task_cb->taskSem)->semId : INVALID_SEM_ID;
        uapi_sfc_reg_write(save_offset, (uint8_t *)(uintptr_t)&semid, sizeof(uint32_t));
        save_offset += sizeof(uint32_t);
        task_name_size[task_cb->taskId] = strlen(task_cb->taskName);
        uapi_sfc_reg_write(save_offset, (uint8_t *)(uintptr_t)&(task_name_size[task_cb->taskId]), sizeof(uint32_t));
        save_offset += sizeof(uint32_t);
        uapi_sfc_reg_write(save_offset, (uint8_t *)(uintptr_t)(task_cb->taskName), strlen(task_cb->taskName) + 1);
        save_offset += strlen(task_cb->taskName) + 1;
        cnt++;
    }
    uapi_sfc_reg_write(flash_save_offset, (uint8_t *)(uintptr_t)&cnt, sizeof(uint32_t));
}

static errcode_t crashinfo_size_check(uint32_t flash_save_size, uint32_t *crashinfo_group_size, uint32_t *left_size)
{
    uint32_t waterline_addr = 0;
    uint32_t waterline_len = 0;
    uint32_t cbarray_addr = 0;
    uint32_t cbarray_len = 0;
    char *task_name = OsCurTaskNameGetExt();
    OsTaskWaterLineArrayGet(&waterline_addr, &waterline_len);
    OsTaskCBArrayGet(&cbarray_addr, &cbarray_len);
    uint32_t task_info_size = crashinfo_taskinfo_get_size((LosTaskCB *)cbarray_addr);
    uint32_t need_space = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) * CRASHINFO_GROUP_CNT +
        sizeof(uint32_t) + strlen(task_name) + 1 + sizeof(g_excInfo) + sizeof(exc_context_t) + STACK_SAVE_SIZE +
        waterline_len + task_info_size + sizeof(uint32_t) + cbarray_len;
    *left_size = flash_save_size - (sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) * CRASHINFO_GROUP_CNT +
        sizeof(uint32_t) + strlen(task_name) + 1 + sizeof(g_excInfo) + sizeof(exc_context_t) + STACK_SAVE_SIZE +
        waterline_len + task_info_size + sizeof(uint32_t));
    if (need_space > flash_save_size) {
        PRINT("No Space to Save the whole Crash Info\r\n");
        return ERRCODE_FAIL;
    }
    uint32_t crashinfo_group_index = 0;
    crashinfo_group_size[crashinfo_group_index++] = strlen(task_name) + 1;
    crashinfo_group_size[crashinfo_group_index++] = sizeof(g_excInfo);
    crashinfo_group_size[crashinfo_group_index++] = sizeof(exc_context_t);
    crashinfo_group_size[crashinfo_group_index++] = STACK_SAVE_SIZE;
    crashinfo_group_size[crashinfo_group_index++] = waterline_len;
    crashinfo_group_size[crashinfo_group_index++] = task_info_size;
    crashinfo_group_size[crashinfo_group_index++] = cbarray_len;
    crashinfo_group_size[crashinfo_group_index++] = sizeof(LosTaskCB);
    return ERRCODE_SUCC;
}

void crashinfo_save(exc_context_t *exc_buf_addr)
{
    uint32_t flash_save_addr = 0, flash_save_size = 0, flash_save_offset = 0, left_size = 0;
    uint32_t crashinfo_group_size[CRASHINFO_GROUP_CNT] = {0};
    uint32_t crashinfo_group_cnt = CRASHINFO_GROUP_CNT;
    bool ret = crashinfo_flash_addr_get(&flash_save_addr, &flash_save_size);
    if (!ret) {
        PRINT("No Space to Save Crash Info\r\n");
        return;
    }
    char *task_name = OsCurTaskNameGetExt();
    uint32_t waterline_addr = 0, waterline_len = 0, cbarray_addr = 0, cbarray_len = 0;
    uint32_t task_name_len = strlen(task_name);

    OsTaskWaterLineArrayGet(&waterline_addr, &waterline_len);
    OsTaskCBArrayGet(&cbarray_addr, &cbarray_len);
    if (crashinfo_size_check(flash_save_size, crashinfo_group_size, &left_size) != ERRCODE_SUCC) {
        cbarray_len = left_size / sizeof(LosTaskCB) * sizeof(LosTaskCB);
    }
    uint32_t task_num = cbarray_len / sizeof(LosTaskCB);
    uint32_t task_info_size = crashinfo_taskinfo_get_size((LosTaskCB *)cbarray_addr);

    flash_save_offset = flash_save_addr;
    if (uapi_sfc_reg_erase(flash_save_offset, flash_save_size) != ERRCODE_SUCC) {
        PRINT("Erase Flash Failed\r\n");
        return;
    }
    flash_save_offset += sizeof(uint32_t);
    uapi_sfc_reg_write(flash_save_offset, (uint8_t *)(uintptr_t)&crashinfo_group_cnt, sizeof(uint32_t));
    flash_save_offset += sizeof(uint32_t);
    uapi_sfc_reg_write(flash_save_offset, (uint8_t *)(uintptr_t)&crashinfo_group_size, sizeof(uint32_t) *
        CRASHINFO_GROUP_CNT);
    flash_save_offset += sizeof(uint32_t) * CRASHINFO_GROUP_CNT;
    uapi_sfc_reg_write(flash_save_offset, (uint8_t *)(uintptr_t)&task_name_len, sizeof(uint32_t));
    flash_save_offset += sizeof(uint32_t);
    uapi_sfc_reg_write(flash_save_offset, (uint8_t *)(uintptr_t)task_name, task_name_len + 1);
    flash_save_offset += task_name_len + 1;
    uapi_sfc_reg_write(flash_save_offset, (uint8_t *)(uintptr_t)&g_excInfo, sizeof(g_excInfo));
    flash_save_offset += sizeof(g_excInfo);

    uapi_sfc_reg_write(flash_save_offset, (uint8_t *)(uintptr_t)exc_buf_addr, sizeof(exc_context_t));
    flash_save_offset += sizeof(exc_context_t);
    uint32_t backtrace_sp = ((ExcContext *)exc_buf_addr)->taskContext.sp;
    crashinfo_back_trace_save(flash_save_offset, backtrace_sp);
    flash_save_offset += STACK_SAVE_SIZE;
    uapi_sfc_reg_write(flash_save_offset, (uint8_t *)(uintptr_t)&task_num, sizeof(uint32_t));
    flash_save_offset += sizeof(uint32_t);
    uapi_sfc_reg_write(flash_save_offset, (uint8_t *)(uintptr_t)waterline_addr, waterline_len);
    flash_save_offset += waterline_len;
    crashinfo_taskinfo_save(flash_save_offset, (const LosTaskCB *)cbarray_addr);
    flash_save_offset += task_info_size;
    uapi_sfc_reg_write(flash_save_offset, (uint8_t *)(uintptr_t)cbarray_addr, cbarray_len);
    uapi_sfc_reg_write(flash_save_addr, (uint8_t *)(uintptr_t)&g_crashinfo_flag, sizeof(uint32_t));
}

bool crashinfo_status_get(void)
{
    uint32_t flash_save_addr = 0;
    uint32_t flash_save_size = 0;
    bool ret = crashinfo_flash_addr_get(&flash_save_addr, &flash_save_size);
    if (!ret) {
        PRINT("No Space to Save Crash Info\r\n");
        return false;
    }
    uint32_t save_flag = 0;
    uapi_sfc_reg_read(flash_save_addr, (uint8_t *)(uintptr_t)&save_flag, sizeof(uint32_t));
    if (save_flag == g_crashinfo_flag) {
        return true;
    }
    return false;
}

void crashinfo_clr(void)
{
    uint32_t flash_save_addr = 0, flash_save_size = 0;
    bool ret = crashinfo_flash_addr_get(&flash_save_addr, &flash_save_size);
    if (!ret) {
        PRINT("No Space to Save Crash Info\r\n");
        return;
    }
    uint32_t save_flag = 0;
    uapi_sfc_reg_write(flash_save_addr, (uint8_t *)(uintptr_t)&save_flag, sizeof(uint32_t));
}

void crashinfo_dump(void)
{
    uint32_t flash_save_addr = 0, flash_save_size = 0;
    bool ret = crashinfo_flash_addr_get(&flash_save_addr, &flash_save_size);
    if (!ret) {
        PRINT("No Space to Save Crash Info\r\n");
        return;
    }
    uint32_t save_flag = 0;
    uint32_t flash_save_offset = flash_save_addr;
    uapi_sfc_reg_read(flash_save_offset, (uint8_t *)(uintptr_t)&save_flag, sizeof(uint32_t));
    if (save_flag != g_crashinfo_flag) {
        PRINT("Crash info nil\r\n");
        return;
    }

    uint32_t waterline_addr = 0, waterline_len = 0, cbarray_addr = 0, cbarray_len = 0, task_name_len = 0;
    char task_name[TASK_NAME_MAX] = {0};

    OsTaskWaterLineArrayGet(&waterline_addr, &waterline_len);
    OsTaskCBArrayGet(&cbarray_addr, &cbarray_len);
    PRINT("--------------Last Crash info dump--------------\r\n");
    flash_save_offset += sizeof(uint32_t);
    flash_save_offset += sizeof(uint32_t);
    flash_save_offset += sizeof(uint32_t) * CRASHINFO_GROUP_CNT;

    uapi_sfc_reg_read(flash_save_offset, (uint8_t *)(uintptr_t)&task_name_len, sizeof(uint32_t));
    flash_save_offset += sizeof(uint32_t);
    uapi_sfc_reg_read(flash_save_offset, (uint8_t *)(uintptr_t)task_name, task_name_len + 1);
    flash_save_offset += task_name_len + 1;

    ExcInfo exc_info = {0};
    uapi_sfc_reg_read(flash_save_offset, (uint8_t *)(uintptr_t)&exc_info, sizeof(ExcInfo));
    PRINT("task:%s\n""thrdPid:0x%x\n""type:0x%x\n""nestCnt:%u\n""phase:%s\n",
        task_name, exc_info.thrdPid, exc_info.type, exc_info.nestCnt, g_exc_occur_stage[exc_info.phase]);
    flash_save_offset += sizeof(ExcInfo);

    exc_context_t exc_context_buffer = {0};
    uapi_sfc_reg_read(flash_save_offset, (uint8_t *)(uintptr_t)&exc_context_buffer, sizeof(exc_context_t));
    exc_info.context = (ExcContext *)(&exc_context_buffer);
    OsExcInfoDisplayContextExt(&exc_info);
    flash_save_offset += sizeof(exc_context_t);
    crashinfo_back_trace_print(flash_save_offset);

    flash_save_offset += STACK_SAVE_SIZE;
    uint32_t task_num = 0;
    uapi_sfc_reg_read(flash_save_offset, (uint8_t *)(uintptr_t)&task_num, sizeof(uint32_t));
    flash_save_offset += sizeof(uint32_t);
    waterline_addr = flash_save_offset + BASE_ADDR;
    uint32_t taskinfo_size = crashinfo_taskinfo_get_size_from_flash(flash_save_offset + waterline_len);
    cbarray_addr = waterline_addr + waterline_len + taskinfo_size;
    crashinfo_taskinfo_title_print();
    crashinfo_taskinfo_print((LosTaskCB *)cbarray_addr, (UINT32 *)waterline_addr, flash_save_offset + waterline_len,
        task_num);
    PRINT("--------------Last Crash info dump end--------------\r\n");
}
#endif

void do_hard_fault_handler(exc_context_t *exc_buf_addr)
{
    if (exc_buf_addr == NULL) { return; }
#ifdef SUPPORT_CPU_TRACE
    cpu_trace_disable();
#endif
    uint32_t exc_type = exc_buf_addr->mcause;
    PRINT("exception:%x\r\n", exc_type);
    if (exc_type == NMI_INTERRUPT) {
        PRINT("Oops:NMI\n");
#ifdef WDT_INVOKE_USER_CALLBACK
        if (watchdog_port_check_nmi_intr(non_os_get_nmi_raw_status()) == ERRCODE_SUCC) {
            irq_wdt_handler();
        }
#endif
    }
    reboot_cause_t reset_cause = 0;
    reset_cause = hal_set_reset_cause(exc_type);
    /* must set magic before memory dump */
    cpu_utils_set_system_status_by_cause(reset_cause);
    hal_save_reg_info(exc_buf_addr);
    deal_with_exception(exc_buf_addr, exc_type, reset_cause);
}

static void deal_with_exception(exc_context_t *exc_buf_addr, uint32_t exc_type, reboot_cause_t reset_cause)
{
    UNUSED(reset_cause);

#if defined(BUILD_APPLICATION_STANDARD) && (CORE == MASTER_BY_ALL)
    set_exception_time_stamp();
    // Record the reason before wait, so apps can get the reason.
    set_cpu_utils_reset_cause(reset_cause);
#endif
    exc_fault_info_display(exc_type, exc_buf_addr);
    if (g_exception_dump_callback != NULL) {
        g_exception_dump_callback(exc_type, exc_buf_addr);
    }
#ifdef CONFIG_SUPPORT_CRASHINFO_SAVE_TO_FLASH
    crashinfo_save(exc_buf_addr);
#endif

#ifdef CONFIG_MIDDLEWARE_SUPPORT_EXCEPT_REBOOT
    cpu_utils_reset_chip_with_log((cores_t)MASTER_BY_ALL, reset_cause);
#elif defined CONFIG_MEMORY_CUSTOMIZE_RSV
    reboot_mem_save_to_flash();
#endif
#ifdef CONFIG_MIDDLEWARE_SUPPORT_EXCEPT_DISABLE_WDT
    uapi_watchdog_disable();
#else
    uapi_watchdog_enable(WDT_MODE_RESET);
#endif
#ifdef CONFIG_MIDDLEWARE_SUPPORT_EXCEPT_WAITFOREVER
    while (1) {}
#endif
}

#if defined(__FREERTOS__)
typedef struct {
    uint32_t ra;      /* X1 */

    uint32_t t0;      /* X5 */
    uint32_t t1;     /* X6 */
    uint32_t t2;     /* X7 */
    uint32_t s0;      /* X8 */
    uint32_t s1;      /* X9 */
    uint32_t a0;      /* X10 */
    uint32_t a1;      /* X11 */
    uint32_t a2;      /* X12 */
    uint32_t a3;      /* X13 */
    uint32_t a4;      /* X14 */
    uint32_t a5;      /* X15 */
    uint32_t a6;      /* X16 */
    uint32_t a7;      /* X17 */
    uint32_t s2;      /* X18 */
    uint32_t s3;      /* X19 */
    uint32_t s4;      /* X20 */
    uint32_t s5;      /* X21 */
    uint32_t s6;      /* X22 */
    uint32_t s7;      /* X23 */
    uint32_t s8;      /* X24 */
    uint32_t s9;      /* X25 */
    uint32_t s10;      /* X26 */
    uint32_t s11;      /* X27 */
    uint32_t t3;      /* X28 */
    uint32_t t4;      /* X29 */
    uint32_t t5;      /* X30 */
    uint32_t t6;      /* X31 */
    uint32_t mstatus;  /* mstatus */
} exc_context_freertos_t;

static void exception_get_exc_reg(exc_context_t *exc_buff_addr, exc_context_freertos_t *exc_reg)
{
    exc_buff_addr->task_context.ra = exc_reg->ra;
    exc_buff_addr->task_context.t0 = exc_reg->t0;
    exc_buff_addr->task_context.t1 = exc_reg->t1;
    exc_buff_addr->task_context.t2 = exc_reg->t2;
    exc_buff_addr->task_context.s0 = exc_reg->s0;
    exc_buff_addr->task_context.s1 = exc_reg->s1;
    exc_buff_addr->task_context.a0 = exc_reg->a0;
    exc_buff_addr->task_context.a1 = exc_reg->a1;
    exc_buff_addr->task_context.a2 = exc_reg->a2;
    exc_buff_addr->task_context.a3 = exc_reg->a3;
    exc_buff_addr->task_context.a4 = exc_reg->a4;
    exc_buff_addr->task_context.a5 = exc_reg->a5;
    exc_buff_addr->task_context.a6 = exc_reg->a6;
    exc_buff_addr->task_context.a7 = exc_reg->a7;
    exc_buff_addr->task_context.s2 = exc_reg->s2;
    exc_buff_addr->task_context.s3 = exc_reg->s3;
    exc_buff_addr->task_context.s4 = exc_reg->s4;
    exc_buff_addr->task_context.s5 = exc_reg->s5;
    exc_buff_addr->task_context.s6 = exc_reg->s6;
    exc_buff_addr->task_context.s7 = exc_reg->s7;
    exc_buff_addr->task_context.s8 = exc_reg->s8;
    exc_buff_addr->task_context.s9 = exc_reg->s9;
    exc_buff_addr->task_context.s10 = exc_reg->s10;
    exc_buff_addr->task_context.s11 = exc_reg->s11;
    exc_buff_addr->task_context.t3 = exc_reg->t3;
    exc_buff_addr->task_context.t4 = exc_reg->t4;
    exc_buff_addr->task_context.t5 = exc_reg->t5;
    exc_buff_addr->task_context.t6 = exc_reg->t6;
    exc_buff_addr->task_context.mstatus = exc_reg->mstatus;
    exc_buff_addr->task_context.sp = (uint32_t)exc_reg;
}

static exc_context_t g_exc_buff_addr;
void do_fault_handler_freertos(void)
{
    exc_context_freertos_t *exc_reg = (exc_context_freertos_t *)(read_csr(mscratch) + CONTEXT_OFFSET);
    exception_get_exc_reg(&g_exc_buff_addr, exc_reg);
    g_exc_buff_addr.ccause = (uint8_t)read_csr(0xFC2);
    g_exc_buff_addr.mcause = read_csr(mcause);
    g_exc_buff_addr.mtval = read_csr(mtval);
    g_exc_buff_addr.task_context.mepc = read_csr(mepc);
    do_hard_fault_handler(&g_exc_buff_addr);
}
#endif

#if defined(__LITEOS__)
void do_fault_handler(uint32_t exc_type, exc_context_t *exc_buff_addr)
{
    UNUSED(exc_type);
    do_hard_fault_handler(exc_buff_addr);
}
#endif

void hal_register_exception_dump_callback(hal_exception_dump_callback callback)
{
    if (callback != NULL) {
        g_exception_dump_callback = callback;
    }
}
