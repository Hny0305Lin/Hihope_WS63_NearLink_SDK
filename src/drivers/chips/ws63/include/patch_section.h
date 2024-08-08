/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: patch section header.
 *
 * Create: 2023-04-13
 */
#ifndef __PATCH_SECTION_H__
#define __PATCH_SECTION_H__

#define PATCH_T_I __attribute__((section(".patch.text.on.itcm")))
#define PATCH_R_I __attribute__((section(".patch.rodata.on.itcm")))
#define PATCH_D_I __attribute__((section(".patch.data.on.itcm")))
#define PATCH_B_I __attribute__((section(".patch.bss.on.itcm")))

#define PATCH_T_S __attribute__((section(".patch.text.on.sram")))
#define PATCH_R_S __attribute__((section(".patch.rodata.on.sram")))
#define PATCH_D_S __attribute__((section(".patch.data.on.sram")))
#define PATCH_B_S __attribute__((section(".patch.bss.on.sram")))

#define PATCH_T_F __attribute__((section(".patch.text.on.flash")))
#define PATCH_R_F __attribute__((section(".patch.rodata.on.flash")))
#define PATCH_D_F __attribute__((section(".patch.data.on.flash")))
#define PATCH_B_F __attribute__((section(".patch.bss.on.flash")))

#endif
