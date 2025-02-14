---
title: 从OpenHarmony原码下载学习repo 
date: 2023/6/9 16:07:27
updated: 2023/6/9 16:07:27
comments: true
tags:
  - OpenHarmony
  - git
  - repo 
categories:
  - git
typora-root-url: ..
---

# 从OpenHarmony原码下载学习repo 

相信各位小伙伴入门OpenHarmony的第一步就是下载源代码，里面就用到了：

```ssh
repo init -u https://gitee.com/openharmony/manifest -b OpenHarmony-3.2-Release --no-repo-verify
repo sync -c
repo forall -c 'git lfs pull'
```

可是各位小伙伴真的了解repo是什么吗？

# repo是什么？

repo原先是google卡法的用于管理Android版本的一个工具。它使用了Python对git进行了一定的封装，也就是说他并不能简化git，它简化了对多个git版本的管理。用repo版本管理库都需要使用git命令来进行操作。因此使用repo工具之前确保已经安装了git。

# 阅读文档

repo内部已经提供了很完善的提供帮助的命令，如果需要获取关于当前有、



[repo: Repo 是一个用于批量管理多仓库工程的工具，本仓库主要对 repo 做了二次开发，使其支持 Gitee Fork+PullRequest 流程](https://gitee.com/oschina/repo)





