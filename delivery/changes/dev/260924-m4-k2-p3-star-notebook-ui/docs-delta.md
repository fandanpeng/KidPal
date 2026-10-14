# Docs Delta · 260924-m4-k2-p3-star-notebook-ui

> integrator 收拢制品（2026-09-30）。本件回答「本包文档面义务是否已全清」，并承载 spec 面（`docs/requirements.md` / `docs/architecture.md`）的 **patch 形态 delta**（integrator scope-map allow 不含两 spec 文件，delta 在此成文、合并后由 human spec 批落地——K1 包 260924-m4-k1-story-cloud-link docs-delta §4 处置模板前置先例，同族 = `docs/crq/tech/ARQ-260918-spec-face-3page-reconcile.md`）。
> TaskPackage `docsSyncRequired=true`；`docRefs = [docs/requirements.md#F2, docs/requirements.md#§2.3, docs/architecture.md#§5.5.4, docs/ui/kidpal-ui-design-v5.html]`；`contractRefs = [interface-contract.md#§1.3③, #§1.3④, #§四]`；`docsOwner = integrator`。

## 1. 结论

- **契约面：本批零触碰**（消费契约现文冻结形状：K1 落地批 5e862d0 已把 story_list 语义拆分 / story_text seq / kidpal_story_delete 三触点写进 `docs/contracts/interface-contract.md` 现文）。`contract-lint` 闸绿（scoped r3 + final r4 均 exit 0）。
- **spec 面（`docs/requirements.md` / `docs/architecture.md`）：本批不可写 → patch 形态 delta 成文于本件 §4，合并后由 human spec 批落地**。gate evidence（`delivery/taskpackages/260924-m4-k1-story-cloud-link/architecture-gate-evidence.md` §6）C2 本批翻 done、C3 补「K2 半 done / K3 live 半 pending」处置注记（本包 integrator 已代落——该件为 CCP 跨包条件 SSOT，C1/C4 处置先例同路径）；spec 面落地前 gate 注记保持 pending 语义（C2 的 spec 联动仅对账行销账、无行为条款）。
- **spec 语义零改动**（对账销账 + 措辞对齐非语义变更；v1.26–v1.60 口径已冻结在前，代码追上）：F2 intro 对账四项 = v1.33 登记的 dev 修复项销账；§2.3 键位对账 = v1.39/v1.47 登记修复项的 P3 相关半销账；AC3 = 引文陈旧口径清扫（双指令措辞对齐，v1.26 已定口径）。
- **ADR 面：零改**。实现与 ADR-007（`docs/ADR.md:60`：seq 账本 / 16 条窗口丙案 / 删除单向 + 隐式 ack / 单源收紧）逐条一致，无新 ADR 实义。
- **workstream 面**：`delivery/workstreams/m4-star-notebook/progress.md` append 收口节（append-only）。

## 2. 文档面变更清单

| 面 | 文件 | 本包动作 | 性质 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md` | **零触碰**（只读消费 K1 冻结三触点；铁律 M4K2-E2） | 只读 |
| spec 面·需求 | `docs/requirements.md` | **本批不可写 → patch 形态 delta 本件 §4.2–§4.6**（F2 对账块四项销账 + §2.3 键位对账 P3 半销账 + AC3 清扫 + 版本行 + 变更记录行） | delta（待批） |
| spec 面·架构 | `docs/architecture.md` | **本批不可写 → patch 形态 delta 本件 §4.7–§4.8**（§5.3 页面组件行 + §5.5.4 改名对账收口） | delta（待批） |
| gate evidence | `delivery/taskpackages/260924-m4-k1-story-cloud-link/architecture-gate-evidence.md` §6 | C2 → done；C3 → K2 半 done 注记（跨包条件 SSOT 更新） | 已落 |
| ADR | `docs/ADR.md` | 零改（ADR-007 已在；本包实现与之一致） | 只读 |
| UI 设计面 | `docs/ui/kidpal-ui-design-v5.html` + `docs/ui/V5/` P3 五面 PNG | 零改（纯消费挂图，builder layout 常量按 PNG 定坐标） | 只读 |
| workstream | `delivery/workstreams/m4-star-notebook/progress.md` | append 收口节 | docs-sync 落点 |
| closeout 制品 | `delivery/changes/dev/260924-m4-k2-p3-star-notebook-ui/`（本件 + `review-packet.md`） | integrator 收拢制品落盘 | closeout |

## 3. docs-sync 机械判据落实

- 判据来源 = scoped/final 相位的 `docs-sync` 闸：TaskPackage 有 `docRefs` 且 `docsSyncRequired=true` → 走 `docsSyncReason` 分支（闸 stdout 逐字引 `docsSyncReason` 原文）。**scoped r3 与 final r4 `docs-sync` 闸均 exit 0**（`verifier-evidence/rounds/r3.json` / `r4.json`）。
- 语义层核对：`docsSyncReason` 声明的收口动作（requirements 三项 + arch 两处）= 本件 §4 的七个 hunk，一一对应，**无声明未做的项、无做而未声明的项**。

## 4. spec 面 patch 形态 delta（本批不可写两 spec 文件；合并后由 human spec 批逐字过目 → 落地）

> **结构性事实前置（如实）**：integrator scope-map allow = `["firmware/**","server/**","docs/systems/**","docs/templates/**","docs/ARCH-REVIEW-QUEUE.md","docs/crq/**","docs/ADR.md","delivery/changes/dev/**","delivery/taskpackages/**","delivery/workstreams/**"]`——不含 `docs/requirements.md` / `docs/architecture.md`（含已废弃 `docs/systems/**` 的不一致面已在 K1 review-packet Findings #1 登记，归 loop-retro/human）。
> **基线**：两文件现行文本 = 本 worktree 工作树态（2026-09-30，**已含** K1 C4 spec 批 c735062 落地后的 arch 文本——本 delta hunks 基于落地后现文，非 K1 件待落地态）。两文件实态行结尾若为 CRLF，`git apply` 本环境不可用（K1 C4 批实测先例），落地方式沿用 **node 字节级直写 apply + 落地前副本先验**。
> **落地通道**：合并后 human spec 批（可同批搭载 `ARQ-260918` 剩余处——如届时仍未落地）；gate evidence 面 C2 注记已先行落 `architecture-gate-evidence.md §6`。

### 4.1 delta 汇总表

| # | 文件:位置 | 动作 | 归属 |
|---|---|---|---|
| H1 | requirements `:7` 版本行 | v1.60 → v1.61 | 版本行随变更记录行同步（v1.60 先例形态） |
| H2 | requirements `:324` F2 intro ⚠️ 实现侧改名对账块 | 原文保留 + 追加【v1.61 对账更新】四项销账 | docsSyncReason「F2 intro 改名对账块四项销账」 |
| H3 | requirements `:167` §2.3 键位实现侧对账块（⑥ 行之后） | 追加【v1.61 对账更新 · P3 相关半销账】一行 | docsSyncReason「§2.3 键位对账③④⑥销账注记（P3 相关半）」+ 门 1 F7（⑤ P3 阅读态页内实例点名） |
| H4 | requirements `:410` S-F2.1 AC3 | 单指令引文 → 双指令对齐措辞（门 1 F1 清扫） | docsSyncReason「S-F2.1 AC3 引文陈旧口径清扫」 |
| H5 | requirements §9 变更记录表（v1.60 行之前插入） | 新增 v1.61 变更记录行 | v1.56「M2-K3 落地收账」先例形态 |
| H6 | architecture `:172` §5.3 页面组件行（story_page 现状半） | story_page 现状描述更新（四态 + 删除确认子态 + v1.47 输入表落地） | docsSyncReason「story_page 现状行更新」 |
| H7 | architecture `:290` §5.5.4 记录会话演出行（⚠️ 改名对账【修复项】） | 修复项销账收口 + CJK 字体接线同批注记 | docsSyncReason「§5.5.4 改名对账【修复项】收口」 |

### 4.2 H1 · requirements `:7` 版本行

```diff
@@ -7,1 +7,1 @@  头部版本行
-> 当前版本 **v1.60**；版本历史与逐次变更见文末 §9「变更记录」。
+> 当前版本 **v1.61**；版本历史与逐次变更见文末 §9「变更记录」。
```

### 4.3 H2 · requirements `:324` F2 intro 对账块（原文保留 + 尾部追加销账句）

```diff
@@ -324,1 +325,2 @@  F2 intro ⚠️ 实现侧改名对账
 > ⚠️ **实现侧改名对账（v1.33 登记，Phase 1 dev 修复项——需求先行钉口径，代码后改）**：活侧 `story_page.c` 盖章文案 `KSTORY_STAMP_FMT` 仍为「故事徽章·第 %d 颗已入册！」，应为「星星徽章」（v1.26 改名）；P3 列表仍渲染页名标题「我的故事」，应为页内无标题、星星计数即页身份（v1.27 口径）；常驻语音入口提示仍只有「我要讲故事」第一指令，应补「帮我记下来」（v1.26）；列表选中行仍反白，应为带边框（v1.31，真机效果差）。冻结验收 needle（`story_p3.acceptance.c` 逐字钉旧文案）须随文案一并更新。
+**【v1.61 对账更新】四项已由 M4-K2 落地收账（260924-m4-k2-p3-star-notebook-ui，2026-09-30）**：① 盖章文案改「星星徽章·第 %d 颗已入册！」（seq 在册带 N）/ seq 缺省降级「星星徽章已入册！」不带 N，N = 契约 `story_text.data.seq`（K1 持久账本，只增不减）；② 页名标题「我的故事」已删、页内无标题、星星计数 ×N 右上角裸排；③ 常驻语音入口双指令「说『我要讲故事』/『帮我记下来』开录新的」已并示；④ 选中行改带边框（反白退役）。冻结 needle 面 = 旧 `story_p3.acceptance.c` 已 archive 退役（`test/acceptance/archive/`），新冻结面 = `story_page_v2.acceptance.c`（13 齿，host 全量 128/0）。CJK 字体接线缺陷（全文件不设字体 → 中文真机空白）同批修复（全 label 显式 `font_puhui_14_1`）。
```

### 4.4 H3 · requirements `:167` §2.3 键位对账块（⑥ 行之后追加销账注记行）

⑥ 行原文（`:167`，保持不动）：

> ⑥ **v1.47 新增键位族全部待实现**：录音态 BOOT 丢弃（短/长/双击同义）+ KEY 长按改同短按保存、P3 列表 BOOT 长按删除确认子态、单双击 300ms 消歧（仅列表态）——随 Phase 1 键位改造一并接线。

```diff
@@ -167,1 +168,2 @@  §2.3 ⚠️ 键位实现侧对账块尾
 > ⑥ **v1.47 新增键位族全部待实现**：录音态 BOOT 丢弃（短/长/双击同义）+ KEY 长按改同短按保存、P3 列表 BOOT 长按删除确认子态、单双击 300ms 消歧（仅列表态）——随 Phase 1 键位改造一并接线。
+> **【v1.61 对账更新 · P3 相关半销账（M4-K2 落地收账，2026-09-30）】**：③ BOOT 双击已接线（P3 列表 = 跳屏落该屏首项、阅读 = 同短按翻屏；全局其余状态 BOOT 双击仍 = 同短按、无消歧负担）；④ P3 的 BOOT 短按族已接（列表光标下移 + 屏底翻屏落该屏首项 + 到底 wrap 永不离页、阅读翻屏末屏 wrap、BOOT 长按删除确认子态）；⑤ 的 **P3 阅读态页内实例「KEY 长按退出阅读回列表」已收口**（门 1 F7 补点名；全局「返回上一层」语义 M1 已接、全局半不动）；⑥ v1.47 键位族 **P3 相关半已落地**（列表 BOOT 长按删除确认子态 + 单双击 300ms 消歧仅列表态〔状态门控、其余态单击零延迟〕+ 录音态输入表语义位 KEY 长按 = 同短按保存 / BOOT 三手势 = 丢弃）——录音态**会话动作接线**（session_ctx 发送、引导态、收尾执行、stt 回显）归 M4-K3。①②⑤ 全局半 M1 已收、本包零触碰。
```

### 4.5 H4 · requirements `:410` S-F2.1 AC3（引文陈旧口径清扫，门 1 F1）

```diff
@@ -410,1 +410,1 @@  S-F2.1 AC3
-- AC3：Given 星星记录本任意时刻，When 孩子看屏，Then 常驻语音入口提示「说『我要讲故事』开录新的」。
+- AC3：Given 星星记录本任意时刻，When 孩子看屏，Then 常驻语音入口提示双指令「说『我要讲故事』/『帮我记下来』开录新的」。（v1.61 措辞对齐清扫：v1.26 已定双指令口径，原单指令引文为陈旧口径残留——与 F2 intro 对账③及设计稿 v5 张力同笔收口，措辞对齐非语义变更）
```

### 4.6 H5 · requirements §9 变更记录表新增 v1.61 行（插于 v1.60 行之前，表按新→旧排序）

```diff
@@ §9 变更记录（:686 前插入一行）
+| v1.61 | 2026-09-30 | **P3 星星记录本实现落地收账（M4-K2）**：① F2 intro ⚠️ 实现侧改名对账**四项销账**（盖章文案「星星徽章·第 N 颗已入册！」N=入册流水号 / 页名「我的故事」已删 / 常驻提示补「帮我记下来」成双指令 / 选中反白改带边框——见对账块【v1.61 对账更新】）；② §2.3 键位实现侧对账 **P3 相关半销账注记**（③ BOOT 双击接线 / ④ P3 BOOT 短按族 / ⑤ P3 阅读态页内实例「KEY 长按退出阅读回列表」收口〔门 1 F7 补点名〕/ ⑥ v1.47 键位族 P3 半落地、录音态会话动作归 M4-K3——见对账块【v1.61 对账更新】）；③ S-F2.1 AC3 引文陈旧口径清扫（单指令 → 双指令「说『我要讲故事』/『帮我记下来』开录新的」，措辞对齐非语义变更——门 1 F1）。实现件 = 260924-m4-k2-p3-star-notebook-ui（host 全量 128/0 + device build 11% 余量 + mutation 13/13）；spec 语义零改动。 | human spec 批（integrator docs-delta patch 形态先例，K1 C4 同族；合并后逐字过目落地） |
```

### 4.7 H6 · architecture `:172` §5.3 页面组件行（story_page 现状半更新）

```diff
@@ -172,1 +172,1 @@  §5.3 组件视图 · 页面组件
-| 页面组件 | `home_p1.c` / `schedule_page.c` / `story_page.c` / `char_page.cc` / `reminder_page.c` | 各页渲染与页内输入消费；story_page 含列表/阅读/录音/盖章四态；reminder_page 含设备侧四态（RECEIVED/QUEUED/RENDERED/ACKED） |
+| 页面组件 | `home_p1.c` / `schedule_page.c` / `story_page.c` / `char_page.cc` / `reminder_page.c` | 各页渲染与页内输入消费；story_page 含列表/阅读/录音/盖章四态 + 删除确认子态（v1.47 输入表 + P3 全手势族 + 300ms 消歧仅列表态 + kidpal_story_delete 发送接线，M4-K2 落地）；reminder_page 含设备侧四态（RECEIVED/QUEUED/RENDERED/ACKED） |
```

### 4.8 H7 · architecture `:290` §5.5.4 记录会话演出行（⚠️ 改名对账【修复项】销账收口）

```diff
@@ -290,1 +290,1 @@  §5.5.4 UI 演出与现状 · 记录会话演出
-- **记录会话演出**（`p3-recording`/`p3-stamp-frame`+`comp-stamp`）MVP 已实现验收；⚠️ 改名对账四项【修复项】：盖章文案「故事徽章→星星徽章」/ 页名「我的故事」应删 / 提示缺「帮我记下来」/ 选中反白应改边框（冻结验收 needle 随文案一并更新）；
+- **记录会话演出**（`p3-recording`/`p3-stamp-frame`+`comp-stamp`）MVP 已实现验收；改名对账四项**已收口（M4-K2 落地，2026-09-30）**：盖章文案 =「星星徽章·第 %d 颗已入册！」（N = 契约 story_text seq，K1 账本）/ 页名「我的故事」已删 / 提示双指令「我要讲故事」+「帮我记下来」/ 选中改带边框；CJK 字体接线修复同批落地（story_page.c 全 label 显式 `font_puhui_14_1`，原「全文件不设字体 → 中文真机空白」缺陷销账）；新冻结验收面 = `story_page_v2.acceptance.c`（旧 `story_p3.acceptance.c` archive 退役）；
```

### 4.9 落地方式与边界（如实）

- **本批不落地两 spec 文件**：integrator 无写权（§4 头注）。delta 以 patch 形态留此，合并后由 **human spec 批**逐字过目 → 落地（node 字节级直写 apply 先例——CRLF 文件 `git apply` 本环境不可用，K1 C4 批 c735062 实测）。
- **不改 `docs/contracts/**`**：恒只读（契约门通道专属）；本包消费 §1.3③④/§四冻结形状，`contract-lint` 绿。
- **gate evidence 联动**：本批已代落 `architecture-gate-evidence.md §6` C2 → done（证据 = 冻结齿 SPV2-A7 计数双口径 + SPV2-A8 seq 消费 + host 全量 128/0 + 收口 commit）、C3 追加「K2 半 done（发送原语 + 隐式 ack 重发接线）/ K3 live 半 pending」注记——先例 = K1 integrator 对 C1/C4 的同路径处置。
- **delta 可 apply 性自检**：各 hunk `-` 原文逐字取自本 worktree 两 spec 文件现行文本（requirements `:7`/`:324`/`:410`、architecture `:172`/`:290`，读取时点 2026-09-30，含 c735062 后状态），全行唯一单命中；H2/H3 为「原文保留 + 追加」、H1/H4/H6/H7 为整行替换、H5 为插行，互不重叠。


## 5. 逐面核对

| 面 | 核对证据 |
|---|---|
| 契约面零写入 | `git status --porcelain docs/contracts/` 零输出；`contract-lint` 闸绿（scoped r3 + final r4 exit 0） |
| spec 面未写（如实） | `git status --porcelain docs/requirements.md docs/architecture.md` 零输出；delta 成文于本件 §4（待 human spec 批） |
| gate evidence C2/C3 处置已落 | `architecture-gate-evidence.md §6`：C2 → done（本批 integrator 代落）；C3 → K2 半 done / K3 live 半 pending 注记 |
| ADR 零改 | `git status --porcelain docs/ADR.md` 零输出；ADR-007 在案、本包实现与之一致 |
| workstream append-only | progress.md 仅文件尾追加收口节，既有行逐字未动 |
| UI 设计面只读 | `docs/ui/V5/` P3 五面 PNG 与 `docs/ui/kidpal-ui-design-v5.html` 零写入（designRefs 纯消费挂图） |
