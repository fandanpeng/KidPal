# docs-delta —— 261015-voice-rightcol-label-overflow

> blocking closeout 制品。integrator 落笔（2026-10-15 制品轮）。本包 = 单包、rigor=lite、sourceType=regression、riskTier=medium：语音轮右列三 label（标头 header / 回显 echo / 问答 answer）宽度 264 旧版式遗留值收窄至 196——右列容器在 ui-v5-align 阶段3（commit 9f4c82a）收窄为 x=192、宽 196，但盒内 label 宽 264 是 260915-m1-k3 时代（右列 x=124 起、宽 264）的遗留值，`LV_LABEL_LONG_WRAP` 下按 264px 换行、容器视口仅 196 → 每行右端约 68px 被裁不可见（human 真机照片实证 TALK 态行尾 1~2 字被裁）。技术方案权威 = sibling `plan.md`（architect `/triage-change` 分流 review 2026-10-15，human 触点①确认 ②单包 Lite + 标头收进）。

## 文档面改动清单

**零文档面改动**（taskpackage `docsSyncRequired=false`，docsOwner = integrator 本件裁定）——docs-sync 闸 r1 scoped（mutation-check 元数据缺口红）→ r2 scoped 八闸全 PASS 承载。plan.md「需求/架构/契约/debt/governance 面：零触」逐条对账：

- `docs/requirements.md`（需求面）：**零 diff**。本包修复既有真机显示缺陷，不改任何需求语义；唯一显示行为变化（换行点从 264 前移到 196、行数变多、滚动更早出现）是 requirements §2.2 x192/y80 冻结锚 + VQ-A2（超屏高滚出）/ VQ-A3（长文滚动看完）滚动语义的**预期行为**——spec 站在修复一侧、未钉像素宽度，无新验收条目、无口径改写。spec 面漂移检出 = 无，无 architect spec 更新交接（`ARQ-260929-docs-sync-spec-handoff` 条款未触发；integrator 对两份 spec 文件无写权、本轮亦无写需求）。
- `docs/architecture.md`（架构面）：**零 diff**。单文件三处字面常量值改动（`lv_obj_set_width` 实参 264→196），零结构性变更、零新增模块、零跨层接线。
- `docs/contracts/**`（契约面）：**零 diff**（contract-lint 闸 r2 PASS）。纯板端 LVGL 显示版式修复，wire 协议零变更（crossEnd.exempt 已定：零 wire 面）；`docs/contracts/interface-contract.md` 仅作 `contractRefs` canary 引用（在册），零触碰。契约变更门不触发。
- `docs/debt/**` / governance 面：零触碰（detect_changes changed_files 中的 debt 条目 `ARQ-261015-launch-prep-host-only-env-defer` 为同分支先行 commit ad30de5 的**非本包**制品，与 launch-prep 相关、与本包无文档耦合）。

### 代码面 doc 钉行（随包在案、非本件义务）

- `firmware/main/display/lcd_display.cc`：三处改动点周边既有注释链（右列容器版式数学 400-192-12、全量字体选型注记）原样在世，本包零新增注释义务、零 doc 漂移。
- `firmware/main/test/voice_rightcol_label.acceptance.c`：文件头注释自承载根因 / seam contract / 齿形态说明（冻结件，acceptance-author 落笔），本件不复制其正文。

## 零改申报（其余面）

- `docs/crq/**`、`.claude/**`、`tools/**`：零触碰（`git status --porcelain` 实证零命中）。
- ADR：**零新增**（`adrRequired=false`；字面常量收窄非契约/跨系统边界裁决，不构成 ADR 事由）。
- `firmware/main/test/voice_round_ui.acceptance.c`（VR-A5 回归观察面）：零 diff（host 全量实跑保持绿，见 review-packet）。
- 264 旧值销账：`grep 264 firmware/main/display/lcd_display.cc` 仅余 2 处**注释**提及（:1011 / :1031，y262-264 底条上沿实线设计注记，与本改动无关）；`lv_obj_set_width` 可执行调用零 264 残留（VR-A4 反证齿在 host 全量实跑中绿承载）。

## Post-cutoff 制品纪律确认

本包 post-EVENTS_SCHEMA_CUTOFF（changeId 前缀 261015 >= 250725）：**不产** `session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl` 晋升；复盘叙事归 planner 收尾相位的 `pod-retro.md`。`delivery/taskpackages/261015-voice-rightcol-label-overflow/verify-runs.jsonl` 在场（verify.mjs dual-write 程序 fs-write，轮次史 = r1 scoped 红〔两次实跑，mutation-check 元数据缺口，planner 补条目后转机〕→ r2 scoped 八闸全绿）。`pod-events.jsonl` 在场（8 行；advisory 检查归 planner/supervisor、non-blocking，本件不阻塞 closeout）。机器证据投影 = `current/verdict.json`（advisory、non-bearing compatibility projection；承重机器件 = `verifier-evidence/rounds/r2.json`）。验收测试文件（本件引用的 `*.acceptance.c`）由验收件承载，本件不复制其正文。
