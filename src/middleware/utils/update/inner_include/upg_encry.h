/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: UPG encry Interface Header.
 */
#ifndef UPG_ENCRY_H
#define UPG_ENCRY_H

#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_COMPRESS_ENCRY
#include "td_type.h"
#include "upg.h"
#include "upg_lzmadec.h"
#include "upg_encry_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

uint32_t upg_encry_and_write_pkt(upg_lzma_decode2_data_t *data, const upg_image_header_t *image, td_bool *first_pkt);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* UPG_LZMADEC_H */
#endif
