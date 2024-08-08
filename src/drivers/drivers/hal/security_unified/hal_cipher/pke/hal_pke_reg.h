/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: define the logic related register micro.
 *
 * Create: 2022-12-12
*/

#ifndef HAL_PKE_REG_H
#define HAL_PKE_REG_H

/************************************************** reg related MICRO DEFINE start************************************/
/* Define non-special val for secure enhancement */
#define PKE_NON_SPECIAL_VAL 0xA5C36987 // 0xA5C36987 is for secure enhancement (with bit 0/1 the same count)
#define PKE_START0_CODE 0xAA5
#define PKE_START1_CODE 0xA5A
#define PKE_BATCH_START_CODE 0x5AA
#define PKE_INT_NOMASK_FINISH_EFFECTIVE_CODE 0x5
#define PKE_INT_NOMASK_FINISH_INVALID_CODE 0xA
#define PKE_ALARM_STATUS_EFFECTIVE_CODE 0x5
#define PKE_ALARM_STATUS_INVALID_CODE 0xA
#define PKE_ALARM_STATUS_CLEAN_CODE 0x6B4A89C6
#define DEFAULT_MASK_CODE 0xE69BC3A7
#define PKE_DRAM_BLOCK_LENGTH 32
#define ALIGNED_TO_WORK_LEN_IN_BYTE 8
#define PKE_MONT_PARAM_LEN 2
#define PKE_FAIL_FLAG_CNT 0xB
#define PKE_MONT_BIT_LEN 64

/* define logic related register micro. */
#define PKE_WORK_LEN                    (0x00)
#define PKE_INSTR0                      (0x04)
#define PKE_INSTR1                      (0x08)
#define PKE_INSTR_ADDR_LOW              (0x0c)
#define PKE_INSTR_ADDR_HIG              (0x10)
#define PKE_INSTR_LEN                   (0x14)
#define PKE_MASK_RNG_CFG                (0x18)
#define PKE_MONT_PARA0                  (0x20)
#define PKE_MONT_PARA1                  (0x24)
#define PKE_INSTR_RDY                   (0x44)
#define PKE_BUSY                        (0x48)
#define PKE_NOISE_EN                    (0x4c)
#define PKE_START                       (0x40)
#define PKE_INT_ENABLE                  (0x80)
#define PKE_INT_NOMASK_STATUS           (0x84)
#define PKE_ALARM_STATUS                (0x8c)
#define PKE_FAILURE_FLAG                (0x90)
#define PKE_DRAM_CLR                    (0xc0)
#define PKE_DRAM_MASK                   (0xf0)
#define PKE_LOCK_CTRL                   (0x810)
#define PKE_LOCK_STATUS                 (0x814)
#define PKE_DRAM_BASE                   (0x1000)

/* define the instr mode num */
#define CMD_MUL_MOD 0
#define CMD_ADD_MOD 1
#define CMD_SUB_MOD 2
#define MAX_RAM_SECTION 127
#define OPCODE_OFFSET   0
#define APOS_OFFSET     4
#define BPOS_OFFSET     11
#define TPOS_OFFSET     18
#define RPOS_OFFSET     25

/************************************************** reg related MICRO DEFINE end************************************/

/************************************************** reg related struct start************************************/

/* Define the union U_PKE_LOCK_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pke_lock              : 1; /* [0]  */
        unsigned int    pke_lock_type         : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;
    /* Define an unsigned member */
    unsigned int    u32;
} pke_lock_ctrl;

/* Define the union U_PKE_LOCK_STATUS */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pke_lock_busy         : 1; /* [0]  */
        unsigned int    pke_unlock_fail       : 1; /* [1]  */
        unsigned int    reserved_0            : 2; /* [3..2]  */
        unsigned int    pke_lock_cnt          : 3; /* [6..4]  */
        unsigned int    reserved_1            : 1; /* [7]  */
        unsigned int    pke_lock_stat         : 8; /* [15..8]  */
        unsigned int    reserved_2            : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int    u32;
} pke_lock_status;

/* Define the union U_PKE_POWER_NOISE_LOCK */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    noise_en              : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} pke_noise_en;

/* Define the union U_PKE_INT_NOMASK_STATUS */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    finish_int_nomask   : 4; /* [0:3] */
        unsigned int    reserved_0          : 28; /* [4:31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} pke_int_no_mask_status;

/* Define the union U_PKE_ALARM_STATUS */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    alarm_int       : 4; /* [0:3] */
        unsigned int    reserved_0      : 28; /* [4:31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} pke_alarm_status;

/* Define the union U_PKE_FAILURE_FLAG */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pke_failure_flag    : 11; /* [0:10] */
        unsigned int    reserved_0          : 21; /* [11:31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} pke_failure_flag;

/* Define the union U_PKE_INT_ENABLE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    finish_int_enable     : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} pke_int_enable;

/* Define the union U_PKE_START */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pke_start0    : 4; /* [0:3] */
        unsigned int    pke_start1    : 4; /* [4:7] */
        unsigned int    pke_batch_start    : 4; /* [8:11] */
        unsigned int    reserved_0    : 20; /* [12:31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} pke_start;

/* Define the union U_PKE_BUSY */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pke_busy              : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} pke_busy;

/* Define the union U_PKE_DRAM_CLR */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dram_clr    : 1; /* [0] */
        unsigned int    reserved_0    : 31; /* [1:31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} pke_dram_clr;

/* Define the union U_PKE_MASK_RNG_CFG */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mask_rng_cfg    : 1; /* [0] */
        unsigned int    reserved_0    : 31; /* [1:31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} pke_mask_rng_cfg;

/* Define the union U_PKE_INSTR_RDY: */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    instr0_rdy    : 1; /* [0] */
        unsigned int    instr1_rdy    : 1; /* [1] */
        unsigned int    batch_instr_rdy    : 1; /* [2] */
        unsigned int    reserved_0    : 29; /* [3:31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} pke_instr_rdy;

/************************************************** reg related struct end************************************/

#endif