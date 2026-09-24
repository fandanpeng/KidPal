# 派发书 · integrator · 260924-perf-k1-roundtrip-latency

> 消费者：integrator。本棒在池树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch` 跑（分支内容 == Perf 分支 `aa5ec4b` + 本 Pod 后续 commit）。
> 事实输入（只读消费，勿改）：`evidence/latency-baseline-260924.md` §7~§12、`evidence/probe-batches/*.json`（8 批）、`taskpackage.json`、`plan.md`（FROZEN v1.0）、`launch-prep-record.md`（含 human 五项裁定 B1~B5 与 Pod 树迁址记录）、`server/deploy/perf-k1/**` 五件工件、`server/acceptance/**` 冻结齿。

## 一、本棒要交的四件

1. **docs-sync（阻塞项，先做）**。verifier 的 scoped 轮当前唯一红闸 = `docs-sync`：判据是 **`docRefs` 里每个文档都必须提及至少一个验收 id**。
   - 本 Pod 实况：**未换模型**（B1 裁 B 案 = 只改 `agents.defaults.thinkingDefault` 为 `off`），故 plan 里「模型切换成为生产常态 → `docs/architecture.md` §2 依赖表 + 决策 19 型号名同步」的**前提未成立** → **无型号漂移，不需要同步 architecture.md**（如实写明即算履行 docsSync 义务）。
   - planner 已把 `docRefs` 中**本 Pod 写不到的** `docs/architecture.md`（planner deny `docs/**`、你的 allow 也不含它）替换为 **`docs/ADR.md`**（在你的 allow 内）——请在 `docs/ADR.md` **新增一条 ADR 条目**（循该文件既有编号与格式），记录：P1 采用「降 thinking 档」而非「换模型」的决策与理由（实测 `thinking=off` 使课表 10.7s→5.3s；`thinking=low` 无收益；内联 `/think:off` 无效）、新增部署工件 `server/deploy/perf-k1/` 五件、以及「型号未变 → 无 architecture.md 型号名漂移」的如实声明。**条目内须提及验收 id（`PERF-A1` 等）**以满足机械判据。
2. **`delivery/changes/dev/260924-perf-k1-roundtrip-latency/` 两个 blocking closeout 制品**：`docs-delta.md`、`review-packet.md`。review-packet **必须载明**：① 本次改动面（`server/deploy/perf-k1/**` 五件 + `server/acceptance/**` 冻结齿 + 证据面）；② **未覆盖面申报**（逐条一行：本次无齿 / 未跑的设计性质 + 现状 + 去向，零缺口明写 `none`）；③ demo 证据的对账结论（本包 demo 形态 = live 探针批 + 真机基线对照轮，非 UI 截图）。
3. **`morning-decision-package.md`**（plan 阶段划分 4 指定由你成文）：整合 plan 的晨间 6 项 + 用 §7~§12 实测数据更新，并**把下面第四节的 A2 破线事故升为最高优先决策点置于首位**。
4. **全量验证 + 收口**：跑全量 pytest（`python -m pytest -q --import-mode=importlib "not live" server/acceptance/` 与 `-m "not live"` 两条口径）与机械闸（`acceptance-coverage-check` / `mutation-check` / `taskpackage-required` / `docs-sync-check`），**实测数字逐条回报**；`node .claude/scripts/closeout-check.mjs` 必须 exit 0。在 Pod 分支 commit（不 push、不 merge）。

## 二、硬边界

- **不改** `server/acceptance/**`（冻结齿，builder/fixer/你都改不动）、**不改** `docs/contracts/**`、**不改** `plan.md` / `taskpackage.json` 的验收语义、**不改** `server/deploy/perf-k1/**` 的实现（有异议 → fail-loud 上报，不自裁）。
- 不 push、不 merge 主干；凭证不入档；evidence / 记录类文件一律 append。
- 散文产出用简体中文、通顺完整的句子；机器字段保持精确格式。

## 三、能力之外的既有事实（勿重新发现，直接消费）

- 五件工件已把 18 条设计态红齿全部转绿（`80 passed / 4 deselected`）；`acceptance-coverage` 与 `mutation-check` 双绿。
- P1 现状：live = v4-flash + `thinkingDefault: off`；引擎已重启；探针会话已 archive；回滚 = `rollback_model_declaration.py`（最新备份 `…bak-perf-k1-20260924T032223`）。
- A2 质量门（display 块出现率与 schema 合法率 100%）**P1 前后均不达标**（pre 0.444 / post 0.222~0.444），根因是**基线固有**的 v6 遵循度 / 「David」人设渗漏（`tools.profile="coding"` + `DREAMS.md` 42KB 等，k2 范围），**非本 Pod 引入**。

## 四、A2 破线：已上浮 human，**不得作为「修复对象」**

planner 已裁定（依据 plan.md A2 的回滚触发线原文是「**切 A 后**任一批 display<100% → 回滚」，而本 Pod 走 B 案；且 A2 未达标为基线固有）：「暂留 B 已应用 + 把 A2 破线作最高优先晨间决策点上浮 human」，裁定全文见 `evidence/latency-baseline-260924.md` §12.1 第 1 条。
你**不得**试图修复它（不在本 Pod 范围，根因归 k2），但**必须在** `review-packet.md` 与 `morning-decision-package.md` 中**如实载明**：现状数字、基线对照、根因、以及「若不接受现状，一条命令可回滚」的路径。

## 五、完成后回报

简体中文逐条：① 改动/新增文件（含路径）；② docs-sync 落实方式与 `docs-sync-check` 实测退出码；③ 全量 pytest 与各机械闸实测数字；④ `closeout-check.mjs` 退出码；⑤ commit SHA；⑥ 需 planner/human 裁决的歧义（不自裁）。不要自评「通过」——合并门判定归机械闸。