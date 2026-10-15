# Review Packet —— 261014-engine-stop-truncation-asr-filter

> integrator 收口制品轮产出（2026-10-14）。状态：**scoped round 3 全绿（八闸 PASS，bearing fact = `verifier-evidence/rounds/r3.json`）** + integrator 独立复跑双绿（相关子集 10 passed + 24 subtests；全 acceptance 目录 174 passed / 4 deselected / 33 subtests 零新增红）+ closeout 制品齐（docs-delta.md / review-packet.md）+ 影响面圈定清单落盘（`test-scope-final.txt`，final bearing 轮 r5 由 planner 持此清单跑）。**supervisor 裁断与 merge-driver 归 planner 主线程，本 integrator 不判定通过、不 merge、不 push**。rigor=lite、sourceType=regression、riskTier=medium（ET-b manual 豁免漏做 = 故事归档毁稿级回归，不宜标 trivial）。
>
> 轮次沿革：r1 scoped（红——taskpackage 缺 `mutationSkipReason` 字段，申报形态缺陷）→ r2 scoped（红——缺 `docsSyncReason` 字段，同族申报形态缺陷）→ **r3 scoped（八闸全绿，当前 scoped 收口证据态）**。r1/r2 两红均为 TaskPackage 申报字段补齐问题（`mutationRequired=false` 未带 skip 理由 / `docsSyncRequired=false` 未带理由），非实现或测试红。

## 来源链

defect of record `docs/crq/tech/TECH-261014-engine-asr-hallucination-after-stop.md`（真机两轮逐字命中：查字/播报收尾后引擎对尾音幻听出「不客气」寒暄 + ASR 静音幻听 "Thank you." 被 LLM 回复推送）→ architect `/triage-change` 分流 review 产出 sibling `plan.md`（2026-10-14，1g 前置技术预评本体，frontmatter `reviewed-by` 行为证；**承重发现 = manual 豁免**：manual 模式的 stop 触发 ASR 是 front pending-stop 归档承重通道 SAO-A2，盲截断即毁稿）→ human triage 触点①「顺势把它跟 engine-patch 一起做成一个小批，做吧」已裁并留痕 plan.md frontmatter → 本包单包。

## 根因与修法

- **根因**（权威 = plan.md + CRQ，不复制证据链）：listenMessageHandler stop 分支在非流式 ASR（Qwen3ASRFlash NON_STREAM）下直接触发缓冲尾音整段识别，stop 分支无 auto 路径既有的最短音频门——设备正常收尾后引擎仍产出一条幻听 ASR 送 LLM/TTS。正常对话轮 ASR 由服务端 VAD 触发、不经 stop，截断 stop 触发路径不伤正常对话。
- **改动点 1（listen 帧旁路截断）**：`connection.py` 新增 `_kidpal_on_listen_message(message)`（async），在 `_route_message` 内 kidpal_speak 认领之后、handleTextMessage 之前调用。解析 JSON（畸形静默返回）；type=="listen" 且 state=="start" → 复位 `_kidpal_listen_closed=False`；state=="stop" 且 `client_listen_mode != "manual"` → 置 True + `asr_audio.clear()` + 非阻塞排空 `asr_audio_queue`（get_nowait 循环）+ 记 info。承重顺序 = 先清后转：handleTextMessage stop 分支读到空 asr_audio 经 len()>0 门不进，stop 触发 ASR 整体不发生；消息照常转 handleTextMessage，上游语义零改。
- **改动点 2（chat 入口守卫）**：`chat` depth==0、在 `self.dialogue.put` 与 TTS FIRST 入队之前：`_kidpal_listen_closed` 置位则丢弃在途 ASR 轮（兜 stop 前 VAD 已触发的竞态窗 ~1s）；命中 `_kidpal_is_asr_hallucination(query)` 则丢弃幻听串。**不置 client_abort**（避免波及 stop 后 front 注入的合法 speak/story 下发）；守卫仅 depth==0（递归轮不拦）。
- **改动点 3（幻听串表）**：模块级 `_KIDPAL_ASR_HALLUCINATIONS = frozenset`（"thank you" / "thanks for watching" / "thank you for watching" / "谢谢观看" / "请不吝点赞"，逐条来源注记随注释在案）+ `_kidpal_is_asr_hallucination(text)`：去中英标点（保留空格）→ strip → lower 后**整串相等**匹配，绝不子串（「让我们一起说thank you吧」不误杀）。放 LLM 入口（chat）而非 ASR 结果入口：单收口在 patch 文件内、离线可测。
- 附带：`_initialize_components` 同区追加启动 marker「kidpal stop-truncation/asr-filter patch active」（部署生效锚）。

## Change Size

- **working-tree diff 面**：`server/deploy/engine-patch/connection.py`（+93/−0，六 hunk）+ `server/acceptance/test_engine_stop_truncation_asr_filter.acceptance.py`（新增 804 行，ET-a..g 七 case）→ 2 文件 +897/−0；执行 ledger（本包 taskpackage 目录：taskpackage.json / plan.md / pod-events.jsonl / verify-runs.jsonl / verifier-evidence/rounds/{r1,r2,r3}.json / input-brief 两份 / test-scope-final.txt）+ `current/` 投影 + 本 closeout 目录。
- **Scope**：引擎侧内部处置修复（engine-patch 单文件三改动点）+ host 验收件——零 wire 形状变更、零契约面、零 docs 面、零 firmware 面、零新增依赖、零新增模块。
- **Split recommendation**：不拆——单主题（stop 收尾幻听治理）单文件原子变，三改动点互相咬合（截断态被守卫消费、幻听串表被守卫调用）。

## Verification Evidence

- **红基线 → 绿（acceptance-author 冻结落账 2026-10-14，改前 HEAD 实跑）**：`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/test_engine_stop_truncation_asr_filter.acceptance.py server/acceptance/test_kidpal_speak_sync.acceptance.py` → 退出码 1，7 failed / 3 passed / 0 error（3 passed = canary `test_kidpal_speak_sync` 全绿，connection.py 邻改 canary 面零回归）；逐齿红因 = 改动点 1-3 未落地（设计态红），非环境错误。双向可满足性实证：按 plan 改动点逐字落地的模拟形态 7/7 全绿；删 manual 豁免条件的变异形态恰 ET-b 独红（承重齿非空过）。
- **修后（integrator 独立复跑 2026-10-14，与 acceptance-author 同命令）**：**10 passed, 24 subtests passed**（新验收件 ET-a..g 七 case 全绿 + canary 三 case 全绿）。
- **全 acceptance 目录基线核验（integrator 独立复跑 2026-10-14）**：`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/` → **174 passed, 4 deselected, 33 subtests passed**，零新增红（全目录口径仅为证据申报；范围圈定决策仍按 `test-scope-final.txt` 相关子集口径，全量回归 = 人工触发位）。
- **scoped 收口证据**：`verifier-evidence/rounds/r3.json`——phase=scoped、round=3、invocationId=`31950d24-d2f2-47c0-bb05-d0eb481cc9d0`、ok=true、observedGateExit=0，八闸全 PASS：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build（真编译，xiaozhi.bin 0x3121c0 bytes、app 分区余量 11%）/ contract-lint / acceptance-coverage（functional = ET-a..g；files = 新验收件）/ mutation-check（skip 申报 PASS）/ docs-sync（docsSyncRequired=false 理由申报 PASS）/ security（securityReviewRequired=false）。verify fact 程序 fs-write，integrator 只消费不重跑。
- **final bearing 轮（r5）= planner 待跑**：持本包圈定清单 `test-scope-final.txt` 跑 `node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --change-id 261014-engine-stop-truncation-asr-filter --round 5 --invocation-id <uuid> --scope-file <清单>`，随后 `verifier-evidence-check.mjs` 校验；final fact ok:true 方可进入 closeout 判断（本 packet 的 r3 绿为 scoped 佐证态，非 final 终审态）。
- **真机轮强度申报**：本包含 live-manual 真机轮（plan 真机节五项：查字轮主验两轮 / 正常对话回归 / 故事录音回归 manual 豁免判定面 / 提醒轮回归 / 引擎日志双 marker 核验），**由 human 手工部署 ECS 后执行，不在本 Pod 跑、不在本 packet 申报为已过**——强度口径 = host 离线验收件（AST 抽真实方法体）+ live-manual 真机轮双层承载，host 面全绿不构成真机目击。
- **范围圈定终验（Step 4，三分法：per-Pod 图索引可用 → 正常圈定，无降级）**：**test-scope 清单落盘路径（一行锚）= `delivery/taskpackages/261014-engine-stop-truncation-asr-filter/test-scope-final.txt`**（integrator 落盘；caller 建议名 verifier-scope-final.json 未采用——verify.mjs 侧 `readScopeList` 按行文本解析、不解析 JSON，格式以 gate 期望为准，命名循 261014-voice-round 包 `test-scope-final*.txt` 先例）。圈定 = 本包 acceptanceTestsFile ∪ blast radius 映射测试 ∪ 共享 seam 相关测试 ∪ 安全余量 → 清单两件 = `test_engine_stop_truncation_asr_filter.acceptance.py`（冻结验收件七齿）+ `test_kidpal_speak_sync.acceptance.py`（canary，connection.py 邻改零改动回归观察面）。**floor = 本包两冻结件**（单包无对端冻结件）。圈定方法与 blast radius 口径逐条留痕于清单注释头（detect_changes 实跑 + connection.py 图符号化核验 + 跨包 blast radius 剔除理由 + manual 豁免安全余量评估）。
- **影响面核对记账（Step 9b advisory）**：`detect_changes` 实跑（Pod 专属索引 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`，base=main，merge_base=0baab93，inbound，含工作树未 commit diff）→ 89 seed / 90 impacted。**口径说明**：该 blast radius 为整分支 vs main 并集，混入同分支已收口包 261014-voice-round-entry-exit-fixes 的改动面（front_app.py / application.cc / reminders 等映射，其终验已由该包 r5 final bearing 轮独立承载）；**本包 diff 面（connection.py 六 hunk + 新验收件）的图符号化核验 = connection.py 不入图符号集**（search_graph 对 `_route_message` / `ConnectionHandler` / `_kidpal_on_listen_message` 零命中——engine-patch 为 deploy 面），故图谱对它无符号级 caller 可走，可执行测试缝 = AST 抽取真实方法体的两件 acceptance（即清单两件）。per-Pod 索引可用、非降级、无 re-index 需求。
- **docs-sync 核验（Step 2）**：`git status --porcelain -- docs/` 空、`git diff --stat -- docs/requirements.md docs/architecture.md docs/contracts/` 空——`docsSyncRequired=false` 申报属实；spec 面漂移检出 = 无，无 architect spec 更新交接。
- **架构/依赖卫生检查摘要**：零新增依赖（frozenset / queue.Empty / json 均为 Python 标准库既有 import 面；connection.py 未新增第三方 import）；零循环依赖；零跨层调用（守卫收口在 patch 文件内，不触 front / firmware）；零绕过 contracts 的本地类型；无重复实现（截断态自有标志 `_kidpal_listen_closed` 与既有 `client_abort` 职责分离，注释留痕）；零镜像内上游文件触碰。

## 未覆盖面申报（对照本包 `acceptance[]`（ET-a..g）+ 引用契约条目 + `plan.md` 效果段，逐条「性质 + 现状 + 去向」）

- **ET-a（stop 截断 auto 模式）/ ET-b（manual 豁免承重齿）/ ET-c（start 复位）/ ET-d（非 listen/畸形零扰动）**：**已覆盖**。性质 = host 行为/结构齿（AST 抽真实方法体 + SimpleNamespace 替身，循 speak_sync harness 先例）。现状 = 四 case 全绿（integrator 复跑 10 passed 在证）；ET-b 双向可满足性实证在案（删豁免条件的变异形态恰 ET-b 独红）。去向 = n/a。
- **ET-e（幻听串表整串匹配）/ ET-f（chat 守卫结构齿）/ ET-g（_route_message 拦截结构齿）**：**已覆盖**。性质 = 行为齿（命中/负锚两向）+ 结构齿（守卫位置序）。现状 = 三 case 全绿；ET-e 负锚两例（「谢谢你，这个字怎么写」「让我们一起说thank you吧」）钉住整串保护。去向 = n/a。
- **真机行为面（live-manual 五项）**：**本次未覆盖——按 plan 执行边界延后（human 手工部署 ECS 后执行）**。性质 = live-manual 真机复验，判定标准非改动面。现状 = host 面全绿不构成真机目击；本 Pod 终点 = host 面转绿 + 圈定终验 + commit 本分支，不部署 ECS、不挂起。去向 = 主线程手工部署 ECS 后真机复验轮执行，判定标准权威 = plan 真机节五项（① 查字轮主验两轮：stop 后无「识别文本:」/无「大模型收到用户消息」/无「发送第一段语音」、设备无「不客气」追加、查字窗驻留不受扰；② 正常对话回归：暖通道多轮问答 + 连续追问 + 唤醒词入口；③ 故事录音回归（manual 豁免判定面，必测）：录音→讲述→KEY 收尾→归档正常；④ 提醒轮回归：提醒 speak 照常出音；⑤ 引擎日志核验：双 marker + 设备重连）。
- **stop 前 VAD 在途轮竞态窗（~1s）**：**接受并留痕（plan 回归风险 3）**。性质 = 设计接受的语义残余——用户恰在收尾瞬间说话则该轮不回，与设备侧 261014-r2 F2「stop 不可撤回」裁决同语义。现状 = 改动点 2 守卫兜在途轮（置位后拦），竞态窗内已触发的轮一并丢弃。去向 = 登记不修（plan 回归风险节在案）；真机第 ② 项正常对话回归为误伤面观察位。
- **真实裸「Thank you.」被滤**：**接受并留痕（plan 回归风险 4）**。性质 = 过滤器设计代价——真实用户说裸 "Thank you." 失一轮回复（info 可观测）。现状 = 整串相等匹配已把误伤面压到最小（带补充内容的合法话不命中）。去向 = 登记不修（中文优先儿童产品，plan 论证在案）。
- **本包引用契约条目（`docs/contracts/interface-contract.md`）**：**零变更、无验收断言增补义务**。性质 = 仅作 `contractRefs` canary 引用——wire 协议零变更（listen start/stop 既有消息，front 旁路消费转发不变），契约条目无 diff，无「契约条目未覆盖」缺口（contract-lint 闸 PASS 佐证）。去向 = n/a。
- **plan「非目标 / 已知残余（不扩本包）」四项**：**有意不覆盖**。性质 = 登记不修。① stop 前在途轮 stt 下行已发出（聊天面板可能闪一条识别文本，cosmetic）；② manual 模式尾音幻听串经 stt 下行进故事累积器（未观测到）；③ 最短音频/能量判据（裁掉，plan 论证）；④ 引擎 stop 语义上游回馈 xiaozhi-esp32-server。去向 = 各自登记，不扩本包。

**零缺口申报：none**——对照本包 `acceptance[]` 全 7 条 + 引用契约条目，无「漏跑」型缺口；上列未覆盖项全部为设计性延后（真机轮 human 手工部署后执行）或设计接受/登记不修，逐条均标性质 + 现状 + 去向，无静默跳过。

## 强度与未覆盖申报速览（caller 派单指名口径）

- **强度申报**：真机五项复验为 **live-manual 轮**、human 手工部署 ECS 后执行，本 Pod 不跑。
- **未覆盖面逐条一行**：① 真机行为面（stop 后无「不客气」/查字窗驻留/正常对话回归/故事录音 manual 归档/提醒轮）→ 归 live 轮；② stop 前 VAD 在途轮竞态窗 ~1s → plan 回归风险 3 接受留痕；③ 真实裸「Thank you.」被滤 → plan 回归风险 4 接受留痕；**其余 `none`**。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| server/deploy/engine-patch/connection.py / server/acceptance/test_engine_stop_truncation_asr_filter.acceptance.py | 本轮改动面 `grep minimal:` **零命中**——零新增简化债务；改动文件内既有标记（如有）原样在世、不属本批 | n/a | n/a |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| FYI | judgement-call | manual 豁免依赖上游形状（riskTier medium 由来） | 「先清后转跳过」依赖 handleTextMessage stop 分支的 `len(conn.asr_audio)` 门存在——该门在引擎镜像内上游文件，本包不在册改动、0.9.6 参考树已核验；若将来引擎镜像升级改掉该门，截断的「整体不发生」效果退化为「识别空音频」（防御深度仍在：幻听串表 + 截断态守卫双层兜底，但 stop 触发识别本身会跑一次） | changed-code hunk（`_kidpal_on_listen_message` docstring 承重顺序注记）+ taskpackage notes「部署时容器内再核」条款 + heuristic（跨文件形状依赖） | 无（plan 部署节已立部署前置机械核对：docker exec grep 确认 `len(conn.asr_audio)` 门存在——归手工部署轮执行，本轮如实登记） |
| NIT | judgement-call | 幻听串表为种子集非穷举 | `_KIDPAL_ASR_HALLUCINATIONS` 五条为真机逐字命中 + 已知幻听模式种子，非 ASR 幻听全空间穷举；新幻听串需手工增补 | changed-code hunk（幻听串表注释来源注记）+ plan 改动点 3「种子，builder 定稿时逐条带来源注记」+ heuristic（开放集治理） | 无（整串匹配语义使误伤面有界；新命中随真机复验轮归 CRQ 条目追加） |
| NIT | judgement-call | ET 结构齿为源文本代理 | ET-f/ET-g 以 AST 源文本位置序钉守卫/拦截接线（结构齿，非端到端行为 oracle）——host 侧 connection.py 不可离线 import（引擎依赖 websockets/opuslib_next/core.*），行为断言经 SimpleNamespace 替身 + 方法体抽取承载，真链路 oracle 归真机轮 | changed-code hunk（验收件 harness 形态）+ speak_sync harness 先例 + plan 验收要点 | 无（与在册 harness 先例同口径；行为 oracle 归真机复验五项，已申报） |

结构闸 PASS/FAIL 不复述为 finding（gate 全绿仍完整执行五轴语义 review，见下）。本轮 findings 面清洁：无任何升级级 finding（明写：BLOCKING = 0）；finding 全为 judgement-call，Basis 为 changed-code hunk + heuristic。

## Five-Axis Review

- **Correctness（是否满足声明口径，边界是否缺失）**：三改动点逐条兑现 plan 修复节——改动点 1 的先清后转顺序、manual 豁免、start 复位、畸形静默返回四边界齐（ET-a..d 钉住）；改动点 2 的 depth==0 限定 + 不置 client_abort + 入队前位置（ET-f 钉住）；改动点 3 的整串匹配 + 负锚两例（ET-e 钉住）。边界：非 listen / 畸形 JSON 零扰动零异常、递归轮不拦、story/合法 speak 下发不受扰——均有对应齿或注释留痕。
- **Test quality（是否断言真实行为，含错误路径）**：验收件经 AST 抽取驱动 `connection.py` 真实方法体（真控制流），SimpleNamespace 替身覆盖 asr_audio/asr_audio_queue/logger；ET-d 断言零扰动零异常（错误路径）、ET-b 断言「不动」（承重齿非空过，双向可满足性实证在案）、ET-e 含两负锚防子串误杀。非「没报错就算过」——红基线 7 failed → 修后 10 passed 的双向可满足性 + 删豁免变异独红实证敏化。
- **Contract/boundary safety（是否漂移 contracts / spec）**：`docs/contracts/**` 零 diff（contract-lint PASS）、`docs/requirements.md` / `docs/architecture.md` 零 diff（docs-sync PASS，integrator 独立核验属实）——零 spec/contract 漂移；wire 协议零变更；manual 模式行为零改动（SAO-A2 归档通道保护，ET-b + 真机第 ③ 项双重钉）。
- **Operability/security（ErrorEnvelope / correlationId / 权限 / 输入 / secret / 日志）**：零新增攻击面——纯引擎侧内部处置，零新增出网路径、零 secret、零权限变化。输入处理面：新增 JSON 解析有 TypeError/ValueError 捕获（畸形静默返回，ET-d 钉住）；幻听过滤输入归一化对非 str 入参返回 False（isinstance 门）。日志面：两条 info（截断生效 / 幻听丢弃）可观测 + 启动 marker（部署生效锚）；零 ERROR 新增。`securityReviewRequired=false`（security 闸 PASS）。
- **Maintainability/simplicity（是否局部、易懂、无过早抽象/重复）**：改动收敛单文件三块 + 单新验收件，零过早抽象、零重复实现、零新增依赖；幻听串表为模块级 frozenset 常量（数据与逻辑分离）、截断态单布尔标志职责单一（与 client_abort 分离留痕）；`minimal:` 零标记。

## Residual Risk

- **plan 回归风险五项承接**：① 正常对话轮被误杀——结构性不可达（正常轮走服务端 VAD、不经 stop 路径；chat 守卫只在 flag 置位后拦），ET-a/c 钉住，真机第 ② 项为行为 oracle；② manual 豁免漏做/漂移（毁稿级）——ET-b 承重齿 + 真机第 ③ 项双重钉；③ stop 前在途真实语音被丢——接受留痕（见未覆盖面申报）；④ 真实裸 "Thank you." 被滤——接受留痕；⑤ 部署时点漂移——手工部署时以 md5 对账兜（既有通道），commit-only 状态下容器重建会回退镜像原版。
- **taskpackage decisionCoverage 筛选**：本包 taskpackage **未启用 decisionCoverage 字段**——`landsIn: dropped && reason` 以 `risk-accepted` 开头的条目 = **零**，本节按模板申报零条目（plan 回归风险 3/4 的接受裁决以 plan.md 回归风险节 + 本 packet 未覆盖面申报双处留痕承载）。
- **Known limitation**：① 真机五项复验未闭环（live-manual，human 手工部署 ECS 后执行）——host 面全绿不构成真机目击；② CRQ `TECH-261014-engine-asr-hallucination-after-stop` 状态未翻转（闭账条件 = 包 merge + 真机复验轮，均不在本 Pod 范围）。
- **Follow-up recommended**：主线程手工部署 ECS（复用 261014 误 await 修实证通道：备份 `.bak-261014-pre2` / ecs_put 上传 / md5 对账 / docker restart / 双 marker 日志核验）→ 真机五项复验 → CRQ 闭账；远端正规部署另行（manifest 已含 connection.py，届时零增量，顺带闭 TECH-261012-edgetts-403-hotpatch 耐久化）。
- **Human decision needed**：手工部署 ECS 时点 + 真机复验轮组织（human 持板，串口捕获规程）。**all-mock 申报**：本包 requiredLayers 含 live-manual（真机轮），不应判 all-mock；本轮真机证据不落（plan 执行边界 human 已裁）——若 supervisor 机械闸判 all-mock，以 plan.md 执行边界（「只 commit 到分支；手工部署 ECS 测试」）为既有 human 裁定留痕、不静默（261012/261013 先例口径）。

## 执行边界留痕（plan 执行边界 + caller 派单，随包在案）

1. **只 commit 到本分支、不部署 ECS**——已遵守（手工部署 = 主线程/human 通道，含部署前置机械核对与 md5 对账）。
2. **不 push、不合并 main、不动 tag**——已遵守（本 packet 与 commit 均在本分支 `dev/260915-m1-k1a-key-layer-dispatch`，`git rev-parse --abbrev-ref HEAD` 已核）。
3. **docs/contracts/** 零触碰、spec 面零自改**——已遵守（`git status --porcelain -- docs/` 空）。
4. **不改实现代码、不改验收件**——已遵守（integrator 只消费 r3 fact + 独立复跑测试，未 Edit 任何 `*.py` 实现面/验收面）。
5. **真机轮不在本 Pod 范围**——已遵守（判定标准随 plan 冻结，未因真机证据缺失挂起）。
6. **final bearing 轮（r5）归 planner**——本 integrator 只落盘圈定清单（`test-scope-final.txt`），verify.mjs final 轮与 `verifier-evidence-check.mjs` 校验由 planner 持清单执行，fact 程序 fs-write。

## 集成执行回报

本轮无 merge-driver 冲突消解（单包制品轮，Pod 分支无对端冲突，全部改动为工作树未 commit 态一次收口），集成复跑清单 n/a；圈定 floor 注（双方冻结件 ∪ 圈定清单）不适用——**单包无对端冻结件，floor = 本包两冻结验收件 `test_engine_stop_truncation_asr_filter.acceptance.py` + `test_kidpal_speak_sync.acceptance.py`**。复跑面 = 影响面圈定清单两件（integrator 独立复跑 10 passed + 24 subtests；全 acceptance 目录 174 passed 仅为证据申报）。detect_changes 记账见 Verification Evidence（89 seed / 90 impacted 为整分支口径；本包 diff 面经图符号化核验收窄为两件 acceptance 缝，per-Pod 索引可用、无降级）。**全量回归 = 人工触发位，不在本次**（final 相位按 `test-scope-final.txt` 圈定跑，非全量）。
