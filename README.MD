# 读爷

[Hexo官方文档](https://hexo.io/zh-cn/docs/)


## 使用 docker compose 开发环境

启动开发环境
```bash
docker compose up -d
```

安装依赖
```bash
docker compose exec dev npm install
```

进入容器
```bash
docker compose exec dev bash
```

交互式新建文章
```bash
docker compose exec dev npm run newblog
```

新建文章
```bash
docker compose exec dev hexo new "新文章标题"
```

## Hexo 常用命令

启动服务器，默认访问网址为 http://localhost:4000/
```bash
hexo server
```

常用选项：
- `-p, --port` - 重设端口
- `-s, --static` - 只使用静态文件
- `-l, --log` - 启用日志