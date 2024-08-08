/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides at channel source \n
 */

#include <stdbool.h>
#include "at_base.h"
#include "at_msg.h"
#include "at_channel.h"

#define AT_FLAG_LENGTH 2

typedef struct {
    at_channel_state_t state;
    at_write_func_t write_func;
    uint32_t len;
    uint8_t* data;
} at_channel_t;

static at_channel_t g_channel_entry[AT_MAX_PORT_NUMBER] = {0};

errcode_t uapi_at_channel_write_register(at_channel_id_t id, at_write_func_t func)
{
    if (id >= AT_MAX_PORT_NUMBER || func == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    g_channel_entry[id].write_func = func;
    return ERRCODE_SUCC;
}

void at_channel_check_and_enable(void)
{
    for (uint16_t id = 0; id < AT_MAX_PORT_NUMBER; id++) {
        if (g_channel_entry[id].state != AT_CHANNEL_UNINITIALIZED) {
            continue;
        }

        if (g_channel_entry[id].write_func == NULL) {
            continue;
        }

        g_channel_entry[id].state = AT_CHANNEL_FREE;
    }
}

at_write_func_t at_channel_get_write_func(at_channel_id_t id)
{
    if (g_channel_entry[id].state == AT_CHANNEL_UNINITIALIZED) {
        return NULL;
    }
    return g_channel_entry[id].write_func;
}

uint8_t* at_channel_get_data(at_channel_id_t id)
{
    return g_channel_entry[id].data;
}

void at_channel_set_state(at_channel_id_t id, at_channel_state_t state)
{
    g_channel_entry[id].state = state;
}

void at_channel_data_reset(at_channel_id_t id)
{
    if (g_channel_entry[id].data != NULL) {
        at_free(g_channel_entry[id].data);
        g_channel_entry[id].data = NULL;
    }
    g_channel_entry[id].len = 0;
    g_channel_entry[id].state = AT_CHANNEL_FREE;
}

void at_channel_data_wait_interactivity(at_channel_id_t id)
{
    g_channel_entry[id].state = AT_CHANNEL_INTERACTIVITY;
}

static bool at_is_channel_block(at_channel_id_t id)
{
    if (g_channel_entry[id].state == AT_CHANNEL_BLOCK ||
        g_channel_entry[id].state == AT_CHANNEL_UNINITIALIZED) {
        return true;
    }
    return false;
}

static bool at_channel_is_data_integrated(at_channel_id_t id)
{
    for (uint32_t index = 1; index < g_channel_entry[id].len; index++) {
        if (g_channel_entry[id].data[index] == '\r' ||
            g_channel_entry[id].data[index] == '\n') {
            g_channel_entry[id].data[index] = '\0';
            return true;
        }
    }
    return false;
}

static void at_channel_del_invalid_data(at_channel_id_t id, uint32_t index)
{
    if (memmove_s(g_channel_entry[id].data, AT_CMD_MAX_LENGTH,
                  g_channel_entry[id].data + index, AT_CMD_MAX_LENGTH - index) != EOK) {
        return;
    }
    g_channel_entry[id].len -= index;
}

static bool at_channel_cmd_head_check(at_channel_id_t id)
{
    uint32_t index;
    for (index = 0; index < g_channel_entry[id].len - 1; index++) {
        if (((g_channel_entry[id].data[index] == 'a') || (g_channel_entry[id].data[index] == 'A')) &&
            ((g_channel_entry[id].data[index + 1] == 't') || (g_channel_entry[id].data[index + 1] == 'T'))) {
            at_channel_del_invalid_data(id, index);
            return true;
        }
    }

    if (g_channel_entry[id].data[index] == 'a' || g_channel_entry[id].data[index] == 'A') {
        at_channel_del_invalid_data(id, index);
    } else {
        at_channel_data_reset(id);
    }

    return false;
}

static errcode_t at_channel_send_msg(at_msg_type_t type, at_channel_id_t id)
{
    at_msg_block_t msg;
    msg.type = type;
    msg.sub_msg.cmd.channel_id = id;
    msg.sub_msg.cmd.len = (uint16_t)g_channel_entry[id].len;

    return at_msg_send(&msg);
}

static errcode_t at_channel_cmd_handle(at_channel_id_t id)
{
    errcode_t ret;

    if (at_channel_cmd_head_check(id) != true) {
        return ERRCODE_SUCC;
    }

    if (at_channel_is_data_integrated(id) != true) {
        return ERRCODE_SUCC;
    }

    at_channel_del_invalid_data(id, AT_FLAG_LENGTH);
    if (g_channel_entry[id].data[0] == '\0') {
        g_channel_entry[id].write_func(AT_RESPONSE_OK);
        return ERRCODE_SUCC;
    }

    g_channel_entry[id].state = AT_CHANNEL_BLOCK;
    ret = at_channel_send_msg(AT_CMD_MSG, id);
    if (ret != ERRCODE_SUCC) {
        at_channel_data_reset(id);
        g_channel_entry[id].state = AT_CHANNEL_FREE;
    }
    return ret;
}

static errcode_t at_channel_interactivity_handle(at_channel_id_t id)
{
    if (at_channel_is_data_integrated(id) != true) {
        return ERRCODE_SUCC;
    }

    g_channel_entry[id].state = AT_CHANNEL_BLOCK;
    return at_channel_send_msg(AT_CMD_INTERACTIVITY_MSG, id);
}

static errcode_t at_channel_data_handle(at_channel_id_t id)
{
    if (g_channel_entry[id].state == AT_CHANNEL_FREE) {
        return at_channel_cmd_handle(id);
    } else {
        return at_channel_interactivity_handle(id);
    }
}

static errcode_t at_channel_prepare(at_channel_id_t id)
{
    if (at_is_channel_block(id) == true) {
        return ERRCODE_AT_CHANNEL_BUSY;
    }

    if (g_channel_entry[id].data == NULL) {
        g_channel_entry[id].data = at_malloc(AT_CMD_MAX_LENGTH);
        if (g_channel_entry[id].data == NULL) {
            return ERRCODE_MALLOC;
        }
        memset_s(g_channel_entry[id].data, AT_CMD_MAX_LENGTH, 0, AT_CMD_MAX_LENGTH);
    }

    return ERRCODE_SUCC;
}

static errcode_t at_channel_data_add(at_channel_id_t id, uint8_t* data, uint32_t len)
{
    if (g_channel_entry[id].len + len > AT_CMD_MAX_LENGTH) {
        at_channel_data_reset(id);
        return ERRCODE_AT_CMD_TOO_LONG;
    }

    if (memcpy_s(g_channel_entry[id].data + g_channel_entry[id].len,
                 len, data, len) != AT_RET_OK) {
        at_channel_data_reset(id);
        return ERRCODE_MEMCPY;
    }
    g_channel_entry[id].len += len;

    return ERRCODE_SUCC;
}

errcode_t uapi_at_channel_data_recv(at_channel_id_t id, uint8_t* data, uint32_t len)
{
    if (id >= AT_MAX_PORT_NUMBER || data == NULL || len == 0) {
        return ERRCODE_INVALID_PARAM;
    }

    errcode_t err_ret = at_channel_prepare(id);
    if (err_ret != ERRCODE_SUCC) {
        return err_ret;
    }

    err_ret = at_channel_data_add(id, data, len);
    if (err_ret != ERRCODE_SUCC) {
        return err_ret;
    }

    return at_channel_data_handle(id);
}
