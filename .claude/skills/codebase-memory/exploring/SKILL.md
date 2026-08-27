---
name: codebase-memory-exploring
description: "Use when the user asks how code works, wants to understand architecture, trace execution flows, or explore unfamiliar parts of the codebase. Examples: \"How does X work?\", \"What calls this function?\", \"Show me the auth flow\". Routes: search_graph for discovery, trace_path for call chains, get_code_snippet for source."
---

# Exploring Code with CodebaseMemory

## When to Use

- "How does authentication work?"
- "What's the project structure?"
- "Show me the main components"
- "Where is the database logic?"
- Understanding code you haven't seen before

## Workflow

```
1. get_architecture({aspects: ["overview"]})          → Project structure at a glance
2. search_graph({query: "<what you want to understand>"}) → Find relevant symbols
3. trace_path({function_name: "<symbol>", direction: "both"}) → Trace call chain
4. get_code_snippet({qualified_name: "<exact qn>"})   → Read source
```

## Tools

**search_graph** — find symbols by name, pattern, or natural language:

```
search_graph({query: "payment processing"})
→ Functions ranked by relevance with qualified names
```

**trace_path** — inbound/outbound call chains:

```
trace_path({function_name: "buildDisplayJson", direction: "inbound"})
→ callers: [handler, POST] at hop 1
```

**get_code_snippet** — read source for a specific symbol:

```
get_code_snippet({qualified_name: "ESP32.server.shim.main.app.route.chat.POST"})
→ Full source of the POST handler（本仓建索引后，qualified_name 前缀 = 项目名 ESP32）
```

**get_architecture** — project structure in one call:

```
get_architecture({aspects: ["overview"]})
→ Languages, packages, entry points, clusters
```

## Example: "查字请求是怎么处理的？"

```
1. search_graph({query: "char stroke display"})
   → POST (server/shim/app/routes/chat.py)
   → build_display_json (server/shim/app/services/display.py)
2. trace_path({function_name: "buildDisplayJson", direction: "both"})
   → callers: chat_handler
   → callees: makeahanzi_lookup, ...
3. get_code_snippet({qualified_name: "ESP32...buildDisplayJson"})
   → Read actual implementation
```
