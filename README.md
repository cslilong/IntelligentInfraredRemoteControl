IntelligentInfraredRemoteControl
================================

This project use the android phone control the device through Infrared.

#基于android的红外遥控软件设计

| . | .	        |
| --------    |            :----:             |
|课程题目：| 嵌入式计算系统         |
|学生姓名：| 李龙                         |
|学生学号：| S11102054               |
|专业班级：| 计算机科学与技术2班 |
|学院名称：| 信息科学与工程学院   |
|授课老师：| 徐成                         |

**2012 年 06月 15日**

###目录
- 1、移植android系统	1
- 1.1烧写uboot。	1
- 1.2 烧写linux内核	1
- 1.3 烧写临时文件系统cramfs	2
- 1.4 烧写android文件系统	4
- 1.4.1 启动cramfs文件系统	4
- 1.4.2 挂载U盘	4
- 1.4.3 烧写android文件系统镜像	5
- 2、红外控制驱动	6
- 2.1 红外控制简介	6
- 2.2 红外编码	7
- 2.3 红外驱动程序	7
- 3、Android客户端程序设计	7
- 3.1 Android系统架构	7
- 3.2 硬件抽象层（HAL）分析	7
- 3.3 Mokid工程代码分析	8
- 3.4 应用程序的使用	10
 
###1、移植android系统
一般系统在nandflash中的分布如下图：<br />

![image006](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image006.jpg)

系统的启动流程是：系统先启动uboot来初始化来设置系统的看门狗定时器，初始化硬件信息，然后加载linux内核驱动等，然后在加载文件系统，所以要移植android系统，我们首先要移植uboot，然后在移植linux内核，再次移植一个临时文件系统，最后再移植我们自己制作的android文件系统。下面我们就介绍一下adroid系统的移植过程。<br />
- 1.1烧写uboot。
开始下载u-boot.bin： <br />
```
# tftp c0008000 u-boot.bin 
```
将u-boot.bin文件通过TFTP32软件的网络功能下载到ANDROID-3G的SDRAM中，地址为c0008000。 <br />
```
# nand erase 0 40000 
```
擦除NANDFLASH上0地址开始大小为0x40000的空间。 <br />
```
# nand write c0008000 0 40000 
```
向NANDFLASH写入从SDRAM上c0008000地址处的文件，写入到NANDFLASH上0地址开始处0x40000大小的内容<br />

- 1.2 烧写linux内核
配置IP地址后，执行以下命令，如图所示： <br />
```
# tftp c0008000 zImage 
```
利用TFTP32软件将内核文件zImage烧写如SDRAM地址为c0008000 <br />
```
# nand erase 40000 3c0000 
```
将NANDFLASH起始地址为0x40000开始处大小为0x3c0000的空间擦除 <br />
```
# nand write c0008000 40000 3c0000 
```
从SDRAM的0xc0000000地址处，向NANDFLASH起始地址为0x40000写入大小为0x3c0000的文件内容。 如图1-1和图1-2：<br />
![image008](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image008.gif)
图1-1<br />
![image010](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image010.gif)
图1-2<br />
1.3 烧写临时文件系统cramfs
把文件系统制作成cramfs格式的文件，这样以便于烧写到nandflash中<br />
```
# tftp c0008000 root_mkfs.cramfs 
```
将文件系统root_mkfs.cramfs下载到SDRAM的0xc0008000地址处 
```
# nand erase 400000 400000 
```
将NANDFLASH上0x400000起始地址处0x400000大小的空间擦除 
```
# nand write c0008000 400000 400000 
```
将SDRAM上0xc0008000地址开始的内容烧写到NANDFLASH的0x400000起始地址，大小为0x400000，如图1-3和图1-4：<br />
![image012](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image012.gif)
图1-3<br />
![image014](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image014.gif)
图1-4<br />
- 1.4 烧写android文件系统
系统烧写cramfs文件系统后，即可利用cramfs文件系统烧写Android文件系统。<br />
- 1.4.1 启动cramfs文件系统 
进入u-boot界面,执行以下命令： <br />
```
# setenv bootargs noinitrd root=/dev/mtdblock0 console=ttySAC0 init=/linuxrc video=fb:LCD640x480 mem=224M 
```
配置启动参数 
```
#saveenv
#boot
```
保存配置参数，启动cramfs文件系统。
- 1.4.2 挂载U盘 
当设备正确检测到U盘并识别后，使用mount命令根据终端提示将U盘分区挂载到ARM端/mnt/sdcard目录，如图1-5： <br />
```
#mount –t vfat /dev/sda1 /mnt/udisk
```
![image016](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image016.gif)
图1-5<br />
- 1.4.3 烧写android文件系统镜像
1) 执行wr_ubifs命令烧写文件系统，如图1-6所示： <br />
![image018](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image018.gif)
图1-6<br />

2) 选择a格式化磁盘分区，之后烧写文件系统<br />
3) 提示是否校正触摸屏 ，选择y校正触摸屏，如图所示： <br />
4) 按照LCD显示器的提示，依次点击触摸屏，如图所示： <br />
5) 输入y重启系统。<br />
6）设置uboot启动参数：<br />
进入u-boot界面,执行以下命令： <br />
```
# setenv bootargs noinitrd console=ttySAC0 ubi.mtd=1 root=ubi0:rootfs rootfstype=ubifs init=/init video=fb:LCD640x480 mem=224M 
```
配置启动参数 <br />
```
#saveenv
#boot  
```
Android文件系统烧写完毕 ！<br />

启动系统以后，便可以进入android系统了。一下是android的一个界面截图：<br />
![image020](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image020.gif)

###2、红外控制驱动
- 2.1 红外控制简介
红外线遥控器已被广泛使用在各种类型的家电产品上，它的出现给使用电器提供了很多的便利。红外遥控系统一般由红外发射装置和红外接收设备两大部分组成。红外发射装置又可由键盘电路、红外编码芯片、电源和红外发射电路组成。红外接收设备可由红外接收电路、红外解码芯片、电源和应用电路组成。通常为了使信号能更好的被传输发送端将基带二进制信号调制为脉冲串信号，通过红外发射管发射。常用的有通过脉冲宽度来实现信号调制的脉宽调制（PWM）和通过脉冲串之间的时间间隔来实现信号调制的脉时调制（PPM）两种方法。
![image022](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image022.jpg)
- 2.2 红外编码
红外编码的数据格式包括了引导码、用户码、数据码和数据码反码，编码总占 32 位。数据反码是数据码反相后的编码，编码时可用于对数据的纠错。注意：第二段的用户码也可以在遥控应用电路中被设置成第一段用户码的反码。如下图所示：
![image024](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image024.jpg)
- 2.3 红外驱动程序
红外驱动程序就是按照各个按键的编码，使用定时器来发送这些编码即可，由于代码比较长，这里不贴出，具体参见cir.c文件，把cir.c放到linux内核的响应目录下，在makefile文件和Kconfig文件下添加相应的配置，就可以把红外控制驱动添加到linux内核中，这样在上层应用程序中就可以直接调用红外驱动程序了。

###3、Android客户端程序设计
- 3.1 Android系统架构
Android系统从下至上，Android系统分成4个层次：<br />
- 第1层：Linux操作系统及驱动；
- 第2层：本地代码（C/C++）框架；
- 第3层：JAVA框架（Framework）；
- 第4层：JAVA应用程序(APP)；
第1层和第2层之间，从Linux操作系统的角度来看，是内核空间与用户空间的分界线，第1层运行于内核空间，第2、3、4层运行于用户空间。 第2层和第3层之间，是本地代码层与JAVA代码层的接口。 第3层和第4层之间，是 Android系统的API接口，对于 Android应用程序的开发，第3层次以下的内容是不可见的，仅考虑系统API即可。 由于 Android系统需要支持JAVA代码的运行，这部分内容是Android的运行环境（Runtime）,由虚拟机和JAVA基本类组成。
- 3.2 硬件抽象层（HAL）分析
硬件抽象层HAL(Hardware Abstraction Layer)是位于Android用户空间和内核空间的一个层次。Android的HAL是为了保护一些硬件提供商的知识产权而提出的，HAL存在的主要原因： 
- 1)并不是所有的硬件设备都有标准的linux kernel的接口。 
- 2）KERNEL DRIVER涉及到GPL的版权，某些设备制造商并不愿意公开硬件驱动，所以才去用HAL方式绕过GPL。 
- 3）针对某些硬件，Android有一些特殊的需求。

***两种 HAL 架构比较***
目前存在两种HAL架构，位于libhardware_legacy目录下的“旧HAL架构”和位于libhardware目录下的"新HAL架构"。两种框架如下图所示。<br />
![image026](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image026.jpg)
libhardware_legacy 是将 *.so 文件当作shared library来使用，在runtime（JNI 部份）以 direct function call 使用 HAL module。通过直接函数调用的方式，来操作驱动程序。应用程序也可以不需要通过 JNI 的方式进行，直接加载 *.so （dlopen）的做法调用*.so 里的符号（symbol）也是一种方式。 
现在的libhardware 架构。HAL stub 是一种代理人（proxy）的概念，stub 虽然仍是以 *.so的形式存在，但HAL已经将 *.so 档隐藏起来了。Stub 向 HAL提供操作函数（operations），而 runtime 则是向 HAL 取得特定模块（stub）的 operations，再 callback 这些操作函数。
- 3.3 Mokid工程代码分析
Android的HAL的实现需要通过JNI(Java Native Interface)，JNI简单来说就是java程序可以调用C/C++写的动态链接库，这样的话，HAL可以使用C/C++语言编写，效率更高。在Android下访问HAL大致有以下两种方式：
- （1）Android的app可以直接通过service调用.so格式的jni 
![image028](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image028.jpg)
- （2）经过Manager调用service 
![image030](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image030.jpg)
在Android下访问HAL两种方式,我们在这里主要分析第一种实现方式： Android的APP可以直接通过service层调用JNI层。APP层、SERVER层、JNI层、HAL层。各层的代码具体参见mokoid文件夹下的源文件。
- 3.4 应用程序的使用
（1）首先打开安卓手机，找到应用程序启动图标<br />
![image032](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image032.jpg)
（2）点击启动图标，进入应用程序<br />
![image034](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image034.jpg)
（3）点击相应的按键即可对控制对象进行对应的控制。如调节声音，切换频道<br />
![image036](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image036.jpg)
（4）实际控制效果，如下图所示：<br />
![image038](https://github.com/cslilong/IntelligentInfraredRemoteControl/raw/master/images/image038.jpg)

