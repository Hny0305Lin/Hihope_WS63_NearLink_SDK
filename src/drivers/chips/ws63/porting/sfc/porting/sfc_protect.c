/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides sfc port template \n
 *
 * History: \n
 * 2022-11-30， Create file. \n
 */
#include "hal_sfc_v150.h"
#ifndef BUILD_NOOSAL
#include "soc_osal.h"
#endif
#include "debug_print.h"
#include "hal_sfc_v150_regs_op.h"
#include "osal_interrupt.h"
#include "securec.h"
#include "tcxo.h"
#include "sfc.h"
#include "sfc_porting.h"
#include "hal_sfc.h"
#include "sfc_protect_gd25q32.h"
#include "sfc_protect.h"

#define SR1_BPX_MASK 0x7C
#define SR2_CMP_MASK 0x40

#define FLASH_PROTECT_NONE_MASK         0x00
#define FLASH_PROTECT_ALL_MASK          0x1C

#define SPI_CMD_WREN             0x06
#define SPI_CMD_WRVSR            0x50

#define STANDARD_SPI             0x0

#define ENABLE                   0x1
#define DISABLE                  0x0

#define READ_MODE                0x1
#define WRITE_MODE               0x0

typedef struct {
    uint8_t sr2; // value of sr2
    uint8_t sr1; // value of sr1
    uint32_t start;
    uint32_t end;
} sfc_protect_cfg_t;

#define FLASH_CHIP_PROTECT_4K       0x001000
#define FLASH_CHIP_PROTECT_8K       0x002000
#define FLASH_CHIP_PROTECT_16K      0x004000
#define FLASH_CHIP_PROTECT_32K      0x008000
#define FLASH_CHIP_PROTECT_64K      0x010000
#define FLASH_CHIP_PROTECT_128K     0x020000
#define FLASH_CHIP_PROTECT_256K     0x040000
#define FLASH_CHIP_PROTECT_512K     0x080000
#define FLASH_CHIP_PROTECT_1M       0x100000
#define FLASH_CHIP_PROTECT_2M       0x200000
#define FLASH_CHIP_PROTECT_3M       0x300000
#define FLASH_CHIP_PROTECT_3584K    0x380000 // 512K
#define FLASH_CHIP_PROTECT_3840K    0x3C0000 // 256K
#define FLASH_CHIP_PROTECT_3968K    0x3E0000 // 128K
#define FLASH_CHIP_PROTECT_4032K    0x3F0000 // 64K
#define FLASH_CHIP_PROTECT_4064K    0x3F8000 // 32K
#define FLASH_CHIP_PROTECT_4080K    0x3FC000 // 16K
#define FLASH_CHIP_PROTECT_4088K    0x3FE000 // 8K
#define FLASH_CHIP_PROTECT_4092K    0x3FF000 // 4K

/**
 * 可写地址区间，前闭后开，[xxx, yyy)
 * 配置必须按照放开写区间的大小，从小到大手动排序
 */
#if defined FLASH_REGION_CFG_LOADERBOOT
static const sfc_protect_cfg_t g_sfc_protect_cfg[] ={
    {0x02, 0x28, FLASH_CHIP_PROTECT_128K, FLASH_CHIP_PROTECT_END}, // 保护前两个Block 0&1
    {0x02, 0x78, FLASH_CHIP_PROTECT_32K, FLASH_CHIP_PROTECT_END}, // 保护前32K
    {0x02, 0x00, 0x0, FLASH_CHIP_PROTECT_END}, // 全片放开, 烧录时需要写前32K
};

#elif defined FLASH_REGION_CFG_FLASHBOOT
static const sfc_protect_cfg_t g_sfc_protect_cfg[] ={
    {0x02, 0x28, FLASH_CHIP_PROTECT_128K, FLASH_CHIP_PROTECT_END}, // 保护前两个Block 0&1
};

#else // APP
static const sfc_protect_cfg_t g_sfc_protect_cfg[] ={
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    {0x02, 0x28, FLASH_CHIP_PROTECT_128K, FLASH_CHIP_PROTECT_END}, // 保护前两个Block 0&1
    {0x02, 0x78, FLASH_CHIP_PROTECT_32K, FLASH_CHIP_PROTECT_END}, // 保护前32K
#else
    {0x42, 0x4C, FLASH_CHIP_PROTECT_4080K, FLASH_CHIP_PROTECT_END}, // 放开最后16K
    {0x42, 0x58, FLASH_CHIP_PROTECT_4064K, FLASH_CHIP_PROTECT_END}, // 放开最后32K
    {0x42, 0x10, FLASH_CHIP_PROTECT_3584K, FLASH_CHIP_PROTECT_END}, // 保护前7/8 (剩余后8个Block)
    {0x42, 0x14, FLASH_CHIP_PROTECT_3M, FLASH_CHIP_PROTECT_END}, // 保护前3/4 (3M)
    {0x02, 0x38, FLASH_CHIP_PROTECT_2M, FLASH_CHIP_PROTECT_END}, // 保护前1/2 (2M)
    {0x02, 0x28, FLASH_CHIP_PROTECT_128K, FLASH_CHIP_PROTECT_END}, // 保护前两个Block 0&1
#endif // end of _PRE_WLAN_FEATURE_MFG_TEST
};
#endif // end of APP
#define SFC_PROTECT_CFG_NUMS (sizeof(g_sfc_protect_cfg) / sizeof(g_sfc_protect_cfg[0]))

// 地址区间，前闭后开
static sfc_protect_cfg_t const *sfc_port_find_addr_protect_cfg(uint32_t start_addr, uint32_t end_addr)
{
    // g_sfc_protect_cfg 中的配置必须按照放开写区间的大小，从小到大手动排序
    for (uint32_t i = 0; i < SFC_PROTECT_CFG_NUMS; i++) {
        if ((g_sfc_protect_cfg[i].start <= start_addr) &&
            (g_sfc_protect_cfg[i].end >= end_addr)) {
            return &g_sfc_protect_cfg[i];
        }
    }
    return NULL;
}

#ifdef BUILD_NOOSAL
// 该接口在 hal_sfc_v150.c 是 STATIC
errcode_t hal_sfc_regs_wait_ready(uint8_t wip_bit)
{
    uint32_t dead_line = 0;
    uint32_t timeout = sfc_port_get_delay_times();
    uint32_t delay = sfc_port_get_delay_once_time();
    hal_spi_opreation_t hal_opreation = {0};
    hal_opreation.opt.cmd = SPI_CMD_RDSR_1;
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
#else
errcode_t hal_sfc_regs_wait_ready(uint8_t wip_bit);
#endif

static void sfc_port_sr_enable_write(bool is_volatile)
{
    hal_spi_opreation_t hal_opreation;

    // 如果是易失性写入，需要使用50H命令
    //       非易失性写入，需要使用06H命令
    // 50H: Write Enable for Volatile Status Register
    // The 50H must be issued prior to a 01H/31H/11H
    hal_opreation.opt.cmd_support = SPI_CMD_SUPPORT;
    hal_opreation.opt.cmd = is_volatile ? SPI_CMD_WRVSR : SPI_CMD_WREN;
    hal_opreation.opt.iftype = STANDARD_SPI;
    hal_opreation.opt.size = 0x0;
    hal_opreation.data_size = 0x0;
    hal_opreation.dummy_byte = 0x0;

#ifndef BUILD_NOOSAL
    hal_sfc_regs_set_opt(hal_opreation); // ROM化的接口
#else
    // ROM化后，函数参数有变化，flashboot和ssb中用的是非ROM的接口
    hal_sfc_regs_set_opt(&hal_opreation);
#endif

    hal_sfc_regs_set_opt_attr(WRITE_MODE, DISABLE, DISABLE); // cmd starts to be sent here
    hal_sfc_regs_wait_config();
}

static void sfc_port_do_write_sr(uint8_t cmd, uint8_t data)
{
    hal_spi_opreation_t hal_opreation;
    cmd_databuf_t databuf;

    // 01H/31H/11H: Write Status Register (WRSRn) (01H/31H/11H)
    // Any other commands can’t be inserted between 01H/31H/11H and 50H
    hal_opreation.opt.cmd_support = SPI_CMD_SUPPORT;
    hal_opreation.opt.cmd = cmd;
    hal_opreation.opt.iftype = STANDARD_SPI;
    hal_opreation.opt.size = 0x0;
    hal_opreation.data_size = 0x1;
    hal_opreation.dummy_byte = 0x0;

    databuf.d32 = 0;
    databuf.b.databyte[0] = data;
    hal_sfc_regs_set_databuf(0, databuf.d32);

#ifndef BUILD_NOOSAL
    hal_sfc_regs_set_opt(hal_opreation); // ROM化的接口
#else
    // ROM化后，函数参数有变化，flashboot和ssb中用的是非ROM的接口
    hal_sfc_regs_set_opt(&hal_opreation);
#endif

    hal_sfc_regs_set_opt_attr(WRITE_MODE, ENABLE, DISABLE); // cmd starts to be sent here
    hal_sfc_regs_wait_config();
    return;
}

/**
 * @brief  写状态寄存器。
 * @param  [in]  is_volatile 是否是易失性写入。
 *                True: 易失性写入, 掉电后配置丢失。
 *                False: 非易失性写入, 掉电后配置不丢失。
 * @param  [in]  cmd 命令字可以是：01H/31H/11H。
 *                    01H: 写 SR1
 *                    31H: 写 SR2
 *                    11H: 写 SR3
 * @param  [in]  data 待写入的SR寄存器的数据。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
void sfc_port_write_sr(bool is_volatile, uint8_t cmd, uint8_t data)
{
    hal_sfc_regs_wait_ready(0x0);
    sfc_port_sr_enable_write(is_volatile);
    sfc_port_do_write_sr(cmd, data);
}

/**
 * @brief  读取状态寄存器的值。
 * @param  [in] cmd 命令字可以是：05H/35H/15H。
 *                   05H: 读 SR1
 *                   35H: 读 SR2
 *                   15H: 读 SR3
 * @retval 读取到的SR中的值。
 * @endif
 */
uint32_t sfc_port_read_sr(uint32_t cmd)
{
    hal_spi_opreation_t hal_opreation;
    hal_opreation.opt.cmd = cmd;
    hal_opreation.opt.iftype = STANDARD_SPI;
    hal_opreation.data_size = 1;
    hal_opreation.dummy_byte = 0;

    hal_sfc_regs_wait_ready(0x0);
#ifndef BUILD_NOOSAL
    hal_sfc_regs_set_opt(hal_opreation); // ROM化的接口
#else
    // ROM化后，函数参数有变化，flashboot和ssb中用的是非ROM的接口
    hal_sfc_regs_set_opt(&hal_opreation);
#endif
    hal_sfc_regs_set_opt_attr(READ_MODE, ENABLE, DISABLE);
    hal_sfc_regs_wait_config();
    return hal_sfc_regs_get_databuf(0);
}

static void sfc_port_block_protect_disable(uint32_t start_addr, uint32_t end_addr)
{
    sfc_protect_cfg_t const *cfg = sfc_port_find_addr_protect_cfg(start_addr, end_addr);
    if (cfg != NULL) {
        sfc_port_write_sr(true, SPI_CMD_WRSR_1, cfg->sr1);
        sfc_port_write_sr(true, SPI_CMD_WRSR_2, cfg->sr2);
    }
}

static void sfc_port_block_protect_enable(void)
{
    sfc_port_write_sr(true, SPI_CMD_WRSR_1, SR1_VALID_VAL);
    sfc_port_write_sr(true, SPI_CMD_WRSR_2, SR2_VALID_VAL);
}

// 地址区间，前闭后开[xxx, xxx)
uint32_t sfc_port_write_lock(uint32_t start_addr, uint32_t end_addr)
{
#ifndef BUILD_NOOSAL
    uint32_t lock_sts = osal_irq_lock();
    sfc_port_block_protect_disable(start_addr, end_addr);
    return lock_sts;
#else
    sfc_port_block_protect_disable(start_addr, end_addr);
    return 0;
#endif
}

void sfc_port_write_unlock(uint32_t lock_sts)
{
#ifndef BUILD_NOOSAL
    sfc_port_block_protect_enable();
    osal_irq_restore(lock_sts);
#else
    unused(lock_sts);
    sfc_port_block_protect_enable();
#endif
}

#define FLASH_GD25Q32           0x1640C8
errcode_t sfc_port_fix_sr(void)
{
    uint32_t flash_id = 0;
    errcode_t err;

    err = hal_sfc_get_flash_id(&flash_id);
    if (err != ERRCODE_SUCC) {
        return err;
    }

    if (flash_id == FLASH_GD25Q32) {
        return sfc_port_fix_sr_gd25q32();
    }

    return ERRCODE_NOT_SUPPORT;
}

