---
id: ARQ-261117-acceptance-coverage-assertion-and-flag-gaps
title: acceptance-coverage-check 的断言识别集不认 Unity（纯 Unity 验收件假红）+ 旗标面与 mutation-check 不一致（不认 --change-id）
domain: gov
status: deferred
severity: SHOULD
owner: architect（评审裁量落点；改闸归 dev-loop 执行面）
trigger: 下一个动 `.claude/scripts/acceptance-coverage-check.mjs` 或机检旗标面的治理批
opened: 2026-11-17
closed: ""
closedBy: ""
paths: [".claude/scripts/acceptance-coverage-check.mjs", ".claude/scripts/mutation-check.mjs"]
source: ["delivery/taskpackages/261104-k4-reward-goal-config/pod-retro.md 踩坑 2", "delivery/taskpackages/261104-k5-story-cover-nav/pod-retro.md 踩坑 4", "delivery/retro/2026-11-17-loop-retro.md §二 T4"]
---

**证据**：
1. **Unity 断言不识别**。261104-k4 踩坑 2 逐字：「`acceptance-coverage` 不认 Unity 断言：gate 的 `hasRealAssertion` 只认 JS/Python 形态，`TEST_ASSERT_*_MESSAGE` 一律不认；纯 Unity 的 C 验收件会假红，K2 能过只是因为恰有一处 libc `assert()`。本包按 K2 先例补真实 OOM 守卫绕过」。脚本实核：`.claude/scripts/acceptance-coverage-check.mjs:74-83` 的 `hasRealAssertion` 只匹配 `expect(` / `assert.*` / `\.assert[A-Z]\w*\s*\(` / 行首裸 `assert`；`TEST_ASSERT_EQUAL_MESSAGE(` 中 `ASSERT` 前是下划线（词字符），不满足 `\bassert` 词边界，因此不匹配。
2. **旗标面不一致**。261104-k5 踩坑 4 逐字：「`acceptance-coverage-check.mjs` 不认 `--change-id`（当位置参数路径），`mutation-check.mjs` 认。两次派单都写错、靠执行者自行纠正。候选 ARQ：补 `--change-id` 对齐先例」。脚本实核：`acceptance-coverage-check.mjs:23` 直接 `process.argv[2]` 当路径；`mutation-check.mjs` 走统一的 `--change-id` 解析。

**影响面**：所有用 ESP-IDF Unity 写 host 验收的固件包（本仓固件验收主力形态）——要么被迫补一个与断言无关的 `assert()`/OOM 守卫来骗过闸（261104-k4 的做法），要么在纯 Unity 件上吃假红。旗标面不一致则是每次派单都可能写错的可用性税，且两个闸的报错都不会说「我不认这个旗标」。

**建议改动**（候选，交 review、不自批）：① `hasRealAssertion` 增加 Unity 形态（`TEST_ASSERT_[A-Z_]+\s*\(`、`TEST_FAIL_MESSAGE` 等），并补相应 self-check 正负例；② `acceptance-coverage-check.mjs` 支持 `--change-id <changeId>`，与 `mutation-check.mjs` 统一调用形状（保留位置参数兼容）；③ 两个闸的报错在收到未知旗标时 fail-loud 提示支持的旗标集，而不是把 `--change-id` 当路径静默解析失败。

**风险**：riskTier: **low**——①是对既有正则的加宽（只增识别面，需补负例防 prose 误报）；②③是 CLI 兼容与报错文本。触 `.claude/scripts/**` → Track B 主路径。验证 = 用 261104-k4 的纯 Unity 验收件跑修复前后对比，并在 self-check 加 Unity 正例与注释 / 字符串负例。