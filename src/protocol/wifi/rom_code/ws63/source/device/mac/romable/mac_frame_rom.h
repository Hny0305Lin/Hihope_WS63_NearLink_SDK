/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Source file defined by the corresponding frame structure and operation interface (cannot be called by
 * the HAL module).
 */

#ifndef MAC_FRAME_ROM_H
#define MAC_FRAME_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "wlan_types_rom.h"
#include "oam_ext_if.h"
#include "mac_frame_common_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* TCP协议类型，chartiot tcp连接端口号 */
#define MAC_TCP_PROTOCAL 6
#define MAC_UDP_PROTOCAL 17
#define MAC_ICMP_PROTOCAL 1

#define MAC_ARP_REQUEST 0x0001
#define MAC_ARP_RESPONSE 0x0002

#define MAC_ICMP_REQUEST 0x08
#define MAC_ICMP_RESPONSE 0x00
#define MAC_SUBMSDU_LENGTH_OFFSET 12 /* submsdu的长度字段的偏移值 */
#define MAC_SUBMSDU_DA_OFFSET 0      /* submsdu的目的地址的偏移值 */
#define MAC_SUBMSDU_SA_OFFSET 6      /* submsdu的源地址的偏移值 */
#define MAC_BYTE_ALIGN_VALUE                4           /* 4字节对齐 */

#define OAL_DECLARE_PACKED    __attribute__((__packed__))
/* 四地址帧结构体 */
struct dmac_ieee80211_frame_addr4 {
    mac_header_frame_control_stru frame_control;
    osal_u16                      duration_value      : 15,
                                  duration_flag       : 1;
    osal_u8                       address1[WLAN_MAC_ADDR_LEN];
    osal_u8                       address2[WLAN_MAC_ADDR_LEN];
    osal_u8                       address3[WLAN_MAC_ADDR_LEN];
    osal_u16                      frag_num            : 4,
                                  seq_num             : 12;
    osal_u8                       address4[WLAN_MAC_ADDR_LEN];
} OAL_DECLARE_PACKED;
typedef struct dmac_ieee80211_frame_addr4 dmac_ieee80211_frame_addr4_stru;
mac_data_type_enum_uint8 dmac_get_arp_type_by_arphdr(const oal_eth_arphdr_stru *rx_arp_hdr);
oal_bool_enum_uint8 dmac_frame_is_null_data(const oal_dmac_netbuf_stru *net_buf);
osal_u8 *dmac_find_ie_etc(osal_u8 eid, osal_u8 *ies, osal_s32 len);
osal_u8 dmac_get_data_type_etc(const oal_dmac_netbuf_stru *netbuff);
osal_bool dmac_is_eapol_key_ptk_etc(const mac_eapol_header_stru *eapol_header);
osal_bool dmac_is_eapol_key_ptk_4_4_etc(const oal_dmac_netbuf_stru *netbuff);
/*****************************************************************************
 函 数 名  : wlan_get_bssid
 功能描述  : 根据"from ds"bit,从帧中提取bssid(mac地址)

*****************************************************************************/
static INLINE__ osal_void dmac_get_bssid(osal_u8 *mac_hdr, osal_u8 *bssid, osal_u16 bssid_len)
{
    unref_param(bssid_len);
    if (mac_hdr_get_from_ds(mac_hdr) == 1) {
        (osal_void)memcpy_s(bssid, WLAN_MAC_ADDR_LEN, mac_hdr + 10, WLAN_MAC_ADDR_LEN); // from_ds的bssid在mac头偏移10比特
    } else if (mac_hdr_get_to_ds(mac_hdr) == 1) {
        (osal_void)memcpy_s(bssid, WLAN_MAC_ADDR_LEN, mac_hdr + 4, WLAN_MAC_ADDR_LEN); // to_ds的bssid在mac头偏移4比特
    } else {
        (osal_void)memcpy_s(bssid, WLAN_MAC_ADDR_LEN, mac_hdr + 16, WLAN_MAC_ADDR_LEN); // IBSS的bssid在mac头偏移16比特
    }
}

/*****************************************************************************
 函 数 名 : oal_dmac_host2net_short
 功能描述  : 16 bit本机字节序到网络字节序转换
*****************************************************************************/
static INLINE__ osal_u16 oal_dmac_host2net_short(osal_u16 val)
{
    return (((val & 0x00FF) << 8) + ((val & 0xFF00) >> 8)); // 移位8字节
}

/*****************************************************************************
 函 数 名 : oal_dmac_net2host_short
 功能描述  : 16 bit网络字节序到本机字节序转换
*****************************************************************************/
static INLINE__ osal_u16 oal_dmac_net2host_short(osal_u16 val)
{
    return (((val & 0x00FF) << 8) + ((val & 0xFF00) >> 8)); // 移位8字节
}

/*****************************************************************************
 函 数 名 : oal_dmac_host2net_long
 功能描述  : 32 bit本机字节序到网络字节序转换
*****************************************************************************/
static INLINE__ osal_u32 oal_dmac_host2net_long(osal_u32 val)
{
    return (((osal_u32)(val & 0x000000FF) << 24) + ((val & 0x0000FF00) << 8) + // 移位8、24字节
        ((val & 0x00FF0000) >> 8) + ((osal_u32)(val & 0xFF000000) >> 24));     // 移位8、24字节
}

/*****************************************************************************
 函 数 名  : mac_get_submsdu_len
 功能描述  : 获取netbuf中submsdu的长度
*****************************************************************************/
static INLINE__ osal_void dmac_get_submsdu_len(const osal_u8 *submsdu_hdr, osal_u16 *submsdu_len)
{
    *submsdu_len = *(submsdu_hdr + MAC_SUBMSDU_LENGTH_OFFSET);
    // 长度2字节，先取高8位，再取低8位
    *submsdu_len = (osal_u16)((*submsdu_len << 8) + *(submsdu_hdr + MAC_SUBMSDU_LENGTH_OFFSET + 1));
}

/*****************************************************************************
 函 数 名  : mac_get_submsdu_pad_len
 功能描述  : 获取submsdu需要填充的字节数
*****************************************************************************/
static INLINE__ osal_void dmac_get_submsdu_pad_len(osal_u16 msdu_len, osal_u8 *submsdu_pad_len)
{
    *submsdu_pad_len = msdu_len & 0x3;

    if ((*submsdu_pad_len) != 0) {
        *submsdu_pad_len = (MAC_BYTE_ALIGN_VALUE - *submsdu_pad_len);
    }
}

/*****************************************************************************
 函 数 名  : mac_rx_get_sa
 功能描述  : 获取收到的帧的源地址
*****************************************************************************/
static INLINE__ osal_void dmac_rx_get_sa(mac_ieee80211_frame_stru *mac_header, osal_u8 **sa)
{
    if (mac_header->frame_control.from_ds == 0) {
        /* IBSS、to AP */
        *sa = mac_header->address2;
    } else if ((mac_header->frame_control.from_ds == 1) && (mac_header->frame_control.to_ds == 0)) {
        /* from AP */
        *sa = mac_header->address3;
    } else {
        /* WDS */
        *sa = ((dmac_ieee80211_frame_addr4_stru *)mac_header)->address4;
    }
}

/*****************************************************************************
 函 数 名  : mac_get_transmitter_addr
 功能描述  : 获取收到的帧的目的地址
*****************************************************************************/
static INLINE__ osal_void dmac_rx_get_da(mac_ieee80211_frame_stru *mac_header, osal_u8 **da)
{
    if (mac_header->frame_control.to_ds == 0) {
        /* IBSS、from AP */
        *da = mac_header->address1;
    } else {
        /* WDS、to AP */
        *da = mac_header->address3;
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of mac_frame_rom.h */
