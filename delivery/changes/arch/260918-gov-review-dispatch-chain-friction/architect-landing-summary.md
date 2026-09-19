# architect-landing-summary · 260918-gov-review-dispatch-chain-friction

packet-id：FROZEN PROPOSAL v1.0 §2 改法集（T1）+ §5 验证集 · 评审轮次 R1 BLOCK(F1..F4) → human 2026-09-19 裁定移出 T1〔预防声明〕 → **R2 GREEN** · change-id 260918-gov-review-dispatch-chain-friction

## 1. 落点对照（manifest 2/2）

| manifest 路径 | T 项 | 落了什么 |
|---|---|---|
| `.claude/skills/kidpal-serial-monitor/SKILL.md` | T1 ①+② | ① 新增一节 `## Device round protocol / 真机轮协调协议`（三条：**单一长捕获**〔含 DTR/RTS 复位伪影识别——判据 = 与捕获启停**秒级吻合**〕/ **human 就绪信号开窗** / **实时口令钉动作点**），每条附 4 包 retro 实证；② 既有 `## Rules that cost time when forgotten` 节补一条指向新节。行数 31 → **43**。既有三节标题逐条保留、内容未动。 |
| `.claude/references/pod-coordination.md` | T1 ③ | 在 §11「demo 对账行」段后补**一行引用指针**（指向该 skill 新节，**不转抄正文**）。**目的表述按 R1-F3 如实化**：明写「主通道 = skill 自身的 description 触发；此处指针是**收口 / 对账读者的次要发现路径**」，并说明「知道这三条有助于**判 demo 证据本身的质量**」（例：与捕获启停秒级吻合的「重启」是伪影而非真重启）。 |

```landing-manifest
.claude/references/pod-coordination.md
.claude/skills/kidpal-serial-monitor/SKILL.md
```

## 2. 验证集执行记录（V1–V7）

| 项 | 命令 / 方法 | 结果 |
|---|---|---|
| V1 | `grep -n "^## Device round protocol" SKILL.md` + 三小条各 `grep -c` | 新节标题命中 **1**；`单一长捕获`=1 / `就绪信号`=1 / `实时口令`=2 ✓ |
| V2 | `wc -l SKILL.md` + 三个既有节标题各 `grep -c` | **43 行**（由 31 增至）；既有三节标题各 **=1**（防重写覆盖）✓ |
| V3 | `grep -c "kidpal-serial-monitor" pod-coordination.md` | **1**（指针在场）；目视核该行**未转抄**正文 ✓ |
| V4 | `grep -c "DTR/RTS" SKILL.md` | **2**（`:10` 原有 1 + 新节伪影模式 1）✓ |
| V5 | `route-regression-check.mjs` + `lint-descriptions.mjs` | `28 ok, 0 fail`；lint exit 0（仅既有 `landing-verifier.md` 超长告警）✓ |
| V6 | 见 §3 偏差 1（**锚点校准**） | **按本意满足**：P2 落地面零触碰 `.claude/scripts` / `.claude/agents` ✓ |
| V7 | `landing-fidelity-check.mjs <本批>` 结构门 | 见 §5（由 landing-verifier 亲跑） |

**真空绿规约核对**：V1/V2/V4 均为**在场性 + 防覆盖**双断言（既有三节标题须仍 =1）；V3 加目视核「未转抄」（防双源漂移）。

## 3. 落地中的事实性偏差（如实申报）

1. **V6 的锚点校准（FROZEN §5 V6 按字面会误报）**：FROZEN V6 写「landing 后跑 `git log --oneline <FROZEN 时点 SHA>..HEAD -- .claude/scripts .claude/agents`，预期零提交」。**本批实际按字面跑得 1 个 commit**——但经核**该 commit 是同族 P4 的合法 landing**（`95bc44b`，其 manifest 本就含 `.claude/scripts/landing-fidelity-check.mjs`），**非本批夹带**。**V6 的冻结措辞隐含「区间内无其它批次落地」这一前提**，而本族四批是**按序连续落地**的，故该前提在本族内不成立。**按本意核**（P2 自己的落地面是否触碰脚本 / agent 面）→ 工作树 `git status` 对这两路径命中 **0**、待提交面恰为 manifest 两路径 ⇒ **本意满足**。**不改冻结稿**，偏差在此申报；**给后续批的提示**：V6 类断言宜锚「本批 landing commit 的范围」或直接用工作树状态，不宜锚「FROZEN 时点..HEAD」。
2. **SKILL.md 的语言**：该 skill 既有内容为英文，本批新增节按 FROZEN §2 的措辞用中文散文（标题保留 FROZEN 给定的双语形态 `## Device round protocol / 真机轮协调协议`）。**中英混排是该文件落地后的实况**，如实登记；新增的既有节指向行同样为中文。理由：FROZEN §2 的条文本身即中文措辞，且仓规「输出标准」要求散文用简体中文。
3. **新节内小条计数**：`grep -c "实时口令"` = 2（新节标题级 1 + 正文引用 1），非 1。V1 的预期写「各 ≥1」，满足。

## 4. 残余风险 / 留根指针

- **留根①（T1〔预防声明〕移出）**：完整交接见 PROPOSAL §6 留根①——含 **R1-F1 的三例运行者身份表**（char-stroke / story-p3 均为**主会话直跑**、无 dispatch prompt 存在，唯 g-relay-fix 是派发）、**双臂修法方向**（派发臂 + 自执行臂）、**human 2026-09-19 的「补上携带臂」裁决**、以及两处计数更正。**下一批须双臂一起落，不得只落口径臂。**
- **留根③（真机轮 checklist 载体缺位）**：本批实测全仓**不存在独立的真机轮 checklist 文件**，故三条统一并入 skill 新节。若将来另立 checklist 载体，需从 skill 回链。
- **交日落复盘项**：本批 T1③ 的指针为「次要发现路径」——建议 `/loop-retro` 观察：真机轮场景是否真由 skill description 触发（主通道成立）、收口/对账读者是否真用到该指针。若从未用到，下一批可评估撤除。
- **本批为四批拆包族之一**（另三批 = merge-chain / closeout-chain / batch-drafting〔已 landed `95bc44b`〕）。四批零文件重叠。

## 5. 落地者自述边界

本摘要由主线程（落地者）自述，**不自评通过**。保真度判定归 `landing-verifier`（其独立跑结构门 V7 + 逐文件人读 diff 对照 FROZEN §2）。

## 6. 核验后注记（append-only，2026-09-19）

landing-verifier 判 **PASS + 1 ADVISORY**（`landing-verdict.md`）。两处观察及处置——**本节只追加，上文不改**：

- **A1（轻，非阻断，判为合规）**：pod-coordination 的指针行含一句触及伪影规则本体的示例（「与捕获启停秒级吻合的『重启』是端口复位伪影」），与 §2 T1③「不转抄正文」字面有**轻微张力**。核验官判**合规**——规则**操作本体**（三条具体做法）未复制、双源漂移风险极低，且该示例正是 R2 背书「判 demo 证据质量」立意的实现。**处置：本批不动**（代码/文本已通过核验，事后改动会使 verdict 失效）；**建议将来撤并 skill 该节时同步复核此示例**。
- **A2（格式级，非 finding）**：新节插于「Reading a smoke round」节与文件末 `Related:` 页脚行之间，页脚顺移——语义无碍。