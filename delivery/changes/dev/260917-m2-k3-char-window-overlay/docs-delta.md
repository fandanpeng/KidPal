# Docs Delta · 260917-m2-k3-char-window-overlay

> integrator 收拢制品（2026-09-19）。本件回答「本包是否需要在 canonical docs / spec 面留痕」；结论与逐面核对如下。

## 结论

- **对 TaskPackage 声明的 `docsSyncRequired=false`：在本包责任范围内成立**——语义权威（`docs/requirements.md` F3 全节 + §2.2/§2.3/§2.4 + NFR-PERF-2）在 v1.36/v1.37/v1.39/v1.43/v1.55 已冻结查字窗口径，本包是**需求先行后的实现侧对账**（页→窗改造 + 右栏对账三项 + 锚改名对账），**需求面零改动**。
- **零契约 / 零 spec / 零 agent-gate 改动（M2K3-E2 铁律守住）**：`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md`、`.claude/**`（agent 定义 / gate）在本包内**逐面零触碰**（`git diff 210bb6e..worktree` 零命中，见「核对方法」）。
- **`server/` 零改动**：查字窗改造全部落板端 `firmware/`，服务端 `view=char` 下发语义与 display 响应块不动（E1 既有）。

## 声明理由（引用 taskpackage docsSyncReason 逐字）

> spec 查字窗口径已冻结（v1.36/v1.37/v1.39/v1.43/v1.55 现文即权威），本包为需求先行后的实现对账（F3 实现侧对账三项 + 锚改名 + 撤页改窗），零 docs 面改动。

## 逐面核对

| 面 | 文件 | 本包动作 | 核对证据 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md`（E1 display 响应块 / D3 笔顺 JSON / B1 课表 MD / shim API） | 零触碰（只读） | contract-lint 闸绿（final r4，exit 0）；`git diff 210bb6e..worktree -- docs/contracts/` 零命中；`architect-reviewer` 门 1 轻量 checklist 已对本批三包含 K3 裁定「零契约改动裁定成立」（裁决记录 = `delivery/workstreams/m2-char-window/review.md` 门 1 节）。注：契约 §1.3 ①「切到 P4 查字页」旧措辞属撤页前过时描述句（门 1 已附观察，不阻断）——契约字段与 display 响应块语义零改动，不构成契约变更 |
| spec 面·需求 | `docs/requirements.md` | 零触碰 | docs-sync 闸绿（final r4，`docsSyncRequired=false`）；`git diff 210bb6e..worktree -- docs/requirements.md` 零命中。F3 全节（S-F3.1 AC1–AC4 / S-F3.2 AC1 / S-F3.3 AC1–AC2）+ §2.2 查字窗行/提醒行 + NFR-PERF-2 已是本包目标的权威现文——实现向需求对账，需求无需改 |
| spec 面·架构 | `docs/architecture.md` | 零触碰 | `git diff 210bb6e..worktree -- docs/architecture.md` 零命中；E2 铁律在案。本包不触系统边界（查字窗层插入 + 键路由列扩 + 5 分钟计时是本仓既有 layer 骨架内的实现对账，不动四态骨架共享对象层或提醒/语音/查字三层仲裁架构——M2K3-E3 逃生门未触发） |
| docRefs | `docs/requirements.md` | 只读引用（F3 / §2.2 / §2.3 / §2.4 / NFR-PERF-2） | 无需更新——需求行语义未变 |
| ADR | `docs/ADR.md` | 零触碰、无需新增 | 层数值码 renumber（插入 CHAR_WINDOW + REMINDER/PROVISION 上移）与页→窗改造的架构裁决早在 requirements F3 各版本 / architecture 渲染节点层定稿，本包是既有裁决的落地对账，**无新系统边界 / 事件 / tool / 契约取舍**，无接受残余风险或推迟迁移/安全事项 → 无 `adrRequired`（TaskPackage 亦未置 `adrRequired`）。详见 Review Packet「Five-Axis Review」第 3 轴与「Residual Risk」 |
| agent 定义 / gate | `.claude/**` | 零触碰 | `git diff 210bb6e..worktree -- .claude/` 零命中 |

## 核对方法

- `git diff --stat 210bb6e -- docs/`、`-- docs/contracts/`、`-- docs/requirements.md`、`-- server/`、`-- .claude/`：**五面全部零命中**（2026-09-19 integrator 收拢时实跑；210bb6e = K3 launch 基线 = main tip）。
- final r4（invocationId `1c7a5463-162c-4e8b-9912-835cf48c9549`，`ok:true`/`observedGateExit:0`，本包自引入假陈述最小修正后重跑）contract-lint / docs-sync / taskpackage-required 三闸 exit 0；`verifier-evidence-check.mjs` 实跑 `factOk:true`。r3（`1221bff2-…`）已被 r4 取代。
- 契约面变更裁定另有门 1（architect-reviewer 轻量 checklist）GREEN 在案。

## 实现侧注记承载位置（改动的 why 随代码注释承载，非 spec 面）

- **层闭集 renumber**：`firmware/main/display/kidpal_layer.h`（常量块 + 头注现役权威改指 `char_window_overlay.acceptance.c` seam [1]）/ `kidpal_layer.c`（`is_stackable` 上界符号引用自动跟随 + transient 集纳入 CHAR_WINDOW 留痕注）。
- **键路由查字窗列**：`firmware/main/key_gesture_parser.h`（`GLOBAL_ACT_CHAR_REPLAY = 5` append-only 注 + `key_route_layer` 第三参语义扩为前台层归一化值）/ `.cc`（查字窗列实现块）。
- **设备侧装配**：`firmware/main/display/kidpal_render_node.h/.cc`（`begin/end_char_window` 夹钳、`front_is_overlay` 闭集纳 CHAR_WINDOW、5 分钟 tick 驱动 seam [9]、`char_window_key_hit` 归零点、两处 M2-K2 写侧死态 API 收拢）。
- **窗呈现面与锚改名**：`firmware/main/display/char_page.h/.cc`（锚 `char-window`、覆盖窗叠层不清屏、右栏四行对账、进度单源上屏、握笔小手占位删除）。
- **5 分钟计时纯逻辑**：`firmware/main/display/char_window.h/.c`（`KIDPAL_CHAR_WINDOW_IDLE_MS` / `kidpal_char_window_idle_step` seam [3]）。
- **板级消费链**：`firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc`（`keya_front_route_top` 前台层归一化 + `GLOBAL_ACT_CHAR_REPLAY` → `kidpal_char_page_replay` + 任意键 `char_window_key_hit`）。
- **通道路由对账**：`firmware/main/mcp_server.cc`（case 3 改经 `begin/end_char_window`，路由语义与调用点本体不动）。

## 已知残留（非本包触碰面，如实记录）

- **归档件数值字面 stale**：`firmware/main/test/acceptance/archive/screen_unification.acceptance.c:216` 的 `#define KIDPAL_LAYER_REMINDER 5` 与活跃 assert —— 该件在 renumber 后不准确，但**已实证不编入任何构建**（host_app 与设备 CMakeLists 均不含），属**非活耦合**；本包未硬改（归档件冻结不改）。`kidpal_layer.{h,c}` 头注已就地改指本包新冻结件，原指向归档件的「逐字一致」旧断言已清除（避免留假陈述）。
- **在世冻结件 stale 注释文本**：`firmware/main/test/reminder_overlay_unify.acceptance.c` 的 `:43-44/:86/:657/:680/:695` 残留 `REMINDER(5)` / `VOICE(3)` / `SERVER_PAGE(4)` 文本（仅注释与诊断字符串、不影响绿）；建议后续批统一 refresh，本包不动以免扩大触碰面。
- **在世冻结件同型裸正扫齿**：`key_layer_dispatch.acceptance.c` 与 `retract_p4_three_page.acceptance.c` 残留**结构同型**的裸正扫齿（正向 `file_contains` / `count_in_slice >= 1`，注释可满足）——本包未实证其假绿、按最小触碰不扩面；建议后续批统一引入注释剥离器补齐。

> 上述三项均为**登记项**（本件 + Review Packet），非本包可改，不阻断合并门。