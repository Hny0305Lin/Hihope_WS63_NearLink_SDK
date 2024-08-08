/*----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2020. All rights reserved.
 * Description: LiteOS Performance Monitor Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2017-01-01
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
 *---------------------------------------------------------------------------*/

#ifndef _LOS_PERF_PRI_H
#define _LOS_PERF_PRI_H

#include "los_perf.h"
#include "arch/perf.h"
#include "los_mp_pri.h"
#include "los_task_pri.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OS_MIN(x, y)               (((x) <= (y)) ? (x) : (y))

#define PERF_EVENT_TO_CODE       0
#define PERF_CODE_TO_EVENT       1
#define PERF_DATA_MAGIC_WORD     0xEFEFEF00

#define SMP_CALL_PERF_FUNC(func)  OsMpFuncCall(OS_MP_CPU_ALL, (SMP_FUNC_CALL)func, NULL)

enum PmuStatus {
    PERF_PMU_STOPED,
    PERF_PMU_STARTED,
};

typedef struct {
    UINTPTR pc;
    UINTPTR fp;
} PerfRegs;

typedef struct {
    UINT32  ipNr;
    UINTPTR ip[PERF_MAX_CALLCHAIN_DEPTH];
} PerfBackTrace;

typedef struct {
    UINT32        cpuid;                /* cpu id */
    UINT32        taskId;               /* task id */
    UINT32        eventId;              /* record type */
    UINT32        period;               /* record period */
    UINT64        time;                 /* record time */
    UINTPTR       pc;                   /* instruction pointer */
    PerfBackTrace callChain;            /* number of callChain ips */
} PerfSampleData;

typedef struct {
    UINT32      magic;           /* magic number */
    UINT32      eventType;       /* enum PerfEventType */
    UINT32      len;             /* sample data file length */
    UINT32      sampleType;      /* IP | TID | TIMESTAMP... */
    UINT32      sectionId;       /* section id */
} PerfDataHdr;

typedef struct {
    UINT32 counter;
    UINT32 eventId;
    UINT32 period;
    UINT64 prevCount;
#ifdef LOSCFG_ARCH_RISCV32
    UINT64 prevOverflowCount;
#endif
    UINT64 count;
} Event;

typedef struct {
    Event per[PERF_MAX_EVENT];
    UINT8 nr;
    UINT8 cntDivided;
} PerfEvent;

typedef struct {
    enum PerfEventType type;
    PerfEvent *current[LOSCFG_KERNEL_CORE_NUM];

    /* Parse user config to initialize events. */
    UINT32 (*config)(PerfEvent *events, const PerfEventConfig *config);
    /* Start pmu on current core.
     * If restart is true, pmu shall in intial state after started, and resources shall initialized.
     */
    UINT32 (*start)(PerfEvent *events, BOOL restart);
    /* Stop pmu on current core and update event counts.
     * If update is true, pmu shall in final state after stoped, and resources can be released.
     * Whether update is used, the event counts shall always updated.
     */
    UINT32 (*stop)(PerfEvent *events, BOOL update);
    /* convert event to name */
    CHAR *(*getName)(Event *event);
} Pmu;

typedef struct {
    /* time info */
    UINT64                  startTime;
    UINT64                  endTime;

    /* instrumentation status */
    enum PerfStatus         status;
    enum PmuStatus          pmuStatusPerCpu[LOSCFG_KERNEL_CORE_NUM];

    /* configuration info */
    UINT32                  sampleType;
    UINT32                  taskIds[PERF_MAX_FILTER_TSKS];
    UINT8                   taskIdsNr;
    UINT8                   needSample;
    UINT8                   taskFilterEnable;
} PerfCB;

#ifndef OsPerfArchFetchIrqRegs
STATIC INLINE VOID OsPerfArchFetchIrqRegs(PerfRegs* regs, LosTaskCB *curTask)
{
    (VOID)regs;
    (VOID)curTask;
}
#endif

STATIC INLINE VOID OsPerfFetchIrqRegs(PerfRegs *regs)
{
    LosTaskCB *curTask = OsCurrTaskGet();
    OsPerfArchFetchIrqRegs(regs, curTask);
    PRINT_DEBUG("pc = 0x%x, fp = 0x%x\n", regs->pc, regs->fp);
}

#ifndef OsPerfArchFetchCallerRegs
STATIC INLINE VOID OsPerfArchFetchCallerRegs(PerfRegs *regs)
{
    (VOID)regs;
}
#endif

STATIC INLINE VOID OsPerfFetchCallerRegs(PerfRegs *regs)
{
    OsPerfArchFetchCallerRegs(regs);
    PRINT_DEBUG("pc = 0x%x, fp = 0x%x\n", regs->pc, regs->fp);
}

extern VOID OsPerfSetIrqRegs(UINTPTR pc, UINTPTR fp);
extern VOID OsPerfUpdateEventCount(Event *event, UINT32 value);
extern VOID OsPerfHandleOverFlow(Event *event, PerfRegs *regs);
extern VOID OsPerfHwReadAllCounter(PerfEvent *events);
extern VOID OsPerfTaskCreate(LosTaskCB *taskCB);
extern VOID OsPerfTaskDelete(LosTaskCB *taskCB);
extern VOID OsPerfTaskSwitch(LosTaskCB *runTask, LosTaskCB *newTask);
extern VOID *OsPerfIrqEnter(UINT32 hwiNum);
extern VOID OsPerfIrqExit(UINT32 hwiNum, VOID *state);
extern PerfEvent *OsPerfIrqEventsGet(UINT32 cpuId);
extern PerfEvent *OsPerfTaskEventsGet(UINT32 taskId);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_PERF_PRI_H */
