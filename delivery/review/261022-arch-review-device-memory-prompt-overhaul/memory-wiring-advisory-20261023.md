# Honcho 记忆接线修正批 — architect advisory（2026-10-23）

> **性质声明**：本文件是 **advisory 意见**，非冻结制品、非评审记录、非验收证据；不豁免任何门、不构成通过判据。human 过目前**零 canonical 写入**：本次检视全程只读（仓内文件只读 + ECS 只读探查），未改 `docs/**`、`.claude/**`、`server/**` 任何内容，也未动 ECS 任何配置（仅 `--dry-run` 与只读 SELECT）。
>
> **落位说明**：architect 的 allow 面含 `delivery/review/**`，本文落此；不动 `delivery/workstreams/**`（owner = 人 / planner / dev-loop）。
>
> **第一判据**：human 2026-10-23 原话「我们之前测试过、已经验证过能用的功能，就直接用起来，不要再去发明什么新的东西了。」本报告的排序、取舍与推荐全部以此为第一判据；凡是"发明物"，无论是否已冻结，一律标注。

---

## 0. 一句话结论

**探针轮验证过的接线 = 上游 npm 插件 `@honcho-ai/openclaw-honcho@1.7.0` + `plugins.entries.openclaw-honcho` 的 4 个配置键**；K3 冻结点名的 `hooks.allowConversationAccess`（顶层）与 `agents.entries.kidpal.plugins`（agent 内嵌）**从来不是这个接线的形状，也没有任何一轮验证用过它们**——它们的来源是把探针配置的「缩略写法」当成了字面路径。

推荐：**原样复用探针接线（甲）**；K3 自研 carrier **弃用**（不是重写——它连插件都不是，且调用的 Honcho 端点在 Honcho 源码里不存在）。甲案会在 §6.7 的「资格门」字面上留下残余偏差，这必须由 human 在「改 spec 口径」与「给上游插件打最小补丁」之间二选一，不能由本 agent 和稀泥。

---

## 1. 「验证过的接线」到底是什么（逐条引证据）

### 1.1 版本与来源

| 事实 | 证据 |
|---|---|
| OpenClaw 生产与探针**同版本**：`2026.8.1 (ea80657)` | ECS `openclaw --version`；探针状态目录 `wizard.lastRunVersion=2026.8.1`、`meta.lastTouchedVersion=2026.8.1`（`/root/.openclaw-probe/openclaw.json`） |
| 插件 = npm 包 `@honcho-ai/openclaw-honcho@1.7.0`（不是自研） | 探针安装目录 `/root/.openclaw-probe/npm/projects/honcho-ai-openclaw-honcho-00035f07ba/` 的 `package.json` + `package-lock.json`（解析址 `registry.npmmirror.com`） |
| 安装方式 = `openclaw plugins install`（OpenClaw 自有插件管理器） | 同目录 `node_modules/@honcho-ai/openclaw-honcho/`（`files` 声明含 `dist / workspace_md / install.js / openclaw.plugin.json`）+ `plugins inspect` 输出的 `Install: Source npm / Spec @honcho-ai/openclaw-honcho / Installed at 2026-10-22T07:40:52.620Z` |
| 运行时插件状态 | `OPENCLAW_STATE_DIR=$HOME/.openclaw-probe openclaw plugins inspect openclaw-honcho --runtime`：`Status: loaded`、`Origin: global`、`Version: 1.7.0`、`Typed hooks: agent_end / before_prompt_build / gateway_start / llm_output / subagent_spawned`、`Tools: honcho_*`、`Policy: allowConversationAccess: true` |

### 1.2 配置键形（探针实测的 4 个键，逐字）

探针轮安装前后的配置差分（`/root/.openclaw-probe/openclaw.json.bak` → `openclaw.json`，全量扁平 diff，**只动了 4 个叶子**）：

```
plugins.entries.openclaw-honcho.enabled                        : (absent) -> true
plugins.entries.openclaw-honcho.hooks.allowConversationAccess  : (absent) -> true
plugins.entries.openclaw-honcho.config.baseUrl                 : (absent) -> "http://127.0.0.1:18100"
plugins.entries.openclaw-honcho.config.workspaceId             : (absent) -> "kidpal-probe"
```

**这就是「验证过的接线」的全部**。注意三件事：

1. `allowConversationAccess` 是**插件条目内**的键（`plugins.entries.<id>.hooks.*`），**不是**顶层 `hooks.*`。顶层 `hooks` 在 schema 里只有 `internal`（`config/ zod-schema`：`hooks: strictObject({ internal: ... })`）。
2. 插件 id 是 `openclaw-honcho`（来自 `openclaw.plugin.json` 的 `"id": "openclaw-honcho"`），其配置 schema 是 `{apiKey?, workspaceId?, baseUrl?, timeoutMs?, noisePatterns?, ownerObserveOthers?, disableDefaultNoisePatterns?, crossSessionSearch?, enableMemoryCompatibilityTools?, recall?, captureSystemRuns?}`（`openclaw.plugin.json .configSchema`，`additionalProperties: false`）。
3. 自托管实例**不需要 apiKey**（schema 里 apiKey 可选；`/opt/honcho/.env` 未设 `AUTH_USE_AUTH`，`curl 127.0.0.1:18100/health` = 200）。

### 1.3 运行时数据事实（Honcho pgvector 库，只读 SELECT）

探针轮写进 Honcho 的实体形状（库内仅一个 workspace `kidpal-probe`，创建于 2026-10-22 07:43Z ≈ 15:43 CST）：

- **peers**：只有两个 —— `owner`、`agent-kidpal`。**没有** `agent:kidpal:openai-user:<device_id>` 这类 peer。
- **sessions**：两条，metadata 分别为
  `{"agentId":"kidpal","sessionClass":"chat","messageProvider":"webchat","openclawSessionKey":"agent:kidpal:openai-user:device-probe-honcho","lastSessionId":"1d9cf621-…"}`
  与 `…openclawSessionKey":"agent:kidpal:openai-user:device-probe-honcho-2"…`。
- **session_peers**：每 session 恰 `owner(observe_me=true, observe_others=false)` + `agent-kidpal(observe_me=true, observe_others=true)`。
- **messages**：6 + 2 条（第一 session 记「喜欢紫色 / 小狗豆豆」，第二个新 session 问「我最喜欢的颜色」答出紫色 —— 跨会话记忆实证）。

**结论：探针验证的是「设备级 session + 家庭单 owner 画像」，不是「设备级 peer」。** debt 证据 7 里写的 `openclawSessionKey=agent:kidpal:openai-user:device-probe-honcho` 是 **Honcho session 的 metadata / OpenClaw 的 session key**，不是 peer 键。K3 冻结件与 arch §6.7 把这两个概念混成了一个（详见 §5、§6）。

### 1.4 探针轮到底"怎么接的"——与 debt 证据 7 的对账

| debt 证据 7 原话 | 核对结果 |
|---|---|
| 「`hooks.allowConversationAccess=true` 后…一次性路径钩子正常触发 → 消息入库」 | **字面缩略**：真实路径是 `plugins.entries.openclaw-honcho.hooks.allowConversationAccess`。探针配置**从未**出现顶层 `hooks.allowConversationAccess`。 |
| 「映射 `openclawSessionKey=agent:kidpal:openai-user:device-probe-honcho`」 | **正确**，且是 session key（DB metadata 原文），不是 peer 键。 |
| 「`before_prompt_build` 注入生效」 | **正确**，插件 `dist/hooks/context.js` 注册 `before_prompt_build`，返回 `appendSystemContext`。 |
| 「配置要点 `EMBEDDING_VECTOR_DIMENSIONS=1536` / `DERIVER_FLUSH_ENABLED=true`」 | **正确**，二者均在 `/opt/honcho/.env`（值不外泄）。 |

矛盾来源判定：**不是版本差异、不是两套机制**，是 debt 记录用了缩略写法、K3 把缩略写法当字面路径抄进了声明工件。

---

## 2. K3 三处失效的根因（一次说清）

1. **声明路径错（生产崩循环的直接原因）**。`honcho-plugin-declaration.json` 写 `hooks.allowConversationAccess`（顶层）+ `agents.entries.kidpal.plugins.honcho`（agent 内嵌）。2026.8.1 的 zod schema 对两处都 `strictObject` 拒收。离线复现（只读，用 `OPENCLAW_CONFIG_PATH` 指向临时副本）：

   ```
   OpenClaw config is invalid: /tmp/cfg-k3-shape.json
     × agents.entries.kidpal: Unrecognized key: "plugins"
     × hooks: Unrecognized key: "allowConversationAccess"
   ```
   同一份生产配置叠加**探针形状**则 `Config valid`（仅提示 `plugin not found`，插件未安装时的正常 stale 警告）。→ 网关 validator 拒收 → 进程 exit 78 → `systemd Restart=on-failure RestartSec=5` 崩循环。

2. **carrier 不是插件**。`server/deploy/261022-openclaw-honcho-plugin/honcho-plugin.js` 是一个 `module.exports = { init, persist, inject, qualifiedPeer, honchoReachable }` 的普通 CommonJS 模块：没有 `definePluginEntry`、没有 `id`、没有 `register(api)`、没有 `openclaw.plugin.json`、没有任何 hooks 注册。即便配置写对，它也无法被 OpenClaw 加载。

3. **它调用的 Honcho 端点不存在**。carrier 向 `http://127.0.0.1:18100/v1/openclaw/persist` 与 `/v1/openclaw/inject` 发 POST；Honcho 源码 `/opt/honcho/src/main.py` 的路由前缀是 `/v3`（workspaces / peers / sessions / scopes / messages / conclusions / keys / webhooks），`grep -rn "openclaw" /opt/honcho/src` 零命中。→ 即使能加载，也是 404。

**判定：K3 自研 carrier 弃用（不是重写）。** 可保留的只有"意图"（资格门），而该意图上游插件不提供（见 §4）。

---

## 3. 上游自带插件盘点（题面第 2 项）

`/opt/honcho` 源码树里两个 Honcho 自家插件，**都不是 OpenClaw 插件**：

| 目录 | 接的框架 | 机制 | 能否用于 kidpal agent |
|---|---|---|---|
| `hermes-plugin-honcho/` | **Hermes**（Python agent 框架） | `pip install honcho-ai` + `hermes memory setup honcho`；memory provider 形态，`plugin.yaml` + Python 模块；上下文注入到 user message（保 prompt cache） | **不能**。框架不同，OpenClaw 不加载 Python memory provider。 |
| `kilo-honcho/` | **Kilo Code**（`kilo serve`） | `kilo plugin @honcho-ai/kilo-honcho`；`~/.honcho/config.json` + `hosts.kilo`；OpenCode 血统的 hook 机制 | **不能**。同上。 |
| （不在源码树）`@honcho-ai/openclaw-honcho@1.7.0`（npm） | **OpenClaw** | `openclaw plugins install` + `plugins.entries` + 插件 SDK hooks | **能，且已经验证过** —— 即探针轮用的那个。 |

资格门（user-present-only）三家**都没有**：`openclaw-honcho` 的 capture 只排除 cron/心跳/系统回合（`helpers.js isSystemRun` + `captureSystemRuns=false` 默认），对"没有 user 字段的一次性回合"与"QQ 家长回合"没有过滤。Hermes/Kilo 是另外的隐私模型（peer/workspace 隔离），与本仓 §6.7 不通用。

---

## 4. OpenClaw 2026.8.1 的定制/注册面（题面第 3 项）

- **安装法**（`openclaw plugins --help` / `install --help`）：`install <path|npm spec|git|clawhub|marketplace>`，支持 `--pin`（记录精确版本）、`--force`、`-l/--link`（本地路径链接）、`--accept-capabilities`。`plugins` 还有 `enable/disable/doctor/inspect --runtime/list/registry/update/validate`。
- **memory 槽**：`memory-core` 是 **bundled（内置）** 插件（`Source: stock:…/dist/extensions/memory-core/index.js`、`Origin: bundled`、`Status: loaded`），生产 `plugins.slots` 未设 → 默认由 memory-core 持有 memory 槽。上游 honcho 插件 1.7.0 **明确不抢 memory 槽**（README「Running alongside memory-core」），以免与内置记忆打架。
- **path 型注册先例**：schema 有 `plugins.load.paths: string[]`（本地插件目录加载），生产 `openclaw.json` 的 `plugins.entries` **没有** path 型先例——现有条目全是 `{enabled, config}` 形态（deepseek / qqbot / tavily / browser / openclaw-qqbot）。所以真要落地"自研插件"，正路是 `plugins install <path>`（或 `plugins.load.paths`），不是往条目里塞 `carrier` 路径。

---

## 5. 与 §6.7 的冲突面（必须让 human 看见的硬事实）

`docs/architecture.md` §6.7 的资格门原文：**「仅当回合来自 kidpal agent 的语音链路、且请求携带 `user=<device_id>` 时…；读侧同门…；资格门全局生效（不只 kidpal）」**，六行表逐条点名 front 一次性回合 / cron / QQ 家长回合均不入库。

**已验证的接线做不到这一条**（不是配置调错，是插件没有这个能力）：

| §6.7 行 | 已验证接线的实际行为 | 差在哪 |
|---|---|---|
| 1/2 front 卡壳引导 / 忠实整理（一次性，无 `user`） | `openai-http` 路径给无 `user` 的请求生成随机 sessionKey（`resolveSessionKey`：`user ? prefix-user:user : prefix:randomUUID()`），插件照常 `agent_end` 入库；`before_prompt_build` 也照常注入画像 | **写侧与读侧都漏**。整理回合被注入"孩子画像"，还有把记忆细节写进"忠实整理"产物的质量风险 |
| 3 cron → main | **已排除**（cron session key 分类 + `captureSystemRuns=false`） | 合规 |
| 5 QQ 家长 → main | 照常入库；QQ 有 `senderId` → 新装默认 `per-sender` 策略会给家长单独 peer（不污染 owner 画像），但消息仍落库，且插件给 agent 注册了 `honcho_search_messages`（跨 session 搜索） | **落消息层面漏**；画像层面基本无污染（`ownerObserveOthers=false` 默认） |
| 设备语音（携 `user`） | 入库，peer = `owner`（**不是** device 级 peer） | 与 arch §1.3「设备级 peer 先行（换设备 = 新 peer）」不符：实际是**设备级 session + 家庭单 owner 画像** |

另有一条本 agent 无法离线证实的项：P1 的 `agents.entries.kidpal.tools.allow=[read,write,exec,web_search]` 是否也过滤**插件注册的** `honcho_*` 工具。代码里 `tools.allow` 走 built-in factory allowlist，插件工具有独立的 `pluginToolAllowlist` 通道，**很可能不受 `tools.allow` 约束** → 部署轮必须实测一次（见 §8 OQ-3）。若确实不受约束，孩子可以通过 `honcho_search_messages` 翻到家长 QQ 内容 —— 这条必须挡。

---

## 6. 方案对比（按 human 原则排序）

### 甲 · 原样复用探针接线（**推荐**）

改动面：
- 生产执行 `openclaw plugins install @honcho-ai/openclaw-honcho --pin`（registry 已是 npmmirror，探针当日成功）。
- 用 `openclaw config patch --dry-run` → 写 4 个键，`workspaceId` 取生产值（建议 `kidpal`，不复用探针的 `kidpal-probe` 测试数据）。
- 声明工件（`honcho-plugin-declaration.json`）改成 4 键；`apply_honcho_plugin.py` 的点分写入器**可以保留**（它只是写键，错的从来不是它）；`honcho-plugin.js` 与它在 `manifest.yaml` 的条目**删除**。
- 冻结验收锚换代（§7）。

资格门怎么保：
- **零代码收敛**：`config.noisePatterns`（上游已文档化、schema 内、探针同版本可验）加两条锚定正则 `/^孩子(到目前为止)?讲的内容：/`，把 front 两条一次性 prompt 的 **user 消息**挡在库外；残余是 assistant 回复单条（低害：引导句不含隐私；整理产物是孩子自己的故事）。
- **兜底**：`captureSystemRuns=false`（默认）已挡 cron/心跳。
- **挡不住的**：整理/引导回合的**画像注入**（读侧），与 QQ 回合的落消息。这两条写进 spec 偏差申报（见下）。

冻结锚换代：见 §7（必做，否则验收与现实继续对不上）。

风险：① 资质门字面不满（隐私承诺降级，须 human 签字）② 整理回合可能被画像"带跑"（faithful 承诺，需真机抽检）③ QQ 消息落库 + 可能可搜（须 OQ-3 实测）。

### 甲+ · 原样复用 + 上游插件最小补丁（备选，仅在 1/2 不可接受时）

改动面：把插件 vendor 进仓（`plugins install <本地路径>`），在 `dist/hooks/capture.js` 与 `context.js` 各加一个判据：`sessionKey` 不含 `:openai-user:` → 直接 return。约 10 行。
资格门：**完整满足**（写侧 + 读侧同时挡 front 一次性回合；QQ 回合仍会入库——`main` agent 的 QQ sessionKey 也不含 `openai-user`，需确认并一并挡）。
风险：**这是唯一需要新写代码的选项**；`openclaw plugins update` 会覆盖补丁 → 需要与 engine-patch 同款的漂移报警锚（ARQ-260907 同族痛点，前车之鉴）。
标注：属于"对已验证物做最小修改"，不是新发明；但仍是本批唯一的新代码面。

### 乙 · 用上游自带 honcho 插件

`hermes-plugin-honcho` / `kilo-honcho` 是 Hermes / Kilo Code 的插件，**不适用 OpenClaw**，**未验证**、也无法验证。**否**。

### 丙 · 修 K3 自研 carrier

三层失效（配置形状 / 不是插件 / 端点不存在）——修它等于从零写一个 OpenClaw 插件 + 重实现 persist/inject，**继续发明**，明确违反 human 原则。**否**。

### 排序

| 案 | 合规度 | 新增代码 | 违反 human 原则？ | 排序 |
|---|---|---|---|---|
| 甲（+noisePatterns） | §6.7 字面 80%（写侧 front 挡住；读侧与 QQ 剩残余） | 0 | 否 | **1** |
| 甲+（+最小补丁） | §6.7 字面 ~100% | ~10 行 | 轻微（改已验证物） | 2 |
| 乙 | — | — | — | 不可用 |
| 丙 | — | 大量 | 是 | 最低 |

**推荐：甲。** 理由正是 human 原则——先把验证过的东西跑起来，残余偏差如实记账，再用实测决定要不要补丁。**但甲必须配一次 `/arch-change` 把 §6.7 的口径改写成"已验证行为"，否则就是拿一个永不满足的冻结验收骗自己。**

---

## 7. 冻结锚换代清单（`server/acceptance/test_261022_memory_isolation.acceptance.py`）

先说结论：**能原样活下来的只有 4 条**，其余围绕 carrier 与顶层 hooks 的锚全部作废。

**存活（语义不变，docstring 需改述）：**

- `test_k3_e2_front_oneshot_payload_no_user`（L416-475）：front payload 键集 `{model,messages,stream}`，与本次接线无关，保持 must-stay-green。
- `test_k3_e6_declaration_schema_and_p1_drift_anchor` 的**结构部分**：`case/changes/drift_guard/backup/rollback` 五键、`changes≥2`、`drift_guard` 含 P1 `tools.allow` 四项现状锚。
  - 其中 `path.endswith("hooks.allowConversationAccess")` **碰巧仍然成立**（新路径 `plugins.entries.openclaw-honcho.hooks.allowConversationAccess` 也以它结尾）——但**必须改注释**，否则下一个人又会以为是顶层键。
  - `path 含 "honcho" 且 value 含 "127.0.0.1:18100"` 仍然成立（`config.baseUrl=http://127.0.0.1:18100`）。
- `test_k3_e6_apply_idempotent_*` / `drift_guard_exit3` / `timestamped_backup` / `rollback_*`：applier 语义没错，**保留**（只需换 declaration 内容）。
- `test_k3_e6_no_openclaw_json_in_repo_and_no_secret`：保留。

**必须换代（当前锚钉的是发明物）：**

| 位置 | 现锚 | 换代后 |
|---|---|---|
| L220 `K3_GATE_MARKER` = `"kidpal honcho memory patch active"` | carrier 自打 marker | 插件加载日志行 `Honcho memory plugin loaded`（`dist/index.js` 原文）+ `plugins inspect --runtime` 的 `Policy: allowConversationAccess: true` |
| L221-222 `K3_PEER_KEY_PREFIX` / `K3_PEER_KEY_VARIANT_RE` | 自造 peer 前缀 | 改述为「**session key** 家族 `agent:kidpal:openai-user:<device_id>`」（并在 arch 里同步纠正"peer"用词）；保留"不另造键"的原意但指向 session key |
| L223 `K3_HOOKS_PATH_SUFFIX` | 顶层 hooks 后缀 | 全路径 `plugins.entries.openclaw-honcho.hooks.allowConversationAccess` |
| L227 `K3_PROBE_ANCHORS`（healthz//health/readyz） | carrier 探针门控 | 删；可达性由 live 证据（Honcho `/health` 200）+ 降级实测承载 |
| L228-229 `K3_DECL_NAME` / `K3_WORKBENCH_TOKEN` | 工作台三件 | 保留 declaration/applier；carrier 相关整体删 |
| L477-523 `test_k3_e2_gate_carrier_anchors` | marker + peer + 探针锚集 | 重写为「声明工件 4 键逐字 + 插件版本 pin 记录（1.7.0）+ 无自研 carrier 残留」静态断言 |
| L525-554 `test_k3_e5_degradation_guards` | carrier 里 `catch`/`while(true)` | 改测**上游插件**的 bounded 失败：`dist/hooks/capture.js` 的 try/catch + 日志，或直接降级为 live 实测（停 Honcho 容器 → 对话照常） |
| L864-897 `test_k3_e6_manifest_registers_the_workbench`（≥3 条含 carrier） | carrier 入 manifest | 降到 ≥2 条（decl + applier + rollback），carrier 条目删除 |
| L919-923 live 证据 schema 里的 `marker` 字段 | 自造 marker 双证 | 换成「session metadata `openclawSessionKey` + 消息条数 + 网关日志」三证；`one_shot` 段改为「一次性回合零 session / 零消息」（甲案下只对 user 消息成立 → 判定口径必须写死） |
| 头注 B.4 / C 节 | 资格门 carrier 三锚集 | 重写为「已验证接线四键 + 残余偏差申报」 |

**新增断言建议（种子见 §8）**：声明与探针对齐（4 键）、carrier 及 manifest 条目零残留、插件版本恰 1.7.0。

---

## 8. 推荐案 shaping（交 human / planner 排期）

> 本 agent 不起草 TaskPackage、不定档；以下为 shaping 意见。

**分档建议：三件两批**

1. **批 A（nano，已在手）**：`server/deploy/honcho/docker-compose.yml` 拓扑回流。工作区已有改动（125+/84−，头注写明改回 2026-10-22 探针拓扑），ECS 已热修、五容器 healthy。**只差回流 + 记账**，不掺别的。
2. **批 B（Lite）**：生产接线 bring-up（甲案）。
3. **面 C（`/arch-change`）**：§6.7 与 §1.3 的口径修正（device 级 peer → device 级 session / 家庭单 owner；资格门改写为已验证行为 + 残余申报）。可与批 B 并行，但**必须同批或先于验收重冻**。

**批 B changeManifest 种子（仓根相对）：**

```
server/deploy/261022-openclaw-honcho-plugin/honcho-plugin-declaration.json   # 改为 4 键
server/deploy/261022-openclaw-honcho-plugin/apply_honcho_plugin.py          # 多数不动；路径/注释更新
server/deploy/261022-openclaw-honcho-plugin/rollback_honcho_plugin.py        # 保留
server/deploy/261022-openclaw-honcho-plugin/honcho-plugin.js                 # 删除
server/deploy/manifest.yaml                                                  # carrier 条目删除
server/acceptance/test_261022_memory_isolation.acceptance.py                 # 锚换代（§7）
```

**验收断言种子：**

- **host（离线）**：declaration 恰含 4 键、`plugins.entries.openclaw-honcho.*` 全路径、`config.baseUrl == "http://127.0.0.1:18100"`、`config.workspaceId` 非 `kidpal-probe`；`changes` 不含顶层 `hooks.*`、不含 `agents.entries.*.plugins`；仓库/工作台零 carrier 残留；manifest 无 carrier 条目；plugin 版本 pin = `1.7.0` 有记录。
- **live（部署轮，结构化证据落盘）**：① 网关日志含 `Honcho memory plugin loaded`，`plugins inspect --runtime` 为 loaded；② 真机一轮语音 → Honcho DB 新增 session，metadata `openclawSessionKey` 前缀 `agent:kidpal:openai-user:`；③ 换新 session 问「我最喜欢的颜色」召回已验证事实；④ front 引导/整理各一轮 → DB 增量符合所选口径（甲：不得出现 user 原文；甲+：不得出现任何行）；⑤ 停 Honcho 容器 → 对话照常、无重试风暴、无用户可见报错；⑥ `tools` 面实测 `honcho_*` 是否暴露（OQ-3）。

**回滚方式（三步，任一级可用）：**

1. `openclaw plugins disable openclaw-honcho && systemctl restart openclaw-gateway`（配置留着，插件不加载）；
2. 还原 `openclaw.json.bak-kidpal-honcho-*` 时间戳备份（现有 `rollback_honcho_plugin.py` 可跑）；
3. `openclaw plugins uninstall openclaw-honcho`（连 npm project 一起清）。

**风险与观察项：** 整理回合的画像注入是否影响"忠实"（真机抽检 3-5 例）；QQ 回合落库与可搜性（OQ-3）；ECS 内存（2C/4G，OpenClaw 网关单进程实测 ~650MB，Honcho 五容器另计，本批后复测 `TECH-261022-openclaw-prep-tax-memory-pressure`）。

---

## 9. 网关安全边界（不再炸的验证序）

**崩循环的真实机理**（生产刚发生过一轮）：K3 的 `apply_honcho_plugin.py` **直接写文件**，绕过了 OpenClaw 的 schema 校验；网关重载时 zod `strictObject` 拒收未知键 → 进程退出 78 → systemd `Restart=on-failure RestartSec=5` → 循环。

**铁律：openclaw.json 只经 OpenClaw 自己的写入口写，写前必须 `--dry-run` 验 schema。**

已验证存在的安全工具（生产实测）：

| 工具 | 作用 | 实测 |
|---|---|---|
| `openclaw config validate` | 不起网关，按 schema 校验当前配置 | 生产当前 `Config valid` |
| `openclaw config validate --json` | 机读结果 | 可用 |
| `openclaw config patch --file <p> --dry-run` / `--stdin --dry-run [--json]` | **校验一次写入但不落盘** | 用探针形状对生产跑：`ok:true, operations:4, checks.schema:true`（生产配置未被改动） |
| `openclaw config set <path> <val> --dry-run` | 单键 dry-run | 可用 |
| `openclaw plugins doctor` | 插件发现/加载/兼容性体检 | 生产当前通过 |
| `openclaw plugins inspect <id> --runtime` | 真加载一次，报 Status/hooks/tools/Policy | 探针实测 loaded |
| `openclaw health` | 查**运行中**网关的运行时隔离/回退 | systemd 网关在跑 |

**推荐应用序（每一步失败即停，回到上一步）：**

1. 备份：`cp openclaw.json openclaw.json.bak-kidpal-honcho-$(date -u +%Y%m%dT%H%M%S)`。
2. 把 4 键写成 JSON5 patch 文件；`openclaw config patch --file <p> --dry-run --json` → 必须 `schema:true`。
3. **先装插件**：`openclaw plugins install @honcho-ai/openclaw-honcho --pin`（顺序原因：配置里有条目但插件未装时 validate 会给 `plugin not found … stale config entry ignored`）。装完再跑一次 `openclaw config validate`。
4. `openclaw plugins inspect openclaw-honcho --runtime` → `Status: loaded` + `Policy: allowConversationAccess: true`；`openclaw plugins doctor` 绿。
5. 正式 apply（`config patch --file`，它会自带校验）；再 `openclaw config validate` 绿。
6. `systemctl restart openclaw-gateway`；`sleep 5; openclaw health`；`journalctl -u openclaw-gateway -n 100` 找 `Honcho memory plugin loaded`，**同时确认进程没有在 5 秒级重启**（`systemctl show -p NRestarts openclaw-gateway`）。
7. 任一步异常 → 立即 `openclaw plugins disable openclaw-honcho && systemctl restart`；必要时还原备份。

**绝对不要做的三件事：**

- 不要手写/脚本直写 `openclaw.json`（K3 的崩因）；
- 不要写顶层 `hooks.allowConversationAccess`（schema 拒收，已验证）；
- 不要在未 `--dry-run` 的情况下用 `config patch` 一次改多个不相关键（失败会连带回滚，排障困难）。

---

## 10. 需要 human 拍板的问题

1. **【核心】甲（原样复用 + noisePatterns，改 spec 口径）还是 甲+（原样复用 + 上游插件最小补丁，保 §6.7 原字面）？** 本 agent 推荐甲；若选甲，须同批走 `/arch-change` 把 §6.7 / §1.3 改述为已验证行为，并如实写残余（front 整理回合的画像注入、QQ 回合落消息）。
2. **生产 `workspaceId` 取什么？** 建议 `kidpal`（探针的 `kidpal-probe` 是带「（测试标记）」的测试数据，不建议迁移）。
3. **`honcho_*` 工具是否需要从 kidpal agent 挡掉？**（OQ-3 实测后定；若 `tools.allow` 管不住，建议 `tools.deny: ["honcho_search_messages"]` 或等价，防孩子搜到家长 QQ 内容。）
4. **K3 冻结验收重冻走哪条路？** 建议作为批 B 同包的「锚换代」（新 TaskPackage 承载），不走「静默改冻结件」。
5. **批 A（compose 回流）是否单独先合？** 建议是——它已经生效于 ECS，与记忆接线零耦合，先落可减少在途面。

---

## 附：本次检视的只读证据清单

- ECS 只读命令：`openclaw --version`、`openclaw plugins list/doctor/inspect --runtime`、`openclaw config validate/patch --dry-run/get`、`python3 /tmp/dumpkeys.py`（自写只读结构转储，secret 值已遮盖）、`docker exec honcho-database-1 psql -c "select …"`（workspaces / peers / sessions / session_peers / messages）、`journalctl -u openclaw-gateway`、`/opt/honcho/.env` 键名核对（值不外泄）。
- 仓内只读：`docs/architecture.md` §1.3/§6.7、`docs/requirements.md` S-F5.1（经 §6.7 引用）、`docs/debt/tech/TECH-261022-device-memory-prompt-overhaul.md`、`server/acceptance/test_261022_memory_isolation.acceptance.py`、`server/deploy/261022-openclaw-honcho-plugin/**`、`server/deploy/manifest.yaml`、`server/gateway/provider.py`、`server/gateway/front/front_app.py`、`delivery/taskpackages/261022-m5-k3-honcho-memory-production/**`。
- 未做：任何写操作（生产与测试 profile 均未写）、任何 gateway 重启、任何插件安装、任何 canonical 文件改动。
