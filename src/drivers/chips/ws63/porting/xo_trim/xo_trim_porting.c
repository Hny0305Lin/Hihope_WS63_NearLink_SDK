/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides tcxo trim port \n
 *
 * History: \n
 * 2024-01-31， Create file. \n
 */
#include "xo_trim_porting.h"
#include "efuse.h"
#include "efuse_porting.h"
#include "clock_init.h"
#include "chip_io.h"
#include "soc_osal.h"
#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
#include "nv.h"
#include "tsensor.h"
#endif

static void cmu_xo_trim_reg_set(uint32_t fine, uint32_t coarse)
{
    u_cmu_xo_sig val;

    val.u32 = readl(CMU_XO_SIG);
    val.bits.rg_cmu_xo_trim_coarse = coarse;
    val.bits.rg_cmu_xo_trim_fine = fine;
    val.bits.rg_cmu_xo_trim_coarse_sel = XO_TRIM_ENABLE;
    val.bits.rg_cmu_xo_trim_fine_sel = XO_TRIM_ENABLE;
    writel(CMU_XO_SIG, val.u32);
}

void cmu_xo_trim_init(void)
{
    uint8_t lock = 0;
    uint16_t value = 0;
    int index;
    uint8_t writable = false;
    uint32_t fine = 0;
    uint32_t coarse = 0;
    uint32_t id[EFUSE_GROUP_MAX] = {EXT_EFUSE_XO_TRIM_1_ID, EXT_EFUSE_XO_TRIM_2_ID, EXT_EFUSE_XO_TRIM_3_ID};
    uint32_t lock_id[EFUSE_GROUP_MAX] = {
        EXT_EFUSE_RSSI_BAND3_1_ID, EXT_EFUSE_RSSI_BAND3_2_ID, EXT_EFUSE_RSSI_BAND3_3_ID
    };

    for (index = (EFUSE_GROUP_MAX - 1); index >= 0; --index) {
        if (uapi_efuse_read_bit(&lock, lock_id[index], EXT_EFUSE_LOCK_XO_TRIM_BIT_POS) != SUCC) {
            return;
        }
        writable = (lock == 0) ? false : true;
        if (writable == false) {
            continue;
        }
        if (uapi_efuse_read_buffer((uint8_t *)&value, id[index], sizeof(value)) != SUCC) {
            return;
        }
        break;
    }
    if (writable == false) {
        return;
    }
    value = (value & 0xFFF);
    fine = (value & 0xFF);
    coarse = ((value >> SIZE_8_BITS) & 0xF);
    cmu_xo_trim_reg_set(fine, coarse);
    return;
}
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
#define CMU_XO_TRIM_TEMP_PARAM_NUM 8    // 8个温度档位值(fine值)(~-20,0,20,40,60,80,100,100~)
#define CMU_XO_TRIM_TEMP_TIMER_LEN 2000 // 2000ms
#define CMU_XO_TRIM_TEMP_LOW (-40)      // 频偏粗调温补使用温度范围的最低温 用于档位计算
#define CMU_XO_TRIM_TEMP_GAP (20)       // 温度档位的间隔20度
#define CMU_XO_TRIM_TEMP_CODE (3)       // 温度档位默认值 对应20~40度
#define CMU_XO_TRIM_FINE_CODE_MAX (127) // 最大可调的fine code值
osal_timer g_xo_trim_temp_timer;
int8_t g_nv_xo_trim_temp_fine_code[CMU_XO_TRIM_TEMP_PARAM_NUM] = {0};
static void cmu_xo_trim_temp_comp_timer_proc(uint32_t para)
{
    int32_t ret = SUCC;
    static uint8_t last_temp_code = 255;
    int8_t cur_temp = 25; // 常温
    uint8_t temp_code = CMU_XO_TRIM_TEMP_CODE;
    if (uapi_tsensor_get_current_temp(&cur_temp) == SUCC) {
        if (cur_temp < CMU_XO_TRIM_TEMP_LOW) {
            cur_temp = CMU_XO_TRIM_TEMP_LOW;
        }
        temp_code = (uint8_t)((cur_temp - CMU_XO_TRIM_TEMP_LOW) / CMU_XO_TRIM_TEMP_GAP);
        if (temp_code >= CMU_XO_TRIM_TEMP_PARAM_NUM) {
            temp_code = CMU_XO_TRIM_TEMP_PARAM_NUM - 1;
        }
        // 确保上一次的温度码有效 并且当前温度码与上一次不同 执行fine code的刷新
        if (last_temp_code != temp_code) {
            u_cmu_xo_sig val;
            val.u32 = para;
            int32_t new_fine = (int32_t)val.bits.rg_cmu_xo_trim_fine + g_nv_xo_trim_temp_fine_code[temp_code];
            new_fine = (new_fine < 0) ? 0 : (new_fine > CMU_XO_TRIM_FINE_CODE_MAX) ?
                CMU_XO_TRIM_FINE_CODE_MAX : new_fine;
            val.bits.rg_cmu_xo_trim_fine = (uint32_t)new_fine; // 取值范围0-127
            writel(CMU_XO_SIG, val.u32);
            last_temp_code = temp_code;
#ifdef SW_UART_DEBUG
            osal_printk("xo update temp:%u,diff:%d,xo:0x%x\r\n",
                temp_code, g_nv_xo_trim_temp_fine_code[temp_code], val.u32);
#endif
        }
    }
    ret = osal_timer_mod(&g_xo_trim_temp_timer, CMU_XO_TRIM_TEMP_TIMER_LEN);
    if (ret != SUCC) {
        osal_printk("xo_trim_temp_comp timer mod err ret:%u!\r\n", ret);
    }
}
void cmu_xo_trim_temp_comp_init(void)
{
    uint32_t ret;
    uint8_t sw = 0;
    uint16_t nv_param_len = 0;
    if (uapi_nv_read(NV_ID_SYSTEM_XO_TRIM_TEMP_SW, 1, &nv_param_len, &sw) != SUCC) {
        osal_printk("xo_trim_temp_comp::nv read sw fail!\r\n");
        return;
    }
    // 开关开启的情况下 启动定时器 查询温度 执行温补
    if (sw == 0) {
        return;
    }
    if (uapi_nv_read(NV_ID_SYSTEM_XO_TRIM_TEMP_PARAM, CMU_XO_TRIM_TEMP_PARAM_NUM, &nv_param_len,
        (uint8_t *)g_nv_xo_trim_temp_fine_code) != SUCC) {
        osal_printk("xo_trim_temp_comp::nv read param fail!\r\n");
        return;
    }
#ifdef SW_UART_DEBUG
    osal_printk("xo_trim_temp_comp val:%d %d\r\n", g_nv_xo_trim_temp_fine_code[0],
        g_nv_xo_trim_temp_fine_code[CMU_XO_TRIM_TEMP_PARAM_NUM - 1]);
#endif
    g_xo_trim_temp_timer.timer = NULL; /* 初始化需要置空 */
    g_xo_trim_temp_timer.handler = (void *)cmu_xo_trim_temp_comp_timer_proc;
    g_xo_trim_temp_timer.data = readl(CMU_XO_SIG);                     // 默认的频偏值
    g_xo_trim_temp_timer.interval = (CMU_XO_TRIM_TEMP_TIMER_LEN << 1); // 首次触发4s 后续2s间隔查询
    ret = (uint32_t)osal_timer_init(&g_xo_trim_temp_timer);
    ret |= (uint32_t)osal_timer_start(&g_xo_trim_temp_timer);
    if (ret != SUCC) {
        osal_printk("xo_trim_temp_comp timer err ret:%u!\r\n", ret);
    }
}
void cmu_xo_trim_temp_comp_print(void)
{
    uint32_t i;
    uint8_t sw = 0;
    uint16_t nv_param_len = 0;
    if (uapi_nv_read(NV_ID_SYSTEM_XO_TRIM_TEMP_SW, 1, &nv_param_len, &sw) != SUCC) {
        osal_printk("xo_trim_temp_comp::nv read sw fail!\r\n");
        return;
    }
    // 开关开启的情况下 启动定时器 查询温度 执行温补
    if (sw == 0) {
        osal_printk("xo_trim_temp_comp::switch off!\r\n");
        return;
    }
    for (i = 0; i < CMU_XO_TRIM_TEMP_PARAM_NUM - 1; i++) {
        osal_printk("TEMP_COMP_RANGE[%u] : [%d, %d], val : [%d]\r\n", i,
            CMU_XO_TRIM_TEMP_LOW + i * CMU_XO_TRIM_TEMP_GAP, CMU_XO_TRIM_TEMP_LOW + (i + 1) * CMU_XO_TRIM_TEMP_GAP,
            g_nv_xo_trim_temp_fine_code[i]);
    }
    osal_printk("TEMP_COMP_RANGE[%u] : [%d, ~ ], val : [%d]\r\n", i,
        CMU_XO_TRIM_TEMP_LOW + i * CMU_XO_TRIM_TEMP_GAP, g_nv_xo_trim_temp_fine_code[i]);
}
#endif