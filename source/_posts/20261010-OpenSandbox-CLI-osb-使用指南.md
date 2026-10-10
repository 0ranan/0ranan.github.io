---
title: OpenSandbox CLI (osb) 使用指南
date: 2026/10/10 12:06:16
updated: 2026/10/10 12:06:16
comments: true
tags:
  - osb
  - OpenSandbox
  - CLI
categories:
  - 工具
typora-root-url: ..
---

# OpenSandbox CLI (osb) 使用指南

`osb` 是 OpenSandbox CLI，可以从终端管理沙箱，支持执行命令、文件操作、生命周期管理、快照、模板等功能。

## 用法

```bash
osb [OPTIONS] COMMAND [ARGS]...
```

## 选项

| 选项 | 说明 |
|------|------|
| `--api-key TEXT` | 用于身份验证的 API 密钥。 |
| `--domain TEXT` | API 服务器域名（例如 localhost:8080）。 |
| `--protocol [http\|https]` | 协议（http/https）。 |
| `--request-timeout INTEGER` | 请求超时时间（秒）。 |
| `--use-server-proxy / --no-use-server-proxy` | 通过沙箱服务器代理路由 execd 和端点流量。 |
| `--config PATH` | 配置文件路径。 |
| `-v, --verbose` | 启用详细/调试输出。 |
| `--no-color` | 禁用彩色输出。 |
| `--version` | 显示版本并退出。 |
| `-h, --help` | 显示此消息并退出。 |

## 命令

| 命令 | 说明 |
|------|------|
| `command` | 在沙箱中执行命令。 |
| `config` | 管理 CLI 配置。 |
| `credential-vault` | 管理沙箱本地的 Credential Vault 状态。 |
| `devops` | 用于沙箱故障排查的实验性诊断。 |
| `diagnostics` | 由 OpenSandbox 支持的稳定沙箱诊断。 |
| `egress` | 管理沙箱的运行时出站策略。 |
| `file` | 对沙箱进行文件操作。 |
| `sandbox` | 管理沙箱生命周期。 |
| `skills` | 管理面向 AI 编码工具的 OpenSandbox 捆绑技能。 |
| `snapshot` | 管理沙箱快照。 |
| `template` | 管理沙箱模板（黄金镜像构建）。 |

## 获取帮助

在终端运行 `osb --help` 或 `osb <命令> --help` 可以查看对应的帮助信息。
