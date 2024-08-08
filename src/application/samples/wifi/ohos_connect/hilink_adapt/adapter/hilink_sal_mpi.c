/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 多精度整数（Multi-precision integer）mbedTLS实现（此文件为DEMO，需集成方适配修改）
 */
#include "hilink_sal_mpi.h"

#include <stddef.h>
#include "mbedtls/bignum.h"
#include "hilink_mem_adapter.h"
#include "hilink_sal_defines.h"

HiLinkMpi HILINK_SAL_MpiInit(void)
{
    mbedtls_mpi *mpi = (mbedtls_mpi *)HILINK_Malloc(sizeof(mbedtls_mpi));
    if (mpi == NULL) {
        HILINK_SAL_WARN("malloc error\r\n");
        return NULL;
    }
    mbedtls_mpi_init(mpi);
    return mpi;
}

mbedtls_mpi *GetMbedtlsMpi(HiLinkMpi mpi)
{
    return mpi;
}

void HILINK_SAL_MpiFree(HiLinkMpi mpi)
{
    if (mpi == NULL) {
        return;
    }
    mbedtls_mpi_free(mpi);
    HILINK_Free(mpi);
}

int HILINK_SAL_MpiExpMod(HiLinkMpi x, HiLinkMpi a, HiLinkMpi e, HiLinkMpi n)
{
    if ((x == NULL) || (a == NULL) || (e == NULL) || (n == NULL)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    int ret = mbedtls_mpi_exp_mod(x, a, e, n, NULL);
    if (ret != 0) {
        HILINK_SAL_WARN("exp mod error %d\r\n", ret);
    }
    return ret;
}

int HILINK_SAL_MpiCmpInt(HiLinkMpi x, int64_t z)
{
    if (x == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    return mbedtls_mpi_cmp_int(x, z);
}

int HILINK_SAL_MpiSubInt(HiLinkMpi x, HiLinkMpi a, int64_t b)
{
    if ((x == NULL) || (a == NULL)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    int ret = mbedtls_mpi_sub_int(x, a, b);
    if (ret != 0) {
        HILINK_SAL_WARN("sub int error %d\r\n", ret);
    }
    return ret;
}

int HILINK_SAL_MpiCmpMpi(HiLinkMpi x, HiLinkMpi y)
{
    if ((x == NULL) || (y == NULL)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    return mbedtls_mpi_cmp_mpi(x, y);
}

int HILINK_SAL_MpiReadString(HiLinkMpi mpi, unsigned char radix, const char *s)
{
    if ((mpi == NULL) || (s == NULL) || (radix < 2) || (radix > 16)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    int ret = mbedtls_mpi_read_string(mpi, radix, s);
    if (ret != 0) {
        HILINK_SAL_WARN("read string error %d\r\n", ret);
    }
    return ret;
}

int HILINK_SAL_MpiWriteString(HiLinkMpi mpi, unsigned int radix, char *buf, unsigned int *bufLen)
{
    if ((mpi == NULL) || (buf == NULL) || (radix < 2) || (radix > 16) || (bufLen == NULL) ||
        (*bufLen == 0)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    size_t oLen = 0;
    int ret = mbedtls_mpi_write_string(mpi, radix, buf, *bufLen, &oLen);
    if (ret != 0) {
        HILINK_SAL_WARN("write string error %d\r\n", ret);
    } else {
        *bufLen = oLen;
    }
    return ret;
}

int HILINK_SAL_MpiReadBinary(HiLinkMpi mpi, const unsigned char *buf, unsigned int bufLen)
{
    if ((mpi == NULL) || (buf == NULL) || (bufLen == 0)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    int ret = mbedtls_mpi_read_binary(mpi, buf, bufLen);
    if (ret != 0) {
        HILINK_SAL_WARN("read binary error %d\r\n", ret);
    }
    return ret;
}

int HILINK_SAL_MpiWriteBinary(HiLinkMpi mpi, unsigned char *buf, unsigned int bufLen)
{
    if ((mpi == NULL) || (buf == NULL) || (bufLen == 0)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    int ret = mbedtls_mpi_write_binary(mpi, buf, bufLen);
    if (ret != 0) {
        HILINK_SAL_WARN("write binary error %d\r\n", ret);
    }
    return ret;
}