# Challenger Findings · R1 · 260908-gov-review-merge-chain-hardening

> challenger（agentId a7405009d10843693）产出；子代理写盘被 harness 拦截，moderator 按返回文本逐字归档（2026-09-08）。基线亲核：main tip fae886a。

## 一、Findings（13 条，falsifiable-first）

**F1 · BLOCKING · hard-violation · T6 lib 撞名 + sweep「新增」误标**
PROPOSAL.md:14/:99/:116 把 `.claude/scripts/lib/governance-gate-shared.mjs` 标为「新增」，但该文件**已存在**（518 行，git log 实证落地于 260907-gov-review-arq-governance-batch-2 commit c099c1b），是 ARQ-097 结构门共享单源（isFrozen/TARGET_PATTERNS/parseSection2AnchorLists/compareFrozenPin 等），消费方 = landing-fidelity-check / proposal-consolidation-check / closeout-check / taskpackage-required-check / devloop-artifact-check / changes-index 六个承重脚本（文件头注 :2-16 自述）。PROPOSAL:97 同句自相矛盾：「提取至 …governance-gate-shared.mjs（既有先例同名 lib）」。按「新增」landing 要么覆盖既有单源（六消费方静默失配）要么追加（blast radius 扩及全闸族而 T6/V2 等价夹具只覆盖两消费方）；charter §8.4「lib 是新增文件」前提同失实。修向：新 lib 改名，或明裁「并入既有文件」并补六消费方影响面 sweep + 既有自检零回退夹具。

**F2 · BLOCKING · hard-violation · V4 负断言永假**
PROPOSAL.md:139 V4 负断言 grep -n "docs/contracts" .claude/dev-loop.settings.json 零命中——本日实测命中 3 行（:7 _doc、:18-19 deny 条目 "Edit(./docs/contracts/**)"/"Write(./docs/contracts/**)" 本身含该字面）。与 canary 7（deny 面零删改）直接冲突 → V4 按字面永不可满足，landing-verifier 必红或被迫静默重释。charter GA-9 称「可机证」——独立跑核结论为不可证真。修向：断言改写（如「permissions 段无 docs/contracts 的 allow/ask 条目」或「deny 两行逐字在位 + grep 行数不增」）。

**F3 · BLOCKING · gap · manifest 缺 T7/销账必改文件，锚集对账机判必红**
T7 目标文件行（:105）唯一 canonical 锚 docs/ARCH-REVIEW-QUEUE.md 不在 manifest 六路径（:12-19），而 V5（:140）明令 landing 时机械结移该文件。亲核 parseSection2AnchorLists（governance-gate-shared.mjs:345-423）：该锚过 ANCHORABLE_TEST 入 §2 并集，注记「（versioned-guard 行销账，随批 landing）」不含「条件」字样、不触发裁剪 → compareAnchorSets 必报 onlyInSection2 失配。charter §1 只承认「三选一无法列」这一半，销账行无豁免理由。修向：FROZEN 前 manifest 补该文件（三选一裁后补落点）或锚行加条件注记。

**F4 · SHOULD · hard-violation · moderator 头注修复不完整：§2 标题残留**
逐字复核确认头注两处互斥已修净（:4/:6 无互斥、packet-binding 已改 T1–T7），但 :57 `## 2. 改法（T1…T6）` 残留，与 :6「T1–T7」及 :101 `### T7` 同文件两个口径——charter §8.1 点名的同类残留漏网。修向：标题改 T1…T7，全仓 grep T1…T6 归零后 FROZEN。

**F5 · SHOULD · hard-violation · T1-4 候选口径「两者恒用 projectDir」自违冻结 canary 10**
PROPOSAL.md:64 把该方向列为评审期候选且无警示；charter §6 canary 10 明文「--source-ref HEAD 求值主体不得从 Pod worktree 改到主仓」。且实际采纳即链级破坏：gate cwd 落主仓 → HEAD=main → off-main BLOCK 恒命中（gate :151-160）。修向：删除该选项或标注已被先验排除。

**F6 · SHOULD · gap · V1-③ 断言预设 A-3 单侧裁断**
PROPOSAL.md:136「cwd ≡ --project-dir 解析主体」——若 A-3 裁「维持两形、只改回退分支」（architect §review-bg 1 判据指向此侧），cwd 与 --project-dir 是有意异主体，断言不可实现。charter GA-10 标了 V3 的 OQ3 依赖，漏了 V1-③ 的 A-3 依赖。须裁后回填。

**F7 · SHOULD · gap · S-2 期望答案未满足：planner 派单格式义务零落点零留根**
T3 前置 1/2（:79）的消费对象由 planner 生产，但 planner.md 不在 manifest、§6 留根清单（:143-150）六条无一涉及——幽灵授权失效链（停等互锁/会话现商/就地越权）无任何成文响应，违反 charter S-2「哪怕落点在留根清单也要有 forcing-function」。

**F8 · SHOULD · gap · T6 等价断言缺显式排除项**
V2（:137）未排除「spawn→import 消除以 merge-driver argv 误判 changeId」语义差（charter GA-10②要求显式排除，该差无可证夹具）——等价主张范围虚宽 = 虚报风险。

**F9 · SHOULD · gap（A-7 供弹）· 双源 census：正则仅覆盖 1/4+ 版本化物**
全仓实测：*.v[0-9]+.md 命中仅 openclaw-system-prompt.v1/v2/v3.md；server/deploy/ 下正则不命中的版本化物 ≥4 类——engine-patch/（ARQ-260907-engine-deploy-patch-drift 承重补丁）、openclaw-patch/、OPENCLAW_SYSTEM_PROMPT_CARRIER（无扩展名）、provider.config.yaml/apply-xiaozhi-overlay.sh（接线面）。T2-1 登记表初始清单（:71）只有单路径 → engine-patch 类 add/add 落通用 blocked（fail-closed 方向对，但 S-1 假绿链在该族原样复发、无并集指引）。A-7 供弹：登记表须为 authority、初始清单 = 实测版本化物超集；V1-① 应补「engine-patch 形态 → 通用 blocked」反例夹具。

**F10 · COULD · T2-2「补三条」实列 ①②③④ 四条**（:72，T3 :79 按「T2 ④」两处四条引用）——计数笔误，canary 8 相邻文件增量口径应精确。

**F11 · COULD · charter 侧注记：canary 11 措辞失配**——enforce-scope.mjs（199 行）无 --self-check 入口，仅 deny-main-push.mjs:494 有；canary 意图（hook 零触碰）不受影响，按「文件零 diff」执行。

**F12 · COULD · question · S-4 类别标注 lint 可读性未定义**——T7「撰写时注释标注类别」（:103）格式未钉（无 guard-class: 类固定形态），VRT-E4 误伤防线全押自由散文注释；A-6 lint 化前须 human/architect 定格式。

## 二、B-0…B-8 断链判定（9/9）

| # | 判定 | 一句话依据 |
|---|---|---|
| B-0 | 半断链 | 三处成文（driver/§10a/T3）有引用锚、无防漂移对账点 |
| B-1 | 真断链 | launch 面无机械消费方；既有扩展点 portfolio-reconcile --check-launch（pod-coordination.md:42 先例）未被引用 |
| B-2 | 真断链 | 登记表 owner/更新时机/vN 回收全未名（「随批维护」四字） |
| B-3 | 真断链 | planner 派单义务 manifest 与留根两处皆无（=F7） |
| B-4 | 真断链 | Basis 匹配规则单侧声明，A-8 未裁前写作侧无从对齐 |
| B-5 | 真断链 | §10a:124 执行者身份未名；dev-loop.settings 仅 Pod 会话加载（:3-6），ask 弹给谁/谁裁授权在场无声明 |
| B-6 | 真断链 | defer eventId 锚格式未定义；gate 仅机械核 human-decision/approve-escalated-merge 五要素（gate :100-105），defer decision 值形态缺位，T5 无从实现 |
| B-7 | 半断链 | architect 已自我指出耦合，PROPOSAL 留根未登记——A-4 回填义务履约前为断 |
| B-8 | 真断链 | 8 实例「只登记不预批」后无发起者/触发时机/记录落点 = 无主工作项 |

真断链 6 · 半断链 2；B-1/B-3/B-6/B-8 比 charter 预估更锋利。

## 三、canary 12 条初判（12/12 独立重派成功）

全绿不碰：#1 五自检、#2 --theirs 取侧（:853 逐字在）、#3 Step 0（:813-815）、#4 kept-blocked（:925-930）、#5 gate 例外清单（:10-19/:136/:377-380/:249 全在）、#6 self-check 名单（:256/:312-334）、#8 §10a/§10b（:120-136）、#9 .gitattributes（:6/:12）、#11 hook 层（附 F11 注记）、#12 scope-map。
活碰撞 2 条：#7 settings deny 面不碰但与 V4 冲突（F2）；#10 off-main 求值主体——唯一被 PROPOSAL 候选口径自违的 canary（F5）。

## 四、A-3 事实层裁定

同形，成立。Step 1b = :1137 gateArgs 含 --project-dir projectDir + :1142 cwd: wt（findWorktree 失败 errorExit :1126-1128，无回退）；approvalRefUnlock = :919 probeWt = findWorktree(tp, projectDir) || projectDir + :920 同款参数 + :921 cwd: probeWt。两路径均「git 在 cwd(worktree) 求值、文件读在 --project-dir(主仓) 求值」；唯一差异 = 豁免路径回退分支（回退主仓 → off-main BLOCK 恒空转，fail-closed 无损）。ARQ 原文「常规路径无此形状」对照系失真确认。修复口径影响：事实层支持「维持两形、只改回退分支」侧；PROPOSAL T1-4 措辞与 V1-③ 预设另一侧，须按裁断回填。回退分支触发面真实（blocked Pod 恰可能缺 worktreePath）。

## 五、计数汇总与最重三条

- 13 findings = BLOCKING 3 + SHOULD 6 + COULD 4；B 系 9/9 判定；canary 12/12 重派；elicitation 3 项（A-8 方向 / GA-8 ask 张力 / V6 模板断言力差）。
- 最重三条：① F1——T6 要落地的 lib 路径已被 518 行承重单源占用而 PROPOSAL 全程按「新增」申报，landing 即撞名；② F2——V4 负断言被 deny 条目自身字面证伪，验证集含一个永不可过的断言；③ F3——T7/V5 必改的 ARCH-REVIEW-QUEUE.md 不在 manifest，锚集对账在 landing 时机判红。

## 六、未攻面

① enforce-scope/scope-map 对 ask/deny 的真跑探针未做；② T7 清单 CHR-E3 :467、live :683-685/:722-724 未逐行入文件复核（仅 P2-A6/VRT :116/防误伤 :294 三处亲核在位）；③ V6 模板 vs 夹具环境断言力差未深挖；④ DEV-GOVERNANCE §6 四元组表 integrator 行未逐项对账；⑤ pod-events 自由事件型与 human-deputy 批先例一致性未复读；⑥ S-1 owner 归属只到断点识别未逐格填满；⑦ T5 匹配与 CDQ 现有 2 行实形未核；⑧ escalation 批时序衔接（明确排除项未入）。
