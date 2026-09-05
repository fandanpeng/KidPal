# Review Packet — TaskPackage 260829-host-test-infra

> integrator 收口。合并门前的审查包。本包为**规划工作流（planning-workstream）先行基建包**
> （subsystem: firmware，testHost: 基建自身，mergeOrder 2），目的是给后续纯逻辑包（2/3/4/5/6/8/U）
> 搭 host 单测基建（ESP-IDF linux target + LVGL 软离屏渲染 + 目录约定），让它们能脱离硬件在 PC 上跑测试。
> **承重判断**：验收行为由 integrator 在 WSL 独立实跑 ELF（4 Tests 全绿 EXIT=0）+ builder 三处 mutation
> 证真共同承载；**final 全量 bearing 验证 `unit`/`acceptance` 两门红（BLOCKING finding #1，机械闸判据
> 滞后，正文详述）——本包在合并门前停住，交 planner/人类对齐 G3 闸后重跑 final 再绿。**
> 结构闸 PASS/FAIL 不复述为 finding。

## Change Size

- **Files added（10，全在 `firmware/**`，untracked 新目录 `firmware/main/test/`，零删除、零改既有代码）**：
  - `firmware/main/test/HOST_TEST_CONVENTIONS.md`（INF-A4 [C4.1]-[C4.5] + INF-E2 兼容矩阵 SSOT）
  - `firmware/main/test/host_infra_smoke.acceptance.c`（冻结验收文件，INF-A1..A4 + boundary）
  - `firmware/main/test/host_app/`（独立 host 单测 ESP-IDF 工程）：`CMakeLists.txt` ×2、
    `sdkconfig.defaults`、`espidf_wsl.sh` / `espidf.ps1`、`main/host_app_main.c`、
    `main/host_infra_seam.c`、`main/idf_component.yml`、`main/seam_bounds.test.c`
- **Files modified（2，integrator 本包直接改）**：
  - `firmware/main/test/HOST_TEST_CONVENTIONS.md` §[C4.4] 期望输出 3→4 Tests（docs-sync，见 docs-delta §2）
  - `parts/taskpackage.json`（`.claude/taskpackages/260829-host-test-infra/` 与 `current/` 同步）
- **Net diff**：文本约 +1.4k 行（约定文档 ~140 行 + 验收文件 ~205 行 + host_app 源码 ~450 行 + CMake 配置）。
  `build/`（构建产物）由 `.gitignore` 命中不跟踪；生成的 `sdkconfig`、`dependencies.lock` 亦被 ignore。
- **Scope**：单子系统（firmware，host 测试基建）。零跨系统调用、零契约面触点、零新增第三方依赖
  （unity/lvgl 为 get 组件，设备树已有）。
- **Split recommendation**：不拆。基建 + 验收 + 约定文档 1:1 内聚，后续包的公共依赖，整体合并。

## Verification Evidence

### 结构闸 + builder scoped 机器件（r2，`ok:true`，observedGateExit 0）

`verifier-evidence/rounds/r2.json` 逐门：taskpackage-required ✅ / firmware-build ✅（degraded，
idf.py 本会话 Git Bash 不可用降级，非假绿）/ contract-lint ✅（契约零触）/ acceptance-coverage ✅
（INF-A1..A4 全映射冻结验收文件）/ mutation-check ✅（3 条 mutation 全标记真红→绿）/ docs-sync ✅ /
security ✅（无人为安全审查面）。

### INF-A2/A3/A4 三条 mutation 证真（taskpackage `mutationEvidence`，builder 2026-09-04 WSL 实测）

| mutation | 破真 | 变红 | 还原 |
|---|---|---|---|
| INF-A2 | 移 `host_app/main/CMakeLists.txt` 的 `WHOLE_ARCHIVE` → 重建 elf 4.1MB→430KB → 跑 ELF 得 0 Tests（空洞红，TEST_CASE 对象被链接器丢弃） | ✅ 变红 as expected | 复跑 4 Tests 全过 |
| INF-A3 | `host_infra_seam.c` 填充色 0x336699→0x204060 → 跑 ELF 得 INF-A3 FAIL（探针真读渲染像素，非静态返回） | ✅ 变红 as expected | 还原 → PASS |
| INF-A4 | `mv HOST_TEST_CONVENTIONS.md` → [C4.1] RED（doc missing） | ✅ 变红 as expected | 还原 → PASS |

### integrator 独立复验（受支持 shell = WSL Ubuntu-24.04，非采信 builder 口头）

```
$ wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
ninja: no work to do. Done            # 源码 ↔ build 产物一致（ninja 依赖校验）
$ cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
... 4 条 TEST_CASE 全 PASS（seam_bounds supp / boundary / INF-A3 / INF-A2）
-----------------------
4 Tests 0 Failures 0 Ignored
OK
EXIT=0
```

独立复证：**4 Tests 0 Failures，EXIT=0**，运行的是当前源码一致产出的 runner，覆盖 INF-A2 真实样例
断言（位反转 4+ 断言 + round-trip + 缓冲变换）+ INF-A3 LVGL 到虚拟屏 6 像素 === 0x336699 + boundary
未渲染哨兵 + seam_bounds 越界哨兵补充测试。INF-A4 人工核验点 [C4.1]-[C4.5] 逐节复核通过（目录总览 /
放置 / 命名 / 运行命令 / 兼容矩阵，均在 `HOST_TEST_CONVENTIONS.md` 同一文档）。

### final 全量 bearing 验证（integrator 跑，r3，`ok:false`，observedGateExit 1）

`verifier-evidence/rounds/r3.json`：scoped 相位全部同名门照 `ok`（taskpackage-required/firmware-build
degraded/contract-lint/acceptance-coverage/mutation-check/docs-sync/security），full 相位 `unit` 与
`acceptance` 两门红：

| 门 | 结果 | reason | 说明 |
|---|---|---|---|
| unit | 🔴 | `has-unit-files-without-runner` | `seam_bounds.test.c` 存在，但 `unit-run-check` 的 firmware runner 证据（仅认 `firmware/host_test/` 或 `sdkconfig.ci.linux`）看不到 `main/test/host_app/` runner |
| acceptance | 🔴 | `has-acceptance-files-without-runner` | `host_infra_smoke.acceptance.c` 存在，但 `acceptance-run-check` 的 firmware runner 证据（仅认 `firmware/acceptance/` 或 `run_acceptance.*`）同样看不到 host runner |

这是 **BLOCKING finding #1（机械闸判据滞后于新基建，非测试假红）**。我的独立 WSL 实跑已证明 runner
真实存在且跑绿，但 G3 真空绿闸不认识它——不改闸则后续每个 host 包都被误伤。

## Simplification Debt

自本次改动 grep `minimal:` 标记 → 零命中（`no-trigger`）。host_app 工程 `COMPONENTS main` 隔离到位、
渲染缝单文件职责单一、`espidf_wsl.sh` 一个入口收拢构建，无过早抽象、无重复实现、无空壳脚手架。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| BLOCKING | judgement-call | 机械闸 / 深 dev-loop 对齐 | **G3 真空绿闸不认识新 host runner**：`main/test/host_app/`（`CONFIG_IDF_TARGET_LINUX` linux-target 工程，`espidf_wsl.sh` 运行入口，ELF 实跑 4 Tests 全绿）存在，但 `unit-run-check` 的 `hasFwUnitRunner`（仅认 `firmware/host_test/` 或 `sdkconfig.ci.linux`）与 `acceptance-run-check` 的 `hasFwAcceptanceRunner`（仅认 `firmware/acceptance/` 或 `run_acceptance.*`）都不识别，final 全量验证 `unit`/`acceptance` 两门报「有测试无 runner 证据」假红。独立 WSL 实跑已证 runner 真实（4 Tests EXIT=0），此为**判据滞后**非测试假红。不改则后续 host 包（2/3/4/5/6/8）照 INF-A4 挂测试每个都触发真空绿 → 基建价值不成立。 | changed-code hunk：`seam_bounds.test.c`（FW_UNIT_RE `*.test.c` 匹配）+ `host_infra_smoke.acceptance.c`（FW_ACCEPTANCE_RE）+ `host_app/sdkconfig.defaults`（`CONFIG_IDF_TARGET_LINUX=y`）；heuristic/tag：`runner-evidence` 判据 cover 面不足；对照先例 afd31ac（server host/live 闸缺口，human 授权直改闸脚本 + 记 ARQ）。 | 升级 planner/human：在 `hasFwUnitRunner`/`hasFwAcceptanceRunner` 补一条「`firmware/main/test/host_app/sdkconfig.defaults` 含 `CONFIG_IDF_TARGET_LINUX=y` ⇒ 有 host runner」判据（精确补丁见下）。**planner/human 改 `.claude/scripts/**`（integrator 出范围，架构门级，按 afd31ac 先例需授权）后，重跑 final 验证转绿再进合并门。** |
| FYI | judgement-call | 文档口径 | C4.4 期望输出 3→4 Tests 修正（integrator docs-sync 直改）——原文档写的 3 Tests 是验收文件自身 3 个 TEST_CASE，实际 runner 因挂载 `seam_bounds.test.c` 补充测试报 4 Tests。已改成真实 4 条明细 + 明示绿判据不依赖固定条数。 | changed-code hunk：`HOST_TEST_CONVENTIONS.md` §[C4.4]；heuristic：文档与实际运行必须一致（防后续包按错误期望硬校）。 | 已随本次提交落地，无进一步动作。 |

**BLOCKING finding #1 精确补丁建议（供 planner/human 采用）**：

```js
// unit-run-check.mjs — hasFwUnitRunner(fwDir) 末尾追加：
const hostSdk = join(fwDir, 'main', 'test', 'host_app', 'sdkconfig.defaults');
if (existsSync(hostSdk) && /^CONFIG_IDF_TARGET_LINUX\s*=y/m.test(readFileSync(hostSdk, 'utf8'))) return true;
// acceptance-run-check.mjs — hasFwAcceptanceRunner(fwDir) 末尾追加同一条（host runner 也跑 *.acceptance.c）。
```

（此补丁已拟，但被标的 scope-map 拦回：`.claude/scripts/**` 不在 integrator 写允许面，如实记录，
不绕过。）

## Five-Axis Review

1. **Correctness**：INF-A2/A3/A4 三条 mutation 全部「破真→变红→还原→绿」证伪空洞（WHOLE_ARCHIVE 移除
   得 0 Tests、填充色变更探针 FAIL、约定文档 mv 断链）。验收断言均为真实计算/渲染结果（位反转、
   round-trip、缓冲变换、LVGL 6 像素 === 0x336699、未渲染哨兵 -1），非「存在符号即过」。独立 WSL
   复验 4 Tests 全绿一致。
2. **Test quality**：host runner 非编译过场——`WHOLE_ARCHIVE` 强制每个 TEST_CASE 对象进链接，否则
   unity 报 0 Tests（INF-A2 mutation 已证）；渲染缝为真 LVGL 软渲染（INF-A3 mutation 已证探针真读像素）。
   `required` 断言 + boundary 哨兵区分「未渲染」与「真帧」，防静态色作弊。
3. **Contract/boundary safety**：`docs/contracts/**` 零写（零契约面触点，只读消费，契约门未触发）；
   `COMPONENTS main` 隔离设备组件树，host 不会被设备芯片驱动污染，也不能编译出 `app_main` 之外的
   设备符号。无跨系统调用、无循环依赖、无新增第三方依赖。安全。
4. **Operability/security**：`espidf_wsl.sh` 是唯一运行入口，切 WSL venv python + 不 source export.sh
   （避开设备工具链依赖），`set -e` 失败快停；linux target 用 WSL apt gcc/cmake/ninja，无权限、无 secret、
   无网络面。无新增可执行权限问题（脚本以 bash 显式调用）。安全。
5. **Maintainability/simplicity**：`HOST_TEST_CONVENTIONS.md` 单文档承载 [C4.1]-[C4.5] + INF-E2 矩阵，
   放置/命名/运行命令一次说清；host_app 工程内聚、约定清晰，后续包「照此挂测试」有唯一手册。无
   `minimal:` 债务。干净。

## Residual Risk

- **Known limitation**：
  - **最终全量 bearing 验证 `unit`/`acceptance` 两门红（BLOCKING）**：机械闸判据不认识新
    `main/test/host_app/` linux-target runner。不是测试假红（WSL 独立复验 4 Tests 全绿），但必须由
    planner/human 对齐 G3 闸（`hasFwUnitRunner`/`hasFwAcceptanceRunner` 补 host runner 识别，精确补丁
    见 finding #1）并重跑 final 转绿后，本包才具备进入合并门的绿形态。**当前 closeout 状态 = BLOCKED，
    非 merge-ready。**
  - buf 分配 400×300×4 = ~480KB 整帧在 OS 堆（CLIB），超出内置 LV_MEM 默认池——这是刻意取舍（文档已记），
    后续包加更大帧须重新评估。
  - `readPixel` 依赖同步单帧刷新（`lv_refr_now`），若后续包引入多帧/动画时序，需扩展 seam 契约。
- **Follow-up recommended**：G3 闸对齐本身（BLOCKING）交 planner/human；一旦对齐，integrator 重跑
  final 全量验证转绿、补记 verdict/verification 后释放合并门。后续 host 包（2/3/4/5/6/8）接入 first
  runner 时确认真空绿判据已吃住 host_app 标记。
- **Human decision needed**：G3 闸 `unit`/`acceptance` 的 firmware runner 判据更新需 human 授权直改
  `.claude/scripts/**`（architect 门级，对照 afd31ac server host/live 先例）；建议顺带把 host/live
  与 firmware host runner 的「runner 证据」规范正式化记 ARQ。

## decisionCoverage 落位反查（3 决策，全 landsIn acceptance/exitCriteria，无 dropped/risk-accepted）

| decision | 承载 acceptance/exit | 状态 |
|---|---|---|
| PROPOSAL-§5-OQ2（先搭 host 基建让 B 段包电脑跑） | INF-A2 | ✅ 已验（linux runner 独立实跑 + mutation 证真） |
| PROPOSAL-§1-测试分层（纯逻辑依赖 host-based + LVGL SDL） | INF-A3 | ✅ 已验（LVGL 软离屏 6 像素探针 + mutation） |
| PROPOSAL-§7-linux-target 兼容性（不预判，本包自验） | INF-E2 | ✅ 已验（INF-E2 兼容矩阵落约定文档，host 实测 vs device view manifest 双证据） |

三决策均 `landsIn: acceptance:.../exitCriteria:...`，无 `dropped`、无 `risk-accepted` 条目，无
「decisionCoverage → landsIn dropped」需单列。