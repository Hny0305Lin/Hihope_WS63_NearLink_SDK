/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag oam log
 * This file should be changed only infrequently and with great care.
 */
#include "diag_oam_log.h"
#include <stdarg.h>
#include "diag.h"

#define OAM_LOG_LEVEL_MASK              0x07
#define OAM_LOG_MOD_ID_MASK             0xF
#define OAM_LOG_MOD_ID_OFFSET           27
#define OAM_LOG_PARAM_MAX_NUM           10

STATIC errcode_t oam_log_msg_print(uint32_t msg_id, uint32_t mod_id, uint8_t *buf, uint32_t size)
{
    uint32_t level = msg_id & OAM_LOG_LEVEL_MASK;
    /* does't check if the buffer is null. because when param count is 0,the buffer is null. */
    return uapi_diag_report_sys_msg(mod_id, msg_id, buf, (uint16_t)size, (uint8_t)level);
}

errcode_t oam_log_print0_press_prv(uint32_t msg_id, uint32_t mod_id)
{
    return oam_log_msg_print(msg_id, mod_id, NULL, 0);
}

errcode_t oam_log_print1_press_prv(uint32_t msg_id, uint32_t mod_id, uint32_t param_1)
{
    zdiag_log_msg1_t msg;
    msg.data0 = param_1;
    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)&msg, sizeof(zdiag_log_msg1_t));
}

errcode_t oam_log_print2_press_prv(uint32_t msg_id, uint32_t mod_id, uint32_t param_1, uint32_t param_2)
{
    zdiag_log_msg2_t msg;
    msg.data0 = param_1;
    msg.data1 = param_2;
    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)&msg, sizeof(zdiag_log_msg2_t));
}

errcode_t oam_log_print3_press_prv(uint32_t msg_id, uint32_t mod_id, zdiag_log_msg3_t *olm)
{
    zdiag_log_msg3_t msg;
    msg.data0 = olm->data0;
    msg.data1 = olm->data1;
    msg.data2 = olm->data2;
    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)&msg, sizeof(zdiag_log_msg3_t));
}

errcode_t oam_log_print4_press_prv(uint32_t msg_id, uint32_t mod_id, zdiag_log_msg4_t *olm)
{
    zdiag_log_msg4_t msg;
    msg.data0 = olm->data0;
    msg.data1 = olm->data1;
    msg.data2 = olm->data2;
    msg.data3 = olm->data3;
    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)&msg, sizeof(zdiag_log_msg4_t));
}

errcode_t oam_log_print_alterable_press_prv(uint32_t msg_id, uint32_t mod_id, uint32_t param_num, ...)
{
    uint32_t log_oam_entry[OAM_LOG_PARAM_MAX_NUM];
    va_list args;
    uint32_t param_count;
    uint32_t i;

    param_count = ((param_num > OAM_LOG_PARAM_MAX_NUM) ? (OAM_LOG_PARAM_MAX_NUM) : param_num);

    va_start(args, param_num);
    for (i = 0; i < param_count; i++) {
        log_oam_entry[i] = (uint32_t)va_arg(args, uint32_t);
    }
    va_end(args);

    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)log_oam_entry, param_count * sizeof(uint32_t));
}

errcode_t oam_log_print_buff_press_prv(uint32_t msg_id, uint32_t mod_id, uint8_t *data, uint32_t data_size)
{
    return oam_log_msg_print(msg_id, mod_id, data, data_size);
}

errcode_t oam_log_print0_press(uint32_t msg_id)
{
    uint32_t mod_id = (uint32_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    return oam_log_msg_print(msg_id, mod_id, NULL, 0);
}

errcode_t oam_log_print1_press(uint32_t msg_id, uint32_t param_1)
{
    zdiag_log_msg1_t msg;
    uint32_t mod_id = (uint32_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    msg.data0 = param_1;
    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)&msg, sizeof(zdiag_log_msg1_t));
}

errcode_t oam_log_print2_press(uint32_t msg_id, uint32_t param_1, uint32_t param_2)
{
    zdiag_log_msg2_t msg;
    uint32_t mod_id = (uint32_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    msg.data0 = param_1;
    msg.data1 = param_2;
    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)&msg, sizeof(zdiag_log_msg2_t));
}

errcode_t oam_log_print3_press(uint32_t msg_id, zdiag_log_msg3_t *olm)
{
    zdiag_log_msg3_t msg;
    uint32_t mod_id = (uint32_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    msg.data0 = olm->data0;
    msg.data1 = olm->data1;
    msg.data2 = olm->data2;
    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)&msg, sizeof(zdiag_log_msg3_t));
}

errcode_t oam_log_print4_press(uint32_t msg_id, zdiag_log_msg4_t *olm)
{
    zdiag_log_msg4_t msg;
    uint32_t mod_id = (uint32_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    msg.data0 = olm->data0;
    msg.data1 = olm->data1;
    msg.data2 = olm->data2;
    msg.data3 = olm->data3;
    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)&msg, sizeof(zdiag_log_msg4_t));
}

errcode_t oam_log_print_alterable_press(uint32_t msg_id, uint32_t param_num, ...)
{
    uint32_t mod_id = (uint32_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    uint32_t log_oam_entry[OAM_LOG_PARAM_MAX_NUM];
    va_list args;
    uint32_t param_count;
    uint32_t i;

    param_count = ((param_num > OAM_LOG_PARAM_MAX_NUM) ? (OAM_LOG_PARAM_MAX_NUM) : param_num);

    va_start(args, param_num);
    for (i = 0; i < param_count; i++) {
        log_oam_entry[i] = (uint32_t)va_arg(args, uint32_t);
    }
    va_end(args);

    return oam_log_msg_print(msg_id, mod_id, (uint8_t *)log_oam_entry, param_count * sizeof(uint32_t));
}

errcode_t oam_log_print_buff_press(uint32_t msg_id, uint8_t *data, uint32_t data_size)
{
    uint32_t mod_id = (uint32_t)((msg_id >> OAM_LOG_MOD_ID_OFFSET) & OAM_LOG_MOD_ID_MASK);
    return oam_log_msg_print(msg_id, mod_id, data, data_size);
}

