# Review Packet · 261024-memory-recall-tool-enable

> integrator 合并前审查包，2026-10-24，final round 2（invocationId `4573a4d9-ef02-4441-9390-155abadb3866`）。等待 supervisor 裁断；不 push、不 merge。上游 = architect 分析件 `delivery/review/261022-arch-review-device-memory-prompt-overhaul/k4-recall-architecture-20261024.md` §0 / §B 选项 3 / §E + human 2026-10-24 四裁决（P0：放行 `honcho_search_conclusions` + AGENTS.md 记忆回忆回合纪律）。story 语义权威 = `docs/requirements.md` v1.67 现文。

## 1. Change Size

| 维度 | 数字 |
|---|---|
| 声明工件 | `server/deploy/261022-openclaw-kidpal-tools/kidpal-agent-tools-declaration.json` +6/−3（allow 四项→恰五项含 `honcho_search_conclusions`；drift_guard 由 `expect: "unset"` 改为 `expect_any: ["unset", {"allow": [四项]}]` 升级基准；`_note` 换代留痕） |
| apply 脚本 | `server/deploy/261022-openclaw-kidpal-tools/apply_agent_tools.py` +122/−29（双形态：直写语义零回退 + `--gen-patch` 产物生成器；`drift_problems` 扩 `expect / expect_any` 恰含其一；`_force_utf8_stdio` 跨平台输出稳定） |
| 提示词镜像 | `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md` +3/−0（回合路由表新增「记忆回忆回合」一行 + 记忆节末新增「记与查旧事是两件事」一段两行；既有内容零删改） |
| 冻结验收件 | `server/acceptance/test_261022_openclaw_kidpal_tools.acceptance.py` +678/−109（K1-E4 锚显式换代四项→五项；新增 drift 升级基准 / 四项基线可升级 / gen-patch 产物形状与零 config 写入三件；新增 K1-E4-B 三条 AGENTS.md 静态语义与锚兼容断言；其余五态断言仅随目标态同步期望值，方法体零语义回退） |
| 回滚脚本 | `rollback_agent_tools.py` **零 diff**（manifest 登记但语义与项数无关：按 backup 文件整文件还原，见 §4 第 7 条） |
| 投影 | `delivery/taskpackages/current/` 两件（taskpackage.json 指针换代 +40/−38、verdict.json final r2 投影 +27/−27，由 verify.mjs 程序 fs-write） |

已跟踪文件合计 **876 插入 / 206 删除（6 文件）**；另有 TaskPackage 目录新增制品（taskpackage.json / plan.md / input-brief.md / pod-events.jsonl / test-scope-final-r2.txt / verify-runs.jsonl / verifier-evidence/rounds/r1.json、r2.json / 本 packet / docs-delta.md / verdict.json / verification.json）。零新增第三方依赖（stdlib-only）；零固件 diff（crossEnd exempt：设备端零感知）；零数据迁移。

## 2. Verification Evidence

- **scoped round 1**（invocation `cf638afa-b0d2-4dff-8896-0bec74bb193b`，15:40:14.292Z–15:40:17.005Z）：`ok:true`、`observedGateExit=0`、8 闸绿——lite 包 `firmware-build` scoped 相位后置（261015-gov-review-lite-fast-path），fact = `verifier-evidence/rounds/r1.json`。
- **final bearing round 2**（invocation `4573a4d9-ef02-4441-9390-155abadb3866`，15:40:21.530Z–15:40:40.190Z）：`ok:true`、`observedGateExit=0`、**10 闸全绿**（taskpackage-required / change-manifest / firmware-build〔真编译，xiaozhi.bin 余量 9%〕/ contract-lint / acceptance-coverage / mutation-check〔豁免声明在册：`mutationSkipReason` 声明/提示词/验收锚换代的 Lite 包，`mutationEvidence.perAcceptance` 空〕/ docs-sync / security / unit / acceptance），fact = `verifier-evidence/rounds/r2.json`。
- **fact 校验**：`node .claude/scripts/verifier-evidence-check.mjs --change-id 261024-memory-recall-tool-enable --phase final --round 2 --invocation-id 4573a4d9-ef02-4441-9390-155abadb3866` → `factOk:true`、exit 0（integrator 复跑确认）。
- **test-scope 清单落盘路径**（Step 4 ① 圈定锚行）：`delivery/taskpackages/261024-memory-recall-tool-enable/test-scope-final-r2.txt`，四行 = 本包 `acceptanceTestsFile` 一件 ∪ `impactedTests` 种子去重后三件（K1 载体迁移 / honcho_memory / 261023 静态语义），与工作树实际 diff 的四件逐一对应。**留根注记**：r2 fact 的 `results` 元数据未含 scope 路径行（verify.mjs 未把 `--scope-file` 透传落 fact），路径以本 packet 与 `verification.json` 的 `finalScopeFile / finalScopeNote` 承载；该差集观察锚同时留在 scope 文件自身。
- **圈定交集实测（r2 final）**：unit 闸 **16 passed / 0 failed**（`structuralCollected=16`，本包无独立单测文件，结构收集即冻结验收件十六项）；acceptance 闸 **16 passed / 0 failed**（`serverAcceptanceFiles=1`，零 live 标记 deselect）。
- **全量口径（integrator 独立复跑，2026-10-24）**：`cd server && python -m pytest -q --import-mode=importlib -m "not live" acceptance` = **346 passed / 7 deselected / 33 subtests passed（28.07s）**。对照冻结时点：冻结前同树基线 340 passed / 7 deselected / 33 subtests；冻结后设计态红 = 346 收集 / 339 passed / 7 failed / 7 deselected / 33 subtests。现在七条设计态红全转绿、既有 339 条零回退，数字逐项自洽。
- **gen-patch 产物独立复核（integrator 实跑）**：`python server/deploy/261022-openclaw-kidpal-tools/apply_agent_tools.py --gen-patch <tmp>/kidpal-tools.patch.json` → rc 0，产物为嵌套树 `{"agents":{"entries":{"kidpal":{"tools":{"allow":["read","write","exec","web_search","honcho_search_conclusions"]}}}}}`，展平后恰一个叶键 `agents.entries.kidpal.tools.allow`，值 = 声明五项；未触碰任何 config、未建备份（冻结断言 `test_k1_e4_gen_patch_product_exactly_target_key_and_zero_config_write` 在验收件内另以字节比对与「config 不存在仍可生成」两态承载）。
- **round 历史**：r1（scoped，ok:true，8 闸）→ r2（final，ok:true，10 闸全绿）。无红轮、无 fixer 返工轮。

## 3. demo 对账行（workstream 包增锚 ①）

plan.md 的 demo 证据形态声明 =「host：验收改代后全量套件绿（基线 340）+ gen-patch 产物断言；live：主线程按命令单 patch --dry-run → --file → 真机问答 + 日志工具调用实证」。逐项对账：

| plan 声明 | 本次实证形态 | 一致性结论 |
|---|---|---|
| host 面：冻结件真跑 | r2 unit 闸 16 passed / acceptance 闸 16 passed（同一冻结件两闸各真跑一次，子进程真跑 apply / rollback / gen-patch 三形态）；冻结时点七条设计态红全部转绿 | 一致 |
| host 面：全量套件绿（基线 340） | integrator 独立复跑全量 `-m "not live"` acceptance = 346 passed / 7 deselected / 33 subtests（28.07s）；冻结时点同命令为 346 收集 / 339 passed / 7 failed / 7 deselected（七条设计态红），现七红全转绿、冻结时点已通过的 339 条零回退；相对冻结前 340 passed 基线净增 6 条通过 | 一致 |
| host 面：gen-patch 产物形状 | rc 0、JSON 子集可被 stdlib 解析、展平恰一个叶键、五项含 `honcho_search_conclusions`、零 config 读写与零备份（本 packet §2 独立实跑 + 冻结断言双层） | 一致 |
| live 面：`openclaw config patch --dry-run` 接受产物 | **未发生（按设计）**——OpenClaw 自有 schema 校验只能在 ECS 跑；归 human 部署轮，命令单见 §5 | 一致（申报为未覆盖面，§4 第 1 条） |
| live 面：`openclaw.json` 四项态实升级 | **未发生（按设计）**——drift 升级基准的真实性只能部署轮实读；offline 只证声明接受该基线、apply 在四项 fixture 上升级 rc 0 并留下四项前态备份 | 一致（§4 第 2 条） |
| live 面：真机「王一诺是谁」答出 9/30 事实 | **未发生（按设计）**——行为观测归部署轮 | 一致（§4 第 3 条） |
| live 面：日志出现工具调用 | **未发生（按设计）**——日志观测归部署轮 | 一致（§4 第 4 条） |
| 锚兼容红线（三方既有冻结件继续绿） | K1-E1 回合路由五行全锚（孩子对话回合行禁工具锚组仍在）、K3_P1_AGENTS_ANCHORS 六串逐字、261023 A3 记忆节四锚 + 对象锁定行逐字——全部由本包冻结件 `test_k1_e4_b_anchor_compat_no_regression` 自含副本断言，并由全量 346 绿中的兄弟冻结件二次确认 | 一致（零回退） |
| 记忆节原句保留 | 「你不需要写任何文件、也不调用任何工具去记」逐字在场；检索澄清为加法新段，不改原句 | 一致 |

## 4. 未覆盖面申报（workstream 包增锚 ②）

对照 `acceptance[]`（K1-E4 / K1-E4-B）与 §B 选项 3 的验收口径，逐条一行：

1. **live：`openclaw config patch --file <gen-patch 产物> --dry-run --json` 接受产物**——零 host 断言（OpenClaw 自有 zod strictObject 校验只能在 ECS 跑；offline 只证 JSON 子集形状与叶键集），归 human 部署轮命令单 §5 第 2 步。
2. **live：真实 `openclaw.json` 四项态实升级（drift 升级基准的真实性）**——零 host 断言；offline 只证声明接受「键缺席 / 四项覆写」两基线、apply 在四项 fixture 上升级 rc 0 且备份内容 = 四项前态；真实现状以部署轮第 0 步只读实读承载。
3. **live：真机问「王一诺是谁」答出 2026-09-30 事实**——零 host 断言（行为观测，验收件头注缺口节显式申报）。
4. **live：日志出现 `honcho_search_conclusions` 工具调用且无「记不清」自述**——零 host 断言（同上）；命令单给出主判据与日志格式不符时的退一步口径（Honcho 侧请求日志时间窗对账），两种口径都无证据即按未生效回报，不臆判。
5. **跨包陈旧基线（不在本包 changeManifest，本包不改）**：`server/deploy/261022-openclaw-honcho-plugin/honcho-plugin-declaration.json` 的 drift_guard 仍是 `{"path": "agents.entries.kidpal.tools.allow", "expect": ["read","write","exec","web_search"]}`，且该四项被 `server/acceptance/test_261022_memory_isolation.acceptance.py:430`（`K3_P1_TOOLS_EXPECT`）逐字钉死。本包落地并 live 升级到五项后，重跑 `apply_honcho_plugin.py` 会因 drift 不中 **exit 3（零写）**——属「绿着的陈旧基线」，不是本包缺陷。部署轮顺序约束：若还需重跑 honcho 插件 apply，必须先于本包升级；建议 follow-up Lite 包把该基线扩为「四项或五项」二选一并同步 `manifest.yaml` 注释。
6. **`manifest.yaml` 陈旧字面（不在本包 changeManifest）**：第 251 行 `note` 写「恰四项 read/write/exec/web_search」、第 313 / 317 行 honcho 条目写「tools.allow 四项已应用态」，`apply_honcho_plugin.py` 头注同形——纯注记陈旧、零机械断言失效（冻结断言只核三条 manifest 登记的 src/dst 结构与 dst 前缀）；建议随第 5 条同批换代。
7. **回滚零换代（语义与项数无关）**：`rollback_agent_tools.py` 不读声明项数，按 backup 文件整文件还原；因此「四项→五项」不改变回滚语义，也无需改脚本。暴露出的运维面：时间戳备份只由**直写形态**产生，而 live 首选 **patch 形态**（零直写路径保留）——patch 形态不自带备份，故命令单第 3 步要求人工 `cp -a` 一份文件名对齐声明 `backup.pattern`（`openclaw.json.bak-kidpal-tools-<UTC 时间戳>`）的副本，使回滚脚本的默认「取最新匹配件」入口继续可用；备选回滚 = 反向 patch（需人工编辑或临时声明文件，因为 `--gen-patch` 恒产声明目标态）。
8. **direct-write 形态在真实 live 文件上不跑**：`openclaw.json` 若含 JSON5 注释，`json.loads` 会 exit 2 零写（fail-closed）；直写形态仅作离线核对与既有回退路径，本批不在 live 实跑。
9. **模型回合判定精度**：提示词措辞本身即交付物，冻结断言只钉语义锚组在场与负锚不出现，不断言句式；模型实际是否按「记忆回忆回合」触发归 live 观测（§5 第 7 步，见 §6 finding 1）。

**零缺口项：none**——除上述已逐条申报的 live 面与跨包/注记面外，K1-E4 / K1-E4-B 的 host 断言全部在冻结件内真跑转绿，无其它未覆盖面。

## 5. live 命令单（human 授权轮照敲；Pod 不执行）

> 前置事实：`/opt/kidpal/openclaw-patch/261022-openclaw-kidpal-tools/` 三件由 deploy.sh 按 manifest md5 铺放（本包不改 manifest，登记已在册）。以下每步给判据与停行；任何一步失败即停并按第 3 步备份回滚，不就地改配置凑结果。

```bash
# 0) 前置只读实读（判据：键缺席（新机）或 {"allow":[read,write,exec,web_search]}（live 现状））
openclaw config get agents.entries.kidpal.tools
openclaw plugins inspect openclaw-honcho --runtime
#    判据：Status: loaded，且工具列表含 honcho_search_conclusions / honcho_search_messages（放行前者、后者仍须被 allow 挡掉）
#    停行：tools 键在场但形状不是 object，或含 allow 之外的多余键 → 停，回报（直写形态会 fail-closed，
#          patch 形态需人工判断合并面）
#    顺序约束：若 261022-openclaw-honcho-plugin 还需重跑 apply，必须在本步之前完成
#          （其 drift_guard 锚四项，升级到五项后重跑会 exit 3 零写，§4 第 5 条）

# 1) 生成 patch（host 已验形状；产物叶键恰一个）
python /opt/kidpal/openclaw-patch/261022-openclaw-kidpal-tools/apply_agent_tools.py \
  --gen-patch /tmp/kidpal-tools.patch.json
#    判据：rc 0；产物 stdlib json 可解析；展平叶键恰 {agents.entries.kidpal.tools.allow: [五项]}
#    停行：rc != 0 或叶键集不符 → 停，回报

# 2) dry-run schema 校验（OpenClaw 自有写入口校验）
openclaw config patch --file /tmp/kidpal-tools.patch.json --dry-run --json
#    判据：回执 checks.schema = true（schema 通过）
#    停行：schema 非 true / 非零退出 → 不改配置，回报

# 3) 人工备份（patch 形态不自带备份；文件名对齐声明 backup.pattern，回滚脚本可直接消费）
cp -a /root/.openclaw/openclaw.json \
  "/root/.openclaw/openclaw.json.bak-kidpal-tools-$(date -u +%Y%m%dT%H%M%S)"
#    判据：备份文件存在、字节数 = 原文件

# 4) 正式写入 + 配置校验
openclaw config patch --file /tmp/kidpal-tools.patch.json
openclaw config validate
#    判据：两条均 rc 0
#    停行：任一失败 → 立即回滚（见本步末），不重启网关
#    回滚（dry-run 先看意图，再真还原；--backup 缺省取最新匹配件）：
#      python /opt/kidpal/openclaw-patch/261022-openclaw-kidpal-tools/rollback_agent_tools.py \
#        --config /root/.openclaw/openclaw.json --dry-run
#      python /opt/kidpal/openclaw-patch/261022-openclaw-kidpal-tools/rollback_agent_tools.py \
#        --config /root/.openclaw/openclaw.json
#    备选回滚（反向 patch）：人工编辑出 {agents.entries.kidpal.tools.allow:[四项]} 的 patch 文件，
#      先 --dry-run --json 核 schema:true，再 --file 正式落；随后 config validate + 重启 + health

# 5) 重载并健康核对
systemctl restart openclaw-gateway
openclaw health
#    判据：health 正常；journalctl -u openclaw-gateway 无 exit 78 / zod strictObject 拒收 / 崩循环
#    停行：任何崩溃迹象 → 按第 4 步回滚命令还原后回报

# 6) 工具面 audit 恰五项
openclaw config get agents.entries.kidpal.tools.allow
#    判据：恰 [read, write, exec, web_search, honcho_search_conclusions]
journalctl -u openclaw-gateway --since "-10 min" | grep -F "No callable tools remain"
#    判据：升级并重启后无新增该行（升级前存在属预期）
#    停行：项数不符或仍有该行 → 停，回报（不自行改配置凑数）

# 7) 真机问答 + 日志工具调用
#    孩子端问：「王一诺是谁」
#    判据：答出 2026-09-30 的事实（小伴早上出卖王一诺那一句所对应事实），语音不出现工具名/内部流程；
#          且不是「记不清了」
journalctl -u openclaw-gateway --since "-10 min" | grep -F "honcho_search_conclusions"
#    判据：出现该工具的调用记录（时间窗覆盖本次提问）
#    退一步口径：若网关日志行不含工具名，改用 Honcho 侧请求日志核对时间窗内
#          POST /peers/owner/representation 的出现与条数（advisory §A.5 端点族）
#    停行：答「记不清」或两种口径都无检索证据 → 停，回报（回合判定可能未命中，
#          按 tighten-only 收敛 AGENTS.md 识别特征，不在部署轮改代码）
```

## 6. Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | AGENTS.md 回合判定 | 「记忆回忆回合」的识别特征（记不记得 / 我以前说过 / 某某是谁）与「实时事实回合」靠语义边界判断，存在误判空间（例如问一个不在记忆里的新人名可能被当成旧事回忆） | changed-code hunk：`AGENTS.md` 回合路由表新增行；验收件头注缺口节 `:199-202` 明示「提示词只钉语义锚组、模型是否照路由执行属 live 观测项」；judgement heuristic | 部署轮观察触发精度；若误触发或漏触发，按 tighten-only 收窄/补强识别特征措辞，不改代码面 |
| NIT | judgement-call | drift 基线语义 | `expect_any` 的 object 候选按**值相等**比对 `agents.entries.kidpal.tools`；live 若在该对象上带 `allow` 之外的键，直写形态会 fail-closed exit 3（不是静默覆盖） | changed-code hunk：`apply_agent_tools.py:95-145`；验收件头注行为等价假设 `:203-213` 已申报 | 不改（设计意图）；直写形态使用前先实读现状；patch 形态不读 config、不受此限 |
| NIT | judgement-call | 可运维性 / 跨平台 | `rollback_agent_tools.py --help` 在 Windows 输出被重定向时抛 `UnicodeEncodeError`（中文帮助文本落到 cp1252）；`apply_agent_tools.py` 已加 `_force_utf8_stdio` 但回滚脚本未跟 | changed-code hunk：`apply_agent_tools.py:148-155`（本包新增的 UTF-8 兜底只落在 apply）；integrator 本机独立实跑 `rollback_agent_tools.py --help` → exit 1 | 不阻塞（ECS/Linux UTF-8 环境等价正常；冻结断言覆盖 dry-run/还原两条实际路径）；可随第 5 条 follow-up Lite 顺手加同款三行兜底 |
| FYI | judgement-call | live 写入路径与 drift | live 首选 `--gen-patch` 形态按冻结契约**不读 config**，因此 drift_guard 在 patch 路径上不参与判定；升级合法性由 OpenClaw 的 `config patch --dry-run` schema 校验承担 | changed-code hunk：`apply_agent_tools.py:197-208`；冻结契约 §2「不读 config、不写 config、零外部命令、零备份」 | 命令单第 0 步用只读实读补上现状核对；不改代码 |

无 BLOCKING、无 SHOULD、无 hard-violation。结构闸（verify.mjs 10 闸）的 PASS/FAIL 不复述为 finding；gate 全绿仍完整执行五轴语义 review（见 §7）。

## 7. Five-Axis Review

1. **Correctness**：K1-E4 明示换代（四项→恰五项，含负锚「`honcho_search_messages` 不得放行」）；K1-E4-B 把「记忆回忆回合」的识别特征、先查一次、查到自然转述、查不到才说记不清、过渡语、与孩子对话回合禁工具的关系六面钉进同一行。冻结时点七条设计态红全转绿，既有 339 条零回退。drift 升级基准让「live 四项态可升级」从不可达变为可达（旧声明在 live 上会 exit 3），这正是本包要解决的直接症状。
2. **Test quality**：全部断言直读仓内制品 + tempfile fixture，子进程真跑 `apply`（首次应用 / 幂等二跑 / 四项基线升级 / 三种真漂移 exit 3 零写零备份 / 时间戳备份 / 只改一个键与 gateway.token、skills 组合态、其它 agent、modelPolicy 零触碰）、`--gen-patch`（叶键集恰一个、字节相等零写、config 缺失仍可生成）与 `rollback`（dry-run 零写 / 一步还原）。负向断言齐（六项态、结构畸形的 tools、原始消息面工具不得出现在 allow 与回合行）；既有锚兼容用自含副本断言，不 import 兄弟冻结件（避免跨件焊死）。mutation 豁免在本包成立：声明/提示词/验收锚换代，冻结断言已把形状与语义逐项钉死。
3. **Contract/boundary safety**：`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md` 零 diff（docs-delta 零 diff 声明）；AGENTS.md 为加法改动，K1 五行 / K3 六串 / 261023 A3 四锚与对象锁定行逐字保留；声明工件 schema 的 `expect | expect_any` 是对既有断言面的加法扩展（原 `expect` 语义逐字保留），不是替换；跨端消息、帧类型、字段与序列化形态零变化（crossEnd exempt）。
4. **Operability/security**：零新增出网（检索走插件既有 memory 工具面到本机 loopback Honcho `127.0.0.1:18100`）；工具面只放一个、且明确不放原始消息面；家长隐私负向锚（不记、不回忆）逐字保留，新增「不逐字复述、不带内部流程」的转述纪律；live 写入口只经 `openclaw config patch`（K3 崩因教训写进脚本头注），直写形态保留备份与一步回滚；exit 码 0/2/3 语义分明；输出 UTF-8 兜底避免 Windows 捕获干扰。零数据迁移、零库写、零密钥入仓。
5. **Maintainability/simplicity**：改动局部（apply 脚本 +122 行，三段新增函数各自单一职责）；无新依赖、无新抽象层、无跨模块引入；`build_patch_tree` 复用既有 `_json_set`，没有为「以后可能的 patch 形态」预造配置项；验收件扩 678 行主要是显式换代留痕与自含锚副本，属冻结件本身的信息密度，不是实现面复杂度。无 `minimal:` 标记新增。

## 8. Simplification Debt（`minimal:` 台账）

grep `minimal:` 在本次全部改动文件（声明、apply、AGENTS.md、冻结验收件）零命中——本包未新增简化债务，无待登记条目。**no-trigger。**

（承接说明：`apply_agent_tools.py` 的直写形态注释已自注「保留为离线核对与既有回退路径」，属已声明的存量形态选择，不是本包新造的简化债。）

## 9. Residual Risk

- **`decisionCoverage` 筛选**（`landsIn === 'dropped' && reason.startsWith('risk-accepted')`）：**none**——本包 taskpackage.json 无 `decisionCoverage` 字段；human 四裁决全部落进 exitCriteria 与 notes，无 risk-accepted dropped 条目。
- **Known limitation**：① live 四项（dry-run 接受产物 / 四项态实升级 / 真机问答 / 日志工具调用）零 host 断言，只能部署轮首验；② 跨包陈旧基线（`261022-openclaw-honcho-plugin` drift 四项）与 `manifest.yaml` / `apply_honcho_plugin.py` 的「四项」字面，本包不改；③ 直写形态的 drift 值相等判定在 live 带多余键时 fail-closed（设计意图，需实读先行）；④ patch 形态不评估 drift，合法性靠 OpenClaw schema；⑤ 回滚脚本的默认入口依赖时间戳备份，patch 形态需人工补一份（命令单第 3 步）；⑥ 模型回合判定精度只在 live 可见。
- **Follow-up recommended**：一个 Lite 包换代 `261022-openclaw-honcho-plugin` 的 drift 基线为「四项或五项」二选一，并同步 `manifest.yaml` 第 251 / 313 / 317 行与 `apply_honcho_plugin.py` 头注的「四项」字面；顺带可给 `rollback_agent_tools.py` 补同款 UTF-8 输出兜底（finding 3）。
- **Human decision needed**：部署轮授权与执行时机；以及「跨包陈旧基线」是排 follow-up 还是显式接受。
- **停等位**：`awaiting-human:live-deploy`——live 四项归 human 部署轮，命令单见 §5；Pod 不自行上 ECS。

## 10. 集成执行回报（Merge-Chain Execution 节）

未触发——本包无 merge-driver 冲突 abort 派单，未执行 rebase / 冲突消解；集成树复跑清单 N/A。

## 11. UI design-review 证据

N/A——本包是云端配置声明 + 工作区提示词镜像 + 验收锚换代，无 UIUX 面；仓内不存 screenshot / video / HTML 证据。

## 12. 架构 / 依赖卫生

- **零新增第三方依赖**：apply 脚本纯 stdlib（json / argparse / pathlib / datetime）；验收件仅用 PyYAML（既有 manifest 断言同一解析器）。
- **无跨层调用 / 无仓内 importer**：`apply_agent_tools.py` 与 `rollback_agent_tools.py` 是独立 CLI，仓内唯一引用者是冻结验收件（按路径以子进程调用），无 Python import 面。graph 复核（`detect_changes`，project `C-AI-KidPal-pod-m2`，base `main`，merge_base `3ffd095d`）：changed_files 12、seed_symbols 40、**impacted_total = 2**，两条都落在被改验收件自身的方法内（`server/acceptance` 模块计 2），production 代码零调用方——爆炸半径自包含，与 test-scope 清单四件一致。
- **无循环依赖**：单文件脚本 + 验收件，无环。
- **无绕过 contracts 的本地类型**：零契约面触碰；声明工件是无 secret 的部署配置，不是接口类型定义。
- **adrRequired=false 成立**：技术权威 = advisory §B 选项 3 + human 四裁决，本包是既有裁决的实现面落地与锚换代，无新架构裁决需要归档 ADR。

## 13. 制品与 commit 状态

- closeout 制品：`docs-delta.md`（零 diff 声明）+ 本 packet + `test-scope-final-r2.txt`（圈定清单）+ `verdict.json`（final round 2 fact 晋升投影 = `verifier-evidence/rounds/r2.json` 逐字副本，supervisor-compatible shape）+ `verification.json`（advisory 投影：finalScopedRun r2 + priorRounds r1）——均落 `delivery/taskpackages/261024-memory-recall-tool-enable/`。
- workstream 记忆：`delivery/workstreams/device-memory-overhaul/progress.md` append 本包收口节。
- commit：`dev/pod-m2` 分支（实现面四件 + 冻结验收件 + `delivery/taskpackages/current/` 投影 + 本包全部制品），**不 push**；`.codebase-memory/` 两件不入 commit。
- 后续门：supervisor-verdict-gate（planner 跑）→ `pre-merge-escalation-gate` → `merge-driver --pod`（planner 主线程）；live 部署轮独立于合并门，归主线程经 human 授权（命令单 §5）。
