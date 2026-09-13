# kidpal-mvp · findings

> 调研发现、风险、偏差。追加式。

- 2026-08-29 实测：固件基座（小智 V2.1.0 整树拷贝）无 host 测试基建、无 linux target 配置、无测试目录；`server/`（shim）预留未建。
- 详见 `PROPOSAL.md` §1「拆分依据（地面真值）」。
- 2026-09-07 包 2（260829-voice-roundtrip）live 冒烟发现：引擎（ECS xiaozhi-esp32-server）display 切分 live 集成两处缺陷——① ```display fence 未剥离即漏进 TTS 文本（设备 `Application: <<` 与引擎 `发送第一段语音` 同证）；② 未发起 kidpal/display tools/call 中转（引擎无 relay 日志、设备无 MCP 到达日志），display text 视图面未被触发（用户所见全文来自文字同步面）。证据：`changes/dev/260829-voice-roundtrip/roundtrip-live-evidence.md` display round 节。归属 = G 包（260903-gateway-directionB）live 集成修复；prompt 注入缺口（overlay 渲染到陈旧诱饵路径）已在本包 fixer 修复。另：主 checkout 本地 `firmware/sdkconfig`（gitignore 机器本地件）过期未随 WS 包更新，后续 worktree 镜像会重踩错板型问题（friction fb2c70c9）。
