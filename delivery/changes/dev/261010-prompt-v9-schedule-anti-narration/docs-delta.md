# docs-delta · 261010-prompt-v9-schedule-anti-narration

> author: integrator · date: 2026-10-11 · TaskPackage: `delivery/taskpackages/261010-prompt-v9-schedule-anti-narration/`（exitCriteria PN-E3）

## 结论一览

| 面 | 动作 | 状态 |
|---|---|---|
| `docs/architecture.md` §5.5.5（:297）现状指针 | 刷 v7→v9 + 补 v8/v9 沿革两项 | **未落（handoff architect）**——enforce-scope 钩子拦截 integrator 写入 |
| `docs/architecture.md` §5.1（:196）现状指针 | 刷 v7→v9 | **未落（同上，随 §5.5.5 一并 handoff）** |
| `server/deploy/README.md` prompt 指针节（:44-51） | 核对（builder 已刷「现状 = v9」） | 核对通过 |
| `docs/contracts/**` | 零改（PN-E2） | 零改（本包 diff 无契约面文件） |
| `docs/requirements.md` | 零改（NFR-SAFE-1 within-spec） | 零改 |

## spec 面 handoff（arch 指针刷新，交 architect）

- **机械拦截事实**：对 `docs/architecture.md` 的两处 Edit 被 `enforce-scope.mjs` 拦截（agent "integrator" 写权枚举不含 `docs/architecture.md`）。派单所引「universal docsTopLevel allowFiles 授权」与 scope-map 机判不符，机械闸为准，不绕行（Bash 重定向写同被 scope-map bashDenyWrite 红线拦，亦不尝试）。与角色文本 ARQ-260929-docs-sync-spec-handoff「integrator 对两份 spec 文件无写权」一致。
- **拟落内容（architect 一次刷到位）**：
  - §5.1（现 :196）组件表 OpenClaw 本体行：`system prompt 版本化 v7（§5.5.5）` → `system prompt 版本化 v9（§5.5.5）`。
  - §5.5.5（现 :297）：`**现状 = v7**` → `**现状 = v9**`，并集窗口在既沿 v1..v7 链尾补两项：`→ v8 +故事段「帮我记下来」第二指令 + 意图边界负例〔M4-K3 双指令批〕→ v9 +课表问答回合行对称反叙述纪律〔v7 修对话行时漏课表行对称条款，TECH-261010 真机内部推理当语音播出根治〕`。沿革正文仍在 CARRIER（唯一全量副本），arch 只留指针 + 单行语义，符合 §5.5.5 自身收敛纪律。
- **v7 漂移收敛说明**：本漂移为**既有漂移的顺带收敛申报**——arch §5.5.5/§5.1 现写 v7，而实际接线自 260924-m4-k3-record-session-live 起已为 v8（v8 批漏刷 arch 指针，v8 批 docs-sync 缺口）。本包 bump v9 后，arch 应一次刷到 v9（不是分两步补 v8 再补 v9），漂移事实由本节留痕；handoff 执行后 arch 指针与三面接线（CARRIER == provider.config 三键 == overlay PROMPT_SRC，均已 v9）重新对齐。
- **去向**：交 planner 派 architect 执行上述两处刷新（architect 对 spec 面有写权）；执行后本 delta 状态表两行翻「已落」。

## README.md 核对（builder 件，integrator 核）

- `server/deploy/README.md:44` 节题「OpenClaw system prompt 版本化接线（现状 = v9）」与 :49-51「当前被接线的那一版 = `openclaw-system-prompt.v9.md`（课表问答回合行补对称反叙述纪律，TECH-261010…其余段落零偏移继承 v8）」与 carrier/provider/overlay 三面一致；:55-57 渲染面/声明面描述与实际两文件键名一致。零改需求，核对通过。

## 零改面核验

- `docs/contracts/**`：`git status` 无契约面文件，PN-E2 零改成立；契约 :184 prompt 意图摘要不涉语音纪律（architect review 已核零命中，TaskPackage docsSyncReason 留痕）。
- `docs/requirements.md`：零改；NFR-SAFE-1（安全边界）within-spec，无行为面外溢。
- 历史 v1..v8 prompt 文件：零回改（PN-A3 sha256 负锚机检承载，V8_SHA256 = 91f4a003…6f4）。
