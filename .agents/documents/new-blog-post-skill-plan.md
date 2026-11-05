# new-blog-post 技能实施计划

## 概述

创建一个新技能 `new-blog-post`，帮助用户在 Hexo 博客项目中创建新的文章文件。该技能自动使用 docker compose 环境（参考 `README.MD` 中的 Docker 开发环境说明），通过运行 `npm run newblog` 交互式脚本来创建文章。

## 当前状态分析

### 项目环境
- `README.MD` 记录了 Docker 开发环境的完整使用方式
- `README.MD` line 10-15: `docker compose up -d` 启动开发环境
- `README.MD` line 25-26: `docker compose exec dev npm run newblog` 交互式新建文章
- `package.json` 中 `newblog` 脚本: `tsc script/newblog.ts ; node script/newblog.js`

### 现有技能参考
- `.agents/skills/learn-shit/SKILL.md` — 唯一的现有技能，使用 YAML front-matter + Markdown body 格式

### 新建文章的两种方式（README.MD 已记录）
1. **交互式**: `docker compose exec dev npm run newblog` — 依次询问 title / tags / categories
2. **原生**: `docker compose exec dev hexo new "标题"` — 使用 scaffolds/post.md 模板

### newblog.ts 脚本工作流
1. 询问文章标题 → 生成文件名 `YYYYMMDD-标题.md`
2. 询问 tags（用 `-` 分隔）
3. 询问 categories（用 `-` 分隔）
4. 生成 front-matter（date/updated 自动填充当前时间）
5. 写入 `source/_posts/` 目录

## 提议变更

### 新建文件

#### `.agents/skills/new-blog-post/SKILL.md`

**内容要点：**
- **name**: "new-blog-post"
- **description**: 描述功能（创建 Hexo 新文章）和触发条件（用户要求创建新文章时）
- **body**: 包含以下工作流说明：
  1. **环境准备**: 首先确保 Docker 开发环境已启动（如果未启动，自动执行 `docker compose up -d`）
  2. **收集信息**: 询问用户以下信息（参考 newblog.ts 的交互流程）：
     - 文章标题（必填）
     - tags 标签（多个标签用 `-` 分隔）
     - categories 分类（多个分类用 `-` 分隔）
  3. **创建文章**: 使用 `docker compose exec dev npm run newblog` 命令创建文章，将用户输入通过管道传入
  4. **文章格式参考**: 引用 `scaffolds/post.md` 模板格式和 `_config.yml` 中的 `new_post_name` 配置
  5. **备选方式**: 如果交互式脚本有问题，可以回退到 `docker compose exec dev hexo new "标题"` + 手动编辑 front-matter

## 假设与决策

1. **默认使用 Docker 环境**: 根据 README.MD，项目使用 docker compose 作为开发环境，技能默认使用 docker compose 命令
2. **使用 npm run newblog 方式**: 用户明确选择此方式，该脚本能自动填充 date/updated/comments/typora-root-url 等字段
3. **交互式输入处理**: `npm run newblog` 脚本使用 readline 交互，AI 代理需要先收集用户输入，再通过 echo 管道传递或直接运行命令
4. **环境检测**: 技能应提示 AI 先确认 docker compose 环境是否已启动（docker compose ps），如未启动则执行 `docker compose up -d`

## 验证步骤

1. 确认 `.agents/skills/new-blog-post/` 目录已创建
2. 确认 `SKILL.md` 文件存在且格式正确（有 front-matter）
3. 确认 name 和 description 字段已正确填写
4. 确认 body 内容完整覆盖创建文章的完整工作流
