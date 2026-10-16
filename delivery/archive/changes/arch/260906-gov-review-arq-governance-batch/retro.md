# retro · 260906-gov-review-arq-governance-batch（Track B 全环：十项 governance ARQ 一批清结 + 包 2 launch 面病灶拔除）

> 主线程第一人称热记忆 · 2026-09-06 · 乘客 #8（治理档）· harness 形态：判官 / 核验官 = 本会话派生的零上下文子 agent（architect / landing-verifier），主线程起草 + 落地 + 编排 · 跨乘客对照：本批是 260905（乘客 #7，单闸对齐 5 文件）之后的「批量清结批」——首吃一单十项、19 文件的量级，也是首个踩出 landing-fidelity 机械门能力边界的批。

**【硬项 R1】** 热记忆即写：verdict 绿后同场续写，未隔场（本批跨两个会话窗口，第二窗口从 landing-verifier 绿后收尾段续跑，FROZEN 前相位均为第一窗口热态）。**【硬项 R2】** §2 计量诚实声明。**【硬项 R3】** §3 每条 F 带现象→根因→证据锚。**【硬项 R4】** §4 每条建议标载体。**【硬项 R5】** §5 保险丝判定。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 分析 + 起草 | ARQ 十项分拣 + 问题类四归并 + OQ 四题 + PROPOSAL v0.1（human「立项 review吧」ack） | 09-06（窗口一） | 09-06 | ~35 min | ~60k（估） |
| 评审 R1 | architect → NEEDS-REVISION（F1/F2/F3/F4 BLOCK + F9 NIT 等） | 09-06 | 09-06 | ~12 min | ~40k（估，判官未留实测表） |
| 修订 + R2 | 问题类归并修正 / T1b 折入 / OQ3=(b) 落法修订 → R2 GREEN（F13/F14/F15 NIT 在册） | 09-06 | 09-06 | ~15 min | ~35k（估） |
| FROZEN | 状态行 + INDEX 钉行（commit 9e71bed） | 09-06 | 09-06 | ~2 min | ~4k（估） |
| landing | T1–T9 落地 19 文件（changeid-shared 新增 + 8 消费方 / coverage 自检 / DEV-GOVERNANCE 两节 / bootstrap 提醒 / merge-driver T5+T6 / .gitattributes / agent def 引用 / TP-A 复核注）+ §5 V1–V13 验证 + 格式修复插曲（commit 0843466） | 09-06 | 09-06 | ~70 min | ~120k（估） |
| 核验 | landing-verifier 独立核验 → GREEN（perFile 19/19） | 09-06（窗口一末） | 09-06 | ~15 min | ~35k（估） |
| 收尾 | T10 十项移 §2 + changes-index upsert（11237b7）+ 门缺口 ARQ 登记（7917681）+ 本 retro | 09-06（窗口二） | 09-06 | ~15 min | ~15k（估） |
| **合计** | | | | **~2 h 44 min** | **~309k ±40%** |

对照：260905 批 ~55 min / ~176k（乘客 #7）。本批约为前批的 3 倍时长 / 1.8 倍 tokens——量级差异来自「一单十项 + 19 文件 + 五个独立验证插曲」，不是节奏劣化；单文件落地成本与前批持平。

## 2. Token 账

计量方式：本批判官 / 核验官子 agent 未留逐轮实测 token 表（前批 260905 留了，本批未继承该习惯——见 F4），主线程全部估计 ±40%（跨度含跨会话窗口、无 harness 计量锚）。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（起草 + 修订 + FROZEN + landing + 收尾） | ~234k（估） | ~76% | landing 段最重（T5/T6 代码 + 五插曲诊断） |
| 判官（R1 + R2） | ~40k（估） | ~13% | R1 实质审（四 BLOCK），R2 轻复核 |
| 核验官（landing-verifier） | ~35k（估） | ~11% | 19 文件逐个人读 diff |

花费最大处：landing 段的「验证 + 诊断」——T6 dry-run 实证、T7 eol 机制诊断、结构门三轮返修占主线程估计值的一半以上。可压缩分析：格式修复插曲（F1）纯因起草形态与机械门契约错位，若 PROPOSAL 模板钉死 `### T<n>` 标题形态可整体省掉。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · FROZEN 后格式修复（最大纯耗）**：现象 = 结构门报「§2 无任何 T 条目」+ 全量 claimed-not-landed → 根因 = PROPOSAL 用 `**T<n> · …**` 粗体段形态写改法标题，而 `parseSection2AnchorLists` 只认 `### T<n>`；manifest 行带括注（`.gitattributes（新增，T7）`）而 `extractManifestBlock` 逐字取行 → 证据锚 = `.claude/scripts/lib/governance-gate-shared.mjs` 锚解析段 + landing 实测输出（verifier 规范化比对确认语义零变更）。教训：drafting 缺陷在 landing 相位才炸，修复落在 FROZEN 后——幸而修复纯格式、verifier 以规范化文本比对证成零语义漂移。
- **F2 · 机械门能力边界首穿（T7/T9 不可机锚）**：现象 = 修完格式后门仍报锚集 ≢ manifest（仅 manifest 有 `.gitattributes` / `changes/dev/.../review-packet.md`）→ 根因 = `CANONICAL_TEST` 只认 `.claude/` / `docs/` / `.planning/` / 仓根三件套，不识仓根点文件与 `changes/**` 形态（本批两落点类首见）→ 证据锚 = 门 error 输出原文 + 260905 批宽免判例。处置 = 整裁腿 + 原因声明登记 + 人读 diff 担纲 + 缺口如实登记新 ARQ（7917681）。
- **F3 · T7 eol 机制黑箱**：现象 = renormalize 后 `git checkout -- '*.sh'` 空转、五件仍 CRLF → 根因 = git 判定 renormalize 后文件 clean（规范化比较相等）即跳过重写，checkout / checkout-index 均 no-op → 证据锚 = 本机实测（checkout 前后字节比对）+ `rm + git checkout -- <file>` 逐件强制重写生效。教训：`.gitattributes` 语义改了「以后怎么 checkout」，不改「已经 checkout 过的字节」；rm 强制重写是到达钉死终端状态的可靠路径。
- **F4 · 判官 / 核验官 token 表未留**：现象 = 本 retro §2 只能给 ±40% 估计 → 根因 = 本批未继承 260905 批「判官逐轮实测表」的记录习惯，子 agent prompt 未要求汇报 token → 证据锚 = 对比 260905 retro §1（52,612 实测）与本批空白。
- **near-miss（最险一刻）**：T9 的 `review-packet.md` diff 出现 `-1`——若那 1 行是内容改动而非「末行无换行符因追加而重新发射」，就是破坏 append-only 纪律的历史记录类文件改写。没出事的原因：核对 diff 时逐字确认删除行 = 纯 trailing-newline 重发射、既有行零改动（V12 实证）。教训：向历史记录类文件追加时，diff 的 -1 必须逐字验尸，不得默认安全。

## 4. 优化建议（按 ROI 排序）

1. **PROPOSAL 模板钉死改法标题 `### T<n>` 形态 + manifest 裸路径形态**：治 F1 · 载体 = 固化候选 → 改 `docs/templates/PROPOSAL.template.md`（登记 ARQ-260906-lfc-canonical-anchor-gap 时一并提及，或下批顺手） · 预期收益 = 消灭 landing 相位格式返修整类（本批约 15 min + 一轮 verifier 额外负担）。
2. **landing-fidelity 闸拓宽 CANONICAL_TEST / TARGET_PATTERNS（认仓根点文件 + `changes/**`）或增白名单参数**：治 F2 · 载体 = 已登记 ARQ-260906-lfc-canonical-anchor-gap（§1.2，下批治理批处理） · 预期收益 = 机锚回真、宽免类 error 清零、人读 diff 负担回落。
3. **子 agent（判官 / 核验官）prompt 统一要求汇报实测 tokens / tool_uses**：治 F4 · 载体 = 固化候选 → ARQ 收件箱（登记时并入建议 2 的行或单独一行） · 预期收益 = retro §2 回到 ±5% 精度，成本对照可续。
4. **eol 归一操作写进 DEV-GOVERNANCE 或 notes**：治 F3 · 载体 = 登记不硬治（`rm + checkout` 是本批一次性操作，`.gitattributes` 已防复发；再遇同类只剩仓外克隆场景） · 预期收益 = 低，备查即可。

## 5. 一句总结

~2 h 44 min / ~309k ±40%（估）/ human 触点 2 次（立项 ack + OQ 四裁）/ 制品 4 件齐（PROPOSAL + landing-summary + landing-verdict + retro）· 对照 260905 批量级 3 倍、节奏持平 · 最险一刻 = T9 diff 的 -1 trailing-newline 验尸（near-miss F3 节）· **保险丝判定：两方独立结构接住了作者偏差**——判官 R1 打掉四个 BLOCK（问题类归并错 / T1b 挂载 / OQ3 落法）防带病冻结，核验官 19 文件逐个人读 diff + 归账 21 条 manifest-外 porcelain 零流浪改动，F1/F2 作者侧偏差全部被独立方拦截或证成 · 最值得带走的一条 = 机械门的契约（标题形态 / 锚正则）是 drafting 相位的输入约束，不是 landing 相位的惊喜。

## 6. 本环遗留（批次型变更特有）

- **交日项核对（PROPOSAL §6 钉死）**：「包 2（260829-voice-roundtrip）launch 是否确实未踩 T6/T7/T8 三病灶」——2026-09-06 实测包 2 status = `queued`、worktreePath = null，**尚未 launch**。去向 = 包 2 launch / 合并时核对（尤其 T6 合并通道自动 skip-worktree 首个真实拓扑实证、T7 host 验收 WSL 跑 `.sh`、T8 验收起草引用 DEV-GOVERNANCE 两条纪律）；若踩，复盘病灶为何未在本批前暴露。挂起处 = 包 2 retro 或 launch checklist，本行不销。
- **门缺口 ARQ**：ARQ-260906-lfc-canonical-anchor-gap 已登记 §1.2（7917681），下批治理批根治，去向明。
- **V5 advisory 枚举**（closeout-check 两条，与本批无因果、如实登记不代处理）：ws-switch 包 machine evidence missing；loop-retro 阈值达（主线程应发起 /loop-retro）。
- **V8 拓扑留根**：主 checkout 两文件 skip-worktree 标记系验证末段手工恢复（当前 transient 拓扑 main 无持有者）；T6 在真实合并拓扑下自动覆盖主 checkout，包 2 合并实证——与交日项同钩。
- **自律件**：包 2 acceptance-author 起草时引用 DEV-GOVERNANCE「## 验收测试纪律」三条；包 2 launch 前 human 自跑 launch 命令（既有交付形态，不改）。
