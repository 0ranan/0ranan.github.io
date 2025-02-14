---
title: aircloud主题下方添加gitee地址
date: 2021/11/15 09:01:00
update: 2021/11/15 09:01:00
tags :
  - html
  - ejs
  - hexo
  - 前端
categories:
  - 编程
  - 前端
  - hexo
typora-root-url: ..

---

# 在aircloud主题下方添加gitee图标

## 需求产生
1. aircloud主题真的好漂亮，我好喜欢！！！！
2. 国内gitee访问速度快，github非常慢
3. aircloud 默认不支持在博客下方添加gitee图标

## 实现

### 观察aircloud主题源代码

1. 主题demo部分在_config.yml部分可以配置相应的社交平台_

   [hexo-aircloud-blog/_config.yml (github.com)](https://github.com/aircloud/hexo-aircloud-blog/blob/master/_config.yml)

   ![image-20211115091424648](/images/20211115-在hexo-aircloud主题下方添加gitee图标\image-20211115091424648.png)

2. 主题/blob/master/layout/_partial/footer.ejs中配置的界面下方的标题栏 _

   [hexo-theme-aircloud/layout/_partial/footer.ejs (github.com)](https://github.com/aircloud/hexo-theme-aircloud/blob/master/layout/_partial/footer.ejs)

   ![image-20211115092705043](/images/20211115-在hexo-aircloud主题下方添加gitee图标\image-20211115092705043.png)

   1. herf 里面应该是链接地址的拼接，

      ```ejs
      <%- config.github_username %>
      ```

      应该是从config文件中读取github的用户名

      ```ejs
       <i class="iconfont icon-github"></i>
      ```

      好像是图标？

      1. 修改拼接格式![image-20211115093208494](/images/20211115-在hexo-aircloud主题下方添加gitee图标\image-20211115093208494.png)在_config中修改![image-20211115093354718](/images/20211115-在hexo-aircloud主题下方添加gitee图标\image-20211115093354718.png)

         1. 效果奇怪怪的![image-20211115093425534](/images/20211115-在hexo-aircloud主题下方添加gitee图标\image-20211115093425534.png)

         2. 搜索

            ```
            class="iconfont icon
            ```

            猜对关键词![image-20211115095406160](/images/20211115-在hexo-aircloud主题下方添加gitee图标\image-20211115095406160.png)[阿里图标的三种使用方式 - 简书 (jianshu.com)](https://www.jianshu.com/p/cb97be966e98)

      2. 在head .ejs中存放了对阿里图标的调用

         ![image-20211115095945291](/images/20211115-在hexo-aircloud主题下方添加gitee图标\image-20211115095945291.png)

         [hexo-theme-aircloud/head.ejs at master · aircloud/hexo-theme-aircloud (github.com)](https://github.com/aircloud/hexo-theme-aircloud/blob/master/layout/_partial/head.ejs)

      3. 注册自己的阿里账号，插入进去

         ![image-20211115100352759](/images/20211115-在hexo-aircloud主题下方添加gitee图标\image-20211115100352759.png)

      4. 插入gitee2 图标

         ![image-20211115100522255](/images/20211115-在hexo-aircloud主题下方添加gitee图标\image-20211115100522255.png)

         ![image-20211115100705213](/images/20211115-在hexo-aircloud主题下方添加gitee图标\image-20211115100705213.png)

         

      5. 完美解决 
      
         ![image-20211115100606419](/images/20211115-在hexo-aircloud主题下方添加gitee图标\image-20211115100606419.png)

### 总结


1. 注册阿里云，创建项目，添加gitee2图标,并在head.ejs中添加自己的css文件

   ```ejs
   	<!-- 它提供的css文件 -->
       <link href="//at.alicdn.com/t/font_620856_pl6z7sid89qkt9.css" rel="stylesheet" type="text/css">
       <!-- 我自己的css文件 -->
   	<link href="//at.alicdn.com/t/font_2937294_fvlq3ab7zh.css" rel="stylesheet" type="text/css">
   ```

   

2. 在footer.ejs文件中添加

   ```ejs
        <% if (config.gitee_username) { %>
           <li>
               <a target="_blank"  href="https://gitee.com/<%= config.gitee_username %>">
                               <span class="fa-stack fa-lg">
                                   <i class="iconfont icon-gitee2"></i>
                               </span>
               </a>
           </li>
           <% } %>
   ```

   

3. 在_config.yml添加自己的配置信息

   ```
   gitee_username:    an-ran0
   ```

4. 完美解决

