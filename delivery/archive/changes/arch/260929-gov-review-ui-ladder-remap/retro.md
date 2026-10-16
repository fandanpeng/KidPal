# retro · 260929-gov-review-ui-ladder-remap

> Track B 闭卷复盘（趁热写、先于合并）。制品四件齐：PROPOSAL（FROZEN v1.0）+ landing-summary + landing-verdict（GREEN）+ 本件。

## 1. 本批做成了什么

UIUX 梯子重排为「上游供给缺多少、下游升多少」语义：设计决策全部上收需求/治理面（L4 画审分离），dev-loop 对 UI 只剩消费（L0/L1）、起包前 UI-ready fail-closed 检查（planner Step 6b，分形判据防误拦 bug 修复包）、L3 兜底升级（停包 → pod-events `agent-escalation` → requirement-change 原 story 档 2 修订批 → frozen 后重核续包）。uiux-author 撤并模式 A 只留生成模式（角色/制品/管线痕迹三件一套清零），写槽对称收紧。九文件落地，V1–V8 全绿（含三条负向断言），landing-verifier 逐字核过五条承重句。

## 2. 起草与评审摩擦（下次照抄 / 避开）

1. **「机械透传替代角色」的判定套路值得复用**：判断一个 agent 翻译/中转位是否还有必要，看三件——消费面是否已被管线自动化（①c 透传）、实证到场率（53 包 1 true）、制品是否还有独立判断增量。三件全否 → 撤并，且角色、制品、管线痕迹**三件必须同批清零**（本批 human OQ-4 追加的第三件是判官 R1/R2 都没抓到的盲区，human 肉眼补上的）。
2. **负向断言（计数 = 0）是撤并批的正确验证形态**——V6/V7/V8 三条把「撤干净」变成机检。但基线计数必须起草期实查留痕（V8 首写 5、实查 6），负向断言的落地期自虐点：**主线程自己新写的注释/句子会撞自己的负向断言**（本批撞了两次：墓碑注释自引 token、dev-loop :68 新句带 token）——落地写新文本前先过一遍「这个文件有什么词不能出现」。
3. **GREEN 后的范围追加走 R3 重裁成立**：OQ-4 追加 T7 后判官补抓了 R3-F1（:151 注释行漏在清扫枚举、V8 必红）——证明「GREEN 后改范围必须回判官」不是仪式，是 real gate。
4. **引用引据前 grep 核位**：本批 R1-F6（manifest-missing 句错记在 planner Step 9，实在 acceptance-author.md:24）与上一批同类错——「记得的句式位置」不可信，引 file:line 前一律现 grep。

## 3. 留根交接

- **landing-verifier 交 retro 的 MINOR**：planner Step 6b 与 dev-loop :68 的「触 UI 包（L1+）跑检查」与「Level 0/1 不跑」字面相抵（L1 两头占）——判非阻塞（误读方向只会多跑检查、偏保守），下一批把「（L1+）」改「（L2/L3 判定）」一行修正。
- 首个触 UI 需求批 + 首个 L3 停包实例回收：UI-ready 检查摩擦、agent-escalation 留痕形态、停包-续包节奏成本、L1 小视觉微调是否兜得住。
- taskpackage-required-check 落 UI-ready 机检齿（若 def 层义务出现跳过实例）。
- DEV-GOVERNANCE roster 行（G1 面）仍待独立 human-approved 批（上一批遗留，本批未触）。

## 4. 模型行为自评

主线程本批五轮（立项对齐 → 起草 → R1 修订 → R3 追加重裁 → 落地）：判官两轮回返共 13 条 finding 全部成立并逐条核销；落地期两处自撞负向断言均自查自纠并申报进 landing-summary §4；无跳步、无凭解释宣布完成。
