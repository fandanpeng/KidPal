# architect-landing-summary · 260920-gov-review-reader-scripts-align

packet-id：FROZEN PROPOSAL v1.0 §2 改法集（T1–T2）+ §5 验证集 · 评审轮次 R1 BLOCK(F1..F4) → **R2 GREEN**（附 2 NIT F5/F6，已在标冻前折入） · change-id 260920-gov-review-reader-scripts-align

## 0. 本批特殊形态

本批是**新模板**（`260920-gov-review-proposal-structure-merge` 落地后）下的**首批**——§1 已并入 §2、每 T 五段固定式（问题/证据/根因/改法/锚清单）。核验官请按新结构核，勿按旧两段式要求。

## 1. 落点对照（manifest 2/2）

| manifest 路径 | T 项 | 落了什么 |
|---|---|---|
| `.claude/scripts/brief-inputs.mjs` | T1 | ① `isMerged()` 重构为 `isMerged(changeId, dir = projectDir)`（`dir` 参数为自检注入用，**既有调用方零变更**）；② 在 pod-events 扫描循环内**增第三判定源**：`/"eventType"\s*:\s*"human-decision"/` **且** `/"decision"\s*:\s*"[^"]*approve-escalated-merge/` 双条件命中即 return true；③ `--self-check` 补双夹具（ESCALATE + 人批 decoy **须入选**、其他 `human-decision` **不入选**、无事件文件 → false），且**先断夹具非空**（真空绿）。imports 增 `mkdirSync`/`mkdtempSync`/`rmSync`/`tmpdir`。 |
| `.claude/scripts/dispatch-stamp.mjs` | T2 | 新增**可选** flag `--model <id>`（`const model = flagVal('--model') \|\| null;`）；payload 以**条件展开** `...(model ? { model } : {})` 写入——**传了则含 `model` 键、不传则不带该键**（向后兼容，既有调用方行为零变更、不制造「0 充数」式伪值）。 |

```landing-manifest
.claude/scripts/brief-inputs.mjs
.claude/scripts/dispatch-stamp.mjs
```

## 2. 验证集执行记录（V1–V5）

| 项 | 命令 / 方法 | 结果 |
|---|---|---|
| V1 | `node .claude/scripts/brief-inputs.mjs --self-check` | **23/23 passed**（原 19 → +4 新夹具）；**四条新夹具名逐行具名在场**：`isMerged 夹具非空（两 decoy 的 pod-events 均在盘）` / `isMerged ESCALATE+人批 decoy 须入选（第三判定源）` / `isMerged 其他 human-decision 不入选（decision 词面不含 approve-escalated-merge）` / `isMerged 无事件文件 → false` ✓ |
| V2 | **三路探针 · temp projectDir 模式**（R1-F1 采纳的设计；起草期已实证可行） | ① `grep -c "flagVal('--model')"` = **1**；② 带 `--model claude-opus-5` 跑 → temp dir 的 `pod-events.jsonl` 该条 `payload` 含 `"model":"claude-opus-5"` ✓；③ 不带 `--model` 跑 → 该条 `payload` **不含** `"model"` 键 ✓；temp dir 用毕 `rm -rf` 清理成功 ✓ |
| V3 | `route-regression-check.mjs` + `lint-descriptions.mjs` | `28 ok, 0 fail`；lint exit 0（仅既有 `landing-verifier.md` 超长告警）✓ |
| V4 | `git status --porcelain`（排除 codebase-memory 两件） | **恰为 manifest 两路径**（`brief-inputs.mjs` + `dispatch-stamp.mjs`），无夹带 ✓ |
| V5 | `landing-fidelity-check.mjs <本批>` 结构门 | 见 §5（由 landing-verifier 亲跑） |

**真空绿规约核对**：V1 的新增夹具**逐行具名**（4 条 `isMerged` 断言全部出现在逐行输出中，非只进总数）；夹具本身**先断非空**（两 decoy 的 pod-events.jsonl 在盘）；V2 探针**事件确落 temp dir**（R1-F1 指出的 v0.2 假绿已消除）。

## 3. 落地中的事实性偏差（如实申报）

1. **T1 的自检夹具块首版插错位**：首版把夹具块插在**逐行打印循环之后**——4 条断言**有跑、计入总数**（19→23），但**名字不在逐行输出里**（违反真空绿规约的「具名」要求）。起草期自检时发现，已**移块到打印循环之前**（`const failed` 行之前）。如实登记这次自我纠正；**教训**：插入新断言时须核「它会不会落在打印循环之后」。
2. **净增益数字从 4/19 口径改为 7/34 口径**（R1-F2）：原引 landing-verdict 的「现判只认出 4/19」为**彼时时点值**；本批现跑（34 候选〔`current/` 不计入〕/ 两源认出 21 / 带 `approve-escalated-merge` 24）得**净增益 7**，且与 R1-F2 独立复跑一致。7 个包已逐个列名。
3. **T2 的 census 数字随回同步 merge 漂移**：起草期 192 / R1 复跑 195（`260920-m2-charwindow-card-fix` 当日 3 条）。已在 T2 证据段加**时点注记**，并把判据改为「`payload.model` 0 条」这一**全覆盖事实**（非具体条数）。
4. **结构门 citation advisory 1 条（如实披露）**：门报 `review.md:5 -> file-not-exist`。该引据在 **PROPOSAL §6 留根①**（冻结稿、落地者不可改），指 `delivery/workstreams/m1-home-qa/review.md`——**跨批观察的第 4 例短形态引据**（本族四例：本批 `review.md`、批② `hooks/verify.mjs`、P1 的 `<batch>/PROPOSAL.md` ×4、P4 残留 `<batch>/retro.md`），共性是**省略了仓内已知前缀**。门照设计「只观察不产 fail」（`ok:true` 不受影响）；**建议随「citation 候选域」下一批一并评估**。

## 4. 残余风险 / 留根指针

- **留根②（子项 b 缩圈移出）**：token 分项机制已在（`buildTokenUsage` 四 flag），缺口是**调用方未喂值**（192 条 exact 中带非零分项仅 4 条）；属 harness 侧可观测性，随子项 c 一并处置。
- **留根③（MODEL-TIER 引用行）**：`planner.md:59` 的条款本批只落**字段面**（T2），**未加 `planner.md` 引用行**（动 agent def 触 self-def 判断点）。
- **留根④**：`planner.md:59`（R1-F4⑤ 更正）。
- **留根⑤（`git log --merges` 判定源形态未采用）**：若将来出现「pod-events 文件缺失但包确已合并」的实况，再评估。
- **留根⑥（与 `ARQ-260920-changes-index-mergedinmain-stale` 的口径差注记）**：原 ARQ 行记「97 次派发仅 44 次 exact」，本批现跑「192 条 dispatch-completed、exact 126」（口径与包范围均不同）；该 ARQ 行按其原口径在册、本批不动。
- **交日落复盘项**：T1 落地后 `/loop-retro` 观察——同 subsystem 先例集是否真变新。
- **本批为「批①」（脚本小修族）**；**同族的批二〔verdict 形态 3 条〕与批四〔dev-loop 约定 4 条〕未启动**，按 human 排序待续。

## 5. 落地者自述边界

本摘要由主线程（落地者）自述，**不自评通过**。保真度判定归 `landing-verifier`（其独立跑结构门 V5 + 逐文件人读 diff 对照 FROZEN §2）。