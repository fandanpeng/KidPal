---
changeId: 261010-p3-storylist-cold-start
sources: 2026-10-10 human 真机使用反馈第 5 条「P3 停默认空态、没有把已有的 story list 列出来」（TECH-261010-p3-storylist-cold-start-empty 条目 source 转写）；关联引用 = TECH-261010-p3-storylist-cold-start-empty、260928-story-connect-push-defer（plan.md 改法节挂起重裁）、REQ-261010-p1-schedule-empty-state（同族冷启动空态）、ARQ-260911-cross-package-behavior-owner-gap、决策 24（板端 RAM 缓存重启即失）、ADR-005 / 契约 §1.3② L85（data.restore 先例）
faceMap: 需求面动（S-F2.1 补充规则 + P3 空态两态话术定稿，requirements.md:408/:267 不对称）；架构面动（arch §5.2 场景④ :213 story_list 推送时机权威行——恢复时点①重载）；契约面疑似动（story_list data.restore additive，复刻 §1.3② L85 先例——仅 restore 标记方案下成立，取舍见 openQ）；实现面动（front_app.py 建连恢复推送 + mcp_server.cc restore 分流——本门②档 Full 已裁，plan = 261010-p3-storylist-cold-start-restore）；governance 不动
facePlan: ① 需求面先行（/requirement-change loop：story-author 起草 S-F2.1 补充规则 + 空态两态话术，requirement-reviewer 审稿 + 档 2 architect 必派，可与 REQ-261010-p1-schedule-empty-state 并批裁定——两 CRQ trigger 字段均点名同族可并批；human 拍措辞）；② 架构面随需求面批同批落（arch §5.2 场景④行重载，Track B 小批或需求批档 2 architect 派遣时同批）；③ 契约面（若取 restore 标记）：architect-reviewer 四分类（预期 CONTRACT_CHANGE_REQUIRED，additive，引 §1.3② L85 / ADR-005 先例）→ human 契约门 → 与实现同批原子落；④ 实现面：reviewed plan 已出（261010-p3-storylist-cold-start-restore，Full），launch 前置 = ①②③ anchors 回填（spine 源条款：先于实现面的 anchors 未回填 = 承载包不得 launch）
anchors: {req: "requirements.md §9 v1.63（2026-10-12 档 1 定稿、frozen；S-F2.1 AC1 限定 + AC5/AC6 空态两态 + 补充规则冷启动恢复 + WF-F2-01 注记）", arch: "arch §5.1.2 :186 front 网关行三时点 + §5.2 场景④ :214 story_list 权威行重载（restore 标记、随契约门挂法）——随 v1.63 批同拍落（story-author 镜像同批义务）", contract: "契约门 HUMAN_APPROVED 2026-10-12（story_list data.restore additive，D1–D4 四处 diff；packet + gate evidence = delivery/taskpackages/261010-p3-storylist-cold-start-restore/contract-gate/；diff 与实现同批原子落——落地通道 CCP 先例：逐字 patch 过目 → git apply；closeout C1 = ECS 开关默认关、烧板验证后人工开闸）"}
openQ: OQ-1 restore 标记（触契约面 additive、P2 先例对称、语义显式——推荐）vs 固件本地页 gating（wire 零变化但改 K2 消费面「非 VOICE 前台一律推前台」既有语义）——呈 human 随立项 ack/契约门一并裁。
status: closed
---

# spine · P3 story_list 冷启动（261010-p3-storylist-cold-start）

> 触面 ≥2 跨面变更的路由账本（change-intake.md 程序；spine 免冻——各面冻结语义由各面制品承载，anchors 记录冻结事实）。触点① human 已确认（2026-10-10）：dev 面 = ②单包 Full（restore 标记方案）、需求面与 REQ-261010-p1 并批走 /requirement-change、契约面过 architect-reviewer 四分类 + human 契约门。

## anchors 回填记录

（各面定稿后由主线程回填）

- **req**（2026-10-12）：requirements.md §9 v1.63 定稿（frozen）——档 1 走 author→reviewer 闭环（reviewer 四查全 PASS、机检绿），human 话术三答在案（无内容空态「本子空空的，讲个故事点亮第一颗星吧」/ 数据未达空态「本子还没翻开，等等再来看看吧」/ 计数 = 无内容显 ×0、数据未达不显）。
- **arch**（2026-10-12）：arch §5.1.2 :186 + §5.2 场景④ :214 镜像句随 v1.63 批同拍落（含 restore 标记「随契约门落、尚未落地」挂法、260928 摘除沿革衔接）。
- **contract**（2026-10-12）：architect-reviewer 四分类（面 A `data.restore` additive = CONTRACT_CHANGE_REQUIRED；面 B §1.5 :180 枚举句 = NO_CONTRACT_CHANGE、随批自愈）→ human 契约门 **批准**（AskUserQuestion 签署）。diff D1–D4 未落——与实现批同拍原子落（落地通道 CCP 先例：逐字 patch 过目 → git apply；closeout C1 = ECS 恢复推送开关默认关、烧板验证后人工开闸）。ADR-010 随批。launch 前置三 anchors 齐 → 实现面 261010-p3-storylist-cold-start-restore 就地起 loop（m2-char-window 先例：本树既有会话即 Pod 本体）。
- **contract·落地态**（2026-10-12）：契约 diff D1–D4 已随 261010-p3-storylist-cold-start-restore 实现同批原子落（C2 兑现）——逐字核对 = gate evidence §5（五条净内容行 needle 逐字 HIT + 5 added / 2 removed + CRLF 保全字节级实证）；JSON 三例回归过 V27 floor（gate evidence §7）；gate 状态翻 LANDED·CLOSED（§10，C1 = ECS 开关默认关、烧板验证后 human 人工开闸，仍 pending 归 human）；ADR-010 随批新增；CDQ-261010 LANDED 行 + arch :186/:214 挂法句翻既成 = planner 代落（integrator scope-map 通道错位，见 review-packet Findings）。

## openQ 账

- OQ-1（2026-10-10 触点①呈 human）：restore 标记（推荐，触契约 additive）vs 固件本地页 gating（wire 零变化、改 K2 既有消费语义）——human 选 restore 标记方案（触点①第三问答「确认」）。
- OQ-2（2026-10-16 整改留痕，261015-gov-review-changes-taxonomy T5 第 3 条）：frontmatter `status: landed` 值域外（枚举 = change-intake.md §2.7 七值）——整改翻 `closed`。依据：anchors 三面已回填（req/arch/contract 三行在册）、契约门 HUMAN_APPROVED 2026-10-12（spine anchors.contract 行）、实现面 261010-p3-storylist-cold-start-restore 已收口（dev INDEX closed）——状态机完整闭环，closed 为实判。
