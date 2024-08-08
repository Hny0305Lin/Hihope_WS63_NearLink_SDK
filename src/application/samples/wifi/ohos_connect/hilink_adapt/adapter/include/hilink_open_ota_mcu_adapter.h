/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: MCU OTA适配层接口（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_OPEN_OTA_MCU_ADAPTER_H
#define HILINK_OPEN_OTA_MCU_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

/* 返回正常 */
#ifndef RETURN_OK
#define RETURN_OK               0
#endif

/* 没有MCU */
#ifndef RETURN_ERROR_NO_MCU
#define RETURN_ERROR_NO_MCU     (-1)
#endif

/* MCU返回其他错误 */
#ifndef RETURN_ERROR
#define RETURN_ERROR            (-2)
#endif

/* MCU不需要升级 */
#ifndef RETURN_MCU_NO_NEED_OTA
#define RETURN_MCU_NO_NEED_OTA  (-3)
#endif

/* 手动升级 */
#ifndef UPDATE_TYPE_MANUAL
#define UPDATE_TYPE_MANUAL      0x00
#endif

/* 自动升级 */
#ifndef UPDATE_TYPE_AUTO
#define UPDATE_TYPE_AUTO        0x01
#endif

/* 开始发送MCU数据 */
#ifndef START_SEND_DATA
#define START_SEND_DATA         0
#endif

/* 发送MCU数据结束 */
#ifndef STOP_SEND_DATA
#define STOP_SEND_DATA          1
#endif

/* 异常终止 */
#ifndef SEND_DATA_ERROR
#define SEND_DATA_ERROR         2
#endif

typedef int (*RptUpgProgressFlagCb)(void);

/*
 * 获取MCU当前版本
 * version表示版本字符串
 * inLen表示输入字符串长度
 * outLen表示输出字符串长度
 * 返回值是RETURN_OK时，表示获取成功
 * 返回值是RETURN_ERROR_NO_MCU时，表示没有MCU
 * 返回值是RETURN_ERROR时，表示获取失败
 * 注意：如果获取不到MCU的版本，则不对MCU进行升级。
 * 建议厂商在MCU正常启动后，或升级启动后，就将MCU的版本号传递给模组，确保模组可以获取到MCU的版本。
 */
int HILINK_GetMcuVersion(char *version, unsigned int inLen, unsigned int *outLen);

/*
 * HiLink SDK调用厂商适配的此接口通知MCU固件传输的状态
 * flag表示升级流程标志
 * 当flag是START_SEND_DATA时，表示通知模组即将开始发送MCU固件数据包
 * 当flag是STOP_SEND_DATA时，表示通知模组完整的MCU固件包已发送完成
 * 当flag是SEND_DATA_ERROR时，表示通知模组本次MCU固件升级异常终止
 * len表示MCU固件包的大小
 * type表示升级类型
 * 当type是UPDATE_TYPE_MANUAL时，表示本次升级流程是由用户主动发起的手动升级
 * 当type是UPDATE_TYPE_AUTO时，表示本次升级流程是经过用户同意的自动升级
 * 返回值是RETURN_OK时，表示处理成功，HiLink SDK继续正常处理后续流程
 * 返回值是RETURN_ERROR时，表示处理失败，HiLink SDK将终止本次MCU升级流程
 * 注意：当flag是STOP_SEND_DATA时，此接口需返回MCU侧固件升级的结果；当flag是其它值时，需返回接口接收到此消息后的处理结果。
 * 开机后10分钟到1小时内随机时间检测一次是否有新版本，之后以当前时间为起点，23小时加1小时内随机值周期性检测新版本。
 * 如果用户打开了自动升级开关，检测到有新版本并且是可以重启的情况下，就进行新版本的下载，下载完成后自动重启。
 * 自动升级流程可能在凌晨进行，因此厂商在实现升级流程相关功能时，确保在升级的下载安装固件和重启设备时避免对用户产生
 * 影响，比如发出声音，光亮等。
 */
int HILINK_NotifyOtaStatus(int flag, unsigned int len, unsigned int type);

/*
 * HiLink SDK调用厂商适配的此接口通知厂商发送MCU固件文件数据
 * data表示发送的数据
 * len表示发送的数据的长度
 * offset表示发送的数据起始位置相对于完整固件包的偏移量
 * 此接口需要返回MCU接收这部分数据的处理结果
 * 返回值是RETURN_OK时，模组将通知的数据正确发送给MCU，且MCU正确处理发送的数据，HiLink SDK将继续正常处理后续流程
 * 返回值是RETURN_ERROR时，模组或MCU未能正常处理通知的MCU的固件文件数据，HiLink SDK将终止本次MCU固件升级流程
 */
int HILINK_NotifyOtaData(const unsigned char *data, unsigned int len, unsigned int offset);

/*
 * 产品OTA升级时下载升级包进度为100%后会重启, APP上显示设备重启中
 * 然后等设备起来后上报进度0, APP上才会显示升级成功
 * 通过回调函数cb来设置Hilink Sdk被拉起后是否立即向APP上报进度0
 * cb返回1时hilink sdk被拉起后不会立即上报进度0，
 * 上报时机由产品自己决定,调用hilink_ota_rpt_prg进行上报
 * cb返回0时hilink sdk被拉起后会立即上报进度0
 * 产品根据自身升级情况判断是否需要注册cb，如不注册则默认hilink sdk被拉起后会立即上报进度0
 */
void HILINK_PowerOnRptUpgProgress(RptUpgProgressFlagCb cb);

#ifdef __cplusplus
}
#endif
#endif /* HILINK_OPEN_OTA_BRIDGE_ADAPTER_H */