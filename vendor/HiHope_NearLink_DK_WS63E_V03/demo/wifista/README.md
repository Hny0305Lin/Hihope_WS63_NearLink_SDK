# wifista

## 1.1 介绍

**功能介绍：** 实现核心板连接路由器热点或者手机热点，案例中手机或者路由器热点需要设置为SSID：H， 密码：12345678。

**硬件概述：** 核心板。硬件搭建要求如图所示：

<img src="../../../../docs/pic/wifiap/image-20240415144302434.png" alt="image-20240415144302434" style="zoom:67%;" />

## 1.2 约束与限制

### 1.2.1 支持应用运行的芯片和开发板

本示例支持开发板：HiHope_NearLink_DK3863E_V03

### 1.2.2 支持API版本、SDK版本

本示例支持版本号：1.10.101

### 1.2.3 支持IDE版本、支持配套工具版本

本示例支持IDE版本号：1.0.0.6；

## 1.3 效果预览

连接上热点后，可以看到addr：192.168.43.194为本机IP地址，server_id：192.168.43.1为服务端IP地址,使用AT+PING=“服务端IP地址”。

![image-20240418172420858](../../../../docs/pic/wifista/image-20240418172420858.png)![image-20240418172612581](../../../../docs/pic/wifista/image-20240418172612581.png)

## 1.4 接口介绍

### 1.4.1 wifi_sta_enable()


| **定义：**   | errcode_t wifi_sta_enable(void);                |
| ------------ | ----------------------------------------------- |
| **功能：**   | 开启SoftAP                                      |
| **参数：**   | config：SoftAp的配置                            |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败              |
| **依赖：**   | include\middleware\services\wifi\wifi_hotspot.h |

### 1.4.2 netifapi_netif_set_addr()


| **定义：**   | err_t netifapi_netif_set_addr(struct netif *netif, const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw); |
| ------------ | ------------------------------------------------------------------------------------------------------------------------------ |
| **功能：**   | 设置网络接口的IP地址                                                                                                           |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败                                                                                             |
| **依赖：**   | open_source\lwip\lwip_v2.1.3\src\include\lwip\netifapi.h                                                                       |

### 1.4.3 netifapi_dhcps_start()


| **定义：**   | err_t netifapi_dhcps_start(struct netif *netif, char *start_ip, u16_t ip_num); |
| ------------ | ------------------------------------------------------------------------------ |
| **功能：**   | 启动dhcp服务                                                                   |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败                                             |
| **依赖：**   | open_source\lwip\lwip_v2.1.3\src\include\lwip\netifapi.h                       |

### 1.4.4 wifi_sta_scan()


| **定义：**   | errcode_t wifi_sta_scan(void);                 |
| ------------ | ---------------------------------------------- |
| **功能：**   | 进行全信道基础扫描                             |
| **参数：**   | void类型                                       |
| **返回值：** | ERRCODE_SUCC：成功   Other：失败               |
| **依赖：**   | include\middleware\services\wifi\wifi_device.h |

### 1.4.5 wifi_sta_connect()


| **定义：**   | errcode_t wifi_sta_connect(const wifi_sta_config_stru *config); |
| ------------ | --------------------------------------------------------------- |
| **功能：**   | 进行连接网络                                                    |
| **参数：**   | config：连接的网络参数                                          |
| **返回值：** | ERRCODE_SUCC：成功   Other：失败                                |
| **依赖：**   | include\middleware\services\wifi\wifi_device.h                  |

### 1.4.6 wifi_sta_get_ap_info()


| **定义：**   | errcode_t wifi_sta_get_ap_info(wifi_linked_info_stru *result); |
| ------------ | -------------------------------------------------------------- |
| **功能：**   | 获取station连接的网络状态                                      |
| **参数：**   | result：连接状态                                               |
| **返回值：** | ERRCODE_SUCC：成功   Other：失败                               |
| **依赖：**   | include\middleware\services\radar\radar_service.h              |

## 1.5 具体实现

步骤一：初始化wifi。

步骤二：启动STA扫描，获取等待连接的网络。

步骤三：DHCP获取IP地址，并PING主机。

## 1.6 实验流程

- 步骤一：在xxx\src\application\samples\peripheral文件夹新建一个sample文件夹，在peripheral上右键选择“新建文件夹”，创建Sample文件夹，例如名称”wifista“。

  ![image-70551992](../../../../docs/pic/wifista/image-20240801170551992.png)
- 步骤二：将xxx\vendor\HiHope_NearLink_DK_WS63E_V03\wifista文件里面内容拷贝到**步骤一创建的Sample文件夹中”wifista“**下。

  ![image-20240418171824704](../../../../docs/pic/wifista/image-20240418171824704.png)
- 步骤三：在xxx\src\application\samples\peripheral\CMakeLists.txt文件中新增编译案例，具体如下图所示（如果不知道在哪个地方加的，可以在“set(SOURCES "${SOURCES}" PARENT_SCOPE)”上面一行添加）。

  ![image-20240805113015053](../../../../docs/pic/wifista/image-20240805113015053.png)
- 步骤四：在xxx\src\application\samples\peripheral\Kconfig文件中新增编译案例，具体如下图所示（如果不知道在哪个地方加，可以在最后一行添加）。

  ![image-20240805113039140](../../../../docs/pic/wifista/image-20240805113039140.png)
- 步骤五：点击如下图标，选择KConfig，具体选择路径“Application/Enable the Sample of peripheral”，在弹出框中选择“support WIFISTA Sample”，点击Save，关闭弹窗。

  <img src="../../../../docs/pic/beep/image-20240801171406113.png" alt="image-20240801171406113" style="zoom: 67%;" /><img src="../../../../docs/pic/wifista/image-20240205105234692-17119401758316.png" alt="image-20240205105234692" style="zoom: 50%;" /><img src="../../../../docs/pic/wifista/image-20240418172048117.png" alt="image-20240418172048117" style="zoom:80%;" />
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
- 步骤十一：连接上热点后，可以看到addr：192.168.43.194为本机IP地址，server_id：192.168.43.1为服务端IP地址,使用AT+PING=“服务端IP地址”。

  ![image-20240418172420858](../../../../docs/pic/wifista/image-20240418172420858.png)![image-20240418172612581](../../../../docs/pic/wifista/image-20240418172612581.png)
