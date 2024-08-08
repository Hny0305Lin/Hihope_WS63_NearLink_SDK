/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides trng driver source \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */

#include "trng.h"
#include "kapi_trng.h"

#include "sl_common.h"

errcode_t uapi_drv_cipher_trng_get_random(uint32_t *randnum)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_cipher_trng_get_random(randnum);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_trng_get_random_bytes(uint8_t *randnum, uint32_t size)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_cipher_trng_get_multi_random(size, randnum);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}