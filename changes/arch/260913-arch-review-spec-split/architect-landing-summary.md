# architect-landing-summary · 260913-arch-review-spec-split

> landing 执行记录（2026-09-13，主线程 packet-bound）。packet = FROZEN v1.1 PROPOSAL §2（T1–T6）+ §5 验证集；manifest 18 行终稿。

## 1. manifest 18 行逐条对账

| # | 路径 | 动作 | 实况 |
|---|---|---|---|
| 1 | docs/requirements.md | 新增（T1） | ✅ 落地：头部（已砍清单 5 件 + UI 锚规则块 6 条 + 锚行指针）+ §1 背景与产品定位 + §2 共享底座（§4.0+§5.2/5.3/5.4 合并去重，「端云」列废弃、节级支撑锚）+ §3 Epic E1 > F1–F4 > 13 story（五件重写）+ §4 体验与视觉风格 + §5 NFR 5 域 7 条 + §6 路线图 + §7 需求类开放问题 4 条 |
| 2 | docs/architecture.md | 新增（T2） | ✅ 落地：§1 Infra（1.1 板子 + 1.2 ECS/网络）+ §2 系统架构（2.1–2.10，含 2.9 提醒调度、2.10 录音归档两个机制迁入节）+ §3 UI 实现策略 + §4 契约层（引用式导航）+ §5 技术决策（28 条 verbatim + 待 brainstorm 处置）+ §6 技术类开放问题 3 条 |
| 3 | docs/archive/spec.md | git mv 迁入（T5） | ✅ `R docs/spec.md -> docs/archive/spec.md`（docs/archive/ 目录本次新建——repo-structure 早已登记该子目录，目录实体此前未建） |
| 4 | docs/CONTRACT-DECISION-QUEUE.md | 挂账登记（T5，F2 裁 (a)） | ✅ 追加 `CDQ-260913-spec-split-fallout` 一行（契约 :4 断链 + 冻结验收件 canary 翻红 + schedule_reminder:1561 直读，处置归 phase 2） |
| 5 | docs/DEV-GOVERNANCE.md | G1 改指（:241） | ✅ 治理路由表「系统 spec」行 → 两文件 |
| 6 | docs/notes/ui-implementation-strategy.md | 改指（:3） | ✅ 「spec 侧索引」→ architecture.md §3 |
| 7 | AGENTS.md | G1 改指（:7） | ✅ 「系统 spec 面」行 → 两文件 + 归档注记 |
| 8 | .claude/commands/arch-change.md | 改指（:24） | ✅ lane 分类 canonical spec 行 → 两文件 |
| 9 | .claude/scope-map.json | T4 | ✅ docsTopLevel allowFiles：−docs/spec.md +docs/requirements.md +docs/architecture.md |
| 10 | .claude/rules/repo-structure.md | T4 | ✅ docs/notes 行「spec 唯一」→ 两文件规则；白名单文件行同步（两处一致） |
| 11 | .claude/agents/architect.md | 自改递归改指（:23/:52，human 知情批准 F6 裁 (a)） | ✅ |
| 12 | .claude/agents/architect-reviewer.md | 自改递归改指（:42/:146） | ✅ |
| 13 | .claude/agents/integrator.md | 自改递归改指（:42 Basis 前缀约定 + :71 消解边界清单） | ✅ 与闸字面集同批联动（本 commit 原子） |
| 14 | .claude/scripts/spec-anchor-check.mjs | 新增（T3） | ✅ 三检 + --reverse + --diff-ids；落地中发现并修复两处解析缺陷（支撑锚 `支撑 = ` 带空格形态漏匹配→真空绿假绿；`## 3.` 顶层编号小节标题不解析），修复后全绿 |
| 15 | .claude/scripts/pre-merge-escalation-gate.mjs | G3 联动（字面集扩展 + 自检用例） | ✅ CONTRACT_SPEC_LITERALS 四件（保留 docs/spec.md 兼容历史 findings）；自检 30/30 绿（新增 4 例：requirements 检出 / architecture + CDQ 核销 / 归档件兼容检出 / 非字面集路径放行） |
| 16 | .claude/skills/write-story/SKILL.md | 新增（T6） | ✅ 按 human 审定草案落地，示例修正见 §3 注记 8 |
| 17 | .delivery/milestones.jsonl | SSOT 改指（R3-F12） | ✅ acceptanceRef ×7 → docs/requirements.md；两处 prose 语义引用一并改指（M0「spec §2」→ architecture.md §1.1；M6「spec §9」→ requirements.md §7 / architecture.md §6） |
| 18 | .delivery/delivery-workspace.json | 权威指针改指（:6） | ✅ → docs/requirements.md §6 路线图 |
| 派生 | .delivery/STATUS.md | 重生成（不手改） | ✅ `node .claude/scripts/delivery-status.mjs` 重跑，ref 链 ×7 指 requirements.md，零 spec.md 残留 |

## 2. spec §6.3「待定/已定跟踪」七条逐项处置清单（T2/R1-F1 收口）

| 跟踪项 | 处置 | 依据 |
|---|---|---|
| 通信协议（WS+Opus 流式，方向 B） | **已结不迁** | 内容已由 architecture.md §2.2/§5.1 决策 9 承载 |
| 音频格式（上行 16k/60ms、下行 24k 协商） | **已结不迁** | 已由 §2.2 架构图承载 |
| 鉴权（强制 token） | **已结不迁** | 已由 §5.1 决策 22 + requirements.md NFR-SEC-1 承载 |
| 设备服务器地址入口（配网直写 NVS，websocket/url/token/version） | **已结不迁** | 配网已成功（human 口径）；实现细节由固件代码本体承载 |
| 传输安全（wss 自签证书） | **已结不迁** | 已由 §5.1 决策 22 + NFR-SEC-1 承载 |
| ASR 云额度与 provider 终选 | **已结不迁** | live 冒烟多轮已跑，human 原话「早就结束了」 |
| 多轮会话连续性 | **迁入** architecture.md §6 开放问题 1 | 🔵 待部署验证，ARQ-260902-multiturn-continuity 在册 |

其余「已结不迁」：§9 已定 recap 行（决策节内容的汇总复述，不单迁）；ECS 算力（已缓解已结，入 §1.2/§5.2 处置行）；离线降级（决策 24 已载）；§5.6:529 暗光行（NFR-READ-2 已砍，环境约束声明不迁；硬件事实留在 §1.1 屏幕行）；§5.6:530 待补行（转入各 feature 节「线框待补」占位 + requirements.md §7 开放问题 4）；spec 头部 v0.2 冻结凭据 blockquote（随归档件本体保留，不迁）。

## 3. story 重写变更注记清单（§5 #8 方程第四操作数）

1. **S-F3.3 收窄**（实质）：删「还想再查下一个字」半条 → 挂 requirements.md §7 开放问题 1；story 原文收窄重写。human 裁 2026-09-13。
2. **S-F3.1 / S-F3.2 可行性 🟡**（实质）：host 证据在但 live 查字链路当前不可跑（human 口述），标定降级并注明 phase 2 修复优先。
3. **S-F2.5 整砍**（实质）：云端润色 story 不迁入，入已砍清单。
4. **全量：机制词剔除**（形式）：spec 验收块中的实现归属细节（cron/幂等散列/重试 60s×3/E1 control/V 编号裁定等）不进 story 正文——S-F1.4 机制段 → architecture.md §2.9；S-F2.2 机制段 → §2.10；底座表 V27/4 秒定时器 → §2.2；story 只留用户可感知口径。
5. **全量：「端云」标注废弃**（形式）：原 story 锚行与底座表的 `[板]/[云]` 列由支撑锚吸收。
6. **S-F1.4 范围口径入补充规则**（形式）：范围/三档时刻/过期不追催留补充规则，机制部分迁 §2.9。
7. **开放问题新增**：「语音查下一个字（指代解析）」入 requirements.md §7（呼应 ARQ-260902-multiturn-continuity）。
8. **skill 示例修正（landing 期发现，human 复核点）**：write-story 草案的 S-F1.4 示例原 AC3 写「断网本地提醒照常弹出」——与已定 NFR-OFF-1「断网无提醒」直接矛盾（草案笔误）；落地版示例对齐 requirements.md 现文（AC5 = 断网无提醒），示例锚改真实小节号（arch §2.9/§3.1）、时刻对齐 20:30。

业务框架句 verbatim 可溯声明：13 条 story 的「作为…我希望…以便…」句除 S-F3.3（注记 1）外均与 docs/archive/spec.md 原文逐字一致。

## 4. 决策正文机械改指注记

architecture.md §5.1 决策 28 条 verbatim 迁入，仅两处内部自引用机械改指（语义不变）：决策 4「spec §3.0.0」→「本文件 §2.2」；决策 17「（§5.7）」→「（本文件 §3.1）」。story 引用（S-Fx.y）指向 requirements.md。

## 5. 验证结果（§5 验证集逐条）

1. **时序前置**：.planning 旧批已归档（ef257fa，landing 前一日完成）✓
2. `node .claude/scripts/spec-anchor-check.mjs` → **ok:true**（story 13、锚行 22、arch 小节 21、设计稿 id 61、errors 0；真空绿断言前置通过）✓
3. `--reverse` → 非空，10 个小节反向清单；抽样人读核对：§2.9→S-F1.4、§2.10→S-F2.2、§2.8→S-F3.1/3.2/3.3+NFR-PERF-2、§3.1→9 个引用方，正确 ✓；`--diff-ids v2 v3` → 61/61 全等、零断锚 ✓
4. census 复扫（`rg -n 'spec\.md'` 排除豁免面）→ live 面零待改残留；仅存命中 = 本批新增文件的溯源注记 ×4 + CDQ 挂账行 + ARCH-REVIEW-QUEUE.md:49（append-only 豁免）+ templates ×5（通用占位豁免）✓
5. `changes-index.mjs --upsert` → ok（unchanged，FROZEN 时已钉）✓
6. escalation-gate `--self-check` → **30/30 绿**（含新增字面集 4 例）；integrator.md:42 约定与闸字面集人工比对一致（同 commit 落地）✓
7. `.delivery/` 重生成：milestones.jsonl + workspace 改指后 `delivery-status.mjs` 重跑，STATUS.md ref 链 ×7 指 requirements.md、零 spec.md 残留 ✓
8. 零丢失核验方程：两新文件节集合 ∪ 已砍清单 5 件 ∪ 已结不迁清单（§2）∪ 本注记清单 ≡ spec.md 九节全集——逐节映射：§1→req §1；§2→arch §1.1；§3→arch §2.1–2.8（3.0/3.0.0/3.0.1/3.0.2/3.1/3.2/3.3/3.4）；§4→req §2 底座 + §3 story 13 条（S-F2.5 转砍）+ 机制段迁 arch §2.2/§2.9/§2.10；§5.1→req §1 设备定位；§5.2/5.3/5.4→req §2.1/2.3/2.4；§5.5→req WF-F1-01（F1 节）；§5.6→req §4（:529/:530 除外，已结不迁清单）；§5.7→arch §3.1；§6.1/6.2→arch §4 导航（细节随 CDQ-260902 归契约）；§6.3→本文件 §2 七条处置；§7→arch §5.1 verbatim + §5.2 处置；§8→req §6；§9→req §7 + arch §6 + 已结不迁 ✓
9. story 五件核验：13 条 + 底座逐条五件齐；AC 编号在场；机制词抽查（cron/散列/幂等/JSON/预合成）在 story 正文区零命中（底座与 NFR 章为共享/质量面，story 正文净）✓
10. skill 卫生：`lint-descriptions.mjs` exit 0（唯一点名 = landing-verifier.md 既有超长，非本批新增）；`route-regression-check.mjs` 28 ok / 0 fail ✓

## 6. 遗留与交接（不动本 packet 结论）

- 冻结验收件 canary 翻红 + schedule_reminder:1561 直读 + 契约 :4 断链：挂 CDQ-260913-spec-split-fallout，处置归 phase 2 验收基线重建（建议：新 canary 统一重钉 + 旧断言改指/退役一个动作完成）。
- S-F3.1/3.2 🟡 + NFR-PERF-2 🟡 + NFR-READ-1 🟡 → phase 2 立项输入（查字链路 live 修复 + 延迟优化 + 字号烧板对照）。
- 设计稿 v2/v3 内 spec 引用各 1 处：v4 迭代时顺带改（§6 留根）。
