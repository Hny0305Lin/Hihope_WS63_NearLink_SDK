# SLE交通灯

## 1.1 介绍

**功能介绍：** 两块星闪开发板，星闪Server端向星闪client端发送指令信息，控制client端交通灯板红灯亮灭。

**硬件概述：** 核心板。硬件搭建要求如图所示：

参考[核心板原理图](../../../../docs/hardware/HiHope_NearLink_DK_WS63E_V03/HIHOPE_NEARLINK_DK_3863E_V03.pdf)、[交通灯板原理图](../../../../docs/hardware/HiHope_NearLink_DK_WS63E_V03/HiSpark_WiFi_IoT_SSL_VER.A.pdf)、[底板原理图](../../../../docs/hardware/HiHope_NearLink_DK_WS63E_V03/HiSpark_WiFi_IoT_EXB_VER.A.pdf)

<img src="../../../../docs/pic/wifista/image-20240415144302434.png" alt="image-20240415144302434" style="zoom:67%;" />

## 1.2 约束与限制

### 1.2.1 支持应用运行的芯片和开发板

本示例支持开发板：HiHope_NearLink_DK3863E_V03

### 1.2.2 支持API版本、SDK版本

本示例支持版本号：1.10.101

### 1.2.3 支持IDE版本

本示例支持IDE版本号：1.0.0.6；

## 1.3 效果预览

Client端接收数据：

![image](../../../../docs/pic/sle_led/image.png)

​	每隔500毫秒，Server会通过星闪连接发送消息，来控制Client端交通灯板上的3个LED灯的亮/灭状态。

## 1.4 接口介绍

#### 1.4.1 enable_sle()


| **定义：**   | errcode_t enable_sle(void);                                |
| ------------ | ---------------------------------------------------------- |
| **功能：**   | 使能SLE协议栈                                              |
| **参数：**   |                                                            |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败                         |
| **依赖：**   | include\middleware\services\bts\sle\sle_device_discovery.h |

#### 1.4.2 sle_connect_remote_device()


| 定义：       | errcode_t sle_connect_remote_device(const sle_addr_t *addr); |
| ------------ | ------------------------------------------------------------ |
| **功能：**   | 发送连接请求                                                 |
| **参数：**   | addr：地址                                                   |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败                           |
| **依赖：**   | include\middleware\services\bts\sle\sle_connection_manager.  |

#### 1.4.3 sle_update_connect_param()


| **定义：**   | errcode_t sle_update_connect_param(sle_connection_param_update_t *params); |
| ------------ | -------------------------------------------------------------------------- |
| **功能：**   | 发送更新连接参数请求                                                       |
| **参数：**   | params：连接参数                                                           |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败                                         |
| **依赖：**   | include\middleware\services\bts\sle\sle_connection_manager.h               |

#### 1.4.4 sle_pair_remote_device()


| **定义：**   | errcode_t sle_pair_remote_device(const sle_addr_t *addr);    |
| ------------ | ------------------------------------------------------------ |
| **功能：**   | 发送配对请求                                                 |
| **参数：**   | addr：地址                                                   |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败                           |
| **依赖：**   | include\middleware\services\bts\sle\sle_connection_manager.h |

## 开发流程

Terminal Node：

步骤 1 调用 enable_sle，打开 SLE 开关。

步骤 2 调用 sle_announce_seek_register_callbacks，注册设备公开和设备发现回调函数。

步骤 3 调用 sle_set_local_addr，设置本地设备地址。

步骤 4 调用 sle_set_local_name，设置本地设备名称。

步骤 5 调用 sle_set_announce_param，设置设备公开参数

步骤 6 调用 sle_set_announce_data，设置设备公开数据

步骤 7 调用 sle_start_announce，启动设备公开。

Grant Node： 步骤 1 调用 enable_sle，打开 SLE 开关。

步骤 2 调用 sle_announce_seek_register_callbacks，注册设备公开和设备发现回调函数。

步骤 3 调用 sle_set_local_addr，设置本地设备地址。

步骤 4 调用 sle_set_local_name，设置本地设备名称。

步骤 5 调用 sle_set_seek_param，设置设备发现参数。

步骤 6 调用 sle_start_seek，启动设备发现，并在回调函数中获得正在进行设备公开的设备信 息。

# 1. 使用介绍

* 步骤一：在xxx\src\application\samples\peripheral文件夹新建一个sample文件夹，在peripheral上右键选择“新建文件夹”，创建Sample文件夹，例如名称”sle_led“。

  ![image-70551992](../../../../docs/pic/beep/image-20240801170551992.png)
* 步骤二：将xxx\vendor\HiHope_NearLink_DK_WS63E_V03\sle_led文件里面内容拷贝到**步骤一创建的Sample文件夹中”sle_led“**下。

  ![image-20240403100728566](../../../../docs/pic/sle_led/image-20240403100728566.png)
* 步骤三：在xxx\src\application\samples\peripheral\CMakeLists.txt文件中新增编译案例，具体如下图所示（如果不知道在哪个地方加的，可以在“set(SOURCES "${SOURCES}" PARENT_SCOPE)”上面一行添加）。

  ![image_20240808184434](../../../../docs/pic/sle_led/image_20240808184434.png)
* 步骤四：在xxx\src\application\samples\peripheral\Kconfig文件中新增编译案例，具体如下图所示（如果不知道在哪个地方加，可以在最后一行添加）。

  ![image_20240808184555](../../../../docs/pic/sle_led/image_20240808184555.png)
* 步骤五：切换到sdk根目录下，执行 python3 build.py -c ws63-liteos-app menuconfig ，会出现选择弹窗，具体选择路径“Application/Enable Sample/Enable thie Sample of Peripheral ”，如果选择编译serverd端，选中“support  SLE_LED_SERVER Sample”；如果选择编译client端，选中“support SLE_LED_CLIENT Sample”。（需要准备两块开发板，选择不同的编译选项，烧录不同的镜像）（

  ![image_20240808164723](../../../../docs/pic/sle_led/image_20240808164723.png)
  ![image_20240808164731](../../../../docs/pic/sle_led/image_20240808164731.png)

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

* 步骤十一：复位两块开发板，等待建立连接后client端的交通信号灯会一直闪烁。