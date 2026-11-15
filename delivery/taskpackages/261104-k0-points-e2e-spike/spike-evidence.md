# spike-evidence · 261104-k0-points-e2e-spike

> 证据载体（K0PTS-L1..L4 live-manual 层唯一载体；spike-first 铁律：文件写入 / CLI 返回 / pytest 读回只作辅证）。
> 真机轮日期：2026-11-05；human 在场操作与目击；planner 取证与成文。
> 验收判据基准：taskpackage exitCriteria K0S-E1..E6（**E1/E2/E3 经 2026-11-05 human 真机轮裁定修正**，修正内容见各条原文与 pod-events human-ruling 事件）。

## 一、部署与环境事实（真机轮前置）

- **ECS server 部署**（human 批准生产面动作）：`front_app.py` / `display_schema.py` / `points.py`（新）→ `/opt/kidpal/front/{front,gateway}/`，`AGENTS.md`（薄意图纪律锚）→ `/root/.openclaw/workspace-kidpal/AGENTS.md`。上传前核远端 == main 基线（三件 CRLF 归一后逐字节一致）；远端原文件备份 `*.bak-20261105-k0`；远端 `py_compile` 通过；`kidpal-front.service` 16:30:05 重启，startup complete（`gateway.points` 导入链随启动加载成功）。
- **设备烧录**：app-only @ `0x20000`（ota_0，NVS 保全），`xiaozhi.bin` 3,302,544 bytes hash verified；资产分区零变更（`git diff 2f2c5871..HEAD -- firmware/` 无 asset/spiffs 面）故无双分区义务。烧后 SD 日志确认启动成功（时间戳平滑推进、story prefetch 恢复、无复位循环）。
- **state 文件**：`/opt/kidpal/points.json`（front 与 CLI 共享，`KIDPAL_POINTS_STATE` 单源）。

## 二、L1 · 真实设备语音记账（K0S-E1 ①②③ + L3）

时间线（2026-11-05，human 对设备说完整记账句「我今天扫地了，加5分」）：

| 时刻 | 环节 | 证据行 |
|---|---|---|
| 16:12:51 | ASR | engine：`识别文本: 我今天扫地了，加五分。` |
| 16:12:51 | LLM | engine：`大模型收到用户消息` |
| 16:12:59.569 | 薄意图截获+落账 | front：`points_intent executed corr_id=49a00ab3… action=add_points delta=+5 balance=5` + `gateway.points points_entry corr_id=651211a9… entry_id=pt_23877cbf471a4d74b818680c6db56d39 balance_after=5 revision=1` |
| 16:12:59 | 权威推送 | engine relay 回执：`{"route":"points_home","balance":5,"revision":1,"recent":1}` |
| 16:12:58* | 设备消费 | 设备 SD 日志：`MCP: display_dispatch view=points_home corr_id=kdp-13280342 balance=5 revision=1 restore=0 takeover=1`（*设备钟与 ECS 钟差 ≈1–2s） |

- **L3 实测时延**：语音句说完（ASR 完成 16:12:51）→ 设备消费权威帧（16:12:58）≈ **7–8s**（大头 = LLM 轮次）。只记录不拍阈值（NFR-PERF-4 口径）。
- **human 目击**：设备停留在对话页，回复文字含「扫地加5分」；专属积分页不可见（见 §五 可见形态定性）。

## 三、QQ canary（K0S-E3 修正判据 + A6 活体）

- **第一笔真实 QQ**（human 家长 QQ 发送「给小伴加3分，CANARYQ1帮妈妈收玩具」）：
  - QQ bot 在线（15:57 重连后 gateway READY）；main agent 处理（deepseek 调用 16:15:35–16:16:19），QQ 收到正常回复；**human 实测时延 ≈45s**（main agent LLM+工具链处理，只记录）。
  - 落账实证：`/opt/kidpal/points.json` 新增 `pt_2dfaea0dc9644c82a2d8575dc1d5700a` delta=+3 reason=「帮妈妈收玩具」（token 被 main agent 剥离）occurred_at=2026-11-05T08:16:18Z balance 5→8 **revision=2**。
  - **单源活体证明**：QQ 链（main agent → 最小 add CLI）与语音链（front → points.py）写同一 state 文件、同一分录形状——无第二账本（K0PTS-A6 的真机版）。
  - 「不抢前台」：设备前台未动（human 目击 + 设备日志无 points 相关渲染）。
- **第二笔（积分页在屏原地刷新）未执行**：其判定面依赖「QQ 落账后设备缓存同步推送（restore=True 活调用方）」——实测确认 **K0 front 无任何 points restore=True 活调用点**（代码唯一调用点在语音成功路径且缺省 restore）；该缝在验收件 GAP-1 冻结期已预申报，本次真机轮兑现确认。按 K0S-E3 修正判据**降级申报顺延 K1/K2**，不宣称该状态已由真实 QQ 链路验证。

## 四、「打开积分本」轮（A6 消费端活体 + 权威单源真值）

human 语音「打开积分本」：

- ASR 16:29:29「打开积分榜。」→ 引擎薄载荷 `{view:points_home,data:{}}` → front `_expand_points_nav_intent` 展开权威帧 → 16:29:38 relay 回执 `{"route":"points_home","balance":8,"revision":2,"recent":2}`。
- 设备 16:29:36：`display_dispatch view=points_home balance=8 revision=2 restore=0 takeover=1`。
- **判定**：设备消费的 balance=8 正是 QQ CLI 写入后的权威状态——「front/设备真实读到 CLI 落账」（K0PTS-A6 单源面）真机成立。

## 五、设备可见形态定性（K0S-E1 修正判据 ③ 的依据）

- K0 设备侧积分渲染 = **最小渲染**：`SetChatMessage("system", "星星 N 分 · 事由")` 文字行（mcp_server.cc 注释明钉：金币/进度线/刻度完整版式归 K2）。文字行宿主 = `chat_message_label_`（建在 `bottom_bar_` 上、页点正上方）。
- 真机观察：语音轮活跃期会话浮层前置（`chrome_mid_sync front=1 voice_live=1`、`KEYBAR ovl=1`），文字行出现在对话页内且可能被模型回复文字快速覆盖；会话「结束」回首页（既有会话底座语义）。
- **「10」残段定性（human 照片实锤，2026-11-05）**：human 在页点上方每页可见的「10」= 第二轮语音记账（16:34:30「今天浇花，加2分」，落账 `pt_b35bb6ed…` delta=+2 balance=10 revision=3，front `points_intent executed` 日志在案）后写入 `chat_message_label_` 的最小渲染行「星星 10 分 · 今天浇花」的可见残段——**该「10」即权威 balance=10 的设备显示，构成 K0S-E1 ③ 的消费端目击本体**（human 无意间目击了权威余额上屏）；同时 human 裁定此形态不可留（残段挂底栏每页可见），归 K2 完整积分页替换，观感问题列入 K2 版式输入。
- **human 裁定（2026-11-05）**：原 K0S-E1「设备实际显示权威余额和原因」隐含专属可见版式前提，与 K0 最小渲染范围不符，判据按 §真值链口径修正；「页面 OK」的验收归 K2（完整积分 UI 包）。

## 六、边界发现（spike 的产出物，交后续包承接）

1. **QQ 通道设备缓存同步推送缺位**：restore=True 活调用方在 K0 未接线（GAP-1 兑现）→ 归 K1（正式 producer 单 writer 化时一并接线）或 K2（消费侧）。
2. **设备侧分页请求运行时触发点未建**：K0a 骨架冻结件明钉 points_ledger 语境 landsIn K2 + K0 验收件 GAP-3 → 归 K2。
3. **「10」残段观感（已定性，见 §五）**：K0 最小渲染文字行的宿主 label 常驻底栏页点上方，长句裁切后残段每页可见——非异物、非缺陷，是 minimal 渲染形态 + label 位置的组合观感；K2 完整积分页替换时消化（版式第一输入）。

## 七、判定汇总（对照 exitCriteria）

| 判据 | 判定 | 依据 |
|---|---|---|
| K0S-E1 | **达成**（按 2026-11-05 修正判据） | §二（语音链真值 + 设备接受）+ §三（QQ 链真值 + 单源）+ §五（最小渲染形态） |
| K0S-E2 | 离线半达成；设备半降级申报顺延 K2 | K0PTS-A5/F3 全绿 + §六.2 |
| K0S-E3 | 第一笔达成；第二笔降级申报顺延 K1/K2 | §三 + §六.1 |
| K0S-E4 | **达成** | K0a merged（3306abac）→ 共享 CMake 追加 → host 基线 278/3 → device build 真编译 → 本真机轮 = 终验 |
| K0S-E5 | **达成** | 本文件 + taskpackage 固化 + Review Packet |
| K0S-E6 | **达成**（未触边界） | 未建 K3 skill、未扩写权、无人工直编 state 冒充 |
