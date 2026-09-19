# architect-landing-summary · 260918-gov-review-merge-chain-friction

packet-id：FROZEN PROPOSAL v1.0 §2 改法集（T1）+ §5 验证集 · 评审轮次 R1 BLOCK(F1/F2) → human 2026-09-19 裁定移出 T1〔升级闸 GREEN 机锚〕 → **R2 GREEN** · change-id 260918-gov-review-merge-chain-friction

## 0. 本批特殊形态（先说清）

本批是四批拆包族里**形态最特殊**的一批：它的主体不是「改文件内容」，而是**改仓库索引态**（11 件运行产物 `git rm --cached`），另加**一行纯注释**。

## 1. 落点对照（manifest 12/12）

| manifest 路径 | T 项 | 落了什么 |
|---|---|---|
| `.claude/scripts/pre-merge-escalation-gate.mjs` | T1 ② | 在 `:199` 的 `if (!conflictedFiles.length) return 'union-safe';` **上方追加 5 行纯注释**，记录核查结论：`merge=union` 文件由 git 自动并、**不出现在 `conflictedFiles`**，天然落该分支判 `union-safe`（② 级不升人），**无需为 union 文件加显式豁免**；依据 = 该行分支逻辑 + ARQ-260917 记录的 k3 修复轮 r15 合并实测「tier3 评审噪音收口」；并点明下一行 `VERSIONED_ARTIFACT_RE_LOCAL` 是**另一条并列路径**、非 union 情形所依赖。**该改动是纯追加注释——零删除、零可执行语句变更**（V5 机械证）。 |
| 11 × `delivery/taskpackages/*/overlap-probe.json` | T1 ① | 全部执行 `git rm --cached`（**只出索引、工作树文件保留**），使 `.gitignore:46` 的既有条目真正生效。逐条：`260829-hardware-validation` / `260901-ui-art-integration` / `260911-display-first-m1` / `260911-ecs-deploy-convergence` / `260911-key-route-a1` / `260911-render-pages` / `260911-screen-unification` / `260915-m1-k1a-key-layer-dispatch` / `260915-m1-k2-keybar-three-slot` / `260915-m1-k3-voice-round-ui` / `260915-m1-n1b-p1-home-battery`。 |

```landing-manifest
.claude/scripts/pre-merge-escalation-gate.mjs
delivery/taskpackages/260829-hardware-validation/overlap-probe.json
delivery/taskpackages/260901-ui-art-integration/overlap-probe.json
delivery/taskpackages/260911-display-first-m1/overlap-probe.json
delivery/taskpackages/260911-ecs-deploy-convergence/overlap-probe.json
delivery/taskpackages/260911-key-route-a1/overlap-probe.json
delivery/taskpackages/260911-render-pages/overlap-probe.json
delivery/taskpackages/260911-screen-unification/overlap-probe.json
delivery/taskpackages/260915-m1-k1a-key-layer-dispatch/overlap-probe.json
delivery/taskpackages/260915-m1-k2-keybar-three-slot/overlap-probe.json
delivery/taskpackages/260915-m1-k3-voice-round-ui/overlap-probe.json
delivery/taskpackages/260915-m1-n1b-p1-home-battery/overlap-probe.json
```

## 2. 验证集执行记录（V1–V7）

| 项 | 命令 / 方法 | 结果 |
|---|---|---|
| V1 | 撤前 + 撤后各跑 `git ls-files \| grep -c overlap-probe.json` | **撤前 = 11**（>0，非空基线已断言）；**撤后 = 0** ✓ |
| V2 | `git status --porcelain --untracked-files=all` + `ls` 在盘核 | 11 件均 `D`（出索引）；**在盘文件数 = 11**（工作树未删）✓ |
| V3 | `git log --oneline -1 -- delivery/taskpackages/260829-hardware-validation/overlap-probe.json` | 命中 `bd81f57`（历史仍在、撤跟踪不改历史）✓ |
| V4 | `node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` | **30/30 全绿**、exit 0，**与改动前零差异**（纯注释改动 → 未触判定语义）✓ |
| V5 | `git diff .claude/scripts/pre-merge-escalation-gate.mjs` 逐行归类 | **非注释改动行数 = 0**；全部 **5 行均为 `//` 追加**、**零删除** ⇒ 可执行语句零变更 ✓ |
| V6 | `route-regression-check.mjs` | `28 ok, 0 fail` ✓ |
| V7 | `landing-fidelity-check.mjs <本批>` 结构门 | 见 §5（由 landing-verifier 亲跑） |

**真空绿规约核对**：V1 **先断言撤前 > 0**（11），否则「撤后 = 0」可能是真空绿；V4 为「零差异」型断言（纯注释改动的正面证据）；V5 是 riskTier=low 与 profile 声明的**机械证据**。

## 3. 落地中的事实性偏差（如实申报）

1. **V5 的首版实现踩线、已改回纯追加**：首版把原行尾注（`// 相交但可自动并…`）上移成独立注释行——**语义零变**，但 `git diff` 呈现为 1 删 1 增的「非注释行对」，**使 V5 的字面断言（非注释改动行数 = 0）不成立**。已改为**保留原行逐字不动、仅在其上方追加 5 行注释**，V5 现**字面成立**（0 非注释行、5 行追加、0 删除）。如实登记这次自我纠正。
2. **跨工作树过渡预期（FROZEN §2 T1③，本批特有余项）**：撤跟踪后 ff 合并进 main（`C:/AI/KidPal`）时，这 11 件会从 **main 工作树盘上消失**（tracked-deleted）；Pod 工作树若恰带本地脏 probe，会在过渡期撞一次「local changes would be overwritten」——**正是本批要治的形态，属过渡期一次性代价、非新增缺陷**。文件为**可再生运行产物**（gate 每跑 `--probe-out` 重写、merge-driver 传入该路径），**无数据丢失**。**本条按 T1③ 要求如实写进诚实注记段。**
3. **结构门 citation advisory 5 条（如实披露）**：门报

   | 引据 | 形态 |
   |---|---|
   | `260914-gov-review-delivery-restructure/PROPOSAL.md:504` | 兄弟批次短引用（缺 `delivery/changes/arch/` 前缀） |
   | `260915-gov-review-worktree-pool-reuse/PROPOSAL.md:3` | 同上 |
   | `260830-arch-review-ui-impl-strategy/PROPOSAL.md:215` | 同上 |
   | `260906-gov-review-index-status-truth-source/PROPOSAL.md:168` | 同上 |
   | `architect-review-r1.md:162` | 同批评审文件短引用 |

   全部落在 **PROPOSAL §7（R1 判官记录）与 §1 自证引文**内——**append-only 面、落地者不可改**。门照设计「只观察不产 fail」（`ok:true` 不受影响）。**与 P4 残留的 `260914-…/retro.md:38` 属同一族**（兄弟批次目录的短形态引用），**本批不治**；如实登记，**建议随「citation 候选域」的下一个治理批一并评估**（该族形态为 `delivery/changes/arch/<batch>/…` 的裸相对名，与 P4-T1 已覆盖的前缀族不同）。

## 4. 残余风险 / 留根指针

- **留根①（T1〔升级闸 GREEN 机锚〕移出）**：完整交接见 PROPOSAL §6 留根①——含**真机制更正**（「恒真」来自 `PROPOSAL.template.md` 头部样板行 `**GREEN 结论前必须在场**`，实测 **16/26** 份含它、单独即令现行 regex 为真）、**真实 verdict 形态 ≥8 种跨两套惯例**、**四候选实测全不成立表**（两候选各漏 3 个已冻结批 = 会错拦合法合并）、以及**建议方向**（判官 verdict 行规范化 + 机器锚）。**该 ARQ 仍在册未结**，队列行已补注本批实测结论。
- **留根②（同族追踪产物文件的系统性清点）**：本批只治 `overlap-probe.json` 这一件；是否还有其他「每次运行重写却被误入库」的产物，本批未做 census。
- **留根③（ARQ-260917 的处置状态）**：① 已由 `c16283b` 落地；②（Step 5b 簿记落点迁出）未做；③ 本批 T1② 已核并成文。
- **交日落复盘项**：T1① 后观察——是否仍出现「merge 前 rebase 撞 unstaged」的同族事故；若出现，即清点未穷举或有新的追踪产物形态。
- **本批为四批拆包族之一**（另三批 = dispatch-chain / closeout-chain / batch-drafting〔均已 landed〕）。四批零文件重叠。

## 5. 落地者自述边界

本摘要由主线程（落地者）自述，**不自评通过**。保真度判定归 `landing-verifier`（其独立跑结构门 V7 + 逐文件人读 diff 对照 FROZEN §2）。

## 6. 核验后注记（append-only，2026-09-19）

landing-verifier 判 **PASS + 2 ADVISORY**（`landing-verdict.md`）。两处观察及处置——**本节只追加，上文不改**：

- **ADVISORY-1（措辞级，非 mismatch）**：FROZEN §2② 写「补**一行**注释」，落地为 **5 行**注释块。核验官判：内容与 §2② 四要素 + §1 残余③ **完全对应、无超纲**；FROZEN 自设的 V5 判据是「非注释改动行数 = 0」（即**改动须落在注释行内**），**非指定行数** ⇒ 不构成 mismatch。**处置：本批不动**（已通过核验，事后改动会使 verdict 失效）。
- **ADVISORY-2（记账遗漏，已在此补记）**：上文 §2 的 **V6 行只记了 `route-regression-check`（28 ok/0 fail），漏记 `lint-descriptions.mjs` 半支**。核验官亲跑 lint → 1 warn（`landing-verifier.md` 超长）、**exit 0**，恰为 FROZEN V6 允许的既有告警 ⇒ **V6 实际通过**，属摘要少记一支。**补记于此：V6 = route-regression `28 ok, 0 fail` + lint-descriptions exit 0（仅既有告警）。**
- **nuance（核验官附注）**：上文 §2 的 V2 措辞「显示为 untracked」——因 `.gitignore:46` 忽略，`git status --porcelain` 实际显示 staged `D` 而非 `??`；上文已如实记「均 D」，未误报。