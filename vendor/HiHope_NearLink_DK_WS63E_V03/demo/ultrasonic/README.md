# hcsr04

## 1.1 介绍

**功能介绍：** 本实验实现了通过GPIO 00和 GPIO 01实现超声波测距，并在屏幕上显示距离。

**软件概述：** GPIO引脚输出高低电平状态。

**硬件概述：** 核心板、交通灯板、HCSR04超声波。超声波数据手册参考https://gitee.com/HiSpark/hi3861_hdu_iot_application/issues/I6WPSS?from=project-issue 里面的HCSR04超声波模块。 超声波(TRIG)插在底板SDA位置，在底板左端寻找SDA，并通过核心板丝印可以看出SDA对应核心板GPIO 00;以超声波ECHO为参考：超声波(ECHO)插在底板SCK位置，在底板左端寻找SCK，并通过核心板丝印可以看出SCK对应核心板GPIO 01硬件搭建要求如图所示：

参考[核心板原理图](../../../../docs/hardware/HiHope_NearLink_DK_WS63E_V03/HIHOPE_NEARLINK_DK_3863E_V03.pdf)、[底板原理图](../../../../docs/hardware/HiHope_NearLink_DK_WS63E_V03/HiSpark_WiFi_IoT_EXB_VER.A.pdf)

<img src="../../../../docs/pic/ultrasonic/image-20240227154223440.png" alt="image-20240227154223440" style="zoom:67%;" />

## 1.2 约束与限制

### 1.2.1 支持应用运行的芯片和开发板

本示例支持开发板：HiHope_NearLink_DK3863E_V03

### 1.2.2 支持API版本、SDK版本

本示例支持版本号：1.10.101

### 1.2.3 支持IDE版本、支持配套工具版本

本示例支持IDE版本号：1.0.0.6；

## 1.3 效果预览

本实验实现了通过GPIO 00和 GPIO 01实现超声波测距，并在屏幕上显示距离。

<img src="../../../../docs/pic/ultrasonic/image-20240227154223440.png" alt="image-20240227154223440" style="zoom:67%;" /><img src="../../../../docs/pic/ultrasonic/image-20240227154244814.png" alt="image-20240227154244814" style="zoom:67%;" />

## 1.4 接口介绍

### 1.4.1 uapi_gpio_set_dir()


| **定义：**   | errcode_t uapi_gpio_set_dir(pin_t pin, gpio_direction_t dir); |
| ------------ | ------------------------------------------------------------- |
| **功能：**   | 设置GPIO的输入输出方向函数                                    |
| **参数：**   | pin： io引脚<br/>dir：输入输出方向                            |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败                            |
| **依赖：**   | include\driver\gpio.h                                         |

### 1.4.2 uapi_gpio_set_val()


| 定义：       | errcode_t uapi_gpio_set_val(pin_t pin, gpio_level_t level); |
| ------------ | ----------------------------------------------------------- |
| **功能：**   | 设置GPIO的输出状态                                          |
| **参数：**   | pin：io引脚<br/>level：设置输出为高或低                     |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败                          |
| **依赖：**   | include\driver\gpio.h                                       |

### 1.4.3 uapi_pin_set_mode()


| **定义：**   | errcode_t uapi_pin_set_mode(pin_t pin, pin_mode_t mode); |
| ------------ | -------------------------------------------------------- |
| **功能：**   | 设置引脚复用模式                                         |
| **参数：**   | pin：io<br/>mode：复用模式                               |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                        |
| **依赖：**   | include\driver\pinctrl.h                                 |

### 1.4.4 uapi_gpio_get_val()


| **定义：**   | gpio_level_t uapi_gpio_get_val(pin_t pin); |
| ------------ | ------------------------------------------ |
| **功能：**   | 读取GPIO的输入状态                         |
| **参数：**   | pin：io                                    |
| **返回值：** | GPIO输入电平值                             |
| **依赖：**   | include\driver\gpio.h                      |

## 1.5 具体实现

步骤一：设置GPIO为输出模式。

步骤二：设置GPIO输出为高或低。

步骤三：通过输出GPIO高低电平状态并模拟对应PWM波形。

## 1.6 实验流程

- 步骤一：在xxx\src\application\samples\peripheral文件夹新建一个sample文件夹，在peripheral上右键选择“新建文件夹”，创建Sample文件夹，例如名称”ultrasonic“。

  ![image-70551992](../../../../docs/pic/ultrasonic/image-20240801170551992.png)
- 步骤二：将xxxx\vendor\HiHope_NearLink_DK_WS63E_V03\ultrasonic文件里面内容拷贝到**步骤一创建的Sample文件夹中”ultrasonic“**。

  ![image-20240418154408339](../../../../docs/pic/ultrasonic/image-20240418154408339.png)
- 步骤三：在xxx\src\application\samples\peripheral\CMakeLists.txt文件中新增编译案例，具体如下图所示（如果不知道在哪个地方加的，可以在“set(SOURCES "${SOURCES}" PARENT_SCOPE)”上面一行添加）。

  ![image-20240805111954537](../../../../docs/pic/ultrasonic/image-20240805111954537.png)
- 步骤四：在xxx\src\application\samples\peripheral\Kconfig文件中新增编译案例，具体如下图所示（如果不知道在哪个地方加，可以在最后一行添加）。

  ![image-20240805112013406](../../../../docs/pic/ultrasonic/image-20240805112013406.png)
- 步骤五：点击如下图标，选择KConfig，具体选择路径“Application/Enable the Sample of peripheral”，在弹出框中选择“support  ULTRASONIC Sample”，点击Save，关闭弹窗。

  <img src="../../../../docs/pic/beep/image-20240801171406113.png" alt="image-20240801171406113" style="zoom: 67%;" /><img src="../../../../docs/pic/ultrasonic/image-20240205105234692-17119401758316.png" alt="image-20240205105234692" style="zoom: 50%;" /><img src="../../../../docs/pic/ultrasonic/image-20240418154439988.png" alt="image-20240418154439988" style="zoom:67%;" />
- 步骤六：点击“build”或者“rebuild”编译

  ![image-20240801112427220](../../../../docs/pic/beep/image-20240801112427220.png)
- 步骤七：编译完成如下图所示。

  ![image-20240801165456569](../../../../docs/pic/beep/image-20240801165456569.png)
- 步骤八：在HiSpark Studio工具中点击“工程配置”按钮，选择“程序加载”，传输方式选择“serial”，端口选择“comxxx”，com口在设备管理器中查看（如果找不到com口，请参考windows环境搭建）。

  ![image-20240801173929658](../../../../docs/pic/beep/image-20240801173929658.png)
- 步骤九：配置完成后，点击工具“程序加载”按钮烧录。

  ![image-20240801174117545](../../../../docs/pic/beep/image-20240801174117545.png)
- 步骤十：出现“Connecting, please reset device...”字样时，复位开发板，等待烧录结束。

  ![image-20240801174230202](../../../../docs/pic/beep/image-20240801174230202.png)
- 步骤十一:“软件烧录成功后，按一下开发板的RESET按键复位开发板，在屏幕上显示对应距离。

  <img src="../../../../docs/pic/ultrasonic/image-20240227154223440.png" alt="image-20240227154223440" style="zoom:67%;" /><img src="../../../../docs/pic/ultrasonic/image-20240227154244814.png" alt="image-20240227154244814" style="zoom:67%;" />
