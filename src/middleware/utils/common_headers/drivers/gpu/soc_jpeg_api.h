/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description:   jpeg api interface
 * Author:
 * Create:  2021-10-15
 */

#ifndef SOC_JPEG_API_H
#define SOC_JPEG_API_H

#include <stdio.h>
#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup jpeg_type jpeg_type
 * @ingroup  gpu_api
 * @{
 */

/*-------------------------------- macro define -------------------------------------------*/

#define JPEG_COMPONENTS  2

/*-------------------------------- struct define ------------------------------------------*/

/**
 * @if Eng
 * @brief  Zoom factor.
 * @else
 * @brief  缩放倍数
 * @endif
 */
typedef enum {
    JPEG_SCALE_1 = 1, /*!< @if Eng Raw resolution output without scaling.
                           @else   原始分辨率输出，无缩放 @endif */
    JPEG_SCALE_2 = 2, /*!< @if Eng Reduce output by 2x.
                           @else   缩小2倍输出 @endif */
    JPEG_SCALE_4 = 4, /*!< @if Eng Reduce output by 4x.
                           @else   缩小4倍输出 @endif */
    JPEG_SCALE_8 = 8, /*!< @if Eng Reduce output by 8x.
                           @else   缩小8倍输出 @endif */
    JPEG_SCALE_MAX    /*!< @if Eng Invalid value.
                           @else   无效值 @endif */
} jpeg_scale_multiple;

/**
 * @if Eng
 * @brief  Color space.
 * @else
 * @brief  颜色空间
 * @endif
 */
typedef enum {
    JPEG_COLOR_SPACE_YUV400 = 0, /*!< @if Eng The format of the original image and output image is YUV400.
                                      @else   支持原图及输出图片格式为yuv400 @endif */
    JPEG_COLOR_SPACE_YUV420,     /*!< @if Eng The format of the original image and output image is YUV420.
                                      @else   支持原图及输出图片格式为yuv420 @endif */
    JPEG_COLOR_SPACE_YUV422_21,  /*!< @if Eng The format of the original image and output image is yuv420_21.
                                      @else   支持原图及输出图片格式为yuv420_21 @endif */
    JPEG_COLOR_SPACE_YUV422_12,  /*!< @if Eng The format of the original image and output image is yuv420_12.
                                      @else   支持原图及输出图片格式为yuv420_12 @endif */
    JPEG_COLOR_SPACE_YUV444,     /*!< @if Eng The format of the original image and output image is yuv444.
                                      @else   支持原图及输出图片格式为yuv444 @endif */
    JPEG_COLOR_SPACE_ARGB8888,   /*!< @if Eng Only the ARGB8888 format is supported..
                                      @else   仅支持输出图片格式为ARGB8888 @endif */
    JPEG_COLOR_SPACE_RGB888,     /*!< @if Eng Only the RGB888 format is supported..
                                      @else   仅支持输出图片格式为RGB888 @endif */
    JPEG_COLOR_SPACE_MAX         /*!< @if Eng Invalid value.
                                      @else   无效值 @endif */
} jpeg_color_space;

/**
 * @if Eng
 * @brief  Input stream information.
 * @else
 * @brief  输入流信息
 * @endif
 */
typedef struct jpeg_source_struct {
    FILE *infile;                              /*!< @if Eng Input stream file handle.
                                                    @else   输入流文件句柄 @endif */
    td_char *vir_buf;                          /*!< @if Eng Virtual address for storing input streams.
                                                    @else   输入码流存放的虚拟地址 @endif */
    td_u32 phy_buf;                            /*!< @if Eng Physical address for storing input streams.
                                                    @else   输入码流存放的物理地址 @endif */
    td_u32 size;                               /*!< @if Eng Total size of input streams.
                                                    @else   输入码流总大小 @endif */
} *jpeg_source_ptr;

/**
 * @if Eng
 * @brief  Output Buffer.
 * @else
 * @brief  输出Buffer
 * @endif
 */
typedef struct jpeg_dst_struct {
    td_char *vir_buf;           /*!< @if Eng Decoding output buffer virtual address.
                                     @else   解码输出Buffer虚拟地址 @endif */
    td_u32 phy_buf;             /*!< @if Eng Physical address of the decoding output buffer.
                                     @else   解码输出Buffer物理地址 @endif */
    td_u32 size;                /*!< @if Eng Address size of the decoding output buffer.
                                     @else   解码输出Buffer地址大小 @endif */
    td_u32 stride;              /*!< @if Eng Row spacing of the decoding output buffer address.
                                     @else   解码输出Buffer地址行间距 @endif */
    td_u32 uv_stride;           /*!< @if Eng Decoding output UV buffer address line spacing.
                                     @else   解码输出 UV Buffer地址行间距 @endif */
    td_u32 uv_offset;           /*!< @if Eng Offset of the decoding output UV buffer relative to the start address.
                                     @else   解码输出 UV Buffer相对起始地址偏移 @endif */
} *jpeg_dst_ptr;

/**
 * @if Eng
 * @brief  Thumbnail Time Definition.
 * @else
 * @brief  缩略图时间定义
 * @endif
 */
typedef struct jpeg_time_struct {
    td_u16 year;        /*!< @if Eng Photo Year.
                             @else   拍照年 @endif */
    td_u8 month;        /*!< @if Eng Photo month.
                             @else   拍照月 @endif */
    td_u8 day;          /*!< @if Eng Photo day.
                             @else   拍照日 @endif */
    td_u8 hour;         /*!< @if Eng Photo hour.
                             @else   拍照时 @endif */
    td_u8 minute;       /*!< @if Eng Photo minute.
                             @else   拍照分 @endif */
    td_u8 second;       /*!< @if Eng Photo second.
                             @else   拍照秒 @endif */
} *jpeg_time_ptr;

/**
 * @if Eng
 * @brief  Thumbnail data stream.
 * @else
 * @brief  缩略图数据流
 * @endif
 */
typedef struct jpeg_thumbnail_struct {
    td_char *buffer;        /*!< @if Eng Virtual address for storing thumbnail streams.
                                 @else   缩略图码流存放的虚拟地址 @endif */
    td_u32 phy_buf;         /*!< @if Eng Physical address for storing thumbnail streams.
                                 @else   缩略图码流存放的物理地址 @endif */
    td_u32 size;            /*!< @if Eng Thumbnail stream size.
                                 @else   缩略图码流的大小@endif */
} *jpeg_thumbnail_ptr;

/**
 * @if Eng
 * @brief  Output Resolution Definition.
 * @else
 * @brief  输出分辨率定义
 * @endif
 */
typedef struct jpeg_output_struct {
    td_u32 width[JPEG_COMPONENTS];    /*!< @if Eng Output width of the YUV component (0-Y, 1_UV).
                                           @else   YUV分量的输出宽度，0-Y,1_UV @endif */
    td_u32 height[JPEG_COMPONENTS];   /*!< @if Eng Output height of the YUV component (0-Y, 1_UV).
                                           @else   YUV分量的输出高度，0-Y,1_UV @endif */
    td_u32 stride[JPEG_COMPONENTS];   /*!< @if Eng Line spacing of the YUV component (0-Y, 1_UV).
                                           @else   YUV分量的行间距，0-Y,1_UV @endif */
    td_u32 size[JPEG_COMPONENTS];     /*!< @if Eng Memory size of the YUV component. (0-Y, 1_UV).
                                           @else   YUV分量的内存大小，0-Y,1_UV @endif */
} *jpeg_output_ptr;

/**
 * @if Eng
 * @brief  Decoder Definition.
 * @else
 * @brief  解码器定义
 * @endif
 */
typedef struct jpeg_decompress_struct {
    td_void *private_data;                /*!< @if Eng Internal management data,
                                               which is bound to the decoder handle of the application.
                                               @else   内部管理数据，和应用的解码器句柄绑定 @endif */
    td_bool thumbnail_en;                 /*!< @if Eng Thumbnail enable.
                                               @else   缩略图使能 @endif */
    td_bool is_hdr;                       /*!< @if Eng hdr or sdr.
                                               @else   hdr 或者 sdr  @endif */
    td_bool output_yuv420sp;              /*!< @if Eng If this parameter is enabled,
                                               the output image format is YUV420SP.
                                               @else   使能此项，输出图片格式为yuv420sp @endif */
    td_u32 image_width;                   /*!< @if Eng Image width.
                                               @else   图片宽度 @endif */
    td_u32 image_height;                  /*!< @if Eng Image height.
                                               @else   图片高度 @endif */
    td_u32 output_width;                  /*!< @if Eng Decoder output width.
                                               @else   解码器输出宽度 @endif */
    td_u32 output_height;                 /*!< @if Eng Decoder output height.
                                               @else   解码器输出高度 @endif */
    td_u32 output_stride;                 /*!< @if Eng Decoder output line spacing.
                                               @else   解码器输出行间距 @endif */
    jpeg_color_space image_color_space;   /*!< @if Eng Image color format.
                                               @else   图片颜色格式 @endif */
    jpeg_color_space output_color_space;  /*!< @if Eng Decoder output color format.
                                               @else   解码器输出颜色格式 @endif */
    jpeg_scale_multiple scale;            /*!< @if Eng Decoding Output Reduction Multiplier.
                                               @else   解码输出缩小倍数 @endif */
    struct jpeg_output_struct output;     /*!< @if Eng Information required for decoding output memory allocation.
                                               @else   解码输出内存分配需要的信息 @endif */
    struct jpeg_time_struct time;         /*!< @if Eng Time information of an image.
                                               @else   图片的时间信息 @endif */
    struct jpeg_thumbnail_struct thumbnail; /*!< @if Eng Thumbnail data of a picture.
                                                 @else   图片的缩略图数据 @endif */
} *jpeg_decompress_ptr;

/**
 * @}
 */

/**
 * @defgroup jpeg_api jpeg_api
 * @ingroup  gpu_api
 * @{
 */

/*-------------------------------- func declares ------------------------------------------*/

/**
 * @if Eng
 * @brief  This interface is used to create a decoder.
           The time and thumbnail information can be obtained after the intestface is returned.
 * @param  [in]  src JPEG image stream data
 * @param  [in]  is_mem Specifies whether the input stream data is of the file type or address type.
 * @param  [out] dinfo Decoder handle. The application must provide memory space.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  创建解码器，该接口返回后可获取时间和缩略图信息
 * @param  [in]  src jpeg图片码流数据
 * @param  [in]  is_mem 指定输入的码流数据是文件类型还是地址类型的
 * @param  [out] dinfo 解码器句柄，需要应用提供内存空间
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32  uapi_jpeg_decomress_create(jpeg_decompress_ptr dinfo, jpeg_source_ptr src, td_bool is_mem);

/**
 * @if Eng
 * @brief  Start decoding. After the interface is returned, YUV data can be obtained.
 * @param  [in]  dinfo Decoder handle. The application must provide memory space.
 * @param  [in]  dst Address space information to be stored in the decoded output data.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  启动解码,该接口返回后可获取yuv数据
 * @param  [in]  dinfo 解码器句柄，需要应用提供内存空间
 * @param  [in]  dst 解码输出的数据要存放的地址空间信息
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32  uapi_jpeg_decompress_start(jpeg_decompress_ptr dinfo, jpeg_dst_ptr dst);

/**
 * @if Eng
 * @brief  Stop decoding.
 * @param  [in]  dinfo Decoder handle. The application must provide memory space.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  停止解码
 * @param  [in]  dinfo 解码器句柄，需要应用提供内存空间
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32  uapi_jpeg_decompress_stop(jpeg_decompress_ptr dinfo);

/**
 * @if Eng
 * @brief  Destroying a Decoder.
 * @param  [in]  dinfo Decoder handle. The application must provide memory space.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  销毁解码器
 * @param  [in]  dinfo 解码器句柄，需要应用提供内存空间
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_void uapi_jpeg_decompress_destroy(jpeg_decompress_ptr dinfo);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SOC_JPEG_API_H */
