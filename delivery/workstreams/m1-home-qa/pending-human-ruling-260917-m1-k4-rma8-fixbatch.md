# pending-human-ruling · 260917-m1-k4-rma8-fixbatch（workstream m1-home-qa）

> 状态：**待人类裁决**。写入时点：2026-09-17，机械 supervisor 合并门 ESCALATE 之后、merge-driver 之前。planner 无 AskUserQuestion 通道（非交互 fail-closed），按纪律落盘停等，不静默推进。包 closeout 制品已齐（`delivery/changes/dev/260917-m1-k4-rma8-fixbatch/`）。

## 裁决事项 1（本包收口主门）· ESCALATED MERGE 批准与否

- 机械闸 verdict = **ESCALATE**（`delivery/changes/dev/260917-m1-k4-rma8-fixbatch/supervisor-verdict.json`，producedBy=supervisor-verdict-gate.mjs，write-once）。升裁原因 = declared riskTier=high（triage 既定：F2 共享骨架回归半径 + 状态机轴），非发现新缺陷；机械判据 `adjudicate:critical/high→ESCALATE(priority-before-evidence)`，diffTier 实为 low、evidenceComplete=true、final r4 九闸全绿。
- 人类若批准合并：planner 落 pod-events `human-decision` 事件（`payload.decision="approve-escalated-merge"`，eventId 即人批凭据）→ `node .claude/scripts/merge-driver.mjs --pod 260917-m1-k4-rma8-fixbatch --human-approval-ref <eventId>` 走标准合并链（merge token 串行化）。人批不豁免 off-main/契约/provenance，闸照拦。
- 人类若驳回：指示退回路径（fixer / 撤包 / 缩圈），board 已置 blocked。

## 裁决事项 2 · RM-A8 确认轮重跑排期（FX-A7，human 持板）

- 本包 merge 后，RM-A8 四项清单（弹窗演出 / 最高层覆盖 / 录音排队例外 / 模拟器截图缺口顺带补）须重跑确认——human 持板 + 主线程操作（烧录 df1c943 或合并后 main 的固件 / 串口 / journalctl 三方互证），沿用 device-round-1 取证形态。
- F2 串口判据已常驻：`voice_look` / `voice_look_skip reason=reminder_persist` 两条日志（taskpackage fixNotes.residentObservation）。

## 移交备忘（非裁决，主线程登记）

- **F0**（speaking 显示锁 60s+ 假死）：不在本包（FX-E3），走 ARQ 或独立批。
- **NIT-1**：`changes-index.mjs` dev 段重建把 k2 行状态判 `open`（board 实为 merged）——表观分歧待核；另该脚本无 `--help` 分支，裸 `--help` 触发全量重建。
- **NIT-2**：TEST 7 返回式断言链齿早停（同 TEST_CASE 先红即停，链式齿只验首齿）——test-quality 治理候选。
