#include "./io_expander.h"
#include "./ssd1306.h"
#include "i2c.h"
#include "pinctrl.h"
#include <app_init.h>
#include <chip_core_irq.h>
#include <common_def.h>
#include <gpio.h>
#include <gpio_porting.h>
#include <osal_addr.h>
#include <osal_debug.h>
#include <osal_task.h>
#include <pwm.h>
#include <stdint.h>
#include <systick.h>
#include <timer.h>
#include <watchdog.h>
#define EXP3_TASK_STACK_SIZE 1000
#define EXP3_TASK_PRIO 10

#define CONFIG_I2C_SCL_MASTER_PIN 15
#define CONFIG_I2C_SDA_MASTER_PIN 16
#define CONFIG_I2C_MASTER_PIN_MODE 2
#define I2C_MASTER_ADDR 0x1

#define I2C_SET_BANDRATE 115200
static int exp3_task(const char *arg) {

  unused(arg);

  uapi_pin_set_mode(CONFIG_I2C_SCL_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
  uapi_pin_set_mode(CONFIG_I2C_SDA_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
  uint32_t baudrate = I2C_SET_BANDRATE;
  uint32_t master_addr = I2C_MASTER_ADDR;
  errcode_t ecode = uapi_i2c_master_init(1, baudrate, master_addr);

  io_expander_init();
  if (ecode != 0) {
    osal_printk("i2c init failed code is %0x \r\n", ecode);
  }

  ssd1306_Init();
  while (1) {
    // ssd1306_DrawRectangle(0, 0, 30, 30, 0x01);
    // ssd1306_Fill(0x00);
    // ssd1306_SetCursor(0, 0);
    // ssd1306_ClearOLED();
    // ssd1306_printf("wwww\n");
    // ssd1306_UpdateScreen();
    // osal_msleep(3000);
    // ssd1306_Fill(0x00);
    // ssd1306_SetCursor(10, 10);
    // ssd1306_printf("position at 10,10");
    osal_msleep(1000);
  }

  return 0;
}

static void exp3_entry(void) {
  osal_task *task_handle = NULL;
  osal_kthread_lock();
  osal_printk("Timer task starting");
  task_handle = osal_kthread_create((osal_kthread_handler)exp3_task, 0,
                                    "exp3", EXP3_TASK_STACK_SIZE);
  if (task_handle != NULL) {
    osal_kthread_set_priority(task_handle, EXP3_TASK_PRIO);
    osal_kfree(task_handle);
  }
  osal_kthread_unlock();
}

/* Run the timer_entry. */
app_run(exp3_entry);
