# plan · 260915-key-layer-dispatch

> workstream：m1-home-qa（M1 开工批，块①全局 UI 基建）｜ workType：整改型（锚 = requirements §2.3「键位实现侧对账」问题登记块）｜ 设计权威 = docs/requirements.md §2.2/§2.3（story 语义权威现文）

## 背景摘录（锚点逐字，权威 = docs/requirements.md §2.3）

> ⚠️ **键位实现侧对账（v1.39 查证登记，Phase 1 dev 修复项——需求先行、代码后改）**：
> ① **BOOT 单击 = 板装配层硬路由**（`InitializeButtons` → `key_route_global(BOOT, CLICK)` 恒产 `GLOBAL_ACT_PAGE_NEXT` → `cycle_next()`，不看当前层；「关窗」是翻页函数开头清 transient 层的**副作用**，正经裁决函数 `kidpal_reminder_key` 设备侧零调用）——本表「覆盖层内 BOOT 不翻页」落地时须拆耦合：dispatch 层按 `layer_top` 分流 + `cycle_next` 的「清层」与「进页」分离，连带 `schedule_reminder_b.acceptance.c` 十余处「BOOT 关闭照常翻页」钉点成批改；
> ⑤ **KEY 长按实现恒回首页**（`GLOBAL_ACT_HOME`→`show_home`），v1.36「返回上一层」未接；

覆盖层优先级与按键语义权威 = §2.3 全矩阵（v1.47 形式）+ §2.2 提醒行/查字窗行（v1.39 隔离口径）。

## 改法取舍

- **取舍 A（分层裁决点）**：在按键 dispatch 层按 `layer_top` 分流，覆盖层顶层时 BOOT/KEY 长按交由顶层覆盖层的裁决函数（如 `kidpal_reminder_key` 首次接上设备侧）；主页面时走既有路由表。备选「每覆盖层自抢按键事件」被否——裁决权分散会复发「BOOT≈翻页」等价假绿，与 260911-key-route-a1 的机制齿教训冲突。
- **取舍 B（清层/进页分离）**：`cycle_next()` 开头的清 transient 层副作用改为显式步骤（关层归关层、翻页归翻页），主页面翻页路径行为不变——这是纯重构，机制齿 KD-A2 钉住。
- **取舍 C（返回上一层语义）**：现行页集（4 页 + 覆盖层）下「上一层」映射 = 覆盖层→关层回原页、主页面→回首页；逐层返回的完整语义随 M2 撤 P4 后的 3 页集自然成立，本包不预支。

## 阶段划分

1. dispatch 层 layer_top 分流改造（KD-A1）+ 清层/进页分离（KD-A2）——host 机制齿先行；
2. KEY 长按 layer-aware 路由（KD-A3）；
3. schedule_reminder_b.acceptance.c 钉点成批改（KD-A4，acceptance-author 留痕）+ 全量回归（KD-A6）；
4. 真机确认轮（KD-A5，human 持板）。

## 铁律约束

- `docs/contracts/**` 与 `docs/requirements.md` 只读（需求零修订，KD-E2）；发现冲突走 spec-lane 逃生门。
- 页循环 4→3 / 撤 P4 / 页序漂移（对账 ②）零触碰——归 M2；P3 键位族 v1.47（对账 ⑥）零触碰——归 M4（KD-E1）。
- `schedule_reminder_b` 钉点修订必须在 acceptance-author 权限内、留痕 + 不豁免验证，不得静默放宽断言。
- 分层模型（layer_top）owner = 本包；keybar/语音轮/提醒各包只读消费（crossPackageBehaviors 已登记）。

## demo 证据形态复述（与 roadmap 一致）

host runner 输出（相对基线零新增红 + 新齿全绿）+ 真机轮（human 持板：覆盖层 BOOT 不翻页、主页面 BOOT 翻页、KEY 长按返回上一层三行为）。
