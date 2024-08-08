/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description:   LOG OML EXCEPTION MODULE
 */

#ifdef SUPPORT_DFX_EXCEPTION
#include "exception.h"
#endif
#include "debug_print.h"
#include "securec.h"
#include "log_def.h"
#include "log_common.h"
#include "log_reg_dump.h"
#include "tcxo.h"
#include "non_os.h"
#include "log_oml_exception.h"
#ifdef USE_CMSIS_OS
#ifdef __LITEOS__
#include "los_task_pri.h"
#endif
#endif
#include "watchdog.h"
#include "watchdog_porting.h"
#if SLAVE_BY_WS53_ONLY
#include "preserve.h"
#endif
#if CORE != CORE_LOGGING
#include "log_buffer.h"
#endif
#if defined(CHIP_CAT1) && (CHIP_CAT1 == 1) && (CORE != CORE_LOGGING)
#else
#include "uart.h"
#endif
#ifdef SDT_LOG_BY_UART
#include "sdt_by_uart_external.h"
#endif
#if MCU_ONLY
#include "non_os_reboot.h"
#include "preserve.h"
#endif

#if CORE == CORE_LOGGING
#ifdef HSO_SUPPORT
#include "last_dump.h"
#include "last_dump_adapt.h"
#endif
#endif

#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
#include "dfx_feature_config.h"
#endif
#define STACK_SIZE 3
#define ONLY_ACORE_NUM 1

#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
#define GENERAL_REG_NUM 32
#elif((ARCH == CM3) || (ARCH == CM7))
#define GENERAL_REG_NUM 13
#endif

#define DUMP_DELAY 6000ULL
#define LOG_DELAY 2000ULL
#if defined(__GNUC__)
extern uint32_t g_stack_system;
#endif

#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
static void log_oml_get_reg_value(uint32_t *general_reg, const exc_context_t *exc_buf_addr)
{
    general_reg[REG_NUM_0] = 0x0;
    general_reg[REG_NUM_1] = exc_buf_addr->task_context.ra;
    general_reg[REG_NUM_2] = exc_buf_addr->task_context.sp;
    general_reg[REG_NUM_3] = exc_buf_addr->gp;
    general_reg[REG_NUM_4] = exc_buf_addr->task_context.tp;
    general_reg[REG_NUM_5] = exc_buf_addr->task_context.t0;
    general_reg[REG_NUM_6] = exc_buf_addr->task_context.t1;
    general_reg[REG_NUM_7] = exc_buf_addr->task_context.t2;
    general_reg[REG_NUM_8] = exc_buf_addr->task_context.s0;
    general_reg[REG_NUM_9] = exc_buf_addr->task_context.s1;
    general_reg[REG_NUM_10] = exc_buf_addr->task_context.a0;
    general_reg[REG_NUM_11] = exc_buf_addr->task_context.a1;
    general_reg[REG_NUM_12] = exc_buf_addr->task_context.a2;
    general_reg[REG_NUM_13] = exc_buf_addr->task_context.a3;
    general_reg[REG_NUM_14] = exc_buf_addr->task_context.a4;
    general_reg[REG_NUM_15] = exc_buf_addr->task_context.a5;
    general_reg[REG_NUM_16] = exc_buf_addr->task_context.a6;
    general_reg[REG_NUM_17] = exc_buf_addr->task_context.a7;
    general_reg[REG_NUM_18] = exc_buf_addr->task_context.s2;
    general_reg[REG_NUM_19] = exc_buf_addr->task_context.s3;
    general_reg[REG_NUM_20] = exc_buf_addr->task_context.s4;
    general_reg[REG_NUM_21] = exc_buf_addr->task_context.s5;
    general_reg[REG_NUM_22] = exc_buf_addr->task_context.s6;
    general_reg[REG_NUM_23] = exc_buf_addr->task_context.s7;
    general_reg[REG_NUM_24] = exc_buf_addr->task_context.s8;
    general_reg[REG_NUM_25] = exc_buf_addr->task_context.s9;
    general_reg[REG_NUM_26] = exc_buf_addr->task_context.s10;
    general_reg[REG_NUM_27] = exc_buf_addr->task_context.s11;
    general_reg[REG_NUM_28] = exc_buf_addr->task_context.t3;
    general_reg[REG_NUM_29] = exc_buf_addr->task_context.t4;
    general_reg[REG_NUM_30] = exc_buf_addr->task_context.t5;
    general_reg[REG_NUM_31] = exc_buf_addr->task_context.t6;
}
#endif

static void log_oml_hard_fault(uint32_t reason)
{
#if (ARCH == CM3) || (ARCH == CM7)
    if (reason & SCB_HFSR_DEBUGEVT_Msk) {
        PRINT("[Hard Fault]: Caused by Debug Event" NEWLINE);
    }
    if (reason & SCB_HFSR_FORCED_Msk) {
        PRINT("[Hard Fault]: Caused by Other Faults" NEWLINE);
    }
    if (reason & SCB_HFSR_VECTTBL_Msk) {
        PRINT("[Hard Fault]: Caused by Fetching vector" NEWLINE);
    }
#else
    UNUSED(reason);
#endif
}

static void log_oml_mem_fault(uint32_t reason)
{
#if (ARCH == CM3) || (ARCH == CM7)
    if (reason & SCB_CFSR_MSTKERR_Msk) {
        PRINT("[Mem Fault] Enter Stack Fault" NEWLINE);
    } else if (reason & SCB_CFSR_MUNSTKERR_Msk) {
        PRINT("[Mem Fault] Quit Stack Fault" NEWLINE);
    } else if (reason & SCB_CFSR_DACCVIOL_Msk) {
        PRINT("[Mem Fault] Data Access Fault" NEWLINE);
    } else if (reason & SCB_CFSR_IACCVIOL_Msk) {
        PRINT("[Mem Fault] Instruction Access Fault" NEWLINE);
    }
#else
    UNUSED(reason);
#endif
}

static void log_oml_bus_fault(uint32_t reason)
{
#if (ARCH == CM3) || (ARCH == CM7)
    if (reason & SCB_CFSR_STKERR_Msk) {
        PRINT("[Bus Fault] Enter Stack Fault" NEWLINE);
    }
    if (reason & SCB_CFSR_UNSTKERR_Msk) {
        PRINT("[Bus Fault] Quit Stack Fault" NEWLINE);
    }
    if (reason & SCB_CFSR_IMPRECISERR_Msk) {
        PRINT("[Bus Fault] Non Exact Data Access Fault" NEWLINE);
    }
    if (reason & SCB_CFSR_PRECISERR_Msk) {
        PRINT("[Bus Fault] Data Access Fault" NEWLINE);
    }
    if (reason & SCB_CFSR_IBUSERR_Msk) {
        PRINT("[Bus Fault] Instruction Access Fault" NEWLINE);
    }
#else
    UNUSED(reason);
#endif
}

static void log_oml_usage_fault(uint32_t reason)
{
#if (ARCH == CM3) || (ARCH == CM7)
    if (reason & SCB_CFSR_DIVBYZERO_Msk) {
        PRINT("[Usage Fault] DIV Zero Fault" NEWLINE);
    }
    if (reason & SCB_CFSR_UNALIGNED_Msk) {
        PRINT("[Usage Fault] unaligned access Fault" NEWLINE);
    }
    if (reason & SCB_CFSR_NOCP_Msk) {
        PRINT("[Usage Fault] Try to execute co-processor instr Fault" NEWLINE);
    }
    if (reason & SCB_CFSR_INVPC_Msk) {
        PRINT("[Usage Fault] Invalid EXC_RETURN to PC Fault" NEWLINE);
    }
    if (reason & SCB_CFSR_INVSTATE_Msk) {
        PRINT("[Usage Fault] Try to Enter ARM State Fault" NEWLINE);
    }
    if (reason & SCB_CFSR_UNDEFINSTR_Msk) {
        PRINT("[Usage Fault] Undefined instruction Fault" NEWLINE);
    }
#else
    UNUSED(reason);
#endif
}

#if SLAVE_BY_WS53_ONLY
static void log_exception_print_para(const exc_context_t *e_contex)
{
    task_context_t contex = e_contex->task_context;
    PRINT("mcause:0x%x\n""ccause:0x%x\n""mstatus:0x%x\n""ra:0x%x\n""sp:0x%x\n""gp:0x%x\n",
        e_contex->mcause, e_contex->ccause, contex.mstatus, contex.ra, contex.sp, e_contex->gp);

    PRINT("a0:0x%x\n""a1:0x%x\n""a2:0x%x\n""a3:0x%x\n""a4:0x%x\n""a5:0x%x\n""a6:0x%x\n""a7:0x%x\n"
              "tp:0x%x\n""t0:0x%x\n""t1:0x%x\n""t2:0x%x\n""t3:0x%x\n""t4:0x%x\n""t5:0x%x\n""t6:0x%x\n",
        contex.a0, contex.a1, contex.a2, contex.a3, contex.a4, contex.a5, contex.a6, contex.a7,
        contex.tp, contex.t0, contex.t1, contex.t2, contex.t3, contex.t4, contex.t5, contex.t6);

    PRINT("s0/fp:0x%x\n""s1:0x%x\n""s2:0x%x\n""s3:0x%x\n""s4:0x%x\n""s5:0x%x\n"
              "s6:0x%x\n""s7:0x%x\n""s8:0x%x\n""s9:0x%x\n""s10:0x%x\n""s11:0x%x\n",
        contex.s0, contex.s1, contex.s2, contex.s3, contex.s4, contex.s5,
        contex.s6, contex.s7, contex.s8, contex.s9, contex.s10, contex.s11);
}
#endif

#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
#if defined(CONFIG_DFX_SDT_DUMP_EXCEPTION) && (CONFIG_DFX_SDT_DUMP_EXCEPTION == DFX_YES)
static uint32_t log_get_fault_type(uint32_t irq_id)
{
    switch (irq_id) {
        case IRQ_ID_INSTRUCTION_ADDRESS_MISALIGNED:
            return OM_INSTRUCTION_ADDRESS_MISALIGNED;
        case IRQ_ID_INSTRUCTION_ACCESS_FAULT:
            return OM_INSTRUCTION_ACCESS_FAULT;
        case IRQ_ID_ILLEGAL_INSTRUCTION:
            return OM_ILLEGAL_INSTRUCTION;
        case IRQ_ID_BREAKPOINT:
            return OM_BREAKPOINT;
        case IRQ_ID_LOAD_ADDERSS_MISALIGNED:
            return OM_LOAD_ADDERSS_MISALIGNED;
        case IRQ_ID_LOAD_ACCESS_FAULT:
            return OM_LOAD_ACCESS_FAULT;
        case IRQ_ID_STORE_OR_AMO_ADDRESS_MISALIGNED:
            return OM_STORE_OR_AMO_ADDRESS_MISALIGNED;
        case IRQ_ID_STORE_OR_AMO_ACCESS_FALUT:
            return OM_STORE_OR_AMO_ACCESS_FALUT;
        case IRQ_ID_ENVIRONMENT_CALL_FROM_UMODE:
            return OM_ENVIRONMENT_CALL_FROM_UMODE;
        case IRQ_ID_ENVIRONMENT_CALL_FROM_SMODE:
            return OM_ENVIRONMENT_CALL_FROM_SMODE;
        case IRQ_ID_ENVIRONMENT_CALL_FROM_MMODE:
            return OM_ENVIRONMENT_CALL_FROM_MMODE;
        case IRQ_ID_INSTRUCTION_PAGE_FAULT:
            return OM_INSTRUCTION_PAGE_FAULT;
        case IRQ_ID_LOAD_PAGE_FAULT:
            return OM_LOAD_PAGE_FAULT;
        case IRQ_ID_STORE_OR_AMO_PAGE_FAULT:
            return OM_STORE_OR_AMO_PAGE_FAULT;
        case IRQ_ID_NMI_INTERRUPT:
            return OM_NMI_INTERRUPT;
#if (ARCH == RISCV31) || (ARCH == RISCV32)
        case IRQ_ID_HARD_FAULT:
            return OM_RISCV_HARD_FAULT;
        case IRQ_ID_LOCK_UP:
            return OM_LOCK_UP;
#else
        case IRQ_ID_ASYNCHRONOUS_EXCEPTION:
            return OM_ASYNCHRONOUS_EXCEPTION;
#endif
        default:
            PRINT("Unknown Fault[%x]" NEWLINE, irq_id);
            return 0;
    }
}
#endif

void log_exception_dump(uint32_t irq_id, exc_context_t *exc_buf_addr)
{
    UNUSED(log_oml_usage_fault);
    UNUSED(log_oml_bus_fault);
    UNUSED(log_oml_mem_fault);
    UNUSED(log_oml_hard_fault);
    UNUSED(irq_id);
    UNUSED(exc_buf_addr);
#if defined(CONFIG_DFX_SDT_DUMP_EXCEPTION) && (CONFIG_DFX_SDT_DUMP_EXCEPTION == DFX_YES)

    uint32_t fault_type = 0;
#if defined(HSO_SUPPORT)
    fault_type = log_get_fault_type(irq_id);
#if MCU_ONLY
    reboot_cause_t reset_cause = get_cpu_utils_reset_cause();
    switch (reset_cause) {
        case REBOOT_CAUSE_BT_WATCHDOG:
        case REBOOT_CAUSE_APPLICATION_CHIP_WDT:
            fault_type = OM_WDT_TIMEOUT_INTERRUPT;
            break;
        case REBOOT_CAUSE_APPLICATION_XIP_CTRL:
            fault_type = OM_APPLICATION_XIP_CTRL_INTERRUPT;
            break;
        case REBOOT_CAUSE_APPLICATION_XIP_CACHE:
            fault_type = OM_APPLICATION_XIP_CACHE_INTERRUPT;
            break;
        case REBOOT_CAUSE_APPLICATION_MDMA:
            fault_type = OM_APPLICATION_MDMA_INTERRUPT;
            break;
        case REBOOT_CAUSE_APPLICATION_SMDMA:
            fault_type = OM_APPLICATION_SMDMA_INTERRUPT;
            break;
        default:
            break;
    }
#endif
#else
    UNUSED(irq_id);
    UNUSED(log_get_fault_type);
    fault_type = OM_WDT_TIMEOUT;
#endif
#if SLAVE_BY_WS53_ONLY
    log_exception_print_para(exc_buf_addr);
    fault_type = log_get_fault_type(irq_id);
#endif
    log_oml_exception_info_send(fault_type, exc_buf_addr);
#endif
    /* Send ram */
    log_oml_memory_dump();
}
#else
void log_exception_dump(uint32_t int_id, uint32_t reason, uint32_t addr, exc_info_t *exc_info)
{
    uint32_t fault_type = 0;
    switch (int_id) {
        case INT_ID_HARD_FAULT:
            fault_type = OM_HARD_FAULT;
            log_oml_hard_fault(reason);
            break;
        case INT_ID_MEM_FAULT:
            fault_type = OM_MEM_FAULT;
            log_oml_mem_fault(reason);
            break;
        case INT_ID_BUS_FAULT:
            fault_type = OM_BUS_FAULT;
            log_oml_bus_fault(reason);
            break;
        case INT_ID_USAGE_FAULT:
            fault_type = OM_USAGE_FAULT;
            log_oml_usage_fault(reason);
            break;
#if CORE == MASTER_BY_ALL
        case INT_ID_CHIP_WATCHDOG_FAULT:
#endif
        case INT_ID_WATCHDOG_FAULT:
            fault_type = OM_WDT_TIMEOUT;
            break;
        default:
            PRINT("Unknown Fault[%d]" NEWLINE, int_id);
            break;
    }
#if MCU_ONLY
    if ((get_cpu_utils_reset_cause() == REBOOT_CAUSE_BT_WATCHDOG) ||
        (get_cpu_utils_reset_cause() == REBOOT_CAUSE_APPLICATION_CHIP_WDT)) {
        fault_type = OM_WDT_TIMEOUT;
    }
#endif
    log_oml_exception_info_send(addr, fault_type, reason, exc_info);

    /* Send ram */
    log_oml_memory_dump();
}
#endif

void log_exception_send_data(const uint8_t *data, uint16_t length)
{
#if CORE == CORE_LOGGING
    uapi_uart_write(LOG_UART_BUS, (const void *)data, length, 0);
#elif defined(SDT_LOG_BY_UART)
    oml_write_uart_fifo((uint8*)data, length, LOGUART_BASE);
#else
    /* APP core need to wait enough share memory to write next block of data */
    uint32_t available = 0;
    while (available <= length) {
        (void)log_buffer_get_available_for_next_message(&available);
    }

    log_event(data, length);
#endif
}

static void log_exception_dump_memory(uint32_t addr, uint32_t length)
{
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
    uint32_t ram_addr = addr;
    uint32_t ram_size = length;
    uint32_t length_tmp = length;
    uint8_t msg_tail = OM_FRAME_DELIMITER;
    om_msg_header_stru_t msg_header = { 0 };
    om_msg_dump_header_stru_t dump_header = { 0 };

    msg_header.frame_start = OM_FRAME_DELIMITER;
    msg_header.func_type = OM_MSG_TYPE_LAST;
    msg_header.prime_id = OM_LOG_SAVE_STACK;

    dump_header.end_flag = 0;
    dump_header.count = 0;

    non_os_enter_critical();
    while (ram_size > 0) {
        length_tmp = MIN(ram_size, DUMP_MAX_LENGTH_PER_TRANS);
        if (length_tmp == ram_size) {
            dump_header.end_flag = OM_FRAME_DUMP_DELIMITER;
        }
        msg_header.frame_len = (uint16_t)(sizeof(om_msg_header_stru_t) + length_tmp + sizeof(msg_tail) +
                               sizeof(om_msg_dump_header_stru_t));
        /* Send exception stack */
        log_exception_send_data((uint8_t *)(&msg_header), sizeof(om_msg_header_stru_t));
        log_exception_send_data((uint8_t *)(&dump_header), sizeof(om_msg_dump_header_stru_t));
        log_exception_send_data((uint8_t *)(uintptr_t)ram_addr, (uint16_t)length_tmp);
        log_exception_send_data((uint8_t *)(&msg_tail), sizeof(msg_tail));
        dump_header.count++;
        ram_addr += length_tmp;
        ram_size -= length_tmp;
    }
    non_os_exit_critical();
#else
    UNUSED(addr);
    UNUSED(length);
#endif
}

void log_oml_dump_stack(void)
{
#if (ARCH == CM3) || (ARCH == CM7)
#ifdef USE_CMSIS_OS
#ifdef __LITEOS__
    LosTaskCB *los_task_cb = NULL;
    uint32_t loops;
    uint32_t stack_addr = 0;
    uint32_t stack_size = 0;
    uint32_t sp = __get_PSP();

    for (loops = 0; loops < g_taskMaxNum; loops++) {
        los_task_cb = (((LosTaskCB *)g_osTaskCBArray) + loops);

        if (los_task_cb->taskStatus & OS_TASK_STATUS_UNUSED) {
            continue;
        }

        if (sp > los_task_cb->topOfStack && sp < los_task_cb->topOfStack + los_task_cb->stackSize) {
            stack_addr = los_task_cb->topOfStack;
            stack_size = los_task_cb->stackSize;
            PRINT("current task stack_point: 0x%x\r\n", sp);
            PRINT("stack addr:0x%x,stack size:0x%x\r\n", stack_addr, stack_size);
            pf_write_fifo_log_alter(LOG_PFMODULE, LOG_NUM_LIB_LOG, LOG_LEVEL_ERROR,
                                    "[PF][DUMP INFO] stack point: 0x%x, stack address: 0x%x, stack size: 0x%x",
                                    STACK_SIZE, sp, stack_addr, stack_size);
            break;
        }
    }
    log_exception_dump_memory(stack_addr, stack_size);
#endif
#endif
#else
    UNUSED(log_exception_dump_memory);
#endif
}

static void log_watch_dog_disable(void)
{
    watchdog_turnon_clk();
    uapi_watchdog_disable();
    watchdog_turnoff_clk();
}

void log_oml_memory_dump(void)
{
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
    log_watch_dog_disable();
#if CORE == BT
    /* BT core has just one block of ram */
    uapi_tcxo_delay_ms(DUMP_DELAY);
#elif CORE == APPS
#ifdef HSO_SUPPORT
    dfx_last_dump();
#else
    log_exception_dump_memory(APP_ITCM_ORIGIN, APP_ITCM_LENGTH);
    log_exception_dump_memory(APP_DTCM_ORIGIN, APP_DTCM_LENGTH);
    log_exception_dump_memory(SHARED_MEM_START, SHARED_MEM_LENGTH);
    log_exception_dump_memory(MCPU_TRACE_MEM_REGION_START, CPU_TRACE_MEM_REGION_LENGTH);
#if CORE_NUMS != ONLY_ACORE_NUM
    log_exception_dump_memory(BT_RAM_ORIGIN_APP_MAPPING, BT_RAM_ORIGIN_APP_MAPPING_LENGTH);
    log_exception_dump_memory(BCPU_TRACE_MEM_REGION_START, CPU_TRACE_MEM_REGION_LENGTH);
#endif
    log_exception_dump_reg();
#endif
    /* mem dump save in flash */
#if (FLASH_DUMP_START != 0)
    dfx_last_dump2flash(FLASH_DUMP_START, FLASH_DUMP_SIZE);
#endif
#ifdef NO_TCXO_SUPPORT
    volatile int time_cnt = 0xFFFF;
    while ((time_cnt--) > 0) {
        __asm__ __volatile__("nop");
        __asm__ __volatile__("nop");
    }
#else
    uapi_tcxo_delay_ms(LOG_DELAY);
#endif
#elif CORE == WIFI
    log_exception_dump_memory(WL_BOOT_ROM_START, WL_BOOT_ROM_LEN);
    log_exception_dump_memory(WL_ITCM_IROM_START, WL_ITCM_IROM_LEN);
    log_exception_dump_memory(WL_TCM_RAM_START, WL_TCM_RAM_LEN);
    log_exception_dump_memory(WL_DTCM_PKTMEM_START, WL_DTCM_PKTMEM_LEN);
    log_exception_dump_memory(WCPU_TRACE_MEM_REGION_START, CPU_TRACE_MEM_REGION_LENGTH);
    tcxo_delay_ms(DUMP_DELAY);
    return;
#elif CORE == CONTROL_CORE
    log_exception_dump_memory(CT_SRAM_ORIGIN, CT_SRAM_LENGTH);
    log_exception_dump_memory(CT_ROM_ORIGIN, CT_ROM_LENGTH);
    log_exception_dump_memory(CCORE_FLASH_PROGRAM_ORIGIN, CCORE_FLASH_PROGRAM_LENGTH);
    uapi_tcxo_delay_ms(DUMP_DELAY);
    return;
#endif
#else  /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES */
#endif /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG */
}

#if defined(__ICCARM__)
#pragma segment = "CSTACK"
#endif

#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
static void log_oml_build_last_run_info(om_exception_info_stru_t *last_run_info,
                                        uint32_t irq_id, const exc_context_t *exc_buf_addr)
{
    uint32_t stack_limit;
#if defined(__GNUC__)
    stack_limit = (uint32_t)((uintptr_t)&(g_stack_system));
#else
    stack_limit = 0;
#endif

    last_run_info->msg_header.frame_start = OM_FRAME_DELIMITER;
    last_run_info->msg_header.func_type = OM_MSG_TYPE_LAST;
    last_run_info->msg_header.prime_id = OM_LOG_RPT_IND;
    last_run_info->msg_header.sn = get_log_sn_number();
    last_run_info->stack_limit = stack_limit;
    last_run_info->fault_type = irq_id;
    last_run_info->fault_reason = 0;
    last_run_info->address = 0;

    last_run_info->psp_value = exc_buf_addr->task_context.sp;
    last_run_info->lr_value = exc_buf_addr->task_context.ra;
    last_run_info->pc_value = exc_buf_addr->task_context.mepc;
    last_run_info->psps_value = exc_buf_addr->gp;
    last_run_info->primask_value = exc_buf_addr->task_context.mstatus;
    last_run_info->fault_mask_value = exc_buf_addr->mtval;
    last_run_info->bserpri_value = exc_buf_addr->ccause;
    last_run_info->control_value = exc_buf_addr->mtval;
    last_run_info->msg_header.frame_len = (uint16_t)sizeof(om_exception_info_stru_t);
    last_run_info->msg_tail = OM_FRAME_DELIMITER;
}
void log_oml_exception_info_send(uint32_t irq_id, const exc_context_t *exc_buf_addr)
{
    om_exception_info_stru_t last_run_info = { 0 };

    log_oml_build_last_run_info(&last_run_info, irq_id, exc_buf_addr);

    log_oml_get_reg_value(last_run_info.reg_value, exc_buf_addr);

    /*
     * Waiting for the entire log to be sent
     * Because it is possible that the uart transmission is not completed,
     * the structure assignment is placed in front to ensure that there is information in the stack.
     */
#if CORE == CORE_LOGGING
#ifdef HSO_SUPPORT
    dfx_last_word_send((uint8_t *)&last_run_info + sizeof(om_msg_header_stru_t),
                       sizeof(last_run_info) - sizeof(om_msg_header_stru_t) - OM_FRAME_DELIMITER_LEN);
#else
    uapi_uart_write(LOG_UART_BUS, (const void *)&last_run_info, sizeof(last_run_info), 0);
#endif
#elif defined(SDT_LOG_BY_UART)
    oml_write_uart_fifo((uint8_t *)&last_run_info, sizeof(last_run_info), LOGUART_BASE);
#else
    log_event((uint8_t *)&last_run_info, sizeof(last_run_info));
    log_trigger();
#endif

#ifdef NO_TCXO_SUPPORT
    volatile int time_cnt = 0xFFFF;
    while ((time_cnt--) > 0) {
        __asm__ __volatile__("nop");
        __asm__ __volatile__("nop");
    }
#else
    uapi_tcxo_delay_ms(LOG_DELAY);
#endif
}
#else
void log_oml_exception_info_send(uint32_t address, uint32_t fault_type, uint32_t fault_reason,
                                 const exc_info_t *exc_info)
{
    if (exc_info == NULL) {
        return;
    }

    om_exception_info_stru_t last_run_info = { 0 };
    uint8_t loop;
    uint32_t stack_limit;

#if defined(__GNUC__)
    stack_limit = (uint32_t)((uintptr_t)&(g_stack_system));
#elif defined(__ICCARM__)
    stack_limit = (uint32_t)__sfe("CSTACK") - 4;  // 4 byte
#else
    stack_limit = 0;
#endif

    last_run_info.msg_header.frame_start = OM_FRAME_DELIMITER;
    last_run_info.msg_header.func_type = OM_MSG_TYPE_LAST;
    last_run_info.msg_header.prime_id = OM_LOG_RPT_IND;
    last_run_info.msg_header.sn = get_log_sn_number();
    last_run_info.stack_limit = stack_limit;
    last_run_info.fault_type = fault_type;
    last_run_info.fault_reason = fault_reason;
    last_run_info.address = address;

    memcpy_s((void *)&last_run_info.reg_value[0], AULREG_VALUE_INDEX, &(exc_info->context->r4), AULREG_VALUE_INDEX);

#if (ARCH == CM3) || (ARCH == CM7)
    last_run_info.psp_value = __get_PSP();
    last_run_info.lr_value = exc_info->context->lr;
    last_run_info.pc_value =  exc_info->context->pc;
    last_run_info.psps_value = exc_info->context->xpsr;
    last_run_info.primask_value = __get_PRIMASK();
    last_run_info.fault_mask_value = (uint32_t)__get_FAULTMASK();
    last_run_info.bserpri_value = (uint32_t)__get_BASEPRI();
    last_run_info.control_value = __get_CONTROL();
    last_run_info.msg_header.frame_len = sizeof(om_exception_info_stru_t);
    last_run_info.msg_tail = OM_FRAME_DELIMITER;
#endif
    /*
     * Waiting for the entire log to be sent
     * Because it is possible that the uart transmission is not completed,
     * the structure assignment is placed in front to ensure that there is information in the stack.
     */
#if CORE == CORE_LOGGING
#ifdef HSO_SUPPORT
    dfx_last_word_send((uint8_t *)&last_run_info + sizeof(om_msg_header_stru_t),
                       sizeof(last_run_info) - sizeof(om_msg_header_stru_t) - OM_FRAME_DELIMITER_LEN);
#else
    uapi_uart_write(LOG_UART_BUS, (const void *)&last_run_info, sizeof(last_run_info), 0);
#endif
#else
    log_event((uint8_t *)&last_run_info, sizeof(last_run_info));
    log_trigger();
#endif
    uapi_tcxo_delay_ms(LOG_DELAY);
}
#endif

void default_register_hal_exception_dump_callback(void)
{
#ifdef SUPPORT_DFX_EXCEPTION
    hal_register_exception_dump_callback(log_exception_dump);
#endif
}
