#include "bh1750.h"
#include "i2c.h"
#include "pinctrl.h"
#define I2C_MASTER_BUS_ID 1
#define CONFIG_I2C_SCL_MASTER_PIN 15
#define CONFIG_I2C_SDA_MASTER_PIN 16
#define CONFIG_I2C_MASTER_PIN_MODE 2
#define I2C_MASTER_ADDR 0x1

#define I2C_SET_BANDRATE 115200
uint16_t bh1750_buff_H = 0;
uint16_t bh1750_buff_L = 0;
void bh1750_SendCMD(uint8_t cmd) {
  uint8_t buffer[] = {cmd};
  i2c_data_t data = {0};
  data.send_buf = buffer;
  data.send_len = sizeof(buffer);
  errcode_t ret =
      uapi_i2c_master_write(I2C_MASTER_BUS_ID, BH1750_ADDR >> 1, &data);
  if (ret != 0) {
    printf("BH1750:I2cWriteCMD(%02X) failed, %0X!\n", cmd, ret);
    return;
  }
}
void bh1750_ReadData(void) {
  uint8_t buffer[2] = {0};
  i2c_data_t data;
  data.receive_len = sizeof(buffer);
  data.receive_buf = buffer;
  errcode_t ret =
      uapi_i2c_master_read(I2C_MASTER_BUS_ID, BH1750_ADDR >> 1, &data);
  if (ret != 0) {
    printf("BH1750:I2cRead(len:%d) failed, %0X!\n", data.receive_len, ret);
    return;
  }
  bh1750_buff_H = data.receive_buf[0];
  bh1750_buff_L = data.receive_buf[1];
}
uint16_t bh1750_GetLightIntensity(void) {
  // 中间部分需要用户完成
  bh1750_ReadData();
  uint16_t data = (bh1750_buff_H << 8) | bh1750_buff_L;
  return (data * BH1750_RES) / 1.2;
}
void bh1750_init(void) {
  // I2C init
  uapi_pin_set_mode(CONFIG_I2C_SCL_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
  uapi_pin_set_mode(CONFIG_I2C_SDA_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
  uint32_t baudrate = I2C_SET_BANDRATE;
  uint32_t master_addr = I2C_MASTER_ADDR;
  uapi_i2c_master_init(I2C_MASTER_BUS_ID, baudrate, master_addr);

  // bh1750 init
  bh1750_SendCMD(BH1750_POWER_ON);
  bh1750_SendCMD(BH1750_MODULE_RESET);
  bh1750_SendCMD(BH1750_CONTINUE_H_MODE);
  osal_msleep(200);
  osal_printk("BH1750 Init SUCC!\r\n");
}
