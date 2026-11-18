---
title: "@Observed与@ObjectLink的坑"
comments: true
typora-root-url: ..
date: 2023/09/09 18:45:00
updated: 2023/09/09 18:45:00
tags:
  - OpenHarmony
  - ArkTS
  - ArkUI
categories:
  - OpenHarmony
---

# @Observed与@ObjectLink的坑

> 此博客由金陵科技学院-开放原子开源社-刘张豪编写

## 概述

之前这篇文章因为拿去参加字节跳动青训营的活动，所以下架了，现在活动我就重新上穿到开源社区了。

@Observed与@ObjectLink是开发中常用的复杂数据类型绑定的技巧。
但是@Observed与@ObjectLink在开发中存在着许多坑。
接下来我将列举我遇到的几个坑。

## 测试环境

测试语言：TypeScript

## 正确使用场景

首先我将举例正确在开发中@Observed与@ObjectLink的正确使用方式，并且正确文件的源文件我也会打包放置在资源文件中。

### 先来看实现效果：

![正确使用效果](/images/20230909-Observed与ObjectLink的坑/正确使用效果.png)

### 源代码：

![正确用法源码](/images/20230909-Observed与ObjectLink的坑/正确用法源码.png)

DataModel.ets

```typescript
@Observed
export class  DataInteface {
  value: number;
}
// 实现数据类
@Observed
export class DataModel implements DataInteface {
  value: number = 0;
}
```

Index.ets

```typescript
import { DataModel, DataInteface } from "../model/DataModel"

@Entry
@Component
struct Index {
  @State
  sensors: DataInteface[] = [
    new DataModel(),
    new DataModel(),
    new DataModel(),
    new DataModel(),
    new DataModel(),
    new DataModel(),
  ];

  build() {
    Row() {
      List() {
        ForEach(this.sensors, (item: DataInteface) => {
          ListItem() {
            SensorView({ sen: item })
          }.width("100%")
        })
      }
      .width("100%")
    }
  }
}

@Component
struct SensorView {
  @ObjectLink
  sen: DataInteface
  build() {
    Button(`点击数据会自增${this.sen.value}`)
      .fontSize(20)
      .backgroundColor(Color.Yellow)
      .onClick(() => {
        this.sen.value++;
        console.log("anran" + this.sen.value)
      })
  }
}
```

## 错误使用方法

### 1.使用interface撰写接口

> 虽然按照TypeScript规范需要使用interface撰写接口，但是@Observed不能修饰interface。所以在OpenHarmony开发中应当使用class撰写接口。
效果：可以进行数据双向绑定，但是编译时会报错。

错误代码：

```typescript
// 接口
@Observed
export interface  DataInteface {
  value: number;
}
// 实现类
@Observed
export class DataModel implements DataInteface {
  value: number = 0;
}
```

### 2. 使用链式编程初始化数据

> 在OpenHarmony中不应当使用链式编程初始化@Observed修饰的对象。因为这将会替代掉@Observed帮助我们生产的对象转而使用原生的数据对象。这将会导致所有的数据无法进行双向绑定。
在实际项目中很容易因为一贯的编程经验而使用链式编程，这时候就会出现问题，而且这种问题很难排查，小编就遇到了这种问题。

效果：

![链式编程初始化效果](/images/20230909-Observed与ObjectLink的坑/链式编程初始化效果.png)

代码：

![链式编程初始化代码](/images/20230909-Observed与ObjectLink的坑/链式编程初始化代码.png)

### 3、直接使用for循环对@Observed对象渲染

直接使用for循环对@Observed对象渲染渲染，而不封装，不使用@ObjectLink对数据引用

运行效果：同错误2

代码：

![for循环渲染代码](/images/20230909-Observed与ObjectLink的坑/for循环渲染代码.png)

### 4、只给接口使用了@Observed没有给实现类写@Observed

运行效果：同错误2

代码：

![仅接口使用Observed代码](/images/20230909-Observed与ObjectLink的坑/仅接口使用Observed代码.png)

### 5、没有给接口使用@Observed，只给实现类写@Observed

效果：程序可以正常双向绑定，但是编译器会给出警告

代码：

![仅实现类使用Observed代码](/images/20230909-Observed与ObjectLink的坑/仅实现类使用Observed代码.png)

## 结论

@Observed 可能是通过某种替代原有对象的方式实现的，所以使用链式编程初始化对象会导致数据双向绑定失效。
而通常来说编译器的警告都是自己编译器给的，很可能不会正常影响程序运行。
@ObjectLink 与 @Observed 能够极大的提高开发效率，但是也存在很多坑。特别是无法使用链式编程这种问题，会让很多从其他开发方向转OpenHarmony的老鸟都掉坑里。所以必须认真对待@ObjectLink 与 @Observed。
