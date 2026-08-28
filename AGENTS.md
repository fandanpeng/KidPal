# AI Native Company Agent Instructions（ESP32/KidPal · 治理档 1+2+3）

> **迁移注记**：本仓治理体系自 `c:\AI\AInativecompany` 迁移，按 `docs/templates/GOVERNANCE-MIGRATION-GUIDE.md` 分档裁剪。**档 1 = Spec 治理**（arch-review 对抗链、契约门、Track B 治理路由、scope 闸、deny-main-push）2026-08-27 迁入；**档 2 = dev-loop 全链**（planner / acceptance-author / builder / fixer / verifier / integrator / supervisor + 测试闸）与**档 3 = portfolio 编排**（portfolio-planner / worktree / merge token / pod-coordination）2026-08-29 迁入（迁移批 `changes/arch/260829-gov-review-migration-tier23`，栈绑定闸按本仓双实现面 firmware/server 重写）。

## 实现面与 vendor 面约定

- **自有实现面** = 固件业务代码 + 云端 shim 代码。具体根目录（候选：`firmware/`、`server/`，或沿用小智工程内 overlay 布局）在动工前由 human 定夺并登记进 `.claude/scope-map.json`——定之前按「未登记实现面」处理，不做机械强制。
- **vendor 面** = `ESP32-S3-RLCD-4.2-Demo/`（官方 Demo + 小智 V2.1.0 源码）。所有 agent 对它只读；需要改它承载的行为时，在自己的 overlay 里做，不直接改 vendor 树。
- **系统 spec 面** = `docs/systems/kidpal/spec.md`（单系统项目，目录名 kidpal = 产品名；多系统时按 `docs/systems/<sys>/` 扩）。
- **契约面** = `docs/contracts/**`（单一事实源 [interface-contract.md](docs/contracts/interface-contract.md)：笔顺 JSON / display 统一响应块 / 课表 MD 结构 / shim API）。契约改动走 architect-reviewer 四分类 + 人类契约门。

## 治理路由（Track B 常驻默认 + Track A 判据升级 · 自 AInativecompany 2026-08-27 移植）

治理 / spec 变更默认走 **Track B 常驻轻路径**——七步闭环：

1. 主线程起草 PROPOSAL（业务语言 + 初步技术方案 + human 取舍点标注，骨架 `docs/templates/PROPOSAL.template.md`）→ human 立项 ack；
2. architect 评审循环至 GREEN（只读 canonical、只写评审记录；影响目标效果的打回项升级 human）；
3. 主线程凭「立项 ack + GREEN」标 FROZEN；
4. 主线程 packet-bound 落地（landing-manifest 绑定）+ 写 landing summary；
5. 主线程派 landing-verifier 独立核验——结构门 `landing-fidelity-check.mjs` + 逐文件人读 diff 核对实际落地 == frozen PROPOSAL 的 landing 清单 → 绿 → commit 分支；
6. 补写 retro report 闭环（骨架 `docs/templates/retro.template.md`）——趁热写、先于合并；
7. human 经 PR / 可回滚路径合并入 main。制品 4 件：PROPOSAL + landing-summary + landing-verdict + retro。状态转移时跑 `node .claude/scripts/changes-index.mjs --upsert <id>` 单行钉进 `changes/INDEX.md`。

**升级判据（命中任一 → Track A full-adversarial，入口 `/arch-change`）**：① 触契约面 `docs/contracts/**`（直走契约门）；② 跨系统结构性变更（如固件协议与服务端语义同时动）。governance 类变更不强制升级；「声明红队」= 自愿逃生门（human 或 architect 任一方随时可启）。**熔断**：评审 3 轮未 GREEN → human 三选一（转 Track A / 撤回·缩圈 / 换新判官实例再战 1 轮——仍不绿自动转 Track A）。**存疑向严**：判据适用存疑默认升 human。

**landing 通道（常态规则）**：恒 = 主线程 packet-bound 一条；architect 恒纯判官 / 起草、永不 landing。
**不动**：契约门（architect-reviewer 四分类 + human 签字）、landing-verifier 独立核验、机械强制层（scope-map / hooks）。

## Hard Rules

- 主 AI 不直改自有实现面做正式工作——实现动作走 dev-loop（planner Pod 派 acceptance-author → builder → verifier↔fixer → integrator → supervisor）；唯一例外 = human 明确说跳过流程的一次性操作，且须诚实记录「跳过流程」。
- **契约冻结**：任何会话不得用 Write/Edit/Bash 重定向写 `docs/contracts/**`，改动走 architect-reviewer 四分类 + 人类契约门。
- Completion 不凭解释宣布——治理变更完成 = landing 完成 + landing-verifier 绿 + retro 齐。
- After `git add`, verify with `git diff --cached --stat` before committing. 历史记录类文件一律 append，不覆盖既有行。
- 本文件遵循 Instruction Budget 三问（模型自发现？所有任务用？数月后仍立？），定期复评修剪。

## CodebaseMemory 使用纪律

- 本仓当前**未建** CodebaseMemory 图索引（ESP-IDF/C++ 支持度待评估）。未建期间，「trace before edit」降级为 Grep + 人工影响面说明，如实记录降级即可——降级是证据事实，不是违规。
- 将来建索引时项目名必须等于目录名 `ESP32`，否则图工具 project not found。
- vendor 面（`ESP32-S3-RLCD-4.2-Demo/`）将来也不入图索引，避免第三方代码淹没调用图。

## 输出标准（人与工件统一）

agent 的散文产出用简体中文写成通顺、完整的句子；领域常用英文术语（skill 名、门名、git/CLI 命令、track/gate/review 类高频词）可保留。内部标识符首次出现必须带一句实质说明；向 human 提问或汇报时不裸甩编号。机器校验的结构字段保持精确格式；证据引用保持逐字精确。散文通顺、引用精确，两者不冲突。禁止读者需自猜的拼接缩写。
