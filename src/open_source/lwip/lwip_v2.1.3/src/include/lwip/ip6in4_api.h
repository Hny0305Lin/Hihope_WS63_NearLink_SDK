/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: IP6IN4 api
 * Author: NA
 * Create: 2020
 */

/**
 * @defgroup Threadsafe_Ip6in4_Interfaces ip6in4
 * @ingroup wifimesh
 */

#ifndef LWIP_HDR_IP6IN4_API_H
#define LWIP_HDR_IP6IN4_API_H
#include "lwip/opt.h"

#if LWIP_IP6IN4

#include <stdint.h>
#define LWIP_IP6IN4_IP6_LEN 16
#define LWIP_IP6IN4_MAC_LEN 6
#define LWIP_IP6IN4_LIFETIME_INFINITE  0xFFFFFFFF
#define LWIP_IP6IN4_LIFETIME_DEFAULT  (30 * 60)

typedef struct {
  uint8_t ip6[LWIP_IP6IN4_IP6_LEN];
  uint32_t ip4;
  uint32_t lifetime;
} lwip_ip6in4_entry_t;

/*
 * Func Name: lwip_ip6in4_entry_add_update
 */
/**
 * @ingroup Threadsafe_Ip6in4_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to add or update entries of ip6in4 table.
 *
 * @param[in]    entry        Indicates the buffer of entries.
 * @param[in]    cnt          Indicates the number of the entries.
 *
 * @returns
 *  0 : On success \n
 *  Negative value : On failure \n
 *
 * @note
 *   - this api takes time to merge the entry.
 */
int lwip_ip6in4_entry_add_update(const lwip_ip6in4_entry_t *entry, uint8_t cnt);

/*
 * Func Name: lwip_ip6in4_entry_delete
 */
/**
 * @ingroup Threadsafe_Ip6in4_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to delete entries in the ip6in4 table.
 *
 * @param[in]    entry        Indicates the buffer of entries.
 * @param[in]    cnt          Indicates the number of the entries.
 *
 * @returns
 *  0 : On success \n
 *  Negative value : On failure \n
 *
 * @note
 *   - this api takes time to handle the entry, when the entry is too big.
 */
int lwip_ip6in4_entry_delete(const lwip_ip6in4_entry_t *entry, uint8_t cnt);

/*
 * Func Name: lwip_ip6in4_entry_sync
 */
/**
 * @ingroup Threadsafe_Ip6in4_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to sync entries of ip6in4 table.
 *
 * @param[in]    entry        Indicates the buffer of entries.
 * @param[in]    cnt          Indicates the number of the entries.
 *
 * @returns
 *  0 : On success \n
 *  Negative value : On failure \n
 *
 * @note
 *   - this api is to sync the entries with the leader.
 */
int lwip_ip6in4_entry_sync(const lwip_ip6in4_entry_t *entry, uint8_t cnt);

/*
 * Func Name: lwip_ip6in4_entry_all_get
 */
/**
 * @ingroup Threadsafe_Ip6in4_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to get all entries of ip6in4 table.
 *
 * @param[out]    entry        Indicates the buffer of entries.
 * @param[out]    cnt          Indicates the number of the entries.
 *
 * @returns
 *  >0 : On success, the real copied entry count. \n
 *  Negative value : On failure \n
 *
 * @note
 *   - this api is to get the whole entries, not only the entry that the node holds.
 */
int lwip_ip6in4_entry_all_get(lwip_ip6in4_entry_t *entry, uint8_t cnt);

/*
 * Func Name: lwip_ip6in4_entry_get
 */
/**
 * @ingroup Threadsafe_Ip6in4_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to get the entry the node holds.
 *
 * @param[out]    entry        Indicates the buffer of entry.
 *
 * @returns
 *  0 : On success \n
 *  Negative value : On failure \n
 *
 * @note
 *   - the node just holds one entry.
 */
int lwip_ip6in4_entry_get(lwip_ip6in4_entry_t *entry);

/*
 * Func Name: lwip_ip6in4_entry_get_by_mac
 */
/**
 * @ingroup Threadsafe_Ip6in4_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to get the entry that is referred to the mac.
 *
 * @param[in]     mac          Indicates the buffer of mac.
 * @param[in]     len          Indicates the length of mac buffer.
 * @param[out]    entry        Indicates the buffer of entry.
 *
 * @returns
 *  0 : On success \n
 *  Negative value : On failure \n
 *
 * @note
 *   - the in parameter mac is the mbr mac of wlan0.
 */
int lwip_ip6in4_entry_get_by_mac(const char *mac, uint8_t len, lwip_ip6in4_entry_t *entry);

/*
 * Func Name: lwip_ip6in4_init
 */
/**
 * @ingroup Threadsafe_Ip6in4_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to init the ip6in4.
 *
 * @param[in]    name        Indicates the netif name.
 * @param[in]    len         Indicates the len of netif name.
 * @param[in]    mbr_cnt     Indicates the number of the ip6in4 entry.
 *
 * @returns
 *  0 : On success \n
 *  Negative value : On failure \n
 *
 * @note
 *   - this api alloc the table buffer.
 */
int lwip_ip6in4_init(const char *name, uint8_t len, uint8_t mbr_cnt);

/*
 * Func Name: lwip_ip6in4_deinit
 */
/**
 * @ingroup Threadsafe_Ip6in4_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to deinit the ip6in4.
 *
 * @returns
 *  0 : On success \n
 *  Negative value : On failure \n
 *
 * @note
 *   - this api free the resource(like the table buffer).
 */
int lwip_ip6in4_deinit(void);

#endif /* LWIP_IP6IN4 */
#endif /* LWIP_HDR_IP6IN4_API_H */
