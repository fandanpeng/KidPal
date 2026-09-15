# retro · 260906-gov-review-index-status-truth-source（Track B 全环：INDEX 状态投影双病灶拔除）

> 主线程第一人称热记忆 · 2026-09-06 · 乘客 #本批首乘（INDEX 投影面）· 判官 / 核验官 = 零上下文 coder 子 agent 形态 · 跨乘客对照：与 260906-arq 批（同日早班车）共享「judge 抓作者盲区」保险丝模式。

**【硬项 R1 · 环后趁热、第一人称】** verdict 绿后即写（landing-verifier GREEN 2026-09-06），第一人称热记忆。

**【硬项 R2 · 计量诚实】** 判官 / 核验官 tokens 有 harness 计量；主线程无精确计量，§2 诚实声明估计方式与误差界。

**【硬项 R3 · 摩擦带根因 + 证据锚】** §3 每条带现象 → 根因 → 证据锚。

**【硬项 R4 · 建议必标载体】** §4 每条建议标载体。

**【硬项 R5 · 保险丝判定】** §5 含两方独立结构接住判定。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 | PROPOSAL v0.1 → v0.2（含 §1 五件证据链搭建） | 2026-09-06 上午 | 同日 | ~1.5h（估） | 见 §2 |
| 评审 R1 | 判官 NEEDS-REVISION（N-1 缺锚必红 / N-2 翻转集 2→3 / N-3 删常量 ReferenceError + NIT×4） | 同日 | 1605s | 1605s | 93,134 |
| 修订 | v0.2 → v0.3（N-1~N-3 全修 + 锚行机检对齐自跑） | 同日 | 352s+ | ~30min（估） | 主线程 |
| 评审 R2 | 判官 GREEN（R2-NIT-1~4） | 同日 | 352s | 352s | 114,742 |
| FROZEN | 凭据齐（ack + OQ 三裁 + R2 GREEN）标 FROZEN v1.0 | 同日 | — | — | — |
| landing | T1–T4 packet-bound 落地 + 验证集重放 + summary | 同日 | ~1h（估） | ~1h（估） | 主线程 |
| 核验 | landing-verifier GREEN（33 tools / 679s） | 同日 | 679s | 679s | 57,174 |
| **合计** | | | | ~4h（估） | ~265k + 主线程 |

对照：260906-arq 批（同日）判官两轮 + verifier 形态相同；本批轮次更少（2 轮评审 vs arq 亦 2 轮），但主线程证据链搭建（§1 五件）占时显著——病灶溯源批的固有成本。

## 2. Token 账

计量方式诚实声明：判官 / 核验官 tokens 为 harness 子 agent 计量（精确）；主线程（起草 + 修订 + 落地 + 本 retro）无 harness 分项计量，按上下文吞吐经验估 ±40%。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（起草 + 修订 + 落地） | ~400k（估 ±40%） | ~60% | 含跨压缩点续作（context 满载后接续） |
| 判官（R1 + R2） | 207,876 | ~31% | R1 重（35 tools 实证 census）/ R2 轻（4 tools） |
| 核验官 | 57,174 | ~9% | 33 tools 含破坏性 mutation 自检 |

花费最大处：主线程 §1 证据链（8 包 census / deriveLanded 实况推演 / git 考古 f0aaa87·3183d93 / sv 值判否五证）——双病灶归因必须钉到行级才能开对方案，省不掉。可压缩分析：R1 的 N-2（翻转集 2→3）若起草期先跑一遍 deriveLanded 实况断言再写数字，可省一轮往返——但 draft 期「先跑再写」本就是 F4 真空绿教训的要求，此处属执行回潮（见 F1）。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 起草期数字未先跑，R1 被抓**：现象 = PROPOSAL v0.2 断言「结构门必绿」且翻转集写 2 批，判官独立复跑发现 §2 无锚行必红 + 实际翻转 3 批（漏 260905）→ 根因 = 起草时凭记忆写验证断言与计数，未在落笔前跑机械闸与实况推演（F4 真空绿教训的回潮）→ 证据锚 = R1 N-1/N-2（PROPOSAL §7）；主线程修订时亲跑 `parseSection2AnchorLists` 实测 union=[] ≢ manifest（§7 裁断①引用）。
- **F2 · 双写者漂移病灶的「各局部合理」性**：现象 = closeout-check（event 真相派）与 changes-index R5（文件真相派）各自局部合理，合流才病灶 → 根因 = 两个治理批（R5 2026-07-24 / closeout-hardening 2026-07-27）在无人看守的接缝处矛盾，INDEX 只是投影受害者 → 证据锚 = closeout-check.mjs:157（旧）vs changes-index.mjs collectDev R5 块（旧）；病灶包 260903-device-ws-switch INDEX 翻 open 实录。
- **F3 · 落地期重复导出语法错**：现象 = T4 首版把 deriveDevClosed 两处 export，`node --check` 即报 Duplicate export → 根因 = 函数声明处已带 export，文件尾列表又加 → 证据锚 = node --check 报错（summary §3 注记 1）；修正零语义漂移，self-check 29/29 复证。
- **near-miss（最险一刻）**：verbatim-quote 观察列 2 条若被当作真实失配打回，将浪费一整轮核验往返。没出的原因 = 主线程在跑完结构门后主动做字节级对账，止于行尾 `\r\n`/`\n` 差异即判定环境性缺口而非实质漂移，并按 arq 批「登记缺口 + 人读 diff 担纲」先例登记为诚实注记 4——verifier 复核注记属实时独立 byte-compare（CRLF 归一后逐字中）背书。教训：advisory 观察列必须逐条定性（真失配 / 机械局限），不能搁置。

## 4. 优化建议（按 ROI 排序）

1. **landing-fidelity 闸 CRLF 归一化 + citation 裸文件名解析**：治 F-near-miss · 载体 = 登记 ARQ 收件箱（与 PROPOSAL §6 留根同批归口；闸脚本改动超本批 frozen 范围）· 预期收益 = verbatim-quote / citationAnchor 两类 advisory 在 Windows 工作树清零，省每批人工定性。
2. **PROPOSAL 起草 checklist 机械前置**：「§2 每 T 锚行在场 + 数字断言先跑再写」两条并入起草相位自检（changeid-prefix 式 `--self-check` 或 landing-fidelity 的 dry 模式）· 治 F1 · 载体 = ARQ 收件箱 · 预期收益 = 消 R1 N-1/N-2 类往返（本批 207k 判官 tokens 中约三分之一耗于此）。
3. **CHANGELOG_ID_CUTOFF 残余 3 副本机械替换小批**：taskpackage-required-check.mjs:95 / devloop-artifact-check.mjs:70 / hooks/supervisor-gate-check.mjs:58 · 载体 = 留批次（PROPOSAL §6 已钉，hooks 属另一冻结面须单独批）· 预期收益 = 谓词真单源。
4. **260830 / 260904 两批归 LANDED_ADJUDICATION 人工钉**：载体 = 留批次（PROPOSAL §6 已钉，人工判定动作）· 预期收益 = INDEX 两 frozen 行纠 landed，机械锚外散文/行内 token 病灶封口。

## 5. 一句总结

~665k tokens（估）/ 4h（估）· human 触点 3 次（立项 ack + OQ 三裁 + 中途 OQ1 retro 判据讨论采主推荐）· 制品 4 件齐 · 对照 arq 批同形态同保险丝模式 · 最险一刻 = CRLF advisory 定性（见 F near-miss）· **保险丝判定：两方独立结构接住作者偏差**——R1 抓 N-1 缺锚必红 / N-2 计数错 / N-3 ReferenceError 三处作者盲区（起草期均未自见），verifier 独立 byte-compare 背书 CRLF 定性并证mutation 自检非真空绿；保险丝两次均熔断在正确位置 · 最值得带走的一条 = 「双写者各局部合理 ≠ 合流正确」——谓词/清单类语义凡有两处以上持份者，单源化要连判定逻辑一起收，光收清单不收判定就是下一个接缝。

## 6. 本环遗留（条件节）

- CHANGE_ID_CUTOFF 残余 3 副本（taskpackage-required-check.mjs:95 / devloop-artifact-check.mjs:70 / hooks/supervisor-gate-check.mjs:58）→ 机械替换小批（建议 3）。
- 260830-arch-review-ui-impl-strategy / 260904-gov-review-escalate-merge-approval 两批散文式 / 行内 token 病灶 → LANDED_ADJUDICATION 人工钉（建议 4）。
- landing-fidelity 闸 CRLF / citation 两缺口 → ARQ 收件箱（建议 1，与 summary 注记 4/5 同登记）。
- 本包 landing 后自身状态 = landed + mergedInMain（簿记动作：INDEX upsert + ARQ-260906-index-rebuild-dev-status-drift 结项行，随合并后收尾段执行）。
