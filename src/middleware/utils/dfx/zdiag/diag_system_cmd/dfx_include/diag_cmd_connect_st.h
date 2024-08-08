/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx connect st
 * This file should be changed only infrequently and with great care.
 */

#ifndef DIAG_CMD_CONNECT_ST_H
#define DIAG_CMD_CONNECT_ST_H
#include <stdint.h>

#define IMEI_SIZE 16
typedef struct {
    uint32_t major_minor_version;
    uint32_t revision_version;
    uint32_t build_version;
} ue_soft_version_t;
typedef struct {
    uint16_t version_v;
    uint16_t version_r;
    uint16_t version_c;
    uint16_t version_b;
    uint16_t version_spc;
    uint16_t reserved[3]; /* pad 3 item */
} ue_product_ver_t;

typedef struct {
    uint32_t rc; /* result code */
    char imei[IMEI_SIZE];
    ue_soft_version_t ue_soft_version;
    ue_product_ver_t build_version;
} diag_cmd_host_connect_cnf_stru_t;

#endif