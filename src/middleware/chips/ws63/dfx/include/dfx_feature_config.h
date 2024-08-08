/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: dfx feature config file
 * This file should be changed only infrequently and with great care.
 */

#ifndef DFX_FEATURE_CONFIG_H
#define DFX_FEATURE_CONFIG_H

#define DFX_YES     1
#define DFX_NO      0

/* 特性:是否支持文件传输 */
#ifndef CONFIG_DFX_SUPPORT_TRANSMIT_FILE
#define CONFIG_DFX_SUPPORT_TRANSMIT_FILE            DFX_NO
#endif

/* 特性:是否支持文件传输回调 */
#ifndef CONFIG_DFX_SUPPORT_TRANSMIT_FILE_HOOK
#define CONFIG_DFX_SUPPORT_TRANSMIT_FILE_HOOK       DFX_YES
#endif

/* 特性:是否支持断点续传 */
#ifndef CONFIG_DFX_SUPPORT_CONTINUOUSLY_TRANSMIT
#define CONFIG_DFX_SUPPORT_CONTINUOUSLY_TRANSMIT    DFX_NO
#endif

/* 特性:是否支持DIAG */
#ifndef CONFIG_DFX_SUPPORT_DIAG
#define CONFIG_DFX_SUPPORT_DIAG                     DFX_YES
#endif

/* 特性:是否支持DIAG接收数据包 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_RX_PKT
#define CONFIG_DFX_SUPPORT_DIAG_RX_PKT              DFX_YES
#endif

/* 特性:是否支持DIAG分帧和组包 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_MULTI_FRAME
#define CONFIG_DFX_SUPPORT_DIAG_MULTI_FRAME         DFX_NO
#endif

/* 特性:是否支持串口连接UART */
#ifndef CONFIG_DFX_SUPPORT_DIAG_UART_CHANNEL
#define CONFIG_DFX_SUPPORT_DIAG_UART_CHANNEL        DFX_YES
#endif

/* 特性:DIAG作为上位机处理IND */
#ifndef CONFIG_DFX_SUPPORT_DIAG_UP_MACHINE
#define CONFIG_DFX_SUPPORT_DIAG_UP_MACHINE          DFX_NO
#endif

/* 特性:DIAG连接支持心跳机制 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_BEAT_HEART
#define CONFIG_DFX_SUPPORT_DIAG_BEAT_HEART          DFX_YES
#endif

/* 特性: 支持密码连接 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_CONNECT_PASSWORD
#define CONFIG_DFX_SUPPORT_DIAG_CONNECT_PASSWORD    DFX_YES
#endif

/* 特性: DIAG支持的日志类型 */
/* 精简型日志 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_MINIMAL_MSG
#define CONFIG_DFX_SUPPORT_DIAG_MINIMAL_MSG         DFX_NO
#endif
/* 标准型日志 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_NORMAL_MSG
#define CONFIG_DFX_SUPPORT_DIAG_NORMAL_MSG          DFX_NO
#endif
/* 扩展型日志(毫秒精度时间戳) */
#ifndef CONFIG_DFX_SUPPORT_DIAG_EXTEND_MSG
#define CONFIG_DFX_SUPPORT_DIAG_EXTEND_MSG          DFX_NO
#endif
/* 完整型日志 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_FULL_MSG
#define CONFIG_DFX_SUPPORT_DIAG_FULL_MSG            DFX_YES
#endif

#if ((CONFIG_DFX_SUPPORT_DIAG_MINIMAL_MSG + CONFIG_DFX_SUPPORT_DIAG_NORMAL_MSG + \
    CONFIG_DFX_SUPPORT_DIAG_EXTEND_MSG + CONFIG_DFX_SUPPORT_DIAG_FULL_MSG) != 1)
#error "SELLECT DIAG LOG TYPE ERROR"
#endif

/* 特性:支持虚拟AT */
#ifndef CONFIG_DFX_SUPPORT_DIAG_VIRTUAL_AT
#define CONFIG_DFX_SUPPORT_DIAG_VIRTUAL_AT          DFX_NO
#endif

/* 特性:支持虚拟SHELL */
#ifndef CONFIG_DFX_SUPPORT_DIAG_VRTTUAL_SHELL
#define CONFIG_DFX_SUPPORT_DIAG_VRTTUAL_SHELL       DFX_YES
#endif

/* 特性: DIAG适配SOC LOG */
#ifndef CONFIG_DFX_SUPPORT_DIAG_ADAPT_SOC_LOG
#define CONFIG_DFX_SUPPORT_DIAG_ADAPT_SOC_LOG       DFX_NO
#endif

/* 特性: 支持SOC LOG */
#ifndef CONFIG_DFX_SUPPORT_SOC_LOG
#define CONFIG_DFX_SUPPORT_SOC_LOG                  DFX_NO
#endif

/* 特性: 系统状态监控 */
#ifndef CONFIG_DFX_SUPPORT_SYS_MONITOR
#define CONFIG_DFX_SUPPORT_SYS_MONITOR              DFX_NO
#endif

/* 特性: 延时重启 */
#ifndef CONFIG_DFX_SUPPORT_DELAY_REBOOT
#define CONFIG_DFX_SUPPORT_DELAY_REBOOT             DFX_NO
#endif

/* 特性: DIAG支持USB */
#ifndef CONFIG_DFX_SUPPORT_DIAG_CONNECT_USB
#define CONFIG_DFX_SUPPORT_DIAG_CONNECT_USB         DFX_NO
#endif

/* 特性: DIAG支持蓝牙连接 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_CONNECT_BLE
#define CONFIG_DFX_SUPPORT_DIAG_CONNECT_BLE         DFX_NO
#endif

/* 特性: 支持文件系统 */
#ifndef CONFIG_DFX_SUPPORT_FILE_SYSTEM
#define CONFIG_DFX_SUPPORT_FILE_SYSTEM              DFX_NO
#endif

/* 特性: 支持日志离线存储到本地 */
#ifndef CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE
#define CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE         DFX_NO
#endif

/* 特性: 只有DIAG日志离线存储到本地 */
#ifndef CONFIG_DFX_STORE_DIAG_ONLY
#define CONFIG_DFX_STORE_DIAG_ONLY                  DFX_NO
#endif

/* DFX模块内存统计 */
#ifndef CONFIG_DFX_MEMORY_DEBUG
#define CONFIG_DFX_MEMORY_DEBUG                     DFX_YES
#endif

/* DFX模块HSO内存操作 */
#ifndef CONFIG_DFX_MEMORY_OPERATE
#define CONFIG_DFX_MEMORY_OPERATE                   DFX_YES
#endif

/* DFX模块SDT内存操作 */
#ifndef CONFIG_DFX_SDT_MEMORY_OPERATE
#define CONFIG_DFX_SDT_MEMORY_OPERATE               DFX_YES
#endif

/* DFX模块SDT dump异常弹窗 */
#ifndef CONFIG_DFX_SDT_DUMP_EXCEPTION
#define CONFIG_DFX_SDT_DUMP_EXCEPTION               DFX_YES
#endif

/* DFX模块BT采样操作 */
#ifndef CONFIG_DFX_BT_SAMPLE_DATA
#define CONFIG_DFX_BT_SAMPLE_DATA                   DFX_YES
#endif

/* DFX模块统计信息 */
#ifndef CONFIG_DFX_STATS
#define CONFIG_DFX_STATS                           DFX_YES
#endif

/* DFX模块应答处理 */
#ifndef CONFIG_DFX_IND_HANDLE
#define CONFIG_DFX_IND_HANDLE                       DFX_YES
#endif

#endif /* DFX_FEATURE_CONFIG_H */