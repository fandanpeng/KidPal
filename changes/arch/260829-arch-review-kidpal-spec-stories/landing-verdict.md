# Landing Verdict · 260829-arch-review-kidpal-spec-stories

> landing-verifier 独立保真核验。核验对象：主线程 packet-bound landing（工作树未 commit 改动）。
> 对照基准：frozen `PROPOSAL.md`（§2 改法 + landing-manifest + §5 验证集）+ `CONTRACT-CHANGE-PACKET.md` + `architecture-gate-evidence.md`。
> **降级声明（证据事实，非放行理由）**：本仓结构门脚本 `landing-fidelity-check.mjs` 未迁移，「结构门」降级为逐文件人读 diff 核对（`git status`/`git diff` 实证）；spec↔contract 机械门亦无脚本兜底（ARQ-260827-contract-mech-gate-gap），V2/V5 人读核对。核验仍基于真实 diff 实证，不采信 landing-summary 自述。

---

## 总结论：绿（落地 == frozen 清单，建议放行；merge 仍归 human）

> verdict: PASS

逐项：A PASS · B PASS · C PASS · D PASS · E PASS（计数 5 PASS / 0 FAIL；含 1 观察项，见 B）。

---

## A. landing-manifest 完备性 —— PASS

landing-manifest 声明三文件：`docs/systems/kidpal/spec.md`、`docs/contracts/interface-contract.md`、`docs/systems/kidpal/ui/kidpal-ui-design-v2.html`。

实测 `git status --short` 改动集（剔除任务书声明的非本批：`.claude/scope-map.json`、`.gitignore`、`changes/INDEX.md`、`firmware/`、`docs/CONTRACT-DECISION-QUEUE.md`、本包 `changes/arch/260829-*` 制品）：

| 本批仓内文件 | 在 manifest | diff 实证 |
|---|---|---|
| `docs/systems/kidpal/spec.md` | ✓ | `git diff` 命中（§4 整节 + §5.3/5.4/5.6），hunk 定位 247/456/469/489 |
| `docs/contracts/interface-contract.md` | ✓ | `git diff` 命中，+4 行全在 §1.3 ③ story_list |
| `docs/systems/kidpal/ui/kidpal-ui-design-v2.html` | ✓ | `git diff` 命中，20 行注释/figcaption/手势表文案 |

- 漏改：无（manifest 三文件全部有真实 diff 对应）。
- 多改：无（本批仓内改动恰为 manifest 三文件；其余改动均属任务书明确排除的更早框架批次或本包制品）。

## B. spec §4 == PROPOSAL §2-T1 —— PASS

- **13 条 story 齐备**：§4.1 课表 5 条（S-F1.1 常显首页 / S-F1.2 课表页 / S-F1.3 语音问课表 / S-F1.4 到点提醒 / S-F1.5 家长录入，spec.md:288-336）＋ §4.2 故事 5 条（S-F2.1 列表 / S-F2.2 录音 / S-F2.3 回看回听 / S-F2.4 盖章 / S-F2.5 润色，spec.md:338-378）＋ §4.3 查字 3 条（S-F3.1 查字看笔顺 / S-F3.2 拼音笔画部首 / S-F3.3 演示控制，spec.md:382-408）。每条带「作为…我希望…以便…」+ UI 锚点 + 端云 `[板]/[云]/[板+云]` + Given/When/Then。
- **§4.0 共享底座在**：语音会话生命周期 7 状态（聆听中/思考中/播报中/语音喊停/提醒弹窗/低电打盹/页面指示，spec.md:260-268）＋ 双层按键手势表（全局行为 + 页内特例，spec.md:270-279）。PROPOSAL 表述「7 状态+双层手势表」与落地一致。
- **已砍功能已删且有声明**：
  - 砍「当前课反白高亮」→ spec.md:303 验收条款「不做『当前课反白高亮』…不承担『现在进行』指示」，并在 §4 顶部已砍声明（spec.md:256）统一收口。
  - 砍「生字自动反白高亮」→ spec.md:364 验收条款「不做『生字自动反白高亮』…问生字走语音」，同样在 :256 声明。
  - 残留扫描：`grep 当前课/生字高亮/反白` 全部命中均为「不做/已砍/废弃」语境或选中项反白（非砍项），无残留的功能性承诺。
- **human 三裁定两侧落地**：
  - B1（KEY 长按 = 回首页）：spec §4.0 手势表 `KEY 长按 | 回首页`（spec.md 手势表）＋ spec.md:280 说明；设计稿侧 gesture-table `KEY 长按` 行改「回首页」（html diff，`原为「预留/P4查下一个字」…B1 裁定改为回首页`）＋ P4 底部「回首页 KEY长按」（html diff）。
  - M2（P3 KEY 双击进阅读）：spec §4.0 手势表 `KEY 双击 | P3 列表=进入阅读/播放选中` ＋ S-F2.1/S-F2.3 验收（spec.md:344,360）；设计稿侧新增 `KEY 双击` 行「P3 列表进入阅读/播放选中」＋ P3 底部「读 KEY双击」（html diff，原「听 BOOT」）。
  - m5（设计稿废弃标注）：spec.md:256 声明指向设计稿废弃标注；设计稿侧 P2 当前课反白 ×3 处、P3 生字反白 ×2 处加 `【已废弃·2026-08-29】` 注释/figcaption（html diff 614/793/812/834/916/927 行段）。

**观察项（不判红）**：S-F1.2 叙述句「看到今天的课排在哪几节、现在在上哪节」（spec.md:298）与其验收条款「不承担『现在进行』指示」（spec.md:303）存在措辞张力——叙述句字面承诺了"现在在上哪节"，而验收已砍该指示的呈现手段。落地方向与 frozen PROPOSAL 一致（PROPOSAL §2-T1 明确砍当前课高亮，落地确实砍了），属 story 文案与砍裁定未完全对齐的措辞瑕疵，非功能缺口、无 scope creep。建议 retro 顺手把 :298 叙述句的「现在在上哪节」删去，不影响本批放行。

## C. 契约 == PROPOSAL §2-T2 + 契约门批准 —— PASS

`git diff docs/contracts/interface-contract.md` 实证仅 +4 行，全部落在 §1.3 ③ story_list：

- **仅加 `total`（additive 可选）**：JSON 样例 `data` 内新增 `"total": 3`（interface-contract.md:84），`items[{id,title,date}]` 结构原样保留，无删/改名/类型变更。
- **无其他改动**：diff 中除 story_list 区外，char/schedule/story_text/text/home/control 六 view、shim API、display 顶层、笔顺 JSON、课表 MD、提醒三——零改动（与 CONTRACT-CHANGE-PACKET §1 out-of-scope 一致）。
- **字段说明两句齐备**（interface-contract.md:94）：
  - 「缺省时板子回退用 `items.length`（向后兼容）」✓
  - 「不得以 `items.length` 顶替 `total` 作正式数据源」✓
  - 另含「P3 右上角故事果计数器（×N）与盖章帧『第 N 颗』（S-F2.1/S-F2.4）」，与 spec S-F2.1/S-F2.4 一致。
- **契约门批准**：`architecture-gate-evidence.md` §4 记 `HUMAN_APPROVED`（human DavidFan，2026-08-29），§10 门状态 CLOSED；变更记录行（interface-contract.md:95）标注 2026-08-29 经契约门批准。落地 == 批准范围（仅 `total` 一字段）。

## D. 破坏性自检 —— PASS

- **spec**：§4 为整节替换（PROPOSAL 授权）；§4 之外 diff 仅 3 个 hunk（§5.3 键位注 456 / §5.4 P2·P3·P4 条目 469 / §5.6 查字界面 489），均为 PROPOSAL §0 声明的「最小同步」，无既有章节丢失。原 F1.1-F3.4 粗粒度清单被 §4 story 取代是 T1 本意，非破坏。
- **契约**：纯新增 +4 行，零删行——无任何既有字段/章节被覆盖。
- **设计稿**：被「删」的两行（「现在上」徽章、「听 BOOT」）均在同一 `<span>` 结构内以「保留原结构 + 追加废弃/修订注释」回归，非净删除样式/结构块；无 CSS 规则、无组件、无 `data-od-id` 锚点被删（V1 复验 19 锚点全在）。
- **历史记录类 append**：契约变更记录为新增行（append 语境），未覆盖既有行；本核验不改任何文件。

## E. 验证集复核 —— PASS

| # | 结论 | 复验 |
|---|---|---|
| V1 | 属实 | spec §4 引用锚点 19 个（canvas-p1..p4 / comp-stamp / gesture-table / ic-penhand / p3-reader / p3-recording / p3-stamp-frame / pose-daily/jump/record/sleep / state-listening/reminder/sleepy/speaking/thinking）逐个 grep 设计稿，全部命中（19/19）。注：landing-summary 写「19/19」，PROPOSAL §1 起草期记「23/23」——两数差异源于口径（起草期含正文内联引用计次，落地核验按去重锚点实体计），两种口径下均全命中，无缺锚。 |
| V2 | 属实 | `git diff docs/contracts/interface-contract.md` 实证仅 story_list 一处（见 C）。 |
| V3 | 属实 | 本批仓内改动 = spec + 契约 + 设计稿（+ 本包制品 + INDEX），见 A。 |
| V6 | 属实 | `node -e JSON.parse` 复跑 story_list 含 total 样例，解析通过（total=3 合法）。 |

V4/V5 为人读项，已在 B/C 核对（13 story 结构齐备、契约一致、已砍功能两侧无残留）。

---

## 破坏性自检状态

已逐项核对：挑了敏感面（契约 story_list 全 diff、spec §4 整节替换区、设计稿手势表）逐字核，未只挑好看文件；契约 diff 小但已逐行确认无夹带；未发现借 landing 夹带权限/写域扩张（本批纯文档 + 一 additive 契约字段）。landing-summary 声称的每个改动均在真实 diff 中找到对应，无「声称落了但 diff 无」的虚报。

## packet-binding 核对

landing-summary 引的 packet-id = FROZEN PROPOSAL v0.2 + landing-manifest + 契约门证据，与本核验对照的 frozen PROPOSAL（同目录 `PROPOSAL.md`，Status: FROZEN v0.2）一致；landing 实际范围（三文件）⊆ manifest 声明范围，无 scope creep。

## 放行建议

**放行 merge**（merge 归 human manual-merge 门）。唯一观察项（S-F1.2 :298 叙述句措辞张力）建议进 retro 顺手修订，不阻塞本批。
