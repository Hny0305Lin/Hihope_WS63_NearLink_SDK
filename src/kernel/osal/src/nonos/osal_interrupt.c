/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: OS Abstract Layer.
 */


unsigned int osal_irq_lock(void)
{
    non_os_enter_critical();
    return 0;
}


void osal_irq_restore(unsigned int irq)
{
    non_os_exit_critical();
    return;
}

int osal_in_interrupt(void)
{
    return 0;
}
