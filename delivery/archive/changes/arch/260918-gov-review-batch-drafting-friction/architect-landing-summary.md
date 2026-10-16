# architect-landing-summary · 260918-gov-review-batch-drafting-friction

packet-id：FROZEN PROPOSAL v1.0 §2 改法集（T1–T3）+ §5 验证集 · 评审轮次 R1 BLOCK → R2 BLOCK → **R3 BLOCK（熔断）** → human 裁定缩圈 → **R4 GREEN** · change-id 260918-gov-review-batch-drafting-friction

## 0. 前置：main → gov 回同步（本批 landing 的共同前置）

落地前先做了 `main → gov/kidpal` 回同步 merge（commit `2492f44`）：main 侧 27 个 commit 拉回，使 `gov → main` 的 ff-only 前置成立（回同步前 main 独有 27 / gov 独有 19，非 ff）。
- **双侧都改的两件**（`docs/ARCH-REVIEW-QUEUE.md` / `delivery/changes/dev/INDEX.md`）均为**追加行**，git 自动并、**零冲突**；已复检双侧行俱在（main 的 `ARQ-260918-spec-face-3page-reconcile` / `ARQ-260918-pod-session-settings-identity-unobservable` 两条 + 本会话的簿记改动；dev INDEX 的 m2-k2/m2-k3 两行 + 本会话补的 m2-k1 行）。
- 按 `merge-to-main` skill 的回同步例外动作，先 `git checkout HEAD --` 还原 `.codebase-memory/` 两件再生漂移再 merge。

## 1. 落点对照（manifest 3/3）

| manifest 路径 | T 项 | 落了什么 |
|---|---|---|
| `.claude/scripts/landing-fidelity-check.mjs` | T1 + T2 | ① **T1**：citation 前缀候选域由硬编码四前缀改**并集形态**——新增 `CITATION_BASE_PREFIXES`（原四前缀原样保留 = 防回归）+ `citationPrefixes(dir)`（`docs/`、`.claude/` 树下**递归**派生、**钉字典序**、按 dir 缓存）；`:90` 旧硬编码循环改调该函数。② **T2**：新增 CLI 解析纯函数 `parseCliArgs()`（`--preflight` 在场时 `positional`/`baseRef` 均置 null，**不误触 reviewDir/baseRef**）+ `BOLD_ANCHOR_RE` / `section2Body()` / `preflightFindings()` / `runPreflight()` + 干跑入口（置于 `--self-check` 之后）；自检电池新增 `runPreflightSelfCheck`（9 断言）并接线。 |
| `docs/templates/PROPOSAL.template.md` | T2 | 硬项 2a 段后新增「**【起草期自检两句】**」：① FROZEN 前跑 `--preflight` + §5 验证集命令实跑；② 路径类变更先查 `path-form-family.md` 族谱表。 |
| `.claude/references/path-form-family.md` | T3 | **新增 65 行**：八类路径文本形态 + 各自「残留清扫子检模式」+ 三类批量替换风险注记 + 损伤总账（四数分属四类）+ 第 6 类实现-判据不一致教训 + 五步用法 + 完备性边界声明（该批实测八类、非理论穷举）。 |

```landing-manifest
.claude/references/path-form-family.md
.claude/scripts/landing-fidelity-check.mjs
docs/templates/PROPOSAL.template.md
```

## 2. 验证集执行记录（V1–V9）

| 项 | 命令 / 方法 | 结果 |
|---|---|---|
| V1 | `node .claude/scripts/landing-fidelity-check.mjs --self-check` | **全电池绿**（exit 0）：`citation-anchors 14/14`（含本批两新增夹具）、**`landing-preflight 9/9`（本批新电池）**、其余八套零回退 |
| V2 | 同上电池内 | 「干跑模式 positional/baseRef 均为 null」两断言通过（`--preflight` 不误触 reviewDir） |
| V3 | `--preflight <本 PROPOSAL>` | `ok:true` / `findings:[]` / `anchorCount:3` / exit 0 |
| V4① | 注入 §2 域内行首加粗锚行的副本 → 干跑 | 报 `bold-anchor-line` + `missing-anchor-list`、**exit≠0** ✓ |
| V4② | 抽掉某 T 整条锚清单行的副本 → 干跑 | 报 `missing-anchor-list` ✓ |
| V4③ | §2 域内**行内引用**该错误形态的副本 → 干跑 | **不报** `bold-anchor-line` ✓（R2-F8 反例） |
| V5 | 四夹具对照（真实仓） | 派生集 **43 项**；新增两条 `dev-loop-operating-system.md` → `.claude/references/`、`PROPOSAL.template.md` → `docs/templates/` **均 resolved**；防回归两条 `DEV-GOVERNANCE.md` → `docs/`、`governance-gate-shared.mjs` → `.claude/scripts/lib/` **均仍 resolved** ✓ |
| V6 | `grep -c "path-form-family" docs/templates/PROPOSAL.template.md` | ≥1 ✓ |
| V7 | `ls .claude/references/path-form-family.md` + 八形态行计数 | 在场；`^\| [1-8] \| \*\*` 命中 **8** ✓ |
| V8 | `route-regression-check.mjs` + `lint-descriptions.mjs` | 全绿（lint exit 0；仅既有 `landing-verifier.md` 超长告警，本批未触该文件） |
| V9 | `landing-fidelity-check.mjs <本批>` 结构门 | 见 §4（由 landing-verifier 亲跑） |

**真空绿规约核对**：V4 三例均为**反例夹具**（注入 / 抽掉 / 散文），非恒绿；V5 的「新增两条」与「防回归两条」分列（防回归口径为**确认既有件保留并具名**，非本批新增——R2-F12）；V1 输出**逐电池具名**计数，防「加了夹具没接线」。

## 3. 落地中的事实性偏差（如实申报）

1. **自检电池计数已增**：`citation-anchors` 由 5 条增至 **14 条**、全电池新增 `landing-preflight` 电池（9 条）。PROPOSAL §5 V1 写的是「新增两夹具 + 确认保留两条」——落地实现为 **+9 条 citation 断言（两新增组 + 具名声明两既有组）+ 9 条 preflight 断言**，方向一致、数量多于字面。**不改冻结稿**，偏差在此申报。
2. **`--preflight` 入口置于 `--self-check` 之后**：PROPOSAL §2 T2 只要求「与既有位置参数形态互不干扰」。实现取「先跑自检（终端式 `runIsFrozenSelfCheck` 会 exit）、再跑干跑」——两者同给时以自检退出码为准。此为接线细节的具体化，未改任何判据。
3. **`citationPrefixes` 按 dir 缓存**：PROPOSAL §2 T1 未提缓存。实现加 `Map` 按 `resolve(dir)` 缓存——**必须按 dir 缓存**，因为自检夹具用临时目录；若做全局单例缓存会让夹具复用真实仓的派生集、造成**假绿**。此为防假绿的具体化。
4. **`parseCliArgs` 的常规分支**：保持改前行为逐字一致（`positional = argv[2]`、`baseRef = argv[3]`），未顺带修正「`--self-check` 会被当作 positional」这一既有小瑕（改了会动到既有 argv 行为面，超出本 T 授权）。

## 4. 残余风险 / 留根指针

- **留根⑥（缩圈移出的第 3 类）**：`--preflight` **不覆盖**「幻影路径 / 注记夹带路径 token」这一类——三轮评审证明其判据设计（对锚并集做仓内存在性检查）**建立在「不存在的即幻影」这一不成立假设上**（§2 锚清单里的路径合法地就可能不在场：新增/迁入/撤回/迁走四形态），枚举补丁不可收敛。完整交接（目标 / 三轮证伪表 / 根因定性 / 两条替代方向 / 下一批动作）见 PROPOSAL §6 留根⑥。**本摘要如实声明：本批覆盖面为两类，非三类。**
- **留根②（验证集「实跑」的机械化）**：模板 checklist 第二句要求「§5 验证集命令实跑」，但**「是否真跑过」无机械判据**（仍是纪律句）。
- **落地后建议**：`/loop-retro` 观察——是否还有 landed 期才暴露的 PROPOSAL 形态错（即 `--preflight` 未覆盖的错类）。
- **本批为四批拆包族之一**（另三批 = merge-chain / dispatch-chain / closeout-chain，均已 FROZEN）。四批零文件重叠；本批先落，为使**后续三批的 landing 核验都用改好后的结构门跑**。
- **结构门 advisory 一条（如实披露）**：门报 `citationAnchorAdvisory.problems = [{ citation: "260914-gov-review-delivery-restructure/retro.md:38", reason: "file-not-exist" }]`。该引据在 **PROPOSAL §7 的 R4 判官记录**内（**append-only 面、落地者不可改**），形态为**缺 `delivery/changes/arch/` 前缀的短引用**；门照设计「只观察不产 fail」（`ok:true` 不受影响）。**落地者不动 §7**，如实登记为残余 advisory；若判官认为该形态值得前缀域覆盖，属下一批的候选（与留根⑥ 同族面）。

## 5. 落地者自述边界

本摘要由主线程（落地者）自述，**不自评通过**。保真度判定归 `landing-verifier`（其独立跑结构门 V9 + 逐文件人读 diff 对照 FROZEN §2）。

## 6. 核验后注记（append-only，2026-09-19）

landing-verifier 判 **PASS**（`landing-verdict.md`）。两处 advisory 及处置如下——**本节只追加，上文不改**（保已核验内容原样）：

- **ADVISORY-1（数字校正）**：上文 §3-1 写「`citation-anchors` 由 **5 条**增至 14 条」——**基线数字有误**，核验官以 `git show HEAD:` 逐条数得**改前为 8 条**，本批新增 **6 条**（references 组 3 + templates 组 3），即 **8 → 14（+6）**。方向与落地均无误，仅基线计数应更正为 **8**。上文按 append-only 不追改，以本行为准。
- **ADVISORY-2（残余，不修）**：`BOLD_ANCHOR_RE` 为行首锚定（符 FROZEN §2 T2 字面要求），但**未镜像 parser 的 `^\s*>` 去引用**——对 `> **目标文件**：` 形态探针不具名；不过该形态仍会被 parser 报为 `missing-anchor-list`（**无假绿、仍 red**，只是具名不精确）。FROZEN 未硬性要求引用前缀镜像；且**代码已通过核验，事后改动会使 verdict 失效**，故本批不动、登记为残余。**若将来判官认为应补镜像，随下一批处理。**