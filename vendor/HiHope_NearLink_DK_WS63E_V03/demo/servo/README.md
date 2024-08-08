# servo

## 1.1 介绍

**功能介绍：** 通过GPIO2控制舵机转动90°、45°、0°、-45°、-90°。。

**软件概述：** GPIO引脚输出高低电平状态。

**硬件概述：** 核心板、交通灯板、SG92R舵机。SG92R频率为50HZ，核心板最小频率不支持50HZ，只能采用GPIO模拟的方式实现舵机转动，SG92R舵机转动要求如下图。硬件搭建要求如图所示：

参考[核心板原理图](../../../../docs/hardware/HiHope_NearLink_DK_WS63E_V03/HIHOPE_NEARLINK_DK_3863E_V03.pdf)、[底板原理图](../../../../docs/hardware/HiHope_NearLink_DK_WS63E_V03/HiSpark_WiFi_IoT_EXB_VER.A.pdf)

<img src="../../../../docs/pic/servo/image-20240418100509871.png" alt="image-20240418100509871" style="zoom:50%;" />  <img src="../../../../docs/pic/gyro/image-20240418101831113.png" alt="image-20240418101831113" style="zoom: 50%;" />

## 1.2 约束与限制

### 1.2.1 支持应用运行的芯片和开发板

本示例支持开发板：HiHope_NearLink_DK3863E_V03

### 1.2.2 支持API版本、SDK版本

本示例支持版本号：1.10.101

### 1.2.3 支持IDE版本、支持配套工具版本

本示例支持IDE版本号：1.0.0.6；

## 1.3 效果预览

舵机转动90°、45°、0°、-45°、-90°。

<img src="../../../../docs/pic/servo/image-20240418101831113.png" alt="image-20240418101831113" style="zoom: 50%;" />

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

### 1.4.3 uapi_gpio_toggle()


| **定义：**   | errcode_t uapi_gpio_toggle(pin_t pin); |
| ------------ | -------------------------------------- |
| **功能：**   | 翻转输出GPIO电平状态.                  |
| **参数：**   | pin：io引脚                            |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败     |
| **依赖：**   | include\driver\gpio.h                  |

## 1.5 具体实现

步骤一：设置GPIO为输出模式。

步骤二：设置GPIO输出为高或低。

步骤三：通过输出GPIO高低电平状态并模拟对应PWM波形。

## 1.6 实验流程

- 步骤一：在xxx\src\application\samples\peripheral文件夹新建一个sample文件夹，在peripheral上右键选择“新建文件夹”，创建Sample文件夹，例如名称”servo“。

  ![image-70551992](../../../../docs/pic/oled/image-20240801170551992-17225929704701.png)
- 步骤二：将xxx\vendor\HiHope_NearLink_DK_WS63E_V03\servo文件里面内容拷贝到**步骤一创建的Sample文件夹中”servo“**。

  ![image-20240418103106401](../../../../docs/pic/servo/image-20240418103106401.png)
- 步骤三：在xxx\src\application\samples\peripheral\CMakeLists.txt文件中新增编译案例，具体如下图所示（如果不知道在哪个地方加的，可以在“set(SOURCES "${SOURCES}" PARENT_SCOPE)”上面一行添加）。

  ![image-20240805104046818](../../../../docs/pic/led/image-20240805104046818.png)
- 步骤四：在xxx\src\application\samples\peripheral\Kconfig文件中新增编译案例，具体如下图所示（如果不知道在哪个地方加，可以在最后一行添加）。

  ![image-20240805104130076](../../../../docs/pic/led/image-20240805104130076.png)
- 步骤五：点击如下图标，选择KConfig，具体选择路径“Application/Enable the Sample of peripheral”，在弹出框中选择“support SERVO Sample”，点击Save，关闭弹窗。

  <img src="../../../../docs/pic/servo/image-20240801171406113.png" alt="image-20240801171406113" style="zoom: 67%;" /><img src="../../../../docs/pic/beep/image-20240205105234692-17119401758316.png" alt="image-20240205105234692" style="zoom: 50%;" /><img src="../../../../docs/pic/servo/image-20240418103238518.png" alt="image-20240418103238518" style="zoom:67%;" />
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
- 步骤十一：“软件烧录成功后，按一下开发板的RESET按键复位开发板，可以发现舵机转动90°、45°、0°、-45°、-90°。
- <img src="../../../../docs/pic/servo/image-20240418101831113.png" alt="image-20240418101831113" style="zoom: 50%;" />
