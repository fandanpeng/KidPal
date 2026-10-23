---
id: TECH-261018-schedule-read-compliance-residual
title: prompt v11 后仍观察到真机漏读——受控 loopback 15/15 都读文件，但设备连续两笔里有一笔零工具调用、直接编造周一课表（含不存在的英语/美术与带物）；属模型服从度长尾，机械注入课表内容为候选根治
domain: tech
status: closed
severity: MAJOR
owner: dev 批（server/deploy prompt / 引擎部署面）
trigger: 下个动 prompt 或引擎部署面的批；或真机再次复现漏读时提前
opened: 2026-10-18
closed: 2026-10-23
closedBy: 261022-m5-n2a-schedule-mechanical-injection（consumed-by：本包落地即满足本条目第 3 层机械注入修复，K5-E5 结项申报由 integrator 收口路径落）
paths: ["server/deploy/openclaw-system-prompt.v11.md", "server/deploy/kidpal-agent-prompt.txt"]
source: 2026-10-18 真机连续两笔（引擎日志 10:18:38 / 10:19:38）+ OpenClaw audit run chatcmpl_9553f6fc（read 在案）与 chatcmpl_d65b3252（零 tool_action）+ loopback 受控复现 15/15（非流式 10、流式 5）
handoff: 与后续 prompt 面批同批；根治候选 = 机械注入，见建议改动
---

**证据（真机 + audit + 受控复现三源）**：`261017-prompt-v11-schedule-recall` 落地后（prompt v11 + KidPal 极简模板，引擎日志实测增强 prompt 5,905 字符、旧小智模板 11,610），2026-10-18 真机连续两笔：

- 10:18「后天有什么课？」——audit 有 `read`，答复正确（周二课表）。
- 10:19「那周一有什么课？」——**audit 零工具动作**，直接给出「第1节语文、第2节数学、第3节英语、第4节体育、第5节音乐（记得带竖笛）、第6节美术（要带油画棒）」；真实周一 = 语文/数学/足球/科学/班会/语文/足球。孩子当场戳穿「周一课程又播报的不对」。

同 prompt、同 model、同 loopback 路径受控复现：「周一有什么课？」5 次、「那周一有什么课？」5 次、流式 5 次，**15/15 均调用 read 并答对**。真机与受控面唯一未复现的差异是设备侧的会话/时序，判为模型服从度长尾（粗估 10% 量级），不是配置未生效、不是 skill 缺失、不是工具不可用。

**影响面**：孩子拿到不存在课程的答复（含「记得带竖笛/油画棒」这类编造带物），且无法自愈；比延迟类问题严重。所有「靠模型自己决定是否调用 read」的路径都带这个长尾。

**建议改动（根治候选，交 review 不自批）**：把课表内容机械注入每轮 system prompt，让答复不再依赖 read 调用——两条落法：① 引擎 `context_providers` 配置 + 模板加回 `{{ dynamic_context }}`（原小智模板有、KidPal 极简模板删掉了），由本地端点供课表 JSON；② 在已 bind-mount 的 `server/deploy/engine-patch/connection.py` 里直接读 `kidpal/schedule.md` 追加到 enhanced prompt 再 `change_system_prompt`。代价 = 每轮 prompt 增约 1.5KB、课表随请求出网（与现状读文件出网等价）。

**风险**：riskTier: medium——改 prompt 组装链路（引擎部署面），但两条落法都只增加上下文、不改 display 契约与回合路由；回滚 = 移除注入。

**结项记（2026-10-23，integrator 收口，K5-E5）**：第 3 层机械注入已由 `261022-m5-n2a-schedule-mechanical-injection`（device-memory-overhaul P5）落地——front `schedule.py` 单源派生（`build_schedule_summary(model, today_iso)` 纯函数，只调 `parse_schedule_md` / `items_for_date` 单源符号，契约 §1.5 红线：引擎零解析、不建第二解析器，建议改动落法②的引擎自解析变体未采用）→ 会话建立时经内部通道帧 `kidpal_schedule_summary`（kidpal_speak 同型先例）→ `connection.py` 在 `change_system_prompt` 链内填充 `{{schedule_summary}}` 占位（窗口 3 日历日 / ≤2048 字节、逐轮在场、缺席填「（本次会话未提供）」不编造）。host 验收锚 = `server/acceptance/test_261022_schedule_injection.acceptance.py`（K5-E1..E4 十断言：单源反证逐项一致、窗口外零泄漏、受控 21 轮零工具调用有料可答）；final bearing r2 绿（fact ok:true，invocation `becb85f0-afdb-4d4d-a281-a40b99fb0325`）。live 复证（真机 ≥20 轮课表提问零工具调用零编造）挂 P5 收口轮 human 部署轮（`awaiting-human:live-deploy`），不阻结项——host 面机械注入已消除「是否读文件」的模型裁量。
