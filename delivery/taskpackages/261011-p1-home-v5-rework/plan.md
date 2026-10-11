---
rigor: full
riskTier: high
sourceType: human-ad-hoc
reviewed-by: architect（triage 分流 review，2026-10-11，同会话 return）
date: 2026-10-11
changeId: 261011-p1-home-v5-rework
origin:
  type: human-ad-hoc
  upstreamType: human-ad-hoc
  upstreamRef: delivery/changes/intake/261011-p1-home-v5-rework.md
  upstreamFrozen: "spine anchors.req 已回填（requirements v1.61 frozen 2026-10-11，§9 v1.61 行在位 = 定稿载体）；anchors.contract 待契约门签后回填 packet id；anchors.arch = 不动（无镜像句，architect 取证在案）"
  workstream: ""
---

# plan · 261011-p1-home-v5-rework（P1 首页落地 v1.61）

> reviewed plan（architect 2026-10-11 分流 review 产出，主线程只搬运不改内容；触点① human 三答确认：②单包 Full / 数组+空数组语义 / V5 范围收窄知悉）。
> 范围三件 + 契约同步：① 空态话术渲染（S-F1.1 AC5/AC6/AC7 两态分叉）；② 明天区多项化（preview 单项→数组，学校课「XX:XX 上学」+ 课外班 + 临时事件同列、按开始时间升序）；③ chip「明早」→「明天」定名（V5 重排收窄后剩余项）。**V5 重排范围经取证收窄**：home_p1.c 已非 V4 布局——ui-v5-align 阶段 2（70ade0b）与 260926 真机对齐轮（6ed6b67、efa8513）已落地 14px 行流文本列表、英文日期「Www DD Mon」（:938-997）、黑帽标+云朵，倒计时贴纸/大 imgfont 时间已退役（:615-618 注记）。

## 字段给值（planner intake 权威消费，勿回退重归一）

```jsonc
"rigor": "full",
"sourceType": "human-ad-hoc",
"origin": {
  "type": "human-ad-hoc",
  "upstreamType": "human-ad-hoc",
  "upstreamRef": "delivery/changes/intake/261011-p1-home-v5-rework.md",  // spine（主线程已建档）
  "upstreamFrozen": "spine anchors.req 已回填（requirements v1.61 frozen 2026-10-11，§9 v1.61 行在位 = 定稿载体）；anchors.contract 待契约门签后回填 packet id；anchors.arch = 不动（无镜像句，architect 取证在案）",
  "workstream": ""
},
"riskTier": "high",
"gateProfile": ["contract-drift", "test-quality", "docs-sync"],
"planningImpact": "none",
"deliveryImpact": "milestone-affecting",
"sourceRefs": ["docs/requirements.md:S-F1.1", "S-F1.1-AC2", "S-F1.1-AC5", "S-F1.1-AC6", "S-F1.1-AC7", "delivery/changes/intake/261011-p1-home-v5-rework.md"]
```

说明：sourceType=human-ad-hoc 贴同型先例 260929-char-window-v5-trim（需求面档 1 定稿 → spine → dev 包，origin 同形）；sourceType=proposal 不合法——upstreamRef「requirements.md §9 v1.61 行」非 proposal 路径，G3 taskpackage-required-check.mjs isProposalByType 会红。story 引用以锚点形态进 `story` 字段：`S-F1.1 · 常显首页看「接下来干嘛」（AC2/AC5/AC6/AC7）`。

## 范围清单（逐文件 changeManifest 种子，仓根相对）

**契约面**（human 契约门批准后同 PR 落，CCP 逐字 patch 先例）：
- `docs/contracts/interface-contract.md` —— §1.3⑥：preview 单对象 → 数组形状 + 空数组语义（[]=确认明日无项）/null 容忍（=未提供）+ 分组口径句扩「明日全量进区（学校课合并『上学』与课外班、临时事件同列）、按开始时间升序」+ 示例值「明早」→「明天」（time 值保持 HH:MM——K1 裁决口径「载荷钉 HH:MM、自然语言标签不入 wire」，home_upcoming.h:30-31）+ 契约变更记录行 append

**server 子面**：
- `server/gateway/schedule.py` —— `home_upcoming_payload`（:433-448）：preview 改为 `group_upcoming(model, tomorrow_iso, "00:00", school_start)` 全量逐项 `{time,name}`（升序由单源 :302 保证，零新排序逻辑；明日「上学」合并项由周中合并分支 :311-314 自动承载）；明日无项 → `preview: []`（不再 None）；docstring 同步
- `server/gateway/front/front_app.py` —— 核证面，预期零 diff（签名未变；三推送调用点：建连推约 :685-694 / 零点+即时推 :708-769 / WS 建连触发 :1005）
- `server/gateway/schedule_push.py` —— 核证面，预期零 diff（单源 helper :117-121 同参）
- `server/gateway/display_schema.py` —— 核证面，预期零 diff（home view 无 upcoming shape floor，已核 :81-85）
- `server/tests/test_schedule_producer_units.py` —— preview 齿改多项形状 + 空数组齿（:117-135 既有 preview 齿重钉）
- `server/acceptance/test_m3k1_schedule_producer.acceptance.py` —— M3K1-A3 frozen shape 齿更新（授权链见风险 R3）
- `server/acceptance/test_tech260928_school_start_wiring.acceptance.py` —— 核证面，预期零 diff（TECH-A1/A2 同参齿保绿，勿重复修）
- `server/acceptance/test_schedule_grid_restore.acceptance.py` —— 核证面，预期零 diff（K6 推1 同参齿）
- `server/acceptance/test_m3k3_voice_schedule_entry.acceptance.py` —— 核证面，预期零 diff

**firmware 子面**：
- `firmware/main/display/home_p1.h` —— seam 签名形改 + preview 容量常量（`HP1_PREVIEW_MAX` 建议 8，与 HP1_ITEM_MAX 同级独立）+ 冻结契约 supersede 注记
- `firmware/main/display/home_p1.c` —— s_preview 单项 → 多项缓存；preview 访问函数多项化；空态两态渲染（AC5/AC6 话术 + AC7 未达提示）；明天区多项行流；「明早」chip →「明天」消费切换；day_rollover 空态语义（见设计要点 4）
- `firmware/main/display/home_upcoming.h` / `home_upcoming.c` —— `kup_parse_preview` 数组解析 + 空数组语义 + 旧单对象形状容忍（部署缓冲，见设计要点 2）；形状注释同步
- `firmware/main/mcp_server.cc` —— 核证面，预期零 diff（:853-869 只经 `kidpal_home_upcoming_apply`）
- `firmware/main/test/schedule_grid.acceptance.c` —— A5 语义权威段重钉 + seam 签名一致性齿（:1037）同步 + preview 多项/空数组齿新增
- `firmware/main/test/p1_home_battery.acceptance.c` —— inject 调用点适配（:693/:771）+ preview 齿适配
- `firmware/main/test/p1_cjk_name.acceptance.c` —— inject 调用点适配（:748-791）
- `firmware/main/test/schedule_grid_restore.acceptance.c` —— 核证面，预期零 diff（:664 contains 齿）
- `firmware/main/test/voice_round_ui.acceptance.c` —— 核证面，预期零 diff（TEST 8 tooth 2 dash/dot 扫描齿保绿）
- `firmware/main/test/host_app/screenshot/home_p1_screenshot.c` —— 截图接线核证/适配
- 资产：spiffs 资产「明早」chip 重烘为「明天」（沿既有文件名换内容最省，先例 efa8513 帽标重烘；重烘后全量 bin 零漂移核证 + **app+art 双分区烧录**，K4 教训）
- 归档件 `firmware/main/test/acceptance/archive/home_p1.acceptance.c` —— **不动**（archive 纪律：移位即退役），supersede 由 home_p1.h 注记 + schedule_grid.acceptance.c REVISION LOG 承载

**设计稿**：
- `docs/ui/kidpal-ui-design-v5.html` —— 三处内容级同步：§2 注（:246）、明天区块注释与内容（:761-763）、规格列表（:784-785）——「上学」行示例置首 + 空态话术占位 + 「仅课外班与临时事件（学校课不上此区）」旧口径句（v1.57 OQ2）改判为 v1.61 全量口径

**不动面核证清单**（exitCriteria 种子）：需求面/架构面零 diff（spec-anchor-check 零新漂移）；mcp_server.cc / display_schema.py / front_app.py / schedule_push.py / 四件 server acceptance 预期零 diff 复跑实证；归档目录零触碰。

## 关键设计要点

1. **契约 preview 多项形状（候选，终形由契约门裁）**：`"preview": [ {"time":"08:00","name":"上学"}, {"time":"19:00","name":"编程"} ]`；`[]` = 确认明日无项（AC6 空态话术的显式 wire 载体）；`null`/缺省 = 未提供（容忍 legacy，向后兼容不变）。**为什么必须引入空数组语义**：现状 producer 明日无项发 `preview: None`，与「未推送」不可区分——AC6 话术无 wire 载体。数组化天然承载三态：有项列表 / `[]` 确认无项 / 缺省未达。
2. **固件解析兼容旧形状一个过渡版**：`kup_parse_preview` 遇旧单对象 `{time,name}` 容忍为 1 项列表（部署缓冲：server 先部署、固件后烧，中间不出现 upcoming 整包拒导致首页日程全丢）；契约文本只定义新数组形状，单对象容忍记为板端防御注记，不作契约双形状。
3. **seam 形改**：`kidpal_home_p1_schedule_inject(day_key, items, count, next_day_items, next_day_count)`（末参从单指针 → 数组+计数，同形改语义）；新增 `HP1_PREVIEW_MAX`（建议 8）；`kidpal_home_p1_schedule_preview` → 多项访问族（preview_count + preview_item(index)，home_p1.c 内部缓存同形）。超容 = 整包拒（沿用纪律）；渲染溢出 = 行流 clip 进卡区 containment（既有 cy1=242 纪律），「占位不破布局优先级」由 clip 兜底。
4. **空态判据（固件侧状态机，本包最重设计点）**：每区 tri-state——(a) 已注入且有项 → 列表；(b) 已注入且无剩余项 → 无安排话术（接下来区「今天接下来没有安排啦」= `s_has_day=1 && count==0`；明天区「明天没有什么课喽」= preview 已注入且为空）；(c) 从未注入 → 数据未达友好提示（AC7，回引 P2 空态口径）。**day_rollover 语义决定**：跨 0:00 清缓存后应转 (c) 数据未达态（清 `s_has_day` 或等价），待每日零点推再注入——否则 0:00 到推送间隙会误显「今天接下来没有安排啦」（AC5×AC7 冲突点，验收齿必须覆盖）。
5. **明天区「上学日」联动**：AC6 空态话术仅非上学日可达（上学日 group_upcoming 周中合并分支自动出「上学」项，明天区必然非空）——验收须构造非上学日场景（周末或学校表无该星期行），防话术齿 vacuous。
6. **「明天」chip 定名**：资产重烘（沿既有 `p1_chip_mingzao.bin` 文件名换内容，或新名+art_assets 消费点同批；登记遗留：home_p1.c:1127-1131「资产闭集只有明早字样」）；契约示例值「明早」是 wire JSON 标签旧漂移，顺修为「明天」+ time 保持 HH:MM；chip 文案（渲染资产）与契约示例值（wire 值）两处不同性质、同批一次收口。
7. **话术渲染走 [S1] 字形 seam**（14px puhui 行流，同 v1.58 纯文本款），无新资产需求；chip「明天」为唯一重烘件。
8. **home_p1.c 纯净性扫描约束**：既有逐字节拷贝、禁子串误触纪律（home_p1.c:186-189 注记）——新代码沿纪律。

## 影响面（受影响的既有验收/单测清单）

- **firmware host runner 全量**：`schedule_grid.acceptance.c`（A5 族 + seam 签名齿 :1037 + E2 零修改纪律齿 :1044 语义随 supersede 授权更新）、`p1_home_battery.acceptance.c`（PH 族 inject 调用）、`p1_cjk_name.acceptance.c`（PCN 族）、`schedule_grid_restore.acceptance.c`、`voice_round_ui.acceptance.c`（TEST 8 tooth 2）、host_infra_smoke、keybar 等全量回归；截图件适配。
- **server host-fast**：`test_schedule_producer_units.py`、`test_m3k1_schedule_producer.acceptance.py`、`test_m3k3_voice_schedule_entry.acceptance.py`、`test_schedule_grid_restore.acceptance.py`、`test_tech260928_school_start_wiring.acceptance.py`。
- **live/真机半**：ECS front 部署（server 改动上 ECS，ecs_put 部署姿势在册）；真机看屏轮（human 持板：空态两态、明天区多项、chip「明天」）；app+art 双分区烧录。
- **机器门**：spec-anchor-check、unit-run-check / acceptance-run-check、mutation-check、closeout-check、supervisor-verdict-gate（high → 升级 human）、前置升级闸。

## 风险与红旗

- **R1 空态三态跨端耦合（最重）**：producer 必须在明日确无项时显式发 `preview: []`；若仍发 `null`，AC6 话术永不触发。齿双向钉：`[]` → 话术；`null` → 未达提示；验收两态都要构造到。
- **R2 契约/固件/云端不同步窗口**：旧解析遇新数组 preview = 整包拒 → 首页日程全丢。缓解 = 要点 2 过渡容忍 + 同批部署（ECS 先 + 同会话烧录）。
- **R3 冻结面修订授权链**：seam 形改触碰多份冻结验收件（schedule_grid A5 段、p1_home_battery/p1_cjk_name 调用点）——超出 tighten-only 窗口（改签名非纯加严）。授权链 = human 契约门 CCP 明示 seam 同形改 + v1.61 AC；各冻结件 REVISION LOG 引 CCP id + §9 v1.61 行留痕；acceptance-author 派发 prompt 明示授权，防 tighten-only 误拦或审查打回。
- **R4 资产重烘零漂移**：重烘后全量 bin git diff 核证仅 chip 一枚 diff（先例 260929 exitCriteria 同款）；烧录双分区纪律（K4 教训入包）。
- **R5 school_start 勿重复修**：TECH-260928 已收口，改动中保持三推送点同参调用、TECH-A1/A2 齿保绿。
- **R6 spec 内过期对账注**：requirements v1.61 补充规则中 v1.57 ⚠️ V5 对账块与实现实况漂移（triage 取证）。本批不动需求面（human 已拍）；已登 spine openQ q4 = 后续勘误批候选，防误导 scoping。
- **R7 triage 取证出入备注**：front_app.py 实路径 server/gateway/front/front_app.py；V5 重排范围收窄（见头部）；sourceType=proposal 机械不合法（G3 isProposalByType）、改 human-ad-hoc。

## launchPreconditions 核对

- spine anchors.req = **v1.61 已 frozen 在位**（§9 v1.61 行 = 定稿载体，commit 7f68b51）✓
- anchors.contract = packet —— **契约门先签后 launch**（facePlan 顺序 2；spine facePlan 中契约面排为实现面前置，未回填则承载包不得 launch，粒度 = 包）
- UI-ready 分形判据：本包触 UI、出生自需求批 v1.61 → 对齐本批 requirements 版本号 ✓；uiuxLevel = **L1**（既有蓝图 + 组件模式 + 状态/文案变化，无新设计语义——「明天」chip 与多项行结构 v5 母版已在位）；designRefs 引 `docs/ui/kidpal-ui-design-v5.html` P1 节（内容级同步在包内完成后即为对齐基准）
- crossEnd：跨端申报必须非空——counterparts 含 firmware 侧（home_upcoming.c `kidpal_home_upcoming_apply`）与 server 侧（schedule.py `home_upcoming_payload`）双向 path+symbol
- governance 面：无前置

## 下一步（facePlan 摘要，全文见 spine）

1. 契约门（先行）：architect-reviewer 四分类 → CCP 起草 → human 签 → 回填 spine anchors.contract
2. planner `/build-feature 261011-p1-home-v5-rework`：acceptance-author（红基线 + 冻结件修订授权）→ builder → verifier↔fixer → integrator（host 全量 + ECS）→ 真机看屏轮（human 持板）→ supervisor（high 升级 human）→ 前置升级闸 → 本地 main 合并
