/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: The random number generation module can be used in the security field,
 *              and it is implemented by referring to the good practices that the company has reviewed.
 */

#include "sha256/sha256.h"
#include "sec_random.h"
#if !CHIP_WS63 && !CHIP_WS53
#include "systick.h"
#include "securec.h"
#if defined(BUILD_APPLICATION_STANDARD) && !defined(LITEOS_ONETRACK)
#ifdef __LITEOS__
#include "los_hwi.h"
#endif
#else
#include "rtc.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

// Define the rand entropy pool. Currently only one entropy pool is used.
static hrand_ctx g_ctx;

// Last random number
static uint32_t g_u_last_rand = 0;

/*****************************************************************************
   Function implementation
*****************************************************************************/
void get_source_bytes(uint8_t *data, uint32_t len);

// Get random source
void get_source_bytes(uint8_t *data, uint32_t len)
{
    uint32_t i;
    uint32_t ul_delta = 0;
    errno_t ret = EOK;
    /* The validity of the input parameters is guaranteed by the upper layer, and no judgment is made here. */
    uint32_t ul_count = (len + 3) / 4; // count = (len + 3) / 4 get a Integer

    for (i = 0; i < ul_count; i++) {
        ul_delta = (uint32_t)uapi_systick_get_us();
#if defined(BUILD_APPLICATION_STANDARD) && !defined(LITEOS_ONETRACK)
        ul_delta ^= osIntContGet();
#else
        ul_delta ^= uapi_rtc_int_cnt_record_get(RTC_0);
#endif
        if (i < ul_count - 1) {
            // Address offset per cycle (i * 4)
            ret = memcpy_s(data + i * 4, HASH_DIGEST_LENGTH - i * 4, (void *)&ul_delta, 4);
        } else {
            // Last operation length (len-i * 4)
            ret = memcpy_s(data + i * 4, HASH_DIGEST_LENGTH - i * 4, (void *)&ul_delta, len - i * 4);
        }
        if (ret != EOK) {
            continue;
        }
    }
}

// Obtain system description information to initialize the random number entropy pool.
static void get_constinfo(uint8_t *buf, const uint8_t max_buf_len)
{
    UNUSED(max_buf_len);
    uint8_t auc_out_buf[SHA256_HASH_SIZE] = { 0 };
    uint32_t u_last_rand = g_u_last_rand;
    errno_t ret = EOK;
    // Entry parameters are guaranteed to be legal by the caller(buf length>=DEFAULT_CONST_LEN)
    if (u_last_rand == 0) {
        ret = memcpy_s(buf, DEFAULT_CONST_LEN, (uint8_t *)DEFAULT_CONST, DEFAULT_CONST_LEN);
    } else {
        sha256_hash((uint8_t *)(uintptr_t)u_last_rand, sizeof(u_last_rand), auc_out_buf, SHA256_HASH_SIZE);
        ret = memcpy_s(buf, DEFAULT_CONST_LEN, auc_out_buf, DEFAULT_CONST_LEN);
    }
    if (ret != EOK) {
        UNUSED(ret);
    }
}

// Entropy pool output and update
static void hrand_bytes(hrand_ctx *ctx, uint8_t *output, uint32_t output_len)
{
    sha256_context_t hash;
    uint8_t one = 1;
    uint8_t two = 2;
    uint8_t temp[HASH_DIGEST_LENGTH] = { 0 };
    errno_t ret = EOK;

    // Enter new entropy and incoming length is 16
    get_source_bytes(temp, 16);

    // update v
    sha256_init(&hash);
    SHA256Update(&hash, &one, 1);
    SHA256Update(&hash, temp, 16);  // Incoming length is 16
    SHA256Update(&hash, ctx->v, POOL_LEN);
    SHA256Update(&hash, ctx->c, POOL_LEN);
    sha256_final(&hash, ctx->v, POOL_LEN);

    // Output entropy
    if (output != NULL && output_len > 0) {
        // output
        sha256_init(&hash);
        SHA256Update(&hash, &two, 1);
        SHA256Update(&hash, ctx->v, POOL_LEN);
        sha256_final(&hash, temp, HASH_DIGEST_LENGTH);

        output_len = (output_len > HASH_DIGEST_LENGTH) ? HASH_DIGEST_LENGTH : output_len;
        ret = memcpy_s(output, output_len, temp, output_len);
        if (ret != EOK) {
            return;
        }

        // Back up data as the source of initialization data after system startup.
        g_u_last_rand = (uintptr_t)ctx->v;
    }
}

// Entropy pool initialization
static void hrand_init(hrand_ctx *ctx)
{
    sha256_context_t hash;
    uint8_t zero = 0;
    uint8_t constbuf[DEFAULT_CONST_LEN] = { 0 };

    // Get initialization information
    get_constinfo(constbuf, sizeof(constbuf));

    // calculate v
    sha256_init(&hash);
    SHA256Update(&hash, "Init", 4);  // The length of the string is 4
    SHA256Update(&hash, constbuf, DEFAULT_CONST_LEN);
    sha256_final(&hash, ctx->v, POOL_LEN);

    // calculate c
    sha256_init(&hash);
    SHA256Update(&hash, &zero, 1);
    SHA256Update(&hash, ctx->v, POOL_LEN);
    sha256_final(&hash, ctx->c, POOL_LEN);

    // Accumulated entropy
    (void)hrand_bytes(ctx, 0, 0);
    (void)hrand_bytes(ctx, 0, 0);
    (void)hrand_bytes(ctx, 0, 0);
    (void)hrand_bytes(ctx, 0, 0);
}

// Get a 4-byte random number
static uint32_t hrand_uint(hrand_ctx *ctx)
{
    uint32_t ret = 0;

    hrand_bytes(ctx, (uint8_t *)&ret, sizeof(ret));
    return ret;
}

// Initialize the random number module
void init_rand(void)
{
    hrand_init(&g_ctx);
}

// Get a secure 32-bit random number for security scenarios such as key generation
uint32_t sec_rand(void)
{
    uint32_t ret = hrand_uint(&g_ctx);

    return ret;
}

// Get an insecure 32-bit random number with less resources
uint32_t nsec_rand(void)
{
    // No entropy pool is used, and the system clock and backup data are taken as new random numbers.
    // Poor randomness and less resources are used.
    return (g_u_last_rand ^ ((uint32_t)(uapi_systick_get_us() + (uint32_t)rand())));
}

// Get multi-byte random number, up to HASH_DIGEST_LENGTH (32) bytes
void sec_rand_bytes(uint8_t *output, uint32_t output_len)
{
    hrand_bytes(&g_ctx, output, output_len);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
