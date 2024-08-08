/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description: System Config HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

/**
 * @defgroup los_config System configuration items
 * @ingroup kernel
 */

#ifndef _LOS_CONFIG_H
#define _LOS_CONFIG_H

#include "los_typedef.h"
#ifndef LOSCFG_MAKE_COMPILER_LIBS
#include "asm/memmap_config.h"
#include "soc/clock.h"
#include "asm/platform.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_LIB_CONFIGURABLE
extern UINT32 g_osSysClock;
extern UINT32 g_tickPerSecond;
extern UINT32 g_taskLimit;
extern UINT32 g_taskMinStkSize;
extern UINT32 g_taskIdleStkSize;
extern UINT32 g_taskDfltStkSize;
extern UINT32 g_taskSwtmrStkSize;
extern UINT32 g_swtmrLimit;
extern UINT32 g_semLimit;
extern UINT32 g_rwsemLimit;
extern UINT32 g_muxLimit;
extern UINT32 g_queueLimit;
extern UINT32 g_timeSliceTimeOut;

extern BOOL    g_nxEnabled;
extern UINTPTR g_dlNxHeapBase;
extern UINT32  g_dlNxHeapSize;

#define LOS_GET_NX_CFG()              (g_nxEnabled)
#define LOS_SET_NX_CFG(value)         (g_nxEnabled = (value))
#define LOS_GET_DL_NX_HEAP_BASE()     (g_dlNxHeapBase)
#define LOS_SET_DL_NX_HEAP_BASE(addr) (g_dlNxHeapBase = (addr))
#define LOS_GET_DL_NX_HEAP_SIZE()     (g_dlNxHeapSize)
#define LOS_SET_DL_NX_HEAP_SIZE(size) (g_dlNxHeapSize = (size))

#define OS_SYS_CLOCK                      g_osSysClock
#define KERNEL_TICK_PER_SECOND            g_tickPerSecond
#define KERNEL_TSK_LIMIT                  g_taskLimit
#define KERNEL_TSK_MIN_STACK_SIZE         g_taskMinStkSize
#define KERNEL_TSK_DEFAULT_STACK_SIZE     g_taskDfltStkSize
#define KERNEL_TSK_IDLE_STACK_SIZE        g_taskIdleStkSize
#define KERNEL_TSK_SWTMR_STACK_SIZE       g_taskSwtmrStkSize
#define KERNEL_SWTMR_LIMIT                g_swtmrLimit
#define KERNEL_SEM_LIMIT                  g_semLimit
#define KERNEL_RWSEM_LIMIT                g_rwsemLimit
#define KERNEL_MUX_LIMIT                  g_muxLimit
#define KERNEL_QUEUE_LIMIT                g_queueLimit
#define KERNEL_TIMESLICE_TIMEOUT          g_timeSliceTimeOut
#else /* !LOSCFG_LIB_CONFIGURABLE */

#ifdef LOSCFG_KERNEL_NX
#define LOS_GET_NX_CFG() true
#define LOS_SET_NX_CFG(value)
#define LOS_GET_DL_NX_HEAP_BASE() LOS_DL_HEAP_BASE
#define LOS_SET_DL_NX_HEAP_BASE(addr)
#define LOS_GET_DL_NX_HEAP_SIZE() LOS_DL_HEAP_SIZE
#define LOS_SET_DL_NX_HEAP_SIZE(size)
#else /* LOSCFG_KERNEL_NX */
#define LOS_GET_NX_CFG() false
#define LOS_SET_NX_CFG(value)
#define LOS_GET_DL_NX_HEAP_BASE() NULL
#define LOS_SET_DL_NX_HEAP_BASE(addr)
#define LOS_GET_DL_NX_HEAP_SIZE() 0
#define LOS_SET_DL_NX_HEAP_SIZE(size)
#endif /* LOSCFG_KERNEL_NX */

#define KERNEL_TICK_PER_SECOND            LOSCFG_BASE_CORE_TICK_PER_SECOND
#define KERNEL_TSK_LIMIT                  LOSCFG_BASE_CORE_TSK_LIMIT
#define KERNEL_TSK_MIN_STACK_SIZE         LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE
#define KERNEL_TSK_DEFAULT_STACK_SIZE     LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE
#define KERNEL_TSK_IDLE_STACK_SIZE        LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE
#define KERNEL_TSK_SWTMR_STACK_SIZE       LOSCFG_BASE_CORE_TSK_SWTMR_STACK_SIZE
#define KERNEL_SWTMR_LIMIT                LOSCFG_BASE_CORE_SWTMR_LIMIT
#define KERNEL_SEM_LIMIT                  LOSCFG_BASE_IPC_SEM_LIMIT
#define KERNEL_RWSEM_LIMIT                LOSCFG_BASE_IPC_RWSEM_LIMIT
#define KERNEL_MUX_LIMIT                  LOSCFG_BASE_IPC_MUX_LIMIT
#define KERNEL_QUEUE_LIMIT                LOSCFG_BASE_IPC_QUEUE_LIMIT
#define KERNEL_TIMESLICE_TIMEOUT          LOSCFG_BASE_CORE_TIMESLICE_TIMEOUT

#endif /* LOSCFG_LIB_CONFIGURABLE */

/**
 * system sections start and end address
 */
/* conflict with int __text_start in compiler */
#ifndef LOSCFG_MAKE_COMPILER_LIBS
typedef CHAR LD_SYMBOL_TYPE;
#else
typedef int LD_SYMBOL_TYPE;
#endif
extern LD_SYMBOL_TYPE __int_stack_start;
extern LD_SYMBOL_TYPE __int_stack_end;
extern LD_SYMBOL_TYPE __rodata_start;
extern LD_SYMBOL_TYPE __rodata_end;
extern LD_SYMBOL_TYPE __bss_start;
extern LD_SYMBOL_TYPE __bss_end;
extern LD_SYMBOL_TYPE __text_start;
extern LD_SYMBOL_TYPE __text_end;
extern LD_SYMBOL_TYPE __ram_data_start;
extern LD_SYMBOL_TYPE __ram_data_end;
extern LD_SYMBOL_TYPE __exc_heap_start;
extern LD_SYMBOL_TYPE __exc_heap_end;
extern LD_SYMBOL_TYPE __heap_start;
extern LD_SYMBOL_TYPE __init_array_start;
extern LD_SYMBOL_TYPE __init_array_end;
extern LD_SYMBOL_TYPE __trace_buf_start;
extern LD_SYMBOL_TYPE __trace_buf_end;

/****************************** System clock module configuration ****************************/
/**
 * @ingroup los_config
 * System clock (unit: HZ)
 */
#ifndef OS_SYS_CLOCK
#define OS_SYS_CLOCK (get_bus_clk())
#endif
/**
 * @ingroup los_config
 * time timer clock (unit: HZ)
 */
#ifndef OS_TIME_TIMER_CLOCK
#define OS_TIME_TIMER_CLOCK OS_SYS_CLOCK
#endif

/**
 * @ingroup los_config
 * The end address of system memory.
 */
extern UINTPTR g_sys_mem_addr_end;

#ifdef LOSCFG_EXC_INTERACTION
/**
 * @ingroup los_config
 * The size of exception interaction memory.
 */
extern UINTPTR g_excInteractMemSize;
#endif

/**
 * limit addr range when search for  'func local(frame pointer)' or 'func name'
 */
#ifndef OS_SYS_FUNC_ADDR_START
#define OS_SYS_FUNC_ADDR_START ((UINTPTR)&__int_stack_start)
#endif
#ifndef OS_SYS_FUNC_ADDR_END
#define OS_SYS_FUNC_ADDR_END g_sys_mem_addr_end
#endif

/**
 * @ingroup los_config
 * Microseconds of adjtime in one second
 */
#ifndef LOSCFG_BASE_CORE_ADJ_PER_SECOND
#define LOSCFG_BASE_CORE_ADJ_PER_SECOND 500
#endif

/**
 * @ingroup los_config
 * Sched clock interval
 */
#define SCHED_CLOCK_INTETRVAL_TICKS 100

/****************************** Swtmr module configuration ********************************/
/**
 * @ingroup los_swtmr
 * Max number of software timers ID
 *
 * 0xFFFF: max number of all software timers
 */
#ifndef OS_SWTMR_MAX_TIMERID
#define OS_SWTMR_MAX_TIMERID ((0xFFFF / KERNEL_SWTMR_LIMIT) * KERNEL_SWTMR_LIMIT)
#endif
/**
 * @ingroup los_swtmr
 * Maximum size of a software timer queue. The default value of LOSCFG_BASE_CORE_SWTMR_LIMIT is 16.
 */
#ifndef OS_SWTMR_HANDLE_QUEUE_SIZE
#define OS_SWTMR_HANDLE_QUEUE_SIZE KERNEL_SWTMR_LIMIT
#endif

/****************************** Memory module configuration **************************/
/**
 * @ingroup los_memory
 * Starting address of the system memory
 */
#ifndef OS_SYS_MEM_ADDR
#define OS_SYS_MEM_ADDR          (&__heap_start)
#endif

/**
 * @ingroup los_dynload
 * Size of Dynload heap in bytes (1MB = 0x100000 Bytes)
 * Starting address of dynload heap
 */
#if defined (LOSCFG_KERNEL_NX) && defined (LOSCFG_KERNEL_DYNLOAD)
#define LOS_DL_HEAP_SIZE  (LOSCFG_KERNEL_DYN_HEAPSIZE * 0x100000)
#define LOS_DL_HEAP_BASE  (SYS_MEM_END - LOS_DL_HEAP_SIZE)
#else
#define LOS_DL_HEAP_SIZE   0
#define LOS_DL_HEAP_BASE   0
#endif

/**
 * @ingroup los_memory
 * Memory size
 */
#ifndef OS_SYS_MEM_SIZE
#define OS_SYS_MEM_SIZE ((g_sys_mem_addr_end) - \
                         ((LOS_DL_HEAP_SIZE  + ((UINTPTR)&__heap_start) + (64 - 1)) & ~(64 - 1)))
#endif

/****************************** fw Interface configuration **************************/
/**
 * @ingroup los_config
 * The core number is one in non-SMP architecture.
 */
#ifdef LOSCFG_KERNEL_SMP
#define LOSCFG_KERNEL_CORE_NUM                          LOSCFG_KERNEL_SMP_CORE_NUM
#else
#define LOSCFG_KERNEL_CORE_NUM                          1
#endif

#define LOSCFG_KERNEL_CPU_MASK                          ((1 << LOSCFG_KERNEL_CORE_NUM) - 1)

/****************************** trace module configuration **************************/
/**
 * @ingroup los_trace
 * It's the total size of trace buffer. Its unit is char.
 * When dynamic memory not enabled, the address and size is calculated by symbols defined in the linker script.
 * Recommended to define the section size using #LOSCFG_TRACE_BUFFER_SIZE.
 */
#ifdef LOSCFG_KERNEL_TRACE
#ifdef LOSCFG_RECORDER_MODE_OFFLINE
#ifdef LOSCFG_KERNEL_MEM_ALLOC
#define LOS_TRACE_BUFFER_SIZE                           LOSCFG_TRACE_BUFFER_SIZE
#else // !LOSCFG_KERNEL_MEM_ALLOC
#define LOS_TRACE_BUFFER_SIZE                           ((UINT32)(&__trace_buf_end - &__trace_buf_start))
#endif
#else
#define LOS_TRACE_BUFFER_SIZE 0
#endif
#endif

/**
 * Version number
 */
#define _T(x)                                   x
#define HW_LITEOS_SYSNAME                       "Huawei LiteOS"
#define HW_LITEOS_SEP                           " "
#define _V(v)                                   _T(HW_LITEOS_SYSNAME)_T(HW_LITEOS_SEP)_T(v)

#define HW_LITEOS_VERSION                       "208.5.0.B010"
#define HW_LITEOS_VER                           _V(HW_LITEOS_VERSION"-SMP")

/**
 * The Version number of Public
 */
#define MAJ_V                                   5
#define MIN_V                                   5
#define REL_V                                   0

/**
 * The release candidate version number
 */
#define EXTRA_V                                 0

#define VERSION_NUM(a, b, c)                    (((a) << 16) | ((b) << 8) | (c))
#define HW_LITEOS_OPEN_VERSION_NUM              VERSION_NUM(MAJ_V, MIN_V, REL_V)

#define STRINGIFY_1(x)                          #x
#define STRINGIFY(x)                            STRINGIFY_1(x)

#define HW_LITEOS_OPEN_VERSION_STRING           STRINGIFY(MAJ_V) "." STRINGIFY(MIN_V) "." STRINGIFY(REL_V)
#if (EXTRA_V != 0)
#define HW_LITEOS_KERNEL_VERSION_STRING         HW_LITEOS_OPEN_VERSION_STRING "-rc" STRINGIFY(EXTRA_V)
#else
#define HW_LITEOS_KERNEL_VERSION_STRING         HW_LITEOS_OPEN_VERSION_STRING
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_CONFIG_H */
