# 派发书 · acceptance-author · 260924-perf-k1-roundtrip-latency（第 2 版）

> 消费者：acceptance-author（冻齿）。**语义权威** = 本包 `plan.md`（FROZEN v1.0）验收口径节 + `taskpackage.json` acceptance[]/exitCriteria[]；A/B 主次以 `launch-prep-record.md`「human 五项业务裁定」**B1 行**为准（plan 冻结文本不回改）。
> 前版 = 同目录 `input-brief-acceptance-author.md`（2026-09-24 01:21，pre-B1）。本版差异只有两处：① B1 翻转导致**声明工件对象改指**；② 证据面扩到 §7~§10。
> 树迁址上下文见 `launch-prep-record.md`「Pod 树迁址记录」——本棒起在池树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch` 跑，分支内容 == Perf 分支 `aa5ec4b`。

## 任务

为 PERF-A1~A4 冻结可执行验收测试，落 `server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py`；RED 基线实跑后把「N failed / M passed（既有齿零新增红）」落账 `taskpackage.json.redBaselineResult`（替换 PENDING 占位）。

## 齿形态（评审 §3 硬性 + B1 修正）

1. **CI 齿只断言机械面**（timing 数字不入 CI 齿——天然 flaky）：
   - **P1 声明工件**在位 + schema 正确 + 与 apply 脚本消费面一致。**B1 修正**：对象 = **thinking 档位声明**（现状实测 `agents.defaults.thinkingDefault` = unset、运行时默认 high），**不是** model id / allowlist 增补（A 案已降为 fallback）。**齿须对 B 主案与 A fallback 两形态皆可判**——建议以声明工件的 `case` 字段参数化（B：`thinkingDefault ∈ {off,low}`；A：model ∈ allowlist 且非推理），具体表达归你；判据恒为声明机制的机械面，不做时延断言。
     理由（防返工）：A/B 裁决发生在 builder 步⑤，齿冻结在其前——齿硬编 B 形态的话，B 不达标转 A 时会因错误理由翻红。
   - apply 脚本幂等 + **drift-guard**（现状锚点核对不中 → exit 3 拒绝；照抄 `server/deploy/openclaw-patch/apply-openclaw-patch.sh` 的 STOP-on-mismatch 形态）+ 时间戳备份 / 一步回滚脚本存在且 dry-run 可跑。
   - 探针脚本 **v6 注入 + hash 断言**（hash 源 = `server/deploy/openclaw-system-prompt.v6.md` 逐字，渲染面 == 被测面，ADR-001 精神）。
   - must-stay-green：`server/deploy/provider.config.yaml` 的 LLM 段声明面零改动 + v6 文件 hash 零回改。
2. **live 齿**（`@pytest.mark.live`，不进 `-m "not live"`）：探针批执行器（标准 3 题 ×N≥3 中位数 + 主机快照 + session 计数记录）+ 门判定器（读证据文件对两层门：保底 6/9/14s、目标 4/6/10s）。**判定器逻辑本身要有 offline 齿**（喂 fixture 证据文件 → 断言判定结果），时延数字本身不进 CI。
3. **mutation 打在配置接线上**（删声明文件 / 改错值 / 删 allow 条目 → 齿红），RED→GREEN 声明 + counterEvidence 回填。

## 红基线

- 命令：`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/`（仓根）。
- 上游已知红 = 0：实测 `git diff --quiet 1746c14 HEAD -- server` 零差异成立 → 基线锚「server 面 57 passed / 0 failed @1746c14」照引，无需重跑。
- 本包新增齿 = 设计态红（实现工件未落盘前），实跑后落账。

## 新增证据（append 于 `evidence/latency-baseline-260924.md`，供齿设计参考）

- §7 P0 主线程代执行证据链：重启 + **五项核验实测全绿**（PERF-A4 的核验项清单可作齿形态参考）；
- §8.1 CLI 地面真值：`openclaw config get agents.defaults.thinkingDefault` = **unset**（drift-guard 现状锚点）；
- §10 pre-P1 真机基线对照锚：闲聊 17.22s / 课表 13.64s / 天气 15.05s。

## 铁律

- 验收测试归你，builder/fixer 改不动；不改 `docs/contracts/**`；不改 `plan.md` / `taskpackage.json` 的 acceptance 语义（发现矛盾 → fail-loud 上报 planner，不自裁）。
- 凭证不进测试文件（live 齿从 ECS 侧配置/shell 变量取，不落仓）。
- evidence 一律 append，不覆盖既有行；散文产出用简体中文通顺完整句。
- 完成交回 planner 由 verifier 判定，不自评通过。