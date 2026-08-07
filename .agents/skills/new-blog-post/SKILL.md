---
name: new-blog-post
description: 使用项目的 newblog 脚本在 Docker 中创建新的 Hexo 博客文章。当用户要求创建、添加或撰写新文章/博客时调用。
---

# 新建博客文章

为此 Hexo 项目创建新的博客文章。始终使用 `README.MD` 中记录的 Docker Compose 开发环境。

## 工作流程

### 1. 确保 Docker 环境正在运行

检查 Docker 开发容器是否已启动。如果未启动，执行以下命令：

```bash
docker compose up -d && docker compose ps
```

### 2. 从用户处获取文章元数据

向用户询问以下信息：

| 字段 | 描述 | 示例 |
|-------|-------------|---------|
| title | 文章标题，必填 | 常用skill收录 |
| tags | 标签，多个用 `-` 分隔 | AI-skill |
| categories | 分类，多个用 `-` 分隔 | 编程 |

**规则：**
- 标题是必填的，不能为空。
- 标签和分类使用 `-` 作为多个值之间的分隔符。

### 3. 使用 `hexo new` 创建文章

**优先使用 `hexo new` 作为主要方式。** `npm run newblog` 脚本使用了 readline，无法通过 `docker compose exec` 管道输入。

```bash
docker compose exec dev hexo new "<标题>"
```

示例：
```bash
docker compose exec dev hexo new "常用skill收录"
```

这会在 `source/_posts/YYYYMMDD-<标题>.md` 处创建文件（在 Docker 内部为：`/app/source/_posts/YYYYMMDD-<标题>.md`）。

### 4. 修复脚手架输出

`scaffolds/post.md` 模板有两个问题需要在创建后修正：

| 问题 | 脚手架输出 | 正确格式 |
|-------|----------------|----------------|
| 标题占位符 | `# {{ title }}` | `# <实际标题>` |
| 日期分隔符 | `2026-06-12 18:43:10` | `2026/06/12 18:43:10` |
| 空标签/分类 | `tags:\ncategories:` | 填充好的 YAML 列表 |

在 Docker 容器内运行以下修复命令（将 `<文件名>` 替换为实际生成的文件名）：

```bash
# 1. 修复日期格式（短横线改为斜线）
docker compose exec dev sed -i 's/^date: \([0-9]\{4\}\)-\([0-9]\{2\}\)-\([0-9]\{2\}\)/date: \1\/\2\/\3/' /app/source/_posts/<文件名>.md

# 2. 修复 updated 格式
docker compose exec dev sed -i 's/^updated: \([0-9]\{4\}\)-\([0-9]\{2\}\)-\([0-9]\{2\}\)/updated: \1\/\2\/\3/' /app/source/_posts/<文件名>.md

# 3. 将 {{ title }} 占位符替换为实际标题
docker compose exec dev sed -i 's/^# {{ title }}$/# <实际标题>/' /app/source/_posts/<文件名>.md

# 4. 填充标签（将空 tags: 行替换为 YAML 列表）
docker compose exec dev sed -i 's/^tags:$/tags:\n  - tag1\n  - tag2/' /app/source/_posts/<文件名>.md

# 5. 填充分类
docker compose exec dev sed -i 's/^categories:$/categories:\n  - cat1\n  - cat2/' /app/source/_posts/<文件名>.md
```

修复后，验证文件内容：
```bash
docker compose exec dev cat /app/source/_posts/<文件名>.md
```

或者，在 Docker 内使用 heredoc 一次性重写整个文件：

```bash
docker compose exec dev bash -c 'cat > /app/source/_posts/<文件名>.md << "EOF"
---
title: <标题>
date: <YYYY/MM/DD HH:mm:ss>
updated: <YYYY/MM/DD HH:mm:ss>
comments: true
tags:
  - <tag1>
  - <tag2>
categories:
  - <cat1>
typora-root-url: ..
---

# <标题>

EOF'
```

当需要修复多个字段时，推荐使用此方式，因为它避免了嵌套 sed 转义的问题。

### 4.5. 修复文件权限

创建文件后，修复其所有权以便宿主机用户可以直接编辑：

```bash
docker compose exec dev chown 1000:1000 /app/source/_posts/<文件名>.md
```

这会将文件所有者更改为宿主机用户 `anran`（uid=1000，gid=1000）。如果不执行此步骤，文件在宿主机上将被 `nobody:nogroup`（uid=65534）拥有，VS Code（或任何宿主机端的编辑器）在尝试保存更改时会报告权限被拒绝的错误。

**重要提示**：`chown` 命令必须在所有内容修改（sed 或 heredoc）完成之后运行，因为 `chown` 只能更改所有权，不能更改文件内容。

### 5. 最终文件的样式

```markdown
---
title: 常用skill收录
date: 2026/06/13 18:43:10
updated: 2026/06/13 18:43:10
comments: true
tags:
  - AI
  - skill
categories:
  - 编程
typora-root-url: ..
---

# 常用skill收录
```

关键细节：
- 文件放置在 `source/_posts/` 目录中。
- 文件名格式：`YYYYMMDD-<标题>.md`（例如：`20260613-常用skill收录.md`）。
- `date` 和 `updated` 使用 `YYYY/MM/DD HH:mm:ss` 格式（斜线，而非短横线）。
- `comments` 始终为 `true`。
- `typora-root-url` 始终为 `..`。
- 脚手架中的 `# {{ title }}` 占位符必须替换为实际标题。

## 注意事项（实际使用中总结的经验）

1. **`printf | npm run newblog` 不可用**：TypeScript 脚本使用了 readline，需要真实的 TTY。通过 `docker compose exec -T` 管道输入会静默失败——命令以退出码 0 结束，但不会创建文件。
2. **权限问题**：通过 `docker compose exec` 在容器内创建的文件（以 root 身份，uid=0）在宿主机上显示为由 `nobody:nogroup`（uid=65534）拥有。这会阻止宿主机用户在 VS Code 或其他编辑器中直接编辑它们。**创建新文件后务必运行 `chown 1000:1000`**（参见步骤 4.5）来解决此问题。
3. **容器路径**：文件在 Docker 内位于 `/app/source/_posts/`，而非宿主机路径。
4. **时区**：Docker 容器可能使用与宿主机不同的时区。生成文件中的日期反映了 Docker 的时钟。
5. **脚手架模板 Bug**：`scaffolds/post.md` 在正文中使用了 `{{ title }}`，但 hexo 不会渲染它——它会保持为字面文本。必须手动替换。

## 项目备注

- 主题：`hexo-theme-aircloud-master`。博客标题："安苒-博客"，作者："刘张豪"。
- 所有命令通过 `docker compose exec dev` 在 Docker 内运行。
- 文章资源文件夹已启用（`_config.yml` 中 `post_asset_folder: true`）。
- Git 提交信息格式必须遵循 `.trae/rules/git-commit-message.md`。
