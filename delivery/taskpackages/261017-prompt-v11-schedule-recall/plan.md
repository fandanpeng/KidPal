---
changeId: 261017-prompt-v11-schedule-recall
title: prompt v11 课表召回 + 去小智模板残留——「提到课表就读」识别扩列、KidPal 极简模板替换通用小智模板、兜底语去「小智」、kidpal agent skills 收敛，五件全部接入 manifest 单一事实源
rigor: lite
riskTier: medium
sourceType: regression
reviewed-by: architect (/triage-change 分流 review，human 触点①确认 ②单包 Lite；同日重裁：arch 面迁出 dev 包独立成 architect 会话，gateProfile 收敛为 evidence-integrity)
date: 2026-10-18
gateProfile: ["evidence-integrity"]
---

# 根因

真机问「后天都有什么课程？」，kidpal agent 答出课表里不存在的英语/美术。OpenClaw audit 显示该回合零工具调用（没读 `kidpal/schedule.md`），kidpal agent 上一次 `read` 停在 2026-10-14。同 prompt、同 model、同 loopback 路径复现 9 次全部正确读取并答对 —— 故障是概率性的「该读未读」，不是 skill 缺失或工具不可用。

两条结构性成因（同批修）：

1. **识别特征枚举窄**。`openclaw-system-prompt.v10.md` 回合路由表「课表问答回合」只枚举「今天/明天/这周有什么课、几点上课、要带什么」。「后天」未命中，按表兜底落入「孩子对话回合」——该行纪律是「不调用任何工具、不读取任何文件」，模型只剩凭印象编造一条路。
2. **通用小智模板与 v10 对冲**。引擎 `prompt_manager` 把镜像内置的 `agent-base-prompt.txt`（非 bind-mount）套在 v10 外面，内含 `play_music`/`hass`/`handle_exit_intent` 工具语义、「工具不匹配就直答」、「禁 ``` 代码块」——与 v10 的「先读 schedule.md」「末尾必须带 ```display 块」直接对立。同一层还承载 `<context>` 日期/星期/天气注入，是课表算「后天星期几」的事实来源，不能裸删。

附带两项：`system_error_response` 未覆盖，出错兜底语为「主人，小智现在有点忙」；kidpal agent 19 个 modelVisible skill 中 18 个是 OpenClaw bundled/extra 噪音，挤占每次注入的 system prompt。

# 方案（包内四段，单包内实施分段，非拆包）

## A. prompt v11 四面接线

新建 `server/deploy/openclaw-system-prompt.v11.md`（v10 全文 + 仅改「课表问答回合」识别特征列，其余段零偏移）。识别特征扩为：

> 凡提到课表、课程、上课、上什么课、几点上课、要带什么，以及任何「今天/明天/后天/大后天/周X/星期X/周几/这周/下周/（某月某）号 + 课」组合，一律判课表问答回合：先读 `kidpal/schedule.md` 再作答，读不到诚实说查不到，不凭印象。

四面接线同批翻 v11，沿用既有机制的全部落点：`OPENCLAW_SYSTEM_PROMPT_CARRIER`（artifact 指针 + 沿革追加一行，历史文件不改）、`provider.config.yaml` 的 `system_prompt` 三键、`apply-xiaozhi-overlay.sh` 的 `PROMPT_SRC`/`PROMPT_DST`、`manifest.yaml` 两条 v10 条目改 v11 且 `.config.yaml` marker 锚集同步。

## B. KidPal 极简模板 + 兜底语

新建 `server/deploy/kidpal-agent-prompt.txt`，只保留三段：

- `<identity>{{base_prompt}}</identity>` —— v10 的入口（`prompt_manager.build_enhanced_prompt` 以 `config["prompt"]` 作为 `base_prompt` 传入）；
- `<context>` —— 今天日期、星期、农历、设备位置、天气；这些是课表「后天星期几」与实时事实回合的事实来源，必须留；
- 中文输出约束一句。

删除：角色白、`core_rules` 的 exit 工具条款、`anti_ai_smell`、`tts_format_constraints`（含禁 ``` 一段）、`tool_and_knowledge`、`speaker_recognition`、空 `<memory>`。

**已知引擎缺陷一并规避**：`build_enhanced_prompt` 传 `current_time="{{current_time}}"` 字面量，模板里写 `{{current_time}}` 会被渲染成字面 `{{current_time}}` 留在 prompt 里——新模板**不写** `Current time` 行，日期/星期由 `today_date`/`today_weekday` 承载。

`apply-xiaozhi-overlay.sh` 的 heredoc 增两键：`prompt_template: /opt/xiaozhi-esp32-server/data/kidpal-agent-prompt.txt`（`data/` 是 bind-mount，模板随 overlay 拷入）与 `system_error_response`（KidPal 口径兜底语，禁「小智」字样）；`provider.config.yaml` 增对应声明段。

## C. openclaw.json kidpal skills 收敛

仿 `server/deploy/perf-k1/` 的定点 patch 先例，新建 `server/deploy/261017-openclaw-kidpal-skills/`：

- `kidpal-agent-skills-declaration.json` —— 声明 `agents.entries.kidpal.skills = ["kidpal-schedule-answer","weather","tavily"]` + `drift_guard`（现状锚 = 未设）+ `backup` + `rollback`；
- `apply_agent_skills.py` / `rollback_agent_skills.py` —— 幂等应用与回滚。

`openclaw.json` 含 `gateway.token`，**整文件不入仓**，只入这份声明。

## D. 债务结项

- 两条 debt 条目翻 `status: closed` + 正文结项行 `consumed-by: 261017-prompt-v11-schedule-recall`，重跑 `node .claude/scripts/debt-render.mjs`。

> **架构面不在本包**（architect 2026-10-18 重裁）：`docs/architecture.md` §5.5.5「现状 = v10」+ §5.1 指针翻 v11 归**独立 arch 面**，由 `--agent architect` 会话在 dev 包落地**之后**执行——依据是 scope-map：`integrator.allow` 不含 `docs/architecture.md`、`planner.deny` 含 `docs/**`，dev-loop 无任何角色可写。顺序 = dev 先落 v11 实物与三面接线、arch 后翻指针（禁「指针先于实物」）；ADR-001 守卫从三面结构解析当前版、不读 arch.md，落地窗口内 arch 短暂落后不假红。

# 影响面

- 只动 `server/deploy` 部署面 + 两条 debt 条目；零固件面、零 front 面。架构 docs-sync（`docs/architecture.md` 两处指针）由独立 arch 面在本包之后落，不占本包 diff。
- 需求面零改动（课表识别扩列是实现细化，S-F1.3 能力句与 AC1 已覆盖「后天」语义）。
- 契约面零改动（display E1 / `schedule` view / B1 全不变）。
- 治理面零改动（skills 是 OpenClaw 运行时 config，非仓内 agent 定义）。
- main agent 的 skills 不受影响（agent 级 allowlist 显式替换只作用于 kidpal）。
- 设备侧行为变化仅两点：课表类问题必读文件后再答；引擎出错兜底语不再自称「小智」。

# 验收要点

## host / 静态

- v11 文件落盘，`OPENCLAW_SYSTEM_PROMPT_CARRIER` / `provider.config.yaml` / `apply-xiaozhi-overlay.sh` / `manifest.yaml` 四面接线同版，ADR-001 版本无关守卫绿。
- v10 文件 sha256 零回改（历史不可变）。
- v11 回合路由表课表行含扩列后的识别特征锚（后天 / 大后天 / 周X / 星期X / 下周 / 几号 / 提到课表）。
- `kidpal-agent-prompt.txt` 含 `<identity>` 与 `<context>`，且不含 `play_music` / `hass` / `handle_exit_intent` / `speaker_recognition` / 禁 ``` 段。
- overlay 渲染产物 `.config.yaml` 含 `prompt_template` 与 `system_error_response` 两锚，兜底语无「小智」。
- skills 声明 apply 幂等、drift_guard 生效、backup/rollback 可跑。
- `manifest.yaml` 新增/修改条目齐（v11 两条 + 模板一条 + skills 声明一条），`.config.yaml` marker 锚集覆盖新键。
- `verify-deploy.sh` 三时机核验绿（零未对账漂移）。

## live（真机轮，human 持板）

- 问「后天都有什么课程？」→ 审计记录出现 `read` 工具动作，答复与 `schedule.md` 周一一行一致（语文/数学/足球/科学/班会/语文/足球），无英语/美术。
- loopback 探针复验同口径。
- `openclaw skills list --agent kidpal` ready = 3（kidpal-schedule-answer / weather / tavily）。
- 人为触发一次引擎出错兜底，确认播报不含「小智」。

# changeManifest（预定改动面，仓根相对）

- `server/deploy/openclaw-system-prompt.v11.md`（新增）
- `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER`
- `server/deploy/provider.config.yaml`
- `server/deploy/apply-xiaozhi-overlay.sh`
- `server/deploy/manifest.yaml`
- `server/deploy/kidpal-agent-prompt.txt`（新增）
- `server/deploy/261017-openclaw-kidpal-skills/**`（新增：声明 JSON + apply + rollback）
- `server/acceptance/test_prompt_v7_round_routing.acceptance.py`（append v11 齿）
- `server/acceptance/test_261010_engine_decision21_overlay_persist.acceptance.py`（扩展新键锚）
- `docs/debt/tech/TECH-261017-schedule-answer-skip-read-hallucination.md`
- `docs/debt/tech/TECH-261017-xiaozhi-template-residue-deploy-ssot-gap.md`
- `docs/debt/tech/INDEX.md`（生成件重渲染）

# impactedTests 种子

- `server/acceptance/test_prompt_v7_round_routing.acceptance.py`
- `server/acceptance/test_m3k3_voice_schedule_entry.acceptance.py::test_a1_prompt_wired_three_faces_version_agnostic`
- `server/acceptance/test_261010_engine_decision21_overlay_persist.acceptance.py`
- `server/acceptance/test_261001_openclaw_model_name_overlay_drift.acceptance.py`（skills 定点 patch 同型守卫参照）
- `server/deploy/verify-deploy.sh`

# 风险与回滚

- **`.vN` 槽位冲突**：已核 `server/deploy/` 无 v11 占用（2026-10-18）；bump 前 builder 再核一次。
- **`.config.yaml` 是生成件**：改错引擎起不来。缓解 = host 侧单测 + 部署后 `verify-deploy.sh` + 改动前 `.bak-<date>-<why>` 备份。
- **`openclaw.json` 定点 patch**：含 secret，apply 前自动 backup、drift_guard 现状锚、rollback 脚本在位。
- **`prompt_template` 与 `prompt` 取值关系**：主线程已实读引擎源码确认（`_load_base_template` 读 `prompt_template`，`build_enhanced_prompt` 以 `config["prompt"]` 作 `base_prompt` 渲染进 `<identity>`）；builder 实现期仍须实测一次渲染产物。
- **回滚**：切回 v10（overlay 重跑）+ 删 `prompt_template`/`system_error_response` 两键 + skills rollback 脚本。

# 非目标

- 不动 requirements.md / docs/contracts/**。
- 不动 main agent 的 skills 与 QQ 录入链路。
- 不替换引擎镜像、不改镜像内 `agent-base-prompt.txt`（新模板走 `data/` bind-mount，靠 `prompt_template` 指向）。
- 不全局 disable bundled skills（只在 kidpal agent 视角收敛）。
- 不处理 `speakers_info` 注入链（小智多说话人功能，孩子对话场景不触发）。
