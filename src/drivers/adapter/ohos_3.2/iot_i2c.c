/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides iot_i2c driver source \n
 *
 * History: \n
 * 2023-10-18， Create file. \n
 */
#include "iot_errno.h"
#include "i2c.h"
#include "iot_i2c.h"

unsigned int IoTI2cInit(unsigned int id, unsigned int baudrate)
{
    uint8_t hscode = 0x0;
#if defined(CONFIG_I2C_SUPPORT_DMA)
    uapi_dma_init();
    uapi_dma_open();
#endif
    if (uapi_i2c_master_init((i2c_bus_t)id, (uint32_t)baudrate, hscode) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }
#if defined(CONFIG_I2C_SUPPORT_INT)
    uapi_i2c_set_irq_mode((i2c_bus_t)id, 1);
#endif
    return IOT_SUCCESS;
}

unsigned int IoTI2cDeinit(unsigned int id)
{
    if (uapi_i2c_deinit((i2c_bus_t)id) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}

unsigned int IoTI2cWrite(unsigned int id, unsigned short deviceAddr, const unsigned char *data, unsigned int dataLen)
{
    if (data == NULL) {
        return IOT_FAILURE;
    }
    i2c_data_t datas = { 0 };
    datas.send_buf = (uint8_t *)data;
    datas.send_len = (uint32_t)dataLen;

    if (uapi_i2c_master_write((i2c_bus_t)id, (uint16_t)deviceAddr, &datas) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}

unsigned int IoTI2cRead(unsigned int id, unsigned short deviceAddr, unsigned char *data, unsigned int dataLen)
{
    if (data == NULL) {
        return IOT_FAILURE;
    }
    i2c_data_t datas = { 0 };
    datas.receive_buf = (uint8_t *)data;
    datas.receive_len = (uint32_t)dataLen;

    if (uapi_i2c_master_read((i2c_bus_t)id, (uint16_t)deviceAddr, &datas) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}

unsigned int IoTI2cSetBaudrate(unsigned int id, unsigned int baudrate)
{
    if (uapi_i2c_set_baudrate((i2c_bus_t)id, (uint32_t)baudrate) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}