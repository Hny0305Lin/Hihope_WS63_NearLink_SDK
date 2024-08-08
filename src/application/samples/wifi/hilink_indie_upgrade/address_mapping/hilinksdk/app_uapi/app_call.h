/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Function invoking macros on the application side. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#ifndef APP_CALL_H
#define APP_CALL_H

#include "func_call_list.h"
#include "hilink_function_mapping.h"

#ifdef __cplusplus
extern "C" {
#endif

#define APP_TBL get_app_tbl()
#define app_call0(nr, t) \
    ((t (*)(void))(APP_TBL[nr]))()
#define app_call1(nr, t, t1, p1) \
    ((t (*)(t1))(APP_TBL[nr]))(p1)
#define app_call2(nr, t, t1, p1, t2, p2) \
    ((t (*)(t1, t2))(APP_TBL[nr]))(p1, p2)
#define app_call3(nr, t, t1, p1, t2, p2, t3, p3) \
    ((t (*)(t1, t2, t3))(APP_TBL[nr]))(p1, p2, p3)
#define app_call4(nr, t, t1, p1, t2, p2, t3, p3, t4, p4) \
    ((t (*)(t1, t2, t3, t4))(APP_TBL[nr]))(p1, p2, p3, p4)
#define app_call5(nr, t, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5) \
    ((t (*)(t1, t2, t3, t4, t5))(APP_TBL[nr]))(p1, p2, p3, p4, p5)
#define app_call6(nr, t, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5, t6, p6) \
    ((t (*)(t1, t2, t3, t4, t5, t6))(APP_TBL[nr]))(p1, p2, p3, p4, p5, p6)
#define app_call7(nr, t, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5, t6, p6, t7, p7) \
    ((t (*)(t1, t2, t3, t4, t5, t6, t7))(APP_TBL[nr]))(p1, p2, p3, p4, p5, p6, p7)

#ifdef __cplusplus
}
#endif
#endif
