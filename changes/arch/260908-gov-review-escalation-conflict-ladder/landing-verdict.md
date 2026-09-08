# landing-verdict · 260908-gov-review-escalation-conflict-ladder

> 判定者 = landing-verifier（独立核验）；判据 = FROZEN v1.0 PROPOSAL（`4c38ba9`）§2 T1-T5 + §5 验证集 + decision-packet 裁定项表；被审 diff = `git show 535de50`（6 manifest 文件 +135/−29）+ 锚行 commit `d1d13ee`；不采信 landing-summary 自述，全部结论出自本 verifier 独立复跑与逐行 diff 对读。

## 0. Verdict：**RED（有条件退回——修完三项即转绿，无需重开设计）**

**根因一句**：六个 manifest 路径的改法本体全部忠实落地（判据换轴、探测、分级路由、消费、成文、销账逐一在案，结构门 ok:true、三自检独立复跑全绿、probe 路由六态独立探针全对），但冻结验证集有两条**静默缺失且 1b 未申报**——V4（升级件内容面夹具）从 summary §2 执行记录表中整体消失、V1 第五例（③ 派发不可用 → ④）无机判宿主亦无申报——与上一批（merge-chain-hardening）verifier 曾判 RED 的「验证项静默缺失」同类。

**GREEN 条件（全部满足后重跑结构门 + verifier 复核，merge 归 human 门）**：

1. **summary §2 执行记录表补 V4 行**：如实申报 V4 夹具未执行；其中「逐文件相交清单」部分补机器断言（见条件 2），「双方版本锚 / ④ 级 architect 意见槽」部分如实申报「无机械宿主」——该两字段无生产者（architect 意见产自 ③ 评审之后、版本锚无组装代码），冻结 T3/V4 对其属超规格，须冻结层追认降档或留根（见 §4 残余 a）。
2. **gate 自检补一条升级件内容断言**（宿主在案、成本极低）：对 `overlap-tradeoff-block` 形态断言 BLOCK reason 或 probe-out JSON 含逐文件实撞/相交清单（verifier 已用独立探针证实 `parallelOverlapProbe` 输出面正确——见 §5，缺的只是把该断言钉进自检电池防回退）。
3. **1b 补两条申报**：① V1 第五例「③ 派发不可用 → ④」无 gate 级机械宿主，由 §10d ③ 行 fallback 句承载（prose fail-closed）；② §10d「已登记 §10c 登记表形态恒归 ②」的登记表 authority 档未机械化——gate carve-out 只用 `.vN.md` 正则兜底档，登记表内非 `.vN` 形态（engine-patch 类）实撞落 ③ 而非 ②，方向从严（fail-closed、不 widen 放行集，与 A-7 补集不变式一致）但与 §10d 成文字面有差，须申报并留根（登记表机械化归后续批，与 1b-3 双源留根同族）。

## 1. 结构门输出（原样记录）

```
$ node .claude/scripts/landing-fidelity-check.mjs changes/arch/260908-gov-review-escalation-conflict-ladder 4c38ba9
{
  "ok": true,
  "gate": "landing-fidelity",
  "reviewDir": "changes/arch/260908-gov-review-escalation-conflict-ladder",
  "baseRef": "4c38ba9",
  "frozen": true,
  "packetBinding": true,
  "proposalTargets": [ 6 路径 = manifest 全集 ],
  "summaryTargets":   [ 同上，6/6 相等 ],
  "unauthorized": [], "claimedNotLanded": [], "plannedNotLanded": [],
  "gitDegraded": false,
  "citationAnchorAdvisory": { "checked": 0, "problems": [] },
  "verbatimQuoteAdvisory": { "checked": [], "missing": [] },
  "advisory": { "changedCanonicalNotInProposal": [] }
}
---exit=0
```

**锚行 commit `d1d13ee` 定性**：冻结（`4c38ba9`）后补 T1-T5 五行「目标文件：」锚——逐行与冻结 manifest 六路径及 decision-packet confirmed 目标集**逐字一致**、T 正文零改动，属非语义格式补齐（fidelity fail-loud 的锚集对账要求）；本 verifier 追认在案。landing 顺序（先跑门 535de50 后补 summary d6a2e7f / 锚行 d1d13ee）吸收了上一批「未实跑结构门」教训，本批结构门 ok:true 为实跑结果。

## 2. 逐文件核对表（manifest 6 路径，全部核对、无抽查跳过）

| # | 文件 | T 项 | 语义 | 冻结依据 → 实际 |
|---|---|---|---|---|
| 1 | `.claude/hooks/supervisor-verdict-gate.mjs` | T1 | ok | `isCrossSystem` + `distinctAppSystems` 整体摘除（退役注记 :213-217 指向判据换轴与 §10d）；`classifyHigh` 保留 core.*/全 mock/安全轴三轴（cross 位改退役注释）；verdict APPROVE/BLOCK 语义与证据完整度判定零触碰（B1 修法红线）；自检两例 cross-system 断言改写为 `retired-impl-axis-*-now-low`（V1 要求）；fp 例输入字面 `cross-system`→`跨系统`（为 V5 同文件 grep 零命中让位，测试语义等价——原意「meta 文本不触发安全轴」保持）；`cross-system` 字面活代码零命中（独立 grep = 0，V5 锚） |
| 2 | `.claude/scripts/pre-merge-escalation-gate.mjs` | T1/T2/T3 | ok（附 V4 缺口） | `parallelOverlapProbe` 导出（merge-base 交集 + `merge-tree --write-tree --name-only` 实撞 + 噪声滤清 1b-1 + add/add 判定 + 异常 fail-closed → tradeoff）；`VERSIONED_ARTIFACT_RE_LOCAL` 同形双源留根注记在案（1b-3）；evaluate devloop 档 `tradeoff → BLOCK parallel-overlap-tier3`（含实撞清单 detail）③ 级路由；六条例外逐字未动（diff 无该区 hunk）；`--probe-out` JSON 制品 PASS/BLOCK 两态都落、落盘失败 advisory 不改退出码（S2）；buildCtx 接线 + 自检 4 新例（25/25 复跑绿）。**缺口**：V4 夹具未落（§0 条件 1/2） |
| 3 | `.claude/scripts/merge-driver.mjs` | T4 | ok（附观察 b） | `lastOverlapProbe` 逐 Pod 隔离（processPod 入口置 null，board 模式防串扰）；Step 1b gateArgs 增 `--probe-out <wt>/.claude/taskpackages/<id>/overlap-probe.json`（Step 0 白名单域内）并在 PASS 后读取（读不到/解析失败 = 无探测面走既有链，不放行）；rebase 实撞与预探测 `no-overlap` 不一致 → warn 存疑向严升 ③ + blocked（fail-closed，冻结 V2 新例字面）；Step 5b payload 增「并行相交面=tier×N件」摘要。既有 T7/T8 夹具零触碰（20/20 复跑绿） |
| 4 | `.claude/references/pod-coordination.md` | T2 | ok | §10d 新节（§10c 之后、相关之前）：分级路由表 ①②③④ + 取舍机判判据（无取舍 = 不同 hunk/语义段；有取舍 = 同 hunk 相撞，版本字面互斥复用 guard-class wiring 判据）+ S1 carve-out（已登记 §10c 形态 add/add 恒归 ②、未登记 → ③ 补集不变式）+ ③ 派发失败/超时 → 直接 ④ fallback（S3）+ ② 通道「git 自并 + **双方冻结件 + 全档复跑**」（OQ1=(a)）+ ④ 升级件内容（逐文件清单 + 双方版本正文 + architect 意见 + 消解选项）+ 人批走既有 `--human-approval-ref`（零改动）。冻结 §0 梯表逐级对齐 |
| 5 | `docs/DEV-GOVERNANCE.md` | T5 | ok | 「landing 通道（常态规则）」段内补一句「例外升级拍板的升级件必含并行相交面内容（分级路由…见 §10d；human 批的是**消解方案**而非放行仪式）」——S5 裁定落点（段内短语、非独立节）逐字形态符合；不新增 human 触点 |
| 6 | `docs/ARCH-REVIEW-QUEUE.md` | T5/V5 | ok | `ARQ-260907-escalation-cross-system-overfire` 自 §1 开放表删除、结移 §2 已结索引（:77，分隔行之后、表结构正确；含承载批引用 + human 三裁与追认在案注记） |

**越权扫描**：535de50 全量 6 文件 = manifest 全集，`changedCanonicalNotInProposal` = []；契约面零触碰；supervisor verdict 三态裁决规则零触碰；无私货。

## 3. 1b 三条偏差裁定

1. **探测面噪声滤清（1b-1）——裁定：落地忠实、等价成立、申报充分。** verifier 独立实证：`merge-tree --name-only` 输出确实混 informational 行（探针 F 态：`Auto-merging a.c` 行在案）；滤清 = 实撞集 ∩ 双方改动交集，结构上正确（merge 冲突只可能发生在双侧都改动的文件上）；tier 判定只依赖滤清后实撞集（探针 A/B/C/D/F 六态复证），判定语义零变。冻结稿「产出 conflictedFiles 清单」的隐含干净清单假设与工具实况有差，落地修正是忠实于意图的实现修正。
2. **V2 新例降档为代码路径亲核（1b-2）——裁定：降档申报成立，一处表述失实如实记录。** ①冻结 V2 两条新例（预探测 no-overlap 但 rebase 实撞 → 升 ③；② 级并集 + 双方齿复跑绿 → 过）均未落自检夹具（driver 自检仍 20 例旧），降档理由（processPod 全流程夹具化超出既有自检边界）与 260908 前批同款，成立；②verifier 独立复核代码路径：warn 分支（merge-driver.mjs :1344-1347，tier==='no-overlap' 与实撞不一致 → 存疑向严 + blocked，fail-closed）与 Step 5b payload 拼接（:961-962）在案且正确；③**但 summary 称「Step 5b payload 断言代测」失实**——自检 20 例无新增、payload 新摘要无任何机器断言（T6 夹具断言的是既有字段），该表述应更正为「纯代码路径亲核、无机器断言」。
3. **gate 本地正则双源留根（1b-3）——裁定：成立，一处指引落空。** `VERSIONED_ARTIFACT_RE_LOCAL` 与 merge-driver `VERSIONED_ARTIFACT_RE` 同形双源、留根注记在 gate 代码头（:166-168 probe docstring + :163 行注）；lib 收编会扩 manifest（本批未授权）理由成立。**指引落空**：申报称「留根注记在…本 summary §4」，但本 summary 无 §4 节（结构止于 §2c）——引用了不存在的节，小瑕疵。另见 §0 条件 3-②（登记表 authority 档未机械化的申报义务）。

## 4. 破坏性自检结论

- **gate 六条例外**（契约面 / off-main / blocked / riskTier / provenance / gov GREEN 锚）：diff 无该区 hunk、逐字未动——canary 承诺兑现。
- **supervisor verdict APPROVE/BLOCK 语义与证据完整度判定**：零触碰（非目标红线）；riskTier 派生其余轴（core.*/全 mock/安全轴）保留、既有断言零回退（hook 69/69，与改前同数——两例原地改写非删减）。
- **260908 前批落地件**：merge-driver T7a-g/T8a-c 夹具原样在册（20/20 含之）、T5 CDQ 核验行（contractFindingsAccounted）原样在册（gate 25/25 含之）、版本化制品 add/add 特判与指引未重写（本批复用）——非目标「不重写」兑现。
- **探测只读性**：`merge-tree --write-tree` 不动工作树/refs（冻结已精确化 ODB dangling 对象可 GC）；probe 异常路径不落盘不放行。
- **契约面 / spec / CDQ 写权**：零触碰。
- **本机版本前提**：git 2.46.0 ≥ 2.38（`merge-tree --write-tree --name-only` 可用）——留根清单「实现期首验」项 verifier 实测可销。
- **核验独立性自评**：6 路径 diff 全量逐行对读；三脚本自检独立复跑；probe 纯函数注入 execFn 六态探针；V5 grep 全套独立复跑；无跳过敏感面（hook 判据宿主 = 自升级高发面，已逐行核）。

## 5. 抽验记录（verifier 独立复跑，2026-09-08）

| 项 | 冻结要求 | 独立复跑结果 |
|---|---|---|
| V1 gate 自检 | 21 旧零回退 + 新例 | **25/25**（21 旧 + ladder 4 新：union-safe 放行 / tradeoff BLOCK / probe-fail fail-closed / no-overlap 放行）。**第五例「③ 派发不可用 → ④」未落**（§0 条件 3-①） |
| V1 hook 自检 | cross-system 摘除断言改写 + 零回退 | **69/69**；`retired-impl-axis-diff-now-low`（schedule-p2 双面板形态 → low）+ `retired-impl-axis-subsystem-now-low`（subsystem 多值 → low）两例在册 |
| V2 driver 自检 | 20 旧零回退 + 2 新例 | **20/20**（零回退）；2 新例未夹具化（1b-2，代码路径亲核由 verifier 复核确认） |
| V3 演练 | 双包抢 v3 全链零 human 触点 | summary 报两轮（噪声暴露→净清单）；verifier 未复演全链，以 probe 六态探针 + 滤清逻辑独立实证替代（探针 A/F 即演练暴露的噪声形态） |
| V4 升级件内容面夹具 | ③/④ 级断言相交清单 + 版本锚 + 意见槽 | **未执行、summary §2 无 V4 行、1b 未申报**——§0 条件 1（最重发现） |
| V5 投影 grep | 逐条 | §10d=1、parallelOverlapProbe=2、DEV-GOVERNANCE「消解方案」=1、hook `cross-system` 字面=0、driver「并行相交面」=3、queue 结移行在 §2（:77）——全过 |
| V6 回放对照 | 双 Pod diff 回放 + 真跨系统正例 | 夹具级双断言在册（retired 两例 = 误报归零；gate tradeoff BLOCK = 不漏报）；全量 git 历史重构回放未做——summary V6 行已如实申报留观测后批，可接受 |
| probe 路由六态（verifier 加测） | — | A 非登记实撞→tradeoff / B 全 `.vN.md` 实撞→union-safe（S1 carve-out）/ C 相交无实撞→union-safe / D 零相交→no-overlap / E merge-base 失败→fail-closed tradeoff / F informational 噪声行滤清——六态全符合冻结 T1/T2 路由语义 |
| 结构门 | ok:true | §1 原样，实跑确认 |

## 6. 判定依据与边界

- 本 verdict 为 verifier 姓氏制品、唯一产出物；verifier 未改任何被审文件（探针在 c:\tmp 只读注入运行）。
- **最重发现**：V4 验证项从 summary §2 执行记录表静默消失且 1b 未申报——改法本体（probe-out JSON 制品 = 逐文件相交清单的机械基底；BLOCK reason 带实撞清单 detail；DEV-GOVERNANCE 句钉 human 触点语义）已忠实落地，缺的是防回退夹具与如实申报，修复成本极低（§0 条件 1-3）。
- **残余留根（不阻塞，随条件 1 申报后归档）**：(a) 冻结 T3/V4 的「双方版本正文摘录锚 / ④ 级 architect 意见槽 / 消解选项」无机械生产者（意见产自 ③ 评审后、无组装代码），§10d ③/④ 行以 runbook prose 承载升级件全规格、gate 只供数据基底——建议冻结层追认该降档并留根；(b) probe 在 buildCtx 对 gov 档也无条件执行（结果 devloop 档才消费）——只读无副作用，纯效率观察；(c) overlap-probe.json 由 Step 1b 落盘后在本 invocation 内不提交，依赖下次 Step 0 白名单收口自愈——卫生观察；(d) union-safe 预测与 rebase 实撞的其它不一致形态（非 no-overlap）无显式 warn——冻结 T4 括注只钉 no-overlap 例，落地按字面。
- 五条件修毕（summary 补申报 + gate 一条断言）后由主线程重跑结构门并交 verifier 复核转绿；merge 决定恒归 human 门。
