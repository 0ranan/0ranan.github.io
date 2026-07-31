---
title: Pi Coding Agent 的使用
date: 2026/07/31 02:57:37
updated: 2026/07/31 02:57:37
comments: true
tags:
  - pi
categories:
  - 教程
typora-root-url: ..
---

# Pi Coding Agent 的使用

## sessions 常用操作

| 命令 | 作用 |
|------|------|
| `pi -c` | 继续最近的会话 |
| `pi -r` | 浏览并选择历史会话 |
| `/resume` | 交互模式内浏览/选择历史会话 |
| `/session` | 查看当前会话信息 |
| `/tree` | 在会话树中回溯分支 |
| `/fork` | 从某个历史消息创建新会话 |
| `/clone` | 复制当前分支为新会话 |
| `/name <名称>` | 给会话命名（方便以后查找） |

在 `/resume` 选择器中还可以搜索、重命名（Ctrl+R）、删除（Ctrl+D）会话。

