/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: mocked shell
 */

#ifndef DIAG_MOCKED_SHELL_H
#define DIAG_MOCKED_SHELL_H

#include "errcode.h"
#include "stdarg.h"
#include "diag.h"

typedef void (*diag_cmd_shell_data_proc)(uint8_t *data, uint32_t data_len);
typedef void (*zdiag_data_put_uart)(uint8_t *data, uint16_t data_len);

int zdiag_mocked_shell_print(const char *fmt, va_list ap);
int zdiag_mocked_shell_uart_puts(uint8_t *data, uint32_t data_len);
void zdiag_mocked_shell_register_directly_uartput(zdiag_data_put_uart uart_hook);
void zdiag_mocked_shell_init(void);
void zdiag_mocked_shell_register_cmd_data_proc(diag_cmd_shell_data_proc data_proc_hook);

void zdiag_mocked_shell_set_enable(bool enable);
bool zdiag_mocked_shell_is_enabled(void);

errcode_t zdiag_cmd_mocked_shell(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t zdiag_cmd_mocked_shell_enable(uint16_t cmd_id, void *cmd_param,
                                        uint16_t cmd_param_size, diag_option_t *option);
#endif /* ZDIAG_MOCKED_SHELL_H */

