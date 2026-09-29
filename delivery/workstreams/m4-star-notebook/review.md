# review · m4-star-notebook

> workstream 级门审裁决耐久记录（append-only；行不覆盖）。

## 契约预审 · architect-reviewer 四分类（2026-09-24）

- **终裁决**：**NEEDS_REVISION → CCP v2 修订并入（主线程同日）**——方向正确、Alternatives 可信（含与 ADR-005「无 device RPC 先例」边界一致性核实），但分类表/Scope/Impact/Human Question 有实质修订项 7 条；v2 落盘后状态 = 待 human 契约门裁十问。
- **审者**：architect-reviewer（只读预审，不自证 eligibility；moderator 复判不受影响）；主线程派发。
- **被审制品**：`delivery/taskpackages/260924-m4-k1-story-cloud-link/CONTRACT-CHANGE-PACKET.md`（DRAFT 初版，三触点 + 五问）。

### 四分类裁定（v2 §0 收编）

| 触点 | 分类 |
|---|---|
| ① §1.3③ story_list total 语义拆分 + 行为句改写 | spec↔contract 漂移（CDQ 行沿用 `ACCOMPANIES_SPEC_CHANGE` 标签保队列一致） |
| ②a §1.3④ story_text 增 data.seq | **独立契约变更**（CONTRACT_CHANGE_REQUIRED——spec v1.47 只钉 N 语义不钉载体；先例 = total CDQ-260829 / upcoming CDQ-260921） |
| ②b §1.3④ 朗读句清理 | spec↔contract 漂移（v1.52 砍朗读） |
| ③ §四 kidpal_story_delete 新增 | **独立契约变更**（CONTRACT_CHANGE_REQUIRED；判据一致先例 = CDQ-260914 kidpal_session_ctx） |
| §1.4 control 表 | 无需改（NO_CONTRACT_CHANGE，丙案否决成立） |

初版 CCP 头部「三触点一律随 spec 改动携带（同 PR）」= 误标（低估 ②a/③ 契约发明度），v2 已拆标。

### 修订清单 7 项（v2 并入状态）

1. 分类拆标 → v2 §0 裁定表。✅
2. Scope 补：story.py 三处钉行（:12-13「契约零改动红线」/:181-183 total 兼两义/:211 恰为三键）+ §1.6 模板 story 段（Q9）+ 节标题术语（Q1）+ display_schema「收紧」失真修正（现状 story_* 零形状 floor，实为新增校验面或维持零校验二选一，Q8）。✅ v2 §1 钉行收口面表
3. items 上限与板端 `KSTORY_LIST_MAX=16` 硬冲突纳入（漏问点 1——超限整单 -1 降级不静默截断，story_page.c:556-564；「全量推送」假设与之矛盾）。✅ v2 Q2 三案（拟乙案抬 cap）
4. Impact 修正：撤「无双源漂移」断言（v6 prompt :153-155/:271-273 仍指示 LLM 产 story 块且 OpenClaw 无 story 工具、一回合不读写文件 ⇒ 臆造双源）+ v6 钉行清单 + 旧板端盖章面注（stamp_index=counter+1 本地现算，story_page.c:1071——窗口内删除致 N 分叉，风险低如实登记）。✅ v2 §3
5. Alternatives 补删除无应答可靠性（漏问点 5——delete 帧丢失 → 重推复活）。✅ v2 §2 + Q5（拟隐式 ack + 超时重发，不破 ADR-005 边界）
6. Human Question 五问扩十问（漏问点 1–7 全并入：16-cap / seq 缺省降级演出 / 删除可靠性 / 硬删软删与审计 / 连带边界与家长通道 / 双源窗口处置 / display_schema floor / §1.6 story 段 / 术语统一）。✅ v2 §4
7. 收口义务补 pass-oracle 第 4 关人工 diff 核对清单（钉行四处 + Q2/Q5/Q7/Q8/Q9 落文处 + 变更记录行）。✅ v2 §3 随包收口义务⑤

### 主线程对磁盘核验（预审关键主张逐条，全属实）

`KSTORY_LIST_MAX=16`（story_page.h:40 逐字「超出 = -1 降级, 不静默截断」）；解析超限 -1（story_page.c:556-564）；`stamp_index = v->counter + 1`（story_page.c:1071）；story.py:12-13/:181-183/:211 三钉行逐字在案；v6 prompt :75-76 view 表 story 形状 + :153-155「total=全集总数…盖章『第 N 颗』据此渲染」+ :271-273「故事果计数取 story_list.total（全集）」；display_schema.py story_* 仅白名单无形状 floor（全文唯一 shape floor = schedule :87-105）；契约 §1.6 模板仅课表段无 story 段（:177）。

### 连带修订（v2 波及面，同日主线程落）

- K1 plan.md：取舍点 A 补隐式 ack 变体与 Q5/Q6 指针；取舍点 E 改「16-cap 硬冲突 + Q2 三案」撤「全量载荷已覆盖」断言；新增取舍点 F（双源窗口 Q7 / display_schema Q8 / §1.6 Q9）；钉行收口面从「如有」改定三处；阶段划分 builder 步同步。
- K1 taskpackage.json：A1 改十问裁定定稿口径 + 人工 diff 核对清单；A3 补 Q2 上限依赖（裁定前不冻容量齿）；A4 补隐式 ack 承接与 Q6 指针；notes 追加预审记录与核验结论。
- K2 plan.md：删除发送补隐式 ack 超时重发（Q5 拟形态）；新增「items 上限随契约门 Q2」取舍条（cap 常量重冻纪律）。
- decision-coverage.json `_doc` 契约面裁定条更新（拆标 + 预审已跑 + 十问要点）。

## 门 1 · 拆包切法轻量 checklist review（2026-09-24）

- **终裁决**：**verdict: GREEN**（首轮即绿，无 blocking finding；advisory 4 条 + nit 5 条（F1–F9）按处置注记同日吸收，处置表见下）。
- **审者**：architect（只读轻量 checklist review + 磁盘抽查；主线程派发）。
- **被审制品**：本 workstream 四件（roadmap.md / decision-coverage.json / progress.md / task_plan.md）+ 三包 taskpackage.json/plan.md + K1 包内 CONTRACT-CHANGE-PACKET.md（v2）+ 上游锚 `delivery/milestones/milestones.jsonl` 行 `M4-star-notebook` + `docs/requirements.md` F2 节（S-F2.1~S-F2.4 + WF-F2-01~05 + 对账块）/ §2.1 / §2.3 / §6 阶段四 / §8 已砍清单 + `docs/architecture.md` §5.1/§5.2④/§5.4/§5.5.4/§7 + `docs/contracts/interface-contract.md` §1.3③④/§1.4/§1.5/§四。

### checklist 五项终态

1. **切片逻辑 — 过**：三包垂直切片成立（K1 云端数据能力+契约 offline 独立验证 / K2 板端展示面 host+device 可演示 / K3 跨端会话链路+live 收口）；「K2 再拆二」备选否决理由显式（story_page.c 单文件族、徒增合并冲突面）；K2/K3 同文件缝切分清晰（story_page「K2 落输入表语义位+回显占位缝、K3 接会话动作」双 plan 互认；mcp_server 双写有 crossPackageBehaviors 专条 + mergeOrder 串行 + intake rebase；单常驻树接力棒无并发双写窗口）。
2. **依赖序与 WIP 合理性 — 过**：dependsOn 与 roadmap 三边、mergeOrder 38/39/40 拓扑一致（board 预填已核）；live 半全挂 K3 五处互锁声明清晰；契约门时序成立（LP1 = K1 包级硬前置；K2 形状齿「契约 diff 未落地前不冻」条款门控，M3 同款）；单常驻树接力棒可行（behind 8 实测吻合、base ref + launch-prep 相位齐）。
3. **decision-coverage 完备 — 过**：milestones 原 nextAction 两块（git diff 旧行逐字核对）14 条 decisionCoverage 全覆盖；S-F2.1 AC1–4 / S-F2.2 AC1–4 / S-F2.3 AC1–2 / S-F2.4 AC1–2 逐条追核均有 landsIn 归属、板端/云端/live 半显式；dropped 七项 reason 达地板且实质成立；crossPackageBehaviors canonical 四键合规（carriedBy ?? owner；K3 包级第 7 条聚合缺口 = F6 已修）。
4. **每包意图陈述一致 — 过**：三包 story 字段全为锚点引用形态（M3 门 1 F1 修复后先例格式逐字同构）；四标题及 AC 号集与 requirements 现文全部吻合；v1.26~v1.57 口径逐条对上；intake 两道机查主张复核成立（§8 只含子项、四 story 本体不在其中；可行性全 ✅ requirements.md:420/435/448/461）。
5. **demo 证据形态可行 — 过**：K1 offline pytest + 契约门十节 evidence；K2 WSL host runner + device build 闸 + deferred 声明；K3 host 齿 + prompt v7 版本化 offline 断言（v7 槽位空闲实证）+ human 持板真机轮；「LLM 意图判定归真机目击」诚实声明在案（K3-A7 note）。

### findings 处置表（F1–F9，主线程同日吸收）

| # | 级别 | 要旨 | 处置 |
|---|---|---|---|
| F1 | advisory | S-F2.1 AC3 现文单指令与对账块③/设计稿 v5 双指令张力，K2 docs-sync 销账面未含 → 落地后 AC3 成 requirements 唯一残留单指令表述 | **已吸收**：K2 docsSyncReason 补「AC3 引文陈旧口径清扫同笔对齐（v1.33 清扫同类编辑动作、变更记录行留痕）」 |
| F2 | advisory | m3-k7 pod/board 协调态陈旧（blocked vs 实已 merged，git 实证 0fe04bf/73fa08b）——K1 launch prep check-launch 可能误读本树占用 | **已修**：pod 态翻 merged + portfolio-reconcile 重算（单一写者纪律；迟翻原因 = 260923 人工 ff 合并未回写协调态，如实留痕 progress.md） |
| F3 | advisory | arch §7 契约门在办①（schedule 周网格）已随 M3-K1 落地仍列在办，K1 docs-sync 只销②④ | **已吸收**：K1 docsSyncReason 补「在办①同笔核销（M3-K1 遗留漂移，本笔顺带收）」 |
| F4 | nit | K3 plan protocol 枚举不全（漏 SendAudio/SendAbortSpeaking/SendMcpMessage），实质主张成立 | **已修**：K3 plan.md 背景摘录改「既有 Send* 面 = 音频/唤醒词/聆听起停/abort/mcp，无 KidPal §四私有消息原语」 |
| F5 | nit | K3 docsSyncReason「§5.5.5 现役 v6→v7」与 arch 正文「现状 = v4」不符（双重漂移） | **已吸收**：K3 docsSyncReason 补「正文 v4→v7 + 补 v5/v6 沿革 + 视情更新 §8.1 债 7」注记 |
| F6 | nit | K3 包级 crossPackageBehaviors 第 7 条（删除发送原语复用 owner=K2/carriedBy=K3）未汇入 workstream 级聚合 | **已修**：decision-coverage.json 补第 7 条（包级登记为准、聚合同步收录） |
| F7 | nit | §2.3 对账⑤的 P3 阅读态页内实例（KEY 长按退出回列表）销账归属未点名 | **已吸收**：K2 docsSyncReason 补「⑤ P3 阅读态页内实例随本包收口」 |
| F8 | nit | input-brief 契约锚 `#§1.3③/#§1.3④` not-found（brief-inputs.mjs 锚模式不匹配「#### ③」标题实形；brief 已诚实标注非权威） | **留观不改**：维持 not-found 诚实标注（消费方回退读 SSOT 无实害）；脚本锚模式兼容 = 治理机检面改进候选，登记留 launch prep / loop-retro 议——不在拆包批改脚本 |
| F9 | nit | K1 riskTier=medium 但同 PR 携带契约 diff——supervisor 合并须按 M4K1-A1 + 十节 gate evidence 核验放行（review 面复述供合并门对表）；CCP 初版四分类拟形误标已由契约预审拆标（v2 §0） | **在案**：plan 铁律 + A1 已含约束；合并门对表用本行 |

### 实现现状主张抽查（a–g 七条全部属实 + 附带实证清单）

a) story_page.c 输入表旧口径（:790-796/:840-843/:812-814）+ HandleStoryKey 全仓生产唯一调用点（waveshare-s3-rlcd-4.2.cc:676）；b) story_page.c 字体引用数 = 0；c) list_stories 生产调用点 = 0（仅 archive 命中）；d) kidpal_session_ctx 固件+front 命中 0 文件（契约 :259 + CDQ LANDED 行在案）；e) v6 故事意图单指令（「帮我记下来」deploy 全目录 0 命中）；f) 盖章 N 无持久源（板端 stamp_index RAM 派生 :753/:1071-1072 + story.py seq/ledger/delete 全 0 命中）；g) story_p3 旧冻结验收双侧 archive 在盘、非 archive 目录 story 类验收 0 件。附带实证：KSTORY_STAMP_FMT 现文「故事徽章」（:27）、页名「我的故事」在世（:1222）、art 资产 ic_star 五尺寸+rec_wave+pose_jump_96+stamp_seal_r8 全在清单、双击窗 350ms 实参（board:627）、font_puhui_14_1 恰 5 处先例（lcd_display.cc）、char_window idle 300000UL、层栈六值+STACK_MAX=8+append-only 授权、ses_machine_dispatch 生产调用零、pending.json 原子写先例（reminder_schedule_lib.py:393）、home_upcoming.c 字体 0 引用（M1 面挂账成立）、board 三包 queued 38/39/40、behind main = 8。

### workType 分流

| 包 | workType | 说明 |
|---|---|---|
| 260924-m4-k1-story-cloud-link | 新建型 | 云端四缺口（拦截/推送 producer/删除执行/seq 账本 + 收尾词/120s）全新面；StoryAccumulator 既有语义零偏移增量扩展；契约三触点随包同 PR |
| 260924-m4-k2-p3-star-notebook-ui | 新建型（重写式） | story_page.{c,h} 输入表按 v1.47 矩阵重写 + 四态渲染对齐（旧冻结面随 archive 退役不走 supersede；四态骨架/盖章几何/fresh_save 链保留）；删除覆盖层 + 键路由 + 字体接线新建面——M3-K2「整页重写=新建型」同款 |
| 260924-m4-k3-record-session-live | 新建型 + 验证尾 | session_ctx 发送原语/引导态/回显消费/prompt v7 全新建；live 确认轮一次性收口全链 = 验证尾（意图判定归真机不造假绿，M3-K3 同款） |

### 附注

- 三包 plan.md「背景摘录（新建型锚「现状与差距」）」节名与 task-intake-and-slicing 骨架逐字一致。
- 无触 Track A 升级判据新增面：契约触点按设计内路径路由（architect-reviewer 四分类预审已跑 + human 契约门 = LP1）；门 1 本身 = Track B 常驻轻路径内拆包门。
- 拆包执行体注记（主线程以 portfolio-planner 纪律执行、非 --agent 形态）已如实记录，审者认定不构成 finding。

## human 两判断点落定（2026-09-24）

- **过目切法**：human 认可三包拆法（AskUserQuestion 选项「认可三包切法」；K2 再拆二备选维持门 1 否决结论）。
- **契约门 HUMAN_APPROVED（十问）**：Q1/Q3/Q4/Q5/Q6/Q7/Q8/Q9/Q10 按拟答认可；**Q2 改判丙案**（≠拟答乙案）——story_list items = front 恒推最新 16 条窗口 + total 恒全量、板端 `KSTORY_LIST_MAX=16` 维持现值不抬；知情注记（长列表跳屏动机面收窄 + 旧条目设备端浏览/删除可达性收窄）已在 CCP §5 Q2 行显式登记，旧条目可达面如未来需要 = 新需求另立（甲案族触 device RPC 边界届时再裁）。裁定逐字落账 = CCP §5；传播面 = K1 plan 取舍点 E / K1-A3 / K2 plan cap 条 / K2-A2 / decision-coverage _doc 追记（同日）。
- **launchPreconditions 状态转移**：LP2 → verified（门 1 GREEN + human 过目）；LP1 → ruling-approved（十问裁定在案；待逐字 patch human 过目 → git apply 落地 + 收口义务同批）。
- **下一步（M3 先例序）**：常驻分支同步 main（behind 8 实测 + ahead 1 = 260924 ops-record 留档 commit，先核内容是否已在 main）→ commit-before-launch（拆包制品 + base/m4-star-notebook ref）→ 契约 diff 落地（乙通道：patch 过目 → git apply → CDQ/ADR/变更记录行/gate evidence/钉行收口同批）→ K1 launch prep（check-launch / launch-card / 图索引 / 环境等价化）→ launch 由 human 执行。

## 门 1 · 重拆轻量 checklist 重审（2026-09-29）

- **背景**：human 指示「M4 只开发 K1，剩下的按新定义的拆包规则重新拆包、覆盖」——剩余 K2/K3 按 260928/260929 拆包新规覆盖重写（切法维持两包边界、制品形态翻新 + defer 包后果吸收），K1 不动。判官 = architect（独立实例、只读审、git show HEAD 对照旧版）；本节由主线程代录裁决。
- **verdict：GREEN**（五项全过；附 4 项机械级建议，已同批全部落盘，无需重开门 1）。
- **checklist 逐项**：① 切片逻辑 PASS（K2/K3 边界与 09-24 human 裁定一致且 DC _doc ⑦ 显式记录非静默沿用；垂直切片 + 尺寸维四判据复核过——story_page.{c,h} 文件面 K2 独占、mcp_server.cc 39→40 串行、fixBudget=2、N=2）；② 依赖序与 WIP PASS（render-roadmap --json 实跑核证 4 包模型含 defer 包 M4-N1b achieved；LP4 与 dependsOn 自洽；K1 merged 前提逐条核到契约现文 §1.3③:92/§1.3④:112/§四:266–267）；③ decision-coverage 完备 PASS（15 决策条 landsIn 全闭合、重裁条双登记、8 跨包条 carriedBy 显式、packageAcceptance 退役后承载核经逐条比对无 load-bearing 丢失；WARN 一条 = 跨包条 1 note 陈旧——已刷新）；④ 意图一致 PASS（story 锚点逐字核对 S-F2.1/2.2/2.3/2.4 现文标题与 AC 号集；defer 后果吸收与 defer 制品逐项一致；uiux L2 判级成立——PNG 五面在盘 + 版本行分形 v1.57≤v1.60 零 P3 牵连；误删核查结论 = 无「只有 RTE 知道的跨切片约束」丢失）；⑤ demo 证据形态 PASS（K2 host+device build+deferred 挂账；K3 offline 齿+prompt v7 断言+live 八步目击，唯一真机包成立；LP7 判据指针正确）。
- **建议小修落盘记录（本批同日）**：① K2 uiAcceptanceRefs 两枚 ID 对齐权威锚名（p3-list→canvas-p3、p3-stamp→p3-stamp-frame）；② DC 跨包条 1 note 刷新对齐 K1-merged 现实（③ 项 WARN 消解）；③ K3 plan「LANLED」笔误改 LANDED；④ 契约 §1.5 第 6 行括注（story_list 三时点枚举）陈旧漂移登记进 K3 docsSyncReason——处置路由 = 随 C4 human spec 批经 architect-reviewer 四分类预判，不在包内自改契约。
- **实现级细节回补（判官点名最值得两条，已落 taskpackage notes）**：K2 ⑧ 阅读分屏 >12 截断回收（防 builder 保留旧截断）、⑨ story_list 重推到达校准幂等消费；K3 ⑧ 标题 ≤12 字、⑨ UTT_START 边、⑩ LLM 意图不可 offline 机判诚实声明。另补 K2 sourceRefs 一枚 S-F2.2-AC1（录音态渲染半锚覆盖完备化）。

## human 判断点落定（重拆批，2026-09-29）

- **过目切法**：human 认可两包切法并落 commit（AskUserQuestion「认可，落 commit」；门 1 重审 GREEN 前提下 K2/K3 边界维持、新规制品形态翻新 + defer 后果吸收进 K3 均在认可面内）。
- **milestones M4 行 state 补翻**：planning → in_delivery（human 裁「现在翻」——K1 已 merged 本地 main、补落 09-24 应随 launch 翻账的欠账）。
- **C4 arch 收口**：human 裁「现在就批」——主线程备 patch 材料（K1 包 docs-delta.md 五 hunk、措辞以 defer 包修订版为准：先替换陈旧新文再 apply）→ human 过目批「落」→ git apply；契约 §1.5 括注漂移（story_list 三时点枚举陈旧）维持登记、走契约门路径（architect-reviewer 四分类预审 + human 契约门），不在本批顺手改。
- **launchPreconditions 状态转移**：LP2 → verified（门 1 重审 GREEN + human 过目）；LP3 待重拆批 commit 后回写。
