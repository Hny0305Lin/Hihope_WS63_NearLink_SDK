/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: implementation for nat64 API
 * Author: NA
 * Create: 2020
 */

#include <stdint.h>
#include "lwip/opt.h"
#if LWIP_NAT64
#include "lwip/nat64_api.h"
#include "lwip/nat64.h"
#include "lwip/nat64_addr.h"
#include "lwip/netifapi.h"
#include "lwip/nat64_v4_dhcpc.h"
#include "rpl_common.h"

static err_t
lwip_nat64_translate(lwip_nat64_entry_t *nate, nat64_entry_t *dst)
{
  int ret;

  ret = memcpy_s(dst->mac.addr, NETIF_MAX_HWADDR_LEN, nate->mac, NETIF_MAX_HWADDR_LEN);
  if (ret != EOK) {
    return ERR_MEM;
  }
  dst->mac.addrlen = NETIF_MAX_HWADDR_LEN;

#if !LWIP_NAT64_MIN_SUBSTITUTE
  dst->ip.addr = nate->ip;
#endif

  dst->lifetime = nate->lifetime;
  dst->conn_time = nate->conn_time;
  dst->state = nate->state;
  dst->dao_sn = nate->dao_sn;
  dst->mnid = nate->mnid;
  dst->orig_mnid = nate->orig_mnid;
  return ERR_OK;
}

static err_t
lwip_nat64_copy(nat64_entry_t *nate, void *data)
{
  lwip_nate_arg_t *arg = (lwip_nate_arg_t *)data;
  lwip_nat64_entry_t *dst = NULL;
  int ret;

  if (arg->index >= arg->cnt) {
    return ERR_VAL;
  }
  if ((arg->is_all == lwIP_FALSE) && (nate->nat64_sync == lwIP_TRUE)) {
    return ERR_OK;
  }
  if (nate->state != NAT64_STATE_ESTABLISH) {
    return ERR_OK;
  }
  dst = &arg->nate[arg->index];

  ret = memcpy_s(dst->mac, NETIF_MAX_HWADDR_LEN, nate->mac.addr, nate->mac.addrlen);
  if (ret != EOK) {
    return ERR_MEM;
  }

  dst->ip = nate->ip.addr;
  dst->lifetime = nate->lifetime;
  dst->conn_time = nate->conn_time;
  dst->state = nate->state;
  dst->dao_sn = nate->dao_sn;
  dst->mnid = rpl_get_mbr_mnid();
  dst->orig_mnid = nate->orig_mnid;
  dst->nat64_sync = lwIP_TRUE;

  arg->index++;
  return ERR_OK;
}

static err_t
lwip_nat64_get(struct netif *nif, void *arg)
{
  lwip_nate_arg_t *nate_arg = (lwip_nate_arg_t *)arg;
  nate_arg->index = 0;
  (void)nat64_entry_traverse(lwip_nat64_copy, arg);

  LWIP_UNUSED_ARG(nif);
  return ERR_OK;
}

static err_t
lwip_nat64_get_cnt(struct netif *nif, void *arg)
{
  LWIP_UNUSED_ARG(nif);
  lwip_nate_arg_t *nate_arg = (lwip_nate_arg_t *)arg;
  nate_arg->index = 0;
  int state = 0;
  nat64_entry_t *nate = NULL;
  while ((nate = nat64_get_next_inuse_entry(&state)) != NULL) {
    if (nate_arg->type == NAT64_ENTRY_GET_ALL) {
      if (nate->state == NAT64_STATE_ESTABLISH) {
        nate_arg->index++;
      }
    } else if ((nate_arg->type == NAT64_ENERY_GET_SYNC) && (nate->nat64_sync != lwIP_TRUE)) {
      if (nate->state == NAT64_STATE_ESTABLISH) {
        nate_arg->index++;
      }
    }
  }

  return ERR_OK;
}

static void
lwip_nat64_handle_add(nat64_entry_t *dst_nate, nat64_entry_t *nate_exist, u8_t mbr_mnid)
{
  err_t ret;
  if (dst_nate->mnid == mbr_mnid) {
    return;
  }
  if ((nate_exist != NULL) && (nate_exist->mnid != dst_nate->mnid)) {
    if (nate_exist->nat64_sync == lwIP_FALSE) {
      if (nate_exist->conn_time < dst_nate->conn_time) {
        return;
      }
      ret = nat64_dhcp_stop(nat64_netif_get(), &(nate_exist->mac), lwIP_FALSE);
      if (ret != ERR_OK) {
        LWIP_DEBUGF(NAT64_DEBUG, ("stop nat64 dhcp proxy fail\n"));
      }
    }
    (void)nat64_entry_update(dst_nate, lwIP_TRUE);
    return;
  }
  (void)nat64_entry_add_new(dst_nate);
}

static err_t
lwip_nat64_handle(struct netif *nif, void *arg)
{
  nat64_entry_t dst_nate;
  u8_t mbr_mnid;
  (void)memset_s(&dst_nate, sizeof(nat64_entry_t), 0, sizeof(nat64_entry_t));

  mbr_mnid = rpl_get_mbr_mnid();

  lwip_nate_arg_t *lwip_nate = (lwip_nate_arg_t *)arg;

  (void)lwip_nat64_translate(lwip_nate->nate, &dst_nate);

  nat64_entry_t *nate_exist = NULL;
  nate_exist = nat64_entry_lookup_by_mac(&dst_nate.mac);

  switch (lwip_nate->type) {
    case NAT64_ENTRY_ADD:
      lwip_nat64_handle_add(&dst_nate, nate_exist, mbr_mnid);
      break;
    case NAT64_ENTRY_DEL:
      if (nate_exist != NULL) {
        if ((nate_exist->mnid == dst_nate.mnid) && (nate_exist->nat64_sync == lwIP_TRUE)) {
          (void)nat64_entry_remove(nate_exist, lwIP_FALSE);
        }
      }
      break;
    case NAT64_ENTRY_UPDATE:
      if (dst_nate.mnid == mbr_mnid) {
        break;
      }
      if ((nate_exist != NULL) && (nate_exist->mnid == dst_nate.mnid) &&
          (nate_exist->nat64_sync == lwIP_TRUE)) {
        (void)nat64_entry_update(&dst_nate, lwIP_TRUE);
      }
      break;
    default:
      break;
  }
  LWIP_UNUSED_ARG(nif);
  return ERR_OK;
}

int
lwip_nat64_entry_delete(lwip_nat64_entry_t *nate)
{
  lwip_nate_arg_t arg;
  err_t ret;
  if (nate == NULL) {
    return ERR_VAL;
  }

  arg.nate = nate;
  arg.type = NAT64_ENTRY_DEL;

  ret = netifapi_call_argcb(lwip_nat64_handle, (void *)&arg);
  if (ret != ERR_OK) {
    return ERR_VAL;
  }
  return ERR_OK;
}

int
lwip_nat64_entry_add(lwip_nat64_entry_t *nate)
{
  lwip_nate_arg_t arg;
  err_t ret;
  if (nate == NULL) {
    return ERR_VAL;
  }

  arg.nate = nate;
  arg.type = NAT64_ENTRY_ADD;

  ret = netifapi_call_argcb(lwip_nat64_handle, (void *)&arg);
  if (ret != ERR_OK) {
    return ERR_VAL;
  }
  return ERR_OK;
}

int
lwip_nat64_entry_update(lwip_nat64_entry_t *nate)
{
  lwip_nate_arg_t arg;
  err_t ret;
  if (nate == NULL) {
    return ERR_VAL;
  }

  arg.nate = nate;
  arg.type = NAT64_ENTRY_UPDATE;

  ret = netifapi_call_argcb(lwip_nat64_handle, (void *)&arg);
  if (ret != ERR_OK) {
    return ERR_VAL;
  }
  return ERR_OK;
}

int
lwip_nat64_entry_get(lwip_nat64_entry_t *nate, uint16_t cnt, uint8_t is_all)
{
  lwip_nate_arg_t arg;
  err_t ret;

  if ((nate == NULL) || (cnt == 0)) {
    return -1;
  }

  arg.nate = nate;
  arg.cnt = cnt;
  arg.is_all = is_all;

  ret = netifapi_call_argcb(lwip_nat64_get, &arg);
  if (ret != ERR_OK) {
    return -1;
  }

  return arg.index;
}

uint16_t
lwip_nat64_entry_cnt(uint8_t type)
{
  err_t ret;
  lwip_nate_arg_t arg;
  (void)memset_s(&arg, sizeof(lwip_nate_arg_t), 0, sizeof(lwip_nate_arg_t));
  arg.type = type;
  ret = netifapi_call_argcb(lwip_nat64_get_cnt, &arg);
  if (ret != ERR_OK) {
    return 0;
  }
  return arg.index;
}

int
lwip_nat64_sync_entry_remove(uint8_t mnid)
{
  uint8_t arg = mnid;
  err_t ret;
  ret = netifapi_call_argcb(nat64_entry_remove_by_mnid, &arg);
  if (ret != ERR_OK) {
    return -1;
  }

  return ERR_OK;
}

static err_t
nat64_clients_mac_get(struct netif *netif, void *arg)
{
  struct mesh_clients_mac_info *clis_info = (struct mesh_clients_mac_info *)arg;
  int state = 0;
  nat64_entry_t *nate = NULL;
  struct mesh_client_mac_info *cli_mac = NULL;
  u16_t cnt = 0;

  (void)netif;

  if ((clis_info == NULL) || (clis_info->num == 0)) {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64_clients_mac_get: invalid arg\n"));
    return ERR_ARG;
  }

  while (((nate = nat64_get_next_inuse_entry(&state)) != NULL) && (cnt < clis_info->num)) {
    cli_mac = &((clis_info->clis)[cnt]);
    (void)memcpy_s(cli_mac->mac, NETIF_MAX_HWADDR_LEN, nate->mac.addr, NETIF_MAX_HWADDR_LEN);
    cli_mac->mac_len = nate->mac.addrlen;
    cnt++;
  }
  clis_info->num = cnt;
  return ERR_OK;
}

static err_t
nat64_client_address_find(struct netif *netif, void *cli_address)
{
  struct mesh_client_address *cli_info = (struct mesh_client_address *)cli_address;
  nat64_entry_t *nate = NULL;
  linklayer_addr_t lladdr = {0};

  (void)netif;

  if (cli_info == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64_client_address_find: invalid arg\n"));
    return ERR_ARG;
  }

  (void)memcpy_s(lladdr.addr, NETIF_MAX_HWADDR_LEN, cli_info->mac, NETIF_MAX_HWADDR_LEN);
  lladdr.addrlen = cli_info->mac_len;
  nate = nat64_entry_lookup_by_mac((const linklayer_addr_t *)&lladdr);
  if (nate == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64_client_address_find: no entry\n"));
    return ERR_VAL;
  }
  cli_info->ip4addr.addr = 0;
  (void)nat64_entry_to4(nate, &(cli_info->ip4addr));
  (void)memset_s(&(cli_info->ip6addr), sizeof(ip6_addr_t), 0, sizeof(ip6_addr_t));
  (void)nat64_entry_to6(nate, &(cli_info->ip6addr));
  cli_info->is_sync = nate->nat64_sync;

  return ERR_OK;
}

err_t
lwip_nat64_clients_mac_get(struct mesh_clients_mac_info *clis_info)
{
  return netifapi_call_argcb(nat64_clients_mac_get, (void *)clis_info);
}

err_t
lwip_nat64_client_address_find(struct mesh_client_address *cli_address)
{
  return netifapi_call_argcb(nat64_client_address_find, (void *)cli_address);
}
#endif

