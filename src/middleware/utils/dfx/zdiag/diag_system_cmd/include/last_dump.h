/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: last dump
 * This file should be changed only infrequently and with great care.
 */

#ifndef LAST_DUMP_H
#define LAST_DUMP_H
#include <stdint.h>

void dfx_last_dump_start(uint32_t file_num);
void dfx_last_dump_data(const char *name, uintptr_t addr, uint32_t size);
void dfx_last_word_send(uint8_t *buf, uint32_t size);
void dfx_last_dump_end(uint32_t file_num);

#endif // !LAST_DUMP_H