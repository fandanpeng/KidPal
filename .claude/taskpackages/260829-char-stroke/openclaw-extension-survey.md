# OpenClaw 回复路径扩展面调查报告（Intent:nointent 依赖评估证据）

> Explore agent · 2026-09-07 · 对象 c:\AI\OpenSourceReference\openclaw-main（2026.7.2 快照，只读参考）+ GitHub 上游补证
> planner 代为落盘存档（调查 agent 只读）

## 0. 证据边界

`c:\AI\OpenSourceReference\openclaw-main`（版本 2026.7.2）是**不完整快照**：`src/` 下只有 `src/agents`（agent 运行时，1043 个文件）和 `src/acp`，**没有 `src/gateway/`、`src/plugins/`、`src/plugin-sdk/`、`src/commands/`**——即网关 HTTP 服务端和 hook runner 的实现源码不在本快照中（`src/agents/session-tool-result-guard-wrapper.ts:7` 还 `import ... "../plugins/hook-runner-global.js"` 可证）。调查用 GitHub API（`gh api` + raw.githubusercontent，仓库 `openclaw/openclaw`）补齐了缺失的 `src/gateway/openai-http.ts`（1396 行）作为佐证。行号凡标注 `[上游]` 者来自上游 main 分支，其余来自本地快照。

## 1. OpenAI 兼容端点：走完整 agent loop，非直通 LLM

- **实现文件（上游）**：`src/gateway/openai-http.ts`（头注释即写明 "Translates OpenAI chat requests to OpenClaw agent runs and SSE/JSON responses"）；路由挂在 `src/gateway/server-http.ts` / `server-public.ts`。本地快照无此文件，仅有 `docs/gateway/openai-http-api.md` 文档。
- 文档 `docs/gateway/openai-http-api.md:15` 明确："Requests run as a normal Gateway agent run (same codepath as `openclaw agent`)"。上游代码佐证：处理器把请求转成 agent command 后调用 `agentCommandFromGatewayIngress(...)`（openai-http.ts:1028 非流式 / 1309 流式）；`model` 字段是 **agent 目标**（`openclaw`、`openclaw/<agentId>`），不是裸 provider model id。
- **最终回复文本的产出点**（关键拼图）：无论流式/非流式，回复内容都从 agent run 结果里取 `resolveAssistantResultText(result)`：
  - 非流式：openai-http.ts:1091 `const content = resolveAssistantResultText(result) || "No response from OpenClaw.";` 然后直接 `sendJson(res, 200, {...chat.completion...})`
  - 流式：openai-http.ts:1353 `finalResultText = resolveAssistantResultText(result);` 后逐块写 SSE。
  - 该文件本身**不派发任何 plugin hook**（grep "hook" 零命中）——它是纯"agent 结果 → OpenAI 形状"的翻译层。但 agent run 内部的 hook（§2）都会照常触发。

## 2. Agent loop 的回复后处理 hook 点

OpenClaw 有完整的 typed plugin hook 系统（`api.on(name, handler, {priority, timeoutMs})`），目录 `docs/plugins/hooks.md`（828 行完整目录）。与"最终回复"相关的：

| Hook | 时机 | 能否改写回复文本 |
|---|---|---|
| `before_agent_reply` | 模型 turn 之前 short-circuit | **能**——返回 `{handled, reply}` 后整个 turn 用合成回复替代（本地快照 `src/agents/cli-runner.ts:490-536`，`hookRunner.runBeforeAgentReply` 返回 `handled` 则直接用 `hookResult.reply.text` 构造返回，**不跑模型**）。是"代码生成回复"，不是"改写 LLM 回复" |
| `before_agent_finalize` | harness 即将接受自然最终答案时 | 只能 `{action:"revise"}` 要求**再跑一轮模型**（带 retry instruction），或 `{action:"finalize"}`；**不能直接替换文本**（hooks.md 375-410 行） |
| `agent_end` | turn 结束后 | 仅观察，fire-and-forget |
| `tool_result_persist` | 工具结果持久化前 | **能改写**由工具结果产生的 assistant 消息（`src/agents/session-tool-result-guard-wrapper.ts:115-136`），但作用于 session 转写/上下文，不是 HTTP 返回 payload |
| `before_message_write` | 消息写 session 前 | 可 block/改写（同文件 78-113 行），同样作用于持久化而非返回 payload |
| `llm_input` / `llm_output` | provider 输入/输出 | 仅观察 |

**注册方式**：插件入口 `definePluginEntry({ register(api) { api.on("before_tool_call", ...) } })`（`docs/plugins/building-plugins.md`，package `@openclaw/plugin-sdk` 的 `plugin-entry` 子路径）。安装：`openclaw plugins install clawhub:<name>` 或配置 `plugins.load.paths` / `plugins.entries.<id>`（`docs/plugins/manage-plugins.md:214`）；非捆绑插件访问原始对话 hook 需在配置里开 `plugins.entries.<id>.hooks.allowConversationAccess=true`（hooks.md 470-480 行）。

另有一套 **internal hooks**（`HOOK.md` + `handler.ts` 脚本，事件如 `message:sent`、`agent:bootstrap`、`gateway:startup`，见 `docs/automation/hooks.md`）——官方明确定位为"操作员侧副作用自动化"，**无改写语义**，不适合本需求。

## 3. Plugin 能否在回复路径上做文本变换：分路径，HTTP 路径无对应 hook

- **channel 投递路径**（Discord/Telegram/WebChat 等）有真正的回复改写 hook：`message_sending`（改写 content 或 cancel）、`reply_payload_sending`（归一化 ReplyPayload 投递前改写）、`before_dispatch`（channel 交接前改写）——决策语义完整、顺序 priority、15s 默认预算（hooks.md 555-590 行）。
- **`/v1/chat/completions` HTTP 路径不走 channel 投递管线**——openai-http.ts 拿到 agent run 结果后自己序列化返回，因此 `message_sending`/`reply_payload_sending`/`before_dispatch` **在这条路径上不触发**。能在该路径触发的只有 agent run 内部 hooks（§2 表），而其中没有"直接改写最终回复文本"的 hook。
- 已触发于该路径的内置 hook 事件全集（agent run 内）：`before_model_resolve`、`agent_turn_prepare`、`before_prompt_build`、`before_agent_run`、`before_agent_reply`、`before_agent_finalize`、`agent_end`、`llm_input`、`llm_output`、`model_call_started/ended`、`before_tool_call`、`after_tool_call`、`tool_result_persist`、`before_message_write`（hooks.md catalog）。

## 4. Skills 机制：纯 prompt 注入，无直通输出

- skill = 目录 + `SKILL.md`（YAML frontmatter + markdown 正文），加载自 `<workspace>/skills`、`~/.openclaw/skills` 等 6 级优先级（`docs/tools/skills.md:18-45`；样例 `skills/weather/SKILL.md`）。skill 不注册代码、没有执行结果通道——它只是教 agent "何时用哪个已有工具"。
- 因此 skill 的"产出"必然经过 LLM/工具结果循环回到上下文，**没有直通输出路径**。用它让 LLM 复述坐标 JSON 正是要避免的 109s 问题，skill 解决不了。

## 5. tools 数组拒收的具体校验点

**关键发现：参考版本 2026.7.2 是支持外部 client tools 的**（`docs/gateway/openai-http-api.md` "Chat tool contract" 章节；CHANGELOG.md:5019，属 **2026.5.12** 版本："Gateway/OpenAI-compatible Chat Completions: support function `tools`, `tool_choice`, `tool_calls`, and `role: "tool"` follow-up turns ... reporting client-tool name conflicts as invalid requests. (#66278)"）。

- **校验/拒收代码位置（上游 main）**：`src/gateway/openai-http.ts`
  - `extractClientToolsFromChatRequest`（201-269 行）：`"tools must be an array"`（206 行）、`"only function tools are supported"`（214 行）、`tool_choice` 各类不支持的报错（254/262/267/269 行）
  - 主 handler 里 967-976 行：解析失败 → `sendInvalidRequest(res, "Invalid tools/tool_choice: ...")` → **400 invalid_request_error**，请求被拒。
- **推论**：KidPal 部署的 OpenClaw 若拒收带 `tools` 的请求，大概率运行的是 **< 2026.5.12 的旧版本**（该功能 2026-05 才加）。请核对线上 gateway 的 `openclaw --version`；升级后 tools/tool_calls 对外即可用。本地快照里没有旧版拒收代码可引（快照只有新版逻辑的文档）。

## 6. 结论：实现「回复后程序化改写」的最可行机制（按可行性排序）

**A（零 hack，但违反 KidPal 冻结位置裁定）：client tools 通道 + 调用方/网关侧拼装——需先升级 OpenClaw ≥ 2026.5.12。**
让 LLM 不在正文里复述坐标，而是发起一次 client tool call，`function.arguments` 就是那个极小 display JSON；端点直接把它结构化返回（`finish_reason:"tool_calls"`，openai-http.ts:1073-1091），KidPal 网关拿到 arguments 后查 makemeahanzi 静态字库拼全量 D3 JSON。tool arguments 是结构化紧凑输出，天然规避逐 token 复述。**但拼装点在网关 = 字库/拼装代码入 shim，违反 human 2026-09-01 冻结的 C1 位置裁定（挂 OpenClaw 侧、绝不入 shim，§1.5 薄桥红线）——KidPal 侧否决。**

**B（KidPal 选定）：patch `src/gateway/openai-http.ts` 的回复文本出口（~10 行量级补丁）。**
改写点天然集中在两处：非流式 1091 行 `const content = resolveAssistantResultText(result) || ...` 和流式 1353 行 `finalResultText = resolveAssistantResultText(result)`。在此对 `content` 做正则提取 `{"view":"char",...}` 小块 → 查字库 → 替换为全量 JSON，即可实现"LLM 之后、返回调用方之前"的纯代码改写，完全脱离 LLM 生成路径。字库（makemeahanzi + 预热表 + LRU）与补丁都挂 OpenClaw 侧（ECS），守 §1.5 薄桥红线。由于本快照缺 `src/gateway/`，补丁需在 ECS 部署源（或上游完整 checkout）上按部署版本开发。

**C（不 patch 核心的插件路径）：不存在。**
在 `/v1/chat/completions` 路径上没有现成 hook 能直接改写返回文本（§2/§3）：`before_agent_finalize` 只能要求模型再跑一轮（更慢），`before_agent_reply` 是整轮替代 LLM，`tool_result_persist`/`before_message_write` 只影响 session 持久化不改返回 payload，channel 投递 hooks 在该路径不触发。

**不推荐/否决**：skill（纯 prompt，必然回灌 LLM 上下文）、internal hooks（`HOOK.md`，仅事件自动化无改写语义）。

## 涉及文件清单

- `docs/gateway/openai-http-api.md`（端点契约；本地快照可读）
- 上游 `src/gateway/openai-http.ts`（端点实现；§1/§5 行号）
- `docs/plugins/hooks.md`（hook 目录与决策语义）、`docs/plugins/building-plugins.md`、`docs/plugins/manage-plugins.md`（注册/安装）
- `src/agents/cli-runner.ts:490-536`（`before_agent_reply` 合成回复实例；本地快照）
- `src/agents/session-tool-result-guard-wrapper.ts:78-136`（`before_message_write`/`tool_result_persist` 派发；本地快照）
- `docs/automation/hooks.md`（internal hooks，否决项）
- `docs/tools/skills.md`、`skills/weather/SKILL.md`（skill=纯 markdown，否决项）
