# 真机轮 C 在线证据对账（2026-11-18，planner 只读取证）

> 采集者 = planner（轮 C 修复轮 intake）。原始输出见 `device-evidence/round-c/`。
> 本文件 = 对账与结论，不是真机轮 C 的完整证据；真机轮 C 仍由 human 持板跑（清单见 review-packet）。
> 效力：本文件仅用于**修复轮 intake 的事实锚定**与新增冻结验收的回引；不构成「真机轮 C 已过」的证据。

## 一、human 口述事实 × 三源对账（逐条一致）

| # | human 口述 | 证据 | 结论 |
|---|---|---|---|
| 1 | 21:20:41 起记录会话、全程无人说话 | front `story_record_ctx_enter src=ctx_story` @21:20:41；引擎 21:20:23/26 两句为**上一会话**闲聊（front 无对应记录语境） | 一致 |
| 2 | 21:22:05 设备正常触发 `story_empty_session_timeout … quiet=60 src=empty_story`（设备日志） | 设备日志由 human 持板读取（KPLOG，未随本目录落盘）；front 侧同刻 `story_stop archived=false reason=pending_stop` + `session_end_boundary routing=pending_stop action=protected` | 一致（设备侧日志未复拉，归属 human 持板证据） |
| 3 | 引擎静音 ASR 幻觉 `识别文本: Okay.` | `engine-asr-2120-2123.txt`：`261118 21:22:06 … 识别文本: Okay.` + 同刻 `kidpal record-context: 抑制 LLM 派发（记录语境闭嘴回显）: Okay.` | 一致 |
| 4 | `Okay.` 迟到转写喂进 pending_stop 累积器 | front 21:22:05 落 pending_stop → 3.9s 后 21:22:09 `story_organize_round ok=true`：只有收到 stt 才会进入整理回合（`_story_feed_from_stt` pending_stop 分支） | 一致（间接但唯一路径） |
| 5 | `empty_material` 守卫在整理前查原始素材，`Okay.` 非空放行 | 代码：`_archive_material_reject_reason` 归一化 `okay` 不在 `_FRONT_ARCHIVE_HALLUCINATIONS`；`len(compact)=4 ≥ 2` → 放行 | 一致 |
| 6 | 整理 LLM 返回「（没有可整理的内容）」被当正文归档 | `ecs-story-2026-11-18-s33477cee.md` 标题与正文逐字为「（没有可整理的内容）」，mtime 21:22 | 一致 |

## 二、控制组（同夜 A5 正常路径，证明缺陷面只在空素材支）

- 21:23:27 第二个记录会话（真实素材）→ 引擎 21:23:33/41 两句真转写 + 21:23:49 `说完了。`（收尾词）+ 21:23:50 尾音幻觉 `嗯。` → front 21:23:52 `archived=true id=2026-11-18-sbe0bff90`，MD 正文为真实整理结果。
- 说明：既有 `insignificant_material` 守卫对「嗯。」有效（compact 长度 1 < 2）；缺口只在**长度足够但无语义的英文静音幻觉**（`Okay.`）与**整理回合占位产物**两层。

## 三、缺陷面归纳（本轮修复输入）

1. **主缺口（排版后守卫缺失）**：`_organize_and_archive` 只判 `lines` 为空；整理 LLM 的占位/拒绝式回复（「（没有可整理的内容）」类）是合法非空行 → 被 `archive_story` 归档。
2. **次缺口（素材侧噪声）**：静音幻觉 `Okay.` 归一化后长度 4、不在幻听集合 → 放行进整理；若被过滤则 pending_stop 10s 后由 `_story_timeout_sweep_body` 释放（既有 SCRF-B10 口径，零归档零演出）。
3. **既有保护正确**：pending_stop 分层保护（`action=protected`）、空累积挂起、尾音短幻觉拒绝——本轮不得回退这些口径。

## 四、不可由本目录替代的事项

- 设备固件侧 `listening_diag` / `story_empty_session_timeout` 行（轮 B 修复固件的真机承载）需 human 持板复拉。
- 真机轮 C 完整清单（A6 / A7 / D3 空素材 / A5 回归 / 引导播报回归）见 review-packet「真机轮 C 复测清单」节；本轮离线修复**不得**申报真机 PASS。