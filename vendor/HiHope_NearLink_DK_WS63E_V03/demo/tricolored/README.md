# tricolored

## 1.1 介绍

**功能介绍：** 通过一个GPIO口控制核心板上SK6812亮绿、红、蓝三色灯。

**软件概述：** 通过GPIO模拟电平高低实现发送不同数据码，控制外设亮不同颜色。

**硬件概述：** 核心板。通过原理图可以看出SK6812 DI脚与核心板GPIO 5相连。SK6812数据手册：https://gitee.com/HiSpark/hi3861_hdu_iot_application/issues/I6WPSS?from=project-issue 里面的SK6812 MINI。硬件搭建要求如图所示：

参考[核心板原理图](../../../../docs/hardware/HiHope_NearLink_DK_WS63E_V03/HIHOPE_NEARLINK_DK_3863E_V03.pdf)

<img src="../../../../docs/pic/timer/image-20240415144302434.png" alt="image-20240415144302434" style="zoom:67%;" />

## 1.2 约束与限制

### 1.2.1 支持应用运行的芯片和开发板

本示例支持开发板：HiHope_NearLink_DK3863E_V03

### 1.2.2 支持API版本、SDK版本

本示例支持版本号：1.10.101

### 1.2.3 支持IDE版本、支持配套工具版本

本示例支持IDE版本号：1.0.0.6；

## 1.3 效果预览

通过一个GPIO口控制核心板上SK6812亮绿、红、蓝三色灯。

<img src="../../../../docs/pic/tricolored/image-20240418171054830.png" alt="image-20240418171054830" style="zoom:67%;" />

## 1.4 接口介绍

### 1.4.1 uapi_reg_setbit()


| **定义：**   | uapi_reg_setbit(addr, val)         |
| ------------ | ---------------------------------- |
| **功能：**   | 设置GPIO的高低电平                 |
| **参数：**   | pin： io引脚<br/>dir：输入输出方向 |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败 |
| **依赖：**   | include\common_def.h               |

### 1.4.2 uapi_tcxo_delay_count()


| 定义：       | errcode_t uapi_tcxo_delay_count(uint64_t ticks_delay)              |
| ------------ | ------------------------------------------------------------------ |
| **功能：**   | 延时ticks                                                          |
| **参数：**   | ticks_delay：延时ticks ，延时time = ticks_delay * （1 / 32000000） |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败                                 |
| **依赖：**   | extern errcode_t uapi_tcxo_delay_count(uint16_t ticks_delay);      |

### 1.4.3 uapi_reg_read32()


| **定义：**   | uapi_reg_read32(addr, val)         |
| ------------ | ---------------------------------- |
| **功能：**   | 读取GPIO电平值                     |
| **参数：**   | pin：io引脚                        |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败 |
| **依赖：**   | include\common_def.h               |

## 1.5 具体实现

步骤一：设置GPIO为输出模式。

步骤二：设置GPIO输出为高或低。

步骤三：通过uapi_tcxo_delay_count设置延时，模拟1码，0码数据。

## 1.6 实验流程

- 步骤一：在xxx\src\application\samples\peripheral文件夹新建一个sample文件夹，在peripheral上右键选择“新建文件夹”，创建Sample文件夹，例如名称”tricolored“。

  ![image-70551992](../../../../docs/pic/timer/image-20240801170551992-17228270862021.png)
- 步骤二：将xxx\vendor\HiHope_NearLink_DK_WS63E_V03\tricolored文件里面内容拷贝到**步骤一创建的Sample文件夹中”tricolored“**。

  ![image-20240418170746645](../../../../docs/pic/tricolored/image-20240418170746645.png)
- 步骤三：在xxx\src\application\samples\peripheral\CMakeLists.txt文件中新增编译案例，具体如下图所示（如果不知道在哪个地方加的，可以在“set(SOURCES "${SOURCES}" PARENT_SCOPE)”上面一行添加）。

  ![image-20240805110732310](../../../../docs/pic/tricolored/image-20240805110732310.png)
- 步骤四：在xxx\src\application\samples\peripheral\Kconfig文件中新增编译案例，具体如下图所示（如果不知道在哪个地方加，可以在最后一行添加）。

  ![image-20240805110753074](../../../../docs/pic/tricolored/image-20240805110753074.png)
- 步骤五：点击如下图标，选择KConfig，具体选择路径“Application/Enable the Sample of peripheral”，在弹出框中选择“support TRICOLORED Sample”，点击Save，关闭弹窗。

  <img src="../../../../docs/pic/tricolored/image-20240801171406113.png" alt="image-20240801171406113" style="zoom: 67%;" /><img src="../../../../docs/pic/beep/image-20240205105234692-17119401758316.png" alt="image-20240205105234692" style="zoom: 50%;" /><img src="../../../../docs/pic/tricolored/image-20240418170817541.png" alt="image-20240418170817541" style="zoom:67%;" />
- 步骤六：点击“build”或者“rebuild”编译

  ![image-20240801112427220](../../../../docs/pic/tricolored/image-20240801112427220.png)
- 步骤七：编译完成如下图所示。

  ![image-20240801165456569](../../../../docs/pic/tricolored/image-20240801165456569.png)
- 步骤八：在HiSpark Studio工具中点击“工程配置”按钮，选择“程序加载”，传输方式选择“serial”，端口选择“comxxx”，com口在设备管理器中查看（如果找不到com口，请参考windows环境搭建）。

  ![image-20240801173929658](../../../../docs/pic/tricolored/image-20240801173929658.png)
- 步骤九：配置完成后，点击工具“程序加载”按钮烧录。

  ![image-20240801174117545](../../../../docs/pic/tricolored/image-20240801174117545.png)
- 步骤十：出现“Connecting, please reset device...”字样时，复位开发板，等待烧录结束。

  ![image-20240801174230202](../../../../docs/pic/tricolored/image-20240801174230202.png)
- 步骤十一：“软件烧录成功后，按一下开发板的RESET按键复位开发板， 通过一个GPIO口控制核心板上SK6812亮绿、红、蓝三色灯。

  <img src="../../../../docs/pic/tricolored/image-20240418171054830.png" alt="image-20240418171054830" style="zoom:67%;" />
