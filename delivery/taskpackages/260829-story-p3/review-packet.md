# Review Packet · 260829-story-p3（包 8 故事双面：列表 / 录制归档 / 回看回听 / 成就盖章）

> dev-loop integrator 收口 · 2026-09-08 · 分支 `dev/260829-story-p3`（worktree `C:/AI/KidPal-pod-story-p3`，commit 冻结、**不 push、不 merge**）
> 承重磅据 = final verifier-round fact `.claude/taskpackages/260829-story-p3/verifier-evidence/rounds/r3.json`
> （`authority:"bearing"`，phase=final，round 3，invocationId `8ac23ae5-57f4-461a-ad5d-6cf42379a652`，
> `ok:true`，`observedGateExit:0`，9 闸全绿）。`.claude/taskpackages/current/verdict.json` 只是 non-bearing 兼容投影，
> 本 packet 的判断不来自它。
> integrator 亲跑三条实机闸的计数在 §3（与 planner 提供的数字**全部一致**，无差异需申诉）。

## 0. Story 摘要（人读版）

孩子说「我要讲故事」→ 云端 OpenClaw 判意图下发 `control/start_recording` → 板子切录音态演出
（REC 闪点 + 大时长 + 声波），网关把该会话切成累积态（不按句送对话）→ 孩子按 KEY 停录 →
整段转写归档成 `stories/*.md` → 保存那一瞬双圈斜盖章「故事徽章·第 N 颗已入册」+ pose-jump 定格
0.8s，N 与右上角故事果计数器严格同步 → 之后可在 P3 列表（`story_list`，含契约 `total` 全集计数）
里选任意一条进阅读态（`story_text` 正文分屏滚动 + BOOT 长按翻页 + KEY 朗读）。误判可挽回：
录音中取消 / 「讲错啦」回闲聊 / 停录 10 秒内「重录」/ 漏判后「刚才那段是故事」补归档（V31 四条）。

## 1. 实现清单（双面 + 部署面 + 修复）

### 设备侧（firmware）

- `firmware/main/display/story_page.c`（新增 1212 行）/ `story_page.h`（新增 111 行）：冻结离线契约
  14 函数 + 3 类型 + 盖章几何常量。列表装载与 `total` 双钉计数器（`kidpal_story_counter` 优先契约
  `total`、缺省回退 `items.length`）；页内输入特例（KEY 单击光标回绕 D1 / 双击进阅读 / BOOT 长按翻页
  末屏钳制 / KEY 朗读请求 / 停录 / 取消，数值码 KSA_* 与冻结验收逐一对齐）；录音态 tick（闪点 + 声波 +
  `mm:ss` 大时长）；盖章帧几何 `{rings:2, inner_dashed:1, angle_deci:-80, hold_ms:800}` 与
  `on_saved` 的 D3 语义（`N = counter+1` 本地推 + 同一 `saved_id` 幂等 + 刷新以云端 `total` 权威覆盖）；
  自研零依赖 JSON 解析器（`ksy_*`，畸形载荷一律 `-1` 不盲渲染，V27）；纯逻辑 + 纯 LVGL
  （`lv_obj_/lv_label/lv_anim` only）→ host_app linux target 与设备树两侧同源编译。
- `firmware/main/mcp_server.cc`（+101/-1）：① **治愈 main 既有破口**——`case 4`（schedule）的
  `app.Schedule(lambda)` 未闭合（`});`+`break`+`}` 缺失，致 `case 3`/`default` 整段嵌进 lambda 体，
  设备 target 必编译红）；② `case 2`（`start_recording`）由 stub 升级为真消费：
  `kidpal_story_on_control` 切录音态 + `app.StartListening()` + 快照渲染；③ additive 补
  `case VRT_ROUTE_STORY_LIST` / `case VRT_ROUTE_STORY_TEXT`（解析失败 → 友好降级提示，成功 →
  `app.Schedule` 真渲染），每条 dispatch 生成 `kdp-<ts>` correlation id 并打 `degrade=` 结构化日志。
- 接线：`firmware/main/CMakeLists.txt`（+`display/story_page.c`）、
  `firmware/main/test/host_app/main/CMakeLists.txt`（`HOST_ACCEPTANCE_SRCS` 增冻结验收 + 被测源，
  `host_infra_smoke` 保 LAST 的 boundary 注册序守卫未破）。

### 云侧（server）

- `server/gateway/story.py`（新增 409 行）：契约 §1.3 ③/④ producer（`list_stories` 全集 `total`
  与分页解耦、`story_text` 恰三键无生字位）+ 归档面（`archive_story` 真落盘 `stories/<date>-<slug>.md`，
  空转写 `ValueError` 拒档，同 id 幂等重写；`read_story_file` 靠记录头哨兵区分「真故事 / 杂物文件」）
  + `StoryAccumulator`（manual 聆听累积态、V31 四条：`cancel` 丢弃即丢弃不可挽回 / `redo` 10 秒窗内
  真删文件放回缓冲 / `recover` 漏判用近句环补档 / 「讲错啦」回闲聊）+ **内存叠压硬约束落实**
  （每会话 `sentence_window=24` 句 + `char_window=4000` 字双上限、`max_sessions=8` LRU、
  `total_buffer_bytes()` 可测面、构造期零磁盘副作用）。纯 python，import 期不碰 fastapi/front。
- `server/gateway/front/front_app.py`（+45/-5）：连接作用域 `session_id` 旁路消费——relay 循环里识别
  `view=control && action=start_recording` → `_get_story_acc().start(session_id)` 切累积态；wire 输出
  形状与逐块帧序零变更（决策 23「front 只消费不重发」；per-block `build_display_tool_calls([block])[0]`
  的安全性由冻结齿 GRF-A3 的 `len(calls)==len(blocks)` 不变式背书，见 §10 轴 1）。

### 部署面（server/deploy，制品 + 接线，未上线）

- `server/deploy/openclaw-system-prompt.v3.md`（新增 160 行）：故事 producer 扩展（判「我要讲故事」→
  `control/start_recording`、停录下一回合只起 ≤12 字标题并回填 `story_text`、`story_list.total` = 全集
  而非本页、V31 四条回退判断、本回合不读写文件）。CR 归一后实测：相对 v2 **102 行逐字保留 + 58 行新增
  + 12 行仅版本字样改动**（v2 基座与 char C1 纪律完整继承，无行为删改）。
- `apply-xiaozhi-overlay.sh`（`PROMPT_SRC`/`PROMPT_DST` 切 v3）+ `provider.config.yaml`
  （`system_prompt.{artifact,source,deploy_target}` 三键同切 v3）+ `OPENCLAW_SYSTEM_PROMPT_CARRIER`
  （版本演进声明）。**两面同切**由 story 自身冻结齿与已重指为版本无关的 CHR-E3 双重机械断言。

### 验收面（冻结，acceptance-author 产）

- `firmware/main/test/story_p3.acceptance.c`（新增 1998 行，8 TEST_CASE / A1-A6 + render + boundary）。
- `server/acceptance/test_story_p3.acceptance.py`（新增 1020 行，offline 20 齿 + live 1 齿机械 skip）。
- `server/acceptance/test_char_stroke.acceptance.py`（+77/-12）：**CHR-E3 版本无关重指**
  （human 裁 `c54bbcde`）——从字面 `.v2` 接入改为「解析两面得当前接线版 → 两面相等 → 该版存在且非空
  → 正文逐字携带 char C1 锚点」，姊妹齿 `test_e3_prompt_v2_landed_with_discipline` 未动一行。

## 2. 验收 ↔ 断言 ↔ 红绿映射

| AC | 设备侧齿（host ELF，TEST_CASE 起始行） | 云侧齿（pytest） | 实跑结果 |
|---|---|---|---|
| P3-A1（structural） | `P3 故事页消费面装配结构` @865（模块存在 + CMakeLists 引用 + mcp 两路由 + render 调用点≥2） | `test_a1_story_module_landed_and_reached`（四入口齐 + 被服务路径引用） | PASS / PASS |
| P3-A2 列表 + 故事果计数 | `story_list 载荷解析 + 故事果计数双钉` @970 | `test_a2_list_total_is_full_set_not_page_slice`、`_ordering_item_shape_and_junk_ignored`、`_empty_dir_is_zero_not_fake` | PASS / 3 PASS |
| P3-A3 光标下移 + 双击进阅读 | `列表光标下移 + KEY 双击进阅读` @1134 | （设备面专属，云侧无对偶） | PASS |
| P3-A4 录制上传归档 | `control/start_recording 进录音态 + REC 演出 + 停录` @1229 | `test_a4_archive_writes_real_md_and_roundtrips`、`_rejects_empty_transcript`、`_title_fallback_is_nonempty`、`_accumulate_mode_not_sentence_by_chat` | PASS / 4 PASS |
| P3-A5 回看回听 | `阅读态正文分屏 + BOOT 长按翻页` @1429 | `test_a5_story_text_shape_no_newword_field`（恰三键、无生字标注位） | PASS / PASS |
| P3-A6 成就盖章 | `盖章帧双圈/-8°/0.8s + 文案 N 与计数同步` @1611 | （V31 四齿承载 N 同步上游事实：`_redo_within_window_undoes_archive`、`_redo_after_window_keeps_archive`、`_recover_rescues_missed_story`、`_cancel_discards_without_archiving`、`_cancelled_content_not_recoverable`、`_wrong_take_returns_to_chat`） | PASS / 6 PASS |
| P3-E2 两面证据 + 回归 | `P3 页渲染面真帧` @1770（反白/×N/翻页/录音·盖章四态像素互异）、`扫描器自检 + 润色/生字高亮守卫` @1929 | `test_e2_existing_directionB_acceptance_green`（subprocess 复跑 directionB 套 exit 0）+ `test_boundary_contract_surface_untouched` + 2 条 buffer 有界齿 | PASS / PASS |

- **structural / suiteGates 豁免枚举**（dev-loop 硬要求，supervisor 门核此项存在）：本轮
  acceptance-coverage 输出 `structural: ["P3-A1"]`、`suiteGates: []`。P3-A1 豁免理由 = 纯装配结构齿
  （文件落盘 + CMakeLists 引用 + mcp 路由字符串 + 跨文件调用点计数），无可断言行为语义，按
  `kind:"structural"` 显式对象标记（260731 成文通道），非 prose 洗白；`suiteGates` 为空 →
  无需 human glance。**逐 id 变异不适用于 P3-A1**（结构门非逐 AC 敏感判定对象，M-4）。
- 冻结相位三决策全按断言原样兑现：D1 列表回绕 / 阅读末屏钳制、D2 录音不预推计数、D3 盖章 N 本地推
  → 刷新以云端 `total` 权威覆盖（**未**改判为「取云端保存响应值」，故未触发 `story_text` 增 `total`
  的 ① 契约触点，闸 A ④ 判定成立且被 boundary 齿机器核销）。

## 3. 三闸实测计数（integrator 亲手跑，2026-09-08）

| 闸 | 命令 | 实测 | 期望 | 判定 |
|---|---|---|---|---|
| C host ELF | `cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- bash espidf_wsl.sh build` 后**从仓库根** `wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf` | **68 Tests 2 Failures 0 Ignored**，exit 2 | 68T2F | **一致** |
| PY 全量套 | `cd server && python -m pytest acceptance/ -q --import-mode=importlib` | **8 failed / 84 passed / 36 skipped** | 8/84/36 | **一致** |
| 设备 build | `cd firmware && "<idf venv python>" c:/tmp/idf_run.py build` | **`Project build complete`**，`xiaozhi.bin binary size 0x2b63e0 bytes. Smallest app partition is 0x3f0000. 0x139c20 bytes (31%) free`，wrapper RC=0 | RC=0 / 0x2b63e0 | **一致** |

**无一不符，无需向 planner 申诉数字差异。** 为保证一手性做的额外固化：

- C 侧：先 `touch story_page.c / story_page.h / story_p3.acceptance.c / mcp_server.cc` 强制真重编
  （ninja 输出可见 `[1/4] Building ... story_page.c.o`、`[2/4] ... story_p3.acceptance.c.o`、
  `[3/4] Linking libmain.a`、`[4/4] Linking host_infra_test.elf`），排除「沿用 07:23 陈旧 ELF 假复跑」。
- 2 红逐条归属：`device_ws_switch.acceptance.c:663`（WSA-A4 `flash.log` 落盘缺失）与 `:830`
  （WSA-A6 `ecs-teardown-evidence.md` 落盘缺失）；本包 8 齿逐条 PASS（@865/970/1134/1229/1429/1611/1770/1929
  全部命中 `:PASS`，story 相关输出恰 8 行 = 无遗漏无多余）。
- 设备 build 真重编证据：日志含 `[1/10] Building C object ... display/story_page.c.obj`、
  `[5/10] Building CXX object ... mcp_server.cc.obj`、`[8/10] Linking CXX executable xiaozhi.elf`，
  `xiaozhi.bin` mtime 与本次运行时刻吻合（非沿用 07:29 产物）。
- 环境事实遵循：ELF 从 `host_app/` 目录跑会多出第 3 红（既有 `VRT-A2` cwd 敏感齿）——本次严格按
  仓库根姿势跑，实测 2 红未复现该伪影。PY 走 Windows `python 3.14.0 + pytest 9.1.1`（WSL python 无
  pytest，本批包一致先例）。设备 build 走 wrapper（裸 `idf.py` 不在 PATH）。
- 单文件复跑：`test_story_p3.acceptance.py` = **20 passed, 1 skipped**（skip 项 =
  `StoryP3LiveAcceptance::test_live_story_roundtrip_evidence`，conftest live 机械闸）；
  `test_char_stroke.acceptance.py` = **8 passed, 3 skipped**（重指后 char 面无连带红）。

## 4. 变异证据摘要（TaskPackage `mutationEvidence` 6 条，全 turnedRed + sensitive）

| 面 / 文件 | 变异（把实现改坏） | 转红齿 |
|---|---|---|
| PY `server/gateway/story.py` | `list_stories` 以页切片 `len(items)` 顶替全集 `total`（契约 §一 硬约束反面） | `test_a2_list_total_is_full_set_not_page_slice`（`-k a2` 单跑红，全量跑恰此 1 红、无连带） |
| C `story_page.c` | 列表光标去回绕（`(c+1)%n` → 到底钳住）破 D1 | P3-A3 齿 `:1162` |
| C `story_page.c` | `on_control` 的 `ctrl_action == 2` 改 `== 99`（control 帧到达不切录音态） | P3-A4 齿 `:1246`（并连带 render `:1880`，证渲染面敏感） |
| C `story_page.c` | 屏数指示格式串中点分隔符 ` · ` 改 ` / ` | P3-A5 齿 `:1502`（逐字文案齿） |
| C `story_page.c` | 盖章斜角 `KSTORY_STAMP_ANGLE_DECI (-80)` 改 `(0)` | P3-A6 齿 `:1638`（冻结几何四元组） |
| C 合变观测 | 上述 4 处 C 变异一并施加 | render 齿 `:1880`（四态画面互异断言失效） |

- 计数链：基线 `68T2F` → 合变 `68T7F`（2 地板 + 5 story 齿），逐齿按 TEST_CASE 名归因、无串扰；
  全部还原后重建复跑回 `68T2F`。
- 还原凭据：`story_page.c` sha256 `051b9c7a…`、`story.py` sha256 `1748e15f…` 均核验回基线。
- 过程诚实记录（planner 留痕，pod-events `c8055cf7`）：变异还原最初用 `git checkout`，对 builder
  **未 commit** 的新建文件静默 no-op 致 sha256 漂移，改用内存逆变换精确还原后核验一致——
  「未 commit 工作树产物的变异还原不得依赖 git checkout」是本批通用教训。
- integrator 旁证：我本次重跑 host ELF 与 PY 全量套的失败集与「变异全还原」状态完全吻合
  （C 无 story 齿红、PY 无 a2 红），即工作树不存在残留变异。

## 5. 跨包回归结论（一手对照实验）

| 姿势 | 命令 | 结果 |
|---|---|---|
| **main 基线**（`bc0951f`，临时 detached worktree `C:/AI/KidPal-intbase-main`，跑完即 `worktree remove`） | `python -m pytest acceptance/ -q --import-mode=importlib` | **8 failed / 64 passed / 35 skipped** |
| **本分支**（`419bdee` + integrator docs 改动） | 同上 | **8 failed / 84 passed / 36 skipped** |

- **失败集逐 ID 相同**（`smoke_e2e` 7 条：a1/a2/a3_device_roundtrip_evidence/a3_mp3/a4_display_line/e1/e2；
  加 `voice_roundtrip_b::test_a4_adherence_recheck_evidence_record`）——即 planner 所载既有地板
  （smoke_e2e 7 = 已 superseded 的 HTTP 时代结构断言；vrt a4 = 遵循度记录缺「处置」条目）。
  **零新红，且本分支净增 +20 passed / +1 skipped**（本包 20 offline + 1 live 齿 + char 面重指后仍 8P/3skip）。
- 机器佐证「不是本包引入」：`git diff bc0951f..HEAD --name-only --
  server/acceptance/test_smoke_e2e.acceptance.py server/acceptance/test_voice_roundtrip_b.acceptance.py
  firmware/main/test/device_ws_switch.acceptance.c docs/evidence` → **空输出**（这 8+2 条红所读的
  文件与证据件，本包一行未动）。
- C 侧同型对照：2 红 = WSA-A4/A6，读的是 gitignore 的本地-only 证据件（`flash.log` /
  `ecs-teardown-evidence.md`），先例 home-p1 `43T2F` / schedule-p2 `39T2F` 同地板。
- **forward-bump 撞包已闭合**：`test_char_stroke.acceptance.py::test_e3_prompt_v2_wired_into_deploy`
  重指后，story 侧要求 v3 与 char 侧要求「当前接线版含 char C1」两条 oracle 同时为真（实测 8P/3skip），
  不再互斥。通用病灶已由 planner 登记 `ARQ-260908-versioned-guard-version-literal-coupling`
  （integrator 未复登，见 §6 finding F2 的补充证据）。
- 本包治愈 main 缺陷：`mcp_server.cc` case4 lambda 未闭合（`f2a67dd` 落地即破，随其
  `firmware-build(降级)` 门混入 main），界内 3 行闭合 + 补 case5/6；设备构建现 RC=0 真绿。
  门根已登 `ARQ-260908-firmware-build-degraded-gate`。

## 6. 架构 / 依赖卫生

- **契约边界**：`docs/contracts/**` 零改动（闸 A architect-reviewer 判 ④ NO_CONTRACT_CHANGE，
  六项逐条预审在 taskpackage `notes.gateA_contractPrereview`）；boundary 齿
  `test_boundary_contract_surface_untouched` 机器核销「view 白名单 / `MAX_DATA_BYTES` 不回退」。
  `story_list`/`story_text` 两 view 与 `total` 字段均为契约既有面（`total` 于 2026-08-29 经契约门批准
  additive），`display_schema.py` 本包未动 → **未绕过 contracts、未新增本地类型平行面**。
- **重复实现**：`story_page.c` 内自带 `ksy_*` JSON 解析器（非复用 `cJSON`），与 `schedule_page.c`
  既有同款模式一致（纯逻辑 host/设备同源、不引 `cJSON` 进 host seam），属**已确立的第三例**（char /
  schedule / story）而非本包新造的重复；真正的收口动作（提公共 display-payload parser）超出本包
  界内，记 §11 OPTIONAL。
- **跨层调用**：云侧归档落盘归 `server/gateway/story.py`（薄网关业务面，与 `schedule.py` /
  `display_schema.py` 纯模块同型），设备只消费下发块（哑终端纯洁性）；无 firmware→server 反向依赖、
  无循环依赖（`front_app` → `story` 单向，`story` 不 import front）。
- **依赖卫生**：零新增第三方依赖（story.py 只用 stdlib：`base64/hashlib/re/unicodedata/collections/
  datetime/pathlib`；story_page.c 零外部依赖）。`package.json` / `requirements` 面无新增条目。
- **内存叠压硬约束**（notes 所载 builder 红线）：`StoryAccumulator` 双上限 + `max_sessions=8` LRU +
  取消即删会话 + 构造期不触文件系统；两条 `test_buffer_bounded_*` 齿机器可测（逐会话环 + 跨会话），
  未在网关同进程面再叠大常驻缓存（char-stroke `char_cache` ~19MB 教训未复发）。
- **越界核对**：`git diff bc0951f..HEAD --name-only` 全部落在 builder/acceptance-author/integrator
  授权面（`firmware/main/**`、`server/**`、`docs/ARCH-REVIEW-QUEUE.md`(planner)、
  `docs/ADR.md`(integrator)、`changes/dev/**`、`.claude/taskpackages/**`)；参考树
  `ESP32-S3-RLCD-4.2-Demo/**` 零接触；`docs/contracts/**` 零接触。

## 7. 机械闸预检（integrator 实跑，逐条命令 + exit）

| 闸 | 命令 | 结果 |
|---|---|---|
| final 证据有效性 | `node .claude/scripts/verifier-evidence-check.mjs --change-id 260829-story-p3 --phase final --round 3 --invocation-id 8ac23ae5-57f4-461a-ad5d-6cf42379a652` | **PASS**，`{"ok":true,"factOk":true,"observedGateExit":0,"evidenceRef":"...rounds/r3.json"}`，EXIT 0 |
| docs-sync | `node .claude/scripts/docs-sync-check.mjs .claude/taskpackages/current/taskpackage.json` | **PASS** EXIT 0（docRefs 三件齐核，`docsNoChangeReason:null` = 有 docs 产出无需该字段） |
| firmware-build（闸本体） | `node .claude/scripts/firmware-build-check.mjs` | PASS 但 **`degraded:true`**（`idf.py 不可用…降级放行，非假绿`）→ 已由 §3 第 3 行 integrator 亲跑真编译补强 |
| firmware-build（严格档） | `VERIFY_FW_BUILD_REQUIRED=1 node .claude/scripts/firmware-build-check.mjs` | `ok:false`「强制真编译失败」——严格档存在但默认关（本包未擅自开全局严格档，门策略归 ARQ-260908-firmware-build-degraded-gate） |
| closeout | `node .claude/scripts/closeout-check.mjs`（默认只读，**未传 `--update-index`**，遵 planner 指示） | **PASS，EXIT 0**：`requiredFiles = [docs-delta.md, review-packet.md]` 齐、`indexProjection:"skipped-read-only"`、`changeId` 正确解析。三条 advisory 全部如实接住（见下） |
| 承重闸（F-04 单源） | closeout-check 内部程序化调用 `taskpackage-required-check` | 未红（承重字段 + decisionCoverage 深校验由该闸单源承担） |

closeout-check 的 3 条 advisory 逐条归属（**均不阻塞**，按 SSOT 表 verdict/verification 与 pod-retro
均为 advisory-only）：

1. `machine evidence missing: verdict.json, verification.json`（`changes/dev/<id>/` 下）——
   脚本产线件、advisory-only；照 260907-g-relay-fix / 260829-char-stroke 先例**不由 agent 手造**。
   承重事实在 `.claude/taskpackages/260829-story-p3/verifier-evidence/rounds/r3.json`，
   non-bearing 投影 `current/verdict.json` 已在仓。
2. `pod-retro.md missing`——归 planner 收尾相位（Q5=warn、零追溯），integrator 不代写。
3. `loop-retro trigger threshold reached: 5 pending + 2 waiting = 7 ≥ 3`——发起责任 = 主线程
   （`/loop-retro`），非本包门。
4. （未出现的 advisory）`pod-events.jsonl not found` 未 fire = live 事件件在档
   （`.claude/taskpackages/260829-story-p3/pod-events.jsonl`，13 事件）。

- **未重跑 `verify.mjs --evidence-mode=round --phase=final --round 3`**，如实记录原因：final 承重 fact
  的 round 序号单调且发布用 `linkSync`（已存在即失败），`preflightProducer` 有
  `expected.round !== state.nextRound → throw "round 3 is not next completed ordinal 4"`——r3 已定版即
  immutable，重跑只会在跑闸前 preflight 红。铸造 r4 属 verifier/planner 相位（须 planner 提供新
  invocationId），且 integrator 无权改判 final round。故本包按 dev-loop「checker PASS + fact ok:true
  才进 closeout」路径执行：**承重事实 = r3（已校验有效）**，实机三闸由 §3 一手复跑补强，
  二者均在档可查。
- `detect_changes`：项目 `C-AI-KidPal-pod-story-p3` 可调，`base=main / merge_base=bc0951f /
  changed_files=29`，但 **`seed_symbols: 0`**（本次新增 `story_page.c` / `story.py` 的符号未入图——
  索引在 launch 期建、此后未增量刷新）→ 逐符号爆炸半径不可算，**如实降级**为：
  `git diff --name-only` 全清单核对（§6 越界核对）+ 关键共享符号 `trace_path` 抽查：
  `_relay_display_blocks_to_device` inbound `callers_total:2` = `pump_upstream_to_client`(hop1) +
  `ws_gateway`(hop2)，全在 `front_app.py` 内（改动外溢半径受控）；
  `build_display_tool_calls` 跨包消费面 = `test_g_relay_fix.acceptance.py`（以 callable 方式加载，
  非源码文本耦合），其冻结不变式 `len(calls)==len(blocks)` 恰为本次 per-block 重构托底，实测该套绿。
  re-index 需求交主线程（不自行触发 full index）。

## 8. Change Size

- Files changed：**30**（14 新增 A / 16 修改 M），`git diff bc0951f...index --stat` =
  **6506 insertions(+), 121 deletions(-)**（含 `.codebase-memory/**` 两份工具派生快照，系
  planner/builder 早先 commit 带入，非本包人手产物）。
- 其中实现面 `firmware/** + server/**` = **16 files, 5441 insertions(+), 36 deletions(-)**；
  冻结验收两文件 = **3018 行**（`story_p3.acceptance.c` 1998 + `test_story_p3.acceptance.py` 1020），
  另有 `test_char_stroke.acceptance.py` +77/-12（CHR-E3 重指）；
  实现纯新增（`story_page.c` 1212 + `story.py` 409 + prompt v3 160）= **1781 行**；
  治理/证据面（`.claude/taskpackages/**` + `docs/ARCH-REVIEW-QUEUE.md`(planner) +
  `docs/ADR.md`(integrator) + `changes/dev/**`(integrator) + 红基线摘要 210 +
  `.codebase-memory/**` 派生）= 其余。
- Scope：单包双面（firmware + server）+ 部署面 + 一处 main 缺陷治愈，与 TaskPackage
  `subsystem:"firmware+server"` 一致，无未声明的第三面。
- **Split recommendation：不建议拆**。story_page / story.py / prompt v3 三件套是同一 AC 集合的
  两面承载，拆开会让 P3-A4（录制→归档→盖章）跨包失联。**建议另开**的是本 packet §9 F1/F3/F4
  指向的三件事（设备输入面接线、live 证据齿形状收紧、mcp 破口的门根治理），它们都**不属于**本包
  冻结验收面。
- 值得 human 知的一行数：**本包 1212 行设备侧新代码 + 409 行云侧新代码，一次未 push**；
  回滚成本 = 单 revert 合并 commit（见 §14）。

## 9. Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| **SHOULD** | judgement-call | Operability（发布面 / 门有效性） | **F1 · main 的设备构建长期无门，本包已治愈一次实际破损但门根未修**：`mcp_server.cc` case4 lambda 未闭合（`case` 标签嵌进 lambda 体）自 `f2a67dd` 起就在 main，char-stroke 自己的 `firmware-build(降级)` 门未拦、后续各包 host runner 不编该文件也看不见，直到本包 acceptance-author 才发现。本包界内修复 + integrator 亲跑设备 build RC=0 证绿（§3）；但 `firmware-build-check.mjs` 在本环境**恒 `degraded:true`**（只探裸 PATH 的 `idf.py`，本仓真实通路是 venv python + `c:/tmp/idf_run.py` wrapper），下一个改 device-only 源文件的包仍会无门通过。 | 实测：`firmware-build-check.mjs` 默认跑 `ok:true, degraded:true, reason:"idf.py 不可用…"`；严格档 `VERIFY_FW_BUILD_REQUIRED=1` 跑 `ok:false`；`r3.json` 载 `firmware-build {ok:true,degraded:true}`（r1/r2 同）；diff hunk `firmware/main/mcp_server.cc:243-245`（补 `});`+`break`+`}`）。 | 本包内无可修动作（门脚本在 `.claude/scripts/`，非本包界内，且改门语义 = 工程门变更须走治理批）。门根已由 planner 登记 **`ARQ-260908-firmware-build-degraded-gate`**（Track A，owner=architect+human）；supervisor 建议：合并本包即消除已知红，**但请把该 ARQ 排在下一次 firmware 包 launch 之前**，否则同型破损会再入 main。 |
| **SHOULD** | judgement-call | Test quality（版本化制品守卫耦合版本字面） | **F2 · live 探针面仍钉旧版字面，carrier 声明与被测版本已分叉**：carrier/overlay/provider 三面现指 v3，而 `test_voice_roundtrip_b.acceptance.py:116 SYSTEM_PROMPT_PATH` 钉 `openclaw-system-prompt.v1.md`（VRT-A4/A7 live 读它）、`test_char_stroke.acceptance.py:144 PROMPT_V2_PATH` 钉 `.v2.md`（CHR-A8/A9 live 读它，:685/:724 用作 live prompt 源）。一旦有 live 环境，这两组齿是「所测 ≠ 生产所用」。同型失效的更坏一面：`test_schedule_p2.acceptance.py:583-586`（P2-A6）以「`server/deploy/**` 全文 grep 完整字面 `.v2.md`」判「渲染源已切 v2」，该字面现仅由 `.v2.md` 自身头注 + `openclaw-schedule.config.yaml:4` 注记 + carrier 历史句维持——**断言仍绿但已不检验其声称检验之物**（假绿比假红贵）。 | 逐文件实测：`grep -rl "openclaw-system-prompt\.v1\.md" server/deploy/` → 仅 `./openclaw-system-prompt.v1.md` 自身；`.v2.md` → v2 自身 + schedule.config.yaml + carrier；`.v3.md` → overlay（真接线）+ v3 + carrier + provider.config.yaml。P2-A6 语料构造式 `DEPLOY_DIR.rglob("*") if p.is_file()`（含 `.md`），对照 story/CHR-E3 的形状只认 `PROMPT_SRC` 与 `provider.config.yaml` 两面结构化指向并显式声明「散文提及不算接线」（`test_char_stroke.acceptance.py:479-480`）。planner 已登 **`ARQ-260908-versioned-guard-version-literal-coupling`**（Track A）载 char a8/a9 一支。 | 本包**不扩改**（无 live 环境则重指无法实证；改冻结验收文件非 integrator 权限）。动作：① integrator 已在 `server/deploy/README.md` 新节 + `OPENCLAW_SYSTEM_PROMPT_CARRIER` 消费面条目**如实写明漂移现状**（不再无条件声称「探针以本声明版为唯一源」），并把「重指前勿改回无条件表述」钉进 carrier；② **交 planner**：把 P2-A6 假绿与 VRT `SYSTEM_PROMPT_PATH` 钉 v1 两个**新增同型实例**并入 ARQ-260908 证据列（本 packet 即证据，integrator 无权改 ARQ 文件）；③ 交 acceptance-author 在下次版本化包 launch 前把三处（VRT-A4/A7、CHR-A8/A9、P2-A6）按 story-p3 载体齿 / CHR-E3 重指后的两面解析形状版本无关化。 |
| **SHOULD** | hard-violation（对 DEV-GOVERNANCE「probe ≠ exercise、live 证据须真跑」的承载形状） | Test quality / 证据面 | **F3 · live 四事实齿的语料 = closeout 制品目录，任何如实描述本包的散文都能把它判绿**：`test_story_roundtrip_evidence`（`test_story_p3.acceptance.py:982`，`LIVE_EVIDENCE_DIR = REPO_ROOT/"changes"/"dev"/"260829-story-p3"`，:189）把该目录下**所有** `.md/.log/.txt/.json` 拼成一个 corpus，只做四组 keyword 存在性判断（`我要讲故事`/`start_recording`；`转写`/`ASR`/`stt`；`stories`+`.md`+`归档`；`徽章`/`盖章`/`story_list`）+ 五个失败标记词否定。integrator 一手复现假绿两次：**(a)** 把红基线摘要副本放进该目录 → `RUN_LIVE=1 pytest ...::test_live_story_roundtrip_evidence` = **1 passed（0.03s）**，零真采音、零真归档（逐文件归因：该副本单文件即满足全部四组 needle，docs-delta.md 满足 3/4、缺 fact②）；**(b)** 撤掉副本、只留本 packet 与 docs-delta 再跑 = 又 **1 passed（0.01s）**。即**任何如实描述本包的 closeout 散文都会让它绿**，撤副本不是修复、只是延迟。 | 实测两条 pass 输出 + 中间一次 fail（`(a)` 与 `(b)` 之间，仅 docs-delta 在档时红在 `P3-A4 live RED: evidence does not record the 整段转写 fact`）。源码：`test_story_p3.acceptance.py:970-981 _evidence_corpus`（`rglob("*")` + suffix 白名单，无文件名/子目录约束、无事实形状要求）、`:189 LIVE_EVIDENCE_DIR = REPO_ROOT/"changes"/"dev"/"260829-story-p3"`、`:982` 四组 keyword 断言 + 五个失败标记词否定。规范来源：`DEV-GOVERNANCE §2`「probe ≠ exercise」（该齿 docstring 自己引它），以及 taskpackage `P3-E2`「真实采音殿后 9b」。 | 本包内 integrator 已做两件：① **不**把红基线副本晋升进该目录（增益为零且额外扩大假绿面，理由写进 `docs-delta.md` §2 末行）；② 本 finding 显式在档，supervisor/human 读到此齿若为绿**不得**当作 live 已核销。**交 planner→acceptance-author（冻结文件，integrator 无权改）**：把 `_evidence_corpus` 收窄到专用子目录或指定记录文件（如 `changes/dev/<id>/evidence/live-story-roundtrip.md`），并要求事实形状而非散文（逐事实时间戳 + `stories/` 下真实文件名 + 音频/转写摘要哈希 + 采样设备 token），失败标记词判定同步升级为结构化 outcome 字段。若判「closeout 目录内散文可作 live 凭据」为可接受设计意图，则须 human 明示并记 watchlist（存疑向严）。 |
| **SHOULD** | judgement-call | Correctness / 两面分工口径 | **F4 · prompt v3 让 LLM 逐字复述整段转写并回填一个它拿不到的 `id`，而真正会算 id/落盘的云侧 producer 未接入任何生产路径**：v3「停录 → 整段转写起标题出正文」节要求模型「正文逐字来自整段转写，不擅自改写、不摘要、不丢句子」且「`id` 由系统给定（形如 `YYYY-MM-DD-slug`），你只需回填 title + text」；但 ① 现有链路里没有任何把 `archive_story()` 生成的 id 送进模型回合的管道，② `story.py::story_text()` / `list_stories()` 两个 producer 在生产路径零调用（全仓 grep：`import story`/`gateway.story` 只出现在 `front_app.py` 与验收文件），③ 让 LLM 整段复述 bulk 数据正是 char-stroke 已付出 109s 延迟代价、并用「出口自动拼装」根治掉的同一病灶。 | prompt hunk：`server/deploy/openclaw-system-prompt.v3.md`（`/tmp/v3.txt` 归一视图 :101-113）；代码 hunk：`server/gateway/story.py:210 story_text()`、`:177 list_stories()`、`:92 archive_story()`；调用面 grep 证据：`grep -rn "gw_story\|archive_story\|list_stories" server/gateway/` 仅命中 `front_app.py` 的 import、`_get_story_acc()` 构造与 `.start()`；对照先例 = char-stroke review-packet §4「v1 时代 109s ← LLM 逐 token 复述坐标 JSON」。 | 不阻塞合并（live 故事面今日不通，见 F4 与 §12）。**交 planner 定夺**：9b 接线前把分工改回「网关 `archive_story` 落盘 → 网关用 `story.py::story_text(record)` 组块下发，模型一回合只出 title」，即 prompt v3.1 或改本节措辞（删「回填 text」与「id 由系统给定」两句），并把「bulk 正文不经 LLM」写成与 char C1 同级的硬纪律。 |
| **NIT** | judgement-call | Correctness（长故事静默截断） | **F5 ·** 三处正文上限互不一致且设备侧静默截断：云侧累积环 `char_window=4000` 字 → 网关 display schema `MAX_DATA_BYTES=65536` B → 设备 `kidpal_story_text_t.text[2048]` B（CJK 约 680 字）。超 2047 字节的正文被 `ksy_put` 逐字节丢弃后**解析仍返回成功**，模型与网关都不知丢了尾；截断点若落在多字节序列中间，会留半截 UTF-8 尾（LVGL 渲染成乱码或不显，不崩）。`mcp_server.cc:304` 注释「全文不丢」在该路径上不成立。 | hunk：`story_page.c:142-147`（`ksy_put` 满则不落）、`:150-231`（`ksy_string` 无溢出标志、仍 `return 1`）、`story_page.h:53`（`text[2048] -- 正文, safe-truncated`）、`story.py:251-259`（`char_window=4000`）、`display_schema.py:39`（`MAX_DATA_BYTES=65536`）。**注意这是冻结验收明许的形状**：`story_p3.acceptance.c:42/65/82` 与 `:1109-1118` 把「超长字段 safe-truncate、仍须解析成功」钉为期望（故非 builder 违约，也非 BLOCKING）。 | 本包不改（改即推翻冻结语义，须经 planner 重派）。建议 9b 前由 planner 决定：把三处上限对齐成一条（例如云侧按设备 2048B 预算截断并显式标 `truncated`，或正文分屏下发），顺手修正 `mcp_server.cc` 那句「全文不丢」。 |
| **NIT** | judgement-call | Operability（会话键可复用） | **F6 ·** 累积态会话键 `session_id = f"conn-{id(websocket)}"` 用 CPython 对象地址，连接关闭后地址可被新连接复用，而 `StoryAccumulator` 仅在超过 `max_sessions=8` 时 LRU 淘汰、**连接关闭不 pop** → 今日无消费方故无影响，9b 接线后可能出现「新连接继承上一个孩子的累积态/录音态」的跨会话串味。 | hunk：`front_app.py:260`（session_id 构造）、`story.py:270-280`（`_ensure` 只在超界时 LRU 淘汰）、`story.py:345-353`（仅 `cancel()` 会 pop 会话）。 | 本包不改（`routing()` 生产零调用，改属扩界）。随 F5（9b 接线）一并：用不可复用键（`uuid4` 或连接对象强引用）+ WS 断开钩子里 `pop` 会话。 |
| **FYI** | judgement-call | Correctness（视图模型 vs 生产接线，包声明内但需读人知） | **F7 ·** 设备的**页内输入面**（`kidpal_story_page_input` KEY/BOOT 手势、`kidpal_story_on_saved` 盖章触发、`kidpal_story_rec_tick` 录音 tick、`kidpal_story_stamp_spec`）与云侧**停录归档触发**（`stop()`/`feed()`/`routing()`/`recover()`/`redo()`）**均无生产调用者**——今日真接进生产的是：设备 `mcp case2/5/6`（control + story_list + story_text → `kidpal_story_page_render` 真渲染）与 front 的 `.start()` 旁路消费。冻结验收按包声明的证据面（P3-A1 结构齿只要求「模块落盘 + 被服务路径引用 ≥1 code 文件」、C 侧 A1 tooth3 只要求 mcp 两路由 + render 调用点 ≥2）与此完全一致，且与已 merged 的 schedule-p2 同型（`kidpal_schedule_page_input` 同样零生产调用者），**非本包违约**；但只读 AC 文本（「KEY 短按光标下移」「按 KEY 停止上传归档」）会误以为设备今日已能响应。 | 调用点 grep（`firmware/main` 内 `_page_input|_on_saved|_rec_tick|_stamp_spec` 除定义/头/验收外零命中；`server/gateway` 内 `.stop(|.feed(|.routing(` 除注释外零命中）+ 冻结齿原文 `test_story_p3.acceptance.py:317-338`、`story_p3.acceptance.c:885-905` + 先例 `firmware/main/display/schedule_page.c:583`。 | 无需本包动作；supervisor 合并门请把「包 8 交付 = 视图模型 + 渲染面，交互与归档触发接线殿后」当作本包 shipped 边界读。建议 planner 在 `.planning/kidpal-mvp/progress.md` 收口行按此口径写（integrator 无 `.planning` 写权限，见 §13），并在 9b 的 intake 里显式收这四项为必接面。 |

**无 BLOCKING finding**（合并门可进）。F1–F4 是本包界外或冻结语义所迫，F5/F6 为小面，F7 是口径澄清。
结构闸（§7 各条 PASS/FAIL 与 degraded 事实）**未复述为 finding**，只按约定进 verification 段；
上表 finding 全部来自 gate 全绿之后的五轴语义 review。

## 10. Five-Axis Review（gate 全绿仍完整执行）

1. **Correctness（对每个声明的 `Sx-Ay` / `P3-Ay`）**：A2 双钉计数器 + `total` 全集语义、A3 D1 回绕、
   A4 `start_recording` 消费 + REC 演出 + 空转写拒档、A5 分屏与逐字屏数文案、A6 冻结几何与 D3 N 同步，
   全部有对应红绿齿（§2）。边界条件实核：空列表（`count<=0` 光标不动 / `total 0` 非假装有故事）、
   `items` 超 `KSTORY_LIST_MAX` 显式 `-1` 降级不静默丢行（`story_page.c:535-542`）、
   `offset` 超界回空页而 `total` 原值、cap 不足时 `stamp_text`/`rec_time` 一律 `-1` 拒绝而非溢出、
   `NULL` 视图与 `KEY_ID_COUNT` 越界键零污染、LVGL 未初始化（`lv_screen_active()==NULL`）不崩。
   跨面一致性缺口见 F4/F5/F7；`redo` 窗口判定用注入时刻（可测），但 `now < last_stop_now` 的时钟回拨
   会被当窗内放行（无生产调用方，随 9b 一并看）。
2. **Test quality**：断言真行为——PY `archive_story` 真落盘真读回（tmp 目录 + 文件存在 + 内容回环）、
   `redo` 真 `unlink` 文件、buffer 有界用 `total_buffer_bytes()`/`recent_depth()` 机器可测；
   C 侧 render 齿走**真帧像素采样**比对四态互异（不是「调用没崩」），故变异 4 处 C 破坏全转红。
   错误路径与降级路径有齿（畸形 JSON / 缺字段 / 超 cap / 脏块降级不静默）。弱点如实记：
   ① live 四事实齿形状不成立（F3）；② 「被服务路径消费」的 A1 齿只要求引用≥1，
   与 F7 的接线缺口之间存在设计让出的空间（这是冻结验收的选择，非漏写）。
   ③ keyword-existence 型断言（P2-A6 / VRT-E4 / live 齿）对散文可被满足——同族弱点，已入 F2/F3。
3. **Contract / boundary safety**：零契约改动（§6 机器佐证）；`story_text` 恰三键、无生字标注位
   （P3-E4 已裁，boundary 齿钉住「不回退白名单与大小上限」）；`total` 用法与契约 §一:95 的
   「不得以 `items.length` 顶替」逐字对应（且该反例正是 PY 变异点）；`control/start_recording`
   走 `FORWARDABLE_CONTROL_ACTIONS` 既有白名单（`display_schema.py:45` 本包未动）；
   LLM 一回合不读写文件的分工在 prompt v3 与 `story.py` 归属注释两侧一致。
   未发现影子类型/平行契约。
4. **Operability / security**：每条设备 dispatch 生成 `kdp-<ts>` correlation id 且日志
   结构化（`story_list_dispatch corr_id=… items=… counter=… degrade=false`），降级路径给儿童友好
   文案（「故事列表有点问题，稍后再试试」）而非裸错误，`degrade=true` 机器可 grep；零新端口/零鉴权改动
   （token 面未触）；零 secret 落仓（`STORIES_DIR` 走 `KIDPAL_STORIES_DIR` 环境变量，缺省
   `/opt/kidpal/stories`，与 secrets.env 纪律一致）；设备侧 `title`/`text` 全部经 `ksy_copy_str`
   带界拷贝，未见缓冲区溢出面（新解析器为手写，是本包最大的攻击面增量，值得记：它只读入
   网关已过 `validate_display` 的载荷，且所有写入受 cap 约束）。`securityReviewRequired` 未声明，
   security 闸按 `false` 通过——本包无新增外部输入面（故事内容来自既有 ASR→display 通道），
   该判定成立。**残余隐私面（FYI，不新增 finding）**：孩子整段口述以明文 MD 长期留存于 ECS，
   除 10 秒 `redo` 窗外无任何删除/保留路径——MVP 单孩子设备可接受，建议 9b/隐私面复议。
5. **Maintainability / simplicity**：改动局部（两新文件 + 三处接线 + 一处 3 行修复），无过早抽象
   （没为「将来的作品类页面」抽基类，`story_page.c` 与 `schedule_page.c` 各自成形）、无重复实现的
   新增（`ksy_*` 是第三例，见 §6）；`session_id` 参数 additive、默认 `None` 保持旧调用形状；
   4 处 `minimal:` 标记全部指向 9b 观感殿后（§11 台账）。`front_app` per-block 重构略微低效
   （每块重新 parse 一次 JSON，旁路消费再 parse 一次），但换来了「wire 输出零变更」的可证性，
   本包判为值得，不打 `shrink`。

## 11. Simplification Debt（`minimal:` 标记台账）

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| `firmware/main/display/story_page.c:1029` | 布局常量按 host seam 400x300 定坐标 | 真机 RLCD 观感校准 | **有**：9b 真机相位（P3-E2 显式殿后） |
| `firmware/main/display/story_page.c:1102` | 故事果图标 `fruit` 压在 `KSTORY_LIST_MAX(16)` 显示位上限 | 真实滚动/多位计数 | **有**：孩子故事数 >16（文本 `×N` 仍显真值，仅图标封顶） |
| `firmware/main/display/story_page.c:1134` | 屏数指示 `total_screens` 显示位钳 12 | 长正文真实分栏 | **有**：正文 >12 屏（与 F5 的 2048B 截断同源，先修截断再谈 12 屏） |
| `firmware/main/mcp_server.cc:322` | 阅读态分页行高硬取 4 行/屏的估算 | 按字号/行高 1.72 真排版 | **有**：9b 真机排版面（spec S-F2.3 已定 13.5px/1.72，本包未接设备字体度量） |

四例全部带殿后去向，无「无触发」项——本轮**没有** `no-trigger` 简化债。

## 12. Residual Risk

**已知限制（本包 shipped 边界）**

1. live 端到端未通：真采音→ASR→整段归档→列表回读→盖章演出 五事实殿后 9b（P3-E2），
   且承载它们的 live 齿形状不成立（F3）——合并后**不得**据该齿绿判断 live 已核销。
2. 设备交互与归档触发接线殿后（F7）：P3 页今日能渲染下发的列表/正文/录音态，
   但 KEY/BOOT 页内手势与保存 ack→盖章尚无生产调用方。
3. `story_text` 正文 2048B 静默截断（F5）；长故事在设备上看不全。
4. prompt v3 与云侧 producer 的正文归属口径待统一（F4），否则 9b 会撞上 LLM 复述 bulk 的延迟与丢句风险。
5. live 面版本字面漂移（F2）：VRT-A4/A7 读 v1、CHR-A8/A9 读 v2，被接线版是 v3。
6. 部署未执行：v3 制品与接线在仓，ECS 仍是旧版 prompt；上线动作序已写进
   `server/deploy/README.md`「v3 上 ECS 的动作序」。
7. S-F2.5 云端润色按 P3-E3 后置，不进本包（PROPOSAL §7 留根），未做任何预埋。
8. 图索引未刷新（`detect_changes seed_symbols=0`），逐符号爆炸半径核对降级为 §7 所列替代姿势。

**decisionCoverage 中 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目**

`taskpackage.json.decisionCoverage` 共 4 条（PROPOSAL-§2-包8 → `acceptance:P3-A2`；
contract-§一-story_list-total → `acceptance:P3-A6`；PROPOSAL-§7-S-F2.5留根 → `exitCriteria:P3-E3`；
PROPOSAL-§3-麦克风采音殿后 → `exitCriteria:P3-E2`），`landsIn` 全部落在 acceptance/exitCriteria，
**`landsIn:"dropped"` 条目 0 条、`risk-accepted` 开头条目 0 条** → 本包无 human risk-accepted 残留需列。
（注：F5 的截断、F7 的接线殿后属「本包证据面之外」，未被登记为 dropped 决策；若 human 认为
F4/F5 应以「risk-accepted」形式进 decisionCoverage，该补登记动作归 planner——integrator 无 taskpackage
执行 ledger 写权限外的改判权。）

**后续建议（不阻塞）**

- 提公共 `display_payload` 解析器（char / schedule / story 三份手写 parser 并存）。`OPTIONAL`
  级别，触发条件 = 第四例出现（包 U 换皮或 9b 若再加一类 view）。
- `.planning/kidpal-mvp/progress.md` 收口段与 `changes/INDEX.md` 单行投影（见 §13 交回项）。

## 13. 未执行 / 不适用项（如实记录）

- **S2 official exploratory scan：未执行**。TaskPackage 未声明 `s2ExploratoryRequired` /
  `s2ExploratoryGoals`（`rigor:"full"`、`riskTier:"high"`，但 planner 未置该字段），按角色边界
  「只在 TaskPackage 声明时执行」，不自创题目、不替代 verifier。若 supervisor 判 high 档须补
  探索式证据，请由 planner 追加声明后重派（所需 `targetPersona` / `metadata` 四件套我这边可即时执行）。
- **UI design-review 证据：本包未声明 `designReviewRequired`**，故不产 design-review 证据段。
  需说明的是本包**确有 UI 工作面**（P3 四态渲染：列表/阅读/录音/盖章），其证据以
  `story_p3.acceptance.c` 的 **render 齿真帧像素采样**（@1770，四态可见差异互异）承载，
  而**非** viewport/a11y/design-drift 摘要；`UIUX-DESIGN-BRIEF` 的对齐属包 U（换皮）面。
  该「有 UI 但未声明 design-review」的 intake 现状交 planner 判断是否补声明，本包不擅自补做、
  也不新增闸。
- **未做**：`.planning/kidpal-mvp/progress.md` 收口段（`.planning/**` 不在本次授权写面内）、
  `changes/INDEX.md` 行投影（遵 planner 指示未传 `--update-index`；INDEX 现仅有 char-stroke 行、
  缺 story-p3 行 → **交回 planner/主线程**跑 `node .claude/scripts/changes-index.mjs --upsert
  260829-story-p3` 或授权 `closeout-check --update-index`）、`docs/ARCH-REVIEW-QUEUE.md` 复登
  （planner 已登两条，本 packet 只引用；F2/F3 新增同型实例并入 ARQ 的动作亦交回 planner）。
- **里程碑面**：`taskpackage.json` 未置 `deliveryMilestoneStateChanged` / `milestoneStateChanged`，
  本包不触发 `.delivery/milestones.jsonl` 跳变，故未改 milestones（closeout-check 内该检查亦未 fire）。

## 14. 制品、commit 与合并门输入

- 晋升 `changes/dev/260829-story-p3/`：`docs-delta.md` + `review-packet.md`（两件 blocking）。
  post-cutoff 口径确认：不产 `session-digest.md` / `loop-health.md` / `loop-metrics.json` /
  `pod-journal.jsonl` / `test-evidence.md` / `final-summary.md` / `changes/dev` 下 taskpackage 快照
  （逐一按 closeout 制品 SSOT 表核过，见 docs-delta §3）；`pod-events.jsonl` 存在（advisory，非 blocking）。
  `pod-retro.md` 归 planner 收尾相位。
- commit：见本节末（feature 分支本地 commit，**不 push、不在 main 上 commit**）。
- 回滚路径：本包全部改动集中在单一 merge commit，回滚 = `git revert -m 1 <merge-sha>`（本地 main，
  由 merge-driver 合并后生成）；只回滚部署面接线而不回滚代码 = 单独 revert
  `server/deploy/{apply-xiaozhi-overlay.sh,provider.config.yaml,OPENCLAW_SYSTEM_PROMPT_CARRIER}`
  三处即可让渲染面退回 v2（v1/v2 文件始终共存、未改）。设备侧已烧录件不受影响（本包未做 OTA）。
- supervisor 输入清单：final fact `r3.json`（ok:true，9 闸）+ `verifier-evidence-check` PASS、
  §3 三闸一手计数、§5 跨包 main-baseline 对照（8/64/35 → 8/84/36 逐 ID 同集）、§4 变异 6 条、
  §9 findings（**0 BLOCKING / 4 SHOULD / 2 NIT / 1 FYI**）、§11 简化债 4 条全带殿后触发、
  §12 残余风险 8 条 + decisionCoverage dropped 为 0。
- **本 packet 不自行判定通过**：合并门由 planner 跑 `supervisor-verdict-gate.mjs` 机械闸裁断；
  riskTier=high 且 §5/§9 存在跨包面（prompt forward-bump、main 缺陷治愈、live 证据齿形状），
  若判 ESCALATE 则入人类门（human 已在本批签过两条相关决策：`c54bbcde` 版本无关重指、
  `221bb868` 主线程变异实验授权）。

### commit（本次收口）

- docs-sync + closeout 制品 = 单独一个 commit（本文件写就后紧接 `git add` →
  `git diff --cached --stat` 核对 → commit），hash 见交付回报。
- 分支既有三点：`f5309c7`（冻结验收落盘）→ `f7c82d4`（实现转绿）→ **`419bdee`**
  （planner 在主线程补登 `ARQ-260908-firmware-build-degraded-gate`，本次收口期间发生；
  planner 交给我的 brief 记的是「两个已提交点」，**实测分支现为三点**，已如实核对内容 =
  仅 ARQ 一行追加，与代码面无冲突）。
- 工作树 `.codebase-memory/{artifact.json,graph.db.zst}` 为工具派生快照，本次 docs commit
  **有意不纳入 staged**（留给主线程按 re-index 节奏处理）。
