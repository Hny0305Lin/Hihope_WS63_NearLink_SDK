/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved. \n
 *
 * Description: Provides the data struct define of vau api \n
 */

#ifndef SOC_VAU_TYPE_H
#define SOC_VAU_TYPE_H

#include "td_base.h"
#include "soc_gfx_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup vau_type vau_type
 * @ingroup  gpu_type
 * @{
 */

 /**
 * @if Eng
 * @brief  Size of the compression header.
 * @else
 * @brief  压缩头的大小
 * @endif
 */
#define VAU_CMP_HEADER_SIZE 16

#define EXT_VAU_MAX_STOPS 7

typedef td_u32   ext_vau_color;

/**
 * @if Eng
 * @brief  matrix.
 * @else
 * @brief  矩阵
 * @endif
 */
typedef struct {
    td_float matrix[3][3];
} ext_vau_matrix;

typedef enum {
    EXT_VAU_BLUR_MODE_GAUSSIAN = 0,
    EXT_VAU_BLUR_MODE_MAX
} ext_vau_blur_mode;

typedef enum {
    EXT_VAU_COLOR_SPACE_BT709 = 0,
    EXT_VAU_COLOR_SPACE_BT2020,
    EXT_VAU_COLOR_SPACE_MAX
} ext_vau_color_space;

/**
 * @if Eng
 * @brief  alpha information.
 * @else
 * @brief  alpha信息
 * @endif
 */
typedef struct {
    td_bool is_alpha_max_128;   /*!< @if Eng Maximum alpha value.
                                     @else   alpha最大值 @endif */
    td_u8 alpha0;               /*!< @if Eng Alpha input value when the output format is 1555.
                                     @else   输出格式为1555时alpha输入值 @endif */
    td_u8 alpha1;               /*!< @if Eng Alpha input value when the output format is 1555.
                                     @else   输出格式为1555时alpha输出值 @endif */
    td_u8 alpha_thd;            /*!< @if Eng Alpha threshold when the output format is 1555.
                                     @else   输出格式为1555时alpha阈值 @endif */
    td_u8 global_alpha;         /*!< @if Eng Global alpha.
                                     @else   全局alpha @endif */
    td_bool golbal_premult_en;  /*!< @if Eng Global alpha premultiplication enable.
                                     @else   全局alpha预乘使能 @endif */
    td_bool global_alpha_en;    /*!< @if Eng Global alpha enable.
                                     @else   全局alpha使能 @endif */
    td_bool premult_en;         /*!< @if Eng Premultiplication enable.
                                     @else   预乘使能 @endif */
    td_bool pixel_alpha_en;     /*!< @if Eng Pixel alpha enable.
                                     @else   像素alpha使能 @endif */
} ext_vau_alpha_info;

/**
 * @if Eng
 * @brief  Paint mode.
 * @else
 * @brief  画笔模式
 * @endif
 */
typedef enum {
    EXT_VAU_PAINT_MODE_NONE = 0x0,     /*!< @if Eng Initialization Value.
                                            @else   初始化值 @endif */
    EXT_VAU_PAINT_MODE_STROKE = 0x1,   /*!< @if Eng Stroke only.
                                            @else   只做描边 @endif */
    EXT_VAU_PAINT_MODE_FILL = 0x2,     /*!< @if Eng Fill only.
                                            @else   只做内部填充 @endif */
    EXT_VAU_PAINT_MODE_ALL = EXT_VAU_PAINT_MODE_STROKE | EXT_VAU_PAINT_MODE_FILL, /*!< @if Eng
                                            Do both strokes and interior fills.
                                            @else   同时做描边和内部填充 @endif */
} ext_vau_paint_mode;

/**
 * @if Eng
 * @brief  Gradient Tile Mode.
 * @else
 * @brief  渐变平铺模式
 * @endif
 */
typedef enum {
    EXT_VAU_PAINT_TILE_MODE_CLAMP = 0,    /*!< @if Eng Extension mode: The color of the last stop point is extended.
                                               For hatches, expand the edge-most color.
                                               @else   延展模式，往外扩最后一个 stop 点的颜色;
                                               对于图案填充，往外扩最边缘的颜色 @endif */
    EXT_VAU_PAINT_TILE_MODE_REPEAT,       /*!< @if Eng Repeat mode: The color of each stop point is repeated.
                                               For hatches, the pattern is tiled repeatedly.
                                               @else   重复模式，重复每个 stop 点的颜色;
                                               对于图案填充，重复平铺该图案 @endif */
    EXT_VAU_PAINT_TILE_MODE_MIRROR,       /*!< @if Eng In mirroring mode, the color of each stop point is repeated
                                               reversely. For hatches, the pattern is repeatedly tiled in reverse.
                                               @else   镜像模式，反向重复每个 stop 点的颜色;
                                               对于图案填充，反向重复平铺该图案 @endif */
    EXT_VAU_PAINT_TILE_MODE_MAX           /*!< @if Eng Invalid value.
                                               @else   无效值 @endif */
} ext_vau_paint_tile_mode;

/**
 * @if Eng
 * @brief  paint type.
 * @else
 * @brief  画笔类型
 * @endif
 */
typedef enum {
    EXT_VAU_PAINT_TYPE_COLOR = 0,         /*!< @if Eng Solid fill.
                                               @else   纯色填充 @endif */
    EXT_VAU_PAINT_TYPE_COLOR_ALPHA_FF,    /*!< @if Eng Solid color fill, but alpha in the path is all FF.
                                               @else   纯色填充,但路径内alpha为全FF @endif */
    EXT_VAU_PAINT_TYPE_LINEAR_GRADIENT,   /*!< @if Eng Linear gradient fill.
                                               @else   线性渐变填充 @endif */
    EXT_VAU_PAINT_TYPE_RADIAL_GRADIENT,   /*!< @if Eng Radial gradient fill.
                                               @else   径向渐变填充 @endif */
    EXT_VAU_PAINT_TYPE_SWEEP_GRADIENT,    /*!< @if Eng Sweep gradient fill.
                                               @else   扫描渐变填充 @endif */
    EXT_VAU_PAINT_TYPE_PATTERN,           /*!< @if Eng Pattern fill.
                                               @else   图案填充 @endif */
    EXT_VAU_PAINT_TYPE_MAX                /*!< @if Eng Invalid input.
                                               @else   无效输入 @endif */
} ext_vau_paint_type;

/**
 * @if Eng
 * @brief  stroke cap type.
 * @else
 * @brief  stroke线冒类型
 * @endif
 */
typedef enum {
    EXT_VAU_STROKE_LINE_CAP_BUTT = 0,    /*!< @if Eng Wireless Cap .
                                              @else   无线帽 @endif */
    EXT_VAU_STROKE_LINE_CAP_ROUND,       /*!< @if Eng Round cable cap .
                                              @else   圆形线帽 @endif */
    EXT_VAU_STROKE_LINE_CAP_SQUARE,      /*!< @if Eng Square cable cap .
                                              @else   方形线帽 @endif */
    EXT_VAU_STROKE_LINE_CAP_MAX          /*!< @if Eng Invalid value.
                                              @else   无效值 @endif */
} ext_vau_stroke_line_cap;

/**
 * @if Eng
 * @brief  stroke connection type.
 * @else
 * @brief  stroke连接类型
 * @endif
 */
typedef enum {
    EXT_VAU_STROKE_LINE_JOIN_MITER = 0,    /*!< @if Eng miter.
                                                @else   斜接 @endif */
    EXT_VAU_STROKE_LINE_JOIN_ROUND,        /*!< @if Eng circular connection.
                                                @else   圆形连接 @endif */
    EXT_VAU_STROKE_LINE_JOIN_BEVEL,        /*!< @if Eng bevel connection.
                                                @else   斜面连接 @endif */
    EXT_VAU_STROKE_LINE_JOIN_MAX           /*!< @if Eng Invalid value.
                                                @else   无效值 @endif */
} ext_vau_stroke_line_join;

/**
 * @if Eng
 * @brief  Antialiasing attribution.
 * @else
 * @brief  抗锯齿属性
 * @endif
 */
typedef enum {
    EXT_VAU_RENDER_QUALITY_NONE_AA,    /*!< @if Eng No anti-aliasing.
                                            @else   不开抗锯齿 @endif */
    EXT_VAU_RENDER_QUALITY_FASTER,     /*!< @if Eng MSAA8X, Bessel stroke fit with 4-segment Bessel.
                                            @else   8X 多重采样抗锯齿模式，
                                            贝塞尔stroke使用4条贝塞尔拟合 @endif */
    EXT_VAU_RENDER_QUALITY_BETTER,     /*!< @if Eng MSAA8X, Bessel stroke fit with 32egment Bessel.
                                            @else   8X 多重采样抗锯齿模式，
                                            贝塞尔stroke使用32根直线拟合  @endif */
} ext_vau_render_quality;

/**
 * @if Eng
 * @brief  Rendering configuration.
 * @else
 * @brief  渲染配置
 * @endif
 */
typedef struct {
    ext_vau_render_quality render_quality;    /*!< @if Eng Rendering quality.
                                                   @else   渲染质量 @endif */
} ext_vau_render_cfg;

/**
 * @if Eng
 * @brief  point.
 * @else
 * @brief  点
 * @endif
 */
typedef struct {
    td_s32 x;
    td_s32 y;
} ext_vau_point;

/**
 * @if Eng
 * @brief  Line.
 * @else
 * @brief  直线
 * @endif
 */
typedef struct {
    td_float x0;
    td_float y0;
    td_float x1;
    td_float y1;
} ext_vau_line;


/**
 * @if Eng
 * @brief  Rectangle, Ellipse.
 * @else
 * @brief  矩形，椭圆
 * @endif
 */
typedef struct {
    td_float x;
    td_float y;
    td_float width;
    td_float height;
} ext_vau_rect, ext_vau_ellipse;

/**
 * @if Eng
 * @brief  Rounded Rectangle.
 * @else
 * @brief  圆角矩形
 * @endif
 */
typedef struct {
    td_float x;
    td_float y;
    td_float width;
    td_float height;
    td_float arc_width;
    td_float arc_height;
} ext_vau_round_rect;

/**
 * @if Eng
 * @brief  Arc Type.
 * @else
 * @brief  弧类型
 * @endif
 */
typedef enum {
    EXT_VAU_ARC_TYPE_OPEN,
    EXT_VAU_ARC_TYPE_CHORD,
    EXT_VAU_ARC_TYPE_PIE,

    EXT_VAU_ARC_TYPE_MAX,
} ext_vau_arc_type;

/**
 * @if Eng
 * @brief  Arc.
 * @else
 * @brief  弧
 * @endif
 */
typedef struct {
    td_float x;
    td_float y;
    td_float width;
    td_float height;
    td_float start_angle;
    td_float angle_extend;
    ext_vau_arc_type type;
} ext_vau_arc;

/**
 * @if Eng
 * @brief  solid paint attribution.
 * @else
 * @brief  纯色画笔属性
 * @endif
 */
typedef union {
    struct {
        td_u32 rgb : 24;
        td_u32 opa : 8;
    };
    td_u32 color;
} ext_vau_solid_attr;

/**
 * @if Eng
 * @brief  stops.
 * @else
 * @brief  停止点
 * @endif
 */
typedef struct {
    ext_vau_color *colors; /*!< @if Eng Stop Point Color Array.
                                @else   停止点颜色数组 @endif */
    td_float *offsets;     /*!< @if Eng Indicates the stop point offset array, which is expressed in floating point.
                                @else   停止点偏移数组，浮点表示 @endif */
    td_u8 num;             /*!< @if Eng Number of stop points, that is, the number of colors and offset arrays.
                                @else   停止点个数，即colors和offset的数组个数 @endif */
} ext_vau_stops;

/**
 * @if Eng
 * @brief  Linear Gradient Fill Properties.
 * @else
 * @brief  线性渐变填充属性
 * @endif
 */
typedef struct {
    ext_vau_point start_point;    /*!< @if Eng Start point coordinate.
                                       @else   起始点坐标 @endif */
    ext_vau_point end_point;      /*!< @if Eng end point coordinate.
                                       @else   终点坐标 @endif */
} ext_vau_linear_gradient_attr;

/**
 * @if Eng
 * @brief  Radial gradient fill attribution.
 * @else
 * @brief  径向渐变填充属性
 * @endif
 */
typedef struct {
    td_u16 radius;          /*!< @if Eng Gradient Radius.
                                 @else   渐变半径 @endif */
    ext_vau_point center;   /*!< @if Eng Gradient Center Point.
                                 @else   圆点 @endif */
} ext_vau_radial_gradient_attr;

/**
 * @if Eng
 * @brief  sweep gradient fill attribution.
 * @else
 * @brief  画笔渐变平铺模式
 * @endif
 */
typedef struct {
    ext_vau_point center;   /*!< @if Eng Gradient Center Point.
                                 @else   渐变中心点 @endif */
    td_u32 start_angle;     /*!< @if Eng Gradient Start Angle.
                                 @else   渐变起始角度 @endif */
} ext_vau_sweep_gradient_attr;

/**
 * @if Eng
 * @brief  Gradient paint attribution.
 * @else
 * @brief  渐变画笔属性
 * @endif
 */
typedef struct {
    ext_vau_paint_tile_mode tile_mode;       /*!< @if Eng Brush Gradient Tile Mode.
                                                  @else   画笔渐变平铺模式 @endif */
    ext_vau_stops stops;                     /*!< @if Eng stops info.
                                                  @else   停止点信息 @endif */
    union {
        ext_vau_linear_gradient_attr linear; /*!< @if Eng Linear gradient fill attribution.
                                                  @else   线性渐变填充属性 @endif */
        ext_vau_radial_gradient_attr radial; /*!< @if Eng Radial gradient fill attribution.
                                                  @else   径向渐变填充属性 @endif */
        ext_vau_sweep_gradient_attr sweep;   /*!< @if Eng Sweep gradient fill attribution.
                                                  @else   扫描渐变填充属性 @endif */
    };
} ext_vau_gradient_attr;

/**
 * @if Eng
 * @brief  Paint attribution.
 * @else
 * @brief  画笔属性
 * @endif
 */
typedef struct {
    ext_vau_paint_type paint_type;       /*!< @if Eng paint type.
                                              @else   画笔类型 @endif */
    union {
        ext_vau_solid_attr solid;        /*!< @if Eng solid paint attribution.
                                              @else   纯色画笔属性 @endif */
        ext_vau_gradient_attr gradient;  /*!< @if Eng Gradient paint attribution.
                                              @else   渐变画笔属性 @endif */
    };
} ext_vau_paint_attr;


/**
 * @if Eng
 * @brief  Dash.
 * @else
 * @brief  虚线
 * @endif
 */
typedef struct {
    td_bool enable;    /*!< @if Eng Whether to enable the dotted line function.
                            @else   是否使能虚线功能 @endif */
    float *array;      /*!< @if Eng dashed array.
                            @else   虚线数组 @endif */
    td_u32 count;      /*!< @if Eng Array Length.
                            @else   数组长度 @endif */
    float offset;      /*!< @if Eng Dotted Line Start Offset.
                            @else   虚线起点偏移 @endif */
    td_bool reset;     /*!< @if Eng Whether to redraw dashed lines for subpaths.
                            @else   子路径是否重新绘制虚线 @endif */
} ext_vau_dash;

/**
 * @if Eng
 * @brief  Stroke attribution.
 * @else
 * @brief  描边属性
 * @endif
 */
typedef struct {
    float width;                    /*!< @if Eng Line width.
                                         @else   线宽 @endif */
    ext_vau_dash dash;              /*!< @if Eng Dash information.
                                         @else   虚线信息 @endif */
    ext_vau_stroke_line_cap cap;    /*!< @if Eng Stroke cap type.
                                         @else   stroke线冒类型 @endif */
    ext_vau_stroke_line_join join;  /*!< @if Eng Stroke connection type.
                                         @else   stroke连接类型 @endif */
    float miter_limit;              /*!< @if Eng Maximum mitter length.
                                         @else   最大的miter长度 @endif */
    ext_vau_paint_attr paint_attr;  /*!< @if Eng Paint attribution.
                                         @else   画笔属性 @endif */
} ext_vau_stroke_attr;

/**
 * @if Eng
 * @brief  Filling type
 * @else
 * @brief  填充类型
 * @endif
 */
typedef enum {
    EXT_VAU_EVEN_ODD_FILL = 0,    /*!< @if Eng The filling rule is Even ODD..
                                       @else   奇偶规则 @endif */
    EXT_VAU_NON_ZERO_FILL,        /*!< @if Eng The filling rule is zero..
                                       @else   非零规则 @endif */
    EXT_VAU_FILL_MAX              /*!< @if Eng Invalid value.
                                       @else   无效值 @endif */
} ext_vau_fill_type;

/**
 * @if Eng
 * @brief  Internal Fill attribution
 * @else
 * @brief  内部填充属性
 * @endif
 */
typedef struct {
    ext_vau_fill_type type;         /*!< @if Eng Filling type.
                                       @else   填充类型 @endif */
    ext_vau_paint_attr paint_attr;  /*!< @if Eng Paint attribution.
                                       @else   画笔属性 @endif */
} ext_vau_fill_attr;

/**
 * @if Eng
 * @brief  Paint.
 * @else
 * @brief  画笔
 * @endif
 */
typedef struct {
    ext_vau_render_cfg render_cfg;    /*!< @if Eng Drawing configuration.
                                           @else   绘制配置 @endif */
    ext_vau_stroke_attr *stroke_attr; /*!< @if Eng Stroke Properties.
                                           @else   描边属性 @endif */
    ext_vau_fill_attr *fill_attr;     /*!< @if Eng Internal Fill Properties.
                                           @else   内部填充属性 @endif */
    ext_vau_paint_mode paint_mode;    /*!< @if Eng Paint mode.
                                           @else   画笔模式 @endif */
} ext_vau_paint;

/**
 * @if Eng
 * @brief  Command Count.
 * @else
 * @brief  命令计数
 * @endif
 */
typedef struct {
    td_bool enable;
    td_u16 move_count;
    td_u16 line_count;
    td_u16 quad_count;
    td_u16 cubic_count;
    td_u16 arc_count;
} ext_cmd_count;

typedef enum {
    VAU_PATH_ABSOLUTE = 0,
    VAU_PATH_RELATIVE = 1,
} vau_path_abs_rel;

typedef enum {
    VAU_PATH_CLOSE_PATH = (0 << 1),
    VAU_PATH_MOVE_TO    = (1 << 1),
    VAU_PATH_LINE_TO    = (2 << 1),
    VAU_PATH_HLINE_TO   = (3 << 1),
    VAU_PATH_VLINE_TO   = (4 << 1),
    VAU_PATH_QUAD_TO    = (5 << 1),
    VAU_PATH_CUBIC_TO   = (6 << 1),
    VAU_PATH_SQUAD_TO   = (7 << 1),
    VAU_PATH_SCUBIC_TO  = (8 << 1),
    VAU_PATH_SCCWARC_TO = (9 << 1),
    VAU_PATH_SCWARC_TO  = (10 << 1),
    VAU_PATH_LCCWARC_TO = (11 << 1),
    VAU_PATH_LCWARC_TO  = (12 << 1),
} vau_path_segment;

enum {
    VAU_PATH_MOVE_TO_DATA_NUM      = 2,
    VAU_PATH_LINE_TO_DATA_NUM      = 2,
    VAU_PATH_HLINE_TO_DATA_NUM     = 1,
    VAU_PATH_VLINE_TO_DATA_NUM     = 1,
    VAU_PATH_QUAD_TO_DATA_NUM      = 4,
    VAU_PATH_CUBIC_TO_DATA_NUM     = 6,
    VAU_PATH_SQUAD_TO_DATA_NUM     = 2,
    VAU_PATH_SCUBIC_TO_DATA_NUM    = 4,
    VAU_PATH_SCCWARC_TO_DATA_NUM   = 5,
    VAU_PATH_SCWARC_TO_DATA_NUM    = 5,
    VAU_PATH_LCCWARC_TO_DATA_NUM   = 5,
    VAU_PATH_LCWARC_TO_DATA_NUM    = 5,
};

typedef enum {
    VAU_PATH_MOVE_TO_ABS    = VAU_PATH_MOVE_TO    | VAU_PATH_ABSOLUTE,
    VAU_PATH_MOVE_TO_REL    = VAU_PATH_MOVE_TO    | VAU_PATH_RELATIVE,
    VAU_PATH_LINE_TO_ABS    = VAU_PATH_LINE_TO    | VAU_PATH_ABSOLUTE,
    VAU_PATH_LINE_TO_REL    = VAU_PATH_LINE_TO    | VAU_PATH_RELATIVE,
    VAU_PATH_HLINE_TO_ABS   = VAU_PATH_HLINE_TO   | VAU_PATH_ABSOLUTE,
    VAU_PATH_HLINE_TO_REL   = VAU_PATH_HLINE_TO   | VAU_PATH_RELATIVE,
    VAU_PATH_VLINE_TO_ABS   = VAU_PATH_VLINE_TO   | VAU_PATH_ABSOLUTE,
    VAU_PATH_VLINE_TO_REL   = VAU_PATH_VLINE_TO   | VAU_PATH_RELATIVE,
    VAU_PATH_QUAD_TO_ABS    = VAU_PATH_QUAD_TO    | VAU_PATH_ABSOLUTE,
    VAU_PATH_QUAD_TO_REL    = VAU_PATH_QUAD_TO    | VAU_PATH_RELATIVE,
    VAU_PATH_CUBIC_TO_ABS   = VAU_PATH_CUBIC_TO   | VAU_PATH_ABSOLUTE,
    VAU_PATH_CUBIC_TO_REL   = VAU_PATH_CUBIC_TO   | VAU_PATH_RELATIVE,
    VAU_PATH_SQUAD_TO_ABS   = VAU_PATH_SQUAD_TO   | VAU_PATH_ABSOLUTE,
    VAU_PATH_SQUAD_TO_REL   = VAU_PATH_SQUAD_TO   | VAU_PATH_RELATIVE,
    VAU_PATH_SCUBIC_TO_ABS  = VAU_PATH_SCUBIC_TO  | VAU_PATH_ABSOLUTE,
    VAU_PATH_SCUBIC_TO_REL  = VAU_PATH_SCUBIC_TO  | VAU_PATH_RELATIVE,
    VAU_PATH_SCCWARC_TO_ABS = VAU_PATH_SCCWARC_TO | VAU_PATH_ABSOLUTE,
    VAU_PATH_SCCWARC_TO_REL = VAU_PATH_SCCWARC_TO | VAU_PATH_RELATIVE,
    VAU_PATH_SCWARC_TO_ABS  = VAU_PATH_SCWARC_TO  | VAU_PATH_ABSOLUTE,
    VAU_PATH_SCWARC_TO_REL  = VAU_PATH_SCWARC_TO  | VAU_PATH_RELATIVE,
    VAU_PATH_LCCWARC_TO_ABS = VAU_PATH_LCCWARC_TO | VAU_PATH_ABSOLUTE,
    VAU_PATH_LCCWARC_TO_REL = VAU_PATH_LCCWARC_TO | VAU_PATH_RELATIVE,
    VAU_PATH_LCWARC_TO_ABS  = VAU_PATH_LCWARC_TO  | VAU_PATH_ABSOLUTE,
    VAU_PATH_LCWARC_TO_REL  = VAU_PATH_LCWARC_TO  | VAU_PATH_RELATIVE,
    VAU_PATH_MAX            = 0xFF
} vau_path_cmd;

/**
 * @if Eng
 * @brief  Path
 * @else
 * @brief  路径
 * @endif
 */
typedef struct {
    ext_cmd_count cmd_count;    /*!< @if Eng Indicates the number of trails..
                                     @else   路径条数提示值 endif */
    td_uchar *cmds;             /*!< @if Eng Path command word array.
                                     @else   路径命令字数组 endif */
    td_u32 cmd_num;             /*!< @if Eng Number of command words.
                                     @else   命令字的个数 endif */
    td_float *datas;            /*!< @if Eng Coordinate data array.
                                     @else   坐标数据数组 endif */
    td_u32 data_num;            /*!< @if Eng Coordinate array array length.
                                     @else   坐标数组数组长度 endif */
} ext_vau_path;

typedef enum {
    EXT_VAU_REFLECT_MODE_X = 0x1,
    EXT_VAU_REFLECT_MODE_Y = 0x2,
    EXT_VAU_REFLECT_MODE_XY = 0x4,
} ext_vau_reflect_mode;

/**
 * @if Eng
 * @brief  affine mode.
 * @else
 * @brief  仿射变换模式
 * @endif
 */
typedef enum {
    EXT_VAU_AFFINE_MODE_TRANSLATE = 0x1,
    EXT_VAU_AFFINE_MODE_ZOOM      = 0x2,
    EXT_VAU_AFFINE_MODE_ROTATE    = 0x4,
    EXT_VAU_AFFINE_MODE_REFLECT   = 0x8,
    EXT_VAU_AFFINE_MODE_SHEARING  = 0x10,
    EXT_VAU_AFFINE_MODE_USE_COEF = 0x20,
} ext_vau_affine_mode;

/**
 * @if Eng
 * @brief  Crop mode.
 * @else
 * @brief  剪切模式
 * @endif
 */
typedef enum {
    EXT_VAU_CROP_INSIDE,
    EXT_VAU_CROP_OUTSIDE,
    EXT_VAU_CROP_MAX
} ext_vau_crop_mode;

/**
 * @if Eng
 * @brief  Crop Attributes.
 * @else
 * @brief  剪切属性
 * @endif
 */
typedef struct {
    td_bool crop_en;
    ext_vau_crop_mode crop_mode;
    ext_rect crop_rect;
} ext_vau_crop_attr;

/**
 * @if Eng
 * @brief  Transfer Attributes.
 * @else
 * @brief  转移属性
 * @endif
 */
typedef struct {
    td_bool enable;
    td_s32 x;
    td_s32 y;
} ext_vau_trans_attr;

/**
 * @if Eng
 * @brief  Rop mode.
 * @else
 * @brief  rop模式
 * @endif
 */
typedef enum {
    EXT_VAU_ROP_BLACK = 0x0,    /*!< @if Eng Write all 0s.
                                     @else   写为全0 @endif */
    EXT_VAU_ROP_NOTMERGEPEN,    /*!< @if Eng Foreground and Background are first or then reversed.
                                     @else   前景与后景先作或再取反 @endif */
    EXT_VAU_ROP_MASKNOTPEN,     /*!< @if Eng Reversing the foreground and re-anding the background.
                                     @else   前景取反再与后景相与 @endif */
    EXT_VAU_ROP_NOTCOPYPEN,     /*!< @if Eng foreground inversion.
                                     @else   前景取反 @endif */
    EXT_VAU_ROP_MASKPENNOT,     /*!< @if Eng Inversion of the background and re-parametering of the foreground.
                                     @else   后景取反再与前景相与 @endif */
    EXT_VAU_ROP_NOT,            /*!< @if Eng foreground inversion.
                                     @else   前景取反 @endif */
    EXT_VAU_ROP_XORPEN,         /*!< @if Eng XOR between foreground and background.
                                     @else   前景与后景取异或 @endif */
    EXT_VAU_ROP_NOTMASKPEN,     /*!< @if Eng Reversal of the foreground and the background.
                                     @else   前景与后景相与后取反 @endif */
    EXT_VAU_ROP_MASKPEN,        /*!< @if Eng The and operation of foreground and background.
                                     @else   前景与后景相与 @endif */
    EXT_VAU_ROP_NOTXORPEN,      /*!< @if Eng Inversion after XOR of foreground and background.
                                     @else   前景与后景异或后取反 @endif */
    EXT_VAU_ROP_NOP,            /*!< @if Eng Directly output background.
                                     @else   直接输出背景 @endif */
    EXT_VAU_ROP_MERGENOTPEN,    /*!< @if Eng Reversing the foreground and then ORing the background.
                                     @else   前景取反再与后景相或 @endif */
    EXT_VAU_ROP_COPYPEN,        /*!< @if Eng Direct output foreground.
                                     @else   直接输出前景 @endif */
    EXT_VAU_ROP_MERGEPENNOT,    /*!< @if Eng The background scene is reversed and then ORed with the foreground.
                                     @else   背景景取反再与前景相或 @endif */
    EXT_VAU_ROP_MERGEPEN,       /*!< @if Eng The foreground is ordained with the background.
                                     @else   前景与后景相或 @endif */
    EXT_VAU_ROP_WHITE,          /*!< @if Eng Write all 1s.
                                     @else   写为全1 @endif */
    EXT_VAU_ROP_MAX             /*!< @if Eng Invalid value.
                                     @else   无效值 @endif */
} ext_vau_rop_mode;

/**
 * @if Eng
 * @brief  ROP information configuration.
 * @else
 * @brief  rop信息配置
 * @endif
 */
typedef struct {
    td_bool rop_enable;                /*!< @if Eng ROP enable.
                                            @else   rop使能 @endif */
    ext_vau_rop_mode alpha_rop_mode;   /*!< @if Eng Alpha rop mode.
                                            @else   alpha rop模式 @endif */
    ext_vau_rop_mode color_rop_mode;   /*!< @if Eng Color rop mode.
                                            @else   color rop模式 @endif */
} ext_vau_rop_opt;

/**
 * @if Eng
 * @brief  colorkey schematic attribute.
 * @else
 * @brief  colorkey模式属性
 * @endif
 */
typedef enum {
    EXT_VAU_COLORKEY_NONE = 0,    /*!< @if Eng Do not perform the colorkey operation.
                                       @else   不做colorkey操作 @endif */
    EXT_VAU_COLORKEY_FOREGROUND,  /*!< @if Eng Perform the colorkey operation on the foreground bitmap.
                                       @else   对前景位图进行colorkey操作 @endif */
    EXT_VAU_COLORKEY_BACKGROUND,  /*!< @if Eng Perform the colorkey operation on the background bitmap.
                                       @else   对背景位图进行colorkey操作 @endif */
    EXT_VAU_COLORKEY_MAX          /*!< @if Eng Invalid colorkey mode.
                                       @else   无效的colorkey模式 @endif */
} ext_vau_colorkey_mode;

/**
 * @if Eng
 * @brief  Color key attributes for individual color components.
 * @else
 * @brief  单个颜色分量的关键色属性
 * @endif
 */
typedef struct {
    td_u16 component_min;          /*!< @if Eng Minimum value of component color keys .
                                        @else   分量关键色最小值 @endif */
    td_u16 component_max;          /*!< @if Eng Maximum value of component color key .
                                        @else   分量关键色最大值 @endif */
    td_bool is_component_out;      /*!< @if Eng Component key is in or out of range .
                                        @else   分量关键色在范围内或范围外 @endif */
    td_bool is_component_ignore;   /*!< @if Eng Ignore Component .
                                        @else   分量是否忽略 @endif */
    td_u16 component_mask;   /*!< @if Eng Component mask. Currently, the value is meaningless and ranges from 0 to 255.
                                        @else   分量掩码，目前该值无意义，0～255均可 @endif */
} ext_vau_colorkey_component;

/**
 * @if Eng
 * @brief  Colorkey value.
 * @else
 * @brief  Colorkey值
 * @endif
 */
typedef union {
    struct {
        ext_vau_colorkey_component alpha;  /*!< @if Eng Alpha value .
                                                @else   alpha 值 @endif */
        ext_vau_colorkey_component red;    /*!< @if Eng Red component .
                                                @else   红色分量 @endif */
        ext_vau_colorkey_component green;  /*!< @if Eng Green component .
                                                @else   绿色分量 @endif */
        ext_vau_colorkey_component blue;   /*!< @if Eng Blue component .
                                                @else   蓝色分量 @endif */
    } colorkey_argb;

    struct {
        ext_vau_colorkey_component alpha;
        ext_vau_colorkey_component clut;
    } colorkey_clut;                        /*!< @if Eng The ES does not support this function .
                                                 @else   ES暂时不支持 @endif */
} ext_vau_colorkey_value;

/**
 * @if Eng
 * @brief  Configuring Colorkey Information.
 * @else
 * @brief  Colorkey信息配置
 * @endif
 */
typedef struct {
    td_bool colorkey_enable;               /*!< @if Eng Colorkey enable .
                                                @else   Colorkey使能 @endif */
    ext_vau_colorkey_mode colorkey_mode;   /*!< @if Eng Colorkey mode.
                                                @else   Colorkey模式 @endif */
    ext_vau_colorkey_value colorkey_value; /*!< @if Eng Colorkey value.
                                                @else   Colorkey值 @endif */
} ext_vau_colorkey_opt;

/**
 * @if Eng
 * @brief  Blend mode.
 * @else
 * @brief  混合模式
 * @endif
 */
typedef enum {
    EXT_VAU_BLEND_ZERO = 0x0,
    EXT_VAU_BLEND_ONE,
    EXT_VAU_BLEND_SRC2COLOR,
    EXT_VAU_BLEND_INVSRC2COLOR,
    EXT_VAU_BLEND_SRC2ALPHA,
    EXT_VAU_BLEND_INVSRC2ALPHA,
    EXT_VAU_BLEND_SRC1COLOR,
    EXT_VAU_BLEND_INVSRC1COLOR,
    EXT_VAU_BLEND_SRC1ALPHA,
    EXT_VAU_BLEND_INVSRC1ALPHA,
    EXT_VAU_BLEND_SRC2ALPHASAT,
    EXT_VAU_BLEND_MAX
} ext_vau_blend_mode;

/**
 * @if Eng
 * @brief  Blend command.
 * @else
 * @brief  混合命令
 * @endif
 */
typedef enum {
    EXT_VAU_BLENDCMD_NONE = 0x0,  /*!< @if Eng Blend enable.
                                       @else   blend使能 @endif */
    EXT_VAU_BLENDCMD_CLEAR,       /*!< @if Eng The drawing will not be submitted to the canvas.
                                       @else   所绘制不会提交到画布上 @endif */
    EXT_VAU_BLENDCMD_SRC,         /*!< @if Eng Display upper-layer drawing picture.
                                       @else   显示上层绘制图片 @endif */
    EXT_VAU_BLENDCMD_SRCOVER,     /*!< @if Eng The upper and lower layers are overlapped.
                                       @else   正常绘制显示，上下层绘制叠盖 @endif */
    EXT_VAU_BLENDCMD_DSTOVER,     /*!< @if Eng Both the upper and lower levels are displayed. Lower top display.
                                       @else   上下层都显示。下层居上显示 @endif */
    EXT_VAU_BLENDCMD_SRCIN,       /*!< @if Eng Draw the intersection of two layers. Show the upper layer.
                                       @else   取两层绘制交集。显示上层 @endif */
    EXT_VAU_BLENDCMD_DSTIN,       /*!< @if Eng Draw the intersection of two layers. Show the lower layer.
                                       @else   取两层绘制交集。显示下层 @endif */
    EXT_VAU_BLENDCMD_SRCOUT,      /*!< @if Eng Draw the non-intersection part from the upper layer.
                                       @else   取上层绘制非交集部分 @endif */
    EXT_VAU_BLENDCMD_DSTOUT,      /*!< @if Eng Draw the non-intersection part from the lower layer.
                                       @else   取下层绘制非交集部分 @endif */
    EXT_VAU_BLENDCMD_SRCATOP,     /*!< @if Eng The non-intersection part of the lower layer and
                                       the intersection part of the upper layer are selected.
                                       @else   取下层非交集部分与上层交集部分 @endif */
    EXT_VAU_BLENDCMD_DSTATOP,     /*!< @if Eng The non-intersection part of the upper layer and
                                       the intersection part of the lower layer are selected.
                                       @else   取上层非交集部分与下层交集部分 @endif */
    EXT_VAU_BLENDCMD_ADD,         /*!< @if Eng The upper and lower layers are superimposed on CNend.
                                       @else   上下两层叠加 @endif */
    EXT_VAU_BLENDCMD_XOR,         /*!< @if Eng Take two layers to draw non-intersections.
                                       Two-layer drawing non-intersection.
                                       @else   取两层绘制非交集。两层绘制非交集 @endif */
    EXT_VAU_BLENDCMD_DST,         /*!< @if Eng Display lower-layer drawing picture.
                                       @else   显示下层绘制图片 @endif */
    EXT_VAU_BLENDCMD_CONFIG,      /*!< @if Eng configuration.
                                       @else   配置 @endif */
    EXT_VAU_BLENDCMD_MAX          /*!< @if Eng Invalid value.
                                       @else   无效值 @endif */
} ext_vau_blend_cmd;

/**
 * @if Eng
 * @brief  Blend alpha mode.
 * @else
 * @brief  blend alpha模式
 * @endif
 */
typedef enum {
    EXT_VAU_OUTALPHA_FROM_NORM = 0,    /*!< @if Eng alpha from superposition formula.
                                            @else   alpha来自叠加公式 @endif */
    EXT_VAU_OUTALPHA_FROM_BACKGROUND,  /*!< @if Eng alpha from the background.
                                            @else   alpha来自后景 @endif */
    EXT_VAU_OUTALPHA_FROM_FOREGROUND,  /*!< @if Eng alpha from foreground.
                                            @else   alpha来自前景 @endif */
    EXT_VAU_OUTALPHA_FROM_GLOBALALPHA, /*!< @if Eng alpha from global alpha.
                                            @else   alpha来自全局alpha @endif */
    EXT_VAU_OUTALPHA_FROM_MAX          /*!< @if Eng Invalid value.
                                            @else   无效值 @endif */
} ext_vau_out_alpha_mode;

/**
 * @if Eng
 * @brief  Compressed header information.
 * @else
 * @brief  压缩头信息
 * @endif
 */
typedef struct {
    td_u32 version;       /*!< @if Eng 0:ES, 1:CS.
                               @else   0:ES, 1:CS @endif */
    td_u32 alpha_bypass;  /*!< @if Eng Indicates whether to enable compression for the alpha channel.
                               The options are as follows: 0: yes; 1: no.
                               @else   Alpha通道是否压缩的使能配置，配置参数:
                               0：进行压缩  1：不进行压缩 @endif */
    td_u32 width;         /*!< @if Eng Image width configuration; 1-pixel aligned, 4 min., 640 max.
                               @else   图像宽度配置；1pixel对齐，最小4，最大640 @endif */
    td_u32 height;        /*!< @if Eng Image height configuration; 1-pixel aligned, 4 min., 640 max.
                               @else   图像高度配置；1pixel对齐，最小4， 最大640 @endif */
    td_u32 stride;        /*!< @if Eng Size of a tile line after compression, in bytes.
                               @else   TILE行压缩后的大小，以byte为单位 @endif */
    td_u32 pixel_format;  /*!< @if Eng Image format (The ES supports only rgb888 and argb8888.).
                               @else   图片格式(ES只支持rgb888,argb8888) @endif */
    td_u32 tile_size;     /*!< @if Eng tile size, in 64bits.
                               @else   无效值 @endif */
    td_u32 alpha_value;   /*!< @if Eng 0:ALPHA rand value; 1:ALPHA all 0;2:ALPHA all 255; 3:ALPHA 0 and 255.
                               @else   0:ALPHA rand value; 1:ALPHA all 0;2:ALPHA all 255; 3:ALPHA 0 and 255 @endif */
    td_u32 mode;          /*!< @if Eng Compression enable flag.
                               @else   压缩使能标志位：0-cmp 1-bypass @endif */
    td_s32 tile_width;    /*!< @if Eng Compression ratio.
                               @else   压缩倍率 @endif */
} ext_vau_cmp_header;

/**
 * @if Eng
 * @brief  blend Information Configuration.
 * @else
 * @brief  混合信息配置
 * @endif
 */
typedef struct {
    td_bool blend_enable;                      /*!< @if Eng blend enable.
                                                    @else   blend使能 @endif */
    ext_vau_blend_cmd blend_cmd;               /*!< @if Eng blend commend.
                                                    @else   blend命令 @endif */
    ext_vau_blend_mode background_blend_mode;  /*!< @if Eng Customized blend mode.
                                                    @else   自定义blend模式 @endif */
    ext_vau_blend_mode foreground_blend_mode;  /*!< @if Eng Customized blend mode.
                                                    @else   自定义blend模式 @endif */
    ext_vau_out_alpha_mode out_alpha_mode;     /*!< @if Eng Alpha Overlay Mode.
                                                    @else   alpha叠加模式 @endif */
} ext_vau_blend_opt;

/**
 * @if Eng
 * @brief  Gaussian fuzzy parameter.
 * @else
 * @brief  高斯模糊参数
 * @endif
 */
typedef struct {
    td_float x_sigma;
    td_float y_sigma;
    td_bool alpha_gaussian;
} ext_vau_gaussian_para;

/**
 * @if Eng
 * @brief  Gaussian Fuzzy Mode Attributes.
 * @else
 * @brief  高斯模糊模式属性
 * @endif
 */
typedef struct {
    td_bool blur_en;                      /*!< @if Eng enable.
                                               @else   使能 @endif */
    ext_vau_blur_mode blur_mode;          /*!< @if Eng Gaussian fuzzy model.
                                               @else   高斯模糊模式 @endif */
    ext_vau_gaussian_para gaussian_attr;  /*!< @if Eng Gaussian fuzzy parameter.
                                               @else   高斯模糊参数 @endif */
} ext_vau_blur_opt;

/**
 * @if Eng
 * @brief  draw attribution.
 * @else
 * @brief  绘制属性
 * @endif
 */
typedef struct {
    ext_vau_path *path;
    ext_vau_paint *paint;
} ext_vau_draw_attr;

/**
 * @if Eng
 * @brief  Rendering Options.
 * @else
 * @brief  渲染选项
 * @endif
 */
typedef struct {
    ext_vau_rop_opt rop_opt;           /*!< @if Eng rop mode attribute.
                                            @else   rop 模式属性 @endif */
    ext_vau_colorkey_opt colorkey_opt; /*!< @if Eng colorkey mode attribute.
                                            @else   colorkey 模式属性 @endif */
    ext_vau_blend_opt blend_opt;       /*!< @if Eng blen mode attribute.
                                            @else   blend 模式属性 @endif */
    ext_vau_blur_opt blur_opt;         /*!< @if Eng Blur mode attribute (not supported currently).
                                            @else   blur 模式属性(暂不支持) @endif */
} ext_vau_blit_opt;

/**
 * @if Eng
 * @brief  Pattern mode.
 * @else
 * @brief  图案填充模式
 * @endif
 */
typedef enum {
    EXT_VAU_PATTERN_DISABLE = 0x0,    /*!< @if Eng Disable.
                                           @else   不使能 @endif */
    EXT_VAU_PATTERN_FILL    = 0x1,    /*!< @if Eng pattern Fill.
                                           @else   fill填充 @endif */
    EXT_VAU_PATTERN_PAD     = 0x2,    /*!< @if Eng pattern pad.
                                           @else   pad填充 @endif */
} ext_vau_pattern_mode;

/**
 * @if Eng
 * @brief  Pattern attribution.
 * @else
 * @brief  图案填充属性
 * @endif
 */
typedef struct {
    ext_vau_pattern_mode mode;    /*!< @if Eng Pattern attribution.
                                       @else   图案填充属性 @endif */
    td_u32 argb;                  /*!< @if Eng fill color.
                                       @else   填充颜色 @endif */
} ext_vau_pattern_attr;

/**
 * @if Eng
 * @brief  Layer information required for executing the vau task.
 * @else
 * @brief  执行vau任务需要的图层信息
 * @endif
 */
typedef struct {
    td_u32 width;                        /*!< @if Eng surface Width,Unit: number of pixels.
                                              @else   surface宽度, 单位：像素数 @endif */
    td_u32 height;                       /*!< @if Eng surface height,Unit: number of pixels.
                                              @else   surface高度, 单位：像素数 @endif */
    td_u32 stride;                       /*!< @if Eng Buffer line spacing.
                                              @else   buffer行间距 @endif */
    td_u32 phy_addr;                     /*!< @if Eng Physical address of the surface.
                                              @else   surface物理地址 @endif */
    td_u32 clut_addr;                    /*!< @if Eng Color Lookup Table Address.
                                              @else   颜色查找表地址 @endif */
    td_uchar *vir_addr;                  /*!< @if Eng Surface virtual address.
                                              @else   surface虚拟地址 @endif */
    ext_gfx_fmt color_fmt;               /*!< @if Eng Color Formatting.
                                              @else   颜色格式 @endif */
    ext_gfx_argb_order argb_order;       /*!< @if Eng Argb color format component layout.
                                              @else   argb颜色格式分量排布 @endif */
    td_bool is_color_surface;            /*!< @if Eng Solid color filling.
                                              @else   是否纯色填充 @endif */
    td_u32 color;                        /*!< @if Eng Fill color corresponding to solid fill.
                                              @else   纯色填充对应的填充颜色 @endif */
    ext_gfx_compress_mode compress_mode; /*!< @if Eng Data Compression Type.
                                              @else   数据压缩类型 @endif */
    ext_vau_alpha_info alpha_info;       /*!< @if Eng Alpha Attribute.
                                              @else   alpha属性配置 @endif */
    ext_rect rect;                       /*!< @if Eng Overlay area of the source.
                                              @else   源的叠加区域 @endif */
    ext_rect out_rect;                   /*!< @if Eng Target region superimposed with this source.
                                              @else   与该源叠加的目标区域 @endif */
    ext_vau_matrix *matrix;              /*!< @if Eng Affine transformation matrix information.
                                              @else   仿射变换矩阵信息 @endif */
    ext_vau_pattern_attr *pattern;       /*!< @if Eng The pattern function is valid only when the matrix is not empty.
                                             (The ES is not used currently.).
                                              @else   pattern功能只有在matrix不为空的时候才有效(ES暂未使用) @endif */
    ext_vau_draw_attr *draw;             /*!< @if Eng The ES is not used currently.
                                              @else   ES暂未使用 @endif */
} ext_vau_surface;

/**
 * @if Eng
 * @brief  Layer information required for executing the vau task.
 * @else
 * @brief  执行vau任务需要的图层信息
 * @endif
 */
typedef struct {
    td_u32 src_surface_cnt;       /*!< @if Eng Number of source surfaces.
                                       @else   源surface的个数 @endif */
    ext_vau_surface *src_surface; /*!< @if Eng Source surface.
                                       @else   源surface @endif */
    ext_vau_surface *dst_surface; /*!< @if Eng Target surface.
                                       @else   目标surface @endif */
    ext_vau_blit_opt *opt;        /*!< @if Eng Configuration attributes of overlay, rop, and colorkey operations.
                                       @else   叠加、rop、colorkey的操作配置属性 @endif */
} ext_vau_surface_list;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SOC_VAU_TYPE_H */
