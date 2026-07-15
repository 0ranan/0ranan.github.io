---
title: OpenSandbox 学习笔记
date: 2026/07/13 09:00:00
updated: 2026/07/13 09:00:00
comments: true
tags:
  - OpenSandbox
  - AI
  - Docker
  - K8s
categories:
  - Agent
typora-root-url: ..
---

# OpenSandbox 学习笔记

## 概述

OpenSandbox 是由阿里巴巴云智能团队开源的**面向 AI 应用的通用沙箱平台**，旨在为 AI Agent 提供安全、隔离、可扩展的代码执行环境。

- **开源协议**：Apache 2.0
- **GitHub**：<https://github.com/opensandbox-group/OpenSandbox>
- **官网**：<https://open-sandbox.ai>

## 架构设计

### Sandbox Lifecycle API

管理沙箱实例的整个生命周期：

| 端点                       | 方法     | 描述       |
| ------------------------ | ------ | -------- |
| `/sandboxes`             | POST   | 创建新的沙盒实例 |
| `/sandboxes/{id}`        | GET    | 查询沙盒状态   |
| `/sandboxes/{id}/pause`  | POST   | 暂停沙盒     |
| `/sandboxes/{id}/resume` | POST   | 恢复沙盒     |
| `/sandboxes/{id}`        | DELETE | 销毁沙盒     |
| `/sandboxes/{id}/renew`  | POST   | 续期沙盒     |

**状态机流转**：

```
Pending → Running → Pausing → Paused → Running (resume)
                     ↓                    ↓
                Terminated            Terminated
                     ↑
                  Failed
```

### Execd API

定义沙盒内部的代码执行接口：

| 路由                        | 方法   | 描述             |
| ------------------------- | ---- | -------------- |
| `/code`                   | POST | 执行代码           |
| `/code/{id}/interrupt`    | POST | 中断正在执行的代码      |
| `/code/context/{id}`      | POST | 创建/切换执行上下文     |
| `/command`                | POST | 执行 shell 命令    |
| `/command/{id}/interrupt` | POST | 中断正在执行的命令      |
| `/files/upload`           | POST | 上传文件到沙盒        |
| `/files/download/{path}`  | GET  | 从沙盒下载文件        |
| `/system/monitor`         | GET  | 获取 CPU/内存等系统指标 |

## K8S 计算体系

从下到上，可以分为四个层级：**容器 → Pod → 工作负载 → 节点/集群**。

### 第一层：容器 (Container) —— 应用的"进程"

这是最底层的执行单元，是实际运行应用代码的地方，比如一个运行着 Nginx 或 Python 程序的 Docker 容器。

- **职责**：它只负责运行一个特定的进程，并打包其运行所需的文件系统、依赖库等。
- **特点**：容器本身是"哑"的，它不知道自己在集群中，也没有自愈或重启的能力。它完全受控于上一层的管理者。

### 第二层：Pod —— 共享上下文的"容器组"

这是 **Kubernetes 调度和管理的最小原子单元**。它位于容器之上，是对容器的第一次封装。

- **职责**：将一个或多个紧密相关的容器（如主应用容器 + 日志收集容器）组合在一起，并为它们提供一个共享的运行环境（称为"Pod 上下文"）。

核心特性：

- **共享网络**：同 Pod 内的容器共享同一个 IP 和端口空间，可以通过 localhost 互相通信。
- **共享存储**：可以挂载共享的数据卷（Volume），方便容器间交换数据。
- **生命周期**：Pod 被视为"临时实体"，可能因节点故障或被驱逐而销毁。你通常不会直接创建单独的 Pod。

### 第三层：工作负载 (Workload) —— 管理 Pod 的"控制器"

这是你**日常部署应用时最常打交道的层级**。它不直接运行应用，而是通过定义一组规则，来替你管理并控制一个或多个 Pod 的生命周期。

| 控制器名称 | 职责与适用场景 |
| --- | --- |
| **Deployment** | **最常用**。管理**无状态应用**（如 Web 服务、API 网关）。支持滚动更新、版本回滚、故障重启和水平伸缩。 |
| **StatefulSet** | 管理**有状态应用**（如数据库、消息队列 Kafka）。为每个 Pod 分配稳定的网络标识（如 `pod-0`）和持久化存储。 |
| **DaemonSet** | 确保集群的**每个节点**上都运行一个 Pod 副本。常用于集群级的日志收集（如 Fluentd）、监控（如 Prometheus Node Exporter）、网络插件（如 Calico）。 |
| **Job / CronJob** | 管理**一次性或定时任务**。Job 负责运行一个任务直到完成（如数据迁移），CronJob 则按计划（如每天凌晨 2 点）触发 Job。 |

### 第四层：节点与集群 (Node & Cluster) —— 承载计算的"物理/虚拟资源"

这是所有计算单元的最终运行环境。

**节点 (Node)**：可以是物理机或虚拟机，是 Kubernetes 集群中的"工作机器"。它提供了 CPU、内存等计算资源。每个节点上运行着：

- **Kubelet**：负责接收指令，管理该节点上的 Pod 和容器的健康状态。
- **容器运行时**：如 Docker 或 containerd，负责实际拉取镜像和运行容器。

**集群 (Cluster)**：由一组节点（通常是多个）组成，共同形成一个统一的资源池。控制平面（Control Plane）负责统一调度，决定 Pod 应被分配到哪个节点上。

### 总结：从应用到资源的完整路径

我们可以用一个简化的工作流程来串联这四个层级：

1. 你编写一个 **Deployment**（第三层）的 YAML 文件，声明需要 3 个 Nginx 副本。
2. Kubernetes 的控制平面收到指令，根据资源情况，决定在某个**节点**（第四层）上创建 3 个 **Pod**（第二层）。
3. 每个 Pod 内部启动一个运行 Nginx 的**容器**（第一层）。
4. Deployment 控制器会持续监控这 3 个 Pod，如果其中一个崩溃了，它会自动创建一个新的来替代，从而实现**自愈**。

