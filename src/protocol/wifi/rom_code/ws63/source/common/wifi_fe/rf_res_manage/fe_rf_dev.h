/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2023. All rights reserved.
 * Description: Header file of  RF resource management interfaces.
 */

#ifndef FE_RF_DEV_ROM_H
#define FE_RF_DEV_ROM_H
#include "fe_extern_if_device.h"
#include "frw_msg_rom.h"
#include "dmac_ext_if_rom.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef osal_u8 hal_rf_power_work_mode_uint8;

typedef struct hh503_cali_param_tag cali_param_stru;

/* hal rf device 结构体 */
typedef struct {
    osal_u8 rf_id;             /* rf device 编号 */
    osal_u8 pll_id;            /* 使用的PLL ID */
    osal_s16 tx_power_dc;      /* 当前通道下TX PWR的DC值，20M/40M/80M实测相差较小，公用使用一个 */
    hal_rf_chn_param chn_para; /* 工作信道参数 */
    osal_u32 rf_reg_base;       /* rf寄存器基地址 */
    osal_u32 rf_pll_reg_base;   /* rf PLL寄存器基地址 */
    osal_u32 rf_ctl_reg_offset; /* rf ctl寄存器偏移地址 */
    osal_u16 aus_base_txpwr_delt_value[WLAN_BAND_BUTT];
    osal_void *owner;                          /* 指向当前占用rf dev的hal device */
    cali_param_stru *cali_data;                /* 校准数据指针 */
    oal_bool_enum_uint8 is_calibrated;         /* rf是否已经校准 */
    hal_rf_power_work_mode_uint8 rf_work_mode; /* rf work状态的工作模式 */
    osal_u8 rf_power_state;
    osal_u8 cur_cali_upc;
    osal_u8 base_power[WLAN_BAND_BUTT]; /* 基准最大发射功率 */
    osal_u8 upc_idx2code[HAL_POW_UPC_LUT_NUM];
    osal_s16 curr_temperature; /* 保存当前温度 */
    osal_s16 upc_amend;
    oal_bool_enum_uint8 upc_code_tobe_updated; /* 是否需要更新upc_code的lut表 */
    osal_char dp_2g11b;
    osal_char dp_2gofdm20;
    osal_char dp_2gofdm40;
    hal_pwr_fit_para_stru *pwr_fit_para_2g;
    osal_u8 cur_rf_fsm_state;                               /* 当前rf状态机状态  */
    hal_dyn_cali_adj_type_enum_uint8 cali_pdet_adjust_flag; /* 动态校准调整状态标志 */
    osal_s16 delt_dbb_scale_dyn_val;
#ifdef _PRE_WLAN_ONLINE_DPD
    osal_u32 *dpd_system;
#endif
    uintptr_t *_rom; /* ROM化后资源扩展指针 */
} hal_rf_dev;

/* rf 资源管理结构体 */
typedef struct {
    hal_rf_dev *hal_rf_res[WLAN_RF_CHANNEL_NUMS]; /* 指向rf dev数组 */
    osal_u32 free_bitmap;                         /* 标记空闲的rf dev */
    osal_u8 master_rf_id;
    osal_u8 rf_num;                        /* rf dev的总数 */
    oal_bool_enum_uint8 comm_rf_ldo_state; /* rf公用的ldo是否打开 打开:true 关闭:false */
    oal_bool_enum_uint8 is_mimo;
    osal_u8 pll0_lock_state;
    osal_u8 pll1_lock_state;
    osal_u8 pll_use_cnt[WLAN_RF_PLL_NUMS]; /* RF对应的PLL的使用计数，对应枚举hal_rf_pll_user_enum_uint8 */
    osal_u8 rsv;
    uintptr_t *_rom; /* ROM化后资源扩展指针 */
} hal_rf_res_manager;

hal_rf_res_manager* hal_rf_get_res(osal_void);

typedef osal_void (*hal_rf_dev_ldo_ops_ptr)(osal_u8 rf_id);

/* rf dev work状态的工作模式 */
typedef enum {
    HAL_RF_WORK_MODE_RX_ONLY, /* 只开接收 */
    HAL_RF_WORK_MODE_TXRX,    /* 收发全开 */

    HAL_RF_WORK_MODE_BUTT
} hal_rf_work_mode_enum;

typedef struct {
    hal_rf_dev_ldo_ops_ptr  poweron;
    hal_rf_dev_ldo_ops_ptr  poweroff;
    hal_rf_dev_ldo_ops_ptr  tx;
    hal_rf_dev_ldo_ops_ptr  dis_tx;
} hal_rf_dev_ops;

hal_rf_dev_ops* hal_rf_get_dev_ops(osal_void);

/* PLL使用者标识枚举 */
typedef enum {
    HAL_RF_PLL_USER_NONE    = 0,
    HAL_RF_PLL_USER_C0      = 1,
    HAL_RF_PLL_USER_C1      = 2,
    HAL_RF_PLL_USER_MIMO    = 3,

    HAL_RF_PLL_USER_BUTT
} hal_rf_pll_user_enum;
typedef osal_u8 hal_rf_pll_user_enum_uint8;

typedef enum {
    HAL_RF_ALWAYS_POWER_ON,
    HAL_RF_SWITCH_ALL_LDO,
    HAL_RF_SWITCH_PA_PPA,

    HAL_RF_OPS_CFG_BUTT
} hal_rf_ops_cfg_enum;
typedef osal_u8 hal_rf_ops_cfg_uint8;

/* 设置是否同时打开两个rf的电源 */
static inline osal_void rf_res_set_is_mimo(osal_u8 rf_chain)
{
    unref_param(rf_chain);
    hal_rf_get_res()->is_mimo = OSAL_FALSE;
}


osal_u32 hal_rf_res_init(osal_void);
osal_void hal_rf_res_free(const hal_rf_dev *rf_dev);

osal_void hal_rf_res_alloc_by_id(hal_to_dmac_device_stru *device, osal_u8 rf_id, osal_u8 pll_id);

osal_void fe_rf_dev_enable_tx(osal_u8 rf_id);

osal_u8 fe_rf_fsm_get_state(osal_u8 rf_id);

osal_void hal_rf_fsm_set_state(hal_rf_dev *rf_dev, osal_u8 state);
osal_void hal_rf_fsm_set_state_byidx(osal_u8 rf_dev_idx, osal_u8 state);
osal_void hal_rf_fsm_work_entry(osal_u8 rf_id);

osal_void fe_rf_set_rf_channel(const hal_rf_chn_param *chn_para);
osal_void hal_rf_dev_disable_tx(osal_u8 rf_id);
osal_void hal_rf_dev_poweron(osal_u8 rf_id);
osal_void hal_rf_dev_poweroff(osal_u8 rf_id);

osal_void fe_rf_dev_sleep(osal_u8 rf_id);

osal_void fe_rf_proc_rf_awake(osal_void);
osal_void fe_rf_enable_rf_tx(osal_void);

osal_void fe_rf_proc_rf_sleep(osal_void);

osal_void fe_rf_disable_rf_tx(osal_void);

osal_u8 hal_get_rf_switch_cfg(osal_void);
osal_void hal_set_rf_switch_cfg(osal_u8 switch_cfg);
hal_rf_dev* fe_get_rf_dev(osal_u8 rf_id);

osal_u32 fe_initialize_rf_dev(void);
osal_u32 fe_reset_rf_dev(const hal_to_dmac_device_stru *hal_device);
osal_s32 fe_rf_proc_rf_sleep_msg(dmac_vap_stru *dmac_vap, frw_msg *msg);
osal_s32 hal_dev_rf_sleep_poweroff(dmac_vap_stru *dmac_vap, frw_msg *msg);
osal_s32 hal_dev_set_rf_option(dmac_vap_stru *dmac_vap, frw_msg *msg);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif