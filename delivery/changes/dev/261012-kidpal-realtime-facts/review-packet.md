# Review Packet · 261012-kidpal-realtime-facts

> author: integrator · date: 2026-10-12 · rigor: lite · riskTier: medium
> TaskPackage: `delivery/taskpackages/261012-kidpal-realtime-facts/taskpackage.json`
> 阶段：verifier scoped round 3 全绿（`verifier-evidence/rounds/r3.json` ok:true，invocationId `ea347132-9b7a-4f3e-875a-90fcac296517`），本 packet 供 supervisor 合并门复核；合并判裁归 supervisor 闸，本 packet 不自判通过。

## Change Size

- Files changed（对照 changeManifest 9 项，全集在场、零清单外实现面）：① `server/deploy/openclaw-system-prompt.v10.md` 新建（80 行）② `OPENCLAW_SYSTEM_PROMPT_CARRIER`（现值 v9→v10 + 演进史 append，历史条目零删）③ `provider.config.yaml`（system_prompt 三键 v10 + 结构注记行）④ `apply-xiaozhi-overlay.sh`（PROMPT_SRC/DST v10 + 注记行）⑤ `server/acceptance/test_prompt_v7_round_routing.acceptance.py`（additive RF 齿 +338 行，tighten-only 窗口）⑥ `openclaw-agents/workspace-kidpal/AGENTS.md` 新建（15 行）⑦ `openclaw-agents/README.md` 新建（20 行）⑧ `docs/architecture.md`（§5.1 + §5.5.5 指针刷，architect handoff，2+/2−）⑨ `server/deploy/README.md`（指针节）。治理面（非 changeManifest）：CRQ TECH-261012 收口节 + tech/INDEX 重渲染、TaskPackage 目录制品（taskpackage / verifier-evidence r1-r3 / verify-runs / pod-events / input-brief ×2 / test-scope-final）、`current/` 投影翻新、本包 `delivery/changes/dev/` 晋升件。
- Net diff: server/docs 追踪面 +393/−37，新建件 +115 行（v10 + 镜像两件），合计约 +508/−37；契约面 0 行，固件 0 行。
- Scope: 单面 server/deploy prompt 族版本化 bump v9→v10（实时事实回合 + 通用反编造）+ 四面接线 + AGENTS.md 镜像 + docs-sync；固件零改动（firmware-build 闸真编译绿 + `git diff firmware/` 空，双重实证）。
- Split recommendation: 不拆——单语义（v9 逐域枚举漏通用实时事实域）驱动的原子 bump，接线与本体必须同批翻（四个版本无关守卫「同批翻即绿、漏翻必红」结构性要求原子提交）。

## Verification Evidence

- Structural gates: verifier scoped round 3 全绿（r3.json 八闸 ok：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build 真编译 build-pass / contract-lint / acceptance-coverage（RF-A1..A5 全映射，structural=[RF-A5]）/ mutation-check 豁免留痕 / docs-sync / security securityReviewRequired:false）。round 1→3 迭代留痕：r1 acceptance-coverage 红（RF-A5 未豁免标注）→ r2 mutation-check 红（mutationSkipReason 缺失）→ r3 全绿，均为 TaskPackage 元数据补正，非实现返工。
- Acceptance: 范围圈定终验（integrator 2026-10-12 实跑）`cd server && python -m pytest -q --import-mode=importlib -m "not live" acceptance/` = **152 passed / 4 deselected / 9 subtests passed，exit 0**——含本包 RF-A1..A4 新齿转绿 + 四个版本无关守卫（m3k3 A1 / MCD / M4K3-A8 / PN-A4）与 261001 / 决策21 守卫零降。红基线（acceptance-author 冻结实跑）：宿主 6 failed / 12 passed，红全部设计态、原因单一（RF-A1×2 + RF-A2×3 + RF-A4 半同构红），RF-A3 与既有 12 绿零触碰零降。test-scope 清单落盘：`delivery/taskpackages/261012-kidpal-realtime-facts/test-scope-final.txt`（detect_changes 正常圈定路径：Pod 专属图索引，inbound blast radius = 0〔验收齿叶子符号 + 数据文件，v9 同形态〕，圈定面 = 接线守卫齿 + 种子 + 安全余量 7 文件；上述全量 152 绿为该清单 superset 强旁证）。**final bearing verification（phase=final + 本 scope-file）待 planner 以该清单执行**——本包 scoped r3 全绿 + 全量 superset 绿已具备，final fact 为 planner 收尾相位动作。
- 零触碰核证（圈定面外零 diff）：`git diff firmware/` 空；`server/deploy/openclaw-system-prompt.v9.md` 及历史 v1..v8 零回改（RF-A3 sha256 机检 + git diff 实证）；`server/acceptance/` 仅 additive 宿主一文件在 diff，其余验收件零触碰；`docs/contracts/**` 零 diff（contract-lint 绿 + NO_CONTRACT_CHANGE 四分类留痕）。
- Security gate: security 闸绿（securityReviewRequired: false）。新出网路径（Tavily 经平台层）已记账 NFR-SEC-2 ④（requirements v1.62，先行捎带批 9d07329）；无凭据落仓、无注入面变化；overlay 渲染路径 secrets 注入逻辑零改。
- Mutation check: 豁免（mutationSkipReason 留痕）——prompt 内容 + 配置接线 + additive 验收齿批，无 DB/事件/落库 mutation 可取证；敏感性由 RF-A1..A4 内容锚 + RF-A3 sha256 负锚 + 四守卫承载；live 行为观测归 RF-A5（structural 豁免，非自动断言）。
- Docs sync: **完成申报**——arch §5.1/§5.5.5 指针刷 v10（architect handoff 路径，pod-events 在案）、README 指针节核证、CRQ 收口、docs-delta 落盘（`delivery/changes/dev/261012-kidpal-realtime-facts/docs-delta.md`，逐行状态表）；requirements v1.62 为先行捎带批（非本包 diff，指针注明）。

## 验收面对账（RF-A1..A5）

- **RF-A1（v10 落盘非空 + 首行 `## 角色`）**：绿——红基线设计态红 ×1 → 修后绿。
- **RF-A2（实时事实回合行锚 + 对话行通用反编造锚 + 底线节第三不变量，内容驱动不钉措辞）**：绿——设计态红 ×3 → 修后绿；v10 :8-9 回合路由表实时事实回合独立行 + 对话行改走条款 + :20 底线三不变量在世。
- **RF-A3（v9 sha256 零回改守卫）**：绿（红基线已绿、零回改维持）。
- **RF-A4（四面接线一致承载实时回合锚 + 反编造锚，红→绿半同构）**：绿——红基线半绿半红（现接线 v9 无该行处红）→ 四面同批翻 v10 后绿。
- **RF-A5（live 观测项，structural 豁免申报）**：**结构性豁免如实申报**——非确定性 live 断言（真机问天气全链路 + 降级路径一次 + 延迟实测），不计入 pytest 通过面、不预支绿；已完成的先行实证 = Tavily kidpal 会话 loopback 探针（B/C/F，audit `web_search succeeded`）+ **Probe A（v9 态编造复现）→ Probe F（v10 态提示语 + 真查）前后对照**（builder 实证，原始日志在 ECS live 面未落仓）；剩余 = human 在场真机轮（journalctl + docker logs 双侧对账 + 目击）。先例同款：PN-A5。

## Demo 对账

- **offline 齿红→绿全程留痕：已具备**——红基线 6 failed / 12 passed（冻结实跑）→ 修后全量 152 passed / 0 failed / 9 subtests，见 verify-runs.jsonl + verifier-evidence r1→r3。
- **ECS 部署与 Tavily 实证：已完成（builder，launchPrecondition 顺序合规）**——Tavily kidpal 会话实证先于 v10 部署；overlay 重跑后 model_name=openclaw/kidpal 与决策 21 两键核对未被冲（261001 冲刷陷阱未复发）；AGENTS.md 先改仓内镜像再同步 live（顺序合规）。
- **真机轮（RF-A5）：pending-human**——见上验收面对账行与 Residual Risk。

## 未覆盖面申报

1. **offline 无行为驱动方（已申报盲区）**：实时事实回合的「提示语先播 → 真查 → 只依据结果作答」是 LLM 运行时行为，offline 验收只能锚 prompt 文本与四面接线一致性，**无任何离线断言能驱动该行为**——这是本包结构性盲区而非遗漏，plan/mutationSkipReason 前置申报在案。部分补偿：loopback Probe A→F 前后对照已在 ECS 侧观测到行为翻转；最终证据归 RF-A5 真机轮 human。
2. **行为等价假设「prompt 写明 ≠ LLM 遵循」**：loopback 侧已观测遵循（Probe F 提示语 + 真查 + 依据结果作答），但单轮探针不构成遵循度保证；真机全链路待 human 目击，长尾漂移（模型换代改变输出风格）无结构性兜底（引擎侧 strip 为 plan 非目标）。
3. **RF-A5 真机轮**：pending-human——问天气全链路（提示语 ~4-5s 内首句语音 / 真实答复目标 ≤15s 上限 20s，先观测后钉阈值）+ 降级路径一次（查不到 → 诚实 + 引导、零编造），双侧日志对账 + human 目击。
4. **延迟阈值**：未钉——human 批口径「先观测后钉」，硬阈值留后续批。
5. 其余面：**none**——契约条目零触（display 块规则零变）；固件零改动；日期/星期类 RTC 直答边界已在 v10 行内显式排除（不进实时事实回合）。

## supervisor all-mock 地雷前置申报

**全文件内容锚 + live 观测项、无 DB/事件断言 → supervisor 机械闸可能判 all-mock、触发 riskTier 重算（medium→high）→ 需 human 放行**（TaskPackage notes 前置申报在案；**v9 同款先例**：261010-prompt-v9-schedule-anti-narration review-packet 同段申报）。本包断言面构成：prompt 文件内容锚（RF-A1/A2）+ sha256 负锚（RF-A3）+ 接线一致守卫（RF-A4）+ structural 豁免（RF-A5）——文本/接线类变更无 DB/事件可断言是其本性，非 mock 规避；mutation 豁免同样源于此。复核者请按 v9 先例形态解读。

## Tavily 外部内容儿童化新暴露面申报

实时事实回合首次把**外部互联网内容**引入儿童语音出口——新暴露面，三层防护在位：① prompt 纪律（v10 实时事实回合行「外部查询结果面向孩子转述须儿童化改写、不逐字照搬网页原文」+ 不合适内容降级）② AGENTS.md 镜像底线节「转述守边界」条 ③ requirements NFR-SAFE-1 联网转述边界（v1.62，行为语义权威）+ 平台 EXTERNAL_UNTRUSTED_CONTENT 包裹在位。**残余风险**：儿童化改写是 LLM 判断而非机械过滤，边界失效无结构性拦截——真机轮重点观测项 + NFR-SAFE-1 ACCEPT-AS-RISK 形态覆盖；出网路径已记账 NFR-SEC-2 ④（Tavily 平台既有计费能力，无新设施）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （无 `minimal:` 标记） | — | — | no-trigger |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| （无 BLOCKING） | — | — | — | — | — |
| NIT | judgement-call | provenance | Tavily loopback 探针（B/C/F）与 Probe A→F 前后对照的原始日志在 ECS live 面（journalctl + audit），未落仓留档；本包制品只承载 builder 回执口径 | `delivery/taskpackages/261012-kidpal-realtime-facts/` 各制品 provenance 标注 + changed-code（CRQ 收口节「原始日志在 ECS live 面……此处为回执口径」）+ 诚实留痕 heuristic（重资产留 live 面与仓内 bootstrap 纪律一致，判 no-action） | 不动——真机轮 RF-A5 双侧对账时一并留痕即可 |

## Five-Axis Review

- **Types（语义一致性）**：v10 = v9 全量 + 三处 additive（实时事实回合独立行 / 对话行改走条款 / 底线第三不变量），diff 实证既有回合行、查字、故事、提醒、硬约束段落零偏移；新行「识别特征 / 纪律 / 降级 / 禁编造 / 儿童化转述 / RTC 排除」语义自洽，对话行与实时事实回合行无句内矛盾（改走而非白名单，v7 整改的回合路由消矛盾纪律延续）。
- **Contracts（契约面）**：零触——NO_CONTRACT_CHANGE 四分类（architect-reviewer 2026-10-12，spine anchors.contract）：契约 §1.6 :182-185 逐字引文只钉四语义面（两部分格式 / display 七 view 白名单 / 课表只文字答+薄载荷 / 星星只文字答），v10 additive 逐面核对零冲突；display 块规则零变；`docs/contracts/**` git diff 空 + contract-lint 绿双证。
- **Process/state（接线一致性）**：四面接线一致 = v10（CARRIER artifact 行 == provider.config artifact/source/deploy_target 三键 == overlay PROMPT_SRC/PROMPT_DST），RF-A4 版本无关守卫机检；演进史 append 零删（CARRIER v9 条目原样保留、v10 条目新增）；ADR-001 守卫版本无关形态保持；AGENTS.md 镜像「冲突以注入 prompt 为准」自声明保持两源不漂移。
- **Security（安全）**：新出网路径记账在位（NFR-SEC-2 ④）；Tavily 平台层既有能力、零新工具设施；无凭据/注入面变化；外部内容儿童化边界三层防护在位（见上专节）；不弱化任何既有安全语义（对话行禁工具、查字绝对禁工具原样保留）。
- **Ops（可运维）**：ECS 部署已完成且核对面全过（model_name + 决策 21 两键未冲）；部署 runbook（README 动作序）对本 bump 仍适用；启动标记行等既有机械报警锚零改；降级口径对 Tavily 工具缺席鲁棒（v10 行内「工具不可用 → 诚实说没查到」）。

## Residual Risk

- Known limitation:
  - offline 无行为驱动方（已申报盲区，见未覆盖面 1）；行为等价假设的真值最终归 RF-A5 真机轮。
  - Tavily 外部内容儿童化边界为 LLM 判断面，无机械拦截兜底（见上专节）。
  - 本包验收全为文件内容锚 + live 观测项、无 DB/事件断言——supervisor 机械闸若判 all-mock 触 riskTier 重算 → 需 human 放行（前置申报，v9 同款先例）。
- Follow-up recommended: planner 以 `delivery/taskpackages/261012-kidpal-realtime-facts/test-scope-final.txt` 执行 final bearing verification（phase=final + scope-file）；真机轮后按实测钉延迟阈值（后续批）。
- Human decision needed: RF-A5 真机轮（human 在场：问天气全链路 + 降级路径一次，journalctl + docker logs 双侧对账 + 目击）；若 supervisor 闸判 all-mock，riskTier 重算的 human 放行。
- `taskpackage.decisionCoverage` 中 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目：**none**（本 TaskPackage 无 decisionCoverage 字段，无风险接受式丢弃决策）。
