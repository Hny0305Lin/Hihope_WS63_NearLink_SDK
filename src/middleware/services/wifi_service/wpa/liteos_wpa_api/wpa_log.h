/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: soc mesh function
 */

#ifndef WPA_LOG_H
#define WPA_LOG_H

#define MSG_EXCESSIVE_LEVEL 0
#define MSG_MSGDUMP_LEVEL 1
#define MSG_DEBUG_LEVEL 2
#define MSG_INFO_LEVEL 3
#define MSG_WARNING_LEVEL 4
#define MSG_ERROR_LEVEL 5
#define WPA_PRINT_LEVEL MSG_DEBUG_LEVEL

#undef CONFIG_DIAG_SUPPORT
#ifndef CONFIG_NO_WPA_MSG
#define wpa_error_log0 wpa_printf
#define wpa_error_log1 wpa_printf
#define wpa_error_log2 wpa_printf
#define wpa_error_log3 wpa_printf
#define wpa_error_log4 wpa_printf

#define wpa_warning_log0 wpa_printf
#define wpa_warning_log1 wpa_printf
#define wpa_warning_log2 wpa_printf
#define wpa_warning_log3 wpa_printf
#define wpa_warning_log4 wpa_printf

#define wpa_msgdump_log0 wpa_printf
#define wpa_msgdump_log1 wpa_printf
#define wpa_msgdump_log2 wpa_printf
#define wpa_msgdump_log3 wpa_printf
#define wpa_msgdump_log4 wpa_printf

#ifdef CONFIG_DIAG_SUPPORT
#define wpa_error_buf(msg_level, fmt, buffer, size) \
			diag_layer_buf_e(0, fmt, buffer, (td_u16)(size))
#define wpa_error_two_buf(msg_level, fmt, buffer1, size1, buffer2, size2) \
						diag_layer_two_buf_e(0, fmt, buffer1, (td_u16)(size1), buffer2, (td_u16)(size2))

#define wpa_warning_buf(msg_level, fmt, buffer, size) \
			diag_layer_buf_w(0, fmt, buffer, (td_u16)(size))
#define wpa_warning_two_buf(msg_level, fmt, buffer1, size1, buffer2, size2) \
									diag_layer_two_buf_w(0, fmt, buffer1, (td_u16)(size1), buffer2, (td_u16)(size2))
#define wpa_msgdump_buf(msg_level, fmt, buffer, size) \
			diag_layer_buf(0, fmt, buffer, (td_u16)(size))
#define wpa_msgdump_two_buf(msg_level, fmt, buffer1, size1, buffer2, size2) \
									diag_layer_two_buf(0, fmt, buffer1, (td_u16)(size1), buffer2, (td_u16)(size2))
#else
#define wpa_error_buf(msg_level, fmt, buffer, size)
#define wpa_error_two_buf(msg_level, fmt, buffer1, size1, buffer2, size2)
#define wpa_warning_buf(msg_level, fmt, buffer, size)
#define wpa_warning_two_buf(msg_level, fmt, buffer1, size1, buffer2, size2)
#define wpa_msgdump_buf(msg_level, fmt, buffer, size)
#define wpa_msgdump_two_buf(msg_level, fmt, buffer1, size1, buffer2, size2)
#endif

#else

#if WPA_PRINT_LEVEL > MSG_ERROR_LEVEL
#define wpa_error_log0(msg_level, fmt)
#define wpa_error_log1(msg_level, fmt, p1)
#define wpa_error_log2(msg_level, fmt, p1, p2)
#define wpa_error_log3(msg_level, fmt, p1, p2, p3)
#define wpa_error_log4(msg_level, fmt, p1, p2, p3, p4)
#define wpa_error_buf(msg_level, fmt, buffer, size)
#define wpa_error_two_buf(msg_level, fmt, buffer1, size1, buffer2, size2)
#else

#ifdef CONFIG_DIAG_SUPPORT
#define wpa_error_log0(msg_level, fmt) \
			diag_layer_msg_e0(0, fmt)
#define wpa_error_log1(msg_level, fmt, p1) \
			diag_layer_msg_e1(0, fmt, (td_u32)(p1))
#define wpa_error_log2(msg_level, fmt, p1, p2) \
			diag_layer_msg_e2(0, fmt, (td_u32)(p1), (td_u32)(p2))
#define wpa_error_log3(msg_level, fmt, p1, p2, p3) \
			diag_layer_msg_e3(0, fmt, (td_u32)(p1), (td_u32)(p2), (td_u32)(p3))
#define wpa_error_log4(msg_level, fmt, p1, p2, p3, p4) \
			diag_layer_msg_e4(0, fmt, (td_u32)(p1), (td_u32)(p2), (td_u32)(p3), (td_u32)(p4))
#define wpa_error_buf(msg_level, fmt, buffer, size) \
			diag_layer_buf_e(0, fmt, buffer, (td_u16)(size))
#define wpa_error_two_buf(msg_level, fmt, buffer1, size1, buffer2, size2) \
						diag_layer_two_buf_e(0, fmt, buffer1, (td_u16)(size1), buffer2, (td_u16)(size2))
#else

#define wpa_error_log0(msg_level, fmt)
#define wpa_error_log1(msg_level, fmt, p1)
#define wpa_error_log2(msg_level, fmt, p1, p2)
#define wpa_error_log3(msg_level, fmt, p1, p2, p3)
#define wpa_error_log4(msg_level, fmt, p1, p2, p3, p4)
#define wpa_error_buf(msg_level, fmt, buffer, size)
#define wpa_error_two_buf(msg_level, fmt, buffer1, size1, buffer2, size2)
#endif

#endif

#if WPA_PRINT_LEVEL > MSG_DEBUG_LEVEL
#define wpa_warning_log0(msg_level, fmt)
#define wpa_warning_log1(msg_level, fmt, p1)
#define wpa_warning_log2(msg_level, fmt, p1, p2)
#define wpa_warning_log3(msg_level, fmt, p1, p2, p3)
#define wpa_warning_log4(msg_level, fmt, p1, p2, p3, p4)
#define wpa_warning_buf(msg_level, fmt, buffer, size)
#define wpa_warning_two_buf(msg_level, fmt, buffer1, size1, buffer2, size2)
#else

#ifdef CONFIG_DIAG_SUPPORT
#define wpa_warning_log0(msg_level, fmt) \
			diag_layer_msg_w0(0, fmt)
#define wpa_warning_log1(msg_level, fmt, p1) \
			diag_layer_msg_w1(0, fmt, (td_u32)(p1))
#define wpa_warning_log2(msg_level, fmt, p1, p2) \
			diag_layer_msg_w2(0, fmt, (td_u32)(p1), (td_u32)(p2))
#define wpa_warning_log3(msg_level, fmt, p1, p2, p3) \
			diag_layer_msg_w3(0, fmt, (td_u32)(p1), (td_u32)(p2), (td_u32)(p3))
#define wpa_warning_log4(msg_level, fmt, p1, p2, p3, p4) \
			diag_layer_msg_w4(0, fmt, (td_u32)(p1), (td_u32)(p2), (td_u32)(p3), (td_u32)(p4))
#define wpa_warning_buf(msg_level, fmt, buffer, size) \
			diag_layer_buf_w(0, fmt, buffer, (td_u16)(size))
#define wpa_warning_two_buf(msg_level, fmt, buffer1, size1, buffer2, size2) \
									diag_layer_two_buf_w(0, fmt, buffer1, (td_u16)(size1), buffer2, (td_u16)(size2))
#else
#define wpa_warning_log0(msg_level, fmt)
#define wpa_warning_log1(msg_level, fmt, p1)
#define wpa_warning_log2(msg_level, fmt, p1, p2)
#define wpa_warning_log3(msg_level, fmt, p1, p2, p3)
#define wpa_warning_log4(msg_level, fmt, p1, p2, p3, p4)
#define wpa_warning_buf(msg_level, fmt, buffer, size)
#define wpa_warning_two_buf(msg_level, fmt, buffer1, size1, buffer2, size2)
#endif

#endif

#if WPA_PRINT_LEVEL > MSG_EXCESSIVE_LEVEL
#define wpa_msgdump_log0(msg_level, fmt)
#define wpa_msgdump_log1(msg_level, fmt, p1)
#define wpa_msgdump_log2(msg_level, fmt, p1, p2)
#define wpa_msgdump_log3(msg_level, fmt, p1, p2, p3)
#define wpa_msgdump_log4(msg_level, fmt, p1, p2, p3, p4)
#define wpa_msgdump_buf(msg_level, fmt, buffer, size)
#define wpa_msgdump_two_buf(msg_level, fmt, buffer1, size1, buffer2, size2)
#else

#ifdef CONFIG_DIAG_SUPPORT
#define wpa_msgdump_log0(msg_level, fmt) \
			diag_layer_msg_i0(0, fmt)
#define wpa_msgdump_log1(msg_level, fmt, p1) \
			diag_layer_msg_i1(0, fmt, (td_u32)(p1))
#define wpa_msgdump_log2(msg_level, fmt, p1, p2) \
			diag_layer_msg_i2(0, fmt, (td_u32)(p1), (td_u32)(p2))
#define wpa_msgdump_log3(msg_level, fmt, p1, p2, p3) \
			diag_layer_msg_i3(0, fmt, (td_u32)(p1), (td_u32)(p2), (td_u32)(p3))
#define wpa_msgdump_log4(msg_level, fmt, p1, p2, p3, p4) \
			diag_layer_msg_i4(0, fmt, (td_u32)(p1), (td_u32)(p2), (td_u32)(p3), (td_u32)(p4))
#define wpa_msgdump_buf(msg_level, fmt, buffer, size) \
			diag_layer_buf(0, fmt, buffer, (td_u16)(size))
#define wpa_msgdump_two_buf(msg_level, fmt, buffer1, size1, buffer2, size2) \
									diag_layer_two_buf(0, fmt, buffer1, (td_u16)(size1), buffer2, (td_u16)(size2))
#else
#define wpa_msgdump_log0(msg_level, fmt)
#define wpa_msgdump_log1(msg_level, fmt, p1)
#define wpa_msgdump_log2(msg_level, fmt, p1, p2)
#define wpa_msgdump_log3(msg_level, fmt, p1, p2, p3)
#define wpa_msgdump_log4(msg_level, fmt, p1, p2, p3, p4)
#define wpa_msgdump_buf(msg_level, fmt, buffer, size)
#define wpa_msgdump_two_buf(msg_level, fmt, buffer1, size1, buffer2, size2)
#endif

#endif
#endif

#endif
