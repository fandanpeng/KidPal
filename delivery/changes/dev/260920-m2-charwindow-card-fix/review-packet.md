# Review Packet · 260920-m2-charwindow-card-fix

> workstream m2-char-window ｜ M2-K3 真机轮修复批次（中段 change）｜ mergeOrder 31 ｜ rigor lite ｜ riskTier medium ｜ subsystem firmware ｜ sourceType human-confirmed-plan（/triage-change 四档裁决 ② Lite + human 触点①三问确认，pod-events `9ea58a1e`）
> integrator 收拢制品（2026-09-20）；合并门判读归 supervisor-verdict-gate 机械闸，本包不 push、不 merge、不在 main 上 commit。

## Change Size

- **实现（builder 未提交改动 → 本次收口 commit，firmware 面 3 文件 +65/-10）**：
  - `firmware/main/display/char_page.cc` +45/-10——① 病灶修复：cardlabel 字体由 `LV_FONT_DEFAULT` 改显式接 `font_puhui_14_1`（全树唯一中文 label 钉默认字体处，`LV_FONT_DECLARE` 随之新增）；② hint_label 补显式接线（原无显式字体、赌 `s_kidpal_scr` 继承）；③ 速查卡四行→五行拼装（`释义：%s` 行，meaning 缺义兜底 `—` 占位）+ `buf[256]→buf[384]`（设备构建闸 `-Werror=format-truncation` 截断上限，见 Verification Evidence）；④ 两处 `kidpal_char_info` initializer positional → designated（防成员漂移，plan.md 改动面 3④）+ meaning 透传。
  - `firmware/main/display/char_stroke.h` +9/-0——`KIDPAL_CHAR_STROKE_HAS_MEANING 1` 探针宏（builder 接线义务，K3 件 fallback 宏同款先例；漏定义 = 行为齿静默缺席编译、TEST 5 结构齿咬住）+ `kidpal_char_info` 增 `const char *meaning`（**NULL 容忍**，human 裁：缺释义只损右栏一行、不降级整窗）+ `kidpal_char_card` 增 `char meaning[96]`（容量对齐 words 惯例）。
  - `firmware/main/display/char_stroke.c` +11/-0——`kidpal_char_card_build` 增 meaning 装配：非空 verbatim 截断安全拷贝 / NULL → 空串且 rc=0（**不进**既有 pinyin/radical/words 的 NULL 拒绝集——旧 payload 整窗降级 = 行为回归）。
- **验收冻结（前序 commit `509bd74`，firmware 面 2 文件 +460/-1）**：
  - `firmware/main/test/char_stroke.acceptance.c` +251/-0——seam [5] + TEST 5（结构半：card_build 函数区 meaning 消费计数）/ TEST 6（行为半：三态——非空 verbatim / NULL→空串 rc=0 / 四字段+progress 零回归），additive-only。
  - `firmware/main/test/char_window_overlay.acceptance.c` +209/-1——seam [10] + TEST 9（MCF-A1 负向齿「LV_FONT_DEFAULT 作 text_font 实参数 == 0」+ MCF-A3 五行拼装齿，注释剥离后计数），tighten-only 留痕（MCF-A4，REVISION LOG 在册）。
- **Net diff**：本包全生命周期（launch 基线 `5982d82` → worktree）firmware 面 **5 文件 +525/-11**（实现 3 + 验收 2，`git diff --numstat 5982d82 -- firmware/` 实测）。
- **簿记面（非实现改动，随收口 commit 冻结）**：`delivery/taskpackages/<id>/taskpackage.json`（mutationEvidence.perAcceptance 3 条回填 + MCF-A4 structural 对象化 +28/-2）、`delivery/taskpackages/current/`（taskpackage 投影 + verdict.json 脚本 fs-write 投影）、`pod-events.jsonl`（append 2 条：builder dispatch-completed + over-scan friction）、`verify-runs.jsonl`、`verifier-evidence/`（immutable，只引路径不复制正文）。
- **Scope**：板端 `firmware/` 单一子系统——字体接线 + meaning 五行兑现。云端 `server/` **零改动**，`docs/**`（含 `docs/contracts/**`）、`.claude/**` **零触碰**（`git diff 5982d82 --numstat -- firmware/ server/ docs/ .claude/` 逐面核对，docs 侧详见同目录 `docs-delta.md`）。
- **Split recommendation**：不拆——字体接线与五行兑现是同一张右栏速查卡的呈现面与数据面（接线决定可见性、meaning 决定第五行内容），验收齿（TEST 5/6/9）亦一次 RED→GREEN 链承载；rigor=lite 单包 65 行实现量，无拆分价值。

## Verification Evidence

- **Structural gates**：final r2 fact **九闸全绿**（`ok:true`、`observedGateExit:0`、invocationId `31bd5da8-40f0-49c5-b3f3-95dd7adeb01f`，2026-09-20T05:24:19.355Z–05:24:34.831Z，evidenceRef `delivery/taskpackages/260920-m2-charwindow-card-fix/verifier-evidence/rounds/r2.json`）——taskpackage-required / **firmware-build（device build 真编译，未降级，`xiaozhi.bin` 0x30b870 bytes、分区余量 11%）** / contract-lint / acceptance-coverage（functional = MCF-A1/A2/A3；structural 豁免 = MCF-A4/A5）/ mutation-check / docs-sync（`docsSyncRequired=false`）/ security（`securityReviewRequired=false`）/ unit（pytest 37 passed）/ acceptance（oracle-pass，firmwareAcceptanceFiles 9）。scoped r1（rounds/r1.json）同样一次绿——**全生命周期两轮 verify 均一次通过、零返工轮**。
- **机械校验**：`.claude/scripts/verifier-evidence-check.mjs --change-id 260920-m2-charwindow-card-fix --phase final --round 2 --invocation-id 31bd5da8-…` **integrator 实跑 exit 0、`factOk:true`、`observedGateExit:0`**（2026-09-20）——final evidence 有效，可进 closeout。
- **host runner 全量（integrator 独立复跑，非闸境）**：`cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- bash ./espidf_wsl.sh build` 后直调 `./build/host_infra_test.elf`（elf 相对 host_app/）→ **`61 Tests / 0 Failures / 0 Ignored`、OK、EXIT=0**（2026-09-20 integrator 实跑）。数字链自洽：K3 收口锚 **58T/0F** + 本包新齿 3（char_stroke TEST 5/6 + char_window_overlay TEST 9）= 61。红基线链（taskpackage `redBaselineResult`）：60T/2F（两条均为设计态义务红 = A1 负向齿病灶本体 + A2 结构半）→ 接线/装配落地后 61T/0F，其余既有齿零新增红。
- **设备构建闸的价值实证（在案实例，「host 全绿 ≠ 设备能编」）**：五行拼装后 `buf[256]` 触发设备侧 `-Werror=format-truncation`（各字段截断上限和最坏 ~354B）——host runner 不编 char_page.cc，**测不出该类缺陷**；builder 放大到 `buf[384]` 修平（代码注释载最坏尺寸分析）。本包 device build 闸真编译通过（r2 首跑全量编译 80s / 终审增量 9.6s，均 exit 0），未降级。
- **Mutation check**：TaskPackage `mutationEvidence.perAcceptance` **三条 record（MCF-A1/A2/A3）全部 `sensitive:true` + `failed as expected`**，各条「单点变异 → 转红 → 还原复绿」链完整、还原指纹与绿态逐字节一致，全仓零变异残留：
  - MUT-MCF-A1-font-unwire（撤 font_puhui 接线还原病灶形态）→ TEST 9 负向齿红「仍有 1 行携带 LV_FONT_DEFAULT 实参」；绿→红 61T/1F → 还原 61T/0F（char_page.cc md5 与绿态一致）。
  - MUT-MCF-A2-meaning-drop（删 NULL 容忍 + verbatim 拷贝分支）→ TEST 6 行为半 + TEST 5 结构半**双咬**；61T/2F → 还原 61T/0F（char_stroke.c md5 一致）。
  - MUT-MCF-A3-line5-drop（删「释义：%s」段回四行）→ TEST 9 五行拼装齿红（card.meaning 计数 / 释义：字面 / — 兜底 / 五行形态四条同咬）；61T/1F → 还原 61T/0F（md5 一致）。
  - **防伪齿核心**：MCF-A1 负向齿对病灶形态即红、修复后转绿（RED→GREEN 真信号），直接针对 K3 A5a 假绿盲区（当时正向包含齿对字体面零覆盖）。本 packet 引用 taskpackage 字段，不重跑。
- **Docs sync**：`docsSyncRequired=false`；实现面零 docs/contracts/spec 触碰经 `git diff` 核对（详见同目录 `docs-delta.md`）。spec v1.56 + 勘误在 launch 基线 `5982d82` 已落账，本包只消费。
- **S2 exploratory**：TaskPackage 未声明 `s2ExploratoryRequired` → 无 S2 official scan（非遗漏）。

## Demo 对账行（taskpackage `testHost` 证据形态核对）

taskpackage 权威口径（`testHost` 逐字）：**「host runner（WSL linux target）+ device build 闸 + 真机腿（human 持板，A7 同轮）」**——真机腿 = MCF-A5，与 K3 A7 设备确认轮**同轮合并取证**（human 持板一次上台、两包证据同轮落账）。逐项对账：

| testHost 证据形态项 | 本包实际证据 | 对账 |
|---|---|---|
| host runner | `char_stroke.acceptance.c` TEST 5/6（meaning 三态）+ `char_window_overlay.acceptance.c` TEST 9（A1 负向齿 + A3 五行拼装齿）→ 全量 **61T/0F EXIT=0**（integrator 独立复跑）；A1/A2/A3 各一条 mutation 反证（`sensitive:true × 3` + 还原复绿、指纹逐字节一致） | 一致 |
| device build 闸 | final r2 `firmware-build` exit 0（真编译未降级、`xiaozhi.bin` 0x30b870 / 余量 11%；`buf[384]` 截断上限修复在编译内） | 一致 |
| 真机腿（human 持板，MCF-A5，与 K3 A7 同轮合并取证） | **未做、deferred**——MCF-A5 为 structural 条目在册（右栏五行可见 + 带调拼音 yǒng 的 ǒ 字形实渲 + 底部提示条可见 + 顺带 schedule/story 页中文可见性一句取证），证据形态 = human 持板取证，落 `delivery/changes/dev/<changeId>/` | **一致但未闭环**——口径本即 deferred（host 无 SPIFFS/内置字体字形与真机恒不等，HOST_TEST_CONVENTIONS「字体现实」节明文；齿只钉接线与数据面行为，不造 host 伪断言） |

**一致性结论：口径一致、真机腿未做。** host runner 与 device build 闸两项已按口径落证；**真机腿（MCF-A5）未执行，本包 host 半闭环——明记此点，不得读成整包已验收**。真机腿随 K3 A7 设备确认轮 human 持板同轮执行（一次性上台、两包证据同轮落账），届时补本目录取证记录。

## 本包特有事项（根因 / 防伪 / 遗留，合并门判读输入）

1. **根因与勘误链**：右栏隐身根因 = `char_page.cc` 原 :555 全树唯一在中文 label 显式钉 `LV_FONT_DEFAULT`（内置 montserrat_14 纯拉丁、无汉字与带调韵母字形）+ 提示条 label 无显式字体；spec v1.56 曾误记「全工程显式加载 font_body」（设计注释被当已落地代码），勘误已随 `5982d82` 独立落账。**font_body 运行时接线 = RP-A4 residual 后续包，本包不并入**——本包接的是 `font_puhui_14_1`（lcd_display.cc 键帽条/语音轮五处真机验证先例，human 触点①确认）。
2. **防伪齿（K3 A5a 假绿盲区的对症修复）**：MCF-A1 负向齿（`LV_FONT_DEFAULT` 作 text_font 实参数 == 0）对病灶即红、修复转绿；正向齿全走注释剥离后计数（sealing pass 纪律）。A1/A2/A3 三条 mutation 证据齐备（还原指纹逐字节、零残留，明细在 TaskPackage mutationEvidence）。
3. **设备构建闸价值实证**：见 Verification Evidence（`buf[256]→buf[384]`，host 不编 char_page.cc 测不出 `-Werror=format-truncation`）。
4. **R1 遗留（真机腿终审）**：`font_puhui_14_1` 对带调韵母（yǒng 的 ǒ）字形覆盖未实证——MCF-A5 human 持板验；缺字升 font_body 接线包（RP-A4 residual）。
5. **provenance**：sourceType=human-confirmed-plan——/triage-change 四档裁决 ② Lite + human 触点①三问确认（② Lite / font_puhui_14_1 显式接线 / meaning 不纳入 validate 闭集），architect 纠偏发现 A（lv_font_load 全树零运行时调用、schedule_page.c:780 为设计注记）与 B（v1.56 对账④事实错误）已分别吸收进 plan 与 spec 勘误。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （空） | changed diff 内无 `minimal:` 标记（`grep -n "minimal:" firmware/main/display/{char_page.cc,char_stroke.c,char_stroke.h}` 实测零命中） | — | — |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | firmware/display | `firmware/main/display/char_stroke.h:23` 注释将本包 changeId 误写为「**240920**-m2-charwindow-card-fix」（应为 260920-）——逐字面错的 ID 字面，不影响编译与齿判定，但会误导后续考古 | changed-code hunk（char_stroke.h +9 块首行注释）+ 同文件同包其余注释及 taskpackage 均写 260920- | 登记（本包不就地改——终审 fact 覆盖当前 diff，任何 post-fact 编辑即破坏「已验 diff == 冻结 diff」不变式且触发整机重验；建议随下次触该文件的批次顺手修正） |
| FYI | judgement-call | firmware/display | `firmware/main/display/char_page.cc:537` device build 告警 `variable 'ch' set but not used [-Wunused-but-set-variable]`（`const char *pinyin = "?" , *radical = "?" , *ch = "?" , *words = "?" , *meaning = "?";`）为**基线既有**告警，本包改动块仅在该声明行追加 `*meaning`，未顺手清理 `ch` | r2 首跑 firmware-build stdout 逐字（编译告警在案）+ `ch` 未用为本包 diff 之前的既有形态（基线已在 K3 packet FYI 登记） | no-trigger——不扩面（与本包「既有项不扩面」约束一致），如实登记；建议后续批清理 |
| FYI | judgement-call | governance/tooling | 本 Pod 两条 over-scan friction 已落 pod-events：① acceptance-author 超读 2 行（host_app CMakeLists 核验 + red-baseline 脚手架目录，`2a4e80e5`）；② builder 超读 2 行（kidpal-idf-env skill + firmware/main/CMakeLists.txt grep BUILTIN_TEXT_FONT 考证 font_puhui_14_1 链接可达性 R3，`ee3ff63f`）——均属冻齿判定/环境闸必要边角 | pod-events 事件 id 在场 + 该等项不在 verify 闸集内 | no-trigger——非阻断项，如实留痕 |

无 BLOCKING。结构闸 PASS/FAIL 不复述为 finding（verifier evidence 承载）；gate 全绿仍完成五轴语义 review（见下）。

## Five-Axis Review

1. **Correctness**：三条功能齿语义齐备——A1 字体接线（负向齿钉死病灶形态归零 + cardlabel/hint_label 两处正向锚）；A2 meaning 三态（非空 verbatim 截断安全 / NULL → 空串 rc=0 **不降级整窗** / 既有四字段与 progress 文案零回归，M2-K1 冻结面未触碰）；A3 五行拼装（五行口径 + 「释义：—」兜底形态钉死不留现场裁量 + refresh_hint 装配区计数）。边界条件齐：meaning NULL 容忍分支不进既有 NULL 拒绝集（旧 payload 零回归）、`buf[384]` 截断上限覆盖最坏字段和、designated initializer 消除 positional 补成员漂移、`cJSON_GetObjectItem` 缺字段走 `"?"` 兜底再由 card_build 空串承接。**消费面闭集**：`kidpal_char_info` / `kidpal_char_card_build` 消费者 grep 全仓 = char_page.cc 两处 + 冻结验收件，无其他调用方；struct 增成员 additive、全部 initializer 已同步 designated 化。
2. **Test quality**：断言真实行为非实现镜像——TEST 6 行为半真调 char_stroke.c 强符号断言三态返回与装配值；TEST 5/9 结构半对设备专属文件（char_page.cc）用稳定切区 + 注释剥离后计数（sealing pass 纪律：负向齿与锚齿不剥）。**负向齿 = 本包关键形态创新**：对现状即红（RED→GREEN 真信号），对症 K3 A5a 正向包含齿假绿盲区。三条功能齿全部 mutation 敏感 + 单点变异逐齿翻红 + 还原复绿（指纹逐字节）。红基线 60T/2F 双证（acceptance-author 实跑 + planner 独立复跑），两条红均为设计态义务红、非实现缺陷漏检。
3. **Contract/boundary safety**：`docs/contracts/**` 零触碰（`git diff` 零命中 + contract-lint 绿）；D3 `meaning` 字段**既有**、本包零契约改动（MCF-E2）；meaning **不纳入** mcp_server validate 六键闭集（human 已裁，validate 零改动）；`server/` 零改动（OpenClaw 生产端取词典 definition 已在下发，crossEnd exempt 在案）；spec 面零漂移（v1.56 五行口径为 launch 基线现文）。字体链路：`font_puhui_14_1` 定义在 xiaozhi-fonts 组件、lcd_display.cc 同组件先例已证可达（builder R3 考证），CMake 零新增接线。
4. **Operability/security**：无 secret / 权限 / correlationId 面（security 闸 `securityReviewRequired=false`，exit 0）。`snprintf` 恒 NUL 结尾、无溢出面（meaning[96] 容量对齐 words 惯例，超长 verbatim 截断为既定语义）；缺义降级口径经 human 裁定（缺释义只损右栏一行，不构成盲渲染风险）。**无新增依赖**（font_puhui_14_1 为既有组件字体，零新 include 边——`LV_FONT_DECLARE` 同文件声明、组件链接已可达）。
5. **Maintainability/simplicity**：改动局部、单源清晰——数据面装配仍单源 `kidpal_char_card_build`（char_page 呈现面只拼装不复制语义）；「缺义兜底」钉死在呈现面一处（`—` 占位 ternary，不留现场裁量）；designated initializer 自文档化。**无过早抽象、无重复实现**：meaning 装配 11 行与既有三字段同款形态；探针宏沿用 K3 件 fallback 宏先例。simplification tags 零命中（`minimal:` 零命中）。两条注释级瑕疵（char_stroke.h ID 误写 NIT + `ch` 未用既有告警 FYI）均非结构冗余，见 Findings。

## Residual Risk

- Known limitation:
  - **MCF-A5 真机腿 deferred（human 持板）——本包 host 半闭环，不得读成整包已验收**：右栏五行可见性（含带调拼音 yǒng 的 ǒ 字形实渲）、底部提示条可见、schedule/story 页中文可见性对比取证，随 K3 A7 设备确认轮同轮执行；device build 闸只担保编译，不担保实机渲染行为（HOST_TEST_CONVENTIONS「字体现实」节明文：host 无 SPIFFS/内置字体字形与真机恒不等）。
  - **R1 字形覆盖风险**：`font_puhui_14_1` 对带调韵母字形覆盖未实证；真机轮若发现缺字 → 升 font_body 接线包（RP-A4 residual，后续包）。
- Follow-up recommended:
  - K3 A7 + 本包 MCF-A5 真机确认轮（human 持板一次上台、两包证据同轮落账）。
  - RP-A4 font_body 运行时接线包（spec 勘误对账④的完整兑现）。
  - `char_page.cc` `ch` 未用变量告警清理 + `char_stroke.h` changeId 注释修正（见 Findings，后续批顺手项）。
- Human decision needed：**无**（无契约/spec 变更面由本包就地裁、无依赖新增、无安全残余）——真机腿走 K3 A7 human 持板既有通道，非本包新增升级点。回滚方式 = 本包 commit 序列 revert（改动面收敛于 firmware 3 实现 + 2 验收冻结件 + 簿记面，无 schema/契约迁移；冻结件 `char_stroke.acceptance.c` / `char_window_overlay.acceptance.c` 的本包段随 revert 回退、K3 既有段不受影响）。
- decisionCoverage：TaskPackage `decisionCoverage` 为**空集（`[]`）**——无 `landsIn: dropped && reason: risk-accepted` 条目，无 D<n> 风险接受挂账。

## 影响面核对（worktree 口径 · step 11b）

- **detect_changes: N/A (worktree)**——工具图与索引 checkout（主 checkout）非同一树；Pod worktree 的 diff 对主索引不可见。本树另有图项目 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`（随树增量刷新），以 `git diff --numstat` 闭集 + 关键共享符号 grep / `trace_path` 抽查替代（2026-09-20 integrator 实跑）。
- `git diff --numstat 5982d82 -- firmware/ server/ docs/ .claude/`：改动闭集 = firmware 5 文件（实现 3 + 验收 2）+ 簿记面；`server/`、`docs/contracts/`、`docs/requirements.md`、`docs/architecture.md`、`.claude/` **零命中**。
- 关键共享符号抽查（均在 firmware/main display 场内，无跨层/跨子系统新增调用方）：
  - `kidpal_char_card_build`（**struct additive 扩展、签名不变**）：grep 全仓消费闭集 = `char_page.cc`（:330 / :571 两处调用）+ 冻结验收件（char_stroke / char_window_overlay）——无其他调用方；`trace_path` 对本树解析 0 边（图调用边解析局限，K3 packet 已在案同型），以 grep 清单为准。
  - `kidpal_char_info` struct（**additive 增 meaning 成员**）：全仓 initializer 消费点 = char_page.cc 两处（均已同步 designated 化 + 补 meaning）+ 验收件（info 显式零初始化）——无漂移消费点。
  - `kidpal_char_page_show`（**呈现面改**）：消费点 = mcp_server case 3 路由（case 3 与 validate 六键闭集零改动，human 已裁）。
  - 字体符号 `font_puhui_14_1`：`LV_FONT_DECLARE` 文件内声明 + xiaozhi-fonts 组件链接（lcd_display.cc 五处既有消费先例），零 CMake 接线新增。
- 索引状态：工具图与工作树差异如实以本节手工抽查承载，未虚报；re-index 需求归主线程/human。

## Merge Packet / Supervisor 输入

- 待合并分支：`dev/260915-m1-k1a-key-layer-dispatch`（本 Pod 常驻分支，本包改动冻结于其上，mergeOrder 31；dependsOn `260917-m2-k3-char-window-overlay` 已在先序）。
- 合并路径：planner 主线程跑 `supervisor-verdict-gate.mjs` 机械闸 → APPROVE 后前置升级闸（`pre-merge-escalation-gate.mjs`）→ 放行 `merge-driver --pod` 本地合并；**本 integrator 不执行 merge、不 push、不在 main 上 commit**。
- **本包 commit 序列**（本分支，launch 基线 `5982d82`）：
  1. `fee19e8` `chore(devloop): 拆包制品落盘——triage ② Lite reviewed plan + taskpackage`
  2. `2259d37` `chore(devloop): briefs 重生成（sourceType 移除后）+ intake/loop 纪律落账`
  3. `509bd74` `test(mcf): 验收冻结落账——char_stroke seam[5]+TEST5/6 + char_window_overlay seam[10]+TEST9`
  4. `ee23f8f` `chore(m2-k3): A7 真机轮 finding① 入 pod-events`
  5. 本次收口 commit：`feat(firmware): m2-k3 真机轮修复——右栏速查卡/提示条 font_puhui_14_1 显式接线 + meaning 五行兑现（MCF-A1~A3 齿全绿）`（builder 实现 3 文件）+ `chore(mcf closeout): 晋升 closeout 制品 + 簿记冻结`（taskpackage mutationEvidence 回填 / current 投影 / verifier-evidence / verify-runs / pod-events / 本目录四制品 / INDEX 行）。
- **最终冻结 diff 的 stat**（`git diff --numstat 5982d82` 口径）：firmware **5 文件 +525/-11** + 簿记面（taskpackage ×2 + current 投影 ×2 + pod-events + verify-runs + verifier-evidence）+ closeout 制品面 `delivery/changes/dev/260920-m2-charwindow-card-fix/` 4 文件 + `delivery/changes/dev/INDEX.md` 单行。**逐面零命中**：`server/`、`docs/contracts/`、`docs/requirements.md`、`docs/architecture.md`、`.claude/`。
- 阻塞项：**无 BLOCKING finding**；`closeout-check` 两 blocking 制品（`docs-delta.md` / `review-packet.md`）已晋升本目录。
- **里程碑 dogfood**：`M2-char-window` 已 `in_delivery`（2026-09-19 补记）——本包为 K3 真机轮修复批次、中段 change，**不动里程碑状态**；`ready_for_acceptance` 转移待 K3 A7 + 本包 MCF-A5 真机轮闭环后由 planner 裁。
- 回滚方式：revert 本包 commit 序列（见 Residual Risk）。
- 遗留脏行说明：`delivery/taskpackages/current/verdict.json`（verify.mjs 脚本 fs-write 的 non-bearing 投影，现指向 r2）为簿记面 M 态，随本轮收口 commit 一并冻结；`.codebase-memory/` 脏为工具再生面常态，不 stage、不还原；`firmware/build/` 与 `firmware/main/test/host_app/build/` 为白名单构建产物，不 stage。
