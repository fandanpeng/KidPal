# architect-landing-summary · 260906-gov-review-index-status-truth-source

packet-id：FROZEN PROPOSAL §2 改法集（T1–T4）+ §5 验证集 · 评审轮次 R1 NEEDS-REVISION → 主线程修订 v0.3 → R2 GREEN · change-id 260906-gov-review-index-status-truth-source

> 落地人：主线程（Track B 步骤 4，packet-bound）· 日期：2026-09-06
> packet = FROZEN PROPOSAL + landing-manifest + 本 summary（architect-landing-summary）
> 凭据 = human 立项 ack（「记录ARQ，出proposal，快速修了」2026-09-06）+ OQ 三题全裁（OQ1=(a) / OQ2=(a) / OQ3=(a)，OQ1 经 human 提议 pod-retro 判据、主线程以 Q5 warn-only 具体失陷表说服、human 采主线程推荐 (a)）+ architect R2 GREEN

```landing-manifest
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/changes-index.mjs
.claude/scripts/closeout-check.mjs
```

> manifest 注：3 路径 = T1–T4 §2 目标文件锚并集（与 PROPOSAL 头部 manifest 注一致）；三文件全 G3 脚本/lib 面，CANONICAL_TEST 可机锚。

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 shared lib 新增收口谓词单源 | `.claude/scripts/lib/governance-gate-shared.mjs`（+`import { CHANGE_ID_PREFIX_RE } from './changeid-shared.mjs'`；新增 `export const CHANGE_ID_CUTOFF` + `isPostCutoffChangeId` + `closeoutRequiredFiles`，带来源注记） | ✅ 已落：2 编辑（import + 谓词块），自检 changeid-prefix 4/4 不回退 |
| T2 changes-index dev 状态推导换谓词 + arch 推导补双锚 | `.claude/scripts/changes-index.mjs`（import 加 `closeoutRequiredFiles`；新增导出 `deriveDevClosed`；collectDev 删 R5 判据块换谓词；VERDICT_HEADING_RE 加编号容差 `(?:\d{1,3}[.、]\s*)?`；deriveLanded 行循环末加 JSON 行锚） | ✅ 已落：5 编辑，self-check 29/29 |
| T3 closeout-check 谓词改具名导入（零行为变更） | `.claude/scripts/closeout-check.mjs`（import 加三具名；头注 :85-88 更新指 lib SSOT 并列 3 处残余副本；本地 `const CHANGE_ID_CUTOFF` 删除；`:250`/`:251` 两处改走 `isPostCutoffChangeId(changeId)` / `closeoutRequiredFiles(changeId)`） | ✅ 已落：4 编辑，实况跑 gate ok:true、requiredFiles 输出 post-cutoff 两件逐字符合 |
| T4 selfCheck 夹具扩充 | `.claude/scripts/changes-index.mjs` selfCheck（landed 三例 f5/f6/f7：编号标题+加粗散文 GREEN→true / JSON 行锚 GREEN→true / 编号标题+散文 RED→false；dev 三例 dv1/dv2/dv3：post-cutoff 全件→closed / 缺 review-packet→open / pre-cutoff 缺 taskpackage→open） | ✅ 已落：total 23 → 29 全过 |

manifest 外零落地：契约面（`docs/contracts/**`）、`docs/spec.md`、自有实现面（firmware/server）、hooks 均未触碰（PROPOSAL §0 非目标）。

## 2. 验证证据（§5 重放，2026-09-06）

landing 前（执行侧自检）：

- **V1** `node .claude/scripts/changes-index.mjs --self-check` → `changeid-prefix 4/4` + `changes-index pass:29 fail:0 total:29 ok:true` ✅（基线 23 亲跑实测，+6 恰至 29，防真空绿断言成立）
- **V2+V3** 全量重建 `node .claude/scripts/changes-index.mjs` → `ok:true devCount:8 archCount:11`；INDEX dev 段 **8 行全 closed**（含无 supervisor-verdict.json 的 cloud-link-bringup / simulator-lvgl-align——sv 不可作判据的实证）；arch 段 **3 行翻 landed**（260905-gov-review-verify-gates-align + 260906-gov-review-arq-governance-batch + 260906-gov-review-architect-loop-advisory）；`260830-arch-review-ui-impl-strategy` / `260904-gov-review-escalate-merge-approval` 维持 frozen（散文式 / 行内 token 病灶，§6 留根）✅
- **V4** `node --check` 三文件全过 ✅；import 解析实测（`node -e import(...)`）不炸——且因 closeout-check 无 invokedDirectly 守卫，import 即实况主流程跑通：`ok:true gate:closeout changeId:260903-device-ws-switch requiredFiles:["docs-delta.md","review-packet.md"]`（post-cutoff 两件，T1 谓词经 lib 单源在承重闸上实况正确）✅
- **V5** 语义零变更：V4 实况输出 + T2/T4 夹具共同确认 pre/post 取值与 FROZEN §2 逐字一致（post 两件 / pre 三件，夹具 dv1–dv3 即 pre/post 反例对）✅

landing 后（核验侧，待 verifier）：

- **V6** INDEX 大 diff = 预期纠偏成果（8 行翻 closed + 3 行翻 landed + 段计数 10→11 因本包自身入列），非病灶 ✅（§2 重建输出在册）
- **V7** 结构门：`node .claude/scripts/landing-fidelity-check.mjs changes/arch/260906-gov-review-index-status-truth-source` → `ok:true`（frozen / packetBinding / unauthorized=[] / claimedNotLanded=[] / plannedNotLanded=[] / gitDegraded:false 全绿）✅；观察列 3 类 advisory 全部登记为已知机械局限（§3 诚实注记 4/5/6），非真实失配
- **V8** 人读 diff 三件：待 landing-verifier 逐文件核对（Track B 步骤 5）
- **V9** Track A 判据复核：manifest 三路径全 `.claude/scripts/` 面，零契约、零 G2、零实现面 ✅

## 3. 诚实注记

1. **落地期机械修正一处（如实登记，语义零变更）**：T4 首版实现把 `deriveDevClosed` 同时以「函数声明处 `export function`」与「文件尾 export 列表」两处导出，ESM 静态解析报 `Duplicate export of 'deriveDevClosed'`（node --check 即拦）。修正 = 删文件尾 export 列表中的重复项、保留函数声明处导出（导出面对外一致：bottom 列表本就不含它，属新增重复而非删既有导出）。`node --check` + self-check 29/29 双证修正后无漂移。
2. **V4 import 测试副作用说明**：closeout-check.mjs 无 invokedDirectly 守卫，`node -e import(...)` 会实况执行主流程（对仓内包跑 closeout gate）。本次输出 ok:true + 2 条 advisory（ws-switch machine evidence missing / loop-retro 阈值达），与 260906-arq 批 V5 advisory 枚举同款、均为在册既有态，与本批改法无因果，如实登记不代处理。
3. **deriveLanded last-match-wins 语义**：JSON 行锚置于行循环末位，与既有锚（STATUS/HEADING/pendingHeading-bold）冲突时后出者胜——FROZEN §2 T2② 钉死的语义，f3 夹具（RED→GREEN 取终）回归不回退。
4. **verbatim-quote advisory 2 条 = 已知环境性缺口（首见登记）**：闸对 PROPOSAL §2 围栏块与目标文件做字节级 includes；本仓 Windows 工作树 .mjs 全量 CRLF（`git ls-files --eol` = i/lf w/crlf，autocrlf 实况），PROPOSAL.md 经 Write 工具落盘为 LF，引号块 LF × 目标 CRLF 逐字必不命中。260906-arq 批 §2 零围栏代码块故未踩此缝，本批系首次使用逐字钉死围栏块的首见暴露。实质内容已 byte-diff 核验一致（逐字符对账止于 `\r\n`/`\n` 差异，block-char 87 / 69 处即行尾）。**根治 = 闸内 CRLF 归一化后 includes（readFileSync 后 .replace(/\r\n/g,'\n') 双侧同处理）**——闸脚本不在本批 manifest（frozen 范围外），登记 ARQ 归口后续治理批；本批按「登记缺口 + 人读 diff 担纲」先例处理，verifier 人读 diff 为语义保真正门。
5. **citationAnchorAdvisory 3 条 file-not-exist（首见登记）**：PROPOSAL §6 引 `taskpackage-required-check.mjs:95` / `devloop-artifact-check.mjs:70` / `hooks/supervisor-gate-check.mjs:58` 为裸文件名引用（无 `.claude/scripts/` 前缀），闸的 citation 解析按仓根相对路径解析故报 file-not-exist；三文件实存。同归闸健壮性缺口，与注记 4 合并登记同一 ARQ。
6. **changedCanonicalNotInProposal 3 文件**：`.planning/kidpal-mvp/progress.md`（会话前已 modified 的在册未提交态）+ `.claude/skills/kidpal-pod-launch-prep/SKILL.md`（未跟踪新 skill 目录）+ `docs/ARCH-REVIEW-QUEUE.md`（ARQ-260906-index-rebuild-dev-status-drift 登记行——本批立项动作、属 packet 治理制品但非 manifest 目标文件）——三者均非本批 landing 改动，如实登记交 moderator/人确认，与本批改法零因果。

## 4. 状态转移记录

- 2026-09-06 human 立项 ack（「记录ARQ，出proposal，快速修了」）+ OQ 三题裁断（OQ1=(a) 经 retro 判据讨论后采主线程推荐 / OQ2=(a) / OQ3=(a)）。
- 2026-09-06 architect R1 NEEDS-REVISION（N-1 缺锚必红 / N-2 翻转集 2→3 批 / N-3 删常量 ReferenceError + NIT-1~4）→ 主线程修订 v0.3 → R2 GREEN（R2-NIT-1~4 在册）→ 主线程凭「立项 ack + GREEN」标 **FROZEN v1.0**。
- 2026-09-06 主线程 packet-bound 落地 T1–T4（§1 清单）→ §5 V1–V6 / V9 全绿（V7 结构门待补记、V8 待 verifier）→ 本 summary 写就，交 landing-verifier 独立核验（Track B 步骤 5）。
- 待办（verifier 绿后）：retro（步骤 6）→ 前置升级闸（gov 分支机锚 + off-main 校验）→ 自动合并本地 main（步骤 7；remote push 留 human）→ ARQ-260906-index-rebuild-dev-status-drift 结项簿记。
