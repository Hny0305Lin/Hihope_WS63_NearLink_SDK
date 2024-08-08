/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: main
 *
 * Create: 2021-03-09
 */

#include "boot_serial.h"
#include "boot_init.h"
#include "boot_reset.h"
#include "boot_verify.h"
#include "secure_verify_boot.h"
#include "efuse.h"
#include "efuse_porting.h"
#include "chip_io.h"
#include "pinctrl.h"
#include "boot_flash.h"
#include "sfc.h"
#include "boot_delay.h"
#include "boot_jump.h"
#include "tcxo.h"
#include "watchdog.h"
#include "drv_pmp.h"
#include "pmp_cfg.h"
#include "malloc_porting.h"
#include "upg_porting.h"
#include "upg_common.h"
#include "upg_alloc.h"
#include "upg_config.h"
#include "soc_porting.h"
#include "drv_flashboot_cipher.h"
#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_AB
#include "upg_ab.h"
#endif
#include "factory.h"
#include "sfc_protect.h"
#include "reset_porting.h"

#define FLASHBOOT_UART_DEFAULT_PARAM    {115200, 8, 1, 0, 0, 2, 1, 4, 0}
#define APP_START_INSTRUCTION 0x40006f
#define DELAY_100MS           100
#define REG_CMU_CFG0          0x40003408
#define APP_IMAGE_HEADER_LEN  ((KEY_AREA_STRUCTURE_LENGTH) + (CODE_INFO_STRUCTURE_LENGTH))
#define BOOT_WATCH_DOG_TIMOUT 7    // 7s
#define FLASH_KEY_SALT_LEN     28
#define FLASH_ENCRY_ADDR_ALINE 256
#define FLASH_NO_ENCRY_FLAG    0x3C7896E1
#define FLASHBOOT_RAM_ADDR     0xA28000

#define FLASH_BOOT_TYPE_REG      0x40000024
#define FLASH_BOOT_TYPE_REG_MAIN 0xA5A5A5A5
#define FLASH_BOOT_TYPE_REG_BKUP 0x5A5A5A5A

const sfc_flash_config_t sfc_cfg = {
    .read_type = FAST_READ_QUAD_OUTPUT,
    .write_type = PAGE_PROGRAM,
    .mapping_addr = 0x200000,
    .mapping_size = 0x800000,
};

static uint32_t sfc_flash_init(void)
{
    return uapi_sfc_init((sfc_flash_config_t *)&sfc_cfg);
}
static uint32_t sfc_flash_read(uint32_t flash_addr, uint32_t read_size, uint8_t *read_buffer)
{
    return uapi_sfc_reg_read(flash_addr, read_buffer, read_size);
}
static uint32_t sfc_flash_write(uint32_t flash_addr, uint32_t flash_write_size, const uint8_t *p_flash_write_data,
                                bool do_erase)
{
    unused(do_erase);
    return uapi_sfc_reg_write(flash_addr, (uint8_t *)p_flash_write_data, flash_write_size);
}

static uint32_t sfc_flash_erase(uint32_t flash_addr, uint32_t flash_erase_size)
{
    return uapi_sfc_reg_erase(flash_addr - sfc_cfg.mapping_addr, flash_erase_size);
}

static void boot_flash_init(void)
{
    flash_cmd_func flash_funcs = {0};
    flash_funcs.init = sfc_flash_init;
    flash_funcs.read = sfc_flash_read;
    flash_funcs.write = sfc_flash_write;
    flash_funcs.erase = sfc_flash_erase;
    boot_regist_flash_cmd(&flash_funcs);

    uint32_t  ret = sfc_flash_init();
    if (ret != ERRCODE_SUCC) {
        boot_msg1("Flash Init Fail! ret = ", ret);
    } else {
        boot_msg0("Flash Init Succ!");
    }
    switch_flash_clock_to_pll();
    config_sfc_ctrl_ds();
}

#define FAMA_REMAP_SRC_BASE_ADDR 0x44007800
#define FAMA_REMAP_LEN_BASE_ADDR 0x44007820
#define FAMA_REMAP_DST_BASE_ADDR 0x44007840
#define FAMA_REMAP_REGION_OFFSET 0x4
#define FAMA_REMAP_LOW_BITS 12
static void dmmu_set(uint32_t src_start_addr, uint32_t src_end_addr, uint32_t dst_start_addr, uint32_t region)
{
    uint32_t src_reg_addr = FAMA_REMAP_SRC_BASE_ADDR + region * FAMA_REMAP_REGION_OFFSET;
    uint32_t len_reg_addr = FAMA_REMAP_LEN_BASE_ADDR + region * FAMA_REMAP_REGION_OFFSET;
    uint32_t dst_reg_addr = FAMA_REMAP_DST_BASE_ADDR  + region * FAMA_REMAP_REGION_OFFSET;
    uint32_t src_start_align_addr = src_start_addr  >> FAMA_REMAP_LOW_BITS;
    uint32_t src_end_align_addr = src_end_addr >> FAMA_REMAP_LOW_BITS;
    uint32_t dst_start_align_addr = dst_start_addr >> FAMA_REMAP_LOW_BITS;
    uint32_t dst_src_offset = 0;
    if (region >= FAMA_REMAP_LOW_BITS) {
        return ;
    }
    writel(src_reg_addr, src_start_align_addr);
    writel(len_reg_addr, src_end_align_addr);
    if (src_start_align_addr > dst_start_align_addr) {
        dst_src_offset = src_start_align_addr - dst_start_align_addr;
        dst_src_offset = ~dst_src_offset;
        dst_src_offset = dst_src_offset + 1;
    } else {
        dst_src_offset = dst_start_align_addr - src_start_align_addr;
    }
    writel(dst_reg_addr, dst_src_offset);
}

static bool flashboot_need_recovery(void)
{
    uint32_t reg = readl(FLASH_BOOT_TYPE_REG);
    writel(FLASH_BOOT_TYPE_REG, 0);
    return (reg == FLASH_BOOT_TYPE_REG_BKUP) ? true : false;
}

static void ws63_flashboot_recovery(void)
{
    if (!flashboot_need_recovery()) {
        return;
    }
    uapi_watchdog_kick();
    boot_msg0("Flashboot backup is working!");
    partition_information_t src_img_info = {0};
    partition_information_t dst_img_info = {0};
    errcode_t ret;
    ret = uapi_partition_get_info(PARTITION_FLASH_BOOT_IMAGE_BACKUP, &src_img_info);
    ret |= uapi_partition_get_info(PARTITION_FLASH_BOOT_IMAGE, &dst_img_info);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("Flashboot partition info get fail!");
        return;
    }

    ret = uapi_sfc_reg_erase(dst_img_info.part_info.addr_info.addr, dst_img_info.part_info.addr_info.size);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("flashboot recovery erase failed!! ret = ", ret);
    }
    ret = uapi_sfc_reg_write(dst_img_info.part_info.addr_info.addr,
        (uint8_t *)(src_img_info.part_info.addr_info.addr + FLASH_START), src_img_info.part_info.addr_info.size);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("flashboot recovery write failed!! ret = ", ret);
    }
    boot_msg0("Flashboot fix ok!");
}

#define UPG_FIX_RETRY_CNT_REG   RESET_COUNT_REG
#define VERIFY_RETRY_CNT_THRES  0x3
#define UPG_FIX_RETRY_CNT_THRES (VERIFY_RETRY_CNT_THRES + 0x3)
#define UPG_AB_RETRY_CNT_THRES (VERIFY_RETRY_CNT_THRES + 0x3)

static uint8_t ws63_get_try_fix_cnt(void)
{
    gp_reg1_union gp;
    gp.u32 = readl(UPG_FIX_RETRY_CNT_REG);
    return gp.bits.fota_fix_app_cnt;
}

static void ws63_set_try_fix_cnt(uint8_t cnt)
{
    gp_reg1_union gp;
    gp.u32 = readl(UPG_FIX_RETRY_CNT_REG);
    gp.bits.fota_fix_app_cnt = cnt & 0xF;
    writel(UPG_FIX_RETRY_CNT_REG, gp.u32);
}

#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_AB
static void ws63_try_fix_app(void)
{
    uint8_t try_cnt = ws63_get_try_fix_cnt() + 1;
    ws63_set_try_fix_cnt(try_cnt);

    /* 启动分区连续验签失败次数等于阈值倍数时，切换启动分区 */
    if (try_cnt % VERIFY_RETRY_CNT_THRES) { // 1 2 4 5 7 8 a b d e
        return;
    } else { // 3 6 9 c f
        boot_msg0("switch booting");
        upg_set_run_region(upg_get_upg_region());
        return;
    }
}
#else // 压缩
/* 尝试ota修复运行区镜像 */
static void ws63_try_fix_app(void)
{
    uint8_t try_cnt = ws63_get_try_fix_cnt();
    /* 连续验签失败次数小于阈值不做处理 */
    if (try_cnt < VERIFY_RETRY_CNT_THRES) {
        ws63_set_try_fix_cnt(try_cnt + 1);
        return;
    }

    /* 修复app超次数阈值后不再尝试 */
    if (try_cnt >= UPG_FIX_RETRY_CNT_THRES) {
        return;
    }
    ws63_set_try_fix_cnt(try_cnt + 1);

    /* 重置升级标记 */
    errcode_t ret = uapi_upg_reset_upgrade_flag();
    if (ret != ERRCODE_SUCC) {
        boot_msg1("reset_upgrade_flag fail, ret = ", ret);
        return;
    }

    /* 请求升级 */
    ret = uapi_upg_request_upgrade(false);
    if (ret != ERRCODE_SUCC) {
        boot_msg0("request_upgrade fail, fota_pkt_not_exit.");
        return;
    }
    boot_msg0("fota_pkt exit, try_fota_fix_app.");
}
#endif

static bool ws63_upg_need_upgrade(void)
{
    uint32_t fota_address = 0;
    partition_information_t info;
    errcode_t ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC || info.type != PARTITION_BY_ADDRESS) {
        boot_msg1("uapi_partition_get_info failed ", __LINE__);
        return false;
    }
    upg_get_upgrade_flag_flash_start_addr(&fota_address);
    fota_upgrade_flag_area_t upg_flag_info;
    ret_val = upg_flash_read(fota_address, sizeof(fota_upgrade_flag_area_t), (uint8_t *)(&upg_flag_info));
    if (ret_val != ERRCODE_SUCC) {
        boot_msg1("upg_flash_read failed ", ret_val);
        return false;
    }
    if (!(upg_flag_info.head_magic == UPG_HEAD_MAGIC &&
          upg_flag_info.head_end_magic == UPG_END_MAGIC &&
          upg_flag_info.complete_flag != 0)) {
        /* 不需要升级直接返回 */
        boot_msg0("No need to upgrade...");
        return false;
    }
    return true;
}

static void ws63_upg_check(void)
{
    (void)ws63_upg_init();
#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_AB
    return;
#endif
    if (ws63_upg_need_upgrade()) {
        boot_msg0("need upgrade");
        errcode_t ret = uapi_upg_start();
        // 1, 没有升级包、或者升级标记区结果已经设置的情况不需要重启
        // 2, 升级模块没有初始化不需要重启
        // 3, 升级成功需要重启
        // 4, 升级失败的时候需要重启重新进入升级流程，尝试三次未成功，后会变为1的情况
        if (!(ret == ERRCODE_UPG_NOT_NEED_TO_UPDATE || ret == ERRCODE_UPG_NOT_INIT || ret == ERRCODE_SUCC)) {
            boot_msg0("--------------------------");
            boot_msg0("upgrade failed, reset now");
        } else {
            // 升级后重启
            boot_msg0("--------------------------");
            boot_msg0("upgrade success, reset now");
        }
        reset();
    }
}

static uint32_t ws63_flashboot_init(void)
{
    errcode_t err;
    uart_param_stru uart_param = FLASHBOOT_UART_DEFAULT_PARAM;
    uapi_tcxo_init();
    hiburn_uart_init(uart_param, HIBURN_CODELOADER_UART);
    boot_msg0("Flashboot Uart Init Succ!");
    uapi_partition_init();
    pmp_enable();
    malloc_port_init();
    boot_msg0("Flashboot Malloc Init Succ!");
    boot_flash_init();
    err = sfc_port_fix_sr();
    if (err != ERRCODE_SUCC) {
        boot_msg1("SFC fix SR ret =", err);
    }
    print_str("flashboot version : %s\r\n", SDK_VERSION);
    return 0;
}

static errcode_t ws63_verify_app(uint32_t addr)
{
    errcode_t ret = ERRCODE_SUCC;
    image_key_area_t *flashboot_key_area = (image_key_area_t *)(FLASHBOOT_RAM_ADDR);

    ret = verify_image_head(APP_BOOT_TYPE, (uint32_t)flashboot_key_area->ext_pulic_key_area, addr);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("flashboot verify_image_app_head failed!! ret = ", ret);
        return ret;
    }

    ret = verify_image_body(addr, addr + APP_IMAGE_HEADER_LEN);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("verify_image_app_body failed!! ret = ", ret);
        return ret;
    }
    return ERRCODE_SUCC;
}

static void ws63_verify_app_handle(uint32_t addr)
{
    errcode_t ret = ws63_verify_app(addr);
    if (ret != ERRCODE_SUCC) {
        ws63_try_fix_app();
        reset();
    }
    set_reset_count(0);
    ws63_set_try_fix_cnt(0);
}

static void ws63_flash_encrypt_config(uint32_t image_addr, uint32_t image_size)
{
    image_code_info_t *img_info = (image_code_info_t *)(uintptr_t)(image_addr + sizeof(image_key_area_t));
    crypto_klad_effective_key flash_key = {0};
    uint32_t start_addr = image_addr + APP_IMAGE_HEADER_LEN;
    uint32_t end_addr = image_addr + image_size;
    int32_t ret = ERRCODE_SUCC;

    if (img_info->code_enc_flag == FLASH_NO_ENCRY_FLAG) {
        boot_msg0("flash_encrypt disable.");
        return;
    }

    if (start_addr % FLASH_ENCRY_ADDR_ALINE != 0 || end_addr % FLASH_ENCRY_ADDR_ALINE != 0) {
        boot_msg2("app_image start or end addr err, must 256byte alignment ", image_addr, end_addr);
        reset();
    }
    boot_msg0("flash_encrypt enable.");
    flash_key.kdf_hard_alg = CRYPTO_KDF_HARD_ALG_SHA256;
    flash_key.key_parity = TD_FALSE;
    flash_key.key_size = CRYPTO_KLAD_KEY_SIZE_128BIT;
    flash_key.salt = img_info->protection_key_l1;
    flash_key.salt_length = FLASH_KEY_SALT_LEN;
    flash_key.oneway = TD_TRUE;
    ret = drv_rom_cipher_config_odrk1(flash_key);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("fapc_set_config drv_rom_cipher_config_odrk1 err = ", (uint32_t)ret);
        reset();
    }
    ret = drv_rom_cipher_fapc_config(0, start_addr, end_addr, img_info->iv, IV_LEN);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("fapc_set_config drv_rom_cipher_fapc_config err = ", (uint32_t)ret);
        reset();
    }

    ret = drv_rom_cipher_fapc_bypass_config(1, end_addr, FLASH_MAX_END, TD_TRUE);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("fapc_set_config drv_rom_cipher_fapc_bypass_config err = ", (uint32_t)ret);
        reset();
    }
}

static uint32_t ws63_ftm_mode_init(uint32_t image_addr)
{
    uint32_t image_size = 0;
    uint32_t jump_addr = 0;
    mfg_factory_config_t mfg_factory_cfg = {0};
    uint32_t run_region = mfg_get_ftm_run_region(&mfg_factory_cfg);
    if (run_region == FTM_REGION_SERVICE) {
        return 0;
    }
    jump_addr = mfg_factory_cfg.factory_addr_start;
    image_size = mfg_factory_cfg.factory_size;
    image_key_area_t *mfg_key_area = (image_key_area_t *)(jump_addr);

    if (mfg_key_area->image_id == FACTORYBOOT_KEY_AREA_IMAGE_ID && mfg_factory_cfg.factory_valid == MFG_FACTORY_VALID) {
        dmmu_set(image_addr, image_addr + image_size, jump_addr, 0);
    }
    return 0;
}

/* the entry of C. */
void start_fastboot(void)
{
    partition_information_t img_info = {0};
    uint32_t image_addr = 0;
    uint32_t image_size = 0;
    errcode_t err;

    // 关闭CMU假负载
    uapi_reg_setbits(REG_CMU_CFG0, 3, 3, 0x7);  // 0x7 -> 0x40003408 bit 5:3 (3 bits)

    boot_clock_adapt();
    dmmu_set(0, 0, 0, FAMA_REMAP_LOW_BITS);
    uapi_watchdog_init(BOOT_WATCH_DOG_TIMOUT);
    uapi_watchdog_enable(WDT_MODE_RESET);
    ws63_flashboot_init();
    set_efuse_period();
    uapi_efuse_init();
    ws63_upg_check(); // 升级模式判断
    err = uapi_partition_get_info(PARTITION_APP_IMAGE, &img_info);
    if (err != ERRCODE_SUCC) {
        boot_msg0("Flashboot get app partition failed!, boot abort!");
        reset();
    }
    image_addr = img_info.part_info.addr_info.addr + FLASH_START;
    image_size = img_info.part_info.addr_info.size;
#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_AB
    char *boot_str = "A";
    uint32_t jump_addr = upg_get_region_addr(upg_get_run_region()) + FLASH_START;
    *boot_str = (image_addr == jump_addr) ? 'A' : 'B';
    boot_msg0(boot_str);
    ws63_flash_encrypt_config(jump_addr, image_size); // flash在线加解密配置
    ws63_verify_app_handle(jump_addr); // A/B验签
    if (image_addr != jump_addr) {
        dmmu_set(image_addr, image_addr + image_size, jump_addr, 0);
    }
#else // 压缩
    ws63_flash_encrypt_config(image_addr, image_size); // flash在线加解密配置
    ws63_verify_app_handle(image_addr); // app验签
#endif
    ws63_ftm_mode_init(image_addr);
    ws63_flashboot_recovery();
    uapi_watchdog_kick();
    jump_to_execute_addr(image_addr + APP_IMAGE_HEADER_LEN);
}
