---
changeId: 260911-display-first-m1
rigor: full
riskTier: high
sourceType: proposal
gateProfile: [frozen-acceptance-revision, device-round, contract-drift]
reviewed-by: 设计权威 = PROPOSAL FROZEN v1.0（human 触点② grilling OQ2=M1 已裁，pin sha1:cd10d1903ac6bb1a / git blob 26138dba）+ review.md F3/F5；本 plan 由 portfolio-planner 依 ARQ-260911-split-package-implementation-prose-gap 起草，planner intake 只读消费、改需留痕
date: 2026-09-11
---

# PKG-4 显示先行 M1 · 包级实施方案

> 承载 PROPOSAL §2.2 裁定（OQ2=M1 围栏闭合即发）+ §7 申报（冻结 acceptance 面必触、
> 契约 reviewer-first 复核）+ review F3（契约面核查）/ F5（批次①部署前置）。
> 逃生门：M3 变体（prompt 改块前置）禁止在 dev-loop 内自行采用——与契约 §1.1
> 「在回复文本末尾嵌入」逐字冲突，若执行中发现要走必须先过 architect-reviewer
> 四分类 + human 契约门；M1 初判不改 E1 契约设备面，开工前 reviewer-first 复核。

## 1. 根因（PROPOSAL §1.2，地面真值）

引擎补丁 DisplayFenceStateMachine 在 LLM 全流 finish() 才发 kidpal_display_blocks
侧通道（connection.py:1442-1456）——TTS 已开讲 1–3s 后屏幕才换页，答案文字写在
被服务端页面盖住的聊天屏上（听得见看不见）。状态机已支持块级闭合检测，缺的是
闭合回调/发射 API 与提前触发位。

## 2. 改法（已裁：M1 围栏闭合即发；M2 是 M1 推论不单列）

- **strip machine API 扩展**：`server/deploy/engine-patch/kidpal_display_strip.py`
  把「块闭合」从 finish() 提前到 feed() 内——块闭合检测命中即经新增闭合回调/发射
  API 发射该块，不等全流 finish。**该文件是冻结离线契约（feed/finish 语义）**，
  API 扩展走 acceptance-author 权限内修订（留痕，不豁免验证）。
- **front 时序复核**：front 逐块中继已是现状（`server/gateway/mcp_relay.py`
  TOOL_DISPLAY kidpal/display relay）——复核逐块中继在「提前发射」新时序下的
  幂等/顺序/回执语义，不预期改 front 代码，复核发现缺口则本包内修。
- **设备侧语音中渲染**：display 块中途到达时按 PKG-2 归一屏的 layer 仲裁规则渲染
  （语音轮页可见聊天 layer 内容同步更新）——**依赖 260911-screen-unification
  （mergeOrder 19）先行合并**。
- **部署前置**：部署窗口必须走修正后的 apply-xiaozhi-overlay.sh——批次①
  260911-ecs-deploy-convergence（mergeOrder 16）为硬前置（review F5：先跑断语音）；
  部署后 engine-patch 在位核验（启动标记行 `kidpal display-strip patch active` +
  `docker inspect` Mounts，ARQ-260907-engine-deploy-patch-drift 缓释流程）。

### 2.1 阶段划分

- **阶段 0 · reviewer-first 契约复核**：开工前复核 M1 不改 E1 契约设备面
  （view schema / mcp 信封 / 白名单不变，时序契约未钉板——review F3 初判）；
  复核结论落包 evidence。发现需改契约 → 停，architect-reviewer 四分类 + human
  契约门，本包不带契约变更走。
- **阶段 1 · strip machine API 扩展**（acceptance-author 先行修冻结齿语义边界 +
  builder 落地）：feed() 内块级闭合即发射 + 闭合回调/发射 API；冻结离线契约修订
  留痕；test_g_relay_fix.acceptance.py 重放绿。
- **阶段 2 · front 时序复核**：逐块中继 × 提前发射新时序复核（幂等/顺序/回执），
  缺口记录并修复。
- **阶段 3 · 设备侧语音中渲染**：消费 PKG-2 layer 仲裁规则渲染中途块；与 PKG-2
  合并基线复跑其冻结齿。
- **阶段 4 · 部署 + 时延实测**：走修正后 overlay 脚本部署 ECS → engine-patch 在位
  核验 → 真机「单击问课表」时延实测：display 页上屏不晚于 TTS 首段后 0.5s
  （kidpal-live-smoke-debug 三日志源按 correlationId 对齐）——§5 workstream 收口
  标准在本包真机轮统一对账（五条并集）。

## 3. 铁律约束（全阶段有效）

1. **M3 禁采**：prompt 改块前置与契约 §1.1 逐字冲突，任何阶段不得采用。
2. **冻结件修订纪律**：kidpal_display_strip.py 冻结离线契约 +
   server/acceptance/test_g_relay_fix.acceptance.py 仅 acceptance-author 权限内
   修订，REVISION LOG 留痕，断言语义不放松（API 扩展 = additive）。
3. **契约面只读**：docs/contracts/** 零触碰；M1 初判不改契约，复核推翻初判 →
   逃生门停。
4. **部署纪律**：只用修正后 apply-xiaozhi-overlay.sh；engine-patch 在位核验不跳过。
5. **依赖锁**：PKG-2 渲染规则未并入前不进阶段 3；批次① 未合并前不进阶段 4。

## 4. 验证 / exitCriteria 对账

- 阶段 1 离线契约齿全绿（修订后）+ 阶段 3 host 复跑绿 + 阶段 4 真机时延实测证据
  落 `changes/dev/260911-display-first-m1/`。
- workstream 级收口（§5 五条）真机轮 owner = 本包（末位）：上屏时延（本包承载）+
  屏幕落点确定（PKG-2）+ 两键行为（PKG-1 语义 / PKG-2 回归）+ 崩溃同族 0 复位
  （PKG-2）统一对账并集。
