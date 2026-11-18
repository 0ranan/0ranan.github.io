---
title: 利用OpenHarmon 轻量系统 实现 通过MQ3 获取空气中酒精含量
comments: true
typora-root-url: ..
date: 2024/06/10 19:19:20
updated: 2024/06/10 19:19:20
tags:
  - OpenHarmony
  - Hi3861
  - MQ3
categories:
  - 嵌入式
---

# 利用OpenHarmon 轻量系统 实现 通过MQ3 获取空气中酒精含量

## 概述

作者：an_ran0（刘张豪）

在辅助驾驶中我们常需要通过各类传感器器实时获取司机的各种状态，
本项目 就是利用HI3861 与MQTT、UPD实现实时获取空气中的酒精含量。

UDP 用于在车内总线传输信号、
MQTT 用于将数据发送至服务器 以便模拟V2X 状态共享。

## 开发环境

- CPU架构:RISC-V
- 运行环境：OpenHarmony 3.0
- 主控型号：OpenHarmony系统 AI HiSpark Wi-Fi IOT（Hi3861）
- 主控外观：

![主控外观-1](/images/20240610-利用OpenHarmon-轻量系统-实现-通过MQ3-获取空气中酒精含量/主控外观-1.png)

![主控外观-2](/images/20240610-利用OpenHarmon-轻量系统-实现-通过MQ3-获取空气中酒精含量/主控外观-2.png)

## 项目开源地址：

[https://gitee.com/jitStackFarm/v2x_device](https://gitee.com/jitStackFarm/v2x_device)

## 依赖库

easy_wifi:
[https://gitee.com/hihopeorg/easy_wifi](https://gitee.com/hihopeorg/easy_wifi)

OpenHarmony_mqtt:
[https://gitee.com/lianzhian/OpenHarmony_mqtt](https://gitee.com/lianzhian/OpenHarmony_mqtt)

感谢前辈们的贡献

## 项目接线图

![项目接线图](/images/20240610-利用OpenHarmon-轻量系统-实现-通过MQ3-获取空气中酒精含量/项目接线图.png)

## 其他相关硬件图片

> 为了避免营销嫌疑，将不提供购买链接

- MQ3 传感器

![MQ3传感器](/images/20240610-利用OpenHarmon-轻量系统-实现-通过MQ3-获取空气中酒精含量/MQ3传感器.png)

## 成果展示

### 图片：

![成果展示-1](/images/20240610-利用OpenHarmon-轻量系统-实现-通过MQ3-获取空气中酒精含量/成果展示-1.jpg)

![成果展示-2](/images/20240610-利用OpenHarmon-轻量系统-实现-通过MQ3-获取空气中酒精含量/成果展示-2.jpg)

## 代码架构图

![代码架构图](/images/20240610-利用OpenHarmon-轻量系统-实现-通过MQ3-获取空气中酒精含量/代码架构图.png)

## 入口程序文件

demo_entry.c

```c
#include <stdio.h>  // 标准输入输出
#include <stdlib.h> // 标准函数库
#include <string.h> // 字符串处理(操作字符数组)

#include "ohos_init.h" // 用于初始化服务(services)和功能(features)
#include "cmsis_os2.h" // CMSIS-RTOS API V2

#include "mq3.h"            // mq3传感器接口
#include "wifi_connecter.h" // easy wifi (station模式)
#include "mqtt_task.h"      // MQTT接口
#include "mqtt_api.h"       // 与服务器通信的mqtt API接口
#include "udp_client.h"     // 与车内设备通信的UDP API接口

#include "net_config.h"
#include "str_format.h"
void SetTrafficLightMain(){
    // // 灯光控制主体
    while (1)
    {
        int id = 0;
        int aout = GetMQ3AOut();
        int dout = GetMQ3DOut();
        char payload[1024] ;
        mq3_str_format(payload,id, aout, dout) ;
        publishMQ3(payload);
    
        SendUdpInfo(payload);
        Sleep( 1000); // 休眠
    }
    return;
}
// 主线程函数
static void mqttDemoTask(void *arg)
{
    (void)arg;
    // 连接AP
    WifiDeviceConfig config = {0};                      // 定义热点配置
    strcpy(config.ssid, PARAM_HOTSPOT_SSID);            // 设置热点配置中的SSID
    strcpy(config.preSharedKey, PARAM_HOTSPOT_PSK);     // 设置热点配置中的密马
    config.securityType = PARAM_HOTSPOT_TYPE;           // 设置热点配置中的加密方式
    int netId = ConnectToHotspot(&config);              // 连接到热点
    if (netId < 0)                                      // 检查是否成功连接到热点
    {
        printf("Connect to AP failed!\r\n");
    }

    printf("Connect to AP success!\r\n");
    
    // 初始化并启动MQTT任务，连接MQTT服务器
    MqttTaskInit();                                                             // 初始化并启动MQTT任务
    const char *host = PARAM_SERVER_ADDR;                                       // MQTT服务器IP地址
    unsigned short port = atoi(PARAM_SERVER_PORT);                              // MQTT服务器端口
    const char *clientId = "jitoa_anran0_mq3_0";                                // MQTT客户端ID
    const char *username = PARAM_SERVER_USERNAME;                                        // MQTT服务器用户名
    const char *password = PARAM_SERVER_PASSWORD;                                        // MQTT服务器密码
    if (MqttTaskConnect(host, port, clientId, username, password) == -1) // 连接MQTT服务器
        printf("Connect to MQTT server failed!\r\n");
    else
        printf("Connect to MQTT server success!\r\n");
    
    // 初始化MQ3 酒精传感器
    InitMQ3();
     inotUdpClient(PARAM_SERVER_ADDR, PARAM_SERVER_PORT);
    SetTrafficLightMain() ;
}
// 入口函数
static void mqttDemoEntry(void)
{
    // 定义线程属性
    osThreadAttr_t attr;
    attr.name = "mqttDemoTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = osPriorityNormal;

    // 创建线程
    if (osThreadNew(mqttDemoTask, NULL, &attr) == NULL)
    {
        printf("[mqttDemoEntry] Falied to create mqttDemoTask!\n");
    }
}

// 运行入口函数
SYS_RUN(mqttDemoEntry);

```

## 答疑交流

欢迎对我的项目感兴趣的小伙伴、或者是需要答疑的小伙伴 可以扫码前来交流
如果你只是单纯想聊聊天，我也欢迎。
QQ群：750659141

![答疑交流-QQ群](/images/20240610-利用OpenHarmon-轻量系统-实现-通过MQ3-获取空气中酒精含量/答疑交流-QQ群.png)
