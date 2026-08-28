# PROPOSAL — 260829-gov-review-migration-tier23（档 2 dev-loop 全链 + 档 3 portfolio 迁移）

> 迁移批（元级动作）。建立 dev-loop / portfolio 治理机器本身的动作无法走七步闭环（机器尚不存在），
> 沿袭档 1（260827-gov-review-migration-bootstrap）先例：human 一句授权即落地依据，落地后补记账，
> Status 标 **LANDED-UNREVIEWED**，首个常规批次恢复标准 Track B 流程。
> 授权来源：human 指令「ESP32 spec已经完成review，你把档2，档3 也迁移过去」（2026-08-29）。

## 状态

- **Status**: LANDED-UNREVIEWED
- **轨道自评**: 元级迁移批（治理机器自举），豁免七步闭环；非 G1 常规变更。
- **落点**: `C:\AI\ESP32`（KidPal 小伴仓）

## 一、目标

把档 1（Spec 治理：arch-review 链 + 契约门 + Track B）之上尚未迁入的两档补齐：

- **档 2 · 单切片开发**：dev-loop 全链（planner / acceptance-author / builder / fixer / verifier /
  integrator / supervisor）+ `/build-feature` `/triage-change` `/loop-retro` 入口 + 测试闸
  （verify-gates 按本仓双实现面重写）+ 合并门。
- **档 3 · 多 Pod 编排**：portfolio-planner（RTE）+ worktree 协调 + merge token + pod-coordination。

## 二、栈差异与重写决策（P3/P4 参数）

源仓栈 = pnpm monorepo + TypeScript + vitest + Prisma + Playwright。
本仓栈 = ESP-IDF + C/C++ 固件（firmware/）+ Python FastAPI shim（server/）+ pytest。

逐闸决策（直搬 / 重写 / 声明缺口）：

| 源仓闸 | 本仓处置 | 理由 |
|---|---|---|
| taskpackage-required | 直搬 | 栈中立（只读 taskpackage.json） |
| schema（prisma-validate） | **替换为 firmware-build** | 无 Prisma；ESP-IDF `idf.py build` 编译即固件结构/类型闸；idf.py 缺失 fail-open（记 degraded 非假绿） |
| types（tsc --noEmit） | **并入 firmware-build**；server 面 Python 类型闸记为缺口 | 无 tsc；mypy 未启用（见 §五 缺口） |
| contract-lint（TS schema lint） | **重写为 markdown 契约 lint** | 契约面 = docs/contracts/interface-contract.md 单文件 markdown；lint 目标转结构完整性（节标题/围栏闭合/断链），不冒充 TS 类型级校验 |
| acceptance-coverage | 直搬 | 栈中立 |
| mutation-check + lib/acceptance-id-shared | 直搬 + structuralWords 对齐 | 脚本栈中立；structuralWords 词表改本仓闸名（firmware-build/contract-lint/compile），自检电池同步 |
| docs-sync | 直搬 | 栈中立 |
| security | 直搬 | 栈中立 |
| unit（vitest） | **重写为 firmware host-Unity + server pytest 双面结构闸** | 治「有测试文件而无真跑」假绿；firmware host 单测需 linux target，闸只做结构担保不强制实机 |
| acceptance（vitest .acceptance.spec.ts） | **重写为 firmware on-target Unity + server pytest 双面结构闸** | 治「有验收而无验收入口」假绿；on-target 验收需实机/qemu，**诚实降级**为结构闸 + 人工/台架触发，证据落 verifier-evidence |
| supervisor-verdict-gate 风险信号 | **重写风险信号** | DROP TABLE/deleteMany → ESP-IDF 破坏面（esp_partition_erase/write、esp_ota_set_boot_partition、esp_flash_erase_region、nvs_flash_erase、f_mkfs）；REAL_DB_MARKERS → REAL_EFFECT_MARKERS（硬件副作用 + FreeRTOS 内核对象 + 文件副作用 + Unity 断言）；CORE 面收窄为契约/分区表/sdkconfig/boards（firmware/main 是热循环非承重核）；跨系统 apps/<sys> → firmware/server 第二段。自检电池 69 例全绿 |
| merge-driver quick-check | 适配 | tsc/prisma → firmware-build/contract-lint（quick-check 半径） |
| worktree-bootstrap | 重写 | pnpm install + prisma generate → .env/sdkconfig 镜像 + shim pip install |

## 三、落地清单（packet-bound）

- agents（6 新）：planner / portfolio-planner / builder / fixer / integrator / acceptance-author
- commands（3 新）：build-feature / triage-change / loop-retro
- references（14 新 + 适配）：dev-loop / dev-loop-operating-system / pod-coordination / code-review-gate /
  error-envelope / escalation-playbook / acceptance-evidence / taskpackage-authoring /
  task-intake-and-slicing / minimal-implementation / residual-scan-patterns / security-hardening /
  observability-k6 / test-quality-antipatterns
- hooks（6 新 + 1 重写）：verify / supervisor-gate-check / supervisor-verdict-gate(重写信号) /
  portfolio-sync / planning-sync / suggest-codebase-memory
- scripts：直搬栈中立批 + 重写 acceptance-run-check / unit-run-check / merge-driver /
  worktree-bootstrap + 新写 contract-lint / firmware-build-check + 拷 mutation-check + lib
- 配置：`dev-loop.settings.json`（新写，deny docs/contracts/**）+ `verify-gates.json`（新写 9 闸）+
  `scope-map.json` 增 dev-loop agents 段（planner/portfolio-planner/builder/fixer/acceptance-author/verifier/supervisor/integrator）
- 骨架：`.claude/taskpackages/`、`.planning/`、`changes/dev/`
- 文档栈引用适配：27 文件（apps→firmware/server、contracts→docs/contracts、vitest→idf.py+pytest）

## 四、验证证据

- `node --check` 全 hooks+scripts：ALL SYNTAX OK
- 自检电池：supervisor-verdict-gate 69/69、mutation-check 8/8、acceptance-id-shared 20/20、
  unit-run-check ok、acceptance-run-check ok、contract-lint ok、firmware-build-check ok
- JSON 合法：verify-gates / scope-map / dev-loop.settings / settings / review-loop.settings 全过
- route-regression-check 56 ok 0 fail；changes-index 重建可用；lint-descriptions 仅 1 条 length warn（landing-verifier）

## 五、诚实记账的缺口（非假绿）

1. **固件 on-target 验收不实机自动跑**：acceptance 闸担保「有验收文件 ⇒ 有入口」，实机验收由
   acceptance 相位人工/台架触发，证据落 verifier-evidence。这是非纯软件项目的固有降级。
2. **spec↔contract 机械对账仍缺**：markdown 契约无法做类型级 spec↔contract 一致性检查，
   档 1 已降级人工（ARQ-260827-contract-mech-gate-gap 追踪）。
3. **server 面 Python 类型闸未启用**：mypy 未配；如需后续加。
4. **firmware-build 闸在无 ESP-IDF 工具链的开发机 fail-open**（记 degraded）；CI/台架可设
   `VERIFY_FW_BUILD_REQUIRED=1` 强制真编译。

## 六、retro（落地后补，趁热）

见同目录 `retro.md`。
