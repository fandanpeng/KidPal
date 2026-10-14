# Review Packet · 260829-char-stroke（查生字 P4 + C1 云侧静态字库）

> integrator closeout · 2026-09-07 · 分支 `dev/260829-char-stroke`（worktree，commit 冻结、不 push 不 merge）
> verdict 来源：final verifier-round fact `verifier-evidence/rounds/r2.json`（ok:true，round 2，invocationId `6f3226ea-74a0-4c5e-88c3-9c48e3d16b62`）；`current/verdict.json` 只是 non-bearing 兼容投影。

## 1. 实现面清单

### 设备侧（firmware，查生字 P4 页）

- `firmware/main/display/char_stroke.h/.c`：9 函数契约——D3 笔顺 JSON 消费（M/Q/L/Z 二次贝塞尔离散）、1024×1024→400×300 坐标变换（Y 轴朝下翻转 + 字形偏上偏移）、1bpp scanline even-odd 光栅化、逐笔动画状态机（950ms 步进、歇 2s 重来）、速查卡数据装配。纯逻辑、无芯片依赖，host_app linux-target 同源。
- `firmware/main/display/char_page.h/.cc`：P4 查字页（米字格 + 三态逐笔动画 + 右栏速查卡：拼音带声调/共 N 画/进度珠/第 X 笔·共 N 笔/部首/组词；KEY 短按重播）。教学字体红线：示范字光栅生成（标准规范字形、零 tilt），不依赖字体文件。
- `firmware/main/mcp_server.cc`：additive 补 `case 3`（VRT_ROUTE_CHAR）——先过 `kidpal_char_data_validate` 缺字段防御降级门（六必需键不全→降级提示、不盲渲染不崩），通过才 `app.Schedule` 切 P4 页。只消费不改 view_router 数值码。
- 接线：`firmware/main/CMakeLists.txt`（+char_stroke.c/char_page.cc）、`firmware/main/test/host_app/main/CMakeLists.txt`（+冻结验收 +被测纯逻辑，include dir 加 `../../../display` 供 `__has_include("char_stroke.h")` 契约头探针）。

### 云侧（server，C1 机制 B = OpenClaw dist 补丁）

- `server/deploy/openclaw-patch/char_stroke_lib.py`：冻结拼装契约——提取最小 char 块 → makemeahanzi 静态字库查表 → 程序化拼全量 D3 JSON；目标字表预热 + LRU 缓存。
- `server/deploy/openclaw-patch/make_openclaw_patch.py`：dist 补丁生成器（TS 镜像语义，锚点 `resolveAgentResponseCommentary` / `streamedAssistantText`，流式 + 非流式双出口覆盖）。
- `server/deploy/openclaw-patch/apply-openclaw-patch.sh` / `warm_char_cache.py` / `DEPLOY-RUNBOOK.md`：ECS 部署载体（版本漂移闸 / 备份 / 软链 / 预热 / 回滚）。
- `server/deploy/openclaw-system-prompt.v2.md`：v2 prompt——LLM 只判意图＋提取目标单字、禁自生成/复述 strokes/medians 坐标、display 块 data 原样用查字工具完整结果；顺带收紧三句纪律（包 2 A4 留根）。v1 共存可 diff。
- 接线：`server/deploy/apply-xiaozhi-overlay.sh` 与 `provider.config.yaml` 切 v2（「验收所测 = 生产所用」纪律保持）。

### 验收/证据面

- `firmware/main/test/char_stroke.acceptance.c`：10 齿冻结验收（host ELF 实跑）。
- `server/acceptance/test_char_stroke.acceptance.py`：offline 8 齿 + live 3 齿（`@pytest.mark.live`，conftest 机械 skip 闸，RUN_LIVE=1 显式触发）。
- `firmware/main/test/acceptance/red-baseline/red-baseline-summary.char-stroke.md`：红基线实测记录（副本归位 `changes/dev/260829-char-stroke/red-baseline-summary.md`）。
- taskpackage 勘误两处已落：`acceptanceTestsFile` 补 python 路径、`mutationEvidence` 新增 8 功能 id。

## 2. 验收证据

- **红基线**：CHR 侧冻结验收先红后绿（`red-baseline-summary.md` 全记录：C 侧 WSL host ELF + python 侧 pytest，红基线命令与实跑输出在案）。
- **机械闸**（verify.mjs bearing 模式，r1 scoped + r2 final 均 ok:true；`verifier-evidence-check.mjs` PASS，factOk:true）：taskpackage-required / firmware-build（degraded，见 §7）/ contract-lint / acceptance-coverage（CHR-A1~A9 全覆盖，files 两文件）/ mutation-check / docs-sync / security / unit / acceptance 全绿（r2 全量 9 闸，r1 历史 mutation-check 红一次→勘误后绿，见 verify-runs.jsonl）。
- **固件验收**：`char_stroke.acceptance.c` 10 齿全 PASS，host ELF（WSL linux-target）实跑。
- **服务端验收**：`test_char_stroke.acceptance.py` offline 8 齿全绿；live 3 齿（ssh reachable / A8 live 拼装块 vs makemeahanzi 逐字对照 / A9 live 端到端延迟 5 秒档）RUN_LIVE=1 全绿，实测 14.95s。

## 3. 变异证据（mutationEvidence，8 功能 id 全 turnedRed + sensitive）

| 齿 | 变异 | 结果 |
|---|---|---|
| CHR-A2 | cs_bezier_t 二次贝塞尔 y 项系数 2.0f→1.0f | 红（还原 sha256 一致） |
| CHR-A3 | kidpal_char_transform Y 翻转移除 | 红 |
| CHR-A4 | cs_raster_row even-odd 配对步进 k+=2→k++ | 红（raster no-op 变异双敏感） |
| CHR-A5 | CS_STROKE_MS 950→940 | 红 |
| CHR-A6 | 速查卡进度措辞间隔符移除 | 红 |
| CHR-A7 | char_page.cc 光栅路径注入 tilt | 红（齿 1 声明稀释弱点见 §8） |
| CHR-A8 | assemble_char strokeCount 回显 +1 | 红 |
| CHR-A9 | CharLruCache.put 淘汰分支 pass 化 | 红 |

首探未红的两处（A2 x 项系数、A8 pinyin join）均属测试几何/数据不敏感，非齿缺陷，改打有效变异即红，诚实注记在 taskpackage。

## 4. Live 证据（含延迟收敛路径）

- live 3 齿 RUN_LIVE=1 全绿（对真 ECS/OpenClaw 2026.8.1）：SSH 可达、A8 拼装块与 makemeahanzi 数据源逐字一致、A9 端到端延迟落 5 秒档（套件实测 14.95s）。
- **延迟收敛路径**：v1 时代 109s（LLM 逐 token 复述坐标 JSON）→ v2 prompt 令 LLM 只出最小块（意图＋单字）→ OpenClaw dist 补丁在回复出口（流式 :streamedAssistantText / 非流式 :resolveAgentResponseCommentary）正则提取最小块 → 查 ECS 静态字库（预热 + LRU）→ 程序化替换为全量 D3 块。延迟预算依赖 v2 措辞有效性，见 §8 风险 2。

## 5. 部署实况（已按 DEPLOY-RUNBOOK 落地 ECS）

- OpenClaw 2026.8.1 dist 补丁已 apply（三绑定 + 信封，流式/非流式双出口），锚点 grep 版本漂移闸通过。
- `char_cache.json` 全量 9574 字已预热；数据集 `/root/.openclaw/workspace/kidpal/chardata` 经软链对齐 `/opt/kidpal/chardata`（graphics/dictionary 各 9574 行校验过）。
- openclaw-gateway 已重启，live 探针绿。
- 回滚路径：DEPLOY-RUNBOOK §2 备份件还原 + 补丁卸载（runbook 内置）。

## 6. 架构 / 依赖卫生 + docs-sync

- 闸 A 契约预审：④ 无需改契约（NO_CONTRACT_CHANGE）——`docs/contracts/**` 零改动；最小块终止于 OpenClaw 回合内不越网关边界；§1.5 薄桥红线被 C1 强化的校验（`test_e3_gateway_thin_bridge_untouched` + `test_gate_a_display_schema_untouched` 冻结齿在案）。
- 无新增外部依赖；无跨层调用（云侧拼装挂 OpenClaw 侧不入 shim，守 human 2026-09-01 位置裁定）；无循环依赖。
- docs-sync 绿：docRefs（PROPOSAL §2 包 4 / spec §3.0.2 D3 / §4.3 S-F3.x / 契约 §1.6）与 shipped behavior 一致，spec/契约无需更新（闸 A 判 ④），无 docs-delta 需求。
- ADR：无强制（adrRequired 未置位）；机制 B 裁决已在 `intake-notes.md` / `gate-a-prereview.md` / `openclaw-extension-survey.md` 完整追溯，随本包制品晋升归档。
- S2 official exploratory scan：TaskPackage 未声明（s2ExploratoryRequired 未置位），不执行。
- detect_changes：N/A (worktree)——本包在 worktree 分支开发，索引不覆盖 worktree diff；以 git diff --stat（10 modified + 12 untracked，全部落 §1 实现面 + taskpackage/current 投影 + .codebase-memory 派生快照）替代核对，无越界文件。

## 7. 机械闸预检（integrator closeout 实跑）

- `docs-sync-check.mjs`：PASS（exit 0）。
- `closeout-check.mjs`：首跑红（changes/dev/260829-char-stroke 缺失）→ 制品晋升（含 docs-delta.md）后 `--update-index` 复跑 PASS，indexProjection=inserted。advisory-only 四条如实记录：① changes/dev 下缺 verdict.json/verification.json（脚本产线件，advisory-only；照 260907-g-relay-fix / 260829-voice-roundtrip 先例不手造，`current/verdict.json` 投影已在仓，supervisor-verdict.json 由 planner 的 supervisor-verdict-gate 产出）；② worktree 内 `.claude/taskpackages/<id>/pod-events.jsonl` 不存在（live 文件在主 checkout，快照已随本目录晋升）；③ pod-retro.md 缺（planner 收尾相位补，零追溯 post-cutoff 条款）；④ loop-retro 触发阈值到（发起责任=主线程，非本包门）。
- `firmware-build-check.mjs`：PASS 但 degraded:true——本机 idf.py 不可用，固件编译未在本机核验（降级放行，非假绿）；设备编译已由 fixer 实测 EXIT=0（verifier 转绿前提）。
- `verifier-evidence-check.mjs --phase final --round 2`：PASS，factOk:true。

## 8. 遗留风险（如实登记）

1. **A7 齿 1 声明稀释弱点**：结构齿「≥2 文件引用 `kidpal_char_raster_stroke(`」因 .h 函数声明也计入引用数（定义+声明=2 恒绿），对「调用者改名」不敏感；已如实记录，收紧建议（排除 .h 声明或另用调用点标记）留 acceptance-author tighten-only 阶段。
2. **live 延迟依赖 v2 措辞**：延迟预算建立在「LLM 只出最小块」的 prompt 约束上；若 OpenClaw 升级改变 agent loop 行为（如回复出口锚点漂移、LLM 复述回潮），延迟需复测——DEPLOY-RUNBOOK 版本漂移闸（锚点 grep）是第一道防线，对不上即停。
3. **char_cache.json 全量 9574 字内存占用（~19MB）**：网关已有 memory pressure warning 一次；后续可评估按需加载或分片，当前 MVP 不动。
4. **Intent:nointent（tools 拒收）**：与本包机制无关（最小块不依赖 LLM tool calling，网关请求不带 tools 数组），仍是 ARQ-260907-openclaw-tools-rejection-rootcause 挂账项。
5. firmware-build 闸本机降级放行：设备编译依赖 fixer 实测记录（EXIT=0），无本机可复现闸兜底。

## 9. Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| （无 BLOCKING / SHOULD） | — | — | — | — | — |

NIT：`.claude/taskpackages/260829-char-stroke/taskpackage.json` 文件末尾缺换行符（勘误追加时引入；机械闸不受影响，live 文件受 enforce-scope 强制层保护、非 integrator 写边界，留 planner 顺手修）。judgement-call：diff hunk（taskpackage.json 末行）+ 风格一致性。changes/dev 快照副本已补换行，不阻塞。

结构闸（closeout-check / docs-sync / firmware-build / verifier-evidence）结果见 §7，不复述为 finding；gate 全绿之外仍完成上述五轴语义 review（实现面逐文件 diff 核对、契约边界、可运维性回滚路径、遗留风险五项、简化债务）。

## 10. Simplification debt（`minimal:` 标记台账）

- 全量 9574 字预热换启动内存（§8.3）：`no-trigger`（无升级触发，MVP 取舍，已在 taskpackage decisionCoverage 落 acceptance:CHR-A8）。
- 三句纪律收紧随本包 prompt 面顺带做而非单开变更：`no-trigger`（包 2 A4 留根的既定归并）。

## 11. Residual Risk（decisionCoverage dropped + risk-accepted）

`taskpackage.json.decisionCoverage` 共 5 条，全部 `landsIn` 落 acceptance/exitCriteria，**`landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目为 0 条**——无 human risk-accepted 残留需列。

## 12. 制品与 commit

- 晋升制品（`changes/dev/260829-char-stroke/`）：taskpackage.json（含勘误的权威快照）、intake-notes.md、gate-a-prereview.md、openclaw-extension-survey.md、red-baseline-summary.md、pod-events.jsonl（主 checkout live 文件对齐快照）、verify-runs.jsonl、verifier-evidence/rounds/r1.json + r2.json、本 packet。
- post-cutoff 口径：不产 session-digest.md / loop-health.md / loop-metrics.json / pod-journal.jsonl；pod-events.jsonl 已存在（advisory，非 blocking）。
- commit：feature 分支冻结 diff，不 push、不 merge；supervisor 合并门由 planner 跑 `supervisor-verdict-gate.mjs` 机械闸，本包不自行判定通过。
