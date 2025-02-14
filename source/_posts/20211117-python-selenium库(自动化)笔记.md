---
title: python selenium库笔记(自动化)
date: 2021/11/17 20:02:56
updated: #2021/11/17 20:02:56
comments: true
tags:
  - 笔记
  - python
  - 自动化
categories:
typora-root-url: ..
---

# python selenium库笔记(自动化)

> selenium 库不光要配置python环境，还要安装对应的浏览器驱动





## 安装selenium库

```cmd
::在python中安装selenium库
pip install selenium
```



## 打开Chrome浏览器

```python
from selenium import webdriver

wd = selenium.webbrowser.Chrome()
```



## 打开指定网址

```python
wd.get("https://www.baidu.com")
```



## 获取WebElement对象

### find_element 和 find_elements 的区别

使用  `find_elements` 选择的是符合条件的 `所有` 元素， 如果没有符合条件的元素， `返回空列表`

使用  `find_element` 选择的是符合条件的 `第一个` 元素， 如果没有符合条件的元素， `抛出 NoSuchElementException 异常`



### 简单的获取方法

```python
#关键点
from selenium.webdriver.common.by import By

#利用id获取WebElement对象
wd.find_element(By.ID, 'username')

#利用CLASS_NAME获取WebElement对象
wd.find_element(By.CLASS_NAME, 'password')

#利用TAG_NAME获取WebElement对象
wd.find_element(By.TAG_NAME, 'input')

#利用CSS_SELECTOR获取WebElement对象
wd.find_element(By.CSS_SELECTOR,'button[type=submit]')

```

### Xpath选择器

```python
elements = driver.find_elements(By.XPATH, "//div/*")
```



## 操控WebElement对象

```
clear　　清除文本

send_keys(value)　　模拟按键输入

click()　　单击

submit()　　提交表单，如搜索框输入内容后的回车操作，就可以通过submit()方法模拟

size　　返回元素的尺寸

text　　获取元素的文本

get_attribute(name)　　获得属性值

is_displayed()　　返回该元素是否可见，布尔类型
```



### 输入框操作

```
# 清除输入框已有的字符串
element.clear() 
# 输入新字符串
element.send_keys('白月黑羽') 
```

### 获取元素文本内容

```python
element.text
```











