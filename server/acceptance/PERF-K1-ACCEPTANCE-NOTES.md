# PERF-K1 冻结验收 · 设计说明 + 红基线 + mutation 证据

> 承载包:`260924-perf-k1-roundtrip-latency`(subsystem: server)。作者 = acceptance-author(本 Pod)。
> **落位偏离如实记**:任务书要求把本文件内容 append 进
> `delivery/taskpackages/260924-perf-k1-roundtrip-latency/evidence/latency-baseline-260924.md`,并把
> 红基线落账 `taskpackage.json.redBaselineResult`;但 enforce-scope 钩子机械拦截(acceptance-author
> 写边界 = `firmware/**/*.acceptance.c(pp)` + `firmware/**/acceptance/**` + `server/**/acceptance/**`
> + `tools/**`,**不含 `delivery/taskpackages/**`**)。故本说明落在写边界内的本目录,并把两份待落地内容
> 原样交 planner(见文末「交 planner 落地件」)。机械强制层不绕过、不自行越界。
>
> 冻结件 = `server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py`(27 齿:23 offline + 4 live)。
> 覆盖 ids:PERF-A1 / PERF-A2 / PERF-A3 / PERF-A4 / PERF-E1 / PERF-E2 / PERF-E3。

## 1. 齿形态(任务书「齿形态」节三分类)

- **CI 齿只断言机械面**(时延数字不入 CI 齿——timing 齿天然 flaky):声明工件在位 + schema + 与 apply
  消费面一致;apply 脚本幂等 + drift-guard(现状锚点核对不中 → 退出码 3,STOP-on-mismatch 形制)+
  时间戳备份 + 一步回滚 dry-run;探针脚本 v6 注入 + hash 断言(渲染面==被测面,ADR-001 / 评审 D1);
  must-stay-green(provider.config.yaml LLM 段 + overlay 引擎 LLM 段 + v6 文件零回改)。
- **live 齿**(`@pytest.mark.live`,不进 `-m "not live"`):探针批结构化证据(标准 3 题 ×N≥3 中位数 +
  主机快照 + session 计数,三段归因)+ 门判定器(读证据对两层门:保底 6/9/14s、目标 4/6/10s)。
- **mutation 打在配置接线**(声明工件的 thinking 值 / model id / allow 条目 / 文件在位),不打超时判定。
- **B 主案与 A fallback 两形态皆可判**:声明工件的 `case` 字段参数化(B:`thinkingDefault ∈ {off,low}`;
  A:model ∈ allowlist 且非推理);判据恒为声明机制的机械面,不做时延断言——齿冻结在 builder 步⑤ A/B
  裁决之前,硬编 B 形态的话「B 不达标转 A」会因错误理由翻红(任务书防返工理由)。

## 2. 红基线(实测 2026-09-24,本 Pod 树)

- 命令:`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/`(仓根)。
- 结果:**18 failed / 62 passed / 4 deselected**。
- **既有齿零新增红**:62 passed = 既有 57(上游基线 `57 passed / 0 failed @1746c14`;本树实测
  `git diff --quiet 1746c14 HEAD -- server` 零差异成立)+ 本包 5 条设计态即绿齿(见 §3 表:
  provider/overlay/v6/wiring 属 must-stay-green,现树即绿)。
- 18 红全部为**设计态红**(实现工件未落盘:声明工件 / apply 脚本 / 回滚脚本 / `probe_gate.py` /
  `probe_batch.py` 均不在位),逐齿映射见 §3。
- **冻结即冒烟**(DEV-GOVERNANCE 验收测试纪律 §3):`--collect-only` 收集零错误(执行通道 error 为零);
  live 面 4 齿被 `-m "not live"` 条目级筛除 + conftest 机械 skip(ARQ-260901-g3)。本包 live 面走证据读
  形态、不 SSH,故无 SSH 线束冒烟项(理由见 §5.3)。

## 3. 逐齿映射(齿 → acceptanceRefs / 断言的真实结果 / 性质档)

| 齿 | coverage | 绑定的真实结果(不是「没报错就算过」) | 性质档 |
|---|---|---|---|
| decl_file_present_and_schema | PERF-A1 | 声明工件在位 + JSON 可解析 + 必需键齐 + case∈{B,A} | 机制级 |
| decl_case_semantics_b_and_a_validators | PERF-A1 | 校验器:合法 B/A 过;thinking='high'、primary 掉出 allow、reasoning=True、两轴混声明 均判红 | 机制级 |
| decl_drift_guard_anchors_case_field | PERF-A1 | B 锚 = `agents.defaults.thinkingDefault`=='unset'(证据 §8.1 CLI 地面真值) | 机制级 |
| apply_script_present_and_consumes_declaration | PERF-A1 | apply 脚本在位且引用声明工件名(声明面==消费面) | 机制级 |
| apply_offline_applies_then_idempotent | PERF-A1 | 真跑①现状锚匹配→变到声明目标态;②再跑→no-op + 不重备份 | 行为级 |
| apply_driftguard_exit3_on_mismatch | PERF-A1 | 现状锚不中→退出码 **3** 且零写入 | 行为级 |
| apply_timestamped_backup_before_write | PERF-A1 | 写前建时间戳备份,内容==变更前 config | 行为级 |
| rollback_dry_run_no_write | PERF-A4 | `--dry-run` exit 0 且零写入 | 行为级 |
| rollback_restores_backup | PERF-A4 | 一步还原:backup 内容写回 config | 行为级 |
| probe_prompt_resolution_version_agnostic | PERF-A1 | 探针解析出的 prompt == 声明面接线(渲染面==被测面,D1);版本无关不硬写 v6 | 机制级 |
| probe_messages_include_production_prompt | PERF-A1 | messages==[system(v6 逐字), user(题)](裸探针=非生产形态,D1/推论甲) | 机制级 |
| probe_script_check_prompt_runs | PERF-A1 | `--check-prompt` exit 0 + 打印解析出的 prompt 与归一 sha256 | 源锚代理 |
| gate_decider_median | PERF-A1 | 奇/偶样本中位数正确 | 行为级 |
| gate_decider_two_layer_a1 | PERF-A1 | 目标全达→PASS;仅保底→BOTTOM_ONLY;保底破→FAIL | 行为级 |
| gate_decider_requires_n_ge_3 | PERF-A1 | N=2→n_ok 假且 verdict FAIL(门=N≥3 中位数,评审 D3) | 行为级 |
| gate_decider_weather_requires_real_retrieval | PERF-A1 | 天气无真实检索数据→保底门红(计时再好也判红) | 行为级 |
| quality_decider_display_and_schema_rates | PERF-A2 | 缺 ```display 块→出现率<1;view 非法(过真门 `validate_display` 判红)→合法率<1;均 quality_pass 假 | 行为级 |
| quality_decider_note_readback | PERF-A2 | 课表缺备注读回→quality_pass 假(S-F1.3 AC1/AC4 不回退) | 行为级 |
| gate_decider_layer_a3_thresholds | PERF-A3 | a3 层阈值保底 8/10s、目标 5/7s 行为正确 | 行为级 |
| e1_provider_llm_segment_unchanged | PERF-E1 | provider.config.yaml LLM 段:base_url /v1、model openclaw/default、stream、forbid_max_tokens | 机制级 |
| e1_overlay_llm_segment_unchanged | PERF-E1/E2 | overlay 引擎 LLM 段:base_url /v1、model_name openclaw/default、禁 max_tokens 注记 | 机制级 |
| e1_prompt_v6_byte_hash_unchanged | PERF-E1/E2 | v6 归一 sha256 == `425d293d…cc62`(本包不动 prompt) | 机制级 |
| e1_prompt_wiring_faces_consistent | PERF-E1 | 声明面与渲染面同指一在场文件(接线不变式 ADR-001,版本无关) | 机制级 |
| live_a1_probe_batches_structured_three_segments | PERF-A1/E3 | live:`probe-batches/*.json` ≥3 段、各题 N≥3、主机快照 + session 计数在场 | 源锚代理 |
| live_a1_gate_decider_applies_to_evidence | PERF-A1 | live:判定器真跑各段证据产出合法 verdict(不崩) | 源锚代理 |
| live_a2_quality_evidence_recorded | PERF-A2 | live:质量证据(display/备注)留痕 | 源锚代理 |
| live_a4_post_restart_five_point_checklist | PERF-A4 | live:五项核验(cron/qqbot/18789/KidPal C1 marker/可达)证据齐 | 源锚代理 |

源锚:`evidence/latency-baseline-260924.md`(§1 延迟解剖 / §7 五项核验 / §8.1 thinkingDefault=unset /
§10 真机基线对照锚)、`architect-review-260924.md`(§3 齿形态裁定 / D1 探针方法学 / D3 方差受控 /
Q5 两层门校准)、`docs/requirements.md#NFR-PERF-1/2`、`docs/contracts/interface-contract.md §四`。

**强度申报**(acceptance-evidence「契约条目 ↔ 齿 的强度申报」T2)逐条写进冻结件头注「强度申报」节:
映射 / 弱于契约处的缺口 / 行为等价假设三件。核心缺口见 §5。

## 4. live 齿与 offline 判定器齿的分界

- **offline 判定器齿**(`gate_decider_*` / `quality_decider_*`):喂**合成 fixture 证据** → 断言判定
  结果。时延数字是构造值(非实测),只验**逻辑**——正是任务书「判定器逻辑本身要有 offline 齿」的落点。
- **live 齿**:读 live 相位**真实产出**的结构化批次(`probe-batches/<segment>.json`,schema ==
  `probe_gate.evaluate_batch` 入参)+ 真实证据 md,断言三段归因齐 / N≥3 / 主机快照 / session 计数 /
  五项核验在场,并对证据跑判定器。**不 auto-run**(`@pytest.mark.live`)。

## 5. 缺口与歧义(交 planner 裁决;不自裁)

1. **`taskpackage.json` 缺 `mutationEvidence` 字段骨架**:mutation-check 闸
   (`.claude/scripts/mutation-check.mjs` L122/124)要求每条 functional acceptance id 有
   mutationEvidence,而本包 taskpackage.json 无该字段(T6 §1 要求 Draft 带骨架)。acceptance-author
   写边界不含 `taskpackage.json`(只被任务书授予 `redBaselineResult` 字段,而机器边界连该字段也不含)
   ——**请 planner 补骨架**。
2. **apply/rollback 载体语言**:任务书措辞「照抄 apply-openclaw-patch.sh 的 STOP-on-mismatch 形态」
   指**行为形制**(锚点不中→拒绝,退出码 3)。冻结契约取 **Python 载体**(`apply_model_declaration.py`
   / `rollback_model_declaration.py`)——理由:本机无 `jq`,Windows 路径经 bash/MSYS 易被扭曲,Python
   载体离线 exercise 最稳(stdlib json、`sys.executable` 可跑);drift-guard「STOP + exit 3」形制逐字保留。
3. **live 面不做 SSH**:SSH 线束助手在仓外(`C:/tmp/ssh_helper.py`,历史 live 件 `SSH_HELPER` 默认值),
   缺它则 §3 冻结冒烟不可过;故取 G 包先例(`test_g_relay_fix` live 面)的 **evidence-read 形态**。
   真机发 HTTP 的探针批执行器归 builder/verifier live 相位。若 planner 要求 live 齿真连 ECS
   (probe≠exercise 更强形态),属 re-spec,请回派。
4. **A 案 allow 语义**:冻结契约要求 A 案 `change.model.primary ∈ change.model.allow`(自洽)+ apply 后
   `primary ∈ modelPolicy.allow`;不钉 allow 全集精确相等(避免过度脆性)。
5. **声明工件格式 = JSON**(非 YAML):math 面零依赖(`json` stdlib),K3 先例读 provider.config.yaml 亦
   走 regex 而非 pyyaml;冻结契约按 JSON 钉。

## 6. mutation 证据(打在配置接线上)

mutation 对象 = 声明工件的**配置接线**(删声明文件 / 改错 thinking 值 / 删 allow 条目 / 改错 model id),
不是超时判定。校验器 = 冻结件模块级纯函数 `validate_declaration()`;复现 = 载入冻结件模块喂 fixture。

实测输出(2026-09-24,本树,`PYTHONIOENCODING=utf-8`):

```
== GREEN controls ==
[GREEN] control B (valid): 0 violation(s)
[GREEN] control A (valid): 0 violation(s)

== M2 改错 thinking 值 (off -> high) ==
[RED  ] M2: 1 violation(s)
        - case=B 要求 change.thinkingDefault ∈ {'off','low'}，实得 'high'

== M3 删 allow 条目 ==
[RED  ] M3: 1 violation(s)
        - case=A 要求 primary 'deepseek/deepseek-chat' ∈ model.allow（删 allow 条目即违规）

== M4 改错 model 语义 (reasoning 翻真) ==
[RED  ] M4: 1 violation(s)
        - case=A 要求 change.model.reasoning 显式为 false（非推理模型）

== M6 改错 model id (primary 不落 allow) ==
[RED  ] M6: 1 violation(s)
        - case=A 要求 primary 'deepseek/deepseek-v4-pro' ∈ model.allow（删 allow 条目即违规）

== M5 两轴混声明 ==
[RED  ] M5: 1 violation(s)
        - case=B 不得同时声明 model 变更（一晚只动一个变量）

== M1 删声明文件 ==
   DECL_PATH: ...\server\deploy\perf-k1\openclaw-model-declaration.json exists: False
   (absent => test_a1_declaration_file_present_and_schema 设计态红)
```

RED→GREEN 声明:GREEN 对照组(合法 B/A 声明 = 0 violation)即「接线正确」态;每条 mutation 使对应齿
(RED)翻红,回填即复绿。**M1 的载体齿 `test_a1_declaration_file_present_and_schema` 现即红(声明工件
不在位 = 设计态红),builder 落盘声明工件后转绿。**

**apply/rollback 行为齿的可判性反证**(证明该三齿非空过、契约可实现)——用一份**临时参考实现**跑这三齿
的 exercise 路径(临时件在 tempdir,不落仓、不触真路径):

```
[GREEN] apply rc=0 target_ok=True backups=1
[GREEN] re-apply rc=0 unchanged=True no_new_backup=True out='SKIP: already at declared target state (idempotent).'
[GREEN] drift-guard rc=3 unchanged=True
[GREEN] rollback dry-run rc=0 no_write=True ; restore rc=0 restored_to_pre=True
[MUT  ] apply w/o drift-guard on mismatch: rc=0 (expected 3 -> tooth RED because rc!=3)
```

末行 = mutation 打在 apply 的 drift-guard 接线上(删锚点核对循环)→ 该齿因 rc!=3 转红。**counterEvidence**:
参考实现使 4 条行为齿全绿 → 契约可实现、齿非不可能红。builder 落盘真实现后,由 verifier 用同一命令组
重放 RED→GREEN(`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/`)。

## 7. 交 planner 落地件(超出 acceptance-author 机器写边界,请代落)

### 7.1 `taskpackage.json` 的 `redBaselineResult` 字段(替换 PENDING 占位)

```
18 failed / 62 passed / 4 deselected —— 2026-09-24 acceptance-author 实跑;命令
`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/`(仓根)。18 红全为
设计态红(声明工件/apply/回滚/probe_gate/probe_batch 未落盘),逐齿映射见
server/acceptance/PERF-K1-ACCEPTANCE-NOTES.md §3;既有齿零新增红 = 62 passed(既有 57 @1746c14,
实测 `git diff --quiet 1746c14 HEAD -- server` 零差异 + 本包 5 条 must-stay-green 设计态即绿齿);
收集零错误(冻结即冒烟 §3 过)。
```

### 7.2 evidence 追加件(append 进 `evidence/latency-baseline-260924.md`,追加一节)

见本文件 §1~§6——planner 可按需整体 append 或抽取 §2/§6 两段。本文件即在写边界内的留档,verifier
可直接读 `server/acceptance/PERF-K1-ACCEPTANCE-NOTES.md`。