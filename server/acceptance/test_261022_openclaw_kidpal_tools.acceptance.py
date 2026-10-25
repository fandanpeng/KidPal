"""Frozen acceptance tests — 261022-m5-k1-prompt-carrier-migration K1-E4
（**2026-10-24 锚换代** → **2026-10-25 锚换代**）+ 261024-memory-recall-tool-enable
K1-E4-B + 261025-kidpal-tools-profile-fix K1-E4-C（防回归锚）
(subsystem: server/deploy/261022-openclaw-kidpal-tools/ +
server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md；TaskPackage =
delivery/taskpackages/261025-kidpal-tools-profile-fix/taskpackage.json；
上游 = delivery/taskpackages/261024-memory-recall-tool-enable/taskpackage.json
notes ⑦ + ECS live 实证（主线程 2026-10-25 亲核）：systemPromptReport.tools.entries
仅 [read, write, apply_patch, exec, web_search]，per-agent
agents.entries.kidpal.tools.profile="full" 覆盖全局 coding 后
honcho_search_conclusions 可达、答出 9-30 事实「王一诺」）。

===========================================================================
锚换代注记（2026-10-25 · 261025-kidpal-tools-profile-fix）
===========================================================================

换代事由（live 实证，本包 provenance）= **加名 ≠ 可达**。P0 批（261024）把
honcho_search_conclusions 加进 agents.entries.kidpal.tools.allow（四项 → 五项），
但 live 验证发现插件工具根本不在递给模型的工具集内：OpenClaw 工具过滤链
（docs/tools/multi-agent-sandbox-tools.md）逐层收窄，全局 tools.profile="coding"
第一道即排除插件工具（group:plugins 不在 coding 内），之后 tools.allow 只能在
**已剩下的集**里挑——加不回被 profile 滤掉的工具名。

**根因注释锚（逐字，禁删；K1-E4-C 自锚）**：
    过滤链 profile 先行，allow 只能在剩余集里挑——加名 ≠ 可达

修复 = per-agent tools.profile="full" 覆盖全局 profile（agent profile 覆盖全局
profile 有文档依据），allow 仍收窄到恰五项。live 已应用并实测生效。本件把 live
已验证形态固化成冻结断言，并把本次教训钉成 K1-E4-C 防回归锚。

换代内容（**显式换代，不是静默放宽**）：

  · 声明写入面：单键 `change {path, allow}` → **双叶键 `changes [{path, value}]`**
    —— 恰两条：agents.entries.kidpal.tools.profile = "full" +
    agents.entries.kidpal.tools.allow = 恰五项。`deny` **不进写入面**（live 升级态
    deny: [] 由幂等分支天然覆盖；deny 非空 = 基线集以外 → fail-closed 判 drift）。
  · drift_guard：从「基线键 unset + 旧四项」升级为**三条叶路径 AND**
    （tools.deny / tools.profile / tools.allow 各一条 expect_any）：
      ① fresh 机（profile 缺席 + allow 缺席）        → 接受
      ② 旧四项 live（allow = 上一代四项）             → 接受
      ③ **P0 中间态（allow 五项 + profile 缺席）**     → 接受（本包最典型升级起点；
         profile 缺席 → apply 补 full；纠错 ①：拒绝它等于拒绝自己的升级路径）
      ④ live 升级态（profile full + allow 五项）       → 接受（幂等 SKIP 承载）
    profile **在场且值 ≠ "full"**（如 "coding"）→ drift；deny 非空 / allow 项数
    不符 / 父键在场但非对象（_MALFORMED 哨兵，不再与「键缺席」混同）→ drift。
  · 幂等预检与 drift **联合**（定向收紧）：SKIP 条件从「target == config」升级为
    「target == config **且** drift 基线无漂移」。否则
    profile:full + allow 五项 + deny 非空 会裸 SKIP，绕过「deny 非空判 drift」红线。
    对 live（deny 空）行为不变（仍 SKIP 零写零备份）；两叶达标但 deny 非空 → exit 3。
    与姊妹脚本 apply_honcho_plugin.py 的「SKIP 先于 drift」**有意不同**（防后来者
    当不一致修掉）。
  · --gen-patch：展平叶键从**一个**（tools.allow）变为**恰两个**
    （tools.profile / tools.allow），值正确、零 config 读写。
  · 放宽项（显式申报，TaskPackage exitCriteria）：① 叶路径下 tools 对象内**未知
    同级键被容忍**（旧「多键即拒」对象形态语义消失）；② allow 比较是**列表顺序
    敏感**相等（顺序不同 = drift，fail-closed——人对着 live 实读核对）。两条均
    要求在声明 `_note` 与本件头注留痕。
  · rollback_agent_tools.py **零改**（备份整档还原机制，语义与键数无关）。
  · manifest.yaml kidpal-tools 三条 note 陈旧字面换代（本件新增负锚：note 不得
    再出现「恰四项」；语义锚：note 须出现 profile 与五项口径）。
  · **K1-E4-C（新条目，防回归锚）**：三件机械承载 —— ① 声明 changes 含
    tools.profile="full"；② profile 在场且 ≠ "full" → exit 3（真跑）；③ allow
    五项 + profile 缺席 → 可升级（真跑）。外加根因注释锚逐字自锚（见上）。

旧 → 新断言对照（2026-10-25 换代，显式留痕，禁静默放宽）：

  · test_k1_e4_declaration_present_and_schema（保留；change 单键 → changes 双叶键，
    新增「旧 change 顶层键退役」负锚）
  · test_k1_e4_allowlist_exactly_five（保留；改读 changes 的 allow 叶值；新增
    profile="full" 与「deny 不进写入面」）
  · 【新增】test_k1_e4_declaration_note_records_drift_upgrade_scope（_note 换代留痕）
  · test_k1_e4_drift_guard_accepts_unset_and_legacy_four →
    **test_k1_e4_drift_guard_upgrade_baselines**（三叶路径 AND；profile / deny 进
    基线面；P0 中间态显式列入接受集）
  · test_k1_e4_apply_script_present_and_consumes_declaration（保留；新增「消费
    changes 双叶键」源锚）
  · test_k1_e4_apply_applies_then_idempotent（保留；目标态扩两叶键）
  · 【新增】test_k1_e4_apply_skips_live_upgraded_state（live 升级态 + deny: [] →
    SKIP 零写零备份）
  · test_k1_e4_apply_accepts_legacy_four_baseline（保留）
  · test_k1_e4_apply_drift_guard_exit3（保留；用例集扩「profile 在场且非 full」/
    「allow 项数不符（三项）」）
  · 【新增】test_k1_e4_apply_deny_nonempty_is_drift_exit3（幂等预检与 drift 联合；
    两叶达标 + deny 非空 → exit 3，不得裸 SKIP）
  · test_k1_e4_apply_timestamped_backup（保留）
  · test_k1_e4_apply_preserves_other_keys_and_composes_with_skills_patch（「只改
    一键」升级为「pre 与终态展平叶差集恰 = {profile: "full", allow: 五项}」）
  · test_k1_e4_gen_patch_product_exactly_target_key_and_zero_config_write →
    **test_k1_e4_gen_patch_product_exactly_target_leaves_and_zero_config_write**
    （一叶 → 恰两叶）
  · test_k1_e4_rollback_dry_run_and_restore（保留，语义零改）
  · test_k1_e4_no_openclaw_json_in_repo_and_no_secret_in_declaration（保留）
  · test_k1_e4_manifest_registers_the_patch_workbench（保留；新增 manifest note
    陈旧字面负锚 + profile / 五项语义锚）
  · 【新增类 ToolsProfileReachabilityAcceptance】K1-E4-C 四条（根因注释锚自锚 /
    声明含 profile="full" / profile 非 full → exit 3 真跑 / allow 五项 + profile
    缺席可升级真跑）
  · K1-E4-B（MemoryRecallRoundAcceptance）零改动；K1-E4 其余 apply / 备份 / 回滚 /
    零入仓 / manifest 语义零回退，仅随两叶项目标态同步期望值。

上一代锚换代注记（2026-10-24 · 261024-memory-recall-tool-enable，保留不抹）：
allow 四项 → 恰五项 {read, write, exec, web_search, honcho_search_conclusions}
（honcho_search_messages 仍不放行——原始消息面含家长代述 / 流程内容）；
drift_guard 从「键 unset」扩为「升级基准」（新机态 + live 既有态双接受）；
apply 增 --gen-patch 形态；新增 K1-E4-B（AGENTS.md 记忆回忆回合行 + 记忆节检索
澄清）。该代对照表已随本次换代吸收（见上），旧名映射以本节为准。

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；纯 python / 离线可跑 / 导入期
零网络 I/O；仿 server/acceptance/test_261017_openclaw_kidpal_skills.acceptance.py
头注 FROZEN OFFLINE CONTRACT 同形制）

落位目录 = server/deploy/261022-openclaw-kidpal-tools/

  1) kidpal-agent-tools-declaration.json —— 声明工件（**JSON**，json.loads 解析，
     零额外依赖）。必需顶层键：case / changes / drift_guard / backup / rollback
     （旧单键 `change` **退役**，不得残留）。
       · case : 非空字符串（取值不作枚举约束）。
       · changes : 非空 list，**恰 2 条**，每条 {path, value}：
           - agents.entries.kidpal.tools.profile = "full"
           - agents.entries.kidpal.tools.allow = **恰五项**、无重复、集合 =
             {read, write, exec, web_search, honcho_search_conclusions}
             （顺序不作语义；honcho_search_messages 不得出现 = 原始消息面负锚）
           - path 集合恰 = {profile, allow} 两叶；**deny 不进写入面**（任何
             path == agents.entries.kidpal.tools.deny 的 change = 红）。
           - 列表顺序（profile 在前、allow 在后）= T1 排版约定、**非语义**，
             本件不钉（钉了 = 脆性断言 + 零语义收益）。
           - value 深拷贝后写入（apply 不得共享声明对象引用）。
       · drift_guard : 非空 list，每项 object 须有非空点分 path，且**恰含
         expect 或 expect_any 之一**。**三条叶路径 AND**（deny / profile / allow
         各恰一条，判定面缺一条 = 红）：
           - agents.entries.kidpal.tools.deny    : expect_any ["unset", []]；
             候选集恰此二者（deny 非空 = 基线集以外 → drift）
           - agents.entries.kidpal.tools.profile : expect_any ["unset", "full"]；
             候选集恰此二者 —— **profile 缺席 = 接受**（apply 补 full，纠错 ①），
             只有 profile 在场且值 ≠ "full"（如 "coding"）才判 drift
           - agents.entries.kidpal.tools.allow   : expect_any
             ["unset", <上一代四项 [read,write,exec,web_search]>,
              <五项 = changes 里 allow 叶值的逐字列表（顺序同）>]；
             候选集恰此三者。五项候选的**列表顺序须与 allow 叶值一致**——allow
             比较是列表顺序敏感相等，顺序不一致则 live 升级态永远判 drift。
         "unset" 的语义 = **键缺席**；父键在场但形状不对（如 tools 为字符串
         "disabled"）= 真漂移（fail-closed；apply 须用 _MALFORMED 类哨兵把
         「父键非对象」与「键缺席」拆开，不得按 unset 放行后静默覆盖畸形配置）。
         显式接受集 = ① fresh 全缺席 ② 旧四项 live ③ **P0 中间态（allow 五项
         + profile 缺席）** ④ live 升级态（profile full + allow 五项，含 deny []）。
       · backup : {"dir": "<ECS 备份目录>", "pattern": "<含 'bak' 的命名模式>"}；
         pattern 是 config 同目录下备份文件的 glob，不得含路径分隔符。
       · rollback : {"script": "rollback_agent_tools.py"}。
       · _note : 非空字符串，须留痕（语义锚组，措辞可换、口径不可删）：
         profile 键面 / 五项（含 honcho_search_conclusions）/ profile 缺席 = 接受
         / 未知同级键被容忍（放宽项申报）/ allow 顺序敏感（放宽项申报）/
         deny 非空判 drift。

  2) apply_agent_tools.py —— **双形态**（全离线 `sys.executable <script> …` 可真跑）。
     形态① 直写应用（语义序 = 261017 PV-A6 同形制，**含本包定向收紧**）：
       ① **幂等预检（与 drift 联合）**：config 现状已等于声明目标（两叶键）**且**
          drift 基线无漂移 → 打印 SKIP 并 exit 0（不写、不再备份）。
          两叶已达标但 deny 非空 → **不进 SKIP**，落入 ② 判 drift → exit 3
          （不得裸跑 target==config 绕过 deny 红线；与姊妹脚本 honcho
          「SKIP 先于 drift」有意不同）。
       ② drift-guard：逐条核对 drift_guard（expect / expect_any 语义见 §1），任一
          条目不中 → 打印 STOP 信息并 **exit 3**（不写、不备份）。
       ③ 备份：写前建时间戳备份，**落 config 同目录**，文件名 = config basename +
          ".bak-..." + UTC `yyyymmddTHHMMSS`（内容 = 原始 config；须匹配声明
          backup.pattern 的 glob）。
       ④ 应用：把 changes 的**两叶键**写进 config；**只改这两个叶键**，其余键
          （含 gateway.token、agents.entries 下其它 agent、全局 tools 块、
          261017 skills 白名单键、以及 tools 对象内的 deny / 未知同级键）
          原样保留。展平后 pre → 终态的**叶差集恰 = {profile: "full",
          allow: 五项}**。成功 exit 0。
     形态② patch 生成器：`--gen-patch <out>`（配合 --decl）从声明生成
       `openclaw config patch` 消费的 patch 文件——JSON 子集形态（stdlib json 可
       解析；JSON5 超集允许但不得破坏 stdlib 解析）；展平叶键集**恰 = 两个叶键**
       {tools.profile: "full", tools.allow: 五项}（嵌套树或扁平点分映射两形皆可，
       展平语义一致）；**不读 config、不写 config、零外部命令、零备份**；成功 exit 0。
     CLI/参数：--config（默认 /root/.openclaw/openclaw.json）、--decl（默认同目录
     kidpal-agent-tools-declaration.json）、--gen-patch <out>。
     用 stdlib json（**不依赖 jq**）。头注 / 报错文案须按 changes 双叶键口径
     （「声明 changes 缺 path/value」），不得残留「只改 tools.allow」单键口径。
     退出码：直写形态缺 config / 两形态缺声明 / 非法 JSON → exit 2
     （--gen-patch 不读 config，故 config 缺失不构成错误）。

  3) rollback_agent_tools.py —— 一步回滚（Python，语义零换代；备份整档还原，
     与键数无关）。CLI：--config、--backup（默认取 config 旁最新声明 backup.pattern
     匹配件）、--dry-run（只打印意图、exit 0、不写）。非 dry-run：还原 backup 内容
     到 config，exit 0。

  4) openclaw.json 整文件不入仓（含 gateway.token）；声明工件内不得出现
     gateway.token 字样、不得出现 token 键。

  5) manifest.yaml 登记：261022-openclaw-kidpal-tools 声明 / apply / rollback
     三件至少各一条（dst = /opt/kidpal/openclaw-patch/261022-openclaw-kidpal-tools/
     下），部署单一事实源（261013 纪律）。三条 note 陈旧字面换代：不得再出现
     「恰四项」，须出现 profile 键面与五项口径。

  6) AGENTS.md（server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md）——
     K1-E4-B 面（**本包零改动**），断言原样保留：回合路由表新增「记忆回忆回合」
     行（additive，既有五行零改动）+ 记忆节保留「不调用任何工具去记」原句 +
     检索澄清句；既有锚兼容红线（K1-E1 五行 / K3_P1_AGENTS_ANCHORS / 261023 A3
     四锚 + 对象锁定行逐字）。

===========================================================================
强度申报（契约条目 → 承载断言 → 性质档）：

  · 声明在位 / schema / change 单键退役         → test_k1_e4_declaration_present_and_schema → 机制级
  · 声明写入面恰两叶（profile full + allow
    恰五项，deny 不进写入面）                   → test_k1_e4_allowlist_exactly_five → 机制级
  · 声明 _note 换代留痕（含两条放宽项申报）      → test_k1_e4_declaration_note_records_drift_upgrade_scope → 源锚代理
  · drift_guard 三叶路径 AND + 四条接受基线
    （fresh / 旧四项 / P0 中间态 / live 升级态）  → test_k1_e4_drift_guard_upgrade_baselines → 机制级
  · apply 消费 changes 双叶键声明              → test_k1_e4_apply_script_present_and_consumes_declaration → 源锚代理
  · apply 真跑：fresh 基线 → 两叶目标态        → test_k1_e4_apply_applies_then_idempotent → 行为级（真跑）
  · 幂等（二次零语义变更、零新备份）             → test_k1_e4_apply_applies_then_idempotent → 行为级（真跑）
  · live 升级态（两叶 + deny []）→ SKIP 零写     → test_k1_e4_apply_skips_live_upgraded_state → 行为级（真跑）
  · apply 真跑：旧四项基线 → 可升级             → test_k1_e4_apply_accepts_legacy_four_baseline → 行为级（真跑）
  · 真 drift 五态拒绝（异值 / 六项 / allow 三项 /
    tools 畸形 / profile 非 full）exit 3 零写零备份 → test_k1_e4_apply_drift_guard_exit3 → 行为级（真跑）
  · 幂等预检与 drift 联合（两叶达标 + deny 非空
    → exit 3，不得裸 SKIP）                      → test_k1_e4_apply_deny_nonempty_is_drift_exit3 → 行为级（真跑）
  · 写前时间戳备份（内容 = 原始 config）         → test_k1_e4_apply_timestamped_backup → 行为级（真跑）
  · 只改两叶键（展平叶差集恰 = {profile, allow}）、
    261017 skills 组合态 / 全局 tools / gateway.token
    零触碰                                       → test_k1_e4_apply_preserves_other_keys_and_composes_with_skills_patch → 行为级（真跑）
  · gen-patch 产物（恰两叶 / 值正确 / 零
    config 读写）                                → test_k1_e4_gen_patch_product_exactly_target_leaves_and_zero_config_write → 行为级（真跑）
  · 回滚 dry-run / 一步还原                     → test_k1_e4_rollback_dry_run_and_restore → 行为级（真跑）
  · openclaw.json 零入仓 / 声明零 secret        → test_k1_e4_no_openclaw_json_in_repo_and_no_secret_in_declaration → 机制级
  · manifest 三件登记 + note 陈旧字面换代        → test_k1_e4_manifest_registers_the_patch_workbench → 机制级
  · K1-E4-C 根因注释锚逐字在场                  → test_k1_e4_c_root_cause_anchor_pinned_in_this_file → 源锚代理（自锚）
  · K1-E4-C ① 声明含 profile="full"            → test_k1_e4_c_declaration_carries_tools_profile_full → 机制级
  · K1-E4-C ② profile 在场且 ≠ "full" → exit 3  → test_k1_e4_c_profile_not_full_is_drift_exit3 → 行为级（真跑）
  · K1-E4-C ③ allow 五项 + profile 缺席 → 可升级 → test_k1_e4_c_allow_five_with_profile_absent_is_upgradable → 行为级（真跑）
  · K1-E4-B 回合行语义 / 记忆节 / 既有锚兼容     → MemoryRecallRoundAcceptance 三条 → 源锚代理（prompt 措辞 = 交付物本身）

缺口（承载断言强度低于契约要求处，逐条）：

  · offline 只钉「脚本对临时 fixture 的行为」与「制品静态形状」；ECS 上真实
    openclaw.json 的 apply / 回滚 / `openclaw` 审计可见工具 = 5 项（含
    honcho_search_conclusions）= live 观测项（human 部署轮，本 Pod 不自行上
    ECS），offline 不证 live 生效。
  · gen-patch 产物「可被 `openclaw config patch --dry-run` 接受」= live 面
    （OpenClaw 自有 schema 校验，含 tools 子树未丢 deny），offline 只证 JSON
    子集形状与叶键集；真机写入口行为归部署轮命令单（--dry-run 校验后 --file 落）。
  · **LIVE 面零 host 断言（显式申报）**：openclaw.json 实应用（命令单）+
    真机问「王一诺是谁」答出 9/30 事实 + 日志出现 honcho_search_conclusions
    工具调用——三项均归 human 部署轮，本文件不落自动断言（marker 不设 live
    用例；host 面转绿后停门 awaiting-human:live-deploy）。
  · 声明 `_note` 是给人读的换代留痕：断言钉「语义锚组在场」（profile / 五项 /
    缺席接受 / 未知同级键 / 顺序敏感 / deny），不断言句式。
  · K1-E4-C 根因注释锚 = **本件自锚**（断言本文件源码含逐字锚句）——它防的是
    「后来者把教训注释删掉后按旧心智改回单键 allow」，不构成运行时行为证明；
    行为本体由 K1-E4-C ②③ 两条真跑承载。
  · 行为等价假设（显式化，不写即默认不成立）：
      - changes 的两条叶路径 ⇔ OpenClaw 运行时按 agent 级 tools.profile +
        tools.allow 收窄可见工具（**含插件工具**——live 实证：profile full 后
        honcho_search_conclusions 可达；运行时键形以部署轮工具列表观测为准；
        若实跑发现键形不符，属验收判法升级，走 tighten-only 窗口经 planner
        调整，不得静默改结构凑合）；
      - 「allow 列表顺序敏感相等」= 设计意图（fail-closed，顺序不同 = drift），
        不是缺陷；若部署轮实读 live 的 allow 顺序与声明不同，应经 planner
        复核声明基线，不得改脚本为顺序无关凑合；
      - 「叶路径下 tools 未知同级键被容忍」= **本包显式放宽**（旧对象形态
        「多键即拒」语义消失）——deny 用独立叶路径兜住非空红线，未知同级键
        本身不构成漂移；
      - 「检索式召回生效」以声明 + 提示词 + live 工具调用的组合为代理——
        OpenClaw 运行时是否真注入并执行该工具归部署轮。
  · fixture 自含性声明：全部断言直读 repo 内制品 + tempfile fixture，无外部
    资产 / 时间真值 / 环境前提依赖（PyYAML 仅 manifest 断言用，与
    verify-deploy.sh 同一解析器）。行为 seam（声明 / 提示词 → OpenClaw 运行时
    工具注入与回合行为）的驱动方 = human 部署轮，offline 侧无驱动方 = 已申报盲区。

Run protocol（文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/
  （standalone: python 本文件）

RED 基线（冻结时点树，2026-10-25，builder 尚未动工；实测取证 = 全量
`-m "not live"` acceptance：**360 收集 / 21 failed / 332 passed /
7 deselected / 33 subtests**；同树**冻结前**基线 = 353 收集 / 0 failed /
346 passed / 7 deselected / 33 subtests）：

  本件设计态红 15 条（全部指向本包未实现的行为，非环境 / 测试自身错误）：
    ① test_k1_e4_declaration_present_and_schema —— 声明仍是旧单键 change，
       无 changes 双叶键；
    ② test_k1_e4_allowlist_exactly_five —— 读不到 allow / profile 叶值；
    ③ test_k1_e4_declaration_note_records_drift_upgrade_scope —— _note 无
       「未知同级键被容忍」等换代留痕（放宽项未申报）；
    ④ test_k1_e4_drift_guard_upgrade_baselines —— drift_guard 只有一条
       tools 对象形态条目，无 deny / profile / allow 三叶路径；
    ⑤ test_k1_e4_apply_script_present_and_consumes_declaration —— 脚本零
       `changes` 字面（仍按单键 change 读）；
    ⑥ test_k1_e4_apply_applies_then_idempotent —— 读不到声明的 allow 叶值；
    ⑦ test_k1_e4_apply_accepts_legacy_four_baseline —— 同上；
    ⑧ test_k1_e4_apply_drift_guard_exit3 —— profile="coding" 态被旧幂等
       分支裸 SKIP（rc 0，应 3）：profile 不在写入面 → 在场非 full 被静默
       放过，正是本次教训的远端形态；
    ⑨ test_k1_e4_apply_deny_nonempty_is_drift_exit3 —— 两叶达标 + deny 非空
       当前裸 SKIP（rc 0，应 3）：幂等预检与 drift 未联合；
    ⑩ test_k1_e4_apply_preserves_other_keys_and_composes_with_skills_patch ——
       同上（目标态读不到）；
    ⑪ test_k1_e4_gen_patch_product_exactly_target_leaves_and_zero_config_write
       —— patch 产物只有 allow 一个叶键（应两个）；
    ⑫ test_k1_e4_manifest_registers_the_patch_workbench —— manifest 三条
       note 仍写「恰四项」/「只改 …tools.allow」；
    ⑬ test_k1_e4_c_declaration_carries_tools_profile_full —— 声明缺
       changes list（无 profile 叶）；
    ⑭ test_k1_e4_c_profile_not_full_is_drift_exit3 —— rc 0（裸 SKIP，应 3）；
    ⑮ test_k1_e4_c_allow_five_with_profile_absent_is_upgradable —— rc 2
       （脚本读不到 changes，应 0）。
  本件冻结即绿（must-stay-green，本包不得回退）：
    test_k1_e4_apply_skips_live_upgraded_state（live 升级态 SKIP 零写）、
    test_k1_e4_apply_timestamped_backup、test_k1_e4_rollback_dry_run_and_restore、
    test_k1_e4_no_openclaw_json_in_repo_and_no_secret_in_declaration、
    test_k1_e4_c_root_cause_anchor_pinned_in_this_file、K1-E4-B 三条。
  姊妹件 test_261022_memory_isolation.acceptance.py 同步有 6 条 K3-E6 设计态
  红（honcho 声明 expect 四项 → 五项未落 + manifest honcho note 字面），
  归本包同一批（TaskPackage exitCriteria K3-E6）。
  零回退：本件与姊妹件之外的既有断言（318 条）全绿。
"""

from __future__ import annotations

import json
import re
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

try:
    import yaml  # noqa: F401  (lazy import in helper; 环境前提缺失 = infra 红)
    _HAS_YAML = True
except ImportError:
    _HAS_YAML = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
DEPLOY_DIR = SERVER / "deploy"
TOOLS_DIR = DEPLOY_DIR / "261022-openclaw-kidpal-tools"
DECL_PATH = TOOLS_DIR / "kidpal-agent-tools-declaration.json"
APPLY_SCRIPT = TOOLS_DIR / "apply_agent_tools.py"
ROLLBACK_SCRIPT = TOOLS_DIR / "rollback_agent_tools.py"
MANIFEST_YAML = DEPLOY_DIR / "manifest.yaml"
AGENTS_MD = (DEPLOY_DIR / "openclaw-agents" / "workspace-kidpal" / "AGENTS.md")

# 2026-10-25 换代：写入面 = 恰两个叶键（tools.profile + tools.allow）。
ALLOW_PATH = "agents.entries.kidpal.tools.allow"
PROFILE_PATH = "agents.entries.kidpal.tools.profile"
DENY_PATH = "agents.entries.kidpal.tools.deny"
TOOLS_OBJECT_PATH = "agents.entries.kidpal.tools"
WRITE_LEAVES = (PROFILE_PATH, ALLOW_PATH)
TARGET_PROFILE = "full"

EXPECTED_TOOLS = ("read", "write", "exec", "web_search",
                  "honcho_search_conclusions")
# 上一代已应用态（live 旧态 = drift_guard 必须接受的「升级基准」之一）。
LEGACY_TOOLS_FOUR = ("read", "write", "exec", "web_search")
# 安全负锚：原始消息面工具明确不放行（advisory §B 选项 3；TaskPackage notes ⑤）。
FORBIDDEN_TOOL = "honcho_search_messages"
MANIFEST_DIR_TOKEN = "261022-openclaw-kidpal-tools"
TIMESTAMP_RE = re.compile(r"\d{8}T\d{6}")
# K1-E4-C 根因注释锚（逐字；禁删——本件自锚断言其在场）。
ROOT_CAUSE_ANCHOR = "过滤链 profile 先行，allow 只能在剩余集里挑——加名 ≠ 可达"

_MISSING = object()
_ABSENT = object()


# --------------------------------------------------------------------------- #
# 读盘 / 进程 helpers（仓根由 __file__ 显式推导，不依赖调用点 cwd）
# --------------------------------------------------------------------------- #

def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _load_json(path: Path):
    txt = _read_text(path)
    if not txt:
        return None, f"{path} 缺失或为空"
    try:
        return json.loads(txt), None
    except ValueError as e:  # noqa: BLE001
        return None, f"{path} 非法 JSON: {e!r}"


def _write_json(path: Path, obj) -> None:
    path.write_text(json.dumps(obj, ensure_ascii=False, indent=2), encoding="utf-8")


def _run_py(script: Path, args, timeout: int = 120):
    """用当前解释器跑 python 脚本（离线可跑，不经 bash/ssh）；脚本缺失 → rc 127。"""
    if not script.is_file():
        return 127, "", f"{script} 不在位（冻结契约工件缺失）"
    proc = subprocess.run(
        [sys.executable, str(script), *[str(a) for a in args]],
        capture_output=True, text=True, encoding="utf-8", errors="replace",
        timeout=timeout,
    )
    return proc.returncode, proc.stdout, proc.stderr


# --------------------------------------------------------------------------- #
# 点分路径读写（openclaw.json 形状）
# --------------------------------------------------------------------------- #

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
    """把 patch 产物 / config 状态（嵌套树）展平为 (点分叶路径, 标量值) 序列
    —— 展平语义一致，故嵌套树与扁平点分映射两形可同一判法（list 作叶子）。"""
    if isinstance(obj, dict):
        for k, v in obj.items():
            yield from _flatten_leaves(v, f"{prefix}.{k}" if prefix else str(k))
    else:
        yield prefix, obj


def _leaf_delta(pre, post) -> dict:
    """pre → post 的展平叶差集：{叶路径: (pre 值 | _ABSENT, post 值 | _ABSENT)}。"""
    a = dict(_flatten_leaves(pre))
    b = dict(_flatten_leaves(post))
    delta = {}
    for key in sorted(set(a) | set(b)):
        before, after = a.get(key, _ABSENT), b.get(key, _ABSENT)
        if before != after:
            delta[key] = (before, after)
    return delta


def _canon_alt(alt) -> str:
    """drift 候选的规范形（判「候选集恰 = 」用，候选内部顺序敏感）。"""
    if isinstance(alt, str):
        return f"str:{alt}"
    return json.dumps(alt, sort_keys=True, ensure_ascii=False)


# --------------------------------------------------------------------------- #
# 声明面 helpers（两类共用；失败信息带 acceptance tag）
# --------------------------------------------------------------------------- #

def _require_decl(tag: str = "K1-E4") -> dict:
    if not DECL_PATH.is_file():
        raise AssertionError(
            f"{tag} RED: 声明工件不在位 —— 冻结契约要求 {DECL_PATH}。"
            "openclaw.json 含 gateway.token 整文件不入仓，只入声明（K1-E4 铁律）。")
    decl, err = _load_json(DECL_PATH)
    if decl is None:
        raise AssertionError(f"{tag} RED: 声明工件不可解析 —— {err}。")
    if not isinstance(decl, dict):
        raise AssertionError(f"{tag} RED: 声明工件顶层必须是 JSON object。")
    return decl


def _require_backup_pattern(decl: dict, tag: str = "K1-E4") -> str:
    backup = decl.get("backup")
    pattern = backup.get("pattern") if isinstance(backup, dict) else None
    if not (isinstance(pattern, str) and pattern.strip()):
        raise AssertionError(
            f"{tag} RED: 声明缺 backup.pattern（前置）。")
    if "/" in pattern:
        raise AssertionError(
            f"{tag} RED: backup.pattern = {pattern!r} 含路径分隔符 —— 须为 config "
            "同目录下备份文件的相对 glob（<config basename>.bak-...-*）。")
    return pattern


def _declared_changes(decl: dict, tag: str = "K1-E4") -> list:
    changes = decl.get("changes")
    if not isinstance(changes, list):
        raise AssertionError(
            f"{tag} RED: 声明缺 changes list —— 冻结契约写入面 = 双叶键 "
            "changes [{path, value}]（2026-10-25 换代：单键 change 退役）。")
    return changes


def _declared_change(decl: dict, path: str):
    """按叶路径取 changes 条目；无 → None。"""
    changes = decl.get("changes")
    if not isinstance(changes, list):
        return None
    for change in changes:
        if isinstance(change, dict) and change.get("path") == path:
            return change
    return None


def _declared_allow(decl: dict, tag: str = "K1-E4") -> list:
    change = _declared_change(decl, ALLOW_PATH)
    if change is None:
        raise AssertionError(
            f"{tag} RED: changes 缺 {ALLOW_PATH!r} 叶键 —— 目标白名单无写入面。")
    allow = change.get("value")
    if not isinstance(allow, list):
        raise AssertionError(
            f"{tag} RED: {ALLOW_PATH} 的 value 不是 list（目标白名单）。")
    return allow


def _declared_profile(decl: dict, tag: str = "K1-E4") -> str:
    change = _declared_change(decl, PROFILE_PATH)
    if change is None:
        raise AssertionError(
            f"{tag} RED: changes 缺 {PROFILE_PATH!r} 叶键 —— 本修复的核心写入面"
            "（per-agent profile 覆盖全局 coding）缺失；加名 ≠ 可达，"
            f"{ROOT_CAUSE_ANCHOR}。")
    return change.get("value")


# --------------------------------------------------------------------------- #
# drift_guard 候选展开（expect / expect_any；按点分 path 分桶）
# --------------------------------------------------------------------------- #

def _drift_alts_by_path(decl: dict) -> dict:
    """返回 {点分 path: [候选...]}；expect 单项 + expect_any 展开。"""
    out: dict = {}
    dg = decl.get("drift_guard")
    if not isinstance(dg, list):
        return out
    for entry in dg:
        if not isinstance(entry, dict):
            continue
        path = entry.get("path")
        if not isinstance(path, str) or not path.strip():
            continue
        bucket = out.setdefault(path, [])
        if "expect" in entry:
            bucket.append(entry.get("expect"))
        ea = entry.get("expect_any")
        if isinstance(ea, list):
            bucket.extend(ea)
    return out


# --------------------------------------------------------------------------- #
# fixture（组合态：261017 skills 白名单已应用 + kidpal 无 tools 覆写 = 新机基线）
# --------------------------------------------------------------------------- #

def _fixture_pre() -> dict:
    """变更前现状（fresh 机基线）：kidpal agent 无 tools 覆写（三叶路径全缺席）；
    全局 tools.profile="coding" 保留 = 本次根因现场（profile 第一道过滤宽不出来）；
    skills 白名单 = 261017 已应用组合态（声明式 patch 组合共存是设计前提）。"""
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
                },
            },
        },
        "modelPolicy": {"allow": ["openclaw/kidpal"]},
    }


def _fixture_legacy_four() -> dict:
    """升级基准②：上一代四项覆写已应用（P0 批之前的 live 形态）——可升级。"""
    state = _fixture_pre()
    _json_set(state, ALLOW_PATH, list(LEGACY_TOOLS_FOUR))
    return state


def _fixture_p0_intermediate() -> dict:
    """升级基准③（本包最典型起点）：P0 已应用态 = allow 五项 + profile **缺席**
    —— 必须可升级（apply 补 profile="full"），不得拒绝自身升级起点。"""
    state = _fixture_pre()
    _json_set(state, ALLOW_PATH, list(EXPECTED_TOOLS))
    return state


def _fixture_upgraded_live() -> dict:
    """升级基准④ = 本包目标态已上 live：profile full + allow 五项 + deny [] ——
    apply 必须 SKIP（零写零备份）。"""
    state = _fixture_pre()
    _json_set(state, TOOLS_OBJECT_PATH, {
        "profile": TARGET_PROFILE,
        "allow": list(EXPECTED_TOOLS),
        "deny": [],
    })
    return state


def _fixture_mismatch() -> dict:
    """真漂移①：tools.allow 已被设成别的白名单（异值）。"""
    state = _fixture_pre()
    _json_set(state, ALLOW_PATH, ["bash", "browser"])
    return state


def _fixture_six_items() -> dict:
    """真漂移②：tools.allow 已被扩到六项（含原始消息面工具）——基线集以外。"""
    state = _fixture_pre()
    _json_set(state, ALLOW_PATH,
              list(LEGACY_TOOLS_FOUR) + [EXPECTED_TOOLS[-1], FORBIDDEN_TOOL])
    return state


def _fixture_allow_short() -> dict:
    """真漂移③：allow 项数不符（三项）——既非 unset、也非上一代四项、也非五项。"""
    state = _fixture_pre()
    _json_set(state, ALLOW_PATH, ["read", "write", "exec"])
    return state


def _fixture_profile_not_full() -> dict:
    """真漂移④：profile 在场且值 ≠ "full"（"coding"）——K1-E4-C ② 的机械承载：
    profile 在场就按值判，不是「在场即过」。"""
    state = _fixture_pre()
    _json_set(state, TOOLS_OBJECT_PATH, {
        "profile": "coding",
        "allow": list(EXPECTED_TOOLS),
    })
    return state


def _fixture_deny_nonempty() -> dict:
    """真漂移⑤（幂等预检与 drift 联合的关键场景）：两叶键已达目标态，但
    tools.deny 非空 —— 不得裸跑 target==config 就 SKIP，须 exit 3。"""
    state = _fixture_pre()
    _json_set(state, TOOLS_OBJECT_PATH, {
        "profile": TARGET_PROFILE,
        "allow": list(EXPECTED_TOOLS),
        "deny": ["bash"],
    })
    return state


def _fixture_malformed_tools() -> dict:
    """真漂移⑥：tools 键在场但不是对象（结构畸形）。键「在场且形状不对」≠键
    缺席——按 unset 放行等于静默覆盖畸形配置，须 fail-closed 判漂移。"""
    state = _fixture_pre()
    _json_set(state, TOOLS_OBJECT_PATH, "disabled")
    return state


def _target_state(pre: dict, profile, allow) -> dict:
    state = json.loads(json.dumps(pre))
    _json_set(state, PROFILE_PATH, profile)
    _json_set(state, ALLOW_PATH, list(allow))
    return state


def _backups(td: Path, pattern: str) -> list[Path]:
    return sorted(td.glob(pattern))


# --------------------------------------------------------------------------- #
# AGENTS.md 锚组（K1-E4-B；自含副本，跨冻结件 import 会把冻结件焊死）
# --------------------------------------------------------------------------- #

ROUTING_HEADING_NEEDLE = "回合类型与纪律路由"
MEMORY_ROUND_NAME = "记忆回忆回合"
MEMORY_SECTION_NEEDLE = "记忆"

# 「记忆回忆回合」行须齐的语义锚组（每组至少一词；措辞换写可、语义删除不可）。
MEMORY_ROUND_ANCHORS = {
    "唯一放行工具字面 honcho_search_conclusions": (
        "honcho_search_conclusions",),
    "识别特征（记不记得 / 我以前说过 / 是谁）": (
        "记不记得", "还记得", "我以前说过", "以前说过", "说过的事", "是谁"),
    "先查一次": (
        "先调", "先查", "查一次", "调用一次", "先检索", "检索一次", "先搜"),
    "查到后自然转述": (
        "自然", "转述", "用自己的话说", "像聊天", "口语"),
    "不逐字复述": (
        "不逐字", "不要逐字", "不得逐字", "不复述", "不照搬", "不原样"),
    "不带内部流程": (
        "内部流程", "不带流程", "不暴露", "不叙述", "不提工具", "不说工具"),
    "家长隐私不回忆": ("家长隐私",),
    "查不到才说记不清": ("记不清",),
    "过渡语播种": (
        "我去翻", "翻翻", "过渡语", "先播", "先说一句"),
    "与孩子对话回合禁工具的关系显式写清": (
        "第二个", "第二处", "第二类", "唯二", "除课表问答",
        "不属于孩子对话回合", "不走孩子对话回合", "不按孩子对话回合"),
}

# K1-E1 回合路由五行（additive 红线：新行不得挤掉既有行；孩子对话回合行
# 禁工具锚组零回退）。
K1E1_ROUND_NAMES = (
    "孩子对话回合", "实时事实回合", "查字回合", "课表问答回合", "提醒 cron 回合")
K1E1_CHILD_ROUND_TOOL_BAN = (
    "不调用任何工具", "禁工具", "禁用工具", "不读取任何文件", "禁读文件")

# K3_P1_AGENTS_ANCHORS 逐字副本（源 = server/acceptance/test_261022_honcho_memory
# .acceptance.py:233-240 的六串；复制不引用，理由同上）。
K3_P1_AGENTS_ANCHORS_COMPAT = (
    "## 静态语义单一事实源",
    "以本文件为准",
    "## 底线（超越一切，任何时候都有效）",
    "查字回合绝对禁工具",
    "回合类型与纪律路由",
    "不引用、不复述任何家长隐私（姓名、联系方式、家庭安排细节）",
)

# 261023 A3 记忆节正锚四串（逐字副本）。
A3_MEMORY_POSITIVES_COMPAT = (
    "不需要写任何文件", "不调用任何工具", "记不清", "绝不编造")
A3_OBJECT_LOCK_KEY = "对象锁定"
# 261023 A3 对象锁定行 S3 澄清两半（逐字副本）。
A3_S3_CLARIFIES = ("孩子自己的日程信息", "不算复述家长隐私")

# 记忆节检索澄清句（把「记」与「查旧事」分开）。
MEMORY_RETRIEVAL_CLARIFY = (
    "检索", "查一下记忆", "翻一翻", "翻我", "honcho_search_conclusions")


def _section_span(text: str, heading_needle: str):
    """取「标题含 heading_needle 的 `## ` 节」窗口：自该标题行起、至下一个
    `## ` 标题行或 EOF。节不存在 → None（调用方判设计态红）。"""
    lines = text.splitlines()
    start = next((i for i, l in enumerate(lines)
                  if l.startswith("## ") and heading_needle in l), None)
    if start is None:
        return None
    rest = lines[start + 1:]
    end = next((j for j, l in enumerate(rest) if l.startswith("## ")), len(rest))
    return "\n".join([lines[start]] + rest[:end])


# --------------------------------------------------------------------------- #
# 声明 _note 换代留痕锚组（源锚代理：措辞可换、口径不可删）
# --------------------------------------------------------------------------- #

NOTE_ANCHOR_GROUPS = {
    "profile 键面": ("tools.profile", "profile"),
    "五项 / 检索工具口径": ("五项", "honcho_search_conclusions"),
    "profile 缺席 = 接受（升级起点）": ("缺席", "缺省", "未设"),
    "未知同级键被容忍（放宽项申报）": (
        "同级键", "未知键", "多余键", "其它键", "未知"),
    "allow 顺序敏感（放宽项申报）": ("顺序",),
    "deny 非空判 drift": ("deny", "拒绝"),
}


class OpenclawKidpalToolsAcceptance(unittest.TestCase):
    """261022-m5-k1-prompt-carrier-migration K1-E4（offline；2026-10-25 换代至
    双叶写入面 tools.profile="full" + tools.allow 恰五项、三叶 drift 升级基准、
    联合幂等预检、gen-patch 两叶）。

    live-manual 观测项（**非自动断言**，structural 申报，本类不落测试）：
      human 部署轮在 ECS 核验——`openclaw` 工具面 audit 可见工具 = read / write /
      exec / web_search / honcho_search_conclusions 恰五项、systemPromptReport
      工具集含 honcho_search_conclusions（live 已实证，仓内固化为本件）；
      `openclaw config patch --dry-run --json` 接受 gen-patch 产物（核 tools
      子树未丢 deny）后 `--file` 落盘；本包 apply 在真实 openclaw.json 上的
      升级回执与 261017 skills patch 组合共存；真机问「王一诺是谁」答出 9/30
      事实。非确定性来源：运行时键形、工具注入与回合行为都是部署后观测量，
      offline 不可机判（行为等价假设见头注缺口节）。host 面转绿后停门等 human
      （awaiting-human:live-deploy），Pod 不自行上 ECS。"""

    maxDiff = None

    def _decl(self) -> dict:
        return _require_decl("K1-E4")

    def _backup_pattern(self, decl: dict) -> str:
        return _require_backup_pattern(decl, "K1-E4")

    def _declared_allow(self, decl: dict) -> list:
        return _declared_allow(decl, "K1-E4")

    def _declared_profile(self, decl: dict) -> str:
        return _declared_profile(decl, "K1-E4")

    # ------------------------------------------------------------------ #
    # K1-E4 · 声明工件在位 / schema / 双叶写入面（机制级）
    # ------------------------------------------------------------------ #

    def test_k1_e4_declaration_present_and_schema(self):
        """coverage: K1-E4 — 声明工件在位 + JSON 可解析 + 五顶层键齐（case /
        changes / drift_guard / backup / rollback）+ 旧单键 change 退役 +
        changes 恰两叶键 {path, value} + drift_guard 每项 path 齐且恰含 expect
        或 expect_any 之一。"""
        decl = self._decl()
        for key in ("case", "changes", "drift_guard", "backup", "rollback"):
            self.assertIn(
                key, decl,
                f"K1-E4 RED: 声明工件缺必需顶层键 {key!r} —— 冻结契约结构 = "
                "case / changes / drift_guard / backup / rollback（2026-10-25 "
                "换代：单键 change → 双叶键 changes）。")
        self.assertIsInstance(decl.get("case"), str, "K1-E4 RED: case 必须是字符串。")
        self.assertTrue(str(decl.get("case", "")).strip(), "K1-E4 RED: case 不得为空。")

        self.assertNotIn(
            "change", decl,
            "K1-E4 RED: 声明仍带旧单键 change —— 写入面已换代到 changes "
            "[{path, value}]（两个叶键），旧键残留会让消费面二义（显式换代，"
            "禁新旧并存）。")

        changes = decl.get("changes")
        self.assertIsInstance(changes, list, "K1-E4 RED: changes 必须是 list。")
        self.assertEqual(
            len(changes), 2,
            f"K1-E4 RED: changes 条数 = {len(changes)}，须恰 2 —— 写入面 = "
            "agents.entries.kidpal.tools.profile + "
            "agents.entries.kidpal.tools.allow（deny 不进写入面：live 升级态 "
            "deny:[] 由幂等分支天然覆盖；deny 非空 = 基线集以外 → 判 drift）。")
        paths = []
        for i, change in enumerate(changes):
            self.assertIsInstance(
                change, dict, f"K1-E4 RED: changes[{i}] 必须是 object。")
            self.assertTrue(
                isinstance(change.get("path"), str) and change["path"].strip(),
                f"K1-E4 RED: changes[{i}].path 必须是非空字符串（点分叶路径）。")
            self.assertIn(
                "value", change,
                f"K1-E4 RED: changes[{i}] 缺 value —— 冻结形状 = {{path, value}}"
                "（value 深拷贝后写入）。")
            paths.append(change["path"])
        self.assertEqual(
            sorted(paths), sorted(WRITE_LEAVES),
            f"K1-E4 RED: changes 的 path 集合 = {sorted(paths)}，须恰 "
            f"{sorted(WRITE_LEAVES)}（双叶键；多一个少一个都不行）。")
        self.assertEqual(
            len(set(paths)), len(paths),
            f"K1-E4 RED: changes 存在重复 path（{paths}）—— 逐叶单写。")

        dg = decl.get("drift_guard")
        self.assertIsInstance(dg, list, "K1-E4 RED: drift_guard 必须是 list。")
        self.assertTrue(dg, "K1-E4 RED: drift_guard 不得为空。")
        for i, entry in enumerate(dg):
            self.assertIsInstance(entry, dict,
                                  f"K1-E4 RED: drift_guard[{i}] 必须是 object。")
            self.assertTrue(
                isinstance(entry.get("path"), str) and entry.get("path", "").strip(),
                f"K1-E4 RED: drift_guard[{i}].path 必须是非空字符串。")
            has_expect = "expect" in entry
            has_any = "expect_any" in entry
            self.assertTrue(
                has_expect or has_any,
                f"K1-E4 RED: drift_guard[{i}] 既无 expect 也无 expect_any —— "
                "基线判定面缺失（换代契约：expect | expect_any 恰含其一）。")
            self.assertFalse(
                has_expect and has_any,
                f"K1-E4 RED: drift_guard[{i}] 同时含 expect 与 expect_any —— "
                "两形互斥，语义不明（换代契约：恰含其一）。")
            if has_expect:
                self.assertTrue(
                    isinstance(entry.get("expect"), str)
                    and entry.get("expect", "").strip(),
                    f"K1-E4 RED: drift_guard[{i}].expect 必须是非空字符串。")
            if has_any:
                ea = entry.get("expect_any")
                self.assertIsInstance(
                    ea, list,
                    f"K1-E4 RED: drift_guard[{i}].expect_any 必须是 list。")
                self.assertTrue(
                    ea,
                    f"K1-E4 RED: drift_guard[{i}].expect_any 不得为空（基线集）。")

        backup = decl.get("backup")
        self.assertIsInstance(backup, dict, "K1-E4 RED: backup 必须是 object。")
        self.assertTrue(
            isinstance(backup.get("dir"), str) and backup.get("dir", "").strip(),
            "K1-E4 RED: backup.dir 必须是非空字符串（ECS 备份目录）。")
        self.assertTrue(
            isinstance(backup.get("pattern"), str) and "bak" in backup.get("pattern", ""),
            "K1-E4 RED: backup.pattern 必须是含 'bak' 的字符串（config 同目录 glob）。")

        rollback = decl.get("rollback")
        self.assertIsInstance(rollback, dict, "K1-E4 RED: rollback 必须是 object。")
        script = rollback.get("script")
        self.assertTrue(
            isinstance(script, str) and script.strip(),
            "K1-E4 RED: rollback.script 必须是非空字符串。")
        self.assertTrue(
            (TOOLS_DIR / script).is_file(),
            f"K1-E4 RED: rollback.script 指向的 {script!r} 不在 {TOOLS_DIR} —— "
            "声明面与回滚面不一致（回滚不可跑）。")

    def test_k1_e4_allowlist_exactly_five(self):
        """coverage: K1-E4 — 写入面恰两叶：tools.profile = "full" +
        tools.allow 恰五项、无重复、集合 = read / write / exec / web_search /
        honcho_search_conclusions（顺序不作语义）；原始消息面工具
        honcho_search_messages 不得放行；deny 不得进写入面。"""
        decl = self._decl()
        allow = self._declared_allow(decl)
        self.assertEqual(
            len(allow), 5,
            f"K1-E4 RED: 白名单项数 = {len(allow)}（{allow}），须恰五项 —— "
            "四项 + honcho_search_conclusions（历史事实检索面；architect "
            "advisory §B 选项 3）。")
        self.assertEqual(
            len(set(allow)), 5,
            f"K1-E4 RED: 白名单有重复项（{allow}）—— 重复项不构成额外能力。")
        self.assertEqual(
            sorted(allow), sorted(EXPECTED_TOOLS),
            f"K1-E4 RED: 白名单 {sorted(allow)} ≠ 契约五项 {sorted(EXPECTED_TOOLS)} "
            "—— 噪音未收敛干净、漏放检索工具或误收多余能力。")
        self.assertNotIn(
            FORBIDDEN_TOOL, allow,
            f"K1-E4 RED: 白名单含 {FORBIDDEN_TOOL!r} —— 原始消息面（家长代述 / "
            "流程内容）明确不放行（advisory §B 选项 3；TaskPackage notes ⑤）。")

        self.assertEqual(
            self._declared_profile(decl), TARGET_PROFILE,
            "K1-E4 RED: 声明的 tools.profile 叶值不是 \"full\" —— 全局 "
            "tools.profile=\"coding\" 第一道即滤掉插件工具，allow 加不回"
            f"（{ROOT_CAUSE_ANCHOR}）。")
        self.assertIsNone(
            _declared_change(decl, DENY_PATH),
            "K1-E4 RED: changes 含 tools.deny 写入面 —— deny 不进写入面"
            "（live 升级态 deny: [] 由幂等分支天然覆盖；deny 非空 = 基线集"
            "以外 → fail-closed 判 drift，不得由 apply 覆写）。")

    def test_k1_e4_declaration_note_records_drift_upgrade_scope(self):
        """coverage: K1-E4 — 声明 _note 换代留痕（源锚代理，语义锚组在场即可）：
        profile 键面 / 五项口径 / profile 缺席 = 接受 / 未知同级键被容忍 /
        allow 顺序敏感 / deny 非空判 drift（后两条 = 本包两条放宽项的显式申报，
        TaskPackage exitCriteria 要求「声明 _note 与验收件头注留痕」）。"""
        decl = self._decl()
        note = decl.get("_note")
        self.assertTrue(
            isinstance(note, str) and note.strip(),
            "K1-E4 RED: 声明缺 _note 或为空 —— 换代留痕（写入面 / drift 基准 / "
            "放宽项）无承载面。")
        for what, group in NOTE_ANCHOR_GROUPS.items():
            self.assertTrue(
                any(w in note for w in group),
                f"K1-E4 RED: 声明 _note 缺留痕锚组「{what}」（任一即可：{group}）"
                "—— 换代口径被删或稀释（builder 须把 V2 口径写进 _note，"
                "不只写进代码）。")

    def test_k1_e4_drift_guard_upgrade_baselines(self):
        """coverage: K1-E4 — drift_guard = 三条叶路径 AND，候选集恰为：
        deny {unset, []} / profile {unset, "full"} / allow {unset, 上一代四项,
        五项（= changes 里 allow 叶值逐字、顺序同）}。等价断言 = 接受 ① fresh
        （全缺席）② 旧四项 live ③ P0 中间态（allow 五项 + profile 缺席，
        apply 补 full）④ live 升级态；profile 在场且 ≠ "full" 判 drift。"""
        decl = self._decl()
        allow = self._declared_allow(decl)
        alts = _drift_alts_by_path(decl)
        self.assertTrue(
            alts,
            "K1-E4 RED: drift_guard 未产出任何可判基线（expect / expect_any "
            "均缺）—— apply 无法判漂移。")
        expected = {
            DENY_PATH: {"str:unset", _canon_alt([])},
            PROFILE_PATH: {"str:unset", _canon_alt(TARGET_PROFILE)},
            ALLOW_PATH: {"str:unset",
                         _canon_alt(list(LEGACY_TOOLS_FOUR)),
                         _canon_alt(list(allow))},
        }
        for path, want in expected.items():
            self.assertIn(
                path, alts,
                f"K1-E4 RED: drift_guard 无锚定 {path!r} 的条目 —— 三叶路径 "
                f"AND 判定面缺一条（实得 path 集：{sorted(alts)}）。"
                "缺 deny 叶 = deny 非空被静默放行；缺 profile 叶 = "
                "profile=\"coding\" 被放行后覆写；缺 allow 叶 = allow 任意 "
                "白名单被当可升级态。")
            got = {_canon_alt(a) for a in alts[path]}
            self.assertEqual(
                got, want,
                f"K1-E4 RED: {path} 的基线候选集 = {sorted(got)}，须恰 "
                f"{sorted(want)}。\n"
                "  · 须含 'unset'（键缺席 = fresh 起点）；\n"
                "  · profile 另须含 \"full\"（live 升级态）；**缺席即接受**，"
                "只有在场且 ≠ \"full\" 才 drift（纠错 ①：拒绝「allow 五项 + "
                "profile 缺席」等于拒绝本包自己的升级起点）；\n"
                "  · allow 另须含上一代四项与五项（五项须与 changes 叶值"
                "**逐字同序**——allow 比较是列表顺序敏感相等，顺序不一致则 "
                "live 升级态永远判 drift）；\n"
                "  · deny 另须含空列表 []（deny 非空 = 基线集以外 → drift）。")

    def test_k1_e4_apply_script_present_and_consumes_declaration(self):
        """coverage: K1-E4 — apply 脚本在位、消费声明工件名、用 stdlib json
        （不依赖 jq）、暴露 --gen-patch 形态、且按 **changes 双叶键**口径消费
        （换代：不得残留「只改 tools.allow」单键口径；头注 / 报错文案同步）。"""
        self.assertTrue(
            APPLY_SCRIPT.is_file(),
            f"K1-E4 RED: apply 脚本不在位 —— 冻结契约要求 {APPLY_SCRIPT}。")
        src = _read_text(APPLY_SCRIPT)
        self.assertIn(
            "kidpal-agent-tools-declaration.json", src,
            "K1-E4 RED: apply 脚本未引用声明工件名 —— 声明面与消费面不一致。")
        self.assertIsNotNone(
            re.search(r"(?m)^\s*(?:import\b[^\n]*\bjson\b|from\s+json\s+import\b)", src),
            "K1-E4 RED: apply 脚本未用 stdlib json（冻结契约：零额外依赖、不依赖 jq）。")
        self.assertIn(
            "--gen-patch", src,
            "K1-E4 RED: apply 脚本未暴露 --gen-patch 形态 —— live 应用须走 "
            "OpenClaw 自有写入口（config patch），零直写路径保留（261023 批 B "
            "先例；TaskPackage notes ⑤）。")
        self.assertIsNotNone(
            re.search(r"\bchanges\b", src),
            "K1-E4 RED: apply 脚本未消费 changes 双叶键 —— 声明写入面已换代"
            "（单键 change 退役），脚本仍按单键读会漏写 tools.profile，"
            f"插件工具不可达的缺口复发（{ROOT_CAUSE_ANCHOR}）。")

    # ------------------------------------------------------------------ #
    # K1-E4 · apply 真跑（幂等 / 双基线 / drift / 备份 / 只改两叶）（行为级）
    # ------------------------------------------------------------------ #

    def test_k1_e4_apply_applies_then_idempotent(self):
        """coverage: K1-E4 — apply 真跑①：fresh 基线（三叶全缺席）→ 声明目标态
        （profile="full" + allow 五项）；②：再跑 → 幂等 no-op（同值、零新备份、
        rc 0）。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        allow = self._declared_allow(decl)
        profile = self._declared_profile(decl)
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            pre = _fixture_pre()
            _write_json(cfg, pre)
            want = _target_state(pre, profile, allow)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(
                rc, 0,
                f"K1-E4 RED: apply 首次运行 rc={rc}（应 0）。stdout={out!r} stderr={err!r}")
            got, gerr = _load_json(cfg)
            self.assertIsNone(gerr, f"K1-E4 RED: apply 后 config 不可解析: {gerr}")
            self.assertEqual(
                got, want,
                "K1-E4 RED: apply 未产生声明目标态（kidpal tools.profile=\"full\" "
                "+ tools.allow = 契约五项）。")
            self.assertEqual(
                _json_get(got, PROFILE_PATH), TARGET_PROFILE,
                "K1-E4 RED: apply 后 tools.profile ≠ \"full\" —— per-agent "
                f"profile 未落库（{ROOT_CAUSE_ANCHOR}）。")
            self.assertEqual(
                _json_get(got, ALLOW_PATH), list(allow),
                "K1-E4 RED: apply 后 tools.allow ≠ 契约五项。")
            backups_after_first = _backups(td, pattern)
            self.assertTrue(
                backups_after_first,
                "K1-E4 RED: 首次 apply 未建备份（前置，供下方幂等对比）。")

            rc2, out2, err2 = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            got2, _ = _load_json(cfg)
            self.assertEqual(
                rc2, 0,
                f"K1-E4 RED: apply 二次运行 rc={rc2}（幂等应 0）。"
                f"stdout={out2!r} stderr={err2!r}")
            self.assertEqual(
                got2, want, "K1-E4 RED: apply 二次运行改动了已达标态（非幂等）。")
            self.assertEqual(
                len(_backups(td, pattern)), len(backups_after_first),
                "K1-E4 RED: apply 二次运行又新建了备份（幂等应不重备份）。")

    def test_k1_e4_apply_skips_live_upgraded_state(self):
        """coverage: K1-E4 — apply 真跑：live 升级态（profile full + allow 五项
        + deny: []）→ 幂等预检命中 SKIP（rc 0），config **逐字节**不变、零备份。
        这是 live 上跑 apply 的常态路径（部署轮期望 SKIP，不是 exit 3）。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            pre = _fixture_upgraded_live()
            _write_json(cfg, pre)
            before = cfg.read_bytes()

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(
                rc, 0,
                f"K1-E4 RED: live 升级态上 apply rc={rc}（应 0 = SKIP）。"
                f"stdout={out!r} stderr={err!r}")
            self.assertIn(
                "SKIP", out,
                f"K1-E4 RED: live 升级态未打印 SKIP（stdout={out!r}）—— "
                "幂等预检未命中（live 就是本包目标态，apply 须 SKIP 零写零备份）。")
            self.assertEqual(
                cfg.read_bytes(), before,
                "K1-E4 RED: SKIP 路径改写了 config（须逐字节不变）。")
            self.assertEqual(
                _backups(td, pattern), [],
                "K1-E4 RED: SKIP 路径建了备份（幂等应零副作用）。")

    def test_k1_e4_apply_accepts_legacy_four_baseline(self):
        """coverage: K1-E4 — apply 真跑：旧四项覆写基线（上一代 live）→ 可升级到
        「profile full + 五项」（rc 0），备份内容 = 四项变更前态。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        allow = self._declared_allow(decl)
        profile = self._declared_profile(decl)
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            pre = _fixture_legacy_four()
            _write_json(cfg, pre)
            want = _target_state(pre, profile, allow)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(
                rc, 0,
                f"K1-E4 RED: 旧四项覆写基线上升级 rc={rc}（应 0）—— "
                "drift_guard 升级基准未落地（四项目标态被自己的基线拦死）。"
                f"stdout={out!r} stderr={err!r}")
            got, gerr = _load_json(cfg)
            self.assertIsNone(gerr, f"K1-E4 RED: apply 后 config 不可解析: {gerr}")
            self.assertEqual(
                got, want,
                "K1-E4 RED: 四项基线升级后 ≠ 两叶目标态（其它键须原样保留）。")
            backups = _backups(td, pattern)
            self.assertTrue(backups, "K1-E4 RED: 升级未建备份（前置）。")
            saved, _ = _load_json(backups[0])
            self.assertEqual(
                saved, pre,
                "K1-E4 RED: 备份内容 ≠ 四项变更前态（备份不及时/被覆盖）。")

    def test_k1_e4_apply_drift_guard_exit3(self):
        """coverage: K1-E4 — 真漂移拒绝（五态，exit 3 零写零备份）：异值白名单
        （bash/browser）、六项态（含原始消息面工具）、allow 项数不符（三项）、
        tools 键结构畸形（在场非对象 ≠ 键缺席）、profile 在场且 ≠ "full"。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        for label, drifted in (
                ("异值白名单", _fixture_mismatch()),
                ("六项态", _fixture_six_items()),
                ("allow 项数不符（三项）", _fixture_allow_short()),
                ("tools 结构畸形", _fixture_malformed_tools()),
                ("profile 在场且非 full", _fixture_profile_not_full())):
            with tempfile.TemporaryDirectory() as td:
                td = Path(td)
                cfg = td / "openclaw.json"
                _write_json(cfg, drifted)

                rc, out, err = _run_py(APPLY_SCRIPT,
                                       ["--config", cfg, "--decl", DECL_PATH])
                self.assertEqual(
                    rc, 3,
                    f"K1-E4 RED: {label} 应判 drift → exit 3（实得 rc={rc}）—— "
                    "基线集以外的状态不得被当作可升级态（fail-closed）。"
                    f"stdout={out!r} stderr={err!r}")
                after, _ = _load_json(cfg)
                self.assertEqual(
                    after, drifted,
                    f"K1-E4 RED: {label} 被拒后仍改写了 config。")
                self.assertEqual(
                    _backups(td, pattern), [],
                    f"K1-E4 RED: {label} 被拒后仍建了备份（拒绝 = 完全 no-op）。")

    def test_k1_e4_apply_deny_nonempty_is_drift_exit3(self):
        """coverage: K1-E4 — 幂等预检与 drift 联合（定向收紧）：两叶键已达目标
        态（profile full + allow 五项）但 tools.deny 非空 → **不得裸 SKIP**，
        须 exit 3、零写零备份。否则「target == config」会绕过 deny 红线
        （与姊妹 honcho 脚本「SKIP 先于 drift」有意不同）。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            pre = _fixture_deny_nonempty()
            _write_json(cfg, pre)
            before = cfg.read_bytes()

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(
                rc, 3,
                f"K1-E4 RED: 两叶达标但 deny 非空时 apply rc={rc}（应 3）—— "
                "幂等预检不得先于 drift 基线判定裸跑 SKIP（deny 非空 = 基线集"
                f"以外）。stdout={out!r} stderr={err!r}")
            self.assertNotIn(
                "SKIP", out,
                f"K1-E4 RED: deny 非空场景打印了 SKIP（stdout={out!r}）—— "
                "幂等预检与 drift 未联合。")
            self.assertEqual(
                cfg.read_bytes(), before,
                "K1-E4 RED: deny 非空被拒后仍改写了 config。")
            self.assertEqual(
                _backups(td, pattern), [],
                "K1-E4 RED: deny 非空被拒后仍建了备份（拒绝 = 完全 no-op）。")

    def test_k1_e4_apply_timestamped_backup(self):
        """coverage: K1-E4 — 写前建时间戳备份（落 config 同目录、匹配声明 pattern、
        文件名含 yyyymmddTHHMMSS、内容 = 原始 config）。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        pre = _fixture_pre()
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            _write_json(cfg, pre)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(rc, 0, f"K1-E4 RED: apply rc={rc}。stderr={err!r}")
            backups = _backups(td, pattern)
            self.assertTrue(
                backups,
                "K1-E4 RED: apply 未建匹配 backup.pattern 的备份 —— openclaw.json "
                "定点 patch 回滚先决缺失。")
            self.assertRegex(
                backups[0].name, TIMESTAMP_RE.pattern,
                f"K1-E4 RED: 备份名 {backups[0].name!r} 不含 UTC 时间戳"
                "（yyyymmddTHHMMSS）—— 非时间戳备份无法确定先后。")
            saved, _ = _load_json(backups[0])
            self.assertEqual(
                saved, pre, "K1-E4 RED: 备份内容 ≠ 变更前 config（备份不及时/被覆盖）。")

    def test_k1_e4_apply_preserves_other_keys_and_composes_with_skills_patch(self):
        """coverage: K1-E4 — apply 只改声明两叶键：gateway.token、261017 skills
        白名单组合态、其它 agent、全局 tools 块（profile="coding"）、modelPolicy
        原样保留；且 pre → 终态的**展平叶差集恰 = {tools.profile: "full",
        tools.allow: 五项}**（「只改一键」断言随双叶写入面换代）。"""
        decl = self._decl()
        allow = self._declared_allow(decl)
        profile = self._declared_profile(decl)
        self.assertTrue(APPLY_SCRIPT.is_file(), "K1-E4 RED: apply 脚本不在位（前置）。")
        pre = _fixture_legacy_four()
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            _write_json(cfg, pre)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(rc, 0, f"K1-E4 RED: apply rc={rc}。stderr={err!r}")
            got, gerr = _load_json(cfg)
            self.assertIsNone(gerr, f"K1-E4 RED: apply 后 config 不可解析: {gerr}")
            self.assertEqual(
                _json_get(got, "gateway.token"), _json_get(pre, "gateway.token"),
                "K1-E4 RED: apply 改动了 gateway.token —— 定点 patch 越界触碰 secret"
                "（冻结契约：只改 agents.entries.kidpal.tools.{profile,allow}）。")
            self.assertEqual(
                _json_get(got, "agents.entries.kidpal.skills"),
                _json_get(pre, "agents.entries.kidpal.skills"),
                "K1-E4 RED: apply 抹掉了 261017 skills 白名单组合态 —— 声明式 patch "
                "组合共存被破坏（plan.md 跨包事实：本包与 261017/P3 作用于同一文件，"
                "均不得整文件改写）。")
            self.assertEqual(
                _json_get(got, "agents.entries.main"), _json_get(pre, "agents.entries.main"),
                "K1-E4 RED: apply 改动了其它 agent 条目 —— 定点 patch 越界。")
            self.assertEqual(
                _json_get(got, "tools"), _json_get(pre, "tools"),
                "K1-E4 RED: apply 改动了**全局** tools 块 —— 定点 patch 越界"
                "（per-agent profile 只写在 agents.entries.kidpal.tools 下；全局 "
                "coding 是别的 agent 的收窄前提，不得被本包放开）。")
            self.assertEqual(
                _json_get(got, "modelPolicy"), _json_get(pre, "modelPolicy"),
                "K1-E4 RED: apply 改动了 modelPolicy —— 定点 patch 越界。")

            delta = _leaf_delta(pre, got)
            self.assertEqual(
                sorted(delta), sorted(WRITE_LEAVES),
                f"K1-E4 RED: pre → 终态展平叶差集 = {sorted(delta)}，须恰 "
                f"{sorted(WRITE_LEAVES)} —— 写入面 = 两个叶键，不多不少"
                f"（多出的叶 = 越界写；少一个 = 漏写 profile 或 allow）。")
            self.assertEqual(
                delta[PROFILE_PATH], (_ABSENT, TARGET_PROFILE),
                f"K1-E4 RED: profile 叶差 = {delta[PROFILE_PATH]!r}，须为 "
                f"（缺席 → \"full\"）。")
            self.assertEqual(
                delta[ALLOW_PATH], (list(LEGACY_TOOLS_FOUR), list(allow)),
                f"K1-E4 RED: allow 叶差 = {delta[ALLOW_PATH]!r}，须为 "
                f"（{list(LEGACY_TOOLS_FOUR)} → 五项）。")

    # ------------------------------------------------------------------ #
    # K1-E4 · gen-patch 形态（patch 生成器，纯离线）（行为级）
    # ------------------------------------------------------------------ #

    def test_k1_e4_gen_patch_product_exactly_target_leaves_and_zero_config_write(self):
        """coverage: K1-E4 — --gen-patch：rc 0 产出 JSON 子集文件、展平叶键集恰 =
        {tools.profile: "full", tools.allow: 五项}**两个**叶键；不写 config
        （字节不变、零备份）；不读 config（config 路径不存在时仍可生成）。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        allow = self._declared_allow(decl)
        profile = self._declared_profile(decl)
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            pre = _fixture_legacy_four()
            _write_json(cfg, pre)
            before = cfg.read_bytes()
            out = td / "kidpal-tools.patch.json"

            rc, out_text, err = _run_py(
                APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH, "--gen-patch", out])
            self.assertEqual(
                rc, 0,
                f"K1-E4 RED: --gen-patch rc={rc}（应 0）—— patch 生成器形态缺失"
                f"（TaskPackage notes ⑤，261023 批 B 先例）。stdout={out_text!r} "
                f"stderr={err!r}")
            self.assertTrue(
                out.is_file(), "K1-E4 RED: --gen-patch 未产出 patch 文件。")
            patch, perr = _load_json(out)
            self.assertIsNone(
                perr,
                f"K1-E4 RED: patch 产物不可被 stdlib json 解析: {perr}（冻结契约："
                "JSON 子集形态，JSON5 超集不得破坏 stdlib 解析）。")
            leaves = sorted(
                (p, json.dumps(v, sort_keys=True, ensure_ascii=False))
                for p, v in _flatten_leaves(patch))
            want = sorted([
                (PROFILE_PATH, json.dumps(profile, ensure_ascii=False)),
                (ALLOW_PATH, json.dumps(list(allow), sort_keys=True,
                                        ensure_ascii=False)),
            ])
            self.assertEqual(
                leaves, want,
                "K1-E4 RED: patch 产物展平叶键集 ≠ 声明两叶键逐字（须恰两个叶键 "
                f"{{profile: \"full\", allow: 五项}}；不多不少）。\n实得：{leaves}")
            self.assertIn(
                "honcho_search_conclusions",
                json.dumps(patch, ensure_ascii=False),
                "K1-E4 RED: patch 产物不含 honcho_search_conclusions —— 检索工具"
                "未进写入面。")
            self.assertNotIn(
                DENY_PATH,
                {p for p, _ in _flatten_leaves(patch)},
                "K1-E4 RED: patch 产物含 tools.deny 叶 —— deny 不进写入面。")

            self.assertEqual(
                cfg.read_bytes(), before,
                "K1-E4 RED: --gen-patch 改写了 config —— 生成器形态须零 config 写入"
                "（配置只经 OpenClaw 自有写入口落盘）。")
            self.assertEqual(
                _backups(td, pattern), [],
                "K1-E4 RED: --gen-patch 建了备份 —— 生成器形态须零副作用。")

            # 不读 config：指向不存在的 config 也能生成（生成器 = 纯声明消费）
            out2 = td / "kidpal-tools.patch2.json"
            rc2, out2_text, err2 = _run_py(
                APPLY_SCRIPT, ["--config", td / "does-not-exist.json",
                               "--decl", DECL_PATH, "--gen-patch", out2])
            self.assertEqual(
                rc2, 0,
                f"K1-E4 RED: --gen-patch 在 config 不存在时 rc={rc2}（应 0）—— "
                "生成器形态不得依赖 config（冻结契约：不读 config）。"
                f"stdout={out2_text!r} stderr={err2!r}")
            self.assertTrue(
                out2.is_file(), "K1-E4 RED: config 缺失时 --gen-patch 未产出文件。")

    # ------------------------------------------------------------------ #
    # K1-E4 · 回滚（dry-run + 一步还原）（行为级）
    # ------------------------------------------------------------------ #

    def test_k1_e4_rollback_dry_run_and_restore(self):
        """coverage: K1-E4 — 回滚脚本 --dry-run 可跑（exit 0）且零写入；非
        dry-run 一步还原 backup 内容（exit 0）。备份整档还原，语义与键数无关
        （本包零改申报）。"""
        self.assertTrue(
            ROLLBACK_SCRIPT.is_file(),
            f"K1-E4 RED: 回滚脚本不在位 —— 冻结契约要求 {ROLLBACK_SCRIPT}。")
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            bak = td / "openclaw.json.bak-kidpal-tools-20261023T000000"
            cur = {"applied": True}
            _write_json(cfg, cur)
            _write_json(bak, {"applied": False})

            rc, out, err = _run_py(
                ROLLBACK_SCRIPT, ["--config", cfg, "--backup", bak, "--dry-run"])
            self.assertEqual(
                rc, 0,
                f"K1-E4 RED: rollback --dry-run rc={rc}（应 0）。"
                f"stdout={out!r} stderr={err!r}")
            after, _ = _load_json(cfg)
            self.assertEqual(after, cur, "K1-E4 RED: --dry-run 改写了 config。")

            rc2, out2, err2 = _run_py(ROLLBACK_SCRIPT, ["--config", cfg, "--backup", bak])
            self.assertEqual(
                rc2, 0, f"K1-E4 RED: rollback rc={rc2}。stdout={out2!r} stderr={err2!r}")
            after2, _ = _load_json(cfg)
            self.assertEqual(
                after2, {"applied": False}, "K1-E4 RED: 回滚未还原 backup 内容。")

    # ------------------------------------------------------------------ #
    # K1-E4 · openclaw.json 零入仓 / 声明零 secret（机制级）
    # ------------------------------------------------------------------ #

    def test_k1_e4_no_openclaw_json_in_repo_and_no_secret_in_declaration(self):
        """coverage: K1-E4 — openclaw.json 整文件不入仓（含 gateway.token）；声明
        工件内不得出现 gateway.token 字样、不得出现 token 键。"""
        leaked = [str(p) for p in SERVER.rglob("openclaw.json") if p.is_file()]
        self.assertEqual(
            leaked, [],
            "K1-E4 RED: 仓内出现 openclaw.json（含 gateway.token secret）—— "
            "只入无 secret 的声明工件，整文件零入仓。实得："
            f"{leaked}")
        decl_text = _read_text(DECL_PATH)
        self.assertTrue(decl_text.strip(), "K1-E4 RED: 声明工件不在位/为空（前置）。")
        self.assertNotIn(
            "gateway.token", decl_text,
            "K1-E4 RED: 声明工件出现 gateway.token 字样 —— 声明面不得承载 secret 键。")
        self.assertIsNone(
            re.search(r'"token"\s*:', decl_text),
            "K1-E4 RED: 声明工件出现 token 键 —— 声明面不得承载 secret。")

    # ------------------------------------------------------------------ #
    # K1-E4 · manifest 登记（部署单一事实源，261013 纪律）（机制级）
    # ------------------------------------------------------------------ #

    def test_k1_e4_manifest_registers_the_patch_workbench(self):
        """coverage: K1-E4 — manifest 至少三条 261022-openclaw-kidpal-tools 条目
        （声明 / apply / rollback），src 在仓内存在、dst 在
        /opt/kidpal/openclaw-patch/261022-openclaw-kidpal-tools/ 消费位；三条
        note 陈旧字面换代（负锚：不得再出现「恰四项」；语义锚：须出现 profile
        键面与五项口径）。"""
        self.assertTrue(_HAS_YAML or self.fail(
            "K1-E4 RED(infra): PyYAML 不在位——manifest 解析前置缺失。"))
        self.assertTrue(
            MANIFEST_YAML.is_file(), "K1-E4 RED: manifest.yaml 不在场。")
        try:
            doc = yaml.safe_load(MANIFEST_YAML.read_text(encoding="utf-8"))
        except Exception as e:  # noqa: BLE001 — 解析错误是 RED，不是 crash
            self.fail(f"K1-E4 RED: manifest.yaml 解析失败: {e!r}")
        entries = [
            e for e in (doc or {}).get("entries", []) or []
            if isinstance(e, dict) and MANIFEST_DIR_TOKEN in (
                str(e.get("src") or "") + str(e.get("dst") or ""))]
        self.assertGreaterEqual(
            len(entries), 3,
            f"K1-E4 RED: manifest 对 {MANIFEST_DIR_TOKEN} 的登记条目 "
            f"{len(entries)} < 3 —— 声明 / apply / rollback 三件须各入部署单一"
            "事实源（261013 纪律：新资产必须登记并接入 verify-deploy.sh）。")
        srcs = [str(e.get("src") or "") for e in entries]
        missing_src = [s for s in srcs if s and not (REPO_ROOT / s).is_file()]
        self.assertEqual(
            missing_src, [],
            f"K1-E4 RED: manifest 登记的仓内源缺失: {missing_src}。")
        bad_dst = [str(e.get("dst") or "") for e in entries
                   if not str(e.get("dst") or "").startswith(
                       "/opt/kidpal/openclaw-patch/" + MANIFEST_DIR_TOKEN)]
        self.assertEqual(
            bad_dst, [],
            f"K1-E4 RED: 登记条目 dst 越出 openclaw-patch 消费位: {bad_dst}。")

        notes = "\n".join(str(e.get("note") or "") for e in entries)
        self.assertNotIn(
            "恰四项", notes,
            "K1-E4 RED: manifest kidpal-tools note 仍写「恰四项」—— 陈旧字面未"
            "换代（目标态 = profile full + 恰五项含 honcho_search_conclusions；"
            f"{ROOT_CAUSE_ANCHOR}）。实得 note：{notes!r}")
        self.assertIn(
            "profile", notes,
            "K1-E4 RED: manifest kidpal-tools note 未提 tools.profile —— 部署面"
            f"文案漏掉本修复的核心键（{ROOT_CAUSE_ANCHOR}）。实得 note：{notes!r}")
        self.assertTrue(
            any(w in notes for w in ("五项", "honcho_search_conclusions")),
            "K1-E4 RED: manifest kidpal-tools note 无五项 / 检索工具口径 —— "
            f"陈旧字面换代未落。实得 note：{notes!r}")


class ToolsProfileReachabilityAcceptance(unittest.TestCase):
    """K1-E4-C · 防回归锚（261025-kidpal-tools-profile-fix；offline）。

    本次教训（live 实证）：`tools.allow` 加名 ≠ 工具可达——OpenClaw 过滤链
    profile 先行，allow 只能在剩余集里挑，所以 per-agent
    `tools.profile="full"` 是 honcho 检索工具可达的**必要条件**。

    机械承载三件（TaskPackage exitCriteria）：① 声明 changes 含
    tools.profile="full"；② profile 在场且 ≠ "full" → exit 3（真跑）；③ allow
    五项 + profile 缺席 → 可升级（真跑，不得拒绝自身升级起点）。外加根因注释
    锚逐字自锚。"""

    maxDiff = None

    def test_k1_e4_c_root_cause_anchor_pinned_in_this_file(self):
        """coverage: K1-E4-C — 根因注释锚逐字在场（本件自锚）：防「后来者删掉
        教训注释、按旧心智把写入面改回单键 allow」的回归。"""
        src = _read_text(Path(__file__).resolve())
        self.assertIn(
            ROOT_CAUSE_ANCHOR, src,
            "K1-E4-C RED: 本验收件缺根因注释锚逐字句 —— 教训未钉进验收件："
            f"{ROOT_CAUSE_ANCHOR}")

    def test_k1_e4_c_declaration_carries_tools_profile_full(self):
        """coverage: K1-E4-C — ① 声明 changes 含一条
        agents.entries.kidpal.tools.profile，value 恰 "full"（且恰一条，不重复）；
        allow 仍收窄恰五项（per-agent profile 放开不等于工具面放开）。"""
        decl = _require_decl("K1-E4-C")
        changes = _declared_changes(decl, "K1-E4-C")
        profile_changes = [
            c for c in changes
            if isinstance(c, dict) and c.get("path") == PROFILE_PATH]
        self.assertEqual(
            len(profile_changes), 1,
            f"K1-E4-C RED: changes 中 profile 叶条目数 = {len(profile_changes)}，"
            f"须恰 1（{PROFILE_PATH}）—— per-agent profile 覆盖是插件工具可达的"
            f"必要条件（{ROOT_CAUSE_ANCHOR}）。")
        self.assertEqual(
            profile_changes[0].get("value"), TARGET_PROFILE,
            f"K1-E4-C RED: tools.profile 叶值 = {profile_changes[0].get('value')!r}，"
            f"须恰 \"full\"（{ROOT_CAUSE_ANCHOR}）。")
        allow = _declared_allow(decl, "K1-E4-C")
        self.assertEqual(
            sorted(allow), sorted(EXPECTED_TOOLS),
            "K1-E4-C RED: allow 不是契约五项 —— profile 放开的同时 allow 必须"
            "收窄不变（安全面：honcho_search_messages 等未列工具仍不可用）。")

    def test_k1_e4_c_profile_not_full_is_drift_exit3(self):
        """coverage: K1-E4-C — ② profile 在场且值 ≠ "full"（"coding"）→ exit 3
        零写零备份（真跑）。profile 在场就按值判，不是「在场即过」——否则
        live 上被 coding 收窄的态会被当可升级态静默叠加。"""
        decl = _require_decl("K1-E4-C")
        pattern = _require_backup_pattern(decl, "K1-E4-C")
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            pre = _fixture_profile_not_full()
            _write_json(cfg, pre)
            before = cfg.read_bytes()

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(
                rc, 3,
                f"K1-E4-C RED: profile=\"coding\"（在场非 full）时 apply rc={rc}"
                "（应 3）—— 该态被判可升级会静默覆盖既有 profile 值。"
                f"stdout={out!r} stderr={err!r}")
            self.assertEqual(
                cfg.read_bytes(), before,
                "K1-E4-C RED: profile 非 full 被拒后仍改写了 config。")
            self.assertEqual(
                _backups(td, pattern), [],
                "K1-E4-C RED: profile 非 full 被拒后仍建了备份（拒绝 = 完全 no-op）。")

    def test_k1_e4_c_allow_five_with_profile_absent_is_upgradable(self):
        """coverage: K1-E4-C — ③ P0 中间态（allow 五项 + profile 缺席）→ 可升级
        （rc 0，apply 补 profile="full"、allow 保持五项）。这是本包最典型的升级
        起点：拒绝它 = 拒绝自己的升级路径。"""
        decl = _require_decl("K1-E4-C")
        allow = _declared_allow(decl, "K1-E4-C")
        profile = _declared_profile(decl, "K1-E4-C")
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            pre = _fixture_p0_intermediate()
            _write_json(cfg, pre)
            want = _target_state(pre, profile, allow)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(
                rc, 0,
                f"K1-E4-C RED: allow 五项 + profile 缺席（P0 已应用态）上 apply "
                f"rc={rc}（应 0）—— 拒绝自身升级起点（纠错 ①：profile 缺席必须"
                "接受，apply 补 full）。"
                f"stdout={out!r} stderr={err!r}")
            got, gerr = _load_json(cfg)
            self.assertIsNone(gerr, f"K1-E4-C RED: apply 后 config 不可解析: {gerr}")
            self.assertEqual(
                got, want,
                "K1-E4-C RED: P0 中间态升级后 ≠ 两叶目标态（allow 五项须保持、"
                "profile 须补 \"full\"）。")
            self.assertEqual(
                _json_get(got, ALLOW_PATH), list(allow),
                "K1-E4-C RED: 升级时 allow 被改动 —— 升级只补 profile，allow 五项"
                "原样保持。")


class MemoryRecallRoundAcceptance(unittest.TestCase):
    """261024-memory-recall-tool-enable K1-E4-B（AGENTS.md 静态语义，offline）。

    被测面 = server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md（静态语义
    单一事实源）。断言口径 = 语义锚组（措辞换写不误红、纪律语义被删才红）。
    live 面（真机问「王一诺是谁」+ 日志工具调用）= human 部署轮，本类零 host
    断言（头注缺口节已申报）。本包（261025）零改动本类。"""

    maxDiff = None

    def _agents_body(self) -> str:
        self.assertTrue(
            AGENTS_MD.is_file(),
            f"K1-E4-B RED: {AGENTS_MD} 不在场 —— 静态语义单一事实源镜像缺失。")
        body = _read_text(AGENTS_MD)
        self.assertTrue(body.strip(), "K1-E4-B RED: AGENTS.md 为空。")
        return body

    def test_k1_e4_b_memory_recall_round_row_semantics(self):
        """coverage: K1-E4-B — 「回合类型与纪律路由」表内新增「记忆回忆回合」行：
        识别特征 + 先调一次 honcho_search_conclusions + 查到自然转述（不逐字 /
        不带内部流程 / 家长隐私不回忆）+ 查不到才说记不清 + 过渡语 + 与孩子对话
        回合禁工具的关系显式写清；honcho_search_messages 不得出现在本行。"""
        body = self._agents_body()
        section = _section_span(body, ROUTING_HEADING_NEEDLE)
        self.assertIsNotNone(
            section,
            f"K1-E4-B RED: AGENTS.md 无「{ROUTING_HEADING_NEEDLE}」节 —— 回合路由"
            "承载面缺失（K3 冻结锚串同缺）。")
        rows = [l for l in section.splitlines() if MEMORY_ROUND_NAME in l]
        self.assertTrue(
            rows,
            f"K1-E4-B RED: 回合路由节内无「{MEMORY_ROUND_NAME}」行 —— 记忆回忆纪律"
            "未落（模型仍按「孩子对话回合禁工具」执行，历史事实永远答不出）。")
        self.assertTrue(
            any(l.lstrip().startswith("|") for l in rows),
            f"K1-E4-B RED: 「{MEMORY_ROUND_NAME}」不在回合路由**表**内（须为表格行）"
            "—— 纪律落点须与其它回合同表，避免与「孩子对话回合」抢判定。")
        row = "\n".join(rows)
        for what, group in MEMORY_ROUND_ANCHORS.items():
            self.assertTrue(
                any(w in row for w in group),
                f"K1-E4-B RED: 「{MEMORY_ROUND_NAME}」行缺纪律锚组「{what}」"
                f"（任一即可：{group}）—— 语义删除或稀释。")
        self.assertNotIn(
            FORBIDDEN_TOOL, row,
            f"K1-E4-B RED: 「{MEMORY_ROUND_NAME}」行出现 {FORBIDDEN_TOOL!r} —— "
            "原始消息面（家长代述 / 流程内容）不得进入儿童端回合纪律。")

    def test_k1_e4_b_memory_section_keeps_no_tool_for_recording_sentence(self):
        """coverage: K1-E4-B — 记忆节保留原句关键半句「不调用任何工具去记」
        （工具不用于「记」仍真；不得为放行检索而删改）+ 「不需要写任何文件」+
        261023 A3 四锚继续在场 + 新增一句检索澄清（记仍自动、旧事回忆走检索）。"""
        body = self._agents_body()
        mem = _section_span(body, MEMORY_SECTION_NEEDLE)
        self.assertIsNotNone(
            mem,
            f"K1-E4-B RED: AGENTS.md 无标题含「{MEMORY_SECTION_NEEDLE}」的 `## ` 节 "
            "—— 记忆守则节被删/改名（261023 A3 承载面缺失）。")
        self.assertIn(
            "不调用任何工具去记", mem,
            "K1-E4-B RED: 记忆节原句半句「不调用任何工具去记」不逐字在场 —— "
            "为放行检索而删改记忆守则会同时破 261023 A3「不调用任何工具」正锚；"
            "正确改法是保留原句（工具不用于「记」仍真）+ 新增检索澄清句。")
        for positive in A3_MEMORY_POSITIVES_COMPAT:
            self.assertIn(
                positive, mem,
                f"K1-E4-B RED: 记忆节缺 A3 正锚 {positive!r}（261023 冻结面"
                "零回退：平台自动承载 / 想不起来诚实说记不清 / 绝不编造）。")
        self.assertTrue(
            any(w in mem for w in MEMORY_RETRIEVAL_CLARIFY),
            "K1-E4-B RED: 记忆节无检索澄清锚（任一即可："
            f"{MEMORY_RETRIEVAL_CLARIFY}）—— 「自动记」与「旧事要查」两个口径"
            "未分开，模型会在「不需要工具」与「先查一次」之间自相矛盾。")

    def test_k1_e4_b_anchor_compat_no_regression(self):
        """coverage: K1-E4-B — 既有锚兼容红线（additive 换代）：K1-E1 回合路由
        五行零回退（孩子对话回合行禁工具锚组仍在）；K3_P1_AGENTS_ANCHORS 逐字；
        261023 A3 对象锁定行 K3 隐私锚串 + S3 澄清两半逐字。"""
        body = self._agents_body()
        for round_name in K1E1_ROUND_NAMES:
            self.assertTrue(
                [l for l in body.splitlines() if round_name in l],
                f"K1-E4-B RED: AGENTS.md 无含「{round_name}」的行 —— K1-E1 回合"
                "路由五行被新行挤掉（换代须 additive）。")
        child_row = "\n".join(l for l in body.splitlines() if "孩子对话回合" in l)
        self.assertTrue(
            any(w in child_row for w in K1E1_CHILD_ROUND_TOOL_BAN),
            "K1-E4-B RED: 孩子对话回合行禁工具锚组消失 —— K1-E1 核心纪律被削弱"
            "（查询放行只许加法：新回合行承载，不动既有回合）。")
        for anchor in K3_P1_AGENTS_ANCHORS_COMPAT:
            self.assertIn(
                anchor, body,
                f"K1-E4-B RED: K3_P1_AGENTS_ANCHORS 锚串不逐字在场：{anchor!r} —— "
                "P1 迁移段单向追加红线被破（test_261022_honcho_memory 同锚）。")
        lock_lines = [l for l in body.splitlines() if A3_OBJECT_LOCK_KEY in l]
        self.assertTrue(
            lock_lines,
            f"K1-E4-B RED: AGENTS.md 无「{A3_OBJECT_LOCK_KEY}」行 —— 261023 A3 "
            "对象锁定条缺位。")
        for line in lock_lines:
            self.assertIn(
                K3_P1_AGENTS_ANCHORS_COMPAT[-1], line,
                "K1-E4-B RED: 对象锁定行 K3 隐私锚串不逐字在场（含右括号）—— "
                "261023 A3 冻结锚被改。")
            for clarify in A3_S3_CLARIFIES:
                self.assertIn(
                    clarify, line,
                    f"K1-E4-B RED: 对象锁定行缺 S3 澄清 {clarify!r} —— 261023 A3 "
                    "甲案裁决在场面被删。")


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
