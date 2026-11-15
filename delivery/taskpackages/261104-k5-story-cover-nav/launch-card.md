# 启动卡 · 261104-k5-story-cover-nav

跑 261104-k5-story-cover-nav（261103-star-points-story-cover 列车，mergeOrder 76，dependsOn=[K2, K4]，full / riskTier **high**，gateProfile=[evidence-integrity, test-quality, state-machine, mcp-tool-contract]）。故事封皮导航：P4 封皮（书封美术 + 「星星记录本」标题 + 现存星星数 + 键帽「打开 / 下一页」）、列表/阅读下沉为 P4 子页、逐层返回（阅读 → 列表 → 封皮 → 首页）、story_list 共享帧消费（薄载荷只进封皮、后台推送恒 `restore:true` 不抢前台）、`ic-coin` 与书封资产。

- **调度判定（LP13 核销）**：dependsOn K2 @`76ed332e`、K4 @`b8c53b89` 均已 merged 本地 main；K5 = 列车最后一包（WIP=1 串行链尾）。复用池树 `C:/AI/KidPal-pod-m2`（上一包 K4 已 merged，常驻分支 `dev/pod-m2` 已 rebase 到 main）；互斥租用检查通过（board 扫描无其他活跃 Pod 落在该树或该分支）。
- **基线锚**（带锚引用三件齐；落盘锚 = `delivery/workstreams/261103-star-points-story-cover/progress.md` 2026-11-15 K5 launch 段）：
  - server 面 = **447 passed / 0 failed / 33 subtests passed / 9 deselected** @ SHA `03630b65`（2026-11-15 实测，本段同日现跑；命令 = `python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-m2/server/acceptance"`（仓根执行）；面域 = `server`）。旧锚 435 @`cad07f2c`（K4 launch 段）因 K3/K4 合并改 server/ 判停失效，本行取代。
  - firmware host 面 = **292 Tests / 3 Failures / 0 Ignored** @ SHA `03630b65`（2026-11-15 实测，本段同日现跑；执行目录 = `firmware/main/test/host_app`（HOST_TEST_CONVENTIONS [C4.4] 文档命令位）；命令 = 仓根 `wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` 后 `cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf`；面域 = `firmware`）。3 红全 p1_auxbar 存量族（AB-A1 电池图标 / AB-A2 温度图标 / AB-A4 wifi 资产），非本包责任。旧锚 292/3 @K4 final `c1de02fe` 因 K4 固件合并后 `git diff --quiet c1de02fe HEAD -- firmware/` 非零而判停失效，本行取代。**本包动 firmware/ 后该引用同样失效**，builder 必须重跑 host 基线并判新增红。
  - 本包验收红基线（acceptance-author 新冻结件）恒现跑，无缓存可言。
- **本包 base ref**：`03630b65`（= 包首提交父；常驻分支 `dev/pod-m2`）。包级 diff（终验 scope 圈定 / detect_changes / 白盒复核 / 合并范围）恒用此 ref，不用 main。SHA 以 `pod-events.jsonl` 的 `pod-started` 事件所记为凭。
- **上一包留根**：`delivery/taskpackages/261104-k4-reward-goal-config/pod-retro.md`，intake 逐条评估（构成 blockingDecision → fail-closed 上报）。重点三条：① 冻结验收跨用例一致性自检（K4 出现过自相矛盾用例）；② `acceptance-coverage` 闸不认 Unity 断言，需沿 K2/K4 先例补真实守卫；③ manifest 含生成件路径时必须对照生成器命名律（K4 五个文件名全错）。
- **铁律约束**（`delivery/taskpackages/261104-k5-story-cover-nav/plan.md` 全文为准）：
  - P4 封皮是故事模块唯一主入口；KEY 短按才进列表，BOOT 从封皮 wrap 回首页，KEY 长按逐层返回，每按一次只上一层。列表/阅读的 BOOT 仍用于页内浏览和翻屏，不恢复全局翻页。
  - 封皮计数等于现存篇数（只信 `story_list.total`），数据未达不显计数、确认空账可显 ×0；故事星星与积分余额不得合并。不得新增 `story_cover` view、`cover_total` 或独立计数 API。
  - 语境分工：K0a 建枚举映射、K2 扩 `points_ledger`、本包扩 `story_cover`；封皮发起普通问答，只有 P4 故事列表产生 `story` 记录语境，路过封皮不得自动开录。
  - 语音「打开故事」只到封皮；OpenClaw 只能发 story_list 空 data 薄载荷，完整列表仍只能由 front 产生。任何后台推送不抢前台。
  - 阅读 KEY 短按零响应与正确键帽以 K0a 为基线，不得再落全局 `SESSION_TOGGLE`；本包不重复承接 TECH-261028，也不吸收 TECH-261022 的独立修复范围。
  - 不改 requirements / architecture / `docs/contracts/**`；发现需要新线格式、计数源或全局键位新语义时立即停并升级。
- **intake 侦察落点（2026-11-15 只读事实核，供 Step 1g 预评起点；逐条带 path:line 在 Pod input-brief 复述）**：
  - 封皮在固件**零实现**（无枚举/无文件）；`stary_cover` 仅在架构文档、UIUX brief、`story_page.c:2322` 注释（K5 语境）、`front_app.py:1039` 已知可忽略 source_page、`four_page_skeleton.acceptance.c:14-15`（封皮归 K5）出现。
  - 今天 P4 列表 KEY 长按 = `KSPV2_ACT_GLOBAL` 直接回首页（`story_page.c:1844-1845`），须改为回封皮；阅读态 KEY 长按 = 回列表（:1940-1944）。
  - `_push_story_list` 共 5 个调用点，只有 WS 建连恢复（`front_app.py:2453`）带 `restore=True`，其余 4 处缺省 False——与 landed 契约「后台推送恒 restore:true」存在差距，按 impactedTestsSeed 重锚。
  - `{"view":"story_list","data":{}}` 空薄载荷目前被 `display_schema.py:209-211`（items 必须是数组）拒——本包 display_schema floor 需按契约开薄载荷口。
  - 资产命名律：`gen_assets.mjs` job `name` = 产物符号名，落 `<name>.bin`；`ic_coin_36_36` 与 `ic_book_48` **已存在**，无 book-cover 资产；spiffs 白名单在 `gen_art_spiffs.mjs:39-83`。
- **跨包串行**：K4→K5 共用资产管线；K4 已产场景资产并合并，本包后序补 `ic-coin` 与书封——生成器、资产清单和生成件不得整文件重生成（K4 已有「纯追加 + 其余还原」先例）。
- **验证形态申报（机器消费行，闸按顶格前缀解析）**：

验证形态申报（JSON）：{"schemaVersion":1,"precedents":{},"deviceOutcome":{"userVisible":true,"acceptanceRefs":["S-F2.6-AC1","S-F2.6-AC2","S-F2.6-AC3","S-F2.6-AC4"],"hostUncompilablePaths":["firmware/main/display/kidpal_render_node.cc"],"reason":"封皮层、P4 子页层级与逐层返回落在设备渲染面（render_node/story_page），host 不可编；K5S-E4 真机导航并集轮逐项看屏"}}

  - **Step 1g 校准义务**：申报的 host 不可编路径必须实际出现在本包合并 diff 中（闸判 `device-outcome-path-not-in-diff`）。Step 1g 预评实读后若确定封皮实现不触 `kidpal_render_node.cc`，planner 必须以实际触达的登记路径（`firmware/main/mcp_server.cc` / `firmware/main/application.cc` / `firmware/main/display/lcd_display.cc`）改写本行并留痕。真机轮证据 = `device-round-evidence.md` 机器消费行，每个申报 ref 须有 PASS 目击。
- **环境事实**：复用池树 `C:/AI/KidPal-pod-m2`（常驻分支 `dev/pod-m2` = `03630b65`）；专属图索引 `C-AI-KidPal-pod-m2` ready（2026-11-15 实查，22215 节点 / 66109 边）。
- **固件严格模式默认开**：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开，此声明行随卡走不得省；final 相位恒跑真编译）。
- **会话标识**：VS Code 扩展会话内联 planner（2026-11-15 起；点火后回填 pid / 终端标识）。
- **命令行**（两形态并列）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
  非 CLI 形态（VS Code 扩展会话）降级面：不能传 `--settings` / `--agent` ⇒ dev-loop.settings.json 三件（Stop 的 supervisor-gate-check、PostToolUse 的 portfolio-sync、permissions.ask 的 git merge/rebase）按缺席处理；起步在 pod-events 记 settings-presence（declaredForm=extension-session），收尾相位手工跑 `supervisor-gate-check.mjs` 与 `portfolio-reconcile.mjs`（K1/K2/K3/K4 先例）。
