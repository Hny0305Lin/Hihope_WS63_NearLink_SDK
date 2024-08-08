/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:  OAM TRACE for RISCV31
 *
 * Create: 2021-10-25
 */
#include "oam_trace.h"
#include "print_config.h"
#include "arch_encoding.h"
#include "arch_trace.h"
#include "memory_config.h"
#include "share_mem_config.h"
#include "arch/exception.h"

#define  USER_STACK_OFFSET_PER   4
#define  USER_STACK_PRINT_DEPTH  128

static bool check_txt_addr_range(uint32_t pc, uint32_t text_start, uint32_t text_end)
{
    if (pc >= text_start && pc < text_end) {
        return true;
    } else {
        return false;
    }
}

bool is_valid_txt_addr(uint32_t pc)
{
    /* flash text */
    if (check_txt_addr_range(pc, (uintptr_t)&__text_begin__, (uintptr_t)&__text_end__)) {
        return true;
    }

    /* tcm text */
    if (check_txt_addr_range(pc, (uintptr_t)&__tcm_text_begin__, (uintptr_t)&__tcm_text_end__)) {
        return true;
    }

    /* sram text */
    if (check_txt_addr_range(pc, (uintptr_t)&__sram_text_begin__, (uintptr_t)&__sram_text_end__)) {
        return true;
    }

    /* rom */
    if (check_txt_addr_range(pc, ROM_START, ROM_START + ROM_LENGTH)) {
        return true;
    }

    return false;
}

#ifdef USE_CMSIS_OS
EXC_PROC_FUNC g_exec_hook;
#endif

static void back_trace(uint32_t sp)
{
    uint32_t back_sp = sp;
    uint32_t count = 0;

    oam_trace_print("*******stack txt info begin*******\r\n");

    while (back_sp != 0) {
        if (is_valid_txt_addr(*((uint32_t *)(back_sp)))) {
            oam_trace_print("traceback %d -- sp addr= 0x%x    sp content= 0x%x\r\n", count, back_sp,
                            *((uint32_t *)(back_sp)));
        }

        back_sp = back_sp + USER_STACK_OFFSET_PER;
        count++;
        if (count == USER_STACK_PRINT_DEPTH) {
            break;
        }
    }
    oam_trace_print("*******stack txt info end*******\r\n");
}

#ifdef SUPPORT_CPU_TRACE
static void riscv_cpu_trace_print(void)
{
    riscv_cpu_trace_item_t *item = (riscv_cpu_trace_item_t *)(uintptr_t)(RISCV_TRACE_MEM_REGION_START);
    uint32_t cpu_trace_end_addr = RISCV_TRACE_MEM_REGION_START + RISCV_TRACE_MEM_REGION_LENGTH;

    oam_trace_print("**************CPU Trace Information************** \r\n");

    for (uint32_t i = 0; &item[i] < (riscv_cpu_trace_item_t *)(uintptr_t)(cpu_trace_end_addr); i++) {
        oam_trace_print("cpu trace %d:\r\n", i);
        oam_trace_print("step time:0x%x   LR:0x%x   PC:0x%x\r\n", item[i].time, item[i].lr, item[i].pc);
    }

    oam_trace_print("**************CPU Trace end*********************** \r\n");
}
#endif

static void print_excptsc(void)
{
    oam_trace_print("cxcptsc = 0x%x\r\n", read_custom_csr(CXCPTSC));
}

#ifdef USE_CMSIS_OS
void exec_fault_handler(uint32_t exc_type, const ExcContext *exc_buff_addr)
{
    riscv_cpu_trace_disable();
    if (g_exec_hook != NULL) {
        g_exec_hook(exc_type, exc_buff_addr);
    }
    // custom oam information
    print_excptsc();
    if (exc_buff_addr != NULL) {
#ifdef USE_ALIOS
        back_trace((uint32_t)exc_buff_addr->taskContext.X2);
#else
        oam_trace_print("*******backtrace begin*******\r\n");
        ArchBackTraceGet(exc_buff_addr->taskContext.s0, NULL, USER_STACK_PRINT_DEPTH, 0);
        oam_trace_print("*******backtrace end*******\r\n");
        back_trace(exc_buff_addr->taskContext.sp);
#endif
    }
#ifdef SUPPORT_CPU_TRACE
    riscv_cpu_trace_print();
#endif
}

void register_os_exec_hook(void)
{
    g_exec_hook = ArchGetExcHook();
    ArchSetExcHook((EXC_PROC_FUNC)exec_fault_handler);
}
#endif
