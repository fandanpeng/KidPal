---
id: ARQ-260909-crossend-consumer-tiers
title: "`crossEnd.counterparts` 的 `{path, symbol}` 只表达**自有实现面**消费方，只读参考树与无源码黑盒两档无处申报（跨端承载覆盖不全）"
domain: gov
status: closed
severity: SHOULD
owner: architect
trigger: pilot 首个②③档包 / 与 OQ2 digest 并论
opened: 2026-09-09
closed: 2026-10-27
closedBy: human 直接指示（主线程代记，261027 P7.5 会话）
paths: []
source: `delivery/changes/arch/260908-gov-review-input-surface-frontload/PROPOSAL.md` §4 非目标末条 + §6 首条；`retro.md` §4 建议 3
---

**保留原因**（有意接受，非漏）：human 2026-09-09 立项收口时裁「本批只做自有面」——① 唯一真炸过的动机包（`260907-g-relay-fix`）对端实测就在 `firmware/main/application.cc`、`mcp_server.cc`，本批形状够用；② 第②档（`ESP32-S3-RLCD-4.2-Demo/**` 小智源码）可读但在版本控制外、跨 checkout 路径不稳，需另立「相对参考根 + symbol、不落行号」子语义；③ 第③档（OpenClaw 引擎、物理声/显表现）**无源码可指**，真正需要的是「哪条 live 断言钉住它」的 oracle 锚语义，而「什么算钉住、如何判缺席」本批无数据支撑，硬造即拍数（夹带会撑破 low-medium 档位）。附带实证：本仓第③档事故确实存在过——OpenClaw 拒 tools 数组（`Intent:nointent`）记录在 `delivery/changes/dev/260903-device-ws-switch/review-packet.md:76` 的 SHOULD 行，`g-relay-fix` notes 以「WS 包 SHOULD #2」引用。**复议触发**：pilot 期首个真遇②或③档消费方的包（该包 `crossEnd` 将无处合规申报，即触发）；或 OQ2 phase-2 digest 提案同批并论（两者都要回答「非源码事实源怎么锚」）。**riskTier: low**（现形状 fail-closed：越界路径 `brief-inputs.mjs` 给 ADV 不硬红，不静默）。**review profile**：Track B（届时按新证据重判三要件；**若黑盒 oracle 锚语义当真设计分叉展开，则该子项走 `full-adversarial + human`**，不得凭本行预授权走快道）。owner=architect。

**结项追注（2026-10-27，human 直接指示，主线程代记）**：human 裁本条**已过时/被消化**，结项。结项时工作树曾出现本文件裸删（无归位痕迹），human 确认删除意图后按正规记账通道补记：还原文件 → status 翻 closed → 本追注 append → 渲染归位。
