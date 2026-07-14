---
title: opensandbox部署笔记
date: 2026/07/14 00:00:00
updated: 2026/07/14 00:00:00
comments: true
tags:
  - opensandbox
  - k8s
  - 运维
categories:
  - 运维
typora-root-url: ..
---

# opensandbox部署笔记

## 在Kubernetes部署

0. 本地模拟创建集群
```
kind create cluster
```

1. 安装Helm

```bash
curl https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-3 | bash
```

2. 构建项目本地镜像
```bash
# 1. 克隆项目（如果已经克隆过可以跳过）
git clone https://github.com/alibaba/OpenSandbox.git

# 2. 进入项目目录
cd OpenSandbox/kubernetes

# 3. 构建控制器镜像，并给它打一个本地标签
make docker-build CONTROLLER_IMG=opensandbox-controller:local

# 4. 构建任务执行器镜像
make docker-build-task-executor TASK_EXECUTOR_IMG=opensandbox-task-executor:local
```

3. 将本地镜像加载到 Kind 集群

```bash
kind load docker-image opensandbox-controller:local
# 如果构建了任务执行器，也需要加载
kind load docker-image opensandbox-task-executor:local
```


4. 利用Helm部署，使用本地镜像

Helm Chart 包，只做应用的部署模板

```bash

# 使用本地镜像重新安装
helm install opensandbox \
  https://github.com/opensandbox-group/OpenSandbox/releases/download/helm/opensandbox-controller/0.2.0/opensandbox-controller-0.2.0.tgz \
  --namespace opensandbox-system \
  --create-namespace \
  --set controller.image.repository=opensandbox-controller \
  --set controller.image.tag=local \
  --set controller.image.pullPolicy=IfNotPresent
```

5. 验证部署

```bash
kubectl get pods -n opensandbox-system
```

## 使用python 服务端

1. 创建python 环境并且安装依赖

```bash
# 创建环境
python3 -m venv .venv
# 安装依赖
.venv/bin/pip install opensandbox-server
# 启动环境
source ./.venv/bin/activate
```

2. 创建配置文件
```bash

opensandbox-server init-config ~/.sandbox.toml --example k8s

```


3. 修改配置文件
在.sandbox.toml 添加 api_key
```
[server]
host = "0.0.0.0"
port = 8080
api_key = "test-key-123"
```

api_key 用于 API 认证 ，防止未授权的请求访问你的 OpenSandbox 服务器。

**工作机制**
  - 服务器收到请求时，检查 HTTP 头中的 OPEN-SANDBOX-API-KEY 是否匹配配置的 api_key
  - 匹配才放行，否则返回 401
  - /health 、 /docs 、 /redoc 这几个端点不需要认证，始终开放

**谁需要传**
  - SDK、CLI、MCP 等所有客户端访问服务器时都需要带上这个 key：


4. opensandbox-server
```bash
# 启动前清除代理
unset ALL_PROXY all_proxy HTTPS_PROXY https_proxy HTTP_PROXY http_proxy
# 启动opensandbox-server
opensandbox-server --config /home/anran/code/OpenSandBox-TestSDK/.sandbox.toml
```

## 使用python客户端进行调试

1. 安装客户端sdk

```bash
pip install opensandbox
```

2. 在集群中创建命名空间

```bash
kubectl create namespace opensandbox
```

3. 准备测试用的镜像
```bash
# 本地拉取镜像
docker pull python:3.11
# 查询集群名称
kind get clusters
#保存docker镜像到本地
docker save python:3.11 -o /tmp/python-3.11.tar

# 取消Kind的代理变量
docker exec -it kind-control-plane sh -c "unset http_proxy https_proxy HTTP_PROXY HTTPS_PROXY && crictl pull python:3.11"
# 将镜像导入集群
docker cp /tmp/python-3.11.tar kind-control-plane:/tmp/
docker exec -it kind-control-plane crictl load /tmp/python-3.11.tar
# 导入到 k8s.io 命名空间
docker exec -it kind-control-plane ctr -n k8s.io images import /tmp/python-3.11.tar

# 验证镜像
docker exec kind-control-plane crictl images | grep python

```

编写sdk测试文件

```python
# 导入 asyncio：Python 标准库异步 I/O 框架，用于运行 async 函数
import asyncio
# 导入 timedelta：表示两个 datetime 之间的差值，用于设定超时时间
from datetime import timedelta
# Sandbox 类：管理一个沙箱实例的完整生命周期（创建、执行命令、销毁）
from opensandbox import Sandbox
# ConnectionConfig：封装与 OpenSandbox 服务端的连接参数
from opensandbox.config.connection import ConnectionConfig

# 定义异步主函数
async def main():
    # 构建与 OpenSandbox 服务端的连接配置
    config = ConnectionConfig(
        # 服务端域名/地址，本机 8080 端口
        domain="localhost:8080",
        # 通信协议 http 或 https
        protocol="http",
        # API 认证密钥，与服务端 [server] api_key 对应；未配置时可省略
        api_key="test-key-123",  # 如果配置了 api_key 则需要
    )

    # 异步创建沙箱实例：向服务端请求分配一个沙箱容器，等待就绪
    sandbox = await Sandbox.create(
        # 指定沙箱模板/镜像，这里是 Python 3.11 运行环境
        "python:3.11",
        # 传入上面构建的连接配置
        connection_config=config,
        # 沙箱最大存活时间，超时后服务端自动清理
        timeout=timedelta(minutes=5),
    )

    # async with 上下文管理器：进入时连接沙箱，退出时自动清理资源
    async with sandbox:
        # 在沙箱内远程执行 shell 命令，返回命令执行结果
        result = await sandbox.commands.run("echo 'Hello from Kind!'")
        # 从结果中提取第一条 stdout 日志的文本内容并打印
        print(result.logs.stdout[0].text)

        # 显式终止沙箱（async with 退出时也会自动调用 kill）
        await sandbox.kill()

# 程序入口：以同步方式运行异步 main 函数
asyncio.run(main())
```



