# Docs Delta · 260928-story-connect-push-defer

> integrator 收拢制品（2026-09-28，Lite 体量）。本件回答「本包文档面义务是否已全清」，并承载核心义务：**K1 docs-delta §4 H1「WS 建连」三时点措辞的 supersession 注记 + arch §5.2④ 修订后建议措辞（patch 形态，供 C4 human spec 批搭载替换）**。
> TaskPackage `docsSyncRequired=true`；`docRefs = [docs/architecture.md#§5.2]`；`contractRefs = [interface-contract.md#§1.3③]`；`docsOwner = integrator`；`rigor=lite`。

## 1. 结论

- **契约面：零触碰**。本包仅摘除建连即推**调用点**一处（`server/gateway/front/front_app.py` ws_gateway 连接处理段），`_push_story_list` 函数本体、②（归档后）③（删除后）调用点与 wire 形状零变化；`kidpal_story_delete` / `kidpal_session_ctx` / `story_list` 形状契约条目全部不在本包改动面。`contract-lint` 闸绿（scoped r3 exit 0）。
- **需求面（`docs/requirements.md`）：零改**。行为权威 = F2 既有冻结口径（不变），本包只消费。
- **架构面（`docs/architecture.md`）：本批不可写 → 本件 §4 给出 arch §5.2④ 的修订后建议措辞（patch 形态）**，与 K1 件 H1 supersession 关系一并申报，合并后随 C4 human spec 批落地（以本包修订版为准）。
- **K1 历史制品：不改写**（铁律）。`delivery/changes/dev/260924-m4-k1-story-cloud-link/docs-delta.md` 与 `review-packet.md` 逐字未动；supersession 为**双向注记**：本包申报「K1 件 H1（及 H2 连带短语）自本包落地起过时、以本包 §4 修订措辞为准」，K1 件本体不动。
- **ADR 面：零改**。本包 = 行为策略（推送时机）修订，不进契约（Q10 既有裁定），无新架构决策（摘除方向 = human 方向锚 `b036f554`，记录于本包 taskpackage / plan / 冻结件 REVISION LOG，不升 ADR）。
- **workstream 面：`delivery/workstreams/m4-star-notebook/progress.md` append 本包收口节**（append-only）。

## 2. 文档面变更清单

| 面 | 文件 | 本包动作 | 性质 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md`（`contractRefs#§1.3③`） | **零触碰**（wire 形状零变化；只读消费） | 只读 |
| spec 面·需求 | `docs/requirements.md` | 零触碰 | 只读 |
| spec 面·架构 | `docs/architecture.md`（`docRefs#§5.2`） | **本批不可写 → 修订后建议措辞成文于本件 §4**（随 C4 批落地） | delta（待批） |
| K1 历史制品 | `delivery/changes/dev/260924-m4-k1-story-cloud-link/docs-delta.md`（+ 同包 review-packet） | **不改写**；supersession 双向注记落本件 §3（K1 件逐字未动） | 只读 + 注记 |
| ADR | `docs/ADR.md` | 零改（行为策略修订，无新决策） | 只读 |
| workstream | `delivery/workstreams/m4-star-notebook/progress.md` | append 本包收口节 | docs-sync 落点 |
| closeout 制品 | `delivery/changes/dev/260928-story-connect-push-defer/`（本件 + `review-packet.md`） | integrator 收拢制品落盘 | closeout |

## 3. K1 docs-delta §4 H1 supersession 注记（核心义务）

> **性质**：K1 件（`delivery/changes/dev/260924-m4-k1-story-cloud-link/docs-delta.md`，commit `5ac633a` 落盘）是「待 human spec 批」的 patch 形态 delta，**本包不修改它**；本节为生效关系申报——C4 human spec 批落地时，arch §5.2④ 的措辞**以本件 §4 修订版为准**。

### 3.1 被本包 supersede 的 K1 文本（两处）

1. **K1 docs-delta §4.2 H1（arch §5.2④ 新文）内句**（`delivery/changes/dev/260924-m4-k1-story-cloud-link/docs-delta.md:54`，逐字）：

   > **story_list = front 机械 producer 三时点推送**（WS 建连 / 归档成功后 / 删除执行后；`items` = 最新 ≤16 条窗口、`total` = 全量现存篇数，二者独立、`items.length` 不顶替 `total`）——推送触发属行为策略（不进契约）；

   过时原因：本包摘除「WS 建连」时点——真机拓扑下设备每会话重开 WS（会话结束关 audio channel、按 KEY 重开新 WS），建连即推 = 每次按 KEY 给设备打 story_list 帧，旧固件无条件渲染 P3 页盖掉聆听态（story_page 零 CJK → 半截页）。**三时点语义修订为「现役两时点 + ①时机归 K3 重裁」**。

2. **K1 docs-delta §4.3 H2（§5.1.2 front 网关行新文）内短语**（连带，同件 `:57` diff 行内）：

   > `story_list` 三时点自推〔建连/归档后/删除后〕

   同步过时：修订为「`story_list` 两时点自推〔归档后/删除后〕（建连即推已摘除，260928-story-connect-push-defer；①时机归 M4-K3）」。

### 3.2 连带（K1 review-packet 未覆盖面申报第 2 条）

K1 `review-packet.md`「未覆盖面申报」第 2 条「WS 建连时点（三时点①）真建连面」（性质：真 ws 建连 offline 不跑；行为等价假设②「ws_gateway 调 `_push_story_list` ≙ 建连即推」显式化、去向 M4-K3 live 复证）——**该条所申报的假设已被本包证伪并处置**：真实拓扑下「建连」= 每会话而非开机一次，假设②不成立；处置 = 摘除建连即推（FIX-A1 负锚齿承载）+ ①时机归 K3 重裁（`crossPackageBehaviors` 承接条在案）。K1 review-packet 不改写，本包申报该条闭环。

### 3.3 对 K3 的消费指引（`crossPackageBehaviors` 承接条对偶）

①的恢复时机归 M4-K3：设备能表达「会话发起自 P3」（`kidpal_session_ctx` 设备半）后，front 在正确语境下推列表。**注意**：session_ctx 触发的是记录会话开始，直接在彼时推列表会顶掉录音引导态——时机需随 K3 一并设计（本包 plan.md 已留警示，本件重申为 docs 面交接锚）。

## 4. arch §5.2④ 修订后建议措辞（patch 形态，供 C4 human spec 批搭载）

> **用法**：C4 批落地 K1 件 H1 时，**先按本节替换句**再 apply（即 K1 H1 hunk 的新文以本节修订句为准）。基线 = K1 件 H1 新文（本件 §3.1 引文上下文）；文件行结尾以工作树实态生成（CCP 落地通道纪律）。

### 4.1 替换句（替换 K1 H1 新文中的「三时点推送」句，其余 H1 文本不动）

```diff
@@ K1 docs-delta §4.2 H1 新文（arch §5.2④）内句替换 @@
- **story_list = front 机械 producer 三时点推送**（WS 建连 / 归档成功后 / 删除执行后；`items` = 最新 ≤16 条窗口、`total` = 全量现存篇数，二者独立、`items.length` 不顶替 `total`）——推送触发属行为策略（不进契约）；
+ **story_list = front 机械 producer 推送（现役两时点）**（归档成功后 / 删除执行后；「WS 建连即推」已摘除——260928-story-connect-push-defer：真机拓扑下设备每会话重开 WS，建连即推会在每次按 KEY 时给设备打 story_list 帧、旧固件无条件渲染 P3 页盖掉聆听态〔story_page 零 CJK → 半截页〕；原时点①的恢复时机〔设备进 P3 语境推送 / RAM 缓存填充〕归 M4-K3 `kidpal_session_ctx` 设备半落地后重裁；`items` = 最新 ≤16 条窗口、`total` = 全量现存篇数，二者独立、`items.length` 不顶替 `total`）——推送触发属行为策略（不进契约）；
```

### 4.2 连带替换句（替换 K1 H2 新文中的「三时点自推」短语，其余 H2 文本不动）

```diff
@@ K1 docs-delta §4.3 H2 新文（§5.1.2 front 网关行）内短语替换 @@
- `story_list` 三时点自推〔建连/归档后/删除后〕
+ `story_list` 两时点自推〔归档后/删除后〕（建连即推已摘除、①时机归 M4-K3，260928-story-connect-push-defer）
```

### 4.3 可 apply 性与边界（如实）

- 两处替换句的 `-` 侧 = K1 docs-delta **新文**（非 `docs/architecture.md` 现文——`docs/architecture.md` 尚未落地任何一方），故本节 patch 的应用顺序 = **先本节替换 K1 件新文 → 再按 K1 H1/H2 hunk 对 `docs/architecture.md` apply**（C4 批单步完成，human 过目）。
- `docs/architecture.md` / `docs/requirements.md` / `docs/contracts/**` 本批零写入（`git status --porcelain` 三者零输出）；K1 件不改写（`git status --porcelain delivery/changes/dev/260924-m4-k1-story-cloud-link/` 零输出）。

## 5. 逐面核对

| 面 | 核对证据 |
|---|---|
| 契约面零写入 | `git status --porcelain docs/contracts/` 零输出；`contract-lint` 闸绿（scoped r3 exit 0） |
| 需求面零写入 | `git status --porcelain docs/requirements.md` 零输出 |
| 架构面未写（如实） | `git status --porcelain docs/architecture.md` 零输出；修订措辞成文本件 §4（随 C4 批落地） |
| K1 历史制品不改写 | `git status --porcelain delivery/changes/dev/260924-m4-k1-story-cloud-link/` 零输出；supersession 双向注记落本件 §3 |
| ADR 零改 | `git status --porcelain docs/ADR.md` 零输出 |
| workstream append-only | progress.md 仅文件尾追加本包收口节，既有行逐字未动 |