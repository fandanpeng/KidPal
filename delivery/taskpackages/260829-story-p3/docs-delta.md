# docs-delta · 260829-story-p3

> integrator docs-sync 结果清单（docsOwner=integrator）。落**实现侧文档**与决策留档，
> 契约面 / spec 正式面零触碰（闸 A 判 ④ NO_CONTRACT_CHANGE）。
> 收口日期 2026-09-08，分支 `dev/260829-story-p3`。

## 1. `docs/` 正式面改动

| 文件 | 改动 | 作者 / 落点 |
|---|---|---|
| `docs/ADR.md` | **新增 ADR-001**「版本化部署制品的接线不变式：渲染面 == 声明面 == 被测面，且守卫必须版本无关」（append-only 首条，本包收口落） | integrator（本次 docs-sync） |
| `docs/ARCH-REVIEW-QUEUE.md` | **新增 2 条 ARQ**：`ARQ-260908-versioned-guard-version-literal-coupling`（Track A，字面版本耦合通用病灶）、`ARQ-260908-firmware-build-degraded-gate`（Track A，firmware-build 闸恒降级门根） | **planner 已登**（commit `f7c82d4` + `419bdee`）；integrator 只引用、**未复登、未改一字** |
| `docs/contracts/interface-contract.md` | **零改动** | 闸 A ④：六项预审（`start_recording` §1.4:132 / `story_list.total` §一:95（2026-08-29 契约门已批 additive）/ `story_text` §1.3 ④:98 / §四 manual 聆听 / `stories/*.md` 非契约面 / V31 四条）全数无需改；`docs/contracts/**` 本包只读消费，hook + settings 双拦 |
| `docs/spec.md` | **零改动**（判定 = 无需更新，理由见下 §2） | — |
| `docs/` 根其余文件 | 零改动；未新增 docs 顶层文件（不触 `scope-map.universal.docsTopLevel` 白名单边界） | — |
| `changes/arch/**` | 零改动（本包无治理 landing） | — |

### spec.md 无需更新的理由

`docs/spec.md#§4.2` 的 S-F2.1..S-F2.5 是以 Given/When/Then 写的**目标行为**（故事列表 /
一键录故事 / 回看回听 / 盖章 / 可选润色），本包交付物与之方向一致、无一处语义相反或措辞漂移，
故不构成 spec 更新触发项；本包实现进度未达的部分是「实现面尚未接入生产路径」（见
review-packet §8 残余限制），属包 8 冻结验收面之外的殿后项（P3-E2 host+PC 两面证据 + 真实采音
殿后 9b），不是 spec 与 shipped behavior 的口径冲突。**若** human 认为「视图模型已冻结断言绿」与
「设备/网关已按 spec 反应」需要在 spec 上分列现状标注，该动作归 planner/human（integrator 无
spec 正式面写权限）。

## 2. 实现侧文档改动（本包 docs-sync 主体）

| 文件 | 改动 | 为什么现在改 |
|---|---|---|
| `server/deploy/README.md` | **新增节**「OpenClaw system prompt 版本化接线（现状 = v3）」：v1/v2/v3 承载对照表、v3 相对 v2 的逐字承载实测数（102 保留 / 58 新增 / 12 仅版本字样）、**渲染面==声明面==被测面**不变式与其两条机械守卫、`v3 上 ECS 的动作序`（并明写「本包只产制品、未执行上线」）、`已知版本字面漂移`（VRT-A4/A7 钉 v1、CHR-A8/A9 钉 v2、P2-A6 假绿、VRT-E4 不受影响） | 原 README 全篇不提 prompt 版本，而 prompt 版本正是「生产用哪一块 prompt」的部署事实；bump 到 v3 后不写现状，下一个包无从判断改哪里 |
| `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` | **消费面条目改写**：原文无条件声明「live 探针（VRT-A4/A7）以该版本化文件为唯一 prompt 源」，实测两探针常量仍分别钉 v1/v2 → 改为如实声明「live 面所测版本落后于本声明版本」并指向 ARQ-260908 + 列出哪些 offline 守卫版本无关 | 声明件自身失真是最贵的一类漂移（读它的人会以为不变式已成立）。改后仍保留裸名 `openclaw-system-prompt` 与 `.v2/.v3` 字面，VRT-E4 / P2-A6 语料断言实测不受影响 |
| `server/deploy/provider.config.yaml` | 注释：`system_prompt` 结构说明补 v3 故事 producer 扩展一行（builder 已落，随实现提交 `f7c82d4`） | 三键值同切 v3 时注释须同切，否则声明面自相矛盾 |
| `server/deploy/apply-xiaozhi-overlay.sh` | 注释：渲染源演进史补「260829-story-p3 起切 v3」一行 + WARN 文案改 v3（builder 已落 `f7c82d4`） | 渲染失败时的 WARN 文案指错文件会误导排障 |
| `server/gateway/story.py` / `firmware/main/display/story_page.h` / `story_page.c` / `mcp_server.cc` | 模块 docstring / 头注就地记 why：契约零改动红线（不加生字字段 P3-E4）、内存叠压硬约束的有界实现、D1/D2/D3 冻结决策内联注记、降级不盲渲染（V27）、归档归属云侧（builder 已落 `f7c82d4`） | 冻结相位决策若只存在 taskpackage 里，读代码的人会在下一次改动时无意推翻 |
| `changes/dev/260829-story-p3/`（本目录） | 晋升 closeout 两件 blocking 制品（`docs-delta.md` + `review-packet.md`）。**红基线副本刻意不晋升到本目录**：源件 `firmware/main/test/acceptance/red-baseline/story-p3-red-baseline-summary.md` 已随 `f5309c7` 入仓、路径稳定，副本无增益；且本目录正是冻结 live 齿 `test_live_story_roundtrip_evidence` 的证据语料根（见 review-packet §9 F3），冗余副本会额外扩大该齿的散文假绿面 | dev-loop closeout 制品 SSOT：这两件维持 blocking 必交；taskpackage.json 快照已退役不产 |

## 3. 退役 / 不产件（post-cutoff 口径核对）

- **不产** `changes/dev/260829-story-p3/taskpackage.json`（gov-review-taskpkg-lifecycle FROZEN 2026-07-25；
  活文件 `.claude/taskpackages/260829-story-p3/taskpackage.json` 唯一承载）。
- **不产** `test-evidence.md`（机器产线运行：verifier-round fact `r1/r2/r3.json` 承重 + 本 packet
  §3 承载 integrator 亲跑计数）、`final-summary.md`（判断 prose 归 review-packet）、
  `session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl`
  （post-EVENTS_SCHEMA_CUTOFF 全退役，需要时 `node .claude/scripts/loop-metrics.mjs --change-id 260829-story-p3` on-demand）。
- `pod-events.jsonl` 存在性 = advisory，本包 live 文件在
  `.claude/taskpackages/260829-story-p3/pod-events.jsonl`（13 事件，含 2 human-decision +
  3 escalation-resolved），非 blocking。
- `verdict.json` / `verification.json` 在 `changes/dev/` 下为**投影·advisory**（脚本产线件，
  非 agent 手造）：本包不手造，承重件 = `verifier-evidence/rounds/r3.json`。
- `pod-retro.md` 归 planner 收尾相位，integrator 不代写（closeout-check 内为 Q5=warn advisory）。

## 4. docs-sync 复跑证据

- `node .claude/scripts/docs-sync-check.mjs --taskpackage .claude/taskpackages/current/taskpackage.json`
  → PASS（见 review-packet §7）。
- 编辑承重守卫语料（carrier）后复跑全量 server 验收套：`8 failed / 84 passed / 36 skipped`，
  与编辑前逐 ID 相同——证明本次 docs-sync 未翻绿任何冻结断言、也未撞红（review-packet §3/§7）。
