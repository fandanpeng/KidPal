# dispatch-progress · builder · 261104-k3-qq-points-channel

> 自包含进度快照（可覆盖重写）。生成 2026-11-14，协调者三条裁定（R1/R2/R3）后已增补重跑。
> 状态：**实现完成，待 verifier 独立判**。

## 产出（实际 diff，全部 ⊆ changeManifest）

| 文件 | 动作 | 内容 |
|---|---|---|
| `server/deploy/openclaw-skills/workspace-main/kidpal-points-write/SKILL.md` | 新建 | 六子命令面 + 钉死解释器/state env/`-m gateway.points` + 恒调 CLI 不吞重 + command_id 造法与回念（含 delete 重放承诺）+ 一次性普通文本报错表 + 兑换纪律 + workspace-kidpal 只读边界 + `reason` 的 `%XX` 编码回话纪律（R3） |
| `server/gateway/points.py` | 改 | 六子命令 CLI（手写解析）、`revision`/`command_id` 回执、`_delete_entry_locked` + 公开 `delete_entry` 转调、**删除的 command_id 重放保护（`delete:<command_id>` 命名空间回执）**、`RECENT_LIMIT_DEFAULT=5`、`_cli_value` 空白编码、`_POINT_ENTRY_ID_RE`、state 形状注释更新 |
| `server/tests/test_points_cli_units.py` | 新建 | **35** 个 unit 测试（回执键集完整相等 / 拒绝四联 / 锁内性行为证明 / 两类幂等 / **删除重放与回执命名空间隔离 6 条** / 只读不建文件） |
| `server/deploy/manifest.yaml` | 改 | kidpal-points-write 条目（src/dst/owner: deploy.sh/verify: md5/restart: none） |
| `server/deploy/openclaw-skills/workspace-main/AGENTS.md` | **零 diff（R2 已由协调者核定）** | 该工作区靠 SKILL.md 目录自动发现，AGENTS.md 无 skill 注册表（§50/§124 只是通用指引），课表 skill 同样未登记；积分纪律已在 SKILL.md 单源承载 |
| `server/gateway/front/front_app.py` | **零 diff** | 观察缝（`:842`/`:887`/`:2363`/`:2371`）与 delete+page handler（`:1049-1149`）已满足 A6 全部断言；`_push_display_payload`（`:597`）与轮询循环结构未触 |
| `server/deploy/deploy.sh` / `verify-deploy.sh` | **零 diff（表驱动）** | A7 表驱动回归守卫本就绿；加 manifest 条目即生效 |

## 协调者裁定后的增补

### R1（代码）：删除的 command_id 重放保护

- 复用**既有** `state.receipts` 表，键 = `delete:<command_id>`（add/redeem 仍走裸 command_id）；
  `_replay_receipt` 只看裸键，永远看不到删除回执。
- 首次真实删除把 `{id, deleted:true, revision, balance}` 记入该命名空间；同 command_id 再来 →
  **在序号换算之前**短路返回首次结果（首删后序号已漂移，先换算就会删到另一条），不再删除、
  revision 不变。
- `deleted=false` 的幂等 no-op 保持**零写入、不记 receipt**（冻结契约与文档同钉）。
- 反向隔离：读删除回执时要求 `deleted` 键在场（否则视为 add/redeem 分录回执）；写删除回执时
  若命名空间键已被 add/redeem 回执占用（病态撞键，command_id 字面量以 `delete:` 开头），
  **不覆盖**对方、本条删除回执降级为不持久并打 WARNING——add/redeem 的传输幂等绝对优先
  （R1「不改 add/redeem 既有重放语义」）。
- add/redeem 的既有重放语义零改动；删除回执与分录回执同表不同命名空间，无第二套存储。

### R2（文档结论保留）：AGENTS.md 保持零 diff

协调者已核 workspace-main/AGENTS.md 确无 skill 注册表，判定成立。若今后要一条家庭面纪律
（如「QQ 积分只经 skill、不手改账本」），请回派明确写入面——本包不擅自加第二语义源。

### R3（文档）：SKILL.md 补 `%XX` 编码回话纪律

已补：回执里的 `reason` 对空白做 `%XX` 百分号编码（`老师 表扬` → `老师%20表扬`），**回话必须
用家长原话，绝不把编码串念给家长**；也不自己解码后改写事由语义（编码只是单行 key=value 的
传输形态）。同时在 command_id 节补一句「delete 也吃重放保护，同 ID 重试不会删第二条」。

## 实跑命令与结果（裁定后复跑）

```
python -m pytest -q --import-mode=importlib server/acceptance/test_qq_points_channel.acceptance.py
  -> 20 passed in 6.65s

python -m pytest -q --import-mode=importlib server/acceptance/test_points_*.acceptance.py
  -> 20 passed, 1 warning in 3.53s

python -m pytest -q --import-mode=importlib server/tests/test_points_cli_units.py
  -> 35 passed in 0.46s        （原 29 + 删除重放/命名空间隔离 6 条）

python -m pytest -q --import-mode=importlib server/
  -> 779 passed, 9 skipped, 60 subtests passed in 56.80s
```

## 变异自检（4 处，均已还原并复跑全绿）

| 变异 | 期望 | 实得 |
|---|---|---|
| 回执字段 `revision=` 改名 `rev=` | A1 回执断言红 | K3Q-A1 add/redeem 2 红 + unit 4 红 |
| index 换算挪到 `_critical` **外**（TOCTOU 窗口） | 锁内性断言红 | `test_index_resolution_and_delete_share_one_in_locked_read` 红（`2 != 1` 次 state 读）；A4 源锚仍绿——unit 层比源锚多钉一层行为 |
| `_delete_entry_locked` 改名 `_delete_entry_unlocked` | A4 源锚红 | `test_k3q_a4_index_resolution_locked_source_anchor` 红 |
| **R1**：`replayed = None`（关掉删除重放守卫） | 删除重放断言红 | `test_delete_index_replay_returns_first_result_and_deletes_once` + `test_delete_entry_id_replay_is_guarded_too` 红（33 passed / 2 failed） |

## 清单外阅读申报

| 清单外文件 | 原因 |
|---|---|
| `server/acceptance/test_points_e2e_spike.acceptance.py`（K0PTS-A6 段） | 确认位置参数 CLI 兼容面与「拒绝非零退出」既有断言，避免改 CLI 打断 K0 冻结件 |
| `server/acceptance/test_points_ledger_core.acceptance.py`（K1L-A3/A5 段，含 receipts 内部形状条款） | 确认 `delete_entry` 签名禁用参数与「回执内部形状为实现自由」——R1 才能安全复用 receipts 表 |
| `server/tests/test_points_spike_units.py` | 既有 unit 断言「add 后同目录恰 `.lock`+`state.json`」与 CLI 参数面，新 CLI 必须不破 |
| `delivery/taskpackages/261104-k3-qq-points-channel/dispatch-progress-acceptance-author.md` | 对齐进度快照格式 |

## 行为面观察（申报节）

- **未运行**：本包 front_app.py 零 delta、无固件路径，未起真机轮、未读 KPLOG/串口。
- 离线串链只证「`run_points_observe_once` 经 `_push_display_payload` 恰推一帧 `points_home`
  （`restore=true`、revision/balance 权威）」。**host 与真机可能不等价的接缝**：真机侧「其他页
  不抢前台 / 积分页原地刷新」的演出半与 NFR-PERF-4 实测时延仍无自动断言覆盖 ⇒ 真实 QQ 轮 +
  看屏/串口（K3Q-E4/K3Q-L1）。
- 契约内现实边界（非缺陷，已写进 SKILL.md）：① 删除 no-op（`deleted=false`）不写账、revision
  不变 → 不推帧，措辞必须与成功分开；② 观察缝的 ledger 帧恒首页且 `page.reset=true`，设备在
  任何页都会回首页。

## 遗留 / 需 planner 知悉

1. **R1 已落地，唯一残留是病态撞键降级**：调用方 command_id 字面量恰为 `delete:<x>` 且与既有
   add/redeem 回执键重合时，该条删除回执不持久（打 WARNING `namespace_collision`），删除本身
   照常执行；此时该 command_id 的删除重放保护失效（退回 R1 前行为），add/redeem 幂等不受影响。
   SKILL.md 推荐的 command_id 形态（`qq:<ts>:<hash>`）不会撞键。
2. **R2**：AGENTS.md 零 diff 结论保留（协调者已核定）；如需家庭面纪律条目请回派明确写入面。
3. **R3**：SKILL.md 已写明 `reason` 的 `%XX` 编码只作传输形态、回话用家长原话且不改写语义。