/*
 * 32-bit CRC for FCS calculation
 * Copyright (c) 2010, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef CRC32_H
#define CRC32_H
#ifndef EXT_CODE_CROP
u32 crc32(const u8 *frame, size_t frame_len);
#else
u32 crc32_wpa(const u8 *frame, size_t frame_len);
#endif
#endif /* CRC32_H */
