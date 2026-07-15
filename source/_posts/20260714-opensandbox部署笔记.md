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

## 在kind部署

```
kubectl create namespace opensandbox
```

### 安装环境

```bash
# 安装Helm
curl https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-3 | bash

# 安装kubectl
curl -LO "https://dl.k8s.io/release/$(curl -L -s https://dl.k8s.io/release/stable.txt)/bin/linux/amd64/kubectl"
sudo install -o root -g root -m 0755 kubectl /usr/local/bin/kubectl

# 安装kind
[ $(uname -m) = x86_64 ] && curl -Lo ./kind https://kind.sigs.k8s.io/dl/v0.32.0/kind-linux-amd64
chmod +x ./kind
sudo mv ./kind /usr/local/bin/kind
```

### 构建项目本地镜像

```bash
# 1. 克隆项目（如果已经克隆过可以跳过）
git clone https://github.com/alibaba/OpenSandbox.git
# 2. 配置环境变量
cd OpenSandbox
REPO_ROOT=$(pwd)


# controller（operator）
make docker-build CONTROLLER_IMG=opensandbox/controller:dev
# task-executor（沙箱内 HTTP 执行代理）
make docker-build-task-executor TASK_EXECUTOR_IMG=opensandbox/task-executor:dev
# image-committer（pause/resume 快照，可选）
make docker-build-image-committer IMAGE_COMMITTER_IMG=opensandbox/image-committer:dev

# ---------- server ----------
docker build -t opensandbox/server:dev -f "$REPO_ROOT/server/Dockerfile" "$REPO_ROOT/server/"

# ---------- 运行时组件 ----------
# 以下必须在仓库根目录执行
cd "$REPO_ROOT"
# execd（沙箱内守护进程，3阶段构建：go + bwrap + alpine）
docker build -t opensandbox/execd:dev -f components/execd/Dockerfile .
# egress（网络出口策略 sidecar，含 iptables/nftables/mitmproxy）
docker build -t opensandbox/egress:dev -f components/egress/Dockerfile .
# ingress（网关路由，纯 Go 二进制 + alpine）
docker build -t opensandbox/ingress:dev -f components/ingress/Dockerfile .
```

### 将本地镜像加载到 Kind 集群

```bash
for img in \
  opensandbox/controller:dev \
  opensandbox/task-executor:dev \
  opensandbox/image-committer:dev \
  opensandbox/server:dev \
  opensandbox/execd:dev \
  opensandbox/egress:dev \
  opensandbox/ingress:dev; do
  kind load docker-image "$img" --name desktop
done
```

#### 验证
```bash
docker exec desktop-control-plane crictl images | grep opensandbox
```

### 利用Helm部署，使用本地镜像

#### 构建 Helm 依赖

```bash
cd "$REPO_ROOT/kubernetes/charts"
helm dependency build opensandbox
```

#### 创建本地 values 文件

```bash
cat > "$REPO_ROOT/kubernetes/charts/my-local-values.yaml" << 'EOF'
opensandbox-controller:
  controller:
    image:
      repository: opensandbox/controller
      tag: dev
      pullPolicy: IfNotPresent
    taskExecutorImage: opensandbox/task-executor:dev
    replicaCount: 1
    logLevel: debug
    snapshot:
      imageCommitterImage: opensandbox/image-committer:dev
      commitJobTimeout: 10m
      registry: ""
      registryInsecure: false
      snapshotPushSecret: ""
      resumePullSecret: ""

opensandbox-server:
  server:
    image:
      repository: opensandbox/server
      tag: dev
      pullPolicy: IfNotPresent
    replicaCount: 1
    gateway:
      enabled: false
    resources:
      limits:
        cpu: "1"
        memory: 4Gi
      requests:
        cpu: "500m"
        memory: 2Gi

  configToml: |
    [server]
    host = "0.0.0.0"
    port = 80
    api_key = "my_api_key"

    [log]
    level = "DEBUG"

    [runtime]
    type = "kubernetes"
    execd_image = "opensandbox/execd:dev"

    [kubernetes]
    kubeconfig_path = ""
    namespace = "opensandbox"
    informer_enabled = true
    informer_resync_seconds = 300
    workload_provider = "batchsandbox"
    batchsandbox_template_file = "/etc/opensandbox/example.batchsandbox-template.yaml"

    [egress]
    image = "opensandbox/egress:dev"
    mode = "dns+nft"
EOF
```

#### 部署

```bash
helm install opensandbox ./opensandbox \
  -f my-local-values.yaml \
  --namespace opensandbox-system \
  --create-namespace
```

#### 验证

```bash
# 检查 Pod 状态
kubectl get pods -n opensandbox-system
# 检查 CRD
kubectl get crd | grep opensandbox
# 查看 controller 日志
kubectl logs -n opensandbox-system -l control-plane=controller-manager -f
# 查看 server 日志
kubectl logs -n opensandbox-system -l app.kubernetes.io/name=opensandbox-server -f
```

### 创建测试资源

#### Pool 资源说明

Pool 是 OpenSandbox 的自定义资源（CRD），用于管理一组可复用的 Pod 池，供上层 BatchSandbox 按需分配。

**模板（template）**：定义了 Pool 中每个 Pod 的规格。Controller 会以此为模板创建 Pod，每个 Pod 是一个独立的沙箱实例。示例中指定的是跑 busybox 镜像，执行 `sleep infinity` 让容器持续运行。

**容量规格（capacitySpec）**：四个字段约束了 Pool 的自动扩缩行为：

| 字段 | 含义 |
| --- | --- |
| poolMin | Pool 中 Pod 总数的下限，Controller 确保至少有这么多 Pod 存在 |
| poolMax | Pool 中 Pod 总数的上限，Controller 不会创建超过此数量的 Pod |
| bufferMin | 空闲（未分配给 BatchSandbox 的）Pod 数量下限，低于此值 Controller 会自动补充 |
| bufferMax | 空闲 Pod 数量上限，超过此值 Controller 会回收多余的空闲 Pod |

四者的约束关系：`bufferMin ≤ bufferMax ≤ poolMax`，且 `poolMin ≤ poolMax`。

**工作流程**：

1. Controller 观测到 Pool 资源后，按 poolMin 创建初始 Pod
2. 当 BatchSandbox 引用该 Pool 并请求沙箱时，Controller 从空闲 Pod 中分配
3. 分配后空闲 Pod 数量下降，若低于 bufferMin，Controller 自动创建新 Pod 补充
4. Pod 被释放回 Pool 后，若空闲数超过 bufferMax，Controller 自动删除多余 Pod
5. 总 Pod 数始终受 poolMin 和 poolMax 约束

Pool 的核心价值在于**预热**：Pod 的创建和启动需要时间，通过维持一定数量的空闲 Pod，BatchSandbox 请求沙箱时可以直接分配已就绪的 Pod，避免冷启动延迟。

#### 创建 Pool 和 BatchSandbox

```bash
# 创建命名空间
kubectl create namespace opensandbox

# 创建 Pool（预热资源池）
kubectl apply -f - << 'EOF'
apiVersion: sandbox.opensandbox.io/v1alpha1
kind: Pool
metadata:
  name: test-pool
  namespace: opensandbox
spec:
  template:
    spec:
      containers:
      - name: sandbox
        image: busybox:latest
        command: ["sleep", "infinity"]
  capacitySpec:
    bufferMax: 5
    bufferMin: 1
    poolMax: 10
    poolMin: 2
EOF

# 创建 BatchSandbox
kubectl apply -f - << 'EOF'
apiVersion: sandbox.opensandbox.io/v1alpha1
kind: BatchSandbox
metadata:
  name: test-batch
  namespace: opensandbox
spec:
  replicas: 2
  poolRef: test-pool
EOF

# 查看状态
kubectl get pools -n opensandbox
kubectl get batchsandboxes -n opensandbox
kubectl get pods -n opensandbox
```

#### 调试完成清理内容

```bash
# 1. 先删 BatchSandbox（它引用了 Pool）
kubectl delete batchsandbox test-batch -n opensandbox

# 2. 再删 Pool（它管理着 Pod）
kubectl delete pool test-pool -n opensandbox

# 3. Pod 会自动消失，不用手动删
```

### 卸载测试资源

```bash
# 卸载 Helm release
helm uninstall opensandbox -n opensandbox-system

# 删除 CRD（注意：会级联删除所有相关资源）
kubectl delete crd batchsandboxes.sandbox.opensandbox.io
kubectl delete crd pools.sandbox.opensandbox.io
kubectl delete crd sandboxsnapshots.sandbox.opensandbox.io

# 删除命名空间
kubectl delete namespace opensandbox-system
kubectl delete namespace opensandbox

# 删除 Kind 集群
kind delete cluster --name opensandbox
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



