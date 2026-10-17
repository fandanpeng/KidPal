# architect-landing-summary · 261017-arch-review-p3-story-body-sd-cache

packet-id：FROZEN PROPOSAL §2 改法集（T1–T6 + 契约包）+ §5 验证集 · 评审轮次 **R1 BLOCK(F1,F2) → R2 GREEN** · change-id 261017-arch-review-p3-story-body-sd-cache（跨面 spine = 261016-p3-story-body-sd-cache）

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 · 决策 24 史实更正 + 窄例外（§10 决策 24 句尾 append；§2.4 原则 5 句尾 append） | `docs/architecture.md` | 已落：两段逐字插入，原文一字未删 |
| T2 · 禁令其余重述点镜像四处（§2.1 :62 / §2.4 原则 1 :97 / §3.1 :115 / §4 :150） | `docs/architecture.md` | 已落：四处「零存储 / 不缓存任何事项」收窄为「零真相源存储 / 窄例外」 |
| T3 · 新增 ADR-011（不改 ADR-005/010 原文） | `docs/ADR.md` | 已落：ADR-011 全文逐字 append 至文末，含 supersedes 注记（只取代否决理由、明写决策本体仍在役） |
| T4 · 契约 §四新增 `kidpal_story_fetch` 上行私有帧 | `docs/contracts/interface-contract.md` | 已落：插在 `kidpal_story_delete` 与 `mcp` 工具调用之间，含「首个请求-应答环」据实声明 |
| T5 · 契约 §1.3④ `story_text` 消费语义注记扩展 | `docs/contracts/interface-contract.md` | 已落：插在 `seq` bullet 与既有契约变更记录之间，含「卡不在位 / 坏卡时不留存、当场显示」 |
| T6 · `architecture.md` §12 变更记录 changeId 行 append（定稿标记） | `docs/architecture.md` | 已落：新行置于 `261015-gov-review-changes-taxonomy` 之上，依据列不含 FROZEN 版本 token |
| 契约包 · 端云协议（三节） | —（PROPOSAL §2 承载） | 结构义务已满足；`pre-merge-escalation-gate.mjs` 契约分支不解析三节 |
| CDQ 登记（主线程账本动作，非 manifest 目标） | `docs/CONTRACT-DECISION-QUEUE.md` | 已落：`CDQ-261017-p3-story-body-sd-cache` 行 append，状态 `PRECHECKED`（待 landing-verdict PASS 翻 LANDED） |

## 2. 验证证据

1. **锚完整性** — `node .claude/scripts/spec-anchor-check.mjs` → `"design": "kidpal-ui-design-v6.html"`、`"designIdCount": 61`、`"errors": []`、`"ok": true`、exit 0。**绿**。
2. **PROPOSAL 结构门（preflight 干跑）** — `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261017-arch-review-p3-story-body-sd-cache/PROPOSAL.md` → `"ok": true`、`"findings": []`、anchorUnion = {docs/architecture.md, docs/ADR.md, docs/contracts/interface-contract.md} ≡ landing-manifest。**绿**。
3. **契约机械凭据闸（post-commit、合并前）** — `pre-merge-escalation-gate.mjs`：**landing 时尚未跑**，归合并门前置（见 §3 诚实注记 3）。
4. **逐字 diff 对账（post-commit，人读）** — **归 landing-verifier**：需逐文件核对实际落地 == FROZEN §2 各 T 的插入 / 替换段（T1 两段 insert、T2 四段 replace、T3 一段 ADR-011、T4 一段帧定义、T5 一段注记、T6 §12 行 append）。
5. **边界守持核（人读）** — 已在 FROZEN PROPOSAL 中由 architect-reviewer R1/R2 两轮核过：carve-out 只给 story 正文、显式排除任意业务数据；提醒 / 课表 / 日志不落卡；断网无提醒语义不变；首个请求-应答环声明在场且不沿用不精确引据。**落地后 text 逐字一致**（见 4，待 verifier 复核）。
6. **ADR-011 记账在场（人读）** — 已落：supersedes 注记指 ADR-005 备选① / ADR-010 备选②③，并注明「决策本体（`data.restore` additive 字段）不受影响、仍在役」。

## 3. 诚实注记

1. **本摘要 §2 的第 3、4 两条在写作时尚未执行**——它们按 SSOT 是 **post-commit / 合并前**动作。本摘要如实标明归属，不预支结论；对应结论以 `landing-verdict.md` 与合并门输出为准。
2. **CDQ 行不进 landing-manifest**——它是跨时间账本登记、非被 diff 的契约路径，其「loop 凭据」列依赖 landing-verdict（landing 后才有）。此裁定经 architect-reviewer R2 明确认可。
3. **合并闸对本批的适用范围存疑（在册 ARQ 已记）**——`pre-merge-escalation-gate.mjs` 的 kind 探测只认 `delivery/taskpackages/<id>` 与 `delivery/review/<id>` 两条 lane，而本批制品是 **intake spine + review PROPOSAL 双落点**；另在册条目 `ARQ-261017-merge-gate-scope-unstated`（另一会话 2026-10-17 登记）已记载该闸对 intake/debt 制品恒 unknown BLOCK 且无豁免口。**本批合并时若撞该 BLOCK，按该 ARQ 记载的既有实况处理并如实留痕，不私改闸。**
4. **`docs/contracts/**` 的写入通道**——按 `.claude/references/../rules/contracts.md`「落地恒归主线程 packet-bound」，本批契约两处由主线程以 Write/Edit 落地（Bash 重定向写恒拦，未使用）；作者 = architect（PROPOSAL 内逐字 diff），主线程只做逐字搬运，未起草任何契约内容。
5. **无降级、无允差、无未落项**。§6 留根清单四项（隐私面 deferred / 双机制债 / `session_id` 显示漂移 / LFN 代价）均为**有意的批外留根**，非漏落。

```landing-manifest
docs/architecture.md
docs/ADR.md
docs/contracts/interface-contract.md
```
