# device-round-evidence · 261117-session-end-intent-longretry

> 真机轮 B 的**结构化目击记录**（2026-11-20 ~ 11-21 human 持板；新固件已烧、front 与 prompt 已部署）。
> **本轮实际覆盖**：B1 告别（human 目击）、S-F1.4 提醒必达（注入测试提醒，设备 KPLOG 实证）、SCR-A8 空素材记录会话自动收尾（human 目击 + 设备 KPLOG 实证）。
> **未跑项**：B2–B8 中除上述以外的项，按 human 明确口径「其他没测，后面观察吧」如实记 NOT_RUN，不伪造 PASS。

## 轮次上下文

- 固件来源 commit：`b78834e9b57a973daeee8eb2f3ca579afc609a2d`（`dev/pod-m2`，含 P2 全部实现；二进制构建于 2026-11-19 23:27）。
- 二进制：`firmware/build/xiaozhi.bin`，SHA-256 `1ce429815345c656d3824054de5ca50af7f89505fda51a2d8b9afc19ba531c1a`，烧录偏移 `0x20000`（ota_0，NVS 保留），`Hash of data verified`。
- front / prompt：`front_app.py` → `c125911f…`、`display_schema.py` → `26c55a41…`、`workspace-kidpal/AGENTS.md` → `d1e53461…`（热部署 + `kidpal-front` 重启；旧件备份 `*.bak-20261120-p2`）。
- 启动确认：SD 日志 `starting → activating → idle`、WiFi 连上、WS 建链、`AutoConnect task exit`；front 日志设备重连 `connection open` + `points_connect pushed`。

## 目击记录

device-round-evidence（JSON）：{"schemaVersion": 1, "changeId": "261117-session-end-intent-longretry", "sourceCommit": "b78834e9b57a973daeee8eb2f3ca579afc609a2d", "binaryRef": "firmware/build/xiaozhi.bin", "binarySha256": "1ce429815345c656d3824054de5ca50af7f89505fda51a2d8b9afc19ba531c1a", "observations": [{"acceptanceRef": "docs/requirements.md#§2.1", "method": "human-visual", "result": "PASS", "observer": "human", "observedAt": "2026-11-20T22:45:00+08:00", "note": "B1 结束通道②：human 对设备说「拜拜」，会话结束生效（human 口述「拜拜测试生效了」）；麦克风关闭/回页细节未逐项目击"}, {"acceptanceRef": "SEI-A1", "method": "human-visual", "result": "PASS", "observer": "human", "observedAt": "2026-11-20T22:45:00+08:00", "note": "端到端路径成立的前提是设备 control case 0 收口统一处理器（EndVoiceSession）；人类可观察结果 = 会话确实结束"}, {"acceptanceRef": "docs/requirements.md#S-F1.4", "method": "kplog", "result": "PASS", "log": {"segment": "KPLOG000", "window": "2026-11-20 22:45:00 ~ 2026-11-20 22:45:02", "excerptRef": "delivery/taskpackages/261117-session-end-intent-longretry/device-evidence/round-b/kplog-excerpt.txt", "excerptSha256": "3efa92f5ebb5f81a10de3546d6bcfc2405c4f8457202d63d54d44472b4840e15"}, "note": "注入测试提醒 rmd-test-261117-224500：front alert_deliver delivered=true device_frames=1 speech=true；设备 alert_dispatch -> alert_render raised=1 tone=ogg_popup -> alert_receipt ack=true dequeued=true"}, {"acceptanceRef": "SCR-A8", "method": "kplog", "result": "PASS", "log": {"segment": "KPLOG000", "window": "2026-11-21 22:31:15 ~ 2026-11-21 22:31:17", "excerptRef": "delivery/taskpackages/261117-session-end-intent-longretry/device-evidence/round-b/kplog-scr-a8-excerpt.txt", "excerptSha256": "1e880885b5172cdebf04df694529448b803722162557e3b27c62f1707c44868c"}, "note": "空素材记录会话静置：story_empty_session_timeout quiet=60 asr=60 src=empty_story -> PAGE_RENDER in=3 sview=0 -> STORY_RENDER page=0（LIST）；human 同步目击自动退出且未归档"}, {"acceptanceRef": "docs/requirements.md#S-F1.4-b2b8", "method": "human-visual", "result": "NOT_RUN", "observer": "human", "observedAt": "2026-11-21T22:31:00+08:00", "note": "B2-B8 未测，human 口径：其他没测，后面观察"}], "deferred": ["B2 告别语不误伤", "B3 伪造防护", "B4 拔网线自愈与退避节奏", "B5 重启 front 自愈", "B6 离线期提醒覆盖窗（仅常规提醒必达已证）", "B7 离线 30 分钟电量差", "B8 长跑期人工入口不僵死与超长离线补投观感"], "humanRuling": "2026-11-20 human 原话：「拜拜测试生效了，其他没测，后面观察吧」——B2–B8 转合并后观察"}

## 未跑项与去向（如实申报）

| 项 | 状态 | 去向 |
|---|---|---|
| B2 告别语不误伤（「拜拜，明天再聊」只结束会话） | NOT_RUN | 合并后观察 |
| B3 伪造防护（引擎流注入 end_session → relay 恒拒） | NOT_RUN | 离线 SEI-B1/B2/B3 已覆盖同一判据；真机观察 |
| B4 拔网线自愈（退避 5/10/20/40/60 后恒 60s） | NOT_RUN | 合并后观察 |
| B5 重启 front 自愈（60s 内重连） | NOT_RUN | 合并后观察 |
| B6 离线期提醒覆盖窗（S-F1.4 离线分支） | NOT_RUN | 合并后观察（常规提醒必达已由本轮注入实证） |
| B7 离线 30 分钟电量差 | NOT_RUN | 合并后观察 |
| B8 长跑期人工入口不僵死 | NOT_RUN | 合并后观察（审计行 `manual_intent_queued`） |

## 本轮附带观察（不构成 SCR-A8 失败项）

- human 目击：进入记录会话后**小伴没有播报引导语**（预期「需要我帮你记点什么？」）。SCR-A8 的判据是静置自动收尾且不归档，该两项均 PASS；引导语静默登记为**合并后观察项**，若复现另立包定位（可能属记录语境 TTS 路径，与本包改动面不同）。