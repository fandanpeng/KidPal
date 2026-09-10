# docs-delta · 260829-hardware-validation（包 9b 硬件真机验收）

> integrator docs-sync 结果清单（docsOwner=integrator）。本包 = 硬件真机验收（riskTier=high，
> testHost=device），文档面动作跨「实现侧文档 + 真机证据日志 + 链路事实梳理」三类。
> 契约面零触碰（`docs/contracts/**` 在 `3edbb24~1..2c753ab` diff 范围内零命中，git 实测确认）。
> 收口日期 2026-09-10，分支 `dev/260829-hardware-validation`，commit `3edbb24` + `2c753ab`。

## 1. `docs/` 正式面改动

| 文件 | 改动 | 内容 / 消费方 / 后续 |
|---|---|---|
| `docs/notes/board-hardware-esp32-s3-rlcd-4.2.md` | **新增**（commit `2c753ab`，94 行） | 官方硬件速查（来源：微雪中文 docs 站 2026-09-10 抓取 + 官方参考 Demo `button_bsp.c` 交叉核对）：三键盘点（BOOT=GPIO0 在用 / PWR 硬件电源键 / **KEY=GPIO18 未接**）、核心规格（N16R8 / 4.2" RLCD 300×400 竖版按 400×300 横用 / ES8311+ES7210 双麦 / PCF85063+SHTC3 / 18650）、已接入外设↔固件消费点对照表、装配注意。文件头明写「事实参考，不是 spec」。**消费方**：后续补接 KEY 的包（含接入方法与悬空 GPIO18 上拉/下拉提示）、硬件排障、spec 键位漂移裁决的事实底座。**待 review 项**：其中「spec↔实现键位对照」一节记录了 spec §4.0/§5.3 两键分工 vs 实现「仅 BOOT 一键」的漂移——该漂移的 spec 处置（补接 KEY 落地两键分工，或 Track B 过渡性修订承认一键方案）**待 human 裁决**，本包不改 spec。 |
| `docs/spec.md` | **零改动** | 有两处**已识别但未修订**的漂移/待同步项，见 §4 遗留清单（① spec §4.0/§5.3 键位分工 vs 实现漂移——待 human 裁处置方向；② VOICE-ROUND-FLOW-260910.md 的链路结论按 human 要求 **review 后再同步 spec**，本包不先行）。integrator 无 spec 正式面写权限，且两处均属「判断未完成」而非「可顺手改」。 |
| `docs/contracts/**` | **零改动** | git 范围 `3edbb24~1..2c753ab` 实测：`git diff --name-only | grep ^docs/contracts` = 空。本包 display 下发走既有契约 view 白名单（text/schedule/story_list/story_text/char/control），无新增 view、无字段增删。 |
| `docs/` 根其余文件 | 零改动 | 未新增 docs 顶层文件，不触 `scope-map.universal.docsTopLevel` 白名单边界；`docs/ADR.md` 本包无新架构裁决条目（渲染夹钳 begin_page/end_page 属实现机制非边界裁决，记 PROGRESS-SUMMARY §二.9）。 |
| `changes/arch/**` | 零改动 | 本包是 dev-loop 包，无治理 landing。 |

## 2. 实现侧文档 / 证据面改动（本包 docs-sync 主体）

| 文件 | 改动 | 为什么现在改 / 消费方 |
|---|---|---|
| `changes/dev/260829-hardware-validation/PROGRESS-SUMMARY-260910.md` | **新增**（commit `3edbb24`，152 行；`2c753ab` 补 §五 6 项） | 当日真机轮 consolidated 记录：里程碑、9 项修复链（含逐项根因）、流程教训、后续 A 设备观感 / B 物理验收 / C ECS 部署 / D dev-loop 收尾 / §五 晚间链路梳理新增问题。**消费方**：交班与收尾的单一入口，后续修包（故事页排版、字体、课表页、ECS 漂移）的 backlog 源。 |
| `changes/dev/260829-hardware-validation/VOICE-ROUND-FLOW-260910.md` | **新增**（commit `2c753ab`，347 行） | 语音轮端到端逐文件链路梳理（事实梳理，非 PROPOSAL）：全链路图、上行/引擎/下行三段逐帧表、UI 三屏主权冲突清单（§五 6 项）、时延归因表（§六，续轮 3–6s 四段串行）、改法候选（§七 5 项，均未立项）、时序图（§八）。经 planner 自 review 修 7 处 + human 质询更正拾音模式（auto 非 manual，pod-events `f7b8c9d0`）。**消费方**：后续 UI 主权收口 / 显示先行 / 首音提速 PROPOSAL 的事实底座。**待 review 项**：human 要求 review 通过后有价值结论再同步 spec——**本包不自行同步**，登记为遗留。 |
| `changes/dev/260829-hardware-validation/*.log`（14 件） | **新增**（commit `3edbb24`） | 真机原始证据：`flash.log`（6 分区烧录 hash verified）+ `boot.log`~`boot8.log`（8 轮开机回归：boot hang→I2C 恢复→BCD→SHTC3 三步→art mount→渲染节点上线→单击修复）+ `voice-round1~4.log`（语音轮四轮：esp_timer 溢出→ssl_receive 溢出→SetStatus 悬空→0 崩溃 0 复位全绿）。HWV-E2「真机实测证据非模拟」的落盘承载。 |
| `.claude/taskpackages/260829-hardware-validation/device-acceptance-checklist.md` | **新增并回写**（commit `3edbb24`） | 16 项勾验清单（表 A 物理 + 表 B v0.4 新增 + 表 C live 五齿）。已回写结论：HWV-A3 部分绿 / HWV-A4 部分绿 / HWV-A6 部分绿（story 主流程轮4 补记）/ 证-1（HWV-A15）绿，其余留白 = OPEN。**消费方**：human 持板续验的执行与回写面。 |
| `server/deploy/openclaw-system-prompt.v4.md` | **新增**（commit `3edbb24`，247 行） | F4 分工修正载体（HWV-A12）：网关 story_text(record) 组块下发、接线 prompt 无「回填」、模型一回合只出 title。消费方 = ECS 部署链。**注意部署漂移未修**：`apply-xiaozhi-overlay.sh:60/62` 仍写 ASR=AliyunStreamASR / Intent=function_call，与 live（Qwen3ASRFlash / nointent）不符——直接跑 overlay 会覆盖 live 断语音。见 §4 遗留。 |
| `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` / `apply-xiaozhi-overlay.sh` / `provider.config.yaml` | **更新**（commit `3edbb24`，+22/-2 / +19/-8 / +8） | 版本化载体声明、渲染源切 v4、provider 配置指向 v4。三面同切由本包冻结齿与既有载体齿机械断言（verify final acceptance 闸 126 passed 覆盖）。 |
| `changes/dev/260829-hardware-validation/`（本目录） | **晋升 closeout 三件**（本次）：`docs-delta.md` + `review-packet.md` + `pod-retro.md` | dev-loop closeout 制品 SSOT：docs-delta / review-packet 维持 blocking 必交；pod-retro 本次按收口派单由 integrator 落（模板 `docs/templates/retro.template.md` 骨架）。不产 taskpackage.json 快照（gov-review-taskpkg-lifecycle FROZEN 2026-07-25 退役）。 |

## 3. 退役 / 不产件（post-cutoff 口径核对）

- **不产** `changes/dev/260829-hardware-validation/taskpackage.json` 快照（退役已落地，活文件
  `.claude/taskpackages/260829-hardware-validation/taskpackage.json` 唯一承载）。
- **不产** `test-evidence.md`（真机证据以 *.log 原始件 + device-acceptance-checklist 回写承载，
  无需手工替代件）、`final-summary.md`（判断 prose 归 review-packet）、
  `session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl`
  （post-EVENTS_SCHEMA_CUTOFF 全退役，需要时 `node .claude/scripts/loop-metrics.mjs --change-id 260829-hardware-validation` on-demand）。
- `pod-events.jsonl` 在档（`.claude/taskpackages/260829-hardware-validation/pod-events.jsonl`，
  38+ 事件含 5 条 human-decision），存在性 = advisory、非 blocking。
- `verdict.json` / `verification.json`（changes/dev/ 下）= 投影·advisory、脚本产线件，不由 agent
  手造；承重件 = `.claude/taskpackages/260829-hardware-validation/verifier-evidence/rounds/r1.json`
  （final round 1，ok:true，9 闸全绿，见 review-packet §1）。

## 4. 未做的文档面动作（诚实遗留清单）

| 未做项 | 现状 | 去向 / 原因 |
|---|---|---|
| **spec 键位漂移未修订** | spec §4.0 全局手势表 + §5.3 设计「KEY 主司对话 / BOOT 主司翻页」两键分工；实现仅接 BOOT（单击对话/双击翻页），KEY(GPIO18) 未接，依附 KEY 的页内特例（P2 问课程 / P3 列表光标+双击进阅读 / P3 阅读念给我听 / P4 播笔顺 / KEY 长按回首页 / 提醒弹窗 KEY 关闭）全部悬空。对照已落 `docs/notes/board-hardware-esp32-s3-rlcd-4.2.md` §一。 | 待 human 裁：补接 KEY 落地 spec 两键分工（新包），或 spec 过渡性修订承认一键方案（Track B）。integrator 不改 spec。 |
| **VOICE-ROUND-FLOW 结论→spec 同步未做** | 链路梳理结论（UI 三屏主权、时延归因、断链①②）在 changes/dev 事实件内，spec 未动。 | 按 human 要求 **review 后再同步 spec**——待 review 项，本包不先行、不自行改 spec/contracts。 |
| **ECS overlay 漂移未修** | `apply-xiaozhi-overlay.sh:60/62`（ASR=AliyunStreamASR / Intent=function_call）与 live config（`provider.config.yaml:77` asr.provider=aliyun vs live Qwen3ASRFlash / nointent）不符——直接跑 overlay 会覆盖 live、断语音。 | 危险项：修漂移使脚本=live 必须先于任何云端调优实验（VOICE-ROUND-FLOW §七.5 前置项）。归 planner 立项，integrator 不在收口 commit 内混入。 |
| **HWV-A7 字号下限结论回写设计面未做** | 清单约定 A7 结论回写 `docs/ui/UIUX-DESIGN-BRIEF.md`（V3 字号下限）。 | A7 真机未验（OPEN，与字体问题耦合——正文用普惠 basic 子集缺「猬」等字，设计字体未接正文），无结论可回写；随 B 物理验收剩余项一并。 |
| **live 五齿 L1-L5 evidence 交班未做** | 提醒 cron §8 部署未落，L1-L5 无真跑证据。 | 归 HWV-A16 剩余面（ECS 部署窗口），随 §C 部署漂移修复一并。 |
| **`.delivery` / `.planning` 面未更新** | 本包 deliveryImpact=milestone-affecting，但 milestone 跳变判定与 progress 收口行归 planner 相位。 | 交回 planner / 主线程；integrator 不代写 `.planning/kidpal-mvp/progress.md`。 |
