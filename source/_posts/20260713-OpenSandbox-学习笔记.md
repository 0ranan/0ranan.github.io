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

