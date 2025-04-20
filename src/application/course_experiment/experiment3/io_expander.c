#include "io_expander.h"
#include "./ssd1306.h"
#include "app_init.h"
#include "common_def.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "i2c.h"
#include "pinctrl.h"
#include "soc_osal.h"
#include "watchdog.h"
int LED = 0;
int BUT = 0;
int time = 0;

uint8_t io_expander_i2c_lock = 1;

void gpio_callback_func(pin_t pin, uintptr_t param) {

  UNUSED(pin);
  UNUSED(param);
  uint8_t tx3_buff[2] = {0x1C, 0x08}; // 读取P0的IO状态
  uint8_t rx3_buff[2] = {0};
  i2c_data_t pca5 = {0};
  pca5.send_buf = tx3_buff;
  pca5.send_len = 2;
  pca5.receive_buf = rx3_buff;
  pca5.receive_len = 2;

  // if(io_expander_i2c_lock){
  //     return ;
  // }

  uapi_i2c_master_read(1, I2C_PCA_ADDR, &pca5);
  time = osal_msleep(2);
  osal_printk("sadsd %X %X \n", *rx3_buff, *(rx3_buff + 1));
  osal_printk("LED=%d\n", LED);
  osal_printk("BUT=%d\n", BUT);
  osal_printk("time=%d\n", time);
  if ((*rx3_buff & 0x80) && (*rx3_buff & 0x40)) {
    LED++;
    ssd1306_Fill(0x00);
    ssd1306_ClearOLED();
    ssd1306_SetCursor(0, 0);
    ssd1306_printf("left turn\n");
    ssd1306_UpdateScreen();

    if (LED >= 3) {
      LED = 0;
    }
  }
  if ((*rx3_buff & 0x80) && (!(*rx3_buff & 0x40))) {
    ssd1306_Fill(0x00);
    ssd1306_ClearOLED();
    ssd1306_SetCursor(0, 0);
    ssd1306_printf("right turn\n");
    ssd1306_UpdateScreen();
    LED--;
    if (LED < 0) {
      LED = 2;
    }
  }
  if (!(*(rx3_buff + 1) & 0x04)) {
    BUT = 1;
  }
  if (!(*(rx3_buff + 1) & 0x08)) {
    ssd1306_Fill(0x00);
    ssd1306_ClearOLED();
    ssd1306_SetCursor(0, 0);
    ssd1306_printf("button left\n");
    ssd1306_UpdateScreen();
    BUT = 2;
  }
  if (!(*(rx3_buff + 1) & 0x10)) {
    ssd1306_Fill(0x00);
    ssd1306_SetCursor(0, 0);
    ssd1306_ClearOLED();
    ssd1306_printf("button right\n");
    ssd1306_UpdateScreen();
    BUT = 3;
  }
  switch (LED) {
  case 0:
    uapi_gpio_set_val(LEDG, GPIO_LEVEL_HIGH);
    uapi_gpio_set_val(LEDR, GPIO_LEVEL_LOW);
    uapi_gpio_set_val(LEDB, GPIO_LEVEL_LOW);
    break;
  case 1:
    uapi_gpio_set_val(LEDB, GPIO_LEVEL_HIGH);
    uapi_gpio_set_val(LEDG, GPIO_LEVEL_LOW);
    uapi_gpio_set_val(LEDR, GPIO_LEVEL_LOW);
    break;
  case 2:
    uapi_gpio_set_val(LEDR, GPIO_LEVEL_HIGH);
    uapi_gpio_set_val(LEDG, GPIO_LEVEL_LOW);
    uapi_gpio_set_val(LEDB, GPIO_LEVEL_LOW);
    break;
  }
  switch (BUT) {
  case 1:
    uapi_gpio_set_val(LEDR, GPIO_LEVEL_HIGH);
    uapi_gpio_set_val(LEDG, GPIO_LEVEL_LOW);
    uapi_gpio_set_val(LEDB, GPIO_LEVEL_LOW);
    osal_msleep(500);
    uapi_gpio_toggle(LEDR);
    osal_msleep(500);
    uapi_gpio_toggle(LEDR);
    osal_msleep(500);
    uapi_gpio_toggle(LEDR);
    osal_msleep(500);
    uapi_gpio_toggle(LEDR);
    break;

  case 2:
    uapi_gpio_set_val(LEDR, GPIO_LEVEL_LOW);
    uapi_gpio_set_val(LEDG, GPIO_LEVEL_HIGH);
    uapi_gpio_set_val(LEDB, GPIO_LEVEL_LOW);
    osal_msleep(500);
    uapi_gpio_toggle(LEDG);
    osal_msleep(500);
    uapi_gpio_toggle(LEDG);
    osal_msleep(500);
    uapi_gpio_toggle(LEDG);
    osal_msleep(500);
    uapi_gpio_toggle(LEDG);
    break;
  case 3:
    uapi_gpio_set_val(LEDR, GPIO_LEVEL_LOW);
    uapi_gpio_set_val(LEDG, GPIO_LEVEL_LOW);
    uapi_gpio_set_val(LEDB, GPIO_LEVEL_HIGH);
    osal_msleep(500);
    uapi_gpio_toggle(LEDB);
    osal_msleep(500);
    uapi_gpio_toggle(LEDB);
    osal_msleep(500);
    uapi_gpio_toggle(LEDB);
    osal_msleep(500);
    uapi_gpio_toggle(LEDB);
    break;
  }

  osal_printk("BUT=%d\n", BUT);
  BUT = 0;
}

void io_expander_init(void) {
  uapi_pin_set_mode(GPIO_12, PIN_MODE_0);
  // gpio_select_core(GPIO_12, 2);
  uapi_pin_set_pull(GPIO_12, PIN_PULL_TYPE_DOWN);
  uapi_gpio_set_dir(GPIO_12, 0);

  uapi_pin_set_mode(LEDG, HAL_PIO_FUNC_GPIO);
  uapi_gpio_set_dir(LEDG, GPIO_DIRECTION_OUTPUT);
  uapi_gpio_set_val(LEDG, GPIO_LEVEL_LOW);

  uapi_pin_set_mode(LEDB, HAL_PIO_FUNC_GPIO);
  uapi_gpio_set_dir(LEDB, GPIO_DIRECTION_OUTPUT);
  uapi_gpio_set_val(LEDB, GPIO_LEVEL_LOW);

  uapi_pin_set_mode(LEDR, HAL_PIO_FUNC_GPIO);
  uapi_gpio_set_dir(LEDR, GPIO_DIRECTION_OUTPUT);
  uapi_gpio_set_val(LEDR, GPIO_LEVEL_LOW);

  osal_printk("io expander init start\r\n");

  // 发送命令初始化P05-P00
  uint8_t tx_buff[2] = {
      0x10,
      0x3B}; // 8位全部起作用配置P10-P16和INT，前面一个是命令，后面一个是输入数据0011
             // 1011
  uint8_t rx_buff[2] = {0};
  i2c_data_t pca2 = {0};
  pca2.send_buf = tx_buff;
  pca2.send_len = 2;
  pca2.receive_buf = rx_buff;
  pca2.receive_len = 2;
  errcode_t ret;
  for (int i = 0; i <= 10; i++) {
    ret = uapi_i2c_master_write(1, I2C_PCA_ADDR, &pca2);
  }
  if (ret != 0) {
    printf("io expander init failed, ret = %0x\r\n", ret);
  }

  uint8_t tx4_buff[2] = {0x18, 0x00}; // B00-B05 0000 1111
  uint8_t rx4_buff[2] = {0};
  i2c_data_t pca6 = {0};
  pca6.send_buf = tx4_buff;
  pca6.send_len = 2;
  pca6.receive_buf = rx4_buff;
  pca6.receive_len = 2;
  for (int i = 0; i <= 10; i++) {
    ret = uapi_i2c_master_write(1, I2C_PCA_ADDR, &pca6);
  }
  if (ret != 0) {
    printf("io expander init failed, ret = %0x\r\n", ret);
  }

  uint8_t tx1_buff[3] = {0x1B, 0xFF, 0x00}; // PA3平常状态
  uint8_t rx1_buff[2] = {0};
  i2c_data_t pca3 = {0};
  pca3.send_buf = tx1_buff;
  pca3.send_len = 3;
  pca3.receive_buf = rx1_buff;
  pca3.receive_len = 2;
  for (int i = 0; i <= 10; i++) {
    ret = uapi_i2c_master_write(1, I2C_PCA_ADDR, &pca3);
  }
  if (ret != 0) {
    printf("io expander init failed, ret = %0x\r\n", ret);
  }

  errcode_t ASD =
      uapi_gpio_register_isr_func(GPIO_12, 0x00000008, gpio_callback_func);
  if (ASD != 0) {
    uapi_gpio_unregister_isr_func(GPIO_12);
    osal_printk("io expander init FAILED:%0x\r\n", ASD);
  } else {
    osal_printk("io expander init SUCC!\r\n");
  }

  io_expander_i2c_lock = 1;
}
