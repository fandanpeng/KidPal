---
changeId: 261017-prompt-v11-schedule-recall
sources: 2026-10-17 human 原话逐字——① 「查一下服务端，我们给 Open claw KidPal 这个 workspace 里面都有哪些 skill？」+「刚刚测试了一笔，查后天的课程，结果他胡说：他说有美术课，其实没有美术课。」；② 「第一个肯定得把那个小智的 prompt 去掉，你看看我们这里边还有没有一些小智的东西夹进来，可能会影响我们的，这个先处理掉。」；③ 「第二点，把 kidpal workspace 下面其他那些没用的，除了那个联网可以去查天气的或搜索的留下之外，其他那些没用的 skills 都可以 disable 掉。」；④ 「然后，补全全链路识别。我就不用『只要提到课表，它就应该去读』，你先不要动手去改，先把怎么改，我们俩先对齐」；⑤ 对齐后确认「1. 同意，2 同意，另外，我们仓库里怎么保障和生产一致」；⑥ 「你先把调查结果记录 tech debt吧，后面再按流程处理」；⑦ 「这两个问题 走triage开始处理吧」。关联引用 = `TECH-261017-schedule-answer-skip-read-hallucination`（本批消费的主触发条目）、`TECH-261017-xiaozhi-template-residue-deploy-ssot-gap`（同批合入的支撑条目）。查重结论：`delivery/intake/` 在册 9 份无同意图 spine；debt 三域未结条命中上述两条——首次消费，走 change-intake §3a 晋升路径新立 spine。
faceMap: 需求面**不动**（课表识别扩列 = 实现细化；requirements.md S-F1.3 :279 能力句 + AC1 :282「明天有什么课」canonical 已覆盖「后天」语义，零改动、不产 §9 版本行）；架构面**动（轻面 docs-sync）**（architecture.md §5.5.5 :299「现状 = v10」+ §5.1 :197 v10 指针翻 v11，按 261012 等先例属 dev 包内 docs-sync，不路由 `/arch-change`）；契约面**不动**（display E1 / `schedule` view / B1 课表 MD 全不变，interface-contract.md §1.6 摘要已覆盖课表问答）；治理面**不动**（skills 收敛是 OpenClaw 运行时 `openclaw.json`，非仓内 `.claude/**` agent 定义 / gate）；实现面**动**（`server/deploy` 五件：prompt v11 四面接线 / KidPal 极简模板 + `prompt_template` 渲染 / `system_error_response` 覆盖 / `openclaw.json` kidpal skills 定点 patch / `manifest.yaml` 条目）。触面 = 2（实现 + 架构 docs-sync）→ 建本 spine。
facePlan: 架构 docs-sync 面 = **本会话内联**（主线程随实现包 docs-sync 步骤落，无独立门）；实现面 = 本门五档分档收尾，承载形态 **②单包 Lite**（architect 2026-10-18 裁，riskTier=medium、sourceType=regression、rigor=lite）。包内实施分段（单包内，非拆包）：A. prompt v11 bump 四面接线（carrier / provider.config 三键 / overlay PROMPT_SRC,DST / manifest 两条目 + `.config.yaml` marker 锚）；B. KidPal 极简模板新增 + `prompt_template` 渲染接线 + `system_error_response` 覆盖；C. `openclaw.json` kidpal skills allowlist 定点 patch（仿 perf-k1 声明+apply+drift_guard+backup+rollback）；D. 两 debt 条目结项（`consumed-by` 本 changeId）+ 架构指针 docs-sync。launch 前置：无上游面待冻（本批无需求/契约面），实现包可直接 launch。
anchors: {req: "不动（零新增需求语义）", arch: "待回填——architecture.md §5.5.5 现状指针 v10→v11 + §5.1 :197 同批；随实现包 docs-sync 段落地后由主线程回填", contract: "不动（零契约变更）", imp: "待回填——TaskPackage 261017-prompt-v11-schedule-recall 收口后回填（verifier/review-packet/supervisor verdict）"}
openQ: OQ-1（L0 主线程裁）changeId 日期前缀取 261017——与两条 debt id 同日对齐（调查与登记均发生在 2026-10-17），实际评审日为 2026-10-18；不另立。OQ-2（**已解，主线程实读引擎源码**）`.config.yaml` 的 `prompt_template` 键与既有 `prompt:` 键是**包裹关系不是二选一**：`core/utils/prompt_manager.py::_load_base_template` 以 `config.get("prompt_template")` 取模板路径（缺省回落镜像内置 `agent-base-prompt.txt`），`build_enhanced_prompt` 以 `config["prompt"]` 作 `base_prompt` 渲染进模板的 `<identity>` 位——新模板据此只留 `<identity>`+`<context>`+中文约束即可。附带发现引擎缺陷：`build_enhanced_prompt` 传 `current_time="{{current_time}}"` 字面量，模板写该变量会被渲染成字面串，新模板不写该行（日期/星期由 `today_date`/`today_weekday` 承载）。builder 实现期仍须实测一次渲染产物。OQ-3（L0 留待 builder 执行）`.vN` 槽位冲突检查——主线程已核 `server/deploy/` 无 v11 占用（2026-10-18），bump 前再核一次。OQ-4（L0 记录）skills allowlist 取 `["kidpal-schedule-answer","weather","tavily"]`——human 原话「除了那个联网可以去查天气的或搜索的留下之外」对应 weather（bundled）与 tavily（extra）两项，其余 16 项从 kidpal agent 视角移除；main agent 不受影响（agent 级 allowlist 显式替换）。
status: in-flight
---

# spine · prompt v11 课表召回 + 去小智模板残留（261017-prompt-v11-schedule-recall）

> 触面 ≥2 跨面变更的路由账本（[change-intake.md](../../.claude/references/change-intake.md) 程序；spine 免冻——各面冻结语义由各面制品承载，anchors 记录冻结事实）。

## 建档前查重（change-intake.md §3）

- **既有 spine 排查**：`delivery/intake/` 在册 9 份逐份按意图与触面比对——无同意图件。最近的 `261012-kidpal-realtime-facts` 是**天气域**实时事实回合（已 in-flight、结论已入 v10），与本批的**课表域识别召回**不同域、不重复建档。
- **debt 三域排查**：命中本批要消费的两条（`TECH-261017-schedule-answer-skip-read-hallucination` MAJOR / `TECH-261017-xiaozhi-template-residue-deploy-ssot-gap` SHOULD），二者 `handoff` 字段本就互指「同批走部署面 loop」——按其走 §3a 晋升路径：`sources.refs` 引两条 id，收口时翻 `closed` + 正文结项行带 `consumed-by: 261017-prompt-v11-schedule-recall`。

## 立项 ack（change-intake.md §4 · 唯一必停人类触点）

四面对齐「为什么改 / 往哪改 / 改动多大」→ **human 2026-10-18 已确认 ②单包 Lite**（触点①，architect 裁决未被改判）。

- **为什么改**：真机问「后天都有什么课程？」答出课表里不存在的英语/美术；audit 证该回合零工具调用，而同一 prompt 同路径 loopback 复现 9/9 次都正确读取并答对——即「该读没读」是概率性失守，根因在识别特征枚举窄（只写今天/明天/这周）+ 引擎通用小智模板与 v10 对冲（「工具不匹配就直答」）。
- **往哪改**：① 课表识别特征扩为「凡提到课表/课程/上课/要带什么，以及任何日期时段表述（今天/明天/后天/大后天/周X/星期X/下周/几号）」；② 用 KidPal 极简模板替换通用小智模板（保留 `<identity>` 与 `<context>` 日期天气注入，删工具语义/emoji/说话人识别/反 AI 味）；③ `system_error_response` 覆盖掉「小智现在有点忙」；④ kidpal agent skills 收敛为 3 项；⑤ 五件全部接入 `server/deploy/manifest.yaml` 单一事实源。
- **改动多大**：②单包 Lite（单 owning 子系统 `server/deploy`，一轮收）；riskTier=medium；无需求/契约/治理面变更，架构面只翻 §5.5.5 现状指针。
- **落盘**：[delivery/taskpackages/261017-prompt-v11-schedule-recall/plan.md](../taskpackages/261017-prompt-v11-schedule-recall/plan.md)（reviewed plan，含 changeManifest 与 impactedTests 种子）。

## 触面事实（查出来的，非申报）

| 面 | 动不动 | 动哪 | 依据 |
|---|---|---|---|
| 需求 | ❌ | 零改动——课表识别扩列属实现细化 | requirements.md:279（S-F1.3 能力句）/ :282（AC1「明天有什么课」canonical） |
| 架构 | ✅（轻，docs-sync） | §5.5.5 现状 = v10 → v11 + §5.1 :197 v10 指针 | architecture.md:299 / :197；先例 = 261012-kidpal-realtime-facts（dev 包内 docs-sync） |
| 契约 | ❌ | 零变更——display E1 / schedule view / B1 全不动 | interface-contract.md §1.6 |
| 实现 | ✅ | server/deploy 部署面五件（见下） | 两 debt 条目的建议改动节 |
| governance | ❌ | 零命中——skills 是 OpenClaw 运行时 config，非仓内 agent 定义 | 不触 `.claude/**`、不触 gate 登记 |

## 档位裁定（architect 2026-10-18 比例化 review）

- **②单包 Lite**，riskTier=medium，sourceType=regression，rigor=lite，`reviewed-by: architect`，date=2026-10-18。
- 非 ①：改实现面行为。非 ⓪nano：排除清单命中（`openclaw.json` secret 承载件安全轴相邻 + 多文件面 + 验收语义净增）。非 ③：单 owning 子系统、两 debt handoff 本就合批，拆包会造成 manifest/overlay/provider.config/carrier 四个共享文件被两包同时认领。非 ②Full：riskTier=medium，非状态机/真 DB/迁移/多包；`openclaw.json` 只增 skills 白名单、不动 gateway token（perf-k1 先例已立）。
- **host 齿可闭环**：零固件面，server pytest + loopback 探针 + `verify-deploy.sh` 三段可闭环，无设备编译 seam → lite 快路径适用。
- **逃生门**：无需 architect-reviewer 四分类（契约面零变更、无 agent 定义/gate 改动）。
