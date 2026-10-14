---
id: TECH-261013-lf-hash-teeth-vs-windows-crlf-checkout
title: 冻结齿按 LF 原始字节哈希钉 prompt 版本文件，但 .gitattributes 未钉这些文件的 eol=lf——Windows autocrlf=true 检出即 CRLF，三条不可变齿恒红并拦死 merge-driver 合并通道
domain: tech
status: closed
severity: MAJOR
owner: 治理面（.gitattributes 钉换行，ARQ-260906 同型先例的范围外续）
trigger: 下次 gov 批顺带处理；或 Windows 检出/合并再踩时
opened: 2026-10-13
closed: 2026-10-13
closedBy: 261013-gov-review-lf-eol-teeth-pin（Track B 全环：frozen v0.2 + R1→R2 GREEN + landing-verifier GREEN；eol=lf 族钉 + V6 齿常量毒化重算 + skill 注记）
paths: [".gitattributes", "server/deploy/openclaw-system-prompt.v7.md", "server/deploy/openclaw-system-prompt.v8.md", "server/deploy/openclaw-system-prompt.v9.md", "server/acceptance/test_m4k3_record_session_live.acceptance.py", "server/acceptance/test_prompt_v7_round_routing.acceptance.py"]
source: 2026-10-13 主线程 dev-loop merge-driver 实跑（261013-qq-schedule-write-closure）被三条不可变齿拦死 + 归因实录
handoff: 治理面（.gitattributes 一行级修补，走 Track B；本条目只登记不实施）
---

**证据**（2026-10-13 实录）：

1. **拦截实录**：merge-driver `--pod 261013-qq-schedule-write-closure` 内嵌校验全 acceptance 目录跑出 3 红（`3 failed, 161 passed`）：`test_a8_v7_prompt_immutable`（test_m4k3_record_session_live.acceptance.py:899）+ `test_pn_a3_v8_prompt_file_immutable` / `test_rf_a3_v9_prompt_file_immutable`（test_prompt_v7_round_routing.acceptance.py）→ merge-driver 将 pod 置 `blocked`，合并通道停摆。
2. **定性 = Windows 检出换行伪影、非内容回改**：测试计算式 = `sha256(read_bytes())`（test_prompt_v7_round_routing.acceptance.py:139-140 `_sha256_raw`），自注「v8/v9 为 LF 落盘，raw == 换行归一双口径同值」（:49/:86）；实测 v9 磁盘 raw sha256 = `1acb2b51…`（CRLF）、LF 归一后 = `0b051e14…` = 冻结期望值；`git cat-file blob HEAD:server/deploy/openclaw-system-prompt.v9.md | sha256sum` = `0b051e14…`——**仓库 blob 为 LF，工作树被 `core.autocrlf=true`（本机实测 true）检出成 CRLF，raw 哈希即伪红**。
3. **.gitattributes 覆盖缺口**：`git check-attr text eol -- server/deploy/openclaw-system-prompt.v9.md` = 双 unspecified；根 `.gitattributes` 现仅钉 `*.sh text eol=lf`（ARQ-260906-shell-scripts-lf-gitattributes，其注记自述「范围克制在 *.sh」——彼时 prompt 哈希齿尚未诞生，本缺口的成年期在其后）。
4. **修复实证**：原地 LF 归一三文件后（`raw.replace(b'\r\n', b'\n')`；blob 未变、`git status` 经 `git add` 刷新 stat 后干净）→ 三齿全绿（全目录 `164 passed`），merge-driver 复跑 `quick-check OK (all 2 gates)` → 合并通过。**零内容改动即转绿 = 伪红确证**。
5. **影响面**：Windows 检出（autocrlf=true）上的任何 merge-driver/evidence 轮都会撞；归因成本高（初诊为「历史哈希漂移/内容回改」，实际是换行）；且每次 `git checkout`/rebase 会再转 CRLF → **per-checkout 复现**，本地修复不持久。Linux 检出/CI 不受影响。

**建议改动**（候选，交治理面 Track B、不自批）：① `.gitattributes` 增一条 `server/deploy/openclaw-system-prompt.*.md text eol=lf`（或泛化 `server/deploy/*.md text eol=lf`——泛化面需评估：其余 deploy md 无 raw 哈希齿，但统一 LF 无副作用）；② 顺带 census：全仓还有哪些「按 raw 字节哈希/diff 判据冻结」的文件（`grep -rn 'sha256\|read_bytes' server/acceptance/ firmware/main/test/` 类机械枚举），对全部命中集合一次性钉 eol，消同型复发——ARQ-260906 的「范围克制」彼时合理，本次证明「按需逐步扩」的模式每遇新齿即复发一次；③ 在 dev-loop/merge 排障手册加一句高频误诊提示（raw-hash 齿红 → 先 `file <file>` 看换行，再怀疑内容）。

**风险**：riskTier: low——一行级 .gitattributes 增补 + census 只读枚举；不触契约、不触实现面；但属治理面工具变更、走 Track B 轻路径。owner = 治理面。

**收口（2026-10-13，261013-gov-review-lf-eol-teeth-pin 批）**：① eol=lf 族钉落地（`.gitattributes` 尾 `server/deploy/openclaw-system-prompt.*.md text eol=lf`，census 命中族 = raw 齿 v6–v9 四件 + 免疫同族 v5，家族 glob 出生即钉）；② census 追加发现 V6 齿冻结常量本身被 CRLF 毒化（`bf5c6b4c…` = 磁盘 CRLF raw ≠ canonical blob `425d293d…`，注释「v6 为 LF 落盘」不实）——齿当年在 Windows CRLF 检出上取值、反向强制 CRLF 态，已重算对齐 canonical（:122–124 + :354–355 同步，prompt 本体零回改）；③ merge-to-main skill 失败信号→处置节加 raw-hash 齿误诊 bullet。双向 census 纪律（红面 + 假绿面）记入批 retro §4 建议 1。证据：delivery/changes/arch/261013-gov-review-lf-eol-teeth-pin/（PROPOSAL + spine + architect-landing-summary + landing-verdict + retro 五件齐）。
