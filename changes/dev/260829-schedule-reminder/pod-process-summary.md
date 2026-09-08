# Pod 全过程复盘 · 260829-schedule-reminder（包 7 日程提醒全链路）

> 编制：planner（主线程），2026-09-08，human 指示「把整个过程梳理一下，单独放到 changes 该 pod 目录下」。
> 数据来源：`pod-events.jsonl`（32+ 事件，派发戳含实测时长）、`verifier-evidence/rounds/r1-r4.json`、`review-packet.md`（含附段 A/B）、`red-baseline-summary.md` §1-§8、`docs/ARCH-REVIEW-QUEUE.md`。本文是**过程账**，不替代上述任何权威制品；数字与制品冲突时以制品为准。
> 时区注记：事件戳为 UTC，括号内为本地（+08:00）。总墙钟 09-07 16:25Z（00:25）→ 09-08 08:50Z（16:50）≈ **16.4 小时**，其中 human 等待两段 ≈ 7.8 小时（夜间裁定 6.8h + 契约/授权裁定 1h），**机器活跃 ≈ 8.6 小时**。

## 一、结果一行

本地 main `e8b73c0`（--no-ff，人批凭据 `55f20c9a`，未 push）；集成树 final r4 九闸全绿；fixer **零轮次**；四条人类裁定全部 AskUserQuestion 实得留痕；合并链两次 fail-closed 均无损回退。

## 二、阶段时间线（阶段 / 执行者 / 实测时长 / 产出）

| # | 阶段 | 执行者 | 时长 | 关键产出 |
|---|---|---|---|---|
| 1 | intake + portfolio 探测 + blockingDecision×2（ECS 只读抽样实证） | planner | ~40min | pod running；NOT-BLOCKED×2（cron --command 实证避开 tools 拒收路径；front 进程隔离+缓存有界约束） |
| 2 | 人类裁定：生产分工=混合分层、语音=借引擎 speak_txt | human | （等待 6.8h 夜间） | eventId acaef302 / 09359e74 |
| 3 | 红基线判读口径校正（60T3F 非 2F；闸面 live-mark 整档排除实况） | planner | ~15min | 基线口径入册；host ELF 首次全量编译（WSL ~10min）+ 设备构建基线**红**实证（25min，mcp_server.cc 语法坏编译器取证） |
| 4 | 冻结验收 r1（C 11 齿 + py 32+5 齿，四通道红基线实跑） | acceptance-author | **37.3min** | 两份冻结件 + 红基线摘要；真形状 fixture（ECS 真 schedule.md/cron 记录抽样） |
| 5 | Finalize TaskPackage（路径校正、current 投影刷新、scope-map C5 核对） | planner | ~20min | taskpackage.json 定稿；发现 current 投影残留上一包（verify.mjs 会拿错包判定的静默假绿陷阱） |
| 6 | 实现 r1（20 文件：设备弹窗面+网关投递+生产者脚本+v3 prompt+引擎补丁认领） | builder（opus 升档） | **109.4min** | aad6dba→f3305b5；59a8561 独立修上游设备构建坏；设备构建转绿 |
| 7 | escalation：冻结件两处自相矛盾（fixture 日期簇 / sink 形状） | builder→planner→acceptance-author | 13.9min | 7c6d7c5→a167498；零语义变更自证（独立推导+四条扰动探针）；planner 亲验 33P/5S |
| 8 | verifier scoped r1/r2 + planner 三项亲验（cwd 敏感齿、degraded 闸、投影） | planner（verify.mjs Bash 直跑） | ~30min+等编译 | r1/r2 evidence；唯一红=mutation 证据待产（设计内） |
| 9 | mutation 证据（39 处单点变异，28 py + 11 C，全部还原核 sha256） | builder | **70.7min** | 37 条记录；3 处不敏感如实归因；85f9afa→102c026 |
| 10 | ARCH-ADVISORY：tighten-only 是否合法（human 点名要评） | architect | **9.8min** | 判既成纪律（7 份冻结件自带页脚+先例）；防滥用五条款 |
| 11 | tighten-only r3（A5 清屏齿改道对象残留层 + A3 引擎腿离线齿） | acceptance-author | **24.6min** | 7ab2d47→6cee439；双向自证（新齿咬旧齿绿） |
| 12 | final r3 终审 9/9 绿 + mutation-check 转绿（37 条回填，planner 压缩短 id 自纠） | planner | ~25min | r3 evidence；85f9afa |
| 13 | integrator 收口 r1（review-packet/docs-delta/ADR×2/ARQ×4/progress） | integrator | **71.9min** | 04739e4→c131fda；**发现 .v3 槽位假绿 + 契约 §三 措辞漂移**两桩 BLOCKING 级事实 |
| 14 | architect-reviewer 契约四分类（planner 误判「非契约差」被纠正后补派） | architect-reviewer | **3.7min** | CONTRACT_CHANGE_REQUIRED；改法 A/B 素材；三触碰点判合法解释 |
| 15 | 人类裁定：A′ 热修+不等；integrator 一次性授权；ARQ 登记 | human | （等待 ~1h） | ec77d40a / dc9a5c15 |
| 16 | tighten-only r4（carrier 宽词齿收紧 + 同源审计挖出 `-1` 逃逸支） | acceptance-author | **22.0min** | a53a6ff→c3479fa；12 变体自证；main-only 假绿复现→闭合 |
| 17 | ARCH-ADVISORY：合并面四组问题（human 点名「先 review」） | architect | **22.7min** | A′ 精细化；并集正解独立复算；**执行者死结判定=integrator**；3 条新事实 |
| 18 | 集成 r2（merge main 形态，10 冲突消解+集成树五组复跑+ARQ 行） | integrator | **67.1min** | 44ded5c 等 4 提交；86T/3F、8F/156P/41S、设备真编绿 |
| 19 | supervisor 机械闸=ESCALATE + r4 集成树补轮 + 人批合并 | planner+human | ~15min | r4 evidence；supervisor-verdict.json；55f20c9a |
| 20 | merge-driver 第 1 跑：**fail-closed**（形态不兼容，见坑 9/10） | planner | ~5min | 无损回退；backup ref；friction a0a4f270 |
| 21 | 集成 r3：真 rebase 线性化（18 停逐停消解+硬不变式树逐字节相等） | integrator | **47.1min** | 9aadb9b/298cbe6；`git rebase main`=up-to-date 自证；hash 映射表 |
| 22 | merge-driver 第 2 跑：rebase OK→quick-check OK→**merge OK** | planner | ~3min | **e8b73c0 入 main**；board merged（第 12 个）；token 释放 |

子代理墙钟合计 ≈ **8.25h**（12 次派发：acceptance-author×4、builder×2、integrator×3、architect×2、architect-reviewer×1）；fixer **0**；verify 轮次 r1/r2 scoped + r3/r4 final；人类出场 5 次（裁定×2 批、tighten 评估指示、A′+授权、合并批准）。

## 三、坑清单（16 条：现象 → 根因 → 代价 → 记录落点 / ARQ 状态）

### A. 上游遗留与环境类

| # | 坑 | 根因 | 代价 | ARQ / 落点 |
|---|---|---|---|---|
| 1 | **main 设备构建自 char-stroke merge 起即坏**（case 3 插进 case 4 的 lambda 体内，收尾三行被挪位；编译器实证 `case label '3' not within a switch`） | merge 后无人复跑设备目标构建；host ELF 独立工程不编该文件 | 本包与 story-p3 **各付一遍**同三行修复（我们 59a8561 最终 rebase drop） | ✅ ARQ-260908-firmware-build-degraded-gate（story-p3 登，本包 review-packet 补事实） |
| 2 | **firmware-build 闸恒 `degraded:true` 放行**（196ms 未编，ok:true≠已核验）；planner 一度把它当「闸绿」对外声称（自纠） | 闸环境无 idf.py，设计 fail-open 但 verdict 不降级 | 设备构建证据全靠人亲跑（基线红 25min + 验证绿 ×3） | ✅ 同上 ARQ（建议 VERIFY_FW_BUILD_REQUIRED=1 机械门） |
| 3 | **「上游已知红」计数无机械来源**：launch 面写 2 条，实测文档命令位置 3 条 | 手写快照，无脚本产出 | 红基线/mutation 判读口径人肉对齐一轮 | ✅ ARQ-260908-known-red-count-no-mechanical-source |
| 4 | **VRT-A2 证据齿吃 cwd**：同一 ELF 从 host_app 跑 3F、从仓库根跑 2F（证据文件 blob 未变、5 组标记全命中） | 齿的 root 定位对调用目录敏感 | 「2 vs 3」分歧排查一轮；观测口径被迫钉死单一 cwd | ✅ 同上 ARQ（evidence 齿 root 应显式注入） |
| 5 | **闸内 pytest 裸跑收集陷阱**：`--import-mode` 缺失时，本仓 `test_*.acceptance.py` 制式文件名（中段点号）即触发 ModuleNotFoundError；今天恰被「8/8 档含 live 标记→整档排除」遮住 | 闸执行器命令与仓内文件名约定不兼容 | 未爆（遮蔽态）；任何人加不含 live 标记的档即红闸 | ✅ ARQ-260908-gate-pytest-import-mode-incompat |

### B. 并行批 / 版本化制品类

| # | 坑 | 根因 | 代价 | ARQ / 落点 |
|---|---|---|---|---|
| 6 | **`.v3` 槽位 add/add 争抢 + 假绿方向不对称**：单边取 main 时提醒约定 181 行静默消失而 carrier 齿仍 PASS（宽词 needle 被课表段偶然命中） | `.vN` 是全局计数器、各 Pod 本地自增；批内无领号协调 | 一轮 architect 复算 + acceptance-author 第四单收紧（22min）+ 消解规则预登记 | ✅ ARQ-260908-versioned-artifact-slot-collision |
| 7 | **多文件版本指向一致性靠人读**（carrier/overlay/provider 三处指一版；本侧 carrier 前言散文陈旧仍写 v1，与自己的渲染脚本矛盾） | 无 repo 级机械门（ADR-001 不变式只有包级齿，集成树上没人跑） | architect 建议 deploy-wiring-check 升格 repo 级门 | ✅ ARQ-260908-versioned-guard-version-literal-coupling（story-p3 侧登）+ advisory 建议归治理批 |
| 8 | **投影/append 共享文件冲突预期**：current/*.json 冲突如预期发生（特判 --theirs 生效）；但 progress.md/INDEX.md 实测 ort 自动合并干净，与在册 ARQ「必冲突」预期不符 | — | 无（如实登记差异） | ✅ ARQ-260907-merge-closeout-bookkeeping-gap（在册，本包附实测差异） |

### C. 集成 / 合并链类（本 Pod 新暴露，全部并证入一行）

| # | 坑 | 根因 | 代价 | ARQ / 落点 |
|---|---|---|---|---|
| 9 | **Pod 分支含 merge-main 提交与 driver 朴素 `git rebase main` 结构性不兼容**：重放 16 提交在 aad6dba 撞墙 → abort+blocked。integrator 的前提「main 是祖先⇒rebase no-op」为假（`merge-base --is-ancestor` 只保合并语义、不保重放），planner 采纳过快 | runbook 未写死集成形态；driver 契约（线性）无处声明 | 一轮集成返工（67.1min 的 merge 形态作废 + 47.1min 重做）；hash 全改写需补映射表 | ✅ ARQ-260908-pod-integration-no-legitimate-executor 事实①（含「禁在 Pod 分支引入 merge commit」写死主张） |
| 10 | **`approvalRefUnlock` 混用两个 worktree**（cwd=findWorktree 解析到 main holder、--project-dir 留在 Pod）→ blocked 态人批解锁路径拿 main 的 HEAD 判 off-main，**恒 fail-closed**；常规路径 :1137 传参反而正确 | 双源解析未对齐 | driver 第 1 跑白烧一轮；恢复靠 ARQ-260907-merge-driver-approval-state-desync 记载的流程外知识（手动置 ready-to-merge） | ✅ 同上 ARQ 事实② + desync ARQ 近亲 |
| 11 | **autocrlf 物化 CRLF 咬 sha256 闸**：`git checkout --ours` 后投影文件 46051B/CRLF vs canonical 45618B/LF，taskpackage-required-check 逐字节比对误红 | core.autocrlf=true + 闸做 sha256 等值 | r2 集成时闸红一次，归一 LF 后复绿；r3 改为消解时即核 | ✅ 同上 ARQ 面 A 消解期陷阱 + review-packet 附段 A.4 |
| 12 | **supervisor-verdict.json fs-write 后无人提交**，合并候选树缺该制品（planner 树净检查恰被 `grep -v .codebase-memory` 漏看一行） | 机械闸只写不提交，收尾簿记无主 | 补提交一次（00df6a5） | ✅ 同上 ARQ 事实③ |

### D. 验收面类（包内闭环，非治理缺口）

| # | 坑 | 根因 | 代价 | 落点 |
|---|---|---|---|---|
| 13 | **冻结件 fixture 日期簇自相矛盾**（钢琴课期望落在已过的一周，与同档 past-skip 齿无共同解）+ **a4 sink 采集形状与自身解包冲突**（ValueError） | acceptance-author r1 笔误级缺陷 | 4 齿残红一轮；r2 修复 13.9min（扰动探针自证零语义变更） | 包内闭环（a167498）；红基线摘要 §6 |
| 14 | **两类同族假绿**：carrier 宽词 needle（见坑 6）+ C 档降级判据 `\|\| strstr(buf,"-1")` 逃逸支恒真（reminder_page.c 内 `-1` 47 次、全树 136 文件同态） | 断言用了低鉴别力锚 | r3/r4 两轮 tighten-only（24.6+22.0min） | 包内闭环（6cee439/c3479fa）；规则缺口→ ✅ ARQ-260908-tighten-only-unwritten（成文 §5 + 防滥用条款） |

### E. 契约 / 治理面类

| # | 坑 | 根因 | 代价 | 落点 |
|---|---|---|---|---|
| 15 | **契约 §三 + spec×3「排程时预合成」措辞 vs 裁定二行为矛盾**；planner 初判「非契约差」**错**（自己引用过 :216 原文没意识到）；dev-loop 无人可写 spec/CDQ（17 角色逐一核过） | 裁定改了机制、canonical 未随批同步；「交回→在册」零机械追踪 | architect-reviewer 四分类 3.7min + 热修文本待治理面落地（**仍开放**） | ✅ 人批 A′（ec77d40a）；CDQ 行+spec 三处=治理面待落；面 B 证据→ ARQ-260908-pod-integration 行内 |
| 16 | Intent:nointent（tools 拒收）与 char_cache ~19MB 内存压力两条留根 | 上游在册风险 | intake 评估各一轮（ECS 实证 NOT-BLOCKED；cron --command 路径根本不触 LLM tool calling） | 无需新 ARQ（ARQ-260907-openclaw-tools-rejection-rootcause 仍开放，本包未触该路径；内存约束写进实现注记并冻结成 MAX_* 常量） |

**ARQ 对账小结**：16 坑中 12 条已进 ARQ 台账（260908 六行 + 260907 两行在册补证），2 条包内闭环（13/14 的修复本体），1 条待治理面落地（15 的 CDQ/spec），1 条评估后无需新账（16）。**没有一条只留在聊天记录里。**

## 四、planner 自身错误账（全部自查自纠、pod-events 留痕）

| 错 | 纠 | 事件 |
|---|---|---|
| 「V20 口径差非契约差」对外说过 | integrator 纠正→architect-reviewer 重归类→向 human 明示撤回 | ed71e847 |
| 「冲突以先入 main 者为准」对 .v3 有害 | 作废改判并集+三面同批指版 | ed71e847 |
| 「firmware-build 闸绿=修复兑现」说过头 | 读 degraded 字段后撤回，改亲跑真编为凭 | 7fbdc520 |
| 事件类型误登 human-decision（planner 注记） | append-only 不回改，追加自纠指针 | ea6ff84a |
| stamp 占位 duration=1000ms | 以 evidence 实测起止重记（1015ms）并注自纠 | 重记戳 |
| 采纳「merge 形态 no-op」前提过快 | friction 留痕+形态改判+硬不变式兜底 | a0a4f270 |
| 后台复合命令 exit 码误读（`build;echo;grep` 取末条） | 改看日志本体；同型陷阱第二次（driver|tail）当场识破 | 27f2f32c |

子代理误报被 planner 亲验抓回 2 起：builder「76T/2F、VRT-A2 转绿」（实为 76T/3F，证据 blob 未变=无造假，属 cwd 误读）；integrator「host_app CMakeLists 自动合并干净」（实测 content 冲突）。两起均如实入册、未影响信任处置。

## 五、遗留与交接（合并后仍开放）

1. **契约 A′ 热修落地**（human/主 checkout 治理面）：契约 §三 :216 + spec :331/:332/:601 + CDQ 行，文本已在会话记录与 review-packet；dev-loop 无写权（坑 15）。
2. **live 5 齿**：ECS 部署（cron 声明+排程脚本+prompt v3 overlay+引擎补丁认领 kidpal_speak，runbook §8）+ human 批准后跑；真机音效/亮屏/弹窗并发时序殿后 9b。
3. **治理批欠账**（本 Pod 贡献 6 行新 ARQ + 3 行在册补证）：tighten-only 成文、集成执行者归属（integrator merge-chain execution 节 + settings git 条目 + runbook）、degraded 闸真编门、known-red 机械快照、pytest import-mode、版本槽领号 + deploy-wiring repo 级门；另 loop-retro 阈值已达（5 pending + 2 waiting，发起责任=主线程）。
4. **残余接受面**：acked_ids 宽容分支（已加 docstring 钉行，未来消费方必须走 consume_receipts）；view_json 不入队列槽（升级路两条写死在 reminder_page.h）；MAX_TRACKED 单点冗余（配对变异已证属性被钉）。
5. **回滚保险**：backup/pre-integration（a53a6ff 旧系）、backup/integrated-union-1bad3e7、backup/integrated-tip-cb3a0f6 三 ref + reflog；main 稳定一段后可清。
6. 旧→新 hash 全映射在 review-packet 附段 B.3（rebase 改写全部 hash，旧对象经 backup ref 可达）。

## 六、一句话教训（给下一个 high 双面板包）

**批内先领版本号，集成只许 rebase 形态，任何「闸绿」先读 degraded 字段，任何外部世界断言先打真记录抽样，任何「应该没问题」的前提自己花两分钟证伪**——本 Pod 全部返工都出在这五句的反面上。
