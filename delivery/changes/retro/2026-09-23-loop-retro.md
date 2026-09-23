# loop-retro · 2026-09-23（Dev Loop OS 周期复盘 · 第 3 轮）

> 范围：`pendingAnalysis` 18 个 new-format 包全析出（M1 批 5 + rma8 修复批 1 + M2 批 5 + M3 批 7），另 3 个 waiting（旧格式、loop-health.md 缺失，维持 waiting 不动）。游标由 `.claude/loop-retro/ledger.jsonl` 判定；本报告只读证据、不重算 verifier/supervisor 事实。
> 机器侧：每包 `pod-events-aggregate.mjs` 全量聚合；散文侧：18 份 pod-retro（planner 第一人称）逐份通读。

## 一、机器聚合总览

- **friction**：18 包共 57 条（over-scan 27 / unknown-malformed 14 / degraded 9 / scan-miss 4 / scope-drift 2 / stall 1）。malformed 14 条全部是 empty-payload（k1a×2、k4-reminder×2、n1b×1、m3-k4×7）——ARQ-260907-friction-event-payload-schema 的 malformed 告警列把它们全亮出来了（检测在工作），但写入口未收敛（详见 GC3）。
- **escalation**：9 条（n1b 2 resolved、m3-k1 1 resolved gate-tooling-bug、其余 6 条合并门 ESCALATE 3 + 真机 human finding 3）。5 条 unresolved 里至少 3 条随后经 human 批合并实际闭合（k2 / k3-char-window / k7 的 approve-escalated-merge 与 merge-completed 事件在案），但事件未标 resolved——机器聚合永久高估未结数。
- **token coverage**：`{dispatchesTotal: 103, dispatchesWithTokenData: 96, dispatchesUnavailable: 7}`。注意 harness 来源的 dispatch（M1 批为主）只有单值 total、input/output 恒 0；transcript 来源（M3 批）才有真实分项（模型 glm-5-3-flash / deepseek-v4-flash 为主）。K1a 的 5 条 dispatch 全部 no-tokenUsage。
- **正向信号**：18 包里 13 包零 agent-escalation；K6 全程 human 离席夜间自主跑、K5 主轮零打断——「前半程过目、后半程自主」的分层出场纪律连续七棒以上成立。M3 批 dispatch token 数据全齐（transcript 通道落地后）。

## 二、跨棒共性主题（跨 Pod 归纳，不复述单包已总结内容）

### T1 · mutation-check / 起包元数据「形状税」第三波跨棒复发（最重主题）

同一颗雷换了五副面孔：短 id（rma8，K4 教训未内化的第二次）、摘要改写 perAcceptance 键（k6 r2 静默失配）、嵌套对象（k7）、语义直觉造枚举值（k7 sourceType）、散文串替代结构体（k7 crossEnd）。rma8 retro 原话「K4 retro 的改进项我没有真正执行」；k7 retro 明言「非阻塞建议第三次入册 = 该升硬条目了」。

正向对照同样清晰：凡「回填前先读 checker 提取器源码」的棒（m2-k1、m3-k2、k5）全部免税；k5/m3-k4 起把「finalize 预飞三闸判据 checklist」当动作清单用，追加轮零元数据红。m3-k1 retro 的判词是对的：**「记坑的形状不如记『读 checker 的提取器再回填』——坑会变形，机制不会」**。

处置：同根已在册（ARQ-260907-taskpackage-format-expectations-frontload，已结 + 2026-09-13 复现在案）→ 按 GC1 记**复现二**追加证据，不新立；升级建议（起包前置清单硬条目 + 报错格式提示）随追加注记进该行。

### T2 · 拆包产物「有保质期的快照」——元数据与磁盘现实腐坏，无闸可拦

三包各修一遍 origin 枚举雷（m2-k1 / m2-k3 / m3-k1：portfolio-planner 产 `milestone` 非闸值域值）；m2-k3 sourceRefs 漏四处真实改动面（最重的一处恰是上包标注「保留不动、由本包统一收」的死态 API 承接面）；m2-k2 四处引用腐坏（路径不存在 / 冻结件已归档 / elf 路径 cwd 基准错 / refs 不覆盖真实改动面）；m3-k4 contractRefs 空数组。共同点：机械闸只查字段形态，查不出「引用指向的东西还在不在」——m2-k2 retro 明言 intake 读代码面是唯一提前手段，且 m3 批起已成 planner 自发惯例（连续净赚），但纯习惯未成文。

处置：查重无同根 → **新立 ARQ-260923-portfolio-decomposer-drift**（治理类，owner=architect）。

### T3 · 派发书模板缺硬条目——Write 被拦 / scope 写权 / mutation 证据落盘责任人

三条约定散落在 6+ 棒 retro 的「下次改进」里，每棒重学一遍：子代理 Write 被环境拦（K7 一棒三撞，前两次白跑转录）；builder 无 taskpackages 写权（K6/K7 两复现，mutation evidence 被迫改派或代落）；mutation 执行证明 provenance 转述差（K5 跨三棒欠账，K6 写进派单后销账走通，K7 又回到代落）。「齿转义 + planner 代落」手法本身已稳定，但依赖 planner 每次想得起。

处置：查重无同根 → **新立 ARQ-260923-dispatch-prompt-hard-terms**（治理类，owner=architect）。

### T4 · 契约面 merge-driver 机械路径整条封死

K7 实证：升级闸 `hasContractGateEvidence` 硬编码 false → 契约面 diff 恒 BLOCK → merge-driver 形同虚设，只能现场即兴人合（K7 靠 pre-merge 预检 + rebase 后 ff 人工走通，pod-events 自记「非 merge-driver」）。与已结的 ARQ-260902-escalate-merge-no-human-path 是孪生缺口：ESCALATE 有人批机械通道，契约 BLOCK 无任何通道。

处置：查重无同根 → **新立 ARQ-260923-contract-merge-deadpath**（治理类，owner=architect）。

### T5 · 编制漂移：planner.md 引用的 uiux-author 不存在

GC1 实核现行锚点：`.claude/agents/` 14 个 def 无 uiux-author.md，而 planner.md frontmatter Agent 列表与正文 Step 6b 仍引用（k3 判 L2 后派不出、直翻降级）。

处置：查重无同根 → **新立 ARQ-260923-planner-roster-reference-drift**（治理类，owner=architect）。

### T6 · 治理工具「用户面」噪声与危险静默

commit-safety「staged 为空」误导提示（读 add 前快照；跨三棒在案）；verify.mjs 未知 phase 值静默回落 scoped（K7 差点把 scoped 绿当 final 绿——危险形态）；治理脚本传参形态不一 + 无 --help（changes-index 触发全量重建、mutation-check 位置参数）；enforce-scope-canary 期望过期漂移无主（m2-k2 四条）。

处置：查重无同根 → **新立 ARQ-260923-gate-tooling-noise-batch**（治理类小修集合）。

### T7 · current/ 指针面：已裁工具牙落地、调用侧未带 + 第二时点暴露

ARQ-260913 残余①未落继续复现：k4-reminder（dispatch 落错包）、m3-k1（漏传 --task-package-id，`--expect-change-id` 硬校验在场但 planner 未传）、m3-k2（中途回填后忘刷副本 → r1 红）。同族第三面：required-check 按 current/ 目录找 brief，双份维护跨三棒被同一闸咬（k5/k6/k7）。

处置：同根在册 → **追加证据**进 ARQ-260913-current-pointer-implicit-global，不新立。

## 三、GC1 · findings 对账 canonical / 已裁决项

对账集 = ARCH-REVIEW-QUEUE 既登项 + 直接相关 canonical（DEV-GOVERNANCE、agent def、queue 现行文本）。结果：

- **3 条复现（已裁决项未被行为闭合，记复现不新立）**：ARQ-260907-taskpackage-format-expectations-frontload（起包格式前置节已落地、行为未闭合，T1）、ARQ-260913-current-pointer-implicit-global（残余①未落 + 新时点，T7）、ARQ-260907-friction-event-payload-schema（检测面已落地、写入口未闭合，见 GC3）。均按「在既例行追加证据注记」处置，引用真实现行锚点。
- **0 条建议与现行 canonical 冲突**（无「补建已退役 def」类漂移——uiux-author 从未在册退役，属编制从未落地 + planner.md 引用未摘，故 T5 走新立而非复现记法；实核 planner.md:4 / :54 与 agents 目录清单）。
- 交叉引用注意：pod-retro 多处把「非阻塞建议入册」（mutation-check 报错提示、brief 双份）当口头债滚动，本轮一并钉进上列 ARQ 行，不再以散文形态滚账。

## 四、GC3 · new-format 覆盖比对（机器聚合 vs 散文主题，人工逐主题核对）

18 包逐主题核对结果，覆盖偏差 3 处（含 ledger=0 / retro=N 型）：

1. **K2（keybar-three-slot）零事件型**：pod-events 仅 3 条（2 human-decision + 1 merge-completed），dispatch=0、friction=0、escalation=0；pod-retro 散文摩擦主题至少 6 条（烧录偏移错位 / 字体缺形 / 捕获窗空转两窗 / fixer 三轮 bisect / 真机轮 FAIL 分账 / 挂账四件）。机器侧完全读不到这包的过程摩擦。
2. **K7（schedule-grid-restore）零事件型**：pod-events 仅 4 条（3 human-decision + 1 merge-completed），dispatch=0、friction=0；pod-retro 头注自报「子代理派发 ×6」且散文摩擦主题至少 6 条（闸形状红两轮 / Write 被拦三次 / 写权代落 / memcmp 裁决 / merge-driver 契约封死 / C1 条件张力）。dispatch-stamp 未跑或未落，机器聚合对该包结构性失明。
3. **m3-k4（cjk-dynamic-text）payload 空转**：friction 7 条全部 empty-payload（unknown）——散文主题丰富（两轮 scoped 红判据、追加轮 live FAIL 两轮、HPB 跨包修订），机器侧只剩「有 7 条看不懂的 friction」。

处置：偏差不采信任一源。零事件型是 malformed 检测结构性拦不住的更深覆盖缺口（ARQ-260907-friction-event-payload-schema 原文已含「ledger=0/retro=N」面）→ 追加证据进该行（含 escalation resolved 闭合链缺），不另新立。**转 ARQ candidate 数：0 新立、1 追加**（同根判定）。

## 五、human correction / 纠偏主题（人类反复出场的方向）

- 真机轮反馈是信息密度最高环节（display-fix：方向修正几乎全部来自 human 现场反馈而非流程闸）——正面经验，保持快节奏，不折算成门。
- n1b 一条 human-correction（视觉对齐 + % 返工）已消账。
- **提请 human 明确一次**：「按 dev loop 正常走、没有 block 的问题不用找我拍板」被连续两棒（m2-k2/m2-k3）planner 解释为「派发书过目门在本次 loop 内免除」，两棒均如实标注「这是我的解释」。若本意只是少请示，该解释超出授权——需要一句明确的话定口径（不立 ARQ，报告提请）。**→ 已裁（2026-09-23，human 当场回复）**：「不用每次都请示」——planner 的解释升级为明确口径，据此新立 **ARQ-260923-dispatch-oversight-uncodified**（把分层出场口径成文：常规派发不逐次请示 + 必升面逐条钉死向严不减 + 免除动作仍落 pod-events 留痕），本报告分流清单由 5 新立增为 6 新立。

## 六、分流清单

**治理类 → architect-owned ARQ（主线程登记进 ARCH-REVIEW-QUEUE）**：

| 动作 | 条目 | 主题 |
|---|---|---|
| 追加证据 | ARQ-260907-taskpackage-format-expectations-frontload | T1 形状税第三波 + 升硬条目建议 |
| 追加证据 | ARQ-260913-current-pointer-implicit-global | T7 残余①复现 + brief 双份同族面 |
| 追加证据 | ARQ-260907-friction-event-payload-schema | GC3 零事件型覆盖缺口 + escalation 闭合链 |
| 新立 | ARQ-260923-portfolio-decomposer-drift | T2 拆包快照腐坏 |
| 新立 | ARQ-260923-contract-merge-deadpath | T4 契约包合并机械死路 |
| 新立 | ARQ-260923-dispatch-prompt-hard-terms | T3 派发模板三硬条 |
| 新立 | ARQ-260923-planner-roster-reference-drift | T5 uiux-author 编制漂移 |
| 新立 | ARQ-260923-gate-tooling-noise-batch | T6 工具用户面小修集合 |
| 新立 | ARQ-260923-dispatch-oversight-uncodified | 派发书过目门口径未成文（human 2026-09-23 裁定「不用每次都请示」后追加登记） |

**实现类 → 正常 dev-loop**：
- ccache 挂进 espidf_wsl.sh（K1a 交 loop-retro 汇集；跨 worktree 共享编译缓存，/mnt/c 编译税的根治项）——单切片，交 planner 下次固件包承接或 human 一次性授权。
- m3-k3 的 trigger=immediate 机器日志缺位（K1 部署面 drop 入口挂载欠账）已在包 review-packet 登记，待后续部署面包偿还。

**信息类 / 接受风险（不强行加门）**：
- host 齿盲区常识集（纯函数齿优先 / 设备字体上 host / 跨多边界采样 / 真机前显式限定绿语义）——已在棒间自传导且前置生效（K6 退避决策表是设计端前置首例），别加门。
- acceptance 撰写约定三条建议（含 padding 结构体禁整包 memcmp / closeout condition「保护意图 vs 证据清单」分离写法 / 负向齿优先）——留给 architect 下次动 acceptance-author.md / acceptance-evidence.md 约定面时参考，K7 retro 原文在案。
- structuralWords 正则不含中文（m3-k2 备查，按其原判「不值当单独立项」维持）。
- 视觉类包 fixBudget 给真机轮留专项余量或「烧录程序面 vs 代码缺陷」分账（K2 建议）。
- 模型池配额熔断预案（熔断先探测备用池、opus 通道独立可用）——K1a 教训，n1b 起已兑现，已内化。

## 七、waiting 清单（不动）

`260831-cloud-link-bringup`、`260831-simulator-lvgl-align`、`260913-happy-flow-host-capture`——旧格式（pre-cutoff）且 loop-health.md 缺失，连续三轮 waiting。若 human 裁定其结构性永不析出，可走 human-only 销账通道（`--writeoff`）；本轮不自调。

## 八、下一周期关注点

1. 六条新立 ARQ 的并批建议：dispatch-prompt-hard-terms、dispatch-oversight-uncodified 与 buildfeature-launch-prep-skip / pod-requirements-not-authority 同族（同动 planner.md 编排文面，四条一批最省）；portfolio-decomposer-drift 的枚举修正半（①）成本极低可先行；contract-merge-deadpath 与 gate-tooling-noise-batch 可搭任意批。
2. 三条复现追加行是「已裁决未闭合」的滚账信号——taskpackage-format 已第三波，下一治理批优先把它升成硬条目 + 报错提示（改 checker 报错是唯一治本半）。
3. loop-retro 阈值滚账（K2 16 → K3 17 → K4 18 → K5 19）本轮清账；K2/K7 零事件型覆盖缺口若不闭合，下轮 GC3 还会报同型偏差。
