# docs-delta · 261022-m5-n2a-schedule-mechanical-injection

- **docsSyncRequired = false**（taskpackage.json `docsSyncReason`）：架构 §5.2 场景②动态 context 装配段 / §5.1.2 front 行课表摘要句 / §5.1.3 依赖方向已由 261022 arch 批 FROZEN v1.1 landing（commit `0411e957`）落稿；本包只落实现面。**本包 docs/** 仅一项改动 = TECH-261018 结项件（`docs/debt/tech/TECH-261018-schedule-read-compliance-residual.md` → frontmatter status: closed + closedBy/consumed-by = 本包 changeId + 正文 append 结项记一段，不覆盖既有行；`debt-render.mjs` 已跑、条目归位 `docs/debt/tech/_archive/2026/`，tech 域两份 INDEX 生成件随之收敛）。`docs/contracts/**` 全程只读、零 diff；spec 两文件零 diff。

## 声明性注记（zero-diff 结论之外如实登记；不改任何 docs 文件）

### 注记 1 · P1 遗留观察项（builder 遗留②，跨包，live 轮顺带实证）

引擎 `PromptManager.build_enhanced_prompt` 对装配壳（`openclaw-agents/kidpal-dynamic-context.v1.txt`）做 jinja2 单遍渲染、逐轮仅替换 `{{current_time}}`——`kidpal-agent-prompt.txt` 中的 `{{today_date}}` / `{{today_weekday}}` / `{{lunar_date}}` / `{{local_address}}` / `{{weather_info}}` 等占位**可能以字面形式进入 live 系统提示词**（未被任何渲染缝消费）。本包的 `{{schedule_summary}}` 占位已由 `connection.py` `_kidpal_apply_schedule_summary` 填充、不受此影响。K5-E4 live 轮（human 部署轮）顺带实证；若确认属 P1 收尾项，另批小修——候选落点 = `connection.py` `change_system_prompt` 同缝（占位替换先例即本包），本包不扩面、不动 canonical docs。

### 注记 2 · 单元层口径（诚实申报）

requiredLayers 含 unit；builder 以既有套件零新增红承载 unit 层（K1 同款先例）：unit 面无新增测试文件，分支/边界行为（字节预算截断首日超预算整行裁、备注回查 weekly/onetime 双表、帧迟到/二次到达回填等）由 builder 一次性探针覆盖、未持久化为断言。final bearing r2 的 unit 闸实测 220 passed / 5 subtests（闸口径，13 件 unit 文件全档零红），与 builder 申报的 115 passed 存在计数口径差异，以 final fact（`verifier-evidence/rounds/r2.json`）为准。schedule.py 单源符号既有覆盖 = `server/tests/test_schedule_producer_units.py`（圈定清单在册、final 跑绿）。
