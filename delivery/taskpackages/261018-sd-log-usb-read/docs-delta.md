# docs-delta · 261018-sd-log-usb-read

> integrator · 2026-10-19 · 分支 `dev/pod-m2` · rigor=lite。本文件是 closeout 的 docs-sync 记账与承接声明：本包**零新增 canonical 文档改动**，下文按 `docsSyncRequired=true` + `docsOwner=integrator` 的闸判据写声明与核证，不重复改写任何 canonical 文档。

## 1. 结论：本包零新增文档改动（docsNoChangeReason 口径 + 核证）

按 TaskPackage `docsNoChangeReason` 既定口径，docs-sync 的落地方式 = **声明 + 核证**：

| 面 | 状态 | 理由 / 落地位置 |
|---|---|---|
| 架构面（决策 24 白名单「设备诊断日志」+ 六处镜像 + ADR-012 + §6.6 canonical 文本） | 已先行落地（不在本包 diff） | commit `5a37b001`（261018 persist 包架构面 landing，landing-verdict GREEN）；本包只消费其形态边界（常开 + 有界轮转 + 16MB 封顶），只增只读取回命令、不改架构决策与 §6.6 canonical |
| 隐私记账 | 已先行落地（不在本包 diff） | `docs/debt/tech/TECH-261016-sd-story-confidentiality.md`（commit `27b4ecd5`）；本包取回命令不新增数据类——回取的仍是已落卡的诊断日志，物理访问威胁模型同族未加宽（architect 比例化 review 结论，securityReviewRequired=false） |
| 契约面 | 零改动 | `docs/contracts/**` 零 diff；本包零 wire 形状改动（KPLOG 帧为设备↔host 本地串口诊断协议，生产端与消费端同在本包 changeManifest 内，不触端云契约面）；契约四分类 = `NO_CONTRACT_CHANGE` |
| 需求 / 架构 spec 面 | 零改动 | `docs/requirements.md` 零 diff；`docs/architecture.md` 自 persist landing 后零 diff（本包 LUR-X3 边界核证项） |
| 取回命令是否需入 canonical | 裁定：不需要 | `tools/diag/kidpal_logs.py` 与 `kplog` 命令属诊断工具面（开发期人工持板使用），非运行时行为、非架构决策面，不入 `docs/architecture.md` §6.6 与需求面；用法自描述在脚本 docstring 与 plan §8 |

**核证命令与结果**（2026-10-19，本 worktree 实跑）：

- `git diff HEAD -- 'firmware/partitions*' 'firmware/sdkconfig*'` → 空（LUR-X3 分区表与 sdkconfig 零改动成立）。
- `git diff HEAD -- docs/` → 空；`git status --porcelain -- docs/contracts/` → 空（契约面零改动成立）。
- `git status --porcelain -- server/` → 空（`server/**` 零 diff，crossEnd=exempt 成立）。
- `docs-sync` 闸（verifier scoped r1，invocationId `56a4e75e-79c4-4c95-ab75-1282fe90835d`）PASS：`docRefs` 六条全部在盘、`docsNoChangeReason` 口径被闸接受。

## 2. 验收 id 与文档面的对应声明（docs-sync 闸判据要求提及 LUR-Ax）

本包验收 LUR-A1..A6 为纯逻辑 / 结构齿（host 承载），LUR-A7 为 live-manual 真机轮：**均无 canonical 文档效果面**——决策 24 白名单与 §6.6 描述的「设备诊断日志」能力已随 persist 架构面 landing 入 canonical，本包是在该能力之上的读取通道增量，不改变已定稿的架构语义；故 LUR-Ax 不产生任何 docs-delta 条目，本节即对应声明。

## 3. 未随本包完成、需承接的项（不表述为已完成）

1. **LUR-A7 真机轮（live-manual）九项 + 降级链 A/B** —— human 持板执行（`awaiting-human:real-device`），本 Pod 零烧录、零真机；单命令 smoke（`python tools/diag/kidpal_logs.py --port COM5 --cmd list --out list.txt`）先行，失败按 plan 降级 A（HAL 直读）/ 降级 B（console primary 切换，须回 `/triage-change` 升档重判）。
2. **final 相位 verifier round** —— 本包目前持有 scoped r1 全绿 fact（rigor=lite 快路径设计态：firmware-build 在 scoped 后置）；final 相位 round（含 firmware-build 真编译真跑 + scope-final.md 圈定范围求交）由 planner/verifier 以 `delivery/taskpackages/261018-sd-log-usb-read/scope-final.md` 为 scope-file 消费执行，integrator 只消费 fact、不重跑。
