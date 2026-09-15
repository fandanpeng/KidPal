# plan · 260915-keybar-three-slot

> workstream：m1-home-qa（M1 开工批，块①全局 UI 基建）｜ workType：新建型（锚 = requirements §2.2 键帽条行「三槽布局与页点 MVP 未实现」现状登记）｜ 设计权威 = docs/requirements.md §2.2 comp-keybar 行 + docs/ui/brand-spec.md「姿态规则」+ 设计稿 v4 §03 comp-keybar

## 背景摘录（锚点逐字，权威 = docs/requirements.md §2.2 键帽条行）

> | 底部键帽条 + 页面指示 | `comp-keybar`：通栏贴底**三槽**（左 KEY 提示 · **中 3 页点** · 右 BOOT 提示） | **条本身任何状态常驻**——它是基座 chrome、不属于任何单页，覆盖层盖不掉（已实现）。三槽内容随态变：**中槽页点仅 P1–P3 主页面显示**（当前页实心放大；v1.36 页循环 4→3，页点随之 4→3），**覆盖层（提醒弹窗、查字窗、语音轮演出）不显示页点**，左右槽照常给操作提示（提醒弹窗 = 左「KEY 知道了」+ 右「BOOT 关闭」——v1.39：窗内 BOOT = 关窗回原页不翻页）。⚠️ 三槽布局与页点 MVP **未实现**（现仅一个居中文案随态联动：说完啦/不等了/打断/知道了），Phase 1 修复项。规格权威 = brand-spec「姿态规则」+ 设计稿 v4 §03 `comp-keybar`（v4 为现行设计母版） |

现状与差距：键帽条条体常驻已实现，但只有单行居中文案随态联动；三槽结构、页点、覆盖层页点隐藏全部缺失。

## 改法取舍

- **取舍 A（页点数量）**：spec 权威 = 中 3 页点（v1.36 页循环 4→3），但撤 P4 常驻页是 M2 工作、M1 撤页会让现行查字不可达。**取**：页点机制按「数量 = 现行页循环页数」实现（阶段一 = 4 点），4→3 对账归 M2 撤页包收回；deviation 显式登记（KB-A2 / KB-E1 / workstream launchPreconditions）。**舍**：本包内提前撤 P4（越阶段、连带查字断供）。
- **取舍 B（显隐裁决源）**：页点显隐读 `layer_top`（260915-key-layer-dispatch 建立的分层模型），与键路由同源；不另立第二真相源（KB-A4/KD 侧 crossPackageBehaviors 已登记 owner 边界）。
- **取舍 C（随态文案）**：左右槽文案切换机制由本包承载，值域只引用 `kidpal_voice_states` 冻结闭集、零增删改；新增态文案（如思考「不等了」若闭集缺）归承载该态的包按冻结映射走，本包不代填（KB-E2）。

## 阶段划分

1. 三槽结构 + 常驻 chrome 改造（KB-A1）——渲染面，host 齿先行；
2. 页点机制 + 覆盖层隐藏（KB-A2/KB-A4，依赖 key-layer-dispatch 的 layer_top）；
3. 随态文案接线（KB-A3）+ 全量回归（KB-A6）；
4. 真机确认轮（KB-A5，human 持板）。

## 铁律约束

- `docs/contracts/**` 与 `docs/requirements.md` 只读；发现冲突走 spec-lane 逃生门。
- `kidpal_voice_states` 冻结闭集零偏移（KB-E2）；页点数量 deviation 只能以显式登记形态存在，不得静默偏离 §2.2。
- keybar 只读 layer_top 显隐、零覆盖层进出裁决权（KB-E3，owner = key-layer-dispatch）。
- 页点 4→3 收口归 M2（KB-E1）——本包合并时 M2 未动，deviation 是**暂态、有对账锚**。

## demo 证据形态复述（与 roadmap 一致）

模拟器截图（三槽条各态形态、覆盖层页点消失）+ host runner 输出（相对基线零新增红 + 新齿全绿）+ 真机轮（human 持板）。
