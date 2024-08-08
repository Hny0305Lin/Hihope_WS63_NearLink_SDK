/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mac_addr依赖efuse和ini文件系统(63未支持)，当前仅用全局变量暂存
 * 对应AT命令：cmd_set_macaddr，cmd_get_macaddr
 */
#include "securec.h"
#include "errcode.h"
#include "soc_osal.h"
#include "osal_timer.h"
#include "debug_print.h"
#include "trng.h"
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
#include "nv.h"
#endif
#if defined(CONFIG_DRIVER_SUPPORT_EFUSE)
#include "efuse.h"
#include "efuse_porting.h"
#endif
#include "mac_addr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SIZE_8_BITS       8
#define WLAN_MAC_ADDR_LEN 6
#define WLAN_MAC_MASK_LEN 2
#define BIT_8_MASK        0x100
#define LOW_8_BITS_MASK   0xFF
#define HIGH_MAC_MASK     0xFE

enum service_nl80211_iftype {
    SERVICE_NL80211_IFTYPE_UNSPECIFIED,
    SERVICE_NL80211_IFTYPE_ADHOC,
    SERVICE_NL80211_IFTYPE_STATION,
    SERVICE_NL80211_IFTYPE_AP,
    SERVICE_NL80211_IFTYPE_AP_VLAN,
    SERVICE_NL80211_IFTYPE_WDS,
    SERVICE_NL80211_IFTYPE_MONITOR,
    SERVICE_NL80211_IFTYPE_MESH_POINT,
    SERVICE_NL80211_IFTYPE_P2P_CLIENT,
    SERVICE_NL80211_IFTYPE_P2P_GO,
    SERVICE_NL80211_IFTYPE_P2P_DEVICE,
    /* keep last */
    SERVICE_NUM_NL80211_IFTYPES,
    SERVICE_NL80211_IFTYPE_MAX = SERVICE_NUM_NL80211_IFTYPES - 1
};

typedef enum {
    ADDR_IDX_STA = 0,
    ADDR_IDX_BLE = 1,
    ADDR_IDX_AP = 2,
    ADDR_IDX_P2P = 3,
    ADDR_IDX_GLE = 4,
    ADDR_IDX_BUTT
}addr_idx;

typedef struct {
    uint8_t   ac_addr[WLAN_MAC_ADDR_LEN];
    uint16_t  us_status;
}dev_addr_stru;

dev_addr_stru g_mac_addr = {0};
dev_addr_stru g_ap_dev_addr = {0};
dev_addr_stru g_p2p_dev_addr = {0};
mac_derivation_ptr g_mac_derivation_ptr = NULL;

void set_mac_derivation_ptr(mac_derivation_ptr ptr)
{
    g_mac_derivation_ptr = ptr;
}

static uint8_t mac_addr_is_zero(const uint8_t *mac_addr)
{
    uint8_t zero_mac_addr[WLAN_MAC_ADDR_LEN] = {0};
    return (memcmp(zero_mac_addr, mac_addr, WLAN_MAC_ADDR_LEN) == 0);
}

#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
static uint32_t mac_addr_nv_check(uint8_t *mac_addr)
{
    uint8_t zero_mac_addr[WLAN_MAC_ADDR_LEN] = {0};
    uint8_t one_mac_addr[WLAN_MAC_ADDR_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    if (memcmp(zero_mac_addr, mac_addr, WLAN_MAC_ADDR_LEN) == 0) {
        return ERRCODE_FAIL;
    }
    if (memcmp(one_mac_addr, mac_addr, WLAN_MAC_ADDR_LEN) == 0) {
        if (memcpy_s(mac_addr, WLAN_MAC_ADDR_LEN, zero_mac_addr, WLAN_MAC_ADDR_LEN) != EOK) {
            PRINT("{mac_addr_nv_check failed !}\n");
            return ERRCODE_MEMCPY;
        }
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
#endif

#if defined(CONFIG_DRIVER_SUPPORT_EFUSE)
static uint32_t get_mac_from_efuse(uint8_t mac[], uint8_t mac_len)
{
    uint8_t index;
    uint16_t efuse_index;

    for (index = 0; index < EFUSE_MAC_NUM; index++) {
        efuse_index = EFUSE_MAC_ADDR_LAST_ID - index;
        if (efuse_read_item(efuse_index, mac, mac_len) == ERRCODE_SUCC) {
            if (mac_addr_is_zero(mac) == ERRCODE_SUCC) {
                return ERRCODE_SUCC;
            }
        }
    }
    return ERRCODE_FAIL;
}
#endif

static void random_ether_addr(uint8_t *addr, uint32_t addr_length)
{
    uapi_drv_cipher_trng_get_random_bytes(addr, addr_length);
}

static addr_idx get_dev_addr_idx(uint8_t type)
{
    addr_idx addr_idx = ADDR_IDX_BUTT;

    switch (type) {
        case SERVICE_NL80211_IFTYPE_STATION:
            addr_idx = ADDR_IDX_STA;
            break;
        case SERVICE_NL80211_IFTYPE_AP:
            addr_idx = ADDR_IDX_AP;
            break;
        case SERVICE_NL80211_IFTYPE_P2P_CLIENT:
        case SERVICE_NL80211_IFTYPE_P2P_GO:
        case SERVICE_NL80211_IFTYPE_P2P_DEVICE:
        case SERVICE_NL80211_IFTYPE_MESH_POINT:
            addr_idx = ADDR_IDX_P2P;
            break;
        case IFTYPE_BLE:
            addr_idx = ADDR_IDX_BLE;
            break;
        default:
            PRINT("get_dev_addr_idx iftype error!!!\n");
            break;
    }

    return addr_idx;
}

static void mac_carry(uint16_t *mac_low, uint16_t *mac_high)
{
    (*mac_high) += (((*mac_low) & BIT_8_MASK)  >> SIZE_8_BITS);
    (*mac_low) = (*mac_low) & LOW_8_BITS_MASK;
}

#define WIFI_MAC_ADDR_DERIVE_BIT 4
#define BLE_MAC_ADDR_DERIVE_BIT 5
static uint32_t get_derived_mac(uint8_t mac[], uint8_t mac_len, addr_idx idx)
{
    uint16_t us_addr[WLAN_MAC_ADDR_LEN];
    uint32_t tmp;
    uint8_t derive_bit = ((idx == ADDR_IDX_BLE) ? BLE_MAC_ADDR_DERIVE_BIT : WIFI_MAC_ADDR_DERIVE_BIT);

    if (mac_len != WLAN_MAC_ADDR_LEN) {
        PRINT("get_dev_addr::get_derived_mac, error mac_len != 6.\n");
        return ERRCODE_FAIL;
    }

    for (tmp = 0; tmp < WLAN_MAC_ADDR_LEN; tmp++) {
        us_addr[tmp] = (uint16_t)g_mac_addr.ac_addr[tmp];
    }
    us_addr[derive_bit] += (uint16_t)idx;  // 4 字节增加idx
    for (tmp = derive_bit; tmp > 0; tmp--) {
        mac_carry(&(us_addr[tmp]), &(us_addr[tmp - 1]));
    }
    if (us_addr[0] > LOW_8_BITS_MASK) {
        us_addr[0] = 0;
    }
    if (idx != ADDR_IDX_BLE) {
        us_addr[0] &= HIGH_MAC_MASK;
    }
    for (tmp = 0; tmp < WLAN_MAC_ADDR_LEN; tmp++) {
        mac[tmp] = (uint8_t)us_addr[tmp];
    }
    return ERRCODE_SUCC;
}

uint32_t get_dev_addr(uint8_t *pc_addr, uint8_t addr_len, uint8_t type)
{
    uint32_t tmp;
    uint8_t  zero_mac[WLAN_MAC_ADDR_LEN] = {0};
    addr_idx addr_idx;
    if (pc_addr == NULL) {
        PRINT("pc_addr == NULL)!!!\n");
        return ERRCODE_FAIL;
    }

    if ((type == SERVICE_NL80211_IFTYPE_AP) && (memcmp(g_ap_dev_addr.ac_addr, zero_mac, WLAN_MAC_ADDR_LEN) != 0)) {
        for (tmp = 0; tmp < WLAN_MAC_ADDR_LEN; tmp++) {
            pc_addr[tmp] = g_ap_dev_addr.ac_addr[tmp];
        }
        return ERRCODE_SUCC;
    }

    if (((type == SERVICE_NL80211_IFTYPE_MESH_POINT) || (type == SERVICE_NL80211_IFTYPE_P2P_CLIENT) ||
        (type == SERVICE_NL80211_IFTYPE_P2P_GO) || (type == SERVICE_NL80211_IFTYPE_P2P_DEVICE)) &&
        (memcmp(g_p2p_dev_addr.ac_addr, zero_mac, WLAN_MAC_ADDR_LEN) != 0)) {
        for (tmp = 0; tmp < WLAN_MAC_ADDR_LEN; tmp++) {
            pc_addr[tmp] = g_p2p_dev_addr.ac_addr[tmp];
        }
        return ERRCODE_SUCC;
    }

    addr_idx = get_dev_addr_idx(type);
    if (addr_idx >= ADDR_IDX_BUTT) {
        PRINT("type=%d,error!!!\n", type);
        return ERRCODE_FAIL;
    }
    if (g_mac_derivation_ptr != NULL) {
        return g_mac_derivation_ptr(g_mac_addr.ac_addr, WLAN_MAC_ADDR_LEN, type, pc_addr, WLAN_MAC_ADDR_LEN);
    } else {
        return get_derived_mac(pc_addr, addr_len, addr_idx);
    }
}

static uint32_t mac_addr_check(const uint8_t *pc_addr, uint8_t mac_len)
{
    uint8_t zero_mac[WLAN_MAC_ADDR_LEN] = {0};
    uint8_t broadcast_mac[WLAN_MAC_ADDR_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    if ((pc_addr[0] & 0x1) == 0x1) {
        PRINT("Multicast mac addr not support!!!\r\n");
        return ERRCODE_FAIL;
    }
    if (memcmp(pc_addr, broadcast_mac, mac_len) == ERRCODE_SUCC) {
        PRINT("Broadcast mac addr not support!!!\r\n");
        return ERRCODE_FAIL;
    }
    if (memcmp(pc_addr, zero_mac, mac_len) == ERRCODE_SUCC) {
        PRINT("Zero mac addr not support!!!\r\n");
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}


uint32_t set_dev_addr(const uint8_t *pc_addr, uint8_t mac_len, uint8_t type)
{
    if (mac_addr_check(pc_addr, mac_len) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    if (type == SERVICE_NL80211_IFTYPE_AP) {
        if (memcpy_s(g_ap_dev_addr.ac_addr, WLAN_MAC_ADDR_LEN, pc_addr, mac_len) != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        } else {
            return ERRCODE_SUCC;
        }
    } else if ((type == SERVICE_NL80211_IFTYPE_MESH_POINT) || (type == SERVICE_NL80211_IFTYPE_P2P_CLIENT) ||
        (type == SERVICE_NL80211_IFTYPE_P2P_GO) || (type == SERVICE_NL80211_IFTYPE_P2P_DEVICE)) {
        if (memcpy_s(g_p2p_dev_addr.ac_addr, WLAN_MAC_ADDR_LEN, pc_addr, mac_len) != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        } else {
            return ERRCODE_SUCC;
        }
    } else {
        if (memcpy_s(g_mac_addr.ac_addr, WLAN_MAC_ADDR_LEN, pc_addr, mac_len) != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        } else {
            return ERRCODE_SUCC;
        }
    }
}

/*****************************************************************************
 功能描述  : 随机化初始mac地址 让单板启动时携带默认mac
 获取优先级：配置文件-->读efuse-->随机生成
*****************************************************************************/
#define RANDOM_DEFOURT_MAC1 0x00
#define RANDOM_DEFOURT_MAC2 0x73
void init_dev_addr(void)
{
    uint8_t index;
    errcode_t get_mac_res = ERRCODE_FAIL;

#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
    uint16_t nv_mac_length;
    if (mac_addr_is_zero(g_mac_addr.ac_addr) != 0) {
        uapi_nv_read(NV_ID_SYSTEM_FACTORY_MAC, WLAN_MAC_ADDR_LEN, &nv_mac_length, &(g_mac_addr.ac_addr[0]));
        get_mac_res = mac_addr_nv_check(&(g_mac_addr.ac_addr[0]));
    }
#endif

#if defined(CONFIG_DRIVER_SUPPORT_EFUSE)
    if (get_mac_res != ERRCODE_SUCC) {
        get_mac_res = get_mac_from_efuse(&(g_mac_addr.ac_addr[0]), WLAN_MAC_ADDR_LEN);
    }
#endif

    if (get_mac_res != ERRCODE_SUCC) {
        random_ether_addr(g_mac_addr.ac_addr, WLAN_MAC_ADDR_LEN);
        g_mac_addr.ac_addr[1] = RANDOM_DEFOURT_MAC1; /* 1 地址第2位 0x00 */
        g_mac_addr.ac_addr[2] = RANDOM_DEFOURT_MAC2; /* 2 地址第3位 0x73 */
        g_mac_addr.us_status = 0;
        get_mac_res = ERRCODE_SUCC;
    }

    PRINT("init_dev_addr, mac_addr:");
    for (index = 0; index < WLAN_MAC_ADDR_LEN - WLAN_MAC_MASK_LEN; index++) {
        print_str("0x%2x,", g_mac_addr.ac_addr[index]);
    }
    for (index = WLAN_MAC_ADDR_LEN - WLAN_MAC_MASK_LEN; index < WLAN_MAC_ADDR_LEN; index++) {
        print_str("0x**,", g_mac_addr.ac_addr[index]);
    }
    print_str("\r\n");
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif