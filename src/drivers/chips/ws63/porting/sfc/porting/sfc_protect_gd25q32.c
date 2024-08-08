/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides sfc port template \n
 *
 * History: \n
 * 2022-11-30， Create file. \n
 */
#include "hal_sfc_v150.h"
#ifndef BUILD_NOOSAL
#include "soc_osal.h"
#endif
#include "debug_print.h"
#include "hal_sfc_v150_regs_op.h"
#include "osal_interrupt.h"
#include "securec.h"
#include "tcxo.h"
#include "sfc.h"
#include "sfc_porting.h"
#include "sfc_protect.h"
#include "sfc_protect_gd25q32.h"

#define SR_READ_RETRY_TIMES (3)

#define SR3_VALID_MASK  0x61
#define SR3_VALID_VAL   0x20

static inline bool is_sr1_valid(uint8_t sr1)
{
    return ((sr1 & SR1_VALID_MASK) == SR1_VALID_VAL);
}

static inline bool is_sr2_valid(uint8_t sr2)
{
    return ((sr2 & SR2_VALID_MASK) == SR2_VALID_VAL);
}

static inline bool is_sr3_valid(uint8_t sr3)
{
    return ((sr3 & SR3_VALID_MASK) == SR3_VALID_VAL);
}

static void sfc_port_delay_us(void)
{
#define DELAY_US_MAX 100
    uapi_tcxo_delay_us((uapi_tcxo_get_count() % DELAY_US_MAX) + 1);
}

static bool sfc_port_is_sr_valid(uint8_t rd_cmd)
{
    bool valid;
    uint8_t expect;
    uint8_t idx;
    uint8_t sr = (uint8_t)sfc_port_read_sr(rd_cmd);

    if (rd_cmd == SPI_CMD_RDSR_1) {
        valid = is_sr1_valid(sr);
        expect = SR1_VALID_VAL;
        idx = 1; // 1 : SR1
    } else if (rd_cmd == SPI_CMD_RDSR_2) {
        valid = is_sr2_valid(sr);
        expect = SR2_VALID_VAL;
        idx = 2; // 2 : SR2
    } else {
        valid = is_sr3_valid(sr);
        expect = SR3_VALID_VAL;
        idx = 3; // 3 : SR3
    }

    if (!valid) {
        print_str("SR%d[0x%x] needs fixing, expect[0x%x]\r\n", idx, sr, expect);
    }

    return valid;
}

static errcode_t sfc_port_gd25q32_read_sr(uint8_t rd_cmd)
{
    uint32_t i;
    bool valid;

    for (i = 0; i < SR_READ_RETRY_TIMES; i++) {
        valid = sfc_port_is_sr_valid(rd_cmd);
        sfc_port_delay_us();
        if (!valid) {
            return ERRCODE_FAIL;
        }
    }
    return ERRCODE_SUCC;
}

static inline void sfc_port_gd25q32_write_sr(uint8_t cmd)
{
    uint8_t data = SR1_VALID_VAL;

    if (cmd == SPI_CMD_WRSR_2) {
        data = SR2_VALID_VAL;
    } else if (cmd == SPI_CMD_WRSR_3) {
        data = SR3_VALID_VAL;
    }

    sfc_port_write_sr(false, cmd, data);
}

errcode_t sfc_port_fix_sr_gd25q32(void)
{
    errcode_t err1;
    errcode_t err2;
    errcode_t err3;
    uint32_t i;
#define SR_FIX_DELAY_MS  60
    for (i = 0; i < SR_READ_RETRY_TIMES; i++) {
        err1 = sfc_port_gd25q32_read_sr(SPI_CMD_RDSR_1);
        err2 = sfc_port_gd25q32_read_sr(SPI_CMD_RDSR_2);
        err3 = sfc_port_gd25q32_read_sr(SPI_CMD_RDSR_3);
        if ((err1 == ERRCODE_SUCC) && (err2 == ERRCODE_SUCC) && (err3 == ERRCODE_SUCC)) {
            if (i == 0) {
                print_str("No need to fix SR!\r\n");
            } else {
                print_str("SR fix ok!\r\n");
            }
            return ERRCODE_SUCC;
        }

        if (err1 != ERRCODE_SUCC) {
            sfc_port_gd25q32_write_sr(SPI_CMD_WRSR_1);
        }

        if (err2 != ERRCODE_SUCC) {
            sfc_port_gd25q32_write_sr(SPI_CMD_WRSR_2);
        }

        if (err3 != ERRCODE_SUCC) {
            sfc_port_gd25q32_write_sr(SPI_CMD_WRSR_3);
        }
        uapi_tcxo_delay_ms(SR_FIX_DELAY_MS);
    }
    return ERRCODE_FAIL;
}
