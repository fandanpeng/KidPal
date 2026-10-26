---
changeId: 261025-spec-reconciliation-memory-writing-scope
sources: 2026-10-25 human 原话逐字——①「按你的裁决归面 C，这是什么，现在在哪里记着」；②「建一个 intake spine ，不用启动实现，明天我再启动实现」。关联引用 = `TECH-261025-dreaming-capture-pollutes-honcho`（主触发 debt，本批消费，其 `paths` 已含 `docs/architecture.md` 与 `docs/decisions/ADR.md`）、`TECH-261022-device-memory-prompt-overhaul`（只引不改，证据 6 与本批第二来源同根）、`delivery/taskpackages/261024-oneshot-user-uniformity/docs-delta.md` §一（六条过时面逐行清单 = 本批第一来源）、`delivery/taskpackages/261025-openclaw-dreaming-disable/docs-delta.md`（「spec 面已知在途」节，申报归本批）、`delivery/review/261022-arch-review-device-memory-prompt-overhaul/dreaming-capture-noise-20261025.md` §4/§5/§6（architect 裁决 + human 四裁决 + 执行次序第 6 条）、`docs/decisions/ADR.md` ADR-016。查重结论：`delivery/intake/` 在册 17 份无同意图 spine（最近邻 `261022-device-memory-prompt-overhaul` 已 in-flight，但意图不同——那批建设记忆子系统本体，本批是口径对账）；debt 查重走 §3a 晋升，消费 TECH-261025、不改 TECH-261022。命名沿革注记：「面 C」是 2026-10-23 `memory-wiring-advisory-20261023.md:218` 给出的批次代号（那一轮把记忆修正拆三面，面 C = `/arch-change` 改 §6.7/§1.3 口径）；那批 10-23 已落一次，其后被 10-24 的 noisePatterns 退役与本日的 dreaming 发现两次甩旧，本 spine 承接的是**第二次对账**，沿用旧称只为可追溯。
faceMap: 需求面**动（语义修改，档位待流程判——AC5 验收口径反转可能触档 2 结构性）**——`docs/requirements.md:598` S-F5.1 AC5「设备自身流程触发的回合…其内容不进记忆画像」现文与 261024 R7/OQ-4 裁决（一次性回合统一携 `user=<device_id>`、落同一设备 session 键族、处置由「隔离」改判「统一归属」）冲突；`:601` 补充规则同段「设备流程触发的回合不入画像」与「设备流程回合不入画像的边界自记忆子系统上线起对新增回合生效」两处连带；`:653-659` NFR-PRIV-1 边界段同口径。frozen v1.67，本批启动须走 `/requirement-change` 的 updating→frozen 流程与 §9 版本行。架构面**动（实质变更，/arch-change Track B 主路径，无 Track A 理由）**——`docs/architecture.md:360` §6.7「实际边界由三件构成」①②两句双失效（① `noisePatterns` 已于 261024 R7/OQ-4 退役并已在 live 清空；② `captureSystemRuns=false` 挡不住 dreaming——今日实测 dreaming 子回合落 honcho chat 分支，占 owner 最近 100 条窗口 13/100，`TECH-261025` 在册）；`:364-366` 表 1/2/3 行同根失准；`:155` §4 NFR-PRIV-1 映射行写着「noisePatterns 挡 front 一次性回合 user 消息 + captureSystemRuns=false 挡系统回合」；`:203` §5.1.2 记忆子系统行「写入范围与残余见 §6.7」随 §6.7 换代；**`:192` §5.1.2 front 行尾「引导 / 整理一次性 LLM 回合不携 user（user 消息不入库…）」（architect 2026-10-26 核出的 spine 漏列项，与 live 实况相反，必须进架构面 landing 清单）**；`docs/decisions/ADR.md` ADR-016 决策②仍写「front 一次性回合的 user 消息以插件 noisePatterns 挡」，且写入范围枚举缺 dreaming 类。**目标形态（architect 裁决）= 由逐行枚举升级为类级规则**——「平台内部系统回合（cron / 心跳 / 记忆巩固与叙事生成等）不入画像」，dreaming-narrative 作为该类的一个实例注明。契约面**不动**（零动作）——记忆写入语义不在 `docs/contracts/**`；契约 §四 `kidpal_story_delete` 只引用不定义，本批不动。治理面**不动**（零命中）——不触 `.claude/**`、指令根、gate。实现面**不动**（零动作）——纯文档面批，无代码、无部署、无数据迁移（dreaming 开关与清理已由 `261025-openclaw-dreaming-disable` + human 运维轮另行消化）。触面 = 2（需求 + 架构）→ 建本 spine。
facePlan: 执行顺序（architect 2026-10-25 裁决）——**1. 需求面** = `/requirement-change`（档位由该面流程判；story-author 盘上 updating 起草 → requirement-reviewer 四查 → 档 2 则 architect 必派 → human 终拍 → 主线程翻 frozen + §9 版本行）；前置 = human 明日起批指令。**2. 架构面** = `/arch-change` Track B（architect 起草 PROPOSAL → architect-reviewer 四查+判档 → FROZEN → packet-bound 落地 → landing-verifier → retro → 合并）；前置 = 需求面 frozen（AC5 与 NFR-PRIV-1 措辞是 §6.7 类级规则的语义输入）。两面同段落连续改写，**顺序不可颠倒**（先需求后架构，避免同一段文字两轮互相覆盖）。**3. 契约面**：零动作（faceMap 已判）。**4. 实现面**：零动作。**合并门**：各面各归各裁（需求面 §9 版本行、架构面 Track B 七步），无跨面联合门。
anchors: {req: "已回填——/requirement-change 档 2 完成（2026-10-26）：requirements **v1.68 frozen**（S-F5.1 AC5 按「内容归属」四段式整段重写 + 补充规则口径对齐与「已知残余」申报 + NFR-PRIV-1 ① 回指 + 可行性行连带换代）；定稿载体 = §9 v1.68 行（列明五处改动与流程留痕）。流程 = story-author 两轮盘上起草（第一版回合粒度表述被 human 当场纠正为内容粒度）→ architect 可行性三件（第一轮 🔴「提醒到点语义回合被错放进统一归属」随第二轮删除消解；第三段类级边界判 🟡——只能静态配置核 + 运维观测，无管线机械闸）→ requirement-reviewer 四查 PASS 零否回（特裁 ② 主动对齐认可、特裁 ④ NFR 粒度张力维持现状）→ **human 2026-10-26 终拍通过**（NFR 粒度维持现状 + 回指）。触 UI：否", arch: "待回填——/arch-change Track B 待起（前置 = 需求面 frozen ✅ 已满足）；landing 清单除 faceMap 五处外，须含 architecture.md:192 漏列项与 ADR-016 带日期注记"}
openQ: OQ-1（开放，需求面裁）本批落点范围 = 261024 docs-delta 的六条过时面 + 本日 dreaming 类级规则（共七处），一次改完还是分两轮？主线程建议一次改完（同一段落连续改写，分轮必互相覆盖）。OQ-2（开放，架构面裁）「平台内部系统回合」的类级定义边界——dreaming / cron / heartbeat / session 启动序列之外，`subagent:*` 是否纳入本类（今日实证 `isSystemRun` 对 `subagent:` 前缀同样只参与分类不参与排除，属同族缺口但尚无命中数据）；定义过宽会误伤将来的合法子代理记忆。OQ-3（开放，架构面裁）§6.7 表 1/2 行（引导 / 整理回合）在 noisePatterns 退役后的正确表述——现行实况 = 一次性回合统一携 `user=<device_id>` 落同一设备 session 键族（261024 R7/OQ-4），故原「user 消息被挡、不入库」两句须整体换代；改写须以 live 实读为准，不得凭旧文推演。OQ-4（开放，需求面流程判）AC5 属「验收口径反转」——判档 1（语义修改）还是档 2（结构性）由 requirement-change 流程按既有判据裁，本 spine 不预判。OQ-5（开放，主线程建议不并）`docs/debt/tech/TECH-261023-openclaw-tool-face-prompt-only-readonly.md:27` 仍写「四项」，但 debt 账本 append-only 且不属 spec 面——建议不在本批顺手改，由该条目自身生命周期处理。**OQ-1 已裁（2026-10-26）：一次改完**（同段落连续改写，分轮必互相覆盖）。**OQ-4 已裁：档 2（结构性）**——NFR 阈值语义变更 + 跨 AC 牵连 ≥2 处直接落档 2。**OQ-3 已裁：按「内容归属」而非「回合」写**——architect 2026-10-26 核实确认：插件的保护按 role 扇出（assistant 消息不进 owner 画像），但设备流程回合的 user 消息是「机器包装指令 + 孩子原话」混合体、整体挂 owner 角色，故回合级表述会与实现不符。**OQ-6（新增，2026-10-26 human 裁「先记残余 + 登 debt」）**：机器包装指令被抽成孩子过程性结论（live 实证 owner 侧 6 条、位于最近 100 条窗口第 35–69 位），需求面按现状记残余申报，机制修复登 `docs/debt/tech/TECH-261026-oneshot-wrapper-prompt-derived-facts.md`（四条候选路径：改包装形态 / deriver 提示词补丁 / 运营清理 / 上游提报）。
status: in-flight
---

# spine · 记忆写入范围口径对账（「面 C」第二次）

> 触面 ≥2 跨面变更的路由账本（[change-intake.md](../../.claude/references/change-intake.md) 程序；spine 免冻——各面冻结语义由各面制品承载，anchors 记录冻结事实）。
> **本 spine 只建账，不启动实现**：human 2026-10-25 明示「不用启动实现，明天我再启动实现」。

## 建档前查重（change-intake.md §3）

- **既有 spine 排查**：`delivery/intake/` 在册 17 份逐份按意图与触面比对——无同意图件。最近邻 [261022-device-memory-prompt-overhaul.md](261022-device-memory-prompt-overhaul.md)（in-flight）是记忆子系统的**建设批**（需求新增 F5 + 架构新增子系统 + 实现拆 P1–P5）；本批只做**口径对账**（把两次变更后过时的 spec 表述改成已验证实况），不建新能力、不碰实现面。两者同 workstream 不同意图，不消费旧 spine、不合并。
- **debt 四域排查**：命中 2 条——主触发 `docs/debt/tech/TECH-261025-dreaming-capture-pollutes-honcho.md`（2026-10-25 登记，`paths` 已含 `docs/architecture.md` / `docs/decisions/ADR.md`，`handoff` 写明「spec 修订 → 面 C arch 批」，本 spine 即其载体，走 §3a 晋升）；只引不改 `docs/debt/tech/TECH-261022-device-memory-prompt-overhaul.md`（其证据 6 记的 dreaming 空转与本批第二来源同根）。

## 立项 ack（change-intake.md §4 · 唯一必停人类触点）

human 2026-10-25 指示「建一个 intake spine，不用启动实现，明天我再启动实现」——即本 spine 的立项确认，同时明确**各面 loop 与实现面均不启动**，起批时机由 human 明日另行给令。

- **为什么改**：`docs/architecture.md` §6.7 与实际 live 行为已经脱节两次——① 10-24 的 `261024-oneshot-user-uniformity` 把 `noisePatterns` 退役，§6.7「user 消息被 noisePatterns 挡」两句失效；② 10-25 查出 `captureSystemRuns=false` 挡不住 dreaming（`memory-core` 做梦子回合被 honcho 插件判为普通 chat 会话，占 owner 最近 100 条画像窗口 13/100）。spec 若继续这样写，读者会以为写入边界已经被两个配置键守住，而实际拦不住。
- **往哪改**：写入范围由**逐行枚举**升级为**类级规则**（平台内部系统回合不入画像），dreaming-narrative 作为实例注明；连带 NFR-PRIV-1 映射行、§5.1.2、ADR-016 注记；需求面 AC5 与 NFR-PRIV-1 边界段同步换代。
- **改动多大**：纯文档面——两文件（`requirements.md` / `architecture.md`）+ ADR 注记；无代码、无部署、无数据迁移；触面 2（需求 + 架构），契约面与治理面零。
- **不在本批**：dreaming 开关本身与脏数据清理（已由 `261025-openclaw-dreaming-disable` 与 human 运维轮分别消化）。

## 触面事实（查出来的，非申报）

见 frontmatter `faceMap`（逐处带 `file:line`；需求面 3 处、架构面 5 处、契约/治理/实现面各零）。

## 收口

（待各面 anchors 回填后追加。）
