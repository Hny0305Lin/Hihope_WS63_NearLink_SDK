/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: UPG verify header file.
 */

#ifndef UPG_VERIFY_H
#define UPG_VERIFY_H

#include "upg.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct upg_auth_data {
    uint32_t length;
    uint8_t *data;
} upg_auth_data_t;

errcode_t verify_hash_cmp(const uint8_t *hash, const uint8_t *hash_res, uint32_t hash_len);

errcode_t secure_authenticate(const uint8_t *key, const upg_auth_data_t *data, uint8_t *sign_buff);

errcode_t calc_hash(uint32_t src_addr, uint32_t src_len, uint8_t *data_sha, uint32_t data_sha_len);

errcode_t uapi_upg_check_head_integrity(const upg_package_header_t *pkg_header);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* UPG_VERIFY_H */
