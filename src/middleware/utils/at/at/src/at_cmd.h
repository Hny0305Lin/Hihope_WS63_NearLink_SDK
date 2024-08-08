/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides at cmd service header. Only for AT module. \n
 */

#ifndef    AT_CMD_H
#define    AT_CMD_H

#include "at_product.h"

uint32_t at_cmd_get_entry_total(void);

uint32_t at_cmd_get_all_entrys(const at_cmd_entry_t **entrys, uint32_t cnt);

const at_cmd_entry_t* at_cmd_find_entry(char *str, uint16_t *offset);

const at_cmd_entry_t* at_cmd_find_entry_by_name(const char *name);

uint32_t at_cmd_get_max_struct_size(void);
#endif
