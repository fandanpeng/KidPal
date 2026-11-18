# 真机轮 C 在线证据（2026-11-18 21:20–21:24 CST）

> 采集者 = planner（Pod `261116-channel-resident-core`，轮 C 修复轮 intake 只读取证）。
> 采集方式 = `python c:/tmp/ssh_helper.py "<remote cmd>"`（ECS 8.137.91.16，只读命令）。
> 本目录仅含**原始输出摘录**，不含推断；推断与对账见 `device-round-c-online-evidence.md`。
> human 2026-11-18 口述事实与本目录逐条对账 —— 一致（见对账文件）。

## 文件

| 文件 | 来源命令 | 内容 |
|---|---|---|
| `ecs-front-journal-2120-2134.txt` | `journalctl -u kidpal-front --since '2026-11-18 21:20:30' --until '2026-11-18 21:24:30'`（去掉 `story_guide_round` / `alert_poll` 噪声行） | front 收尾链审计行 |
| `engine-asr-2120-2123.txt` | `docker logs xiaozhi-esp32-server \| grep '261118 21:2[0-3]' \| grep '识别文本\|record-context'` | 引擎 ASR 识别文本 + 记录语境抑制行 |
| `ecs-story-2026-11-18-s33477cee.md` | `cat /opt/kidpal/stories/2026-11-18-s33477cee.md` | 被误归档的垃圾故事原文 |
| `ecs-story-2026-11-18-sbe0bff90.md` | `cat /opt/kidpal/stories/2026-11-18-sbe0bff90.md` | 同夜 A5 正常归档对照（真实素材） |

## 关键时间线（三源交叉，原始行见上列文件）

| 时刻 (CST) | 源 | 事实 |
|---|---|---|
| 21:20:41 | front | `story_record_ctx_enter src=ctx_story`——记录会话开始（`sess-88902e04…`） |
| 21:22:05 | front | `story_stop archived=false reason=pending_stop` + `session_end_boundary routing=pending_stop action=protected`——空累积挂起（分层保护正确） |
| 21:22:06 | 引擎 | `识别文本: Okay.` + `kidpal record-context: 抑制 LLM 派发（记录语境闭嘴回显）: Okay.`——静音 ASR 幻觉 |
| 21:22:09 | front | `story_organize_round ok=true id=2026-11-18-s33477cee` → `story_finish archived=true`——幻觉喂入累积器 → 整理 → 归档 |
| 21:23:27 | front | 第二个记录会话开始（真实素材） |
| 21:23:49/50 | 引擎 | `识别文本: 说完了。` 然后 `识别文本: 嗯。` |
| 21:23:52 | front | `story_organize_round ok=true id=2026-11-18-sbe0bff90` → `archived=true`——A5 正常路径（控制组） |

## 垃圾故事原文要点

- `kidpal-id: 2026-11-18-s33477cee`，标题行 `# （没有可整理的内容）`，正文同句。
- 标题字段 base64 `77yI5rKh5pyJ5Y+v5pW055CG55qE5YaF5a6577yJ` 解码 = `（没有可整理的内容）`。
- 文件 mtime = 2026-11-18 21:22（与 front `story_finish archived=true` 同刻）。