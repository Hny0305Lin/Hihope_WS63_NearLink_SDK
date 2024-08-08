/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: LiteOS NMI adapt header. \n
 *
 * History: \n
 * 2023-11-09, Create file. \n
 */

#ifndef NMI_ADAPT_H
#define NMI_ADAPT_H

#include "arch/exception.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

extern VOID OsClearNMI(VOID);
extern VOID OsExcInfoDisplay(UINT32 excType, const ExcContext *excBufAddr);
extern VOID irq_wdt_handler(VOID);
extern VOID do_hard_fault_handler(ExcContext *exc_buf_addr);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif