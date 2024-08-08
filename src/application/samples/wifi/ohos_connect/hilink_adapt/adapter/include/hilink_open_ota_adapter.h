/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: OTA适配层接口（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_OPEN_OTA_ADAPTER_H
#define HILINK_OPEN_OTA_ADAPTER_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* OTA升级分区1 */
#ifndef UPGRADE_FW_BIN1
#define UPGRADE_FW_BIN1         0x00
#endif

/* OTA升级分区2 */
#ifndef UPGRADE_FW_BIN2
#define UPGRADE_FW_BIN2         0x01
#endif

/* 只有MCU升级时立即重启 */
#ifndef RESTART_FLAG_NOW
#define RESTART_FLAG_NOW        0x01
#endif

/* 有模组时切换分区后再重启 */
#ifndef RESTART_FLAG_LATER
#define RESTART_FLAG_LATER      0x02
#endif

/* 模组可以重启 */
#ifndef MODULE_CAN_REBOOT
#define MODULE_CAN_REBOOT       0x01
#endif

/* 模组不能重启 */
#ifndef MODULE_CANNOT_REBOOT
#define MODULE_CANNOT_REBOOT    0x00
#endif

/* 手动升级 */
#ifndef UPDATE_TYPE_MANUAL
#define UPDATE_TYPE_MANUAL      0x00
#endif

/* 自动升级 */
#ifndef UPDATE_TYPE_AUTO
#define UPDATE_TYPE_AUTO        0x01
#endif

/* 返回正常 */
#ifndef RETURN_OK
#define RETURN_OK               0
#endif
/* 没有MCU */
#ifndef RETURN_ERROR_NO_MCU
#define RETURN_ERROR_NO_MCU     (-1)
#endif
/* 返回其他错误 */
#ifndef RETURN_ERROR
#define RETURN_ERROR            (-2)
#endif
/* MCU不需要升级 */
#ifndef RETURN_MCU_NO_NEED_OTA
#define RETURN_MCU_NO_NEED_OTA  (-3)
#endif

typedef void (*GetOtaVerCb)(const char *version);
typedef void (*TrigSelfUpdateCb)(const char *url, const unsigned int size);

/*
 * Flash初始化
 * 返回值是true时，表示初始化正常
 * 返回值是false时，表示初始化异常
 */
bool HILINK_OtaAdapterFlashInit(void);

/*
 * 判断需要升级的分区
 * 返回值是UPGRADE_FW_BIN1时，表示升级固件到分区1
 * 返回值是UPGRADE_FW_BIN2时，表示升级固件到分区2
 */
unsigned int HILINK_OtaAdapterGetUpdateIndex(void);

/*
 * 擦除需要升级的分区
 * size表示需要擦除的分区大小
 * 返回值是0时，表示擦除成功
 * 返回值是-1时，表示擦除失败
 */
int HILINK_OtaAdapterFlashErase(unsigned int size);

/*
 * 升级数据写入升级的分区
 * buf表示待写入数据
 * bufLen表示待写入数据的长度
 * 返回值是0时，表示写入成功
 * 返回值是-1时，表示写入失败
 */
int HILINK_OtaAdapterFlashWrite(const unsigned char *buf, unsigned int bufLen);

/*
 * 读取升级分区数据
 * offset表示读写偏移
 * buf表示输出数据的内存地址
 * bufLen表示输出数据的内存长度
 * 返回值是0时，表示读取成功
 * 返回值是-1时，表示读取失败
 */
int HILINK_OtaAdapterFlashRead(unsigned int offset, unsigned char *buf, unsigned int bufLen);

/*
 * 分区升级结束
 * 返回值是true时，表示结束正常
 * 返回值是false时，表示结束异常
 */
bool HILINK_OtaAdapterFlashFinish(void);

/* 获取升级区间最大长度 */
unsigned int HILINK_OtaAdapterFlashMaxSize(void);

/*
 * 根据标志重启模组
 * flag表示重启标志
 * 当flag是RESTART_FLAG_NOW时，表示只有MCU升级时立即重启
 * 当flag是RESTART_FLAG_LATER时，表示有模组时切换分区后再重启
 */
void HILINK_OtaAdapterRestart(int flag);

/*
 * 开始模组升级
 * type表示升级类型
 * 当type是UPDATE_TYPE_MANUAL时，表示本次升级流程是由用户主动发起的手动升级
 * 当type是UPDATE_TYPE_AUTO时，表示本次升级流程是经过用户同意的自动升级
 * 返回值是RETURN_OK时，表示处理成功，HiLink SDK将开始启动升级流程
 * 返回值是RETURN_ERROR时，表示处理不成功，HiLink SDK将终止本次升级流程
 * 注意：在手动场景场景下，HiLink SDK在接收到用户发出的升级指令后，将直接调用此接口；
 * 在自动升级场景下，当HiLink SDK在调用HilinkGetRebootFlag接口返回值是MODULE_CAN_REBOOT时，HiLink SDK将调用此接口。
 * 厂商可在此接口中完成和升级流程相关的处理。
 * 开机后10分钟到1小时内随机时间检测一次是否有新版本，之后以当前时间为起点，23小时加1小时内随机值周期性检测新版本。
 * 如果用户打开了自动升级开关，检测到有新版本并且是可以重启的情况下，就进行新版本的下载，下载完成后自动重启。
 * 自动升级流程可能在凌晨进行，因此厂商在实现升级流程相关功能时，确保在升级的下载安装固件和重启设备时避免对用户产生
 * 影响，比如发出声音，光亮等。
 */
int HILINK_OtaStartProcess(int type);

/*
 * 模组升级结束
 * status表示升级结果
 * 当status是100时，表示升级成功
 * 当status不是100时，表示升级失败
 * 返回值是RETURN_OK时，表示处理成功，HiLink SDK将置升级标志或切换运行区标志
 * 返回值不是RETURN_OK时，表示处理不成功，HiLink SDK将终止本次升级流程
 * 注意：HiLink SDK在将固件写入到OTA升级区后，且完整性校验通过后，将调用厂商适配的此接口；
 * 厂商可在此接口中完成和升级流程相关的处理。
 * 开机后10分钟到1小时内随机时间检测一次是否有新版本，之后以当前时间为起点，23小时加1小时内随机值周期性检测新版本。
 * 如果用户打开了自动升级开关，检测到有新版本并且是可以重启的情况下，就进行新版本的下载，下载完成后自动重启。
 * 自动升级流程可能在凌晨进行，因此厂商在实现升级流程相关功能时，确保在升级的下载安装固件和重启设备时避免对用户产生
 * 影响，比如发出声音，光亮等；升级类型是否为自动升级可参考接口HilinkOtaStartProcess的参数type的描述。
 */
int HILINK_OtaEndProcess(int status);

/*
 * 判断模组是否能立即升级并重启
 * 返回值是MODULE_CAN_REBOOT时，表示模组可以立即升级并重启，HiLink SDK将开始自动升级流程。
 * 返回值是MODULE_CANNOT_REBOOT时，表示模组不能立即升级并重启，HiLink SDK将不进行本次自动升级流程。
 * 注意：在用户同意设备可以自动升级的情况下，HiLink SDK调用此接口获取设备当前业务状态下，模组是否可以立即升级并重启的标志。
 * 只有当设备处于业务空闲状态时，接口才可以返回MODULE_CAN_REBOOT。
 * 当设备处于业务非空闲状态时，接口返回MODULE_CANNOT_REBOOT。
 */
int HILINK_GetRebootFlag(void);

/*
 * 触发设备的ota新版本检测
 * 检测到的版本，sdk将通过cb回调函数上报。如果不需要获取版本号，则可将cb置空。
 * 上报的version如果为空，则未成功获取到新版本。
 * 注意:1、建议调用频次最多一天一次
 * 2、首次通过手机配网成功后不能调用，建议连云成功后至少5s后调用
 */
void HILINK_TrigOtaVersionCheck(GetOtaVerCb cb);

/*
 * 触发设备的ota新版本升级
 * 检测到的版本，sdk将通过cb回调函数触发升级
 */
void HILINK_TrigOtaSelfUpdate(TrigSelfUpdateCb cb);

#ifdef __cplusplus
}
#endif

#endif