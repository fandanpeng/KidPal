# architect-landing-summary · 261013-gov-review-lf-eol-teeth-pin

packet-id：FROZEN v0.2 PROPOSAL §2 改法集（T1–T3）+ §5 验证集（7 条） · 评审轮次 R1（1 BLOCK 已修）→ R2 GREEN · change-id 261013-gov-review-lf-eol-teeth-pin

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 冻结齿目标文件缺 eol=lf 钉 | `.gitattributes` | 文件尾追加注记块 + `server/deploy/openclaw-system-prompt.*.md text eol=lf`；既有条目（`*.sh` / 五处 `merge=union`——含 crq INDEX 两行自身 / crq 归档 INDEX）零改动 |
| T2 V6 齿冻结常量 CRLF 毒化 | `server/acceptance/test_prompt_v7_round_routing.acceptance.py` | 两处 byte 级替换：:122–124 `V6_SHA256` → canonical `425d293d…` + 重算注记；:354–355 docstring 期望值记录位同步（BLOCK-R1-1 修正面） |
| T3 merge 侧高频误诊无提示 | `.claude/skills/merge-to-main/SKILL.md` | 「失败信号 → 处置」节 `git diff --check` 锚 bullet 后插入 raw-hash 齿误诊 bullet 一条，其余零改 |

## 2. 验证证据

1. `git check-attr text eol`（v5–v9）→ 5 文件 × 2 属性全 `text: set` / `eol: lf` → 绿。
2. 重物化（rm + `git checkout --` 五件）后 `git ls-files --eol` 全 `i/lf w/lf`；python 字节探针 `b'\r\n' not in b` v5–v9 全过 → 绿。
3. `git cat-file blob HEAD:server/deploy/openclaw-system-prompt.v6.md | sha256sum` = `425d293d35b106b581785a78055f5718068531b240d52db763591eed68b0cc62`，与工作树 raw 哈希、新常量三处一致（blob 零回改）→ 绿。
4. `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance -k immutable` → **5 passed**（V6 齿假绿转真绿）→ 绿。
5. 同命令全目录 → **164 passed, 4 deselected, 9 subtests passed**（与 PROPOSAL §3 基线 2026-10-13 实测逐字同数）→ 绿。
6. `node .claude/scripts/landing-fidelity-check.mjs --preflight <PROPOSAL>` → ok=true，anchorUnion 恰 = manifest 三路径（起草期 + v0.2 后两次）→ 绿。
7. post-commit 复跑（验证 4 + 5 + `git status` 五件 prompt 干净）→ landing-verifier 绿后 commit 时执行。

landing-verifier 独立复核（2026-10-13）：一次性 25 断言比对脚本 25/25 PASS——T1 HEAD 前缀逐行零改 + 尾部纯追加；T2 替换前块 HEAD blob 唯一命中 + HEAD⊕替换 == 工作树 byte 全等 + 三处哈希一致；T3 新 bullet 恰 1 处、插入位锚定；prompt v5–v9 `git diff HEAD` 全空；改动集恰 == manifest 三件零夹带。首轮 RED（本 summary 未按 `architect-landing-summary.md` 形态落、缺 packet-id 行与 manifest 围栏）已由本件修正，canonical 三文件 diff 本体零返工。

## 3. 诚实注记

- 破坏性自检：prompt v5–v9 文件本体零回改（blob 零 diff，磁盘仅 eol 物化）；齿语义「历史版本零回改」保持；其余 acceptance 测试判据形态零触碰。
- 重物化覆盖 v5–v9 五件；v1–v4/v10 保留检出态盘面（无 raw 齿钉、`git status` 恒干净）——verifier 判可接受，属允差。
- census 枚举命令面偏窄（未含 `server/deploy/`、`server/gateway/`）——由 architect R1 全仓补扫补齐，终判不受影响（PROPOSAL §3 注记 + §7 R1/R2 在案）。
- 留根：TECH-261013-lf-hash 条目 closed 翻转随本批收口（landing 完成 + verifier 绿 + retro 齐后主线程执行）；姊妹条目 TECH-261013-lang-config-thinking-key-regen-landmine 固件批另行。

```landing-manifest
.gitattributes
server/acceptance/test_prompt_v7_round_routing.acceptance.py
.claude/skills/merge-to-main/SKILL.md
```
