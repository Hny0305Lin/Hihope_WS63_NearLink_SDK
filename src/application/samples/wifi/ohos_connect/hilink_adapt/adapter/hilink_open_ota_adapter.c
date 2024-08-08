/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: OTA适配实现 （此文件为DEMO，需集成方适配修改）
 */
#include "hilink_open_ota_adapter.h"
#include "securec.h"
#include "sfc.h"
#include "partition.h"
#include "upg_porting.h"
#include "upg_common_porting.h"
#include "partition_resource_id.h"
#include "hilink_sal_defines.h"
#include "watchdog.h"

typedef struct {
    /* 升级的偏移 */
    unsigned int write_offset;
    /* 可升级区域的总大小 */
    unsigned int max_size;
} upg_param;

/* 记录升级相关的信息，比如开始地址，擦写块信息，可升级的最大区域 */
static upg_param g_upgrade;

const sfc_flash_config_t sfc_cfg = {
    .read_type = FAST_READ_QUAD_OUTPUT,
    .write_type = PAGE_PROGRAM,
    .mapping_addr = 0x200000,
    .mapping_size = 0x800000,
};

static errcode_t upg_get_upgrade_part_start_addr(uint32_t *start_address)
{
    errcode_t ret_val;
    partition_information_t info;
    ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }
    *start_address = info.part_info.addr_info.addr;
    return ERRCODE_SUCC;
}

static errcode_t upg_get_upgrade_part_size(uint32_t *size)
{
    errcode_t ret_val;
    partition_information_t info;
    ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }
    *size = info.part_info.addr_info.size - UPG_UPGRADE_FLAG_LENGTH;
    return ERRCODE_SUCC;
}

/*
 * Flash初始化
 * 返回值是true时，表示初始化正常
 * 返回值是false时，表示初始化异常
 */
bool HILINK_OtaAdapterFlashInit(void)
{
    uint32_t ret = uapi_sfc_init((sfc_flash_config_t *)&sfc_cfg);
    if (ret != ERRCODE_SUCC) {
        return false;
    }
    if (memset_s(&g_upgrade, sizeof(upg_param), 0, sizeof(upg_param)) != EOK) {
        return false;
    }
    uint32_t size = 0;
    if (upg_get_upgrade_part_size(&size) != 0) {
        HILINK_SAL_WARN("get upgrade max file len fail.\r\n");
        return false;
    }
    g_upgrade.max_size = size;
    return true;
}

/*
 * 判断需要升级的分区
 * 返回值是UPGRADE_FW_BIN1时，表示升级固件到分区1
 * 返回值是UPGRADE_FW_BIN2时，表示升级固件到分区2
 */
unsigned int HILINK_OtaAdapterGetUpdateIndex(void)
{
    return UPGRADE_FW_BIN1;
}

/*
 * 擦除需要升级的分区
 * size表示需要擦除的分区大小
 * 返回值是RETURN_OK时，表示擦除成功
 * 返回值是RETURN_ERROR时，表示擦除失败
 */
int HILINK_OtaAdapterFlashErase(unsigned int size)
{
    errcode_t ret_val;
    if (size > g_upgrade.max_size) {
        return RETURN_ERROR;
    }
    upg_prepare_info_t prepare_info;

    prepare_info.package_len = size;
    ret_val = uapi_upg_prepare(&prepare_info);
    if (ret_val != ERRCODE_SUCC) {
        return RETURN_ERROR;
    }
    return RETURN_OK;
}

/*
 * 升级数据写入升级的分区
 * buf表示待写入数据
 * bufLen表示待写入数据的长度
 * 返回值是RETURN_OK时，表示写入成功
 * 返回值是RETURN_ERROR时，表示写入失败
 */

/* 需要记录写入后的offset */
int HILINK_OtaAdapterFlashWrite(const unsigned char *buf, unsigned int bufLen)
{
    (void)uapi_watchdog_kick();

    errcode_t ret_val;
    uint32_t start_addr;
    if ((buf ==  NULL) || (bufLen == 0) || ((g_upgrade.write_offset + bufLen) > g_upgrade.max_size)) {
        return RETURN_ERROR;
    }
    ret_val = upg_get_upgrade_part_start_addr(&start_addr);
    if (ret_val != ERRCODE_SUCC) {
        return RETURN_ERROR;
    }
    // 写入
    ret_val = upg_flash_write(start_addr + g_upgrade.write_offset, bufLen, buf, false);
    if (ret_val != ERRCODE_SUCC) {
        return RETURN_ERROR;
    }
    g_upgrade.write_offset += bufLen;
    return RETURN_OK;
}

/*
 * 读取升级分区数据
 * offset表示读写偏移
 * buf表示输出数据的内存地址
 * bufLen表示输出数据的内存长度
 * 返回值是RETURN_OK时，表示读取成功
 * 返回值是RETURN_ERROR时，表示读取失败
 */
int HILINK_OtaAdapterFlashRead(unsigned int offset, unsigned char *buf, unsigned int bufLen)
{
    errcode_t ret_val;
    uint32_t start_addr;
    if ((buf ==  NULL) || (bufLen == 0) || ((offset + bufLen) > g_upgrade.max_size)) {
        return RETURN_ERROR;
    }
    ret_val = upg_get_upgrade_part_start_addr(&start_addr);
    if (ret_val != ERRCODE_SUCC) {
        return RETURN_ERROR;
    }
    // 读取
    ret_val = upg_flash_read(start_addr + offset, bufLen, buf);
    if (ret_val != ERRCODE_SUCC) {
        return RETURN_ERROR;
    }
    return RETURN_OK;
}

/*
 * 分区升级结束
 * 返回值是true时，表示结束正常
 * 返回值是false时，表示结束异常
 */

 /* 下载镜像结束，需要调起本地升级的接口 */
bool HILINK_OtaAdapterFlashFinish(void)
{
    errcode_t ret;
    ret = uapi_upg_request_upgrade(false);
    if (ret != ERRCODE_SUCC) {
        return false;
    }

    return true;
}

/* 获取升级区间最大长度 */
unsigned int HILINK_OtaAdapterFlashMaxSize(void)
{
    errcode_t ret_val;
    uint32_t size;
    ret_val = upg_get_upgrade_part_size(&size);
    if (ret_val != ERRCODE_SUCC) {
        return 0;
    }
    return size;
}

/*
 * 根据标志重启模组
 * flag表示重启标志
 * 当flag是RESTART_FLAG_NOW时，表示只有MCU升级时立即重启
 * 当flag是RESTART_FLAG_LATER时，表示有模组时切换分区后再重启
 */
void HILINK_OtaAdapterRestart(int flag)
{
    upg_reboot();
    return;
}

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
int HILINK_OtaStartProcess(int type)
{
    return RETURN_OK;
}

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
int HILINK_OtaEndProcess(int status)
{
    return RETURN_OK;
}

/*
 * 判断模组是否能立即升级并重启
 * 返回值是MODULE_CAN_REBOOT时，表示模组可以立即升级并重启，HiLink SDK将开始自动升级流程。
 * 返回值是MODULE_CANNOT_REBOOT时，表示模组不能立即升级并重启，HiLink SDK将不进行本次自动升级流程。
 * 注意：在用户同意设备可以自动升级的情况下，HiLink SDK调用此接口获取设备当前业务状态下，模组是否可以立即升级并重启的标志。
 * 只有当设备处于业务空闲状态时，接口才可以返回MODULE_CAN_REBOOT。
 * 当设备处于业务非空闲状态时，接口返回MODULE_CANNOT_REBOOT。
 */
int HILINK_GetRebootFlag(void)
{
    return MODULE_CAN_REBOOT;
}
