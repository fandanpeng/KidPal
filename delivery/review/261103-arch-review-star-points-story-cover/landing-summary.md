# landing-summary · 261103-arch-review-star-points-story-cover

- 批次：261103-arch-review-star-points-story-cover（架构+契约面 Track B，FROZEN v1.0）
- 落地 commit：b930f498（manifest 三文件 + PROPOSAL FROZEN 翻转，恰四文件）
- 起草 → 评审 → 修订链：DRAFT v0.1 → R1 BLOCK(F1) → v0.2（5f377f08）→ R2 GREEN（affa3712）→ FROZEN → 本落地

## 机械凭据链

1. **装配器**：`assemble-edits.mjs` 43 编辑（architecture 29 / contract 13 / ADR 1）全部恰命中一次后写仓外副本（`C:\tmp\assemble-261103`，输出根在仓外、packet-bound 兜底），逐文件 sha256 记录在装配器输出中；多行清单按目标文件实际 CRLF 行尾生成（首跑 5 锚 CRLF 0 命中即 fail-loud，未写任何文件）。
2. **落地后剥除编者注偏差 21 处**：装配落盘后由 cite-check 逐 fence 揪出起草→落地链上混入的编者注（键位行尾注、场景⑥标题标注、§6.5/§6.2/§5.5x 标注、QQ skill 查账插句、session_ctx 头部沿袭与「ADR-013 沿革不变」、char 句对账注、story_list 四段加引注、⑦⑧ bullet 加引注、§1.4 动作行加引注、delete/page 消息头尾加语、§1.6 prompt 尾注、delete 隐式 ack 括注），逐条以恰命中断言剥除，canonical 与 FROZEN packet 逐字对齐。R1 两条非阻断注记**不落架构正文**、改记此处由下游 brief 承载：① M2K2-A2 冻结验收夹具改写义务 → K2/K5 包 brief 显式点名；② QQ skill 查账只读路径 → K3 包 brief 显式点名（S-F6.4 AC2，读 points.py 状态答询、同课表读 MD 先例）。
3. **16 fence 逐行核验**（自研 `C:\tmp\verify-landing.mjs`，嵌套围栏感知 + cue 手工映射）：T1 五行、T2 组件行/front 段、T3 场景④⑥、T4 三行/逻辑事实段/§6.5、T5 三块、T6 ①③⑦⑧、T8 ctx/delete/page、ADR-018 附件——全部逐行在位；其中 T2front 段 / T4 逻辑事实段 / T6③ / T9ADR 四件因提取脚本 cue 匹配缺陷走特征行 grep 补验（全部命中）。
4. **cite-check（tool v1）**：`ok:false`——**已知盲区，不构成落地缺陷**：该工具按「整块连续包含」匹配，本批三类形态不在 v1 覆盖面（其输出自带注记「blockquote 承载的插入段与多 span / 混合散文行内形态不在 v1 覆盖面」）：① T1 五行 = 行集散落进既有表格槽位（不可重排 canonical 表格）；② T6⑦⑧ 等大块内嵌 ```json 围栏致 checker 围栏配对错位；③ front 段等行内追加形态。替代证据 = 上述 1-3（装配器恰一次断言 + 逐行核验 + 剥除断言链）。
5. **其余组**：需求哈希 landing 前后均 `e775822a3e6c1138d8103de6ac5c281a902f0f0a`（零触 requirements）；spec-anchor-check 基线 A 逐值吻合（78/48/20/44、errors:[]、ok）；preflight ok、anchorUnion = manifest 三件；契约两新 JSON 示例严格解析通过 + 字段断言（revision/balance/target/recent、items/page、整数余额、page.total≠items.length）；正向断言六符号（KP_PAGE_POINTS/points_home/points_ledger/kidpal_points_page/kidpal_points_delete/ADR-018）全命中；负向断言七句现役旧语面在目标节零残留（两处命中均在豁免面：§9 as-is 历史基线、char 对账注记自身引用旧措辞）；ADR-018 恰一次、ADR-019 零命中、ADR diff 纯增 8 行；git diff 恰 manifest 三件 + PROPOSAL 翻转。

## 验证集 §5 对照

- §5.1–6、8–9：全部实跑绿（上表）；§5.3 装配器在 FROZEN 后补跑（FROZEN 标记先于装配器首跑——顺序瑕疵如实记此处，装配器结果 ok 后 FROZEN 才提交；首跑失败即 fail-loud 未写文件，风险已被断言链覆盖）。
- §5.10 契约机械凭据闸：待 landing-verifier PASS 后由主线程运行 `pre-merge-escalation-gate.mjs`（见 §4）。
- §5.11 落地后全量复跑：不窄化，landing-verifier 独立复跑第 1/2/4/5/6 项。
- §5.12–16：K0–K5 实现期验收，归 dev-loop，不在本批。

## 契约机械凭据闸凭据（human 2026-11-04 授权例外）

- `pre-merge-escalation-gate.mjs 261103-arch-review-star-points-story-cover` 实测 **BLOCK（off-main）**：落地 commit 已直接在本地 main（b930f498），无独立分支可合——主线程流程偏差（gov Track B 应走分支 + 闸合并），如实申报。
- 三凭据联立状态：FROZEN PROPOSAL v1.0 ✓ + landing-verifier PASS（landing-verdict.md）✓ + manifest 覆盖 `docs/contracts/interface-contract.md` ✓——off-main 隔离一项不满足。
- **human 裁定（2026-11-04，AskUserQuestion 触点）**：「授权例外直落 main」——闸的分支隔离语义本批以人工授权替代，远端未推送、无回滚风险；本节即 BLOCK 凭据 + 授权记录。后续 gov 批恢复分支隔离动线，本例外不创先例。

## R1/R2 裁定落实

- 判档主路径 Track B、四分类 `ACCOMPANIES_SPEC_CHANGE` 沿 R1 裁定生效。
- CDQ-261103-star-points-story-cover 由主线程按 `ACCOMPANIES_SPEC_CHANGE` 登记（`docs/debt/contract/`），`loopEvidence` 指本 changeId + landing-verdict 结果（本节 + landing-verifier verdict）。
- ARQ-260918-spec-face-3page-reconcile 随 architecture 旧页叙述改写按 consumed-by 结项（主线程处理，见同批 debt 提交）。
