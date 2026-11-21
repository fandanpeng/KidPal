# 真机轮 A 证据（2026-11-17）· 261116-channel-resident-core

> **性质**：真机轮 A = human 持板 + P1 固件烧板 + P1 front 部署 ECS 的**诊断轮**。结果 **FAILED**（三缺陷），本文件是缺陷取证与复测输入，**不是** `device-round-evidence.md`（后者要求逐 acceptanceRef PASS + 目击固件 `sourceCommit`/`binarySha256`；本轮不放行合并，故不生成、不伪造 PASS）。
> **三源**：设备 KPLOG / front systemd journal / 引擎 docker logs。最小脱敏摘录与 SHA-256：

| 源 | 摘录 | SHA-256 |
|---|---|---|
| device KPLOG（`/c/tmp/roundA-full.log`，含 `KPLOG000b.TXT` 并窗） | `device-evidence/round-a/kplog-excerpt.txt` | `32e034e2c27e7b90bcce780cbcde4b25ccb3598c30d9d6dcea06128885b08151` |
| front journal（ECS `journalctl -u kidpal-front`） | `device-evidence/round-a/front-journal-excerpt.txt` | `4fd8acd182e4b90efc417e70269df04d536ab3d76b69627c7c7fc9fdd4ecce45` |
| engine docker（ECS `docker logs xiaozhi-esp32-server`） | `device-evidence/round-a/engine-docker-excerpt.txt` | `1465aef458c258b0311360dfe6561d4c0fb82157b7464a8d2fe42645fe7c2649` |

- 观察窗：2026-11-17 22:24–22:53 CST（缺陷集中段 22:46:30–22:53:30）。
- 目击固件 commit / 镜像 SHA-256：**未采集**（human 未交付；不补造）。轮 B 复测须由 human 交付 `sha256(xiaozhi.bin)` 与烧板来源 commit，才可生成 `device-round-evidence.md`。

## 一、D2 · 丢弃/会话关闭不掐断在途回合（已确证，最重）

**三源时序（22:46:49–22:46:53）**：

| 时刻 | 源 | 证据 |
|---|---|---|
| 22:46:49 | device | `story_key discard_rec cancel_count=1` → `local_close_settle src=discard_record state=session_end_requested` → `listening -> idle` → `local_close_settle src=overlay_close state=idle` |
| 22:46:49.322 | front | `story_discard corr_id=sess-… consumed=true cancelled=True payload_session_id=c1fa8409-…` |
| 22:46:49.453 | front | `session_end_boundary corr_id=sess-… src=listen_stop routing=chat action=release_only` |
| 22:46:49 | engine | `收到listen消息：{"type":"listen","state":"stop"}` |
| 22:46:50 | engine | `识别文本: 嗯。` → `Abort message received` → `退出记录语境（恢复 LLM）` → `大模型收到用户消息: 嗯。` |
| 22:46:53 | device | `<< 嗯～` + `StateMachine: State: idle -> speaking` |

**机制**（代码实证）：丢弃时 front 只在 `_consume_kidpal_control` 里 `acc.cancel`，**不释放记录语境**；随后设备 `listen stop` 先被**转发给引擎**，front 才在 `_story_listen_stop_boundary` 发 `active=false`。引擎收到 stop 时 `_kidpal_record_context` 仍为真 → `_kidpal_on_listen_message` 不置 `_kidpal_listen_closed`、不清 `asr_audio`（记录语境豁免截断）→ stop 触发的尾音 ASR（「嗯。」）成立；`active=false` 恰在 ASR 回合期到达 → `chat()` depth==0 读到标志已假 → 派发 LLM/TTS。

**同族复现**：22:48:01（`Okay.` → 「嗯～时间不早啦…晚安」）、22:49:59（`うん。` → 「晚安啦～」）、22:53:24（`嗯。` → 「嗯～」）。三处均伴随设备 `idle -> speaking`。

**判据**：丢弃/会话关闭必须使引擎**不再产出该回合任何 TTS**；设备 Idle 不得被迟到 TTS 拽回 Speaking。**不得削弱** `_kidpal_listen_closed` 与 abort 既有语义。

## 二、D3 · 空素材会话静置不结束（已确证）

**三源时序（22:51:03–22:53:24）**：

| 时刻 | 源 | 证据 |
|---|---|---|
| 22:51:03 | device | `story_key rec_session_slot page=0` → `idle -> listening` |
| 22:51:05 | engine | `进入记录语境（抑制 LLM）` + `收到listen消息 state=start mode=auto` |
| 22:51:13 → 22:53:21 | front | `story_guide_round corr_id=sess-… skipped=no_session`（每 4s 一条，共 ~30 条） |
| 22:53:24 | device | `story_key discard_rec cancel_count=3` → `idle`（human 手动退出） |

**机制**：会话 `routing==accumulate` 且 `accum` 为空 → `stuck_due_sessions` 每 tick 命中 → `_story_guide_round` 取 `build_organizing_input` 得 None → `skipped=no_session` **不 mark_guide_round** → `auto_finish_due_sessions`（需 `guide_rounds>=2`）永不到期 → front 侧无收尾。设备侧 60s 无语音 producer 对记录语境**硬豁免**（requirements §2.1 v1.65「记录会话由引导/归档链收尾」）——但空素材恰恰没有引导链，于是设备常驻 Listening，静置 2 分钟不结束。22:46:41 亦有一条 `skipped=no_session`（首句 ASR 未到前的空 accum）。

**空素材不归档现状**：`_story_finish_session` 的 `material` 空分支与 `_archive_material_reject_reason`（`insignificant_material` / `pure_hallucination_material`）已能拒绝空/幻听素材；22:49:59、22:53:24 的尾音 ASR（`うん。` / `嗯。`）均未成篇。**本轮缺陷是「会话不结束」，不是「空素材被归档」**；轮 A 未观测到空故事入册。

**判据**：无有效素材时自动收尾 = 「释放、不归档、不推 story_list、不盖章」，与 pending 10s 释放口径统一；有素材保持原归档语义。

## 三、D1 · 记录语境/引擎抑制的「失同步」——事实澄清 + 真实缺陷面

**三源实证（引擎侧）**：轮 A 每个故事会话均进入记录语境，且在语境内的 ASR 被抑制：

```
261117 22:41:46 … kidpal record-context: 进入记录语境（抑制 LLM）
261117 22:46:32 … kidpal record-context: 进入记录语境（抑制 LLM）
261117 22:46:44 … kidpal record-context: 抑制 LLM 派发（记录语境闭嘴回显）: 123 45677452321。
261117 22:46:58 / 22:47:05 / 22:47:59 / 22:48:16 / 22:49:45 / 22:49:52 / 22:51:05 …
```

→ **human 的原始推断「front 未进入记录语境、引擎因此正常处理 ASR」与三源日志不符**：设备听到的 TTS 全部来自 D2 的「会话结束尾音 ASR」路径（第一节），不是语境缺失。front 侧之所以看起来「从未进入」，是因为**front 对语境进入没有审计行**（只有引擎打），而 front 每次会话结束都打 `session_end_boundary … routing=chat action=release_only`（该行语义 = stop 时会话已被 discard/pending 释放，**不是**从未进入）。

**仍成立的真实缺陷面（按 human 要求修）**：

1. **注入依赖边缘**：`_consume_kidpal_control` 的 ctx(story) 分支只在 `ctx_push_edge`（`routing != "accumulate"`）时注入 `kidpal_record_context active=true`；`control/start_recording` 消费沿（`_inject_record_context_enter`）同样只在边缘。若累积器仍处 `accumulate`（泄漏的会话态、重复 ctx、边缘竞态）→ 不注入 → 引擎不抑制。要求改为**每会话开始无条件注入（幂等）**。
2. **可观测性缺口**：front 侧无「进入记录语境」审计行，导致三源对账只能靠引擎日志、误判成本高。修复轮补一条可判别的 front 审计行。

## 四、轮 A 覆盖记账（相对 KRC-E4 A1–A13）

| 项 | 观测结果 |
|---|---|
| A1 温通道进聆听 ≤0.82s | 未专项量测（本轮聚焦缺陷段） |
| A2 连续 5 次零冷通道零 burst | 未专项量测 |
| A3 闲置 >5min 首按键 | 未专项量测 |
| A4 会话间隙提醒必达 | 未专项量测 |
| A5 记录会话全链四式 | **部分失败**：丢弃路径暴露 D2；空素材静置暴露 D3；KEY 收尾归档、2 轮引导收尾未单独复跑 |
| A6 60s 静默自动关闭 | **失败**：记录语境下豁免生效且空素材无引导链 → 不关闭（D3） |
| A7 连续讲 >90s 不误收 | 未专项量测 |
| A8/A9/A10/A11/A12/A13 | 未跑/未专项量测 |
| D1 语境抑制 | 引擎侧全绿；front 注入边缘与审计行按第三节修 |

## 五、轮 B 复测清单（种子；最终版写入 review-packet）

1. **B-D1**：故事页 KEY 起会话 → front 审计行出现「进入记录语境（含无条件/幂等形态）」+ 引擎 `进入记录语境`；说一句普通问句（如「明天是星期几？」）→ 引擎只出 stt 回显，**零 TTS**、设备不 `idle -> speaking`。
2. **B-D2**：录音中说一句话后 **BOOT 丢弃**（尾音在途）→ 设备回列表且停留 Idle ≥5s；引擎日志无该回合 `大模型收到用户消息` / `发送第一段语音`；front 日志出现 abort + `active=false` 先于 listen stop 的次序证据。回归：KEY 收尾仍正常归档（A5 录音态 KEY 收尾）。
3. **B-D3**：故事页 KEY 起会话后**完全不说话**，静置 60–70s → 设备自动回列表（非 BOOT 手动）；front 无 `story_list` 推送、无 `story_text`、STORIES_DIR 零新增；随后普通问答立即可应答（语境已释放）。
4. **回归**：A5 全链四式、A6 普通问答 60s 静默关闭、A7 连续讲 >90s 不误收。