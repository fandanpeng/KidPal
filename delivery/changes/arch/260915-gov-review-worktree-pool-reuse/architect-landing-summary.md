# architect-landing-summary · 260915-gov-review-worktree-pool-reuse

packet-id：FROZEN PROPOSAL §2 改法集（T1–T6）+ §5 验证集（V1–V9） · 评审轮次 R1 需修订 → R2 GREEN（review-r2-architect.md；N3/N4 NIT 冻结前已收进 T1②/V8） · change-id 260915-gov-review-worktree-pool-reuse

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 ①–⑤ launch-prep skill 重写（树池 + 换包协议 + base ref 三处对齐 + 启动卡收缩） | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | 产物清单③、步骤 2（门命令 + 基线锚点/差异判停/第三态五要素）、步骤 3（换包协议四条 + 按需新建命令）、步骤 4（仅新建树）、步骤 5/6（复用树沿用索引/重指）、步骤 7（四要素启动卡）、坑节（传染条移位 + 交接卫生新增）全部落地 |
| T1 ⑥ dev-loop-operating-system 同族措辞 | `.claude/references/dev-loop-operating-system.md` | `:50` 每包 worktree add → 运行时 worktree（复用 ∨ 按需）+ §2c 指针 |
| T2 pod-coordination 增「worktree 池复用」节 | `.claude/references/pod-coordination.md` | 新增 §2c（无结构绑定 + 换包协议四条 + 相容声明）；§2b 机械承载行的「worktree add 前」扩为「新建或复用换包前」（连带一行，同文件一致性） |
| T3 基线锚点 + 差异判停 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` 步骤 2 | producer 五要素（计数+命令+日期+量测 SHA+面域）、consumer `git diff --quiet <anchor> HEAD -- <面域>` 判停、带锚引用三件齐判据、边界（本包验收齿恒现跑）全部成文 |
| T4 integrator 分支亲核对齐 | `.claude/agents/integrator.md` | `:64` 亲核改「本树常驻分支（非 main/master 即过，merge-driver 同口径）」；`:68` rebase/merge 限常驻分支；「禁引入 merge commit」驱动契约原文保留 |
| T5 kidpal-idf-env 补两坑 | `.claude/skills/kidpal-idf-env/SKILL.md` | 新增「Two more WSL gotchas」节（-T 失效 + MSYS_NO_PATHCONV，含成功形态命令块），证据指针引本批 evidence-wsl-pitfalls.md |
| T6 architecture §5.5.6 开发拓扑记账 | `docs/architecture.md` | §5.5.6 构建/测试两行之间插入「开发拓扑」一行（树池 + 环境等价三件套 + 热缓存随树 + skill 指针）；纯记账无行为变更 |

## 2. 验证证据

V1 `grep -n 'worktree add' SKILL.md` → 唯一命中 = 步骤 3 按需新建条件形态（`-b <常驻分支名> <专用 base 分支>`）✓。V2 `grep -nE '当前 main|--base-ref main|-b dev/<changeId> main' SKILL.md` → 0 命中（exit 1）✓。V3 pod-coordination「worktree 池」节在位，四关键词（空闲判读/常驻分支/交接卫生/按需扩容）grep 全中 ✓。V4 SKILL.md 锚点协议 5 命中 +「三件齐」1 命中 ✓。V5 integrator `dev/<changeId>` 0 命中 + 驱动契约句在位 ✓。V6 kidpal-idf-env MSYS/No-such-option 4 命中 + `-T main` 仅以「已失效」警示形态出现 ✓。V7 architecture `:301` 开发拓扑行在位含三关键词 ✓。V8 dev-loop-operating-system `每个 ready TaskPackage` 0 命中 ✓。V9 = landing 后即时试验（N1b 换包，未到期、留观察）。连带守卫：`spec-anchor-check.mjs` ok:true（architecture 触碰零锚伤）；`route-regression-check.mjs` 28 ok / 0 fail；`lint-descriptions.mjs` 仅 1 条既有 warn（landing-verifier.md 超长，与本批无关）。

## 3. 诚实注记

1. **V8 落地期返工一次**：首版替换行保留了「每个 ready TaskPackage」前缀，按冻结 V8 字面不达标——已改写为「TaskPackage ready 即就运行时 worktree…」后清零（零语义变更，措辞级返工如实记录）。
2. **pod-coordination §2b 连带一行**：机械承载行「在 `git worktree add` 前跑」扩为「在新建 worktree 或复用换包前跑」——不在 §2 任何 T 的逐字改法内，属 T2 同文件一致性连带（复用形态下该行字面不可达），landing-verifier 人读 diff 核对时请注意此行。
3. **残留观察**：V9（N1b 在 K1a worktree 的首例换包 + 基线差异判停）按 OQ3 排期在 K1a 合并后执行，证据将落 `delivery/workstreams/m1-home-qa/progress.md`——不阻塞本批收口。
4. lint-descriptions 1 条既有 warn（landing-verifier.md description 超长）非本批引入、未处理（留根：下个触 landing-verifier 的治理批顺手收）。

```landing-manifest
.claude/agents/integrator.md
.claude/references/dev-loop-operating-system.md
.claude/references/pod-coordination.md
.claude/skills/kidpal-idf-env/SKILL.md
.claude/skills/kidpal-pod-launch-prep/SKILL.md
docs/architecture.md
```
