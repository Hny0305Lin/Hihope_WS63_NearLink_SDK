
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
#define EXP1_TASK_STACK_SIZE 1000
#define EXP1_TASK_PRIO 10
static pin_t leds[] = {GPIO_09, GPIO_07, GPIO_11};
static timer_handle_t my_handles[3];
static int my_delays[] = {1111100, 4313110, 2110000};
void mytmcallback(uintptr_t data)
{
  int led_position = (int )data;
  uapi_gpio_toggle(leds[led_position]);
  uapi_timer_start(my_handles[led_position], my_delays[led_position], mytmcallback, data);
}

static int exp1_task(int *arg)
{
  osal_printk("arg is %d \n", *arg);
  free(arg);
  uapi_gpio_init();
  for (int i = 0; i < 3; ++i)
  {
    uapi_gpio_set_dir(leds[i], GPIO_DIRECTION_OUTPUT);
  }

  uapi_timer_init();
  uapi_timer_adapter(TIMER_INDEX_1, 27, 1);
  for (int i = 0; i < 3; ++i)
  {
    uapi_timer_create(TIMER_INDEX_1, &my_handles[i]);
    uapi_timer_start(my_handles[i], my_delays[i], mytmcallback, i);
  }

  while (1)
  {
    osal_msleep(100);
  }
  return 0;
}

static void exp1_entry(void)
{
  osal_task *task_handle = NULL;
  osal_kthread_lock();
  osal_printk("exp1 task starting\n");
  int *a = (int *)malloc(sizeof(int));
  *a = 2;
  task_handle = osal_kthread_create((osal_kthread_handler)exp1_task, a,
                                    "exp1", EXP1_TASK_STACK_SIZE);
  if (task_handle != NULL)
  {
    osal_kthread_set_priority(task_handle, EXP1_TASK_PRIO);
    osal_kfree(task_handle);
  }
  osal_kthread_unlock();
}

/* Run the timer_entry. */
app_run(exp1_entry);
