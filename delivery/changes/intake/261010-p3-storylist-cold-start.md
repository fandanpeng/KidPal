---
changeId: 261010-p3-storylist-cold-start
sources: 2026-10-10 human 真机使用反馈第 5 条「P3 停默认空态、没有把已有的 story list 列出来」（TECH-261010-p3-storylist-cold-start-empty 条目 source 转写）；关联引用 = TECH-261010-p3-storylist-cold-start-empty、260928-story-connect-push-defer（plan.md 改法节挂起重裁）、REQ-261010-p1-schedule-empty-state（同族冷启动空态）、ARQ-260911-cross-package-behavior-owner-gap、决策 24（板端 RAM 缓存重启即失）、ADR-005 / 契约 §1.3② L85（data.restore 先例）
faceMap: 需求面动（S-F2.1 补充规则 + P3 空态两态话术定稿，requirements.md:408/:267 不对称）；架构面动（arch §5.2 场景④ :213 story_list 推送时机权威行——恢复时点①重载）；契约面疑似动（story_list data.restore additive，复刻 §1.3② L85 先例——仅 restore 标记方案下成立，取舍见 openQ）；实现面动（front_app.py 建连恢复推送 + mcp_server.cc restore 分流——本门②档 Full 已裁，plan = 261010-p3-storylist-cold-start-restore）；governance 不动
facePlan: ① 需求面先行（/requirement-change loop：story-author 起草 S-F2.1 补充规则 + 空态两态话术，requirement-reviewer 审稿 + 档 2 architect 必派，可与 REQ-261010-p1-schedule-empty-state 并批裁定——两 CRQ trigger 字段均点名同族可并批；human 拍措辞）；② 架构面随需求面批同批落（arch §5.2 场景④行重载，Track B 小批或需求批档 2 architect 派遣时同批）；③ 契约面（若取 restore 标记）：architect-reviewer 四分类（预期 CONTRACT_CHANGE_REQUIRED，additive，引 §1.3② L85 / ADR-005 先例）→ human 契约门 → 与实现同批原子落；④ 实现面：reviewed plan 已出（261010-p3-storylist-cold-start-restore，Full），launch 前置 = ①②③ anchors 回填（spine 源条款：先于实现面的 anchors 未回填 = 承载包不得 launch）
anchors: {}
openQ: OQ-1 restore 标记（触契约面 additive、P2 先例对称、语义显式——推荐）vs 固件本地页 gating（wire 零变化但改 K2 消费面「非 VOICE 前台一律推前台」既有语义）——呈 human 随立项 ack/契约门一并裁。
status: intake
---

# spine · P3 story_list 冷启动（261010-p3-storylist-cold-start）

> 触面 ≥2 跨面变更的路由账本（change-intake.md 程序；spine 免冻——各面冻结语义由各面制品承载，anchors 记录冻结事实）。触点① human 已确认（2026-10-10）：dev 面 = ②单包 Full（restore 标记方案）、需求面与 REQ-261010-p1 并批走 /requirement-change、契约面过 architect-reviewer 四分类 + human 契约门。

## anchors 回填记录

（各面定稿后由主线程回填）

## openQ 账

- OQ-1（2026-10-10 触点①呈 human）：restore 标记（推荐，触契约 additive）vs 固件本地页 gating（wire 零变化、改 K2 既有消费语义）——human 选 restore 标记方案（触点①第三问答「确认」）。
