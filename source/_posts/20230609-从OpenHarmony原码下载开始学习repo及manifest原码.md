---
title: 从OpenHarmony原码下载开始学习repo及manifest原码
comments: true
typora-root-url: ..
date: 2023/06/09 21:01:00
updated: 2023/06/09 21:01:00
tags:
  - OpenHarmony
  - repo
  - Git
categories:
  - OpenHarmony
---

# 从OpenHarmony原码下载开始学习repo及manifest原码

相信各位小伙伴入门OpenHarmony的第一步就是下载源代码，里面就用到了：

```
repo init -u https://gitee.com/openharmony/manifest -b OpenHarmony-3.2-Release --no-repo-verify
repo sync -c
repo forall -c 'git lfs pull'
```

可是各位小伙伴真的了解repo是什么吗？

## repo是什么？

repo原先是google卡法的用于管理Android版本的一个工具。它使用了Python对git进行了一定的封装，也就是说他并不能代替git，它简化了对多个git版本的管理。用repo版本管理库都需要使用git命令来进行操作。因此使用repo工具之前确保已经安装了git。

repo-gitee仓库：[https://gitee.com/oschina/repo](https://gitee.com/oschina/repo)

OpenHarmony manifest-gitee仓库：[https://gitee.com/openharmony/manifest](https://gitee.com/openharmony/manifest)

## 阅读文档（help命令）

repo内部已经提供了很完善的提供帮助的命令，在命令行直接输入repo可以获取关于repo当前可以用的命令。

![repo可用命令](/images/20230609-从OpenHarmony原码下载开始学习repo及manifest原码/repo可用命令.png)

可以看到我当前在一个被repo初始的目录，我们当前只有两个可以看使用的命令。

同时，也可以用help命令查看当前命令所有的参数、功能如：

![repo help帮助](/images/20230609-从OpenHarmony原码下载开始学习repo及manifest原码/repo-help帮助.png)

## 初始化repo仓库（init命令）

![repo init输出](/images/20230609-从OpenHarmony原码下载开始学习repo及manifest原码/repo-init输出.png)

可以看到，我们初始化以后从 [https://gitee.com/oschina/repo](https://gitee.com/oschina/repo) 这个路径下下载了什么东西下来。

![repo隐藏目录](/images/20230609-从OpenHarmony原码下载开始学习repo及manifest原码/repo隐藏目录.png)

如上图所示，在当前目录下多了个.repo目录，但是这个目录在ls指令下并不能显示，显然这是个隐藏目录。

进入这个目录：

![repo目录内容](/images/20230609-从OpenHarmony原码下载开始学习repo及manifest原码/repo目录内容.png)

我们当前看到了很多的文件这里就是从gitee上下载下来的文件了。

## 那么下载repo init -u会发生什么呢

回到最开始的目录文件夹下，删除现有的.repo/文件夹，让我们跑一遍鸿蒙的给我们的仓库的初始化操作

删除.repo/文件夹：

![删除repo目录](/images/20230609-从OpenHarmony原码下载开始学习repo及manifest原码/删除repo目录.png)

使用鸿蒙给我们的命令重新初始化一遍仓库：

![OpenHarmony仓库初始化](/images/20230609-从OpenHarmony原码下载开始学习repo及manifest原码/openharmony仓库初始化.png)

可以注意到，我们这次不但拉取了repo的代码仓库，还拉取了openharmony的代码仓库。

![manifest目录结构](/images/20230609-从OpenHarmony原码下载开始学习repo及manifest原码/manifest目录结构.png)

![manifest的ohos与chipsets目录](/images/20230609-从OpenHarmony原码下载开始学习repo及manifest原码/manifest-ohos-chipsets.png)

如上图，我们可以看到这个目录结构其实与openharmony的manifest源码一致。

这里有两个目录，一个是**ohos**目录、一个是**chipsets**目录。

ohos存储的通常是海思平台提供的一些能力，与具体硬件实现无关。

chipsets目录存储的通常是不同厂商的芯片驱动路径。

举个例子，我们如果想找HI3861的相关gitee仓库：

![HI3861的gitee仓库](/images/20230609-从OpenHarmony原码下载开始学习repo及manifest原码/hi3861-gitee仓库.png)

项目仓库路径=gitee仓库统一路径+项目名

所以我要找他们所有的soc信息都在这个路径下：

[https://gitee.com/openharmony/device_soc_hisilicon](https://gitee.com/openharmony/device_soc_hisilicon)

在这里：

![device_soc_hisilicon仓库](/images/20230609-从OpenHarmony原码下载开始学习repo及manifest原码/device-soc-hisilicon.png)

## 仓库同步 repo sync -c

当我们运行完repo sync -c对应的代码就会同步到这个目录底下

![repo sync结果1](/images/20230609-从OpenHarmony原码下载开始学习repo及manifest原码/repo-sync结果-1.png)

![repo sync结果2](/images/20230609-从OpenHarmony原码下载开始学习repo及manifest原码/repo-sync结果-2.png)

可以看到path的路径路径其实就是同步完后的本地路径。

下载完源代码，在最后repo forall -c 'git lfs pull'

将剩下的大文件全部下载完成，到这里整个源码下载的过程就结束了。
