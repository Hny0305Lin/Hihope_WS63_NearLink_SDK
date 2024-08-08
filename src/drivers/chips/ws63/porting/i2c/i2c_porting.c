/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides i2c port template \n
 *
 * History: \n
 * 2022-08-15， Create file. \n
 */

#include "i2c_porting.h"
#include "hal_i2c.h"
#include "hal_i2c_v150.h"
#include "hal_i2c_v150_comm.h"
#include "pinctrl.h"
#include "osal_interrupt.h"
#include "chip_core_irq.h"
#include "arch_port.h"

#define BUS_CLOCK_TIME_24M 24000000UL
#define BUS_CLOCK_TIME_40M 40000000UL

#ifdef BOARD_FPGA
#define I2C_CLOCK_DEFAULT BUS_CLOCK_TIME_24M
#else
#define I2C_CLOCK_DEFAULT BUS_CLOCK_TIME_40M
#endif

typedef void (*i2c_porting_irq_handler)(void);

typedef struct i2c_irq_handler {
    uint32_t irq_id;
    i2c_porting_irq_handler irq_handler;
} i2c_irq_handler_t;


uintptr_t g_i2c_base_addrs[I2C_BUS_MAX_NUM] = {
    (uintptr_t)I2C_BUS_0_BASE_ADDR,
    (uintptr_t)I2C_BUS_1_BASE_ADDR,
#if I2C_BUS_MAX_NUM > 2
    (uintptr_t)I2C_BUS_2_BASE_ADDR,
#if I2C_BUS_MAX_NUM > 3
    (uintptr_t)I2C_BUS_3_BASE_ADDR,
#endif
#endif
};

uint32_t g_i2c_clock[I2C_BUS_MAX_NUM] = {
    (uint32_t)I2C_CLOCK_DEFAULT,
    (uint32_t)I2C_CLOCK_DEFAULT,
#if I2C_BUS_MAX_NUM > 2
    (uint32_t)I2C_CLOCK_DEFAULT,
#if I2C_BUS_MAX_NUM > 3
    (uint32_t)I2C_CLOCK_DEFAULT,
#endif
#endif
};

uintptr_t i2c_porting_base_addr_get(i2c_bus_t bus)
{
    return g_i2c_base_addrs[bus];
}

static void irq_i2c0_handler(void)
{
    hal_i2c_v150_irq_handler(I2C_BUS_0);
    osal_irq_clear(I2C_0_IRQN);
}

static void irq_i2c1_handler(void)
{
    hal_i2c_v150_irq_handler(I2C_BUS_1);
    osal_irq_clear(I2C_1_IRQN);
}

#if I2C_BUS_MAX_NUM > 2
static void irq_i2c2_handler(void)
{
    hal_i2c_v150_irq_handler(I2C_BUS_2);
}
#endif

#if I2C_BUS_MAX_NUM > 3
static void irq_i2c3_handler(void)
{
    hal_i2c_v150_irq_handler(I2C_BUS_3);
}
#endif

static i2c_irq_handler_t g_i2c_irq_id[I2C_BUS_MAX_NUM] = {
    {
        I2C_0_IRQN,
        irq_i2c0_handler,
    },
    {
        I2C_1_IRQN,
        irq_i2c1_handler,
    },
#if I2C_BUS_MAX_NUM > 2
    {
        I2C_2_IRQN,
        irq_i2c2_handler,
    },
#if I2C_BUS_MAX_NUM > 3
    {
        I2C_3_IRQN,
        irq_i2c3_handler,
    },
#endif
#endif
};

void i2c_port_set_clock_value(i2c_bus_t bus, uint32_t clock)
{
    if (bus >= I2C_BUS_MAX_NUM) {
        return;
    }

    g_i2c_clock[bus] = clock;
}

uint32_t i2c_port_get_clock_value(i2c_bus_t bus)
{
    if (bus >= I2C_BUS_MAX_NUM) {
        return 0;
    }

    return g_i2c_clock[bus];
}

void i2c_port_register_irq(i2c_bus_t bus)
{
    osal_irq_request(g_i2c_irq_id[bus].irq_id, (osal_irq_handler)g_i2c_irq_id[bus].irq_handler, NULL, NULL, NULL);
    osal_irq_set_priority(g_i2c_irq_id[bus].irq_id, irq_prio(g_i2c_irq_id[bus].irq_id));
    osal_irq_enable(g_i2c_irq_id[bus].irq_id);
    return;
}

void i2c_port_unregister_irq(i2c_bus_t bus)
{
    osal_irq_disable(g_i2c_irq_id[bus].irq_id);
    return;
}

uint32_t i2c_porting_lock(i2c_bus_t bus)
{
    unused(bus);
    return osal_irq_lock();
}

void i2c_porting_unlock(i2c_bus_t bus, uint32_t irq_sts)
{
    unused(bus);
    osal_irq_restore(irq_sts);
}

#ifdef TEST_SUITE
void i2c_port_test_i2c_init_pin(void)
{
    uapi_reg_write32(I2C_1_SCL_PIN_CTRL_REG, PIN_CTRL_MODE_2);
    uapi_reg_write32(I2C_1_SDA_PIN_CTRL_REG, PIN_CTRL_MODE_2);
}
#endif
