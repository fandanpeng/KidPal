# Review Packet —— 261011-p1-home-v5-rework

> integrator 制品轮产出（2026-10-12）。状态：两面全绿 + 契约门 CLOSED（LANDED）+ closeout 制品齐，feature 分支已 commit 待合并门；final bearing verify（scoped r2 之后）由 planner 以本包 test-scope-final.txt 为 scope-file 跑，本件不代判。

## Change Size

- Files changed（实现 + 文档面）: 17（firmware 11 + server 3 + docs 3），+1180 / −247；另 delivery 制品面（taskpackage / current 投影 / pod-events）与 build 产物 bin 随包。
- Scope: 跨固件/云端两子面 + 契约同步——板端 preview 数组解析与三态 seam（home_p1.h/.c、home_upcoming.h/.c）+ producer 多项化（schedule.py）+ 三份冻结验收件修订（R3 授权链）+ units 齿 + spiffs chip「明早」→「明天」重烘 + 契约 §1.3⑥ 四触点 + V5 母版三处 human 直落。
- Split recommendation: 不拆——契约/固件/云端/齿面同批原子变（spine q1 裁定：拆包反制造端云中间态红窗口）。

## Verification Evidence

- verifier-round fact（bearing）: `delivery/taskpackages/261011-p1-home-v5-rework/verifier-evidence/rounds/r2.json`（phase=scoped, round=2, invocationId=0db6ce8f-d87b-4c5d-bb73-24bb7a621e5e, ok=true, observedGateExit=0）——八闸全 PASS（taskpackage-required / change-manifest pre-cutoff 存量放行 / firmware-build 真编译 / contract-lint / acceptance-coverage / mutation-check / docs-sync / security）。
- **影响面圈定终验（integrator 2026-10-12 实跑）**：
  - server 全量 `python -m pytest -q --import-mode=importlib -m "not live" tests acceptance` → **236 passed, 4 deselected, 9 subtests passed**（exit 0；留痕 `/c/tmp/hv5_integ_server_pytest.txt`）——与预期 236 passed 精确一致。
  - firmware host 全量 `wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build`（ninja no work to do = r2 构建现行）→ `./build/host_infra_test.elf` → **155 Tests / 0 Failures / 0 Ignored, OK**（exit 0；留痕 `/c/tmp/hv5_integ_host_build.txt` + `/c/tmp/hv5_integ_host_run.txt`）——与预期 155/0 精确一致。
- test-scope 清单落盘路径（final verify --scope-file 消费）: `delivery/changes/dev/261011-p1-home-v5-rework/test-scope-final.txt`（detect_changes 16 impacted ∪ acceptanceTestsFile/impactedTestsSeed 种子合成；本行已按留根纪律记入，供 final fact results 元数据比对「圈定 vs 事后全 blast radius」差集）。
- 影响面核对记账（Step 9b advisory，范围判定归 test-scope 清单）: detect_changes（Pod 专属图索引 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`，base=main，merge_base 1660248，inbound depth=2）= 53 seed symbols / 16 impacted，全落预期面——firmware/main 7（三冻结验收件 + home_p1 render_frame + screenshot + archive happy_flow_capture）、server gateway 4（front_app 三推送点 + ws_gateway）、server tests/acceptance 5（units + TECH-260928 school_start 同参齿 TechA1/A2/A3）。**零跨模块越界、零预期外符号**；front_app 三推送点与 TECH-260928 齿为 blast radius 命中（既有在役消费方被影响、非被改动），两面全绿实证零回归。Pod 专属图索引用常（launch 面必办项在位），**无降级**。
- 核证面零 diff 实证（exitCriteria③）: front_app.py / schedule_push.py / display_schema.py / mcp_server.cc / 其余四件 server acceptance / 归档件 `git diff` 零命中（integrator 实证）。
- spec-anchor-check 复跑（pod-events b7e2d94a 裁 Pod 侧核对归 Pod）: ok=true，designIdCount=60 不变、errors 零（2026-10-12 integrator 实跑）。
- 契约面: contract-lint PASS（r2）；CCP §3 逐字核对 = gate evidence §11 C1 节（净内容 9/9 行逐字一致 + 2 处呈现性差异如实登记；CRLF 保全字节级实证）。
- mutation check: PASS（r2；五条 perAcceptance 变异全 turnedRed，变异已还原复原绿）。
- final bearing verify: **pending-planner**（`verify.mjs --phase=final --scope-file delivery/changes/dev/261011-p1-home-v5-rework/test-scope-final.txt` + verifier-evidence-check + verdict.json/verification.json 投影产出——机器产线件程序 fs-write，integrator 只圈定不代跑）。

## demo 对账行（pod-coordination §11，与未覆盖面申报同址正交）

- **demo 证据 = 本轮两面实跑输出 + app/art 产物构建留痕**：① server producer 面全量 236 passed（含 M3K1-A3 frozen shape 齿 + preview 多项形状齿 + 空数组齿）；② firmware host 全量 155 Tests / 0 Failures（HV5-AC2/AC5/AC6/AC7/SEAM 新齿全绿 + 既有套件零损失）；③ r2 firmware-build fact 真编译留痕——xiaozhi.bin 0x311e50 bytes（app 分区 11% 余量）、art.bin spiffsgen 0x80000 重打包（含新 chip 资产）、esptool write_flash 命令三分区 0x20000（app）/ 0x780000（art）/ 0x800000（generated assets）；④ 重烘后全量 bin 仅 chip 一枚 diff（exitCriteria④，builder 报告留痕）+ p1_label 同批核通过。与本包效果段（spine ①②③④）一致性结论：**一致**——空态两态分叉、明天区多项化、chip 定名、契约四触点各有在世齿承载且全绿。
- **真机看屏轮 = human 持板硬前置（未跑，如实标注 pending-human）**：看屏场景（空态两态 / 明天区多项 / chip「明天」上屏）无机器证据替位；app+art 双分区烧录 + human 目击为 exitCriteria⑤ 承载位。

## 未覆盖面申报（对照 acceptance[] + 契约条目 + spine 效果段）

- 真机看屏轮（空态两态上屏 / 明天区多项行流 / chip「明天」渲染目击）——pending-human（exitCriteria⑤；host 像素级齿 + screenshot 件为结构替位，非真机目击）。
- ECS front 部署（端云同批部署防 R2 错配窗口，CCP Impact 实测链路行）——pending，归 planner/主线程部署位，未在本包轮执行。
- 板端旧单对象过渡容忍的线上验证（CCP 触点②部署缓冲的真实旧载荷场景）——host 齿已绿（旧单对象 → 1 项列表），线上旧 server 行为共存窗验证 pending（真机轮 + ECS 部署后自然覆盖）。
- 其余 acceptance 条目（HV5-AC2/AC5/AC6/AC7/SEAM host+server 齿）与契约条目（四触点 + C1/C2/C3）：已覆盖，零缺口面 = none（除上述三项 pending 外无未申报面）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （`minimal:` 标记 grep 全改动文件 = 零命中，无台账条目） | — | — | — |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | 派单/scope-map | 派单把 CDQ append 列入 integrator 义务，但 scope-map integrator allow 面不含 `docs/CONTRACT-DECISION-QUEUE.md`——Edit 被 enforce-scope 钩子拦截，CDQ 行全文备妥交 planner 代落（内容零缺、通道错位） | changed-process hunk（本包派单「你的职责 2」vs enforce-scope.mjs 实拦回执）；机械闸优先 heuristic——非契约/spec 字面违反，不引规则前缀 | planner 代落 CDQ 行（文本见 docs-delta 指向的交办节）；后续派单把 CDQ append 归 planner 或扩 scope-map（后者须 human） |
| NIT | judgement-call | CCP §3 文本 | CCP §3 变更记录行 -/+ 两行漏行首 markdown bullet「- 」，落地版保留 bullet（与 2026-09-21 先例行同构）；packet 与落地文本存在笔误级呈现差，已在 C1 差异清单如实登记 | changed-artifact hunk（CCP :81-:82 vs 契约 :159）+ 逐字精确 heuristic | 无（已随 gate evidence §11 留痕消解） |
| NIT | judgement-call | crq/tech | ARQ-260907-contract-1-5-strip-wording 的「下次契约门」trigger 被本批字面触发，但 §1.5 校准不在本批批准范围——已 append 留痕维持 deferred，存在「trigger 触发却未处置」的账面张力 | changed-artifact hunk（ARQ-260907 append 段）+ 记账诚实性 heuristic | 无（顺延下次含 §1.5 触点的契约门；已留痕） |

结构闸 PASS/FAIL 不复述为 finding（gate 全绿仍完整执行五轴语义 review，见下）。无 hard-violation finding——如出现将以 `docs/contracts/**` / `docs/requirements.md` / `docs/architecture.md` 字面前缀起引 Basis；本轮 none。

## Five-Axis Review

- 安全：零新增攻击面——wire 形状演进走既有 display 块通道与鉴权建连序列；producer 单源纪律不变（front schedule.py 机械计算，ADR-004 延续）；旧单对象过渡容忍是**放宽解析**面，仅接受既有在发形状、不引入新值域（非 HH:MM time 维持拒包，home_upcoming.h 齿在世）；零新增依赖、零新增出网路径。
- 契约：四触点逐字落地并经 human 契约门（先批后落）；三态语义在契约、producer、板端三面同批同形（`[]`/null/非空数组语义齿 AC6/AC7 + producer 恒发数组不发 None）；:155 seam 引用与实现签名一致；变更记录行先例格式在场。
- 边界与依赖卫生：零新增依赖；producer→seam 注入链路无跨层调用新增（front_app 三推送点既有调用点、TECH-260928 同参齿零偏移）；板端旧访问族退役反向 canary（结构齿）防死代码残留；spiffs 资产管线（chip 重烘）走既有 gen_assets 通道，零旁路。
- 可运维性：数据未达态不清屏、换友好提示（AC7），0:00 跨天清缓存转未达态防误显话术；拒包后 seam 状态零变化（两区已有数据维持显示）——坏载荷不砸现网显示；producer 明日无项发 `[]` 语义显式，日志/排查面清晰。
- 资源与功耗：preview 定容 HP1_PREVIEW_MAX=8 + 超容整包拒，板端缓存与行流渲染面上界受控；spiffs 资产 160 字节级替换、分区余量 11% 不变。

## Residual Risk

- Known limitation: 真机看屏轮三场景（空态两态 / 明天区多项 / chip「明天」）pending-human；ECS front 部署与端云同批时序窗（R2 错配窗口）pending 部署位执行；旧单对象线上共存窗未实测（host 齿已绿）。
- Follow-up recommended: TECH-261011-spec-v5-reconcile-note-drift（requirements v1.61 内 v1.57 ⚠️ 对账注勘误，档 0 勘误批候选）；ARQ-260907 §1.5 措辞校准顺延下次契约门。
- Human decision needed: riskTier=high → supervisor 裁断后仍升 human 人批合并；真机看屏轮 human 持板为硬前置。decisionCoverage 中 `landsIn: dropped && reason: risk-accepted` 条目 = 无（本 taskpackage 未启用 decisionCoverage 字段，Residual Risk 节按模板申报零条目）。

## 集成执行回报

本轮无 merge-driver 冲突消解（单包制品轮，Pod 分支无对端冲突），集成复跑清单 n/a；圈定 floor 注（双方冻结件 ∪ 圈定清单）不适用——单包无对端冻结件，floor = 本包三份冻结验收件（schedule_grid / p1_home_battery / p1_cjk_name，均在 test-scope-final.txt 首段）。

## 交 planner 代办（本轮回执）

1. **CDQ 行代落**：`docs/CONTRACT-DECISION-QUEUE.md` 表末 append `CDQ-261011-p1-home-v5-rework` 行（行全文已拟，见下）——integrator Edit 被 enforce-scope 拦截（机械闸优先，未绕行）。
2. **final bearing verify**：`node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --scope-file delivery/changes/dev/261011-p1-home-v5-rework/test-scope-final.txt --change-id 261011-p1-home-v5-rework --round <final-round> --invocation-id <uuid>` + `verifier-evidence-check.mjs`；verdict.json / verification.json 投影由程序产出。

CDQ 行全文（planner 代落用，append-only）：

> `| CDQ-261011-p1-home-v5-rework | 2026-10-11 | interface-contract.md §1.3⑥ 四触点（preview 单对象→多项数组+三态语义 / 示例值「明早」→HH:MM 顺修 / :155 seam 引用签名 5 参同形顺修 / 变更记录行 append） | CONTRACT_CHANGE_REQUIRED（AC6/AC7 两态需不同 wire 值，null 已被占用；预审裁定表见 packet/gate evidence §3） | human 契约门 AskUserQuestion 批准 2026-10-11（packet §5，四触点一次批、无附加条件） | **LANDED**（2026-10-12 契约 diff 随包同 PR 落地；逐字核对留痕 = gate evidence §11 C1 节，净内容 9/9 行逐字一致 + 2 处呈现性差异如实登记；CRLF 保全字节级实证） | 261011-p1-home-v5-rework | 无——实现面随包落地；真机看屏轮归 human 持板（非契约门条件）；tech 记账指针 = docs/crq/tech/TECH-261011-spec-v5-reconcile-note-drift.md |`
