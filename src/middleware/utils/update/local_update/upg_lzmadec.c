/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: UPG lzma decode functions source file
 */

#include <stddef.h>
#include <stdint.h>
#include "securec.h"
#include "chip_io.h"
#include "td_type.h"
#include "upg_alloc.h"
#include "upg_porting.h"
#include "upg_common.h"
#include "upg_debug.h"
#if (UPG_CFG_SUPPORT_RESOURCES_FILE == YES)
#include "dfx_file_operation.h"
#endif
#include "upg_encry.h"
#include "upg_lzmadec.h"

#define OTA_ENCRY_FLAG      0x3C7896E1
STATIC void *upg_lzma_alloc(const ISzAllocPtr unused, size_t size)
{
    unused(unused);
    void *p;
    p = upg_malloc((uint32_t)size);
    if (p == NULL) {
        upg_log_err("[UPG] upg_lzma_alloc failed!!! size = 0x%x\r\n", (uint32_t)size);
        return NULL;
    }
    return p;
}

STATIC void upg_lzma_free(const ISzAllocPtr unused, void *address)
{
    unused(unused);
    if (address == NULL) {
        return;
    }
    upg_free(address);
}

static inline void lzmadec_init(CLzmaDec *p)
{
    return LzmaDec_Init(p);
}

__attribute__((unused)) static inline int32_t lzmadec_decodetodic(CLzmaDec *p, SizeT dic_limit, const Byte *src,
    SizeT *srclen, ELzmaFinishMode finish_mode, ELzmaStatus *status)
{
    return LzmaDec_DecodeToDic(p, dic_limit, src, srclen, finish_mode, status);
}

static inline void lzmadec_freeprobs(CLzmaDec *p, ISzAllocPtr alloc)
{
    return LzmaDec_FreeProbs(p, alloc);
}

static inline int32_t lzmadec_allocateprobs(CLzmaDec *p, const Byte *props, unsigned props_size, ISzAllocPtr alloc)
{
    return LzmaDec_AllocateProbs(p, props, props_size, alloc);
}

STATIC SRes lzmadec_decodetobuf(CLzmaDec *p, Byte *dest, SizeT *dest_len, const Byte *src, SizeT *src_len,
                                ELzmaFinishMode finish_mode, ELzmaStatus *status)
{
    return LzmaDec_DecodeToBuf(p, dest, dest_len, src, src_len, finish_mode, status);
}

uint32_t upg_lzma_init_buf(upg_lzma_decode2_data_t *val_data)
{
    ISzAlloc alloc;
    val_data->buf.in_size = 0;
    val_data->buf.write_pos = 0;
    val_data->buf.inbuf = upg_lzma_alloc(&alloc, IN_BUF_SIZE);
    if (val_data->buf.inbuf == NULL) {
        return ERRCODE_MALLOC;
    }

    val_data->buf.outbuf = upg_lzma_alloc(&alloc, OUT_BUF_SIZE);
    if (val_data->buf.outbuf == NULL) {
        upg_lzma_free(&alloc, val_data->buf.inbuf);
        val_data->buf.inbuf = NULL;
        return ERRCODE_MALLOC;
    }
    return ERRCODE_SUCC;
}

uint32_t upg_lzma_init(CLzmaDec *p, upg_lzma_decode2_data_t *val_data, const Byte *props, uint32_t props_len)
{
    LzmaDec_Construct(p);

    ISzAlloc alloc;
    alloc.Alloc = upg_lzma_alloc;
    alloc.Free = upg_lzma_free;

    if (props_len < LZMA_PROPS_SIZE) {
        return ERRCODE_UPG_INVALID_PARAMETER;
    }

    int32_t res = lzmadec_allocateprobs(p, props, LZMA_PROPS_SIZE, &alloc);
    if (res != 0) {
        return (uint32_t)res;
    }

    uint32_t unpack_size = 0;
    for (uint32_t i = 0; i < props_len - LZMA_PROPS_SIZE; i++) {
        unpack_size += (uint32_t)props[LZMA_PROPS_SIZE + i] << (i * 8); /* 8: 用于计算偏移位数 */
    }
    uint32_t dic_size = unpack_size;

    if ((p->prop).dicSize <= unpack_size) {
        dic_size = (p->prop).dicSize;
    }

    if (dic_size == 0) {
        return ERRCODE_UPG_INVALID_PARAMETER;
    }

    p->dic = upg_lzma_alloc(&alloc, dic_size);
    if (p->dic == NULL) {
        return ERRCODE_MALLOC;
    }

    p->dicBufSize = dic_size;
    val_data->decompress_len = unpack_size;
    if (upg_lzma_init_buf(val_data) != ERRCODE_SUCC) {
        upg_lzma_free(&alloc, p->dic);
        p->dic = NULL;
        return ERRCODE_MALLOC;
    }

    lzmadec_init(p);
    return ERRCODE_SUCC;
}

void upg_lzma_deinit(CLzmaDec *p, upg_lzma_decode2_data_t *val_data)
{
    ISzAlloc alloc;
    alloc.Alloc = upg_lzma_alloc;
    alloc.Free = upg_lzma_free;

    upg_lzma_free(&alloc, p->dic);
    p->dic = NULL;

    lzmadec_freeprobs(p, &alloc);

    upg_lzma_free(&alloc, val_data->buf.inbuf);
    val_data->buf.inbuf = NULL;

    upg_lzma_free(&alloc, val_data->buf.outbuf);
    val_data->buf.outbuf = NULL;
}

STATIC bool upg_lzma_dec_check_ret(SRes res, uint32_t unpack_size, bool in_out_zero, ELzmaStatus status, SRes *ret)
{
    if ((res != SZ_OK) || (unpack_size == 0)) {
        *ret = res;
        return true;
    }

    if (in_out_zero) {
        if (status != LZMA_STATUS_FINISHED_WITH_MARK) {
            *ret = SZ_ERROR_DATA;
            return true;
        }
        *ret = res;
        return true;
    }
    return false;
}

/* 读镜像，并解密 */
static uint32_t upg_lzma_read_pkt_decry(upg_lzma_decode2_data_t *data, const upg_image_header_t *image)
{
    uint32_t in_size = (data->compress_len > IN_BUF_SIZE) ? IN_BUF_SIZE : data->compress_len;
    UNUSED(image);
    if (upg_read_fota_pkg_data(data->in_offset, data->buf.inbuf, &in_size) != ERRCODE_SUCC) {
        return SZ_ERROR_DATA;
    }
#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_COMPRESS_ENCRY
    if (upg_decry_fota_pkt(data->buf.inbuf, in_size, image) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
#endif
    data->buf.in_size = in_size;   /* 本次待处理长度 */
    data->in_offset += in_size;    /* 读imag偏移地址 */
    data->compress_len -= in_size; /* 待处理imag长度 */
    return ERRCODE_SUCC;
}

ELzmaFinishMode upg_lzma_get_mode(SizeT *out_processed, uint32_t unpack_size)
{
    /* 最后一段解压后的长度不足4k, 解压需要切换finish模式 */
    ELzmaFinishMode finish_mode = LZMA_FINISH_ANY;
    if (unpack_size < OUT_BUF_SIZE) {
        *out_processed = (SizeT)unpack_size;
        finish_mode = LZMA_FINISH_END;
    }
    return finish_mode;
}

uint32_t upg_lzma_write_image(upg_lzma_decode2_data_t *data, const upg_image_header_t *image, td_bool *first_pkt)
{
    uint32_t ret = ERRCODE_SUCC;
    UNUSED(first_pkt);
    /* 直接写flash */
    if (image->re_enc_flag != OTA_ENCRY_FLAG) {
#if defined(UPG_CFG_SUPPORT_ERASE_WHOLE_IMAGE) && defined(YES) && (UPG_CFG_SUPPORT_ERASE_WHOLE_IMAGE == YES)
        ret = upg_write_new_image_data(data->out_offset, data->buf.outbuf, &data->buf.write_pos, data->image_id, false);
#else
        ret = upg_write_new_image_data(data->out_offset, data->buf.outbuf, &data->buf.write_pos, data->image_id, true);
#endif
        if (ret != ERRCODE_SUCC) {
            return SZ_ERROR_DATA;
        }
        data->out_offset += data->buf.write_pos;
        data->buf.write_pos = 0;
        return ERRCODE_SUCC;
    }
#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_COMPRESS_ENCRY
    /* 加密后写flash */
    ret = upg_encry_and_write_pkt(data, image, first_pkt);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg_encry_fota_pkt fail ret = 0x%x \r\n", ret);
        return ERRCODE_FAIL;
    }
#endif
    return ret;
}

uint32_t upg_lzma_decode_to_midbuf(CLzmaDec *p, upg_lzma_decode2_data_t *data,
    const upg_image_header_t *image, td_bool *first_pkt)
{
    SRes res_ret;
    upg_lzma_buf_t *buf = &data->buf;
    size_t in_pos = 0;             /* 解压进度 */
    size_t in_size = buf->in_size; /* 待解压长度 */
    ELzmaStatus status  = LZMA_STATUS_NOT_SPECIFIED;
    ELzmaFinishMode finish_mode = LZMA_FINISH_ANY;

    while (in_pos < in_size) {
        SizeT in_processed = in_size - in_pos; /* 本次待解压长度 */
        SizeT out_processed = OUT_BUF_SIZE;    /* 预计解压后长度 */
        finish_mode = upg_lzma_get_mode(&out_processed, data->decompress_len);
        SRes res = lzmadec_decodetobuf(p, buf->outbuf + buf->write_pos, &out_processed, buf->inbuf + in_pos,  /* 解压 */
            &in_processed, finish_mode, &status);
        if (res != SZ_OK) {
            upg_log_err("[UPG] lzmadec_decodetobuf fail ret = 0x%x. \r\n", res);
            return res;
        }

        in_pos += in_processed;
        data->decompress_len -= out_processed;
        buf->write_pos += out_processed;
        upg_calculate_and_notify_process(out_processed);
        /* midbuf缓冲区剩余空间不足4k 或 若解压完成，则写入flash */
        if ((OUT_BUF_SIZE - buf->write_pos) < IN_BUF_SIZE || data->decompress_len == 0 || in_pos == in_size) {
            if (upg_lzma_write_image(data, image, first_pkt) != ERRCODE_SUCC) {
                upg_log_err("[UPG] upg_lzma_write_image fail.\r\n");
                return ERRCODE_FAIL;
            }
        }
        if (upg_lzma_dec_check_ret(res, data->decompress_len, (in_processed == 0 && out_processed == 0),
            status, &res_ret)) {
            return (uint32_t)res_ret;
        }
    }
    return ERRCODE_SUCC;
}

/* 压缩加密升级：解密->解压->加密 */
uint32_t upg_lzma_decode(CLzmaDec *p, upg_lzma_decode2_data_t *data, const upg_image_header_t *image)
{
    td_bool first_pkt = TD_TRUE;
    uint32_t ret = ERRCODE_SUCC;

    while (data->compress_len > 0) {
        /* 读镜像 -> 解密 */
        ret = upg_lzma_read_pkt_decry(data, image);
        if (ret != ERRCODE_SUCC) {
            upg_log_err("[UPG] upg_lzma_read_pkt_decry fail ret = 0x%x.\r\n", ret);
            return ret;
        }

        /* 解压 -> 加密 -> 写flash */
        ret = upg_lzma_decode_to_midbuf(p, data, image, &first_pkt);
        if (ret != ERRCODE_SUCC) {
            upg_log_err("[UPG] upg_lzma_decode_to_buf fail ret = 0x%x.\r\n", ret);
            return ret;
        }

        upg_watchdog_kick(); /* 踢狗 */
    }
    return ret;
}

#if (UPG_CFG_SUPPORT_RESOURCES_FILE == YES)
errcode_t upg_resource_file_decode(CLzmaDec *p, upg_lzma_decode2_data_t *data, upg_resource_node_t *file_info)
{
    errcode_t ret = ERRCODE_SUCC;
    size_t in_size = 0; /* 待读出压缩数据的大小 */
    size_t out_len = 0; /* 解压缩后数据的大小 */
    uint32_t file_len = file_info->file_len; /* 文件的大小，即本次需要解压缩出来的原始数据长度 */
    uint32_t in_offset = data->in_offset; /* 已解压数据的压缩包的偏移位置 */
    uint32_t file_offset = 0; /* 本次解压出的数据长度, 用作写入的偏移位置 */
    ELzmaStatus status  = LZMA_STATUS_NOT_SPECIFIED;

    if (file_info->offset != data->out_offset) {
        return ERRCODE_UPG_INVALID_OFFSET;
    }

    int32_t write_fd = dfx_file_open_for_write((const char *)file_info->file_path);
    if (write_fd < 0) {
        return ERRCODE_UPG_FILE_OPEN_FAIL;
    }

    while (file_len > 0) {
        in_size = (data->compress_len > IN_BUF_SIZE) ? IN_BUF_SIZE : data->compress_len;
        ret = upg_read_fota_pkg_data(in_offset, data->buf.inbuf, (uint32_t *)(uintptr_t)&in_size);
        if (ret != ERRCODE_SUCC) {
            break;
        }

        out_len = (file_len > OUT_BUF_SIZE) ? OUT_BUF_SIZE : file_len;
        if (lzmadec_decodetobuf(p, data->buf.outbuf, &out_len, data->buf.inbuf, &in_size,
            LZMA_FINISH_ANY, &status) != SZ_OK) {
            ret = ERRCODE_UPG_DECOMPRESS_FAIL;
            break;
        }

        if (dfx_file_write_fd(write_fd, file_offset, data->buf.outbuf, out_len) != (int32_t)out_len) {
            ret = ERRCODE_UPG_FILE_WRITE_FAIL;
            break;
        }

        in_offset += in_size;
        data->compress_len -= in_size;
        file_offset += out_len;
        file_len -= out_len;

        upg_calculate_and_notify_process((uint32_t)out_len);

        if (data->compress_len == 0 || (in_size == 0 && out_len == 0)) {
            break;
        }
    }

    data->in_offset = in_offset;
    data->out_offset += file_offset;
    if (in_size == 0 && out_len == 0 && status != LZMA_STATUS_FINISHED_WITH_MARK && file_info->file_len != 0) {
        ret = ERRCODE_UPG_DECOMPRESS_FAIL;
    }

    dfx_file_fsync(write_fd);
    dfx_file_close(write_fd);
    return ret;
}
#else
errcode_t upg_resource_file_decode(CLzmaDec *p, upg_lzma_decode2_data_t *data, upg_resource_node_t *file_info)
{
    unused(p);
    unused(data);
    unused(file_info);
    return ERRCODE_SUCC;
}
#endif