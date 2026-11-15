---
title: 使用 systemd 配置 opensandbox 开机自动启动
date: 2026/10/22 16:18:20
updated: 2026/10/22 16:31:08
comments: true
tags:
  - OpenSandbox
  - systemd
  - Docker
categories:
  - 运维
typora-root-url: ..
---

# 使用 systemd 配置 opensandbox 开机自动启动

## 1. unit 文件内容

`/etc/systemd/system/opensandbox.service`

```ini
[Unit]
Description=OpenSandbox Server (uvx)
Documentation=https://github.com/opensandbox-group/OpenSandbox
After=network-online.target docker.service
Wants=network-online.target
Requires=docker.service

[Service]
Type=simple
User=root
Group=root
WorkingDirectory=/root
Environment=HOME=/root
Environment=PATH=/root/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
# 如果 .sandbox.toml 里不启用 api_key，就必须打开下面这行，
# 否则非交互启动会卡在 "type YES" 的确认而失败
#Environment=OPENSANDBOX_INSECURE_SERVER=YES
ExecStart=/root/.local/bin/uvx opensandbox-server
Restart=on-failure
RestartSec=5s
TimeoutStopSec=30s
LimitNOFILE=65535
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
```

## 2. 写入命令

```bash
cat > /etc/systemd/system/opensandbox.service <<'EOF'
[Unit]
Description=OpenSandbox Server (uvx)
Documentation=https://github.com/opensandbox-group/OpenSandbox
After=network-online.target docker.service
Wants=network-online.target
Requires=docker.service

[Service]
Type=simple
User=root
WorkingDirectory=/root
Environment=HOME=/root
Environment=PATH=/root/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
ExecStart=/root/.local/bin/uvx opensandbox-server
Restart=on-failure
RestartSec=5s
TimeoutStopSec=30s
LimitNOFILE=65535
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
EOF
```

## 3. 启动前必须先改 /root/.sandbox.toml

当前是 `host = "127.0.0.1"` / `port = 8080`，和容器访问的 `172.20.0.1:8090` 对不上，改成：

```toml
[server]
host = "0.0.0.0"
port = 8090
max_sandbox_timeout_seconds = 86400

api_key = "<你的 API Key>"
```

启用 api_key 后就不需要 `OPENSANDBOX_INSECURE_SERVER=YES`；两者选一即可，推荐用 api_key，和后端配置一致。

## 4. 启用与查看

```bash
systemctl daemon-reload
systemctl enable --now opensandbox
systemctl status opensandbox --no-pager
journalctl -u opensandbox -f
```
