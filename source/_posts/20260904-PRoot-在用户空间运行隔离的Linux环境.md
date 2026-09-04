---
title: PRoot — 在用户空间运行隔离的 Linux 环境
date: 2026/09/04 14:35:00
comments: true
tags:
  - Linux
  - PRoot
  - Termux
  - 环境配置
categories:
  - 笔记
typora-root-url: ..
---

# PRoot — 在用户空间运行隔离的 Linux 环境

> 参考
>
> [PRoot 官方文档](https://proot-me.github.io/)
> [Termux PRoot-Distro 文档](https://github.com/termux/proot-distro)
> [PRoot GitHub](https://github.com/proot-me/PRoot)

## 什么是 PRoot

**PRoot** 是一个在用户空间（user-space）实现的工具，它可以模拟 `chroot`、`mount --bind` 和 `binfmt_misc` 的功能。简单来说，它让你**不需要 root 权限**就能运行一个隔离的 Linux 环境。

在 Android 上，Termux 配合 PRoot 是一个非常强大的组合。你可以在不 root 手机的情况下，运行完整的 Debian、Ubuntu、Arch 等发行版。

---

## 为什么用 PRoot

| 需求 | 解决方案 |
|------|---------|
| 没有 root 权限 | PRoot 完全在用户空间运行，不需要 root |
| 想在 Android 上跑完整 Linux | PRoot + Termux = 完整的 Linux 环境 |
| 需要隔离的开发环境 | 每个 PRoot 容器相互独立 |
| 运行不同架构的程序 | PRoot 内置 QEMU 用户态模拟 |
| 不想折腾 root | 开箱即用，一条命令安装发行版 |

我当前写这篇博客的环境，就是跑在 **Debian 13 (Trixie) + PRoot-Distro** 里的。验证一下：

```bash
$ uname -a
Linux localhost 6.17.0-PRoot-Distro #1 SMP PREEMPT_DYNAMIC aarch64 GNU/Linux
```

看到 `PRoot-Distro` 了吗？这就是证明。

---

## 安装 PRoot（Termux 环境）

### 1. 安装 Termux

从 F-Droid 或 GitHub 下载 Termux（Google Play 版已停止维护）：
- https://f-droid.org/packages/com.termux/
- https://github.com/termux/termux-app/releases

### 2. 安装 proot-distro

```bash
pkg update
pkg install proot-distro -y
```

### 3. 查看可用的发行版

```bash
proot-distro list
```

输出示例：

```
Alias          Name                       Status
------------   ----------------------     ------
alpine         Alpine Linux (edge)
archlinux      Arch Linux
/debian        Debian (stable)            Installed: yes
fedora         Fedora
ubuntu         Ubuntu (LTS)
```

### 4. 安装发行版

```bash
# 安装 Debian
proot-distro install debian

# 安装 Ubuntu
proot-distro install ubuntu

# 安装 Alpine（轻量级）
proot-distro install alpine
```

### 5. 进入发行版

```bash
# 登录 debian（root 用户）
proot-distro login debian

# 登录并指定用户（推荐创建普通用户）
proot-distro login debian --user anran
```

---

## 常用命令速查

### 发行版管理

```bash
# 安装
proot-distro install <alias>

# 登录
proot-distro login <alias>

# 备份（导出为 tar 文件）
proot-distro backup --output debian-backup.tar.gz debian

# 恢复
proot-distro restore debian-backup.tar.gz

# 删除
proot-distro remove debian

# 重置（重新安装）
proot-distro reset debian
```

### 容器内常用操作

进入 Debian 后，就是一个完整的 Linux 环境：

```bash
# 更新源
apt update && apt upgrade -y

# 安装基础工具
apt install git vim curl wget -y

# 安装开发环境
apt install build-essential python3 nodejs -y

# 查看当前环境
cat /etc/os-release
uname -a
```

---

## PRoot 与 Docker 的对比

| 特性 | PRoot | Docker |
|------|-------|--------|
| 需要 root | ❌ 不需要 | ✅ 通常需要 |
| 内核支持 | ❌ 不需要 cgroup | ✅ 需要 |
| 隔离性 | 进程级 | 操作系统级 |
| 性能 | 接近原生（ptrace 开销） | 接近原生 |
| 适用场景 | Android、无 root 服务器 | 服务器、CI/CD |
| 网络 | 共享主机网络 | 独立网络栈 |

**结论**：有 root 用 Docker，没 root 用 PRoot。

---

## 实际应用场景

### 场景 1：在 Android 上部署 Hexo 博客

我就是这么做的。手机 + Termux + PRoot-Debian + Git + Node.js = 完整的博客开发环境。

```bash
# 在 PRoot Debian 里
apt install git nodejs npm -y
npm install -g hexo-cli
hexo init blog
cd blog
hexo server
```

### 场景 2：运行 AI 编程助手

像 Kimi Code、Claude Code 这类 CLI 工具，在 PRoot 里完全能跑。

```bash
# 安装 Node.js 后
npm install -g @moonshot-ai/kimi-code
kimi --version
```

### 场景 3：多发行版并存

同时装 Debian（稳定）、Arch（新软件）、Alpine（轻量），按需切换。

```bash
proot-distro install debian
proot-distro install archlinux
proot-distro install alpine

# 想用什么进什么
proot-distro login debian    # 写博客、稳定开发
proot-distro login archlinux # 尝鲜新软件
proot-distro login alpine    # 跑轻量服务
```

---

## 常见问题

### Q: PRoot 里能跑 Docker 吗？

**不能**。Docker 需要内核级的容器支持（cgroups、namespaces），PRoot 是用户空间的，满足不了。但可以装 Podman（无守护进程版本）。

### Q: PRoot 和 chroot 有什么区别？

chroot 需要 **root 权限**，而且只是改变了根目录，不处理系统调用。PRoot 用 `ptrace` 拦截和重写系统调用，不需要 root，功能也更强大。

### Q: 为什么我的 PRoot 环境显示 `localhost`？

这是正常的。PRoot 没有独立的网络命名空间，所以主机名和宿主机（Termux）一致。

### Q: 文件存放在哪里？

```
/data/data/com.termux/files/usr/var/lib/proot-distro/installed-rootfs/<发行版名>/
```

可以直接在 Termux 里访问这个路径，和 PRoot 内互通。

---

## 总结

PRoot 是一个被低估的神器。它让「在没有 root 权限的设备上运行完整 Linux」这件事变得简单、稳定、可用。

如果你有一台 Android 手机，装个 Termux + PRoot，你就拥有了一个口袋里的 Linux 工作站。写博客、跑脚本、甚至开发完整项目，都不在话下。

> **我现在的环境**：Android → Termux → PRoot-Distro → Debian 13 → Node.js → Hexo → 这篇博客。
>
> 全部在手机上完成。🔥
