/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: flashboot symc API functions implemention.
 *
 * Create: 2023-12-16
*/

#include "crypto_errno.h"
#include "drv_flashboot_cipher.h"
#include "crypto_common_def.h"

#include "crypto_osal_lib.h"
#include "hal_spacc_reg.h"

/* Timeout. */
#define CONFIG_SYMC_WAIT_TIMEOUT_IN_US 10000
#define CONFIG_SYMC_CLEAR_TIMEOUT_IN_US 10000
#define CONFIG_RKP_WAIT_TIMEOUT_IN_US 10000
#define CONFIG_KLAD_COM_ROUTE_TIMEOUT_IN_US 10000

#define SYMC_NODE_DEPTH         2

/* ! spacc symc int entry struct which is defined by hardware, you can't change it */
typedef struct {
    td_u32 sym_first_node : 1;         /* !<  Indicates whether the node is the first node */
    td_u32 sym_last_node : 1;          /* !<  Indicates whether the node is the last node */
    td_u32 rev1 : 7;                   /* !<  reserve */
    td_u32 odd_even : 1;               /* !<  Indicates whether the key is an odd key or even key */
    td_u32 rev2 : 22;                  /* !<  reserve */
    td_u32 sym_alg_length;             /* !<  symc data length */
    td_u32 sym_start_addr;             /* !<  symc start low addr */
    td_u32 sym_start_high;             /* !<  symc start high addr */
    td_u32 iv[4];                      /* !<  symc IV */
} crypto_symc_entry_in;

/* ! spacc symc out entry struct which is defined by hardware, you can't change it */
typedef struct {
    td_u32 rev1;           /* !<  reserve */
    td_u32 sym_alg_length; /* !<  syma data length */
    td_u32 sym_start_addr; /* !<  syma start low addr */
    td_u32 sym_start_high; /* !<  syma start high addr */
} crypto_symc_entry_out;


static const uint32_t g_symc_chn = 1;
static uint32_t g_node_idx = 0;
static crypto_symc_entry_in __attribute__((aligned(4))) g_entry_in[SYMC_NODE_DEPTH];
static crypto_symc_entry_out __attribute__((aligned(4))) g_entry_out[SYMC_NODE_DEPTH];

static int32_t inner_symc_romable_wait_done(void)
{
    uint32_t i;
    out_sym_chan_raw_int last_raw;

    for (i = 0; i < CONFIG_SYMC_WAIT_TIMEOUT_IN_US; i++) {
        last_raw.u32 = spacc_reg_read(OUT_SYM_CHAN_RAW_LAST_NODE_INT);
        last_raw.bits.out_sym_chan_raw_int &= (0x01 << g_symc_chn);
        if (last_raw.bits.out_sym_chan_raw_int != 0) {
            spacc_reg_write(OUT_SYM_CHAN_RAW_LAST_NODE_INT, last_raw.u32);
            break;
        }
        crypto_udelay(1);
    }
    if (i >= CONFIG_SYMC_WAIT_TIMEOUT_IN_US) {
        return ERROR_SYMC_CALC_TIMEOUT;
    }

    return 0;
}

int32_t drv_rom_cipher_symc_init(void)
{
    /* 1. Lock one symc channel gloably. */
    spacc_reg_write(SPACC_SYM_CHN_LOCK, 0x10);  // 0x10: use bit[7:4], which is channel 1, 0x1 means tee cpu lock.

    /* 2. set channel node start addr & node depth. */
    spacc_reg_write(IN_SYM_CHN_NODE_START_ADDR_L(g_symc_chn), (uint32_t)(g_entry_in));
    spacc_reg_write(IN_SYM_CHN_NODE_LENGTH(g_symc_chn), SYMC_NODE_DEPTH);
    spacc_reg_write(OUT_SYM_CHN_NODE_START_ADDR_L(g_symc_chn), (uint32_t)(g_entry_out));
    spacc_reg_write(OUT_SYM_CHN_NODE_LENGTH(g_symc_chn), SYMC_NODE_DEPTH);

    /* 3. set the chn ctrl. */
    /*
     * 0x8000aa00:
     * bit[31] is 1'b1, means channel enable.
     * bit[15:12] is 4'ha, means symc_chn_ds is non-secure.
     * bit[11:8] is 4'ha, means symc_chn_ss is non-secure.
     */
    spacc_reg_write(IN_SYM_CHN_CTRL(g_symc_chn), 0x8000aa00);   // 0x8000aa00: refer to the above.
    return 0;
}

int32_t drv_rom_cipher_symc_deinit(void)
{
    /* 1. Clear symc channel. */

    /* 2. Unock symc channel. */
    spacc_reg_write(SPACC_SYM_CHN_LOCK, 0);
    return 0;
}

static int32_t inner_symc_crypto(uint32_t keyslot_chn_num, uint8_t *iv, uint32_t iv_length,
    uint64_t src, uint64_t dst, uint32_t length, bool is_decrypt)
{
    int32_t ret;
    in_sym_chn_node_wr_point in_node_wr_ptr;
    out_sym_chn_node_wr_point out_node_wr_ptr;
    uint32_t iv_get[CRYPTO_AES_IV_SIZE_IN_WORD];
    uint32_t ptr;
    in_sym_chn_key_ctrl key_ctrl = {0};
    uint32_t i;

    if (iv_length != CRYPTO_AES_IV_SIZE) {
        return -1;
    }
    /* 1. set the chn_key_ctrl and out_ctrl for aes-128-ctr encrypt/decrypt. */
    /*
     * 0x1620000 for encrypt, 0x11620000 for decrypt
     * bit[28] is 1'b0/1'b1, means sym_alg_decrypt is encrypt/decrypt.
     * bit[27:26] is 2'b0, means sym_alg_data_witdh is 128bit for AES.
     * bit[25:24] is 2'b1, means sym_alg_key_len is 128bit.
     * bit[23:20] is 4'h6, means sym_alg_mode is CTR.
     * bit[19:16] is 4'h2, means symc_alg_sel is AES.
     * bit[8:0] is keyslot_chn_num, means sym_key_chn_id is keyslot_chn_num.
     */
    if (is_decrypt) {
        key_ctrl.u32 = 0x11620000;    // 0x11620000: refer to the above.
    } else {
        key_ctrl.u32 = 0x1620000;     // 0x1620000: refer to the above.
    }
    key_ctrl.bits.sym_key_chn_id = keyslot_chn_num;
    spacc_reg_write(IN_SYM_CHN_KEY_CTRL(g_symc_chn), key_ctrl.u32);
    spacc_reg_write(IN_SYM_OUT_CTRL(g_symc_chn), 0);    // 0: disable dma copy.

    /* 2. set input node. */
    g_entry_in[g_node_idx].sym_first_node = 1;
    g_entry_in[g_node_idx].sym_last_node = 1;
    g_entry_in[g_node_idx].odd_even = 0;
    g_entry_in[g_node_idx].sym_alg_length = length;
    g_entry_in[g_node_idx].sym_start_addr = (uint32_t)src;
    g_entry_in[g_node_idx].sym_start_high = 0;
    (void)memcpy_s(g_entry_in[g_node_idx].iv, sizeof(g_entry_in[g_node_idx].iv), iv, iv_length);
    /* 3. set output node. */
    g_entry_out[g_node_idx].sym_alg_length = length;
    g_entry_out[g_node_idx].sym_start_addr = (uint32_t)dst;
    g_entry_out[g_node_idx].sym_start_high = 0;

    g_node_idx = (g_node_idx + 1) % SYMC_NODE_DEPTH;
    /* 4. start. */
    /* configure out node wr_point. */
    out_node_wr_ptr.u32 = spacc_reg_read(OUT_SYM_CHN_NODE_WR_POINT(g_symc_chn));
    ptr = out_node_wr_ptr.bits.sym_chn_node_wr_point + 1;
    out_node_wr_ptr.bits.sym_chn_node_wr_point = ptr % SYMC_NODE_DEPTH;
    spacc_reg_write(OUT_SYM_CHN_NODE_WR_POINT(g_symc_chn), out_node_wr_ptr.u32);

    /* configure in node wr_point. */
    in_node_wr_ptr.u32 = spacc_reg_read(IN_SYM_CHN_NODE_WR_POINT(g_symc_chn));
    ptr = in_node_wr_ptr.bits.sym_chn_node_wr_point + 1;
    in_node_wr_ptr.bits.sym_chn_node_wr_point = ptr % SYMC_NODE_DEPTH;
    spacc_reg_write(IN_SYM_CHN_NODE_WR_POINT(g_symc_chn), in_node_wr_ptr.u32);

    /* 5. wait done. */
    ret = inner_symc_romable_wait_done();
    if (ret != 0) {
        return ret;
    }

    for (i = 0; i < CRYPTO_AES_IV_SIZE_IN_WORD; i++) {
        iv_get[i] = spacc_reg_read(CHANN_CIPHER_IVOUT(g_symc_chn) + i * CRYPTO_WORD_WIDTH);
    }
    ret = memcpy_s(iv, iv_length, iv_get, sizeof(iv_get));
    if (ret != EOK) {
        return ERROR_MEMCPY_S;
    }
    return 0;
}

int32_t drv_rom_cipher_symc_encrypt(uint32_t keyslot_chn_num, uint8_t *iv, uint32_t iv_length,
    uint64_t src, uint64_t dst, uint32_t length)
{
    return inner_symc_crypto(keyslot_chn_num, iv, iv_length, src, dst, length, false);
}

int32_t drv_rom_cipher_symc_decrypt(uint32_t keyslot_chn_num, uint8_t *iv, uint32_t iv_length,
    uint64_t src, uint64_t dst, uint32_t length)
{
    return inner_symc_crypto(keyslot_chn_num, iv, iv_length, src, dst, length, true);
}