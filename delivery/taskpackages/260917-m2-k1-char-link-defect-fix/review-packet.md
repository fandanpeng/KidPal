# Review Packet · 260917-m2-k1-char-link-defect-fix

> workstream m2-char-window ｜ M2-K1 ｜ mergeOrder 28 ｜ rigor full ｜ riskTier medium ｜ subsystem firmware
> integrator 收拢制品（2026-09-18）；合并门判读归 supervisor-verdict-gate 机械闸，本包不 push、不 merge。

## Change Size

- Files changed: firmware 4 文件修改（`char_stroke.c` +63/-3、`char_page.cc` +45/-12、`custom_lcd_display.cc` +10/-4、`test/host_app/main/CMakeLists.txt` +9/-0）+ 新增冻结验收件 `firmware/main/test/char_stroke.acceptance.c`（815 行）；bookkeeping 面 = taskpackage.json（mutationEvidence 回填）、current/ 投影两件、input-brief 两件、pod-events.jsonl / verify-runs.jsonl / verifier-evidence/（immutable 引用不复制）。
- Net diff: firmware 面 +127/-19；全 diff 含 bookkeeping 约 +260/-120。
- Scope: 板端 `firmware/` 单一子系统——① SVG 解析器补 C 命令（char_stroke.c 数据面）② char_page 动画帧去大栈数组 + taskLVGL 栈定容收编（char_page.cc / custom_lcd_display.cc 装配面）。云端 `server/` 零改动，`docs/` 零触碰。
- Split recommendation: 不拆——两缺陷同属「查字链路修复」一个 story 面，验收件单文件冻结、一次 RED→GREEN 链承载，拆分徒增集成闸轮数。

## Verification Evidence

- Structural gates: final r3 fact 九闸全绿（ok:true，invocationId `5aaa1e91-2c09-4a93-8ef5-bb5f8ee5d9a6`，2026-09-18T00:24:58Z–00:25:12Z）——taskpackage-required / firmware-build（device build 真编译，VERIFY_FW_BUILD_REQUIRED=1，xiaozhi.bin 0x30b780 bytes、分区余量 11%）/ contract-lint / acceptance-coverage / mutation-check / docs-sync / security / unit（pytest 37 passed）/ acceptance（oracle-pass，firmwareAcceptanceFiles 20）。`verifier-evidence-check.mjs --phase final --round 3` 实跑 PASS（factOk:true，observedGateExit 0）。
- Acceptance/unit/api/ui/e2e:
  - host runner 全量绿（builder 实跑）：39 Tests / 0 Failures——既有套件基线 35T/0F @ be3cf9f（260915 T3 判停引用，firmware/、server/ 两面对 be3cf9f 零差异）+ 新验收件 4 齿 = 39，账实相符。
  - 新齿 RED→GREEN 链：红基线 2026-09-18 acceptance-author standalone 实跑（未修复树）4 Tests / 3 Failures（runner exit=3）——A1 / A3 / boundary 接线红、A2 PASS（修复前行为冻结参照）；红名 verbatim 在 taskpackage `redBaselineResult` 字段在案。
  - M2K1-A4（live 端到端复证）deferred 挂账 M2-K3 设备轮（TaskPackage structural 条目 note 在案）；无 S2 scan（TaskPackage 未声明 s2ExploratoryRequired）。
  - 轮次链：r1 scoped 红（taskpackage-required 于拆包元数据形态，origin.type 值域问题，planner finalize 权内改 delivery-story 后闸绿，friction event `8c4ec567` 在案）→ r2 scoped 全绿 → r3 final 全绿（本包承重 fact）。verifier-evidence/ 目录 immutable，本包只引用不改写。
- Security gate: final r3 exit 0。
- Mutation check: taskpackage `mutationEvidence.perAcceptance` 三条 record（M2K1-A1 / A2 / A3）全部 `sensitive:true` + `failed as expected`——MUT-M2K1-A1-cmd-c-removed（tokenizer 退回 M/L/Q/Z → A1 红名复现）、MUT-M2K1-A2-bezier-segs-changed（CS_BEZIER_SEGS 8→10 → Q 离散 9 点参照逐位漂移红）、MUT-M2K1-A3-frame-path-stack-array-restored（帧路径恢复非 static [256] → 结构红复现）；各自还原后 standalone 4T/0F。红→绿链完整，本 packet 引用 taskpackage 字段不重跑。
- Docs sync: docsSyncRequired=false；零 docs 触碰经 git diff 核对（详见同目录 `docs-delta.md`）。

## Demo 对账行（workstream 包锚 3 · roadmap §③ K1 demo 证据形态核对）

roadmap 权威口径（`delivery/workstreams/m2-char-window/roadmap.md` §③ K1）：**「demo 证据形态：host runner（C 命令齿 RED→GREEN + 既有命令回归齿）+ device build 闸；真机半 deferred 归 K3 设备轮」**。逐项对账：

| roadmap 证据形态项 | 本包实际证据 | 对账 |
|---|---|---|
| host runner：C 命令齿 RED→GREEN | 红基线 A1 红（「最小 C 路径解析返回 -1——含 C 命令的 makemeahanzi 笔画被整笔丢弃」verbatim 在 redBaselineResult）→ 修复后 39T/0F 全量绿含 A1 GREEN；mutation MUT-M2K1-A1 红复现在案 | 一致 |
| host runner：既有命令回归齿 | A2（M/L/Q/Z 参照表逐位一致 + 防御面零回归）修复前后双态在案（红基线期 PASS 作冻结参照、修复后 GREEN）；MUT-M2K1-A2 密度敏感红复现 | 一致 |
| device build 闸 | final r3 firmware-build exit 0（真编译，custom_lcd_display.cc 板级面在编译内，E3 满足） | 一致 |
| 真机半 deferred 归 K3 设备轮 | M2K1-A4 structural deferred 条目 + note（单真机不两轮撞车，live 复证 + NFR-PERF-2 live 复测挂 K3 launchPreconditions 第 4 条）显式挂账 | 一致 |

**一致性结论：一致。** demo 证据形态四项与 roadmap 权威口径逐项吻合，无缺项、无超出声明的证据形态、无未声明的降级。数字链自洽：35（既有基线）+ 4（新齿）= 39 = 全量实跑；红基线 3 Failures 与 A1/A3/boundary 红因构成一致。**无不一致项，无需上报 planner。**

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （空） | changed diff 内无 `minimal:` 标记（grep 实证零命中） | — | — |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | firmware/display | `charpage_parse_data`（char_page.cc:119-134）部分失败路径（pre_pts/pre_counts calloc 一成一败 return 0）遗留已分配数组，随后 `charpage_show` 的 memset 清零会泄漏；本 diff 把两个新数组接入该既有部分失败形态 | changed hunk char_page.cc:122-125 + 资源所有权一致性启发（同函数 :94-104 既有四数组同款形态，非本包引入）；OOM 才可达且页面降级不渲染，无用户可见影响 | no-trigger——如后续批触 parse 失败路径，顺带补 free 后 return |
| FYI | judgement-call | firmware/display | `charpage_render_frame` 帧路径仍余 `kidpal_cs_point mids[64]`（~512B）栈局部 | changed hunk char_page.cc:226 + 结构齿阈值口径（~6.5KB 级 [256] 形态）；mids 为既有代码、量级低于结构齿阈值两个数量级 | no-trigger |

无 BLOCKING、无 SHOULD。结构闸 PASS/FAIL 不复述为 finding（verifier evidence 承载）；gate 全绿仍完成五轴语义 review（见下）。

## Five-Axis Review

1. **Correctness**：三验收齿语义齐备——A1 C 命令三次贝塞尔离散（词汇表内补全，段终点精确落于曲线、B(0.5) 逼近断言钉真离散非端点复制、与 Q/L/Z 同路径零互扰、负坐标/十进制 makemeahanzi 实况形状覆盖）；A2 既有四命令参照表逐位比对 + 未知命令拒绝语义不放宽（防御面零回归断言在齿内）；A3 结构齿钉帧路径零非 static [256] 数组（红基线期 3 处在位计数吻合红名）。边界条件（截断参数/缓冲溢出/NULL/空串）在 A2 防御面齿内覆盖。C 段多点循环逐段消费与既有 Q 多段循环同构，无漏读。
2. **Test quality**：断言真实行为非实现镜像——A1 断言点集内容（B(0.5) 数学逼近、端点精确），A2 断言逐位一致（参照值 = 修复前解析器实测 dump），A3 源级树扫结构断言（装配面非 host 编译，K4 test 7 先例）。三条齿全部 mutation 敏感红复现（sensitive:true × 3）。红基线由 acceptance-author 独立实跑落账，非 builder 自证。
3. **Contract/boundary safety**：`docs/contracts/**` 零触碰（git diff 零命中 + contract-lint 绿）；D3 strokes 为 makemeahanzi SVG path 自由文本，C 命令属词汇表内解析补全非契约改动（E2 铁律 + architect 快审确认归类在案）；小写 `c` 按既有 m/l/q/z 绝对同语义先例消费并有注释锚定；server/ 零改动，事件/tools/shared types 未触。
4. **Operability/security**：预解析堆缓冲 `n × 256 × sizeof(kidpal_cs_point)`（9 笔 ≈ 18KB 堆）落 `charpage_parse_data` 生命周期内、`charpage_free_state` 释放（:152-155），换入前 free + memset 防重入（:436-443 实查）；解析输入仍走既有 cJSON 树，无新攻击面；ESP_LOG 降级路径保留（unparseable → no blind render）。无 secret/权限/correlationId 面。
5. **Maintainability/simplicity**：改动局部——三文件各一机制（C 命令补全 / 预解析去栈 / 定容收编），复用既有 Q 段离散管线不引新渲染路径；`KP_TASK_LVGL_STACK_SIZE` 命名常量收编消除了 2026-09-15 一次性补丁的散写魔法值与「本行届时随包收编」挂账义务；无投机抽象、无重复实现。simplification tags 零命中（deletion test：删掉预解析层复杂度直接消失于帧路径，属合理承载非浅模组）。

## Residual Risk

- Known limitation:
  - M2K1-A4 live 端到端复证 deferred 归 M2-K3 设备轮——真机查『勇』9 笔完整演示不崩、示范字无缺笔的最终确认在 K3 持板轮；本包 device build 闸只担保编译，不担保实机行为。
  - NFR-PERF-2（5 秒档）黄灯转绿判据挂 K3 live 复测，本包不承载。
  - `decisionCoverage` 为空集（`[]`）——无 `landsIn: dropped && reason: risk-accepted` 条目，无 D<n> 风险接受挂账。
- Follow-up recommended: M2-K3 设备轮执行 A4 清单 + NFR-PERF-2 串口计时（roadmap §③ K3 demo 形态已预登记）。
- Human decision needed: 无——无契约/spec/依赖新增面；回滚方式 = 本包 commit 单点 revert（改动面收敛于 firmware 3 源文件 + 1 测试接线 + 新验收件，无 schema/契约迁移）。

## 影响面核对（worktree 口径）

- detect_changes: **N/A (worktree)**——Pod worktree 内 diff 对索引不可见，按规程以 `git diff --stat` + 关键共享符号 trace_path 抽查替代：
  - `charpage_render_frame` 入站 callers = charpage_timer_cb / kidpal_char_page_show / kidpal_render_node_cycle_next / McpServer::AddCommonTools，全在 firmware/main/display+main 场内，无跨层新增调用方。
  - `kidpal_char_stroke_parse` 唯一强定义 home = `firmware/main/display/char_stroke.c`（图内另匹配为验收件 weak 哨兵，CMakeLists 强符号覆盖关系已在接线注释锚定）；消费方 = char_page.cc（本包改为 parse 期一次性预解析）+ 验收件。
- 索引状态：工具图与工作树差异如实以本节手工抽查承载，未虚报；re-index 需求归主线程/human。

## Merge Packet / Supervisor 输入

- 待合并分支：`dev/260915-m1-k1a-key-layer-dispatch`（在飞常驻分支，本包 commit 冻结于其上，mergeOrder 28）。
- 合并路径：planner 主线程跑 `supervisor-verdict-gate.mjs` 机械闸 → APPROVE 后前置升级闸（`pre-merge-escalation-gate.mjs`）→ 放行 `merge-driver --pod` 本地合并；本 integrator 不执行 merge、不 push。
- 回滚方式：单 commit revert（见 Residual Risk）。
- 遗留脏行说明：`delivery/changes/.changes-index-state.json` 与 `delivery/workstreams/INDEX.md` 的 M 态为 EOL 归一化伪脏（`git diff` 内容零差异，仅 CRLF 警告），与本包无关、未裹挟进 commit、保持原样。
