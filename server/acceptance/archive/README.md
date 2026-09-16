# 冻结验收归档区（server/acceptance/archive）

> 2026-09-16 human 裁定（M1 Phase 1 已进入；pod-events `260915-m1-n1b-p1-home-battery`
> 本批 human-decision 事件）：MVP / 验证期积累的 server 面冻结验收整体退出现役，统一
> 归档于此。归档 = `git mv` 移位 + 本清单登记，文件本体冻结态不变（builder/verifier
> 仍不得改写），历史 TaskPackage 的验收证据链不受影响（文件仍在本仓、可按路径检索）。
> 收集口径注记：验收闸收集 glob（`acceptance-run-check.mjs` PY_ACC_RE）只命中
> `acceptance/` 直下 `test_*.py`，`acceptance/archive/` 子目录天然不入收集——移位即
> 退役，零治理脚本改动（与固件侧 `firmware/main/test/acceptance/archive/` 同构）。
> human 裁定范围注记：`server/tests/test_reminder_units.py`（包 7 实现单测，guard
> 仍在生产运行的 `server/gateway/reminders.py`）**保留现役不归档**（human 裁定，
> AskUserQuestion 留痕）。

## 归档件清单（10 件，来源包 → 文件）

| 来源包（era） | 归档文件 | 备注 |
|---|---|---|
| 260829 批（MVP） | test_char_stroke.acceptance.py | 查生字 P4 server 半边；docstring 孪生引用 char_stroke.acceptance.c 已随 544a804 归档 |
| 260831 批（MVP） | test_cloud_link.acceptance.py | 配网云链路；docstring 引 device_ws_switch.acceptance.c（已归档） |
| 260829 批（MVP） | test_schedule_p2.acceptance.py | P2 课表页 server 半边 |
| 260829 批（MVP） | test_schedule_reminder.acceptance.py | 包 7 提醒投递 server 半边；**SRE-E4 随归档消红** |
| 260829 批（MVP） | test_smoke_e2e.acceptance.py | smoke e2e 结构齿；**SMK-A4 随归档消红** |
| 260829 批（MVP） | test_story_p3.acceptance.py | P3 故事页 server 半边 |
| 260829 批（MVP） | test_story_wiring.acceptance.py | 260829-hardware-validation 接线面 |
| 260901 批（验证期） | test_gateway_directionB.acceptance.py | 方向 B 网关结构齿 |
| 260903 批（验证期） | test_g_relay_fix.acceptance.py | 网关 relay 修复回归；注释引 screen_unification.acceptance.c SU-A2（已归档） |
| 260829 批（MVP） | test_voice_roundtrip_b.acceptance.py | 已于同日先行按模块级 skip 退役（pod-events bba7d239，commit e990eb2）；本批连文件一起移入归档区，skip 块随移位自然失效 |

## 归档时点事实

- 归档前 server 面基线（本 worktree 实跑 2026-09-16）：**2 failed / 122 passed /
  14 skipped / 45 deselected**。2 红 = 在册既存红（K1a inventory R1/R2，K1a 同款
  记账）：R1 = `test_smoke_e2e::test_a4_display_line_evidence`（SMK-A4，证据件
  `server/acceptance/_artifacts/smoke-e2e/screen.md` 缺失）；R2 =
  `test_schedule_reminder::test_e4_contract_point3_evidenced_and_poll_form_retired`
  （SRE-E4，读 `docs/spec.md` —— 260913 spec 拆分后路径悬空致断言落空）。两条均随
  归档自然消红：**非修复、非豁免，勿计入修复战果**。
- 14 skipped = voice_roundtrip_b 模块级 skip（8 条）+ live 面显式 skip。
- 45 deselected = `@pytest.mark.live` 面（`-m "not live"` 条目级筛除）。
- 扫尾项 ARQ-260916-server-mvp-teeth-sweep（上次事件登记）由本批闭账。

## 归档后影响面

- `server/gateway/`（reminders.py、mcp_relay、provider 等生产在线代码）离线守卫
  清零，唯一保留 = `server/tests/test_reminder_units.py`（reminders.py 实现单测）；
  回归保护真空期由 M1 后续包（K3 语音轮 UI / K4 提醒弹窗等）的 acceptance-author
  按新验收重钉补位。
- 验收闸 `acceptance-run-check.mjs`：pyAccFiles 收集数 10 → 0，走「全无 → 跳过绿
  （起步期合法）」分支，由 verifier 证据闸担纲「真跑过」。

## 后续纪律

- 归档件**不得**直接复活回 `server/acceptance/` 现役位；M1 后续包需要回归守卫时，
  由改动包的 acceptance-author 新写钉点，不复活整份归档件。
- 归档件 docstring 中指向固件侧已归档孪生的引用随件入档，不再清理（历史证据链
  语义不变）。
