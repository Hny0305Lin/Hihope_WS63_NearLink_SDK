# Windows 环境搭建

**备注：仅Liteos 系统版本支持，Openharmony系统版本不支持windows环境搭建**

## HiSpark Studio安装

*  下载并安装Windows版本的[HiSpark studio工具包](https://hispark-obs.obs.cn-east-3.myhuaweicloud.com/HiSparkStudio.exe)。

* 下载完成后，双击"HiSparkStudio.exe"安装。

   ![image-20240808155329000](../docs/pic/tools/image-20240808155329000.png)

* 安装界面如下，选择“我同意此协议”，点击“下一步”。

   ![image-20240801092735347](../docs/pic/tools/image-20240801092735347.png)
   
* 根据用户自身磁盘空间大小，选择对应的磁盘进行安装，选择完成后，点击下一步。

![image-20240801093020583](../docs/pic/tools/image-20240801093020583.png)

* 根据用户自身需要勾选附加任务，默认全部勾选，选择完成后，点击下一步。

![image-20240801093118299](../docs/pic/tools/image-20240801093118299.png)

* 点击“安装”，进行工具的安装。

![image-20240801093302132](../docs/pic/tools/image-20240801093302132.png)

* 安装过程如下，等待安装完成。

![image-20240801093323050](../docs/pic/tools/image-20240801093323050.png)

* 出现如下界面代表安装完成，点击完成即可。

![image-20240801093459447](../docs/pic/tools/image-20240801093459447.png)

* HiSpark studio打开主页界面如下。

![image-20240801093700218](../docs/pic/tools/image-20240801093700218.png)

## SDK代码下载

* 下载SDK代码，目前提供两种方式，第一种方式如下:

  * 第一种方式通过git指令下载（在Windows上使用git指令下载代码，需要提前安装git，这部分用户自行百度学习），在xx盘鼠标右键选择“Open Git Bash here”，如下图所所示

  ![image-20240807142741095](../docs/pic/tools/image-20240807142741095.png)

  * 在命令框中输入`git clone https://gitee.com/HiSpark/fbb_ws63.git`指令下载代码，等待下载完成。

  ![image-20240807151920249](../docs/pic/tools/image-20240807151920249.png)
  
  ​			![image-20240801103855026](../docs/pic/tools/image-20240801103855026.png)



* 第二种方式ZIP下载方式如下：

  * [SDK包下载地址](https://gitee.com/HiSpark/fbb_ws63)，在主界面上点击“克隆/下载”，选择“下载ZIP”，等待下载，下载完成如下。

    ![image-20240801100909013](../docs/pic/tools/image-20240801100909013.png)

    ![image-20240801101225144](../docs/pic/tools/image-20240801101225144.png)

  *  解压“fbb_ws63_master”，解压方式选择“Extract Here”

  ![image-20240801103718350](../docs/pic/tools/image-20240801103718350.png)

  

## 新建工程

* 代码下载完成后，打开HiSpark studio工具，点击“新建工程”。

  ![image-20240801104223178](../docs/pic/tools/image-20240801104223178.png)

* 新建工程界面中芯片：“WS63”，工程名：“xxx”（用户自定义，但是不能带中文，特殊符号），软件包：“xxx/fbb_ws63/src”（SDK软件包存放路径，这个地方一定要选到src层级，否则新建会失败），配置选择完成后，点击“完成按钮”。

    ![image-20240801112357075](../docs/pic/tools/image-20240801112357075.png)

* 点击“build”或者“rebuild”编译

    ![image-20240801112427220](../docs/pic/tools/image-20240801112427220.png)

* 编译完成如下图所示。

    ![image-20240801165456569](../docs/pic/tools/image-20240801165456569.png)

## 镜像烧录

* 硬件搭建：Typec线将板端与PC端连接

![image-20240801173105245](../docs/pic/tools/image-20240801173105245.png)

* 安装驱动“ch340g驱动”（[CH340G驱动下载地址](https://www.wch.cn/downloads/CH341SER_EXE.html)，**如果该链接失效或者无法下载，用户自行百度下载即可**），安装ch340g驱动，安装前单板需要与PC端相连，点击安装即可。

  ![image-20240801173439645](../docs/pic/tools/image-20240801173439645.png)

  ![image-20240801173618611](../docs/pic/tools/image-20240801173618611.png)

* 安装成功后，在HiSpark Studio工具中点击“工程配置”按钮，选择“程序加载”，传输方式选择“serial”，端口选择“comxxx”，com口在设备管理器中查看。

  ![image-20240801173929658](../docs/pic/tools/image-20240801173929658.png)

* 配置完成后，点击工具“程序加载”按钮烧录。

  ![image-20240801174117545](../docs/pic/tools/image-20240801174117545.png)

* 出现“Connecting, please reset device...”字样时，复位开发板，等待烧录结束。

  ![image-20240801174230202](../docs/pic/tools/image-20240801174230202.png)

* 烧录完成后，复位开发板可以在串口中看到对应的打印信息（**串口工具自行选择即可**）。

  ![image-20240801174330359](../docs/pic/tools/image-20240801174330359.png)