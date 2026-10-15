# Docs Delta · 260920-m2-charwindow-card-fix

> integrator 收拢制品（2026-09-20）。本件回答「本包是否需要在 canonical docs / spec 面留痕」；结论与逐面核对如下。

## 结论

- **对 TaskPackage 声明的 `docsSyncRequired=false`：在本包责任范围内成立**——语义权威（`docs/requirements.md` S-F3.2 五行口径）已在 v1.56（`a24a718`）+ 勘误（`5982d82`）独立落账，本包是**需求先行后的实现侧对账**（右栏速查卡与底部提示条字体接线 + meaning 五行兑现），**需求面零改动、本包只消费**。
- **零契约 / 零 spec / 零 agent-gate 改动（MCF-E2 铁律守住）**：`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md`、`.claude/**`（agent 定义 / gate）在本包内**逐面零触碰**（`git diff 5982d82..worktree` 零命中，见「核对方法」）。
- **`server/` 零改动**：本包全部落板端 `firmware/`；D3 笔顺 JSON 的 `meaning` 字段**既有**（本包未动契约），OpenClaw 生产端取词典 definition 已在下发（`server/deploy/openclaw-patch/char_stroke_lib.py` 既有链路，crossEnd exempt 在案）。

## 声明理由（引用 taskpackage docsSyncReason 逐字）

> spec v1.56 + 勘误已落账（a24a718/5982d82），本包只消费 S-F3.2 五行口径——零 docs 面改动。

## 根因与勘误链（docs-sync 关联事实，便于复盘考古）

- **右栏隐身根因**：`char_page.cc` 原第 555 行是**全树唯一**在中文 label（右栏速查卡 cardlabel）上显式钉 `LV_FONT_DEFAULT` 的调用（`LV_FONT_DEFAULT` = 内置 montserrat_14 纯拉丁字体，无汉字与带调韵母字形）——速查卡整栏在真机隐身；底部提示条 hint_label 则无显式字体（赌字体继承，同样不可靠）。
- **spec 曾误记、已勘误**：spec v1.56 对账④一度表述「全工程中文页面均显式加载 font_body」，实为**设计注释被当成已落地代码**——architect /triage-change 纠偏发现 B，勘误随 `5982d82` 落账。font_body 的运行时接线 = RP-A4 residual **后续包**，本包不并入。
- **本包接线选型**：`font_puhui_14_1`（全量 14px 普惠体，xiaozhi-fonts 组件定义）——沿用 `lcd_display.cc` 键帽条/语音轮**五处真机验证先例**，显式逐 label 接线（human 触点①确认）。

## 逐面核对

| 面 | 文件 | 本包动作 | 核对证据 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md`（D3 笔顺 JSON 含 meaning 字段） | 零触碰（只读） | contract-lint 闸绿（final r2，exit 0）；`git diff 5982d82..worktree -- docs/contracts/` 零命中。meaning 字段**既有**于 D3，本包零契约改动（MCF-E2） |
| spec 面·需求 | `docs/requirements.md` | 零触碰（本包只消费） | docs-sync 闸绿（final r2，`docsSyncRequired=false`）；`git diff 5982d82..worktree -- docs/requirements.md` 零命中。S-F3.2 AC1 五行口径（拼音带声调/共 N 画/部首/释义/组词）已由 v1.56（`a24a718`）+ 勘误（`5982d82`）冻结为现文——在 5982d82 之后本包启动，勘误已是本包 launch 基线的一部分 |
| spec 面·架构 | `docs/architecture.md` | 零触碰 | `git diff 5982d82..worktree -- docs/architecture.md` 零命中；MCF-E2 铁律在案。本包不触系统边界（同组件内既有字体的显式接线 + 既有装配函数 additive 扩展，不动渲染节点层/层仲裁架构） |
| docRefs | `docs/requirements.md` | 只读引用（S-F3.2 / F3 / §2.2） | 无需更新——需求行语义未变 |
| ADR | `docs/ADR.md` | 零触碰、无需新增 | 字体显式接线选型（font_puhui_14_1、designated initializer、meaning NULL 容忍）均为既有先例（lcd_display.cc 五处 / K3 件 fallback 宏）的沿用，**无新系统边界 / 事件 / tool / 契约取舍**，无接受残余风险或推迟迁移/安全事项 → 无 `adrRequired`（TaskPackage 亦未置）。详见 Review Packet「Five-Axis Review」第 3 轴与「Residual Risk」 |
| agent 定义 / gate | `.claude/**` | 零触碰 | `git diff 5982d82..worktree -- .claude/` 零命中 |

## 核对方法

- `git diff --numstat 5982d82 -- firmware/ server/ docs/ .claude/`：**仅 firmware 5 文件命中**（实现 3 + 验收冻结 2，+525/-11）；`server/`、`docs/**`、`.claude/` **全部零命中**（2026-09-20 integrator 收拢时实跑；5982d82 = 本包 launch 基线，即 v1.56 勘误 commit）。
- final r2（invocationId `31bd5da8-40f0-49c5-b3f3-95dd7adeb01f`，`ok:true` / `observedGateExit:0`，2026-09-20T05:24:19.355Z–05:24:34.831Z）contract-lint / docs-sync / taskpackage-required 三闸 exit 0；`verifier-evidence-check.mjs` integrator 实跑 `factOk:true`、exit 0。
- scoped r1（rounds/r1.json）同样一次绿——全生命周期两轮 verify 均一次通过，零返工轮。

## 实现侧注记承载位置（改动的 why 随代码注释承载，非 spec 面）

- **字体显式接线**：`firmware/main/display/char_page.cc`（`LV_FONT_DECLARE(font_puhui_14_1)` + 病灶行 cardlabel 改接 + hint_label 补显式接线「不赌字体继承」注）。
- **五行拼装**：`firmware/main/display/char_page.cc`（速查卡 snprintf 五行 + 「释义：—」缺义兜底 + `buf[256]→buf[384]` 设备构建闸截断上限注）。
- **meaning 数据面**：`firmware/main/display/char_stroke.h`（`KIDPAL_CHAR_STROKE_HAS_MEANING` 探针宏 + `kidpal_char_info.meaning` NULL 容忍语义注 + `kidpal_char_card.meaning[96]`）/ `.c`（`kidpal_char_card_build` NULL 容忍分支 + verbatim 截断安全拷贝注）。
- **designated initializer**：`firmware/main/display/char_page.cc`（refresh_hint 与 show 内两处 positional → designated，防成员漂移注）。
