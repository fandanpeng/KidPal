# plan · 261013-qq-schedule-write-closure

> sourceType=regression（TECH CRQ 驱动）· rigor=lite · riskTier=medium · 2026-10-13 起包。
> 上游 = [TECH-261012-qq-schedule-write-format-gap](../../docs/crq/tech/TECH-261012-qq-schedule-write-format-gap.md) 剩余修复项 ①④（②③已由部署单源批承接收口）。

## 0. 问题与目标

家长经 QQ 让 OpenClaw main agent 改写 `kidpal/schedule.md` 是课表主录入通道（B1 §2.4），但 main workspace 无写课表 skill：无 B1 格式纪律（脏日期格如 `2026-10-12 周一` 直接写进一次性安排区块）、无写后机检、无写后即时重算。front 单源 parser 大声拒绝（P2-A2 整包拒）后三触发全静默停摆、家长零感知（2026-10-12 live 实录，详见 TECH 条目证据 1）。

**目标**（TECH 修复建议 ①④，human 2026-10-13 裁定形态）：

1. **①** main workspace 补 `kidpal-schedule-write` skill：B1 写入纪律 + 写后机检（front 单源 parser 实际解析）+ 通过后即时重算落 refresh marker（设备「接下来」≈1 分钟刷新）+ 失败还原并 QQ 引导正确格式。
2. **④**（human 裁定 = alerts 件 + agent QQ 转告）：front 对 `schedule_md_err` 落 `/opt/kidpal/alerts/schedule-parse-error.json` 告警件（形状与 deploy-drift.json 同构）；main agent 经轻量 cron 检测后 QQ 主动告知家长。

## 1. 冻结裁定（human 2026-10-13 会话，不可回改）

- ④ 形态 = alerts 件 + agent QQ 转告（弃「仅 alerts 件」与「推设备屏幕」两案）。
- 范围 = TECH ①④ 全两项（②③不重做，归 261013-gov-review-deploy-single-source-loop）。
- B1 契约零改动：skill 只读消费 interface-contract.md §2.2/§2.3 既有格式。

## 2. 改动面（changeManifest）

- 新增 `server/deploy/openclaw-skills/workspace-main/kidpal-schedule-write/SKILL.md`
- 新增 `server/deploy/openclaw-skills/workspace-main/kidpal-schedule-health/SKILL.md`
- 改 `server/deploy/manifest.yaml`（两 skill 条目 + cron_jobs 一条）
- 改 `server/deploy/openclaw-skills/README.md`（表 +2 行）
- 改 `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md`（§8.3c：health cron 注册 + QQ 路由目标指引）
- 改 `server/gateway/schedule.py` + `server/gateway/front/front_app.py`（④ 告警写 + :687 catch 位挂接）
- 新增 `server/tests/test_schedule_alert_units.py`
- append `docs/crq/tech/TECH-261012-qq-schedule-write-format-gap.md`（收口补记）

## 3. B1 写入格式权威摘录（interface-contract.md §2.2/§2.3，skill 文本据此展开）

- 每周固定课表：表头 `| 星期 | 时间 | 课程 | 地点 | 备注 |`；星期 = 周一~周日；时间 = `HH:MM-HH:MM` 或 `HH:MM`。
- 一次性安排：表头 `| 日期 | 时间 | 事项 | 地点 | 备注 |`；日期 = 纯 `YYYY-MM-DD`（**严禁星期后缀**——2026-10-12 live 事故源）；五列形状；表头逐字范本（曾实测漂成「星期/日期」「课程/事项」致拒）。
- §2.4：QQ 录入 = 主通道，agent 自知改写 + 即时触发三档重算与推送。

## 4. 技术方案（planner+architect 预评）

> 2026-10-13 · architect 1g lite 预评（只出意见、不落地、不豁免任何门、不构成通过判据；planner 消费。本节为「planner 只读消费 plan.md」规则的唯一例外。architect 侧 scope-map 写槽不含 delivery/taskpackages/**，故由 planner 代落，内容逐字未改。）

### 4.1 ① 写后机检（kidpal-schedule-write skill）——命令形态与失败处置
- **parser 真实签名（已读 `server/gateway/schedule.py` 核对，勿凭印象）**：`parse_schedule_md(md_text: str)` 吃 **md 文本**（不是文件路径）；`group_upcoming(model, date_iso, now_hhmm, school_start=None)` 吃 **model dict**（不是文本）。机检必须「先 `Path(...).read_text()` 再 `parse_schedule_md(text)`」，确认新行在列再 `group_upcoming(model, 今天, now)` 看当日「接下来」是否含该事项。
- **import 形态（关键坑）**：`KIDPAL_SERVER_DIR=/opt/kidpal/front python3 -c "from gateway import schedule"` 不天然成立——`KIDPAL_SERVER_DIR` 是 `reminder_schedule_lib._schedule_module()` 自己读的 env（先试 `from gateway import schedule`、ImportError 才回退到 `${KIDPAL_SERVER_DIR}/gateway/schedule.py` 按文件路径加载），不是 Python 的 sys.path 注入。skill 用 `python3 -c` 最稳写法 = 按**绝对文件路径**加载（与 `schedule_push._schedule_callable` 同款，零 cwd/PYTHONPATH 依赖）：
  ```
  /opt/kidpal/venv/bin/python3 -c '
  import importlib.util
  from pathlib import Path
  spec = importlib.util.spec_from_file_location("sched", "/opt/kidpal/front/gateway/schedule.py")
  m = importlib.util.module_from_spec(spec); spec.loader.exec_module(m)
  md = Path("/root/.openclaw/workspace/kidpal/schedule.md").read_text(encoding="utf-8")
  model = m.parse_schedule_md(md)
  print("parse OK", len(model["weekly"]), len(model["onetime"]))
  '
  ```
  解释器必须 `/opt/kidpal/venv/bin/python3`（钉绝对路径，261013 部署批 T4 / TECH-261012 证据 3 的 3.6.8 坑）；`/opt/kidpal/front/gateway/schedule.py` 是 manifest 铺放的 canonical 位（本包不另建第二套解析）。
- **失败处置 = 还原优先（不进入开放自修循环）**：编辑前**必须先备份** schedule.md（同目录 `.bak` 或记下 diff），机检不过 → **还原本次编辑** + QQ 回家长「正确格式范本 + 具体坏 token」，不反复试改——整份 schedule.md 解析失败 = 三触发全静默停摆（本包要消除的病灶），宁可丢一次录入也不留坏文件。机检通过 → 才落 refresh marker（`alerts/schedule.refresh`）触发③ 即时重算。

### 4.2 ④ 告警写函数——落点与最小侵入
- **落点 = `front_app.py`，不是 `schedule.py`**。`schedule.py` 是冻结纯解析模块（docstring「纯 python、offline 可导入、不得偏移」），加文件 I/O 会扩冻结契约面 + 破坏零 I/O 纯净性。`front_app.py` 已有 `ALERTS_DIR`、`log`、现成 catch 位（:686-688），私有 helper（如 `_write_schedule_parse_alert(err_type, detail)`）就地最小侵入。**`schedule.py` 无需改动**。
- **挂接点**：`_load_schedule_model()` 的 `except ValueError` 块（现 :687 `log.warning("schedule_md_err …")` 行旁）加一行调用。`_load_schedule_model` 每分钟被 `run_schedule_poll_once` 调一次（已核 :828 + `reminder_poll_loop` 每 tick），脏文件期间每分钟重写告警件 → `checked_at` 持续刷新，修好后解析成功、不再写 → `checked_at` 冻结（health skill 据此判「已恢复」，front 无需在成功路径删件）。
- **字段形状定稿建议**（与 deploy-drift.json「同构」= 同族机器可读告警件，非字面同字段）：
  ```json
  { "checked_at": "ISO8601 UTC", "error": "ValueError", "detail": "invalid date token: '2026-10-12 周一'（截断 ~120 字）" }
  ```
  `checked_at` 对齐 deploy-drift.json 同名字段口径；`error` = `type(e).__name__`；`detail` = `str(e)[:120]`（与现日志行同源）。**不加 ack 字段**——front 每分钟覆写该件，ack 若写件内会被下一分钟冲掉（ack 归 health skill 自己的状态件，见 4.3）。
- **写失败静默吞**：告警写必须自身 try/except 全吞（至多 debug 日志），不得让写告警件失败反向上抛——`_load_schedule_model` 的契约是「返回 None、不炸轮询循环」，告警写必须守这条线。

### 4.3 health skill + cron 形态（④ 的 agent 转告半边）
- **cron = agent 型 `every 15m`（对齐 kidpal-reminder-semantic 先例）**：`expect_agent: main` + `expect_schedule: "every 15m"`。**频率必须 15m**——verify-deploy.sh 的 cron 核验把 `expect_schedule` 硬编成 `every_min == 15`（已读 :123-127），非 15m 会假报漂移；15m 检测粒度对本病灶（误持续 17 分钟才被人工发现）足够。
- **ack 语义防重复打扰（关键）**：front 告警件**不含 ack**，ack 状态放 health skill 自有的独立状态件 `/opt/kidpal/alerts/schedule-parse-health.json`（单写者 = health skill/main agent，front 不碰）。内容建议 `{ "last_notified_fingerprint": "<error>|<detail>", "last_notified_at": ISO }`。检测逻辑：
  1. 无 `schedule-parse-error.json` → 无告警，不动状态件（可顺手清）。
  2. 有件但 `checked_at` 距 now > ~2-3 分钟 → 已恢复（front 不再刷新 = 解析已过），清状态件、不打扰。
  3. 有件且新鲜、且 `last_notified_fingerprint` == 当前 fingerprint → 已告知过，跳过。
  4. 否则 → QQ 告知家长（坏 token + 正确格式引导），写状态件（新 fingerprint + 时间）。
  fingerprint 建议用 `error + "|" + detail`（detail 含具体坏 token，换一个坏 token 会再次告知，正符合「家长又写坏一个不同格」场景）。
- **QQ 路由目标缺口不虚诺**：health cron 是「无入站消息要回」的主动 announce，OpenClaw 要能投到家长 QQ 需先配 delivery/announce 路由目标（TECH-261012 证据 5 与 260924 ops-record §六「announce → no route, fail-closed」同源缺口；家长 openid = accounts.dad 的 allowFrom 唯一项，mom 仍 `enabled:false` 占位）。runbook §8.3c 必须**如实写两层**：① 注册命令；② 「QQ 路由目标前置」——路由未配时 health 回合仍跑、仍能读到告警件，但主动告知 fail-closed 投不出，**可观测面降级 = 告警件 + journalctl `schedule_md_err`**（不是「什么都不会发生」）；路由配置 = human post-deploy 动作，指到 260924 ops-record 的 accounts.dad/mom 映射位，不虚诺「配好就能收到」。

### 4.4 manifest 条目与部署「单一写者」共存
- 两 skill 各一条 `entries`：`src: server/deploy/openclaw-skills/workspace-main/kidpal-schedule-{write,health}/SKILL.md`、`dst: /root/.openclaw/workspace/skills/<同名>/SKILL.md`、`owner: deploy.sh`、`verify: md5`、`restart: none`——与既有 5 条 skill 条目字段完全同款（skill 由 OpenClaw 下一回合读取，无服务重启面）。
- `cron_jobs` 增 `kidpal-schedule-health` 一条（见 4.3）。**一次写对**：`verify: md5` 的 src 必须在仓内先建好（builder 同批落 SKILL.md + manifest 条目，原子），否则 verify-deploy.sh 报「仓内源缺失」；「顺序别反」= 改清单进 review → deploy.sh 铺放（manifest 文件头纪律）。
- 告警件 `schedule-parse-error.json` 与 ack 件 `schedule-parse-health.json` 是**运行态输出，不进 manifest**（同 pending.json / semantic.json 先例）。

### 4.5 单元测试落地提示（给 acceptance-author）
- 告警写函数在 front_app.py → 单测沿用既有 `FrontPollOnceIntegration._load_front` 的 fastapi-mock 装载模式。**坑**：`SCHEDULE_MD`/`ALERTS_DIR` 是 front_app 模块级常量、**import 时**从 env 读（:668-669），测试须在 `_load_front()` **之前**把 `KIDPAL_SCHEDULE_MD`/`KIDPAL_ALERTS_DIR` 指向 temp 文件/目录，再 `_load_front()`，再调 `_load_schedule_model()` 断言告警件有/无。

### 4.6 changeManifest 复核（意见，planner 已按此收敛）

1. **多项（已移除）**：`server/gateway/schedule.py`——告警写函数按 4.2 落 front_app.py，冻结纯解析器零改动；若保留会引入无意义 diff 并可能诱导 builder 把 I/O 写进冻结模块。
2. **无漏项**，逐条核对：`server/gateway/__init__.py` 不需要加（不新增 gateway 模块）；`DEPLOY-RUNBOOK.md` 在册且不进 manifest（repo 侧运行手册非铺放源），正确；两新 SKILL.md + manifest.yaml + README.md + test 文件 + TECH append 均在。
3. **落点提示**：builder 必须同批落两个 SKILL.md + manifest 条目（原子），否则 `verify: md5` 的 src 缺失会让 verify-deploy.sh 报「仓内源缺失」。

### 4.7 impactedTests 种子复核（意见）

1. 两条种子对核心行为够：`test_schedule_alert_units.py`（additive）+ `test_reminder_units.py`（零改动回归观察）。
2. **不加 PyYAML 版 manifest 完整性 pytest**：本仓 server 测试面零 `import yaml`（`load_ladder_config` 是手写 YAML-lite 行解析），manifest 的 yaml 解析 + 完整性扫描 canonical 归属 = ECS 侧 verify-deploy.sh；落 host pytest 会破纯 stdlib 纪律。QSW-A2 的 yaml 断言应作 acceptance 侧/ECS 验证口径。
3. 可选廉加固（stdlib-only，非必做）：新测试文件内加一条「manifest 引用的两个新 SKILL.md src 路径在仓内真实存在」的 `Path.exists()` 断言——不碰 yaml，专抓 src 拼写错/忘建文件。

## 5. 验收面草案（acceptance-author 冻结权威）

见 taskpackage.json acceptance[] QSW-A1~A5；QSW-A5 = live-manual structural 殿后项（依赖 ECS bootstrap + openclaw cron 注册 + QQ 路由目标）。

## 6. 风险与依赖

- QSW-A5 依赖 261013-gov-review-deploy-single-source-loop 的 ECS bootstrap 未发生——offline 面先行，live 留痕殿后。
- QQ 路由目标（家长 openid）= 既有缺口（TECH-261012 证据 5 噪音红在案），live 配置归 human post-deploy。
- `server/gateway/schedule.py` 是 front 单源解析器、2026-10-13 部署批刚核过 live 一致——本包改动必须保持「与部署一致性共存」（改动进仓后下次 deploy.sh 铺放即生效，无需另通道）。
