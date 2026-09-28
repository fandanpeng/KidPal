# architect-landing-summary · 260928-gov-review-requirement-loop

packet-id：FROZEN PROPOSAL §2 改法集（T1–T7）+ §5 验证集 · 评审轮次 R1 BLOCK(F1,F2) → R2 BLOCK(F8) → R3 GREEN（[review.md](review.md)）· change-id 260928-gov-review-requirement-loop

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 流程改版（状态机 + 盘上审稿 + architect 必派 + 消费保护） | `.claude/references/requirement-change.md` | 新增「头部状态机」节（置/翻规则 + 档 0 豁免 + 边界场景两句）、「消费保护规则」节；档 1/档 2 流程步骤改盘上起草与盘上审稿（含改前/改后对照与 dispatch 输入构成）；角色分工表五行全改 |
| T2 story-author 定义改版 | `.claude/agents/story-author.md` | description 交付形态句改盘上写；tools 行 `Read, Edit, Write, Grep, Glob, Bash`（F1 第 ④ 点）；「交付形态（盘上交付）」节含改前/改后句级对照（F2）；「硬边界」写面两路径 + Bash 白名单 bullet 原位保留（F8 口径）；职责边界两条不动 |
| T3 requirement-reviewer 定义小改 | `.claude/agents/requirement-reviewer.md` | description「落出 updating 态修订后」；四查节增审稿对象句（盘上文件 + 对照两相核对）；结论形态「引盘上修订原句」；tools 不变（恒无 Write） |
| T4 scope-map 写槽 | `.claude/scope-map.json` | story-author 条目 `allow: ["docs/requirements.md", "docs/architecture.md"]`、`deny: ["docs/contracts/**"]`（OQ-1 裁 (a) 双路径在位）；requirement-reviewer 条目不动 |
| T5 新增命令 | `.claude/commands/requirement-change.md` | 新建；八步工作流（档位判定 / 档 0 收束 / 盘上起草 / architect 三件 / 四查否回循环 / 落稿 / 机检与消费保护 / 记账收尾）；harness 已注册（会话 skill 列表实见 /requirement-change） |
| T6 spec 面状态行 + §9 句 | `docs/requirements.md` | 头部 `:8` 新增状态行（frozen · v1.59 · 2026-09-29）；§9 纪律引言追加状态机与消费保护句（句子级增补，零既有行覆盖） |
| T7 AGENTS.md 入口行 | `AGENTS.md` | 「需求面变更」节改写为 /requirement-change loop 入口（含免派例外注记与两批凭据，OQ-4 裁 (a) 携带） |

**manifest 外落地偏差一处（如实登记）**：`.claude/route-fixtures/routing.json` story-author 必留信号 `无 Write` → `frozen 翻转不归本 agent`——原断言被 FROZEN T2（经 R1 F1 评审确认的 tools/写权变更）直接证伪，V3 首跑红（`✗ SIGNAL: story-author description dropped required routing signal "无 Write"`），最小修正后 V3 绿（36 ok / 0 fail）。该文件不在 landing-manifest（PROPOSAL §6 留根仅记「补新命令断言」，未预见旧断言被证伪）；判定 = FROZEN 改法的机械必然推论（description 新信号集的 fixture 对齐），非语义扩权，留 landing-verifier 复核。

## 2. 验证证据

| # | 命令 | 实际输出关键行 | 判定 |
|---|---|---|---|
| V1 | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260928-gov-review-requirement-loop/PROPOSAL.md` | `ok: true anchors: 7 findings: []`（landing 后复跑，FROZEN 前同绿） | 绿 |
| V2 | `node .claude/scripts/lint-descriptions.mjs` | `exit 0`；唯一告警 = 既有 `landing-verifier.md: over-length (363 > 320)`，本批未触该文件（基线告警，260928 起草期已在册） | 绿（告警级） |
| V3 | `node .claude/scripts/route-regression-check.mjs` | 首跑红（上节偏差）；fixture 最小修正后 `36 ok, 0 fail` | 绿（修正后） |
| V4 | `node .claude/scripts/spec-anchor-check.mjs` | `"storyCount": 13, "anchorLines": 31, "errors": [], "ok": true`——与 260928 spine anchors 段基线（60/46/13/31 四组数）一致 | 绿 |
| V5 | `grep -n "^> 状态：" docs/requirements.md` | 恰一行 `:8`，`frozen` 态在位（名实注记：状态行在场自检；消费保护 = 约定级成文，F3 口径） | 绿 |
| V6 | 三文件残留 grep（`同会话 return` / `无 Write` / `不落任何文件` / `交回修订稿`） | 三命中全为合法存活句：requirement-change.md:72（reviewer 行「恒无 Write」——reviewer 本就无写权，语义正确）、story-author.md:29（「同会话 return 交主线程的内容」——新交付形态正文用词，无旧语义）、requirement-reviewer.md:27（reviewer 无 Write——正确语义）；`不落任何文件` / `交回修订稿` 零命中 | 绿（三归类销项成立） |
| V7 | enforce-scope 行为探针（story-author 写 requirements 放行 / 写 contracts 拦截） | 待本 commit 落盘后执行（post-commit 项，探针结果随 verdict 补录） | 待执行 |
| — | `node -e "JSON.parse(scope-map)"` | `scope-map JSON ok`（T4 改后形态合法） | 绿 |

真空绿规约：V1/V3/V4 均为真实脚本实跑、非重放断言；V6 输入非空（三文件在盘、模式四条全跑）；无新增自检依赖。

## 3. 诚实注记

- **manifest 外偏差一处**：`.claude/route-fixtures/routing.json`（见 §1 尾段）——本 summary 明示申报，请 landing-verifier 将其纳入「偏差 vs FROZEN packet」核对而非按未申报溢出处置。
- **约定级约束披露（R2 F8 口径）**：story-author 两 allow 路径的 Bash 重定向写不在 `universal.bashDenyWrite`（scope-map.json:25 仅 contracts / .claude / taskpackages 三项）覆盖内，旁路约束 = agent def「Bash 仅限跑 spec-anchor-check」白名单（约定级）；Bash 旁路机拦新齿归 OQ-3 留根同流。
- **消费保护为约定级成文**（OQ-3 裁 (a)）：机检齿（taskpackage-required-check 校验引用版本号在 §9 在位）留根，实包 2–3 单后立项。
- **未落项（均为 FROZEN §6 既有留根，非缩水）**：route-fixtures 补 /requirement-change 新命令断言（下批顺带）；架构档挂入同一 loop（需求面 loop 实跑后）；architect.md 入口清单补档 2 条目（self-def 边界）；launchPreconditions 接线与 spine anchors 联动（ARQ-260928 批 / 首个 spine 实证）。
- **降级**：无。**ADVISORY 类披露**：lint 既有告警 1 条（landing-verifier.md 超长，基线在册，本批未触）。

```landing-manifest
.claude/references/requirement-change.md
.claude/agents/story-author.md
.claude/agents/requirement-reviewer.md
.claude/scope-map.json
.claude/commands/requirement-change.md
docs/requirements.md
AGENTS.md
```
