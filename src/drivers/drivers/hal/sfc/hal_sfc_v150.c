 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides SFC HAL source \n
 *
 * History: \n
 * 2022-12-01, Create file. \n
 */
#include <stdint.h>
#include <stdbool.h>
#include "tcxo.h"
#include "securec.h"
#include "hal_sfc_v150_regs_op.h"
#include "hal_sfc.h"

#define SPI_CMD_RDSR             0x05
#define SPI_CMD_RDID             0x9F
#define SPI_CMD_WREN             0x06

#define STANDARD_SPI             0x0

#define ENABLE                   0x1
#define DISABLE                  0x0

#define READ_MODE                0x1
#define WRITE_MODE               0x0

#define FLASH_CMD_INDEX          0x0
#define FLASH_CMD_OFFSET         0x1
#define FLASH_CMD_BIT            0x2

#define FLASH_OPREATION_MAX_NUM  0x3
#define SFC_DATABUF_LENGTH       0x40
#define ADDR_SHIFT_BITS          16

#define FLASH_SIZE_D_VALUE        0xF

#define SFC_V150_MIN_SIZE        0x10000
#define FLASH_ID_MASK            0xFFFFFF

#define get_word_num(byte_num) (((byte_num) >> 2) + (((byte_num) & 0x3) != 0))

#if defined(CONFIG_SFC_SUPPORT_LPM)
#define SFC_BUS_CONFIG_REG_NUM   5
static uint32_t g_sfc_suspend_regs[SFC_BUS_CONFIG_REG_NUM] = {0};
#endif

STATIC errcode_t hal_sfc_regs_wait_ready(uint8_t wip_bit)
{
    uint32_t dead_line = 0;
    uint32_t timeout = sfc_port_get_delay_times();
    uint32_t delay = sfc_port_get_delay_once_time();
    hal_spi_opreation_t hal_opreation;
    hal_opreation.opt.cmd = SPI_CMD_RDSR;
    hal_opreation.opt.iftype = STANDARD_SPI;
    hal_opreation.data_size = 1;
    hal_opreation.dummy_byte = 0;

    /* 轮询策略修改：每次x微秒，轮询n次 */
    do {
        hal_sfc_regs_set_opt(&hal_opreation);
        hal_sfc_regs_set_opt_attr(READ_MODE, ENABLE, DISABLE);
        hal_sfc_regs_wait_config();
        uint32_t reg_val = hal_sfc_regs_get_databuf(0);
        if (((reg_val >> wip_bit) & 0x1) == 0) {
            return ERRCODE_SUCC;
        }
        uapi_tcxo_delay_us(delay);
    }while (dead_line++ < timeout);

    return ERRCODE_SFC_FLASH_TIMEOUT_WAIT_READY;
}

STATIC errcode_t hal_sfc_execute_type_cmd(uint8_t cmd_len, uint8_t* cmd)
{
    hal_spi_opreation_t hal_opreation = { {SPI_CMD_SUPPORT, cmd[FLASH_CMD_INDEX], STANDARD_SPI, 0x0}, cmd_len - 1, 0 };
    uint32_t data_en = cmd_len > 1 ? ENABLE : DISABLE;
    errcode_t ret = hal_sfc_regs_wait_ready(0x0);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    hal_sfc_regs_set_opt(&hal_opreation);
    cmd_databuf_t databuf;
    databuf.d32 = 0;
    for (uint32_t i = 1; i < cmd_len; i++) {
        databuf.b.databyte[i - 1] = cmd[i];
    }
    hal_sfc_regs_set_databuf(0, databuf.d32);
    hal_sfc_regs_set_opt_attr(WRITE_MODE, data_en, DISABLE);
    hal_sfc_regs_wait_config();
    return ERRCODE_SUCC;
}

STATIC errcode_t hal_sfc_execute_type_proc(uint8_t cmd, uint8_t offset, uint8_t bit)
{
    hal_spi_opreation_t hal_opreation = { {SPI_CMD_SUPPORT, cmd, STANDARD_SPI, 0x0}, 1, 0 };
    hal_sfc_regs_wait_ready(0x0);
    hal_sfc_regs_set_opt(&hal_opreation);
    hal_sfc_regs_set_opt_attr(READ_MODE, ENABLE, DISABLE);
    hal_sfc_regs_wait_config();
    uint32_t data = hal_sfc_regs_get_databuf(0);
    if (((data >> offset) & 0x1) == bit) {
        return ERRCODE_SUCC;
    }
    return ERRCODE_SFC_CMD_ERROR;
}

STATIC errcode_t hal_sfc_execute_cmds(flash_cmd_execute_t *command)
{
    errcode_t ret;
    flash_cmd_execute_t *current_cmd = command;
    while (current_cmd != NULL) {
        switch ((current_cmd->cmd_type)) {
            case FLASH_CMD_TYPE_CMD:
                ret = hal_sfc_execute_type_cmd(current_cmd->cmd_len, current_cmd->cmd);
                if (ret != ERRCODE_SUCC) {
                    return ret;
                }
                break;
            case FLASH_CMD_TYPE_PROCESSING:
                ret = hal_sfc_execute_type_proc(current_cmd->cmd[FLASH_CMD_INDEX],
                                                current_cmd->cmd[FLASH_CMD_OFFSET], current_cmd->cmd[FLASH_CMD_BIT]);
                if (ret != ERRCODE_SUCC) {
                    return ret;
                }
                break;
            case FLASH_CMD_TYPE_END:
                return ERRCODE_SUCC;
            default:
                return ERRCODE_SFC_CMD_ERROR;
        }
        current_cmd++;
    }
    return ERRCODE_SUCC;
}

STATIC errcode_t hal_sfc_write_enable(void)
{
    errcode_t ret = hal_sfc_regs_wait_ready(0x0);
    uint8_t cmd[1] = {SPI_CMD_WREN};
    ret = hal_sfc_execute_type_cmd(1, cmd);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return hal_sfc_regs_wait_ready(0x0);
}
#if defined(CONFIG_SFC_SUPPORT_DMA)
bool g_dma_busy = false;
errcode_t hal_sfc_dma_read(uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size)
{
    if (g_dma_busy == true) {
        return ERRCODE_SFC_DMA_BUSY;
    }
    g_dma_busy = true;
    errcode_t ret = hal_sfc_regs_wait_ready(0x0);
    if (ret != ERRCODE_SUCC) {
        g_dma_busy = false;
        return ret;
    }
    hal_sfc_regs_set_bus_dma_flash_saddr(flash_addr);
    hal_sfc_regs_set_bus_dma_mem_addr(read_buffer);
    hal_sfc_regs_set_bus_dma_len(read_size);
    hal_sfc_regs_set_bus_dma_ahb_ctrl();
    hal_sfc_regs_set_bus_dma_ctrl(READ_MODE);
    hal_sfc_dma_wait_done();
    g_dma_busy = false;
    return ERRCODE_SUCC;
}

SFC_SECTION
errcode_t hal_sfc_dma_write(uint32_t flash_addr, uint8_t *write_data, uint32_t write_size)
{
    if (g_dma_busy == true) {
        return ERRCODE_SFC_DMA_BUSY;
    }
    g_dma_busy = true;
    errcode_t ret;
    ret = hal_sfc_write_enable();
    if (ret != ERRCODE_SUCC) {
        g_dma_busy = false;
        return ret;
    }
    hal_sfc_regs_set_bus_dma_flash_saddr(flash_addr);
    hal_sfc_regs_set_bus_dma_mem_addr(write_data);
    hal_sfc_regs_set_bus_dma_len(write_size);
    hal_sfc_regs_set_bus_dma_ahb_ctrl();
    hal_sfc_regs_set_bus_dma_ctrl(WRITE_MODE);
    hal_sfc_dma_wait_done();
    g_dma_busy = false;
    return ERRCODE_SUCC;
}

#endif

errcode_t hal_sfc_get_flash_id(uint32_t *flash_id)
{
    errcode_t ret = hal_sfc_regs_init();
    ret = hal_sfc_regs_wait_ready(0x0);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    hal_spi_opreation_t hal_opreation = { {SPI_CMD_SUPPORT, SPI_CMD_RDID, STANDARD_SPI, 0x0}, 0x3, 0 };
    hal_sfc_regs_set_opt(&hal_opreation);
    hal_sfc_regs_set_opt_attr(READ_MODE, ENABLE, DISABLE);
    hal_sfc_regs_wait_config();
    *flash_id = FLASH_ID_MASK & hal_sfc_regs_get_databuf(0);
    return hal_sfc_regs_wait_ready(0x0); /* 0 : flash status bit */
}

STATIC uint32_t get_size(uint32_t size)
{
    uint32_t ret = 0;
    uint32_t size_tmp = size;
    while ((size_tmp & 0x1) == 0) {
        size_tmp >>= 1;
        ret++;
    }
    return ret;
}

errcode_t hal_sfc_init(flash_spi_ctrl_t *spi_ctrl, uint32_t mapping, uint32_t flash_size)
{
    if (flash_size < SFC_V150_MIN_SIZE) {
        return ERRCODE_INVALID_PARAM;
    }
    errcode_t ret = hal_sfc_execute_cmds(spi_ctrl->quad_mode);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    uint32_t addr = mapping >> ADDR_SHIFT_BITS;
    hal_sfc_regs_set_bus_baseaddr(addr);
    uint32_t hal_flash_size = get_size(flash_size >> FLASH_SIZE_D_VALUE);
    hal_sfc_regs_set_bus_flash_size(hal_flash_size);
    hal_sfc_regs_set_bus_read(spi_ctrl->read_opreation);
    if (unlikely(spi_ctrl->write_opreation.cmd != 0)) {
        hal_sfc_regs_set_bus_write(spi_ctrl->write_opreation);
    }
    hal_sfc_regs_set_timing();
    hal_sfc_regs_wait_ready(0);
    return ERRCODE_SUCC;
}

void hal_sfc_deinit(void)
{
    hal_sfc_regs_deinit();
}

SFC_SECTION
STATIC errcode_t hal_sfc_reg_read_once(uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size,
                                       spi_opreation_t read_opreation)
{
    errcode_t ret = hal_sfc_regs_wait_ready(0x0);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    hal_spi_opreation_t opreation = {
        .opt = read_opreation,
        .data_size = read_size,
        .dummy_byte = read_opreation.size
    };
    hal_sfc_regs_set_opt(&opreation);
    hal_sfc_regs_set_cmd_addr(flash_addr);
    hal_sfc_regs_set_opt_attr(READ_MODE, ENABLE, ENABLE);
    hal_sfc_regs_wait_config();

    uint32_t read_buffer_tmp[MAX_DATABUF_NUM];
    uint16_t end_pos = (uint16_t)get_word_num(read_size);
    for (uint16_t i = 0; i < end_pos; i++) {
        read_buffer_tmp[i] = hal_sfc_regs_get_databuf(i);
    }

    if (memcpy_s(read_buffer, read_size, (uint8_t *)read_buffer_tmp, read_size) != EOK) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

SFC_SECTION
errcode_t hal_sfc_reg_read(uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size,
                           spi_opreation_t read_opreation)
{
    uint32_t cur_addr = flash_addr;
    uint8_t *buf_ptr = read_buffer;
    uint32_t remained_size = read_size;
    uint32_t cur_size = SFC_DATABUF_LENGTH;
    while (remained_size > 0) {
        cur_size = remained_size > cur_size ? cur_size : remained_size;
        errcode_t ret = hal_sfc_reg_read_once(cur_addr, buf_ptr, cur_size, read_opreation);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
        cur_addr += cur_size;
        buf_ptr += cur_size;
        remained_size -= cur_size;
    }
    return ERRCODE_SUCC;
}

SFC_SECTION
STATIC errcode_t hal_sfc_reg_write_once(uint32_t flash_addr, uint8_t *write_data, uint32_t write_size,
                                        spi_opreation_t write_opreation)
{
    errcode_t ret;
    ret = hal_sfc_write_enable();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    hal_spi_opreation_t opreation = {
        .opt = write_opreation,
        .data_size = write_size,
        .dummy_byte = write_opreation.size
    };
    hal_sfc_regs_set_opt(&opreation);
    hal_sfc_regs_set_cmd_addr(flash_addr);
    uint32_t write_buffer_tmp[MAX_DATABUF_NUM];

    if (memcpy_s((uint8_t *)write_buffer_tmp, MAX_DATABUF_NUM * sizeof(uint32_t), write_data, write_size) != EOK) {
        return ERRCODE_FAIL;
    }
    uint16_t end_pos = (uint16_t)get_word_num(write_size);

    for (uint16_t i = 0; i < end_pos; i++) {
        hal_sfc_regs_set_databuf(i, write_buffer_tmp[i]);
    }

    hal_sfc_regs_set_opt_attr(WRITE_MODE, ENABLE, ENABLE);
    hal_sfc_regs_wait_config();
    return hal_sfc_regs_wait_ready(0x0);
}

SFC_SECTION
errcode_t hal_sfc_reg_write(uint32_t flash_addr, uint8_t *write_data, uint32_t write_size,
                            spi_opreation_t write_opreation)
{
    errcode_t ret;
    uint32_t unaligned_size = SFC_DATABUF_LENGTH - (flash_addr % SFC_DATABUF_LENGTH);
    if (write_size < unaligned_size) {
        unaligned_size = write_size;
    }
    uint32_t cur_addr = flash_addr;
    uint8_t *buf_ptr = write_data;
    uint32_t remained_size = write_size;
    /* Part1: 起始地址没有64字节对齐 */
    ret = hal_sfc_reg_write_once(flash_addr, buf_ptr, unaligned_size, write_opreation);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }
    remained_size -= unaligned_size;
    if (remained_size == 0) {
        return ERRCODE_SUCC;
    }
    buf_ptr += unaligned_size;
    cur_addr += unaligned_size;
    uint32_t cur_size = SFC_DATABUF_LENGTH;
    while (remained_size > 0) {
        cur_size = remained_size <= SFC_DATABUF_LENGTH ? remained_size : SFC_DATABUF_LENGTH;
        ret = hal_sfc_reg_write_once(cur_addr, buf_ptr, cur_size, write_opreation);
        if (unlikely(ret != ERRCODE_SUCC)) {
            return ret;
        }
        buf_ptr += cur_size;
        cur_addr += cur_size;
        remained_size -= cur_size;
    }
    return ERRCODE_SUCC;
}

SFC_SECTION
errcode_t hal_sfc_reg_erase(uint32_t flash_addr, spi_opreation_t erase_opreation, bool delete_chip)
{
    errcode_t ret;
    uint32_t addr_en = delete_chip ? DISABLE : ENABLE;
    ret = hal_sfc_write_enable();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    hal_spi_opreation_t opreation = { .opt = erase_opreation, .data_size = 0x0, .dummy_byte = 0x0};
    hal_sfc_regs_set_opt(&opreation);
    hal_sfc_regs_set_cmd_addr(flash_addr);
    hal_sfc_regs_set_opt_attr(WRITE_MODE, DISABLE, addr_en);
    hal_sfc_regs_wait_config();
    return hal_sfc_regs_wait_ready(0x0);
}

STATIC errcode_t hal_sfc_regs_read_flash_info(uint32_t opt_type, uint8_t cmd, uint8_t *buffer, uint32_t length)
{
    unused(opt_type);
    errcode_t ret;
    spi_opreation_t opreation = { .cmd_support = SPI_CMD_SUPPORT, .cmd = cmd, .iftype = 0x0, .size = 0};
    hal_spi_opreation_t hal_opreation = { .opt = opreation, .data_size = length, .dummy_byte = 0x0};
    ret = hal_sfc_regs_wait_ready(0x0);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    };
    hal_sfc_regs_set_opt(&hal_opreation);
    hal_sfc_regs_set_opt_attr(READ_MODE, ENABLE, DISABLE);
    hal_sfc_regs_wait_config();
    uint32_t temp_data = hal_sfc_regs_get_databuf(0);
    if (memcpy_s(buffer, length, &temp_data, length) != EOK) {
        return ERRCODE_FAIL;
    }
    return hal_sfc_regs_wait_ready(0x0);
}

STATIC errcode_t hal_sfc_regs_set_flash_attr(uint32_t opt_type, uint8_t cmd, uint8_t *buffer, uint32_t length)
{
    unused(opt_type);
    errcode_t ret;
    spi_opreation_t opreation = { .cmd_support = SPI_CMD_SUPPORT, .cmd = cmd, .iftype = 0x0, .size = 0};
    hal_spi_opreation_t hal_opreation = { .opt = opreation, .data_size = length, .dummy_byte = 0x0};
    ret = hal_sfc_regs_wait_ready(0x0);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    };
    uint8_t data_en = DISABLE;
    if (length != 0) {
        data_en = ENABLE;
        uint32_t write_buffer_tmp[MAX_DATABUF_NUM];
        if (memcpy_s((uint8_t *)write_buffer_tmp, MAX_DATABUF_NUM * sizeof(uint32_t), buffer, length) != EOK) {
            return ERRCODE_FAIL;
        }
        uint16_t end_pos = get_word_num(length);

        for (uint16_t i = 0; i < end_pos; i++) {
            hal_sfc_regs_set_databuf(i, write_buffer_tmp[i]);
        }
    }
    hal_sfc_regs_set_opt(&hal_opreation);
    hal_sfc_regs_set_opt_attr(WRITE_MODE, data_en, DISABLE);
    hal_sfc_regs_wait_config();
    return hal_sfc_regs_wait_ready(0x0);
}

STATIC errcode_t hal_sfc_regs_read_flash_unique_id(uint32_t opt_type, uint8_t cmd, uint8_t *buffer, uint32_t length)
{
    unused(opt_type);
    unused(cmd);
    errcode_t ret;
    spi_opreation_t opreation = { .cmd_support = SPI_CMD_SUPPORT, .cmd = 0x4B, .iftype = 0x0, .size = 0x0 };
    hal_spi_opreation_t hal_opreation = { .opt = opreation, .data_size = length, .dummy_byte = 0x1};
    ret = hal_sfc_regs_wait_ready(0x0);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }
    hal_sfc_regs_set_opt(&hal_opreation);
    hal_sfc_regs_set_cmd_addr(0);
    hal_sfc_regs_set_opt_attr(READ_MODE, ENABLE, ENABLE);
    hal_sfc_regs_wait_config();
    uint32_t read_buffer_tmp[MAX_DATABUF_NUM] = {0};
    uint32_t end_pos = length >> 2;
    for (uint32_t i = 0; i < end_pos; i++) {
        read_buffer_tmp[i] = hal_sfc_regs_get_databuf(i);
    }
    if (likely((length & 0x3) != 0)) {
        read_buffer_tmp[end_pos] = hal_sfc_regs_get_databuf(end_pos);
    }
    if (memcpy_s(buffer, length, (uint8_t *)read_buffer_tmp, length) != EOK) {
        return ERRCODE_FAIL;
    }
    return hal_sfc_regs_wait_ready(0x0);
}

static const hal_sfc_reg_flash_opreation_t g_flash_opreations[FLASH_OPREATION_MAX_NUM] = {
    hal_sfc_regs_read_flash_info,
    hal_sfc_regs_set_flash_attr,
    hal_sfc_regs_read_flash_unique_id
};

errcode_t hal_sfc_reg_flash_opreations(uint32_t opt_type, uint8_t cmd, uint8_t *buffer, uint32_t length)
{
    return g_flash_opreations[opt_type](opt_type, cmd, buffer, length);
}

#if defined(CONFIG_SFC_SUPPORT_LPM)
errcode_t hal_sfc_suspend(void)
{
    g_sfc_suspend_regs[0x0] = hal_sfc_regs_get_sfc_bus_config1();
    g_sfc_suspend_regs[0x1] = hal_sfc_regs_get_sfc_bus_config2();
    g_sfc_suspend_regs[0x2] = hal_sfc_regs_get_sfc_bus_flash_size();
    g_sfc_suspend_regs[0x3] = hal_sfc_regs_get_bus_base_addr_cs0();
    g_sfc_suspend_regs[0x4] = hal_sfc_regs_get_bus_base_addr_cs1();
    return ERRCODE_SUCC;
}

errcode_t hal_sfc_resume(flash_cmd_execute_t *quad_mode)
{
    errcode_t ret = hal_sfc_execute_cmds(quad_mode);
    hal_sfc_regs_set_sfc_bus_config1(g_sfc_suspend_regs[0x0]);
    hal_sfc_regs_set_sfc_bus_config2(g_sfc_suspend_regs[0x1]);
    hal_sfc_regs_set_sfc_bus_flash_size(g_sfc_suspend_regs[0x2]);
    hal_sfc_regs_set_bus_base_addr_cs0(g_sfc_suspend_regs[0x3]);
    hal_sfc_regs_set_bus_base_addr_cs1(g_sfc_suspend_regs[0x4]);
    hal_sfc_regs_set_timing();
    hal_sfc_regs_wait_ready(0);
    return ret;
}
#endif