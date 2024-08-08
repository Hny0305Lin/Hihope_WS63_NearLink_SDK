/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Low-power management operations for peripheral devices.
 *
 * Create:  2021-4-28
 */

#include "lpm_dev_ops.h"
#include "securec.h"
#include "irmalloc.h"
#include "soc_osal.h"

#define ops_(a, b) ((a) ? (a) : (b))

static lpm_dev_ops_t *g_dev_ops = NULL;
static lpm_clock_init_callback g_display_clocks_init_handler = NULL;

bool lpm_dev_ops_init(void)
{
    if (g_dev_ops != NULL) {
        return false;
    }
    uint32_t irq_sts = osal_irq_lock();
    g_dev_ops = irmalloc(sizeof(lpm_dev_ops_t) * DEV_MAX);
    if (g_dev_ops == NULL) {
        osal_irq_restore(irq_sts);
        return false;
    }
    if (memset_s(g_dev_ops, sizeof(lpm_dev_ops_t) * DEV_MAX, 0, sizeof(lpm_dev_ops_t) * DEV_MAX) != EOK) {
        osal_irq_restore(irq_sts);
        return false;
    }
    osal_irq_restore(irq_sts);

    return true;
}

bool lpm_dev_ops_deinit(void)
{
    if (g_dev_ops == NULL) {
        return false;
    }
    uint32_t irq_sts = osal_irq_lock();
    irfree(g_dev_ops);
    g_dev_ops = NULL;
    osal_irq_restore(irq_sts);

    return true;
}

bool lpm_dev_ops_register(lpm_dev_id_t id, lpm_dev_ops_t *ops)
{
    if (id >= DEV_MAX || (g_dev_ops == NULL) || (ops == NULL)) {
        return false;
    }
    uint32_t irq_sts = osal_irq_lock();
    if (memcpy_s(&g_dev_ops[id], sizeof(lpm_dev_ops_t), ops, sizeof(lpm_dev_ops_t)) != EOK) {
        osal_irq_restore(irq_sts);
        return false;
    }
    osal_irq_restore(irq_sts);

    return true;
}

bool lpm_dev_ops_unregister(lpm_dev_id_t id)
{
    if (id >= DEV_MAX || (g_dev_ops == NULL)) {
        return false;
    }
    uint32_t irq_sts = osal_irq_lock();
    if (memset_s(&g_dev_ops[id], sizeof(lpm_dev_ops_t), 0, sizeof(lpm_dev_ops_t)) != EOK) {
        osal_irq_restore(irq_sts);
        return false;
    }
    osal_irq_restore(irq_sts);

    return true;
}

int lpm_dev_ops_update(lpm_dev_id_t id, lpm_dev_ops_t *ops)
{
    if (id >= DEV_MAX || ops == NULL) {
        return LPM_RET_ERR;
    }

    if (g_dev_ops == NULL) {
        return LPM_RET_UNINIT;
    }
    uint32_t irq_sts = osal_irq_lock();
    g_dev_ops[id].power_on    = ops_(ops->power_on, g_dev_ops[id].power_on);
    g_dev_ops[id].power_sts   = ops_(ops->power_sts, g_dev_ops[id].power_sts);
    g_dev_ops[id].set_voltage = ops_(ops->set_voltage, g_dev_ops[id].set_voltage);
    g_dev_ops[id].get_voltage = ops_(ops->get_voltage, g_dev_ops[id].get_voltage);
    g_dev_ops[id].clock_en    = ops_(ops->clock_en, g_dev_ops[id].clock_en);
    g_dev_ops[id].clock_sts   = ops_(ops->clock_sts, g_dev_ops[id].clock_sts);
    g_dev_ops[id].set_freq    = ops_(ops->set_freq, g_dev_ops[id].set_freq);
    g_dev_ops[id].get_freq    = ops_(ops->get_freq, g_dev_ops[id].get_freq);
    g_dev_ops[id].set_div_num = ops_(ops->set_div_num, g_dev_ops[id].set_div_num);
    g_dev_ops[id].get_div_num = ops_(ops->get_div_num, g_dev_ops[id].get_div_num);
    g_dev_ops[id].sub_clken   = ops_(ops->sub_clken, g_dev_ops[id].sub_clken);
    g_dev_ops[id].resume      = ops_(ops->resume, g_dev_ops[id].resume);
    g_dev_ops[id].suspend     = ops_(ops->suspend, g_dev_ops[id].suspend);
    osal_irq_restore(irq_sts);
    return LPM_RET_OK;
}

int lpm_dev_power_on(lpm_dev_id_t id, bool on)
{
    if (g_dev_ops == NULL) {
        return LPM_RET_ERR;
    }
    if (g_dev_ops[id].power_on) {
        return g_dev_ops[id].power_on(on);
    } else {
        return LPM_RET_NOREG;
    }
}

int lpm_dev_get_power_sts(lpm_dev_id_t id)
{
    if (g_dev_ops == NULL) {
        return LPM_RET_ERR;
    }
    if (g_dev_ops[id].power_sts) {
        return g_dev_ops[id].power_sts();
    } else {
        return LPM_RET_NOREG;
    }
}

int lpm_dev_set_voltage(lpm_dev_id_t id, int vset)
{
    if (g_dev_ops == NULL) {
        return LPM_RET_ERR;
    }
    if (g_dev_ops[id].set_voltage) {
        return g_dev_ops[id].set_voltage(vset);
    } else {
        return LPM_RET_NOREG;
    }
}

int lpm_dev_get_voltage(lpm_dev_id_t id)
{
    if (g_dev_ops == NULL) {
        return LPM_RET_ERR;
    }
    if (g_dev_ops[id].get_voltage) {
        return g_dev_ops[id].get_voltage();
    } else {
        return LPM_RET_NOREG;
    }
}

int lpm_dev_clock_en(lpm_dev_id_t id, bool on)
{
    if (g_dev_ops == NULL) {
        return LPM_RET_ERR;
    }
    if (g_dev_ops[id].clock_en) {
        return g_dev_ops[id].clock_en(on);
    } else {
        return LPM_RET_NOREG;
    }
}

int lpm_dev_get_clock_sts(lpm_dev_id_t id)
{
    if (g_dev_ops == NULL) {
        return LPM_RET_ERR;
    }
    if (g_dev_ops[id].clock_sts) {
        return g_dev_ops[id].clock_sts();
    } else {
        return LPM_RET_NOREG;
    }
}

int lpm_dev_set_freq(lpm_dev_id_t id, int freq)
{
    if (g_dev_ops == NULL) {
        return LPM_RET_ERR;
    }
    if (g_dev_ops[id].set_freq) {
        return g_dev_ops[id].set_freq(freq);
    } else {
        return LPM_RET_NOREG;
    }
}

int lpm_dev_get_freq(lpm_dev_id_t id)
{
    if (g_dev_ops == NULL) {
        return LPM_RET_ERR;
    }
    if (g_dev_ops[id].get_freq) {
        return g_dev_ops[id].get_freq();
    } else {
        return LPM_RET_NOREG;
    }
}

int lpm_dev_set_div_num(lpm_dev_id_t id, int clk_div)
{
    if (g_dev_ops == NULL) {
        return LPM_RET_ERR;
    }
    if (g_dev_ops[id].set_div_num) {
        return g_dev_ops[id].set_div_num(clk_div);
    } else {
        return LPM_RET_NOREG;
    }
}

int lpm_dev_get_div_num(lpm_dev_id_t id)
{
    if (g_dev_ops == NULL) {
        return LPM_RET_ERR;
    }
    if (g_dev_ops[id].get_div_num) {
        return g_dev_ops[id].get_div_num();
    } else {
        return LPM_RET_NOREG;
    }
}

int lpm_dev_sub_bus_clken(lpm_dev_id_t id, int bus, bool on)
{
    if (g_dev_ops == NULL) {
        return LPM_RET_ERR;
    }
    if (g_dev_ops[id].sub_clken) {
        return g_dev_ops[id].sub_clken(bus, on);
    } else {
        return LPM_RET_NOREG;
    }
}

void lpm_display_clocks_init_register_callback(lpm_clock_init_callback callback)
{
    if (callback != NULL) {
        g_display_clocks_init_handler = callback;
    }
}

void lpm_display_clocks_init(void)
{
    if (g_display_clocks_init_handler != NULL) {
        g_display_clocks_init_handler();
    }
}