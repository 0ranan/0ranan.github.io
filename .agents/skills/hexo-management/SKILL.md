---
name: hexo-management
description: 在 Docker 容器中管理 Hexo 博客的完整生命周期操作。当用户需要预览、生成、部署或测试博客时调用。覆盖 hexo server（预览）、hexo generate（生成）、hexo deploy（部署）、hexo clean（清理）等核心操作。
---

# Hexo 博客管理

通过 Docker Compose 开发环境管理 Hexo 博客的预览、生成、部署和测试操作。所有命令均在 Docker 容器内执行，确保环境一致性。

## 前置条件

确认 Docker 开发容器已启动：

```bash
docker compose ps
```

若未启动，先拉起容器：

```bash
docker compose up -d && docker compose ps
```

容器名：`myblog-dev`，工作目录：`/app`。

### Docker 操作异常处理

**以下情况必须暂停操作并通知用户，由用户人工确认后决定下一步：**

| 异常类型 | 触发条件 | 处理方式 |
|----------|----------|----------|
| 镜像拉取失败 | `docker compose pull` 或 `up` 时出现 `pull access denied`、`not found`、网络超时 | 告知用户具体错误信息，询问是否切换镜像源或检查网络 |
| 镜像构建失败 | `docker compose build` 出现非零退出码 | 展示构建日志中的错误行，请用户检查 `Dockerfile` 或网络 |
| 容器启动失败 | `docker compose up` 后 `docker compose ps` 显示 `Exit` 或 `Restarting` | 展示 `docker compose logs --tail=30` 的日志，请用户排查 |
| 端口冲突 | 启动预览时提示 `address already in use` | 告知用户端口 4000 被占用，询问是否先停止占用进程 |
| 权限异常 | 操作报 `Permission denied` | 告知用户具体文件/目录和权限信息，询问是否调整权限 |

**规则：**
- 遇到以上任一异常，**必须停止当前操作流程**，向用户报告错误信息
- **禁止自动重试**构建、拉取、启动等 Docker 操作
- **禁止在异常状态下继续执行后续步骤**
- 等待用户确认并给出明确指令后，再继续操作

## 操作一览

| 操作 | 命令 | 说明 |
|------|------|------|
| 预览 | `hexo server` | 启动本地开发服务器，端口 4000 |
| 生成 | `hexo generate` | 生成静态文件到 `public/` 目录 |
| 部署 | `git push` | 推送至 main 分支，触发 GitHub Actions 自动部署 |
| 清理 | `hexo clean` | 清除缓存（`db.json`）和 `public/` 目录 |

---

## 1. 预览博客

### 1.1 启动预览服务器

```bash
docker compose exec -d dev npx hexo server -p 4000
```

> `-d` 让服务器在后台运行，不阻塞终端。

验证是否启动成功：

```bash
docker compose exec dev curl -s http://localhost:4000/health 2>/dev/null || curl -s -o /dev/null -w "%{http_code}" http://localhost:4000/
```

返回 200 即表示成功。

### 1.2 停止预览服务器

```bash
docker compose exec dev pkill -f "hexo server"
```

### 1.3 预览时注意事项

- 端口 4000 已通过 `compose.yaml` 映射到宿主机，可直接浏览器访问 `http://localhost:4000/`
- 修改 Markdown 源文件后 Hexo 会**自动重新生成**，刷新浏览器即可看到更新
- 若修改了 `_config.yml` 或主题文件，需重启 `hexo server` 才能生效
- 若端口被占用，先执行停止命令再重新启动

---

## 2. 生成静态文件

### 2.1 标准生成

```bash
docker compose exec dev npx hexo generate
```

生成完成后，静态文件输出到 `public/` 目录（容器内路径：`/app/public/`）。

### 2.2 清理后生成（推荐在部署前使用）

```bash
docker compose exec dev npx hexo clean && docker compose exec dev npx hexo generate
```

先清除缓存再生成，避免旧数据残留导致的问题。

### 2.3 生成时的常见错误

| 错误现象 | 可能原因 | 解决方案 |
|----------|----------|----------|
| 生成卡住不动 | 文章 Front-matter 格式错误 | 检查最近修改的文章 YAML 头是否有语法问题 |
| `<% ... %>` 渲染异常 | EJS 模板语法错误 | 检查主题 `layout/` 目录下最近修改的 `.ejs` 文件 |
| 图片 404 | 图片路径引用错误 | 确认图片路径与 `typora-root-url` 配置一致 |

---

## 3. 部署博客

### 3.1 部署方式

本项目通过 **GitHub Actions** 自动部署，配置文件为 `.github/workflows/pages.yml`。工作流为：推送 `main` 分支 → GitHub Actions 自动构建并部署到 GitHub Pages。

### 3.2 部署前检查清单

在提交并推送之前，建议按顺序执行：

```bash
# 1. 清理 + 生成，确认构建无报错
docker compose exec dev npx hexo clean && docker compose exec dev npx hexo generate

# 2. 本地预览，肉眼确认页面效果
docker compose exec -d dev npx hexo server -p 4000

# 3. 确认无误后停止预览、提交推送
docker compose exec dev pkill -f "hexo server"
git add -A
git commit -m "..."  # 遵循 .agents/rules/git-commit-message.md 规范
git push origin main
```

### 3.3 查看部署状态

推送后访问 GitHub 仓库的 Actions 页面查看构建进度：
- 仓库地址：`https://github.com/0ranan/0ranan.github.io/actions`
- 站点地址：`https://0ranan.github.io/`

### 3.4 部署失败排查

| 常见错误 | 排查方向 |
|----------|----------|
| `pages build and deployment` 失败 | 查看 Actions 日志，通常是 `hexo generate` 阶段报错 |
| 部署成功但页面无更新 | 检查浏览器缓存，或等待几分钟（GitHub Pages 有分发延迟） |
| 主题样式丢失 | 检查 `_config.yml` 中 `url` 和 `root` 配置是否正确 |

---

## 4. 测试博客

### 4.1 构建测试

验证项目能否正常生成静态文件：

```bash
docker compose exec dev npx hexo clean && docker compose exec dev npx hexo generate
```

检查生成输出中是否有 `ERROR` 或 `WARN` 信息。

验证生成的静态文件：

```bash
# 检查 public/ 目录是否生成
docker compose exec dev ls /app/public/

# 检查首页是否正常
docker compose exec dev cat /app/public/index.html | head -20
```

### 4.2 链接有效性测试

启动预览服务器后，用 curl 验证关键页面：

```bash
# 启动预览服务器
docker compose exec -d dev npx hexo server -p 4000
sleep 3

# 验证首页
curl -s -o /dev/null -w "首页: %{http_code}\n" http://localhost:4000/

# 验证标签页
curl -s -o /dev/null -w "标签: %{http_code}\n" http://localhost:4000/tags/

# 验证关于页
curl -s -o /dev/null -w "关于: %{http_code}\n" http://localhost:4000/about/

# 验证归档页
curl -s -o /dev/null -w "归档: %{http_code}\n" http://localhost:4000/archives/
```

以上四个页面应全部返回 200。

### 4.3 内容完整性测试

检查最近生成的文章页面是否正常：

```bash
# 列出最新文章的链接
curl -s http://localhost:4000/ | grep -o 'href="[^"]*"' | grep '/2026/' | head -5

# 逐个检查返回状态
curl -s -o /dev/null -w "%{http_code} %{url_effective}\n" http://localhost:4000/2026/07/10/K8S-概念笔记/
```

### 4.4 测试后清理

```bash
docker compose exec dev pkill -f "hexo server"
```

---

## 操作决策树

当用户提出需求时，按以下流程判断该执行什么操作：

```
用户说"预览" / "看看效果"
  → 执行「预览博客」：启动 hexo server，返回预览地址

用户说"生成" / "构建"
  → 执行「生成静态文件」：hexo clean && hexo generate

用户说"部署" / "发布" / "上线"
  → 执行「部署博客」完整流程：生成验证 → 确认后暂存提交推送

用户说"测试" / "检查"
  → 执行「测试博客」：构建测试 + 链接检查

用户说"清理" / "清除缓存"
  → 执行 hexo clean
```

---

## 注意事项

1. **所有 Hexo 命令必须在 Docker 容器内执行**：通过 `docker compose exec dev` 前缀运行，不要在宿主机直接运行 hexo。
2. **容器工作目录**：Docker 内项目路径为 `/app`，与宿主机通过 bind mount 同步，文件修改即时生效。
3. **权限问题**：若容器内创建了新文件，在宿主机上可能显示为 `nobody:nogroup`，需要时执行 `docker compose exec dev chown 1000:1000 <文件路径>` 修复。
4. **端口占用**：预览服务器使用 4000 端口，若冲突先执行停止命令。
5. **Git 提交规范**：提交信息必须遵循 `.agents/rules/git-commit-message.md` 中的格式。
6. **构建前清理**：部署前务必执行 `hexo clean` 避免缓存问题导致生成内容不更新。

## 项目信息

- 博客标题："安苒-博客"，作者："刘张豪"
- 主题：`hexo-theme-aircloud-master`
- 站点 URL：`https://0ranan.github.io/`
- Docker 容器名：`myblog-dev`，服务名：`dev`
- GitHub Actions 工作流：`.github/workflows/pages.yml`
- Git 提交规范：`.agents/rules/git-commit-message.md`
