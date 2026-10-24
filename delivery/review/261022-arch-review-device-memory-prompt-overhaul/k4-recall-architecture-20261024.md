# K4「历史召回不动」问题 · 架构分析（2026-10-24）

> **性质声明**：本文件是 architect advisory 意见，非冻结制品、非评审记录、非验收证据；不豁免任何门、不构成通过判据。human 明示要求本件；**零 canonical 写入**：仓内与 ECS 全程只读（PostgreSQL 只读 SELECT、Honcho REST 只读 GET/检索 POST、源码 grep/读、journal/docker logs 读），未改任何配置、未写任何库、未重启任何服务、未动 `docs/**` 与 `server/**`。
>
> **基线**：ECS `/opt/honcho` = `b94a416c`（2026-10-21，v3.4.0）；本地参考克隆 `/c/tmp/honcho-upstream-ref/honcho` = `7478be29`（2026-10-23），两者被引用函数逐条核对存在且语义一致（ECS 行号略偏移）。插件 = `@honcho-ai/openclaw-honcho@1.7.0`；OpenClaw = 2026.8.1 (ea80657)。工作区 = Honcho workspace `kidpal`。
>
> **引用简写**：`PLUG` = `/root/.openclaw/npm/projects/honcho-ai-openclaw-honcho-00035f07ba/node_modules/@honcho-ai/openclaw-honcho/dist/`；`HON` = `/opt/honcho/src/`；仓内路径以仓根相对书写。

---

## 0. 一句话结论

1. **问题不是「backfill 在独立会话」，而是两条结构叠加**：(a) backfill 会话里 `agent-kidpal` 成员的 `observe_others` 是 `null`，deriver 从未以 agent 视角产出过 `(observer=agent-kidpal, observed=owner)` 的结论——库里这类结论 **0 条**；(b) 被动注入的「用户画像」= 该 pair 的**最近 100 条结论**窗口（`WORKING_REPRESENTATION_MAX_OBSERVATIONS=100`，按结论创建时间倒序），实测该窗口（owner 自身视角、391 条结论）只覆盖 **2026-10-13 至 2026-10-24**，9/30 的「王一诺」事实在窗口之外。
2. **所以「再重灌」不是答案，而且重灌也答不出**：即便把 2879 条灌进主会话，让 agent 视角产出结论，被动窗口仍只保留最近 100 条，9 月事实会被更晚的结论挤出——这与今天 owner 自身视角的实测完全同构（该视角已经有 344 条 backfill 结论，但 recent-100 里没有王一诺）。重灌只会新增一次 3 小时级 LLM 重派生、丢掉批次隔离、并且仍然通不过同一句测试。
3. **今天就已经可用的路径是检索**：`honcho_search_conclusions` 查的是 `owner` 自身视角（`(owner, owner)` 集合），该集合**已经**包含 backfill 的 344 条结论；实测以工具默认参数（`maxDistance=0.5`）查询「王一诺是谁」，第一条结果就是 2026-09-30 的原始事实。消息级检索同样命中（`POST /peers/owner/search` 直接返回「今天早上我出卖了王一诺。」）。
4. **但检索今天被三重锁死**：① kidpal agent 的 `tools.allow` 是四项白名单，插件注册的 `honcho_*` 五个工具被过滤掉；② `AGENTS.md` 明确规定孩子对话回合「不调用任何工具」，记忆守则还写「不需要调用任何工具」；③ 工具不可用时插件连工具指引段落都不注入。模型只能按守则回答「记不清了」——它的行为**完全符合当前指令**。
5. **长稳解是分层，不是移数据**：稳定身份事实走 **peer card**（每轮必注入、不受窗口影响）；历史细节走 **检索**（一次工具调用）；被动窗口只承担「近期上下文」。若要求「不调用工具也必须记得」，唯一可靠载体是 card（人工/受控生成），不是画像窗口。

---

## A. 召回链路完整测绘

### A.1 被动注入的那一次调用，逐参读

插件在每轮 `before_prompt_build` 发起（`PLUG/hooks/context.js:42-50`）：

```js
context = await session.context({
  summary: true,
  tokens: 2000,
  peerTarget: participantPeer,          // 实测 = owner
  ...(recall.scope ? {} : { peerPerspective: agentPeer }),  // 实测 = agent-kidpal
  ...recall,                            // recall.automatic 缺省 "workspace" → {}
});
```

生产配置（`/root/.openclaw/openclaw.json`）只有 `baseUrl` / `workspaceId` / `noisePatterns` / `hooks.allowConversationAccess` 四个键，**`recall` 未配置**。按 `PLUG/config.js:97-100` 缺省解析为 `{automatic:"workspace", ask:"workspace", scopeName:undefined}`；`PLUG/helpers.js:264-269` 中 `workspace` 分支返回 `{}`——即**不带 `limitToSession`**。由此：

- 服务端视角：`observer = peer_perspective = agent-kidpal`，`observed = peer_target = owner`（`HON/routers/sessions.py`：`observer = peer_perspective or peer_target; observed = peer_target`）。
- 读的是 `(agent-kidpal, owner)` 这个 collection 的 working representation，**不限会话**（workspace 级）。
- `tokens=2000` **不限制 representation**：服务端先无条件取 representation 与 card，再用 `adjusted_limit = 2000 − estimate_tokens(representation) − estimate_tokens(card)` 去分配 summary（40%）+ 最近消息（60%）（`HON/routers/sessions.py:1043-1050`）。实测 representation 4370 字符时 `messages=3`、`summary=null`。
- 返回三段被拼进 `appendSystemContext`（`PLUG/hooks/context.js:61-69`）：`Key facts:`（peerCard）+ `User context:`（peerRepresentation）+ `Earlier in this conversation:`（session summary）。

### A.2 三段各自「从哪来、看得见哪些会话」

| 段 | 来源与重算时机 | 数据范围（实测/代码） | 为什么现在是空的/不空 |
|---|---|---|---|
| **peerCard** | 存在 observer 的 `internal_metadata`，键 `owner_peer_card`（`HON/crud/peer_card.py:123-126`）；只由两条路径写：`PUT /v3/workspaces/{ws}/peers/{observer}/card?target=observed`（`HON/routers/peers.py:629-661`），或 `card_refresh` dream（`HON/dreamer/orchestrator.py:414-565`，受 `configuration.peer_card.create` 门控）。**omni dream 不刷新 card**（omni 分支只跑 deduction + induction，`orchestrator.py:623-640`）。 | 按 `(workspace, observer, observed)` 键，**无会话维度**；每次注入全量、不截断。默认 `PEER_CARD.ENABLED=true`（`HON/config.py:1015-1019`），`use/create` 的解析缺省都取它（`HON/utils/config_helpers.py:140-143`）。 | 库里 `peers.internal_metadata` 两个 peer 都是 `{}`；`queue` 里 `task_type='dream'` 只有两条 `omni`，**没有任何 `card_refresh` 行**。所以 card 从未生成，`context.peer_card=None`（实测）。 |
| **peerRepresentation** | `(observer, observed)` collection 里的 documents（结论）。无 search_query 时 = **最近 100 条、按 `documents.created_at` 倒序**（`HON/crud/representation.py:486-528`；上限 `HON/config.py:961-964` 默认 100）。结论由 deriver 的 representation 工作单元从消息抽取，按「谁能观察谁」扇出。 | **跨会话**（collection 无会话维度；`limitToSession` 才是收窄开关，只有 `honcho_session` 工具用）。但对 `(agent-kidpal, owner)` 这个 pair，只有 live 会话有 47 条结论；backfill 会话 **0 条**。 | 实测插件式请求返回 4370 字符、`sections=1`、`summary=false`、`card=null`；日志 `[honcho-diag] sections=1 lens=[4238] wangyinnuo=no`（2026-10-24 22:48:53），内容全是 10-23/10-24 的小猫、方可馨、西安等 live 事实。 |
| **summary** | 每个会话自己的滚动摘要：消息序号每 20 条出短摘要、每 60 条出长摘要（`HON/config.py:1223-1224`），每次取最近 20/60 条消息 + 上一条摘要，存进 `sessions.internal_metadata.summaries`（`HON/utils/summarizer.py:478-630`）。注入时**只读当前 session**（`_get_both_summaries_task(db, ws, session_id)`），并按 40/60 预算择优（`_select_summary_for_context`，`HON/routers/sessions.py:217-263`）。 | 单会话；不含其它会话的摘要。 | 全部 6 个 kidpal 会话的 `internal_metadata` 都是 `{}`——**一条摘要都没存下来**。原因见 A.3 的 deriver 失败面：摘要 LLM 调用超时 → 走 `_basic_fallback_summary` → 而代码只在 `if not is_fallback:` 时才 `_save_summary`（`HON/utils/summarizer.py:584-607`），**降级摘要永远不落库**；异常在 `_create_summary` 内被吞（`summarizer.py:763-772`），队列行仍标 `processed=true` 且 `error` 为空。当前容器日志：`Error generating summary!` **184 次**、`Request timed out` **687 次**。 |

### A.3 关键推论：为什么「独立会话」不是主因

- 结论存储是 **pair 级 collection**，不是会话级；representation 默认跨会话。若把 backfill 的 `(owner, owner)` 结论直接拿 owner 视角读，服务端确实能把它们端出来——实测 `peer_target=owner`（无 perspective）返回 10700 字符、100 条，窗口日期跨度 `2026-10-13 … 2026-10-24`，证明**跨会话是通的**，不通的是「agent 视角没有这批结论」+「窗口只留最近 100 条」。
- 真正的分叉发生在 deriver 的扇出条件：一条消息的观察者 = 发送者自己（若 `observe_me`）+ 会话内 `observe_others=true` 的其它成员（`HON/deriver/enqueue.py:344-372`；`get_effective_observe_me` 在 `:257-291`，会话配置优先、缺省 True）。backfill 会话的 `session_peers.configuration` 实测为：

```
agent-kidpal | {"observe_me": null, "observe_others": null}
owner        | {"observe_me": null, "observe_others": null}
```

  `null` 不等于 true，`if session_peer_config is None or not session_peer_config.observe_others: continue` → agent-kidpal 永远不会成为 owner 消息的观察者。会话成员是怎么变成这样的：回灌脚本 `create_session` 只发 `"peers": {owner: {}}`（`server/deploy/honcho/backfill/backfill_honcho.py:659-668`），agent-kidpal 是写 assistant 消息时被服务端 get-or-create 顺手加进来的，配置为空。

- 库内分布与代码预测**逐条吻合**（backfill 会话 2879 条消息 / 1422 条结论）：

| pair | backfill 会话 | live 会话 | 说明 |
|---|---|---|---|
| owner → owner | 344 | 47 | 自身视角，正常 |
| agent-kidpal → agent-kidpal | 1078 | 199 | 小伴自己的话，正常 |
| **agent-kidpal → owner** | **0** | **47** | 注入读的就是这一格；backfill 全空 |

### A.4 `recall.automatic="workspace"` 与 `scopeName` 到底改变什么（实读）

- `workspace`（缺省）：`sessionRecallOptions` 返回 `{}`，不传 `limitToSession`，representation 覆盖该 pair 的所有会话。**今天就是这个模式。**
- `session`：传 `limitToSession:true`，服务端把 representation 收窄到当前会话，并因 allowlist 规则**丢掉 peer card**（`HON/routers/sessions.py:1000-1035`）。`honcho_session` 工具用这个，所以它「只看当前会话」。
- `scope`：传 `{scope: name}`，服务端把 observer 换成 scope peer（`HON/routers/sessions.py`：`if scope is not None: [observer] = await crud.resolve_scope_peers(...)`），读的是 `(scope_peer, owner)` 集合；插件同时不再传 `peerPerspective`（两者互斥）。**`scopeName` 只有在 `automatic="scope"` 时生效**；单配 scopeName 无效（`PLUG/config.js:91-96`）。
- 结论：**workspace 并不能「让召回跨会话」——它本来就是跨会话的；它也不能凭空造出 agent 从未观察过的结论。** scope 能改变「谁来观察」，是下面选项 2 里唯一不改数据的机制，但同样受 recent-100 窗口与「丢掉 agent 视角」的代价约束。

### A.5 Honcho 侧其它跨会话注入/检索面（能力边界、默认开关、自托管可用性）

| 面 | 端点/机制 | 数据范围 | 默认/开关 | 自托管可用性 | 对本问题的评价 |
|---|---|---|---|---|---|
| 会话上下文 | `GET /sessions/{id}/context` | 当前会话 summary + 最近消息 + 指定 perspective 的 representation/card | 无开关 | 可用 | 插件在用的就是它 |
| peer representation | `POST /peers/{peer}/representation` | 该 peer 作为 observer 的自我视角；带 `target` 则是指定视角；支持 `search_query` / `include_most_frequent` / `session_id` / `scope` | 无开关 | 可用 | **`honcho_search_conclusions` 走这里，查 owner 自身视角，已含 backfill** |
| dialectic | `POST /peers/{peer}/chat` | `observer → observed` 的结论 + 推理工具；**预取与 `search_memory` 固定 agent 视角**；`search_messages` 按观察者成员关系可触及 backfill 会话原始消息（`HON/utils/agent_tools.py:670-700, 1662-1717, 1812-1833`；`crud.search_messages` 的 observer 作用域） | `honcho_ask` 用 `agentPeer.chat(target=owner)` | 可用（DeepSeek 各 reasoning level 已配） | 预取与结论检索**看不到 backfill**；只有模型主动调 `search_messages` 才可能碰到，**不是确定性召回面**，不作为解法 |
| 消息检索 | 会话级 `POST /sessions/{id}/search`；peer 级 `POST /peers/{peer}/search`；workspace 级 `POST /workspaces/{ws}/search` | 原始消息全文/语义；跨会话 | 插件工具 `honcho_search_messages`，`crossSessionSearch` 缺省 true | 可用 | **实测命中** 9/30 原句；但返回原话、可能含家长代述与流程性内容，儿童端不宜直接开放 |
| peer card | `GET/PUT /peers/{observer}/card?target=` | 人工/梦生成的稳定事实列表 | `PEER_CARD_ENABLED=true`；omni dream 不刷新 | 可用 | **每轮必注入**，是唯一不受窗口影响的被动通道；目前为空 |
| scope | `POST /scopes`、`POST /scopes/{id}/sessions`、`GET /scopes/{id}/status` | scope 作为静默观察者；加入已有消息的会话会触发 **零 LLM 的结论复制** 到 `(scope, P)`（`HON/deriver/scope_backfill.py:1-24`） | 需 workspace 级 key（本机鉴权关闭，实测 200） | 可用 | 能把 backfill 的 `(owner,owner)` 344 条复制进 scope 视角；但 recent-100 仍会裁掉 9 月；且注入从 agent 视角切换为 scope 视角 |
| conclusions API | `POST/GET/DELETE /workspaces/{ws}/conclusions[/{id}]` | 任意 `observer_id/observed_id/session_id` 的 explicit 结论；只做 embedding，不做 LLM、不查重（`HON/crud/document.py:1168-1300`） | 无开关 | 可用 | 毒结论清除的受控通道；也提供「perspective 复制」原语（非上游文档化的迁移用法，见 4.2c） |
| dream（omni） | `POST /workspaces/{ws}/schedule_dream` 或自动调度 | 每个 collection 的 deduction/induction 高层结论 | `DREAM_ENABLED=true`；阈值 50 条新 explicit、静默 60 分钟、间隔 8 小时（`HON/config.py:1405-1416`；`HON/dreamer/dream_due.py:100-135`） | 可用 | 长期固化的正路；但本库**跑过 2 次 omni 梦、0 条 deductive/inductive 结论、`document_sources` 0 行**——需要一次受控复跑定性 |

---

## B. 端态选项矩阵（历史内容要让模型「记得」，全部可行路径与代价）

> 先钉死一个判据：**「被动注入里有」和「模型答得出」不是一回事**。被动注入是 recency-100 窗口 + 可选 summary + 可选 card；本次问题的最小可验收行为是「问『王一诺是谁』能答出 9/30 的事」。任何方案都要按这个行为验收，而不是按「库里有没有数据」验收。

### 选项 1：重灌进主会话（去掉 `:backfill:` 后缀）

- **做法**：给回灌脚本加「目标会话 = live wire 名」的模式（现脚本 `wire_session_name` 派生 base64 批次键，`backfill_honcho.py:659-668`；主会话 wire 名 = `chat-openai-user-kidpal-71ac8e42f72a10d65e3631ac`），2879 条带 `created_at` 追加进主会话；主会话成员已有 `agent-kidpal observe_others=true`（`PLUG/hooks/capture.js:58-72`），所以会派生 `(agent,owner)` 结论。
- **代价（逐项）**：
  1. **重派生成本**：2879 条又要过一遍 DeepSeek；上次从开始写入到结论库稳定约 3 小时 10 分（10:43→13:53 UTC），期间 31 个 representation 单元因 8000 token 截断整批丢空、2 个 embedding 批失败（`RepresentationSaveError`）。live 记忆新鲜度会被挤占数小时。
  2. **批次隔离全失**：回滚只有会话级删除（`DELETE /sessions/{id}` 异步级联，`HON/crud/session.py:delete_session`）；消息级没有删除 API（`PUT` 只改 metadata）。要撤销只能删掉整个主会话——**连 live 记忆一起删**，或用 SQL 按指纹做库内手术（自造通道）。
  3. **摘要路径也不靠谱**：主会话会变成 3000+ 条消息，摘要按 20/60 条节奏重算；但摘要 LLM 现在每次超时、降级摘要不落库，所以「摘要覆盖 2900 条历史」在现状下不成立。
  4. **被动窗口仍裁掉旧事实**：结论按创建时间倒序取 100 条；最后一次灌入的最后一批对应语料最后几天（10 月下旬），9 月事实必然在窗口外。**已有实证**：owner 自身视角现在就有 344 条 backfill 结论，recent-100 只覆盖 10-13～10-24，`王一诺` 不在其中。
  5. **注入预算被挤占**：representation 不按 token 截断，却先扣 `tokens=2000` 的预算；窗口内容变多/变长会把最近消息挤到 0（今天 4370 字符已只剩 3 条消息）。
- **「先试灌单文件再决定」可行吗？** 技术上可行，但**单文件试灌会产生假通过**：单文件的结论是新写的，必然落在 recent-100 内，diag 一定 `wangyinnuo=YES`；全量灌入后它会被后续批次挤出窗口（这正是今天 owner 视角窗口位移的实测形态）。若要试，必须按「全量规模 + 首尾 canary 都在窗口内」验收，等于把全量成本先付掉。**结论：重灌不是推荐路径；它不能修复本症状，只增加一次不可回滚的污染风险。**

### 选项 2：不重灌，启用跨会话注入/检索面

**2a. scope（`recall.automatic="scope"` + `scopeName`）**

- 机制：`POST /scopes {id:"kidpal-memory"}` → `POST /scopes/kidpal-memory/sessions {session_ids:[backfill, live]}` → 服务端把两个会话的 `(P,P)` explicit 结论**零 LLM 复制**到 `(scope, P)`（`HON/deriver/scope_backfill.py:1-24`、`:150-260`），进度查 `GET /scopes/{id}/status`；再把插件配置改成 `recall.automatic="scope"`、`recall.scopeName="kidpal-memory"`。
- 能力边界：复制的是 owner 的**自我视角**结论（344 条），所以 scope 视角确实能包含 9/30 事实；但注入仍是 recent-100，复制顺序不保证（源查询无 `ORDER BY`），9 月事实进不进窗口不可控——**不能作为「问一句就答出」的验收路径**。另外注入的 observer 从 agent-kidpal 换成 scope，**丢掉小伴自己的 theory-of-mind 结论**（`(agent,agent)` 1078 条不会被读到），人格一致性可能退化。
- 评价：适合「跨会话被动上下文合并」的长期形态，不适合作为本症状的第一修复；切换前必须做 A/B（同一组问题分别用 agent 视角与 scope 视角比较注入内容与回答）。

**2b. peer card（`(agent-kidpal, owner)` 卡片）**

- 机制：`PUT /v3/workspaces/kidpal/peers/agent-kidpal/card?target=owner`，body `{"peer_card":["…","…"]}`；插件每轮把 `Key facts:` 段注入（`PLUG/hooks/context.js:61-63`）。也可 `POST /workspaces/kidpal/schedule_dream {dream_type:"card_refresh", observer:"agent-kidpal", observed:"owner", session_id:"…", rebuild:true}` 走 LLM 生成。
- 能力边界：card **无会话维度、无窗口截断、每轮必注入**——正是「稳定身份事实」的正确载体。但 card_refresh 的工具只有 `get_recent_observations / search_memory / update_peer_card`，作用域是 `(agent-kidpal, owner)` collection——**同样看不到 backfill**。所以在现状下，能自动生成的 card 也只会覆盖 live 事实；要让卡片包含「王一诺」，要么人工写卡片，要么先把 backfill 结论复制进 agent 视角（2c）后再跑 card_refresh。
- 评价：**推荐作为「必须被动记得」类事实的落点**（同学/好友/宠物/关键事件，控制在十几条），但需要明确 card 的维护责任（人工 or 受控脚本），不能假装它会自动长出来。

**2c. perspective 复制（`POST /conclusions` 把 `(owner,owner)` 结论复制成 `(agent-kidpal,owner)`）**

- 机制：`POST /v3/workspaces/kidpal/conclusions`，body `{conclusions:[{content, observer_id:"agent-kidpal", observed_id:"owner", session_id:"<backfill>"}]}`，每批 ≤100；服务端只做 embedding、写 explicit 结论、不查重（`HON/routers/conclusions.py:29-60`；`HON/crud/document.py:1168-1300`）。344 条可分 4 批。
- 效果：注入的 `(agent,owner)` 视角瞬间获得历史结论；无需重派生、可逆（`DELETE /conclusions/{id}` 逐条删）。
- 限制与风险：① 这是把上游 API 用在「跨观察者迁移」这个**未被文档化的用法**上（scope_backfill 才是官方复制机制，但它只复制到 scope 视角），按 human「不发明新东西」原则必须显式标注；② created_at 全部是复制时刻，窗口仍是 recent-100——按复制顺序决定谁能留在窗口（可以人为把 9 月批次放最后，但这是对窗口的操纵，不稳定）；③ 未来 live 新结论会持续把这些副本挤出窗口；④ 344 次 embedding 调用（DashScope）需关注 batch 上限 10 的已知坑。
- 评价：可作为「强制被动注入 backfill」的最轻临时手段，但**不解决长期窗口语义**，不建议作为架构答案。

**2d. 调大 `WORKING_REPRESENTATION_MAX_OBSERVATIONS`**

- 把 100 调到 391+ 可以让 owner 自我视角覆盖全部历史，但注入体积会从 4.3k 字符涨到 30k+ 级，挤爆 `tokens=2000` 预算（最近消息归零）、抬高每轮 token 成本与延迟。**不推荐**；它是把检索问题硬塞进 prompt。

### 选项 3：检索式召回（工具面 + 提示词）

- **已具备的能力（实测）**：
  - `honcho_search_conclusions` → `participantPeer.representation({searchQuery, topK:10, maxDistance:0.5})` → `(owner, owner)` 自我视角的语义检索。查询「王一诺是谁」**第一条结果即 9/30 事实**；查询「王一诺」（0.7）同样命中。`maxDistance=0.3` 过严会漏（实测），建议保持默认 0.5 或 0.7。
  - `honcho_search_messages`（`from:"user"`）→ `POST /peers/owner/search`，**直接返回原句**「今天早上我出卖了王一诺。」（同一句因历史回放重复 4 条）。
- **当前为什么没生效（三重锁）**：
  1. 工具未放行：插件 runtime 明确注册了 `honcho_session / honcho_context / honcho_search_conclusions / honcho_ask / honcho_search_messages`（`openclaw plugins inspect openclaw-honcho --runtime`），但 kidpal agent 的显式白名单是 `["read","write","exec","web_search"]`；OpenClaw 的工具策略流水线把 agent allowlist 作为一层策略套在**包含插件工具在内的全量工具**上（`dist/tool-policy-pipeline-BEgOJwIf.js` + `dist/tool-policy-DkFBFlba.js`：`analyzeAllowlistByToolType` 识别 `pluginTools.has(entry)`，随后 `filterToolsByPolicy` 过滤全量），日志实证「No callable tools remain after resolving explicit tool allowlist (agents.kidpal.tools.allow: …)」（2026-10-24 02:27）。**OQ-3 至此有答案：`tools.allow` 管得住插件工具。**
  2. 提示词禁止：`server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md` L11「孩子对话回合…**不调用任何工具**…任何『先查一下 / 先确认一下』的内部动作都是违规」；L90「记忆由平台自动承载、自动注入——**你不需要写任何文件、也不调用任何工具去记**；想不起来…就诚实说『记不清了』」。模型今天的回答（「我记住的人里没有这个名字，我不能瞎猜」）是**按指令执行的结果**，不是模型缺陷。
  3. 指引缺失：插件的 memory prompt supplement 只在有工具可用时才注入（`PLUG/index.js` `buildPromptSection`，`anyTool` 门控）；工具被过滤后，模型连「可以查」都不知道。
- **差异（检索 vs 被动注入）**：被动注入是每轮固定成本、只看最近 100 条、零延迟；检索是按需一次工具往返（多一轮 LLM + 一次 embedding，预计 +2–5 秒），但能翻全量历史、可按问题聚焦，返回的是原始结论（英文、可能带流程性内容），需要提示词约束「只用于回答、不逐字复述、家长隐私不回忆」。对 8 岁儿童的语音体验，需要一句「我去翻翻我记得的事」之类的过渡语，与现有「实时事实回合」同款体验。
- **评价**：**这是唯一「今天就能让历史事实可用」且不需要动数据的路径**；代价是工具面放行 + 回合纪律修订，都是可回滚的配置/提示词变更（但触及 K1-E4 冻结的四项白名单，必须走 dev-loop + 验收锚更新）。

### 组合与排序建议

1. **P0（立即，零数据移动，可回滚）**：放行 `honcho_search_conclusions`（只此一个；不要放 `honcho_search_messages`，原始消息面含家长代述/流程内容）；AGENTS.md 增加「记忆回忆回合」或改写记忆守则：**画像里没有 → 先调一次记忆检索再回答；检索也没有才说记不清；记忆内容只用于回答、不逐字复述、不暴露给用户**。验收 = 真机问「王一诺是谁」答出 9/30 事实，日志出现工具调用且无「不知道」自述。
2. **P0.5（运维，可逆）**：用 `DELETE /conclusions/{id}` 清掉剩余 3 条毒结论（见 D 节）；它们不在注入视角里，但会污染 agent 侧检索/梦。
3. **P1（稳定事实，推荐）**：给 `(agent-kidpal, owner)` 建一张**小卡片**（同学/好友/宠物/关键事件，约 10–15 条），先人工写入并验证注入（`Key facts:` 段出现）；后续是否用受控脚本/card_refresh 维护，需 human 定责任面。card 是「孩子不提、你也该记得」的唯一可靠被动通道。
4. **P2（可选，先 A/B）**：建 scope（backfill + live），在不改 production 配置的前提下，用 `session.context?scope=kidpal-memory` 与现行 perspective 各跑一组问题比较；只有明确改善且不丢人格结论才切 `recall.automatic="scope"`。
5. **P3（未来导入）**：回灌脚本 `create_session` 必须显式声明成员配置（owner `observe_me=true/observe_others=false`；agent-kidpal `observe_me=true/observe_others=true`），并在 Gate B 增加「读路径 + 注入/检索」断言（见 C 节）。对**现网这一批**不重灌。
6. **P4（可靠性债，独立批）**：摘要超时与「降级摘要不落库」、representation 8000 token 截断、embedding batch 失败、omni 梦零产出。这些不阻塞 P0，但决定被动通道与长期固化是否可用。
7. **不建议**：调大窗口上限（2d）、重灌主会话（选项 1）、单文件试灌后直接全量（假通过）。

---

## C. 流程缺陷复盘：三轮返工为什么每轮都漏下一层

| 轮次 | 当时的 Gate | 验证了什么 | **没验证什么** |
|---|---|---|---|
| R1（v1 灌入 1716 条） | Gate A 解析正确性 / 计数 | 消息条数、逐行解析、指纹 | 结论的 role 归属（peer 映射） |
| R2（修 role / v2 重建） | Gate B 试灌读回（role→peer）；Gate C 回滚彻底；Gate D 计数/分布/抽检 | 单文件试灌的 role→peer 映射；旧批次硬删干净；新批次条数、队列、结论分布 | **会话成员观察配置**；**读路径真正读的那个 pair**（`agent-kidpal→owner`）；**注入内容**；摘要是否落库 |
| R3（diag 实证） | 无 Gate，靠人加日志才发现 | 注入只有 1 段、无 summary/card、无王一诺 | — |

具体病灶：

1. **Gate B 只验「写进去的形状」，没验「读出来的形状」**。它检查 `peer_name == owner/agent-kidpal` 的映射，这是消息层的镜像；真正决定召回的是 **deriver 的观察者扇出**（依赖 `session_peers.configuration`）。试灌用的是同一个 `create_session` 缺陷路径，若当时查一下 `session_peers` 或 `documents(observer, observed)`，缺陷当场就会暴露。
2. **Gate D 的分布断言写成了「owner→owner / agent→agent 都非空」——恰好就是 backfill 能产出的两个 pair**，于是必然通过；它没有断言「注入实际读取的 pair」`observer=agent-kidpal, observed=owner`。这是典型的「测试镜像了写者，而不是读者」。
3. **没有任何 gate 定义在用户可见行为上**。三轮验收都停在「库里有正确的条数/归属/日期」，没有一条是「小伴被问 X 能答出 Y」。症状直到人肉真机测试才出现。
4. **摘要/梦这类「被动通道」没有健康断言**：140 个摘要任务全部 processed、0 error，但 0 条落库；梦跑过但 0 条派生结论。队列状态是「成功」，能力状态是「空」。
5. **试灌设计的位移盲区**：单文件试灌天然把 canary 放在窗口最前，无法验证全量后 canary 是否被挤出。必须按窗口模型验收。

**防复发的结构建议（建议固化为 Gate E，插在 Gate B 与全量 apply 之间）**：

- **E1 静态成员断言**：目标会话 `session_peers` 必须含 owner（`observe_me=true`）与 agent（`observe_me=true, observe_others=true`）；缺一即停。
- **E2 派生断言**：队列排空后，`documents(observer=agent-kidpal, observed=owner, session_name=<批>)` 条数 > 0，且试灌文件的 canary 文本出现在该 pair 的结论里。
- **E3 读路径/注入断言（关键）**：直接调用插件同款请求 `GET /sessions/<live>/context?summary=true&tokens=2000&peer_target=owner&peer_perspective=agent-kidpal`，断言 canary 出现在 `peer_representation`；**并且**按窗口位移模型断言：该 pair 结论总数 ≤100 时 canary 必须在窗口内，>100 时必须以「检索命中」作为补充验收（`POST /peers/owner/representation {search_query: canary}`）。
- **E4 用户可见行为断言**：真机（或等价的 agent 回合）问一句 canary 问题，回答正确；同时 diag 日志 `wangyinnuo=YES` 或工具调用可见。
- **E5 通道健康断言**：至少一条 summary 落进 `sessions.internal_metadata.summaries`；新批 representation 失败数 = 0 或 ≤ 事先声明阈值；omni 梦产出可核查的派生结论（或明确声明「本批不依赖梦」）。
- **原则**：**验收从读路径往回写**——先定义「模型这一轮实际收到什么」，再推回库内形状与写入参数；任何「写侧正确」都不自动等于「读侧可见」。

### C.1 与治理账本 ARQ-261024-new-mechanism-spike-first 的对账（不重复登记）

human 已就本事件登记 `docs/debt/gov/ARQ-261024-new-mechanism-spike-first.md`（域 gov，status deferred，severity MAJOR，owner = architect 方案与落点 + 主线程 Track B 起草）：它承载的是**流程机制缺口**——「首次引入新子系统 / 新外部依赖集成 / 新数据迁移」的批缺「端到端最小验证 spike 先行」，五层问题每层各吃一轮返工。本节与该 ARQ 的分工是：**ARQ 管流程缺口与治理落点；本件管技术事实、根因链与端到端验证判据；技术残余归 `docs/debt/tech/TECH-261022-device-memory-prompt-overhaul.md`（及其 2026-10-24 append）**。三者不互相替代，也不重复登记。

两处衔接需要主线程在本条 ARQ 的后续 append 中精确化（我不改账本，登记动作归主线程）：

1. **ARQ 证据第 5 条的机制表述**：现文写「插件召回 = `session.context(当前会话)`——跨会话结论结构性不可见」。代码与实测的精确形态是：`session.context` **默认不限于当前会话**（`recall.automatic` 缺省 `workspace` 时不传 `limitToSession`，representation 跨会话读同一 pair collection）；真正阻断是两层——(a) backfill 会话的 `agent-kidpal` 成员 `observe_others=null`，agent 视角从未产出 `(agent-kidpal, owner)` 结论（0 条）；(b) 被动画像只保留最近 100 条结论，9 月事实落在窗口外。这个区别决定修复方向：不是「把数据搬到当前会话」，而是「补观察者成员配置 / 走检索 / 用 card」。
2. **ARQ 建议落点 (c)「Gate 链补位」的具体判据**：可直接引用本文件 C 节的 E1–E5 作为该落点的内容种子；尤其 **E3 的「窗口位移」断言**与 **E4 的「用户可见行为」断言**是这次最贵一层的直接补丁。ARQ 已预标「若落地为机械 gate（G3）须升 full-adversarial」——本 agent 不改变该形态注记。

---

## D. 遗留状态盘点：哪些已经安全、不用动

| 资产 | 实测状态 | 判定 |
|---|---|---|
| backfill 会话消息 | 2879 条（owner 1580 / agent-kidpal 1299），`created_at` = 2026-09-24～2026-10-23 的 12:00 +08:00（04:00 UTC），归属与日期均正确 | **安全**；不要再重灌 |
| backfill 结论（owner→owner） | 344 条 explicit，日期正确（如「On the morning of 2026-09-30, owner stated: 今天早上我出卖了王一诺」），session_name 正确 | **安全且已可用**（检索路径实测命中） |
| backfill 结论（agent→agent） | 1078 条 explicit，小伴自己的话，归属正确（两元钱事件归 agent 而非 owner） | **安全**；但当前注入不读它 |
| live 会话 | 167 条消息 / 293 条结论（截至今晚实测：agent→agent 199、agent→owner 47、owner→owner 47），今日召回正常（diag 注入含西安、小猫、方可馨等事实） | **安全**；不动 |
| 毒结论 | 3 条仍存：`5BFcg3QJJmUOGxNe6zTVP`（backfill，agent→agent「unsure which person」）、`ckT98fZXTw9LwuLP4Gjdv` / `Ium0bFpvKIgiayGBCpZYu`（live，agent→agent「had no information / not among the people」，2026-10-24 14:49 UTC 生成）。**均不在注入视角**（注入读 agent→owner） | 不阻塞；建议用 `DELETE /conclusions/{id}` 清掉，且用 P0 的检索行为从根上防止再生 |
| 摘要 | 6 个会话 `internal_metadata` 全空；140 个 summary 队列任务 processed 无 error，但降级摘要不落库 | **不安全但不污染**；属可靠性债 |
| 梦 | 2 条 omni 梦 processed；0 条 deductive/inductive 结论；`document_sources` 0 行 | 功能未产出，需一次受控复跑定性 |
| 队列 | 33 条 representation 错误：31 条 backfill `EmptyRepresentationError`（8000 token 截断）、2 条 live `RepresentationSaveError`（embedding batch）；覆盖 2314/2879 条消息（80.4%），未覆盖 565（owner 390 / agent 175） | 可接受但需记账；重灌不可解决、反而复制缺陷 |
| 批次隔离/回滚 | 旧 transcript 会话已硬删；现仅 backfill + live 两批，回滚工具按 source-kind 可选 | **安全**（这正是不要重灌主会话的原因） |
| diag 打点 | `PLUG/hooks/context.js` 是**线上热补丁**（存在 `.bak-diag`，2026-10-24 18:34 修改） | 非仓内资产；建议在诊断结论固化后决定「入仓成正式日志开关」或还原，避免插件升级时静默丢失 |

**可以直接回答 human 的问题**：不重灌的前提下，backfill 的 344 条 owner 自我结论 + 2879 条原始消息**今天已经可以被检索到**；缺的不是数据，是「让模型去查」和「把稳定事实放进必注入卡片」。

---

## E. 需要 human 拍板的问题

1. **是否接受「检索优先」的记忆模型**（孩子对话回合允许一次 `honcho_search_conclusions`，多一轮往返，约 +2–5 秒）？这需要改 `tools.allow`（触及 K1-E4 冻结的四项白名单，走 dev-loop + 验收锚更新）与 AGENTS.md 回合纪律。拒绝的话，就只能靠人工卡片，历史细节仍答不出。
2. **peer card 的维护责任与范围**：人工维护（可控、不自动）还是做受控生成脚本？卡片里放哪些事实（同学/好友/宠物/关键事件）？家长隐私边界沿用 AGENTS.md 两条即可。
3. **是否实验 scope**（先只读 A/B，不切配置）？若切，接受「丢 agent 自身视角」的代价吗？
4. **毒结论清理**：是否用 `DELETE /conclusions/{id}` 清掉剩余 3 条？
5. **可靠性债排期**：摘要超时/不落库、8000 token 截断、embedding batch、omni 梦零产出——是否作为紧随的独立 Lite 包。
6. **明确不再重灌**：除非未来出现「必须把历史灌进某个特定 session」的新需求，且先过 Gate E。

---

## 附：本次只读证据清单

- ECS 只读：`openclaw plugins inspect openclaw-honcho --runtime`；`/root/.openclaw/openclaw.json` 键值（脱敏读取）；`journalctl -u openclaw-gateway`（diag 行 22:48:53 / 22:51:24）；`docker logs honcho-deriver-1`（summary 超时 184 次、timeout 687 次、empty representation 231 次）；`docker exec honcho-database-1 psql` 只读 SELECT（sessions / session_peers / messages / documents / queue / collections / peers）。
- ECS 只读 REST：`POST /v3/workspaces/kidpal/peers/owner/representation`（search_query 命中 9/30 事实）；`POST /v3/workspaces/kidpal/peers/agent-kidpal/representation`（target=owner，未命中）；`GET /v3/workspaces/kidpal/sessions/<live>/context`（三种视角对比）；`POST /v3/workspaces/kidpal/search` 与 `POST /v3/workspaces/kidpal/peers/owner/search`（原始消息命中）；`POST /v3/workspaces/kidpal/scopes/list`（空）。
- 源码实读：`HON/deriver/enqueue.py:257-372`；`HON/crud/representation.py:241-528`；`HON/config.py:961-964, 1015-1019, 1223-1224, 1405-1416`；`HON/routers/sessions.py:719-1160`；`HON/routers/peers.py:436-680`；`HON/routers/scopes.py:41-230`；`HON/routers/conclusions.py:29-190`；`HON/routers/workspaces.py:155-195, 236-290`；`HON/utils/summarizer.py:370-790`；`HON/crud/document.py:1168-1300`；`HON/deriver/scope_backfill.py:1-260`；`HON/dreamer/dream_due.py:100-180`；`HON/dreamer/orchestrator.py:590-760`；`PLUG/hooks/context.js`、`capture.js`、`config.js`、`helpers.js`、`state.js`、`peers.js`、`tools/*.js`；OpenClaw `dist/tool-policy-pipeline-BEgOJwIf.js`、`dist/tool-policy-DkFBFlba.js`。
- 仓内只读：`server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`（L11 回合纪律、L88-93 记忆守则）；`server/deploy/honcho/backfill/backfill_honcho.py:659-730`；`server/deploy/261022-openclaw-kidpal-tools/kidpal-agent-tools-declaration.json`；`server/acceptance/test_261022_openclaw_kidpal_tools.acceptance.py`（四项白名单冻结锚）；本目录 `k4-attribution-fix-review-20261024.md`、`memory-wiring-advisory-20261023.md`。
- 未做：任何写操作（未改配置/库/文件），未重启服务，未安装/更新插件。
