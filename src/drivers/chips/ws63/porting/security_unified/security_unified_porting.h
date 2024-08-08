/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: register address related to product.
 *
 * Create: 2023-01-18
*/

#ifndef SECURITY_UNIFIED_PORTING_H
#define SECURITY_UNIFIED_PORTING_H

/* Timer. */
#define crypto_udelay(us)       uapi_tcxo_delay_us(us)
#define crypto_msleep(ms)       uapi_tcxo_delay_ms(ms)

/* Register Base Addr. */
#define SEC_SUBSYS_BASE_ADDR    0x44100000

#define KM_REG_BASE_ADDR        (SEC_SUBSYS_BASE_ADDR + 0x12000)
#define KM_REG_SIZE             (0x2000)

#define PKE_REG_BASE_ADDR       (SEC_SUBSYS_BASE_ADDR + 0x10000)
#define PKE_REG_SIZE            (0x2000)

#define TRNG_REG_BASE_ADDR      (SEC_SUBSYS_BASE_ADDR + 0x14000)
#define TRNG_REG_SIZE           (0x1000)

#define SPACC_REG_BASE_ADDR     (SEC_SUBSYS_BASE_ADDR)
#define SPACC_REG_SIZE          (0x10000)

#define FAPC_REG_BASE_ADDR      (0x48000000)
#define FAPC_REG_SIZE           (0x2000)

/* define micro related to memory pool. */
#define MAX_PROCESS_NUM             1
#define CRYPTO_HASH_VIRT_CHN_NUM    4
#define CRYPTO_SYMC_VIRT_CHN_NUM    4

/* symc hard channel num. */
#define CRYPTO_SYMC_HARD_CHANNEL_MAX    3
#define CRYPTO_SYMC_HARD_CHANNEL_MASK   (0xFFFE)

/* hash hard channel num. */
#define CRYPTO_HASH_HARD_CHN_CNT        3
#define CRYPTO_HASH_HARD_CHN_MASK       (0xFFFE)

#define CRYPTO_DRV_DMA_SIZE             (128)
#define CRYPTO_DRV_AAD_SIZE             (128)
#define CRYPTO_HASH_DRV_BUFFER_SIZE     (256)

/* Define the time out us */
#define PKE_TIME_OUT_US               10000000
#define PKE_MAX_TIMES                 3000

#define HASH_TIME_OUT_US              10000000
#define CRYPTO_HAL_HASH_CALC_TIMEOUT  6000

#define CRYPTO_SYMC_CLEAR_TIMEOUT     6000
#define CRYPTO_SYMC_WAIT_TIMEOUT      6000

#define KSLOT_TIMEOUT                 1000000
#define HKL_LOCK_TIMEOUT              10000
#define KLAD_CLR_ROUTE_TIMEOUT        1000000
#define KLAD_COM_ROUTE_TIMEOUT        1000000
#define RKP_LOCK_TIMEOUT_IN_US        1000000
#define RKP_WAIT_TIMEOUT_IN_US        1000000

/* PKE related defines */
/* define PKE rom lib related register micro. */
#ifdef DEBUG_IN_RAM
#define PKE_ROM_LIB_START_ADDR              ((uintptr_t)g_instr_rom)  /* the instr start address */
#else
#define PKE_ROM_LIB_START_ADDR              (0xc00000)  /* the instr start address */
#endif
#define PKE_ROM_LIB_INVALID_ADDR            0xFFFFFFFF
#define ROM_LIB_INSTR_NUM                   646         /* define the instr number, that put in the ROM. */

/* for romable code patch */
#ifdef CRYPTO_ROM_ENABLE
#define CRYPTO_STATIC
#else
#define CRYPTO_STATIC static
#endif

#endif