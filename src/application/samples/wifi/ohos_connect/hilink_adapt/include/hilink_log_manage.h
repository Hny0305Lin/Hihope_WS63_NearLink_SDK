/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLink日志级别管理（此文件为DEMO，需集成方适配修改）
 */

#ifndef _HILINK_LOG_MANAGE_H_
#define _HILINK_LOG_MANAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HILINK_LOG_INVALID = -1, /* 非法值 */
    HILINK_LOG_MIN     = 0,
    HILINK_LOG_EMERG   = 0, /* 非常紧急, 系统不可用 */
    HILINK_LOG_ALERT   = 1, /* 紧急级别, 必须立即采取措施 */
    HILINK_LOG_CRIT    = 2, /* 致命级别 */
    HILINK_LOG_ERR     = 3, /* 错误级别 */
    HILINK_LOG_WARN    = 4, /* 告警级别 */
    HILINK_LOG_NOTICE  = 5, /* 通知级别 */
    HILINK_LOG_INFO    = 6, /* 信息级别 */
    HILINK_LOG_DEBUG   = 7, /* 调试级别 */
    HILINK_LOG_MAX     = HILINK_LOG_DEBUG
} HiLinkLogLevel;

/*
 * 配置当前日志打印最高级别, release版本默认为HILINK_LOG_ERR, debug版本默认为HILINK_LOG_DEBUG
 * 例如配置的日志级别为HILINK_LOG_ERR, 表示级别在HILINK_LOG_ERR以下(HILINK_LOG_WARN等)的日志不会打印出来
 */
void HILINK_SetLogLevel(HiLinkLogLevel level);

/*
 * 查询当前日志打印最高级别
 * 例如查询的日志级别为HILINK_LOG_ERR, 表示级别在HILINK_LOG_ERR以下(HILINK_LOG_WARN等)的日志不会打印出来
 */
HiLinkLogLevel HILINK_GetLogLevel(void);

#ifdef __cplusplus
}
#endif

#endif /* _HILINK_LOG_MANAGE_H_ */
