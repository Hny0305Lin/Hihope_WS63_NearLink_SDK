/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides iot_flash driver source \n
 *
 * History: \n
 * 2024-01-09， Create file. \n
 */
#include "iot_errno.h"
#include "sfc.h"
#include "iot_flash.h"

const sfc_flash_config_t sfc_cfg = {
    .read_type = FAST_READ_QUAD_OUTPUT,
    .write_type = PAGE_PROGRAM,
    .mapping_addr = 0x200000,
    .mapping_size = 0x800000,
};

unsigned int IoTFlashInit(void)
{
    return uapi_sfc_init((sfc_flash_config_t *)&sfc_cfg);
}

unsigned int IoTFlashDeinit(void)
{
    uapi_sfc_deinit();
    return IOT_SUCCESS;
}

unsigned int IoTFlashWrite(unsigned int flashOffset, unsigned int size, const unsigned char *ramData,
                           unsigned char doErase)
{
    if (doErase != 0) {
        uapi_sfc_reg_erase(flashOffset, size);
    }
    return uapi_sfc_reg_write(flashOffset, (uint8_t *)ramData, size);
}

unsigned int IoTFlashRead(unsigned int flashOffset, unsigned int size, unsigned char *ramData)
{
    return uapi_sfc_reg_read(flashOffset, ramData, size);
}

unsigned int IoTFlashErase(unsigned int flashOffset, unsigned int size)
{
    return uapi_sfc_reg_erase(flashOffset, size);
}