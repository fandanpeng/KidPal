---
changeId: 260928-char-window-key-triggered-play
status: closed（2026-09-28：需求面档 2 v1.59 落稿 + 架构镜像句同批 + 实现面 Pod 收口（verifier 全绿 + supervisor ESCALATE 人批消解 + closeout 过）；遗留三项均在人侧、非面门 = ① 分支合 main（merge-to-main，human 发起）② remote push（human）③ 真机持板验证轮（deferred 登记）+ loop-retro cadence（advisory 阈值已到，发起责任主线程））
sources:
  human-intent-verbatim:
    - 「（2026-09-26 首提）查字的需求我想调整一下，当问什么字的时候 直接在 田字格里显示出来打字，右侧显示出来解释，按Key键 播报的时候再开始播报」
    - 「（2026-09-26 三轮收敛后定稿口径）我们换个思路：如果现在的播报还是正常的，就跟原来一样正常播报。只是一开始直接显示完整的字，点击播放的时候，它才开始逐笔去绘画、写那个字。」
  human-rulings-verbatim:
    - 「同意取消（演完歇 2 秒自动重播）」——2026-09-26
    - 「不缓存，改成按Key 现取」——2026-09-26 提出、同日经三路线对照后由定稿口径取代（现取路线撤回，播报保持现状）
    - 「architecture 一起改」——2026-09-26
    - 「走trackB 不要走tacka」——2026-09-26
    - 「『笔名顺序与演示一致』，同意删掉，用这个方案」——2026-09-28
    - 「你读最新 triage 拆包规则，dev loop 流程，然后把这个需求变更 走 triage 走完整dev loop」——2026-09-28（立项 ack：四面对齐「为什么改 / 往哪改 / 改动多大」已于 2026-09-26/28 会话内完成并获认可，本指示即放行全流程）
  refs:
    - delivery/changes/arch/260926-arch-review-char-window-key-triggered-play/PROPOSAL.md
    - S-F3.1 · S-F3.2 · S-F3.3 · NFR-PERF-2
    - delivery/workstreams/ui-v5-align/findings.md §1.3（keybar 键帽值域既有 gap）
faceMap:
  requirement:
    touched: yes
    detail: docs/requirements.md——§2.2 查字窗行(:68)、§2.3 手势矩阵查字行(:146)、§2.4 P4 撤页注(:181)、F3 窗口交互包(:462)、WF-F3-01 线框(:466-480)、S-F3.1 AC1/AC3/AC4 + 新增 AC5(:487-494)、S-F3.3 AC1 + 线框行(:514/:517)、NFR-PERF-2 阈值终点(:569)、头部版本行(:7)与 §9 追加 v1.59 行。变更档位 = 档 2 结构性（新增 AC、NFR 阈值语义变更、跨 story 连锁 ≥2 处）。
  architecture:
    touched: yes（联动同步，不独立开面）
    detail: docs/architecture.md :111（查字延迟句「到笔顺演示开始」）、:211（场景③「KEY 短按重播」+ 版本串）——两处为 requirements 口径逐字镜像句，随需求面档 2 同批同步（裁定见 openQ Q3，human 终拍一并批）。
  contract:
    touched: no
    detail: 零触点（逐条核证见 260926 PROPOSAL §3 + 2026-09-26 会话取证）：D3 笔顺 JSON、E1 view=char、TTS 链路（引擎 edge-tts）、kidpal_session_ctx source_page 闭集全部不变；不新增端→网关消息。无契约门。
  implementation:
    touched: yes
    detail: firmware/main/display/char_page.cc——建窗初始态改静态整字（REST 相位即整字，char_stroke.c:501-525 既有相位复用）+ 不建动画计时器；kidpal_char_page_replay()(:499-503) 改「未起播→起 / 已起播→重播」+ started 标志。firmware/main/display/keybar.c——查字窗键帽值域补齐（既有 gap 非本批引入）。估 ~20 行、单文件簇、低风险。播报链路（application.cc 音频门 / mcp_server.cc char 路由 / protocols/** / server/** / 云端）零改动。→ /triage-change 分档。
  governance:
    touched: no
    detail: 探测无命中——不改 .claude/** 任何 agent 定义 / gate / settings。
facePlan:
  order:
    - "1. 需求面档 2（上游，先）——story-author 起草（含牵连面清单）→ requirement-reviewer 四查 → human 终拍（architect 可行性覆盖确认 + 架构镜像句同批一并批）→ 主线程落稿 requirements.md + architecture.md + §9 v1.59 行 → 回填 anchors.req / anchors.arch"
    - "2. 实现面（下游，后）——/triage-change 分流门：architect 比例化 review 四档 → human 触点①确认 → reviewed plan 落 delivery/taskpackages/<id>/plan.md → planner /build-feature <id> 完整 dev loop（acceptance-author → builder → verifier↔fixer → integrator → supervisor 机械闸）"
    - "3. launchPreconditions——anchors.req 与 anchors.arch 未回填前，承载实现包不得 launch（change-intake.md §8）"
anchors:
  req: "v1.59 已落稿（2026-09-28）——docs/requirements.md §9 追加 v1.59 行即定稿标记（requirement-change.md 定稿载体条款）。档 2 全流程走毕：story-author 起草（A1–A14 + 否回补稿 B1–B8）→ requirement-reviewer 四查（首查 FAIL 3 否回 → 补稿后复审 PASS）→ human 终拍批准（2026-09-28：批准落稿 / 架构同批 / 日期列 2026-09-28）；spec-anchor-check 落稿后复跑绿（60/46/13/31、errors=[]，与基线逐字段一致）。"
  arch: "v1.59 同批落稿（2026-09-28）——architecture.md :111 查字延迟句 + :211 场景③ 两处镜像句同步（openQ Q3 裁定，human 终拍确认「确认同批改」）。"
  contract: "none（零触点，无需门，无回填物）"
  imp: "260928-char-window-key-triggered-play（taskpackage.json = 执行 ledger；Pod 循环 2026-09-28 收口）——verifier r1/r2 簿记红自愈 → r3 scoped 绿 / r4 final 绿（106T/0F、既有 103 齿零回归、idf.py build 过、红基线 3 红全为本包新齿设计态）；supervisor 机械闸 ESCALATE（基线驮前序已批 commit 触发 security 轴路径，非本包问题）→ human 拍板「人批升级合并」（AskUserQuestion 2026-09-28）→ human-decision eventId 84c4e953 + agent-escalation-resolved 4368504c 落账；closeout-check 过（docs-delta/review-packet 在场、INDEX inserted）；commit d39ff23/d60e08e/100a4aa + 收口尾注落分支。真机轮 deferred（review-packet 未覆盖面段登记），合 main / remote push 留 human。"
openQ:
  - id: Q1
    q: 既有 260926-arch-review-char-window-key-triggered-play PROPOSAL（Track B 草案，DRAFT v2.1）如何处置？
    a: 2026-09-28 新规落地（260928-gov-review-requirement-face 立 requirement-change.md 三档流程）后，需求面变更改走三档流程；该 PROPOSAL 不再走 Track B 评审。处置 = 目录保留不删，Status 标注 superseded-by-intake；其 T1–T6「原句逐字→新句逐字」byte 对账改法集（23/23 双向命中）转为本 spine 需求面起草的参考材料。changes-index 该条目状态随处置更新。
  - id: Q2
    q: 需求面路线曾裁定「走 Track B」，现按新规改走 requirement-change 三档，是否冲突？
    a: 不冲突。260926 裁「走 Track B」时三档流程尚未存在；新规落地后需求面变更的程序 SSOT = requirement-change.md（AGENTS.md「需求面变更」节）。三档流程的 human 触点（档 2 意图裁定 + 终拍）不少于 Track B（立项 ack + 评审 GREEN），对抗力度以 reviewer 否回循环 + human 终拍承接。裁定记 openQ，如 human 有异议可在终拍时改回。
  - id: Q3
    q: architecture.md 两处镜像句由谁裁？
    a: 主线程裁定随需求面档 2 同批同步（L2 呈人形态——human 终拍时一并批，不单独开 /arch-change）。理由：两句是 requirements 口径的逐字镜像（描述性引用，非独立架构结论），单独走 Track A 对抗 review 与体量不成比例；260922-arch-review-spec-v5-ui-align 先例即 spec 两文件同批收口。**处置：2026-09-28 human 终拍确认「确认同批改」，已随 v1.59 落稿。**
  - id: Q4
    q: 档 2 流程第 3 步「architect 可行性判断（出场由 human 指定）」是否派发？
    a: human 未指定出场；可行性证据已在 260926 PROPOSAL 起草期取得并经 human 过目（REST 相位即静态整字、代码 ~20 行、零契约触点、播报链路零改、NFR-PERF-2 终点重锚理由）→ 主线程判定为已覆盖、不重复派发；终拍时随呈 human 确认，如要求补派即补。**处置：2026-09-28 human 终拍随拍通过（未要求补派），结论已按档 2 第 6 步浓缩回填 §9 v1.59 行依据列。**
  - id: Q5
    q: 分支拓扑——spec 落稿与实现包落在哪个分支？
    a: 当前分支 dev/260915-m1-k1a-key-layer-dispatch（非 main），工作区另有 ui-v5-align 会话遗留脏文件（delivery/workstreams/ui-v5-align/findings.md、firmware/main/display/lcd_display.cc——非本变更产物，提交时排除）。策略 = spec 落稿先在本分支 commit；Pod launch 时按 kidpal-pod-launch-prep 从当前态建 worktree；合并 main 时机在 Pod 收口后随 merge-to-main 一并定。不阻塞前两步。
  - id: Q6
    q: story-author / requirement-reviewer 两 agent 加载失败（本会话与 headless claude -p 均报 not found），如何处置？
    a: 执行测出 bug（260928 批 ebea91a 落地件），按「执行测出 bug 走快路径」惯例打最小补丁：两文件 frontmatter description 未加引号且值含 `: `（YAML 解析失败、agent 被静默丢弃——全仓 14 份 agent def 枚举对照，可加载的 12 份全部带引号、失败的恰是未加引号的 2 份），补双引号后 headless 探针通过（回「收到」）。跳过流程一次性修复、如实记录本条；残留建议（lint-descriptions 扩「description 含引号缺失」齿）留待 loop-retro 捡起。
