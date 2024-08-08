/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#ifdef SUPPORT_AUDIO_LIEYIN_TOOL
#include <stdint.h>
#include "common_def.h"
#include <string.h>
#include <strings.h>
#include "securec.h"
#include "debug_print.h"
#include "audio_crc16.h"
#include "audio_lieyin.h"

#define LIEYIN_ADAPT_AT_CMD_R               "CRXXAT^AUDIO"
#define LIEYIN_ADAPT_AT_CMD_L               "CLXXAT^AUDIO"
#define LIEYIN_MODULE_NAME                  "cc0402"
#define LIEYIN_CHAR_MAX_LENGTH              16
#define BYTE_MASK                           0xff
#define BIT_SHIFT_8                         8
#define BIT_SHIFT_16                        16
#define BIT_SHIFT_24                        24

typedef uint32_t (*lieyin_func_callback)(uint8_t *para, uint32_t len);
typedef struct {
    char *func_name;
    lieyin_func_callback callback;
    bool is_bin;
} lieyin_cmd_process_t;

typedef struct {
    uint8_t tag;
    uint8_t resp; // for ux handle flag
    uint8_t src;
    uint8_t dst;
    uint16_t len;
} pkt_header_t;

typedef enum {
    CMD_WRITE_DATA_OK = 0,
    CMD_WRITE_DATA_CRC_ERROR,
    CMD_WRITE_DATA_WRITE_FAIL,
    CMD_MALLOC_MEM_FAIL,
} audio_cmd_ret_code_t;

static uint32_t cc0402_write_bin(uint8_t *para, uint32_t len);
static uint32_t cc0402_read_bin(uint8_t *para, uint32_t len);

static const lieyin_cmd_process_t g_cc0402_cmds[] = {
    {"write_bin", cc0402_write_bin, true},
    {"read_bin", cc0402_read_bin, true},
};

static void audio_cmd_feedback(uint8_t *para, uint16_t len)
{
    if (para == NULL) {
        PRINT("[AudioCmdBinFeedback] para is NULL");
        return;
    }

    uint16_t data_crc = checksum_crc16(0, para, len);
    char feedback_lead[] = "ASK";
    uint16_t send_len = len + sizeof(pkt_header_t) + strlen(feedback_lead) + sizeof(data_crc);
    uint8_t *send_buf;

    send_buf = (uint8_t *)malloc(send_len);
    if (send_buf == NULL) {
        PRINT("[AudioCmdBinFeedback] malloc is failed");
        return;
    }

    pkt_header_t *header = (pkt_header_t *)send_buf;
    uint8_t *ptr_send = send_buf;

    header->resp = 1;
    header->tag = 0;
    header->dst = 'R';     // right earphone
    header->src = 'C';     // PC End
    header->len = len + strlen(feedback_lead);

    ptr_send += sizeof(pkt_header_t);
    if (memcpy_s(ptr_send, strlen(feedback_lead), feedback_lead, strlen(feedback_lead)) != EOK) {
        PRINT("[AudioCmdBinFeedback] memcpy_s is failed");
        free(send_buf);
        return;
    }

    ptr_send += strlen(feedback_lead);
    if (memcpy_s(ptr_send, len, para, len) != EOK) {
        PRINT("[AudioCmdBinFeedback] memcpy_s is failed");
        free(send_buf);
        return;
    }

    ptr_send += len;
    if (memcpy_s(ptr_send, sizeof(data_crc), (uint8_t *)&data_crc, sizeof(data_crc)) != EOK) {
        PRINT("[AudioCmdBinFeedback] memcpy_s is failed");
        free(send_buf);
        return;
    }

    uapi_uart_write(TEST_SUITE_UART_BUS, send_buf, send_len, 0);
    uapi_uart_write(TEST_SUITE_UART_BUS, '\r', 1, 0);
    uapi_uart_write(TEST_SUITE_UART_BUS, '\n', 1, 0);
}

static uint32_t cc0402_write_bin(uint8_t *para, uint32_t len)
{
    if (para == NULL || len < (sizeof(uint32_t) + sizeof(uint8_t))) {
        PRINT("err param len=%d\r\nFAILED", len);
        return 0;
    }

    uint8_t *ptr = para;
    uint32_t address;
    uint8_t *data = ptr + sizeof(address);
    uint16_t data_len = 0;
    uint8_t feedback_ret = CMD_WRITE_DATA_OK;

    data_len = len - sizeof(address);

    if (data_len == 0 || data_len > 450) { /* 450: short cmd max data len */
        PRINT("err param len=%d, data_len=%u\r\nFAILED", len, data_len);
        return 0;
    }

    address = ((*(ptr + 0x3)) << BIT_SHIFT_24) + ((*(ptr + 0x2)) << BIT_SHIFT_16) +
        ((*(ptr + 1)) << BIT_SHIFT_8) + (*ptr);

    /* to write bin */
    /* send feedback */
    audio_cmd_feedback(&feedback_ret, sizeof(feedback_ret));
    return 1;
}

static uint32_t cc0402_read_bin(uint8_t *para, uint32_t len)
{
    if (para == NULL || len < (sizeof(uint32_t) + sizeof(uint16_t))) {
        PRINT("err param len=%d\r\nFAILED", len);
        return 0;
    }

    uint8_t *ptr = para;
    uint32_t address;
    uint16_t data_len;
    uint8_t feedback_ret = CMD_WRITE_DATA_OK;
    uint8_t *send_buf = NULL;

    address = ((*(ptr + 0x3)) << BIT_SHIFT_24) + ((*(ptr + 0x2)) << BIT_SHIFT_16) +
        ((*(ptr + 1)) << BIT_SHIFT_8) + (*ptr);
    ptr += sizeof(address);
    data_len = ((*(ptr + 1)) << BIT_SHIFT_8) + (*ptr);

    send_buf = malloc(data_len + sizeof(feedback_ret));
    if (send_buf == NULL) {
        PRINT("[Cc0401ReadBin]malloc fail!\r\n");
        return 0;
    }

    /* read to send_buf + sizeof(feedback_ret), mocked */
    for (uint32_t i = 0; i < data_len; i++) {
        send_buf[sizeof(feedback_ret) + i] = (uint8_t)i;
    }

    send_buf[0] = feedback_ret;
    audio_cmd_feedback(send_buf, data_len + sizeof(feedback_ret));

    return 1;
}

static uint8_t lieyin_cmd_argument_get(char *argument, const char *string, char split_char)
{
    uint8_t i = 0;
    uint16_t index = 0;
    while (string[index] != 0) {
        if (string[index] == split_char || (i == LIEYIN_CHAR_MAX_LENGTH - 1)) {
            break;  /* we have found the first element */
        }
        argument[index] = string[index];
        i++;
        index++;
    }
    argument[index] = '\0';
    return i;
}

static int32_t lieyin_command_proc_bin(uint8_t *para, uint32_t length, lieyin_func_callback callback)
{
    uint8_t *ptr = para;
    uint8_t *cmd_data;
    errno_t ret;
    uint16_t data_len, data_crc, crc_check;

    data_len = ((*(ptr + 1)) << BIT_SHIFT_8) + (*ptr);
    ptr += sizeof(data_len);

    cmd_data = (uint8_t *)malloc(data_len);
    if (cmd_data == NULL) {
        PRINT("[AudioCmdProcBin] malloc fail! cmdDatalen:%d", data_len);
        return -1;
    }

    ret = memcpy_s(cmd_data, data_len, ptr, data_len);
    if (ret != EOK) {
        free(cmd_data);
        return -1;
    }

    ptr += data_len;
    data_crc = ((*(ptr + 1)) << BIT_SHIFT_8) + (*ptr) ;

    crc_check = checksum_crc16(0, (uint8_t *)cmd_data, data_len);
#ifdef AUDIO_LIEYIN_DEBUG
    PRINT("[AudioCmdProcBin] cmdDatalen:%d data_crc:0x%x crc_check:0x%x\r\n", data_len, data_crc, crc_check);
#endif
    if (crc_check != data_crc) {
        PRINT("[AudioCmdProcBin] crc check is failed, rec_len=0x%x, len_val=0x%x, cal_crc=0x%x, rec_crc=0x%x",
            length - sizeof(data_len) - sizeof(data_crc), data_len, crc_check, data_crc);
        free(cmd_data);
        return -1;
    }

    if (callback != NULL) {
        callback(cmd_data, data_len);
    }

    free(cmd_data);
    return 0;
}

void audio_lieyin_command_execute(char *module, char *func, char *para, uint32_t length)
{
    if (strcasecmp(module, LIEYIN_MODULE_NAME) != 0) {
        return;
    }

    for (uint32_t i = 0; i < (sizeof(g_cc0402_cmds) / sizeof(lieyin_cmd_process_t)); i++) {
        if (strcasecmp(func, g_cc0402_cmds[i].func_name) == 0) {
            if (g_cc0402_cmds[i].is_bin) {
                (void)lieyin_command_proc_bin((uint8_t *)para, length, g_cc0402_cmds[i].callback);
            } else {
                g_cc0402_cmds[i].callback((uint8_t *)para, length);
            }
        }
    }
}

void audio_lieyin_command_receive(char *buf, uint32_t length)
{
    char first_argument[LIEYIN_CHAR_MAX_LENGTH] = { 0 };
    char module[LIEYIN_CHAR_MAX_LENGTH] = { 0 };
    char func_name[LIEYIN_CHAR_MAX_LENGTH] = { 0 };
    uint8_t argument_length;
    char *current_cmd = buf;
    uint32_t remain_len = length;

    argument_length = lieyin_cmd_argument_get(first_argument, current_cmd, '=');
#ifdef AUDIO_LIEYIN_DEBUG
    PRINT("= argument:%d\r\n", argument_length);
#endif
    /* if it is empty just return */
    if (argument_length == 0 || (argument_length + 1 >= remain_len)) {
        return;  /* EARLY RETURN */
    }
    current_cmd += argument_length + 1;
    remain_len -= argument_length + 1;

    argument_length = lieyin_cmd_argument_get(module, current_cmd, ',');
#ifdef AUDIO_LIEYIN_DEBUG
    PRINT(",1 argument:%d\r\n", argument_length);
#endif
    if (argument_length == 0 || (argument_length + 1 >= remain_len)) {
        return;  /* EARLY RETURN */
    }
    current_cmd += argument_length + 1;
    remain_len -= argument_length + 1;

    argument_length = lieyin_cmd_argument_get(func_name, current_cmd, ',');
#ifdef AUDIO_LIEYIN_DEBUG
    PRINT(",2 argument:%d\r\n", argument_length);
#endif
    if (argument_length == 0 || (argument_length + 1 > remain_len)) {
        return;  /* EARLY RETURN */
    }
    current_cmd += argument_length + 1;
    remain_len -= argument_length + 1;
#ifdef AUDIO_LIEYIN_DEBUG
    PRINT("param len:%d\r\n", remain_len);
#endif

    if ((strcasecmp(first_argument, LIEYIN_ADAPT_AT_CMD_R) == 0) ||
        (strcasecmp(first_argument, LIEYIN_ADAPT_AT_CMD_L) == 0)) {
        audio_lieyin_command_execute(module, func_name, current_cmd, remain_len);
    }
}
#endif