---
rigor: full
riskTier: high
sourceType: delivery-story
reviewed-by: pending（门 1 待发起）
date: 2026-09-17
---

# M2-K2 撤 P4 常驻页（页循环 4→3 + 枚举对齐 + 页点 4→3）

> 来源：里程碑 `delivery/milestones/milestones.jsonl#M2-char-window` nextAction 块「撤 P4 常驻页、页循环 4→3、锚 canvas-p4→char-window 对账改名（F3 全节）」。
> 需求权威：`docs/requirements.md` §2.4 行 186（P4 查字页已撤销）+ §2.2 行 69（页点 4→3）+ 行 166 ②（页序漂移理顺）。

## 背景摘录（锚点摘录，story 语义权威 = requirements 现文）

- **枚举漂移**：`firmware/main/display/kidpal_render_node.h:25-28` 枚举 `KP_PAGE_HOME=0/CHAR=1/SCHEDULE=2/STORY=3`，真机翻页序 = 首页→查字→课表→故事，与 spec P1→P2→P3 不一致（行 166 ② 明注「撤 P4 改 3 页时一并理顺」）。
- **页点 deviation**：M1-K2（keybar-three-slot，已 merged）launch 前拍板按现行 4 页循环渲染 4 页点，`firmware/main/display/keybar.c` 现有 PAGE_P4 槽位——4→3 对账归本包收回（m1-home-qa progress.md 2026-09-16 launch 批既定，无新增拍板项）。
- **冻结钉点碰撞**：撤页必触 `schedule_reminder_b.acceptance.c` 等冻结套件的 4 页循环断言（十余处钉点成批改，行 165 对账注记先例同型）——修订一律走 acceptance-author 留痕，不绕闸。
- **查字通路保留**：P4 撤的是**常驻页位**，不是查字能力——mcp_server view=char 路由与 char_page 渲染面保留（M2-K3 改窗的生存面），本包不得顺手拆通路。

## 改法取舍

- 枚举重排：`HOME→SCHEDULE→STORY` 对齐 spec 页序（CHAR 从枚举撤出或收编为窗层归属——枚举层的最终形态由 M2-K3 改窗决定，本包只保证撤出循环、对齐三页序）。
- 冲突面协调：dependsOn M2-K1（K1 动 char_page.cc 动画帧，本包动 render_node 的 CHAR 停靠分支与 char_page 邻近引用），串行合并消冲突，非技术必需依赖。

## 阶段划分

1. acceptance-author 冻齿：M2K2-A1（三页循环齿，修订既有 4 页断言留痕）+ A2（枚举对齐 + 页点 3 点结构齿）+ A3（键路由回归靶）+ A4（查字通路保留靶），实跑红基线落账。
2. builder：render_node 循环撤 CHAR + 枚举重排 + keybar 页点 4→3（PAGE_P4 槽位撤除）。
3. verifier↔fixer：host 全量零新增红（key_route_global / key_layer_dispatch 既有语义零回归）+ device build 闸。
4. integrator/supervisor 闭环；真机翻页确认 deferred 声明（挂 M2-K3 设备轮，M2K2-A5 structural）。

## 铁律约束

- 不改 `docs/contracts/**`、不改 spec、不改 agent 定义/gate。
- 冻结套件钉点修订只走 acceptance-author 留痕通道。
- 查字数据通路不顺手拆（越包红线，M2K2-E3）。

## demo 证据形态复述

host：三页循环齿 + 页点 3 点齿 + 键路由回归靶全绿留痕；真机半不在本包——demo 口径 = host 全量绿 + device build 绿 + M2K2-A5 deferred 挂账声明。BOOT 翻 3 页循环 + 页点 3 点的真机目击归 M2-K3 设备轮。
