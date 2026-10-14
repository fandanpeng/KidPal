# loop-retro · 2026-10-14（Dev Loop OS 周期复盘 · 第 4 轮）

> 范围：`pendingAnalysis` 18 个 new-format 包全析出（260924 批 4 + 260928 批 3 + 260929 批 1 + 261001 批 3 + 261010 批 3 + 261012 批 2 + 261013 批 1 + 261011 批 1），另 3 个 waiting（旧格式、loop-health.md 缺失，连续第四轮维持 waiting 不动）。游标由 `.claude/loop-retro/ledger.jsonl` 判定；本报告只读证据、不重算 verifier/supervisor 事实。
> 机器侧：每包 `pod-events-aggregate.mjs` 全量聚合；散文侧：18 份 pod-retro（planner 第一人称）逐份通读。18 份 pod-retro 全部在场，jsonl malformed 行全 0。
> 落账址注记：本报告初版落账于 `docs/crq/gov/`（dev/pod-m2 @ 33f8eaf），主账本同日改名 `docs/debt/`（261014-gov-review-crq-debt-rename，05e73eb）——本版为 debt 新址移植版；pod 侧旧址 commit 已 revert，以本版为准。

## 一、机器聚合总览

- **token coverage**：`{dispatchesTotal: 79, dispatchesWithTokenData: 76, dispatchesUnavailable: 3}`。三个 unavailable 全部是旧格式 schema 包（260928-school-start / 261001-repair / 261010-storylist）——aggregate 脚本未识别旧 schema，tokens/escalations 全零，跨包统计已以 jsonl 人工口径补正。其余 76 条 dispatch token 数据全齐（transcript 通道稳定）。
- **escalation**：agent-escalation 事件 12 条（不含以 human-decision 侧写的 supervisor 升级），gate-tooling-bug 仍是最大家族（5+ 例）；3 条机械判 unresolved 实义已闭合（详见 GC1/簿记条目）。supervisor/pre-merge 机械升级另有多连发（all-mock 判定三连发 + 自证陷阱两例），见主题 T2/T3。
- **longtail dispatch（≥40min）3 条**：260924-m4-k2 builder build 83.8min（190 toolUses，批内最长）、260924-perf-k1 builder build 49.1min、260924-m4-k3 builder build 41.3min——三条全是 build 相位，信息记录不立项。
- **正向信号**：fixer 轮整体少而准（多数包零 fixer；261010-device-reconnect 一轮命中；唯一多轮打偏的 261001-repair D2 反证「两轮不中即转日志定死」规则的价值）；over-scan 申报纪律持续良好；SendMessage 续棒复用（260928-story-connect 三棒）零 Red Flag；429 配额中断靠「逐项中断点清单 + 不 redo 已落盘半成品」无痛续棒。

## 二、跨批共性主题（跨 Pod 归纳，不复述单包已总结内容）

### T1 · 包元数据/冻结件形状税第四波——闭环建议至今未落，本波 6 包同型红

school-start 三连 Write 坏 taskpackage.json（「闸是唯一救了我的东西」）、char-window-key origin.upstreamType 闭集外自造 + contractRefs 空数组、storylist exitCriteria/coverage 紧邻位、realtime-facts structural 漏标 + mutationSkipReason 层级、topbar 逐字全文匹配短锚红、tomorrow-row coverage 合写正则 + perAcceptance 骨架不全。形态继续变形（coverage 合写 `A/B`、全角冒号/括号新变体），但收敛实践也出现了：tomorrow-row「起包直接给 N 条全量骨架（占位 turnedRed=false）」、perf-k1「六闸预跑基线让 finalize 后的红极便宜」。

处置：同根已在册（ARQ-260907-taskpackage-format-expectations-frontload，closed）→ GC1 记**复现三**追加证据，不新立；报错格式提示（治本半）与「一标一 id / 全集骨架」两条硬条目加急建议随追加注记进该行。

### T2 · supervisor/pre-merge 闸不消费前置申报与同款先例——all-mock 升级三连发

storylist（critical 触契约）/ prompt-v9（三理由逐条判读）/ realtime-facts（medium→high）三包全是 launch-card ⑦ 前置申报在先、同款先例在册，闸一律全 diff 重算升级，human 逐次人批同型包。

处置：查重无同根 → **新立 ARQ-261014-supervisor-escalate-preflight-ignored**。

### T3 · pre-merge 闸「自证陷阱」继续咬——contract-findings 字面扫描假阳性四例横跨三批

261001-m4-k3 `e3395f1a`（:95 否定句同族第三例，retro 自注）、261010-device-reconnect `13baa956`（条目 source）、261013-tomorrow-row `502f2515`（L61 复述触发）。ARQ-261011（deferred）立项后同型继续复现。

处置：同根在册 → **追加证据**进 ARQ-261011-contract-gate-literal-selfquote-false-positive（两个新时点补记 + 升 urgency 建议）。

### T4 · 收口义务的写权承载缺位——四包复发

home-v5 母版 docs/ui 三处无角色可写（`fe6c3f46`，human 裁主线程直落）+ CDQ 行无角色（`930257e5`）；storylist 收口三件全撞 enforce-scope（「第三次烧同一类坑」）；prompt-v9 写权口径两次误判（`359e51bb`）。定性：闸全对，缺「收口义务 → 承载写点」映射表与前置核对。

处置：查重无同根（ARQ-260923-dispatch-prompt-hard-terms 治的是派发书硬条目、ARQ-260929-docs-sync-spec-handoff 治的是 spec 面流向，均不同根）→ **新立 ARQ-261014-closeout-obligation-write-authority-vacuum**。

### T5 · pod-events 簿记一致性与遥测保真度——六类实据

escalation-resolved 闭合缺（3 例，聚合器高估未结数）、actor 误落 planner（2 例）、同决定双记（openclaw）、`resolves` vs `refEventId` 关联失效、时间戳倒挂（realtime-facts）、MODEL-TIER 升档「声明 ≠ 实际」（m4-k1 两次升档均 deepseek-v4.1-flash）+ dispatch-stamp 0 充数 + mainThreadUsage 快照不刷新（tomorrow-row 10 条逐字段相同）。

处置：与 friction-event-payload-schema 的 payload 面不同根（治的是事件生命周期与数据保真，非 payload 校验）→ **新立 ARQ-261014-pod-events-booking-telemetry-fidelity**。

### T6 · 合并闸工具链热修两例未入账 + INDEX.md 生成件相撞

merge-driver ReferenceError（`a8a3b44c`，热修 0303e98）、defaultDetectChangesCli 失配（`9b1181bc`，热修 670139a）——已修在树但从未入 CRQ 账本；tomorrow-row `502f2515` INDEX.md 与 main 全量重建同 hunk 相撞（「生成件以渲染器为权威」口径闸不知道）。附教育面三例（required-check 参数形状 / final 裸跑 fail-closed / Windows UUID）。

处置：查重无同根 → **新立 ARQ-261014-merge-gate-tooling-hotfix-batch**。

### T7 · 真机轮已成 high tier 事实上的阻断合并硬门，但责任位与成文缺位

defer-merge-pending-live-round 常态链三连（m4-k3 / archive-order / device-reconnect）+ m4-k3「三包零烧录零部署」human 现场推（`260b1f48`）+「以齿代现象」双向实证两包独立建议成文 + 真机轮红利实证（topbar TB-A4 白捡既有缺陷归因）。

处置：同根已在册——`ARQ-261001-host-blind-outcome-device-gate`（deferred，261001 D2 同一实证面、同一「设备轮升阻断合并硬门」建议，riskTier high / full-adversarial profile）→ GC1 记**追加证据**进该条（常态链三连 / 责任位缺位 / 红利实证 / 独立复证 / 伴生建议两条），**不新立**。

### T8 · 覆盖缺口从「零事件型」扩大为「结构性无事件类型承载」（→ GC3）

verifier 红轮 / brief-inputs fail-loud / 写权拦截 / 簿记自伤等摩擦一律无事件类型承载（events 只录五类），「retro 有主题、events 无对应 event」为常态（perf-k1 friction=0 而 retro 七主题全裸；261001-repair events 只剩 human 门与合并簿记骨架）。

处置：同根在册（ARQ-260907-friction-event-payload-schema，closed）→ GC1 记**追加证据二**；簿记一致性面独立成 T5 新条目。

## 三、GC1 · findings 对账 canonical / 已裁决项

对账集 = debt 账本在册 + 归档条目（friction-event-payload-schema / dispatch-prompt-hard-terms / docs-sync-spec-handoff / taskpackage-format 等 260923 批全部翻 closed 进 `_archive/2026/`）+ 直接相关 canonical（agent def、DEV-GOVERNANCE 现行文面）。结果：

- **4 条复现/追加（已裁决项未被行为闭合，记复现不新立）**：ARQ-260907-taskpackage-format-expectations-frontload（复现三，T1）、ARQ-261011-contract-gate-literal-selfquote-false-positive（两新时点，T3）、ARQ-260907-friction-event-payload-schema（追加证据二，T8/GC3）、ARQ-261001-host-blind-outcome-device-gate（常态链与责任位增量，T7）。均按「在既例行追加证据注记」处置，引用真实现行锚点（含归档路径与 debt 新址）。
- **0 条建议与现行 canonical 冲突**：无「补建已退役 def」类漂移。重点核过两处疑似：home-v5 `fe6c3f46` 提「uiux-author 不可派」——实核现行 def 在册且「dev-loop 包内不派本 agent」是其设计边界（仅 requirement-change 档 2 出场），非编制漂移复发，母版同步真空走 T4 新条目；T7 初判新立——实核 ARQ-261001 同根在册，按 GC1 改走追加。
- 交叉引用注意：pod-retro 多处仍把改进项当口头债滚动（perf-k1「docs-sync 错配建议登记 gov ARQ 至今未登记」——实核 `ARQ-260929-docs-sync-spec-handoff` 已 closed 于 260929 批，该棒建议所指的具体错配已被既有裁定覆盖，无需新立）。

## 四、GC3 · new-format 覆盖比对（机器聚合 vs 散文主题，人工逐主题核对）

new-format 包 18 个，逐主题核对结果：

- **系统性常态偏差**：「retro 有主题、events 无对应 event」命中近乎全部 18 包（verifier 红轮 / 写权拦截 / 簿记自伤无事件类型承载）——按 T8 追加进 ARQ-260907-friction-event-payload-schema，不另计新立。
- **直接失真 3 处**（retro 断言与事件流相反，皆因成文早于收尾相位、事后未追记）：char-window-v5「无 agent-escalation」vs `c89d7607`；openclaw「全程无 human 入场」vs `c3d96131`；tomorrow-row「零 agent-escalation」vs `502f2515`。→ 同上追加行，pod-retro 落笔时点纪律（收尾后追记或断言加写作时点限定）提请治理审视。
- **时序可解释类**（不计偏差）：合并门批准事件晚于 retro 成文（多包同型）；home-v5 retro 把 `930257e5` 标 pending 而其后 `d41f0b6e` 已裁（同型）。
- **记账形态小噪声**：openclaw 同一批准双记（`a5c3b27a`/`c3d96131`）、旧 schema 包 aggregate 全零——随 T5 新条目处置。

**转 ARQ candidate 数：0 新立、1 追加（同根判定）**。偏差不采信任一源。

## 五、human correction / 纠偏主题（人类反复出场的方向）

- 本轮 human 出场高度集中在**预期位**：合并门批准（approve-escalated-merge / approve-contract-merge-manual）与真机轮裁决（真机目检驱动 ruling 三连、TB-A4/RF-A5 持板记录）——无反复纠偏型新主题，延续上轮「真机轮是信息密度最高环节」结论。
- loop 内纠偏仅 2 例且均单发：device-reconnect `13baa956`（闸误报知情放行）、m4-k3 `260b1f48`（发现零烧录零部署）——后者已入 T7 追加行。
- **负向信号**（不立 ARQ、报告提请）：三处 retro↔events 直接失真（GC3）说明 planner 收尾相位后无「追记 reflex」——若 pod-retro 落笔时点纪律要成文，随 T5 条目一并考虑。

## 六、分流清单

**治理类 → architect-owned ARQ（主线程登记进 docs/debt/gov/）**：

| 动作 | 条目 | 主题 |
|---|---|---|
| 追加证据（复现三） | ARQ-260907-taskpackage-format-expectations-frontload | T1 形状税第四波 + 硬条目加急 |
| 追加证据 | ARQ-261011-contract-gate-literal-selfquote-false-positive | T3 自证陷阱两新时点 + 升 urgency |
| 追加证据二 | ARQ-260907-friction-event-payload-schema | T8 结构性覆盖缺口 + 三处直接失真 |
| 追加证据 | ARQ-261001-host-blind-outcome-device-gate | T7 真机轮事实硬门：常态链/责任位/红利/复证 |
| 新立 | ARQ-261014-supervisor-escalate-preflight-ignored | T2 前置申报/先例不消费 |
| 新立 | ARQ-261014-closeout-obligation-write-authority-vacuum | T4 收口义务写权承载缺位 |
| 新立 | ARQ-261014-pod-events-booking-telemetry-fidelity | T5 簿记一致性与遥测保真度 |
| 新立 | ARQ-261014-merge-gate-tooling-hotfix-batch | T6 热修留痕 + 生成件权威口径 |

**实现类 → 正常 dev-loop**：无（本波 18 包内未发现确定的 firmware/** 缺陷待修；topbar TB-A4 顺带立出的 TECH-261013-p3-reader-entry-no-text-silent-noop 已走既有 triage 通道，不重复）。

**信息类 / 接受风险（不强行加门）**：
- longtail build ×3（83.8 / 49.1 / 41.3 min）——build 相位固有编译税，观察项不立项。
- 429 中断续棒模式（逐项中断点清单 + 不 redo 半成品）——已内化正向；续棒期 MUTANT/取证残留盘点一条建议留给下次动 launch-prep/续棒文面时参考（storylist 实证残留险情）。
- 「以齿代现象」持续内化（archive-order 刻意留痕不重蹈、m4-k3-repair 日志定死、device-reconnect builder 主动上交行为疑点）——正向，别加门；「行为面观察单列 builder 模板节」建议随 T7 追加行 ⑤ 处置。
- grep --include 漏 `.service` 扩展（openclaw architect 补捞）——方法论：零残留类扫描先无 include 穷尽再 carve-out，留经验不立项。
- 单次教育面摩擦（Windows UUID / required-check 参数形状 / final 裸跑 fail-closed / JSON 自伤双牙兜住 / 误用真实 pod-events 探脚本产生假事件已删）——闸全部兜住且一次性学会，随 T6 条目 ③ 文面句处置，不单独立项。

## 七、waiting 清单（不动）

`260831-cloud-link-bringup`、`260831-simulator-lvgl-align`、`260913-happy-flow-host-capture`——旧格式（pre-cutoff）且 loop-health.md 缺失，连续第四轮 waiting。若 human 裁定其结构性永不析出，可走 human-only 销账通道（`--writeoff`）；本轮不自调。

## 八、下一周期关注点

1. **四条新 ARQ 的并批建议**：supervisor-escalate-preflight-ignored、closeout-obligation-write-authority-vacuum 两条都动 DEV-GOVERNANCE / 闸文面，可与 ARQ-261001（真机硬门，Track A）同窗口先后处置；merge-gate-tooling-hotfix-batch 与 ARQ-261011（同脚本 pre-merge-escalation-gate.mjs）必须同批；pod-events-booking-telemetry-fidelity 独立小批（纯脚本面）。
2. **四条追加行是滚账信号**：形状税已第四波（报错格式提示是唯一治本半，建议下一治理批优先）；自证陷阱三批四撞；覆盖缺口从零事件型扩成结构性；真机硬门已三波独立实证（261001 D2 ×2 + 261010 device-reconnect）——ARQ-261001 是本波最该优先升处置的条目（riskTier high，在册三周未动）。
3. token coverage 连续两轮保持高覆盖（76/79），旧 schema 包是仅余盲区；`261013` 起的包若再出现 mainThreadUsage 不刷新，T5 条目 ④ 优先排查。
4. 六条已闭环 ARQ 本轮翻 closed（260923 批全部 + friction/taskpackage 两行）——复现记法今后统一引 `docs/debt/gov/_archive/<年>/` 锚点（账本已改名 debt，别再引 docs/crq 旧路径；本波 pod-retro 已出现引用被归档路径的 docRefs 旧路径红闸，闸兜住了）。
