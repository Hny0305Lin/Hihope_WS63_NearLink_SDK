/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: hcc adapt SDIO completion.
 * Author:
 * Create: 2021-07-26
 */

#ifndef HCC_ADAPT_SDIO_HEADER
#define HCC_ADAPT_SDIO_HEADER

#ifdef CONFIG_HCC_SUPPORT_SDIO
#include "hcc_bus_types.h"
#include "hcc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hcc_bus *hcc_adapt_sdio_load(hcc_handler *hcc);
td_void hcc_adapt_sdio_unload(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* CONFIG_HCC_SUPPORT_SDIO */
#endif /* HCC_ADAPT_SDIO_HEADER */
