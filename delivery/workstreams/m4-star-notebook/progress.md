# progress · m4-star-notebook

> 执行日志（append-only）：改了什么、测了什么、失败/修复。行不覆盖。

## 2026-09-24 · 拆包（主线程，portfolio-planner 纪律）

- 输入 = `delivery/milestones/milestones.jsonl` 行 `M4-star-notebook` nextAction 两块（sourceType=delivery-story）；story 机查两道通过（S-F2.1/S-F2.2/S-F2.3/S-F2.4 无 §8 已砍命中、可行性全 ✅——S-F2.3 朗读砍除后纯板端渲染 host 已验、S-F2.2 manual 聆听+网关累积态已备）。
- readiness 调查（Explore 子代理 + 主线程磁盘核对，图索引 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`）关键事实：
  - 板端 `story_page.c` 四态渲染在世（260829-story-p3 产物、FROZEN OFFLINE CONTRACT 头注）但**页内输入表为 v1.27 前旧口径**（KEY CLICK=光标下移 / KEY 长按录音态=丢弃——均与 v1.47 矩阵相反）；P3 页内按键设备侧**只接了 BOOT 长按一条缝**（`waveshare-s3-rlcd-4.2.cc:674-677` HandleStoryKey 唯一调用点）；列表态 300ms 单双击消歧**无机制**。
  - **`story_page.c` 全文件 CJK 字体零引用**（LVGL 默认 montserrat_14 纯拉丁——P3 列表/阅读/空态中文真机很可能空白；char_window_overlay.acceptance.c 有 LV_FONT_DEFAULT 负向齿先例）。同族 `home_upcoming.c`/`reminder_page.c` = M1 面，挂账不入本批。
  - `kidpal_session_ctx` 契约已 LANDED（CDQ-260914）但**双端实现零**（固件无发送原语、front 无拦截）；arch to-be ⑥ 接线点在案（application.cc 会话发起入口读 layer_top / front 上行泵帧解析处）。
  - front 故事接线半条：control/start_recording → StoryAccumulator.start、stt 帧 → feed、listen-stop → stop 归档 + story_text 下发**在世**；`list_stories()` **零生产调用点**（story_list 从不主动下发——「进页即见列表」无数据源）；收尾词/120s 超时/删除执行面/流水号账本**全缺**；盖章 N 现 only 活在易失 `stamp_index`（重启即失，S-F2.4 AC2「只增不减」不成立）。
  - 覆盖层基建齐备（layer 栈 + begin/end 夹钳 + dismiss_front_overlay + 查字窗 idle-step 5min 先例）——删除确认弹窗可复用基建，渲染/裁决/10s 超时需新写。
  - art 资产集内 `ic_star_48/38/22/20/18`、`stamp_seal_r8`、`rec_wave`、`pose_jump` 在世；列表行现用 `●` 与方块堆（非星星图形）。
  - prompt 现役 v6：故事意图**只有「我要讲故事」单指令**（「帮我记下来」全文零命中）——v7 bump 归 K3。
- 拆 3 包（M3 镜像切法，D1）：`260924-m4-k1-story-cloud-link`（mergeOrder 38）/ `260924-m4-k2-p3-star-notebook-ui`（39，dependsOn K1）/ `260924-m4-k3-record-session-live`（40，dependsOn K1+K2）。
- 重叠面核查结论（详 decision-coverage `_doc`）：story_p3 旧冻结验收板/云双侧已 archive 退役（新齿不走 supersede；story_page.h FROZEN 头注随 K2 重写声明退役口径）；在办零冲突（m3-k7 已合并收口 9-23、perf-k1 merged、audio-bargein-hw queued 休眠音频面）；prompt v7 槽位空闲；M1 基建（键路由分层/键帽条三槽/语音轮四态/覆盖层）全 merged 为下游消费非冲突。
- **契约触点裁定：触契约面三触点**——① §1.3③ story_list `total` 语义拆分（旧「已录总数=故事果计数」兼盖章 N ↔ v1.47 计数=现存篇数、盖章 N=流水号只增不减）+ 板子行为句旧交互（「KEY 选择」）改写 = spec↔contract 漂移；② §1.3④ story_text 增 `seq`（additive，盖章 N 承载）+ 行为句「或仅靠 TTS 朗读」清理（v1.52 砍朗读）= 漂移；③ 删除云端同步（S-F2.1 AC4 / arch §5.4【链路载体待契约】）= 契约新增载体（拟甲案 `kidpal_story_delete` §四私有消息，D2）。三触点合一 CONTRACT-CHANGE-PACKET（草案落 K1 包目录），走 architect-reviewer 四分类预审 + 人类契约门，diff 随 K1 同 PR（launchPreconditions 第 1 条）。`kidpal_session_ctx` 契约既有零改动、只落实现。
- 真机半安排：K1/K2 的 live 半全部 structural deferred 挂 K3 设备轮（单真机不两轮撞车，M2/M3 先例）；K3 live 确认轮一次性目击全链（双入口开录→收尾三式→归档→盖章 seq→列表+1→阅读→删除→云同步→重启流水号不变）。
- 四件套（task_plan / roadmap / progress / decision-coverage）+ 三包 taskpackage.json/plan.md + K1 CCP 草案 + 双 input-brief（brief-inputs.mjs 拆包侧同批产物）落盘；roadmap ② 节 render-roadmap.mjs 机械渲染；board 预填三 pod queued（portfolio-reconcile.mjs 单一写者纪律，pods/*.json → board.json）；milestones.jsonl M4 行 planningRefs 挂接（state 保持 planning——翻 in_delivery 归 human 过目/首包 launch 时点，注册表纪律 state 由人/integrator 维护）。
- 待办（本相位后）：门 1（architect 轻量 checklist）+ 契约预审（architect-reviewer）并行派发 → 修复 → human 两判断点（过目切法 + 契约门裁 CCP 五问）→ 常驻分支同步 main（现 behind 8）→ commit-before-launch + `base/m4-star-notebook`。

## 2026-09-24 · 契约预审 + CCP v2（主线程派发 architect-reviewer，同日）

- **契约预审（architect-reviewer 四分类）**：裁定 **NEEDS_REVISION**——方向正确、Alternatives 可信（删除甲案与 ADR-005「无 device RPC 先例」边界一致性核实），修订清单 7 项 + 漏问点 1–7。四分类拆标：触点① = spec↔contract 漂移；②a seq = **独立契约变更**（spec 只钉 N 语义不钉载体）/ ②b 朗读句 = 漂移；③ kidpal_story_delete = **独立契约变更**（CDQ-260914 判据一致先例）；§1.4 control = 无需改。初版「三触点一律随 spec 携带」误标已撤。
- **漏问点实质三项（预审批评的核心缺口，主线程对磁盘逐条核实全属实）**：① **16-cap 硬冲突**——板端 `KSTORY_LIST_MAX=16` 超限整单 -1 降级（story_page.h:40 / story_page.c:556-564），初版「story_list 全量载荷建连即推已覆盖」断言与 >16 篇实况矛盾（Q2 三案，拟乙案 K2 抬 cap + front 超 64KB 载荷截最新 N + total 恒全量）；② **真双源窗口**——v6 prompt 仍指示 LLM 产 story 块且 OpenClaw 无 story 工具（臆造），初版「无双源漂移」断言撤回（Q7 拟 front relay 收紧 story_* = 机械推送单源）；③ **删除无应答可靠性**——delete 帧丢失 → 重推复活（Q5 拟隐式 ack + 超时重发，不破 ADR-005 边界）。另有 seq 缺省降级演出（Q4）、硬删/软删与家长通道（Q6）、display_schema story floor（Q8，现状零形状 floor「收紧」表述失真已修）、§1.6 story 段（Q9）、术语统一（Q10）。
- **CCP v2 落盘**（同日主线程修订并入）：§0 四分类裁定表 + §1 钉行收口面表（story.py:12-13/:181-183/:211 + v6 三处钉行清单）+ Impact 补旧板端盖章面注（stamp_index=counter+1 story_page.c:1071）+ Human Question 五问扩**十问** + 收口义务补 pass-oracle 人工 diff 核对清单。连带修订：K1 plan.md 取舍点 A/E/F + 阶段划分；K1 taskpackage A1/A3/A4 + notes 预审记录；K2 plan.md 隐式 ack + Q2 cap 依赖条；decision-coverage _doc 契约面裁定条 + LP1；review.md 契约预审节（耐久记录）。
- **门 1（architect 轻量 checklist）**：已并行派发，报告待回——回报后修复落 review.md 门 1 节，再停等 human 两判断点（过目切法 + 契约门裁十问）。

## 2026-09-24 · 门 1 GREEN + findings 吸收（主线程派发 architect，同日）

- **门 1（architect 轻量 checklist）**：首轮 **verdict: GREEN**——checklist 五项全过、实现现状主张 a–g 七条磁盘抽查全属实、无 blocking finding；advisory 4 + nit 5（F1–F9）。裁决耐久记录 = review.md 门 1 节（含处置表）。
- **findings 同日吸收**：F1（K2 docsSyncReason 补 S-F2.1 AC3 单指令陈旧口径清扫）/ F3（K1 docsSyncReason 补 arch §7 在办① M3-K1 遗留漂移同笔核销）/ F4（K3 plan protocol 枚举修正）/ F5（K3 docsSyncReason 补 §5.5.5 正文 v4 双重漂移注记）/ F7（K2 docsSyncReason 补对账⑤ P3 阅读页内实例点名）——五处文本修订落盘；F6（workstream 级 crossPackageBehaviors 补第 7 条「删除发送原语复用」）已修；F2（m3-k7 pod/board 协调态陈旧 blocked vs 实已 merged——git 实证 main 0fe04bf merge + 73fa08b pod-retro）已按单一写者纪律翻 merged + portfolio-reconcile 重算（43 pods；迟翻原因 = 260923 人工 ff 合并未回写协调态，如实留痕）；F8（brief 契约锚 §1.3③ not-found = 脚本锚模式与「#### ③」标题实形不匹配）留观不改——brief 诚实标注无实害，脚本改进候选登记留 loop-retro 议；F9（K1 合并门对表 gate evidence）在案复述。
- **input-brief 再生成**：taskpackage 修订后三包 × 双角色六份重跑 brief-inputs.mjs（0 warnings 保持）。
- **停等 human 两判断点**：① 过目切法（门 1 GREEN 已在案——三包拆法 + mergeOrder 38/39/40 + D1~D8 取舍点 + workType 分流）；② 人类契约门裁 CCP v2 十问（Q2 items 上限三案 / Q5 删除可靠性 / Q6 硬删软删与边界 / Q7 双源窗口为实质裁点，其余六问拟答低争议）。commit-before-launch（常驻分支先同步 main behind 8 + base/m4-star-notebook）在两判断点后执行。

## 2026-09-24 · human 两判断点落定（同日）

- **过目切法**：human 认可三包拆法（门 1 GREEN 在案后过目）。
- **契约门 HUMAN_APPROVED 十问**：唯一偏差 = **Q2 改判丙案**（最新 16 条窗口 + total 全量、板端 cap 不抬；知情注记含旧条目可达性收窄，CCP §5 逐字）；余九问按拟答（Q5 甲案+隐式 ack 重发 / Q6 硬删+审计+边界仅 story MD+家长通道挂账 / Q7 front relay 收紧 story 块 / Q8 display_schema story floor / Q9 §1.6 story 段随 diff / Q1/Q3/Q4/Q10 认可）。裁定落账 CCP §5 + review.md；传播 K1/K2 plan + acceptance + decision-coverage（LP1 ruling-approved / LP2 verified）。
- 待办：常驻分支同步 main → commit-before-launch + base/m4-star-notebook → 契约 diff 落地（逐字 patch human 过目 → git apply + 收口义务同批）→ K1 launch prep。

## 2026-09-24 · 契约 diff 落地（主线程，human 逐字 patch 过目批「落」，K1 launch 前）

- **落地通道**：CCP v2 §5 十问 HUMAN_APPROVED → 主线程起草逐字 patch（12 处替换全部 assert 单命中；CRLF 以工作树实态生成——i/lf w/crlf 实测，git apply --check 先行绿）→ patch 全文贴示 human 过目 → 批「落」→ `git apply` 两文件 cleanly（+24/−14 与 --stat 自证一致）。
- **契约 diff**：§1.3③（标题「星星记录本列表」+ 行为句 v1.27 交互 + total=现存篇数 + items 最新 ≤16 窗口条〔Q2 丙案〕+ 变更记录行）/ §1.3④（标题「刚存好/阅读态」+ 示例 seq + 行为句〔朗读清理 + fresh_save 盖章 N=data.seq〕+ seq 条 + 变更记录行）/ §1.5-3（story floor + relay 收紧句）+ §1.5-6（故事面机械职责新条）/ §1.6 模板 story 段 / §四 `kidpal_story_delete` 新条目（隐式 ack + 硬删 + 边界仅 story MD + 家长通道挂账）。
- **随包收口义务同批完成**：story.py 三处钉行更新（doc 零行为——pytest `134 passed / 4 deselected / 0 failed` 实证）；CDQ 三行登记（CDQ-260924-m4-k1-storylist-semantics / -storytext-seq / -story-delete，均 LANDED）；ADR-007 追加（seq 账本 + 16 条窗口丙案〔含 Q2 改判备选记录〕+ 删除单向消息/隐式 ack + 单源收紧，ADR-005 边界逐字对齐）；gate evidence 九节落盘（`architecture-gate-evidence.md`，条件 C1–C5 挂 K1/K2/K3）；evidence 四样本（`evidence/validate-display-samples.md`：S1–S3 正 + S4 delete 伪装 display 负样本白名单拒绝；如实注记 Q8 floor 归 C1 未实现）；人工 diff 逐字核对清单过（pass-oracle 第 4 关降级如实记录，ARQ-260827 在册）。
- **LP1 → verified**（decision-coverage 落账）；**LP2 verified**（门 1 GREEN + human 过目切法）；LP3 = 拆包 commit f05684d + base/m4-star-notebook（rebase 后常驻分支 = main + ops-record c325e89 + 拆包 f05684d + 派生再生 fd67c18）——本落地 commit 为其后继，K1 intake 应将契约 diff 计入基线（M3 同款注记：redBaseline 面无行为变化）。
- **待办**：K1 launch prep（check-launch / launch-card / 图索引新鲜度 / 环境等价化 / 基线重锚注记）→ launch 由 human 执行；arch §5.2④/§5.4/§5.1.2/§7①②④收口 = K1 integrator docs-sync（C4），不在本批。

## 2026-09-24 · launch 面就绪（本批 = 单包 K1）

**调度判定**：本批 = 单包 K1（mergeOrder 38）。依赖图说了算——K2 dependsOn [K1]、K3 dependsOn [K1,K2] 均未就绪；无 running Pod、mergeToken holder=none。常驻树串行接力续棒（上一棒 m3-k7 已 merged 收口 9-23、pod 态陈旧已翻正；perf-k1 走专树已 merged），同树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`、常驻分支 `dev/260915-m1-k1a-key-layer-dispatch`（rebase 同步 main 完毕：behind 8 清零，链 = main 5cf067f + ops-record c325e89 + 拆包 f05684d + 派生 fd67c18 + 契约落地 5e862d0）。

**commit-before-launch（F-03/F-05）**：拆包制品 commit f05684d、专用 base ref = `base/m4-star-notebook`；契约落地批 5e862d0 与本 launch-prep 批为其后继，base ref 随本批快进（仍含全部拆包制品；check-launch 的 launchPreconditions 读侧 = base 上 decision-coverage，故 LP 形状修正〔canonical {text,scope,status,evidence}，260921 T6 形态〕须 commit 后机械门才见——修正前首跑 ok:true 但 LP summary 读旧形 5 pending，修正后复核 5 verified + LP4 属 K3 折叠）。

**上游已知红基线（260909 T5 机械来源口径，本树现跑 @ 5e862d0，2026-09-24）**：
- **server 面**：`python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/acceptance"` → **80 passed / 0 failed / 4 deselected**（面域 = server；全套 offline 含 server/tests = 134 passed / 0 failed 同日佐证。锚 SHA = 5e862d0）。零红。
- **firmware host 面**：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash <repo>/firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf` → **103 Tests / 0 Failures / 0 Ignored，exit 0**（面域 = firmware；与 m3-k7 期 103 锚零偏移——契约落地批 firmware 零触碰实证。锚 SHA = 5e862d0）。零红。
- Pod intake 按 260915 T3 差异判停协议消费本段：`git diff --quiet 5e862d0 HEAD -- server/`（或 `-- firmware/`）零差异 → 引用本行不重跑；有差异 → 同命令重跑该面。

**环境事实**：图索引复用树既有项目 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`（index_status ready，18040 nodes / 48690 edges，skipped 0，随树 watcher 增量自刷新）；固件环境等价化沿用树内既有 sdkconfig / dependencies.lock / managed_components 软链（host 构建刚实证 103T/0F，跳过重建）；board worktreePath 已预填本树（--set-pod queued + reconcile 43 pods）。

**launch 启动卡**：已落 `delivery/taskpackages/260924-m4-k1-story-cloud-link/launch-card.md`（五要素齐；含 intake 特别注意 = 契约已先行落地、Q7/Q8 执行面归本包 C1）。双 input-brief 在场核过（拆包侧产、launch 侧核，F1/F3–F7 吸收后已再生成 0 warnings）。

**launchPreconditions 回写**（launch 执行者核验）：LP1 verified（CCP §5 + gate evidence + 5e862d0）；LP2 verified（review.md 门 1 GREEN + human 过目 2026-09-24）；LP3 verified（base/m4-star-notebook，本段 commit-before-launch 注记）；LP4 = K3 真机面 unverified 保持（K3 launch 前再核）；LP5 verified（v7 槽位空闲 / archive 双侧在案 / 层栈 append-only 授权 / 回归靶声明）；LP6 verified（index_status ready 本段）。

**遗留 human 项**：launch 由 human 执行（交互会话形态默认）——本 skill 不代跑 launch，命令行两形态见启动卡第五要素。milestones.jsonl M4 行 state planning→in_delivery 建议随 launch 同笔落账（注册表纪律 state 归 human/integrator 维护，拆包侧未代翻）。

## 2026-09-24 · K1 Pod 开跑（planner intake，human 交互式 launch）

- Launch 形态 = 启动卡命令行①（人在场交互式），池树 + 常驻分支 `dev/260915-m1-k1a-key-layer-dispatch`（board worktreePath 预填本树，池树复用纪律一致）；Step 1① portfolio handoff 四处探测全命中（taskpackage / board 条目 / workstream 四件套 / launch-card），F-09 不触发；board queued→running（portfolio-reconcile 43 pods，mergeToken holder=none）；current/ 投影按起包 checklist ⓪ 字节级刷新（旧投影指向 perf-k1：taskpackage.json + 双 brief 三件 cp）。
- 基线锚按启动卡差异判停协议消费：`git diff --quiet 5e862d0 HEAD -- server/` 与 `-- firmware/` 均 exit 0 → server 80 passed / firmware host 103T0F（@5e862d0）引用不重跑。家谱注记：5e862d0 = `base/m4-star-notebook` 上的契约落地批；常驻分支 rebase（main cf9db09）后同义 commit = 3a9ff24。
- 上一包留根 perf-k1 pod-retro 八节逐条评估：**无 blockingDecision**。承接纪律：派发书三条硬约定 / ErrorEnvelope 路由先判 failingStep 属谁的面（元数据·投影·文档面归 planner，实现面才归 fixer）/ builder 重头形态 opus 升档 / dispatch-stamp 绝不传 0 充数 / 向 human 汇报首段白话结论。
- Step 7 保鲜判定：双 brief generated-at 09:59 早于契约落地（refs `server/gateway/story.py` + `docs/contracts/interface-contract.md` 其后被 5e862d0 改动）→ 重跑 `brief-inputs.mjs` 双角色（0 warnings / 10 条无锚 advisory = 门1 F8 留观同族，不阻断）。1f 四问核验过：acceptanceTestsFile=[]（新建包齿未冻，notes 在案）；红基线命令面在世（archive 退役齿仅只读引用）；cwd 基准正确（仓根=池树）；sourceRefs 覆盖真实改动面（front_app / story / display_schema + 契约 + spec + CDQ + deploy README）。
- **C4 arch 收口通道核定（结构性事实，非阻断，处置在案）**：docs-sync 机械闸现绿（docsSyncReason 在场 → mention 检查走 reason 分支；诊断实跑 ok:true exit 0）。但 `docs/architecture.md` 对 pod 全角色不可写（scope-map 白名单：integrator allow 不含、planner deny docs/**；该文件写入史 = 治理批 landing 通道专属）——ARQ-260918（deferred，owner=architect+主线程 spec 批，「dev-loop 角色对 docs/** 无写权、不代改」）同族在案。处置：integrator 把 C4 delta 以 patch 形态落 `docs-delta.md` + review-packet 申报未覆盖面，合并后由 human spec 批落地（ARQ-260918 四处可同批搭载），gate evidence C4 届时才翻 done；合并报告向 human 明示。同面簿记待办：milestones.jsonl M4 行 state planning→in_delivery（注册表 state 归 human/integrator 维护，pod 内无写权路径）。
- 派 acceptance-author（冻 M4K1-A2~A6 齿 + C1 机械执行面〔Q7 relay 收紧 + Q8 story floor〕承载齿；红基线恒现跑；mutation 第一方记录落 `server/acceptance/` notes、taskpackage 回填件返 planner 代落）；等待期 planner 按 Step 1e 通读 requirements F2 全节 + §2.1（行为权威，供 Step 8 finalize 与 8b 验收方向核 / AC 回引对账）。

## 2026-09-24 · K1 冻结相位（planner：acceptance-author 交付 → Step 8 finalize）

- **冻结件落仓**：`server/acceptance/test_m4k1_story_cloud_link.acceptance.py`（26 齿 = 21 设计态红 + 5 现状绿 must-stay，1269 行，sha256 `61c0501…49cc26` 回执与磁盘一致）+ `M4K1-ACCEPTANCE-NOTES.md`（§1 builder MUST 清单 / §3 逐齿映射 / §4 mutation 第一方实测〔MUT-1~4、6、7 运行期扰动 apply→还原；MUT-5 + §7.3 实现级变异声明待转绿后重放〕/ §5 缺口与自由度 8 条显式化 / §6 强度申报三件套 / §8 canonical JSON）。A7 不冻齿（structural deferred 归 K3，notes 声明）。
- **红基线落账 + planner 第二方复核**：`21 failed / 139 passed / 4 deselected`（acceptance-author 实跑逐字；139 = 上游锚 134@5e862d0 + 5 现状绿齿）；planner 同命令复跑 **21 红名与回执逐条一致**、收集零错误（164 项，冻结即冒烟过）。
- **Step 8 finalize**：taskpackage.json 回填三字段（acceptanceTestsFile / redBaselineResult 逐字 / mutationEvidence.perAcceptance ×6——acceptanceId 与 acceptance[] 原串程序化核对一致，从 notes §8 fence 直取免手抄）；current/ 投影原子 cp 刷新（sha256 字节绑定纪律）；mutation-check 诊断绿（functional A1–A6 / structural A7 豁免）；docs-sync 诊断绿（intake 已验）。双 brief 在场（本 Pod 已重生成，refs 未再变不重跑）。crossEnd counterparts 三件在场。
- **锚 2 · 验收方向核（8b）= PASS**：roadmap 本包效果段回指（S-F2.1 AC1 数据源半→A3 齿 / AC4 云端同步半→A4 齿；S-F2.2 AC1–AC2 云端累积/收尾半→A2+A5 齿；S-F2.4 AC2 流水号账本半→A6 齿）与 notes §3 逐齿映射对得上，无多引无少引；新建型 workType 按 AC 回引对账口径核过；requirements F2 全节 + §2.1 已通读（1e），齿断言与行为权威一致（120s 归档不丢弃 / 一刀切纯记录 / 计数=现存篇数 vs 盖章 N=流水号两数分离）。
- 簿记：dispatch-stamp（phase=acceptance，harness 遥测 62 tool-uses / 1449s / token exact）+ over-scan friction（申报 17 行，代表 = 两行超先例封顶）已落 pod-events。
- 派 builder（**opus 升档**——重头形态：验收面从零钉 + story 云端机械面首次实现；绿定义三条 = 全套 offline 零红〔21 齿全转绿〕+ §8 mutation 全套重放第一方原始输出〔落盘责任人 = planner 代落 evidence/〕+ 冻结件 sha256 不变自检）。

## 2026-09-24 · K1 实现相位 + tighten-only §6 修订（planner）

- **builder 交付**（派发显式 opus；遥测派生实际生效模型 deepseek-v4.1-flash，如实留痕）：三面实现（story.py +261 / display_schema.py +24 / front_app.py +149−21）+ 新增单测 22 条；26 齿中 25 绿 + **Red Flag 即停上报**——`test_a6_restart_survival` 冻结齿 fixture 缺陷（start 后缺 feed，空累积下 stop→None 依冻结既有语义，齿不可满足、两向皆红），未自裁改齿凑绿。FS-EVIDENCE-1 过（diff 全在 server/**：三实现文件 + 单测）；planner 第二方复核 Red Flag 成立（单测复跑 `unexpectedly None` + archive 先例 test_story_p3:492-493 empty-stop→None + E1 空转写 ValueError 三方印证）。
- **ESCALATION-BRIDGE**：agent-escalation `c1096ab9`（ambiguous-acceptance / blocking / action=stop）→ 重派 acceptance-author 走 DEV-GOVERNANCE §6 tighten-only「修自己 fixture 缺陷」路径（非必升面，环内自主）→ agent-escalation-resolved `1be9997e`。
- **§6 防滥用五件逐件过（planner 机判复核）**：① `git diff -U0` 删除行 = 0（三 hunk 全 +、共 34 行；REVISION LOG 块刻意置于 `__main__` guard 前规避 EOF 换行正规化，v1 末行逐字不变）；② 原变异（mutationEvidence A6 条 mutation）重放**转红实测 `3 != 4`** + 还原复绿——fixture 采纳 planner 派发预警的删除步判别形态（seed 3 → delete 1 → restart → feed → stop → seq=4：账本派生 4 vs 计数派生 3，可判别）；③ 三件记录齐（冻结件 REVISION LOG 带 Reason + sha256 核销链 `61c0501…→正文态 1be76af9…→交付形态 5d4ebbf2…` 分记〔块内自指不可解，本仓首例核销口径〕+ notes §9 十节）；④ 范围帽（同冻结件 + 同 notes、零新建文件）；⑤ 意图锚定（assertIsNotNone + seq==4 断言本体逐字不动、钉 M4K1-A6 原意图，非 re-spec）。notes 侧唯一「删除行」= EOF 换行正规化伪影（v1 行逐字保留 + v2 行追加，append-only 合规审计过）。
- **canonical 连带更正**：mutationEvidence A5/A6 按 builder 重放实测更正——expectedRed 多声明 2 条移出至 `expectedRed_notSensitive_advisory` 字段（a5_front_wiring = 源锚代理齿、子串判据 docstring 提及即满足、对实现变异不敏感，本次不收紧=范围帽，收紧评估归 loop-retro 素材；a6_rearchive = 计数派生偶然保绿）；A6 expectedRed 保留 4 条（restart_survival 修订后对原变异转红实测、不再属多声明）；taskpackage.notes 追加 §6 修订记录；current/ 投影原子刷新；mutation-check 诊断绿。
- **修订后全套 offline = 182 passed / 0 failed / 4 deselected**（acceptance-author 4.46s + planner 复跑 132.93s 双复核；132s = 两子代理收尾窗口机器负载波动、非闸面事实，retro 观察项）。构成 = 上游锚 134@5e862d0 + 冻结 26 齿全绿 + builder 单测 22。
- builder 首跑 mutation 重放 6 组 + MUT-5 第一方原始输出在案（A6 组因缺陷齿未闭合、2 条 advisory 已收进 canonical）；**重派 builder 闭环**：复跑绿定义 + 对修订齿重放全套变异（corrected expectedRed 口径）+ 三条自检，回执后 planner 落 evidence/ 并进 verify scoped 轮。

## 2026-09-24 · K1 integrator 收口（scoped r1 七闸全绿 → closeout 制品 → commit 冻结）

- **scoped verify round 1**：`verifier-evidence/rounds/r1.json`（bearing、producedBy=verify.mjs、observedGateExit=0、ok:true）七闸全绿（taskpackage-required / firmware-build〔真编译 build-pass、`xiaozhi.bin` 0x30e840B、本包 firmware 零触碰〕/ contract-lint / acceptance-coverage〔A1–A6 functional、A7 structural 豁免〕/ mutation-check / docs-sync / security）。**final 相位 `verify.mjs --phase=final` 本批未跑**（派单未含 final round 与 invocationId，避免造伪 round fact）。
- **全量复跑**：`python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance` → **`182 passed, 4 deselected in 4.15s`**（exit 0；构成 = 上游锚 134@5e862d0 + 冻结 26 齿全绿 + builder 单测 22）；firmware 面 diff-stop 已验零触碰（不重跑 host 齿）。
- **closeout 制品两件（blocking）落 `delivery/taskpackages/260924-m4-k1-story-cloud-link/`**：`docs-delta.md`（含 C4 arch 收口 patch 形态 delta 五处 hunk）+ `review-packet.md`（含 demo 对账行 = **一致** + 未覆盖面申报 15 条〔含 none 明写〕）；`closeout-check.mjs` 默认只读 exit 0。
- **C4 arch 收口（结构性事实）**：integrator 无 `docs/architecture.md` 写权（scope-map `agents.integrator.allow` 含废弃 `docs/systems/**` 却无 `docs/architecture.md`）→ delta 以 patch 形态落 `docs-delta.md §4`（§5.2④ `:213` / §5.1.2 front 网关行 `:185` + 故事归档行 `:192` 连带 / §5.4 故事行 `:261` + `.seq.json` 行 / §7 在办 `:341`），合并后由 human spec 批落地（ARQ-260918 四处可同批搭载）；`architecture-gate-evidence.md §6` C4 保持 pending + 处置注记、C1 翻 done（证据 = 冻结齿 A1 组 + MUT-A1 组 + 本批收口 commit）。
- **docs-sync 可写面**：本 progress.md append 收口节（append-only，不改既有行）；`docs/ADR.md` 零改（ADR-007 已在、本包实现与之一致）；契约面/需求面/架构面本批零写入。
- **commit 冻结批**（分支 `dev/260915-m1-k1a-key-layer-dispatch`，未 push/未 merge）：实现四件（`story.py` / `display_schema.py` / `front_app.py` / 新增 `server/tests/test_story_units.py`）+ closeout 制品 + pod 簿记面（`evidence/`、`verifier-evidence/`、`verify-runs.jsonl`、`pod-events.jsonl`）+ gate evidence C1/C4 注记 + 本 progress 节；`changes-index.mjs --upsert 260924-m4-k1-story-cloud-link` 钉 dev INDEX 单行（生成件勿手改）。
- **结构性发现去向**：scope-map integrator allow 与 AGENTS.md「docs 平铺」不一致（C4 类义务结构性不可直达）→ review-packet Findings #1，去向 = loop-retro / human（**不自登记 ARQ、不动 `docs/crq/**`**）。
- **等待 supervisor 机械合并门**：integrator 不 push、不 merge；supervisor APPROVE + 前置升级闸（`pre-merge-escalation-gate.mjs`）放行后由 planner 主线程 `merge-driver --pod` 本地合并（可回滚）。

## 2026-09-24 · K1 合并完成（§10e 契约包人合，planner 代 merge-driver 簿记）

- **supervisor 机械闸 = ESCALATE（riskTier critical）**：G6 从合并批 diff 重派生——含契约落地批（`3a9ff24`）触 `docs/contracts/**` critical 面 → critical 一律停人（向严设计；final 九闸绿 + evidenceComplete 在 verdict 与 r2 bearing fact）。human 批准本地 main 合并（pod-events `cac7682a`，approve-escalated-merge）。
- **前置升级闸结构性 BLOCK → §10e 人合通道**：通用条 1 的 `hasContractGateEvidence` 证据位未接入（脚本自注「恒 false 向严」）→ 契约面 diff 恒 BLOCK、无机械放行口；planner 落 escalation `988f78e9`（gate-tooling-bug）升 human 三通道，human 裁 = B（一次性人合）。执行时发现 **main 侧同日已落 `pod-coordination §10e` 契约包人合 runbook**（gov 批 `11fb5c1`，OQ-3=(a) human 批准）——人合 = 契约包**唯一合法通道、非流程跳过**，裁定实质与 runbook 同构，按 §10e 四步执行。
- **§10e 四步留痕**：① 预检 = main 新增 5 commit（23aed35..b35d3ed）与本批**零路径相交**（comm 实测）+ 契约门凭据在案（CCP §5 HUMAN_APPROVED / CDQ 三行 LANDED / gate evidence LANDED）；② 批准锚 pod-events `6faf8b7b`（`approve-contract-merge-manual`，§10e 首立专用 decision 值）+ escalation `988f78e9` resolved（`c2a73805`，留根项触发事实同录）；③ `git merge --no-ff ed5af0e` 入本地 main → **merge commit `757ccb3`**（双亲 b35d3ed + ed5af0e，message 注明人合 + 双批准锚 + 「remote push 归 human」）；④ Step 5b 三件代记 = merge-completed 事件 `e12a8b94`（payload 注「非 merge-driver：契约包人合（§10e）」）+ changes-index（`--upsert` 对 dev lane fail-loud = §10e-4b 与脚本 lane 语义不符**实测留痕**，改走 dev lane 正典全量重建 devCount 43）+ merge message 锚（③）。
- **并行工作树处置（human 授权留痕）**：main 工作树有 V5 会话未提交 hook 再生件（`delivery/workstreams/INDEX.md`/`index.jsonl`，与本批 diff 同路径）→ git 拒 merge；human AskUserQuestion 授权后代 checkout 还原两件再生件（V5 未跟踪面零触碰；hook 下次编辑 workstreams/ 自动重建 union 含 ui-v5-align 行）。§10e 步骤 1 预检增补素材：应加「main 工作树净度」一条。
- **board/roadmap 收口**：pod status = **merged**（portfolio-reconcile 单一写者，43 pods）；render-roadmap ② 节重渲染，**M4-K1 = achieved**（渲染面 K1a/K1b 拆分显示）。arch INDEX 计数漂移（main committed 41 vs 重建派生 43）= arch lane 陈旧面，按「各 lane 只写自己 INDEX」纪律已还原不代笔、向 human 明示。
- **§10e 留根项触发事实成立**（ARQ-260923-contract-merge-deadpath 建议①「runbook 实走摩擦 ≥1 次实测」）：三处摩擦——池侧旧 reference 无 §10e 指针（BLOCK 输出指针 main 侧已修、池侧随本次 ff 到位）/ §10e-4b `--upsert` lane 语义不符 / 常驻分支落后 main 时 runbook 不可见（多线同日并行时差）。详 pod-retro §八；归 human / loop-retro 立项评估。
- **合并入 main 面**：实现四件 + 冻结 26 齿 + notes（含 §9 §6 修订记录）+ mutation-replay 证据 + closeout 两件 + gate evidence（**C1 done / C4 pending 待 human spec 批**，delta 五 hunk 在 docs-delta §4）+ 治理面五 commit（ops-record / 拆包 / 派生 / 契约落地 / launch-prep）随批入 main。**K1 完成判据齐**：landing 完成 + verifier 绿 + retro 齐（含 §八合并门追记）。待 human：remote push / C4 spec 批（ARQ-260918 四处可搭载）/ milestones M4 行 state 翻 in_delivery / K2 launch 时点裁量。

## 2026-09-28 · 止血包 260928-story-connect-push-defer integrator 收口（Lite，单点摘除建连即推）

- **scoped verify round 3**：`verifier-evidence/rounds/r3.json`（bearing、ok:true、observedGateExit=0）**七闸全绿**（taskpackage-required / firmware-build〔真编译 build-pass、firmware 零触碰〕/ contract-lint / acceptance-coverage〔FIX-A1 functional、FIX-A2 structural〕/ mutation-check / docs-sync / security）。轮次史如实：r1 红（taskpackage-required——contractRefs 空，finalize 补正填 §1.3③）→ r2 红（acceptance-coverage——FIX-A2 structural 标记形态，补正2）→ r3 全绿；两轮红均 TaskPackage 元数据口径、非实现/齿面红。
- **全量复跑**：`python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance` → **`193 passed, 4 deselected, 2 subtests passed in 2.17s`**（exit 0）。红→绿账 = 冻齿态 1F/192P（唯一红 = FIX-A1 设计态红）→ 193P/0F（192→193 即 FIX-A1 一齿）。冻结件 sha256 `78c4e1ea…58457`（@73b5d3d）只读未动。
- **实现面**：`server/gateway/front/front_app.py` 单文件 **+7/−6**——摘除 ws_gateway 建连即推 `story_list` 调用点（三时点①），原位注释留根因 + human 方向锚 `b036f554` + ①归 K3；`_push_story_list` 本体与②③调用点、wire 形状零变化。detect_changes（since=73b5d3d）：seed_symbols=1、impacted_total=1（新负锚齿自身），blast radius 零既有符号。
- **closeout 两件落 `delivery/taskpackages/260928-story-connect-push-defer/`**：`docs-delta.md`（核心义务 = **K1 docs-delta §4 H1「三时点推送（WS 建连/归档后/删除后）」supersession 注记 + §5.1.2 H2「三时点自推」连带短语 + K1 review-packet 未覆盖面第 2 条（建连时点①）闭环申报；arch §5.2④ 修订后建议措辞以 patch 形态成文（现役两时点 + ①归 K3 重裁），供 C4 human spec 批先替换 K1 件新文再 apply——K1 件不改写**）+ `review-packet.md`（demo 对账 = 一致；未覆盖面 5 条含「设备侧实际停弹页面 = 真机复验项归 K3」明写 + none 项 = 跨子系统 e2e/correlationId）；`closeout-check.mjs` 只读 exit 0。
- **commit 冻结批**（分支现 HEAD 起）：实现一件 + closeout 两件 + 本包 pod 簿记面（taskpackage.json〔含 planner finalize 补正×2〕/ pod-events.jsonl / verifier-evidence/ r1-r3 / evidence/ / verify-runs.jsonl + current/taskpackage.json 投影对）；**排除他会话脏件 5 面**（.changes-index-state.json / arch/INDEX.md / 260926-arch-review 目录 / TECH-260928 crq 件 / current/verdict.json——不触碰不 stage 不还原）。
- **changes-index**：无参全量重建（dev lane 正典路径；`--upsert` 为 arch lane 专属）——本包 dev INDEX 行在案即达成；arch/INDEX.md 与 .changes-index-state.json 若被重建波及**维持不 commit**（他会话纠缠面，如实申报）。
- **等待 supervisor 机械合并门**：supervisor APPROVE + 前置升级闸放行后由 planner 主线程 `merge-driver --pod` 本地合并；真机复验（设备侧停弹页面）归 M4-K3 设备轮。

## 2026-09-28 · 止血包生产部署（合并前真机验证，human 指令「先部署，我测试一下」锚 766df538）

- **部署面**：ECS `kidpal-front.service`（/opt/kidpal/front，uvicorn wss:8000）。远端此前为混装态（front_app 含 m4-k1 建连即推、story.py/display_schema 疑 m3 期旧版）——本次按**分支被测一致态**（193P/0F，HEAD 8ca0e85）整体同步 5 件：`gateway/story.py`、`gateway/display_schema.py`、`gateway/schedule.py`、`gateway/schedule_push.py`、`front/front_app.py`（md5 逐一与仓内比对一致；schedule 两件 = school-start 修复 545aa7a 随批就位）。远端原件备份 `.bak-260928-story-fix`。
- **重启核验**：systemctl restart → active，Uvicorn https://0.0.0.0:8000 监听在位，启动日志零 import 错误（2026-09-28 13:18 CST）。
- **部署机制注记**：Windows 命令行 32KB 上限 → base64 分块（20KB/块）经 ssh_helper 上传；story.py 首传因超限静默失败，分块后成功（md5 校验兜底抓到）。
- **合并门状态**：supervisor 机械闸 ESCALATE（high：all-mock 启发式标定错配嫌疑，verdict 落仓 3f63b0b）→ human 裁「先部署测后裁」→ 合并保持 blocked 待真机结论。

## 2026-09-29 · K2 launch prep（主线程 skill：kidpal-pod-launch-prep；launch 由 human 执行）

- **调度判定**：本批 = K2 单包——K1 已 merged（2026-09-24，dependsOn 实存）；K3 dependsOn K1+K2、K2 未 merged → K3 不可入批（依赖图说了算，不硬凑并行）。perf-k1 树（C:/AI/KidPal-pod-perf-k1-roundtrip-latency，分支 dev/260924-perf-k1-roundtrip-latency）分支未入 main 且 board 无对应在途包——来历待核，本批不复用，留 human（遗留项）。
- **机械门**：`portfolio-reconcile --check-launch` ok:true / blocking 空 / warn 空（base ref = `base/m4-star-notebook` 随本批 ff 快进至 5575a23，仍含重拆批 d2b331c 全部拆包制品；本包相关 launchPreconditions 5 verified / 0 pending——LP7 UI-ready 含其中，P3 五面 PNG 在盘核过 + 重拆门 1 已裁 uiux L2 成立）；`settings-lint.mjs` 0 RED / 0 WARN。
- **双面基线重锚 @ 5575a23（本树实测；launch 批仅触 delivery/ 治理面，面域差异判停预期零差异）**：
  - server 面：`python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/acceptance"` → **117 passed / 0 failed / 4 deselected, 2 subtests passed**（全套 offline 含 server/tests = 193 passed 同日佐证，与 defer 包 193 零偏移）。面域 = server；锚 SHA = 5575a23。零红。旧锚 80 passed @ 5e862d0 因 K1/defer 批新齿全绿而偏移，作废。
  - firmware host 面：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf` → **107 Tests / 0 Failures / 0 Ignored，exit 0**。面域 = firmware；锚 SHA = 5575a23。零红。旧锚 103T0F @ 5e862d0 作废（char-window-v5-trim / char-window-key-triggered-play 等固件批新增齿全绿）。
- **树与环境**：常驻树 C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch 接力棒复用（K1 merged → 直接续跑零复位）；常驻分支 dev/260915-m1-k1a-key-layer-dispatch @ 5575a23 = main HEAD、tracked 净 + untracked 零残留（交接卫生过）；managed_components 软链在位 + host build ELF 热缓存保住。图索引 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` mode=full 强制刷新（18296 节点 / 49896 边与刷新前一致 = 已最新；parse_partial 102 件 = 既有 best-effort 信号非新增；skipped 0）。
- **board 预填**：260924-m4-k2-p3-star-notebook-ui → queued + worktreePath = C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch（44 pods，mergeToken holder=none）。
- **启动卡**：已落 `delivery/taskpackages/260924-m4-k2-p3-star-notebook-ui/launch-card.md`（五要素齐 + intake 特别注意：契约三触点纯消费零改 / story_page.h 旧 FROZEN 头注退役勿误守 / 拆包侧 brief 撤回 planner Step 7 自产）。
- **遗留 human 项**：① launch 由 human 在常驻树自跑（命令行两形态见启动卡第五要素；本 prep 会话占用树内会话位，launch 前先收本会话）；② perf-k1 树来历待核（分支 dev/260924-perf-k1-roundtrip-latency 未入 main、board 无对应在途包——留池 or 复位归 human 裁）；③ milestones.jsonl M4 行 state 收口时点沿 K1 遗留同批裁量。

## 2026-09-30 · K2 integrator 收口（final r4 十闸全绿 → closeout 制品 → commit 冻结）

- **影响面圈定终验（final 相位两步走）**：① 圈定 = detect_changes（project `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`，since=base/m4-star-notebook：changed_files 32 / seed_symbols 163 / impacted 34 全在 firmware/main）∪ impactedTestsSeed 合成 test-scope 清单落 `delivery/taskpackages/260924-m4-k2-p3-star-notebook-ui/test-scope-final-r4.txt`（14 现役测试文件 + archive 排除留痕；正常圈定路径非降级）；② final bearing verification = `verify.mjs --evidence-mode=round --phase=final --scope-file <清单> --round 4 --invocation-id 2322f6f7-2608-40c7-88e3-de91b0f1ea73` → **r4 fact ok:true 十闸全绿**（8 scoped + unit/acceptance full 档求交），`verifier-evidence-check.mjs` → `{"ok":true,"factOk":true}` exit 0。
- **host 全量回归（真实退出码）**：`wsl espidf_wsl.sh build && ./build/host_infra_test.elf` → **128 Tests / 0 Failures / 0 Ignored，EXIT_CODE=0**（基线 107T0F @ 5575a23 + 新齿 21 = acceptance 13 + unit 8，既有齿零偏移）；device build 闸过（`xiaozhi.bin` 0x310b20 / 余 11%）；mutation 13/13（B1 摘源清单敏感路径 + tofu 遮蔽旁注如实）。
- **closeout 两件落 `delivery/taskpackages/260924-m4-k2-p3-star-notebook-ui/`**：`docs-delta.md`（spec patch 形态七 hunk：requirements 版本行 v1.61 / F2 对账块四项销账 / §2.3 键位对账 P3 半销账 + 门1 F7⑤点名 / AC3 双指令清扫（门1 F1）/ 变更记录行；arch §5.3 story_page 现状行 + §5.5.4 改名对账销账——基线 = c735062 落地后现文）+ `review-packet.md`（demo 对账 = 一致 + 未覆盖面 9 条 + 真机目测半 deferred 挂 K3 声明 + UI 文本证据 + pod-events 交叉引用 12 事件 + minimal 债务 5 处全登记 + Findings 3 NIT 零 hard-violation）。`closeout-check --update-index` ok:true（advisory 3 条：verdict/verification 按 SSOT advisory 不晋升包目录〔r4 fact 承载〕/ pod-retro 归 planner 收尾相位 / loop-retro 阈值触发归主线程）。
- **gate evidence 跨包条件代落**：K1 包 `architecture-gate-evidence.md §6` **C2 → done**（SPV2-A7/A8/B3 证据）+ **C3 → K2 半 done / K3 live 半 pending** 注记（先例 = K1 integrator 对 C1/C4 同路径）。
- **changes-index**：无参全量重建（dev lane 正典路径）——本包行在案（closed，devCount 48）；`arch/INDEX.md` 重建波及维持不 commit（他 lane 纠缠面，defer 包先例）。
- **commit 冻结批**（分支 dev/260915-m1-k1a-key-layer-dispatch，未 push/未 merge）：实现 16 tracked 文件（+1635/−513）+ 新齿 2 件 + closeout 两件 + gate evidence C2/C3 + pod 簿记面（taskpackage.json / pod-events.jsonl / verifier-evidence r1–r4 / verify-runs.jsonl / test-scope 清单 / mutation .md / 双 brief）+ current/ 投影四件 + dev INDEX + 本 progress 节。
- **遗留**：① spec 批七 hunk 待 human 逐字过目落地（合并后，ARQ-260918 剩余处可同批）；② milestones M4 行 state 沿 K1 遗留同批裁量；③ 真机目测半 + 手感 + wire 真发 + 时点①重裁归 K3 设备轮；④ 代存 mutation .md 投影陈旧（Findings #1，planner 择机同步）。
- **等待 supervisor 机械合并门**：integrator 不 push、不 merge；supervisor APPROVE + 前置升级闸放行后由 planner 主线程 `merge-driver --pod` 本地合并。

## 2026-09-30 · K3 launch prep（主线程 skill：kidpal-pod-launch-prep；launch 由 human 指令代跑）

- **调度判定**：本批 = K3 单包接力棒——dependsOn K1+K2 均 merged（K2 2026-09-30T00:36 入 main，merge commit ddf2492）；mergeOrder 40 升序合法；WIP 余量足；依赖图说了算不硬凑并行。
- **机械门**：`portfolio-reconcile --check-launch` ok:true / blocking 空 / warn 空（base ref = `base/m4-star-notebook`）；`settings-lint.mjs` 3 files / 0 RED / 0 WARN。launchPreconditions 4 verified / 1 pending（LP4 = 真机不撞车 + live 轮 human 在场持板——live 确认轮前置项，设备轮前核对，不阻 host/实现相位）。
- **树同步**：常驻树 C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch 接力棒复用（K2 merged → 续跑零复位）；发现常驻分支与 main 分叉（两侧各 1 笔 K2 后簿记 commit，均为 pod-events.jsonl 单行追加）→ rebase main 同步（2c7343a = main ddd3875 + 簿记 1 笔，rebase 干净无冲突）；tracked 净 + untracked 零残留；managed_components 软链在位 + host build 热缓存保住（ninja no work to do）。
- **双面基线锚**：
  - server 面：**引用既有锚**——launch 批至 HEAD 对 `server/` 零差异（`git diff --quiet 5575a23 HEAD -- server` 实测），引 K2 prep 锚行：117 passed / 0 failed / 4 deselected（server/acceptance；全套 offline 193P 同日佐证）@ 5575a23，面域 = server，命令见 K2 prep 锚行。零红。
  - firmware host 面：重锚实测——`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf` → **128 Tests / 0 Failures / 0 Ignored，exit 0**（2026-09-30，本树 HEAD 2c7343a 实测；面域 = firmware；与 K2 final r4 128T0F 一致 = K2 merged 后零偏移）。零红。旧锚 107T0F @ 5575a23 因 K2 新齿 21 入账作废。
- **图索引**：`C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` mode=full 刷新（18414 节点 / 50277 边，含 K2 增量；skipped 0；parse_partial 104 = 既有 best-effort 信号非新增）。
- **board 预填**：260924-m4-k3-record-session-live → queued + worktreePath = C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch（44 pods，mergeToken holder=none）。
- **启动卡**：已落 `delivery/taskpackages/260924-m4-k3-record-session-live/launch-card.md`（五要素齐 + intake 特别注意七条：时点①重裁必答题 / K2 冻结面零偏移 / prompt v7 版本化演进 / 板端零新增计时器 / KEY 长按语义反转负锚 / 拆包 brief 撤回 / LP4 live 轮 human 在场）。
- **遗留 human 项**：① milestones.jsonl M4 行 state 收口时点沿 K1/K2 遗留同批裁量；② perf-k1 树来历待核（沿 K2 prep 遗留）；③ live 确认轮真机时点由 human 裁量（单真机不撞车核对归设备轮前）。

## 2026-09-30 · K3 合并 + 部署 + 真机初测处置（planner 代记；human 指令「立马执行烧录和部署」授权，pod-events 260b1f48）

- **合并**：supervisor 机械闸 ESCALATE（high riskTier 必升面）→ human 批（91bb853b 五要素凭据）→ 升级闸两拦两解（①人批事件 actor 归属修正 ②review-packet :95 否定句与契约字面同行共现触 T5 扫描假阳性——gate-tooling-bug 第三例 e3395f1a，措辞修正 69be42d 解除）→ merge-driver 本地合并 `ae480ec`，main tip `4cb77a8`。M4K3-DC-1（P3 冷浏览无列表数据源）human 裁接受 dropped。
- **根因判定（真机初测两问题）**：human 初测「我要讲故事只有录音波浪 / 录完 P3 无故事」= **K1/K2/K3 三包零烧录零部署**（板子跑 M3 期旧固件、ECS 服务旧代码），非新代码缺陷；live 轮照 LP4 顺延。
- **烧录**：全量 flash（bootloader/partition/ota_data/app@0x20000/art@0x780000/generated_assets@0x800000，NVS 不擦），bin = 136T0F 构建产物（3,215,584 B）；启动观测零 RE-ENUM + kidpal render home 正常。
- **ECS 部署**：front_app.py（ctx 条件推送）+ apply-xiaozhi-overlay.sh + prompt v8 + provider.config.yaml 四件 SFTP（md5 逐件核对一致，远端备份 .bak-260930-m4k3-deploy）；overlay 重渲（v8 prompt 注入 .config.yaml，「帮我记下来」双指令在文）；kidpal-front 重启 active + 0.0.0.0:8000 在听；引擎容器重启后设备 23:31 重连、display-strip 补丁标记在役。
- **部署面摩擦留根**：MSYS 路径转换把首传四件改写进远端伪路径（MSYS_NO_PATHCONV=1 重传解除，误写面已清）；c:/tmp 写入被 enforce-scope 拦（改用现成 ssh_put/ecs_put 工具链，未新写脚本）；SFTP 子系统路径受限（ecs_put base64-over-exec 既有解）。
- **live 轮重点目击项（部署后新增一条）**：触发词入口（非 P3 页说「我要讲故事」）的记录语境置位/演出切页与 P3 KEY 入口不同源——host 齿为纯逻辑决策表、真机沿未目击，初测复验时重点看。

## 2026-10-01 · K3-repair launch prep（主线程 skill：kidpal-pod-launch-prep；launch 面补齐）

- **调度判定**：本批 = Package A 单包（纯固件修复）——前序 K3 包（260924-m4-k3-record-session-live）已 merged（ae480ec，main tip 4cb77a8）；本包为 K3 落地后 2026-10-01 真机冒烟暴露的四缺陷止血（D1 列表无消费方 / D2 层栈让位 / D4a 键路由毁稿 / D4b 悬垂 panic），dependsOn 实存、mergeOrder 42（K3=40、defer=41 后）。Package B（D3-甲，跨面）受 spec/contract anchors 门控暂停；Package C（openclaw model_name 冲刷）独立小包，未 launch。
- **机械门**：`portfolio-reconcile --check-launch 261001-m4-k3-record-session-live-repair --base-ref base/m4-star-notebook` ok:true / blocking 空 / warn 空（base ref 随本批 ff 至 8ea3bce = main + 本批拆包制品 commit）；launchPreconditions 4 verified / 0 pending（另 3 条属其他包、折叠不逐条列）。`settings-lint.mjs` 3 files / 0 RED / 0 WARN。
- **树同步**：常驻树 C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch 接力棒复用（K3 merged → 续跑零复位）；常驻分支与 main 分叉（各 2 笔，均 m4-k3 pod-events/progress 簿记）→ `git rebase main` 同步（干净无冲突，HEAD 72ce58f）；tracked 净 + untracked 零残留；managed_components 软链在位 + host build 热缓存保住（ninja no work to do）。
- **双面基线锚 @ 72ce58f（本树实测）**：
  - server 面：`python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/acceptance"` → **127 passed / 0 failed / 4 deselected, 2 subtests passed**（@72ce58f；面域 = server）。零红。旧锚 117P @ 5575a23 因 K3 新齿 10 入账作废。
  - firmware host 面：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf` → **136 Tests / 0 Failures / 0 Ignored，exit 0**（@72ce58f；面域 = firmware）。零红。旧锚 128T0F @ 2c7343a 作废（K3 新齿 8 入账）。
- **图索引**：`C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` 复用树沿用（index_status ready，18553 节点 / 50753 边，parse_partial 105 = 既有 best-effort 信号；本批只触 delivery/ 治理面 + .claude/skills/，均在索引排除面内 → 无增量，未强制重刷）。
- **board 预填**：261001-m4-k3-record-session-live-repair → queued + worktreePath = C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch + mergeOrder 42（45 pods，mergeToken holder=none）。
- **启动卡**：已落 `delivery/taskpackages/261001-m4-k3-record-session-live-repair/launch-card.md`（五要素齐 + intake 特别注意五条：本包纯固件四缺陷 D3/逐句回显不在验收面 / D2-D4a 同一次决策一致选定 / D4b 三修并行 / 契约零改 + Package C 独立 / live 轮 human 持板）。
- **本批 commit**（分支 dev/260915-m1-k1a-key-layer-dispatch，未 push/未 merge）：72ce58f（kidpal-ecs-gateway skill 记录 OpenClaw 400 根因，human 裁随本批）+ cdc04c5（spine + Package A 三件 + Package C 计划）；专用 base ref `base/m4-star-notebook` ff 至 8ea3bce（main 4cb77a8 + 制品 cherry-pick commit）。
- **遗留 human 项**：① live 真机复验矩阵四窗须 human 在场持板（LP4 同族，单真机不撞车）；② Package B（D3-甲）受 spec/contract anchors 门控，Track A 定形后另立包；③ milestones.jsonl M4 行 state 收口时点沿 K1/K2/K3 遗留同批裁量；④ perf-k1 树来历待核（沿 K2/K3 遗留）。

## 2026-10-01 · K3-repair（Package A）integrator closeout（host 面绿；真机轮 deferred）

- **本批**：261001-m4-k3-record-session-live-repair = K3 落地后真机 P3 冒烟四缺陷止血（D1 列表无消费方 / D2 层栈让位 / D4a 键路由毁稿 / D4b 悬垂 panic）；纯固件面（`firmware/main/**` 7 文件 +230/−17 + 新齿 3 件）；契约/spec/服务端零改。分支 dev/260915-m1-k1a-key-layer-dispatch（未 push/未 merge）。
- **影响面圈定终验**（Step 4）：detect_changes（project `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`，since `base/m4-star-notebook`，merge_base 4cb77a8；changed_files 26 / impacted 7 节点[firmware/main]）∪ impactedTestsSeed → 清单 `delivery/taskpackages/261001-m4-k3-record-session-live-repair/test-scope-final-r2.txt`。**scoped r1 全绿（8 闸）+ final r2 全绿（10 闸含 full unit/acceptance）**，`verifier-evidence-check` → `{"ok":true,"factOk":true}`。
- **真跑 host 齿**：`145 Tests / 0 Failures / 0 Ignored，exit 0`（RSR-A1..A5 + RSR-C1 逐齿 PASS，K1/K2/K3 既有齿零回归）。5 条真实变异逐 AC 敏感（真红→真绿）。
- **docs-delta**：requirements/契约/spec 零自改；检出两处 spec 面归属他项（§5.2④ 机制对账归 Package B / §5.2④① consumer 补正随 K3 pending delta）。
- **未覆盖面申报**：真机四窗目击 E3 + AFE VAD 真沿 + 120s 静默归档 = **设备轮盲区，本相位 deferred，如实申报**；逐句回显/D3 = Package B；server 面 N/A。
- **等待 supervisor 机械合并门**：`riskTier=high` → 机械闸必产 ESCALATE，合并须 human 批；integrator 不 push、不 merge；supervisor APPROVE/ESCALATE-human 后由 planner 主线程 `merge-driver --pod` 本地合并。
- **遗留 human 项**：① live 真机复验矩阵四窗须 human 在场持板（LP4 同族）；② §5.2④① consumer 补正文本与 K3 pending delta 同批交 human spec 批；③ milestones.jsonl M4 行 state 收口时点沿 K1/K2/K3 遗留同批裁量。

## 2026-10-01 · K3-repair（Package A）integrator **re-closeout**（D2 真机诊断定死后重收）

- **背景**：首收 5950bb7 后 supervisor 机械闸 ESCALATE（声明 high / 实 diffTier low）→ human 裁「先真机复验再裁」（pod-events `ef280e1a`），保持 5950bb7 冻结、烧本分支跑 K3R-E3。
- **D2 走了两轮弯路后由真机日志定死**：真根因 = **键帽派发漏 `KSTORY_PAGE_RECORDING` 分支**（`lcd_display.cc` `kidpal_keybar_sync_locked`，K2 留给 K3 未落）+ **REC 指示相位算术恒 off**（`story_page.c` `(rec_seconds*1000/250)%2 ≡ 0`）；前两轮攻「VOICE 覆盖层驻留」为弯路（VOICE 层 round-1 即已让位，`voice_end: top=2`）。补严缝 [R4]/[R5] 经 architect 判对 D2 no-op、作历史留根（齿 RSR-A6/A7 冻结）。
- **真机验收**：human 目视 v6 通过（底栏「收尾/结束」+ `00:14` + 常亮黑 REC 方块）；D4a `story_key stop_rec archive_pend=1`、D4b 零 panic（早前已过）；D1 推送到达后可显示已证（r6 `count=2`）。证据 `c:/tmp/serial-live-repair-r{2..6}.log`；pod-events `e2ce7911`（D2 定死过）/ `64a4012d`（UI 还原度出包）/ `97c36553`（D1 冷浏览 dropped 复裁接受）。
- **re-closeout 验证**：影响面圈定 project 同上（changed_files 39 / impacted 45 节点）→ `test-scope-final-r4.txt`；**scoped r3 + final r4 全绿（10 闸）**，`verifier-evidence-check` `{"ok":true,"factOk":true}`；真跑 host 齿 **147 Tests / 0 Failures**（RSR-A1..A7 + RSR-C1）。实现面 = 8 改（含 `lcd_display.cc`，planner 已扩 changeManifest）+ 新齿 3 件。
- **未覆盖面**：D2 派发点 `lcd_display.cc` **host 不可编 → 无法机判**，architect 建议升「设备轮硬门」（治理面改动，**归 human 裁**）；D1 冷浏览 dropped（human 接受）；录音页 UI 还原度出包；逐句回显 + D3（原「manual 聆听产不出 stt 帧」被真机反驳，真缺口在 front story 累积/回传接线）= Package B。
- **等待 supervisor 机械合并门**：stale `supervisor-verdict.json`（45ddc85 落，验证旧码）须 planner 在 re-closeout 后重跑 `supervisor-verdict-gate.mjs` 覆盖；`riskTier=high` → 必产 ESCALATE，合并须 human 批。

## 2026-10-01 · B1/C launch prep（归档时序包 + model_name 冲刷包）

- **调度判定**：两包均无 `dependsOn`、各为单 owning 包，可即时开工。顺序 = **B1 `261001-m4-k3-story-archive-order`（mergeOrder 43，workstream m4-star-notebook）先行 → C `261001-openclaw-model-name-overlay-drift`（mergeOrder 44，独立变更、无 workstream）其后**。两包均预填**同一常驻树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`**（一树一会话 → 串行跑，沿 M1–M4 单常驻树接力棒形态）；若要并行，C 可移至空闲树 `C:/AI/KidPal-pod-perf-k1l-roundtrip-latency`（**建/移树决策归 human**，本批未动）。
- **launch 机械门**：`node .claude/scripts/portfolio-reconcile.mjs --check-launch <changeId> --base-ref base/m4-star-notebook`——首跑 blocking = 缺 `taskpackage.json`，补齐（含 input-brief 双份 + `docsOwner`）后**两包复跑绿**。`settings-lint.mjs` 未跑（本批未改 settings）。
- **基线锚（producer 五要素）** @**60744cb**（面域分别记）：
  - **server 面**：`python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/acceptance` → **127 passed / 0 failed / 4 deselected / 2 subtests passed，exit 0**。
  - **firmware host 面**：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf` → **147 Tests / 0 Failures / 0 Ignored，exit 0**。旧锚 136T0F @72ce58f 因 K3-repair 落地有固件差异而作废（差异判停：`git diff --quiet 72ce58f HEAD -- firmware` 非空）。
- **图索引**：`C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` 复用树沿用（无需重建）。
- **board 预填**：两包 `status=queued` + `worktreePath` + `mergeOrder 43/44`（47 pods，mergeToken holder=none）。
- **启动卡**：`delivery/taskpackages/261001-m4-k3-story-archive-order/launch-card.md` 与 `delivery/taskpackages/261001-openclaw-model-name-overlay-drift/launch-card.md`（五要素齐）。
- **本批制品**：B1（plan.md + taskpackage.json + input-brief 双份 + 启动卡）、C（plan.md + taskpackage.json + input-brief 双份 + 启动卡 + 上游 CRQ `docs/crq/tech/ARQ-261001-openclaw-model-name-overlay-drift.md`）。
- **遗留 human 项**：① B1 转绿后真机复验须 human 持板（P3 按 KEY 停录 → 列表新增 + 盖章）；② 并行与否（C 是否移至 perf 树）；③ B1 须改写已 merged 包（K1）冻结验收的时序假设——改冻结 oracle 的签字走 dev-loop 常规人门。

## 2026-10-01 · B1（story-archive-order）integrator closeout（host/验收面绿；真机轮 deferred）

- **本批**：261001-m4-k3-story-archive-order = 「归档早于转写」顺序缺陷修复（缺 261001 真机 r2 实录——listen/state=stop 上行帧先到、转写 stt 1.7s 后才到；旧实现于 stop 帧立即 `acc.stop()`，累积必空 → 返 None + 打回 chat → 转写丢弃 → 内容彻底丢失）。修法 = pending-stop 挂起 + 转写到达后归档（+ accum 非空立即归档兼容分支）+ 正文剔尾随收尾词 + 10s 超时释放。**纯 server 面**（`server/gateway/front/front_app.py` + `server/gateway/story.py` + 验收件改真机序 + 单测）；契约/spec/固件零改。分支 dev/260915-m1-k1a-key-layer-dispatch（未 push/未 merge）。
- **影响面圈定终验**（Step 4）：detect_changes（project `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`，since `base/m4-star-notebook`，merge_base 60744cb；changed_files 25 / seed_symbols 48 / impacted_total 76 节点[server/gateway 44 · server/acceptance 20 · firmware/main 3 名碰撞 · builtins 9]）∪ impactedTestsSeed → 清单 `delivery/taskpackages/261001-m4-k3-story-archive-order/test-scope-final-r3.txt`（server 面 12 在职测试件圈 7）。**scoped r1 红=taskpackage 元数据缺口 → scoped r2 全绿 → final r3 全绿（10 闸含 full unit/acceptance）**，`verifier-evidence-check` → `{"ok":true,"factOk":true}`。
- **真跑**：final r3 unit 闸 149 passed / acceptance 闸 77 passed（scope 7+5 文件）；全量锚 = `server/acceptance` 133 passed / 0 failed / 4 deselected（红基线 12 failed / 124 passed）+ `server/tests` 89 passed。4 条 SAO mutation 逐 AC 敏感（真红→真绿）。
- **docs-delta**：requirements/契约/spec 零自改（`git status docs/` 零输出）；§5.2④ 判不需要文本补正（architect 判不构成漂移）；检出三处 spec 面归属他项（§5.2④ 机制对账归 Package B / 收尾后引擎播报归 B3 / 120s sweep 剔词归 B2/B3）。
- **未覆盖面申报**：120s sweep 剔词 + 逐句回显/自动收尾/sweep 复活（B2/B3）；收尾后引擎播报（B3）；`_story_finish_on_phrase` 正齿（随 B2/B3 补）；**SAO-E3 真机复验（human 持板）尚未跑、如实标 pending**；固件/跨端 N/A。
- **等待 supervisor 机械合并门**：`riskTier=high` → 机械闸必产 ESCALATE，合并须 human 批；integrator 不 push、不 merge；ESCALATE-human 批后由 planner 主线程 `merge-driver --pod` 本地合并。
- **遗留 human 项**：① SAO-E3 真机复验须 human 持板（P3 按 KEY 停录 → 列表新增 + 盖章；串口判据 `stop_rec archive_pend=1` 后出现转写、front 不再 `empty_accum`）；② §5.2④「归档动作等转写到达」文本补正（advisory）随 spec 批交 human；③ milestones.jsonl M4 行 state 收口时点沿 K1/K2/K3 遗留同批裁量（本包无跳变）。

## 2026-10-10 · 261010 真机 bug 批 launch prep（主线程 skill：kidpal-pod-launch-prep；triage 批 = ②Lite + ②Full 串行，launch 按 human 指示 headless 代跑）

- **调度判定**：本批 = 261010 真机使用 bug 台账 dev 面两包（triage 分流 = architect review + human 触点①确认：② `261010-prompt-v9-schedule-anti-narration` 单包 Lite〔mergeOrder 45〕→ ① `261010-device-reconnect-on-server-close` 单包 Full riskTier high〔mergeOrder 46〕，串行接力——同常驻树一树一会话，沿 M1–M4 单常驻树接力棒形态）。两包零文件交集，理论可并行但建/移树决策归 human 未动；perf-k1 空闲树不可复用（常驻分支 dev/260924-perf-k1-roundtrip-latency 压两笔未合 main 的证据/裁定 commit，aa5ec4b 非主谱系，留待 human 裁）。第三包 `261010-p3-storylist-cold-start-restore`（Full + spine 261010-p3-storylist-cold-start）受 launchPreconditions 门控（需求/架构/契约 anchors 未回填）不在本批。
- **机械门**：`settings-lint.mjs` 3 files / 0 RED / 0 WARN；`portfolio-reconcile --check-launch` 两包结果见下（base-ref = main，随本批 ff 至 prep 制品 commit）。
- **树同步**：常驻树 C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch 沿用；main（348c681）落后常驻分支 7 笔（261010 会话治理批：CRQ 登记 ×3 + 决策 21 仓内根治 04c86a9 + 结项 + portfolio 模型 + triage 落盘）且零独有提交 → 纯 ff 同步，无 rebase 无冲突；tracked 净（.codebase-memory 两件 = 索引器常驻脏，AGENTS.md 口径不追交）；固件环境在位（sdkconfig / dependencies.lock / managed_components 软链 / host build 热缓存 ninja no work to do）。
- **双面基线锚 @ 2d94c36（本树实测，producer 五要素）**：
  - server 面：`python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/acceptance` → **139 passed / 0 failed / 4 deselected / 9 subtests passed，exit 0**（2026-10-10；面域 = server）。零红。旧锚 127P @60744cb 作废（决策 21 批新齿 test_261010_engine_decision21_overlay_persist 3 齿 + subtests 入账）。
  - firmware host 面：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf` → **147 Tests / 0 Failures / 0 Ignored，exit 0**（2026-10-10；面域 = firmware；热缓存 no work to do）。零红。与 B1/C prep 锚 147T0F @60744cb 持平（其后 firmware 面零差异）。
- **建包件**：两包 taskpackage.json 随本批补齐（triage 建包职责：origin regression 型 upstreamRef 指 CRQ 条目 + planRef 回指 sibling plan.md）；check-launch blocking 件 = taskpackage.json 在 base-ref 可及。
- **board 预填**：两包 queued + worktreePath = C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch + mergeOrder 45/46（前批 C=44 后继）。
- **图索引**：`C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` mode=full 刷新随本批执行（决策 21 批 server/deploy 改动入索引面）。
- **遗留 human 项**：① 包（device-reconnect）真机轮 human 在场持板 + riskTier high 合并人批为硬前置；② 包（prompt-v9）supervisor 若判 all-mock 需 human 放行 + PN-A5 真机课表问答观测 human 在场；③ 包 spine 上游面（需求面与 REQ-261010-P1 并批 /requirement-change → 契约门 data.restore additive → anchors 回填后 launch）另行推进；perf-k1 树两笔未合 commit 去向待裁。

## 2026-10-11 · 261011-p1-home-v5-rework launch prep（单包批，requirement v1.61 落地）

- **调度判定**：本批 = 单包 `261011-p1-home-v5-rework`（②单包 Full riskTier high；triage 分流 = architect review + human 触点①三答确认：②单包 Full / preview 数组+空数组语义 / V5 范围收窄知悉）。来源链 = requirement v1.61（S-F1.1 空态 + 明天区改判，档 1 全程走毕 REQ-261010/260916 双销行）→ spine 261011-p1-home-v5-rework → 契约门 CCP HUMAN_APPROVED（preview 多项数组 + 三态语义四触点，先批后落）。串行接力沿常驻树（上一包 device-reconnect 已 merged）。
- **树同步**：常驻树 C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch 沿用；main（7d337f6）含 device-reconnect merge 2 笔独有 → 本批**回同步 rebase**（4 笔线性重放 017c542/5478b41/a9bd4db/4a9e87f，零冲突）→ main 纯落后、prep 制品 commit 后 ff。tracked 净、untracked 0；固件环境在位（sdkconfig / dependencies.lock / managed_components 软链 / host build 热缓存）。
- **双面基线锚 @ 4a9e87f（本树实测，producer 五要素）**：
  - server 面：`python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/acceptance` → **144 passed / 0 failed / 4 deselected / 9 subtests passed，exit 0**（2026-10-11；面域 = server）。零红。旧锚 139P @2d94c36 作废（两面差异判停均命中重跑：device-reconnect 批 firmware/server 两面均有改动）。
  - firmware host 面：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf` → **153 Tests / 0 Failures / 0 Ignored，exit 0**（2026-10-11；面域 = firmware；增量编译 boot_auto_connect 齿入链）。零红。旧锚 147T0F @2d94c36 作废（firmware 面 +6 齿）。
- **机械门**：`settings-lint.mjs` 3 files / 0 RED / 0 WARN；`portfolio-reconcile --check-launch 261011-p1-home-v5-rework --base-ref main` → prep 制品 commit + main ff 后复跑（首轮 blocking = taskpackage.json 未进基准 ref，属时序预期）。
- **建包件**：taskpackage.json 随本批补齐（triage 建包职责：origin human-ad-hoc 型 upstreamRef 指 spine + planRef 回指 sibling plan.md；字段给值取 architect 分流裁决；acceptance 齿/红基线字段留 acceptance-author 冻结轮回填）。check-launch blocking 件 = taskpackage.json 在 base-ref 可及。
- **board 预填**：queued + worktreePath = C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch + mergeOrder 47（前批 device-reconnect=46 后继）。
- **图索引**：`C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` mode=full 刷新随本批执行（requirement/契约/spree prep 制品入索引面）。
- **遗留 human 项**：① 真机轮 human 在场持板（空态两态 + 明天区多项 + chip「明天」）+ riskTier high 合并人批为硬前置；② headless 启动由主线程代跑（human 2026-10-11 指示「拆好包后准备好环境，然后 headless 启动开发」）；③ spine openQ q4（v1.57 对账注过时勘误候选）另行批。

## 2026-10-12 · 261012-kidpal-realtime-facts launch prep（单包批，TECH-261012 修法 + requirements v1.62 先行捎带）

- **调度判定**：本批 = 单包 `261012-kidpal-realtime-facts`（②单包 Lite riskTier medium；triage 快审 + human 触点①三答确认）。来源链 = TECH-261012（天气幻觉，真机看屏轮新发现）→ human 定向「补 kidpal 环境：AGENTS.md 充实 + 工具对齐」→ triage 裁 prompt v10「实时事实回合」+ Tavily 实证 + AGENTS.md 仓内镜像；需求面档 1 先行捎带已落稿 v1.62（NFR-SEC-2 ④ / NFR-SAFE-1）；契约面 NO_CONTRACT_CHANGE（§1.6 四分类留痕 spine anchors.contract）。串行接力沿常驻树（上一包 261011 已 merged）。
- **树同步**：常驻树沿用；main 含 261011 契约包人合 merge（952b855）+ 簿记（0654b5b/850f409）→ 回同步 rebase（2 笔线性重放 7c8cd50/9d07329 零冲突）→ main 纯落后、prep 制品 commit 后 ff。固件环境在位（host build 热缓存 no work to do）。
- **双面基线锚 @ 9d07329（本树实测，producer 五要素）**：
  - server 面：`python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/acceptance` → **145 passed / 0 failed / 4 deselected / 9 subtests passed，exit 0**（2026-10-12；面域 = server）。零红。旧锚 144P @4a9e87f 作废（261011 批 preview 多项齿入账）。
  - firmware host 面：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf` → **155 Tests / 0 Failures / 0 Ignored，exit 0**（2026-10-12；面域 = firmware；ninja no work to do 热缓存）。零红。本批固件零改动。
- **机械门**：`settings-lint.mjs` 0 RED（本批未复跑前值 0 RED 在册，prep commit 后随 check-launch 复核）；槽位核验 server/deploy/ 无并行 .v10 占用（ARQ-260908 纪律）绿；`portfolio-reconcile --check-launch --base-ref main` → prep 制品 commit + main ff 后复跑。
- **建包件**：taskpackage.json 随本批补齐（origin regression 型 upstreamRef 指 TECH-261012 条目 + planRef 回指 sibling plan.md；RF 齿/红基线字段留 acceptance-author 冻结轮回填）。
- **board 预填**：queued + worktreePath = C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch + mergeOrder 48（前批 47 后继）。
- **图索引**：`C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` mode=full 刷新随本批执行。
- **遗留 human 项**：① 真机轮 human 在场（问天气全链路 + 降级路径一次）；② supervisor 若判 all-mock 需 human 放行（launch-card ⑦ 前置申报）；③ headless 启动由主线程代跑（human 既定口径）；④ loop-retro advisory 持续在账（17 ≥ 3 阈值）。

## 2026-10-12 · 261010-p3-storylist-cold-start-restore launch prep（单包批；spine 三面本会话收齐 + requirements v1.63 定稿 + 契约门 HUMAN_APPROVED）

- **调度判定**：本批 = 单包 `261010-p3-storylist-cold-start-restore`（②单包 Full riskTier medium；triage 2026-10-10 architect review + human 触点①裁：restore 标记方案、②单包 Full）。来源链 = TECH-261010-p3-storylist-cold-start-empty（真机反馈第 5 条「P3 停默认空态、没有把已有的 story list 列出来」）→ spine 261010-p3-storylist-cold-start 三面 2026-10-12 本会话收齐：需求面档 1 走毕（story-author 起草 → requirement-reviewer 四查 PASS → v1.63 frozen；human 话术三答在案）→ 架构镜像同批落（§5.1.2 :186 + §5.2 场景④ :214）→ 契约面 architect-reviewer 四分类（面 A CONTRACT_CHANGE_REQUIRED / 面 B NO_CONTRACT_CHANGE）→ human 契约门 **批准**（CCP = taskpackage contract-gate/，D1–D4 先批后落、与本包同批原子落）。串行接力沿常驻树（上一包 261012-kidpal-realtime-facts 已 merged）；**launch 形态 = 本树既有会话即 Pod 本体就地起 loop**（m2-char-window 先例；human 2026-10-12 指示「我们开始处理这个吧」）。
- **树同步**：常驻分支 dev/260915-m1-k1a-key-layer-dispatch = main tip fde1eb9、零独有提交；本批 prep 制品 commit 后 main 纯落后 ff。
- **双面基线锚 @ fde1eb9（本树实测，producer 五要素）**：
  - server 面：`python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/acceptance` → **152 passed / 0 failed / 4 deselected / 9 subtests passed，exit 0**（2026-10-12；面域 = server）。零红。旧锚 145P @9d07329 作废（差异判停命中：realtime-facts 批 server 面 8 文件入账须重跑，实测 +7 齿）。
  - firmware host 面：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf` → **155 Tests / 0 Failures / 0 Ignored，exit 0**（2026-10-12；面域 = firmware；热缓存 ninja no work to do）。零红。firmware 面 git diff 9d07329..fde1eb9 零差异，与 261012 锚 155T0F @9d07329 持平。
- **机械门**：`settings-lint.mjs` 3 files / 0 RED / 0 WARN；`portfolio-reconcile --check-launch 261010-p3-storylist-cold-start-restore --base-ref main` → prep 制品 commit + main ff 后复跑（首轮 blocking = taskpackage.json 未进基准 ref，属时序预期）。
- **建包件**：taskpackage.json 随本批补齐（triage 建包职责：origin regression 型 upstreamRef 指 TECH CRQ 条目 + upstreamFrozen 记 spine 三 anchors + planRef 回指 sibling plan.md；acceptance 齿 SL-A1~A6 预填、红基线/变异骨架字段留 acceptance-author 冻结轮回填）。
- **board 预填**：queued + worktreePath = C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch + mergeOrder 49（前批 261012=48 后继）。
- **启动卡**：`delivery/taskpackages/261010-p3-storylist-cold-start-restore/launch-card.md`（五要素 + intake 特别注意 ⑧ 条：CCP 先批后落通道 / FIX-A1 翻正授权链 / schedule :926-991 分流形态复刻 / ECS 开关默认关 / all-mock 前置申报 / 真机轮 human 在场 / ctx(story) 归并处置归本包）。
- **图索引**：`C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` mode=full 刷新随本批执行。
- **遗留 human 项**：① SL-A5 真机轮 human 在场持板（重启冷启动翻 P3 见列表 + 空态两态 + 无前台抢占）；② supervisor 若判 all-mock 触 riskTier 重算 high → 需 human 放行；③ ECS 恢复推送开关烧板验证后 human 人工开闸（gate evidence C1）；④ 契约 diff 逐字 patch 过目 → git apply（CCP 先例通道，人工 diff 核对留痕）。

## 2026-10-17 · 261016-p3-story-body-sd-cache launch prep（单包批；spine 三面 anchors 齐 + triage 改判 ②单包 Full）

- **调度判定**：本批 = 单包 `261016-p3-story-body-sd-cache`（②单包 Full，riskTier **high**——triage 2026-10-17 architect 分流原判 ③拆包〔A/B/C 三包、A∥B 波次 1、C depends A〕，human 触点①改判 ②单包 Full；单向守卫：改的是档位、风险级不降）。来源链 = 跨面 spine `delivery/intake/261016-p3-story-body-sd-cache.md`（`/triage-change` 作容器走完需求 / 架构+契约两面）→ 上游 anchors 三面回填齐：需求 v1.64 frozen（S-F2.1 AC2 兑现 + 新增 AC7 + 正文缓存边界 + WF-F2-06 + 设计母版 v6）、架构 261017 批 landing b3f50ebd（决策 24 窄例外 + ADR-011 + 六处禁令重述点）、契约 CDQ-261017 LANDED（§四 `kidpal_story_fetch` 上行帧 + §1.3④ `story_text` 消费语义，形状零改动）。消费 debt = TECH-261013-p3-reader-entry-no-text-silent-noop（含 id 错配缺陷追加段）；留根 TECH-261016（隐私防护 deferred）。
- **launch 形态**：human 2026-10-17 指示「launch prep 完成后就在本 worktree build-feature」——本树既有会话即 Pod 本体就地起 loop（m2-char-window / 261010-p3-storylist 先例），不另建树、不派 headless。
- **树同步**：worktree = `C:/AI/KidPal-pod-m2`，常驻分支 `dev/pod-m2` = main tip b6cc8945、零独有提交；交接卫生核对 tracked 净、untracked 仅本批 prep 制品；固件环境在位（sdkconfig / dependencies.lock / host_app sdkconfig / managed_components 软链 / host build 热缓存）。
- **双面基线锚 @ b6cc8945（本树实测，producer 五要素）**：
  - server 面：`python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m2/server/acceptance` → **174 passed / 0 failed / 4 deselected / 33 subtests passed，exit 0**（2026-10-17；面域 = server）。零红。旧锚 152P @fde1eb9 作废（差异判停命中：fde1eb9..b6cc8945 server 面 36 文件改动，须重跑）。
  - firmware host 面：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf` → **182 Tests / 3 Failures / 0 Ignored**（2026-10-17；面域 = firmware；runner exit 0，判红以 summary 行为准）。**上游已知红 3 条** = `p1_auxbar_style` 的 AB-A1 / AB-A2 / AB-A4：261015-p1-auxbar-style-fix 的 04f8409a「看屏轮打回直修」改了实现（ic_batt_15→ic_batt_20、thermo 落位 x0=218 越出齿域 [211,217]、wifi 位图退役）而冻结齿未同步——**基线本身的消解另立批，不塞给本包**。旧锚 155T0F @fde1eb9 作废（firmware 面 41 文件改动）。
- **机械门**：`settings-lint.mjs` 3 files / 0 RED / 0 WARN；`portfolio-reconcile --check-launch 261016-p3-story-body-sd-cache --base-ref main` → blocking = `delivery/taskpackages/261016-p3-story-body-sd-cache/taskpackage.json`（未进基准 ref，属时序预期——intake 建包首提交后即消）。
- **建包件**：taskpackage.json 留 intake 建并首提交（plan.md 附录「结构化种子」为直接输入：origin proposal 型 upstreamRef = `delivery/review/261017-arch-review-p3-story-body-sd-cache/PROPOSAL.md`；gateProfile 五项；crossEnd 声明 `front_app.py` / `protocol.cc` 两个 counterpart；decisionCoverage 八映射含一条 `dropped`〔隐私只落盘不加防护，reason 引 TECH-261016〕）。
- **board 预填**：queued + worktreePath = `C:/AI/KidPal-pod-m2`；mergeOrder null（非拆包产物）。
- **图索引**：`C-AI-KidPal-pod-m2` 已在册（moderate，ready，19025 nodes / 52704 edges）；复用树随树增量刷新。
- **启动卡**：`delivery/taskpackages/261016-p3-story-body-sd-cache/launch-card.md`（六要素 + 上游已知红 3 条锚 + riskTier high 的真机轮/合并门预期）。
- **遗留 human 项**：① 真机 live-manual 轮 human 持板（SD 读回 roundtrip / 双击三类记录进阅读 / 离线降级提示 / id 错配回归位 / 预取期语音不卡顿）；② riskTier high ⇒ supervisor 合并门升级 human（预期路径，非缺陷）；③ **launch prep 附带发现，建议另立批**：p1_auxbar_style 冻结齿与 04f8409a 直修漂移（3 红），归资产面独立批或 ARQ，不塞给本包。
