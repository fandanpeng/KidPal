---
rigor: lite
riskTier: low
sourceType: human-ad-hoc
reviewed-by: human 方向裁定（2026-09-28「先止血方案吧」）
date: 2026-09-28
---

# 止血：摘除 story_list 三时点①建连即推（M4-K1 真机缺陷修复）

> 来源：human 真机发现（按 KEY 应出聆听态、弹出半截莫名页面）+ planner 根因报告。story 语义权威 = 本包 taskpackage story 行；行为权威 = requirements F2 既有冻结口径（不变）。

## 根因（已三方核实，2026-09-28 会话在案）

- M4-K1 三时点①在 ws_gateway 连接处理里建连即推 story_list（front_app.py :962-967）。
- 设备拓扑：会话结束关 audio channel（application.cc 多路径 + CanEnterSleepMode 要求通道关闭）→ **每次按 KEY 重开新 WS** → 建连即推在每次会话开始时触发。
- 旧固件（K2 未跑）mcp_server.cc 收到 story_list 即 `kidpal_render_node_begin_page()` 无条件渲染 P3 页 → 盖掉聆听态；story_page.c 零 CJK 字体 → 半截页面。
- M4K1-ACCEPTANCE-NOTES §5.2 行为等价假设②（「ws_gateway 调 _push_story_list ≙ 建连即推」）显式申报未验——真实拓扑下该假设不成立，live 面按 M4K1-A7 deferred 归 K3，提前爆雷。

## 改法（单点止血）

- **摘除** ws_gateway 连接处理段 `_push_story_list(websocket)` 调用（含其 try/except 包裹段）。`_push_story_list` 函数本体、②（停录归档后）③（删除执行后）调用点**不动**。
- ①的恢复时机归 K3：设备能表达「会话发起自 P3」（session_ctx）后，front 在正确语境下推列表（届时「进页即见列表 + RAM 缓存」语义重裁——注意 session_ctx 触发的是记录会话开始、直接在彼时推列表会顶掉录音引导态，时机需随 K3 一并设计，不在本包范围）。
- 验收：FIX-A1 负锚齿（建连零 story_list 帧）先红后绿；FIX-A2 a3 齿按新时机语义修订（正常改验收通道，human 方向锚留痕）；其余既有齿零偏移。

## 铁律

- 不动契约面 / spec 面 / agent 定义；不改 K1 历史制品（supersession 注记落本包 docs-delta）。
- `_push_story_list` 函数与②③调用点保持在场；wire 形状零变化。
- 删除/归档推送（②③）在本包验证范围：既有 a3/a4 齿保持绿。

## demo 证据形态

pytest offline：FIX-A1 负锚齿红→绿全程留痕 + 既有齿零新增红；无 live 面（真机复验归 K3 设备轮，M4K1-A7 口径不变）。
