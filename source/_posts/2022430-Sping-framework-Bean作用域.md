---
title: Sping-framework Bean作用域
date: 2022/4/30 2:29:06
updated: 2022/5/1 1:03:39
comments: true
tags:                                              
  - JAVA
  - 后端
categories:
typora-root-url: ..
---

# Sping-framework Bean作用域



当您创建一个 bean 定义时，您创建了一个用于创建由该 bean 定义定义的类的实际实例的方法。 bean 定义是一个配方的想法很重要，因为这意味着，与一个类一样，您可以从一个配方创建许多对象实例。您不仅可以控制要插入到从特定 bean 定义创建的对象中的各种依赖项和配置值，还可以控制从特定 bean 定义创建的对象的范围。这种方法功能强大且灵活，因为您可以通过配置选择您创建的对象的范围，而不必在 Java 类级别烘焙对象的范围。可以将 Bean 定义为部署在多个范围之一中。 Spring Framework 支持六个范围，其中四个仅在您使用 Web 感知 ApplicationContext 时可用。您还可以创建 [自定义范围](https://docs.spring.io/spring-framework/docs/current/reference/html/core.html#beans-factory-scopes-custom)

下表描述了支持的范围：

| **Scope**                                                    | **Description**                                              |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [singleton](https://docs.spring.io/spring-framework/docs/current/reference/html/core.html#beans-factory-scopes-singleton) | （默认）将单个 bean 定义限定为每个 Spring IoC 容器的单个对象实例。 |
| [prototype](https://docs.spring.io/spring-framework/docs/current/reference/html/core.html#beans-factory-scopes-prototype) | 将单个 bean 定义限定为任意数量的对象实例。                   |
| [request](https://docs.spring.io/spring-framework/docs/current/reference/html/core.html#beans-factory-scopes-request) | 将单个 bean 定义限定为单个 HTTP 请求的生命周期。 也就是说，每个 HTTP 请求都有自己的 bean 实例，该实例是在单个 bean 定义的后面创建的。 仅在 Web 感知 Spring .ApplicationContext 的上下文中有效 |
| [session](https://docs.spring.io/spring-framework/docs/current/reference/html/core.html#beans-factory-scopes-session) | 将单个 bean 定义限定为 HTTP 的生命周期。 仅在 Web 感知 Spring .SessionApplicationContext 的上下文中有效 |
| [application](https://docs.spring.io/spring-framework/docs/current/reference/html/core.html#beans-factory-scopes-application) | 将单个 bean 定义限定为 . 仅在 Web 感知 Spring .ServletContextApplicationContext 的上下文中有效 |
| [websocket](https://docs.spring.io/spring-framework/docs/current/reference/html/web.html#websocket-stomp-websocket-scope) | 将单个 bean 定义限定为 . 仅在 Web 感知 Spring .WebSocketApplicationContext 的上下文中有效 |

|     | 从 Spring 3.0 开始，线程范围可用，但默认情况下未注册。 有关详细信息，请参阅  [SimpleThreadScope](https://docs.spring.io/spring-framework/docs/5.3.19/javadoc-api/org/springframework/context/support/SimpleThreadScope.html). 有关如何注册此或任何其他自定义范围的说明，请参阅使用 [Using a Custom Scope](https://docs.spring.io/spring-framework/docs/current/reference/html/core.html#beans-factory-scopes-custom-using). |
| ---- | ------------------------------------------------------------ |
|      |                                                              |

## singleton(单例)

仅管理一个单例 bean 的一个共享实例，并且所有对具有与该 bean 定义匹配的一个或多个 ID 的 bean 的请求都会导致 Spring 容器返回一个特定的 bean 实例。

换句话说，当您定义 bean 定义并将其限定为单例时，Spring IoC 容器会创建该 bean 定义所定义的对象的一个实例。 此单个实例存储在此类单例 bean 的缓存中，并且该命名 bean 的所有后续请求和引用都返回缓存的对象。 下图显示了单例作用域的工作原理：

![image-20220430230117296](/../Users/Air14-2020/AppData/Roaming/Typora/typora-user-images/image-20220430230117296.png)

Spring 的单例 bean 概念不同于四人组 (GoF) 模式书中定义的单例模式。 GoF 单例对对象的范围进行硬编码，以便每个 ClassLoader 创建一个且仅一个特定类的实例。 Spring 单例的范围最好描述为每个容器和每个 bean。 这意味着，如果您在单个 Spring 容器中为特定类定义一个 bean，则 Spring 容器会创建该 bean 定义定义的类的一个且仅一个实例。 单例范围是 Spring 中的默认范围。 要将 bean 定义为 XML 中的单例，您可以定义一个 bean，如下例所示：

```xml
<bean id="accountService" class="com.something.DefaultAccountService"/>

<!-- 以下是等价的，虽然是多余的（单例范围是默认的） -->
<bean id="accountService" class="com.something.DefaultAccountService" scope="singleton"/>
```

## Prototype 

bean 部署的非单例原型范围导致每次对特定 bean 发出请求时都会创建一个新的 bean 实例。 也就是说，bean 被注入到另一个 bean 中，或者您通过容器上的 getBean() 方法调用来请求它。 通常，您应该对所有有状态 bean 使用原型范围，对无状态 bean 使用单例范围。 下图说明了 Spring 原型范围：

## 我的测试



```java
    @Test
    public void beanScopesDemo() {
        //这个例子证明了单例(singleton)模式下不同ApplicationContext产生的对象不一样
        System.out.println(   new ClassPathXmlApplicationContext("beans_scopes_demo.xml").getBean("singleton", BeanScopesDemo.class).toString());
        System.out.println(   new ClassPathXmlApplicationContext("beans_scopes_demo.xml").getBean("singleton", BeanScopesDemo.class).toString());
    }

    @Test
    public void beanScopesDemo2() {
        //这个例子证明了单例(singleton)模式下同ApplicationContext产生的对象一样
        ApplicationContext context =   new ClassPathXmlApplicationContext("beans_scopes_demo.xml");
        System.out.println(context.getBean("singleton"));
        System.out.println(context.getBean("singleton"));
    }

    @Test
    public void beanPrototypeDemo() {
        //这个例子证明了原型(prototype)模式下同ApplicationContext产生的对象不一样
        ApplicationContext context =   new ClassPathXmlApplicationContext("beans_scopes_demo.xml");
        System.out.println(context.getBean("prototype"));
        System.out.println(context.getBean("prototype"));
    }

```

```xml
<?xml version="1.0" encoding="UTF-8"?>
<beans xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xmlns="http://www.springframework.org/schema/beans"
       xsi:schemaLocation="http://www.springframework.org/schema/beans http://www.springframework.org/schema/beans/spring-beans.xsd">

    <!--单例模式-->
    <bean id="singleton" class="io.anran0.di.BeanScopesDemo" scope="singleton"/>
    <!--原型模式-->
    <bean id="prototype" class="io.anran0.di.BeanScopesDemo" scope="prototype"/>

</beans>
```

```java

/*
 * <p>项目名称: spring-stady </p>
 * <p>文件名称: BeanScopesDemo </p>
 * <p>描述: 用于测试Bean的生命周期的类</p>
 * <p>创建时间: 2022/04/30 22:19:59 </p>
 * <p>公司信息: ************公司 *********部</p>
 * @author <a href="mail to: 1715139614@qq.com" rel="nofollow">安苒an-ran0</a>
 * @version v1.0
 */
public class BeanScopesDemo {
    private static int i = 0;
    private int index;

    public BeanScopesDemo() {
        System.out.println("Bean scopes demo被初始化");
        index = i++;
    }

    @Override
    public String toString() {
        return "BeanScopesDemo{" +
                "index=" + index +
                '}';
    }
}
```

