# PROPOSAL · 261013-gov-review-lf-eol-teeth-pin

> **状态：FROZEN v0.2 · 2026-10-13 · 凭据：human 立项 ack（2026-10-13 四答 + 「合并一批，走 intake spine」补裁）+ architect R1→R2 GREEN（§7；R1 唯一 BLOCK 已修并复核）+ spine openQ q1/q2 resolved**
> **承载 ARQ**：`TECH-261013-lf-hash-teeth-vs-windows-crlf-checkout`（tech 域条目，治理面承接；另携同批发现 V6 齿常量 CRLF 毒化——census 新事实，见 T2）。同日姊妹条目 `TECH-261013-lang-config-thinking-key-regen-landmine` 不在本批（固件批另行）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。
> **跨面 intake**：spine = `delivery/changes/intake/261013-gov-review-lf-eol-teeth-pin.md`（触面 ≥2：治理 + 实现；human 裁「合并一批，走 intake spine」，2026-10-13）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T3）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low（仓配置一行 + 验收冻结常量对齐 canonical + skill 注记一条；零实现行为变更、零契约触碰；验证集全机械可判）。
> **review profile**：G2+G3（governance：`.gitattributes` 仓配置 + `.claude/skills/merge-to-main/SKILL.md` 注记；implementation 一行冻结常量按 triage 地板 ① 直接修并入，裁决留痕 spine openQ q2；G1/契约面为零）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：GREEN 结论前必须在场。

```landing-manifest
.gitattributes
server/acceptance/test_prompt_v7_round_routing.acceptance.py
.claude/skills/merge-to-main/SKILL.md
```

---

## 0. 想实现的效果与差距

目标效果一句话：raw 字节哈希冻结齿在任何检出环境（Windows autocrlf=true / Linux / CI）下都判同一件事——canonical 仓库内容是否被回改，而不是检出换行态；合并通道不再被换行伪影拦死。

现状差距：`.gitattributes` 只钉了 `*.sh`（ARQ-260906，彼时范围克制合理），prompt 哈希齿其后诞生且未被钉；Windows 检出即 CRLF，raw 齿恒红（TECH-261013-lf-hash 条目实录：merge-driver 内嵌校验 3 红、pod 置 blocked）。且 census 追加发现：V6 齿冻结常量本身就是在 Windows CRLF 检出上取的值（毒化齿），当前反向强制 CRLF 态、Linux/CI LF 检出必红——钉 eol 与重算常量互为前置（钉生效重物化 v6 为 LF 后旧常量必红），故同批不可拆。

非目标：不改任何 prompt 文件本体（v5–v9 内容零回改，齿语义 = 历史版本零回改，保持）；不动 merge-driver / 各 acceptance 测试的判据形态；不做全仓 `* text=auto` 类泛化 renormalize（影响面失控，本批不做）；固件 `TECH-261013-lang-config-thinking-key-regen-landmine` 另批处理。

## 2. 问题、根因与改法（T1–T3）

**【硬项 · 起草期实证义务】断言留痕**：本节全部行为断言的实测命令与输出落在 §3 census 证据段（2026-10-13 实测）；引用 SHA 均经 `git cat-file` / `git rev-parse` 现核。

### T1 · 冻结齿目标文件缺 eol=lf 钉（Windows 检出换行伪影恒红）

**问题**：prompt 版本文件 v6–v9 被四条 raw 字节 sha256 齿钉（`sha256(read_bytes())` 口径），`.gitattributes` 对它们 text/eol 双 unspecified——Windows `core.autocrlf=true` 检出即 CRLF，raw 哈希与冻结值（LF 口径）必异，齿恒红；且每次 checkout/rebase 复现，本地 LF 归一修复不持久。TECH-261013-lf-hash 条目实录：merge-driver `--pod 261013-qq-schedule-write-closure` 内嵌校验 3 红、pod 置 blocked、合并通道停摆。

**证据**（2026-10-13 实测，本 worktree）：
- `git check-attr text eol -- server/deploy/openclaw-system-prompt.v6.md …v9.md` → 4 文件 × 2 属性全部 `unspecified`。
- `git ls-files --eol` → v5 `i/lf w/crlf`、v6 `i/lf w/crlf`、v7–v9 `i/lf w/lf`（v7–v9 的 LF 是 TECH 条目里那次原地归一后存活态，per-checkout 会再翻 CRLF——伪影复现性本批起草期已实证一次）。
- `git cat-file blob HEAD:…v9.md | sha256sum` = `0b051e14…`（LF）= 冻结期望值；磁盘 CRLF raw = `1acb2b51…`（TECH 条目证据 2 同口径复测一致）。

**根因**：冻结齿按 raw 字节钉内容，但检出换行属环境态；两者解耦缺一道钉——eol=lf 使检出侧不引入换行漂移，raw 口径才能稳定指向 canonical 字节。ARQ-260906 钉 `*.sh` 时该齿族尚未诞生（范围克制彼时合理），此后「按需逐步扩」模式每遇新齿复发一次，本批以**census 命中族钉**收口该模式。

**改法**：`.gitattributes` 追加一行 `server/deploy/openclaw-system-prompt.*.md text eol=lf`，配同文件既有注释惯例的注记块（先例形态、与既有条目无路径交集）。范围论证：census 命中的 raw 齿集 = v6/v7/v8/v9 四件（T1 census，§3）；同族 v5 的齿是归一口径（`read_text` + `\r\n→\n`，免疫），钉 LF 对其零副作用；家族 glob 使未来 prompt 新版本（版本化铁律下每版必配冻结齿）**出生即钉**，消「每遇新齿复发一次」。仓内 blob 侧已全 LF（`i/lf` 五件实测），无需 renormalize。

目标文件：`.gitattributes`（改）

### T2 · V6 齿冻结常量 CRLF 毒化（重算对齐 canonical blob）

**问题**：`server/acceptance/test_prompt_v7_round_routing.acceptance.py:124` 的 `V6_SHA256 = "bf5c6b4c11fd155241a0c5e51adbe2cbcce82768426e12e1a135dfa573f6197b"` 不等于 v6 canonical 内容的哈希——它等于 **CRLF 字节**的哈希。该齿现为假绿（磁盘 CRLF 恰好匹配毒化常量），方向与齿意图（守 canonical 内容零回改）相反；Linux/CI LF 检出上必红。

**证据**（2026-10-13 实测）：
- 磁盘 v6（CRLF）raw sha256 = `bf5c6b4c11fd…` = 冻结常量（`python -c` 读字节 + hashlib 实测）。
- `git cat-file blob HEAD:server/deploy/openclaw-system-prompt.v6.md | sha256sum` = `425d293d35b106b581785a78055f5718068531b240d52db763591eed68b0cc62`（canonical LF）≠ 冻结常量。
- 注释（:122）自称「v6 为 LF 落盘」——与常量实际口径矛盾；测试断言消息（:355）记「260925-prompt-v7-round-routing 批冻结」——冻结时点在 Windows 检出上取值所致。

**根因**：齿冻结时点的取值介质 = Windows CRLF 工作树文件而非 canonical blob；eol 钉缺失使该伪影当时不可见，毒化值入库。同型风险已由 T1 的 eol 钉从环境侧消除；本 T 把已入库的毒化值改正。

**改法**：`V6_SHA256` 常量重算为 canonical blob 哈希 `425d293d35b106b581785a78055f5718068531b240d52db763591eed68b0cc62`，注释同步修正口径注记。v6 文件本体零回改（blob 不动：改前 `425d293d…` = 改后，齿守的「历史版本一律不改」语义完整保持）。承重替换（原文 byte 级钉死）：

替换前（:122–124，verbatim）：

```python
# 冻结时点 v6 内容 hash（raw bytes 口径，v6 为 LF 落盘；v7 bump 与一切后续包不得改 v6 本体）。
# 计算式 = sha256(read_bytes())。
V6_SHA256 = "bf5c6b4c11fd155241a0c5e51adbe2cbcce82768426e12e1a135dfa573f6197b"
```

替换后（完整句，byte 级）：

```python
# 冻结时点 v6 内容 hash（raw bytes 口径 = canonical blob 口径，eol=lf 钉后检出即 LF；
# v7 bump 与一切后续包不得改 v6 本体）。
# 计算式 = sha256(read_bytes())。261013-gov-review-lf-eol-teeth-pin 批重算：原冻结值
# bf5c6b4c… 为 Windows CRLF 检出伪影（census 实测），重算对齐 canonical blob 425d293d…。
V6_SHA256 = "425d293d35b106b581785a78055f5718068531b240d52db763591eed68b0cc62"
```

同文件齿测试自身期望值记录位（:354–355 docstring，verbatim）——同步修正（BLOCK-R1-1 正形；旧值单记残留会复刻「内容被回改」误诊且与 :122 处置不对称）：

```python
        """coverage: V7-A1 — v6 本体零回改（E2 铁律同形态；v1..v5 由既有各包齿钉）。
        v6 冻结时点 sha256 = bf5c6b4c…（260925-prompt-v7-round-routing 批冻结）。"""
```

替换后（完整句，byte 级）：

```python
        """coverage: V7-A1 — v6 本体零回改（E2 铁律同形态；v1..v5 由既有各包齿钉）。
        v6 冻结时点 sha256 = 425d293d…（260925-prompt-v7-round-routing 批冻结；
        261013-gov-review-lf-eol-teeth-pin 批重算对齐 canonical blob——原值 bf5c6b4c… 为 Windows CRLF 检出伪影）。"""
```

仍限同文件（`server/acceptance/test_prompt_v7_round_routing.acceptance.py` 两处替换），不扩 landing-manifest、不新增验证项。

门路由留痕：实现面一行、零行为变更——按 `/triage-change` 地板判 ① 直接修（change-intake §1 比例化），免独立 triage 会话；改法实质由本批 architect 评审一并裁（spine openQ q2，L0 裁定在案）。与 T1 互为前置：eol 钉生效重物化 v6 为 LF 后旧常量必红，故同批不可拆。

目标文件：`server/acceptance/test_prompt_v7_round_routing.acceptance.py`（改）

### T3 · merge 侧高频误诊无提示（raw-hash 齿红先疑换行再疑内容）

**问题**：TECH-261013-lf-hash 条目实录初诊为「历史哈希漂移/内容回改」，实为检出换行伪影；归因成本高。仓内无独立「dev-loop/merge 排障手册」，最近承载位 = merge-to-main skill 的「失败信号 → 处置」节（该节即合并拦红类信号的处置清单）。

**证据**：`grep -rln '排障' docs/ .claude/ delivery/`（2026-10-13）命中为零手册文件；merge-to-main SKILL.md「失败信号 → 处置」节现为四条 bullet（ff 失败 / conflict marker / .codebase-memory 覆写 / 意料外文件），无换行伪影条目。

**根因**：齿红症状（哈希不等）与病灶（检出换行）跨层，无提示则默认沿「内容被改」方向归因。

**改法**：在该节追加一条 bullet（追加位：`git diff --check` 条目之后）：

```markdown
- 不可变齿（raw sha256 类）拦红 → 先核工作树换行（`git ls-files --eol`）再疑内容回改：Windows autocrlf 检出 CRLF 即伪红（TECH-261013-lf-hash-teeth-vs-windows-crlf-checkout 实录）；eol 钉集见 `.gitattributes`。
```

目标文件：`.claude/skills/merge-to-main/SKILL.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| governance | `.gitattributes` | G2 | 仓检出配置；消费方 = 全体 clone/worktree 的 checkout/merge 行为 |
| governance | `.claude/skills/merge-to-main/SKILL.md` | G2 | skill 正文注记一条（DEV-GOVERNANCE §6 归 G2，R1 注记 2 修正）；消费方 = merge-to-main skill 执行会话 |
| implementation | `server/acceptance/test_prompt_v7_round_routing.acceptance.py` | 实现面 · ①直接修并批 | 验收冻结常量一行 + 注记（R1 注记 2 类别修正：实现面非 G2）；消费方 = acceptance 齿测试与 merge-driver 内嵌校验 |
| requirement / architecture / contract | — | 未触 | 零触碰（spine faceMap 判不动；prompt 部署面非契约面） |

**census 证据（机械枚举，2026-10-13，本 worktree）**：

- 枚举命令：`grep -rn 'read_bytes\|sha256' server/acceptance/*.py`（去注释）+ `grep -rln 'read_bytes\|sha256' firmware/main/test/ .claude/scripts/`。命中展开归类：
  - **raw 字节齿（`_sha256_raw` = `hashlib.sha256(path.read_bytes())`）**：`test_prompt_v7_round_routing.acceptance.py`（V6 :361 / V8 :436 / V9 :727）+ `test_m4k3_record_session_live.acceptance.py`（V7 :900，`_sha256_raw` :787-788）——目标 = `server/deploy/openclaw-system-prompt.v6/v7/v8/v9.md` 四件，全入 T1 钉集。
  - **归一/文本口径齿（autocrlf 免疫，免钉）**：`test_m3k3_voice_schedule_entry.acceptance.py`（:517 `sha256(v5.encode())`，v5 经 `_read_text` 文本模式读入 + 注释 :197 自注归一口径）；`test_perf_k1_roundtrip_latency.acceptance.py`（`_sha256_norm` :229-234 显式 `\r\n→\n`）。
  - **治理脚本哈希**：`.claude/scripts/taskpackage-required-check.mjs` :305-306（canonical↔current 同 worktree 双侧同态对比，无跨环境冻结值）；`merge-driver.mjs` :958/:1013-1020（T1-2 已有 LF 归一防，自身免疫）。
  - **firmware 面**：`firmware/main/test/boot_auto_connect.acceptance.c` 为 C 侧运行时哈希，无仓内冻结文件目标（展开核过）。
  - **R1 判官补扫**（超出上行枚举命令覆盖面）：`server/deploy/`（verify-deploy.sh 行尾归一 md5 / perf-k1/probe_gate.py LF 归一 / openclaw-patch/reminder_schedule_lib.py 运行时字符串散列）+ `server/gateway/`（reminders.py 运行时字符串散列）+ `firmware/scripts/`（versions.py ELF 二进制）——全部免疫或无仓内 raw 齿，census 终判「raw 齿目标 = prompt v6–v9 四件」在扩展覆盖面上亦成立（详见 §7 R1/R2）。
- 基线实跑：`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance` → **164 passed, 4 deselected, 9 subtests passed in 2.31s**（当前盘态：v6 CRLF 恰配毒化常量——本批后同命令应同数通过，齿值不同）。
- 免疫面齿当前实跑：`-k immutable` → 5 passed（V6 毒化齿在列，假绿实测）。

**sweep 透镜**：本批不落新门/新断言（T2 为既有断言的冻结值改正；T1 为 git 机制配置），无判定 pattern 预跑义务；eol 钉生效面以 §5 验证集 1 充当。

## 4. human 取舍点（OQ）

- 立项 ack：human 2026-10-13 四答（lf-hash 先行 / census 按命中集合钉 / 固件批后置 / 走 triage）+ V6 毒化发现后补裁「合并一批，走 intake spine」。全量留痕 = spine `sources` 与 `openQ q1`。
- 本批无新增 OQ：eol 钉范围（命中族 + 同族无害成员，家族 glob 出生即钉）在 §2 T1 论证并交判官审；齿常量重算为 canonical 对齐、无方案分歧（备选「保持 CRLF 常量并把齿改归一口径」判据形态动得更大，弃）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

1. **pre-commit**：`git check-attr text eol -- server/deploy/openclaw-system-prompt.v5.md server/deploy/openclaw-system-prompt.v6.md server/deploy/openclaw-system-prompt.v7.md server/deploy/openclaw-system-prompt.v8.md server/deploy/openclaw-system-prompt.v9.md` → 5 文件 × 2 属性 = `text: set` + `eol: lf`。
2. **pre-commit**：重物化后字节探针——`python -c`（read_bytes 含 `b'\r\n'` 判 CRLF）对 v5–v9 → 全 LF（重物化动作 = landing 时 `rm` + `git checkout --` 五件，eol 钉生效后 checkout 产 LF）。
3. **pre-commit**：`git cat-file blob HEAD:server/deploy/openclaw-system-prompt.v6.md | sha256sum` → `425d293d…`（blob 零回改实证，改前改后同值）。
4. **pre-commit**：`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance -k immutable` → 5 passed（V6 齿对新常量绿，假绿转真绿）。
5. **pre-commit**：全目录基线等数——`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance` → 164 passed（与 §3 基线同数；齿值修正不改通过数）。
6. **pre-commit**：`node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL 路径>` → 锚集/manifest 形态零错（anchorUnion 逐 token 核过：仅三条 manifest 路径，注记零夹带）。
7. **post-commit**：重跑验证 4 与 5（幂等 + commit 后齿态复绿）；`git status` 五件 prompt 文件干净（eol 钉后无 per-checkout 漂移噪音）。

## 6. 留根清单

- `TECH-261013-lf-hash-teeth-vs-windows-crlf-checkout` closed 翻转：随本批 landing 完成 + landing-verifier 绿 + retro 齐后由主线程执行（改条目 frontmatter + `node .claude/scripts/crq-render.mjs`）。
- 姊妹条目 `TECH-261013-lang-config-thinking-key-regen-landmine`：不在本批，固件批另行开变更（human 已裁路由 /triage-change + CMake 必生成方向）。
- 「按需逐步扩」模式收口声明：本批 census 命中集一次钉齐；未来新冻结齿若按 raw 口径新增，**出生即钉**依赖家族 glob 覆盖——若新齿目标在 `server/deploy/openclaw-system-prompt.*.md` 族外，登记时须同步评估 eol 钉（此为自律件，非硬门）。
- 归档注记：ARQ-260906 注释自述「范围克制在 `*.sh`」与本批关系（成年期缺口补齐）已写入 T1 注记块，历史条目不回改。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · 2026-10-13 · architect

**结论：打回（1 项 BLOCK、0 项 QUESTION）**——T1/T2/T3 改法方向与证据全部经判官实测复核成立；唯一打回项为 T2 改法未同步 :355 docstring 旧值引用（同文件自相矛盾，正形见下），修毕 R2 复核后可 GREEN。

**逐项复核（判官自证，2026-10-13 本 worktree 实测）**：

- **T1 证据复现**：`git check-attr text eol` 五件 × 双属性全 `unspecified` ✓；`git ls-files --eol` v5/v6 `i/lf w/crlf`、v7–v9 `i/lf w/lf` ✓（家族 v1–v4/v10 亦全 `i/lf`，「仓内 blob 侧已全 LF、无需 renormalize」论证成立）；v9 canonical blob sha256 = `0b051e14…` = 冻结期望值 ✓。
- **T2 证据复现**：v6 canonical blob = `425d293d35b106b581785a78055f5718068531b240d52db763591eed68b0cc62` ✓；磁盘 v6（CRLF）raw sha256 = `bf5c6b4c11fd…` = 现冻结常量（python 字节探针实测）✓。毒化定性旁证：`test_perf_k1` 的 `V6_SHA256_NORM` 归一齿常量恰为 canonical 值（归一口径当年即取对，raw 口径取了 CRLF 伪影），当前盘态两齿双绿共存与「假绿」诊断互恰。
- **T3 承载位复现**：merge-to-main SKILL.md「失败信号 → 处置」节现四条 bullet、无换行伪影条目、追加位（`git diff --check` 条目后）明确 ✓；`grep -rln 排障` 命中均为无关面（CRQ 条目 / opendesign-connect / kidpal-live-smoke-debug / retro 制品），「无独立 dev-loop/merge 排障手册」成立、最近承载位取该 skill 合理 ✓；与 :35 既有「CRLF 噪音」条目主题相邻但语义不重复（status 噪音 vs 齿红归因顺序）。
- **census 独立复核（判官补扫，超出 PROPOSAL §3 枚举命令覆盖面）**：§3 枚举命令未覆盖 `server/deploy/` 与 `server/gateway/`，判官做全仓 sweep 补扫——`verify-deploy.sh` 为行尾归一 md5（自免疫）、`perf-k1/probe_gate.py` 为 LF 归一 sha256（免疫，与 PERF-A1 断言口径一致）、`reminder_schedule_lib.py` / `reminders.py` 散列运行时字符串非仓内文件、`firmware/scripts/versions.py` 读 ELF 二进制、`test_m4k1_story_cloud_link.acceptance.py` 命中全为 REVISION LOG 注释（census「去注释」过滤合理）、`boot_auto_connect.acceptance.c` 无 64-hex 冻结常量（运行时读，「展开核过」声明采信）。**终判：census 结论「raw 齿目标 = prompt v6–v9 四件」在枚举命令覆盖面之外亦成立，采信**；枚举命令面偏窄记为注记、不打回。
- **基线复现**：`pytest -k immutable` → 5 passed（V6 毒化齿假绿在列，实测）✓；全目录 → 164 passed / 4 deselected / 9 subtests passed，与 §3 基线逐字同数 ✓。
- **spine 一致性**：faceMap 五行（req/arch/contract 不动 + implementation 一行 + governance 两件）与 PROPOSAL 三 T 逐一对应 ✓；facePlan 顺序 T1→T2→T3 与「互为前置、同批不可拆」论证一致 ✓；triage 地板路由留痕（openQ q2）对照 change-intake §1「不改实现面行为 → ① 直接修免门」判据适用成立（冻结常量对齐 canonical blob，断言语义与判据形态均不变），L0 裁定 + human 立项 ack「合并一批」在案 ✓；anchors 空为常态（spine 免冻，arch 面锚待 FROZEN 后由主线程回填）。
- **manifest 对账**：`node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL>` ok=true，anchorUnion 恰等于 manifest 三路径 = §2 各 T 目标文件并集 ✓。
- **§5 验证集**：七条全机械可判、pre-commit / post-commit 时点标注明确 ✓；重物化机制（`rm` + `git checkout --`，eol 钉生效后 checkout 产 LF）成立 ✓。
- **sweep 表 UNCLASSIFIED=0** ✓；非目标（不 renormalize 全仓、不改 prompt 本体、不动判据形态、固件批另批）合理，与 human 四答 + 补裁一致 ✓。

**打回项**：

- **BLOCK-R1-1 · T2 改法未同步 :355 docstring 旧值，落改后同文件自相矛盾**。`test_prompt_v7_round_routing.acceptance.py:355` docstring 记「v6 冻结时点 sha256 = bf5c6b4c…（260925-prompt-v7-round-routing 批冻结）」；T2 承重替换块只覆盖 :122–124，改后常量为 `425d293d…` 而 :355 仍单记旧值且无重算注记——该行正是齿测试自身的期望值记录位，未来维护者按其对 hash 会复刻本批 TECH 条目的「内容被回改」误诊；且与 :122 新注释「旧值为 CRLF 伪影、已重算」的处置不对称（本批病灶定性即「注释与实际口径矛盾」，不应留同型新矛盾）。PROPOSAL §2 T2 证据段已点名 :355，起草时在场而改法未处置。**正形**：T2 替换块扩一行覆盖 :355，将该 docstring 行同步为新值 + 重算注记（如「v6 冻结时点 sha256 = 425d293d…（260925 批冻结、261013 批重算对齐 canonical blob——原值 bf5c6b4c… 为 Windows CRLF 检出伪影）」或等义表述）；仍限同文件、不扩 landing-manifest、不新增验证项（§5 验证 4/5 已覆盖该文件行为面）。

**注记（不阻塞，供 FROZEN 前顺手处置或 retro 回收）**：

1. census 枚举命令下次建议扩至 `server/deploy/` 与 `server/gateway/`（本次判官补扫证实结论不受影响）。
2. §3 sweep 表治理面标签精度：`.gitattributes` 标 G2 实为仓根检出配置（G1–G3 表无对应行，属治理面近似）；`merge-to-main/SKILL.md` 标 G3，按 DEV-GOVERNANCE §6 `skills/**` 属 G2；acceptance py 标 G2 实为实现面。三者无流程差（Track B + human ack + landing 门同链），建议 FROZEN 前顺手修正。
3. 重物化五件只覆盖 v5–v9；家族成员 v1–v4/v10 留 CRLF 盘态（无 raw 齿、text 属性下 status 恒干净），可接受，下次 checkout 自然归位。
4. PROPOSAL §0→§2 编号跳 §1 与 canonical 模板一致（模板本身无 §1），非缺陷。

**R2 入口**：仅 BLOCK-R1-1 一项。主线程修订 T2 替换块后本判官 R2 复核，预期 GREEN；R2 复核面 = :355 修订 diff + landing-manifest 不变确认。

### R2 · 2026-10-13 · architect

**结论：GREEN（无打回项）**——BLOCK-R1-1 修复经判官窄复核成立；v0.2 全部修订（T2 两处替换块 + sweep 表类别修正 + census 补扫注记）与 R1 正形一致，无新 BLOCK。

**R2 复核实录（窄面 + v0.2 增量，2026-10-13 本 worktree 实测）**：

- **BLOCK-R1-1 修复核验**：T2 承重替换块已扩为同文件两处。① :122–124 常量块「替换前」三行与文件现行行 122/123/124 **byte 级逐行全 MATCH**（python 行级比对）；② :354–355 docstring「替换前」两行与文件现行行 354/355 **byte 级全 MATCH**，且旧值行 `v6 冻结时点 sha256 = bf5c6b4c…` 经全文件扫描**唯一**（无第三处残留引用——仓级 grep 佐证：`bf5c6b4c` 在目标文件仅 :124/:355 两处，其余命中均在证据/历史件）；③ 「替换后」两块均为完整句形态、语义与 R1 正形一致（新值 `425d293d…` + 260925 冻结 / 261013 重算对齐 canonical blob / 原值 CRLF 伪影三要素齐），替换后 docstring 为合法 Python 三行 docstring；④ 仍限同文件、不扩 manifest、不新增验证项——与 R1 正形四约束逐一相符。
- **manifest 不变确认**：landing-manifest 围栏块仍恰三路径（`.gitattributes` / `server/acceptance/test_prompt_v7_round_routing.acceptance.py` / `.claude/skills/merge-to-main/SKILL.md`）；`landing-fidelity-check.mjs --preflight` 复跑 ok=true、anchorUnion 仍恰等于 manifest 三路径。
- **v0.2 增量核验**：sweep 表 SKILL.md G3→G2（引 DEV-GOVERNANCE §6）与 acceptance py 改标「实现面 · ①直接修并批」两处类别修正正确（R1 注记 2 处置）；census 补扫注记行已追加（R1 注记 1 处置，结论引 §7 R1）；状态行升 v0.2 如实；§7 R1 记录 append-only 完好未改。
- **独立旁证（判官仓级 grep 偶得，录入以强固 T2 定性）**：`delivery/taskpackages/260924-perf-k1-roundtrip-latency/evidence/latency-baseline-260924.md:80` 早已记录「ECS 渲染件 v6 sha256 = 425d293d…cc62 == 仓内 LF 规范化后 sha256；仓内 raw CRLF = bf5c6b4c…」——canonical 值在 2026-09-24 的 live 实测中即已现形，与 T2「重算对齐 canonical」的取值独立互恰。该件为历史 evidence（append-only），无需动。

**注记（不阻塞，FROZEN 前顺手修正一行；prose-only、无机械消费方、不影响 landing 对账）**：

- census 补扫注记行的目录归组错置：probe_gate.py 实在 `server/deploy/perf-k1/`、reminder_schedule_lib.py 实在 `server/deploy/openclaw-patch/`（两者均在 `server/deploy/` 子树而非 `server/gateway/`）；versions.py 在 `firmware/scripts/`；`server/gateway/` 的实际命中件 = reminders.py（运行时字符串散列，原注记漏列）。census 终判（「raw 齿目标 = prompt v6–v9 四件」）不受影响。建议 FROZEN 前把该行归组改为：`server/deploy/`（verify-deploy.sh 行尾归一 md5 / perf-k1/probe_gate.py LF 归一 / openclaw-patch/reminder_schedule_lib.py 运行时字符串散列）+ `server/gateway/`（reminders.py 运行时字符串散列）+ `firmware/scripts/`（versions.py ELF 二进制）。

**R2 后状态**：立项 ack（在案）+ GREEN（本节）→ 主线程可标 FROZEN 并走 packet-bound landing 序列（landing-manifest 绑定 + landing-verifier 独立核验 + retro + 前置升级闸）。
