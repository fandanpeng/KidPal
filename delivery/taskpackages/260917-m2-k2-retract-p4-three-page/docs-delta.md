# Docs Delta · 260917-m2-k2-retract-p4-three-page

> integrator 收拢制品（2026-09-18）。本件回答「本包是否需要在 canonical docs / spec 面留痕」；结论与逐面核对如下。

## 结论

- **对 TaskPackage 声明的 `docsSyncRequired=false`：在本包责任范围内成立**——语义权威（`docs/requirements.md`）在 v1.36 已冻结撤页语义，本包是需求先行后的实现对账，**需求面零改动**。
- **但独立核对发现一处 architecture 面 stale（须主线程 spec 批 reconcile，非本包可改）**：`docs/architecture.md` §5.1.1 渲染节点层（行 170）现文仍写「页循环枚举 `HOME→CHAR→SCHEDULE→STORY`（…撤 P4 后改 3 页【修复项】）」，本包落地后该描述与真实现（`HOME→SCHEDULE→STORY`）不再相符；同文行 381 to-be ④「撤 P4 常驻页 → 3 页循环 + char-window 覆盖窗」之撤页半亦已达成。**本包 E2 铁律明禁改 spec（`docs/requirements.md` + `docs/architecture.md`），且该文历史沿革均为主线程 spec/gov 批 commit（`git log -- docs/architecture.md` 实证无 dev 包提交）——故不在本包就地改，如实登记、交主线程 spec 批收回【修复项】标记。** 详见 Review Packet「Findings」与「Residual Risk」。

## 声明理由（引用 taskpackage docsSyncReason 逐字）

> spec 撤页语义已冻结（requirements v1.36/v1.39 现文即权威），本包为需求先行后的实现对账（行 186 + 行 166 ②），零 docs 面改动。

## 逐面核对

| 面 | 文件 | 本包动作 | 核对证据 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md`（E1 display 响应块 / D3 笔顺 JSON 等） | 零触碰（只读） | contract-lint 闸绿（final r2，exit 0）；`git diff 01f8b97..HEAD -- docs/contracts/` 零命中；E2 铁律在案。注：契约行 63 有「切到 P4 查字页」旧措辞，但该行属契约正文的**板子行为描述**，撤页只改「翻页可停靠页位」、契约字段与 display 响应块语义零改动（`view=char` 路由仍走同一块），故不构成契约变更（其余契约引用见下「观察」） |
| spec 面·需求 | `docs/requirements.md` | 零触碰 | docs-sync 闸绿（final r2，`docsSyncRequired=false`）；`git diff 01f8b97..HEAD -- docs/requirements.md` 零命中；行 69「中槽页点仅 P1–P3 主页面显示（v1.36 页循环 4→3，页点随之 4→3）」+ 行 186「P4 查字页已撤销（2026-09-14 v1.36）」已是本包目标的权威现文——实现向需求对账，需求无需改 |
| spec 面·架构 | `docs/architecture.md` | **零触碰（E2 禁触），但发现 stale 一处** | 行 170（§5.1.1 渲染节点层）【修复项】标记 + 行 381 to-be ④ 的撤页半随本包达成后不再准确；本包无权改（E2 + 历史沿革全为 spec/gov 批）→ 登记交主线程 spec 批（见上「结论」与 Review Packet Findings） |
| docRefs | `docs/requirements.md` | 只读引用（§2.4 行 186 / §2.2 键帽条行 / §2.3 行 166 ②） | 无需更新——需求行语义未变；行 166 ②「页序漂移…撤 P4 改 3 页时一并理顺」的「一并理顺」义务恰由本包履行（该行属 v1.39 立的**实现侧对账台账**，非 spec 语义，其消解标记可随主线程 spec 批同批收） |
| ADR | `docs/ADR.md` | 零触碰、无需新增 | 撤 P4 改窗的架构裁决早在 requirements v1.36 / architecture §5.1.1 定稿，本包是既有裁决的落地对账，无新系统边界/事件/tool/契约取舍 → 无 `adrRequired`（TaskPackage 亦未置 `adrRequired`） |

## 核对方法

- `git diff --stat 01f8b97 HEAD -- docs/` 与 `-- docs/contracts/`、`-- docs/requirements.md`：除包外 gov 提交 `13cd811`（ARQ 登记，纯 append 1 行、与本包无关）外零命中（2026-09-18 integrator 收拢时实跑）。
- `grep -n "P4\|页循环\|3 页\|查字" docs/architecture.md` → 命中行 170 / 381 / 404（404 自注「本条为历史口径，现状见 §5.1.1」→ 定 §5.1.1 行 170 为现状权威，故其 stale 须收）。
- final r2（invocationId `97a5acfc-ad34-45db-b9a3-8f2d2475e983`）contract-lint / docs-sync / taskpackage-required 三闸 exit 0。

## 实现侧注记承载位置（改动的 why 随代码注释承载，非 spec 面）

- 页枚举重排与 CHAR 撤出：`firmware/main/display/kidpal_render_node.h`（枚举定义块）。同文件 `kidpal_render_node_cache_char` / `kidpal_render_node_cache_char_data` 声明处已加写侧死态注记。
- 键路由页序与 `PAGE_P4` 撤值：`firmware/main/key_gesture_parser.h`；`key_page_next` 回绕基数注记在同文件 / `.cc`。
- 键帽条页槽表与页点同源：`firmware/main/display/keybar.h` / `keybar.c`（页槽表注释 + `default: return -1` 完整性注释）。
- 设备侧页索引映射塌恒等：`firmware/main/display/lcd_display.cc`（`kidpal_keybar_sync_locked` 映射注释）。
- 查字路由保留说明：`firmware/main/mcp_server.cc`（case 3 区注释，路由语义零改动）。