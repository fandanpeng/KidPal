# architect-landing-summary · 261019-arch-review-p3-record-dialogue-guide

packet-id：FROZEN PROPOSAL v1.2 §2 改法集（T1–T7 + 契约包）+ §5 验证集 · 评审轮次 **R1 BLOCK(F1,F2,F3,F4) → v1.1 修毕 → R2 GREEN（附 R2-N1/N2 随 v1.2 收口）** · change-id 261019-arch-review-p3-record-dialogue-guide（跨面 spine = 261019-p3-record-dialogue-guide，facePlan 顺序 2；261001 Package B 吸收定形）

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 · 决策 25 取代注记（§10 决策 25 句尾 append，悬空指针同修） | `docs/architecture.md` | 已落：正文一字未删，注记逐字 append 于同条目句尾（:424，内联同段） |
| T1 · 新增 ADR-013（五件格式，append-only） | `docs/ADR.md` | 已落：ADR-013 全文逐字 append 至文末（含决策④ provider 出口钉 / 备选⑥ front 直连放弃，v1.2 版） |
| T2 · §5.1.2 front 行三片段（称谓「故事机械面→故事面」/ 片段 A 记录语境对话态+卡壳计时器族 / 片段 B 卡壳计时 sweep）+ story.py 行片段 | `docs/architecture.md` | 已落：四处逐字替换，行内其余内容不动 |
| T3 · §5.2 场景④ 链路前半整段重写（对话式引导记录 / Package B 定形 / E3 检出器 / 整理步 / prompt 载体与 provider 出口钉 / 丢稿口径显式化）+ 段尾误判兜底句 | `docs/architecture.md` | 已落：整段逐字替换，story_list 三时点 / relay 收紧 / 删除链路后半段原样保留 |
| T4 · 七处联动（§2.3 :91 / §5.1.1 :169 / §9 to-be② / §5.1.1 :173 / §5.5.4 :293 / §2.4 原则 3 append / §4 NFR 映射表补 NFR-PERF-3 行） | `docs/architecture.md` | 已落：七处逐字替换/append/插行；:325/:419 引擎侧 120s 保留面未动 |
| T5 · 契约 §1.4 新增 `start_organizing` 表行 + 表后契约变更记录条 + §1.4 节标题括注对账 | `docs/contracts/interface-contract.md` | 已落：新行插在 `start_recording` 行后；变更记录插在表后、§1.5 之前；节标题改「OpenClaw 判意图或 front 检出后下发」 |
| T6 · 契约措辞对账：§1.4 `start_recording` 行 + §四 `kidpal_session_ctx` 条两片段 | `docs/contracts/interface-contract.md` | 已落：三片段逐字替换，wire 形状零改（消息形状/闭集/幂等/兼容句全保留） |
| T7 · `architecture.md` §12 变更记录 changeId 行 append（定稿标记） | `docs/architecture.md` | 已落：新行置于 `261018-sd-log-persist` 之上，changeId = spine changeId `261019-p3-record-dialogue-guide`，依据列含「§4 NFR 映射表」（R2-N1 收口）、无 FROZEN 版本 token |
| 契约包 · 端云协议（三节） | —（PROPOSAL §2 承载） | 结构义务已满足；四分类 = ACCOMPANIES_SPEC_CHANGE（reviewer R1/R2 独立裁定维持） |
| CDQ 登记（主线程账本动作，非 manifest 目标） | `docs/CONTRACT-DECISION-QUEUE.md` | 已落：`CDQ-261019-p3-record-dialogue-guide` 行 append（三触点），状态 `PRECHECKED`（待 landing-verdict PASS 翻 LANDED） |

## 2. 验证证据（landing 时实跑，2026-10-19）

1. **PROPOSAL 结构门（preflight 干跑）** — `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261019-arch-review-p3-record-dialogue-guide/PROPOSAL.md` → `"ok": true`、`"findings": []`、`"advisories": []`、anchorUnion = {docs/ADR.md, docs/architecture.md, docs/contracts/interface-contract.md} ≡ landing-manifest。**绿**。
2. **锚完整性** — `node .claude/scripts/spec-anchor-check.mjs` → `"designIdCount": 64`、`"errors": []`、`"ok": true`（基线 64 不跌，本批不改小节编号、不触 UI 锚）。**绿**。
3. **120s 语义残留对账（§5 条 3 硬断言）** — `grep -n "60s/120s 双阈值\|front 侧计时 sweep 自动收尾\|120s 纯决策" docs/architecture.md` → **零命中**（基线命中 :91/:389/:214/:193 均落在被替换行内，R1-F2 收窄版断言满足）。
4. **悬空指针对账（§5 条 4 硬断言）** — `grep -o "机制细节见 §2.10" docs/architecture.md` → **恰 2 处命中**（:424 原句一处 + T1 注记引述一处，同一条目区间内联 append；`grep -n` 计行 = 1 行 2 处，符合 R2-N2 钉定措辞）；`grep -n "实际承载 = §5.1.2 + §5.2 场景④"` → 1 命中且与注记同行。**满足**。
5. **契约措辞对账（§5 条 5 硬断言）** — `grep -n "网关切该会话为累积态\|即切该会话累积态\|设备 manual 聆听仍经既有 listen 消息承载" docs/contracts/interface-contract.md` → **零命中**（R1-F3 收窄版断言满足）；`grep -c "start_organizing"` → **2 命中**（§1.4 表行 + 变更记录条）。**满足**。
6. **契约机械凭据闸（post-commit、合并前）** — `pre-merge-escalation-gate.mjs`：**landing 时尚未跑**，归合并门前置（见 §3 诚实注记 3）。
7. **逐字 diff 对账（人读）** — **归 landing-verifier**：需逐文件核对实际落地 == FROZEN v1.2 §2 各 T 的插入/替换段（T1 一段 append、T2 四片段、T3 整段替换 + 尾句、T4 七处、T5 一行 + 一条变更记录 + 一处节标题括注、T6 三片段、T7 一行；另 ADR-013 append 与 CDQ 行）。
8. **边界守持核（人读）** — **归 landing-verifier**：60s 问答超时语义不动；§6.4/决策 21 引擎配置历史叙述原样；归档下游零改；`start_organizing` additive、既有三 action 语义不变；§5.5.5 零偏移。

## 3. 诚实注记

1. **本摘要 §2 的第 6、7、8 条在写作时归属 post-commit / landing-verifier**——本摘要如实标明归属，不预支结论；对应结论以 `landing-verdict.md` 与合并门输出为准。
2. **CDQ 行不进 landing-manifest**——跨时间账本登记、非被 diff 的契约路径，其「loop 凭据」列依赖 landing-verdict。261017 R2 NIT-3 同款裁定。
3. **合并闸对本批的适用范围存疑（在册 ARQ 已记）**——`pre-merge-escalation-gate.mjs` 的 kind 探测只认 `delivery/taskpackages/<id>` 与 `delivery/review/<id>` 两条 lane，本批制品是 intake spine + review PROPOSAL 双落点；`ARQ-261017-merge-gate-scope-unstated` 已记载该闸对 intake/debt 制品恒 unknown BLOCK 且无豁免口。本批合并时若撞该 BLOCK，按该 ARQ 记载的既有实况处理并如实留痕，不私改闸。
4. **`docs/contracts/**` 的写入通道**——按 rules/contracts.md「落地恒归主线程 packet-bound」，本批契约三处由主线程以 Write/Edit 落地（Bash 重定向写恒拦，未使用）；作者 = architect（PROPOSAL 内逐字 diff），主线程只做逐字搬运，未起草任何契约内容。
5. **无降级、无允差、无未落项**。§6 留根清单（TECH-261014 注记义务 / 验收种子转移 / 261001 spine 回填 / 261016 合并序 / 卡壳阈值拍数 / 素材保全取舍）均为**有意的批外留根**，非漏落。

```landing-manifest
docs/architecture.md
docs/ADR.md
docs/contracts/interface-contract.md
```
