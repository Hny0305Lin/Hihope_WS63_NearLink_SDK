/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal source file.
 *
 * Create: 2023-05-10
*/

#include "dfx.h"
#include <string.h>
#include "common.h"
#include "mbedtls_platform_hardware_config.h"

#if defined(MBED_HARDEN_DEBUG)

typedef struct {
    mbedtls_ecp_group_id group_id;
    const char *name;
} mbedtls_ecp_group_id_name_mapping;

static const mbedtls_ecp_group_id_name_mapping group_id_name_table[] =
{
    { MBEDTLS_ECP_DP_NONE,       "MBEDTLS_ECP_DP_NONE"       },
    { MBEDTLS_ECP_DP_SECP192R1,  "MBEDTLS_ECP_DP_SECP192R1"  },
    { MBEDTLS_ECP_DP_SECP224R1,  "MBEDTLS_ECP_DP_SECP224R1"  },
    { MBEDTLS_ECP_DP_SECP256R1,  "MBEDTLS_ECP_DP_SECP256R1"  },
    { MBEDTLS_ECP_DP_SECP384R1,  "MBEDTLS_ECP_DP_SECP384R1"  },
    { MBEDTLS_ECP_DP_SECP521R1,  "MBEDTLS_ECP_DP_SECP521R1"  },
    { MBEDTLS_ECP_DP_BP256R1,    "MBEDTLS_ECP_DP_BP256R1"    },
    { MBEDTLS_ECP_DP_BP384R1,    "MBEDTLS_ECP_DP_BP384R1"    },
    { MBEDTLS_ECP_DP_BP512R1,    "MBEDTLS_ECP_DP_BP512R1"    },
    { MBEDTLS_ECP_DP_CURVE25519, "MBEDTLS_ECP_DP_CURVE25519" },
    { MBEDTLS_ECP_DP_SECP192K1,  "MBEDTLS_ECP_DP_SECP192K1"  },
    { MBEDTLS_ECP_DP_SECP224K1,  "MBEDTLS_ECP_DP_SECP224K1"  },
    { MBEDTLS_ECP_DP_SECP256K1,  "MBEDTLS_ECP_DP_SECP256K1"  },
    { MBEDTLS_ECP_DP_CURVE448,   "MBEDTLS_ECP_DP_CURVE448"   },
};

static const char* mbedtls_ecp_group_id_to_str(mbedtls_ecp_group_id group_id)
{
    for (int i = 0; group_id_name_table[i].name != NULL; i++) {
        if (group_id_name_table[i].group_id == group_id) {
            return group_id_name_table[i].name;
        }
    }
    return "Unknown";
}

void mbedtls_dump_grp_id(mbedtls_ecp_group_id group_id)
{
    const char* group_id_name = mbedtls_ecp_group_id_to_str(group_id);
    mbedtls_printf("group id = %s!\n", group_id_name);
}

void mbedtls_dump_hex(const char* name, const unsigned char* buf, unsigned int buf_len)
{
    unsigned int i;
    if (name == NULL) {
        mbedtls_printf("you should give a name!\n");
        return;
    }
    if (buf == NULL) {
        mbedtls_printf("%s == NULL!\n", name);
        return;
    }
    mbedtls_printf("\n---------------%s--------------\n", name);
    for (i = 0; i < buf_len; i++) {
        mbedtls_printf("%02x ", *(buf + i));
        if ((i + 1) % PRINT_LINE_LEN == 0) {
            mbedtls_printf("\n");
        }
    }
    mbedtls_printf("\n");
}

void mbedtls_dump_mpi(const char* name, const mbedtls_mpi *mpi)
{
    size_t buf_len;
    unsigned char* buf;
    if (name == NULL) {
        mbedtls_printf("you should give a name!\n");
        return;
    }
    if (mpi == NULL) {
        mbedtls_printf("%s == NULL!\n", name);
        return;
    }
    buf_len = mbedtls_mpi_size(mpi);
    buf = (unsigned char*)mbedtls_calloc(1, buf_len);
    if (buf == NULL) {
        mbedtls_printf("ERROR! mbedtls_calloc failed! LINE = %d\n", __LINE__);
    }
    if (mbedtls_mpi_write_binary(mpi, buf, buf_len) != 0) {
        mbedtls_printf("ERROR! mbedtls_mpi_write_binary failed!\n");
        mbedtls_free(buf);
        return;
    }
    mbedtls_dump_hex(name, buf, buf_len);
    mbedtls_free(buf);
}

void mbedtls_dump_point(const char* name, const mbedtls_ecp_point *point)
{
    size_t name_len;
    size_t xyz_name_len;
    char *xname;
    char *yname;
    char *zname;
    if (name == NULL) {
        mbedtls_printf("you should give a name!\n");
        return;
    }
    if (point == NULL) {
        mbedtls_printf("%s == NULL!\n", name);
    }
    name_len = strlen((char *)name);
    xyz_name_len = name_len + strlen(".x") + 1;
    xname = (char *)mbedtls_calloc(1, xyz_name_len);
    yname = (char *)mbedtls_calloc(1, xyz_name_len);
    zname = (char *)mbedtls_calloc(1, xyz_name_len);
    if (xname == NULL || yname == NULL || zname == NULL) {
        mbedtls_printf("ERROR! mbedtls_calloc failed! LINE = %d\n", __LINE__);
        goto cleanup;
        return;
    }
    (void)snprintf(xname, xyz_name_len, "%s%s", name, ".x");
    (void)snprintf(yname, xyz_name_len, "%s%s", name, ".y");
    (void)snprintf(zname, xyz_name_len, "%s%s", name, ".z");
    mbedtls_dump_mpi(xname, &point->MBEDTLS_PRIVATE(X));
    mbedtls_dump_mpi(yname, &point->MBEDTLS_PRIVATE(Y));
    mbedtls_dump_mpi(zname, &point->MBEDTLS_PRIVATE(Z));
cleanup:
    if (xname != NULL) {
        mbedtls_free(xname);
    }
    if (yname != NULL) {
        mbedtls_free(yname);
    }
    if (zname != NULL) {
        mbedtls_free(zname);
    }
}

#else

void mbedtls_dump_grp_id(mbedtls_ecp_group_id group_id){
    unused(group_id);
    return;
}

void mbedtls_dump_hex(const char* name, const unsigned char* buf, unsigned int buf_len)
{
    unused(name);
    unused(buf);
    unused(buf_len);
    return;
}

void mbedtls_dump_mpi(const char* name, const mbedtls_mpi *mpi)
{
    unused(name);
    unused(mpi);
    return;
}

void mbedtls_dump_point(const char* name, const mbedtls_ecp_point *point)
{
    unused(name);
    unused(point);
    return;
}

#endif