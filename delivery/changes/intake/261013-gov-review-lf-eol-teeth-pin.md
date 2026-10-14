---
changeId: 261013-gov-review-lf-eol-teeth-pin
sources:
  humanVerbatim:
    - "分析一下 今天 新增的这两个TECH ，今天把他处理了"
    - （2026-10-13 四答）"lf-hash 先，固件后（推荐）" / "census 后按命中集合钉（推荐）" / "本批一并做 CMake 必生成" / "过 /triage-change 判档（推荐）"
    - （2026-10-13 V6 毒化发现后补裁）"合并一批，走 intake spine"
  refs:
    - docs/crq/tech/TECH-261013-lf-hash-teeth-vs-windows-crlf-checkout.md（本批直接承载：CRLF 齿伪红 + .gitattributes 缺钉）
    - docs/crq/tech/TECH-261013-lang-config-thinking-key-regen-landmine.md（不承载——固件批另行，走 /triage-change，见 facePlan 注记）
    - 先例 ARQ-260906-shell-scripts-lf-gitattributes（.gitattributes eol=lf 钉的形态与注释惯例）
    - 本日 census 机械枚举（见 PROPOSAL §3 证据）
faceMap:
  requirements: 不动——本批纯工具链/验收齿面，零 story 语义变更
  architecture: 不动——零架构面文档触碰（.gitattributes 为仓配置非 arch 面）
  contract: 不动——prompt 部署面非契约面；docs/contracts/** 零触碰（census 判据 = 本批改动文件清单与契约面零交集）
  implementation: 动（一行）——server/acceptance/test_prompt_v7_round_routing.acceptance.py:124 V6_SHA256 常量 CRLF 毒化重算（2026-10-13 census 实测：磁盘 CRLF raw 哈希 bf5c6b4c… = 冻结常量 ≠ canonical blob LF 哈希 425d293d…；注释「v6 为 LF 落盘」不实——齿当年在 Windows CRLF 检出上冻结，现反向强制 CRLF 态、Linux/CI LF 检出必红）
  governance: 动——.gitattributes 增 `server/deploy/openclaw-system-prompt.*.md text eol=lf`（census 命中族钉：raw 齿 v6/v7/v8/v9 四件 + 免疫面 v5 同族无害）+ .claude/skills/merge-to-main/SKILL.md 失败信号→处置节加 raw-hash 齿误诊提示一条（TECH 条目建议③；独立排障手册不存在，最近位 = merge-to-main skill）
facePlan:
  - 单批承载（human 2026-10-13 裁「合并一批，走 intake spine」）：治理面 Track B 轻路径承载全部三 T；PROPOSAL 落 delivery/changes/arch/261013-gov-review-lf-eol-teeth-pin/
  - 实现面 V6 齿常量（faceMap implementation 行）：按 /triage-change 地板判 ① 直接修——零实现行为改动（测试文件冻结常量对齐 canonical blob，非行为变更），免独立 triage 会话；仍受 Track B architect 评审一并裁（双保险）；human 路由确认 = 立项 ack（合并一批）在案
  - 顺序：T1 .gitattributes 钉 → T2 V6 常量重算 → T3 skill 注记；landing 后统一跑齿验证（immutable 五齿 + 全 acceptance 目录）；V6 常量修正与 eol 钉互为前置（钉生效重物化 v6 为 LF 后旧常量必红，故同批不可拆）
  - TECH-① 条目 closed 翻转随本批 landing 完成 + verifier 绿 + retro 后由主线程执行（crq-render 重建 INDEX）；TECH-② 不在本 spine 内，固件批另行开变更（human 已裁 /triage-change 路由 + 本批一并做 CMake 必生成）
anchors:
  arch: "PROPOSAL FROZEN v0.2（R1→R2 GREEN）→ packet-bound landing（commit 5f2e431）→ landing-verifier GREEN（首验 RED 仅制品件形态、canonical 零返工）→ retro 齐；制品五件 = delivery/changes/arch/261013-gov-review-lf-eol-teeth-pin/"
  imp: "V6 齿常量重算按 triage 地板 ① 直接修并批（零行为变更，openQ q2 L0 裁定在案）；随 T2 落地于 test_prompt_v7_round_routing.acceptance.py（:122–124 + :354–355），landing-verifier 25/25 断言核过"
openQ:
  - id: q1-批量形态
    question: V6 齿常量毒化怎么纳入？（分两批 dev 先行 / 合并一批走 spine / Track B 先行齿修 defer）
    ruling: human 2026-10-13 裁——合并一批，走 intake spine
    status: resolved
  - id: q2-齿修门路由
    question: 实现面一行常量修正走独立 triage 还是并入 Track B？
    ruling: L0 主线程裁——triage 地板判据「不改实现面行为 → ① 直接修免门」适用（冻结常量对齐 canonical 零行为变更）；并入 Track B 单批，architect 评审对 T2 改法实质把关（依据：change-intake §1 比例化）
    status: resolved
status: closed
