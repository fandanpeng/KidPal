# architect-landing-summary · 260907-gov-review-architect-dispatch-model-override

packet-id：FROZEN PROPOSAL v1.1 §2 改法集（T1–T2）+ §5 验证集（V1–V6） · 评审轮次 R1 GREEN（0 BLOCK / 3 NIT，F1–F3 已回灌） · change-id 260907-gov-review-architect-dispatch-model-override

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 新增 G3 hook | `.claude/hooks/agent-model-guard.mjs` | 新增，逐字按 PROPOSAL §2 T1 围栏源码落地（含 R1-F3 完备性依赖注释两行） |
| T1 settings 注册 + `_doc` 登记 | `.claude/settings.json` | PreToolUse 增第三 matcher 块 `Agent|Task`（现三块：Bash / Edit\|Write\|MultiEdit / Agent\|Task）；`_doc` 末行（原 :8）补尾逗号（R1-F2 衔接句）后追加登记行，逐字按 PROPOSAL |
| T2 MODEL-TIER 边界句 | `.claude/agents/planner.md` | :57 MODEL-TIER 段末追加边界句，逐字按 PROPOSAL §2 T2「替换后完整句」；该行其余未动 |

## 2. 验证证据

- **V1 hook 自检**：`node .claude/hooks/agent-model-guard.mjs --self-check` → stdout `{"selfCheck":"agent-model-guard","pass":15,"fail":0,"total":15,"ok":true}`，exit 0。绿。（真空绿规约：自检输出含新增自检名 `agent-model-guard` + 全量计数；无重放类。）
- **V2 探针三形态**：architect+opus → exit 2，stderr 逐字含 `agent-model-guard: architect 派发携带 model="opus" — planner.md:71 …`；architect 无 model → exit 0；builder+opus → exit 0。绿。
- **V3 G2 回归**：`node .claude/scripts/route-regression-check.mjs` → `route-regression-check: 28 ok, 0 fail`，exit 0；`node .claude/scripts/lint-descriptions.mjs` → exit 0，1 条**既有**告警（landing-verifier.md 超长，本批未触该文件，非本批引入；初稿 summary 误写「零告警」，landing-verifier 复核时修正）。绿。
- **V4 settings 结构**：`node -e "JSON.parse(...)"` → `settings.json OK; PreToolUse matchers: ["Bash","Edit|Write|MultiEdit","Agent|Task"]`。绿（R1-F2 衔接后 JSON 合法、matcher 块计数 = 3）。
- **V5 在线触发探针**：**待 landing-verifier 绿 + 本会话新 hook 注册生效后执行**（PreToolUse 注册对进行中会话的生效时机属 harness 行为，本会话内 Agent 派发若被拦即同时是 V5 证据；若本会话未触发，由 landing-verifier 独立核验后在新会话补跑，探针红即打回）。
- **V6 landing 保真度**：由 landing-verifier 独立执行 `landing-fidelity-check.mjs`，本 summary 不预填。

## 3. 诚实注记

- **V5 未在本 summary 落笔时完成**（见 §2 V5 条目）：1a 未实测项（matcher=Agent 在 2.1.263 触发）的终验依赖在线派发，安排如 §2 V5 所述，未落即如实披露，不假绿。
- **OQ3 留根（human 裁 (a)）**：AInativecompany 仓 `.claude/agents/architect.md` 分叉漂移 `model: opus` 本批不动，留根 §6。
- **census 计数为下界**（R1-F1 修订后形态）：harness 侧全量无时间下界，architect 派发记 ≥19、带覆盖恰 1，判官 R1 独立枚举复核一致。
- **无降级 / 无允差 / 无未落项**：manifest 三路径全部落地，V1–V4 全绿。

```landing-manifest
.claude/hooks/agent-model-guard.mjs
.claude/settings.json
.claude/agents/planner.md
```
