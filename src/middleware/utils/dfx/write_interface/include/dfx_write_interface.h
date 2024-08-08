/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx write interface
 * This file should be changed only infrequently and with great care.
 */

#ifndef DFX_WRITE_INTERFACE_H
#define DFX_WRITE_INTERFACE_H
#include <stdint.h>

/* dfx_data_transmit_interface.h */
typedef enum {
    DFX_DATA_TYPE_STRING,   /* 纯string数据 */
    DFX_DATA_TYPE_UAPI_LOG, /* UAPI LOG */
    DFX_DATA_DIAG_PKT,
    DFX_DATA_DIAG_PKT_CRITICAL,
} dfx_data_type_t; /* 数据类型 */

typedef struct dfx_write_data_interface {
    int32_t (*write)(void *fd, dfx_data_type_t data_type, uint8_t *data[], uint16_t len[], uint8_t cnt); /* 写数据接口 */
    void *fd;    /* 传入给write的fd */
} dfx_write_data_interface_t; /* 写数据抽象 */

#endif