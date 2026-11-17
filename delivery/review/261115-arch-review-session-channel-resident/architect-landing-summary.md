# architect-landing-summary · 261115-arch-review-session-channel-resident

packet-id：`261115-arch-review-session-channel-resident` 的 FROZEN PROPOSAL §2 改法集（T1–T7）+ §5 验证集 · 评审轮次 R1 BLOCK(F1,F2,F3) → v1.1 → R2 GREEN · change-id `261115-arch-review-session-channel-resident`

> **lane**：`packet-bound`（架构+契约面恒由主线程落地；无 author-implementation lane，故不附 `implementation-review.json` / `implementation-verification.json`，独立核验由 `landing-verdict` 承载）。
> **FROZEN 凭据**：human 2026-11-16 立项 ack + OQ1/OQ2/OQ3 三条裁 (a)；R1 四查 BLOCK(F1,F2,F3) → v1.1 修毕 → R2 靶向复核 GREEN；主线程自冻（PROPOSAL Status 行 = FROZEN v1.1）。
> **判档 / 四分类**：主路径 Track B；主 `ACCOMPANIES_SPEC_CHANGE` + 次 `SPEC_CONTRACT_DRIFT`；Track A = 否。

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 §6.4 整节重写（三层解耦 + 终止源闭集 + 心跳保活 + 自愈巡检 + 封顶长跑） | `docs/architecture.md` | 已落：§6.4 整节改写为「L1/L2/L3 三层 + 保活 + 巡检 + 长重试 + 引擎侧零改动 + RC-B6 观测前提变更」 |
| T2 §5.1.1 / §5.1.2 / §5.2（统一会话结束处理器 + front 每会话态 + 结束三通道） | `docs/architecture.md` | 已落：§5.1.1 新增「语音会话生命周期收口」行、会话状态机行 ⚠️ 对账改写、设备状态机行补边界承载；§5.1.2 front 行补生命周期锚与每会话收尾职责；§5.2 场景①补建连推送频率、场景②新增会话结束段与云端结束意图链、场景④ discard 时序改以 `listen stop` 为锚 |
| T3 §8.1 风险表 + §5.1.1 ⚠️ + §3.3 + §10 决策 29（账实同步） | `docs/architecture.md` | 已落：§8.1 风险 3 缓解句改写 + 风险 14 补注记 + 新增 15/16/17 三行；§3.3「设备常驻在线」行加沿革注记；§10 决策 29 追加「⚠️ 2026-11-16 取代注记（部分）」（正文一字不改，含 8/9 对账半句） |
| T4 §四 新增 `listen` 帧节 + `kidpal_keepalive` + discard 时序 + ctx 边沿 + 变更记录 | `docs/contracts/interface-contract.md` | 已落：§四 `abort` 条后新增 `listen` 与 `kidpal_keepalive` 两条；`kidpal_story_discard` 发送时机改以 `listen stop` 为锚；`kidpal_session_ctx` 升格会话开始边界并补边沿完整化；§四节尾追加变更记录条 |
| T5 §1.4 / §1.5 / §1.6（`end_session` 截获-自推 + prompt 纪律 + 变更记录） | `docs/contracts/interface-contract.md` | 已落：§1.4 表行改写 + 表后段拆「截获执行类 / 截获转发类」两型；§1.5 第 3 条补控制类分流规则；§1.6 新增告别回合 prompt 纪律；§1.4 节尾追加变更记录条 |
| T6 ADR-019（决策本体） | `docs/decisions/ADR.md` | 已落：append-only 追加 ADR-019（五件 + 会话边界注入四注入面评估附段） |
| T7 §12 变更记录追加 changeId 行 | `docs/architecture.md` | 已落：§12 表头下新增本批行（含 §3.3 / §5.1.1 / §5.1.2 / §5.2 / §6.4 / §8.1 / §10 / §12 改动面与依据） |

## 2. 验证证据

| # | 命令 | 实跑输出关键行 | 判 |
|---|---|---|---|
| V1 | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md` | `ok:true` · `section2BodyFound:true` · `anchorCount:3` · `anchorUnion` = arch / contract / ADR 三件 · `findings:[]` · `advisories:[]` | 绿 |
| V2 | `node .claude/scripts/spec-anchor-check.mjs` | `ok:true` · `errors:[]` · `archSectionCount:48`（与 pre 等值）· `anchorLines:44` | 绿 |
| V3 | `node .claude/scripts/contract-lint.mjs` | `ok:true` · `warnings` = 1 条既有归档链接断裂，未新增 | 绿 |
| V4 | `grep -c 'kidpal_keepalive' docs/contracts/interface-contract.md` | `2`（定义条 + 变更记录条） | 绿 |
| V5 | `grep -c 'listen` 状态消息' docs/contracts/interface-contract.md` | `2` | 绿 |
| V6 | `grep -c 'kidpal_keepalive' docs/architecture.md` | `2`（§6.4 两处） | 绿 |
| V7 | `grep -c '^## ADR-019' docs/decisions/ADR.md`；`grep -c '^## ADR-018'` | `1`；`1`（未误改既有条目） | 绿 |
| V8 | `grep -c '261115-arch-review-session-channel-resident' docs/architecture.md`；§12 行数 | `3`；`11`（pre 10 → post 11） | 绿 |
| V9 | `grep -c '结束会话，回首页常显'`；`grep -c 'WS 通道保持常驻'` | `0`；`2` | 绿（负向 + 正向配对） |
| V10 | `grep -c 'front 自推通路也不把这两个 action 发给设备'`；`grep -c '截获转发类'` | `0`；`2` | 绿 |
| V11 | `grep -c '设备关闭音频通道（断连）'`；`grep -c '结束信号'` | `0`；`2` | 绿 |
| V12 | `grep -c 'end_session' docs/contracts/interface-contract.md` | `8`（≥6） | 绿 |
| V13 | `grep -c '会话收尾本地关后 T=5s 常驻重建'`；`grep -c '常驻重建'` | `0`；`3` | 绿 |
| V14 | `landing-fidelity-check --cite-check <PROPOSAL>` | `ok:true` · `frozen:true` · `checkedCount:0` | **覆盖缺口如实申报**：工具只覆盖 §2 各 T 节内围栏块，本批承重原文位于 §2A；改用 `git diff -U0` 删除行与 §2A 三处承重原文逐字对账（见 §3） |
| V17 | `grep -c '重连 + 常驻重建已落（261010 包）'`；`grep -c '自愈巡检'` | `0`；`4` | 绿 |
| V18 | `grep -c '会话收尾本地关后短延迟（T=5s，kResidentRebuildDelayS，已冻结进齿）常驻重建'`；`grep -c '取代注记'`；`grep -c '2026-11-16 取代注记'` | `1`（正文一字不改）；`6`；`1` | 绿（**预期覆写**：取代注记 post 原写 5，实跑 6，§12 行自述命中未计入预期） |
| V19 | `grep -c '常驻重建' docs/architecture.md` | `3` | **预期覆写**：原写 post ≥4（假设 §6.4 保留该措辞），实跑 3（§3.3 / §8.1 / §10）——以实跑为准，已在 PROPOSAL §5 V19 行与 §7 落地实跑记录留痕 |

## 3. 诚实注记

- **V14 覆盖缺口**：`--cite-check` 的抽取面 = §2 各 T 节内围栏块（工具自述 `checkedCount:0`、`blockquote:false`）；本批承重原文全部位于 `## 2A`，不在覆盖面。替代对账已做：`git diff -U0 docs/contracts/interface-contract.md` 的三条删除行与 §2A 承重原文逐字一致（① §1.4 表行、② 表后网关决定类段、③ `kidpal_story_discard` 发送时机句），改后文本命中 V9/V10/V11 正向断言。**这不是绿的自报，而是工具覆盖面缺口的如实申报。**
- **两处 V 预期覆写**（arch-change「以落地实跑为准覆写 §5 并在 §7 留痕」）：V18 正向配对 = 6（原 5）；V19 = 3（原 ≥4）。均在 PROPOSAL §5 与 §7 落笔。
- **未落项（按 manifest 边界，非遗漏）**：`server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` 与 workspace `AGENTS.md` 的告别回合纪律属实现包 P2 部署面；`docs/requirements.md:48` ⚠️ 对账注记按 PROPOSAL §6 留根 1 由主线程另行路由；`firmware/main/protocols/protocol.cc:129-136` 与 `application.h:126-130` 的注释漂移按留根 8 交 P1 planner 扩单通道。
- **契约 diff 范围**：`git diff --stat` = 3 files / 55 insertions / 15 deletions；既有帧形状零改动、display 白名单仍 9、`kidpal_*` 上行帧字段零改动、引擎侧零 diff。
- **与 FROZEN 包的关系**：本 summary 只汇总落地与实跑；独立核验归 landing-verifier 的 `landing-verdict.md`，合并放行归 `pre-merge-escalation-gate.mjs` 三件联立凭据。

## 4. v1.2 增量落地（门 1 F-01 回修 · 2026-11-16）

- **依据**：FROZEN PROPOSAL **v1.2**（R3 增量复核：F-01 **CLOSED** / BLOCK(F-04) → v1.2 补正 → R4 一句话闭合 **GREEN（F-04 闭合）**）。
- **canonical delta（`docs/architecture.md` 五处）**：
  1. §5.1.2 front 行「每语音会话收尾」改为**按 `acc.routing` 分层**：`chat` 不动作 / `accumulate` 归档优先 / **`pending_stop` 受保护、禁止当刻 cancel 与释放、交 10 秒巡检**（巡检 pending 分支补 `_release_record_context`）/ 防御性行不 cancel；原「残余累积态取消」撤销。
  2. §5.1.2 同段 `kidpal_session_ctx` 边沿完整化改**分态**（`pending_stop` 不动作 / `accumulate` cancel+释放 / `chat` 标记真只释放）。
  3. §5.2 场景② 括注改为「按累积器路由态分层释放；`pending_stop` 受保护、交 10 秒巡检释放，不在 stop 当刻取消」。
  4. §8.1 新增风险行 **18**（记录会话空累积结束后 ≤10 秒的引擎抑制窗 = accepted residual；不得用提前释放消除）。
  5. §12 既有行「改动面」列追加 F-01 回修子句，并把 §8.1 计数由「新增 15/16/17 三行」改为「**新增 15/16/17/18 四行**」（不新增行、不改 changeId）。
- **canonical delta（`docs/decisions/ADR.md` 一处）**：ADR-019「关联」件计数 `新增 15/16/17` → **`新增 15/16/17/18 四行`**（与 §8.1 / §12 三处同数原子落）。
- **契约面零改动**：`proposedContractDiff` 四处一字未改、`landing-manifest` 三件不变、V4/V5/V9–V12 预期值不变（依据：已落地契约现文 `grep -c 'pending_stop|记录语境释放|残余累积'` = 0；ADR-019 裁定六条无一条钉释放时点）。
- **机械门（delta 落地后实跑）**：`landing-fidelity-check --preflight` = `ok:true` / `anchorCount:3` / `findings:[]` / `advisories:[]`；`--cite-check` = `ok:true` / `frozen:true`；`spec-anchor-check` = `ok:true` / `errors:[]` / `archSectionCount:48`；`contract-lint` = `ok:true`（唯一既有 warning 未新增）；三处计数 `grep -rn '15/16/17' docs/` 仅 arch §12 与 ADR 两文件、均「四行」。

```landing-manifest
docs/architecture.md
docs/contracts/interface-contract.md
docs/decisions/ADR.md
```