/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: This file provides information for ethtool. reference to linux kernel :include/uapi/linux/ethtool.h
 * Author: none
 * Create: 2020
 */

#ifndef __ETHTOOL_H
#define __ETHTOOL_H


#include "lwip/opt.h"

/* don't build if not configured for use in lwipopts.h */
#if LWIP_NETIF_ETHTOOL && LWIP_SOCKET
#include "lwip/netif.h"
#include "lwip/sockets.h"
#include "lwip/arch.h"
#include "netif/ifaddrs.h"

#if LWIP_LITEOS_COMPAT && LWIP_LINUX_COMPAT
#include <linux/ethtool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !LWIP_LITEOS_COMPAT || !LWIP_LINUX_COMPAT
#define ETHTOOL_GSET    0x00000001
#define ETHTOOL_SSET    0x00000002
#define ETHTOOL_GLINK   0x0000000a

/** @brief This struct is to pass data for link control and status */
struct ethtool_cmd {
  /** < Command number = %ETHTOOL_GSET or %ETHTOOL_SSET */
  u32_t cmd;
  /** < Bitmask of %SUPPORTED_* flags for the link modes, physical connectors and other link features
   for which the interface supports autonegotiation or auto-detection. Read-only. */
  u32_t supported;
  /** < Bitmask of %ADVERTISED_* flags for the link modes, physical connectors and other link features
   that are advertised through autonegotiation or enabled for auto-detection. */
  u32_t advertising;
  /** < Low bits of the speed, 1Mb units, 0 to INT_MAX or SPEED_UNKNOWN */
  u16_t speed;
  /** < Duplex mode; one of %DUPLEX_* */
  u8_t duplex;
  /** < Physical connector type; one of %PORT_* */
  u8_t port;
  /** < MDIO address of PHY (transceiver); 0 or 255 if not applicable. */
  u8_t phy_address;
  /** < Historically used to distinguish different possible PHY types,
   but not in a consistent way.  Deprecated. */
  u8_t transceiver;
  /** < Enable/disable autonegotiation and auto-detection; either %AUTONEG_DISABLE or %AUTONEG_ENABLE */
  u8_t autoneg;
  /** < Bitmask of %ETH_MDIO_SUPPORTS_* flags for the MDIO protocols supported by the interface;
   0 if unknown. Read-only. */
  u8_t mdio_support;
  /** < Historically used to report TX IRQ coalescing; Read-only; deprecated. */
  u32_t maxtxpkt;
  /** < Historically used to report RX IRQ coalescing; Read-only; deprecated. */
  u32_t maxrxpkt;
  /** < High bits of the speed, 1Mb units, 0 to INT_MAX or SPEED_UNKNOWN */
  u16_t speed_hi;
  /** < Ethernet twisted-pair MDI(-X) status; one of %ETH_TP_MDI_*. If the status is unknown or not applicable,
   the value will be %ETH_TP_MDI_INVALID. Read-only. */
  u8_t eth_tp_mdix;
  /** < Ethernet twisted pair MDI(-X) control; one of %ETH_TP_MDI_*.
   If MDI(-X) control is not implemented, reads yield %ETH_TP_MDI_INVALID and writes may be ignored or rejected.
   When written successfully, the link should be renegotiated if necessary. */
  u8_t eth_tp_mdix_ctrl;
  /** < Bitmask of %ADVERTISED_* flags for the link modes and other link features
   that the link partner advertised through autonegotiation; 0 if unknown or not applicable. Read-only. */
  u32_t lp_advertising;
  /* reserved 8(sizoef(u32_t) * 2) bytes */
  u32_t reserved[2];
};

/** @brief This structure is for passing single values. */
struct ethtool_value {
  u32_t cmd; /** < Indicates command. */
  u32_t data; /** < Indicates data. */
};
#endif

struct ethtool_ops {
  /** < Reports whether physical link is up.
    Will only be called if the netdev is up.
    Should usually be set to ethtool_op_get_link(), which uses netif_carrier_ok(). */
  s32_t (*get_link)(struct netif *netif);
  s32_t (*get_settings)(struct netif *netif, struct ethtool_cmd *cmd); /** < Gets the current settings */
  s32_t (*set_settings)(struct netif *netif, struct ethtool_cmd *cmd); /** < Configures settings */
  /** < Function to be called before any other operation.  Returns a  negative error code or zero. */
  int (*begin)(struct netif *netif);
  /** < Function to be called after any other operation except @begin.
     Will be called even if the other operation failed. */
  void (*complete)(struct netif *netif);
};

s32_t dev_ethtool(struct netif *netif, struct ifreq *ifr);
#ifdef __cplusplus
}
#endif

#endif /* LWIP_NETIF_ETHTOOL && LWIP_SOCKET */

#endif /* __ETHTOOL_H */

