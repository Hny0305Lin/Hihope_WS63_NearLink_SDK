/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: software nv by zdiag
 */

#include "nv.h"
#include "uapi_crc.h"
#include "dfx_adapt_layer.h"
#include "nv_adapt_zdiag.h"

#define KVALUE_MAX_LENGTH 4060

typedef struct {
    uint32_t key;
} zdiag_nv_read_input_t;

typedef struct {
    uint32_t ret;
    uint32_t key;
    uint16_t crc;
    uint8_t attr;
    uint8_t pad;
    uint32_t length;
    uint8_t data[0];
} zdiag_nv_read_output_t;

typedef struct {
    uint32_t key;
    uint16_t crc;
    uint8_t attr;
    uint8_t pad;
    uint32_t length;
    uint8_t value[0];
} zdiag_nv_write_input_t;

typedef struct {
    uint32_t ret;
    uint32_t key;
} zdiag_nv_write_output_t;

errcode_t zdiag_adapt_nv_read(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_param_size);
    errcode_t res;
    zdiag_nv_read_input_t *nv_read_input = (zdiag_nv_read_input_t *)cmd_param;
    zdiag_nv_read_output_t *nv_read_output = NULL;
    uint16_t klength;
    nv_read_output = (zdiag_nv_read_output_t *)dfx_malloc(0, sizeof(zdiag_nv_read_output_t) + KVALUE_MAX_LENGTH);
    if (nv_read_output == NULL) {
        return ERRCODE_MALLOC;
    }
    nv_read_output->ret =
        (uint16_t)uapi_nv_read((uint16_t)(nv_read_input->key), KVALUE_MAX_LENGTH, &klength, nv_read_output->data);
    nv_read_output->key = (uint16_t)nv_read_input->key;
    nv_read_output->length = (uint32_t)klength;
    nv_read_output->crc = uapi_crc16(0, (uint8_t *)&(nv_read_output->length),
        nv_read_output->length + sizeof(nv_read_output->length));
    nv_read_output->attr = 0;
    nv_read_output->pad = 0;
    res = uapi_diag_report_packet(cmd_id, option, (uint8_t *)nv_read_output,
        (uint16_t)(sizeof(zdiag_nv_read_output_t) + nv_read_output->length), true);
    dfx_free(0, nv_read_output);
    return res;
}

errcode_t zdiag_adapt_nv_write(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_param_size);
    errcode_t res;
    zdiag_nv_write_output_t nv_write_output = {0};
    zdiag_nv_write_input_t *nv_write_input = (zdiag_nv_write_input_t *)cmd_param;
    /* crc校验 */
    uint16_t crc16 = uapi_crc16(0, (uint8_t *)&(nv_write_input->length),
        nv_write_input->length + sizeof(nv_write_input->length));
    if (crc16 != nv_write_input->crc) {
        nv_write_output.ret = ERRCODE_DIAG_CRC_ERROR;
    } else {
        nv_write_output.ret = uapi_nv_write((uint16_t)(nv_write_input->key), nv_write_input->value,
            (uint16_t)nv_write_input->length);
    }
    nv_write_output.key = (uint32_t)nv_write_input->key;
    res = uapi_diag_report_packet(cmd_id, option, (const uint8_t *)&nv_write_output, sizeof(nv_write_output), true);
    return res;
}