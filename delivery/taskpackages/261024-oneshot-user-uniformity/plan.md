# 261024-oneshot-user-uniformity · plan.md（lite · 技术语义权威）

TaskPackage: `delivery/taskpackages/261024-oneshot-user-uniformity/taskpackage.json`
来源: human-ad-hoc（2026-10-24 Pod m2 launch 指令，human 原话已录 origin.human-verbatim + pod-events human-decision a1963744）
红基线（2026-10-24 本树实跑）: 333 passed / 7 deselected / 33 subtests passed

## human 方向权威（不可重裁）

1. **所有回合统一携 `user=<device_id>`**——front 引导/整理两条一次性 LLM 回合全部落同一设备 session 键族 `agent:kidpal:openai-user:<device_id>`（OpenClaw 以请求 `user` 字段派生 session，engine 主链路同源，见 `server/deploy/engine-patch/openai.py:140-144/:190-194` 与契约 §四 `interface-contract.md:287`）。
2. **noisePatterns 补丁键退役**——声明工件 `server/deploy/261022-openclaw-honcho-plugin/honcho-plugin-declaration.json` 5键→4键。
3. **K3-E2 方向反转显式换代**——`test_k3_e2_front_oneshot_payload_no_user`（断言 payload 恰 {model,messages,stream} 零 user）换代为新形状 {model,messages,stream,user} 且 user=<device_id>；human 裁 R7/OQ-4 隔离口径退役，换代显式留痕、非静默放宽。

## 技术方案（planner+architect 预评；2026-10-24，architect 前置预评档 1g，advisory 效力声明附末）

### ① provider 单源扩展（`server/gateway/provider.py`）

`openclaw_stream_request(messages, user=None)`：`if user:`（**真值判据**，与 engine-patch openai.py 两腿逐字同源——None/空串 → 键缺席 = 回落旧行为）→ payload 增 `"user"` 键。单源构造保持、契约 §四禁 max_tokens 约束不动。模块头注 Frozen offline contract 行同步补可选 `user` 一句（archive 下 test_gateway_directionB 不收集，头注可改）。**零契约动作**：front 是同一 loopback 客户端沿用契约 §四已定案的 user 语义。

### ② front 调用链（`server/gateway/front/front_app.py`）

- `_openclaw_chat_once(system_text, user_text, timeout_s=10.0, user=None)` 透传至 `gw_provider.openclaw_stream_request(messages, user=user)`。
- **设备身份提取**：从腿上的 starlette device_ws 读 `headers.get("device-id")`（starlette Headers 大小写不敏感）。helper 防御式：`getattr(device_ws, "headers", None)` + **`isinstance(v, str) and v.strip()` 双保险**（防 Mock truthy 值把 json.dumps 炸掉）。**透传原值，不 strip、不归一**——与 engine 端 `connection.py:373` 原字符串直传同源，避免分叉出第二个 session 键族（正是 human 抱怨的「维护不知道是啥机制」）。
- **条件传参（architect 修正一，采纳）**：`kwargs = {"user": device_id} if device_id else {}`，`asyncio.to_thread(..., **kwargs)`。语义 = device-id 缺席 → 诚实降级（payload 缺 user 键）+ 调用缝留痕；**八处既有桩零改**（unit 四处 + 姊妹验收件四处，见 architect 回执清单），爆炸半径最小。
- 落点：
  - `_story_finish_session`：device_id 派生放**归档内容守卫之后、to_thread 之前**（离消费点最近；`test_archive_guard_units.py` AST 结构锚不受扰）；`_organize_and_archive(raw, corr_id=None, device_id=None)` 与 `_organize_and_archive_offloop` 同形透传（全仓仅两调用点，已核）。
  - `_story_guide_round`：**LLM 调用前取一次会话腿快照仅用于身份**；播报仍按现序在回复后取 `_session_legs(sid) or list(_alert_legs)`（不动在途断连的播报目标判据——非本包行为面）。
- **多腿兜底归属**：映射缺席退 `_alert_legs` 时身份取第一条腿——单设备 MVP 成立（决策 22），多设备将来会错归属，申报残余。

### ③ 指令挪 system（评估项裁决 = **挪**，architect 预评同裁，附部署洞补丁）

- 整理回合：user 消息 = **material 纯文**；引导回合：user 消息 = **spoken 纯文**。两 prompt 制品（`faithful-organize.v1.md` / `guide-round.v1.md`）本就作为 request 级 system 全文装载且自包含指令（任务定义/忠实义务/输出格式/≤20字护栏），user 消息尾随祈使句是冗余强化。收益：noisePatterns 退役后包装句不再随消息入设备 session 被 deriver 抽成 observation（引导回合包装句占比尤其大——正是要清的噪声）。
- **部署洞（architect 关键发现，必须同批补）**：`server/deploy/manifest.yaml` front 条目 12 条**不含** prompts/*.md，deploy.sh/verify-deploy.sh 只按 manifest 逐文件——两枚 prompt 制品 live 可能根本不在位（`_load_prompt_artifact` 恒 ""、只留 warning），现行两回合靠的恰是 user 消息尾随指令。「挪 + 制品缺席 = live 指令全丢」典型假绿。补丁三件：
  1. manifest.yaml 登记 `server/gateway/front/prompts/faithful-organize.v1.md` 与 `guide-round.v1.md` 两条（`restart: none`、`verify: md5`；新增条目不影响既有 manifest 断言——按 token/path 前缀过滤，architect 已核）；
  2. **空制品回填兜底**：`_load_prompt_artifact` 返回 "" 时，user 消息回填**现行完整包装形态**（整理 = `孩子讲的内容：\n{material}\n\n请按指令把这段话忠实整理成一篇。`；引导 = `孩子到目前为止讲的内容：\n{spoken}\n\n孩子停顿了，请按指令给一句轻声引导。`）+ warning 留痕——最坏情况退回现行 live 行为，不是无指令；
  3. 部署注记（plan.md + docs-delta 申报）：新登记条目在 live 缺席会被 deploy 前置漂移核验判漂移 REFUSE，该次部署须 `--first-run` 或先手工置入两文件；顺带偿还 261022 polish 改 guide-round.v1.md 但 manifest 未登记的欠账。
- 回归风险 = 「最近指令」效应，离线不可证：真机 3–5 例抽检（整理：首行标题/正文分段/忠实度/**不得混入既往篇目内容**——user 入 session 后读侧检索范围变设备 session 自身历史，忠实整理可能被自家既往故事带跑；引导：≤20 字、不代编）作为申报残余。

### ④ 声明换代（`honcho-plugin-declaration.json`）

changes[] 与顶层 plugins 块同步删 noisePatterns 条目（5键→4键逐字）；**_note 重写硬约束**：`agent:kidpal:openai-user:` 字面必须保留（`test_k3_e2_verified_wiring_declaration_anchors` 的 session 家族锚 + 变体负锚读工作台并集）、不得引入 `agent:kidpal:` 其它形态；插件 pin/load-log/Policy 字面一并保留。apply/rollback **零代码改动**（build_patch_tree/target_state/drift_problems 全从 changes 派生、无键数硬编码，architect 已核）。**noisePatterns 退役无机械验证面（如实申报，禁止假牙）**：patch 只能表达 additive 目标态、不能表达键删除，且 apply 幂等 SKIP 先于 drift-guard（live 残留 noisePatterns 时 target_state 相等 → SKIP，drift-guard 永不到）——**不加 `expect: unset` 冒充验证**；退役实证 = 部署轮实读 `/root/.openclaw/openclaw.json`（或 `openclaw config get plugins.entries.openclaw-honcho.config.noisePatterns`），写进 live 注记。

### ⑤ manifest.yaml 口径同步

`:297` / `:303` 两处 note 仍写「5 键（含 config.noisePatterns）」——部署单一事实源不许说谎，纯 note 改动、零 entries 变化（除 ③ 新增两条 prompt 条目）。

### ⑥ 验收换代（acceptance-author 冻结承载）

- 主换代件 `test_261022_memory_isolation.acceptance.py`：`K3_DECL_CHANGES` 5→4（`:327-336`）；payload 锚反转为恰 {model,messages,stream,user} 且 user=<device_id>（含降级路径：device-id 缺席 → 缺 user 键）；5键逐字锚→4键；patch 生成器恰 4键；头注 B1 挡板措辞退役申报 + 换代事由（human 2026-10-24 裁 R7/OQ-4 隔离口径退役）写进断言 docstring 与头注；live 段口径反转。
- **改名（architect 建议）**：`test_k3_e2_front_oneshot_payload_no_user` → `test_k3_e2_front_oneshot_payload_carries_device_user`；`test_k3_e6_patch_generator_product_five_keys` → `..._four_keys`——名字钉死数与断言数不一致正是 human 抱怨的机制不透明；旧名映射写进 docstring 与 A3，历史 taskpackage 旧名原样不动。
- **live one_shot 证据 schema 增强**：每条证据增 `openclaw_session_key` 字段（前缀 `agent:kidpal:openai-user:` 断言）——只证「不再挡」不够，要证「全部落同一 session 键族」。
- 触链回归（零改必绿）：test_story_cancel_lifecycle_units 四族、test_archive_guard_units AST 锚、四姊妹验收件桩缝（条件传参下无感）。

## 残余申报（真机/部署轮兜底，离线不可证）

1. 指令挪 system 的输出质量（真机 3–5 例抽检，见 ③）。
2. 同段孩子内容先进引导、再进整理回合 → 消息库两遍、observation 可能重复膨胀——检索质量面，部署轮看检索。
3. noisePatterns live 退役实证（见 ④，部署轮实读）。
4. spec 过时面（docs-delta 显式列举，挂到在途 /requirement-change & arch 批）：arch §6.7 第 1/2 行（arch:364-365）、arch:155（NFR-PRIV-1 行）、arch:360（§5.1.2 记忆写入范围段）、docs/requirements.md:598（S-F5.1 AC5「设备流程回合不进画像」）、:601（补充规则）、:654（NFR-PRIV-1 边界段）。

## 效力声明（architect 预评档原文）

「本预评为 advisory 输入——不豁免任何门、不改验收与 TaskPackage 事实、不构成通过判据。」技术方案本节由 planner 依预评回填定稿；与冻结验收冲突时以验收为准。

## 修正案 R2(live 证伪回退;2026-10-24 human-correction f13a53a8)

**live 实证(2026-10-24 10:10-10:11 真实记录会话)**:「指令挪 system」裁决被证伪——整理/引导回合模型**不跟** request 级 system 指令(OpenClaw openai-http 路径以 AGENTS.md 载体为主,请求级 system 疑似被替换/忽略):整理产出聊天腔、坏归档件 stories/2026-10-24-sc4238164(131 字节,留待重录覆盖或 human 手删,本包不碰生产数据);引导回复聊天腔超长(播报侧截 30 字、全量进记忆库)。

**回退形态(human 原则「回到验证过的形态」;修正 ③ 裁决)**:
- user 消息恢复「指令 + 素材」包装:整理 = `孩子讲的内容：\n{material}\n\n请按指令把这段话忠实整理成一篇。`;引导 = `孩子到目前为止讲的内容：\n{spoken}\n\n孩子停顿了，请按指令给一句轻声引导。`——**恒定包装,不再区分制品在场与否**;旧「空制品回填兜底」分支随之坍缩(制品缺失 = 仅 warning,行为无差)。
- request 级 system 制品装载**保留**(两处并存不冲突,host 无感)。
- **批 C 携 user 半保留不动**(live 实证归属 owner/agent-kidpal 完美):payload {model,messages,stream,user}、条件传参、device-id 原值透传全部不变。
- 两 prompt 制品 manifest 登记保留(无害,为制品本身到达 live)。
- 验收换代:冻结件 ①② 的「user 消息 = 纯文」断言改为「含指令包装」;④ 回退路径断言同步;换代事由 = live 证伪 2026-10-24 + human-correction f13a53a8,显式留痕。
- 申报残余更新:①③ live 证伪相关残余清账(离线盲区已实证、已回退);①(检索质量面:引导/整理回复全量进记忆库含聊天腔)与 ②(双回合重复入记忆)继续在册,部署轮看检索。
