# landing-verdict · 261115-arch-review-session-channel-resident

verdict: PASS

- 核验对象：仓库 `c:\AI\KidPal-pod-m2`，分支 `dev/pod-m2`，**reviewedCommit = `87dbf77593b470c9516184c216f2bc3e2804c937`**，**reviewedTree = `5cf0b1fa1ddbbc69b7f60c321a9724e1fc2ed0a1`**（`git rev-parse 87dbf775^{tree}` 逐字相符；HEAD 即 reviewedCommit，`git status --short` 空 ⇒ 评审后零实现尾改）。
- PROPOSAL packet-id = §2 改法集（T1–T7）+ §5 验证集；Status 行 = **FROZEN v1.1**；判档 = Track B 主路径；契约四分类 = 主 `ACCOMPANIES_SPEC_CHANGE` + 次 `SPEC_CONTRACT_DRIFT`。
- 核验人：landing-verifier（独立第二核验；只读 canonical，唯一写入 = 本文件）。作者与起草者自述一律不采信，下列每项均独立读盘 / 亲跑。
- 评审钉点链：R1 四查 BLOCK(F1,F2,F3) → v1.1 → R2 靶向复核 GREEN → 主线程自冻 → packet-bound 落地（canonical 三件 + delivery 制品同提交）。R1/R2 评审段在场，PROPOSAL 于本提交首次入库。

## 核验矩阵（义务 × 方法 × 结果）

| # | 义务 | 方法（独立亲跑，不采信自报） | 结果 |
|---|---|---|---|
| 1 | 钉点与尾改 | `git rev-parse HEAD` / `git rev-parse 87dbf775^{tree}` / `git status --short` | PASS：HEAD=reviewedCommit，tree=reviewedTree，工作树 clean，无评审后尾改 |
| 2 | diff ⊆ manifest、无夹带 | `git show --stat --name-only 87dbf775` 逐文件人读；`git show --name-only \| grep -E '^(docs/requirements\|\.claude/\|firmware/\|server/)'` | PASS：非 delivery 改动恰 manifest 三件（`docs/architecture.md` / `docs/contracts/interface-contract.md` / `docs/decisions/ADR.md`）；零 requirements/.claude/firmware/server 触碰；delivery 制品（spine/INDEX/PROPOSAL/summary/package-split）不计入 canonical diff，如实单列 |
| 3 | T1 §6.4 整节重写 | 读 `git diff … -- docs/architecture.md`：三层解耦（L1/L2/L3）、终止源闭集三条、`kidpal_keepalive` 保活（阈值逻辑零改动、语义升级）、主循环 tick 自愈巡检（守卫三条件 + 零第二条重连路径）、封顶长跑 `{5,10,20,40,60}` + 人工意向位、备选四否决留档、引擎侧零 diff | PASS：FROZEN §2 T1 四条机制本体逐条在位 |
| 4 | T2 §5.1.1/§5.1.2/§5.2 | 读 diff：§5.1.1 新增「语音会话生命周期收口」行（六触发源单一处理器、动作序①–⑤、Idle 清位链全保留）；§5.1.2 front 行生命周期锚（连接期六件保持绑连接、teardown 降兜底、每会话收尾三件、ctx 边沿完整化）；§5.2 场景①建连频率、场景②会话结束段 + 云端结束意图链、场景④ discard 改以 `listen stop` 为锚 | PASS：统一处理器、front 每会话态带内驱动、结束三通道三面全落 |
| 5 | T3 §8.1 + §5.1.1 ⚠️ + §3.3 + §10 决策 29 | 读 diff：风险 3 缓解句改写 + 沿革注记；风险 14 补 session.maintenance 注记；新增 15/16/17 三行；§5.1.1 会话状态机 ⚠️ 改写（60s producer 已接线、纯模型仍 oracle）；§3.3 假设行沿革注记；§10 决策 29 追加「⚠️ 2026-11-16 取代注记（部分）」 | PASS：四处「常驻重建」引用全数对账；决策 29 正文经字节前缀核对逐字保留（见下「承重对账」） |
| 6 | T4 §四 listen + keepalive + discard + ctx + 变更记录 | 读 contract diff：`listen` 帧节四件事（形状三值含 `detect` 本板确发 / 会话边界语义 / 网关分辨权在累积器路由态 / discard 先于 stop 顺序约束）；`kidpal_keepalive` 五件事（单键无载荷形状 / front 30s 周期 / 设备侧 no-op / 省略即判死自愈 / 向后兼容降级）；discard 发送时机改句；`kidpal_session_ctx` 升格会话开始边界 + 边沿完整化；§四节尾变更记录条 | PASS：四件事与五件事逐项齐备，无缩略 |
| 7 | T5 §1.4/§1.5/§1.6 | 读 contract diff：§1.4 表行改写；表后段拆「截获执行类 / 截获转发类」两型；§1.5 第 3 条补控制类分流（self_push 放行 `start_organizing`+`end_session`、`add_points`/`redeem_points` 恒拒、白名单仍 9）；§1.6 告别回合 prompt 纪律；§1.4 节尾变更记录条 | PASS |
| 8 | T6 ADR-019 五件 + 编号 + append-only | `git diff … -- docs/decisions/ADR.md`；`grep -n '^## ADR-0'` | PASS：ADR-019 五件（日期/决策六条/理由/备选六否决/关联）+ 注入面评估附段齐备；编号恰为 ADR-018 之后（现最大）；diff 为 `@@ -157,3 +157,12 @@` 纯追加，无既有条目被改 |
| 9 | T7 §12 变更记录行 | 读 diff §12 段：新行插于表头分隔行下、`261103-…` 行之上；`sed -n '/^## 12./,$p' \| grep -c '^\| 26'` = 11 | PASS：倒序纪律遵守，改动面逐节枚举含 §3.3 / §10（R1-F2 两点） |
| 10 | §5 机械门 V1–V3 | 亲跑 `landing-fidelity-check --preflight` / `spec-anchor-check` / `contract-lint` | PASS：preflight ok:true、section2BodyFound:true、anchorCount:3、anchorUnion=三件、findings/advisories 空；spec-anchor ok:true、errors:[]、archSectionCount=48（与 pre 等值）；contract-lint ok:true、唯一既有 warning（未新增） |
| 11 | §5 token 断言 V4–V19 | 亲跑全部 grep（逐项） | PASS：V4=2 / V5=2 / V6=2 / V7=1（ADR-018 仍 1）/ V8=3（§12 行数 11）/ V9 0→2 / V10 0→2 / V11 0→2 / V12=8 / V13 0→3 / V17 0→4 / V18 mech=1、取代注记=6、`2026-11-16 取代注记`=1 / V19=3，与 summary 逐值吻合 |
| 12 | §5 V14 cite-check 覆盖缺口 | 亲跑 `--cite-check`；读 `collectCiteQuotes` 抽取域 | PASS（缺口属实、替代对账等价）：见下「V14 判定」 |
| 13 | §2A 承重原文对账（替代 cite-check） | 程序化：原文块 ⊆ PROPOSAL + ⊆ 父提交 + ∉ reviewed；改后块 ⊆ PROPOSAL + ⊆ reviewed | PASS：三条承重块全部双向逐字成立，见下「承重对账」 |
| 14 | V18/V19 覆写纪律 | 读 `.claude/references/arch-change.md:44`「落地期实跑若与预期有字节差异，以落地实跑为准覆写 §5 并在 PROPOSAL §7 留痕」+ 核 PROPOSAL §5 V18/V19 行与 §7 落地实跑记录 | PASS：两处覆写（V18 取代注记 6 / V19 常驻重建 3）均在 §5 行内标注「落地实跑覆写」并在 §7 留痕，合规 |
| 15 | 既有不变量存续 | grep + 读落地文本 | PASS：见下「不变量抽查」 |

## §2A 承重原文对账（独立程序化，等价于 cite-check 的 §2A 覆盖）

`git show 87dbf775^:` 与 `git show 87dbf775:` 分别导出父提交 / reviewedCommit 的契约全文，与 PROPOSAL §2A 三个承重围栏块逐字比对（UTF-8，子串）：

```
A1_tableRow    inPROPOSAL=True  inPARENT=True  stillInREVIEWED=False
A2_paraBack    inPROPOSAL=True  inPARENT=True  stillInREVIEWED=False
A4_discard     inPROPOSAL=True  inPARENT=True  stillInREVIEWED=False
A1_result      inPROPOSAL=True  inREVIEWED=True
A2_result      inPROPOSAL=True  inREVIEWED=True
A4_result      inPROPOSAL=True  inREVIEWED=True
ALL_OK = True
```

即：`end_session` 表行、表后网关决定类段、`kidpal_story_discard` 发送时机句三条承重原文，父提交内容逐字等于 PROPOSAL 原文块、且已在 reviewedCommit 中消失（被替换）；三条改后文本逐字等于 PROPOSAL 改后块、且在 reviewedCommit 中在位。第四条处方（§四 `listen` + `kidpal_keepalive` 两条新增 bullet）PROPOSAL 只给 prose 指令不给围栏块，改由 V4/V5/V6 正向断言 + 契约 diff 人读核其四件事 / 五件事齐备（行 6）。

## V14 判定（cite-check 覆盖缺口性质）

`--cite-check` 输出 `ok:true`、`frozen:true`、`targetCount:3`、`checkedCount:0`，`coverage.note` = 「blockquote 承载的插入段与多 span / 混合散文行内形态不在 v1 覆盖面」。读 `landing-fidelity-check.mjs:707` 的 `collectCiteQuotes`：抽取域从 `/^##\s*2[.、]/`（§2 改法节）起、至下一个 `/^##\s/` 前止——本批下一个 `## ` 标题即 `## 2A. 契约包三节`，故 §2A 整节确在工具覆盖面之外。**结论：`checkedCount:0` 属工具 v1 覆盖面的真实缺口，非判据失效、亦非作者漏做**；summary 对此如实申报（非以绿自报）。主线程用 `git diff -U0` 删除行替代对账，经本审程序化重做后等价成立（见上节 ALL_OK=True），甚至强于申报口径。

## 不变量抽查（真实语义风险，只读）

- **261010 [S9] 结果不变量**：§6.4 L1 明记「261010 既有语义零改动」「`local_close_pending_` 分类机制保留」；§5.1.1 收口行动作序③ SetDeviceState(Idle) 后 Idle 分支清位链「全保留」逐条列出。A 路径（服务端/网络侧断 → Idle + TryStartAutoConnect）零改动。
- **261024 overlay_close 时序自证**：收口行动作序① ResetDecoder 即时静音 + AbortSpeaking（261024 既有两步）先于② SendStopListening，时序先静音后发结束信号，与既有语义同序；覆盖层关层列为首批六触发源之一。
- **OQ3=c 回答页驻留**：§5.1.1 清位链含 `kidpal_render_node_voice_end`（OQ3=c 回答页驻留）；§5.2 会话结束段「前台按 OQ3=c 回落」；契约 §1.4 表行同步写「按 arch OQ3=c 回答页驻留规则回落」。
- **S-F1.4 提醒驻留与录音态排队例外**：清位链含 `RaiseReminderQueue()`（录音态提醒排队延后、收尾立刻弹）与 keybar 场景复位（REMINDER 在顶豁免）。
- **记录语境积累/丢弃链（discard 先于 listen stop）**：§5.1.1 行、§5.2 场景④、契约 §四三处一致钉「`kidpal_story_discard` 先于 `listen stop`，同一 TCP 有序，front 先消费 discard 再消费 stop」，且保留 cancel-on-disconnect 兜底（human 2026-10-21 裁维持）。
- **AUTO 守卫三条件与零第二条重连路径**：§6.4「state==Idle && protocol_ && !IsAudioChannelOpened() && !auto_connect_in_flight_ → 调既有 TryStartAutoConnect」，明写「守卫三条件与零第二条重连路径不变量保持；不自建 task、不自己 OpenAudioChannel」。
- **决策 21 / 24 / 29 保留与取代边界**：决策 21 在 §10 未进本批 diff（风险 9 的「方案二」行仅 context，未改）；决策 24 未改；决策 29 正文经字节前缀核对 = 旧 320 字符为新行精确前缀（`old is exact prefix of new: True`），仅追加取代注记，正文一字未改。`local_close_pending_` 与「零误重连」逐字保持（含 8/9/5 三段计数对账半句）。

## 非阻断观察（不构成否回，留档备查）

1. **V18/V19 预期覆写**：V18 延续注记命中数原写 5、实跑 6（§12 行自述未计入预期）；V19 `常驻重建` 原写 post ≥4、实跑 3（§6.4 重写后不保留该措辞）。两处均按 arch-change.md:44 以实跑为准覆写 §5 并在 §7 留痕，覆写本身正确、负向配对（V13/V17 旧句归零）仍成立。
2. **PROPOSAL 首落于本提交**：PROPOSAL（含 R1/R2 评审段）与 canonical 落地同提交入库，R1/R2 段在场且与 FROZEN 状态行、§7 落地实跑记录自洽；因父提交无该文件，无法与独立历史版本比对「是否被改写」，但内部一致性成立。
3. **§6.4 L1 终止源枚举**：FROZEN 将「重启/OTA/AEC/ResetProtocol」四真拆通道点列为终止源第三条，落地文本以「四个真拆通道点」概括（未逐一列名），§2 实证段与 FROZEN 原文已逐一列出，语义等价、无缩水。

## 独立结论

reviewedCommit `87dbf775` / tree `5cf0b1fa` 的实际非证据 diff 恰为 manifest 三件（architecture/contract/ADR），T1–T7 按 FROZEN v1.1 改法集逐项兑现，四处退役机制引用全数对账、决策 29 正文逐字保留，§2A 三条承重原文与改后文本经程序化双向对账全绿，V4–V19 全部独立复跑吻合（含两处合规覆写），cite-check 缺口系工具 v1 覆盖面的真实边界且替代证据链等价成立。无夹带、无权限或保护面越界、无评审后实现尾改。本审独立放行结论：**PASS**，可交主线程跑 `pre-merge-escalation-gate.mjs` 机械凭据闸。