/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Description: NAT64 api
 * Author: NA
 * Create: 2019
 */

/**
 * @defgroup Threadsafe_NAT64_Interfaces nat64
 * @ingroup wifimesh
 */

#ifndef LWIP_HDR_NAT64_API_H
#define LWIP_HDR_NAT64_API_H

#include "lwip/err.h"
#include "lwip/ip_addr.h"
#define NAT64_ENTRY_ADD 103
#define NAT64_ENTRY_DEL 104
#define NAT64_ENTRY_UPDATE 105
#define NAT64_ENTRY_GET_ALL 106
#define NAT64_ENERY_GET_SYNC 107

#if LWIP_NAT64
typedef struct {
  uint8_t mac[NETIF_MAX_HWADDR_LEN];
  uint32_t ip;
  uint32_t lifetime; /* in seconds */
  uint32_t conn_time;
  uint8_t state;
  uint8_t dao_sn;
  uint8_t mnid;
  uint8_t orig_mnid;
  uint8_t nat64_sync : 1;
} lwip_nat64_entry_t;

typedef struct {
  lwip_nat64_entry_t *nate;
  uint16_t cnt;
  uint16_t index;
  uint8_t type;
  uint8_t is_all;
} lwip_nate_arg_t;

struct mesh_client_address {
  ip4_addr_t ip4addr;
  ip6_addr_t ip6addr;
  u8_t mac[NETIF_MAX_HWADDR_LEN]; /* the key to query the nat64 table. */
  u8_t mac_len;
  u8_t is_sync; /* indicate wether the node is belonged to other MBR or not */
};

struct mesh_client_mac_info {
  u8_t mac[NETIF_MAX_HWADDR_LEN];
  u8_t mac_len;
};

struct mesh_clients_mac_info {
  u16_t num; /* the mac info buffer number for input, and the actual info count for output. */
  struct mesh_client_mac_info clis[0];
};

/*
 * Func Name: lwip_nat64_init
 */
/**
 * @ingroup Threadsafe_NAT64_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to init the nat64 module.
 *
 * @param[in]    name         Indicates the network interface that work with the Router.
 * @param[in]    len          Indicates the network interface name len.
 *
 * @returns
 *  0 : On success \n
 *  Negative value : On failure \n
 *
 * @note
 *   - this api just start the stateful NAT64, and just only one NAT64 instance in the system.
 */
int lwip_nat64_init(const char *name, uint8_t len);

/*
 * Func Name: lwip_nat64_deinit
 */
/**
 * @ingroup Threadsafe_NAT64_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to deinit the NAT64 module, set the flags and free the memory.
 *
 * @returns
 *  0 : On success \n
 *  Negative value : On failure \n
 *
 * @note
 *   - this api can be called many times.
 */
int lwip_nat64_deinit(void);

/*
 * Func Name: lwip_nat64_entry_add
 */
/**
 * @ingroup Threadsafe_NAT64_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to add the entry from external MBR.
 *
 * @param[in]    nate         Indicates the NAT64 entry to be added.
 *
 * @returns
 *  0 : On success \n
 *  Negative value : On failure \n
 */
int lwip_nat64_entry_add(lwip_nat64_entry_t *nate);

/*
 * Func Name: lwip_nat64_entry_delete
 */
/**
 * @ingroup Threadsafe_NAT64_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to delete the NAT64 entry.
 *
 * @param[in]    nate         Indicates the NAT64 entry to be deleted.
 *
 * @returns
 *  0 : On success \n
 *  Negative value : On failure \n
 */
int lwip_nat64_entry_delete(lwip_nat64_entry_t *nate);

/*
 * Func Name: lwip_nat64_entry_update
 */
/**
 * @ingroup Threadsafe_NAT64_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to update the NAT64 entry.
 *
 * @param[in]    nate         Indicates the NAT64 entry to be updated.
 *
 * @returns
 *  0 : On success \n
 *  Negative value : On failure \n
 */
int lwip_nat64_entry_update(lwip_nat64_entry_t *nate);

/*
 * Func Name: lwip_nat64_entry_get
 */
/**
 * @ingroup Threadsafe_NAT64_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to copy the nat64 entry.
 *
 * @param[out]   nate         Indicates the nat entry buffer to be copied to.
 * @param[in]    cnt          Indicates the entry count of the nate.
 * @param[in]    is_all       Indicates the copy flag: local or all .
 *
 * @returns
 *  >=0 : the nat entry count \n
 *  Negative value : On failure \n
 */
int lwip_nat64_entry_get(lwip_nat64_entry_t *nate, uint16_t cnt, uint8_t is_all);

/*
 * Func Name: lwip_nat64_sync_entry_remove
 */
/**
 * @ingroup Threadsafe_NAT64_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to delete the nat64 entry according to the mnid value.
 *
 * @param[in]    mnid         Indicates the key to operator on.
 *
 * @returns
 *  0 : On success \n
 *  Negative value : On failure \n
 */
int lwip_nat64_sync_entry_remove(uint8_t mnid);

/*
 * Func Name: lwip_nat64_entry_cnt
 */
/**
 * @ingroup Threadsafe_NAT64_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to get the number of NAT64 entry.
 *
 * @param[in]    type         Indicates the type of the NAT64 entry.
 *
 * @returns
 *  >=0 : the NAT64 entry number \n
 *  Negative value : On failure \n
 */
uint16_t lwip_nat64_entry_cnt(uint8_t type);

/*
 * Func Name: lwip_nat64_clients_mac_get
 */
/**
 * @ingroup Threadsafe_NAT64_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to get all mac address in the nat64 table.
 *
 * @param[out]    clis_info          Indicates the buffer that store the mac infos.
 *
 * @returns
 *  0 : On success \n
 *  Negative value : On failure \n
 *
 * @note
 *   - the num in the clis_info has two functions:
 *       1.as the input parameter, indicated as the mac info entry count;
 *       2.as the output parameter, indicated as the actual info count that has been copied.
 */
err_t lwip_nat64_clients_mac_get(struct mesh_clients_mac_info *clis_info);

/*
 * Func Name: lwip_nat64_client_address_find
 */
/**
 * @ingroup Threadsafe_NAT64_Interfaces
 *
 * @brief
 *
 *  This is a thread safe API, used to get the ip address according the mac address.
 *
 * @param[out]    cli_address         Indicates the address struct.
 *
 * @returns
 *  0 : On success \n
 *  Negative value : On failure \n
 */
err_t lwip_nat64_client_address_find(struct mesh_client_address *cli_address);

#endif
#endif
