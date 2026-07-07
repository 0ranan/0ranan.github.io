---
title: ubuntu+windows 双系统下系统引导的制作
date: 2026/07/07 06:19:51
updated: 2026/07/07 06:19:51
comments: true
tags:
  - Ubuntu
  - Windows
  - 双系统
categories:
  - 教程
typora-root-url: ..
---

# ubuntu+windows 双系统下系统引导的制作

在 Windows + Ubuntu 双系统中，如果 GRUB 菜单里没有 Windows 选项，通常是因为新版 GRUB 出于安全考虑默认禁用了系统探测功能。请按照以下步骤恢复：

## 1. 启用 os-prober（最关键步骤）

自 Ubuntu 22.04 / GRUB 2.06 起，os-prober 默认被禁用，需手动开启：

```bash
sudo nano /etc/default/grub
```

在文件末尾添加或修改以下行（若已存在但被注释，去掉 `#`）：

```bash
GRUB_DISABLE_OS_PROBER=false
```

保存退出（Nano 编辑器按 `Ctrl+O` 回车保存，`Ctrl+X` 退出）。

## 2. 重新生成 GRUB 配置

执行更新命令让配置生效：

```bash
sudo update-grub
```

**成功标志：** 输出中必须包含类似 `Found Windows Boot Manager on /dev/nvme0n1p1...` 的信息。

## 3. 重启验证

重启电脑，GRUB 菜单中应出现 "Windows Boot Manager" 选项。
