---
title: HarmonyOS应用开发学习
date: 2023/5/25 13:58:09
updated: 2023/5/25 13:58:09
comments: true
tags:
  - 标签1
categories:
  - 分类1
typora-root-url: ..
---

# HarmonyOS应用开发学习

常用标签

通用父类标签：Component

> PS：Component类似于android开发的View

文本标签 Text 

> 类似与android开发的TextView



Flex布局
flexGrow	number	

> 设置父容器的剩余空间分配给此属性所在组件的比例。



Row - 对齐方式
- alignItems	VerticalAlign	

> 设置子组件在垂直方向上的对齐格式。
> 默认值：VerticalAlign.Center
> 从API version 9开始，该接口支持在ArkTS卡片中使用。

- - justifyContent	FlexAlign	
> 设置子组件在水平方向上的对齐格式。
> FlexAlign.Start
> 从API version 9开始，该接口支持在ArkTS卡片中使用。
> 

Column - 对齐方式

- alignItems	HorizontalAlign	
> 设置子组件在水平方向上的对齐格式。
  默认值：HorizontalAlign.Center
  从API version 9开始，该接口支持在ArkTS卡片中使用。

- justifyContent	 
> FlexAlign	设置子组件在垂直方向上的对齐格式。
  默认值：FlexAlign.Start
  从API version 9开始，该接口支持在ArkTS卡片中使用。
  示例	