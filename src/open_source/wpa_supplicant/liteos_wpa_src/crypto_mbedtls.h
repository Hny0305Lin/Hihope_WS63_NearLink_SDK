/*
 * Wrapper functions for crypto libraries
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 */

#ifndef CRYPTO_MBEDTLS_H
#define CRYPTO_MBEDTLS_H

#include "library/common.h"
#include "mbedtls/ecp.h"
#include "mbedtls/bignum.h"

struct crypto_ec {
	mbedtls_ecp_group ecp;
	mbedtls_mpi a;
};

#define crypto_bignum mbedtls_mpi
#define crypto_ec_point mbedtls_ecp_point
#define AES_128_ALT_BLOCK_SIZE 16
#define AES_256_ALT_BLOCK_SIZE 32
#define AES_128_CRYPTO_LEN 128
#define DHM_PARM_MEM 2048
#define DHM_PARM_G_LEN 2

#endif
