/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description: Hwi Inner HeadFile
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

#ifndef _LOS_HWI_PRI_H
#define _LOS_HWI_PRI_H

#include "los_hwi.h"
#include "los_spinlock.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_KERNEL_SMP
extern SPIN_LOCK_S g_hwiSpin;
#define HWI_LOCK(state)       LOS_SpinLockSave(&g_hwiSpin, &(state))
#define HWI_UNLOCK(state)     LOS_SpinUnlockRestore(&g_hwiSpin, (state))
#else
#define HWI_LOCK(state)       LOS_SpinLockSave(NULL, &(state))
#define HWI_UNLOCK(state)     LOS_SpinUnlockRestore(NULL, (state))
#endif

#define OS_HWI_INVALID_IRQ                  0xFFFFFFFF

/**
 * This represents that the status of interrupt is not supported.
 */
#define OS_HWI_UNSUPPORTED_STATUS 0xFFFFFFFF

typedef struct tagHwiHandleForm {
    HWI_PROC_FUNC hook;         /* User registered callback function */
    union {
        HWI_ARG_T shareMode;    /* the head node of the shared interrupt: this parameter means the shared flag bit */
        HWI_ARG_T registerInfo; /* not the head node of the shared interrupt or no-shared interrupt: this parameter
                                   means address of a parameter registered by a user. */
    };
#ifdef LOSCFG_SHARED_IRQ
    struct tagHwiHandleForm *next;
#endif
    UINT32 respCount; /* Interrupt response counts */
} HwiHandleInfo;

/**
 * The interrupt status structure.
 */
typedef struct {
    UINT32       isCreated;
    UINT32       pri;        /* priority */
    UINT32       enable;
    UINT32       pending;
    UINT32       affinity;   /* each bit represents a core, 0 means that interrupt affinity is not supported */
} HwiStatus;

typedef struct {
    UINT32 (*triggerIrq)(HWI_HANDLE_T hwiNum);
    UINT32 (*clearIrq)(HWI_HANDLE_T hwiNum);
    UINT32 (*enableIrq)(HWI_HANDLE_T hwiNum);
    UINT32 (*disableIrq)(HWI_HANDLE_T hwiNum);
    UINT32 (*setIrqPriority)(HWI_HANDLE_T hwiNum, HWI_PRIOR_T priority);
    UINT32 (*getCurIrqNum)(VOID);
    CHAR *(*getIrqVersion)(VOID);
    HwiHandleInfo *(*getHandleForm)(HWI_HANDLE_T hwiNum); /* This function must be registered. */
    VOID (*handleIrq)(VOID);
#ifdef LOSCFG_KERNEL_SMP
    UINT32 (*setIrqCpuAffinity)(HWI_HANDLE_T hwiNum, UINT32 cpuMask);
    UINT32 (*sendIpi)(UINT32 target, UINT32 ipi);
#endif
#ifdef LOSCFG_DEBUG_HWI
    UINT32 (*getIrqStatus)(HWI_HANDLE_T hwiNum, HwiStatus *hwiStatus);
#endif
} HwiControllerOps;

typedef struct tagBhwork {
    struct LOS_DL_LIST entry;           /* Pointer to a doubly linked list of a work. */
    VOID *data;                         /* Input parameter of the work handling function. */
    HWI_BOTTOM_HALF_FUNC workFunc;      /* Work handling function. */
} HwiBhworkItem;

extern const HwiControllerOps *g_hwiOps;

extern UINT32 OsHwiInit(VOID);
extern size_t OsIrqNestingCntGet(VOID);
extern VOID OsIrqNestingCntSet(size_t val);
#ifdef LOSCFG_HWI_BOTTOM_HALF
extern UINT32 OsHwiBottomHalfInit(VOID);
#endif
#ifdef LOSCFG_DEBUG_HWI
extern VOID PrintHwiInfo(VOID);
extern VOID OsHwiStatusAllPrint(VOID);
extern VOID OsHwiStatusPrint(UINT32 hwiNum, VOID *devId);
extern UINT32 OsHwiStatusGet(HWI_HANDLE_T hwiNum, VOID *devID, HwiStatus *hwiStatus);
#endif
/**
 * OS Interrupt entry, called by assembly code in dispatch.S. The unified entrance of external interrupt is taken over
 * by the kernel. The handleIrq hook function MUST be registered in the interrupt controller driver layer, otherwise it
 * will not respond. eg: Used for arm(cortex-a/r)/arm64.
 */
extern VOID OsIntEntry(VOID);

STATIC INLINE HwiHandleInfo *OsGetHwiForm(UINT32 hwiNum)
{
    if ((g_hwiOps == NULL) || (g_hwiOps->getHandleForm == NULL)) {
        return NULL;
    }
    return g_hwiOps->getHandleForm(hwiNum);
}

STATIC INLINE UINT32 OsIntNumGet(VOID)
{
    if ((g_hwiOps == NULL) || (g_hwiOps->getCurIrqNum == NULL)) {
        return LOS_ERRNO_HWI_PROC_FUNC_NULL;
    }
    return g_hwiOps->getCurIrqNum();
}

STATIC INLINE BOOL OsIntIsRegisted(UINT32 num)
{
    HwiHandleInfo *hwiForm = OsGetHwiForm(num);

    if (hwiForm == NULL) {
        return false;
    }
#ifdef LOSCFG_SHARED_IRQ
    return (hwiForm->next != NULL);
#else
    return (hwiForm->hook != NULL);
#endif
}

STATIC INLINE HWI_ARG_T OsIntGetPara(UINT32 num)
{
    HwiHandleInfo *hwiForm = OsGetHwiForm(num);

    if (hwiForm == NULL) {
        return 0;
    }
    return hwiForm->registerInfo;
}

STATIC INLINE CHAR *OsIntVersionGet(VOID)
{
    if ((g_hwiOps == NULL) || (g_hwiOps->getIrqVersion == NULL)) {
        return NULL;
    }
    return g_hwiOps->getIrqVersion();
}

/**
 * If the interrupt is in the scenario where the kernel does not take over,
 * the interrupt controller driver layer realizes the general entry ISR and passes the hwiNum and
 * handleForm to this interface.
 * eg: Used for arm(cortex-m),xtensa,riscv.
 */
extern VOID OsIntHandle(UINT32 hwiNum, HwiHandleInfo *handleForm);

/**
 * When interrupting the controller driver initialization, controller-related operations need to be implemented and
 * registered;Because the function pointer is not judged in the abstraction layer, all function members defined in
 * HwiControllerOps need to be registered. If this function is not supported, you can call the LOS_Panic interface in
 * the implementation of the stub function to report an error in time.
 */
STATIC INLINE VOID OsHwiControllerReg(const HwiControllerOps *ops)
{
    g_hwiOps = ops;
}

#define HWI_IS_REGISTED(num) OsIntIsRegisted(num)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_HWI_PRI_H */
