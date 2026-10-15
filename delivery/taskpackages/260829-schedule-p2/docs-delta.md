# docs-delta — 260829-schedule-p2（包 6：课表页 今天/明天渲染 + 语音问课表 + 家长录入→可消费链路）

> 性质：功能实现批（firmware+server 双面，full rigor，milestone-affecting，mergeOrder 9，riskTier high）。
> 核心结论：**canonical docs（`docs/spec.md`、`docs/contracts/**`）零改动**——本包是 spec §4.1（S-F1.1/S-F1.2/S-F1.3）与契约 B1/§1.3 ② 的**首次实现履职**，只消费不修改 canonical；verifier 终审 r3.json docs-sync 闸 PASS（docsNoChangeReason: null，acceptance-coverage 全齿在世即担保行为↔文档不漂移）。
> docs-sync 判定（integrator）：**无需同步 spec/notes**，理由逐条见 §A。

## A. docRefs 现状与无需更新理由

| docRef | 现状 | 本包影响 | 无需更新理由 |
|---|---|---|---|
| `.planning/kidpal-mvp/PROPOSAL.md` | FROZEN（sha1:88a5bdb8）：§2 包 6 = 课表页渲染 + 语音问课表 + 录入→可消费 | 逐条落地（P2-A1~A6） | PROPOSAL 措辞与本包交付一一对应，无漂移；收口记走 `.planning/kidpal-mvp/progress.md` append（integrator 例行进度线，非 PROPOSAL 本体回写） |
| `docs/spec.md#§4.1` | S-F1.1（接下来分组）/ S-F1.2（课表页两态）/ S-F1.3（语音问课表）已定稿，含 2026-08-29 human「砍反白高亮」裁定 | 逐字实现（两态圆点、无反白、星标=设备侧渲染标志、BOOT 长按切换） | spec 语义全部被本包行为覆盖，零新增语义需要回写。两处实现级细化不构成 spec 漂移、由代码内注记承载：① group_upcoming「上学」项进行中语义（合并项自 school_start 起至当日下一事项开始前计 upcoming，`server/gateway/schedule.py` docstring 注记，满足全部冻结断言，S-F1.1「合并成一项 XX:XX 上学」未被改变）；② `is_tomorrow` = title 含「明天」的设备侧最小判定（`firmware/main/mcp_server.cc` case 4 注记 + minimal 升级路径，契约 data 形状零变更） |
| `docs/contracts/interface-contract.md` | B1 课表 MD 结构（表体零变更）+ §1.3 ② schedule view data 形状 `{title, items:[{time,name,location}]}` 零增减 | 只读消费 | 本包不发明 view、不加字段：B1 三类区块解析按 §2.3 语义（脏行 ValueError 大声拒绝、过期一次性安排忽略、`remind_before_min` 废止不消费）；`school_start` 走 producer 显式入参 + repo 侧配置载体（`server/deploy/openclaw-schedule.config.yaml`），**不是契约字段、不触 B1 表体**；contract-lint 闸 PASS。星标 = 设备侧渲染标志画在定位行，不进 display data 形状（taskpackage notes 钉定口径） |

## B. 本包落地（非 canonical 文档面）

| 目标 | 位置 | 说明 |
|---|---|---|
| `firmware/main/display/schedule_page.{c,h}` | firmware/ | 新建：冻结离线契约 5 函数强定义（parse / dot_state / page_input / locate / page_render），纯逻辑 + 纯 LVGL 双面同源 |
| `server/gateway/schedule.py` | server/ | 新建：B1 解析/筛选/决策 26 producer 分组纯模块（无 fastapi 依赖，offline 可导入，载荷过真实 `display_schema.validate_display` 门） |
| `server/deploy/openclaw-system-prompt.v2.md` | server/ | 新建：v1 逐字保留 + 课表 producer 扩展（决策 26 分组 + QQ 录入改写约定）；版本化落盘惯例，v1 文件不改可 diff |
| `server/deploy/openclaw-schedule.config.yaml` | server/ | 新建：`school_start` 载体（producer 显式入参的 repo 可测配置，改配置即时生效、永不烧板） |
| `firmware/main/mcp_server.cc` | firmware/ | case 4 路由 ack + 渲染消费（corr_id=kdp-<tick>、畸形 data 显式降级不盲渲染） |
| `firmware/main/CMakeLists.txt`、`firmware/main/test/host_app/main/CMakeLists.txt` | firmware/ | 装配接线（设备构建 + host runner 双编入） |
| overlay 三件（`apply-xiaozhi-overlay.sh` / `OPENCLAW_SYSTEM_PROMPT_CARRIER` / `provider.config.yaml`） | server/deploy/ | prompt 渲染源 v1→v2 切换（声明面/渲染面/消费面三处同步；v1 文件仍在世，VRT-A4/A7 既有验收引用不受影响——`test_voice_roundtrip_b.acceptance.py` 引用的 v1.md 未删除，integrator grep 核实） |
| 冻结验收两文件 + 红基线摘要 | firmware/test、server/acceptance、red-baseline/ | acceptance-author 产出（冻结件，本包只执行不改）；C 侧红基线 `firmware/main/test/acceptance/red-baseline/schedule-p2-red-baseline-summary.md` |
| `changes/dev/260829-schedule-p2/*` | changes/dev/ | closeout/audit 制品：本 docs-delta + review-packet |

## C. deployment-facing 注记（不新增 README 节，理由如实记录）

prompt v2 切换的运维语义已由三处内联注释钉死（`OPENCLAW_SYSTEM_PROMPT_CARRIER` 声明面 + `apply-xiaozhi-overlay.sh` 渲染面 WARN 行 + `provider.config.yaml` 声明面），`openclaw-schedule.config.yaml` 文件头自带 `school_start` 语义说明——延续包 2（260829-voice-roundtrip）确立的「版本化文件 + 载体内联声明」形态，未复用 `server/deploy/README.md` 补节先例（该先例服务的是 engine-patch 首例部署形态的漂移治理，本包无对应风险面）。如 operator 侧需要展开版指引，归 P2-A6 live 真录一轮时一并补（operator 殿后项）。
