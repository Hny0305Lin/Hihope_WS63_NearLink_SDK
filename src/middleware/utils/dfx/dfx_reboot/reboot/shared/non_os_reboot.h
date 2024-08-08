/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  Reboot interface.
 * Author:
 * Create:  2018-10-15
 */

#ifndef NON_OS_REBOOT_H
#define NON_OS_REBOOT_H

#include "core.h"
#include "errcode.h"
#include "common_def.h"
#if EXCEPTION_TEST_ENABLE == YES && defined(SUPPORT_IPC)
#include "ipc_actions.h"
#endif

/**
 * @defgroup connectivity_drivers_non_os_reboot REBOOT
 * @ingroup  connectivity_drivers_non_os
 * @{
 */
#define REBOOT_POWER_ON_TRIGGER_STATUS           0xdead0001UL
#define REBOOT_SOFT_RESET_TRIGER_STATUS          0xdead0002UL
#define REBOOT_NMI_WDGTIMEOUT_TRIGGER_STATUS     0xdead0003UL
#define REBOOT_NMI_XIP_CTRL_TRIGER_STATUS        0xdead0004UL
#define REBOOT_NMI_XIP_CACHE_TRIGGER_STATUS      0xdead0005UL
#define REBOOT_HARD_FAULT_TRIGER_STATUS          0xdead0006UL
#define REBOOT_SOFT_PANIC_TRIGGER_STATUS         0xdead0007UL
#define REBOOT_OTHER_CORE_ABNORMAL_TRIGER_STATUS 0xdead0008UL
#define REBOOT_HARD_WDGTIMEOUT_TRIGER_STATUS     0xdead0009UL
#define REBOOT_BT_WDGTIMEOUT_TRIGER_STATUS       0xdead000aUL
#define REBOOT_ULP_WDGTIMEOUT_TRIGER_STATUS      0xdead000bUL
#define REBOOT_UPG_COMPLETION_TRIGER_STATUS      0xdead000cUL
#define REBOOT_ULP_PIN_RST_TRIGER_STATUS         0xdead000dUL
#define REBOOT_UNDEFINED_TRIGER_STATUS           0xdeadaaaaUL
#define STANDARD_REBOOT_MAGIC                    0xdeaddeadUL
#define ABNORMAL_REBOOT_MAGIC                    0xdeadbeafUL

/**
 * @brief  This is a special enum, it is split into three sections
 * Only 16 bits are used.
 * Bits 13 to 15 are flags setting which core caused the reboot
 * Bit 8 sets if the reset reason is custom or fixed
 * Bits 0 to 7 contain the reason
 */
typedef enum {
    REBOOT_CAUSE_UNKNOWN = 0x0000,                 // !< Used as a default for the reboot_reset_reason variable
    REBOOT_CAUSE_BT_RESET_UNKNOWN = 0x8000,  // !< REBOOT_CAUSE_BT_RESET_UNKNOWN
    REBOOT_CAUSE_BT_SYSRESETREQ = 0x8001,
    REBOOT_CAUSE_BT_WATCHDOG = 0x8002,
    REBOOT_CAUSE_BT_STANDARD_CHIP_WATCHDOG = 0x8004,
    REBOOT_CAUSE_BT_UPDATER_CHIP_WATCHDOG = 0x8005,
    REBOOT_CAUSE_BT_RPC_TIMEOUT = 0x8006,
    REBOOT_CAUSE_BT_HARDFAULT = 0x8007,
    REBOOT_CAUSE_BT_NNMIFAULT = 0x8008,
    REBOOT_CAUSE_BT_UNKNOWN_ISR = 0x8009,
    REBOOT_CAUSE_BT_PANIC = 0x800a,
    REBOOT_CAUSE_BT_STD_WDT_FRST = 0x800b,         /* force reboot */
    REBOOT_CAUSE_BT_STD_CHIP_WDT_FRST = 0x800c,    /* force reboot */
    REBOOT_CAUSE_BT_XIP_HARDFAULT = 0x800d,
    REBOOT_CAUSE_BT_SSB_EXCEPTION_REBOOT = 0x800e,
    REBOOT_CAUSE_BT_BUCK1_PROTECT_FRST = 0x8010,   /* buck1 protect */
    REBOOT_CAUSE_BT_BUCK2_PROTECT_FRST = 0x8020,   /* buck2 protect */

    REBOOT_CAUSE_BT_MONITOR_REBOOT_REQ = 0x8100,   // !< Start of custom reset reasons
    REBOOT_CAUSE_BT_REGIONS_UPDATED = 0x8101,
    REBOOT_CAUSE_BT_FOTA_UPGRADE = 0x8102,
    REBOOT_CAUSE_BT_END = REBOOT_CAUSE_BT_FOTA_UPGRADE + 1,

    REBOOT_CAUSE_GNSS_GLOBAL = 0x7000,
    REBOOT_CAUSE_GNSS_NMI = 0x7001,        // !< watchdog is nmi interrupt
    REBOOT_CAUSE_GNSS_HARDFAULT = 0x7002,
    REBOOT_CAUSE_GNSS_PANIC = 0x7003,
    REBOOT_CAUSE_GNSS_END = REBOOT_CAUSE_GNSS_PANIC + 1,

    REBOOT_CAUSE_PROTOCOL_GLOBAL = 0x4000,
    REBOOT_CAUSE_PROTOCOL_SYSRESETREQ = 0x4001,
    REBOOT_CAUSE_PROTOCOL_WATCHDOG = 0x4002,
    REBOOT_CAUSE_PROTOCOL_AXI_LOCK = 0x4003,
    REBOOT_CAUSE_PROTOCOL_AXI_LOCK_TO_BT = 0x4004,
    REBOOT_CAUSE_PROTOCOL_WDT_FRST = 0x4005,             /* force reboot */
    REBOOT_CAUSE_PROTOCOL1_WATCHDOG = 0x4006,
    REBOOT_CAUSE_PROTOCOL_ADDR_MON = 0x4007,
    REBOOT_CAUSE_PROTOCOL1_ADDR_MON = 0x4008,
    REBOOT_CAUSE_PROTOCOL_RPC_TIMEOUT = 0x4100,          // !< Start of custom reset reasons
    REBOOT_CAUSE_PROTOCOL_END = REBOOT_CAUSE_PROTOCOL_RPC_TIMEOUT + 1,

    REBOOT_CAUSE_APPLICATION_GLOBAL = 0x2000,
    REBOOT_CAUSE_APPLICATION_SYSRESETREQ = 0x2001,
    REBOOT_CAUSE_APPLICATION_WATCHDOG = 0x2002,
    REBOOT_CAUSE_APPLICATION_PANIC = 0x2003,
    REBOOT_CAUSE_APPLICATION_HARDFAULT = 0x2004,
    REBOOT_CAUSE_APPLICATION_NNMIFAULT = 0x2005,
    REBOOT_CAUSE_APPLICATION_UNKNOWN_ISR = 0x2006,
    REBOOT_CAUSE_APPLICATION_WDT_FRST = 0x2007,          /* force reboot */
    REBOOT_CAUSE_APPLICATION_CHIP_WDT = 0x2008,
    REBOOT_CAUSE_APPLICATION_XIP_CTRL = 0x2009,
    REBOOT_CAUSE_APPLICATION_XIP_CACHE = 0x200a,
    REBOOT_CAUSE_APPLICATION_MDMA = 0x200b,
    REBOOT_CAUSE_APPLICATION_SMDMA = 0x200c,
    REBOOT_CAUSE_APPLICATION_STD_CHIP_WDT_FRST = 0x200d,
    REBOOT_CAUSE_APPLICATION_STD_BT_WDT_FRST = 0x200e,
    REBOOT_CAUSE_APPLICATION_STD_ULP_WDT_FRST = 0x200f,
    REBOOT_CAUSE_APPLICATION_STD_ULP_PIN_FRST = 0x2010,

    REBOOT_CAUSE_UPG_COMPLETION = 0x2040, // !< Start of custom reset reasons
    REBOOT_CAUSE_MON_MEM_ALMOST_EMPTY = 0x2041,
    REBOOT_CAUSE_DFU_UPG_FAIL = 0x2042,

    REBOOT_CAUSE_APPLICATION_SLDM = 0x2080,

    REBOOT_CAUSE_APPLICATION_RPC_TIMEOUT = 0x2100,       // !< Start of custom reset reasons

    REBOOT_CAUSE_POWER_ON = 0xF0F0,
} reboot_cause_t;

typedef enum {
    VETO_REBOOT_REASON_RESERVE,
    VETO_REBOOT_REASON_WR_NV,     /* 写NV项过程中，在超时内，不强制重启 */
    VETO_REBOOT_REASON_WR_MR_DOC, /* 写表档案过程中，在超时内，不强制重启 */
    VETO_REBOOT_REASON_USR_RESERVED_2,
    VETO_REBOOT_REASON_USR_RESERVED_3,
    VETO_REBOOT_REASON_USR_RESERVED_4,
    VETO_REBOOT_REASON_USR_RESERVED_5,
    VETO_REBOOT_REASON_USR_RESERVED_6,
    VETO_REBOOT_REASON_USR_RESERVED_7,

    /* STA在抄控器对其做整表测试时，不允许重启 */
    VETO_REBOOT_REASON_NDM_METER_CHECK,
    /* 台区识别时不允许重启 */
    VETO_REBOOT_REASON_TF_IDENTIFY,
    /* STA上电11小时内，锁定网络状态下，不允许重启 */
    VETO_REBOOT_REASON_LOCK_NETWORK,
    /* 进入测试模式后 24 小时内不允许重启 */
    VETO_REBOOT_REASON_SYS_RST_DELAY_ENTER_PHY_TEST_MODE,
    VETO_REBOOT_REASON_UPG, /* 升级过程中, 不强制重启。 */
    VETO_REBOOT_REASON_BOOT_UPG,
    VETO_REBOOT_REASON_NO_PB, /* 无法收到PB块，导致频偏设置长时间超时（1000分钟），不强制重启 */

    /* CCO在写组网系列号的时候，不允许重启； */
    VETO_REBOOT_REASON_SUIT_NET,
    /* CCO/STA在写SNID的时候，不允许重启； */
    VETO_REBOOT_REASON_WRT_SNID,
    /* STA在写关联随机数的时候，不允许重启 */
    VETO_REBOOT_REASON_RELATING_RANDOM_NUM,
    VETO_REBOOT_REASON_MAX
} veto_reboot_reason_t;

#ifdef IPC_NEW
typedef struct {
    uint16_t req_reboot_reason;
    uint16_t core;
} req_reboot_msg;
#endif

/**
 * @brief  Initialise the reboot subsystem
 * @return None
 */
void reboot_init(void);

/**
 * @brief  De-initialise the reboot subsystem
 * @return None
 */
void reboot_deinit(void);

#if CORE == MASTER_BY_ALL
/**
 * @brief  Perform a chip reboot.
 * @return None
 */
void reboot_chip(void);

/**
 * @brief  Gets the last reason that the security core rebooted
 * @return The reason, or REBOOT_CAUSE_UNKNOWN if there is more than 1 reason
 */
reboot_cause_t reboot_get_security_reboot_reason(void);
#endif

/**
 * @brief  Perform a software reboot of the entire system.
 * @param  cause The reason we are asking for a chip reset, bits 8 to 15 are ignored as this command
 *              can only set a custom reboot for this core
 * @return None
 */
void reboot_system(reboot_cause_t cause);

/**
 * @brief  Gets the reason of the last reset
 * @return The reason we last reset
 */
reboot_cause_t reboot_get_reset_reason(void);

#if CORE == APPS && EXCEPTION_TEST_ENABLE == YES && defined(SUPPORT_IPC)
/**
 * @brief  send a exceptin test command to bt core.
 * @param  exception_test_command The exception test comand
 * @return None
 */
void exception_test(ipc_exception_test_command_e exception_test_command);
#endif

/*
* @brief 延时重启,在一段时间内投票反对重启，仅影响uapi_reboot_system_check_veto接口。
* @param reason 延时原因
* @param sec 延时时间
* @retval 返回实际延时时间。如果系统未对最大延时时间进行配置返回传入时间。如果系统进行了限制则返回时间小于最大延时时间。
*/
uint32_t uapi_reboot_vote_against(veto_reboot_reason_t reason, uint32_t sec);

/*
* @brief 去延时重启
* @param reason 延时原因
* @retval 返回实际延时时间。如果系统未对最大延时时间进行配置返回传入时间。如果系统进行了限制则返回时间小于最大延时时间。
*/
void uapi_reboot_unvote_against(veto_reboot_reason_t reason);

/*
* @brief 重启。受延时重启影响
* @param reason 重启原因
*/
void uapi_reboot_system_check_veto(reboot_cause_t reason);

/**
 * @}
 */
#endif
