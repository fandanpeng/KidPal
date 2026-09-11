# device-round-record — 260911-key-route-a1（PKG-1 键位路线落地 · KR-A5 真机确认轮）

> integrator 归档 · 2026-09-11 · 分支 `dev/260911-key-route-a1`（base = main `d087fb7`）。
> 条目：**KR-A5**（`kind:"structural"`，human 持板 device-round，一等豁免通道，先例 260829-hardware-validation 全 structural）。
> 执行模式由 human 裁定（pod-events `8561409b-…`）：human 选「**现在烧录 + agent 驱动串口捕获**」——agent 经 esptool 烧 COM5 并捕获日志，human 持板操作 KEY/BOOT 手势并口述观察，证据归档本目录。
> **PASS 裁定**（human，pod-events `c0ffe62a-70f3-4080-9945-7b1877459008`，2026-09-11）：KEY(GPIO18) 三手势真机可达、电平/上拉 `active_high=false` 实测稳定（悬空风险销账）、BOOT 回归翻页 + 长按故事翻屏保留、连按消抖计数正常、全程零自触发；HWV-A2 顺带三项核验。human 口述「符合预期」与串口日志逐项互证。

## 1. 烧录事实

| 项 | 值 |
|---|---|
| 端口 | COM5 |
| 分区 | app @ `0x20000`（esptool `write_flash 0x20000`） |
| 载荷 | `firmware/build/xiaozhi.bin`（本包设备构建产物，0x2c28b0 = 2894000 字节，r3 firmware-build `build-pass` 严格模式真编） |
| hash | verified（回读校验通过） |
| 复位 | hard reset（`--after hard_reset`，烧录后硬复位启动） |

## 2. 日志 enum 值对照（读 `firmware/main/key_gesture_parser.h` 冻结纯逻辑逐字核实）

串口日志里的数字裸值，须按冻结 `key_gesture_parser.h` 的枚举读，不能靠直觉映射：

| 枚举 | 值 | 源码事实 |
|---|---|---|
| `key_id_t` | `KEY_ID_KEY`=0 / `KEY_ID_BOOT`=1 | `typedef enum { KEY_ID_KEY = 0, KEY_ID_BOOT, KEY_ID_COUNT }` |
| `key_gesture_t` | `NONE`=0 / `CLICK`=1 / `DOUBLE_CLICK`=2 / `LONG_PRESS`=3 | `typedef enum { KEY_GESTURE_NONE = 0, KEY_GESTURE_CLICK, …LONG_PRESS }` |
| `global_action_t` | `NONE`=0 / `SESSION_TOGGLE`=1 / `HOME`=2 / `PAGE_NEXT`=3 | `typedef enum { GLOBAL_ACT_NONE = 0, GLOBAL_ACT_SESSION_TOGGLE, GLOBAL_ACT_HOME, GLOBAL_ACT_PAGE_NEXT }` |

> 故日志 `key=0` = KEY、`key=1` = BOOT；`gesture=1` = 单击、`gesture=2` = 双击、`gesture=3` = 长按；
> `action=1` = 发起/结束会话、`action=2` = 回首页、`action=3` = 翻页。（`key_gesture_parser` 日志 = parser 一致性观测、
> `key_route_dispatch` 日志 = iot_button 手势回调的权威 dispatch，两者独立事件流。）

## 3. 日志 ↔ 观察互证表（KR-A5 手势面）

| # | human 观察 | 串口日志证据（裸值 + register 位置） |
|---|---|---|
| 1 | **KEY 单击 = 发起对话，语音轮真启动** | `key_route_dispatch action=1`（gestures3 L25, 589215）→ `kidpal_rn: …yielding to xiaozhi SetupUI (voice)`（L26）→ `StateMachine: idle->connecting` + `WS: Connecting to wss://8.137.91.16:8000`（L27-28）→ **`WS: Session ID: bbcb4917-ebaf-4e69-90fb-e6fc3a273773`**（L31）→ `>> 明天星期几？`（L38）→ `<< 明天是星期六呀…`/`<< 可以睡个小懒觉…`（L41/47）。**证明 = 真 WS 会话 + 真 ASR + 真 TTS，非日志自嗨** |
| 2 | **KEY 双击 = 零 dispatch**（全局层无语义） | `key_gesture_parser key=0 gesture=2`（L51/L52, 616655/621985）——parser 观测到双击，但随后**无任何** `key_route_dispatch` 行（`key_route_global(KEY, DOUBLE_CLICK)→GLOBAL_ACT_NONE`，dispatch 层无日志） |
| 3 | **KEY 长按 = 回首页** | `key_route_dispatch action=2`（L54/L60/L89）→ `kidpal_rn: KidPal render node pushing home 400x300`（L55/L62/L90）；parser 侧 `key_gesture_parser key=0 gesture=3`（L59/L64/L94）逐次互证 |
| 4 | **BOOT 单击 = 翻页（主司翻页，去对话化）** | `key_gesture_parser key=1 gesture=1`（L79, 650055）→ `key_route_dispatch action=3`（L80, 650235）→ `kidpal_rn: cycle -> CHAR (P4)`（L81）。去对话化的负向证明 = **无任何** BOOT 单击触发 `action=1`（SESSION_TOGGLE）/`yielding to SetupUI` 的日志；BOOT 单击恒 `action=3` |
| 5 | **BOOT 长按 = 零 dispatch**（故事翻屏接缝保留，P3 页内特例） | `key_gesture_parser key=1 gesture=3`（L104, 667865）——parser 观测到 BOOT 长按，但**无** `key_route_dispatch`（BOOT 长按仍走 `HandleStoryKey(KEY_ID_BOOT, LONG_PRESS)` 页内翻屏直连，非全局 dispatch，接缝原样保留） |
| 6 | **连按 = 每击一 dispatch（消抖有效、零双触发）** | L133–L224（BOOT 连段）：每一个 `key_gesture_parser key=1 gesture=1` 紧随**恰好一个** `key_route_dispatch action=3`，页序每击严格前进一步（CHAR→SCHEDULE→STORY→HOME→CHAR…回绕），无一次按出两条 dispatch、无窜页 |
| 7 | **两段捕获 idle = 零自发事件** | `device-round-keyroute-boot.log` 全部 192 行（60s 复位启动）与 `device-round-keyroute-gestures3.log` 被动手势段之外的 idle 区，**零** `key_gesture_parser` / `key_route_dispatch` 行——GPIO18 悬空零自触发、零抖动脉冲 |

## 4. `active_high=false` 确认回填（悬空风险销账）

- KR-A5 立项风险 = GPIO18 无板上拉/下拉「悬空风险」。真机实测：`active_high=false`（低有效，GP18_Active 0）稳定，无抖动、无自触发（§3 #7 零自发事件 + §3 #6 消抖计数正常）。
- **回填**：该实测结果已回填进板级 `key_button_` 构造实参并冻结——`waveshare-s3-rlcd-4.2.cc` 构造器 `key_button_(KEY_BUTTON_GPIO, false, 0, CONFIG_BUTTON_SHORT_PRESS_TIME_MS, false)`（`active_high=false`），`config.h` KEY GPIO 定义注释同步记「有效电平 = 低（GP18_Active 0）」。**消抖参数复用 `CONFIG_BUTTON_SHORT_PRESS_TIME_MS=180` 与 BOOT 一致**（KR-A1 消抖复用，防漂移）。

## 5. HWV-A2 顺带三项核验（原 260829-hardware-validation 遗留 OPEN 面）

HWV-A2「按键手感（消抖/职责分离符合 §4.0）」在原包历史为 **OPEN**（`changes/dev/260829-hardware-validation/review-packet.md`：长按 story PAGE_TURN 待验 + 消抖连按计数未做 + KEY(GPIO18) 未接只验了 BOOT 半边）。本包 KR-A5 真机轮顺带补全三项：

| 原 OPEN 项 | 本轮核验结果 |
|---|---|
| ① 长按 story PAGE_TURN 冻结语义待验 | **验**：BOOT 长按仍走 `HandleStoryKey(KEY_ID_BOOT, LONG_PRESS)` 故事翻屏接缝（§3 #5 零 dispatch + 接缝原样保留，KR-E1 页内特例零触碰） |
| ② 消抖连按计数未做 | **验**：连按每击一 dispatch、零双触发、页序严格逐步（§3 #6） |
| ③ KEY(GPIO18) 未接、两键分工只验 BOOT 半边 | **验**：KEY 接线完成 + 三手势真机可达 + 职责分离实测（KEY=对话 §3 #1/#3、BOOT=翻页 §3 #4），spec §4.0 两键分工全量落地 |

## 6. 观察项（非本包缺陷，归 PKG-2 归一屏参考）

1. **渲染节点循环序 `home→char→schedule→story`**（gestures3 L81/L143/L148/L157…）：BOOT 单击 `action=3` 后经 `kidpal_render_node_cycle_next()` 驱动的**渲染节点**页循环序是 home→char→schedule→story，与 spec §4.0 冻结纯逻辑页序（`key_page_next` P1→P2→P3→P4）不同。这是**既有 `cycle_next` 渲染实现**的页序（本包只接线 `GLOBAL_ACT_PAGE_NEXT` → `cycle_next`，未动渲染节点页序），非本包引入。**归 PKG-2（260911-screen-unification，归一屏）在其页面体系重构时参考/对齐。**
2. **语音 active 期 HOME dispatch 后前台让位 SetupUI**（gestures3 L55 pushing home 之后 L56 `kidpal_rn: voice active -> show xiaozhi SetupUI (yield KidPal)`；L62→L63 复现）：语音会话活跃期间 KEY 长按回首页后，前台按**既有仲裁逻辑**仍让位回 xiaozhi SetUpUI（语音态优先）。这是既有显示仲裁行为，非本包引入。**归 PKG-2 归一屏「仲裁器显式化」参考。**

## 7. 证据件归档

- `device-round-keyroute-boot.log`（本目录）：60s 复位启动捕获（clean boot 至 idle + WiFi 701 + IP 192.168.3.36，零自发按键事件）。
- `device-round-keyroute-gestures3.log`（本目录）：被动手势捕获（KEY/BOOT 三手势 + 连按 + 语音轮 + 翻页循环，dispatch/parser 双路互证）。
- 两份日志均自 `C:/tmp/` 原样字节拷贝（`cp`，未做 EOL 归一），长度分别为 192 行 / 717 行，合计 909 行。

> 归档人 = integrator（本相位），机器面由 planner 移交；本文件是 KR-A5 的承重 human 证据叙事，判绿依据 = pod-events `c0ffe62a` human PASS 裁定 + 上述日志逐项互证。