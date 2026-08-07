---
alwaysApply: true
scene: git_message
---

# Git 提交信息规范

## 格式

```
type(scope): 简短描述

详细描述本次提交的具体改动内容
```

## 规则

1. **第一行**（标题行）：`type(scope): 简短描述`
   - 使用中文描述
   - 不超过50个字符
   - 不加句号
2. **第二行**：空行
3. **第三行起**（正文）：详细描述本次改动内容
   - 使用中文描述
   - 说明做了什么、为什么做
   - 如果有多项改动，用编号列表列出

## type 类型

| type | 说明 |
|------|------|
| ci | CI/CD 构建系统及其脚本变化 |
| docs | 文档内容变化 |
| feat | 新功能 |
| fix | Bug 修复 |
| build | 基础构建系统或依赖库的变化 |
| chore | 其他不涉及源码或测试的变动 |
| style | 格式变动（不影响代码运行） |
| refactor | 重构（既不是新增功能，也不是修复Bug） |
| perf | 性能优化 |
| revert | 代码回滚 |

## scope 范围

scope 为可选字段，表示影响范围，如 `github actions`、`hexo`、`theme`、`docker` 等。

## 示例

```
ci(github actions): 添加github pages部署工作流

新增.github/workflows/pages.yml配置文件，实现main分支推送时自动构建并部署项目到GitHub Pages，使用Node.js20环境完成依赖安装、项目构建和页面部署流程
```

```
fix(theme): 修复标签页跳转链接错误的问题

将原有的硬编码标签链接替换为使用hexo的url_for方法生成的正确链接
```

```
docs: 调整博客文章的标签和分类信息

1. 为Git本地仓库操作文章新增编程、git标签
2. 移除Git常用命令文章的持续更新标签
```

```
build(hexo): 优化博客文章创建与配置

1. 调整文章文件名格式为年月日-标题
2. 新增updated、categories、typora-root-url等文章frontmatter字段
3. 添加npm run newblog和原生hexo新建文章的文档说明
4. 修改日期显示格式为YYYY/MM/DD
```

