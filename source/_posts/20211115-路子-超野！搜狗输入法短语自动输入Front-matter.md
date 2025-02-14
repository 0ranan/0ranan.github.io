---
title: 搜狗输入法短语自动输入Front-matter
date: 2021/11/15 20:30:22
updated: 2021/11/15 20:30:22
comments: true
tags:
  - 办公
  - 骚操作
  - hexo
categories:
  - 办公
typora-root-url: ..
---


# 搜狗输入法短语自动输入Front-matter

> 每次打Markdown到自己的博客里去都要自己手输Front-matter，用惯了自动化工具的我表示很不爽！
>
> 那有什么办法可以让电脑自动输入Front-matter呢？
>
> 好像……思前想后，就搜狗输入法的自定义短语可以做到。

1. ## 打开更多设置

   ![image-20211115210751127](/images/20211115-路子-超野！搜狗输入法短语自动输入Front-matter/image-20211115210751127.png)

2. ## 自定义短语设置

   ![image-20211115211325488](/images/20211115-路子-超野！搜狗输入法短语自动输入Front-matter/image-20211115211325488.png)

3. ## 直接编辑配置文件

   ![image-20211115211357460](/images/20211115-路子-超野！搜狗输入法短语自动输入Front-matter/image-20211115211357460.png)

4. 在最后一行假如如下配置

   ![image-20211115211443802](/images/20211115-路子-超野！搜狗输入法短语自动输入Front-matter/image-20211115211443802.png)

   


```搜狗输入法自定义短语
blogfm,1=
#---
title: 可爱小标题
date: $year/$month/$day $fullhour:$minute:$second
updated: #$year/$month/$day $fullhour:$minute:$second
comments: true
tags:
  - 标签1
categories:
  - 分类1
---
```

​		或者

```搜狗输入法自定义短语
blogfm,1=
#---
title: 可爱小标题
date: $year/$month/$day $fullhour:$minute:$second
updated: #$year/$month/$day $fullhour:$minute:$second
comments: true
tags:
  - 标签1
categories:
  - 分类1
typora-root-url: ..
---
```
5. <kbd>Ctrl</kbd>+<kbd>s</kbd>保存配置文件，然后退出

6. 下回输入时直接中文输入法，<kbd>blogfm</kbd>，直接自动输出了所需要的Front-matter。

   ![image-20211115212128613](/images/20211115-路子-超野！搜狗输入法短语自动输入Front-matter/image-20211115212128613.png)

   
