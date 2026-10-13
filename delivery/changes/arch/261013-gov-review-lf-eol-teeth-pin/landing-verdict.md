# landing-verdict · 261013-gov-review-lf-eol-teeth-pin

> landing-verifier 独立核验 · 2026-10-13 · 分支 `gov/261013-gov-review-lf-eol-teeth-pin`（工作树未提交态）
> 核验对象 = frozen PROPOSAL v0.2 的 landing 清单（manifest 三路径）对实际 diff 的保真度。

## 最终结论（第二轮窄复验后）：**GREEN**（放行 commit；merge 仍归 human / 前置升级闸序列）

```json
{
  "ok": true,
  "gate": "landing-verdict",
  "canonicalFidelity": "byte 级成立（首轮 25 断言 25 PASS，第二轮 canonical 零漂移复核）",
  "structuralGate": "landing-fidelity-check.mjs exit 0，ok=true（frozen / packetBinding / 三方清单相等 / 三空集）",
  "evidenceArtifact": "architect-landing-summary.md 五组件齐（packet-id 行 + 落地对应表 + 验证证据 + 诚实注记 + landing-manifest 三路径围栏）",
  "advisories": [
    "summary §1 T1 行散文计数「两处 merge=union」实为 5 处（HEAD 同为 5）——承重断言「既有条目零改动」机器实证为真，建议主线程顺手改该计数；不阻塞",
    "结构门 verbatimQuoteAdvisory 两条 missing = PROPOSAL 两「替换前」块——被替换后当然不在工作树，属替换型编辑的设计内误报；首轮已证其在 HEAD blob 唯一存在（各 1 命中），裁决消解"
  ]
}
```

放行建议：进入协议步骤 6/7（retro 补写 → changes-index upsert → TECH 条目 closed 翻转 + crq-render → 前置升级闸 → ff 合并本地 main）。

---

## 第二轮窄复验实录（2026-10-13，响应主线程制品修正）

- **结构门复跑**：`node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/261013-gov-review-lf-eol-teeth-pin` → **exit 0**，`ok=true`，`frozen=true`，`packetBinding=true`，`proposalTargets == summaryTargets == [".gitattributes", "server/acceptance/test_prompt_v7_round_routing.acceptance.py", ".claude/skills/merge-to-main/SKILL.md"]`，`unauthorized=[]`，`claimedNotLanded=[]`，`plannedNotLanded=[]`，`gitDegraded=false`。citation-anchor / lint-descriptions advisory 均无问题（landing 不含 .claude/agents/**，未附跑 lint 属正常分支）。
- **制品五组件对账**（architect-landing-summary.md ↔ 模板五组件）：① packet-id 行在场且指 FROZEN v0.2 PROPOSAL §2（T1–T3）+ §5 验证集、评审轮次 R1→R2 GREEN、change-id 一致；② §1 落地对应表三行 = manifest 三路径；③ §2 验证证据七条与 PROPOSAL §5 及本 verifier 实跑输出逐项一致（5 passed / 164 passed, 4 deselected, 9 subtests / canonical 哈希 `425d293d…` / anchorUnion 三路径），并如实记录 post-commit 项未执行；④ §3 诚实注记四条（重物化允差 / census 命令面偏窄由 R1 补扫收口 / 留根去向）如实；⑤ ```landing-manifest``` 围栏恰三路径、逐行 == FROZEN PROPOSAL manifest == 实际改动集。
- **首轮结论引用正确性**：summary §2 末段记入的本 verifier 首轮 25 断言结果与首轮 verdict 记录一致（25/25 PASS、首轮 RED 归因于制品形态、canonical 零返工），无失实转述。
- **canonical 零漂移**：`git status --porcelain` + `git diff --stat HEAD` 三文件改动量 1/8/11 行，与首轮 byte 级核验时的形态逐字同形（11 = 8 行替换块净增 + 3 行 docstring 重排，首轮已 byte 级全等证实）。
- **summary 散文计数 advisory**（不阻塞）：§1 T1 行「既有条目（`*.sh` / 两处 `merge=union` / crq INDEX）零改动」——实测 `.gitattributes`（工作树与 HEAD 同）含 `merge=union` **5** 处（workstreams progress ×1、taskpackages pod-events ×1、crq INDEX ×2），且 crq INDEX 两行本身就是 merge=union（原句双重计列）；「既有条目零改动」这一承重断言由纯追加实证为真。建议主线程把该括注改准（如「`*.sh` / `merge=union`×5 / `.codebase-memory` 注记」）。
- **设计侧门**：N/A（Track B 无 decision-packet，proposal-consolidation-check 不跑，前提由评审记录 GREEN + FROZEN 承载）。

## 首轮核验存档（2026-10-13，判 RED 的证据面，全部继续有效）

### 首轮结论：RED（结构门红——制品件不合格；canonical 落地本体 byte 级保真全部成立）

首轮 RED 事实：主线程当时手写制品为 `landing-summary.md`，SSOT 要求 `architect-landing-summary.md`（协议步骤 4 + `docs/templates/architect-landing-summary.template.md`「复制勿手写」），且该文件零 code fence、无字面 packet-id token——结构门 fail-fast（`architect-landing-summary.md is required`）。主线程已按正形修正（旧件删除、按模板落盘新件），canonical 未动，本轮窄复验确认修正成立，终判翻 GREEN。

### 逐文件语义核对（首轮 byte 级机械比对，25 断言 / 25 PASS）

核验方法：一次性脚本（临时写于本目录、验毕已删）运行时从 `git diff --name-only HEAD` 推导路径，将「HEAD blob + PROPOSAL §2 替换块」与工作树文件做**行尾归一后 byte 级全等比对**（.py / SKILL.md 为 Windows CRLF 检出，属环境态，blob 均 LF；prompt 五件工作树已物化 LF）。

| 文件 | 结构触及 | 语义匹配 | expected（frozen §2） | actual（diff 实证） |
|---|---|---|---|---|
| `.gitattributes` | ✔ 纯追加 | **ok** | 追加注记块 + `server/deploy/openclaw-system-prompt.*.md text eol=lf`（先例注释惯例形态） | HEAD 29 行前缀逐行零改（含 `*.sh` / `merge=union` 既有条目），尾部追加 7 注释行 + 1 pin 行，pin 行逐字 == T1 指定行 |
| `server/acceptance/test_prompt_v7_round_routing.acceptance.py` | ✔ 两处替换 | **ok** | :122–124 常量块 + :354–355 docstring 两块 byte 级替换（BLOCK-R1-1 正形） | 两「替换前」块在 HEAD blob 各唯一命中；HEAD+替换 == 工作树全等；旧值 `bf5c6b4c` 残留恰 2 处（均为新溯源注记自身，与 PROPOSAL 替换后块隐含计数一致）、新值 `425d293d` 恰 3 处（常量 + 两注记）同判 |
| `.claude/skills/merge-to-main/SKILL.md` | ✔ 单 bullet 追加 | **ok** | 「失败信号 → 处置」节 `git diff --check` bullet 后追加指定一行 | HEAD 无该 bullet；工作树恰 1 处且插入位 = 失败信号节锚 bullet（HEAD :32）之后 :33，其余行零改 |

**V6_SHA256 独立复核**：`git cat-file blob HEAD:server/deploy/openclaw-system-prompt.v6.md | sha256sum` = `425d293d35b106b581785a78055f5718068531b240d52db763591eed68b0cc62` == 工作树 `V6_SHA256` 常量 == PROPOSAL T2 指定值，三处一致。

### §5 验证集复跑（verifier 实跑，2026-10-13 本 worktree）

1. `git check-attr text eol`（v5–v9）→ 5 文件 × 2 属性 = `text: set` / `eol: lf` ✔
2. `git ls-files --eol`（v5–v9）→ 全 `i/lf w/lf attr/text eol=lf`；python 字节探针 `b"\r\n" not in data` → 五件全 LF（size 20372/22464/10585/11332/11678）✔
3. `git cat-file blob HEAD:…v6.md | sha256sum` → `425d293d…cc62`（blob 零回改）✔
4. `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance -k immutable` → **5 passed, 163 deselected** ✔（V6 齿对新常量真绿）
5. 全目录同参 → **164 passed, 4 deselected, 9 subtests passed**（与 §3 基线逐字同数）✔
6. `landing-fidelity-check.mjs --preflight <PROPOSAL>` → ok=true，anchorUnion 恰 manifest 三路径 ✔
7. post-commit 项：commit 后由主线程复跑（验证 4 + 5 + `git status` 五件 prompt 干净），本轮不适用。

### 破坏性自检

- prompt v5–v9：`git diff HEAD --` 五件均空（blob + 工作树零内容回改）；齿语义「历史版本零回改」保持。
- `.gitattributes` 既有行零改动（纯追加实证，工作树与 HEAD 均 5 处 merge=union 同数）。
- SKILL.md 除新 bullet 外零改动；改动集 == manifest 三件，无夹带、无私货、无权限/写域扩张。
- 敏感面：本批无 G1/G3/self-def/review-loop agent def 触碰；SKILL.md（G2）单行注记与被批 packet 逐字一致。

## 核验过程注记（诚实披露）

- verifier 自用比对脚本曾两次因 heredoc 管道字符损码产生假 FAIL（行尾口径 / 溯源注记含旧哈希前缀被误判 / CJK 字面量失配），均为脚本自身缺陷而非落地缺陷；终版以 Write 落盘（review 目录内、验毕删除）、路径全部运行时由 git 推导、关键字面量（pin 行、CANON 哈希）另经 `git diff` 直读输出双重印证，结果可信。
- verifier 只读本批 codebase、只写本 verdict + 已删临时脚本，未触碰任何 canonical 文件。
- verdict 文件内全部路径 / 哈希 / 计数 token 已用宽容正则复核为纯 ASCII 且与 git 实证一致。
