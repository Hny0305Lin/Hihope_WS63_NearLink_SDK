/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: UPG lzma Interface Header.
 */

#ifndef UPG_LZMADEC_H
#define UPG_LZMADEC_H

#include <stdint.h>
#include "LzmaDec.h"
#include "upg_definitions.h"
#include "upg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/* header: 5 bytes of LZMA properties and 8 bytes of uncompressed size */
#define LZMA_HEAD_SIZE       ((LZMA_PROPS_SIZE) + 8) /* 8: lzma解压算法固定长度 */
#define LZMA_HEAD_ALIGN_SIZE ((LZMA_HEAD_SIZE) + 3)  /* PROPS(5byte) + head(8byte) + fill_align(3byte) */
#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_COMPRESS_ENCRY
#define OUT_BUF_SIZE 0x4000
#else
#define OUT_BUF_SIZE 0x1000
#endif
#define IN_BUF_SIZE  0x1000

/* 本块数据解压处理结构体 */
typedef struct {
    uint8_t *inbuf;     /* 暂存从压缩分区读取数据 */
    uint8_t *outbuf;    /* 暂存写入运行分区的数据 */
    uint32_t write_pos; /* 写入outbuf缓冲区下标 */
    uint32_t in_size;   /* 本次读取数据块，解压前数据长度 */
} upg_lzma_buf_t;

typedef struct upg_lzma_decode2_data {
    uint32_t image_id;
    uint32_t in_offset;       /* 已解压部分，解压前的总长度，用作当前待解压的下标 */
    uint32_t out_offset;      /* 已解压部分，解压后的总长度，用作解压后写入flash的偏移 */
    uint32_t compress_len;    /* 剩余待解压部分，解压前的总长度 */
    uint32_t decompress_len;  /* 剩余待解压部分，解压后的总长度 */
    upg_lzma_buf_t buf;
} upg_lzma_decode2_data_t;

uint32_t upg_lzma_init(CLzmaDec *p, upg_lzma_decode2_data_t *val_data, const Byte *props, uint32_t props_len);
void upg_lzma_deinit(CLzmaDec *p, upg_lzma_decode2_data_t *val_data);
uint32_t upg_lzma_decode(CLzmaDec *p, upg_lzma_decode2_data_t *data, const upg_image_header_t *image);
errcode_t upg_resource_file_decode(CLzmaDec *p, upg_lzma_decode2_data_t *data, upg_resource_node_t *file_info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* UPG_LZMADEC_H */
