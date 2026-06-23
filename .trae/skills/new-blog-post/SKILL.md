---
name: new-blog-post
description: Creates a new Hexo blog post using the project's newblog script inside Docker. Invoke when the user asks to create, add, or write a new article/blog post.
---

# New Blog Post

Create a new blog post for this Hexo project. Always use the Docker Compose development environment as documented in `README.MD`.

## Workflow

### 1. Ensure Docker environment is running

Check if the Docker dev container is up. If not, start it:

```bash
docker compose up -d && docker compose ps
```

### 2. Gather post metadata from the user

Ask the user for the following information:

| Field | Description | Example |
|-------|-------------|---------|
| title | 文章标题，必填 | 常用skill收录 |
| tags | 标签，多个用 `-` 分隔 | AI-skill |
| categories | 分类，多个用 `-` 分隔 | 编程 |

**Rules:**
- Title is required and cannot be empty.
- Tags and categories use `-` as the separator between multiple values.

### 3. Create the post with `hexo new`

**Use `hexo new` as the primary method.** The `npm run newblog` script uses readline and does not work with piped input through `docker compose exec`.

```bash
docker compose exec dev hexo new "<标题>"
```

Example:
```bash
docker compose exec dev hexo new "常用skill收录"
```

This creates a file at `source/_posts/YYYYMMDD-<标题>.md` (inside Docker: `/app/source/_posts/YYYYMMDD-<标题>.md`).

### 4. Fix the scaffold output

The `scaffolds/post.md` template has two issues that must be corrected after creation:

| Issue | Scaffold output | Correct format |
|-------|----------------|----------------|
| Title placeholder | `# {{ title }}` | `# <实际标题>` |
| Date separator | `2026-06-12 18:43:10` | `2026/06/12 18:43:10` |
| Empty tags/categories | `tags:\ncategories:` | Populated YAML lists |

Run the following fix commands inside the Docker container (replace `<FILENAME>` with the actual generated filename):

```bash
# 1. Fix date format (dash to slash)
docker compose exec dev sed -i 's/^date: \([0-9]\{4\}\)-\([0-9]\{2\}\)-\([0-9]\{2\}\)/date: \1\/\2\/\3/' /app/source/_posts/<FILENAME>.md

# 2. Fix updated format
docker compose exec dev sed -i 's/^updated: \([0-9]\{4\}\)-\([0-9]\{2\}\)-\([0-9]\{2\}\)/updated: \1\/\2\/\3/' /app/source/_posts/<FILENAME>.md

# 3. Replace {{ title }} placeholder with actual title
docker compose exec dev sed -i 's/^# {{ title }}$/# <实际标题>/' /app/source/_posts/<FILENAME>.md

# 4. Populate tags (replace empty tags: line with YAML list)
docker compose exec dev sed -i 's/^tags:$/tags:\n  - tag1\n  - tag2/' /app/source/_posts/<FILENAME>.md

# 5. Populate categories
docker compose exec dev sed -i 's/^categories:$/categories:\n  - cat1\n  - cat2/' /app/source/_posts/<FILENAME>.md
```

After fixing, verify the file content:
```bash
docker compose exec dev cat /app/source/_posts/<FILENAME>.md
```

Alternatively, rewrite the entire file in one go using a heredoc inside Docker:

```bash
docker compose exec dev bash -c 'cat > /app/source/_posts/<FILENAME>.md << "EOF"
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

This approach is preferred when fixing multiple fields, as it avoids nested sed escaping issues.

### 4.5. Fix file permissions

After creating the file, fix its ownership so the host user can edit it directly:

```bash
docker compose exec dev chown 1000:1000 /app/source/_posts/<FILENAME>.md
```

This changes the file owner to the host user `anran` (uid=1000, gid=1000). Without this step, the file will be owned by `nobody:nogroup` (uid=65534) on the host, and VS Code (or any host-side editor) will report a permission denied error when trying to save changes.

**Important**: The `chown` command must run **after** any content modifications (sed or heredoc) are completed, since `chown` only changes ownership, not file content.

### 5. What the final file looks like

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

Key details:
- File is placed in `source/_posts/` directory.
- Filename format: `YYYYMMDD-<title>.md` (e.g., `20260613-常用skill收录.md`).
- `date` and `updated` use `YYYY/MM/DD HH:mm:ss` format (slashes, not dashes).
- `comments` is always `true`.
- `typora-root-url` is always `..`.
- The `# {{ title }}` placeholder from scaffold must be replaced with the actual title.

## Gotchas (learned from real usage)

1. **`printf | npm run newblog` does not work**: The TypeScript script uses readline which requires a real TTY. Piping input through `docker compose exec -T` silently fails — the command exits with code 0 but no file is created.
2. **Permission issue**: Files created by `docker compose exec` inside the container (as root, uid=0) appear on the host as owned by `nobody:nogroup` (uid=65534). This prevents the host user from editing them directly in VS Code or other editors. **Always run `chown 1000:1000` on new files** (see step 4.5) to fix this.
3. **Container paths**: Files inside Docker are at `/app/source/_posts/`, not the host path.
4. **Timezone**: The Docker container may use a different timezone than the host. The date in generated files reflects Docker's clock.
5. **Scaffold template bugs**: `scaffolds/post.md` uses `{{ title }}` in the body but hexo doesn't render it — it stays as literal text. This must be manually replaced.

## Project Notes

- Theme: `hexo-theme-aircloud-master`. Blog title: "安苒-博客", author: "刘张豪".
- All commands run inside Docker via `docker compose exec dev`.
- Post asset folders are enabled (`post_asset_folder: true` in `_config.yml`).
- Git commit message format must follow `.trae/rules/git-commit-message.md`.
