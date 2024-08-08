/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2022. All rights reserved.
 * Description: osal adapt atomic source file.
 */
#ifdef OSAL_IRQ_RECORD_DEBUG
#include "securec.h"
#include "osal_inner.h"
#include "osal_adapt_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

debug_irq_record g_irq_record = {0};
void osal_irq_record(irq_type_enum type, td_u32 caller, td_u32 stauts)
{
    if (type >= IRQ_TYPE_BUFF) {
        return;
    }

    if (g_irq_record.idx[type] >= RECORD_CNT_MAX) {
        g_irq_record.idx[type] = 0;
    }

    g_irq_record.caller[type][g_irq_record.idx[type]] = caller;
    g_irq_record.stauts[type][g_irq_record.idx[type]++] = stauts;
}

debug_irq_record *osal_get_irq_record(void)
{
    return &g_irq_record;
}

void osal_clear_irq_record(void)
{
    memset_s(&g_irq_record, sizeof(g_irq_record), 0, sizeof(g_irq_record));
}

void osal_print_irq_record(void)
{
    osal_printk("=======> osal_print_irq_record: \r\nirq_type[0:irq_lock, 1:irq_unlock, 2:irq_restore]\r\n");
    for (td_u32 type = 0; type < IRQ_TYPE_BUFF; type++) {
        for (td_u32 cnt = 0; cnt < RECORD_CNT_MAX; cnt++) {
            osal_printk("%d irq_type[%d], caller: 0x%x, stauts: 0x%x", cnt, type,
                        g_irq_record.caller[type][cnt], g_irq_record.stauts[type][cnt]);
            if (g_irq_record.idx[type] == (cnt + 1)) {
                osal_printk("   <-- pointer: %d.\r\n", cnt);
            } else {
                osal_printk("\r\n");
            }
        }
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // #ifdef OSAL_IRQ_RECORD_DEBUG