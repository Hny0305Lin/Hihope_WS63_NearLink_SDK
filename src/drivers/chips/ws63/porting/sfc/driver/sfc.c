/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides sfc driver source \n
 *
 * History: \n
 * 2022-11-29， Create file. \n
 */
#include <stdbool.h>
#include <securec.h>
#include <tcxo.h>
#include <sfc_porting.h>
#include <hal_sfc.h>
#include "osal_interrupt.h"
#include "sfc.h"

#define BYTES_64_MASK             0x3F
#define BYTES_4K_MASK             0xFFF
#define BYTES_4_MASK              0x3
#define BYTES_64K                 0x10000
#define BYTES_4K                  0x1000
#define BYTES_64                  0x40
#define BYTES_4                   0x4
#define BYTES_18                  0x12

#define ERASE_CHIP                0
#define SFC_READ_TIME_SHIFT_NUM   6
#define GREEDY_MIN_ERASE_NUM      2

static bool g_sfc_inited = false;
static bool g_sfc_unknown_flash = false;
static flash_spi_ctrl_t g_flash_ctrl;

#if defined(CONFIG_SFC_ALLOW_ERASE_WRITEBACK)
static uint8_t g_first_buffer[4096] = {0};
static uint8_t g_last_buffer[4096] = {0};
#endif /* CONFIG_SFC_ALLOW_ERASE_WRITEBACK */

STATIC errcode_t check_init_param(const sfc_flash_config_t *config)
{
    uint32_t mapping_size = min(config->mapping_size, g_flash_ctrl.chip_size);
    if (config->mapping_addr < sfc_port_get_sfc_start_addr() ||
        config->mapping_addr + mapping_size - 1 > sfc_port_get_sfc_end_addr()) {
        return ERRCODE_SFC_ADDRESS_OVERSTEP;
    }
    g_flash_ctrl.chip_size = mapping_size;
    return ERRCODE_SUCC;
}

STATIC errcode_t check_opt_param(uint32_t addr, uint32_t size)
{
    if (unlikely(!g_sfc_inited)) {
        return ERRCODE_SFC_NOT_INIT;
    }
    if ((addr + size) > g_flash_ctrl.chip_size || (addr + size) <= addr || (addr + size) < size) {
        return ERRCODE_INVALID_PARAM;
    }
    return ERRCODE_SUCC;
}

STATIC errcode_t build_flash_ctrl(const flash_spi_info_t *spi_info, sfc_read_if_t read_type, sfc_write_if_t write_type)
{
    spi_opreation_t read_cmd = spi_info->read_cmds[read_type];
    spi_opreation_t write_cmd = spi_info->write_cmds[write_type];
    if (read_cmd.cmd_support != SPI_CMD_SUPPORT || write_cmd.cmd_support != SPI_CMD_SUPPORT) {
        return ERRCODE_SFC_CMD_NOT_SUPPORT;
    }
    g_flash_ctrl.read_opreation = spi_info->read_cmds[read_type];
    g_flash_ctrl.write_opreation = spi_info->write_cmds[write_type];
    g_flash_ctrl.erase_opreation_array = spi_info->erase_cmds;
    g_flash_ctrl.chip_size = spi_info->chip_size;
    if (spi_info->erase_cmd_num < GREEDY_MIN_ERASE_NUM) {
        return ERRCODE_SFC_PORT_INVALID_PARAM;
    }
    g_flash_ctrl.erase_cmd_num = spi_info->erase_cmd_num;
    g_flash_ctrl.quad_mode = spi_info->quad_mode;
    return ERRCODE_SUCC;
}

STATIC errcode_t build_cmds(uint32_t flash_id, sfc_read_if_t read_type, sfc_write_if_t write_type)
{
    flash_spi_info_t *flash_spi_infos = sfc_port_get_flash_spi_infos();
    uint32_t flash_num = sfc_port_get_flash_num();
    for (uint32_t i = 0; i < flash_num; i++) {
        if (flash_id == flash_spi_infos[i].chip_id) {
            errcode_t ret = build_flash_ctrl(&flash_spi_infos[i], read_type, write_type);
            return ret;
        }
    }
    flash_spi_infos = sfc_port_get_unknown_flash_info();
    errcode_t ret = build_flash_ctrl(&flash_spi_infos[0], STANDARD_READ, PAGE_PROGRAM);
    if (ret == ERRCODE_SUCC) {
        g_sfc_unknown_flash = true;
    }
    return ret;
}

STATIC errcode_t do_greedy_erase(uint32_t hal_erase_size, uint32_t start_sector)
{
    uint32_t erase_opt_index;
    uint32_t temp_size = 0;
    errcode_t ret = ERRCODE_FAIL;
    /* All parameters are aligned in 4KB. */
    uint32_t remain_size = hal_erase_size;
    uint32_t current_addr = start_sector;
    spi_opreation_t current_erase_opt = {0};

    while (remain_size > 0) {
        for (erase_opt_index = 1; erase_opt_index < g_flash_ctrl.erase_cmd_num; erase_opt_index++) {
            current_erase_opt = g_flash_ctrl.erase_opreation_array[erase_opt_index];
            temp_size = current_erase_opt.size;
            if ((remain_size >= temp_size) && ((current_addr & (temp_size - 1)) == 0)) {
                break;
            }
        }
        /* Generally, the 4K erase is not configured for this branch. Check the erase array at the port layer. */
        if (erase_opt_index == g_flash_ctrl.erase_cmd_num) {
            return ERRCODE_SFC_ERASE_FORM_ERROR;
        }
        ret = hal_sfc_reg_erase(current_addr, current_erase_opt, false);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
        remain_size -= temp_size;
        current_addr += temp_size;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_sfc_init(sfc_flash_config_t *config)
{
    errcode_t ret;
    if (unlikely(g_sfc_inited)) {
        return ERRCODE_SUCC;
    }
    sfc_port_lock_init();

    uint32_t flash_id;
    ret = hal_sfc_get_flash_id(&flash_id);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }
    ret = build_cmds(flash_id, config->read_type, config->write_type);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }
    ret = check_init_param(config);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }
#if !defined(CONFIG_SFC_ALREADY_INIT)
    flash_spi_ctrl_t *spi_ctrl = &g_flash_ctrl;
    ret = hal_sfc_init(spi_ctrl, config->mapping_addr, g_flash_ctrl.chip_size);
    if (ret == ERRCODE_SUCC) {
        g_sfc_inited = true;
        ret = g_sfc_unknown_flash ? ERRCODE_SFC_FLASH_NOT_SUPPORT : ERRCODE_SUCC;
    }
    return ret;
#else
    g_sfc_inited = true;
    return ERRCODE_SUCC;
#endif
}

errcode_t uapi_sfc_init_rom(sfc_flash_config_t *config)
{
    errcode_t ret;
    if (unlikely(g_sfc_inited)) {
        return ERRCODE_SUCC;
    }
    ret = hal_sfc_regs_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    flash_spi_info_t *flash_spi_info = sfc_port_get_unknown_flash_info();
    ret = build_flash_ctrl(&flash_spi_info[0], STANDARD_READ, PAGE_PROGRAM);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }
    g_sfc_unknown_flash = true;
    ret = check_init_param(config);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }
    flash_spi_ctrl_t *spi_ctrl = &g_flash_ctrl;
    ret = hal_sfc_init(spi_ctrl, config->mapping_addr, g_flash_ctrl.chip_size);
    if (ret == ERRCODE_SUCC) {
        g_sfc_inited = true;
        ret = g_sfc_unknown_flash ? ERRCODE_SFC_DEFAULT_INIT : ERRCODE_SUCC;
    }
    return ret;
}

void uapi_sfc_deinit(void)
{
    if (unlikely(!g_sfc_inited)) {
        return;
    }
#if !defined(CONFIG_SFC_ALREADY_INIT)
    hal_sfc_deinit();
#endif
    g_sfc_inited = false;
    g_sfc_unknown_flash = false;
}

SFC_SECTION
errcode_t uapi_sfc_reg_read(uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size)
{
    errcode_t ret = check_opt_param(flash_addr, read_size);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }
    uint32_t current_addr = flash_addr;
    uint8_t *buffer_ptr = read_buffer;
    uint32_t remained_size = read_size;
    /* flash_addr 4 byte misaligned */
    /* Part1.1: 检查非对齐长度 */
    uint32_t misaligned_addr = current_addr & BYTES_4_MASK;
    uint32_t lock_sts = sfc_port_lock();
    if (misaligned_addr != 0) {
        current_addr -= misaligned_addr;
        uint8_t cur_size = (uint8_t)((BYTES_4 - misaligned_addr) > read_size ? read_size :
            (BYTES_4 - misaligned_addr));
        /* Part1.2: 使用向前对齐后的地址读取4个字节至临时buffer */
        uint8_t temp_buf[BYTES_4] = {0};
        ret = hal_sfc_reg_read(current_addr, temp_buf, BYTES_4, g_flash_ctrl.read_opreation);
        if (ret != ERRCODE_SUCC || memcpy_s(buffer_ptr, cur_size, temp_buf + misaligned_addr, cur_size) != EOK) {
            ret = ret == ERRCODE_SUCC ? ERRCODE_MEMCPY : ret;
            sfc_port_unlock(lock_sts);
            return ret;
        }
        current_addr += BYTES_4; // 以对齐后的地址+4
        buffer_ptr += cur_size; // 以原地址增加读取到的长度
        remained_size -= cur_size; // 以原长度减少读取到的长度
    }
    /* flash_addr 4 byte aligned */
    /* Part2: 读取地址及长度对齐部分的数据 */
    uint32_t aligned_size = remained_size - (remained_size & BYTES_4_MASK);
    if (likely(ret == ERRCODE_SUCC) && aligned_size != 0) {
        ret = hal_sfc_reg_read(current_addr, buffer_ptr, aligned_size, g_flash_ctrl.read_opreation);
        buffer_ptr += aligned_size;
        current_addr += aligned_size;
        remained_size -= aligned_size;
    }
    /* Part3: 读取长度不对齐部分的数据 */
    if (likely(ret == ERRCODE_SUCC) && remained_size != 0) {
        /* 读取4字节，并将非对齐部分拷贝至read_buffer */
        uint8_t temp_buf[BYTES_4] = {0};
        ret = hal_sfc_reg_read(current_addr, temp_buf, BYTES_4, g_flash_ctrl.read_opreation);
        if (memcpy_s(buffer_ptr, remained_size, temp_buf, remained_size) != EOK) {
            ret = ERRCODE_MEMCPY;
        }
    }
    sfc_port_unlock(lock_sts);
    return ret;
}

SFC_SECTION
errcode_t uapi_sfc_reg_write(uint32_t flash_addr, uint8_t *write_data, uint32_t write_size)
{
    errcode_t ret = check_opt_param(flash_addr, write_size);
    if (unlikely(ret != ERRCODE_SUCC)) { return ret; }
    uint32_t current_addr = flash_addr;
    uint8_t *buffer_ptr = write_data;
    uint32_t remained_size = write_size;
    uint8_t temp_buf[BYTES_4];
    /* flash_addr 4 byte misaligned */
    /* Part1.1: 检查非对齐长度 */
    uint32_t misaligned_addr = current_addr & BYTES_4_MASK;
    uint32_t lock_sts = sfc_port_write_lock(flash_addr, flash_addr + write_size);
    if (misaligned_addr != 0) {
        current_addr -= misaligned_addr;
        uint8_t cur_size = (uint8_t)((BYTES_4 - misaligned_addr) > remained_size ? remained_size :
            (BYTES_4 - misaligned_addr));
        /* Part1.2: 将写入数据非对齐地址开始的数据拷贝至临时buffer */
        ret = hal_sfc_reg_read(current_addr, temp_buf, BYTES_4, g_flash_ctrl.read_opreation);
        if (ret != ERRCODE_SUCC || memcpy_s(temp_buf + misaligned_addr, cur_size, buffer_ptr, cur_size) != EOK) {
            ret = ret == ERRCODE_SUCC ? ERRCODE_MEMCPY : ret;
            sfc_port_write_unlock(lock_sts);
            return ret;
        }
        /* Part1.3: 使用临时buffer往向前对齐后的地址写入四个字节 */
        ret = hal_sfc_reg_write(current_addr, temp_buf, BYTES_4, g_flash_ctrl.write_opreation);
        current_addr += BYTES_4; // 以对齐后的地址+4
        buffer_ptr += cur_size; // 以原地址增加读取到的长度
        remained_size -= cur_size; // 以原长度减少读取到的长度
    }
    /* flash_addr 4 byte aligned */
    /* Part2: 写入地址及长度对齐部分的数据 */
    uint32_t aligned_size = remained_size - (remained_size & BYTES_4_MASK);
    if (likely(ret == ERRCODE_SUCC) && aligned_size != 0) {
        ret = hal_sfc_reg_write(current_addr, buffer_ptr, aligned_size, g_flash_ctrl.write_opreation);
        buffer_ptr += aligned_size;
        current_addr += aligned_size;
        remained_size -= aligned_size;
    }
    /* Part3: 写入长度不对齐部分的数据 */
    if (likely(ret == ERRCODE_SUCC) && remained_size != 0) {
        /* 构造非对齐部分的4字节临时数据 */
        ret = hal_sfc_reg_read(current_addr, temp_buf, BYTES_4, g_flash_ctrl.read_opreation);
        if (ret != ERRCODE_SUCC || memcpy_s(temp_buf, BYTES_4, buffer_ptr, remained_size) != EOK) {
            ret = ret == ERRCODE_SUCC ? ERRCODE_MEMCPY : ret;
            sfc_port_write_unlock(lock_sts);
            return ERRCODE_MEMCPY;
        }
        ret = hal_sfc_reg_write(current_addr, temp_buf, BYTES_4, g_flash_ctrl.write_opreation);
    }
    sfc_port_write_unlock(lock_sts);
    return ret;
}

#if defined(CONFIG_SFC_SUPPORT_DMA)
SFC_SECTION
errcode_t uapi_sfc_dma_read(uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size)
{
    errcode_t ret = check_opt_param(flash_addr, read_size);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }
    uint32_t lock_sts = sfc_port_lock();
    ret = hal_sfc_dma_read(flash_addr, read_buffer, read_size);
    sfc_port_unlock(lock_sts);
    return ret;
}

SFC_SECTION
errcode_t uapi_sfc_dma_write(uint32_t flash_addr, uint8_t *write_buffer, uint32_t write_size)
{
    errcode_t ret = check_opt_param(flash_addr, write_size);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }
    uint32_t lock_sts = sfc_port_write_lock(flash_addr, flash_addr + write_size);
    ret = hal_sfc_dma_write(flash_addr, write_buffer, write_size);
    sfc_port_write_unlock(lock_sts);
    return ret;
}
#endif /* CONFIG_SFC_SUPPORT_DMA */

SFC_SECTION
errcode_t uapi_sfc_reg_erase(uint32_t flash_addr, uint32_t erase_size)
{
    errcode_t ret = check_opt_param(flash_addr, erase_size);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }

    uint32_t end_addr = flash_addr + erase_size;
    uint32_t start_sector = flash_addr & ~BYTES_4K_MASK;
    uint32_t end_sector = (end_addr & BYTES_4K_MASK) == 0 ? end_addr : (end_addr & ~BYTES_4K_MASK) + BYTES_4K;
    uint32_t hal_erase_size = end_sector - start_sector;

#if defined(CONFIG_SFC_ALLOW_ERASE_WRITEBACK)
    /* Backup data to RAM */
    uint32_t first_size = flash_addr - start_sector;
    if (likely(first_size != 0)) {
        uapi_sfc_reg_read(start_sector, g_first_buffer, first_size);
    }
    uint32_t last_size = end_sector - end_addr;
    if (likely(last_size != 0)) {
        uapi_sfc_reg_read(end_addr, g_last_buffer, last_size);
    }
#else
    if (flash_addr != start_sector || end_addr != end_sector) {
        return ERRCODE_INVALID_PARAM;
    }
#endif /* CONFIG_SFC_ALLOW_ERASE_WRITEBACK */

    uint32_t lock_sts = sfc_port_write_lock(flash_addr, flash_addr + erase_size);
    /* Erasing with greedy algorithms */
    ret = do_greedy_erase(hal_erase_size, start_sector);
    sfc_port_write_unlock(lock_sts);

#if defined(CONFIG_SFC_ALLOW_ERASE_WRITEBACK)
    /* Write back data from RAM */
    if (likely(first_size != 0)) {
        uapi_sfc_reg_write(start_sector, g_first_buffer, first_size);
    }
    if (likely(last_size != 0)) {
        uapi_sfc_reg_write(end_addr, g_last_buffer, last_size);
    }
#endif /* CONFIG_SFC_ALLOW_ERASE_WRITEBACK */
    return ret;
}

SFC_SECTION
errcode_t uapi_sfc_reg_erase_chip(void)
{
    if (unlikely(!g_sfc_inited)) {
        return ERRCODE_SFC_NOT_INIT;
    }
    uint32_t lock_sts = sfc_port_write_lock(0x0, FLASH_CHIP_PROTECT_END);
    errcode_t ret = hal_sfc_reg_erase(0x0, g_flash_ctrl.erase_opreation_array[ERASE_CHIP], true);
    sfc_port_write_unlock(lock_sts);
    return ret;
}

SFC_SECTION
errcode_t uapi_sfc_reg_other_flash_opt(sfc_flash_op_t cmd_type, uint8_t cmd, uint8_t *buffer, uint32_t length)
{
    if (unlikely(!g_sfc_inited)) {
        return ERRCODE_SFC_NOT_INIT;
    }
    if (unlikely(length > BYTES_18)) {
        return ERRCODE_INVALID_PARAM;
    }
    uint32_t lock_sts = sfc_port_lock();
    errcode_t ret = hal_sfc_reg_flash_opreations(cmd_type, cmd, buffer, length);
    sfc_port_unlock(lock_sts);
    return ret;
}

#if defined(CONFIG_SFC_SUPPORT_LPM)
errcode_t uapi_sfc_suspend(uintptr_t arg)
{
    unused(arg);
    hal_sfc_suspend();
    return ERRCODE_SUCC;
}

errcode_t uapi_sfc_resume(uintptr_t arg)
{
    unused(arg);
    return hal_sfc_resume(g_flash_ctrl.quad_mode);
}
#endif  /* CONFIG_SFC_SUPPORT_LPM */

#if defined(CONFIG_SFC_SUPPORT_WRITE_PROTECT)
errcode_t uapi_sfc_lock_protect(sfc_flash_protect_region_t region)
{
    if (unlikely(!g_sfc_inited)) {
        return ERRCODE_SFC_NOT_INIT;
    }
    uint32_t flash_id = 0;
    errcode_t ret = hal_sfc_get_flash_id(&flash_id);
    if (ret != ERRCODE_SUCC || flash_id == 0) {
        return ERRCODE_FAIL;
    }
    uint32_t flag = osal_irq_lock();
    ret = hal_sfc_lock_protect(flash_id, region);
    osal_irq_restore(flag);
    return ret;
}

errcode_t uapi_sfc_unlock_protect(void)
{
    if (unlikely(!g_sfc_inited)) {
        return ERRCODE_SFC_NOT_INIT;
    }
    uint32_t flash_id = 0;
    errcode_t ret = hal_sfc_get_flash_id(&flash_id);
    if (ret != ERRCODE_SUCC || flash_id == 0) {
        return ERRCODE_FAIL;
    }
    uint32_t flag = osal_irq_lock();
    ret = hal_sfc_unlock_protect(flash_id);
    osal_irq_restore(flag);
    return ret;
}
#endif
