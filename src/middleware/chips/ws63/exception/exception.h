/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:HAL UTILS INTERFACE
 */

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdint.h>
#include "platform.h"
#include "errcode.h"

#if (ARCH == RISCV31) || (ARCH == RISCV70)
#include "vectors.h"
#endif

/** @defgroup connectivity_drivers_hal_hal HAL
  * @ingroup  connectivity_drivers_hal
  * @{
  */
#define WAIT_APPS_DUMP_DELAY_MS_EACH_TIME 16000U
#define WAIT_APPS_DUMP_DELAY_TIMES 2
#define WAIT_APPS_REFRESH_FLASH_MS 2000

#define EXCEP_INFO_FOR_R4_TO_R11 8
#ifndef LOSCFG_EXC_SIMPLE_INFO
#if defined(__LITEOS__)
#define EXC_INFO_SAVE_TP_INDEX 16
#endif
#if defined(__FREERTOS__)
#if CORE == WIFI
#define EXC_INFO_SAVE_TP_INDEX 9
#else
#define EXC_INFO_SAVE_TP_INDEX 8
#endif
#endif
#else
#define EXC_INFO_SAVE_X4_INDEX 15
#endif

enum INT_ID_FAULT_TYPE_ENUM {
    INT_ID_NMI_FAULT = 2,
    INT_ID_HARD_FAULT = 3,
    INT_ID_MEM_FAULT = 4,
    INT_ID_BUS_FAULT = 5,
    INT_ID_USAGE_FAULT = 6,
    INT_ID_CHIP_WATCHDOG_FAULT = 59,
    INT_ID_WATCHDOG_FAULT = 47,

    INT_ID_INDEX_BUTT
};

/**
 * riscv fault type
 */
enum IRQ_ID_FAULT_TYPE_ENMU {
    IRQ_ID_INSTRUCTION_ADDRESS_MISALIGNED = 0,
    IRQ_ID_INSTRUCTION_ACCESS_FAULT = 1,
    IRQ_ID_ILLEGAL_INSTRUCTION = 2,
    IRQ_ID_BREAKPOINT = 3,
    IRQ_ID_LOAD_ADDERSS_MISALIGNED = 4,
    IRQ_ID_LOAD_ACCESS_FAULT = 5,
    IRQ_ID_STORE_OR_AMO_ADDRESS_MISALIGNED = 6,
    IRQ_ID_STORE_OR_AMO_ACCESS_FALUT = 7,
    IRQ_ID_ENVIRONMENT_CALL_FROM_UMODE = 8,
    IRQ_ID_ENVIRONMENT_CALL_FROM_SMODE = 9,
    IRQ_ID_RESERVED_0 = 10,
    IRQ_ID_ENVIRONMENT_CALL_FROM_MMODE = 11,
    IRQ_ID_INSTRUCTION_PAGE_FAULT = 12,
    IRQ_ID_LOAD_PAGE_FAULT = 13,
    IRQ_ID_RESERVED_1 = 14,
    IRQ_ID_STORE_OR_AMO_PAGE_FAULT = 15,

#if (ARCH == RISCV70)
    IRQ_ID_ASYNCHRONOUS_EXCEPTION = 24,
    IRQ_ID_NMI_INTERRUPT = 0x00000FFF
#else
    IRQ_ID_HARD_FAULT = 16,
    IRQ_ID_LOCK_UP = 17,
    IRQ_ID_NMI_INTERRUPT = 0x8000000C
#endif
};

/**
 * @brief  Structure which is pushed onto the stack by the Cortex-M0 during
 * exception processing - stack frame.
 */
typedef struct stack_frame_t {
    unsigned long stacked_r0;
    unsigned long stacked_r1;
    unsigned long stacked_r2;
    unsigned long stacked_r3;
    unsigned long stacked_r12;
    unsigned long stacked_lr;
    unsigned long stacked_pc;
    unsigned long stacked_psr;
} exception_stack_frame_t;

typedef struct excep_info_t {
#if (ARCH == CM3) || (ARCH == CM7)
    unsigned long regs[EXCEP_INFO_FOR_R4_TO_R11];  // r4-r11
#endif
    unsigned long sp;
    unsigned long sp_bottom;
    unsigned long exp_task_id;
    unsigned long task_array;
    unsigned long task_max_num;
} exception_info_t;

#if (ARCH == CM7)
typedef struct  {
#ifdef LOSCFG_ARCH_FPU_ENABLE
    uint32_t s16;
    uint32_t s17;
    uint32_t s18;
    uint32_t s19;
    uint32_t s20;
    uint32_t s21;
    uint32_t s22;
    uint32_t s23;
    uint32_t s24;
    uint32_t s25;
    uint32_t s26;
    uint32_t s27;
    uint32_t s28;
    uint32_t s29;
    uint32_t s30;
    uint32_t s31;
#endif
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t primask;
    uint32_t sp;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t xpsr;
#ifdef LOSCFG_ARCH_FPU_ENABLE
    uint32_t s0;
    uint32_t s1;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
    uint32_t s12;
    uint32_t s13;
    uint32_t s14;
    uint32_t s15;
    uint32_t fpscr;
    uint32_t no_name;
#endif
} exc_content_t;

typedef struct {
    uint16_t phase;
    uint16_t type;
    uint32_t fault_addr;
    uint32_t intnum_or_taskid;
    uint16_t nest_cnt;
    uint16_t reserved;
    exc_content_t *context;
} exc_info_t;

#endif
#if (ARCH == RISCV31) || (ARCH == RISCV70)
#ifdef SUPPORT_CALLSTACK
typedef struct {
    uint32_t ra;
    uint32_t fp;
} exc_stack_info_t;
#else
typedef struct {
    uint32_t sp_addr;
    uint32_t sp_content;
} exc_backtrace_info_t;
#endif

#if defined(__LITEOS__)
typedef struct {
    char task_name[16];
    uint32_t thrd_pid;
    uint16_t type;
    uint16_t nest_cnt;
#ifndef LOSCFG_EXC_SIMPLE_INFO
    char phase[8];
#else
    uint16_t phase;
    uint16_t ret;
#endif
    uint32_t ccause;
    uint32_t mcause;
    uint32_t mtval;
    uint32_t gp;
    uint32_t mstatus;
    uint32_t mepc;
    uint32_t ra;
    uint32_t sp;
#ifndef LOSCFG_EXC_SIMPLE_INFO
    uint32_t tp;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t s0;
    uint32_t s1;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
#else
    uint32_t x4;
    uint32_t x5;
    uint32_t x6;
    uint32_t x7;
    uint32_t x8;
    uint32_t x9;
    uint32_t x10;
    uint32_t x11;
    uint32_t x12;
    uint32_t x13;
    uint32_t x14;
    uint32_t x15;
    uint32_t x16;
    uint32_t x17;
    uint32_t x18;
    uint32_t x19;
    uint32_t x20;
    uint32_t x21;
    uint32_t x22;
    uint32_t x23;
    uint32_t x24;
    uint32_t x25;
    uint32_t x26;
    uint32_t x27;
    uint32_t x28;
    uint32_t x29;
    uint32_t x30;
    uint32_t x31;
#endif
#ifdef SUPPORT_CALLSTACK
    uint32_t stack_size;
    exc_stack_info_t stack[0];
#else
    uint32_t backtrace_size;
    exc_backtrace_info_t backtrace[0];
#endif
} exc_info_save_t;
#define EXC_INFO_SAVE_SIZE sizeof(exc_info_save_t)
#endif

#if defined(__FREERTOS__)
typedef struct {
    uint16_t uw_exc_type;
    uint16_t ret;
    uint32_t mepc;
    uint32_t mstatus;
    uint32_t mtval;
    uint32_t mcause;
#if CORE == WIFI
    uint32_t cxcptsc;
#endif
    uint32_t ra;
    uint32_t sp;
    uint32_t gp;
    uint32_t tp;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t s0;
    uint32_t s1;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
#ifdef SUPPORT_CALLSTACK
    uint32_t stack_size;
    exc_stack_info_t stack[0];
#endif
} exc_info_save_t;
#define EXC_INFO_SAVE_SIZE sizeof(exc_info_save_t)
#endif
#endif
#if (ARCH == CM3) || (ARCH == CM7)
typedef struct {
    uint32_t lr;
    uint32_t fp;
} exc_stack_info_t;
typedef struct {
    char task_name[16];
    uint32_t task_id;
    uint32_t task_stack_size;
    uint32_t system_mem_addr;
    char phase[22];
    uint16_t type;
    uint32_t fault_addr;
    uint32_t int_num_or_task_id;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t pri_mask;
    uint32_t sp;
    uint32_t lr;
    uint32_t pc;
    uint32_t xpsr;
    uint32_t psp;
    uint32_t pri_mask_scb;
    uint32_t fault_mask;
    uint32_t base_pri;
    uint32_t control;
    uint32_t scb_icsr;
    uint32_t scb_scr;
    uint32_t scb_shcsr;
    uint32_t scb_cfsr;
    uint32_t scb_hfsr;
    uint32_t scb_dfsr;
    uint32_t scb_mmfar;
    uint32_t scb_bfsr;
    uint32_t scb_afsr;
#ifdef LOSCFG_BACKTRACE
    uint32_t stack_size;
    exc_stack_info_t stack[0];
#endif
} exc_info_save_t;
#define EXC_INFO_SAVE_SIZE sizeof(exc_info_save_t)
#endif
/**
 * @brief  Callback to output the exception information.
 */
#if (ARCH == RISCV31) || (ARCH == RISCV70)
typedef void (*nmi_proc_func)(exc_context_t *);
typedef void (*hal_exception_dump_callback)(uint32_t irq_id, exc_context_t *exc_buf_addr);
#else
typedef void (*hal_exception_dump_callback)(uint32_t id, uint32_t reason, uint32_t addr, exc_info_t *exc_info);
#endif

/**
 * @brief  Prototype for the assembly language hard-fault handler. This will setup the stack and attempt to
 * generate a stack trace to aid debug.
 * @param  exc_buf_addr Fault param1.
 */
#if (ARCH == RISCV31) || (ARCH == RISCV70)
void do_hard_fault_handler(exc_context_t *exc_buf_addr);
#if defined(__LITEOS__)
void do_fault_handler(uint32_t exc_type, exc_context_t *exc_buff_addr);
#endif
#if defined(__FREERTOS__)
void do_fault_handler_freertos(void);
#endif
#elif (ARCH == CM7)
/**
 * @brief  Prototype for the assembly language hard-fault handler. This will setup the stack and attempt to
 * generate a stack trace to aid debug.
 * @param  exc_info exception information.
 * @param  context register information.
 */
void do_hard_fault_handler(uint32_t exc_info, exc_content_t *context);
#endif

/**
 * @brief  Register the exception dump callback
 * @param  callback The dump callback func
 */
void hal_register_exception_dump_callback(hal_exception_dump_callback callback);

/**
 * @brief  Wait for the apps to finish the preparing for rebooting.
 * eg.  notify the apps and finish coredump if the debug is enable;
 *      Wait for the apps refresh the flash.
 * @return
 */
void wait_apps_prepare_for_rebooting(void);

/**
 * @brief  Save the exception reset information for debug after the restart.
 * @return
 */
void hal_save_exception_reset_info(void);

#ifdef CONFIG_SUPPORT_CRASHINFO_SAVE_TO_FLASH
void crashinfo_save(exc_context_t *exc_buf_addr);
bool crashinfo_status_get(void);
void crashinfo_dump(void);
void crashinfo_clr(void);
#endif

#if (ARCH == RISCV31) || (ARCH == RISCV70)
#ifndef SUPPORT_CALLSTACK
void back_trace(uint32_t fp);
#endif
#ifndef USE_CMSIS_OS
void do_trap_unknown(exc_context_t *exc_buff_addr);
void do_trap_insn_misaligned(exc_context_t *exc_buff_addr);
void do_trap_insn_fault(exc_context_t *exc_buff_addr);
void do_trap_insn_illegal(exc_context_t *exc_buff_addr);
void do_trap_load_misaligned(exc_context_t *exc_buff_addr);
void do_trap_load_fault(exc_context_t *exc_buff_addr);
void do_trap_store_misaligned(exc_context_t *exc_buff_addr);
void do_trap_store_fault(exc_context_t *exc_buff_addr);
void do_trap_ecall_u(exc_context_t *exc_buff_addr);
void do_trap_ecall_s(exc_context_t *exc_buff_addr);
void do_trap_ecall_m(exc_context_t *exc_buff_addr);
void do_trap_break(exc_context_t *exc_buff_addr);
void do_insn_page_fault(exc_context_t *exc_buff_addr);
void do_load_page_fault(exc_context_t *exc_buff_addr);
void do_store_page_fault(exc_context_t *exc_buff_addr);
void do_hard_fault(exc_context_t *exc_buff_addr);
void do_lockup(exc_context_t *exc_buff_addr);
void default_handler(void);
void nmi_handler(void);
#endif

#ifdef SAVE_EXC_INFO
#if CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES
/**
 * @brief  Write exc info to the file system.
 * @return The length of file written if success or -1 if failed.
 */
int32_t exc_info_write(const char *path, uint32_t offset, const uint8_t *buf, uint32_t size);
#endif
#ifdef CFG_DRIVERS_MMC
/**
 * @brief  Emmc write exc info to the file system from sector after reboot system.
 * @return Success or fail.
 */
errcode_t emmc_exc_info_write(void);
#endif
#endif
#endif

/**
  * @}
  */
#endif
