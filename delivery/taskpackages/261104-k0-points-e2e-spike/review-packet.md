# Review Packet — 261104-k0-points-e2e-spike

> integrator 收口（2026-11-05，本树 dev/261016-sd-card-mount，包级 base ref 22b30654）。
> **收口形态 = 离线面收口 + 诚实挂起，非 ready-to-merge 宣告**：LP8 全项 pending（K0a
> 261104-k0a-four-page-skeleton merged 之后）、live-manual 四条 pending（human 在场轮）。

## Change Size

- Files changed: 15 tracked modified + 12 untracked new（实现面 11 server 文件 +490/−21、
  8 firmware 文件；新件 = points.py 482 行、points_page.c 915 / .h 110、冻结验收两件
  1236+995 行、units 490 行、red-baseline 冒烟两脚本、包级投影与事件文件）。
- Net diff: 全 diff（含 delivery 投影）+916/−268 tracked，另上述新件。
- Scope: server points 最小写入（points.py 单 writer）+ display_schema 白名单 7→9 与来源
  门禁 + front 薄意图截获/导航展开/分页应答 + workspace-kidpal AGENTS.md 积分纪律 +
  firmware view 路由 additive 7/8 + points_page 消费 + mcp_server 消费接线 + protocol
  kidpal_points_page 上行原语。
- Split recommendation: 不拆——spike-first 单包最小通路，mergeOrder 71（K0a 先合）。

## Verification Evidence

- Structural gates: verify.mjs **8/8 全绿** @ 2026-11-04T23:52Z（taskpackage-required /
  change-manifest / firmware-build / contract-lint / acceptance-coverage / mutation-check /
  docs-sync / security；verifier-round fact = `verify-runs.jsonl` 末轮，machine projection =
  `current/verdict.json`〔non-bearing〕）。其中 firmware-build = ESP-IDF **device build 真编译
  绿**（末轮 ninja 增量零重编 = 编译产物与当前源一致 → mcp_server.cc / protocol.cc /
  points_page.c / view_router.c 设备侧编译证据在案，builder 轮「cpp 未经编译器验证」的诚实
  缺口已被该闸覆盖收窄；host build 仍按 LP8 后置）。
- Acceptance/unit/api/ui/e2e: scoped final（范围圈定清单 =
  `delivery/taskpackages/261104-k0-points-e2e-spike/test-scope-final.txt`）——
  server 新件 6/6 + units 51 + record_dialogue_guide 17 = **74 passed**；m4k1 53（§1.6 锚
  重钉后全绿）+ record_cancel_lifecycle + record_front_polish + schedule_grid_restore =
  **69 passed, 3 subtests passed**（合计 143 passed / 0 failed）；firmware WSL gcc 实编通道
  **COMPILE_EXIT=0 + F1/F2/F3 PASS，F4 仅剩 LP8 后置 CMake 登记守一条红**（唯一红因 =
  host_app/main/CMakeLists.txt 未登记，K0S-E4 有意挂起）。全量回归 = 人工触发位未跑
  （基线锚 395 passed @ 6ce77123 引用不重跑）。
- Security gate: securityReviewRequired=false，闸绿。结论一句：**无扩大写权**——state 落盘
  单源于 `KIDPAL_POINTS_STATE` env（front 与 CLI 同名 env、无第二账本路径），CLI 输入校验
  （非整数/越界/缺 env 拒写）由 K0PTS-A6 验收覆盖，畸形薄意图拒绝面由 K0PTS-A2 覆盖，
  零新增依赖（points.py 纯 stdlib、零 fastapi）。
- Mutation check: 28 条可执行变异全敏感（27 初轮 + A2-2 经 acceptance-author 锐化冻结 case
  后复放转敏感）+ F4 CMake 登记守 deferred（LP8，登记行未在世）；变异不适用 = L1..L4
  live-manual 四条（structural 层，真实消费端目击是价值本体）。收尾零残留（8 文件 sha256
  一致 + C:/tmp 第二账本已删）。
- Docs sync: docsSyncRequired=false 豁免成立，闸绿；判定详见 `docs-delta.md`（architecture.md
  组件级登记已随上游 261103 arch 批落齐，spec/契约面零 diff、零新增文档债）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| server/gateway/points.py:195 | 最小可靠写（进程内锁 + tmp+replace），不保证跨进程原子 | 单 writer 正式化（跨进程锁/幂等/余额重算不变量）在同一文件补齐 | K1 261104-k1-points-ledger-core launch |
| firmware/main/mcp_server.cc:1944 | points_home 最小渲染（SetChatMessage 一行：权威余额 + 最近一笔事由） | 完整积分主页版式（金币/进度线/刻度，V9 points-home.png） | K2 261104-k2-points-device-ui launch |
| server/gateway/front/front_app.py `_POINTS_PAGE_STEP=1` | 分页步进恒 1（每请求下发 1 条，首尾 canary 离线闭环形态） | 满窗翻页（设备缓存容量 6）与步进正式化 | K2 261104-k2-points-device-ui launch |
| firmware/main/protocols/protocol.cc:223 `SendKidpalPointsPage` | 上行分页请求原语在世但无运行时调用方（设备侧暂无 UI 触发点） | 设备详细账本页的翻页触发接线（front 应答消费链已离线闭环） | K2 261104-k2-points-device-ui launch |

（前两条为源内 `minimal:` 标记收集；后两条为收口 review 补记的同一升级轨迹。无 upgrade
trigger 的条目：无——四条均有明确触发包。）

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | firmware/protocol | `SendKidpalPointsPage` 定义在世、front 消费链闭环，但设备侧无运行时调用方——K0 真机轮设备不会自发发起分页请求，live-manual K0PTS-L4 的首尾 canary 需依赖 K2 的触发点或人工注入路径取证 | changed hunk protocol.cc:223 + crossPackageBehaviors（K2 owner「P3 静态占位到最小渲染接管」）；与 K0PTS-F4 冻结锚口径一致（帧构造在世 = 驱动方锚），非缺陷 | K2 接线触发点；K0PTS-L4 取证路径在 live 轮明确（不伪造） |
| NIT | judgement-call | server/front | `POINTS_STATE_PATH` front 侧带默认值 `/opt/kidpal/points.json` 而 CLI 侧缺 env 即拒写（exit 2）——env 未设时两入口行为不对称（front 落默认路径、CLI 拒跑） | changed hunk front_app.py POINTS_STATE_PATH + 一致性启发；无第二账本风险（同名 env、同一路径常量），仅不对称 | no-trigger（deploy 面按 RUNBOOK 设 env；K1 正式化时顺手统一） |

结构闸 PASS/FAIL 不复述为 finding（约定级约束）；gate 全绿仍执行了五轴语义 review（下节）。
无 BLOCKING、无 SHOULD。

## Five-Axis Review

- **类型/边界**：points.py 全入口显式类型门（`_is_positive_int` 显式排 bool——bool 是 int
  子类的经典坑已堵）；`read_state` 对损坏账本 raise ValueError 绝不静默当空账覆盖（数据
  丢失面守住了）；C 侧 parse 全路径畸形降级 -1、mcp 消费失败显式降级不盲渲染（V27 纪律
  同型）。restore 双向语义（front 追加键 + 设备分流）与 D-07 对齐，A4 变异正反两向均敏感。
- **错误处理**：拒绝路径三不原则一致（不落账/不推帧/不透传）；CLI 拒绝非零退出 + stderr
  一次性可判说明；`_emit` 编码退化防 cp1252 假失败；推送失败不改变「已消费」语义（控制
  帧恒不透传，异常只记日志）——正确取舍。
- **契约/单源**：白名单 7→9 尾部 additive、0..6 零偏移双向锚（display_schema + view_router）
  一致；GATEWAY_DECISION_CONTROL_ACTIONS 双路径恒拒与 FORWARDABLE 不相交；分录六键、
  page 四键、target 四键逐字对契约；balance 由分录重算不信任磁盘副本；分页 keyset
  (occurred_at,id) 倒序、reset 语义、跨版本拒绝与契约 §四 逐条对齐。无第二账本旁路
  （front 与 CLI 同一执行器 + 同一 env 名，A6 断言钉死）。
- **并发/资源**：C 侧消费缓存单例 + 注释载明竞态归因（mcp 回调同步在接收任务、渲染经
  Schedule 归主任务）；0.7KB 载荷堆分配遵守 ssl_receive 4096B 栈纪律；points 写入进程内锁
  持锁面最小（read+write），K1 补跨进程锁已记账。
- **安全**：见 Verification Evidence security 行——无扩大写权、零新依赖、注入面（控制帧
  透传/伪造余额）被来源门禁与决定类恒拒双层封死；workspace-kidpal AGENTS.md 的「不产
  余额/revision/不宣称已记账」纪律锚落盘并被 A3 断言钉死。

**UI design-review 证据（uiux level 2，纯文本，bootstrap 期无仓内视觉资产）**：viewport
coverage — K0 设备消费面 = SetChatMessage 一行文本（星星 N 分 · 最近一笔事由），非版式面；
a11y — 文本行可读、无色彩/字号决策点；design-drift — 与 `docs/ui/V9/points-home.png` 的
完整版式差异 = 有意 drift（minimal 裁定记 mcp_server.cc:1944 + planner ruling，完整版式归
K2）；external artifact — 无；residual risk — 看屏真实观感（字号/可读性）随 LP8 真机轮一并
取证，本包不下结论。

## Residual Risk

- Known limitation: ① **LP8 全项 pending**——`firmware/main/test/host_app/main/CMakeLists.txt`
  登记追加、host build、device build 基线重取、真机终验、ready-to-merge、F4 CMake 登记守
  变异实放，全部等 K0a(261104-k0a-four-page-skeleton) merged 之后（K0S-E4）；本包 P3 接管
  使用 K0a 留下的缝，未触碰 K0a 任何文件。② **live-manual 四条 pending**——K0PTS-L1（真实
  语音记账目击）/L2（真实 QQ 双状态 canary）/L3（NFR-PERF-4 实测时延）/L4（首尾 canary 看
  屏），载体 `spike-evidence.md` **未创建**（human 在场轮产物，收口不伪造）；QQ 取证路径按
  LP7（门 1 裁定的最小 producer 链）执行。③ cpp host 编译证据缺口（host build LP8）。
- Follow-up recommended: K1 单 writer 正式化（跨进程锁/幂等/重算）、K2 完整版式 + 分页触发
  接线 + 满窗翻页、K3 正式 QQ skill、CLI/front env 对称性顺手统一。
- Human decision needed: 无新增——本包无 `decisionCoverage` 中 `landsIn: dropped` 且
  `reason` 以 `risk-accepted` 开头的条目（七条 decision 全部 landsIn 到 exitCriteria 或
  K1/K2/K3 承接包），无 risk-accepted 台账项可列；ready-to-merge 判定本身按 K0S-E4 等 K0a
  merged 后由后续轮作出。

---

## 附件 · 2026-11-05 真机轮收口（planner 补记，human 在场裁定）

- **真机轮证据**：见 [spike-evidence.md](spike-evidence.md)（语音记账 ×2、QQ canary ×1、「打开积分本」权威帧轮、时延实测 语音≈8s / QQ≈45s、「10」残段定性）。
- **验收判据修正（human 裁定）**：K0S-E1/E2/E3 判定面从「设备可见版式」修正为「真值链 + 最小渲染形态」；原判据隐含专属积分页前提，与 K0 最小渲染范围不符。修正后：E1 达成、E4/E5/E6 达成、E2/E3 按降级申报（设备分页触发点与 QQ 缓存同步推送归 K2/K1，验收件 GAP-1/GAP-3 预申报兑现）。
- **K2 输入（版式第一课）**：最小渲染行宿主 `chat_message_label_` 挂 `bottom_bar_`（页点上方），残段每页可见——K2 完整积分页落地时须替换该渲染面并消化残段观感。
- **挂起转正**：LP8 全项完成（共享 CMake 追加 + host 基线 278/3 + device build 真编译 + 本真机终验）；live-manual 四条按修正判据落账（L4 降级申报）。
