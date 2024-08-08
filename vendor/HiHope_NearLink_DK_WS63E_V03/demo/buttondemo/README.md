# button

## 1.1 介绍

**功能介绍：** 按键控制交通灯板上红色LED灯亮灭。

**软件概述：** GPIO引脚可以通过软件控制，使得CPU能够读取或写入GPIO引脚的电平值。而GPIO中断控制器则可以通过监测GPIO引脚的状态来检测外部设备的信号，这些信号可以触发中断请求，从而让CPU能够快速响应外部设备的事件。

**硬件概述：** 核心板、交通灯板。通过交通灯板上丝印可以看出是Switch与底板的MOSI相连，底板左边位置MOSI对应核心板上GPIO14。硬件搭建要求如图所示：

参考[核心板原理图](../../../../docs/hardware/HiHope_NearLink_DK_WS63E_V03/HIHOPE_NEARLINK_DK_3863E_V03.pdf)、[交通灯板原理图](../../../../docs/hardware/HiHope_NearLink_DK_WS63E_V03/HiSpark_WiFi_IoT_SSL_VER.A.pdf)、[底板原理图](../../../../docs/hardware/HiHope_NearLink_DK_WS63E_V03/HiSpark_WiFi_IoT_EXB_VER.A.pdf)

<img src="../../../../docs/pic/beep/image-20240415144302434.png" alt="image-20240415144302434" style="zoom:67%;" />

## 1.2 约束与限制

### 1.2.1 支持应用运行的芯片和开发板

本示例支持开发板：HiHope_NearLink_DK3863E_V03

### 1.2.2 支持API版本、SDK版本

本示例支持版本号：1.10.101

### 1.2.3 支持IDE版本

本示例支持IDE版本号：1.0.0.6

## 1.3 效果预览

通过交通灯板上的按键控制红色LED灯亮灭

## 1.4 接口介绍

#### 1.4.1 uapi_gpio_set_dir()


| **定义：**   | errcode_t uapi_gpio_set_dir(pin_t pin, gpio_direction_t dir); |
| ------------ | ------------------------------------------------------------- |
| **功能：**   | 设置GPIO的输入输出方向函数                                    |
| **参数：**   | pin： io引脚<br/>dir：输入输出方向                            |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败                            |
| **依赖：**   | include\driver\gpio.h                                         |

#### 1.4.2 uapi_gpio_set_isr_mode()


| 定义：       | errcode_t uapi_gpio_set_isr_mode(pin_t pin, uint32_t trigger);                                                       |
| ------------ | -------------------------------------------------------------------------------------------------------------------- |
| **功能：**   | HAL层GPIO设置中断模式                                                                                                |
| **参数：**   | pin：io引脚<br/>tigger：GPIO中断类型：1 : 上升沿中断；2 : 下降沿中断；3 : 双边沿中断；4 : 低电平中断；8 : 高电平中断 |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败                                                                                   |
| **依赖：**   | include\driver\gpio.h                                                                                                |

#### 1.4.3 uapi_gpio_register_isr_func()


| **定义：**   | errcode_t uapi_gpio_register_isr_func(pin_t pin, uint32_t trigger, gpio_callback_t callback);                                                        |
| ------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------- |
| **功能：**   | 注册GPIO的中断                                                                                                                                       |
| **参数：**   | pin：io引脚<br/>tigger：GPIO中断类型：1 : 上升沿中断；2 : 下降沿中断；3 : 双边沿中断；4 : 低电平中断；8 : 高电平中断  <br/>callback： 指向回调的指针 |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败                                                                                                                   |
| **依赖：**   | include\driver\gpio.h                                                                                                                                |

#### 1.4.4 uapi_gpio_enable_interrupt()


| **定义：**   | errcode_t uapi_gpio_enable_interrupt(pin_t pin); |
| ------------ | ------------------------------------------------ |
| **功能：**   | 使能GPIO指定端口的中断                           |
| **参数：**   | pin：io引脚                                      |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败               |
| **依赖：**   | include\driver\gpio.h                            |

## 1.5 具体实现

步骤一：设置GPIO为输入模式；

步骤二：注册中断类型，中断函数等；

步骤三：根据中断类型不同，判断IO电平是否发生变化，实现按键功能

## 1.6 实验流程

- 步骤一：在xxx\src\application\samples\peripheral文件夹新建一个sample文件夹，在peripheral上右键选择ZeroScript-Sample，创建Sample文件夹，例如名称”buttondemo“。

  ![image-20240205104416249](../../../../docs/pic/beep/image-20240801170551992.png)
- 步骤二：将xxx\vendor\HiHope_NearLink_DK_WS63E_V03\buttondemo文件里面内容拷贝到**步骤一创建的Sample文件夹中”buttondemo“**。

  ![image-20240808155814777](../../../../docs/pic/button/image-20240808155814777.png)
- 步骤三：在xxx\src\application\samples\peripheral\CMakeLists.txt文件中新增编译案例，具体如下图所示（如果不知道在哪个地方加的，可以在“set(SOURCES "${SOURCES}" PARENT_SCOPE)”上面一行添加）。

  ![image-20240808155959504](../../../../docs/pic/button/image-20240808155959504.png)
- 步骤四：在xxx\src\application\samples\peripheral\Kconfig文件中新增编译案例，具体如下图所示（如果不知道在哪个地方加，可以在最后一行添加）。

  ![image-20240808155947041](../../../../docs/pic/button/image-20240808155947041.png)
- 步骤五：点击如下图标，选择KConfig，具体选择路径“Application/Enable the Sample of peripheral”，在弹出框中选择“support BUTTON Sample”，点击Save，关闭弹窗。

  <img src="../../../../docs/pic/beep/image-20240801171406113.png" alt="image-20240801171406113" style="zoom: 67%;" /><img src="../../../../docs/pic/beep/image-20240205105234692-17119401758316.png" alt="image-20240205105234692" style="zoom: 50%;" /><img src="../../../../docs/pic/button/image-20240808160042906.png" alt="image-20240808160042906" style="zoom:67%;" />
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
- 步骤七“软件烧录成功后，按一下开发板的RESET按键复位开发板，可以通过交通灯板上的按键控制红色LED灯亮灭。
