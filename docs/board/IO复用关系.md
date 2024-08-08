# 一.IO复用关系表

| IO/MODE   | 0        | 1         | 2             | 3              | 4                  | 5                 | 6                   | 7            |
| --------- | -------- | --------- | ------------- | -------------- | ------------------ | ----------------- | ------------------- | ------------ |
| GPIO_00   | GPIO_00  | PWM0      | DIAG[0]       | SPI1_CSN       | JTAG_TDI           |                   |                     |              |
| GPIO_01   | GPIO_01  | PWM1      | DIAG[1]       | SPI1_IO0       | JTAG_MODE          | BT_SAMPLE         |                     |              |
| GPIO_02   | GPIO_02  | PWM2      | DIAG[2]       | SPI1_IO3       | WIFI_TSF_SYNC      | WL_GLP_SYNC_PULSE | BGLE_GLP_SYNC_PULSE |              |
| GPIO_03   | GPIO_03  | PWM3      | PMU_32K_TEST  | SPI1_IO1       | HW_ID[0]           | DIAG[3]           |                     |              |
| GPIO_04   | SSI_CLK  | PWM4      | GPIO_04       | SPI1_IO1       | JTAG_ENABLE        | DFT_JTAG_TMS      |                     |              |
| GPIO_05   | SSI_DATA | PWM5      | UART2_CTS     | SPI1_IO2       | GPIO_05            | SPI0_IN           | DFT_JTAG_TCK        |              |
| GPIO_06   | GPIO_06  | PWM6      | UART2_RTS     | SPI1_SCK       | REFCLK_FREQ_STATUS | DIAG[4]           | SPIO0_OUT           | DFT_JTAG_TDI |
| GPIO_07   | GPIO_07  | PWM7      | UART2_RXD     | SPI0_SCK       | I2S_MCLK           | DIAG[5]           |                     |              |
| GPIO_08   | GPIO_08  | PWM0      | UART2_TXD     | SPI0_CS1_N     | DIAG[6]            |                   |                     |              |
| GPIO_09   | GPIO_09  | PWM1      | RADAR_ANT0_SW | SPI0_OUT       | I2S_DO             | HW_ID[1]          | DIAG[7]             | JTAG_TD0     |
| GPIO_10   | GPIO_10  | PWM2      | ANT0_SW       | SPI0_CS0_N     | I2S_SCLK           | DIAG[0]           |                     |              |
| GPIO_11   | GPIO_11  | PWM3      | RADAR_ANT1_SW | SPI0_IN        | I2S_LRCLK          | DIAG[1]           | HW_ID[2]            |              |
| GPIO_12   | GPIO_12  | PWM4      | ANT1_SW       |                | I2S_DI             |                   | HW_ID[3]            |              |
| GPIO_13   | GPIO_13  | UART1_CTS | RADAR_ANT0_SW | DFT_JTAG_TD0   | JTAG_TMS           |                   |                     |              |
| GPIO_14   | GPIO_14  | UART1_RTS | RADAR_ANT1_SW | DFT_JTAG_TRSTN | JTAG_TCK           |                   |                     |              |
| UART1_TXD | GPIO_15  | UART1_TXD | I2C1_SDA      |                |                    |                   |                     |              |
| UART1_RXD | GPIO_16  | UART1_RXD | I2C1_SCL      |                |                    |                   |                     |              |
| UART0_TXD | GPIO_17  | UART0_TXD | I2C0_SDA      |                |                    |                   |                     |              |
| UART0_RXD | GPIO_18  | UART0_RXD | I2C0_SCL      |                |                    |                   |                     |              |
