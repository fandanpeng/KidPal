# K4 corpus 归因 bug · 架构对抗 review（2026-10-24）

> 性质：architect advisory（human 明示「架构师再过一遍再执行」）。零 canonical 写入；ECS 全程只读探查（PostgreSQL `honcho-database-1`、sqlite 只读、语法树的 `sed/grep` 只读）。本件 = 回执文本，不是通过判据，不豁免任何门。
> 审查对象：主线程「K4 corpus 归因 bug 诊断与修复方案」+ planner HOLD 前源码实读包（documents/session_name、document_sources、指纹锚、fixer 补丁形态、失败工作单元）。
> 实读基线（本会话 2026-10-24 现跑）：ECS 语料 21 文件 / 1652 非空行；Honcho `kidpal` workspace 7 个 active session / 1812 条消息 / 724 份 documents；backfill 两 session（同一 batch `20261024T001446Z`，session-corpus 1652 条全 owner，openclaw-transcript 64 条 owner 31 + agent-kidpal 33）。

---

## 0. 一句话结论

**根因诊断成立（双层前缀 + 首层方括号当 role），但拟定修复「只改 role 解析」不足以消除主症状**——人肉实锤那两条「owner advised / owner told 星星 还两元钱」的源消息 440/444/445 全部是 `User:` 开头的行（第二层 role 本来就是 user），它们把「小伴的话」放在用户消息的上下文块里；只修第二层 role，这三条仍归 owner，症状原样复发。同时发现四件预备件层面的硬问题：transcript 批次的 shadow 导出是跨会话混采且不可复现、回滚工具不能按批次选择、deriver 管线有 107 个工作单元静默失败、历史消息被错误地盖成 2026-10-24 的时间戳。**建议：role 修复保留；补「续行归组 + 内嵌 turn 拆分」的确定性解析层（无损）；transcript 批次建议一并回滚不重灌；回滚前先加选择性删除；重灌前先修 deriver 两个失败模式；重灌前跑试灌读回门。**

---

## 1. 逐条对抗结论（CONCEDE / REBUT）

### 1.1 根因确认

**CONCEDE（结论成立），附三点精确化。**

- 实读 21 个语料文件：**1652 非空行全部**匹配 `^\[kidpal/sessions/kidpal/<uuid>#L<n>\] (User|Assistant): `。User 行 1127、Assistant 行 525。无 JSON 行、无「纯 User: 无方括号」行、无 `[user]` 单层旧格式行，`grep -cE '^\[(user|assistant|system|tool)\]'` = 0。所以主线程的「双层前缀」假设对现状是**精确**的；「第三种行格式」在现行语料里不存在，parser 也不会撞上——单层旧格式与无前缀残行目前都只是理论分支。
- `split_corpus_line` 取证：首层 `[kidpal/sessions/...#L5]` 被当作 role，content = `User: ...` / `Assistant: ...`；`add_message` 判 `role == "assistant"` 才路由 `agent-kidpal`，其余一律 owner。1652 行 100% 落 owner，与 DB 实测（corpus session 1652 条 peer 全 = owner）逐条吻合。
- **1026/1683 的数字自洽，但语义要说准**：1683 = corpus 1652 + transcript owner 31；1026 = 525 条直接 `Assistant:` 行 + 501 条「`User:` 行内容里嵌了 `Assistant:` 文本」（消息体字符串层面）。它**不是**「Assistant 行占比」——直接 Assistant 行只占 31.8%。这个区分很重要，因为它直接决定方案是否治得到主症状（见 §2）。

### 1.2 污染边界：transcript 批次与 live

- **CONCEDE**：transcript 批次 64 条里 user/assistant 的 role 确实从 `event_json.message.role` 直取，owner 31（28 user + 3 toolResult）、agent-kidpal 33，人工抽样与 DB 对读一致；live 插件今日写入的 `chat-openai-user-kidpal-71ac...`（88 条，44 owner / 44 agent）**归因干净**，owner 消息是真实童言（`今天妈妈说准备养一只小猫。` 等），不依赖 corpus parser，与本 bug 无关。
- **REBUT「transcript 批次干净」**，三条证据：
  1. `add_message` 的规则是「非 assistant 一律 owner」，所以 **3 条 `toolResult` 被归成 owner**（内容是 763/950 字的「星玥的课表」文件正文）。transcript 会话里确有 2 份 documents 的 source 窗口引用了这些 toolResult（`owner has a child named 星玥` / `owner is preparing to adopt a kitten`）。量小，但「干净」不成立。
  2. shadow 导出脚本 `C:/tmp`（ECS 侧 `/tmp/export_shadow.js`，**不在仓内**）用 `seq` 当 shadow 主键，而真实表 `transcript_events` 的主键是 `(session_id, seq)`——`seq` 是会话内序号。实读：源表 3203 行、含 `message.role` 的 1307 行、**distinct seq 只有 120 个，1187 行在主键冲突时被静默 `skipped`**。也就是说 64 条是「按 seq 去重后的偶然抽样」，不是一次完整会话导出。
  3. 现行 `/tmp/transcript-shadow.sqlite` 是 64 行（id 4..67）；P2 会话 `6d68754e-9516-4635-9b33-74d8e9cb23b5` 现在已有 124 个事件、120 个带 role（59 user / 60 assistant / 1 toolResult）。**若今天重跑同一脚本，结果会变成 120 行（113 条来自该 P2 会话 + 7 条来自另外 3 个会话的杂条）**，与已灌入的 64 条不一致。结论：transcript 批次的 role 归属按条正确，但**取样面不可复现、且混了 2 条以上其它会话的事件**，不能以「干净」名义保留。
- 另：真实表形状 = `(session_id, seq, event_json, created_at)`，而工具/验收契约写的是 `id / role / content` 直读，**生产源与契约存在形状漂移**；目前只有仓外 `/tmp` 脚本把两者接起来。这是 K4 源适配的残留缺口，回滚重灌会踩到。

### 1.3 方案对抗

**a) 「回滚重灌」vs「原地修」：CONCEDE 回滚重灌，REBUT 原地修。**

- 实读 `/opt/honcho/src/routers/messages.py`：消息只有 `POST`（写）、`POST /list`（读）、`GET`、`PUT`，**`PUT` 仅覆盖 metadata**（docstring 原文「Update the metadata of a message」）；content 与 peer_name 无更新通道。原地改必须直接 SQL 改库（自造通道），而且已派生的 521 份 documents 不会自动失效、deriver 不会重算——原地修在 API 语义下不成立。
- 队列孤儿：**CONCEDE 无风险**。实读 `crud/session.py:delete_session`：硬删级联顺序 = active_queue_sessions → queue items → message embeddings → documents → messages → session_peers → session 行；queue/session FK 也指向被删行。当前 `queue` 表 **0 条未处理**（representation 1778 已处理），所以不存在「队列里未消化的同 session 单元变孤儿」。唯一注意：删除走 202 异步（入队 deletion task），**必须等硬删完成**再重灌（见 §4 Phase 2）。
- 一个额外的实际风险：live 插件 recall 缺省 `recall.automatic = "workspace"`（实读插件 `dist/config.js:98`），即**全 workspace 召回**。从「标记 inactive」到「硬删完成」的窗口里，被污染的 documents 仍可被 live 回合召回。回滚与重灌之间必须插「硬删完成」机械门。

**b) 结论清理的确定性：CONCEDE 可确定性圈定，而且比草案更简单。**

- 实读并复核：`documents` 有 `session_name` 列，724 行**无一 NULL**；corpus session 名下的 521 行 + transcript session 名下 33 行 = 554，与总数 724 对得上。`document_sources` 表**当前 0 行**，`documents.source_ids` 全 NULL，`level` 全 explicit，`deleted_at` 全 NULL——即「推导树下游派生文档」目前**不存在**。
- 因此：**不需要自写「按 message_ids 反查 + 反向遍历 document_sources」的清理器**；`delete_session` 的级联已按 `session_name` 删 documents（且先删 documents 再删 session，FK 顺序正确）。planner 草案①②在现状下 ① 即全量、② 是空操作，可保留为题面防御但不要当成必要步骤。修正一处：`crud/document.py` 的 `or_(linked, legacy_source_ids...)` 是「查下游」的读路径（`get_documents_by_source` 一类），不是删除路径，引用时别写成删除机制。
- 有一个**必须实测的门**：硬删是异步的。回滚 plan/apply 后要查 `sessions/messages/documents/queue` 里该 session 的残留都为 0，并确认 `kidpal` workspace 的 messages 回落到 1812 − 1716 = 96 的量级（±live 新增）。Redis 现在 DBSIZE=0，无缓存残影。

**c) 重灌会不会再翻车：REBUT「修好 parser 就能干净重灌」。**

- 派生管线本身有两类**静默失败**（`queue.processed=true` 且 `error` 非空，即失败也标已处理、不自动重试）：
  1. **EmptyRepresentationError 83 条**（82 条 corpus owner + 1 条 transcript agent，全部落在 00:14:47–00:16:04 的 backfill 窗口）：`parse_class=truncated`，其中 56 条 `output_tokens=2500`、**27 条 `output_tokens=8000`**。也就是说「已调到 8000」并没有消灭截断；重灌后长消息仍可能整条派生为空。
  2. **RepresentationSaveError 24 条**：全 workspace 都有（corpus 11、transcript 8、live/probe 5），根因 = DeepSeek 侧 embedding 报 `batch size is invalid, it should not be larger than 10.: input.contents`。这是 DashScope 兼容端点 batch 上限 10 与 Honcho 默认 `max_batch_size=100` 的冲突（`src/embedding_client.py:250`）。**不修这个，重灌的派生会继续成批丢单元。** Honcho 支持 `EMBEDDING_MODEL_CONFIG__MAX_BATCH_SIZE=10`（`src/config.py:455` 的 `max_batch_size` 字段 + `EMBEDDING_` 前缀/`__` 嵌套）。
- 覆盖度基线：corpus 1652 条消息里只有 **1186 条**被任何 documents 引用（71.8%）。去掉合法的短句不算，失败单元至少解释 93 条。重灌后不能只报「documents 涨了」，要报「新批次 queue error=0 + 覆盖消息数」。
- 预期条数：**取决于是否采纳「续行归组 + 内嵌 turn 拆分」**（§2）。按现行 fixer 补丁（逐行一条）= user 1127 / assistant 525 / 共 1652；按归组后拆分 = user 1580 / assistant 1299 / 共 2879（我用真实 21 文件离线模拟所得，见 §1.5）。
- 「apply 前机械验证」的具体判据见 §4 Gate A/B。

**d) 与 live 并发竞态：CONCEDE 指纹幂等设计够用，但有三个补丁。**

- 指纹/批次隔离本身成立：live 消息无 `kidpalBackfillFingerprint`，`list_fingerprints` 只扫 active session，回滚只删带 `kidpalBackfillBatch` 的 session；新 batch tag 生成新 session 键，不会与旧会话或 live 会话撞名。
- 补丁一：**回滚工具不能只删 corpus**（见 1.4），必须加选择性过滤，否则会连 transcript 一起删。
- 补丁二：旧 session「inactive 但未硬删」窗口内，`list_fingerprints` 已经看不到它（list 只回 active），所以重灌不会因旧指纹被跳过——这是好事；但旧 documents 也仍可见（见 1.3a），必须等硬删完成。
- 补丁三：重灌期间 live 的 derivation 与 corpus 的 derivation 共享同一 deriver 与 DeepSeek 配额，1652 条 × 每轮 LLM 调用会挤占 live 记忆新鲜度数小时——排期上建议在低使用时段执行，并记录「live 新事实延迟写入」为已知影响。

### 1.4 遗漏面（主线程方案未覆盖的）

**F1（阻断级）：内嵌上下文块——只修 role 治不到主症状。**
- 逻辑组（同文件 + 同 `#L` + 同第二层 role 的连续行）实读 1007 组：user 484 / assistant 523；**252 个 user 组的内容里含内嵌 `User:`/`Assistant:` 标记**（246 组带 `[Chat messages since your last reply - for context]` / `[Current message - respond to this]` 包装标记），即约一半用户消息是小伴回复的「上下文回放」。
- 主症状实证：documents `AYsupxBaZzaY4pQ0tXrsH`（owner advised 星星 还两元钱）的 source = 消息 440/444/445，三条的 content 都以 `User:` 开头、尾巴是小伴的原话。**修完 role，这三条还是 user → 症状照旧。**另一条同窗口的 441 是真 `Assistant:` 行，会修对。
- 处置二选一（需 human 拍板，见 §5 D1）：① 确定性拆分（推荐，无损，见 §2）；② 维持逐行 + 只加 session 级 deriver custom_instructions 提示「消息体内以 `Assistant:` 标注的段落不是作者本人的话」——成本低但不是确定性保证，且 `DERIVER.MAX_CUSTOM_INSTRUCTIONS_TOKENS` / 开关需另查。

**F2：续行碎片化。** 1652 行里 **645 行是续行**（同组重复 `#L` 前缀，占 39%），display JSON / 笔画路径被拆成多条独立消息。现行逐行设计会把 `M 319 588 Q 409 612 ...` 这类片段当独立 owner/agent 消息喂给 deriver。归组后 1007 条，拆分后 2879 条，内容完整。

**F3：时间戳全被盖成 2026-10-24。** 消息 `created_at` 用写入时刻，`MessageCreate.created_at` 字段其实支持回填（`schemas/api.py:347`），但工具没传。结果：**201/521 份 corpus documents 声称「On 2026-10-24, owner said ...」**，其中多数是 9/24–10/23 的内容。这直接破坏「孩子成长时间线」类推理，重灌是唯一修复时机（语料文件名 = 内容日期，实读 2026-09-24.txt 的内容确为 9/24 当天）。

**F4：toolResult 归 owner。** 契约/实现只区分 assistant 与非 assistant。建议把 `toolResult`（含未来 `tool*`）也路由 `agent-kidpal`；对现存 transcript 批次的影响只在重灌时体现。

**F5：派生静默失败 + 覆盖度 71.8%。** 见 1.3c。

**F6：transcript 源不可复现 + 形状漂移。** 见 1.2；另外 `ORDER BY seq` 跨会话混序，ORDER BY 的 tie 顺序在 SQLite 中无保证，抽样结果可能随手变。

**F7：语料快照未固定。** 语料目录是每日滚动的（每天 03:00 新增一个 `YYYY-MM-DD.txt`）。plan 与 apply 之间若跨过 03:00，条数会漂。验收判据必须绑「21 文件 + 逐文件 sha256」的快照，或 plan/apply 连跑并记录文件清单。

**F8：「owner 真事实里有多少实际来自 agent 语句」。** 回答：521 份 corpus documents 中，390 份的 source 窗口含直接 `Assistant:` 消息、439 份含内嵌 assistant 文本、**0 份不含任何 `Assistant:` 字符串**（三者有重叠）。即**全部 521 份都在污染窗口内**，不存在「只删一部分文档」的可行切法——按 session 整体回滚重灌是唯一干净路径，这点主线程的判断是对的。

### 1.5 fixer 补丁裁定

- 实读工作树未提交补丁（`git diff server/deploy/honcho/backfill/backfill_honcho.py`）：抽 `_without_leading_bracket_layers`（循环剥行首方括号层）+ `_second_layer_role`（只认 user/assistant 两 token、大小写归一、冒号后去一空格），命中即用第二层 role 与小写归一键，否则回落单层旧行为；指纹仍锚原文行。
- 判定：**保留（KEEP）**，这是必要的第一层。补丁形态与我实读的真实语料逐条吻合：我下载 21 文件离线跑补丁后 `parse_corpus_dir` → **1652 条、unparsed 0、user 1127 / assistant 525、指纹 1652 个全唯一**，与独立 shell 计数（`grep -cE '^\[[^]]*\] User: '` / `Assistant`）完全一致；剩余 61 条「内容仍以 User:/Assistant: 起头」的行是合法的内嵌上下文首段，不是解析缺陷。
- 自跑：`python -m unittest ...test_backfill_honcho_unit.py` **34 tests OK**；`python server/acceptance/test_261022_honcho_backfill.acceptance.py` **15 test methods OK**（与 planner 报的 34/333 一致量级；我这边的 acceptance runner 按 test method 计 15）。
- 已知可接受歧义：单层旧格式行若 content 恰好以 `User: `/`Assistant: ` 起头会被改判成第二层 role（如 `[user] Assistant: x`）。真实语料零单层行，属理论风险；若想收紧，可在 `_second_layer_role` 前要求外层方括号内容看起来像源引用（含 `/` 或 `#L`），但不建议在本轮引入新启发式。文档里把这条歧义写明即可。
- **但补丁不能单独上线**：见 F1，它解决不了主症状。若 human 选 D1 方案 ①，本补丁作为「第二层识别」组件并入新解析器；若选方案 ②，把 custom_instructions 一起带上。

---

## 2. 修正后的最终方案（与主线程方案不同处已标注）

**建议采用「无损归组 + 内嵌 turn 拆分」作为解析层**，理由：F1 决定只改 role 不够；而该拆法在真实语料上可验证、无内容丢弃（只是把一条物理行/一组续行还原成原始回合序列），能同时消掉 F2。离线模拟结果：1007 组 → 2879 条消息（user 1580 / assistant 1299），3526 个 role 标记**全部**前置空白、assistant 组内 0 个标记（无误拆面），空段 0，最长续行组 43 段。

### 2.1 解析器 v2（新 Lite 包实现，dev-loop 内落）

1. **逐行保留现有双层识别**（fixer 补丁：剥首层源引用、第二层 user/assistant 定 role、大小写归一、单层旧格式回落）。
2. **续行归组**：同文件 + 同 `[<ref>]` + 同第二层 role 的连续行合并为一条逻辑消息（换行保留），首行行号作锚。
3. **内嵌 turn 拆分**：对逻辑消息体按 `(?<=\s)(User|Assistant): ` 切段；首段沿用外层 role，其后各段按标记定 role。只对 user 组启用（实读 assistant 组零标记）；包装标记 `[Chat messages since your last reply - for context]`、`[Current message - respond to this]` 作为协议标签剥离并计数（不算内容丢弃，报告里单列 `wrapperMarkersStripped`）。若 human 要求连标记也保留，改为不剥、只计数。
4. **created_at 回填**：`YYYY-MM-DD.txt` → 该日中午 12:00 Asia/Shanghai 的 UTC 时间（或该日 00:00 +08:00，二选一，报告里钉死口径），经 `MessageCreate.created_at` 出网；由此 documents 的时间线回到真实日期。duck 协议 `add_message` 增加可选 `created_at` 参数（向后兼容；验收同步补）。
5. **指纹**：`v1:session-corpus:<sha256>` over `源文件 | 首行行号 | 段序 | role | 段文本`；同一源重跑逐位稳定；跨源命名空间不变。旧批次的旧指纹随 session 删除自然消失，不会与新版撞车。
6. **报告扩展**（additive，不破冻结顶层键）：`perSource[kind]` 增 `perRole: {user, assistant, other}`、`groups`、`segments`、`wrapperMarkersStripped`、`corpusFiles: [{name, sha256, lines}]`；`backfill` 顶层加 `perRoleTotal`。plan/apply 两态都出。
7. **toolResult 路由**：`role in {assistant, toolResult, tool}` → `AGENT_PEER`（transcript 源用）。
8. **回滚工具选择性删除**：加 `--source-kind {session-corpus,openclaw-transcript}`（或 `--session-key <canonical>`，二选一；建议 source-kind，因 batch 值即键、后缀可判），缺省仍 = 全部（保持旧行为），plan 输出必须列出 matched/skipped 键。**不加这个，主线程「只回滚 corpus」在工具契约下做不到**（实读：`list_backfill_batches` 返回 2 个 session 键，`delete_sessions` 一次性删光）。

### 2.2 transcript 批次：建议一并回滚，不重灌（替代主线程的「保留」）

- 依据：1.2 的三条（混采/不可复现/3 条 toolResult 误归），加上 **P2 会话同一窗口已由 live 插件写入**（live session 88 条，内容与 shadow 64 条高度重叠）。保留 = 保留一份部分重叠、混会话、不可复现的数据，还会被 deriver 二次派生（当前 transcript 会话已产 33 份 documents，与 live session 的 125 份存在重复事实）。
- 处置：本次一并删掉；transcript 源的重建（per-session 导出 + id=(session_id,seq) + role 含 toolResult + created_at 保留）登记为 tech debt，等真正需要补 P2 以前的历史时再做。若 human 坚持保留，则必须先把 `/tmp/export_shadow.js` 改成按 `session_id` 过滤、主键改 `(session_id, seq)`，并把脚本入仓（否则不可复现），再重灌。

### 2.3 重灌前置：deriver 两个失败模式

1. `EMBEDDING_MODEL_CONFIG__MAX_BATCH_SIZE=10` 写入 `/opt/honcho/.env` + `docker compose up -d`（restart 不重读 env）。同时把该键补进仓内 `server/deploy/honcho/honcho.env.example`（部署资产变更走 loop）。
2. 截断：先查 `DERIVER_MODEL_CONFIG__MAX_OUTPUT_TOKENS` / fallback 是否可提到 16000；DeepSeek 侧上限不明，则接受「重灌后 EmptyRepresentation 允许少量」，但必须**显式计数并落报告**，不许静默。重灌后 gate：新 batch 的 queue error = 0（或 ≤ 事先声明阈值，超阈升级人）。
3. 顺带：`LLM_DEFAULT_MAX_TOKENS=8000` 是 ECS live 改动、未入仓（debt 已有 2026-10-24 append），保持现状并在 `.env.example` 同步注释口径。

---

## 3. 执行序（human 拍板后）

Phase 0（现在，零写）：human 拍 §5 D1–D5。
Phase 1（dev-loop Lite 包，仓内）：解析器 v2 + 回滚选择性删除 + 报告扩展 + 单测/验收修订；host 绿 = 34 单测 + acceptance 全绿 + 真实语料离线审计（Gate A）。ECS 侧 `/opt/honcho/.env` 两键与 `.env.example` 同步（部署资产走同一批或紧随批）。
Phase 2（ECS，human）：`rollback --apply`（带 source-kind/全量）→ 等硬删完成（Gate C）→ 记录回滚报告。
Phase 3（ECS，human）：试灌（Gate B）→ 通过后全量 `apply`，报告落盘（perRole + corpusFiles sha256）。
Phase 4：派生排空 + 后验门（Gate D）：queue 无 pending、新 batch error 计数、documents 分布、语义抽检（两元钱类、时间线、agent 自述）。
Phase 5：收口（retro/debt：transcript 源重建、截断残量、碎片化的历史处理）。

---

## 4. apply 前机械验证判据（可直接作为验收断言）

**Gate A（host，离线，零网络）**
- A1 单测全绿；A2 acceptance 全绿（更新后）。
- A3 真实语料快照审计（我本次已实跑通过，脚本可固化为测试）：21 文件 → 识别行 = 非空行（unparsed 0）；`perRole` 与独立 shell 计数逐值相等；指纹唯一数 = 消息数；无 content 以首层源引用 `[kidpal/sessions/` 开头。
- A4 拆分正确性抽样：用 2026-09-30.txt（同时含纯 user 行、内嵌上下文组、story-prompt 组）断言「消息 440/444/445 类内容被拆成 assistant 段，且 content 不含外层 `User: `/`Assistant: ` 前缀」。

**Gate B（ECS，试灌 → 读回 → 删除）**
- B1 取 `2026-09-30.txt` 单文件到 `/tmp/k4-trial/`，独立 batch 做 `--apply`（约百条量级）。
- B2 读回该 session 全部消息：每条 `peer_name == owner` 当且仅当 role=user，`agent-kidpal` 当且仅当 assistant；条数 = 该文件解析数；content 与 A4 期望段逐字一致（抽样 20 条）。
- B3 删除试灌 session（同批新加的选择性删除或精确 key），确认硬删完成。B2 失败 = 停，不回滚生产批次。
- 说明：plan 报告只能证明「解析出什么 role」，只有 B2 能证明「role → peer 出网映射」在真实 REST 上正确；这一步同时验证 agent-kidpal 自动入会话（transcript session 已有先例，`session_peers` 实测两口都在）。

**Gate C（回滚后，硬删完成）**
- `sessions` 中旧两键行不存在（或 is_active=false 且随后消失）；`messages`/`documents`/`queue` 中旧 session 残留 = 0；`kidpal` workspace messages 回落到 96 ± live 增量；`documents` 总数回落到 170 量级（724 − 554 = 170）。
- 若 transcript 一并回滚：再核 33 份 transcript documents 一并消失。

**Gate D（重灌后）**
- D1 计数：新 session messages = 报告 ingested；owner/agent 条数与 perRole 一致；`unparsed=0`；`skipped` 仅来自预期范围。
- D2 派生：queue 无 pending；新 batch work unit 的 error 计数 = 0（或 ≤ 阈值并升级）；`documents` 按 (observer, observed) 分布 owner→owner / agent→agent 都非空。
- D3 症状回归（语义抽检，人工读 5–10 条）：`return the two yuan` / `advised 星星` / `not a story but real events` 类事实必须 observed=agent-kidpal（小伴自述），不得再出现 owner；时间线类事实日期落在 9/24–10/23 而非 2026-10-24。
- D4 覆盖度报告：distinct covered messages / total，允许缺口但必须列出（EmptyRepresentation 单元清单）。

---

## 5. 需 human 拍板的决策

- **D1（核心）**：解析层是否采用「归组 + 内嵌 turn 拆分」（无损，2879 条，推荐），还是「只修 role」（1652 条，主症状会复发）。注：K4 验收的「逐非空行一条消息」是 host 侧最小可测格式的代理，原文已声明真语料适配延后到 live 轮对账；采用拆分 = 兑现该缺口申报，但仍需 human ack + 新 Lite 包的验收修订。
- **D2**：transcript 批次一并回滚不重灌（推荐）还是保留/修复后重灌。
- **D3**：created_at 回填口径（文件日期当天 12:00 +08:00 / 00:00 +08:00）。
- **D4**：`.env` 的 `EMBEDDING_MODEL_CONFIG__MAX_BATCH_SIZE=10` 与截断阈值——是否接受「重灌后仍允许少量 EmptyRepresentation，超阈升级」。
- **D5**：重灌排期窗口（live 记忆新鲜度让位数小时）。

---

## 6. 证据与路径

- advisory：本文件（`delivery/review/261022-arch-review-device-memory-prompt-overhaul/k4-attribution-fix-review-20261024.md`）
- 被审实现：`server/deploy/honcho/backfill/backfill_honcho.py`（工作树未提交补丁）、`rollback_honcho_backfill.py`、`server/acceptance/test_261022_honcho_backfill.acceptance.py`、单测 `test_backfill_honcho_unit.py`
- ECS 只读实读：`/opt/honcho/src/crud/session.py`（delete_session 级联）、`/opt/honcho/src/deriver/deriver.py`（`message_ids = [... if m.peer_name == observed]`）、`/opt/honcho/src/routers/messages.py`（PUT 仅 metadata）、`/opt/honcho/src/config.py`（embedding max_batch_size）
- 数据面实读（PostgreSQL）：`honcho-database-1` 的 sessions / messages / documents / document_sources / queue / session_peers；workspace `kidpal`
- 语料离线快照：`c:/tmp/k4-corpus-review/txt/`（21 文件，只读副本；审计脚本在 `c:/tmp/k4-scripts/`，非仓内制品）
