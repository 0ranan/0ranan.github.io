---
title: WireGuard 家庭组网笔记
date: 2026/09/29 22:18:00
updated: 2026/09/29 22:18:00
comments: true
tags:
  - wireguard
  - vpn
  - 组网
  - 内网穿透
categories:
  - 笔记
typora-root-url: ..
---

# WireGuard 家庭组网笔记

目标：人在外网（手机流量、外地 WiFi、云服务器），能访问家庭内网（群晖 NAS、VM 上的服务）。方案选 WireGuard + wg-easy，部署在家庭内一台 Debian VM 上。本文记录完整搭建过程，重点在两层 NAT 家宽环境下端口转发的排查方法。

## 网络环境

家庭宽带为电信，动态公网 IP，80/443 端口被运营商封锁（未备案），且是两层 NAT：

| 层级 | 设备 | 地址 | 说明 |
|:---|:---|:---|:---|
| 公网 | 电信出口 | 动态公网 IP | 80/443 入站被封 |
| 第一层 | 光猫 | 192.168.0.1 | 路由模式，PPPoE 由光猫拨号 |
| 第二层 | TP-Link 路由器 | 192.168.1.1 | WAN 口拿光猫下发的 192.168.0.x |
| 主机 | Debian VM | 192.168.1.6 | wg-easy 部署在这里 |
| NAS | 群晖 | 192.168.1.53 | 最终要访问的目标 |

> 两层 NAT 意味着：任何入站端口转发都要配两次，少一层都不通。

## 方案选型

| 方案 | 优点 | 缺点 | 结论 |
|:---|:---|:---|:---|
| 商业内网穿透（花生壳等） | 零配置 | 限速、收费、端口数受限 | 否 |
| frp + 云服务器 | 稳定 | 流量全绕云服务器，带宽受云上行限制 | 备用 |
| ZeroTier / Tailscale | 零端口转发 | 依赖第三方协调服务器 | 备用 |
| WireGuard 直连 | 性能最好、依赖最少 | 需要公网 IP + 端口转发 | 选它 |

家宽有动态公网 IP（配合 DDNS 解析域名），WireGuard 走 UDP，被运营商照顾的概率远低于 TCP，直连是性能最优解。

## 部署 wg-easy

### 1. 环境检查

WireGuard 需要内核模块支持，先确认 VM 内核：

```bash
# 场景：确认内核是否原生支持 WireGuard（需要 >= 5.6）
uname -r
# 输出 6.12.x，原生支持，无需 dkms
```

群晖真机内核是 4.4，跑不了 WireGuard，所以选择 VM。模块开机自启：

```bash
# 场景：写入模块加载清单，重启后自动加载 wireguard
echo "wireguard" | sudo tee /etc/modules-load.d/wireguard.conf
```

### 2. docker compose

```yaml
# 场景：在家庭 VM（192.168.1.6）上部署 wg-easy
# 格式：WG_HOST 填 DDNS 域名，客户端配置里的 Endpoint 就是它
services:
  wg-easy:
    image: ghcr.io/wg-easy/wg-easy:latest
    container_name: wg-easy
    environment:
      WG_HOST: zhyw.xinmiao.net.cn        # 客户端连接的公网地址
      WG_DEFAULT_ADDRESS: 10.8.0.x        # 隧道 IP 分配池
      WG_DEFAULT_DNS: 223.5.5.5           # 阿里公共 DNS
      LANG: zh
    volumes:
      - ./wireguard:/etc/wireguard
    ports:
      - "51820:51820/udp"                 # WireGuard 数据面
      - "51821:51821/tcp"                 # Web 管理面板（仅内网使用）
    cap_add:
      - NET_ADMIN
      - SYS_MODULE
    sysctls:
      net.ipv4.ip_forward: 1
      net.ipv4.conf.all.src_valid_mark: 1
    restart: unless-stopped
```

```bash
# 场景：启动并确认容器健康
docker compose up -d
docker ps
# 输出 wg-easy (healthy) 即成功
```

> 管理面板（51821）不做公网转发，只在内网访问。人在外网想管理面板，先连 WG 再访问 10.8.0.1:51821，或者走 SSH 隧道转发。

### 3. 初始化

浏览器打开 `http://192.168.1.6:51821`，首次进入设置管理员密码。之后点"新建客户端"生成配置，手机扫二维码即可导入。

## 打通公网：两层端口转发

全程最容易翻车的环节。两层 NAT，两层都要配：

| 设备 | 规则 | 方向 |
|:---|:---|:---|
| 光猫 192.168.0.1 | UDP 51820 → 路由器 WAN IP（192.168.0.x） | 公网 → 第二层 |
| 路由器 192.168.1.1 | UDP 51820 → 192.168.1.6 | 第二层 → VM |

> WireGuard 是 UDP 协议。光猫转发规则里协议默认通常是 TCP，改成 UDP 后要确认保存，并检查规则处于启用状态。

### 验证：别信 TCP 探测，用 tcpdump

这次排查最大的收获：**部分电信网络对入站 TCP 会全端口 SYN-ACK 应答**，导致 nc、端口扫描工具对任何端口都显示"已连接"——TCP connect 探测法在这种网络下完全失效。

正确姿势是在目标机上抓包实测：

```bash
# 场景：在 VM（192.168.1.6）上抓包，验证外网 UDP 包是否到达
# 格式：sudo tcpdump -ni any udp port <端口> -c <包数> -l
sudo tcpdump -ni any udp port 51820 -c 5 -l
```

```bash
# 场景：在外网任意机器上向公网 IP 发 UDP 探测包
# 格式：python3 直接向 公网IP:端口 发 UDP 数据报
python3 -c "import socket; \
s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM); \
[s.sendto(b'test',('你的公网IP',51820)) for _ in range(5)]"
```

tcpdump 有输出 = 转发链路通；零输出 = 包死在某一层 NAT，逐级排查。

### 排查记录

| 现象 | 排查手段 | 根因 |
|:---|:---|:---|
| 手机连 WG 无网络 | 外网发 UDP 探测包，VM tcpdump 零命中 | 两层 NAT 只配置了一层 |
| 补完两层后仍不通 | 再次探测，依然零命中 | 端口手误配成 52820（此前 TCP 对照实验用过的号），与正式端口 51820 不一致 |
| 改对端口后立即通 | tcpdump 看到包一路进入 docker 容器（172.20.0.2:51820） | — |

> 教训一：两层 NAT 排查要抱着"包死在哪一层"的思路，每改一处就用 tcpdump 验证一次，不要一次改两处。
> 教训二：调试用的实验端口和正式端口分开记录，改配置时核对两遍数字。

## 内网互通的原理

连上 WG 后客户端访问内网（192.168.1.x）为什么直接就能通？答案在 wg-easy 的 MASQUERADE：

1. 客户端发包 `10.8.0.x → 192.168.1.53`，匹配 AllowedIPs 进入隧道
2. VM 解封装后转发，包出容器时源地址被改写成 192.168.1.6
3. 群晖收到的是"邻居 VM"的请求，正常应答
4. 回程经 VM 的连接跟踪还原地址，加密送回客户端

于是不同视角下身份不同：

| 观察者 | 看到的你 |
|:---|:---|
| 自己 | 10.8.0.x（隧道 IP） |
| 内网设备（群晖日志） | 192.168.1.6（VM） |
| 外网网站（全隧道模式） | 家宽公网 IP |

> 代价：内网日志分不清 VPN 访客是谁；内网设备也无法主动访问 10.8.0.x。

### 可选：静态路由打通反向访问

在路由器上加一条静态路由：`10.8.0.0/24 → 192.168.1.6`。意义是告诉路由器"隧道网段住在 VM"，之后内网设备可以主动访问隧道 IP，且隧道端看到的是真实来源 IP。

| 场景 | 不加静态路由 | 加静态路由 |
|:---|:---|:---|
| VPN 客户端访问内网 | 通（NAT 兜底） | 通 |
| 内网主动访问隧道 IP | 不通 | 通 |
| 隧道端看到的来源 | 全是 192.168.1.6 | 真实内网 IP |

## 外网服务器接入

除手机外，还可以把云服务器、其他场所的设备接进家里内网。客户端配置分两类：

| 设备类型 | AllowedIPs | 说明 |
|:---|:---|:---|
| 手机/电脑（想借家宽出口） | 0.0.0.0/0 | 全隧道，所有流量绕家里一圈 |
| 服务器（只想互相访问） | 10.8.0.0/24, 192.168.1.0/24 | 半隧道，不劫持公网出口 |

服务器配置模板：

```ini
# 场景：云服务器接入家庭内网，只路由内网流量，不接管公网出口
# 格式：wg-quick 标准配置，保存为 /etc/wireguard/wg0.conf
[Interface]
PrivateKey = <客户端私钥>
Address = 10.8.0.4/24

[Peer]
PublicKey = <服务端公钥>
PresharedKey = <预共享密钥>
AllowedIPs = 10.8.0.0/24, 192.168.1.0/24
PersistentKeepalive = 25
Endpoint = zhyw.xinmiao.net.cn:51820
```

```bash
# 场景：启用隧道并设置开机自启
sudo systemctl enable --now wg-quick@wg0

# 验证：隧道通断
sudo wg show
ping 10.8.0.1
ping 192.168.1.53
```

> PersistentKeepalive = 25 表示每 25 秒发一次保活包，维持两层 NAT 的会话表，保证家里能随时反向找到它。不需要反向访问可以去掉。

## 踩坑汇总

| 坑 | 现象 | 解法 |
|:---|:---|:---|
| 两层 NAT 只转发一层 | 外网 UDP 探测零到达 | 光猫、路由器各配一次 UDP 51820 |
| TCP 探测假阳性 | nc 显示 connected 但实际不通 | 改用 tcpdump 实测 |
| 光猫协议未切 UDP | 规则看着对但包不到 | 检查协议字段并启用规则 |
| 实验/正式端口混淆 | 改完依旧不通 | 核对端口数字 |
| 管理面板暴露公网 | 被扫描器盯上 | 51821 不做公网转发 |

## 参考

- wg-easy：https://github.com/wg-easy/wg-easy
- WireGuard：https://www.wireguard.com
