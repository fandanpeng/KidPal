# Review Packet · 260920-m2-charwindow-display-fix

> workstream m2-char-window ｜ M2 查字窗真机轮修复批次（中段 change，落法 A）｜ mergeOrder 32 ｜ rigor lite ｜ riskTier medium ｜ subsystem firmware+server（跨两端）｜ sourceType human-confirmed-plan（/triage-change 2026-09-20 裁决 ② Lite 合一包 + crossEnd 双向申报 + human 触点①四问确认）
> integrator 收拢制品（2026-09-20）；合并门判读归 planner 主线程 supervisor-verdict-gate 机械闸，本包不 push、不 merge、不在 main 上 commit。**前置依赖 `260920-m2-charwindow-card-fix`（mo31）已 merged（`a259601`）。**

## Change Size

- **实现（builder 未提交改动 → 本次收口 commit，firmware 1 文件 +70/-10、server 5 文件 +63/-26、新文件 1）**：
  - `firmware/main/display/char_page.cc` +70/-10——① 右栏去遮挡（MCD-A1）：cardlabel 起点 y8→30（清出恒显顶部状态带 top_bar_ y0-24 + 余量，**内容让位、不动 chrome**）+ `lv_obj_set_width(160)` 定宽 + `LV_LABEL_LONG_WRAP`（释义转中文后防右缘裁切，右缘 388−228=160 同刀钉死）；② 提示条让位（MCD-A2）：hint_label y280→244（避开 bottom_bar_ 键帽条带 y252-300）；③ 空行不显示（MCD-A3）：部首/组词行判空条件拼装（无空行、无占位符）；④ 组词 join（MCD-A4）：words 前 N 项以「、」join（非只取 [0]）+ **仅容量截断发生时**的 UTF-8 半字回退（planner 裁决②装配面义务——card_build 96B 字节截断冻结语义零触碰，超长串永不到它手上）。
  - `server/deploy/openclaw-patch/make_openclaw_patch.py` +27/-7——`k1cStripAppendix`（MCD-A7：只认 `<details` 真实开/闭锚、有开无闭透传不误伤、迭代移除处理嵌套闭锚）接进**流式 `k1cStreamStep` 与非流式 `k1cRewriteText` 两出口**；非流式改写改走 `k1cLookupBlock`（与流式同一 meaning merge 语义，消双路径漂移）；`k1cLookupBlock` 增 MCD-A5 meaning merge（最小块 meaning 覆盖 cache 值、缺键回落、浅拷贝不毒化跨请求共享字库缓存）。
  - `server/deploy/openclaw-patch/char_stroke_lib.py` +12/-2——python 镜像 `replace_minimal_block`（既有 C1 冻结契约函数）沿原签名扩展 meaning merge 语义 + docstring 冻结验收指针收编到现役件 `test_char_meaning_merge.acceptance.py`（原指已归档件）。
  - `server/deploy/openclaw-system-prompt.v5.md`（新文件，258 行）——v4 基座逐字保留可 diff + char 最小意图块 data 增 meaning 新约（中文释义 ≤24 字、与语音那句同源同字）+ 面向设备回复「只含口语正文 + 一个 display 块、禁任何自检/来源/附录/明细/HTML 结构」（内部自检走日志面）。
  - `OPENCLAW_SYSTEM_PROMPT_CARRIER`（声明面）+ `apply-xiaozhi-overlay.sh`（PROMPT_SRC/DST 渲染面）+ `provider.config.yaml`（artifact/source/deploy_target 三键）——v5 bump 链三面同步。
- **验收冻结（前序 commit `93bb20e`）**：板端 `char_window_overlay.acceptance.c` +303/-3（seam + TEST 10 去遮挡源锚/提示条让位 + TEST 11 空行不显示/join/数据面守卫）；云面新件 `server/acceptance/test_char_meaning_merge.acceptance.py` +491（10 齿：merge/strip 双路径 node 真跑 + python 镜像语义 + prompt v5 锚沿「解析两面得版本再断言」形态、避开版本字面耦合）。
- **Net diff（全生命周期，launch 基线 `d174a05` → worktree）**：firmware 2 文件（实现 1 + 冻结件 1）+ server 6 文件 + prompt v5 新文件；`docs/**`、`.claude/**` 零命中（`git diff --numstat d174a05 -- docs/ .claude/` 实测零行）。
- **Scope**：firmware 单文件 + server/deploy 部署工件面；`docs/contracts/**`、spec、agent/gate、`**/*.acceptance.*`（冻结件本包内零修订）、`char_stroke.c/.h` 纯逻辑合约（U 冻结面 + B8 守卫）**全部零触碰**。
- **Split recommendation**：不拆——板端让位与云面 meaning/strip 是同一条「播报什么显示什么」链路的显示面与数据面（MCD-A5 merge 不落、A1 让位后右栏仍是英文释义；A7 不剥、播报仍夹带附录），拆开任一半都拿不到完整 human 判据验证。

## Verification Evidence

- **九门全绿（三次独立轮次）**：scoped r1（verifier，`3e6f5cac-…`）→ final r2（verifier 终审，`1f5ed007-…`，unit/acceptance 两 full 闸入场）→ **final r3（integrator 独立复跑，`fdf34129-032d-4e25-a5a5-d9165e6d7755`，2026-09-20，`ok:true` / `observedGateExit:0`）**。evidenceRef `delivery/taskpackages/260920-m2-charwindow-display-fix/verifier-evidence/rounds/{r1,r2,r3}.json`。全生命周期三轮 verify **均一次通过、零返工轮**。
  - firmware-build：device build **真编译未降级**（xiaozhi.bin 0x30b9b0 bytes、app 分区余量 11%）。
  - unit：pytest 47 passed（= server/tests 37 零回归 + 本包云面新验收件 10 齿被 server 面收集；integrator 拆账实测 `--collect-only`：37 `tests/test_reminder_units.py` + 10 `acceptance/test_char_meaning_merge.acceptance.py`）。
  - acceptance：oracle-pass + executor-run 10 passed（云面新件全量真跑）。
  - mutation-check / acceptance-coverage：functional = MCD-A1/A2/A3/A4/A5/A7 全部 sensitive 在册；structural 豁免 = MCD-A6（真机腿，见下）。
- **机械校验**：`.claude/scripts/verifier-evidence-check.mjs` integrator 实跑两轮——r2（`--phase final --round 2 --invocation-id 1f5ed007-…`）与 r3（`--round 3 --invocation-id fdf34129-…`）均 **exit 0、`factOk:true`、`observedGateExit:0`**。final evidence 有效，可进 closeout。
- **板端 host runner 全量（integrator 独立复跑，非闸境）**：`cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- bash ./espidf_wsl.sh build`（ninja no work to do——elf 与当前 char_page.cc 同步）→ 直调 `./build/host_infra_test.elf` → **`63 Tests / 0 Failures / 0 Ignored`、OK、EXIT=0**（2026-09-20 integrator 实跑）。数字链自洽：card-fix 收口锚 61T/0F（mo31 merged）+ 本包新齿 2（TEST 10/11）= 63。红基线链（taskpackage `redBaselineResult`，acceptance-author 实跑 + planner 独立复跑双证）：63T/2F（唯二红 = TEST 10/11 设计态义务红）→ 实现落地后 63T/0F，其余既有齿零新增红。
- **Mutation check**：TaskPackage `mutationEvidence.perAcceptance` **六条 record（A1–A4 实现态 + A5/A7 freeze 相位影子树代表实现）全部 `sensitive:true` + `failed as expected`**，「单点变异 → 转红 → 还原复绿」链完整；A1–A4 md5 与绿态逐字节一致、零残留；A5 双路径去 merge / A7 去 strip 与**过宽误剥两方向**均咬合（正向不误剥齿独立敏感）。本 packet 引用 taskpackage 字段，不重跑。
- **Impact audit（worktree 降级形态）**：detect_changes **N/A (worktree)**（Pod worktree diff 对主索引不可见）——替代抽查：① `git diff --numstat d174a05` 闭集核对（见 Change Size，docs/.claude 零命中）；② 共享符号 `trace_path`（CodebaseMemory 图 KidPal 主索引）：`kidpal_char_page_show` 调用方闭集 = `Application::Initialize` / `kidpal_render_node` 三处 / `McpServer::AddCommonTools`，全在既有显示路由面内、零跨层新增消费；③ TS 注入函数（`k1cRewriteText`/`k1cStreamStep`）为 patch 生成器产出的注入串、不在图内，消费闭集 = `make_openclaw_patch.py` 两处 NEW 注入点 + DEPLOY-RUNBOOK 注入说明（grep 实证）；④ python 镜像 `replace_minimal_block` 消费方 = 云面新验收件（反射断言）+ 归档件（退役面，不入现役）。
- **Docs sync**：`docsSyncRequired=false`；零契约/spec/agent-gate 触碰经 `git diff` 逐面核对（详见同目录 `docs-delta.md`，含文档债三处登记）。
- **S2 exploratory**：TaskPackage 未声明 `s2ExploratoryRequired` → 无 S2 official scan（非遗漏）。
- **Loop metrics / Pod Journal（post-cutoff dual-format）**：不产 `session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl`；`pod-events.jsonl` 在场（advisory 检查通过，builder dispatch-completed + over-scan friction 已锚），缺失不阻塞 closeout。

## Demo 对账行（taskpackage `testHost` 证据形态核对）

taskpackage 权威口径（`testHost` 逐字）：**「host runner（板端）+ device build 闸 + server pytest（云面 host-fast）+ 真机/live 腿（human 持板）」**——真机/live 腿 = MCD-A6（human 持板，含 ECS 部署后真跑一次查字）。逐项对账：

| testHost 证据形态项 | 本包实际证据 | 对账 |
|---|---|---|
| 板端 host runner | `char_window_overlay.acceptance.c` TEST 10（A1 三源锚 + A2 提示条）/ TEST 11（A3 空行不显示 + A4 join + 数据面 96B/UTF-8 守卫）→ 全量 **63T/0F EXIT=0**（integrator 独立复跑）；A1–A4 四条 mutation 反证（`sensitive:true`、还原复绿、md5 逐字节零残留） | 一致 |
| 云面 host-fast pytest | `test_char_meaning_merge.acceptance.py` **10 passed**（A5 merge 双路径 + A7 strip 双路径 node 真跑 + 正向不误剥 + prompt v5 锚两面解析）；unit 闸 47 passed（37 零回归 + 本包 10） | 一致 |
| device build 闸 | r3 `firmware-build` exit 0（真编译未降级、xiaozhi.bin 0x30b9b0 / 余量 11%） | 一致 |
| 真机/live 腿（human 持板，MCD-A6） | **未做、deferred**——MCD-A6 为 structural 条目在册：① 真实遮挡消除（层序渲染只有上屏可证，源锚齿只是决策代理）；② 中文释义真机上屏可读、wrap 不溢出；③ 提示条新位置可见；④ 播报与屏显一致（human 判据最终裁判）；⑤ live 腿按「probe != exercise」真跑一次查字端到端 | **一致但未闭环**——口径本即 deferred（host 无真实层序渲染与设备字体现实，源锚齿只是决策代理） |

**一致性结论：口径一致、真机腿未做。** 前三项已按口径落证；**MCD-A6 未执行，本包 host 半闭环——明记此点，不得读成整包已验收**。**部署与烧板不在本包**（planner 按 human 指示 post-merge 执行，走 DEPLOY-RUNBOOK / kidpal-flash-device skill）——ECS 部署 + 烧板是本包落地链的下一站，MCD-A6 真机腿在其后由 human 持板执行并落账本目录。

## 本包特有事项（根因 / 防伪 / 遗留，合并门判读输入）

1. **遮挡根因链（planner 原指认已证伪纠正）**：右栏遮挡真凶 = **恒显顶部状态带** `top_bar_`（LV_OPA_50 半透，y0-24）+ `status_bar_`（`KidPalReparentChatToLayer` 把 `chat_top[]` reparent 到 `lv_layer_top`，任何状态恒显）——planner 原指认 voice_page 右内容区已被实证证伪（voice_page 默认 HIDDEN、显隐单点 `kidpal_chrome_mid_sync_locked`，勘误入账 `f8b4a4c`）。**修法取内容让位（不动 chrome）**：让状态带随窗隐藏会扩大前台维度显隐面、横跨提醒 K4 最高层与语音轮四态既有闭环——收益/风险不成比例，让位是单文件局部修。
2. **播报杂质根因（human finding③）**：OpenClaw 回复夹带 `<details>来源与自检</details>` 附录、网关 display-strip 只剥 ` ```display ` 围栏未剥该附录 → 整块进 TTS 被念出（14:45 轮日志逐字在案）。修法双层：**网关 strip 硬化**（`k1cStripAppendix` 流式+非流式两出口，只认真实开闭锚、有开无闭透传不误伤正常正文）+ **prompt v5 附录禁令**（面向设备回复只含口语正文 + 一个 display 块，自检走日志面）。
3. **human 判据（本包验收方向权威）**：「板子端就是一个展示的地方，云端给什么我们显示什么」+「播报什么显示什么（除笔名）」（pod-events `98c9591f` / `84beaf72`）。**落法 B（display 块通用文本通道）= 另立契约门立项**（human 同意，workstream progress 2026-09-20 节已登记，触发通道 /arch-change）——本轮**零新增字段、零预造通用容器**（MCD-E3 守住，diff 实证）。
4. **组词撤回**：human 裁播报内容够了 → 播报不读组词 → 显示面**空行不显示**（不补组词数据、不重建 char_cache，architect 备选「中文词典源重建 9574 字 cache」作废）——A3 条件拼装 + A4 join 只对已有数据形态负责。
5. **prompt v5 bump 链**：新文件 + carrier + overlay 脚本 + provider.config 三键同步；**bump 前已核 v5 槽位零占用**（ARQ-260908 版本化工件抢槽教训纪律）；两面一致守卫齿（版本无关解析形态）在场且绿。v4 及以下历史文件零触碰、可 diff。
6. **跨端双路径一致性（260907-g-relay-fix 同族风险的对症防范）**：meaning merge 与 strip 均钉**流式 + 非流式两出口**，齿两路径都打（A5/A7 齿 + freeze 相位影子树变异四方向）——「离线绿真机红」的最大结构性风险源已按同族教训封堵。
7. **文档债（如实登记，不扩面）**：`docs/architecture.md`「prompt 现状 = v4」（3 处）与 `server/deploy/README.md`「当前 = v3」、`DEPLOY-RUNBOOK.md` 引 v3，为既有漂移、本包 bump 后进一步滞后——spec 面与既有漂移非本包可改，机器守卫（版本无关齿）不受字面漂移影响，交后续批（明细见 docs-delta.md 文档债表）。
8. **既有噪音（非本包引入）**：`char_page.cc:537` 附近 `ch` 未用变量告警为基线既有，本包未触碰该行、零新增告警（device build -Werror 全绿实证）。
9. **里程碑 dogfood**：M2 已 `in_delivery`，本包为真机轮修复批次、中段 change——**不动里程碑状态**，`milestones.jsonl` / `delivery-status.mjs` 本轮零调用（无阶段跳变）。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | server/流式出口 | **`<details>` 跨 delta 分裂时流式 strip 漏剥**——`k1cStripAppendix` 在 `k1cStreamStep` 入口**逐 delta** 执行；若开锚 `<details…>` 与闭锚 `</details>` 落在不同 delta（LLM 流式切分常见），开锚侧「有开无闭」按设计透传、闭锚侧无开锚同样透传 → 附录仍进 TTS。A7 node 行为齿整段文本**单次调用**喂入（`test_char_meaning_merge.acceptance.py` driver `const st = k1cStreamStep({id:"s"}, T)`），未覆盖跨 delta 切分 | changed-code hunk（make_openclaw_patch.py `k1cStreamStep` 首行 strip 时机 + `k1cStripAppendix`「有开无闭透传」设计取舍——docstring 明示该取舍针对「正文提及」误伤，delta 粒度是其未列的盲区）+ 启发式：流式状态机对 ` ```display ` fence 已有跨 delta 缓冲先例、`<details>` 未沿用 | ① **MCD-A6 真机 live 腿人耳验证**：部署后真跑查字，若附录再现即回 triage（本条即升级依据）；② 后续批硬化：strip 改跨 delta 累积（有开无闭片段挂起，等闭锚或流结束再放行），并补跨 delta 分裂齿。不 BLOCKING 的理由：prompt v5 附录禁令是第一道防线（14:45 事故在 v4 无禁令期），非流式路径完整剥离，冻结齿按其声明 scope 全绿 |
| NIT | judgement-call | firmware/char_page.cc | words join 的 UTF-8 回退依赖「末尾连续 0x80-0xBF 即半字」启发式，理论上截断点恰落合法字符尾字节序列中段时的判定已由「只在 truncated 时回退」限定，但 4 字节 emoji 序列中 0x80-0xBF 连续字节 >1 的逐字节回退会多删至多 2 字节（视觉缺半个 emoji 的极端形态） | changed-code hunk（join 循环 truncated 分支 while 回退）+ 启发式：查字数据源（云端字库）当前为纯 CJK 文本、emoji 不在 words 语料域 | 不动；真机腿 A6 ④「播报与屏显一致」取证时顺带目视 words 行即可 |
| NIT | judgement-call | server/文档 | `DEPLOY-RUNBOOK.md` 部署操作示例仍写「渲染 v3 prompt」（:158）——部署操作者按 runbook 走会看到与实际渲染目标（v5）不一致的字面，可能引发误判 | changed-surroundings grep（runbook :141/:158 引 v3，本包未触该文件） | 并入 docs-delta.md 文档债表第 3 行，随后续部署批刷新（部署执行者操作时以 `apply-xiaozhi-overlay.sh` 实际 PROMPT_SRC 为准） |

**结构闸结果**：final r3 九闸全绿（结构闸 PASS/FAIL 不复述为 finding）；结构闸之外，author 五轴语义 review 段完整执行（见下）。

## Five-Axis Review（author 语义审查）

1. **正确性**：A1 y8→30 与状态带 y0-24 的几何推导在案（160 定宽 = 388−228 右缘闭合）；A3 条件拼装后最坏缓冲 ~354B < buf[384]（编译分析随注释在场，device build -Werror 全绿复证）；A5 merge 浅拷贝语义 python/TS 双镜像一致（缺键回落、空串不覆盖）；A7 剥离只认真实锚、正向不误剥齿独立在场。已知边界 = Findings 第 1 条（流式跨 delta）。
2. **边界**：UTF-8 半字只在容量截断时回退（不误删合法尾字）；96B 封顶在装配层完成、card_build 冻结语义零触碰（U 冻结面 + B8 守卫齿在场）；JSON parse 失败/缓存未命中双路径均原样放行（不亮半成品）。
3. **契约**：D3 meaning/words 字段**既有**，本轮零新增字段、零预造容器（MCD-E3）；display 块 view/data 信封不变式（契约 §1.3①）两路径维持；`docs/contracts/**` 全生命周期零触碰（diff 实证）。
4. **安全**：`securityReviewRequired=false`；meaning 为 LLM 受控串但下发链经 JSON.stringify / cJSON 解析 + 板端 96B 硬封顶（32 CJK 字），无注入放大面；strip 不引入正则 DoS（indexOf 线性）；prompt 附录禁令收窄回复面。无新增依赖。
5. **可运维性**：bump 链三面同步 + 两面守卫齿绿，部署后无需额外手工接线；runbook 字面漂移已登记（Findings 第 3 条）；部署与烧板 = 落地链下一站（post-merge，planner 按 human 指示执行）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （空） | changed diff 内无 `minimal:` 标记（`grep -rn "minimal:" firmware/main/display/char_page.cc server/deploy/openclaw-patch/{make_openclaw_patch.py,char_stroke_lib.py} server/deploy/openclaw-system-prompt.v5.md server/deploy/{apply-xiaozhi-overlay.sh,provider.config.yaml,OPENCLAW_SYSTEM_PROMPT_CARRIER}` 实测零命中，no-trigger） | — | — |

## Residual Risk

- **MCD-A6 真机/live 腿未做**（human 持板）——本包 host 半闭环，**不得读成整包已验收**。真机腿五项判据见 Demo 对账行；live 腿同时是 Findings 第 1 条（流式跨 delta strip）的人耳验证点。
- **部署后云面生效前置**：网关 strip/merge 与 prompt v5 在 ECS 重新 apply-overlay 前不生效——板端（烧板后）与云面（部署后）生效时点不同步，真机腿须在**两端都已落地**后执行，否则验证的是旧云面。
- `taskpackage.json.decisionCoverage` 为空数组 → **无 `landsIn: dropped` 且 `reason: risk-accepted` 条目**，无 human 风险接受账目需列示。
- 遮挡修法为内容让位（源锚代理 + 真机腿终审）；若真机仍见遮挡，后备方案（chrome 显隐联动）属跨闭环改动，须另走 triage。

## 合并门输入摘要

- 回滚路径：本包收口 commit 单点 revert 即回 `93bb20e` 态（验收冻结件独立在前序 commit，revert 不触冻结面）；prompt v5 bump 链 revert 后 v4 文件原样在场、carrier/三键指回 v4 即恢复。
- Merge packet：本 packet + docs-delta.md + verifier-evidence rounds r1–r3 + taskpackage（mutationEvidence 六条）。**supervisor-verdict.json 未在本目录**（机械闸由 planner 主线程执行后落盘，本 integrator 不判定通过）；closeout-check 全绿 + 本分支已 commit 是 planner 跑闸的前置。
- 集成执行回报（Merge-Chain §消解后复跑清单）：**N/A**——本轮 merge-driver 未发生冲突 abort，无 Pod 分支消解执行。
