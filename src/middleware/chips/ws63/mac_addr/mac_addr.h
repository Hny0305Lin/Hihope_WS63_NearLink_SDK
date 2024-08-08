/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: wal common msg api.
 */
 
#ifndef __MAC_ADDR_H__
#define __MAC_ADDR_H__
 
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define IFTYPE_BLE 0xF1
#define IFTYPE_SLE 0xF2
typedef uint32_t(*mac_derivation_ptr)(uint8_t* origin_mac, uint8_t num, uint8_t type,
    uint8_t *output_mac, uint8_t out_put_num);
void set_mac_derivation_ptr(mac_derivation_ptr ptr);
void init_dev_addr(void);
uint32_t get_dev_addr(uint8_t *pc_addr, uint8_t addr_len, uint8_t type);
uint32_t set_dev_addr(const uint8_t *pc_addr, uint8_t mac_len, uint8_t type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
 
#endif
