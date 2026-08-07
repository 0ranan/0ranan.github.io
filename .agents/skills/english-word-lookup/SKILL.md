---
name: "english-word-lookup"
description: "查询英语单词的详细信息，包括翻译、音标、短语、近义词、例句和同根词。当用户询问英语单词的含义、翻译、发音或想要查词时调用。Uses the xxapi.cn English dictionary API to look up detailed English word information. Invoke when user asks about English word meanings, translations, pronunciation, or wants to look up a word."
---

# 英语单词查询

通过 API 查询英语单词的详细信息，包括翻译、音标（英音/美音）、短语、近义词、例句和同根词。

## 使用方法

当用户询问某个英语单词的信息时，使用 `WebFetch` 工具调用以下 API：

```
https://v2.xxapi.cn/api/englishwords?word={单词}
```

将 `{单词}` 替换为用户要查询的英语单词。

## 返回结果展示

将 API 返回的数据整理为以下格式展示给用户：

### 1. 基本信息
- **单词**: `word`
- **词书**: `bookId`（如 CET4_2 表示四级词书）

### 2. 音标与发音
- **英音**: `/ukphone/` — [试听](ukspeech)
- **美音**: `/usphone/` — [试听](usspeech)

### 3. 释义
按词性列出每个翻译：
- `translations[].pos` `translations[].tran_cn`

### 4. 常用短语
列出 `phrases` 数组中的短语（展示前 20 条）：
- `phrases[].p_content` — `phrases[].p_cn`

### 5. 近义词
按词性分组展示 `synonyms`：
- `synonyms[].pos` `synonyms[].tran`
- 列出 `synonyms[].Hwds[].word`

### 6. 例句
列出 `sentences` 中的例句：
- `sentences[].s_content`（英文）
- `sentences[].s_cn`（中文翻译）

### 7. 同根词
按词性分组展示 `relWords`：
- `relWords[].Pos`
- 列出 `relWords[].Hwds[].hwd` — `relWords[].Hwds[].tran`

## 展示模板示例

查询单词 `heat` 后，应按如下中文格式展示：

---

**heat** | 词书: CET4_2

**音标**
- 英音: /hiːt/ [试听](https://dict.youdao.com/dictvoice?audio=heat&type=1)
- 美音: /hit/ [试听](https://dict.youdao.com/dictvoice?audio=heat&type=2)

**释义**
- n. 热，炎热
- v. 变热

**常用短语**
- heat transfer — 热传递，传热
- heat treatment — 热处理
- heat exchanger — 热交换器
- ...（最多展示 20 条）

**近义词**
- n. 高温；压力；热度；热烈：pressure, high temperature
- vt. 使激动；把…加热：impassion, emotionalize

**例句**
- Ice needs heat to melt.
  冰需要热才能融化。
- Insulating the attic is a good way to reduce heat loss.
  为阁楼安装隔热层是减少热量流失的好方法。

**同根词**
- adj. heated 热的；激昂的；激动的 | heatable 可被加热的 | heatless 无热的
- adv. heatedly 加热地；激昂地，热情的
- n. heater 加热器；加热工
- v. heated 加热；使兴奋（heat的过去分词）

---

## 注意事项

- 只调用 API 一次，不要重复请求
- 如果 API 返回错误（code 不为 200），告知用户查询失败并显示错误信息
- 短语最多展示 20 条，避免信息过载
- 单词查询不区分大小写
