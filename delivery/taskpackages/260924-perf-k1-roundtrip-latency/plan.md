---
rigor: full
riskTier: medium
sourceType: human-ad-hoc
reviewed-by: architect advisory(260924,architect-review-260924.md,判定:Track B dev taskpackage 成立/不触契约/medium;D1~D4 修正已全部吸收)
date: 2026-09-24
状态:FROZEN v1.0(supersedes 同目录 plan-draft.md;草案与评审留档不回改)
---

# 260924-perf-k1 语音问答往返延迟优化(P0 止血 + P1 模型切换)

> 来源:human 2026-09-23 夜实测「问一个问题要等 10~15s」+ 指示当晚走完「architect 评审 → TaskPackage → launch prep → dev-loop」,晨起看效果。
> 需求锚:docs/requirements.md **NFR-PERF-1**(思考响应不冷场,4s 安抚阈值)+ **NFR-PERF-2**(查字端到端 5 秒档,2026-09-15 实测 ~21.6s「优化归 Phase 1」——本包即该 Phase 1 优化的同根因主攻)。
> 证据基线:`evidence/latency-baseline-260924.md`(三源拆解 + 探针 + 主机状态 + §6 勘误限定);架构评审:`architect-review-260924.md`(判定与 D1~D4 修正,本 plan 逐条吸收)。

## 背景摘录(现状与差距)

- **现状**:问答往返 10~15s、天气 28~31s。延迟解剖(证据 §1~§3):ASR≈0、引擎转发≈0、TTS 首句 1.2~2.0s;**大头全在 OpenClaw 网关内部**——①deepseek-v4-flash 是 reasoning 模型(reasoning:true),闲聊一句话单轮生成 ≈9s;②agent 多轮工具循环(课表/天气各 3 次上游调用);③每条消息 2.7~6.1s preflight(HTTP 路径**每请求新建 session** + ~25k tokens 静态上下文装配 + SQLite 写,2GB 主机 swap 抖动放大,slow SQLite hold 活跃)。
- **主机劣化**:RAM 1870MB、swap 已用 ~700-790MB、gateway RSS 500~650MB 每 5 分钟 memory pressure 告警、进程连跑 2 天+;同题半小时内 2 倍方差(13.1s vs 6.6s)。
- **差距**:NFR-PERF-1 的 4s 安抚阈值之上,真实回答应在儿童耐心窗口内到达;NFR-PERF-2 明示 5 秒档优化归 Phase 1。

## 改法取舍(architect 裁决吸收后的冻结口径)

- **本包范围 = P0 + P1 + 条件步(仅测量/运维卫生)。P2 治理全部拆 k2**(归因卫生:一晚只动一个变量;prompt v7 版本化纪律成本不入运维型当晚包——评审 Q3 裁定)。
- **探针方法学(评审 D1,硬性)**:凡 A1/A2 判定探针,messages = [system(仓内 `server/deploy/openclaw-system-prompt.v6.md` **逐字**,脚本内 hash 断言,渲染面==被测面), user(题目)];裸探针数字只做背景参考,不做门证据。
- **方差受控(评审 D3)**:所有门 = **N≥3 中位数**;每批探针前后记录主机状态快照(free -m + memory pressure 告警计数 + session store 计数);**当晚同窗口重测基线配置**作对照锚,跨夜数字禁止直接对比。
- **模型裁决(评审 Q1)**:主案 A = `deepseek-chat`(非推理,catalog 在册 reasoning:false);备案 B = v4-flash 关 thinking——**可行性未证实,timebox ≤30min**(查 OpenClaw models 配置能力/一次带备份的 catalog 试验),到时未证实即放弃 B 直接走 A,不烧当晚窗口。
- **共 agent 代偿(评审 Q2)**:qqbot 与 voice 共用 main agent(实证:唯一 agent、零 routing)——改 primary 必然同切 qqbot。当晚**不做结构隔离**(第二 agent 成本不对称),代偿 = 提醒 semantic.json 形状冒烟(当晚)+ QQ 录入回归 1 条(晨间 human);隔离与 per-channel 模型策略记 k2 候选。
- **落仓防漂移**:openclaw.json 的模型选择现状不在仓内跟踪——本包新增**仓内声明 artifact + 幂等 apply 脚本**,drift-guard 必配(现状 jq 锚点核对不中即 exit 3 拒绝,照抄 openclaw-patch「STOP on mismatch」形态),时间戳备份 + 一步回滚脚本。
- **时延数字不做 CI 齿(评审 §3)**:timing 齿天然 flaky——CI 齿只断言机械面(声明在位/apply 幂等/drift-guard/hash 断言/回滚 dry-run);时延门 = live 证据记录 + 冻结阈值人工/脚本判定;mutation 打在**配置接线**上(删声明文件/改错 model id/删 allow 条目 → 齿红),不打超时判定。
- **contingency(评审 D4,预写免现场决策)**:若 A 案质量失败(display 块合法率 <100%)且 B 案不可行 → 当晚回滚保 v4-flash 现状、只留 P0 收益,火力转 k2(静态上下文瘦身/轮次收敛/store 维护/RAM 升级),数据留晨间 human 判定。

## 验收口径(冻结;两层门 = 评审 Q5 校准)

- **PERF-A1 探针门**(live 证据型):标准 3 题(闲聊「你好呀」/课表「明天有什么课程?」/天气「今天西安天气怎么样?」)×N≥3 中位数,带 v6 prompt。
  - **保底门(当晚必须达标,否则回滚或留数据晨判)**:闲聊 ≤6s、课表 ≤9s、天气 ≤14s。
  - **目标门(达标=当晚收工;未达=k2 立项依据,不算当晚失败)**:闲聊 ≤4s、课表 ≤6s、天气 ≤10s。
  - 天气题附加:答复必须含真实检索数据(tavily 轮成功证据),否则计时再好也判红。
- **PERF-A2 质量守卫(全量判定)**:每批探针全部响应计 display 块出现率与 schema 合法率,**要求 100%**(front V27 机械兜底≠过);课表题含备注读回(S-F1.3 AC1/AC4 口径不回退);语音 ≤3 句纪律;提醒 semantic.json 形状冒烟 1 次;晨间 human QQ 录入回归 1 条(MD 改写 + 三档重算触发 + 提醒文案质量)。**回滚触发线:切 A 后任一批 display 块合法率 <100% → 当晚回滚**。
- **PERF-A3 晨间真机轮**(human 持板,排引擎重启后**会话早段**——引擎侧历史随轮次增长是系统性混淆因子):引擎日志`大模型收到用户消息`→`发送第一段语音`,保底闲聊 ≤8s、课表 ≤10s;目标闲聊 ≤5s、课表 ≤7s(对齐公式 A3=A1+[1.5,2.5]s TTS 首句+转发)。三源取证(串口/front systemd/引擎 docker,kidpal-live-smoke-debug 纪律)。
- **PERF-A4 回滚与核验**:openclaw.json 时间戳备份 + 一步还原 + 探针复测回基线形态;**P0 重启后五项核验**——①`openclaw cron list` 的 kidpal-reminder-plan 在册且 Next 正常(提醒=安全面,不许被重启静默弄丢);②qqbot 通道 5 分钟周期内重连;③18789 仍锁 loopback(V28);④char-stroke dist 补丁 marker 在位(runbook 校验点);⑤引擎容器→18789 可达。清理类动作(条件步)专属回滚锚 = 先行 `openclaw backup` SQLite 快照。

## 阶段划分(dev-loop)

1. **acceptance-author 冻齿**:机械面齿(声明 artifact/apply 幂等+drift-guard exit-3/探针脚本 v6 hash 断言/回滚 dry-run/引擎 LLM 段与 v6 文件 must-stay-green)+ live-marked 探针齿(不入 CI);RED 基线实跑落账 taskpackage.json.redBaselineResult。
2. **builder 当晚序列(= 评审 §6 八步)**:
   ① 证据与回滚锚冻结:主机快照 + `openclaw sessions list` 计数 + `openclaw backup create` + openclaw.json 时间戳备份,evidence append;
   ② 探针脚本落仓(v6 注入+hash 断言+N≥3 中位数+计时输出+session 计数记录),**当晚基线配置重测**(对照锚);
   ③ P0 重启 openclaw-gateway → 五项核验 → 探针复测(归因第二段);
   ④ P1 声明 artifact + 幂等 apply 脚本落仓(drift-guard/备份/一步回滚);
   ⑤ B 案 timebox ≤30min → A/B 探针裁决(带 v6);
   ⑥ A 案 apply → 探针批 + A2 全量判定;触发回滚线 → 回滚留数据(contingency 路径);
   ⑦ 条件步:slow SQLite 重启后 30min 内再现或频繁 → `sessions archive/cleanup`(backup 快照在先)+ 复测(归因第三段,分开记);preflight 触发条件(A1 保底门红 / 重启后 preflight>2.5s / slow SQLite 再现)任一 → **仅测量面仪表化**(session create/context assemble/first POST 三段时戳 + workspace 注入清单,含 DREAMS.md 40KB 是否进上下文——直接决定 k2 瘦身面),不改任何配置;
   ⑧ 引擎 docker 重启(晨间真机轮开窗前,给 A3 干净起点)+ 全部证据 append 整理 + 晨间决策包素材成文。
   探针批次结束对探针新建 session 跑 archive(backup 在先)——探针不加剧它要测的 store 膨胀(评审 Q6)。
3. **verifier↔fixer**:offline 全量 `-m "not live"` 零新增红 + live 证据核对(N≥3 中位数对两层门、A2 全量、五项核验记录、三段归因数字齐)。
4. **integrator**:docsSync 判定(模型切换成为生产常态 → architecture.md §2 依赖表 + 决策 19 出网记账型号名同步;回滚路径 → 无漂移不 sync,如实记录)+ delivery/changes/dev 制品 + review packet + **morning-decision-package.md**(晨间 6 项,见下)+ closeout。
5. **supervisor 合并门** → 晨间 human:A3 真机轮 + QQ 录入回归 + 拍板项。

## 晨间 human 决策点(评审 §7,integrator 成文进 morning-decision-package.md)

1. A3 真机轮(human 持板、会话早段、三源取证);
2. 共 agent 回归:QQ 录入 1 条测试安排 + 提醒 semantic.json 形状抽查复核;
3. 模型定案拍板:A/B 案批准为生产常态(附成本注记 deepseek-chat input 2x/output 1.5x vs v4-flash,单设备量级绝对成本微小)+ architecture.md 型号名 docsSync 或 ARQ 二选一;
4. P3 拍板:4GB 升级 decision packet(停机 resize 窗口排期)+ 过渡项(每夜 gateway 重启 cron / sessions 维护定时)取舍;
5. P2/k2 立项:携当晚仪表化数据,范围候选 = 静态上下文瘦身(~25k tokens/请求:workspace bootstrap 注入清单 + v6 22.5KB)/ session store retention(511 会话 93MB 且生产固有速率增长)/ 课表 3 轮工具循环收敛(prompt v7,槽位预约先行;注意 v6「一回合作答:不调用任何工具」与「读 schedule.md 筛选」内部张力)/ 引擎侧历史治理 / 「David」人设注入质检(workspace USER.md/memory 是否波及儿童生产会话)/ per-channel 模型隔离与 `openclaw/default` 名称路由语义调研;
6. OpenClaw 2.0.4(npm)升级窗口:须过 runbook §0 版本漂移闸(char-stroke dist 锚点),可与 P3 停机窗口合并。

## 铁律约束

- 不改 `docs/contracts/**`;不改引擎 `.config.yaml` LLM 段(base_url /v1、model_name openclaw/default、流式禁 max_tokens——契约 §四/V9 全保持);prompt v6 零回改、本包不动 prompt;不改 spec、不改 agent 定义/gate。
- ECS 运维动作已获 human 当晚预授权(任务书口径,评审 §3 确认),但**逐步留痕**(evidence append-only)、凭证不入档、动 openclaw.json 前必备份、清理类动作必先 `openclaw backup` 快照。
- 不动 qqbot 配置、不升 OpenClaw、不加 agent/不做结构隔离(k2 候选);重启后五项核验缺一即处置并记录。
- 时延数字不入 CI 齿;真机 A3 轮 = human 持板,当晚不造假绿、不自行代跑。
- mutation 齿自证(RED→GREEN 声明 + counterEvidence 回填),打在配置接线上。

## demo 证据形态复述

offline:机械面齿 RED→GREEN 全程留痕 + pytest 全量零新增红;live:三段归因探针批(当晚基线重测/P0 后/P1 后,各 N≥3 中位数 + 主机快照 + session 计数)+ A2 全量判定记录 + 五项核验记录 +(条件步)仪表化数据与 archive 前后对比;晨间:A3 真机轮设备记录 + 三源日志 + QQ 回归 + morning-decision-package 逐项拍板。证据一律 append 进 `evidence/`,不回改。
