# Docs Delta · 260923-m3-k6-boot-auto-connect

> integrator 收拢制品（2026-09-23）。本件回答「本包文档面义务是否已全清」；TaskPackage `docsSyncRequired=false`，逐面核对与事实锚如下。

## 结论

- **TaskPackage 声明 `docsSyncRequired=false`（docsSyncReason 逐字）：「建连时机是行为策略非契约/spec 面——契约 §1.3⑥ 明文推送时机不进契约（architect 2026-09-23 已核 interface-contract.md 确认不触契约面）；spec 未规定连接建立机制，无文档同步义务。」——本包责任范围内成立，无文档同步义务，义务已全清。零 docs 增量。** 本包全生命周期对 `docs/**` 零写入（`git status --porcelain docs/` 实跑零输出，2026-09-23 integrator；server/front/device_state_machine.cc/protocol.h 零 diff 同批实证），docs-sync 闸两轮绿（r3 scoped + r4 final，`ok:true`，stdout `docsSyncRequired:false` 与 taskpackage `docsSyncReason` 同口径回显）。
- **零 docs 面改动的理由**：
  - 本包改的是**建连时机**（boot 激活完成后 + 网络重连时自动 `OpenAudioChannel`），是行为策略——plan.md 契约核查节 architect CONFIRMED：interface-contract.md §1.3⑥ 明文「推送时机（WS 建连/每日零点/新建日程即时）属行为策略，权威承载 = arch §5.2 场景① + §10 决策 26，不进契约」。server 侧建连推送（front_app.py 建连即推首页「接下来」）为既有在役能力，设备连上即生效，server/front 零改动（A-only 范围裁定）。
  - 本包复用既有 hello 握手，零新消息形状——wire 形状不变则契约面（E1 display JSON / B1 课表 MD / 提醒推送语义 / 小智协议消息子集）射程内无新增承载义务。
  - spec 面（`docs/requirements.md` / `docs/architecture.md`）未规定连接建立机制——S-F1.1 需求语义「常显首页看接下来」不受建连时机实现方式影响，shipped behavior 向既有 spec 收敛、零漂移。
  - docRefs `docs/requirements.md#S-F1.1` 锚在世且零修订（冻结齿 TEST 6 tooth 2 canary 断言 requirements 含 `S-F1.1` + 契约文件在世 PASS，r4 final 全绿）。
- 实现细节由代码注释（application.h `auto_connect_in_flight_` / `TryStartAutoConnect` 语义头注、application.cc [K6] 各锚注记）与 closeout 制品（本目录 review-packet.md + 本件 + verdict.json）承载，不另立 docs 文档。

## 文档面变更清单

| 面 | 文件 | 本包动作 | 性质 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md`（contractRefs 唯一条目） | **零触碰**（铁律 K6-E2；只读消费） | 只读 |
| spec 面·需求 | `docs/requirements.md`（docRefs `#S-F1.1`） | 零触碰（既有已验收需求） | 只读 |
| spec 面·架构 | `docs/architecture.md` | 零触碰 | 只读 |
| workstream 过程账 | `delivery/workstreams/m3-schedule/progress.md` | 本包零写（K6 段落账归 planner 真机轮收尾相位，exitCriteria K6-E3 后置义务） | 只读 |
| closeout 制品 | `delivery/changes/dev/260923-m3-k6-boot-auto-connect/`（本件 + review-packet.md + verdict.json） | integrator 收拢制品落盘 | closeout |

## 逐面核对

| 面 | 核对证据 |
|---|---|
| 契约面零写入 | `git status --porcelain docs/` 零输出（2026-09-23 integrator 实跑）；contract-lint 闸绿（r3 scoped + r4 final，exit 0） |
| spec 面零写入 | 同上零输出；docs-sync 闸绿（r4 final stdout `docsSyncRequired:false` 与 taskpackage `docsSyncReason` 同口径） |
| 协议/server/front 零改 | `git diff --name-only \| grep -E "server/\|device_state_machine\|protocol\.(h\|cc)\|websocket_protocol"` 零命中（exit 1）；变更面全部在 firmware/** 与 delivery/**（见 review-packet Change Size）；K6-E2 铁律逐项复核见 review-packet |
| S-F1.1 锚在世 | 冻结齿 TEST 6 tooth 2（需求面 canary + 契约文件在世）PASS（r4 final 面内全绿 + host runner 98/0） |
