# architect-landing-summary · 261026-gov-review-final-phase-dispatch-contract

packet-id：FROZEN PROPOSAL（v0.2）§2 改法集（T1–T3）+ §5 验证集（B1–B5 / V1–V11） · 评审轮次 R1 BLOCK(F1,F2)+NIT → R2 聚焦复核 GREEN（GREEN 轮 = R2） · change-id 261026-gov-review-final-phase-dispatch-contract

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1.1 派发书 final 相位三条契约块（谁跑 / scope-file 谁产 / round 序数）+ 正确序列 + 两条同根先例 | .claude/agents/planner.md | 插入「派发书三条硬约定」blockquote 之后、「分层出场口径」blockquote 之前（锚句逐字核对）；块首自标「本段 = SSOT，命令真身 = integrator.md Step 4」 |
| T1.2 Step 10 `[SCOPED/FULL]` 两处替换（消与 integrator 冲突的 final 命令） | .claude/agents/planner.md | 原文一/二按字节替换；新句带 scope-file 完整命令 + fail-closed 语义 + 契约块 ③ 指针 |
| T1.3 verify.mjs fail-closed 报错补「正确序列 + 不占序」指引 | .claude/hooks/verify.mjs | :239 stderr 文案追加两句；退出码与判定零变；`缺 --scope-file` 字面与 `260928 批 T9` 出处注记保留 |
| T2.1 AGENTS.md:46 首句 moderate → full + 实测依据 + mode 生效语义 | AGENTS.md | 按 FROZEN 替换文本逐字落地 |
| T2.2 launch-prep SKILL 三处（:18 / :104 / :109 后追加） | .claude/skills/kidpal-pod-launch-prep/SKILL.md | moderate+persistence → full+persistence；mode=moderate → mode=full；「不要传 moderate」注记一条追加于步骤 5 末 |
| T2.3 cli SKILL 两处（Modes 块追加语义行 + :42 示例无害占位 + 下方追加提醒句） | .claude/skills/codebase-memory/cli/SKILL.md | mode 生效语义一条落 Modes 块末；delete_project 示例改 `<项目名>` 占位 + 追加「先 list_projects 核对」句；:19-20 模式定义行按边界有意保留 |
| T3.1 派发书进度留痕契约（自包含快照 + 惯例非门） | .claude/agents/planner.md | 紧接 T1 契约块之后插入（与 T1 同一插入区，中间一个 blockquote 空行分隔） |
| T3.2 dev-loop.md 文件地图新增 dispatch-progress 行 | .claude/references/dev-loop.md | 插在 `input-brief.md` 条目之后、`pod-events.jsonl` 条目之前 |
| T3.3 scope-map 三角色窄 allow（OQ1 已裁 (a)，无条件落地） | .claude/scope-map.json | builder / fixer / acceptance-author 的 allow 数组末尾各追加 `delivery/taskpackages/*/dispatch-progress-*.md`；deny 与其余项零改动（V6 三行 true + deny JSON 逐字打印核对） |

落地文件并集 = 7 项，与头部 landing-manifest 及 FROZEN PROPOSAL §2 锚并集逐条一致（V7 preflight `ok:true / anchorCount:7 / advisories:[] / findings:[]`）。

## 2. 验证证据

主线程在落地盘上（gov worktree，改动在文件、未 commit）逐条实跑：

- **V1**：`node .claude/hooks/verify.mjs --self-check` → `{"selfCheck":"verify.mjs","pass":16,"fail":0,"total":16,"ok":true}` exit 0（文案改动不增不减 case）。
- **V2**：`--gate-child --phase=final` → stderr 含「正确序列」计 1；进程 exit 2（基线 B3 = 0 / exit 2，字面出现 + 退出码不变）。
- **V3**：planner.md 五项 grep：`scope-file`=3、`next completed ordinal`=1、`dispatch-progress-`=1、`supervisor-verdict.json`=4、`taskpackage-required-check.mjs`=3——均 ≥1（基线前两项 = 0）。
- **V4**：四项字面清零——AGENTS「（moderate 模式）」=0、launch-prep `mode=moderate`=0、`moderate + persistence`=0、cli SKILL `delete_project '{"project":"C-AI-KidPal"}'`=0（基线各 1）。
- **V5**：七条 enforce-scope stdin 探针（cwd = 本 gov worktree）：builder / fixer / acceptance-author 写 `dispatch-progress-<agent>.md` 均 **exit 0**（正例翻转，基线 B4 = exit 2）；护栏 builder 写 `review-packet.md` / `taskpackage.json` 均 **exit 2**（反例不破）；planner / integrator 写 progress 文件均 **exit 0**。
- **V6**：`node -e` scope-map 断言 → 三行均 `true`；三角色 deny 数组 JSON 逐字打印，与基线一致零改动。
- **V7/V9 前半**：`landing-fidelity-check.mjs --preflight` → `ok:true`、`section2BodyFound:true`、`anchorCount:7`、`advisories:[]`、`findings:[]`，anchorUnion 7 项与 manifest 逐条一致；exit 0。
- **V8**：`git diff --name-only -- docs/contracts/` → 空输出（契约面零触碰）。
- **B2 复跑**：`enforce-scope.mjs --self-check` → `ok:true`、8 臂全绿，exit 0。

真空绿规约：V4/V5 均为逐字判别断言（清零 / 翻转 / 护栏保持），V6 直接断言数组成员与 deny 逐字值；未出现「断言恒真」形态。

## 3. 诚实注记

1. **V9 后半（逐文件人读 diff）与 V10 / V11 未跑**：V9 人读核对面归 landing-verifier；V10 = post-commit 全量复跑、V11 = 本批自身过闸，依赖落地 commit——本 summary 时点未发生，不预支绿。
2. **落地与 FROZEN 文本的形态差异（两处，均非语义，landing-verifier 复核确认）**：①T3.1 与 T1.1 两块同插于「派发书三条硬约定」与「分层出场口径」之间，两块之间落了 blockquote 空行（`>`）分隔——FROZEN 只约束「紧接 T1 契约块之后」的相对位置，未规定块间空行形态；语义与文本逐字一致。②cli/SKILL.md 的「先 list_projects 核对」提醒句落在 bash 围栏内而非 FROZEN 所述「该代码块下方」——文本逐字一致、位置紧邻、语义不变（landing-verdict.md D2 独立核出并判 non-blocking；本条为该核验后的自述订正，原「一处」计数不精确）。
3. **V5 探针 payload 的 file_path 命名**：正例按各自 agent 名落 `dispatch-progress-<agent>.md`（builder→dispatch-progress-builder.md 等），较 FROZEN 示例的统一 `dispatch-progress-builder.md` 更贴实际写权语义；glob 匹配面相同，判定等价。
4. **ARQ 结项未执行**：三条 ARQ 的 status 翻转（含主条目 paths 订正 `.claude/references/planner.md` → `.claude/agents/planner.md`）= FROZEN §6 结项动作，归 landing-verifier 绿后主线程执行（本 summary 时点三条仍 deferred，防「先结项后核验」倒序）。
5. **留根不关**（FROZEN §6 七项）：launch card 不加 final 契约、scope-file 存量命名变体不追溯、progress 注记惯例非门、moderate 历史散文不追改、KidPal-claude 建图模式未单列核实、bearing preflight 文案未含正确序列、OQ1 窄写权越界复议触发——各自复议触发在 FROZEN §6。

```landing-manifest
AGENTS.md
.claude/agents/planner.md
.claude/hooks/verify.mjs
.claude/references/dev-loop.md
.claude/scope-map.json
.claude/skills/codebase-memory/cli/SKILL.md
.claude/skills/kidpal-pod-launch-prep/SKILL.md
```
