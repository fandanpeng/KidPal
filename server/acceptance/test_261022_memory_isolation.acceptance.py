"""Frozen acceptance tests — TaskPackage 261023-honcho-memory-wiring-restoration
（批 B 记忆接线 bring-up 甲案；本件 = K3 冻结锚换代承载。锚换代是**显式治理
动作**——advisory §10.4 human 已裁：作为批 B 锚换代承载，非静默改冻结件。
rigor=lite；riskTier=medium；subsystem = server/deploy/261022-openclaw-honcho-
plugin 插件工作台（声明 / apply / rollback）+ manifest 工作台条目 + 验收锚
换代。栈生产化与 v12 守则见姊妹件 test_261022_honcho_memory.acceptance.py。）

锚换代再承载（261024-oneshot-user-uniformity · human 2026-10-24 裁 R7/OQ-4
隔离口径退役）：front 引导/整理两条一次性 LLM 回合统一携 user=<device_id>
（WS 会话腿 device-id 头原值透传），全部落 agent:kidpal:openai-user:<device_id>
设备 session 键族；noisePatterns 补丁键退役（声明 5键→4键）；指令挪 request
级 system（user 消息 = 孩子内容纯文；空制品回退 = 回填现行完整包装形态）。
旧名映射（显式换代、非静默放宽）：
  test_k3_e2_front_oneshot_payload_no_user
    → test_k3_e2_front_oneshot_payload_carries_device_user（方向反转）；
  test_k3_e6_patch_generator_product_five_keys
    → test_k3_e6_patch_generator_product_four_keys。
历史 taskpackage 引旧名处原样不动（本映射为准）。

上游 = delivery/review/261022-arch-review-device-memory-prompt-overhaul/
memory-wiring-advisory-20261023.md（§1 已验证接线证据 / §6 方案甲 / §7 锚换代
清单 / §8 shaping / §9 网关安全序）+ 本包 plan.md 冻结语义六条 + human 三项
裁决（2026-10-23：方案甲 = 原样复用官方插件零新代码 / 生产 workspaceId=kidpal
新库不迁探针数据 / 与批 A 解耦不改 compose）+ human 原则（第一判据：「我们
之前测试过、已经验证过能用的功能，就直接用起来，不要再去发明什么新的东西
了。」凡冻结物与该原则冲突，以该原则为准）。

背景（K3 三层失效根因，advisory §2）：K3 自研 carrier 三层失效——① 声明路径
错（顶层 hooks.allowConversationAccess + agents.entries.kidpal.plugins 均被
2026.8.1 zod strictObject 拒收 → 网关 exit 78 崩循环）；② honcho-plugin.js 不
是插件（无 definePluginEntry / openclaw.plugin.json）；③ 其调用的
/v1/openclaw/* 端点在 Honcho 源码不存在（路由前缀 /v3）。裁决 = carrier 弃用
（不是重写），原样复用探针验证过的上游插件。debt 证据 7 的
「hooks.allowConversationAccess=true 后一次性路径钩子即入库」是**字面缩略**：
真实路径是 plugins.entries.openclaw-honcho.hooks.allowConversationAccess
（插件条目内 hooks，非顶层）。

已验证接线（advisory §1 逐条引证据）与验证行为口径：

  · 上游 npm 插件 @honcho-ai/openclaw-honcho@1.7.0（`openclaw plugins install
    --pin` 安装）+ plugins.entries.openclaw-honcho 配置键——探针实测 4 键
    （enabled / hooks.allowConversationAccess / config.baseUrl /
    config.workspaceId）。261023 批曾增补第 5 键 config.noisePatterns 作 user
    消息挡板；261024 批 human 裁 R7/OQ-4 隔离口径退役——该键从声明删除
    （5键→4键），退役实证 = 部署轮实读 openclaw.json（patch 只能表达 additive
    目标态、apply 幂等 SKIP 先于 drift-guard，键删除无机械验证面，如实申报）。
  · 验证行为 = **设备级 session + 家庭单 owner 画像**：Honcho session
    metadata `openclawSessionKey = agent:kidpal:openai-user:<device_id>`
    （这是 session key / OpenClaw session key，**不是 peer 键**——DB peers 只
    有 owner / agent-kidpal 两个、无设备级 peer）。本件验收一律按 **session
    key 家族**表述；spec（arch §6.7 / §1.3）"peer" 用词修正归面 C
    /arch-change，不归本包。
  · 资格门口径（261024 换代后）：上游插件无 user-present-only 资格门（三家都
    不提供）。cron/心跳由 captureSystemRuns=false（默认）天然挡；front 两条
    一次性回合统一携 user=<device_id>，与主链路同落设备 session 键族——原
    甲案「noisePatterns 挡板隔离」随 human 2026-10-24 裁 R7/OQ-4 退役，一次性
    回合的处置由「隔离」改判「统一归属」（B1 挡板措辞随键一并退役；R1 残余
    只余 assistant 回复单条入库）。

残余偏差申报（甲案挡不住的两条，如实申报、不静默；spec 口径修正归面 C，
integrator docs-delta 逐条对账）：

  R1. front 整理/引导回合的**读侧画像注入**（before_prompt_build 照常注入）
      与 assistant 回复单条入库——261024 换代后 user 原文随统一 user 通道入
      设备 session 库（one_shot 判定口径已反转），残余只余 assistant 回复
      单条入库 = 申报、**不判红**（低害：整理产物是孩子自己的故事；指令挪
      system 后 user 消息 = 纯孩子内容，无指令句噪声入 session）。
  R2. QQ 回合落消息（senderId 独立 peer、owner 画像基本无污染，但消息入库且
      可能经 honcho_search_messages 可搜）——可搜性待部署轮 OQ-3 实测，本包
      只申报不实现挡置。

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；纯 python / 离线可跑 / 导入期
零网络 I/O）

  A) front 一次性回合统一携 user=<device_id>（261024 换代——方向反转，旧口径
     「payload 恰 {model, messages, stream} 零 user」退役，旧名
     test_k3_e2_front_oneshot_payload_no_user →
     test_k3_e2_front_oneshot_payload_carries_device_user，换代事由见头注首段）：
     _openclaw_chat_once 增 user 透传，payload 经 openclaw_stream_request(
     messages, user=None) 单源构造（if user: 真值判据，与 engine-patch
     openai.py 两腿逐字同源；契约 §四禁 max_tokens 约束不动）；设备身份 = 会话
     腿 device_ws.headers.get("device-id") 原值透传（不 strip 不归一，isinstance
     str + strip 双保险防御式；条件传参 kwargs={"user": device_id}
     if device_id else {}，八处既有桩签名零改）→ payload 键集恰
     {model, messages, stream, user} 且 user=<device_id>；降级路径 = device-id
     缺席 → payload 缺 user 键（诚实降级 + 调用缝留痕）；指令挪 request 级
     system：整理回合 user 消息 = material 纯文、引导回合 user 消息 = spoken
     纯文（两条 prompt 制品全文作为 request 级 system 装载），_load_prompt_
     artifact 空回退 = user 消息回填现行完整包装形态（最坏退回旧 live 行为）
     + warning 留痕。

  B) server/deploy/261022-openclaw-honcho-plugin/ 插件工作台（声明 / apply /
     rollback 三件；自研 carrier 已弃用删除）：

       1) honcho-plugin-declaration.json（**JSON**，stdlib json 解析）——
          changes **恰 4 条**点分键、全部以 plugins.entries.openclaw-honcho.
          为全前缀（逐字）：
            · plugins.entries.openclaw-honcho.enabled = true
            · plugins.entries.openclaw-honcho.hooks.allowConversationAccess =
              true —— **插件条目内 hooks**（2026.8.1 顶层 hooks 只有 internal，
              strictObject 拒收顶层 allowConversationAccess = K3 崩因 ①；
              本键的全路径语境以本条为准，任何「顶层 hooks」读法 = 误读）
            · plugins.entries.openclaw-honcho.config.baseUrl =
              "http://127.0.0.1:18100"（本机 loopback）
            · plugins.entries.openclaw-honcho.config.workspaceId = "kidpal"
              （**恰 "kidpal"**，非探针的 "kidpal-probe"——human 裁决 ②：
              新库、探针数据不迁）
          旧第 5 键 config.noisePatterns 已随 261024 换代从 changes 与顶层
          plugins 块同步删除（B1 挡板措辞退役申报：甲案资格门 user 消息挡板
          随 R7/OQ-4 隔离口径退役；声明 _note 换代申报须保留
          agent:kidpal:openai-user: session 家族字面、不得引入 agent:kidpal:
          其它形态变体）；退役无机械验证面（apply 幂等 SKIP 先于 drift-guard，
          patch 不能表达键删除），实证 = 部署轮实读 openclaw.json，禁
          expect:unset 假牙。
          顶层 hooks.* 与 agents.entries.*.plugins 零出现（schema 拒收键，
          负锚）。其余结构同前代：case / changes / drift_guard / backup /
          rollback 五顶层键；drift_guard 必含 P1 已应用态现状锚
          {"path": "agents.entries.kidpal.tools.allow",
           "expect": ["read","write","exec","web_search"]}（launch-card 红线
          原样保留：P1 漂移时本包 STOP 拒绝叠加）；backup / rollback 形制
          不变（backup.pattern 含 "bak"、config 同目录 glob、无路径分隔符；
          rollback.script = rollback_honcho_plugin.py）。声明零 secret。

       2) apply_honcho_plugin.py —— **双形态安全化**（「点分写入器直写
          openclaw.json」路径撤销——K3 崩因，advisory §9 铁律：openclaw.json
          只经 OpenClaw 自己的写入口写，写前必 --dry-run schema 校验）：
            · 形态① patch 生成器：CLI `--gen-patch <out>`（配合 --decl）从
              声明生成 `openclaw config patch` 消费的 patch 文件；产物可被
              stdlib json 解析（JSON 子集形态；JSON5 超集允许但不得破坏
              stdlib 解析），展平叶键集**恰 = 声明 4 键逐字**（嵌套树或扁平
              点分映射两形皆可，展平语义一致）。
            · 形态② 安全应用编排：advisory §9 序——写前时间戳备份 →
              `config patch --file <p> --dry-run --json`（schema:true 必核）
              → `plugins install @honcho-ai/openclaw-honcho --pin` →
              `plugins inspect openclaw-honcho --runtime`（Status: loaded）
              + `plugins doctor` → 正式 `config patch --file <p>` →
              `config validate` → `systemctl restart openclaw-gateway` +
              health/NRestarts 核对；**任一步失败即停**。
            · 注入通道（离线桩真跑契约）：`--openclaw-bin <path>`——**全部**
              外部副作用命令（openclaw CLI 与 systemctl）经该注入桩执行，桩
              收到完整 argv（argv[0] = "openclaw" 或 "systemctl"）；.py 结尾
              的桩经 sys.executable 调起（跨平台）。幂等预检（config 只读
              直判现状已达标 → 打印 SKIP、exit 0）与 drift-guard（P1 锚不中
              → 打印 STOP、exit 3）均**先于任何外部命令**、零写零备份。
              CLI 缺 config / 缺声明 / 非法 JSON → exit 2。
            · **冻结负锚：脚本内零「直写 openclaw.json」路径**——离线桩真跑
              的 config-hash 逐调用协议承载（见
              test_k3_e6_apply_safe_orchestration_call_order）：桩在每次调用
              起点记录 config 内容 sha256；正式 patch 调用及其之前全部记录
              == 变更前 hash、其后全部 == 目标态 hash → config 只在 OpenClaw
              写入口（桩的正式 config patch 调用内）被改写一次，其余任何
              时点零直写；dry-run 失败 / 正式 patch 失败两个失败序场景下
              config 终态与逐调用 hash 双查零变。
            · 写前时间戳备份语义保留：config 同目录、basename + ".bak-..." +
              UTC yyyymmddTHHMMSS、内容 = 原始 config（fixture 真跑）。
       3) rollback_honcho_plugin.py —— 备份还原一步回滚语义保留（--config /
          --backup / --dry-run；非 dry-run 还原 backup 内容 exit 0）；脚本
          头注申报**三级回滚序**（① `plugins disable openclaw-honcho` +
          restart——低危首选，配置留着插件不加载；② 还原时间戳备份
          openclaw.json.bak-kidpal-honcho-*（本脚本承载）；③
          `plugins uninstall openclaw-honcho`——连 npm project 一起清）。

       4) 插件运行时证据字面（静态锚 = **工作台并集**含两字面——声明注记与
          apply 编排核对面为自然承载点；运行时行为 = live 齿判定）：
          插件加载日志行 "Honcho memory plugin loaded"（上游插件
          dist/index.js 原文，journalctl -u openclaw-gateway 核对）+
          `plugins inspect openclaw-honcho --runtime` 的
          "Policy: allowConversationAccess: true"。工作台并集含 session key
          家族表述 agent:kidpal:openai-user:<device_id>（session metadata
          openclawSessionKey，非 peer 键）；工作台内 "agent:kidpal:" 键族
          唯一形态 = "agent:kidpal:openai-user:" 变体（任何其它变体 = 红）。
          声明/工作台并集记插件版本 pin 1.7.0 与包名
          @honcho-ai/openclaw-honcho。旧 carrier 时代的三锚集（自打 marker /
          可达性探针门控 / peer 键）随 carrier 一并作废：可达性由 live 证据
          （Honcho /health 200）+ 降级实测承载，host 零探针锚断言。

  C) K3-E5 降级护栏 = **live 申报（host 零假绿断言）**：仓内无上游插件
     vendor 源码可锚 bounded 失败形态（插件经 `openclaw plugins install` 装
     于 ECS，dist/ 不入仓），故本代不设 host 文本锚（carrier 时代的
     catch/while(true) 文本锚随 carrier 一并作废）；「停 Honcho 容器 → 对话
     照常、无重试风暴、无用户可见报错」由 live 齿
     test_live_k3_e5_degradation_probe 对部署轮实测结构化证据判定。

  D) live 证据判定齿（@pytest.mark.live，host 不 auto-run，conftest 机械
     skip）：live/部署轮把结构化探针证据落
     delivery/taskpackages/261023-honcho-memory-wiring-restoration/memory-probes/
     *.json，本件 live 齿判定（**三证 schema**，换代自 marker 双证）：
       · 已验证接线探针文件 schema：
           {"plugin_runtime": {"load_log_hit": bool,
                               "inspect_policy_hit": bool},
            "main_link": [{"openclaw_session_key": str,
                           "db_message_delta": int,
                           "gateway_log_hit": bool}],
            "one_shot":  [{"round": "guide"|"organize",
                           "openclaw_session_key": str,
                           "user_text_in_db_delta": bool}]}
         判定 = plugin_runtime 两证全真（loaded 日志行 + inspect Policy 行）；
         main_link ≥1 且逐条 openclaw_session_key 以
         "agent:kidpal:openai-user:" 为前缀（**证①** session metadata）且
         db_message_delta ≥ 1（**证②** DB 消息增量）且 gateway_log_hit ==
         true（**证③** 网关日志行）；one_shot 覆盖 {"guide","organize"} 各
         ≥1 且逐条 openclaw_session_key 以 "agent:kidpal:openai-user:" 为
         前缀（261024 换代增字段：一次性回合与主链路同 session 键族——只证
         「不再挡」不够，要证「全部落同一 session 键族」）且
         user_text_in_db_delta == true（**换代口径反转**：user 统一携入后
         user 原文进设备 session 库 = 正证；旧甲案「user 原文零出现」判定随
         human 2026-10-24 裁 R7/OQ-4 隔离口径退役；assistant 回复入库 = 残余
         R1 申报、不判红；read_side 判定不设）。
       · seeded 负向探针集文件 schema（不变）：
           {"probes": [{"category": "姓名"|"电话"|"家庭安排", "seed": str,
                        "query": str, "profile_hit": bool, "reply_hit": bool}]}
         判定 = category 覆盖三档各 ≥1、全部 probes profile_hit == false 且
         reply_hit == false（S-F5.1-AC3 / NFR-PRIV-1 量化口径不变）。
       · 降级实测证据文件 schema：
           {"degradation": {"honcho_stopped": bool, "conversation_ok": bool,
                            "retry_storm": bool, "user_visible_error": bool}}
         判定 = honcho_stopped == true 且 conversation_ok == true 且
         retry_storm == false 且 user_visible_error == false。
       · OQ-3（honcho_* 工具是否对 kidpal agent 暴露 / 可搜性）：部署轮实测
         申报项，本包只申报不判分，不设 live 判定。
     证据缺位 = live 相位设计态红（不 auto-run，ARQ-260901-g3 同纪律）。

===========================================================================
host/live 分工边界（rigor=lite + riskTier=medium，如实申报）：

  · host 面（本件）= front payload 结构断言（行为级，离线真跑）+ 声明 4 键 /
    负锚 / pin / 运行时证据字面 / 残留静态断言（机制级）+ apply 双形态
    fixture 真跑（patch 生成产物 / 调用序桩真跑 / 失败即停零写 / 幂等 /
    drift-guard / 备份 / 回滚）。
  · live/部署轮面 = 三证采集（plugin_runtime / main_link / one_shot 换代口径）+
    seeded 负向探针集 + 降级实测（停 Honcho 容器）+ OQ-3 实测——由本件
    live 齿（D 节）对结构化证据判定；真实安装 / patch / restart 执行面 =
    human 部署轮（或主线程经 human 授权），Pod 不自行上 ECS，host 面转绿后
    status awaiting-human:live-deploy。

强度申报（契约条目 → 承载断言 → 性质档）：

  · E2 user 通道统一（front 半：一次性回合携 user=<device_id> + 降级路径
    + 空制品回退）
                                     → test_k3_e2_front_oneshot_payload_carries_device_user → 行为级
  · A1 声明 4 键逐字 / 负锚 / workspaceId / pin / 运行时证据字面 / session 家族
                                     → test_k3_e2_verified_wiring_declaration_anchors → 机制级
  · A1 声明结构 + drift_guard P1 现状锚
                                     → test_k3_e6_declaration_schema_and_p1_drift_anchor → 机制级
  · A2 patch 生成器产物恰 4 键       → test_k3_e6_patch_generator_product_four_keys → 行为级（真跑）
  · A2 安全编排 §9 调用序 + 逐调用 hash 零直写协议
                                     → test_k3_e6_apply_safe_orchestration_call_order → 行为级（真跑）
  · A2 失败即停 / 失败序零写         → test_k3_e6_apply_failure_stops_zero_write → 行为级（真跑）
  · A2 幂等 SKIP                     → test_k3_e6_apply_idempotent_and_preserves_p1_state → 行为级（真跑）
  · A2 drift-guard exit 3 零写       → test_k3_e6_apply_drift_guard_exit3 → 行为级（真跑）
  · A2 写前时间戳备份                → test_k3_e6_apply_timestamped_backup → 行为级（真跑）
  · A2 回滚 dry-run + 还原 + 三级回滚序申报
                                     → test_k3_e6_rollback_dry_run_and_restore → 行为级（真跑）+ 源锚
  · A1/A3 声明零 secret + openclaw.json 零入仓
                                     → test_k3_e6_no_openclaw_json_in_repo_and_no_secret → 机制级
  · A3 carrier 撤净（负锚）+ manifest 恰 3 条
                                     → test_k3_e2_verified_wiring_declaration_anchors（负锚半）
                                       + test_k3_e6_manifest_registers_the_workbench → 机制级
  · live 三证 / 换代 one_shot        → test_live_k3_e2_verified_wiring_probes_three_evidence
                                     → 行为级（live 证据判定）
  · E3 seeded 负向探针集 100% 零命中 → test_live_k3_e3_seeded_negative_probe_set_zero_hit
                                     → 行为级（live 证据判定）
  · E5 降级（停容器对话照常）        → test_live_k3_e5_degradation_probe → 行为级（live 证据判定）

缺口（承载断言强度低于契约要求处，逐条）：

  · 「零直写 openclaw.json」的行为证明存在离线残余盲区：若编排器在桩的正式
    config patch **之后**做冗余直写，逐调用 hash 协议不可分辨（终态相同）；
    但 dry-run 失败 / 正式 patch 失败两个失败序场景下任何直写都会被终态与
    逐调用 hash 双查捕获。直写残留上真实网关的后果 = zod strictObject 拒收
    exit 78 崩循环（K3 崩因自证），live 轮 NRestarts 核对兜底。
  · 幂等预检 / drift-guard 的「先于任何外部命令」离线只证声明形态与 fixture
    行为（本机 PATH 无 openclaw，越序外部调用即 crash 暴红）；ECS 实况是否
    真为 P1 已应用态由 apply 时 STOP / SKIP 回执承载（261017 PV-A6 同款缺口）。
  · patch 文件被真实 OpenClaw 消费的 JSON5 细节形状不作冻结——只钉 stdlib
    json 可解析 + 展平叶键集恰 4 键；真实消费行为 = live 轮
    `config patch --dry-run --json` schema:true 回执。
  · 插件运行时行为（loaded 日志 / Policy 行 / 消息入库 / 一次性回合 user 落
    session 键族）不可离线实证——静态锚（工作台并集两字面 + 声明键逐字）是
    **源锚代理**，行为本体 = live 三证齿 + 降级实测。行为等价假设显式化：
    工作台并集含两字面 ≈ 部署轮按该两字面采集证据（若实测日志行/Policy 行
    字面不符，属验收判法升级，走 tighten-only 窗口经 planner 调整，不得
    静默绕过）。
  · E5 降级 host 零断言 = 有意降级（仓内无插件源码可源锚），不是遗漏；假绿
    风险由「live 证据缺位 = 设计态红」兜住。

fixture 自含性声明：全部 host 断言直读 repo 内制品 + tempfile fixture（P1 已
应用态组合态与 openclaw 桩脚本均内嵌本文件生成）；front 一次性回合链 fixture
自含（内存 StoryAccumulator + 桩腿 device_ws/engine_ws + urlopen SSE 桩 +
STORIES_DIR 重定向 tempfile——零真实归档落盘、零网络 I/O、零外部设备前提）；
零网络 I/O、零外部资产、零环境前提（PyYAML 仅 manifest 验收测试 case 用）；
桩协议经 --openclaw-bin 注入，.py 桩经 sys.executable 调起（跨平台）。
evidence 类断言仓根由 __file__ 显式推导，不依赖调用点 cwd。行为 seam 驱动方
对账：声明 4 键 → OpenClaw 运行时插件加载的驱动方 = 部署轮（offline 无驱动方
= 已申报盲区）；探针证据 → live 齿判定闭环；停容器降级实测驱动方 = 部署轮；
「模型真的记 / 真的拒」驱动方 = 真机轮；「一次性回合真落设备 session 键族」
驱动方 = live 轮 one_shot 证据（host 半只证 payload user 键装配）。

must-stay-green（冻结时点即绿，builder 不得回退）：
  · 换代件内负锚/保留面半（冻结时点即绿）：降级路径断言（device-id 缺席 →
    payload 缺 user 键）与空制品回退断言（user 消息回填现行完整包装形态）
    在现行树已绿、换代后须保持；声明零 secret 负锚保；
    agent:kidpal:openai-user: session 家族字面保留 + 零其它 agent:kidpal:
    形态变体负锚保。
  · 姊妹件零触碰：test_261022_honcho_memory（K3-E1 栈面 + K3_P1_AGENTS_ANCHORS）、
    test_261022_honcho_backfill（K4_GATE_CONFIG_NEEDLES 负锚天然不红——
    backfill 树本就不引用声明工件）、test_261023_agents_md_static_semantics_
    alignment、四姊妹验收件 test_m4k1_story_cloud_link / test_record_cancel_
    lifecycle / test_record_dialogue_guide / test_record_front_polish 的桩缝
    （条件传参 kwargs={"user": device_id} if device_id else {} 下八处既有桩
    签名无感零改）。本包不改 compose、不改 AGENTS.md。
  · unit 件零改必绿：test_story_cancel_lifecycle_units 四族 +
    test_archive_guard_units AST 结构锚。
  · K1 冻结件（test_261022_openclaw_kidpal_tools / test_261017_openclaw_kidpal_skills
    / test_261022_m5_k1_prompt_carrier_migration）零触碰。
  · drift_guard P1 现状锚逐字保；openclaw.json 零入仓 + 零 secret 负锚保。

Run protocol（文件名两点须 importlib 模式）:
  cd server && python -m pytest -q --import-mode=importlib -m "not live" acceptance
  （standalone: python 本文件）
"""

from __future__ import annotations

import asyncio
import contextlib
import importlib.util
import json
import re
import shutil
import subprocess
import sys
import tempfile
import unittest
from datetime import datetime, timezone
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

try:
    import yaml  # manifest 验收测试 case 前置；缺席 = infra 红（K1 同口径）
    _HAS_YAML = True
except ImportError:  # pragma: no cover
    _HAS_YAML = False

try:
    import pytest
    _HAS_PYTEST = True
except ImportError:  # pragma: no cover
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
DEPLOY = SERVER / "deploy"
WORKBENCH = DEPLOY / "261022-openclaw-honcho-plugin"
DECL_PATH = WORKBENCH / "honcho-plugin-declaration.json"
APPLY_SCRIPT = WORKBENCH / "apply_honcho_plugin.py"
ROLLBACK_SCRIPT = WORKBENCH / "rollback_honcho_plugin.py"
MANIFEST_YAML = DEPLOY / "manifest.yaml"
FRONT_APP = SERVER / "gateway" / "front" / "front_app.py"
EVIDENCE_DIR = (REPO_ROOT / "delivery" / "taskpackages"
                / "261023-honcho-memory-wiring-restoration" / "memory-probes")

# --------------------------------------------------------------------------- #
# 冻结字面（变更 = 语义放宽/收紧，走 tighten-only 纪律反审）
# --------------------------------------------------------------------------- #

K3_PLUGIN_ID = "openclaw-honcho"
K3_PLUGIN_PACKAGE = "@honcho-ai/openclaw-honcho"
K3_PLUGIN_VERSION_PIN = "1.7.0"
K3_PLUGIN_LOAD_LOG = "Honcho memory plugin loaded"
K3_PLUGIN_POLICY_LINE = "Policy: allowConversationAccess: true"
K3_PLUGIN_ENTRY_PREFIX = "plugins.entries.openclaw-honcho."
K3_HOOKS_FULL_PATH = "plugins.entries.openclaw-honcho.hooks.allowConversationAccess"
K3_HONCHO_BASE_URL = "http://127.0.0.1:18100"
K3_HONCHO_BASE_URL_HOST = "127.0.0.1:18100"
K3_WORKSPACE_ID = "kidpal"
# 261024 换代：旧第 5 键 K3_NOISE_PATTERN / noisePatterns 声明条目删除
# （human 2026-10-24 裁 R7/OQ-4 隔离口径退役；声明 5键→4键）。
K3_DECL_CHANGES = [
    {"path": "plugins.entries.openclaw-honcho.enabled", "value": True},
    {"path": K3_HOOKS_FULL_PATH, "value": True},
    {"path": "plugins.entries.openclaw-honcho.config.baseUrl",
     "value": "http://127.0.0.1:18100"},
    {"path": "plugins.entries.openclaw-honcho.config.workspaceId",
     "value": "kidpal"},
]
# 261024 换代：front 一次性回合统一携 user 的冻结字面（A3 主锚）。
K3_ONESHOT_DEVICE_ID = "dev-k3-e2-01"
K3_ONESHOT_ORGANIZE_WRAP = ("孩子讲的内容：\n{material}\n\n"
                            "请按指令把这段话忠实整理成一篇。")
K3_ONESHOT_GUIDE_WRAP = ("孩子到目前为止讲的内容：\n{spoken}\n\n"
                         "孩子停顿了，请按指令给一句轻声引导。")
# session key 家族（非 peer 键）：session metadata openclawSessionKey 前缀
K3_SESSION_KEY_PREFIX = "agent:kidpal:openai-user:"
K3_SESSION_KEY_VARIANT_RE = re.compile(r"agent:kidpal:(?!openai-user:)")
# 负锚：schema 拒收键（K3 崩因 ①）
K3_TOP_HOOKS_RE = re.compile(r"^hooks(\.|$)")
K3_AGENT_PLUGINS_RE = re.compile(r"agents\.entries\.[^.]+\.plugins(\.|$)")
# 负锚：自研 carrier 残留（弃用删除，零残留）
K3_CARRIER_FILE = "honcho-plugin.js"
K3_CARRIER_MARKER = "kidpal honcho memory patch active"
K3_CARRIER_ENDPOINT = "/v1/openclaw"
# 结构与形制（保留面）
K3_P1_TOOLS_PATH = "agents.entries.kidpal.tools.allow"
K3_P1_TOOLS_EXPECT = ["read", "write", "exec", "web_search"]
K3_DECL_NAME = "honcho-plugin-declaration.json"
K3_APPLY_NAME = "apply_honcho_plugin.py"
K3_ROLLBACK_NAME = "rollback_honcho_plugin.py"
K3_WORKBENCH_TOKEN = "261022-openclaw-honcho-plugin"
K3_SECRET_KEY_RE = re.compile(r"sk-[A-Za-z0-9]{16,}")
K3_TIMESTAMP_RE = re.compile(r"\d{8}T\d{6}")

_FRONT_MODULE_CACHE = None


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _iter_tree_text(root: Path):
    if not root.is_dir():
        return
    for p in sorted(root.rglob("*")):
        if p.is_file():
            txt = _read_text(p)
            if "\x00" in txt:
                continue
            yield p, txt


def _sha_file(path: Path) -> str:
    import hashlib
    try:
        return hashlib.sha256(path.read_bytes()).hexdigest()
    except OSError:
        return "MISSING"


def _run_py(script: Path, args, timeout: int = 120):
    """用当前解释器跑 python 脚本（离线可跑，不经 bash/ssh）；脚本缺失 → rc 127。"""
    if not script.is_file():
        return 127, "", f"{script} 不在位（冻结契约工件缺失）"
    proc = subprocess.run(
        [sys.executable, str(script), *[str(a) for a in args]],
        capture_output=True, text=True, encoding="utf-8", errors="replace",
        timeout=timeout)
    return proc.returncode, proc.stdout, proc.stderr


_MISSING = object()


def _json_get(obj, dotted: str):
    cur = obj
    for part in dotted.split("."):
        if not isinstance(cur, dict) or part not in cur:
            return _MISSING
        cur = cur[part]
    return cur


def _json_set(obj: dict, dotted: str, value) -> None:
    parts = dotted.split(".")
    cur = obj
    for p in parts[:-1]:
        nxt = cur.get(p)
        if not isinstance(nxt, dict):
            nxt = {}
            cur[p] = nxt
        cur = nxt
    cur[parts[-1]] = value


def _flatten_leaves(obj, prefix: str = ""):
    """把 patch 产物（嵌套树或扁平点分映射两形皆可）展平为
    (点分叶路径, 标量值) 序列——展平语义一致，故两种形态可同一判法。"""
    if isinstance(obj, dict):
        for k, v in obj.items():
            yield from _flatten_leaves(v, f"{prefix}.{k}" if prefix else str(k))
    else:
        yield prefix, obj


def _canon_changes(changes) -> list:
    """changes → 排序后的 (path, value-canon) 对（判「逐字」用，序不敏感）。"""
    return sorted(
        (c["path"], json.dumps(c["value"], sort_keys=True, ensure_ascii=False))
        for c in changes)


# --------------------------------------------------------------------------- #
# fixture（P1 已应用态组合态：K1 tools.allow 四项在位 + 261017 skills 白名单
# 在场 + 插件键未设 = 本包 drift_guard 现状锚，launch-card 双声明式 patch
# 顺序红线）
# --------------------------------------------------------------------------- #

def _fixture_p1_applied() -> dict:
    return {
        "gateway": {"token": "FIXTURE-GATEWAY-TOKEN-0123456789"},
        "tools": {"profile": "coding"},
        "agents": {
            "defaults": {"model": {"primary": "openclaw/kidpal"}},
            "entries": {
                "main": {"skills": ["kidpal-schedule-write"]},
                "kidpal": {
                    "prompt": "kidpal/base",
                    "skills": ["kidpal-schedule-answer", "weather", "tavily"],
                    "tools": {"allow": ["read", "write", "exec", "web_search"]},
                },
            },
        },
        "modelPolicy": {"allow": ["openclaw/kidpal"]},
    }


def _target_state(pre: dict, changes) -> dict:
    state = json.loads(json.dumps(pre))
    for change in changes:
        _json_set(state, change["path"], json.loads(json.dumps(change["value"])))
    return state


def _backups(td: Path, pattern: str) -> list[Path]:
    return sorted(td.glob(pattern))


# --------------------------------------------------------------------------- #
# openclaw 桩（A2 离线桩真跑 fixture）：--openclaw-bin 注入通道的桩一侧。
# 冻结契约：桩收到完整 argv（argv[0] = "openclaw"|"systemctl"）；每次调用起点
# 把 {argv, config_sha256} 追加进 jsonl 日志（逐调用 hash 协议 = 「零直写
# openclaw.json」负锚的行为承载）；fail_mode 注入两个失败点（dry-run schema
# 拒 / 正式 patch 失败）。桩的成功路径行为 = 模拟 OpenClaw 写入口：正式
# config patch 把 patch 文件展平深合并进 config（JSON 子集，两形态皆收）。
# --------------------------------------------------------------------------- #

_STUB_TEMPLATE = '''
import hashlib, json, sys
LOG = %(log)r
CFG = %(cfg)r
FAIL_MODE = %(fail_mode)r


def _sha(path):
    try:
        with open(path, "rb") as fh:
            return hashlib.sha256(fh.read()).hexdigest()
    except OSError:
        return "MISSING"


argv = sys.argv[1:]
with open(LOG, "a", encoding="utf-8") as fh:
    fh.write(json.dumps({"argv": argv, "config_sha256": _sha(CFG)},
                        ensure_ascii=False) + "\\n")

fail = False
if FAIL_MODE == "fail_dry_run" and "patch" in argv and "--dry-run" in argv:
    fail = True
if (FAIL_MODE == "fail_patch" and "patch" in argv
        and "--dry-run" not in argv and "--file" in argv):
    fail = True
if fail:
    print('{"ok": false, "checks": {"schema": false}}')
    sys.exit(1)

if argv and argv[0] == "openclaw":
    rest = argv[1:]
    if rest[:2] == ["config", "patch"]:
        if "--dry-run" in rest:
            print('{"ok": true, "operations": 5, "checks": {"schema": true}}')
        else:
            idx = rest.index("--file")
            with open(rest[idx + 1], encoding="utf-8") as fh:
                patch = json.load(fh)
            with open(CFG, encoding="utf-8") as fh:
                cfg = json.load(fh)

            def flat(obj, prefix=""):
                if isinstance(obj, dict):
                    for k, v in obj.items():
                        yield from flat(v, (prefix + "." + k) if prefix else k)
                else:
                    yield prefix, obj

            for dotted, val in flat(patch):
                cur = cfg
                parts = dotted.split(".")
                for p in parts[:-1]:
                    cur = cur.setdefault(p, {})
                cur[parts[-1]] = val
            with open(CFG, "w", encoding="utf-8") as fh:
                json.dump(cfg, fh, ensure_ascii=False, indent=2)
            print("Config valid")
    elif rest[:2] == ["config", "validate"]:
        print("Config valid")
    elif rest[:1] == ["plugins"] and "inspect" in rest:
        print("Status: loaded")
        print("Origin: global")
        print("Version: 1.7.0")
        print("Policy: allowConversationAccess: true")
    else:
        print("ok")
else:
    print("ok")
'''


def _oc_cmd(argv, sub) -> bool:
    """argv[0] == "openclaw" 且其后恰以 sub 子序列开头。"""
    return bool(argv) and argv[0] == "openclaw" and tuple(
        argv[1:1 + len(sub)]) == tuple(sub)


def _argv_text(argv) -> str:
    return " ".join(argv)


# --------------------------------------------------------------------------- #
# front_app.py 离线装载（RDG/M4K1/SC 冻结件同款先例）
# --------------------------------------------------------------------------- #

def _ensure_fastapi_stub() -> None:
    try:
        import fastapi  # noqa: F401
        if isinstance(getattr(fastapi, "FastAPI", None), type):
            return  # real fastapi（或带可用 FastAPI 类的 stub）在场 —— use it
    except ImportError:
        pass

    class _StubFastAPI:
        def __init__(self, *args, **kwargs):
            pass

        def _identity(self, *args, **kwargs):
            def deco(fn):
                return fn
            return deco

        get = post = websocket = _identity

    stub = ModuleType("fastapi")

    def _permissive_getattr(name):
        return Mock()

    stub.__getattr__ = _permissive_getattr  # type: ignore[method-assign]
    stub.WebSocketDisconnect = type("WebSocketDisconnect", (Exception,), {})
    stub.HTTPException = type("HTTPException", (Exception,), {})
    stub.FastAPI = _StubFastAPI
    sys.modules["fastapi"] = stub
    if "fastapi.responses" not in sys.modules:
        resp = ModuleType("fastapi.responses")
        resp.__getattr__ = _permissive_getattr  # type: ignore[method-assign]
        sys.modules["fastapi.responses"] = resp


def _load_front_app():
    """Import server/gateway/front/front_app.py by path, offline. (mod|None, err)."""
    global _FRONT_MODULE_CACHE
    if _FRONT_MODULE_CACHE is not None:
        return _FRONT_MODULE_CACHE
    if not FRONT_APP.is_file():
        return None, "server/gateway/front/front_app.py missing — 网关接线面不在世。"
    _ensure_fastapi_stub()
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    try:
        spec = importlib.util.spec_from_file_location("kidpal_front_app_k3", FRONT_APP)
        if spec is None or spec.loader is None:
            return None, "front_app.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _FRONT_MODULE_CACHE = (mod, None)
        return mod, None
    except Exception as e:  # noqa: BLE001 — import error 是 RED，不是 crash
        return None, (f"front_app.py failed to import: {e!r}. 冻结口径：该模块必须"
                      "离线可装载（import 期零网络 I/O；fastapi 缺席走本地 stub）。")


class _FakeSseResponse:
    """urlopen 桩：SSE 双形态（delta 流 + [DONE]），context manager。"""

    def __init__(self, body: bytes):
        self._body = body

    def read(self):
        return self._body

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        return False


_SSE_BODY = (b'data: {"choices":[{"delta":{"content":"\xe5\xa5\xbd\xe7\x9a\x84"}}]}\n\n'
             b"data: [DONE]\n\n")


class _LegDeviceWs:
    """一次性回合会话腿 device_ws 桩：starlette 风格 headers（原样 dict，
    .get("device-id") 语义与生产一致）。headers=None 模拟无 headers 属性的裸腿
    （覆盖防御式 getattr 路径）。"""
    def __init__(self, headers=None):
        if headers is not None:
            self.headers = headers


class _LegEngineWs:
    """一次性回合会话腿 engine_ws 桩：记录 send() 注入帧（kidpal_speak 播报 /
    kidpal_record_context 出语境信号），零网络。"""
    def __init__(self):
        self.sent = []

    async def send(self, raw):
        self.sent.append(raw)


@contextlib.contextmanager
def _captured_oneshot_payloads(front):
    """一次性回合 payload 捕获缝（行为级离线真跑）：spy
    gw_provider.openclaw_stream_request（单源真构造不动、只捕获产物）+
    urlopen 换 SSE 桩（_FakeSseResponse）。yield 捕获列表；with 退出恢复原状。"""
    captured = []
    real_build = front.gw_provider.openclaw_stream_request

    def spy_build(*args, **kwargs):
        captured.append(real_build(*args, **kwargs))
        return captured[-1]

    import urllib.request as _urlreq  # noqa: PLC0415 — 与被测面同款惰性 import
    orig_urlopen = _urlreq.urlopen
    _urlreq.urlopen = lambda *a, **kw: _FakeSseResponse(_SSE_BODY)
    front.gw_provider.openclaw_stream_request = spy_build
    try:
        yield captured
    finally:
        front.gw_provider.openclaw_stream_request = real_build
        _urlreq.urlopen = orig_urlopen


# --------------------------------------------------------------------------- #
# K3-E2 · 已验证接线（front 半 = 行为级；声明半 = 机制级源锚代理）
# --------------------------------------------------------------------------- #


class K3MemoryIsolationAcceptance(unittest.TestCase):

    maxDiff = None

    def test_k3_e2_front_oneshot_payload_carries_device_user(self):
        """coverage: A1, A3, K3-E2 — front 一次性回合统一携 user=<device_id>
        （261024 换代主锚；行为级，离线真跑）。换代事由 = human 2026-10-24 裁
        R7/OQ-4 隔离口径退役；旧名 test_k3_e2_front_oneshot_payload_no_user
        （断言 payload 恰 {model, messages, stream} 零 user）方向反转、显式
        换代非静默放宽（旧名映射见本件头注首段与 A 节）。断言四路：
        ① 整理收尾链：_story_finish_session 从会话腿 device_ws.headers 提取
          device-id 原值透传 → payload 键集恰 {model, messages, stream, user}
          且 user=<device_id>；messages 恰 [system, user] 且 user 消息 =
          material 纯文（指令挪 request 级 system——旧包装句不再尾随 user）；
        ② 引导链：_story_guide_round 同形状，user 消息 = spoken 纯文；
        ③ 降级路径（must-stay-green 保留面）：device-id 缺席 → payload 缺
          user 键（诚实降级，不冒用身份）；
        ④ 空制品回退（must-stay-green 保留面）：_load_prompt_artifact 返 ""
          → user 消息回填现行完整包装形态（plan.md ③ 形态 2，最坏退回旧
          live 行为）。
        ①② 在现行树设计态红（payload 尚无 user 键、user 消息尚带包装句）；
        ③④ 在现行树与换代后均须绿。"""
        tag = "K3-E2"
        mod, err = _load_front_app()
        self.assertIsNotNone(mod, f"{tag} RED: front_app.py 离线不可装载 —— {err}")
        front = mod
        material = "我昨天去了动物园，看到了大象和猴子。"
        spoken = "从前有一只小猫"
        now = datetime(2026, 10, 24, 12, 0, 0, tzinfo=timezone.utc)
        sid_fin = "sess-k3-e2-finish"
        sid_guide = "sess-k3-e2-guide"
        sid_degraded = "sess-k3-e2-degraded"
        with tempfile.TemporaryDirectory(prefix="k3-e2-oneshot-") as td:
            old_dir, old_acc = front.STORIES_DIR, front._story_acc
            front.STORIES_DIR = str(td)
            front._story_acc = None
            try:
                acc = front._get_story_acc()
                acc.start(sid_fin, now=now)
                acc.feed(sid_fin, material, now=now)
                acc.start(sid_guide, now=now)
                acc.feed(sid_guide, spoken, now=now)
                acc.start(sid_degraded, now=now)
                acc.feed(sid_degraded, spoken, now=now)
                engine_ws = _LegEngineWs()
                device_ws = _LegDeviceWs({"device-id": K3_ONESHOT_DEVICE_ID})
                naked_ws = _LegDeviceWs({"other-header": "x"})
                with _captured_oneshot_payloads(front) as captured:
                    # -- ① 整理收尾链：device-id 头在会话腿上 ----------------
                    record = asyncio.run(front._story_finish_session(
                        sid_fin, acc, [(device_ws, engine_ws)]))
                    self.assertIsNotNone(
                        record,
                        f"{tag} RED(infra): 整理收尾链未归档（{record!r}）—— "
                        "fixture 缺陷，非被测面（LLM 桩应返回非空文本）。")
                    self.assertEqual(
                        len(captured), 1,
                        f"{tag} RED: 整理收尾链未恰一次经 openclaw_stream_request "
                        "构造 payload（单源构造被旁路 = 契约 §四禁 max_tokens "
                        "约束失去唯一承载点）。")
                    p_fin = captured[-1]
                    self.assertEqual(
                        set(p_fin), {"model", "messages", "stream", "user"},
                        f"{tag} RED: 整理一次性回合 payload 键集 = "
                        f"{sorted(p_fin)}，须恰 {{model, messages, stream, "
                        "user}} —— 261024 换代口径：front 一次性回合统一携 "
                        "user=<device_id>（human 2026-10-24 裁 R7/OQ-4 隔离"
                        "口径退役；旧断言「恰 3 键零 user」已反转）。")
                    self.assertEqual(
                        p_fin.get("user"), K3_ONESHOT_DEVICE_ID,
                        f"{tag} RED: 整理一次性回合 payload user = "
                        f"{p_fin.get('user')!r}，须恰会话腿 device-id 头原值 "
                        f"{K3_ONESHOT_DEVICE_ID!r}（不 strip 不归一——与 engine "
                        "端原字符串直传同源，避免分叉出第二个 session 键族）。")
                    self.assertEqual(
                        [m.get("role") for m in p_fin["messages"]],
                        ["system", "user"],
                        f"{tag} RED: 整理一次性回合 messages 形状 = "
                        f"{[m.get('role') for m in p_fin['messages']]}，须恰 "
                        "[system, user]（request 级 system 承载整理制品指令）。")
                    self.assertEqual(
                        p_fin["messages"][-1]["content"], material,
                        f"{tag} RED: 整理回合 user 消息 = "
                        f"{p_fin['messages'][-1]['content']!r}，须为 material "
                        "纯文（指令挪 request 级 system——包装句尾随 user 会随 "
                        "统一 user 通道进设备 session 被 deriver 抽成噪声）。")
                    # -- ② 引导链：同形状 + spoken 纯文 ----------------------
                    captured.clear()
                    front._register_session_legs(sid_guide, device_ws, engine_ws)
                    try:
                        asyncio.run(front._story_guide_round(sid_guide, now))
                    finally:
                        front._unregister_session_legs(device_ws)
                    self.assertEqual(
                        len(captured), 1,
                        f"{tag} RED: 引导回合未恰一次经 openclaw_stream_request "
                        "构造 payload。")
                    p_guide = captured[-1]
                    self.assertEqual(
                        set(p_guide), {"model", "messages", "stream", "user"},
                        f"{tag} RED: 引导一次性回合 payload 键集 = "
                        f"{sorted(p_guide)}，须恰 {{model, messages, stream, "
                        "user}}（261024 换代口径，与整理回合同一 user 通道）。")
                    self.assertEqual(
                        p_guide.get("user"), K3_ONESHOT_DEVICE_ID,
                        f"{tag} RED: 引导一次性回合 payload user = "
                        f"{p_guide.get('user')!r}，须恰 {K3_ONESHOT_DEVICE_ID!r}。")
                    self.assertEqual(
                        p_guide["messages"][-1]["content"], spoken,
                        f"{tag} RED: 引导回合 user 消息 = "
                        f"{p_guide['messages'][-1]['content']!r}，须为 spoken "
                        "纯文（指令挪 request 级 system）。")
                    # -- ③ 降级路径：device-id 缺席 → 缺 user 键 -------------
                    captured.clear()
                    front._register_session_legs(sid_degraded, naked_ws, engine_ws)
                    try:
                        asyncio.run(front._story_guide_round(sid_degraded, now))
                    finally:
                        front._unregister_session_legs(naked_ws)
                    self.assertEqual(
                        len(captured), 1,
                        f"{tag} RED: 降级路径未恰一次构造 payload。")
                    p_deg = captured[-1]
                    self.assertNotIn(
                        "user", p_deg,
                        f"{tag} RED: device-id 缺席时 payload 仍携带 user 键 "
                        f"({p_deg.get('user')!r}) —— 降级口径：device-id 缺席 = "
                        "诚实降级（条件传参 kwargs={}, 缺 user 键），不得冒用"
                        "身份或发空串 user。")
                    self.assertEqual(
                        set(p_deg), {"model", "messages", "stream"},
                        f"{tag} RED: 降级路径 payload 键集 = {sorted(p_deg)}，"
                        "须恰 {model, messages, stream}（无 user 键）。")
                    # -- ④ 空制品回退：user 消息回填现行包装形态 -------------
                    captured.clear()
                    saved_loader = front._load_prompt_artifact
                    front._load_prompt_artifact = lambda filename: ""
                    try:
                        front._organize_and_archive(material, "corr-k3-e2-fb")
                    finally:
                        front._load_prompt_artifact = saved_loader
                    self.assertEqual(
                        len(captured), 1,
                        f"{tag} RED: 空制品回退路径未恰一次构造 payload。")
                    p_fb = captured[-1]
                    self.assertNotIn(
                        "user", p_fb,
                        f"{tag} RED: 空制品回退路径未携 device 身份却带 user 键"
                        " —— 回退路径直调 _organize_and_archive 无 device_id，"
                        "payload 不得凭空带 user。")
                    self.assertEqual(
                        [m.get("role") for m in p_fb["messages"]], ["user"],
                        f"{tag} RED: 空制品回退路径 messages 形状 = "
                        f"{[m.get('role') for m in p_fb['messages']]}，须恰 "
                        "[user]（制品缺失 = 无 request 级 system，user 消息"
                        "回填完整包装形态承指令）。")
                    self.assertEqual(
                        p_fb["messages"][-1]["content"],
                        K3_ONESHOT_ORGANIZE_WRAP.format(material=material),
                        f"{tag} RED: 空制品回退路径 user 消息 = "
                        f"{p_fb['messages'][-1]['content']!r}，须回填现行完整"
                        "包装形态（plan.md ③ 形态 2：最坏情况退回旧 live 行为，"
                        "不是无指令）。")
            finally:
                front.STORIES_DIR = old_dir
                front._story_acc = old_acc

    def test_k3_e2_verified_wiring_declaration_anchors(self):
        """coverage: A1, A3, K3-E2 — 已验证接线声明锚集（机制级源锚代理；
        换代自旧 gate_carrier_anchors，自研 carrier 三锚集作废）：声明工件
        changes 恰 4 键逐字（探针验证 4 键；旧第 5 键 noisePatterns 已随
        261024 换代删除——human 2026-10-24 裁 R7/OQ-4 隔离口径退役，5键→4键
        显式换代非静默放宽）且恰 plugins.entries.openclaw-honcho.* 全前缀；
        顶层 hooks.* 与 agents.entries.*.plugins 零出现（schema 拒收键负锚）；
        workspaceId 恰 "kidpal" 非 "kidpal-probe"；插件版本 pin 记录 1.7.0 +
        包名；插件运行时证据字面（loaded 日志行 + inspect Policy 行）在工作台
        并集在场（静态锚 = 声明/文档并集；运行时行为 = live 齿）；session key
        家族 agent:kidpal:openai-user: 字面保留（261024 换代后 _note 仍须记录；
        设备级 session + 家庭单 owner 画像；工作台内 "agent:kidpal:" 键族唯一
        形态 = openai-user: 变体，任何变体 = 红）；零自研 carrier 残留（零
        honcho-plugin.js 文件 / 零 marker 字面 / 零 /v1/openclaw 端点）；零内联
        secret。"""
        tag = "K3-E2"
        self.assertTrue(
            WORKBENCH.is_dir(),
            f"{tag} RED: {WORKBENCH} 不在世 —— 插件工作台缺失（声明 / apply / "
            "rollback 三件共建此目录）。")
        texts = {str(p.relative_to(WORKBENCH)): txt
                 for p, txt in _iter_tree_text(WORKBENCH)}
        self.assertTrue(texts, f"{tag} RED: 工作台零文件。")
        union = "\n".join(texts.values())

        # -- 声明 4 键逐字（A1；261024 换代 5键→4键） --------------------------
        self.assertTrue(
            DECL_PATH.is_file(),
            f"{tag} RED: 声明工件不在位 —— 冻结契约要求 {DECL_PATH}。")
        decl, derr = _load_json_file(DECL_PATH)
        self.assertIsNone(derr, f"{tag} RED: 声明工件不可解析: {derr}")
        changes = decl.get("changes")
        self.assertIsInstance(changes, list, f"{tag} RED: changes 须为 list。")
        self.assertEqual(
            len(changes), 4,
            f"{tag} RED: changes 条数 = {len(changes)}，须恰 4（探针验证 4 键；"
            "noisePatterns 第 5 键已随 261024 换代退役——human 2026-10-24 裁 "
            "R7/OQ-4 隔离口径退役，头注 B1）。")
        paths = [c.get("path") for c in changes]
        self.assertEqual(
            len(set(paths)), len(paths),
            f"{tag} RED: changes 存在重复 path —— 逐键单写。")
        self.assertEqual(
            _canon_changes(changes), _canon_changes(K3_DECL_CHANGES),
            f"{tag} RED: changes ≠ 冻结 4 键逐字（头注 B1，261024 换代后口径；"
            f"noisePatterns 条目不得残留）。\n实得："
            f"{json.dumps(changes, ensure_ascii=False, indent=2)}")
        for p in paths:
            self.assertTrue(
                str(p).startswith(K3_PLUGIN_ENTRY_PREFIX),
                f"{tag} RED: change path {p!r} 不以 {K3_PLUGIN_ENTRY_PREFIX!r} "
                "为全前缀 —— 已验证接线只认插件条目内全路径。")
            self.assertIsNone(
                K3_TOP_HOOKS_RE.match(str(p)),
                f"{tag} RED: change path {p!r} 命中顶层 hooks.* —— 2026.8.1 "
                "zod strictObject 拒收（K3 崩因 ①，advisory §9 绝对不要做第 "
                "2 条）。allowConversationAccess 的唯一合法路径 = 插件条目内 "
                f"{K3_HOOKS_FULL_PATH}。")
            self.assertIsNone(
                K3_AGENT_PLUGINS_RE.search(str(p)),
                f"{tag} RED: change path {p!r} 命中 agents.entries.*.plugins "
                "—— schema 拒收键（K3 崩因 ①；agent 内嵌插件注册不是接线形状）。")
        ws = _json_get(decl, "plugins.entries.openclaw-honcho.config.workspaceId")
        self.assertEqual(
            ws, K3_WORKSPACE_ID,
            f"{tag} RED: workspaceId = {ws!r}，须恰 {K3_WORKSPACE_ID!r} 非"
            f"{'kidpal-probe'!r}（human 裁决 ②：生产新库，探针数据不迁）。")

        # -- 插件版本 pin + 运行时证据字面（工作台并集静态锚） ------------------
        for anchor in (K3_PLUGIN_PACKAGE, K3_PLUGIN_VERSION_PIN):
            self.assertIn(
                anchor, union,
                f"{tag} RED: 工作台并集缺插件版本锚 {anchor!r} —— 插件版本 pin "
                "1.7.0 与包名须有记录（--pin 安装记录的声明面留痕）。")
        for anchor in (K3_PLUGIN_LOAD_LOG, K3_PLUGIN_POLICY_LINE):
            self.assertIn(
                anchor, union,
                f"{tag} RED: 工作台并集缺插件运行时证据字面 {anchor!r} —— 静态"
                "锚 = 声明/文档并集含两字面（loaded 日志行 = 上游插件 "
                "dist/index.js 原文；Policy 行 = plugins inspect --runtime "
                "核对面）；运行时行为本体 = live 齿判定。")

        # -- session key 家族（非 peer 键） ------------------------------------
        self.assertIn(
            K3_SESSION_KEY_PREFIX, union,
            f"{tag} RED: 工作台并集缺 session key 家族 {K3_SESSION_KEY_PREFIX!r} "
            "—— 验证行为 = 设备级 session + 家庭单 owner 画像（session "
            "metadata openclawSessionKey，advisory §1.3；非 peer 键）。")
        variants = K3_SESSION_KEY_VARIANT_RE.findall(union)
        self.assertEqual(
            variants, [],
            f"{tag} RED: 工作台出现非 openai-user 形态的 agent:kidpal 键族变体 "
            f"{variants} —— 键族不另造：工作台内 agent:kidpal: 唯一形态 = "
            "agent:kidpal:openai-user: 变体。")

        # -- 零自研 carrier 残留（A3 负锚） ------------------------------------
        carrier_files = [p for p in WORKBENCH.rglob("*")
                         if p.is_file() and p.name == K3_CARRIER_FILE]
        self.assertEqual(
            carrier_files, [],
            f"{tag} RED: 工作台存在 {K3_CARRIER_FILE} —— 自研 carrier 已弃用"
            "（不是插件：无 definePluginEntry / openclaw.plugin.json；调用的 "
            "/v1/openclaw/* 端点在 Honcho 源码不存在），须删除不得残留。")
        self.assertNotIn(
            K3_CARRIER_MARKER, union,
            f"{tag} RED: 工作台残留 carrier 自打 marker {K3_CARRIER_MARKER!r} "
            "—— carrier 弃用后零残留（旧报警锚随载体一并作废）。")
        self.assertNotIn(
            K3_CARRIER_ENDPOINT, union,
            f"{tag} RED: 工作台残留 {K3_CARRIER_ENDPOINT} 端点字面 —— 该端点在 "
            "Honcho 源码不存在（路由前缀 /v3），carrier 弃用后零残留。")

        # -- 零内联 secret -----------------------------------------------------
        leaks = K3_SECRET_KEY_RE.findall(union)
        self.assertEqual(
            leaks, [], f"{tag} RED: 工作台内联 secret（sk- 长键）—— 凭据不入仓。")


# --------------------------------------------------------------------------- #
# K3-E6 · openclaw.json 声明式 patch 组合（P1 已应用态为现状锚）+ apply 双形态
# --------------------------------------------------------------------------- #


class K3OpenclawPatchAcceptance(unittest.TestCase):

    maxDiff = None

    def _require_workbench(self, tag):
        self.assertTrue(
            WORKBENCH.is_dir(),
            f"{tag} RED: {WORKBENCH} 不在世 —— 声明式 patch 工作台缺失"
            "（声明 / apply / rollback 三件）。")

    def _decl(self, tag) -> dict:
        self.assertTrue(
            DECL_PATH.is_file(),
            f"{tag} RED: 声明工件不在位 —— 冻结契约要求 {DECL_PATH}。"
            "openclaw.json 含 gateway.token 整文件不入仓，只入无 secret 声明"
            "（K3-E6 铁律）。")
        txt = _read_text(DECL_PATH)
        try:
            decl = json.loads(txt)
        except ValueError as e:  # noqa: BLE001
            self.fail(f"{tag} RED: 声明工件不可解析 —— {e!r}")
        self.assertIsInstance(decl, dict, f"{tag} RED: 声明工件顶层须为 object。")
        return decl

    def test_k3_e6_declaration_schema_and_p1_drift_anchor(self):
        """coverage: A1, K3-E6 — 声明工件结构 schema（case/changes/drift_guard/
        backup/rollback 五顶层键）；changes ≥2 且恰一条 path ==
        plugins.entries.openclaw-honcho.hooks.allowConversationAccess（**插件
        条目内 hooks 全路径**——2026.8.1 顶层 hooks 只有 internal，顶层键被
        strictObject 拒收 = K3 崩因 ①；本断言语境 = 全路径，任何「顶层 hooks
        后缀」读法 = 误读）value true；baseUrl 127.0.0.1:18100 指本机
        loopback；drift_guard 必含 P1 已应用态现状锚（tools.allow = 契约四项
        逐字）——launch-card 红线原样保留；backup/rollback 形制不变；apply 脚
        本在位、引声明工件名、stdlib json、且源码含 OpenClaw 写入口序字面
        （config patch / --dry-run）。"""
        tag = "K3-E6"
        self._require_workbench(tag)
        decl = self._decl(tag)
        for key in ("case", "changes", "drift_guard", "backup", "rollback"):
            self.assertIn(
                key, decl,
                f"{tag} RED: 声明工件缺必需顶层键 {key!r}（冻结契约结构 = case / "
                "changes / drift_guard / backup / rollback，261017/K1-E4 同族）。")
        self.assertIsInstance(decl.get("case"), str)
        self.assertTrue(str(decl.get("case", "")).strip(), f"{tag} RED: case 空。")

        changes = decl.get("changes")
        self.assertIsInstance(changes, list, f"{tag} RED: changes 须为 list。")
        self.assertGreaterEqual(
            len(changes), 2,
            f"{tag} RED: changes 条数 = {len(changes)} < 2。")
        for i, change in enumerate(changes):
            self.assertIsInstance(
                change, dict, f"{tag} RED: changes[{i}] 须为 object。")
            self.assertTrue(
                isinstance(change.get("path"), str) and change["path"].strip(),
                f"{tag} RED: changes[{i}].path 须为非空点分路径。")
            self.assertIn(
                "value", change,
                f"{tag} RED: changes[{i}] 缺 value（本包声明形状 = {{path, value}}，"
                "插件配置为任意 json 子树）。")
        hooks = [c for c in changes if c.get("path") == K3_HOOKS_FULL_PATH]
        self.assertEqual(
            len(hooks), 1,
            f"{tag} RED: path == {K3_HOOKS_FULL_PATH} 的 change 数 = {len(hooks)}，"
            "须恰 1（**插件条目内 hooks 全路径**——探针验证过的接线键形，"
            "advisory §1.2 逐字；顶层 hooks.allowConversationAccess 被 2026.8.1 "
            "schema 拒收，多次声明 = 双写）。")
        self.assertIs(
            hooks[0].get("value"), True,
            f"{tag} RED: 钩子授权 value = {hooks[0].get('value')!r}，须为 true —— "
            "allowConversationAccess=false 则插件钩子不触发、记忆管线整体不工作。")
        honcho = [c for c in changes
                  if "honcho" in str(c.get("path")).lower()
                  and K3_HONCHO_BASE_URL_HOST in json.dumps(c.get("value"), ensure_ascii=False)]
        self.assertTrue(
            honcho,
            f"{tag} RED: 零 change 的 value 携带 baseUrl "
            f"{K3_HONCHO_BASE_URL_HOST} —— 插件 baseUrl 须指本机 loopback"
            "（自托管 Honcho，advisory §1.2）。")

        dg = decl.get("drift_guard")
        self.assertIsInstance(dg, list, f"{tag} RED: drift_guard 须为 list。")
        self.assertTrue(dg, f"{tag} RED: drift_guard 不得为空。")
        for i, entry in enumerate(dg):
            self.assertIsInstance(entry, dict, f"{tag} RED: drift_guard[{i}] 须为 object。")
            self.assertTrue(
                isinstance(entry.get("path"), str) and entry["path"].strip(),
                f"{tag} RED: drift_guard[{i}].path 须为非空字符串。")
            self.assertIn(
                "expect", entry, f"{tag} RED: drift_guard[{i}] 缺 expect。")
        p1_anchor = [e for e in dg if e.get("path") == K3_P1_TOOLS_PATH]
        self.assertTrue(
            p1_anchor,
            f"{tag} RED: drift_guard 未锚定 {K3_P1_TOOLS_PATH} —— launch-card "
            "红线：本包 drift_guard 现状锚 = P1 已应用态（tools.allow = 契约四项"
            "在位），P1 漂移时本包必须 STOP 而非叠加写。")
        self.assertEqual(
            p1_anchor[0].get("expect"), K3_P1_TOOLS_EXPECT,
            f"{tag} RED: P1 现状锚 expect = {p1_anchor[0].get('expect')!r}，须恰 "
            f"{K3_P1_TOOLS_EXPECT}（K1-E4 已应用态逐字）。")

        backup = decl.get("backup")
        self.assertIsInstance(backup, dict, f"{tag} RED: backup 须为 object。")
        self.assertTrue(
            isinstance(backup.get("dir"), str) and backup["dir"].strip(),
            f"{tag} RED: backup.dir 须为非空字符串。")
        pattern = backup.get("pattern")
        self.assertTrue(
            isinstance(pattern, str) and "bak" in pattern,
            f"{tag} RED: backup.pattern 须含 'bak'（config 同目录 glob）。")
        self.assertNotIn("/", pattern, f"{tag} RED: backup.pattern 不得含路径分隔符。")
        rollback = decl.get("rollback")
        script = (rollback or {}).get("script")
        self.assertTrue(
            isinstance(script, str) and script.strip(),
            f"{tag} RED: rollback.script 须为非空字符串。")
        self.assertEqual(
            script, K3_ROLLBACK_NAME,
            f"{tag} RED: rollback.script = {script!r}，冻结契约 = "
            f"{K3_ROLLBACK_NAME}。")
        self.assertTrue(
            (WORKBENCH / script).is_file(),
            f"{tag} RED: rollback 脚本 {script!r} 不在工作台 —— 回滚不可跑。")
        self.assertTrue(
            APPLY_SCRIPT.is_file(),
            f"{tag} RED: {K3_APPLY_NAME} 不在位 —— 冻结契约工件缺失。")
        apply_src = _read_text(APPLY_SCRIPT)
        self.assertIn(
            K3_DECL_NAME, apply_src,
            f"{tag} RED: apply 脚本未引用声明工件名 —— 声明面与消费面不一致。")
        self.assertIsNotNone(
            re.search(r"(?m)^\s*(?:import\b[^\n]*\bjson\b|from\s+json\s+import\b)",
                      apply_src),
            f"{tag} RED: apply 脚本未用 stdlib json（冻结契约：零额外依赖、"
            "不依赖 jq）。")
        self.assertIn(
            "config patch", apply_src,
            f"{tag} RED: apply 脚本零 OpenClaw 写入口字面（config patch）—— "
            "advisory §9 铁律：openclaw.json 只经 OpenClaw 自己的写入口写。")
        self.assertIn(
            "--dry-run", apply_src,
            f"{tag} RED: apply 脚本零 --dry-run 字面 —— 写前必 --dry-run schema "
            "校验（advisory §9 铁律）。")

    def test_k3_e6_apply_idempotent_and_preserves_p1_state(self):
        """coverage: A2, K3-E6 —apply 幂等预检（P1 已应用态 fixture + 插件键
        已达标态）：① config 现状已等于全部声明目标 → 打印 SKIP、exit 0（不
        写、不再备份、不调外部命令）；② P1 组合态零触碰（tools.allow 四项 /
        261017 skills / gateway.token / main 条目原样）——双声明式 patch 顺序
        组合（P1 先、本包后）互不覆盖。"""
        tag = "K3-E6"
        decl = self._decl(tag)
        changes = decl["changes"]
        pattern = decl["backup"]["pattern"]
        pre = _fixture_p1_applied()
        want = _target_state(pre, changes)
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            _write_json_file(cfg, want)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(
                rc, 0, f"{tag} RED: apply rc={rc}（幂等应 0）。"
                f"stdout={out!r} stderr={err!r}")
            self.assertIn(
                "SKIP", out,
                f"{tag} RED: apply 现状已达标未打印 SKIP（幂等预检语义，stdout="
                f"{out!r}）。")
            got, gerr = _load_json_file(cfg)
            self.assertIsNone(gerr, f"{tag} RED: config 不可解析: {gerr}")
            self.assertEqual(
                got, want,
                "K3-E6 RED: 幂等运行改动了已达标 config（SKIP = 完全 no-op）。")
            self.assertEqual(
                _json_get(got, K3_P1_TOOLS_PATH), K3_P1_TOOLS_EXPECT,
                "K3-E6 RED: P1 tools.allow 被改动 —— 双声明式 patch 组合"
                "被破（P1 已应用态是现状锚与保留面）。")
            self.assertEqual(
                _json_get(got, "gateway.token"), _json_get(pre, "gateway.token"),
                "K3-E6 RED: gateway.token 被改动 —— 越界触碰 secret。")
            self.assertEqual(
                _json_get(got, "agents.entries.kidpal.skills"),
                _json_get(pre, "agents.entries.kidpal.skills"),
                "K3-E6 RED: 261017 skills 白名单组合态被抹 —— 声明式 patch "
                "组合共存被破（launch-card：两包均幂等 apply、不整文件改写）。")
            self.assertEqual(
                _json_get(got, "agents.entries.main"), _json_get(pre, "agents.entries.main"),
                "K3-E6 RED: 其它 agent 条目被改动 —— 越界。")
            self.assertEqual(
                _backups(td, pattern), [],
                "K3-E6 RED: 幂等 SKIP 又新建了备份（幂等应不重备份）。")

    def test_k3_e6_apply_drift_guard_exit3(self):
        """coverage: A2, K3-E6 —drift_guard 现状锚（P1 已应用态）不中 → STOP
        exit 3、零写、零备份、零外部命令：P1 tools.allow 被漂移（改白名单 /
        键缺席）时本包必须拒绝叠加——双声明式 patch 的顺序纪律机械承载。"""
        tag = "K3-E6"
        decl = self._decl(tag)
        pattern = decl["backup"]["pattern"]
        for mutate, label in (
                (lambda s: _json_set(s, K3_P1_TOOLS_PATH, ["bash", "browser"]),
                 "tools.allow 被改成别的白名单"),
                (lambda s: s["agents"]["entries"]["kidpal"].pop("tools"),
                 "tools.allow 键缺席（P1 已应用态丢失）")):
            with tempfile.TemporaryDirectory() as td:
                td = Path(td)
                cfg = td / "openclaw.json"
                drifted = _fixture_p1_applied()
                mutate(drifted)
                _write_json_file(cfg, drifted)
                rc, out, err = _run_py(
                    APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
                self.assertEqual(
                    rc, 3,
                    f"K3-E6 RED: drift-guard 不中（{label}）应 exit 3（实得 "
                    f"rc={rc}）。stdout={out!r} stderr={err!r}")
                after, _ = _load_json_file(cfg)
                self.assertEqual(
                    after, drifted,
                    f"K3-E6 RED: drift-guard 拒绝（{label}）后仍改写了 config。")
                self.assertEqual(
                    _backups(td, pattern), [],
                    f"K3-E6 RED: drift-guard 拒绝（{label}）后仍建了备份"
                    "（拒绝 = 完全 no-op）。")

    def _orchestrate(self, tag: str, fail_mode: str = "ok") -> dict:
        """A2 编排桩真跑共用具：P1 已应用态 fixture + 注入 .py 桩
        （--openclaw-bin）真跑 apply 全序。返回 dict(rc/out/err/records/td/cfg/
        pre/target/decl/backup_pattern)；调用方负责 shutil.rmtree(td)。
        records = 桩逐调用 [{argv, config_sha256}]（冻结契约：桩收到完整
        argv，argv[0] = "openclaw"|"systemctl"；config_sha256 = 桩在该次调用
        起点的 config 内容 hash —— 逐调用 hash 协议 = 「零直写 openclaw.json」
        负锚的行为承载）。"""
        decl = self._decl(tag)
        pre = _fixture_p1_applied()
        td = Path(tempfile.mkdtemp(prefix="k3-orch-"))
        cfg = td / "openclaw.json"
        _write_json_file(cfg, pre)
        stub = td / "stub_openclaw.py"
        log = td / "stub-invocations.jsonl"
        stub.write_text(
            _STUB_TEMPLATE % {"log": str(log), "cfg": str(cfg),
                              "fail_mode": fail_mode},
            encoding="utf-8")
        rc, out, err = _run_py(
            APPLY_SCRIPT,
            ["--config", cfg, "--decl", DECL_PATH, "--openclaw-bin", stub])
        records = []
        if log.is_file():
            for line in log.read_text(encoding="utf-8").splitlines():
                if line.strip():
                    records.append(json.loads(line))
        return {"rc": rc, "out": out, "err": err, "records": records, "td": td,
                "cfg": cfg, "pre": pre, "target": _target_state(pre, decl["changes"]),
                "decl": decl, "backup_pattern": decl["backup"]["pattern"]}

    def test_k3_e6_apply_safe_orchestration_call_order(self):
        """coverage: A2, K3-E6 —安全应用编排调用序（advisory §9 序，桩真跑）：
        首个外部命令 = config patch --dry-run（dry-run 先于任何写）；序链
        dry-run → plugins install（@honcho-ai/openclaw-honcho --pin）→
        plugins inspect --runtime → plugins doctor → 正式 config patch（--file、
        无 --dry-run）→ config validate → systemctl restart → 其后有
        health/NRestarts 核对；逐调用 config-hash 协议 = 「零直写
        openclaw.json」冻结负锚的行为承载（正式 patch 调用及其前全部 hash ==
        变更前、其后全部 == 目标态 → config 只在 OpenClaw 写入口被改写一次）；
        终态 config == 声明目标态（4 键 + P1/gateway/skills 保留）。"""
        tag = "K3-E6"
        run = self._orchestrate(tag)
        try:
            self.assertEqual(
                run["rc"], 0,
                f"{tag} RED: 编排 rc={run['rc']}（应 0）。stdout={run['out']!r} "
                f"stderr={run['err']!r}")
            records = run["records"]
            self.assertTrue(
                records,
                f"{tag} RED: 注入桩零调用记录 —— 外部副作用命令未经 --openclaw-bin "
                "注入通道执行（冻结契约：openclaw CLI 与 systemctl 全部经桩，"
                "桩收到完整 argv）。")
            argvs = [r["argv"] for r in records]

            def idx_of(pred, label):
                hits = [i for i, a in enumerate(argvs) if pred(a)]
                self.assertTrue(
                    hits,
                    f"{tag} RED: 编排序缺 {label} 调用（advisory §9 序；实得序 = "
                    f"{[_argv_text(a) for a in argvs]}）。")
                return hits[0]

            i_dry = idx_of(
                lambda a: _oc_cmd(a, ["config", "patch"]) and "--dry-run" in a,
                "config patch --dry-run")
            i_install = idx_of(
                lambda a: bool(a) and a[0] == "openclaw" and "install" in a
                and K3_PLUGIN_PACKAGE in _argv_text(a) and "--pin" in a,
                "plugins install @honcho-ai/openclaw-honcho --pin")
            i_inspect = idx_of(
                lambda a: bool(a) and a[0] == "openclaw" and "inspect" in a
                and "--runtime" in a,
                "plugins inspect --runtime")
            i_doctor = idx_of(
                lambda a: bool(a) and a[0] == "openclaw" and "doctor" in a,
                "plugins doctor")
            i_formal = idx_of(
                lambda a: _oc_cmd(a, ["config", "patch"]) and "--dry-run" not in a
                and "--file" in a,
                "正式 config patch --file")
            i_validate = idx_of(
                lambda a: _oc_cmd(a, ["config", "validate"]),
                "config validate")
            i_restart = idx_of(
                lambda a: bool(a) and a[0] == "systemctl" and "restart" in a,
                "systemctl restart")
            self.assertEqual(
                [i_dry, i_install, i_inspect, i_formal, i_validate, i_restart],
                sorted([i_dry, i_install, i_inspect, i_formal, i_validate,
                        i_restart]),
                f"{tag} RED: 编排序违反 advisory §9（dry-run → install → inspect "
                f"→ 正式 patch → validate → restart；实得序 = "
                f"{[_argv_text(a) for a in argvs]}）。")
            self.assertTrue(
                i_doctor > i_install and i_doctor < i_formal,
                f"{tag} RED: plugins doctor 须在 install 之后、正式 patch 之前"
                f"（§9 序第 4 步；实得 index={i_doctor}）。")
            self.assertTrue(
                any(("health" in _argv_text(a) or "NRestarts" in _argv_text(a))
                    for a in argvs[i_restart + 1:]),
                f"{tag} RED: restart 之后零 health/NRestarts 核对调用（§9 序第 "
                "6 步：确认进程没有在 5 秒级重启 = 崩循环报警）。")
            # 逐调用 hash 协议：正式 patch 调用及其之前 == 变更前；其后 == 终态
            # （pre_sha 取自 pre 快照——config 文件在编排中已被桩改写，不能反推）
            pre_snap = run["td"] / "_pre-snapshot.json"
            _write_json_file(pre_snap, run["pre"])
            pre_sha = _sha_file(pre_snap)
            final_sha = _sha_file(run["cfg"])
            for i, r in enumerate(records):
                if i <= i_formal:
                    self.assertEqual(
                        r["config_sha256"], pre_sha,
                        f"{tag} RED: 第 {i} 次调用（{_argv_text(argvs[i])!r}）时 "
                        "config 内容已被改写 —— 正式 OpenClaw 写入口之前零直写"
                        "（advisory §9 铁律；K3 崩因 = 脚本直写绕过 schema 校验）。")
                else:
                    self.assertEqual(
                        r["config_sha256"], final_sha,
                        f"{tag} RED: 第 {i} 次调用（{_argv_text(argvs[i])!r}）时 "
                        "config 内容 ≠ 编排终态 —— 正式 patch 之后出现了额外"
                        "改写（含任何脚本直写路径）。")
            # 终态：config == 声明目标态（经桩的 OpenClaw 写入口改写一次）
            got, gerr = _load_json_file(run["cfg"])
            self.assertIsNone(gerr, f"{tag} RED: config 不可解析: {gerr}")
            self.assertEqual(
                got, run["target"],
                f"{tag} RED: 编排终态 ≠ 声明目标态（OpenClaw 写入口消费 patch "
                "失败或 patch 产物形状不符）。")
            self.assertEqual(
                _json_get(got, K3_P1_TOOLS_PATH), K3_P1_TOOLS_EXPECT,
                f"{tag} RED: 终态 P1 tools.allow 被破坏。")
        finally:
            shutil.rmtree(run["td"], ignore_errors=True)

    def test_k3_e6_apply_failure_stops_zero_write(self):
        """coverage: A2, K3-E6 —任一步失败即停、失败序零写（桩真跑，两个失败
        点）：① config patch --dry-run schema 拒（§9 序第 2 步）→ 非零退出，
        其后零 install / inspect / doctor / 正式 patch / validate / restart
        调用；② 正式 config patch 失败 → 其后零 validate / restart 调用。
        两场景下逐调用 config-hash 全程 == 变更前、终态 config == 变更前
        （失败序零写 = 「零直写 openclaw.json」负锚在失败路径的行为承载）。"""
        tag = "K3-E6"
        for fail_mode, banned_labels in (
                ("fail_dry_run",
                 ["install", "inspect", "doctor", "正式 patch", "validate",
                  "restart"]),
                ("fail_patch", ["validate", "restart"])):
            run = self._orchestrate(tag, fail_mode=fail_mode)
            try:
                self.assertNotEqual(
                    run["rc"], 0,
                    f"{tag} RED: 失败点 {fail_mode} 编排仍 exit 0（任一步失败即"
                    f"停被破）。stdout={run['out']!r}")
                records = run["records"]
                self.assertTrue(
                    records, f"{tag} RED: 失败场景桩零记录（前置）。")
                argvs = [r["argv"] for r in records]

                def hits(pred):
                    return [i for i, a in enumerate(argvs) if pred(a)]

                if fail_mode == "fail_dry_run":
                    self.assertTrue(
                        hits(lambda a: _oc_cmd(a, ["config", "patch"])
                             and "--dry-run" in a),
                        f"{tag} RED: dry-run 失败场景却无 dry-run 调用（前置）。")
                else:
                    self.assertTrue(
                        hits(lambda a: _oc_cmd(a, ["config", "patch"])
                             and "--dry-run" not in a and "--file" in a),
                        f"{tag} RED: 正式 patch 失败场景却无正式 patch 调用"
                        "（前置）。")
                pre_snap = run["td"] / "_pre-snapshot.json"
                _write_json_file(pre_snap, run["pre"])
                pre_sha = _sha_file(pre_snap)
                for label in banned_labels:
                    if label == "install":
                        banned = hits(
                            lambda a: bool(a) and a[0] == "openclaw"
                            and "install" in a)
                    elif label == "inspect":
                        banned = hits(
                            lambda a: bool(a) and a[0] == "openclaw"
                            and "inspect" in a)
                    elif label == "doctor":
                        banned = hits(
                            lambda a: bool(a) and a[0] == "openclaw"
                            and "doctor" in a)
                    elif label == "正式 patch":
                        banned = hits(
                            lambda a: _oc_cmd(a, ["config", "patch"])
                            and "--dry-run" not in a and "--file" in a)
                    elif label == "validate":
                        banned = hits(lambda a: _oc_cmd(a, ["config", "validate"]))
                    else:
                        banned = hits(
                            lambda a: bool(a) and a[0] == "systemctl"
                            and "restart" in a)
                    self.assertEqual(
                        banned, [],
                        f"{tag} RED: 失败点 {fail_mode} 之后仍出现 {label} 调用 "
                        f"（{banned}）—— 任一步失败即停被破（advisory §9：每一步"
                        "失败即停，回到上一步）。")
                for i, r in enumerate(records):
                    self.assertEqual(
                        r["config_sha256"], pre_sha,
                        f"{tag} RED: 失败场景第 {i} 次调用时 config 已被改写 —— "
                        "失败序零写被破（含任何脚本直写路径）。")
                got, _ = _load_json_file(run["cfg"])
                self.assertEqual(
                    got, run["pre"],
                    f"{tag} RED: 失败场景终态 config ≠ 变更前（失败序零写被破）。")
            finally:
                shutil.rmtree(run["td"], ignore_errors=True)

    def test_k3_e6_apply_timestamped_backup(self):
        """coverage: A2, K3-E6 —写前时间戳备份（编排桩真跑）：config 同目录、
        匹配声明 backup.pattern、文件名含 UTC yyyymmddTHHMMSS、内容 = 原始
        config（P1 已应用态快照）。"""
        tag = "K3-E6"
        run = self._orchestrate(tag)
        try:
            self.assertEqual(
                run["rc"], 0,
                f"{tag} RED: 编排 rc={run['rc']}。stderr={run['err']!r}")
            backups = _backups(run["td"], run["backup_pattern"])
            self.assertTrue(
                backups, f"{tag} RED: apply 未建匹配 backup.pattern 的备份。")
            self.assertRegex(
                backups[0].name, K3_TIMESTAMP_RE.pattern,
                f"{tag} RED: 备份名 {backups[0].name!r} 不含 UTC 时间戳。")
            saved, _ = _load_json_file(backups[0])
            self.assertEqual(
                saved, run["pre"],
                f"{tag} RED: 备份内容 ≠ 变更前 config（P1 已应用态快照失真）。")
        finally:
            shutil.rmtree(run["td"], ignore_errors=True)

    def test_k3_e6_patch_generator_product_four_keys(self):
        """coverage: A2, K3-E6 — patch 生成器（形态①）：--gen-patch 从声明生成
        `openclaw config patch` 消费的 patch 文件；产物可被 stdlib json 解析
        （JSON 子集形态），展平叶键集恰 = 声明 4 键逐字（嵌套树或扁平点分映射
        两形皆可，展平语义一致）；零额外键（含零顶层 hooks.* / 零
        agents.entries.*.plugins——由恰 4 键等价承载）。换代事由 = human
        2026-10-24 裁 R7/OQ-4 隔离口径退役：旧名
        test_k3_e6_patch_generator_product_five_keys（恰 5 键含 noisePatterns）
        随声明第 5 键退役一并换代，名字钉死数与断言数一致。"""
        tag = "K3-E6"
        self._require_workbench(tag)
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            out = td / "honcho-plugin.patch.json"
            rc, out_text, err = _run_py(
                APPLY_SCRIPT, ["--decl", DECL_PATH, "--gen-patch", out])
            self.assertEqual(
                rc, 0,
                f"{tag} RED: patch 生成器 rc={rc}（应 0）。stdout={out_text!r} "
                f"stderr={err!r}")
            self.assertTrue(
                out.is_file(), f"{tag} RED: --gen-patch 未产出 patch 文件。")
            patch, perr = _load_json_file(out)
            self.assertIsNone(
                perr,
                f"{tag} RED: patch 产物不可被 stdlib json 解析: {perr}（冻结"
                "契约：JSON 子集形态，JSON5 超集不得破坏 stdlib 解析——离线"
                "可断言性前提）。")
            leaves = sorted(
                (p, json.dumps(v, sort_keys=True, ensure_ascii=False))
                for p, v in _flatten_leaves(patch))
            want = sorted(
                (c["path"], json.dumps(c["value"], sort_keys=True,
                                       ensure_ascii=False))
                for c in K3_DECL_CHANGES)
            self.assertEqual(
                leaves, want,
                f"{tag} RED: patch 产物展平叶键集 ≠ 声明 4 键逐字（261024 换代；"
                f"noisePatterns 键不得残留）。\n实得："
                f"{leaves}")

    def test_k3_e6_rollback_dry_run_and_restore(self):
        """coverage: A2, K3-E6 —回滚脚本 --dry-run 可跑（exit 0）且零写入；非
        dry-run 一步还原 backup 内容（exit 0）；脚本头注申报三级回滚序（①
        plugins disable + restart 低危首选；② 还原时间戳备份（本脚本）；③
        plugins uninstall）——源锚 = 头注含 plugins disable / plugins uninstall
        字面。"""
        tag = "K3-E6"
        self.assertTrue(
            ROLLBACK_SCRIPT.is_file(),
            f"{tag} RED: 回滚脚本不在位 —— 冻结契约要求 {ROLLBACK_SCRIPT}。")
        rb_src = _read_text(ROLLBACK_SCRIPT)
        self.assertIn(
            "plugins disable", rb_src,
            f"{tag} RED: 回滚脚本头注缺三级回滚序第 ① 级字面（plugins disable "
            "+ restart，低危首选）——三级回滚序申报缺失。")
        self.assertIn(
            "plugins uninstall", rb_src,
            f"{tag} RED: 回滚脚本头注缺三级回滚序第 ③ 级字面（plugins "
            "uninstall，连 npm project 一起清）——三级回滚序申报缺失。")
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            bak = td / "openclaw.json.bak-kidpal-honcho-20261023T000000"
            cur = {"applied": True}
            restored = {"applied": False}
            _write_json_file(cfg, cur)
            _write_json_file(bak, restored)

            rc, _out, err = _run_py(
                ROLLBACK_SCRIPT, ["--config", cfg, "--backup", bak, "--dry-run"])
            self.assertEqual(
                rc, 0, f"{tag} RED: rollback --dry-run rc={rc}（应 0）。stderr={err!r}")
            after, _ = _load_json_file(cfg)
            self.assertEqual(after, cur, f"{tag} RED: --dry-run 改写了 config。")

            rc2, _out2, err2 = _run_py(ROLLBACK_SCRIPT, ["--config", cfg, "--backup", bak])
            self.assertEqual(rc2, 0, f"{tag} RED: rollback rc={rc2}。stderr={err2!r}")
            after2, _ = _load_json_file(cfg)
            self.assertEqual(
                after2, restored, f"{tag} RED: 回滚未还原 backup 内容。")

    def test_k3_e6_no_openclaw_json_in_repo_and_no_secret(self):
        """coverage: A1, K3-E6 — openclaw.json 整文件不入仓（含 gateway 凭据）；
        声明工件零 secret（gateway.token 字样 / token 键 / sk- 长键零出现）。"""
        tag = "K3-E6"
        leaked = [str(p) for p in SERVER.rglob("openclaw.json") if p.is_file()]
        self.assertEqual(
            leaked, [],
            f"{tag} RED: 仓内出现 openclaw.json（含 gateway 凭据）—— 整文件"
            f"零入仓（launch-card 红线）。实得：{leaked}")
        self.assertTrue(
            DECL_PATH.is_file(),
            f"{tag} RED: 声明工件不在位（前置）——负锚无承载面。")
        decl_text = _read_text(DECL_PATH)
        self.assertNotIn(
            "gateway.token", decl_text,
            f"{tag} RED: 声明工件出现 gateway.token 字样。")
        self.assertIsNone(
            re.search(r'"token"\s*:', decl_text),
            f"{tag} RED: 声明工件出现 token 键 —— 声明面不得承载 secret。")
        self.assertEqual(
            K3_SECRET_KEY_RE.findall(decl_text), [],
            f"{tag} RED: 声明工件内联 secret（sk- 长键）。")

    def test_k3_e6_manifest_registers_the_workbench(self):
        """coverage: A3, K3-E6 — manifest 对 261022-openclaw-honcho-plugin 登记
        **恰 3 条**（声明 / apply / rollback；自研 carrier 条目已删除——负锚：
        零 honcho-plugin.js 登记行），src 在仓内存在、dst 在
        /opt/kidpal/openclaw-patch/261022-openclaw-honcho-plugin/ 消费位
        （261013 部署单一事实源纪律）。"""
        tag = "K3-E6"
        self._require_workbench(tag)
        self.assertTrue(
            _HAS_YAML or self.fail(
                f"{tag} RED(infra): PyYAML 不在位——manifest 解析前置缺失。"))
        self.assertTrue(MANIFEST_YAML.is_file(), f"{tag} RED: manifest.yaml 不在场。")
        try:
            doc = yaml.safe_load(MANIFEST_YAML.read_text(encoding="utf-8"))
        except Exception as e:  # noqa: BLE001
            self.fail(f"{tag} RED: manifest.yaml 解析失败: {e!r}")
        entries = [
            e for e in (doc or {}).get("entries", []) or []
            if isinstance(e, dict) and K3_WORKBENCH_TOKEN in (
                str(e.get("src") or "") + str(e.get("dst") or ""))]
        self.assertEqual(
            len(entries), 3,
            f"{tag} RED: manifest 对 {K3_WORKBENCH_TOKEN} 的登记条目 "
            f"{len(entries)} ≠ 3 —— 工作台登记恰 3 条（decl + apply + rollback；"
            "自研 carrier 条目删除，A3 撤净）。")
        srcs = [str(e.get("src") or "") for e in entries]
        expected_srcs = {f"{K3_WORKBENCH_TOKEN}/{K3_DECL_NAME}",
                         f"{K3_WORKBENCH_TOKEN}/{K3_APPLY_NAME}",
                         f"{K3_WORKBENCH_TOKEN}/{K3_ROLLBACK_NAME}"}
        got_srcs = set()
        for s in srcs:
            parts = s.replace("\\", "/").split("/")
            got_srcs.add("/".join(parts[-2:]))
        self.assertEqual(
            got_srcs, expected_srcs,
            f"{tag} RED: manifest 登记的 src 集合 = {sorted(got_srcs)}，须恰 "
            f"{sorted(expected_srcs)}（carrier 条目删除、三件齐）。")
        carrier_entries = [s for s in srcs if s.endswith(K3_CARRIER_FILE)]
        self.assertEqual(
            carrier_entries, [],
            f"{tag} RED: manifest 仍登记 {K3_CARRIER_FILE} —— 自研 carrier 已"
            "弃用，零登记残留（A3 负锚）。")
        missing = [s for s in srcs if s and not (REPO_ROOT / s).is_file()]
        self.assertEqual(missing, [], f"{tag} RED: 登记的仓内源缺失: {missing}。")
        bad_dst = [str(e.get("dst") or "") for e in entries
                   if not str(e.get("dst") or "").startswith(
                       "/opt/kidpal/openclaw-patch/" + K3_WORKBENCH_TOKEN)]
        self.assertEqual(
            bad_dst, [], f"{tag} RED: 登记条目 dst 越出 openclaw-patch 消费位: {bad_dst}。")


def _load_json_file(path: Path):
    txt = _read_text(path)
    if not txt:
        return None, f"{path} 缺失或为空"
    try:
        return json.loads(txt), None
    except ValueError as e:  # noqa: BLE001
        return None, f"{path} 非法 JSON: {e!r}"


def _write_json_file(path: Path, obj) -> None:
    path.write_text(json.dumps(obj, ensure_ascii=False, indent=2), encoding="utf-8")


# --------------------------------------------------------------------------- #
# LIVE 齿 —— 三证探针 / seeded 负向探针集 / 降级实测
# （@pytest.mark.live，不 auto-run；conftest 机械 skip，ARQ-260901-g3）
# --------------------------------------------------------------------------- #

_LIVE_SCHEMA_NOTE = (
    "三证探针证据 schema = {plugin_runtime: {load_log_hit, inspect_policy_hit}, "
    "main_link: [{openclaw_session_key, db_message_delta, gateway_log_hit}], "
    "one_shot: [{round, openclaw_session_key, user_text_in_db_delta}]}（261024 "
    "换代判定口径，冻结于本件头注 D 节）；seeded 证据 schema = {probes: "
    "[{category, seed, query, profile_hit, reply_hit}]}；降级证据 schema = "
    "{degradation: {honcho_stopped, conversation_ok, retry_storm, "
    "user_visible_error}}")


class K3MemoryProbeLiveAcceptance(unittest.TestCase):
    """live 证据判定齿（三证 schema，换代自 marker 双证）：① session metadata
    openclawSessionKey（前缀 agent:kidpal:openai-user:）+ ② 消息条数（DB 增量）
    + ③ 网关日志行；one_shot 判定口径 = 261024 换代（每条证据增
    openclaw_session_key 前缀断言——一次性回合与主链路同落设备 session 键族；
    user_text_in_db_delta == true 正证——user 统一携入后原文进设备 session 库，
    旧甲案「user 原文零出现」判定随 human 2026-10-24 裁 R7/OQ-4 隔离口径退役；
    assistant 回复入库 = 残余 R1 申报、不判红）+ E3 seeded 负向探针集
    （S-F5.1-AC3 / NFR-PRIV-1 量化口径不变）+ E5 降级实测（停 Honcho 容器 →
    对话照常）。

    probe ≠ exercise 注记（DEV-GOVERNANCE 验收测试纪律 §2）：本类只对 live 相位
    **真实产出**的结构化证据施加判定；真实上 ECS 发探针（含 DB/日志三证采集、
    停容器降级实测、OQ-3 实测）归部署轮执行（Pod 不自行上 ECS，
    awaiting-human:live-deploy）。证据缺位 = live 相位设计态红（不 auto-run）。
    """

    maxDiff = None

    if _HAS_PYTEST:
        pytestmark = pytest.mark.live

    def _evidence_files(self, tag) -> list[Path]:
        files = sorted(EVIDENCE_DIR.glob("*.json")) if EVIDENCE_DIR.is_dir() else []
        self.assertTrue(
            files,
            f"{tag} LIVE RED: {EVIDENCE_DIR} 零结构化探针证据 —— 部署轮须先把 "
            f"三证探针 / seeded 负向集 / 降级实测证据落盘"
            f"（{_LIVE_SCHEMA_NOTE}）。",
        )
        return files

    def test_live_k3_e2_verified_wiring_probes_three_evidence(self):
        """coverage: A3, K3-E2 — 已验证接线三证探针：plugin_runtime 两证全真
        （loaded 日志行 + inspect Policy 行）+ 主链路正证（携 user 真实聊天回合
        三证：session metadata 前缀 agent:kidpal:openai-user: / DB 消息增量
        ≥1 / 网关日志行）+ 一次性回合正证（guide 与 organize 各 ≥1，261024
        换代口径：逐条 openclaw_session_key 前缀 agent:kidpal:openai-user: =
        与主链路同 session 键族，user 原文进设备 session 库；assistant 回复
        入库 = 残余 R1 申报、不判红）。逐条判定，任一命中形态不符 = live 红。"""
        tag = "K3-E2(live)"
        docs = []
        for p in self._evidence_files(tag):
            obj, err = _load_json_file(p)
            self.assertIsNotNone(obj, f"{tag} LIVE RED: {p} 不可解析: {err}")
            if "main_link" in obj:
                docs.append((p, obj))
        self.assertTrue(
            docs,
            f"{tag} LIVE RED: 证据文件缺 main_link 段（{_LIVE_SCHEMA_NOTE}）。")
        positive = 0
        one_shot_rounds: set = set()
        for p, obj in docs:
            plugin_runtime = obj.get("plugin_runtime") or {}
            self.assertTrue(
                plugin_runtime.get("load_log_hit"),
                f"{tag} LIVE RED: {p.name} plugin_runtime.load_log_hit = "
                "false —— 网关日志零 'Honcho memory plugin loaded'（上游插件 "
                "dist/index.js 原文；插件未加载 = 接线未生效）。")
            self.assertTrue(
                plugin_runtime.get("inspect_policy_hit"),
                f"{tag} LIVE RED: {p.name} plugin_runtime.inspect_policy_hit = "
                "false —— plugins inspect openclaw-honcho --runtime 零 "
                "'Policy: allowConversationAccess: true'（插件条目内 hooks 授权"
                "未生效）。")
            for item in obj.get("main_link") or []:
                positive += 1
                session_key = str(item.get("openclaw_session_key") or "")
                self.assertTrue(
                    session_key.startswith(K3_SESSION_KEY_PREFIX),
                    f"{tag} LIVE RED: {p.name} 主链路 openclaw_session_key = "
                    f"{session_key!r} 不以前缀 {K3_SESSION_KEY_PREFIX!r} 开头 "
                    "（证① session metadata；设备级 session + 家庭单 owner "
                    "画像 = 已验证接线口径，非 peer 键）。")
                self.assertGreaterEqual(
                    int(item.get("db_message_delta") or 0), 1,
                    f"{tag} LIVE RED: {p.name} 主链路 db_message_delta = "
                    f"{item.get('db_message_delta')!r} < 1（证② DB 消息增量："
                    "真实聊天回合须有消息入库）。")
                self.assertTrue(
                    item.get("gateway_log_hit"),
                    f"{tag} LIVE RED: {p.name} 主链路 gateway_log_hit = false "
                    "（证③ 网关日志行缺失）—— 三证不全 = live 红。")
            for item in obj.get("one_shot") or []:
                one_shot_rounds.add(item.get("round"))
                one_shot_key = str(item.get("openclaw_session_key") or "")
                self.assertTrue(
                    one_shot_key.startswith(K3_SESSION_KEY_PREFIX),
                    f"{tag} LIVE RED: {p.name} 一次性回合（{item.get('round')}）"
                    f"openclaw_session_key = {one_shot_key!r} 不以前缀 "
                    f"{K3_SESSION_KEY_PREFIX!r} 开头 —— 261024 换代口径：一次性"
                    "回合与主链路同落 agent:kidpal:openai-user:<device_id> 设备 "
                    "session 键族（只证「不再挡」不够，要证「全部落同一 session "
                    "键族」）。")
                self.assertIs(
                    item.get("user_text_in_db_delta"), True,
                    f"{tag} LIVE RED: {p.name} 一次性回合（{item.get('round')}）"
                    "user_text_in_db_delta = false —— 261024 换代口径反转：user "
                    "统一携入后 user 原文须进设备 session 库（旧甲案「user 原文"
                    "零出现」判定随 human 2026-10-24 裁 R7/OQ-4 隔离口径退役）；"
                    "assistant 回复入库 = 残余 R1 申报、不判红。",
                )
        self.assertGreaterEqual(
            positive, 1,
            f"{tag} LIVE RED: 主链路正证零条 —— 缺携 user 真实聊天回合的三证。")
        self.assertTrue(
            {"guide", "organize"} <= one_shot_rounds,
            f"{tag} LIVE RED: 一次性回合正证未覆盖 guide 与 organize 两类"
            f"（实得 {sorted(r or '' for r in one_shot_rounds)}）—— arch §6.7 "
            "六行表第 1/2 行各须一次正证探针。")

    def test_live_k3_e3_seeded_negative_probe_set_zero_hit(self):
        """coverage: K3-E3, S-F5.1-AC3, NFR-PRIV-1 — seeded 负向探针集：注入
        家长姓名 / 电话 / 家庭安排后再问询（含变着问法套问），画像与答复
        100% 零命中。三档 category 各 ≥1、全部 probes 双零——任一命中即 live 红
       （三层纵深任一失守即不达标：来源门 / 抽取策略 / 注入过滤）。"""
        tag = "K3-E3(live)"
        docs = []
        for p in self._evidence_files(tag):
            obj, err = _load_json_file(p)
            self.assertIsNotNone(obj, f"{tag} LIVE RED: {p} 不可解析: {err}")
            if "probes" in obj:
                docs.append((p, obj))
        self.assertTrue(
            docs,
            f"{tag} LIVE RED: 证据文件缺 seeded probes 段"
            f"（{_LIVE_SCHEMA_NOTE}）。")
        categories: set = set()
        total = 0
        for p, obj in docs:
            for probe in obj.get("probes") or []:
                total += 1
                categories.add(probe.get("category"))
                self.assertFalse(
                    probe.get("profile_hit") or probe.get("reply_hit"),
                    f"{tag} LIVE RED: {p.name} seeded 探针（{probe.get('category')}："
                    f"{probe.get('seed')!r} → {probe.get('query')!r}）出现命中"
                    f"（profile_hit={probe.get('profile_hit')} "
                    f"reply_hit={probe.get('reply_hit')}）—— 家长隐私被记忆/复述"
                    "（S-F5.1-AC3 / NFR-PRIV-1 ①：注入式探针 100% 不回忆；"
                    "三层纵深任一失守即不达标）。",
                )
        self.assertGreaterEqual(
            total, 3, f"{tag} LIVE RED: seeded 探针总数 = {total} < 3（三档各 ≥1）。")
        self.assertTrue(
            {"姓名", "电话", "家庭安排"} <= categories,
            f"{tag} LIVE RED: seeded 探针 category 未覆盖三档（实得 "
            f"{sorted(c or '' for c in categories)}）—— 家长姓名 / 联系电话 / "
            "家庭安排三类注入缺一不可（S-F5.1-AC3 枚举）。",
        )

    def test_live_k3_e5_degradation_probe(self):
        """coverage: A4, K3-E5 — 降级实测（live 申报形态，host 零假绿断言：
        仓内无上游插件 vendor 源码可源锚，host 文本锚随 carrier 作废）：停
        Honcho 容器 → 对话照常（conversation_ok）、无重试风暴（retry_storm）、
        无用户可见报错（user_visible_error）。任一不符 = live 红。"""
        tag = "K3-E5(live)"
        docs = []
        for p in self._evidence_files(tag):
            obj, err = _load_json_file(p)
            self.assertIsNotNone(obj, f"{tag} LIVE RED: {p} 不可解析: {err}")
            if "degradation" in obj:
                docs.append((p, obj))
        self.assertTrue(
            docs,
            f"{tag} LIVE RED: 证据文件缺 degradation 段（{_LIVE_SCHEMA_NOTE}）"
            "—— 部署轮须实测一次：停 Honcho 容器 → 对话照常。")
        for p, obj in docs:
            d = obj.get("degradation") or {}
            self.assertTrue(
                d.get("honcho_stopped"),
                f"{tag} LIVE RED: {p.name} degradation.honcho_stopped = false "
                "—— 降级实测前置（Honcho 容器须真实停止）不成立，证据无效。")
            self.assertTrue(
                d.get("conversation_ok"),
                f"{tag} LIVE RED: {p.name} degradation.conversation_ok = false "
                "—— Honcho 不可达阻塞了对话（K3-E5：记忆失败不阻塞对话、不进"
                "用户可见兜底）。")
            self.assertFalse(
                d.get("retry_storm"),
                f"{tag} LIVE RED: {p.name} degradation.retry_storm = true —— "
                "无界重试风暴（K3-E5：重试须有界或直接降级）。")
            self.assertFalse(
                d.get("user_visible_error"),
                f"{tag} LIVE RED: {p.name} degradation.user_visible_error = "
                "true —— 记忆失败泄漏进用户可见兜底（K3-E5 红线）。")


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
