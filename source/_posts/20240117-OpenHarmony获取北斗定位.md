---
title: OpenHarmony获取北斗定位
comments: true
typora-root-url: ..
date: 2024/01/17 20:33:00
updated: 2024/01/17 20:33:00
tags:
  - OpenHarmony
  - Hi3861
  - 北斗
categories:
  - 嵌入式
---

# OpenHarmony获取北斗定位

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

![主控外观-1](/images/20240117-OpenHarmony获取北斗定位/主控外观-1.png)

![主控外观-2](/images/20240117-OpenHarmony获取北斗定位/主控外观-2.png)

## 项目开源地址

[https://gitee.com/jitStackFarm/v2x_device](https://gitee.com/jitStackFarm/v2x_device)

## 依赖库

easy_wifi:
[https://gitee.com/hihopeorg/easy_wifi](https://gitee.com/hihopeorg/easy_wifi)

OpenHarmony_mqtt:
[https://gitee.com/lianzhian/OpenHarmony_mqtt](https://gitee.com/lianzhian/OpenHarmony_mqtt)

感谢前辈们的贡献

## 项目接线图

![项目接线图](/images/20240117-OpenHarmony获取北斗定位/项目接线图.png)

## 原理补充

北斗模块本质上可以理解为 ，

基于URAT串口通信的位置黑盒，

他们可以把位置信息通过AT指令的规范拿到。

也可以通过AT指令，来拿到更多的信息。
