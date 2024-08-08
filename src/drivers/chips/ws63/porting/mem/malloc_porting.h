/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: malloc header.
 */

#ifndef MALLOC_PORTING_H
#define MALLOC_PORTING_H

extern unsigned int g_intheap_begin;
extern unsigned int g_intheap_size;
void malloc_port_init(void);

#endif
