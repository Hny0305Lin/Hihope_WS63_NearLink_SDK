/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_device_rom.c.
 */

#ifndef HAL_DEVICE_ROM_H
#define HAL_DEVICE_ROM_H

/*****************************************************************************
    头文件包含
*****************************************************************************/
#include "osal_types.h"
#include "hal_common_ops_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define WLAN_HAL_OHTER_BSS_ID                   6   /* 其他BSS的广播ID */
#define WLAN_HAL_OTHER_BSS_OTHER_ID             7   /* 来自其他BSS的其他帧(组播、单播) */
/* P2P 中断统计 */
typedef struct {
    osal_u32 ct_window_end_count;
    osal_u32 noa_absent_start_count;
    osal_u32 noa_absent_end_count;
    osal_u32 absent_start_count;
    osal_u32 absent_end_count;
} hal_p2p_ps_stat_stru;

/* 设备级 中断统计信息，!!!!本结构中修改任何一个成员都需要通知SDT同步修改，否则解析错误!!!! */
typedef struct {
    osal_u32 radar_detect_cnt; /* 检测到雷达 */
    /* P2P 节能统计 */
    hal_p2p_ps_stat_stru p2p_stat;
} hal_irq_device_stats_stru;

typedef struct tag_hal_hal_device_stru {
    hal_to_dmac_device_stru hal_device_base;
    /* device上挂的指针，使用时需要强转(后续需要使用宏替代，是不是指链表模板) */
    hal_vap_stru *vap_list[HAL_MAX_VAP_NUM];
    hal_irq_device_stats_stru irq_stats;

    osal_u8 cur_cali_chn; /* 当前校准的通道 */
    osal_u8 cali_data_received  : 1, /* device是否收到host下发的校准数据 */
          dbdc_cali           : 1,
          noa_type            : 2, /* 表示软件模拟触发一次雷达中断 */
          wur_ac : 4;
    osal_u8 vap_num; /* device下的vap的数量 */
    osal_u8 wur_ap_vap_id : 4, /* wur在ap模式下使用，指示当前ap的vap id */
            wur_twbtt_vap_id : 4; /* 指示wur的twbtt功能借用了哪个vap的TSF能力 */

    uintptr_t *_rom; /* ROM化后资源扩展指针 */
} hal_device_stru;

typedef struct {
    volatile oal_bool_enum_uint8 fcs_done;
} hal_fcs_mgr_stru;

/*****************************************************************************
    函数声明
*****************************************************************************/
osal_void hal_alg_isr_hook_run(osal_u8 vap_id, hal_device_stru *hal_device, hal_isr_type_enum_uint8 isr_type);
osal_u8 hal_device_calc_up_vap_num(const hal_to_dmac_device_stru *hal_device);
osal_u8 hal_get_hal_fcs_done(osal_void);
osal_void hal_set_hal_fcs_done(osal_u8 param);
osal_void hal_device_state_idle_set_rx_filter_reg(const hal_to_dmac_device_stru *hal_device);
osal_void hal_device_reset_bcn_rf_chain(hal_to_dmac_device_stru *hal_device, osal_u8 up_vap_num);
osal_u32 hal_device_update_phy_chain(hal_to_dmac_device_stru *hal_device, oal_bool_enum_uint8 set_channel);
osal_void hal_device_state_scan_set_rx_filter_reg(const hal_to_dmac_device_stru *hal_device);
osal_void hal_device_state_work_set_rx_filter_reg(hal_to_dmac_device_stru *hal_device);
oal_bool_enum_uint8 hal_device_find_is_ap_up(hal_to_dmac_device_stru *hal_device);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_device_rom.h */
