/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: UPG ab mode config file for product.
 */
#ifndef UPG_AB_H
#define UPG_AB_H

typedef enum  {
    UPG_REGION_A,
    UPG_REGION_B,
    UPG_REGION_COUNT
} upg_region_index;

upg_region_index upg_get_run_region(void);
upg_region_index upg_get_upg_region(void);
uint32_t upg_get_region_addr(upg_region_index upg_region);
uint32_t upg_get_region_size(upg_region_index upg_region);
uint32_t upg_ab_image_read(upg_region_index upg_region, uint32_t offset, uint8_t *buf, size_t len);
uint32_t upg_ab_image_write(upg_region_index upg_region, uint32_t offset, uint8_t *buf, size_t len);
errcode_t upg_set_run_region(upg_region_index upg_region);
errcode_t upg_region_erase(upg_region_index region);
errcode_t upg_ab_start(upg_region_index upg_region);

#endif /* UPG_AB_H */