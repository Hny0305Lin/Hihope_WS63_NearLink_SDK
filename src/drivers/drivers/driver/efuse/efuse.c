/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides efuse driver source \n
 *
 * History: \n
 * 2022-10-20, Create file. \n
 */

#include "efuse.h"
#include <stdio.h>
#include "securec.h"
#include "soc_osal.h"
#include "errcode.h"
#include "tcxo.h"
#include "hal_efuse.h"
#include "efuse_porting.h"

#define EFUSE_CHAR_BIT_WIDE              8
#define EFUSE_CALC_CRC_MAX_LEN          32

errcode_t uapi_efuse_init(void)
{
    errcode_t ret = ERRCODE_FAIL;
    efuse_port_register_hal_funcs();
    hal_efuse_funcs_t *hal_funcs = hal_efuse_get_funcs();
    ret = hal_funcs->init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
}

/*
 * Removes power from the efuse system
 */
errcode_t uapi_efuse_deinit(void)
{
    hal_efuse_funcs_t *hal_funcs = hal_efuse_get_funcs();
    if (hal_funcs != NULL) {
        hal_funcs->deinit();
    }
    efuse_port_unregister_hal_funcs();
    return ERRCODE_SUCC;
}

#ifdef EFUSE_BIT_OPERATION
/*
 * Reads a bit from EFUSE memory
 */
errcode_t uapi_efuse_read_bit(uint8_t *value, uint32_t byte_number, uint8_t bit_pos)
{
    uint8_t read_value = 0;
    if ((byte_number >= EFUSE_MAX_BYTES) || (value == NULL) || (bit_pos >= EFUSE_MAX_BIT_POS)) {
        return ERRCODE_FAIL;
    }
    hal_efuse_funcs_t *hal_funcs = hal_efuse_get_funcs();
    uint32_t irq_sts = osal_irq_lock();
    if (hal_funcs->read_byte(byte_number, &read_value) == ERRCODE_SUCC) {
        *value = (read_value >> bit_pos) & 1U;
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }
    osal_irq_restore(irq_sts);
    return ERRCODE_FAIL;
}
#endif

/*
 * Reads multiple bytes from EFUSE memory, into the provided buffer
 */
errcode_t uapi_efuse_read_buffer(uint8_t *buffer, uint32_t byte_number, uint16_t length)
{
    if ((length == 0) || (byte_number >= EFUSE_MAX_BYTES) ||
    ((byte_number + length) > EFUSE_MAX_BYTES) || (buffer == NULL)) {
        return ERRCODE_FAIL;
    }
    hal_efuse_funcs_t *hal_funcs = hal_efuse_get_funcs();
    uint32_t irq_sts = osal_irq_lock();
    for (uint32_t i = 0; i < length; i++) {
        if (hal_funcs->read_byte(byte_number + i, &buffer[i]) != ERRCODE_SUCC) {
            osal_irq_restore(irq_sts);
            return ERRCODE_FAIL;
        }
    }
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

#ifdef EFUSE_BIT_OPERATION
/*
 * Writes a bit to EFUSE memory
 */
errcode_t uapi_efuse_write_bit(uint32_t byte_number, uint8_t bit_pos)
{
    if (byte_number >= EFUSE_MAX_BYTES || bit_pos >= EFUSE_MAX_BIT_POS) {
        return ERRCODE_FAIL;
    }

    uint8_t write_value = 0;
    hal_efuse_funcs_t *hal_funcs = hal_efuse_get_funcs();
    uint32_t irq_sts = osal_irq_lock();
    if (hal_funcs->read_byte(byte_number, &write_value) != ERRCODE_SUCC) {
        osal_irq_restore(irq_sts);
        return ERRCODE_FAIL;
    }

    if ((write_value & bit(bit_pos)) != 0) {
        osal_irq_restore(irq_sts);
        return ERRCODE_INVALID_PARAM;
    }

    hal_efuse_region_t region = hal_efuse_get_region(byte_number);
    if (hal_funcs->write_op(byte_number, (uint8_t)bit(bit_pos), region) == ERRCODE_SUCC) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }

    osal_irq_restore(irq_sts);
    return ERRCODE_FAIL;
}

errcode_t uapi_efuse_write_bit_with_flag(uint32_t byte_number, uint8_t bit_pos, uint32_t flag)
{
    if (flag != EFUSE_WRITE_PROTECT_FLAG) {
        return ERRCODE_EFUSE_INVALID_PARAM;
    }
    if (byte_number >= EFUSE_MAX_BYTES || bit_pos >= EFUSE_MAX_BIT_POS) {
        return ERRCODE_FAIL;
    }

    uint8_t write_value = 0;
    hal_efuse_funcs_t *hal_funcs = hal_efuse_get_funcs();
    uint32_t irq_sts = osal_irq_lock();
    if (hal_funcs->read_byte(byte_number, &write_value) != ERRCODE_SUCC) {
        osal_irq_restore(irq_sts);
        return ERRCODE_FAIL;
    }

    if ((write_value & bit(bit_pos)) != 0) {
        osal_irq_restore(irq_sts);
        return ERRCODE_INVALID_PARAM;
    }

    hal_efuse_region_t region = hal_efuse_get_region(byte_number);
    if (hal_funcs->write_op(byte_number, (uint8_t)bit(bit_pos), region) == ERRCODE_SUCC) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }

    osal_irq_restore(irq_sts);
    return ERRCODE_FAIL;
}
#endif

static errcode_t efuse_write_param_check(uint32_t byte_number, const uint8_t *buffer, uint16_t length)
{
    if ((length == 0) || (byte_number >= EFUSE_MAX_BYTES) ||
    ((byte_number + length) > EFUSE_MAX_BYTES) || (buffer == NULL)) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/*
 * Writes multiple bytes to EFUSE memory, from the provided buffer
 */
errcode_t uapi_efuse_write_buffer(uint32_t byte_number, const uint8_t *buffer, uint16_t length)
{
    errcode_t ret;

    ret = efuse_write_param_check(byte_number, buffer, length);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_EFUSE_INVALID_PARAM;
    }
    hal_efuse_funcs_t *hal_funcs = hal_efuse_get_funcs();
    uint32_t irq_sts = osal_irq_lock();
    if (hal_funcs->write_buffer_op(byte_number, buffer, length) != ERRCODE_SUCC) {
        osal_irq_restore(irq_sts);
        return ERRCODE_FAIL;
    }

    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

errcode_t uapi_efuse_write_buffer_with_flag(uint32_t byte_number, const uint8_t *buffer, uint16_t length, uint32_t flag)
{
    if (flag != EFUSE_WRITE_PROTECT_FLAG) {
        return ERRCODE_EFUSE_INVALID_PARAM;
    }
    errcode_t ret;

    ret = efuse_write_param_check(byte_number, buffer, length);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_EFUSE_INVALID_PARAM;
    }
    hal_efuse_funcs_t *hal_funcs = hal_efuse_get_funcs();
    uint32_t irq_sts = osal_irq_lock();
    if (hal_funcs->write_buffer_op(byte_number, buffer, length) != ERRCODE_SUCC) {
        osal_irq_restore(irq_sts);
        return ERRCODE_FAIL;
    }

    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

errcode_t uapi_efuse_get_die_id(uint8_t *buffer, uint16_t length)
{
    if ((length == 0) || (buffer == NULL)) {
        return ERRCODE_FAIL;
    }
    hal_efuse_funcs_t *hal_funcs = hal_efuse_get_funcs();
    errcode_t ret = hal_funcs->get_die_id(buffer, length);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_efuse_get_chip_id(uint8_t *buffer, uint16_t length)
{
    if ((length == 0) || (buffer == NULL)) {
        return ERRCODE_FAIL;
    }
    hal_efuse_funcs_t *hal_funcs = hal_efuse_get_funcs();
    errcode_t ret = hal_funcs->get_chip_id(buffer, length);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
}

static uint8_t efuse_count_zero_in_byte(uint8_t value)
{
    uint8_t num_zeroes = 0;
    uint8_t count = value;
    for (uint8_t i = 0; i < EFUSE_CHAR_BIT_WIDE; i++, count = count >> 1) {
        if ((count & 1) == 0) {
            num_zeroes++;
        }
    }
    return num_zeroes;
}

/*
 * Calculate OPT zero-counting CRC for a block upto 32 bytes (256 bits)
 */
errcode_t uapi_efuse_calc_crc(const uint8_t *buffer, uint8_t length, uint8_t *crc)
{
    int i;
    errcode_t ret_value = ERRCODE_SUCC;

    if ((buffer == NULL) || (crc == NULL) || (length > EFUSE_CALC_CRC_MAX_LEN)) {
        ret_value = ERRCODE_EFUSE_INVALID_PARAM;
    } else {
        *crc = 0;
        for (i = 0; i < length; i++) {
            *crc += efuse_count_zero_in_byte(buffer[i]);
        }
    }

    return ret_value;
}

errcode_t uapi_soc_read_id(uint8_t *id, uint16_t id_length)
{
    return uapi_efuse_get_die_id(id, id_length);
}

#ifndef CUSTOMER_RSVD_EFUSE_START_BIT
#define CUSTOMER_RSVD_EFUSE_START_BIT 0
#define CUSTOMER_RSVD_EFUSE_BIT_LEN 0
#endif

errcode_t uapi_efuse_user_read_buffer(uint32_t offset, uint8_t *buffer, uint16_t length)
{
    if (((length + offset) * EFUSE_CHAR_BIT_WIDE) > CUSTOMER_RSVD_EFUSE_BIT_LEN) {
        return ERRCODE_EFUSE_INVALID_PARAM;
    }
    return uapi_efuse_read_buffer(buffer, (offset + CUSTOMER_RSVD_EFUSE_START_BIT / EFUSE_CHAR_BIT_WIDE), length);
}


errcode_t uapi_efuse_user_write_buffer(uint32_t offset, const uint8_t *buffer, uint16_t length)
{
    if (((length + offset) * EFUSE_CHAR_BIT_WIDE) > CUSTOMER_RSVD_EFUSE_BIT_LEN) {
        return ERRCODE_EFUSE_INVALID_PARAM;
    }
    return uapi_efuse_write_buffer((offset + CUSTOMER_RSVD_EFUSE_START_BIT / EFUSE_CHAR_BIT_WIDE), buffer, length);
}

#ifdef EFUSE_BIT_OPERATION
errcode_t uapi_efuse_user_write_bit(uint32_t byte_offset, uint8_t bit_pos)
{
    if ((byte_offset * EFUSE_CHAR_BIT_WIDE + bit_pos) > CUSTOMER_RSVD_EFUSE_BIT_LEN) {
        return ERRCODE_EFUSE_INVALID_PARAM;
    }
    return uapi_efuse_write_bit((byte_offset + CUSTOMER_RSVD_EFUSE_START_BIT / EFUSE_CHAR_BIT_WIDE), bit_pos);
}


errcode_t uapi_efuse_user_read_bit(uint32_t byte_offset, uint8_t bit_pos, uint8_t *value)
{
    if ((byte_offset * EFUSE_CHAR_BIT_WIDE + bit_pos) > CUSTOMER_RSVD_EFUSE_BIT_LEN) {
        return ERRCODE_EFUSE_INVALID_PARAM;
    }
    return uapi_efuse_read_bit(value, (byte_offset + CUSTOMER_RSVD_EFUSE_START_BIT / EFUSE_CHAR_BIT_WIDE), bit_pos);
}
#endif