<!--
AGENTS.md 模板：给【build agent】的操作手册——“如何建设”，与 spec.md（“建设什么”）解耦。
复制到模块根目录（或 docs/systems/Sx/AGENTS.md）后填写。
删除本注释块和所有 〈占位说明〉。
-->

# AGENTS.md · 〈模块名〉

> 本文件指导 AI build agent 如何在本模块内开发。产品需求见同目录 `spec.md`；数据/事件/工具契约见仓库 `contracts/`。

---

## 1. 模块定位

- 路径：`modules/〈name〉/`（公司模块化单体内）
- PG schema 前缀：`〈prefix〉.*`
- 对应规格：`docs/systems/Sx-〈name〉/spec.md`

## 2. 技术栈（统一，不得偏离）

Next.js App Router · TypeScript · PostgreSQL · Prisma · pg-boss · Vercel AI SDK · zod · SSE。

## 3. 目录约定

```
modules/〈name〉/
  src/
    routes/        # Next 路由 / API
    domain/        # 领域服务（纯逻辑）
    agent/         # agent + prompts
    tools/         # tool registry（同时供 chat + 自动化）
    db/            # Prisma 访问（schema 在 /contracts）
  tests/           # 测试，命名含需求 ID（Sx-Ay）
```

## 4. 契约纪律（铁律）

- 数据模型只改 `contracts/schema.prisma`，改完跑 `prisma migrate`。
- 事件只在 `contracts/events/` 定义；新增事件须同时声明 producer/consumer。
- 工具只在 `contracts/mcp-tools/` 定义 input/output zod。
- **禁止**在本模块内重定义契约里已有的类型；从契约 import。

## 5. 开发流程（每个需求）

正式实现默认走 Dev loop：planner → acceptance-author → builder → verifier↔fixer → integrator。主 AI 不直接实现本模块。

1. planner 认领稳定 ID（`Sx-Ay`），读其 Given/When/Then，并生成 TaskPackage。
2. 如疑似需要改契约，先派 architect-reviewer 预审；确需改契约则停在人工契约门。
3. acceptance-author 先写冻结验收测试，并确认红基线。
4. builder 只在验收文件与红基线证据齐全后，实现领域服务 → 工具 → UI，并补单元测试。
5. verifier 跑结构闸与功能闸，失败交 fixer 聚焦修复。
6. integrator 全绿后做 docs-sync、全量验证与 PR 摘要，停在人工合并门。

### 5.1 自分析回路（AI 自己开发 / 测试 / 修 bug）

build agent 失败时不靠翻散文日志，而是用**机器可读信号**自迭代：

1. 跑 typecheck/lint/test，读其**结构化输出**（JSON/JUnit，见 §6）。
2. 失败 → 取 `ErrorEnvelope`（`category` + `fingerprint`）定位与聚类同类失败。
3. 复现/运行时 bug → 用 `correlationId` 查 `core.LogEntry` + `core.AuditLog` + `core.AiRun`，重建完整因果链（SQL 查询，非 grep）。
4. 吃自家狗粮：E2E 跑 S2 AITester，其 failure-analyzer 输出即 AI 可读的 bug 归因（`errorCategory` + 假设 + 反写建议）。
5. 修复 → 回到第 1 步，直到全绿。

## 6. 常用命令

| 目的 | 命令 |
|---|---|
| 安装 | `〈pnpm install〉` |
| 迁移 | `〈prisma migrate dev〉` |
| 起服务 | `〈pnpm dev〉` |
| 单测（机器可读） | `〈pnpm test --reporter=json〉` |
| E2E（吃自家狗粮 AITester） | `〈…〉` |
| Lint/Typecheck | `〈pnpm lint && pnpm typecheck〉` |

> 测试/typecheck 输出尽量走机器可读格式（JSON/JUnit），便于 build agent 自动解析失败并自迭代（见 §5.1）。

## 7. 护栏 / 安全

- 所有 AI 调用走 K5；记录 AiRun + 成本到 K6。
- 所有写操作经 K1 取 actor，写 K6 ChangeHistory。
- **可观测性（K6）**：入口生成并向下透传 `correlationId`（审计/事件/日志/AiRun 同值）；错误统一装箱为 `ErrorEnvelope`（`category`+`fingerprint`）；关键决策写结构化 `LogEntry`（`event` 用固化词表），禁用散文日志。
- 高风险动作（〈本模块的人工确认关卡〉）必须人工确认，agent 不得自动执行。
- 不提交 secrets（走内核 secrets store）。

## 8. Definition of Done（DoD）

- [ ] 覆盖认领需求的全部 Given/When/Then
- [ ] 验收测试由 acceptance-author 先写并确认红基线；builder 只让它们转绿
- [ ] 单元测试覆盖变更分支与边界；验收/单测断言真实结果，不是“没报错”
- [ ] 契约已更新且 migrate 通过；如无契约变更，已记录无变更理由
- [ ] typecheck + lint + 单测 + 验收 + 相关 E2E 全绿
- [ ] AiRun/审计/成本记录就位
- [ ] 可观测性就位：correlationId 全链透传、错误装箱 ErrorEnvelope、关键决策写结构化 LogEntry
- [ ] integrator 已完成 docs-sync：spec.md / AGENTS.md / HANDOVER 需要同步的地方已更新，或记录无需更新理由
