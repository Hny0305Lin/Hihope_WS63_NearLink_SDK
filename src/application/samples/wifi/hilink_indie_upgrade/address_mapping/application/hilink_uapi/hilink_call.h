  /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: hilink function mapping \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */
#ifndef HILINK_CALL_H
#define HILINK_CALL_H

#include "func_call_list.h"
#include "app_function_mapping.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HILINK_TBL get_hilink_tbl()
#define hilink_call0(nr, t) \
    ((t (*)(void))(HILINK_TBL[nr]))()
#define hilink_call1(nr, t, t1, p1) \
    ((t (*)(t1))(HILINK_TBL[nr]))(p1)
#define hilink_call2(nr, t, t1, p1, t2, p2) \
    ((t (*)(t1, t2))(HILINK_TBL[nr]))(p1, p2)
#define hilink_call3(nr, t, t1, p1, t2, p2, t3, p3) \
    ((t (*)(t1, t2, t3))(HILINK_TBL[nr]))(p1, p2, p3)
#define hilink_call4(nr, t, t1, p1, t2, p2, t3, p3, t4, p4) \
    ((t (*)(t1, t2, t3, t4))(HILINK_TBL[nr]))(p1, p2, p3, p4)
#define hilink_call5(nr, t, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5) \
    ((t (*)(t1, t2, t3, t4, t5))(HILINK_TBL[nr]))(p1, p2, p3, p4, p5)
#define hilink_call6(nr, t, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5, t6, p6) \
    ((t (*)(t1, t2, t3, t4, t5, t6))(HILINK_TBL[nr]))(p1, p2, p3, p4, p5, p6)
#define hilink_call7(nr, t, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5, t6, p6, t7, p7) \
    ((t (*)(t1, t2, t3, t4, t5, t6, t7))(HILINK_TBL[nr]))(p1, p2, p3, p4, p5, p6, p7)

#ifdef __cplusplus
}
#endif
#endif
