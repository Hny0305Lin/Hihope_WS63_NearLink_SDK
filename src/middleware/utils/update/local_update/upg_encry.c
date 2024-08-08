/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: UPG encry image functions source file
 */
#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_COMPRESS_ENCRY
#include <stddef.h>
#include <stdint.h>
#include "chip_io.h"
#include "securec.h"
#include "common_def.h"
#include "upg_common.h"
#include "upg_debug.h"
#include "upg_encry.h"

/* 加密镜像后，写flash */
uint32_t upg_encry_and_write_pkt(upg_lzma_decode2_data_t *data, const upg_image_header_t *image, td_bool *first_pkt)
{
    uint32_t ret;
    uint32_t head_offset = 0;
    uint32_t read_len = 0;
    uint32_t res_len = 0;
    upg_lzma_buf_t *buf = &data->buf;
    /* 升级包未配置加密直接返回 */
    if (image->re_enc_flag != OTA_ENCRY_FLAG) {
        return ERRCODE_SUCC;
    }

    /* 镜像头处理 */
    ret = upg_process_cryto_info(&head_offset, first_pkt);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* 加密要求16byte对齐 */
    read_len = buf->write_pos;
    res_len = read_len % BYTES_IN_ONE_LINE;
    read_len = (read_len / BYTES_IN_ONE_LINE) * BYTES_IN_ONE_LINE;
    upg_log_dbg("[UPG] upg_encry_fota_pkt: midbuf[write_pos:%u, read_len:%u, res_len:%u] \r\n",
        buf->write_pos, read_len, res_len);
    /* 将16byte对齐部分加密写flash */
    if (read_len != 0) {
        /* 加密 */
        ret = upg_encry_fota_pkt(buf->outbuf + head_offset, read_len - head_offset, image);
        if (ret != ERRCODE_SUCC) {
            upg_log_err("[UPG] upg_encry_fota_pkt:drv_rom_cipher_symc_decrypt failed, ret = 0x%x \r\n", ret);
            return ret;
        }

        /* 写flash */
#if defined(UPG_CFG_SUPPORT_ERASE_WHOLE_IMAGE) && defined(YES) && (UPG_CFG_SUPPORT_ERASE_WHOLE_IMAGE == YES)
        ret = upg_write_new_image_data(data->out_offset, buf->outbuf, &read_len, data->image_id, false);
#else
        ret = upg_write_new_image_data(data->out_offset, buf->outbuf, &read_len, data->image_id, true);
#endif
        if (ret != ERRCODE_SUCC) {
            return SZ_ERROR_DATA;
        }
        data->out_offset += read_len;
    }
    /* 将16byte对齐后剩余部分拷贝到outbuf头部 */
    if (res_len != 0) {
        if (memmove_s(buf->outbuf, OUT_BUF_SIZE, buf->outbuf + read_len, res_len) != EOK) {
            upg_log_err("[UPG] upg_encry_fota_pkt:memmove_s failed.\r\n");
            return ERRCODE_FAIL;
        }
        buf->write_pos = res_len;
        return ERRCODE_SUCC;
    }

    buf->write_pos = 0;
    return ERRCODE_SUCC;
}
#endif
