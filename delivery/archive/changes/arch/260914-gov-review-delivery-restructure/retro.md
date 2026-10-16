# retro · 260914-gov-review-delivery-restructure（Track B 全环：四顶层收拢 delivery/ 结构重组）

> 主线程第一人称热记忆 · 2026-09-15 · harness 形态：判官 / 核验官 = 零上下文 coder 子 agent 降级形态（architect / landing-verifier agent）· 跨乘客对照：全仓级路径迁移首例（无先例可对，规模为 Track B 历来之最——142 锚、649 件 commit）。

**【硬项 R2 · 计量诚实】** 主线程无 token 计量（§2 估计声明）；子 agent 计量取自 task 通知实测值。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 | PROPOSAL v0.1→v0.3（含 human 三 OQ + R1 扩围裁定） | 2026-09-14 | 2026-09-14 | ~2h（估） | 无计量 |
| 评审 R1 | RED（BLOCK R1 + 必改 R2–R4 + ADVISORY R5–R8） | 09-14 | 09-14 | ~16min | 596 |
| 评审 R2 | 轻 RED（R10 分段字面量） | 09-14 | 09-14 | ~9min | 452 |
| 评审 R3 | 轻 RED（R15 形态家族封口） | 09-14 | 09-14 | ~4min | 314 |
| 评审 R4 | 终审 GREEN（R17） | 09-14 | 09-14 | ~1min | 232 |
| FROZEN | v1.0 → v1.1（收口即迁演进注记，human 讨论） | 09-14 | 09-14 | ~30min（估） | 无计量 |
| landing | T1–T6 全落 + V1–V6（human 通知 main sync 后启动） | 09-15 | 09-15 | ~2.5h（估） | 无计量 |
| 核验 | 首轮 FAIL（红项 4+1 退回）→ 红项修复 commit 1857dd3 → 复核轮 PASS | 09-15 | 09-15 | ~12min + ~5min | 406 + 376 |
| **合计** | | | | ~6h（估 ±1h） | 判官合计 1594 + 预审 2009 |

对照：前乘客（260913-spec-split 等）判官多为单轮或两轮；本环四轮全因「路径表示形态空间」一物——详见 §3-F2。

## 2. Token 账

计量方式诚实声明：主线程（起草 + 落地）无计量工具读数——按会话长度与工具轮次粗估 600k–900k tokens（误差界 ±30%）；判官与预审官为 task 通知实测。核验官 tokens 待其完成回填。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（起草 + 落地 + 手工修复） | ~750k（估 ±30%） | ~99% | landing 期批量替换三损伤类的诊断与修复占大头 |
| 判官（R1–R4） | 1,594 | <1% | 四轮均高效，单轮聚焦 |
| architect-reviewer（T6 契约预审） | 2,009 | <1% | 一次到位 + 顺手核出行 4 陈旧指针 |

花费最大处：landing 期损伤诊断（孤儿反斜杠 / 双应用 / 转义漏网三类，每类都要「全树搜损伤面→逐行读→定点修」）。可压缩分析：若起草期就把「路径表示形态族谱」列为硬项（见 §4-2），三类损伤可全部预防。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · PROPOSAL 锚清单行起草期遗漏、四轮评审未捕获、landing-fidelity gate fail-loud 才炸**：现象 = 结构门一次吐 6 条「缺锚清单：T1…T6 无『目标文件：』行」→ 根因 = 模板硬项 2a 在起草与四轮评审中均被人眼跳过，判官的 manifest≡census comm 对账以 census 为基准、恰好绕过了 `parseSection2AnchorLists` 这条独立机检 → 证据锚：`landing-fidelity-check.mjs` 首跑 errors 六条（2026-09-15 实测输出）。
- **F2 · 批量替换的路径形态空间四连击**：现象 = ①相对链接形 `../../changes/` 漏网（r4 卫兵字符类排除 `/`，而 V1 判据 `[^a-z]` 包含 `/`——实现与判据不一致）；②第二遍补刀引入 1 处双应用（压缩形 `.claude/taskpackages/changes/dev` 被两轮叠加）；③源码正则转义形 `\.planning` 被斜杠规则命中留孤儿反斜杠（3 文件 6 处，changes-index SyntaxError 暴露）+ `changes\/` 转义形两轮均不可见（4 处静默漏网）；④**成员访问粘连形**——`taskPackage.deliveryMilestoneStateChanged` 等点号属性访问里的 `.delivery`/`.planning` 被点前缀规则误吃（3 脚本 + 1 C 件 18 处，landing-verifier 首轮 FAIL 才接住）→ 根因 = 「路径」在仓内有**八种**文本表示（斜杠、分段单引号、分段双引号、Python join、C 实参、regex 转义、相对链接、点号成员访问），起草期只枚举了前五类 → 证据锚：`landing-verdict.md` 首轮 FAIL 红项、`closeout-check.mjs:317/332-333`、`taskpackage-required-check.mjs:198-243`、`arch-review-handoff-check.mjs:117-118`、`device_ws_switch.acceptance.c:864`（修复前实测）。
- **F2b · 在途包 canonical 活面误判**：现象 = required-check 冒烟报 canonical↔current 哈希失配 → 根因 = census 起草期把 `.claude/taskpackages/` 整树按历史面排除，未识别「在途包 canonical = 活面」（current/ 批改了、canonical 没改）→ 证据锚：pre-landing `cmp` 实测两者字节一致、失配 100% landing 造成；修复 = current 回写 canonical（landing-summary §3-12）。
- **F3 · 机批按文件粒度 vs 行级豁免按行粒度**：现象 = 批量替换吃掉 requirements 3 行 / ADR 4 行 / CDQ 4 行 / ARQ §2 32 行的 append-only 历史行 → 根因 = 豁免语义（「历史行不动」）无法进替换规则，只能事后定点回退；回退脚本首版又被 CRLF 错位全数跳过（diff 无 `\r`、工作树带）→ 证据锚：`landing-revert-history.mjs` 首跑「内容错位，手工处理」×43 → CRLF 修正后 32+12 行精确恢复。
- **F4 · 权限分类器误拦授权编辑**：现象 = repo-structure.md 三行新增被 auto 分类器拒，理由文本自认「landing 已授权、不构成阻断」却仍拒绝 → 根因 = 分类器对「大规模 tracked 文件重写」的风险模型与治理授权状态不通气 → 证据锚：Edit 工具 denial 消息（2026-09-15）；human 手动放行后同一编辑成功。
- **F5 · V1 子检 1 冻结命令口径瑕疵**：现象 = 按冻结文本跑得 24 命中、逐行核对全为新前缀被自家模式误咬 → 根因 = 子检命令照抄主命令正向模式但漏了第二段 `delivery/` 过滤，且冻结前未实跑 → 证据锚：PROPOSAL §5 V1 子检 1 原文 vs landing-summary §2 补滤版。
- **near-miss（最险一刻）**：正则转义损伤（F2-③）恰好落在 `changes-index.mjs` 的 import 链上、启动即 SyntaxError——**如果它只落在 `verify.mjs` 钩子或不被任何验证集启动的文件里，就是 dev-loop 运行期静默错路径**（子检 2 兜底能抓裸字面量，但孤儿反斜杠形 `\delivery` 不在任何冻结子检的模式里——是我在诊断时全树 `grep -F '\delivery'` 顺手清的）。没出事靠的是运气不是设计；§4-3 是补设计。

## 4. 优化建议（按 ROI 排序）

1. **PROPOSAL 起草期 / FROZEN 前干跑一次结构门**：`landing-fidelity-check.mjs <draft-dir>` 在 DRAFT 阶段就能跑（锚清单缺失、manifest 不一致当场炸）→ 治 F1 · 载体 = 固化候选 → ARQ 收件箱（建议名：draft-time-gate-dry-run）· 预期收益 = 结构硬项从「landing 期返工」前移到「起草期即时修」。
2. **路径引用形态族谱清单固化**：斜杠 / 分段单引号 / 分段双引号（任意段值）/ Python join / C 实参 / regex 转义（`\.planning` 与 `changes\/` 两子形）/ 相对链接 / **点号成员访问粘连形**——八类 + 各自的「残留清扫子检模式」配对成表，进 `.claude/references/`（供下次任何路径迁移 PROPOSAL 直接引用）→ 治 F2（含核验官接住的 ④）· 载体 = 固化候选 → ARQ 收件箱（建议名：path-form-family-reference）· 预期收益 = 本环四轮评审 + landing 四损伤类的全部诊断成本一次性预防。
3. **验证集命令冻结前实跑**：V1–V6 每条命令在 PROPOSAL FROZEN 前实跑一次（环境红先立基线本就是规约，子检 1 这种「能跑但断言错」的瑕疵只有实跑能抓）→ 治 F5 · 载体 = 固化候选（并入建议 1 的 dry-run 规约）· 预期收益 = 冻结验证集的「可判性」从纸面变实测。
4. **append-only 行级豁免的机批前置过滤**：豁免件（requirements/ADR/ARQ/CDQ/ledger）在批量替换前先从文件清单剔除、改完活行后再整体 diff 复核——本环是「先吃掉再回退」，多花了 43 行回退 + CRLF 两轮 → 治 F3 · 载体 = 留批次（下次同类操作自律，见 landing-summary §3 自律件）· 预期收益 = 回退工序消失。
5. **权限分类器与治理授权的通气**：误拦已授权 landing 编辑，理由文本自相矛盾 → 治 F4 · 载体 = 登记不硬治（分类器行为不在仓内治理面，human 侧知悉即可）· 预期收益 = 无（记录备查）。

## 5. 一句总结

成本 ~750k tokens / ~7h（估）· human 触点 7 次（三 OQ + R1 扩围 + T6 扩围 + repo-structure 放行 + 红项修复放行）· 制品 4 件齐（PROPOSAL FROZEN v1.1 + landing-summary〔含红项修复轮 §3-11/12〕+ verdict + 本 retro）· 对照前乘客：判官轮数最多（4 轮）但单轮最廉（合计 1,594 tokens），RED 全部有货 · 最险一刻 = 正则转义损伤靠 changes-index 启动崩溃才暴露（否则运行期静默）· **保险丝判定：两方独立结构（判官独立 + verifier 独立）接住作者偏差——且真实开火过一次**：核验官首轮 FAIL 接住了全部机检（四轮评审 + V1–V6 + node --check）漏掉的第 8 形损伤（成员访问粘连 18 处）与 canonical 对齐缺口，红项修复后复核转绿（verdict 件留痕）· 最值得带走的一条：**「路径迁移的难度不在路径，在路径的八种文本表示」——形态族谱表值得固化**（§4-2）。

## 6. 本环遗留

- **升级闸 BLOCK → human 批放行（2026-09-15）**：前置升级闸命中唯一例外项「docs/contracts/** diff 且同批无契约门证据」（`hasContractGateEvidence` 硬编码 false、向严设计，人批豁免条款明文不豁免契约面）→ 升级 human；human 于本对话裁定「批准」合并（依据 = T6 三行零语义 + 两次预签 + 四分类预审 + 复核 PASS）。merge 执行见 `delivery/changes/INDEX.md` mergedInMain 列；remote push 归 human。
- 留根：stale 指针（requirements/ADR/ARQ/CDQ 历史行 + INDEX origin 数据列 46 行）——读档按当时语境理解，实质修订时随批刷新（PROPOSAL §6）。
- 操作项：主索引 / C-AI-KidPal-gov 图随 watcher 自刷新吃新 `.cbmignore`（`delivery/` 排除生效）；合并 main 后主 checkout 同理。
- 登记不硬治：权限分类器误拦（§4-5）。
- ADVISORY 处置：判官 R5–R8 / R11–R13 全采；R14 comm 对账口径差异（10 件无命中正当 vs 实际 3 件真零命中 + 4 目录根 + CDQ）由 landing 期结构修正吸收（landing-summary §3-10）。
