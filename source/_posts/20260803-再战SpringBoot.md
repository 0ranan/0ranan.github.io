---
title: 再战SpringBoot
date: 2026/08/03 07:46:13
updated: 2026/08/04 15:32:00
comments: true
tags:
  - SpringBoot
  - Java
  - 后端
categories:
  - 编程
typora-root-url: ..
---

# 再战SpringBoot

## 心路历程

上次学习 Spring 的时候还是四年前[Sping-framework Bean作用域](https://0ranan.github.io/2022/04/30/Sping-framework-Bean%E4%BD%9C%E7%94%A8%E5%9F%9F/)，好多概念之前就没能捋清楚。现在到了 Java 17，Spring Boot 3.5.3，Spring Framework 也到了 7。

上一个彻底熟练掌握的版本还是 Java 8，新的语法让我在 AI 时代本就不富裕的脑容量雪上加霜。

无奈，重新整理知识点。

## 学习版本
```
Java 17
Spring Boot 3.5.3
Spring Framework 7
```

## 依赖注入（DI）与构造器注入

chatAssistant 通过构造器注入（Constructor Injection）装配，这是 Spring 最推荐的注入方式。

### 注入点

ChatController.java：

```java
private final ChatAssistant chatAssistant;              // ① final 字段
public ChatController(ChatAssistant chatAssistant) {    // ② 唯一构造器，参数声明类型
    this.chatAssistant = chatAssistant;
}
```

两个关键点：

- 字段用 final：一旦注入不可变，防止后续被意外替换；
- 只有一个构造器：Spring 4.3 起，单构造器可省略 @Autowired，容器自动从该构造器解析依赖。

### Bean 从哪来

注入的是 ChatAssistantConfig.java 中 @Bean 方法构建的动态代理对象（AiServices.builder(...).build() 生成）。

### 完整注入链路（3 步）

```
① 启动时 ChatAssistantConfig.chatAssistant() 方法执行
   → @Bean 把返回值（ChatAssistant 代理对象）注册进 Spring 容器
② ChatController 实例化，发现构造器参数是 ChatAssistant
   → Spring 按「类型」从容器查找 ChatAssistant 类型的 Bean
③ 找到后调用构造器，把代理对象塞进 this.chatAssistant
   → 后续 controller 里 chatAssistant.chatStream(...) 调的就是它
```

### 为什么能匹配上

依赖注入默认按类型匹配：@Bean 方法返回类型与构造器参数声明类型一致即注入。若容器中有多个同类型 Bean，Spring 才会报 NoUniqueBeanDefinitionException。

## @Configuration + @Bean：声明式配置

LangChain4jConfig.java 中 chatMemoryProvider() 方法的写法涉及四五个语法点。

```java
@Configuration
public class LangChain4jConfig {

    @Bean
    ChatMemoryProvider chatMemoryProvider() {
        return memoryId -> MessageWindowChatMemory.builder()
                .id(memoryId)
                .maxMessages(20)
                .chatMemoryStore(new InMemoryChatMemoryStore())
                .build();
    }
}
```

### @Configuration + @Bean

- @Configuration：告诉 Spring「这个类是配置类」，启动时会把它注册成 Bean，并处理里面的 @Bean 方法。
- @Bean：作用于方法上。Spring 启动时会调用一次该方法，把返回值放进容器。之后其他代码需要 ChatMemoryProvider 时，直接注入即可，不用自己 new。

### 方法名即 Bean 名

方法叫 chatMemoryProvider()，返回类型是 ChatMemoryProvider，Spring 就把这个 Bean 命名为 chatMemoryProvider。这是惯例，想改名可以加 @Bean("自定义名")，但一般没必要。

### Lambda 表达式

ChatMemoryProvider 是函数式接口（只有一个抽象方法 get(memoryId)），因此可以用 Lambda 简写。等价的匿名内部类写法：

```java
ChatMemoryProvider chatMemoryProvider() {
    return new ChatMemoryProvider() {
        @Override
        public ChatMemory get(Object memoryId) {
            return MessageWindowChatMemory.builder()
                    .id(memoryId)
                    .maxMessages(20)
                    .chatMemoryStore(new InMemoryChatMemoryStore())
                    .build();
        }
    };
}
```

每个 memoryId 对应一份独立的记忆，互不串台。

### Builder 建造者模式（链式调用）

builder() 返回构建器对象，每个 .xxx() 填一个参数，最后 .build() 一次性生成实例。参数可读、顺序无关，比一堆构造器重载干净。

- .maxMessages(20)：滑动窗口，只保留最近 20 条消息，超出丢弃旧的。
- new InMemoryChatMemoryStore()：存内存，进程重启即丢失。

### 返回类型推导

方法声明返回类型是 ChatMemoryProvider（父接口），Lambda 实际返回的是 MessageWindowChatMemory（具体实现），编译器自动做向上转型。

## Java Record Classes 精要

> 人肉理解: record 就是所有属性都是final 的一种class ,被编译器生成过class类常用的方法,比如hashCode,equals,toString 更方便纯数据的对象进行访问.

### 定义

Record 是一种特殊的类，用于建模纯数据载体（plain data aggregates），以极简的声明替代传统类的样板代码。

声明格式：`record 名称(组件列表) { 可选主体 }`，例如 `record Rectangle(double length, double width) {}`。

### 自动生成的成员

为每个组件自动生成：

- **private final 字段**（组件字段）
- **公共访问器方法**（与组件同名，如 `length()`、`width()`）
- **规范构造器**（签名与组件列表一致，负责赋值）
- **equals() / hashCode()**：基于所有组件值判断相等
- **toString()**：包含所有组件名称和值

### 构造器定制

- 可显式声明规范构造器（需手动赋值）
- 更简洁的**紧凑构造器**：省略参数列表，末尾自动完成字段赋值，适合验证或转换逻辑，例如：

```java
public Rectangle {
    if (length <= 0 || width <= 0) throw new IllegalArgumentException();
}
```

### 显式成员声明

- 可覆盖访问器、equals、hashCode、toString（需保持与隐式版本的行为一致）。
- 支持静态字段、静态初始化器、静态方法。
- 允许实例方法、嵌套类/接口（嵌套记录类隐式 static）。
- **禁止**：实例变量（非静态字段）、实例初始化器、native 方法。

### 关键特性

- **隐式 final**：不能继承 Record 类。
- 支持泛型、实现接口、注解（注解可传播到组件字段）。
- 与密封类/接口良好协作。

### 局部记录类

在方法内定义，类似局部类，但隐式 static，不能访问所在方法的局部变量（与普通局部类不同）。

### 内部类的静态成员

从 Java SE 16 起，内部类可声明静态成员（含记录类成员），此前仅允许常量变量。

### 序列化

Record 实例可序列化，但不能自定义 writeObject/readObject 等方法；序列化由组件决定，反序列化由规范构造器控制。

### API 相关

- 所有 Record 的父类：`java.lang.Record`（隐式导入）。
- `Class` 提供 `getRecordComponents()` 和 `isRecord()` 方法。

### 注意事项

避免在非 `java.lang` 包中定义名为 Record 的类，否则与系统类冲突；需使用全限定导入解决歧义。

