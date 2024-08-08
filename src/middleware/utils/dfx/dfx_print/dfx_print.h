/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: dfx print header file
 */

#ifndef DFX_PRINT_H
#define DFX_PRINT_H

#ifdef CONFIG_DFX_SUPPORT_PRINT
#include <stdint.h>
#include "debug_print.h"
#endif

typedef enum {
    DFX_PRINT_LEVEL_INFO = 0,
    DFX_PRINT_LEVEL_WARN = 1,
    DFX_PRINT_LEVEL_ERROR = 2,
    DFX_PRINT_LEVEL_MAX
} dfx_print_level;

#ifdef CONFIG_DFX_SUPPORT_PRINT

uint8_t dfx_print_get_level(void);
void dfx_print_set_level(uint8_t level);

#define dfx_print_info(fmt, args...) do { \
        if (dfx_print_get_level() <= DFX_PRINT_LEVEL_INFO) { \
            PRINT(fmt, ##args); \
        } \
    } while (0)

#define dfx_print_warn(fmt, args...) do { \
        if (dfx_print_get_level() <= DFX_PRINT_LEVEL_WARN) { \
            PRINT(fmt, ##args); \
        } \
    } while (0)

#define dfx_print_error(fmt, args...) do { \
        if (dfx_print_get_level() <= DFX_PRINT_LEVEL_ERROR) { \
            PRINT(fmt, ##args); \
        } \
    } while (0)

#define dfx_print(level, fmt, args...) do { \
        if (dfx_print_get_level() <= level) { \
            PRINT(fmt, ##args); \
        } \
    } while (0)
#else

#define dfx_print_info(fmt, args...) PRINT(fmt, ##args)
#define dfx_print_warn(fmt, args...) PRINT(fmt, ##args)
#define dfx_print_error(fmt, args...) PRINT(fmt, ##args)
#define dfx_print(level, fmt, args...) PRINT(fmt, ##args)

#endif
#endif /* DFX_PRINT_H */