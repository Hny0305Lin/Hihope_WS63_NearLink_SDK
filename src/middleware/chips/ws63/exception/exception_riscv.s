/******************************************************************************
 * @brief    exception asm module implementation
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018. All rights reserved.
******************************************************************************/
    .align 2
    .type   HardFault_Handler, %function
    .type   NMI_Handler, %function
    .extern do_hard_fault_handler
    .extern do_NMI_Handler

HardFault_Handler:
    .global HardFault_Handler

NMI_Handler:
    .global NMI_Handler

