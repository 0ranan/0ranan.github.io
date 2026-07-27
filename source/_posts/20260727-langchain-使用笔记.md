---
title: langchain 使用笔记
date: 2026/07/27 02:37:45
updated: 2026/07/27 02:37:45
comments: true
tags:
  - AI
  - langchain
categories:
  - AI
typora-root-url: ..
---

# langchain 使用笔记

## LangGraph Agent 示例

```python
from langchain_openai import ChatOpenAI
from langchain_core.tools import tool
from langgraph.prebuilt import create_react_agent
import os, json
from langchain_core.messages import messages_to_dict


@tool
def get_weather(city: str) -> str:
    """获取城市天气信息"""
    return f"{city} 总是阳光明媚"


SYSTEM_PROMPT = """\
你是一个智能助手，请根据用户的问题调用合适的工具来回答。
回答时语言简洁、准确，并在末尾给出总结。
"""

agent = create_react_agent(
    model=ChatOpenAI(
        model="deepseek-v4-flash",
        api_key=os.environ["OPENAI_API_KEY"],
        base_url="https://api.deepseek.com/v1",
    ),
    tools=[get_weather],
    system_prompt=SYSTEM_PROMPT,
)

result = agent.invoke(
    {"messages": [{"role": "user", "content": "今天南京天气怎么样？"}]}
)
final_message = result["messages"][-1]

print(json.dumps(messages_to_dict(result["messages"]), indent=2, ensure_ascii=False))
print(final_message.content)
```

