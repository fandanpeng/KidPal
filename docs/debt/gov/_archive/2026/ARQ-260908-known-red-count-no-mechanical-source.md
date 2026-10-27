---
id: ARQ-260908-known-red-count-no-mechanical-source
title: 「上游已知红」计数在 launch 面是**手写快照**、无机械来源，且 evidence 类扫描齿的 root 定位**吃调用 cwd**——同一 ELF 两份数字，红基线与 mutation 判读全靠人肉对口径
domain: gov
status: closed
severity: SHOULD
owner: architect（定规约）+
trigger: ① 已由 260909-gov-review-gate-truthfulness T5 承接（2026-09-09：launch prep 步骤 2 现跑命令绝对路径口径 + progress.md 落行 + knownRedFloor 改引用制品）；② C 侧 evidence 齿 root 注入**已由 `260920-gov-review-bearing-strength-align` T2 搭车件落地**（`acceptance-evidence.md` 新增「evidence 类扫描齿的 root 注入」节——显式接收仓根 / 禁依赖调用 cwd；landing-verdict `PASS + 3 ADVISORY`、commit `e3b19ee`，2026-09-20）；③ runner 报告头未做——**③ 留下次动 acceptance-evidence 约定的治理批**
opened: 2026-09-08
closed: 2026-10-27
closedBy: 261027-gov-review-p7-devloop-doc-cluster
paths: []
source: 260829-schedule-reminder（pod-events friction `12425ced` 根因查明 + `2e444628`/`27f2f32c` 双版本对证与编译器实证 + 260829-story-p3 同族收紧先例 `7ee73c9`，2026-09-08）
---

**证据**（260829-schedule-reminder，2026-09-08）：① 本包 launch prompt 写「main host ELF 已知红 **2** 条」，planner 亲跑在文档命令位置（`firmware/main/test/host_app/`，HOST_TEST_CONVENTIONS **[C4.4]** 规定位）实测 **3** 条；同一 ELF 从仓库根跑 = **2** 条。② 分歧根因已查明并排除「证据缺失说」：`voice_roundtrip_b.acceptance.c:878`（VRT-A2/E2 齿）所读的 `delivery/changes/dev/260829-voice-roundtrip/roundtrip-live-evidence.md` **自包 2 提交 `167d1a3` 起 blob 未变**（起点与 HEAD 同为 `b6a8701`），其内 5 组标记（`session_id` / 识别文本|>>|stt / 语音生成成功|发送第一段语音|tts / SentenceType|…|sentence_start / state-speaking|文字同步|上屏）经独立脚本**逐组核为全部命中**，`vrt_find_evidence` 的交替串确实按 `|` 拆分匹配 ⇒ 判红不是「live 未做」而是**该齿自身 root 定位在特定 cwd 下的判定问题**（包 2 面遗留，不在本包边界内修）。③ 同族（语料范围过宽 → 靠无关散文维持判定）的第二实例见 main 面 260829-story-p3：其 live 齿原以「整个 `delivery/changes/dev/<id>/` 目录全文 grep 标记」为判据，收口时被 tighten 收紧为「语料收窄 `live-evidence/` 子目录 + 要求结构化事实形状」（提交 `7ee73c9`，方向 = 改严）⇒ 该族齿的判定稳定性问题是复发的，不是本包孤例。**影响面**：① 每个 firmware/host 包 launch 都要重付一次「既有红到底几条」的人肉对账（本包付了两条 friction 事件 `2e444628`/`12425ced` 才钉死口径），且判读口径若错会直接把先行红误记成本包回归（本包 builder 即误报过一次「76T/2F、VRT-A2 已转绿」，planner 亲跑纠正为 76T/3F）；② 手写快照随 main 前进必然腐化，无门报警；③ evidence 齿的判定不稳定 = 「同一实现两次跑出不同红名单」，与 acceptance-evidence.md 的证据可重现要求相悖。**建议改动**（候选，交 review，不自批）：① 新增脚本产出**已知红快照**（如 `.claude/scripts/known-red-snapshot.mjs` 或 `--self-check-known-red`，固定 cwd、逐条列 `file:line + 齿名 + 判据缺失物`），launch 面（`--check-launch` / prompt 生成器）引用该快照而**不再手抄数字**；② evidence 类扫描齿（VRT-A2/E2、WSA-A4/A6、本包 live 齿 `_require()`）统一改为**显式接收仓根 / 由 runner 注入 root**，禁依赖调用 cwd（落 `.claude/references/acceptance-evidence.md` 撰写约定 + acceptance-author agent def 引用行）；③ host runner 侧同步把「执行目录」写进报告头（现值口径：本包一律取文档命令位置 = 3 条既有红）。**riskTier: low**（判读成本与误记风险，非行为缺陷）。**review profile**：Track B（G2/G3 混合：新增只读快照脚本 + 验收齿撰写约定成文。机检硬排除逐项：不触 contracts、无 spec-contract drift、不改 agent frontmatter tools/name/model、非 mixed lane、不触 L0；若 ② 落到「修既有已 merged 包的冻结齿 root 定位」则触**他人冻结件** → 须该包 owner（acceptance-author）+ human 逐次授权，本条只登记规约、不预批任何冻结件改动）。验证计划：快照脚本对当前 main 跑出 3 条既有红并逐项与人工核对一致；改 root 注入后 VRT-A2 齿在两种 cwd 下判定相同（现值：文档位置红、仓根绿 ⇒ 修后应恒绿或恒红且理由一致）。owner=architect（定规约）+ acceptance-author（重指既有 evidence 齿）。

**结项追注（2026-10-27，261027-gov-review-p7-devloop-doc-cluster T4）**：子项③ 已落地——acceptance-evidence.md root 注入节末追加「host runner 报告头记执行目录 + 现值口径单源」段（报告头写明执行目录、现值数字一律取文档命令位置、兑现方 = 跑测者手写约定级、机械化维持 260920 批留根④ 边界另走 dev-loop）。①② 前已消化（260909 批 T5 / 260920 批 T2），本批 ③ 落地即整条结项。R1 GREEN、落地五件 sha256 对账一致。
