# Review Packet —— 261010-p3-storylist-cold-start-restore

> integrator 制品轮产出（2026-10-12）。状态：final bearing verify（round 5，scope 圈定）全绿 + 契约门 CLOSED（LANDED）+ closeout 制品齐，feature 分支 commit 待合并门；supervisor 裁断与 merge-driver 归 planner 主线程。

## 来源链

- 真机反馈（2026-10-10 human 第 5 条「P3 停默认空态」）→ TECH-261010-p3-storylist-cold-start-empty（根因与 scoping 权威）→ spine `delivery/changes/intake/261010-p3-storylist-cold-start.md`（OQ-1 human 裁 restore 标记方案）→ requirements §9 v1.63（frozen 2026-10-12，档 1 author→reviewer 闭环）+ arch :186/:214 镜像同拍 → 契约门 CCP HUMAN_APPROVED 2026-10-12（D1–D4 先批后落）→ 本包 dev-loop（launch prep @63c49cc 双面基线锚：server 152P / firmware 155T）。

## Change Size

- Files changed（工作树全量，含文档与 delivery 制品）: 24 modified + 5 新增（input-brief 两件 / verifier-evidence 轮事实 / verify-runs.jsonl / test-scope-final.txt / crq 归档件）；实现面 = firmware 7（mcp_server.cc / kidpal_render_node.cc/.h / story_page.c/.h / story_page_v2.acceptance.c / story_page_v2.test.c）+ server 4（front_app.py / display_schema.py / test_m4k1 / test_m4k3）；文档面 = 契约 + ADR + crq 四路径；净 +1418 / −354。
- Scope: 跨固件/云端两子面 + 契约同步——front 建连恢复推送（restore=True，try/except 不阻断建连）+ 板端 restore 分流（只填 RAM 缓存不接管前台、P3 在屏仅重绘本页，复刻 schedule :926-991 形态）+ 空态两态分叉渲染（AC5 ×0 计数 + 话术 / AC6 不显计数 + 话术）+ FIX-A1 负锚齿翻正（两冻结件 REVISION LOG 授权链）+ display_schema V27 floor 联动扩 + 契约 D1–D4。
- Split recommendation: 不拆——契约/固件/云端/齿面同批原子变（契约门 C2 = 面 B 分类成立前提，拆批即回炉）。

## Verification Evidence

- verifier-round fact（bearing，final）: `delivery/taskpackages/261010-p3-storylist-cold-start-restore/verifier-evidence/rounds/r5.json`（phase=final, round=5, invocationId=7734c17d-81f6-4f36-a032-a727545f0e70, ok=true, observedGateExit=0）——十闸全 PASS：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build（真编译，xiaozhi.bin 0x312100 bytes、app 分区 11% 余量）/ contract-lint / acceptance-coverage / mutation-check / docs-sync / security / unit（full）/ acceptance（full）。scoped r1–r4 轨迹 = 同目录（r4 为 verifier 最后 scoped 轮全绿）。
- verifier-evidence-check: `{"ok":true,"gate":"verifier-evidence","evidenceRef":".../rounds/r5.json","factOk":true,"observedGateExit":0,"phase":"final","round":5}`，exit 0。
- **影响面圈定终验（integrator 2026-10-12 实跑，Step 4 两步走）**：
  - **① 范围圈定**：detect_changes（Pod 专属图索引 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`，base=main，merge_base 63c49cc，inbound depth=2）= 27 seed symbols / 77 impacted（firmware/main 18 + server/gateway 14 + server/acceptance 37 + server/tests 8），零跨模块越界、零预期外符号；∪ taskpackage impactedTestsSeed（SL-A1~A6 承载件）∪ 共享 seam 消费面（display_schema floor 的 M3K1/M3K3 schema 齿 + story_units + reminders hop1）∪ 安全余量（K7A4 hop2、producer units）。**test-scope 清单落盘路径（final verify --scope-file 消费）: `delivery/taskpackages/261010-p3-storylist-cold-start-restore/test-scope-final.txt`**（留根观察「圈定 vs 事后全 blast radius」差集的锚）。Pod 专属图索引用常（launch 面必办项在位），无降级；语义铁律合规（非全量、非本包档缩水）。
  - **② final bearing verification**（scope 圈定面实跑）：unit 闸 `183 passed, 3 subtests passed`（scope 交交集 8 文件 = 3 units + 5 acceptance）；acceptance 闸 `92 passed, 3 subtests passed`（5 server 冻结/相关件 + firmware acceptance oracle 1 件）；firmware host 单 ELF 全量 `162 Tests / 0 Failures / 0 Ignored, OK`（WSL espidf_wsl.sh build「no work to do」+ host_infra_test.elf 实跑）——对红基线（159T/3F 设计态红）3 红全数转绿、既有 155T 零损失。SL-A5 live-manual 真机轮不在自动断言面（human 持板，见未覆盖面申报）。
- 契约面: contract-lint PASS（r5）；CCP §2 逐字核对 = gate evidence §5（五条净内容行程序 needle 逐字 HIT + 实测 5 added / 2 removed + CRLF 保全字节级实证：282 行全 CRLF、0 裸 LF）；JSON 三例回归（restore PASS / 非 restore PASS / 畸形 restore REJECT）= gate evidence §7。
- mutation check: PASS（r5；六条 perAcceptance SL-A1~A6 全 turnedRed=true 且还原复绿）。
- FIX-A1 翻正授权链（exitCriteria③）: REVISION LOG 2026-10-12 条三件齐（CCP id + requirements §9 v1.63 行 + human 方向锚留痕）在冻结件留痕（test_m4k1 :1856-1846 区段，integrator grep 实证）；test_m4k3 :25 跨包负锚同批翻正。
- 影响面核对记账（Step 9b advisory，范围判定归 test-scope 清单）: detect_changes 77 impacted 全落预期面——front_app 既有消费方函数族（_push_display_payload / _consume_kidpal_control 等）、reminders hop1（validate_alert_entry 等）、schema 消费面齿为 blast radius 命中（既有在役消费方被影响、非被改动），两面实跑全绿实证零回归。
- final fact 与圈定路径对应: verify.mjs fact 顶层键白名单封闭、scope-file 不入 fact 元数据——落盘路径锚 = 本节 test-scope 行 + r5 轮 fact 由 verifier-evidence-check 校验对应同一 invocationId（留根纪律以本 packet 行承载，261011 同款）。

## demo 对账行（pod-coordination §11，与未覆盖面申报同址正交）

- **demo 证据 = 本轮两面实跑输出 + 产物构建留痕**：① server scope 面 92 passed（SL-A1 恰一帧 restore / SL-A6 推送失败不阻断 / ②③零偏移 assertNot restore / K1 27 齿 / M3K1·M3K3 schema 齿 / K7A4）；② firmware host 162 Tests / 0 Failures（SLP-SLA2/3/4/5 新齿全绿 + 既有套件零损失）；③ r5 firmware-build 真编译留痕（xiaozhi.bin 0x312100 / art.bin spiffsgen 重打包 / 分区余量 11%）；④ JSON 三例回归（restore / 非 restore / 畸形）过 V27 floor。与本包效果段（story 契约 + front 恢复推送 + 板端 restore 分流 + 空态两态渲染）一致性结论：**一致**——各效果点有在世齿承载且全绿。
- **真机轮 = human 持板硬前置（未跑，如实标注 pending-human）**：SL-A5 重启冷启动翻 P3 见列表含计数 + 任意页无 P3 抢占 + 空态两态话术各验收一次——无机器证据替位，为 exitCriteria⑤ 承载位。

## Provenance 申报（诚实留痕）

- **前序 builder 重试轮半成品**：builder 重试轮中断留下半成品工作树（含 kidpal_render_node.cc AC6 分支 MUTANT 变异取证残留），续跑 builder 按升级裁定「不 redo 已落盘半成品」接棒，先还原 MUTANT 残留再全量复核——taskpackage notes（exitCriteria⑦）与 mutationEvidence SL-A5 notes（「摘 AC6 macro 消费（builder 还原前序残留 MUTANT 时取证）」）在案。
- **acceptance-author brief 前置降级**：input-brief-acceptance-author.md 首轮 manifest-missing（planner Step 5 前置漏跑 brief-inputs.mjs），author 按角色规范降级自扫并显式标注（未静默）；builder 派发前 brief-inputs.mjs 补跑。
- **超预算面**：iterationBudget 3 / fixBudget 3 / supervisorBlockBudget 2——本包 verifier scoped 走 r1–r4 轮（含 brief/manifest 前置红与齿位对齐修），无 supervisor block 消耗。

## 未覆盖面申报（对照 acceptance[] + 契约条目 + spine 效果段）

- SL-A5 真机冷启动轮——pending-human（exitCriteria⑤；human 在场持板：重启见列表含计数 / 任意页无 P3 抢占 / 空态两态话术各验收一次；host 结构齿 + 变异取证为替位证据、非真机目击）。
- AC6「不显计数」半——host 不可达（structural kind 豁免自动断言面），归 SL-A5 真机轮；host 可测前置半（AC6 话术渲染）已落齿并变异取证。
- ECS 恢复推送开关 human 人工开闸（gate evidence C1；默认关在役）——pending-human；ECS front 部署（端云同批部署位）pending，归 planner/主线程部署位。
- 其余 acceptance 条目（SL-A1~A4/A6 + 既有 K1/SPV2/KD/K7/SAO 守卫齿）：已覆盖（final 轮全绿），零缺口面 = none（除上述 pending 外无未申报面）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| firmware/main/display/story_page.c:986/:1246/:1328/:1372 | 既有 `minimal:` 标记四处（非本包新增——`git diff` grep `minimal:` 零新增命中；本包改动文件内既有标记原样在世） | 各标记自带 ceiling 注记 | 沿用各标记既有 upgrade trigger，本批零新增债务 |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | 派单/scope-map | 派单把收口义务三件（CDQ 行 append / arch :186/:214 挂法句翻既成 / spine anchors.contract 回填 + status 收口）列入 integrator 义务，但 scope-map integrator allow 面均不含对应路径——三件 Edit 全被 enforce-scope 拦截（拦截回执在案），全文交 planner 代落。261011 CDQ 同款已复发第二次（该包 retro 改进项「收口义务清单逐文件核写权」未沉淀为派单前置），arch/spine 为首见 | changed-process hunk（本包派单「任务 2」vs enforce-scope.mjs 三次实拦回执）；机械闸优先 heuristic——非契约/spec 字面违反，不引规则前缀 | planner 代落三件（文本见「交 planner 代办」节）；后续派单前置对 scope-map 核 integrator 收口面写权（扩 scope-map 须 human） |
| NIT | judgement-call | gate evidence §5 | gate evidence 原稿预判「契约落本批后 arch :186/:214 两句**自动转既成——无需回改**」，派单则明确要求翻既成落盘——两口径不一致，按派单从严执行（patch 已拟待 planner 落）；预判句已随 closeout 回填改写，不残留误导 | changed-artifact hunk（原稿 §5 vs 派单任务 2）+ 记账一致性 heuristic | 无（随 planner 落 patch 消解） |
| NIT | judgement-call | taskpackage-required | sourceRefs 含锚点形态 `docs/requirements.md:S-F2.1`，闸报 warning「path-not-on-disk」（锚点形态非文件路径，warning 级不红）——锚点形态引用约定与闸解析能力的历史错配，r1–r5 恒现 | gate stdout hunk（r5 taskpackage-required warnings）+ 留痕诚实性 heuristic | 无（warning 不阻塞；如后续静默化可在 anchor 解析层收口，非本批义务） |

结构闸 PASS/FAIL 不复述为 finding（gate 全绿仍完整执行五轴语义 review，见下）。无 hard-violation finding——如出现将以 `docs/contracts/**` / `docs/requirements.md` / `docs/architecture.md` 字面前缀起引 Basis；本轮 none。

## Five-Axis Review

- 安全：零新增攻击面——restore 字段 additive 且仅 front 建连附带推送路径可置（生产纪律入契约 D1）；V27 floor 同批扩（畸形 restore 非 bool 字面 REJECT 实证）；LLM 直产 restore 载荷路径不存在（relay 收紧 Q7 单源，区别于 §1.3② 的 F-C5 残余风险面，ADR-010 如实记差异）；建连推送 try/except 失败不阻断建连（SL-A6 齿在世）；零新增依赖、零新增出网路径。
- 契约：D1–D4 逐字落地并经 human 契约门（先批后落、同批原子 = C2 兑现）；板子行为句例外子句 / floor 枚举 / 变更记录行与 §1.3② ADR-005 先例形态逐句同构；缺省/false 设备行为完全不变（K2 消费面零偏移 assertNot restore 齿在世）；变更记录行先例格式在场。
- 边界与依赖卫生：零新增依赖；restore 分流复刻同文件 schedule :926-991 在役形态、零新基建（mcp_json_get_bool 复用、on_screen 谓词与 home/schedule 逐字同构、refresh_story 单入口自持锁）；空态渲染入口两处同源纪律不破（story_page.h KSTORY 宏单源，SPV2-B2 续绿）；seam 四件宣告与实现一致（taskpackage notes ⑤）。
- 可运维性：restore 载荷板端解析失败 = 静默降级（记日志、缓存零变化、不动前台），front V27 校验面入板前独立生效——两层降级互不替代；ECS 开关默认关 + 烧板验证后人工开闸（schedule 先例同型），回滚 = 契约 diff revert + 实现包 revert 同批；ctx(story) 条件推送保留（冻结齿硬约束，建连段 ≥1 帧与 ctx 段既有断言并存，归并处置零动摇 arch :214 表述）。
- 资源与功耗：restore 帧复用既有 story_list 载荷上限（items ≤16 窗口 / MAX_DATA_BYTES 65536），板端只写 RAM 列表缓存（决策 24 不变式保持，NVS 备选明确否决）；app 分区余量 11% 不变、无 spiffs 资产变更（app 分区单烧口径核对通过）。

## Residual Risk

- Known limitation: SL-A5 真机冷启动轮 pending-human；AC6 不显计数半 host 不可达（真机轮覆盖）；ECS 开关开闸与 ECS front 部署 pending-human/部署位。
- Follow-up recommended: 派单前置「收口义务清单对 scope-map 核写权」（261011 改进项二次触发，见 Findings SHOULD 行）；既有冻结件陈旧注释指针（record_session_live.acceptance.c:16-22 / M4K1-ACCEPTANCE-NOTES.md 引旧齿名）= 纯注释零断言承载，planner 裁不修、留档备查。
- Human decision needed: SL-A5 真机轮持板 + C1 开闸（human）；riskTier=medium + 已知 all-mock 申报（SL-A1~A4/A6 wire 帧与 host 纯逻辑断言、无 DB/事件断言——261012 v9 同款先例）→ supervisor 机械闸如判 all-mock 触 riskTier 重算 high，升 human 放行。decisionCoverage 中 `landsIn: dropped && reason: risk-accepted` 条目 = 无（本 taskpackage 未启用 decisionCoverage 字段，Residual Risk 节按模板申报零条目）。

## 集成执行回报

本轮无 merge-driver 冲突消解（单包制品轮，Pod 分支无对端冲突，全部改动为工作树未 commit 态一次收口），集成复跑清单 n/a；圈定 floor 注（双方冻结件 ∪ 圈定清单）不适用——单包无对端冻结件，floor = 本包三份冻结验收件（test_m4k1 / test_m4k3 / story_page_v2.acceptance，均在 test-scope-final.txt 首段并实跑全绿）。已知锚 @fde1eb9（progress.md 带锚行：server 152P / firmware 155T）供引用；全量回归 = 人工触发位，本轮只跑 scope 圈定面。

## 交 planner 代办（本轮回执）

1. **CDQ 行代落**：`docs/CONTRACT-DECISION-QUEUE.md` 表末 append `CDQ-261010-p3-storylist-cold-start-restore` 行（integrator Edit 被 enforce-scope 拦截，机械闸优先未绕行；261011 同款）。
2. **arch :186/:214 挂法句翻既成**：`docs/architecture.md` 两处最小 patch（见下）。
3. **spine 回填**：`delivery/changes/intake/261010-p3-storylist-cold-start.md` frontmatter `status: in-flight` → `status: landed`；anchors 回填记录节追加 contract 落地态条目（见下）。
4. **final bearing verify 已由 integrator 实跑完成**（r5 全绿 + verifier-evidence-check {ok:true,factOk:true}）——supervisor-verdict-gate 与 merge-driver 归 planner 主线程流程；supervisor-verdict write-once（verdict 在盘重跑须先显式删旧件）。

CDQ 行全文（planner 代落用，append-only）：

> `| CDQ-261010-p3-storylist-cold-start-restore | 2026-10-12 | interface-contract.md §1.3③ 四处（data.restore additive 字段定义 + 生产纪律两 bullet 插入 / 板子行为句 restore 例外子句 modifying / §1.5 第 3 条 floor 枚举补 restore 可选布尔 / 变更记录行 append）——D1–D4 逐字 diff | CONTRACT_CHANGE_REQUIRED（恢复性推送与主动推送需 wire 区分，复刻 §1.3② / ADR-005 先例；预审裁定表见 packet/gate evidence §3，面 A=CONTRACT_CHANGE_REQUIRED、面 B §1.5 :180 枚举句=NO_CONTRACT_CHANGE） | human 契约门 AskUserQuestion 批准 2026-10-12（packet §5，四触点一次批；附带裁定 = ECS 开关默认关→烧板验证后人工开闸 + ADR-010 随批 + 面 B 不动） | **LANDED**（2026-10-12 契约 diff 随包同 PR 落地；逐字核对留痕 = gate evidence §5，五条净内容行程序 needle 逐字 HIT + 实测 5 added / 2 removed；CRLF 保全字节级实证） | 261010-p3-storylist-cold-start-restore | 无——实现面随包落地；ECS 恢复推送开闸归 human（gate evidence C1，非契约门条件）；tech 记账指针 = docs/crq/tech/TECH-261010-p3-storylist-cold-start-empty.md（随批 closed，已归档 _archive/2026/） |`

arch :186 patch（最小改动，只翻挂法）：

> 旧句（:186 front 网关行内）：`additive 字段随契约门落，沿革与语义权威 = §5.2 场景④`
> 新句：`additive 字段已随契约门落（2026-10-12 human 批准，随 261010-p3-storylist-cold-start-restore 同批原子落），沿革与语义权威 = §5.2 场景④`

arch :214 patch（最小改动，只翻挂法）：

> 旧句（:214 §5.2 场景④内）：`**随契约门落、尚未落地**`
> 新句：`**已随契约门落地（2026-10-12 human 批准，随 261010-p3-storylist-cold-start-restore 同批原子落）**`

spine anchors.contract 落地态回填（anchors 回填记录节追加，append-only）：

> - **contract·落地态**（2026-10-12）：契约 diff D1–D4 已随 261010-p3-storylist-cold-start-restore 实现同批原子落（C2 兑现）——逐字核对 = gate evidence §5（五条净内容行 needle 逐字 HIT + 5 added / 2 removed + CRLF 保全字节级实证）；JSON 三例回归过 V27 floor（gate evidence §7）；gate 状态翻 LANDED·CLOSED（§10，C1 = ECS 开关默认关、烧板验证后 human 人工开闸，仍 pending 归 human）；ADR-010 随批新增；CDQ-261010 LANDED 行 + arch :186/:214 挂法句翻既成 = planner 代落（integrator scope-map 通道错位，见 review-packet Findings）。
