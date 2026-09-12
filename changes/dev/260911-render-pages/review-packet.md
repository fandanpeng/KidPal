# Review Packet — 260911-render-pages（批次③ 渲染观感 + V3 整屏还原双轨 · 收口刷新）

> integrator 产出 · 2026-09-12（收口刷新：真机轮 RP-A5 四轮 fixer 收敛 + human 裁定 PASS 后）· 分支 `dev/260911-render-pages`（base = main）。
> 供 supervisor 合并门复核。本包 `riskTier=high`（`gateProfile=[host-teeth, device-round, frozen-acceptance-revision]`），照常触发 supervisor 升级人类。
> 交付本体：缺陷轨四条（#9 故事页排版 READER 按 screens 切片只画当前屏 + #10 字体缺字「猬」GB2312 全集自产子集 + #11 课表页数据链固件缓存空态友好页 + #12 P2-A1 恒红齿修齿方案 B）+ 还原轨（P2/P3/P4 页面换皮 V3 框族/网点/字体 + P4 笔顺页观感 + 语音轮五态演出换皮 `kidpal_voice_states` 纯逻辑单源 + V3 字体体系并包 fonts_root 三字体 SPIFFS 非 KAP1 落地 + 唯一新增 KAP1 = `bubble_tail_24.bin` 走 U 闭集修订 47→48）+ **真机轮 RP-A5 收敛增量**（`kidpal_voice_apply_look` 两段式锁纪律重写 + 姿态图挂 `emoji_box_` 聊天 chrome + 泄漏清零；五态显隐纪律 + story 空态；d12 底纹 + 内容页前台藏猫；`Board::GetInstance()` boot 死锁根因修复 = 自注册 C 桥 `kidpal_lcd_voice_chrome_front`）。
> **合并门必读**：本包无 BLOCKING finding。承重 final fact **r6** **非全绿**（`ok:false`），唯一红 = `unit`/`acceptance` 两闸同源的 `test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（SMK-A4）——**在册他人红**（owner=260829-smoke-e2e，见同目录 `existing-red-inventory.md` R1；与先例 PKG-4 合并夜同一红、同一处置通道），与本包零相关、零新增红。host runner（WSL ELF 全量）收敛为 2 红（WSA-A6:830 / WSA-A4:663，均在册既存红；VRT-A2·E2 从 repo root 跑转绿、在册 P2-A1 已由 RP-A1 修齿消解）。故合并门需**按红证据 ESCALATE 通道**提 human 裁定「留红如案、红转交各自 owner」；本包自身面（scoped 7 闸 + 真机轮 human PASS + 冻结面零越界）全绿。

## 真机轮 RP-A5 全记录（2026-09-12 · 首轮失败 → fixer r1-r4 → human PASS）

> 背景：批次③ closeout 时 RP-A5/RP-A8/RP-A9/RP-A10 真机证据 pending（owner=human，先例同 PKG-4）。human 持板首轮即暴露**冻结级缺陷**，随后 fixer 四轮收敛、真机轮全绿、human 肉眼裁定 PASS。本节为收口刷新新增，是全包观感面的收口判据。

### 时间线与证据日志清单

| 阶段 | 结果 | 证据日志（本目录） |
|---|---|---|
| RP-A5 首轮（失败） | listening 态到达、AFE 任务正常、KEY 路由与 WS 链路本体正常，但**上行音频零帧**（引擎 3 分钟零 opus 超时）；视觉乱象 = 猫满屏跑/两只猫/笑脸盖页 | `rpa5-boot.log` / `rpa5-round.log`（首轮，前次 commit `d4a94f2` 已入库） |
| fixer r1 | `lcd_display.cc` 重写 `kidpal_voice_apply_look`：两段式锁纪律（资产加载/像素合成锁外、对象树操作锁内）+ 姿态图挂 `emoji_box_`（聊天 chrome 表情区，PKG-2 仲裁内）替代裸挂 `lv_layer_top` + 泄漏/旧图堆叠清零 → **语音链路完全恢复**（两轮 ASR/LLM/TTS + 干净挂断） | `rpa5-r1-voice-restored.log` |
| fixer r2 | 五态显隐纪律（idle 藏姿态/气泡/笑脸，语音轮中猫顶替笑脸，二者不得同框）+ story 空态（`kidpal_render_node_cycle_next` count<=0 改画友好空态，#11 课表空态同族）→ 待机页干净、语音轮猫顶替笑脸 | `rpa5-r2-visibility.log` |
| fixer r3（引入 boot 死锁） | d50→d12 底纹（d50 为设计唯一半调压暗档、整页平铺呈「很多黑点」，r3 起改三档中最稀疏的 d12）+ 内容页前台藏猫（`KidPalVoiceChromeFront` 前台维度）+ 故事文案对齐设计稿；但 render_node 桥在板卡构造期经 `Board::GetInstance()` 取 display，重入函数内 static 初始化 `__cxa_guard` **永久阻塞**，boot 挂于 show_home 前 | `rpa5-r3-boot-hang.log`（日志止于 `render home 400x300 … tick=1054`，无后续） |
| fixer r4（终态） | 桥改**自注册 C 接口** `kidpal_lcd_voice_chrome_front`（`SetupUI` 登记 self 指针，绝不经 `Board::GetInstance()`）→ boot 冒烟过（走到 activating→idle）+ **真机轮全绿**：两轮完整语音（listening→speaking→listening→idle 全程）、挂断 `voice_end top=4` 回答驻留（OQ3=c 落点确定语义兑现）、BOOT 翻页 `cycle -> SCHEDULE (P2) n=7` 出真实表格（`schedule_render items=7 drawn=7`，RP-A3 课表数据链真机腿兑现） | `rpa5-r4-boot-smoke.log` / `rpa5-r4-device-round.log` |
| **human 肉眼裁定（2026-09-12）** | **PASS**——d12 底纹稀疏 ✓、答案页无猫 ✓、故事空态文案 ✓、待机页干净 ✓、语音轮猫无笑脸 ✓ | pod-events `ee11264c`（fixer-round 2-4 收敛记录）+ `52833536`（human-decision） |

### human 决策（2026-09-12，pod-events `52833536`）

- **RP-A4 三字体运行时接线（lv_font_load 兑现）拆后续包**，本包先收口；**RP-A4 真机腿（「猬猾狸」抽样可见）记 residual**。
- 背景：v3_restore 齿 6 被 `schedule_page.c:779-782` 注释文本糊绿（齿强度缺陷，acceptance-author 已留痕 REVISION LOG）；三字体仅落 bin 文件、零运行时接线代码——真机腿上不可见缺字修复效果，故运行时接线与其真机验证同属后续包。

## Change Size

- Files changed（本收口刷新窗口增量，`git diff --numstat -- firmware/` 实测：**4 files changed, +250/−48**）：
  - `firmware/main/display/lcd_display.cc`（+208/−25）：`kidpal_voice_apply_look` 两段式锁纪律重写（锁外资产加载/像素合成、锁内对象树；锁获取超时带日志非静默）；姿态图父容器 = `emoji_box_`（SetupUI 活分支）；泄漏/旧图堆叠清零；`KidPalVoiceChromeFront(bool)` 前台维度（ armed && front 两条件与，重断言中部显隐）；自注册 C 桥 `kidpal_lcd_voice_chrome_front`（`s_kidpal_lcd_self` 经 SetupUI 登记，boot 期单例重入死锁根因修复）。
  - `firmware/main/display/lcd_display.h`（+6）：`KidPalVoiceChromeFront` 声明 + 前台维度语义头注（调用方持锁约定、键帽条/聊天文本不受此开关影响）。
  - `firmware/main/display/kidpal_render_node.cc`（+41/−8）：`rn_voice_chrome_front_locked()` 桥（7 个前台切换点接入：show_home/show_chat/voice_begin/voice_end/begin_page/begin_reminder/end_provision）；story 空态（KP_PAGE_STORY 分支 count<=0 友好提示，#11 同族）。
  - `firmware/main/display/schedule_page.c`（+7/−3）：底纹 d50→d12（注释同时留痕 r3 真机裁定依据）。
  - 证据日志 7 件（本目录 `rpa5-*.log`，首轮 2 件前次已入库、r1-r4 五件本次新增）+ 刷新制品（review-packet / verdict / verification / docs-delta / pod-retro）。
- 前窗批次③本体（14 files +838/−53，实现面 10 文件 + 资产管线 2 件 + bin 4 件 + 冻结验收件 3+1）见本包前窗制品与本 commit 祖先，未回退。
- Scope: `subsystem=firmware`；`docs/contracts/**` 与 `docs/spec.md` **零改动**（contract-lint 闸 PASS）；`server/**` 零改动；本窗口 `docs/` 唯一变动 = `docs/ARCH-REVIEW-QUEUE.md` +2 行（ARQ-260912 登记，非 integrator 落笔、fixer 轮交回披露后登记，docs/contracts 之外受控文档路径）。
- Split recommendation: **不拆**（前窗裁定不变）；本窗口 RP-A4 运行时接线拆后续包 = human 决策（见上节），非本包拆分。

## Verification Evidence

> **承重 fact 声明（诚实口径）**：final completed-only round = **r6**（`phase:"final"`，identity = changeId+phase+round+invocationId），
> `verifier-evidence/rounds/r6.json`：`ok:false`、`observedGateExit:1`、`authority:"bearing"`、`producedBy:"verify.mjs"`、
> `invocationId 180e8b70-aca2-440e-ba5e-e2390bd7c41c`、`startedAt 2026-09-12T15:36:34.074Z / completedAt 15:36:57.851Z`（integrator 收口相位实跑 `--evidence-mode=round` 产出）。
> `ok:false` 的**唯一**成因 = `unit`/`acceptance` 两 full 闸同源红 `test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（SMK-A4，在册他人红 R1）；scoped 7 闸**全绿**。`verifier-evidence-check.mjs --phase final --round 6 --invocation-id 180e8b70-…` 已核验（exit 0，factOk:false 但 evidence 合法——与 r5 / 先例 PKG-4 同形态）。
> `current/verdict.json` 为同一 `producedBy:"verify.mjs (script fs-write)"` 的投影（non-bearing compatibility projection），closeout 判断以 r6 fact 为准；本目录 `verdict.json` = r6 投影晋升副本。

- **verifier 逐轮收敛轨迹（r1→r6）**：
  - **r1**（inv `63cb93d9-…`，scoped，`ok:false`）：唯一红 = `acceptance-coverage`（扩包后 coverage 未同步）→ acceptance-author 冻结轮交付后消解。
  - **r2**（inv `e8935cf8-…`，scoped，`ok:false`）：唯一红 = `mutation-check`（缺 `mutationSkipReason` 字段）→ planner 补记后消解。
  - **r3**（inv `3753750d-…`，scoped，`ok:false`）：唯一红 = `docs-sync`（docs/notes 锚点缺 acceptance id）→ 主线程落锚点后消解。
  - **r4**（inv `9bf6aeb8-…`，scoped，`ok:true`）：七闸全绿（前窗收口态）。
  - **r5**（inv `7f3c2a41-…`，final，`ok:false`）：scoped 7 闸全绿 + `unit`/`acceptance` 红（唯一 SMK-A4）——前窗 closeout 承重 fact。
  - **r6**（inv `180e8b70-…`，final，`ok:false`，**本收口刷新承重 fact**）：真机轮四轮 fixer 终态树上实跑；scoped 7 闸全绿 + `unit`/`acceptance` 红（唯一 SMK-A4，同名单零新增红）。`unit` 计数 `1 failed / 168 passed / 6 skipped / 45 deselected`、`acceptance` 计数 `1 failed / 131 passed / 6 skipped / 45 deselected`。
- **r6 逐闸（实测 ms）**：taskpackage-required 180 / **firmware-build 9412（`build-pass` 真编译非降级——fixer 终态二进制 `xiaozhi.bin 0x2c3920`、`0xac6e0 bytes (20%) free`，与前窗 0x2c35b0 差异 = 本窗口四文件改动已入固件）** / contract-lint 167 / acceptance-coverage 178 / mutation-check 148（skipped 有因）/ docs-sync 173 / security 144 / unit 全量 / acceptance 全量。
- **host 固件验收腿（WSL ELF 全量，verifier 真机轮窗口实跑，planner 采信口径）**：**124 Tests / 2 Failures / 0 Ignored**——2 红 = WSA-A6:830 / WSA-A4:663（均在册他人红，owner=260903-device-ws-switch）；VRT-A2·E2:878 从 repo root 跑转绿（cwd 敏感判读已钉明）；在册 P2-A1 已由 RP-A1 修齿消解出失败清单。本包齿（v3_restore 6 + story_p3 10 + schedule_p2 7 + ui_art_integration 11 + story_page_stack 3）全绿。**infra 前置**：该腿实跑依赖 fixer 轮的官方 `EXTRA_CFLAGS='-Wno-error=comment'` 绕过（冻结验收件注释含 `docs/contracts/**` 序列触发 host `-Werror=comment`），缓存在 `host_app/build/build.ninja`——`fullclean` 后需重注，已登记 **ARQ-260912-host-werror-comment-acceptance-gap**（SHOULD，owner=architect+acceptance-author）。
- **真机轮（RP-A5 设备腿）**：见 §真机轮 RP-A5 全记录——两轮完整语音 + 挂断驻留 + SCHEDULE n=7 出表格 + human 肉眼裁定 PASS（2026-09-12）。设备腿证据 = 7 份 serial 日志（本目录 `rpa5-*.log`）。
- **Security gate**：r6 `ok:true`（`securityReviewRequired:false`）。本窗口无新公网入口、无新 secrets、无新依赖；fixer 改动全在设备面显示路径。
- **Docs sync**：r6 `ok:true`。本窗口 docs 增量 = `docs/ARCH-REVIEW-QUEUE.md` ARQ-260912 一条（§docs-delta 追记）；docs/contracts/** 与 docs/spec.md 零触碰（`git diff --name-only -- docs/contracts/ docs/spec.md` 空输出实测）。
- **Closeout**：`node .claude/scripts/closeout-check.mjs`（默认只读）→ 见 §Closeout。
- **影响面核对（step 11b）**：Pod worktree 内 diff 对索引不可见 → `detect_changes` 如实标注 N/A (worktree)，以 `git diff --numstat -- firmware/`（本窗口 4 文件 +250/−48）＋关键共享符号抽查替代：① 新符号 `kidpal_lcd_voice_chrome_front` 定义 = lcd_display.cc、消费 = kidpal_render_node.cc 单点桥，无第三方消费；② `KidPalVoiceChromeFront` 仅 render_node 桥经 C 接口触达；③ `rn_voice_chrome_front_locked` 只在 7 个已持锁前台切换点内调用（文件头线程约束注记）；④ 底纹 d50→d12 只在 `ksp_dots_dsc()` 单点消费；⑤ story 空态只在 cycle_next 的 KP_PAGE_STORY 分支。

## 逐 Acceptance 状态映射（RP-A1~A11）

| 项 | 状态 | 承载证据 |
|---|---|---|
| RP-A1 P2-A1 齿修复（方案 B） | **host 绿** | 前窗承载不变（schedule_p2 7/7）；host 全量复核在册 P2-A1 已消解出失败清单 |
| RP-A2 故事页排版 | **host 绿 / 真机 PASS** | 前窗齿全绿；真机轮故事页空态文案 human ✓（rpa5-r4-device-round.log + human 裁定） |
| RP-A3 课表页数据链 | **host 绿 + 真机兑现** | 真机轮 `cycle -> SCHEDULE (P2) n=7` + `schedule_render items=7 drawn=7` 出真实表格（rpa5-r4-device-round.log） |
| RP-A4 V3 字体体系并包 | **host 绿（资产/齿面）/ 运行时接线拆后续包（human 决策 2026-09-12）/ 真机腿 residual** | fonts_root 三字体入库 + 字体分型闭集齿（v3_restore test 6）绿；「猬猾狸」抽样可见真机腿未兑现（齿 6 被注释文本糊绿披露，见 §Findings）——运行时接线与其真机验证同属后续包 |
| RP-A5 真机整屏对照轮 | **PASS（human 2026-09-12）** | §真机轮 RP-A5 全记录：四轮 fixer 收敛 + human 肉眼裁定五项全 ✓；证据 7 份 serial 日志 |
| RP-A6 冻结面零越界 | **host 绿** | contract-lint PASS + `git diff --name-only -- docs/contracts/ docs/spec.md` 空输出；本窗口冻结验收件零改动 |
| RP-A7 host runner 终跑 | **host 绿（零新增回归）** | 124 Tests / 2 Failures——2 红 = 在册他人红（WSA-A6:830 / WSA-A4:663）；VRT-A2·E2 从 repo root 跑转绿 |
| RP-A8 P2/P3/P4 页面换皮 | **host 齿绿 + 真机 PASS** | v3_restore test 5 框族齿绿；真机轮 d12 底纹稀疏 ✓ + 答案页无猫 ✓（human 裁定） |
| RP-A9 五态演出换皮 | **host 齿绿 + 真机 PASS** | v3_restore test 1/2/3 绿；真机轮待机页干净 ✓、语音轮猫顶替笑脸 ✓、挂断回答驻留 top=4 ✓（human 裁定 + rpa5-r4-device-round.log） |
| RP-A10 P4 笔顺页观感 | **host 齿绿 + 真机 PASS（同轮收口）** | char_stroke 冻结齿复跑绿；真机轮按 human 整屏对照裁 PASS |
| RP-A11 新增验收齿 crossEnd 登记 | **DONE** | 前窗承载不变（v3_restore 6 齿红基线先行在案） |

## Simplification Debt

`grep -r "minimal:" firmware/main/display/ firmware/main/test/ tools/asset-pipeline/` 本窗口新增命中 0 处（前窗销账结论不变）。无新抽象债务：本窗口改动 = 缺陷修复（锁纪律/泄漏/死锁），非新抽象。本包无 `no-trigger` 简化债务条目。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| FYI | judgement-call | final 闸红（留红如案，跨包） | **SMK-A4 final 闸红（不伪造不代修）**：r6 `unit`/`acceptance` 两闸唯一红 = `test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（`server/acceptance/_artifacts/smoke-e2e/screen.md` 缺失）。与本包零相关（本包 `git diff -- server/` 零改动；先例 PKG-4 合并夜同一红同名单） | Basis: final r6 fact `ok:false` stdout（`AssertionError: SMK-A4 RED: …screen.md missing`，`test_smoke_e2e.acceptance.py:422`）+ `existing-red-inventory.md` R1（owner=260829-smoke-e2e）；heuristic: `[evidence]` 族红非本包固件面，不越权代采 | 留红如案、提 human 按红证据 ESCALATE 通道裁（红转交 owner=260829-smoke-e2e）。本包不代采不代修 |
| FYI | judgement-call | host runner 基线红（留红如案，跨包） | **host 基线 2 红留案 + VRT-A2 转绿 + P2-A1 已消解**：WSA-A6:830 / WSA-A4:663（owner=260903-device-ws-switch）——在册他人红，本包不代修；VRT-A2·E2:878 从 repo root 跑转绿（cwd 敏感判读已钉明）；在册 P2-A1 已由 RP-A1 修齿消解出失败清单 | Basis: `red-baseline-freeze.md` §一（human 指示 2026-09-12：不重跑、引在册）+ 真机轮窗口 host 全量 124/2F 实测计数；heuristic: 2 红均为证据齿类、本包零相关面 | 留红如案，基线消解走独立批 |
| SHOULD | judgement-call | 验收齿强度（本包暴露、非本包可独修） | **v3_restore 齿 6 被注释文本糊绿**：齿断言以文本出现为满足，`schedule_page.c:779-782` 注释（含字体分型关键词）即令齿转绿——三字体运行时接线缺失未被齿拦住，直至真机轮 human 肉眼发现「界面还是老的」才披露。acceptance-author 已留痕 REVISION LOG；同族隐患 = 其余文本匹配齿 | Basis: changed-code hunk `schedule_page.c` 本窗口 d12 注释块（`git diff` 实测在该注释邻域）+ human-decision `52833536`（齿强度缺陷原话）；heuristic: 文本匹配齿对「注释即满足」无防御，属齿强度自检缺一环（pod-retro 教训②） | 齿强度自检补强（注释豁免/结构锚定）走 acceptance-author + architect 治理批；本包不就地改冻结件。不阻塞本包合并（该齿守卫的接线面已由 human 决策拆后续包收口） |
| SHOULD | judgement-call | host 验收基建（已登记 ARQ） | **冻结验收件注释触发 host `-Werror=comment`**：`schedule_p2.acceptance.c:1103` / `story_p3.acceptance.c:2233` 注释含 `docs/contracts/**` 序列（`/*` 嵌套形态），host runner 编译失败；fixer 用官方 `EXTRA_CFLAGS='-Wno-error=comment'` 环境变量绕过（未改仓库文件），缓存于 `host_app/build/build.ninja`，`fullclean` 后需人工重注 | Basis: `docs/ARCH-REVIEW-QUEUE.md` ARQ-260912-host-werror-comment-acceptance-gap（SHOULD，2026-09-12 登记，fixer 轮披露）；heuristic: 绕过活会话隐性知识、跨会话不可见 = 运维面债务 | 按 ARQ-260912 处置（生成面注释约定 + 构建面把 flag 钉进 host_app CMake 二选一或并行）；下一个 host 验收批立项时兑现。不阻塞本包合并 |
| SHOULD | judgement-call | RP-A4 拆包边界（human 决策记录） | **三字体运行时接线拆后续包**：本包 fonts_root 三字体仅落资产（bin）+ 分型闭集齿，零运行时接线代码；lv_font_load 兑现与其真机腿（「猬猾狸」抽样可见）同属后续包。本包收口不含字体运行时效果 | Basis: human-decision pod-events `52833536`（2026-09-12 原话）；heuristic: human 显式拆包裁定，本 integrator 只记录不扩大 | 后续包立项时承接（接线 + 真机腿一并兑现）；本包 residual 记录见 §Residual Risk。不阻塞本包合并 |

**Kind 说明**：五行均 `judgement-call`（无一条是本包 repo diff 引入的 hard-violation；SMK-A4/host 2 红为在册留红，齿强度/ARQ/拆包三行均为已登记或已裁定的治理项）。无 BLOCKING → 不停合并门；但 final fact `ok:false` 触发红证据 ESCALATE（见 §合并门须知）。

## Five-Axis Review

1. **Correctness（对 taskpackage acceptance + V3 设计稿 + S0 决策记录）**：本窗口 = 真机轮缺陷修复兑现。锁纪律（r1）= 两段式「锁外 IO/合成、锁内对象树」+ 锁获取超时带日志，根因（锁外 LVGL 调用腐蚀共享堆殃及 WS/TLS 上行）对症；挂载（r1）= 姿态图入 `emoji_box_` 聊天 chrome（PKG-2 仲裁内）替代裸挂 lv_layer_top，旧图堆叠/泄漏清零；显隐（r2/r4）= armed && front 两条件与，五态互斥（猫顶替笑脸、不得同框）由 human 肉眼五项裁 ✓；底纹（r3）= d50→d12 有真机裁定依据（d50 整页平铺呈「很多黑点」）；死锁（r4）= 自注册 C 桥替代 `Board::GetInstance()` boot 期重入，boot 冒烟 + 两轮完整语音 + SCHEDULE n=7 实测兑现。RP-A3 课表真机腿（items=7 drawn=7）、RP-A9 挂断驻留 top=4（OQ3=c 语义）真机兑现。
2. **Test quality**：真机轮本身即最强验证（引擎 3 分钟零 opus 超时首轮抓住冻结级缺陷，human 持板目视收口）——host 齿对设备面缺陷（锁外 LVGL/裸挂顶层/构造期重入）结构性失明，恰坐实 ARQ-260911-verdict-uncovered-surface 的判据（设计 vs 验证面轴需真机轮补位）。齿强度缺陷（齿 6 糊绿）由本包暴露、已留痕，补强走治理批。
3. **Contract / boundary safety**：`docs/contracts/**` 与 `docs/spec.md` 零触碰（r6 contract-lint PASS + git 空输出实测）；冻结验收件本窗口零改动；自注册桥只在 lcd_display.cc（定义）/kidpal_render_node.cc（消费）两点，不扩 wire 面；`server/**` 零改动。
4. **Operability / security**：无新公网入口/凭据/依赖；死锁修复本身是运维面正向（boot 不再挂于 show_home 前）；遗留运维债务 = ARQ-260912（fullclean 后 host 验收需重注 EXTRA_CFLAGS，已登记有主）。
5. **Maintainability / simplicity**：修复方向 = 收敛而非扩面——显隐语义收敛到「armed && front」单标志 + 7 个前台切换点统一驱动；自注册桥带完整根因头注（`Board::GetInstance()` 禁忌写成注释纪律）；story/schedule 空态同族复用（#11 注释直接引用 R-S0.2）。

## S2 Exploratory Scan

TaskPackage 未声明 `s2ExploratoryRequired` / `s2ExploratoryGoals` ⇒ 本相位**未执行** S2 official exploratory scan（前窗结论不变，如实记录「未声明 ⇒ 未执行」）。required-by-risk 侧注记：`riskTier=high` 但 planner 未开该面；真机轮已由 human 持板收口，本包风险面均有机械齿/结构闸/真机证据承载。

## UI design-review 证据（bootstrap 期纯文本口径 · 真机轮收口后刷新）

- **Viewport coverage**：设备固定 400×300 1bpp RLCD 单一 viewport；P2/P3/P4 + 语音轮五态 + P4 笔顺页全页面在改（前窗口径不变）。
- **A11y summary**：1bpp 对比度（V3 黑白稿原生满足）+ 字号阶梯落位；HWV-A7 顺带核验归并真机轮（human 整屏对照裁 PASS）。
- **Design-drift summary（V3 稿 ↔ 实现）**：结构面 drift 由 v3_restore test 5/3/6 机判钉住；观感面 drift 前窗归 RP-A5 human 对照轮——**本窗口已收口**：human 肉眼裁定 d12 底纹稀疏 ✓、答案页无猫 ✓、故事空态文案 ✓、待机页干净 ✓、语音轮猫无笑脸 ✓（2026-09-12）。
- **External artifact reference**：V3 设计稿 `docs/ui/kidpal-ui-design-v3.html` + 定稿裁定快照 `tools/asset-pipeline/report/snapshots/`；真机 serial 证据 7 份（本目录 `rpa5-*.log`）。
- **Residual risk**：RP-A4 三字体**运行时**观感未收口（拆后续包，human 决策）——设备当前渲染用既有字体，缺字修复（「猬」）效果真机不可见，直至后续包接线 + 真机腿兑现；不新增 UI gate（本包无 UI BLOCKING finding）。

## Residual Risk

- Known limitation / 留红如案（均非本包引入、owner 已登记）：
  1. **SMK-A4 final 闸红**（existing-red R1，owner=260829-smoke-e2e）：`screen.md` 证据件缺口，本包不代采、不伪造，提 human ESCALATE 裁。
  2. **host 基线 2 红**（WSA-A6:830 / WSA-A4:663，existing-red R2-R3）：在册既存跨包红，owner=260903-device-ws-switch，本包不代修，基线消解走独立批。
- 已登记治理债（非本包独修、均有主）：
  3. **齿强度缺陷**（v3_restore 齿 6 被注释文本糊绿）：acceptance-author 已留痕 REVISION LOG；齿强度自检补强走治理批（§Findings SHOULD-3）。
  4. **ARQ-260912 host `-Werror=comment`**：fullclean 后 host 验收需重注 `EXTRA_CFLAGS='-Wno-error=comment'`（绕过缓存在 `host_app/build/build.ninja`）。
- Pending（human 决策 2026-09-12 已裁、属后续包承接）：
  - **RP-A4 三字体运行时接线 + 真机腿**（「猬猾狸」抽样可见）：拆后续包，接线与真机验证一并兑现。
- Human decision needed：
  - **红证据 ESCALATE**：final fact r6 `ok:false`（SMK-A4 留红如案 + host 2 红在册），提 human 裁「接受本包合并（红转交各自 owner / 独立批基线消解）或先修再合并」——由 supervisor-verdict-gate 机械闸 + planner 前置升级闸（`pre-merge-escalation-gate.mjs`）承接，本 integrator 不自判。
  - **risk-accepted 台账**：`taskpackage.decisionCoverage` 十条决策的 `landsIn` 无一为 `dropped` 且 `reason` 以 `risk-accepted` 开头（本相位实查 10 条）→ 本包无 risk-accepted 待追认项。
- 里程碑面：`deliveryImpact=milestone-affecting` 但无里程碑阶段跳变——`.delivery/milestones.jsonl` 本相位不改；`.planning` workstream 收口段归主线程。

## Closeout

`node .claude/scripts/closeout-check.mjs`（本相位实跑，默认只读、**不传 `--update-index`**）：制品补齐后 `requiredFiles:["docs-delta.md","review-packet.md"]` 两件 blocking 制品在本目录在场；`verifier-evidence/rounds/r6.json` = 本相位 `--evidence-mode=round` 新产承重 fact；`existing-red-inventory.md` 非 blocking 在场。
advisories（advisory-only，不阻塞按 SSOT）：① `verdict.json`（verify.mjs script fs-write 投影晋升）/ `verification.json`（integrator 投影）= non-bearing，承重 = r6 fact；② `pod-retro.md` 已 append 真机轮教训段（planner 收尾相位产物，Q5=warn）；③ `pod-events.jsonl` 在场（advisory 满足：真机轮 fixer-round `ee11264c` + human-decision `52833536` 等 14 事件）。
post-cutoff（>= 250725）不产 session-digest / loop-health / loop-metrics / pod-journal。

## Commit 冻结（合并门复核的对象）

- 本收口刷新 commit：固件 4 文件（fixer r1-r4 终态）+ 证据日志 5 件（rpa5-r1/r2/r3/r4-boot-smoke/r4-device-round）+ `docs/ARCH-REVIEW-QUEUE.md`（ARQ-260912）+ `verifier-evidence/rounds/r6.json` + `current/verdict.json`（r6 fs-write 投影）+ `verify-runs.jsonl` / `pod-events.jsonl` + 刷新制品 5 件（review-packet / verdict / verification / docs-delta / pod-retro）。commit 前按 AGENTS.md Hard Rule 用 `git diff --cached --stat` 核对；**不 stage `.codebase-memory/**`**（机器再生面常驻脏，Pod 分支禁止私带快照 commit）。
- 可回滚路径：全部交付物为 feature 分支 `dev/260911-render-pages` 上的本地 commit（**未 push、未动 main**）；回滚 = 分支级 revert（fixer 改动含前窗基底之上的增量，revert 本窗口 commit 即回退至 r5 前窗 closeout 态；d4a94f2 真机轮首轮证据与 0f5bf0b 批次③本体独立保留）。main 零暴露。
- 合并相位：supervisor 机械闸（`supervisor-verdict-gate.mjs`）裁断 → planner 前置升级闸（`pre-merge-escalation-gate.mjs`）→ 放行后 `merge-driver --pod` 本地合并（本包 mergeOrder 21，dependsOn 260911-screen-unification 已合并）。本 integrator 不自行判定通过、不执行 merge、不 push、不动 merge-driver。

## 合并门须知（短清单）

1. 无 BLOCKING finding；五行均为留红如案 / 已登记治理债 / human 已裁定拆包项。
2. 承重 fact = **r6**（phase final，scoped 7 闸绿 + unit/acceptance red = SMK-A4 在册 R1，`ok:false`）；supervisor 复核读 `verifier-evidence/rounds/r6.json`。
3. 真机轮 RP-A5 全绿 + human 肉眼裁定 PASS（2026-09-12，五项全 ✓）；证据 7 份 serial 日志在本目录。
4. 本包齿全绿（v3_restore 6 + story_p3 10 + schedule_p2 7 + ui_art 11 + stack 3）；host 全量 124/2F 零新增红（VRT-A2 从 repo root 跑转绿、P2-A1 已消解）。
5. 冻结面（`docs/contracts/**`、`docs/spec.md`、`managed_components/**`）git 零触碰（r6 contract-lint PASS + 空输出实测）；本窗口冻结验收件零改动。
6. **红证据 ESCALATE**：final fact `ok:false` 触发人类裁决通道（SMK-A4 + host 2 红留红如案，owner 均已登记），riskTier=high 照常升级。
7. human 待办（均不阻塞本包 merge）：SMK-A4 留红如案裁定（owner 260829-smoke-e2e）；host 基线 2 红走独立批；RP-A4 运行时接线后续包立项；ARQ-260912 / 齿强度补强进各自治理批。
