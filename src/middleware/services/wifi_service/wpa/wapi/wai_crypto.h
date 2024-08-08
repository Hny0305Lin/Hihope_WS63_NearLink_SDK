/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: WAPI Implementation
 */

#ifndef WAI_CRYPTO_H
#define WAI_CRYPTO_H

int wai_hmac_sha256(const unsigned char *data, unsigned int datalen,
    const unsigned char *key, unsigned int keylen, unsigned char *output, unsigned int outlen);
int wai_kd_hmac_sha256(const unsigned char *data, unsigned int datalen,
    const unsigned char *key, unsigned int keylen, unsigned char *output, unsigned int outlen);
int wai_sm4_ofb_decrypt(const unsigned char *iv, unsigned int ivlen,
    const unsigned char *key, unsigned int keylen, const unsigned char *input, unsigned int inlen,
    unsigned char *output, unsigned int *outlen);

#endif /* end of WAI_CRYPTO_H */
