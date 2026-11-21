# dispatch-progress · builder · 261116-channel-resident-core（真机轮 C 修复轮）

> 本文件 = 当前派发（真机轮 C 修复轮：两层守卫，SCRF-C1..C4）的自包含进度快照。
> 覆盖写；不载 tokens / gates / rounds / verdict。权威判据归 verifier。

## 本轮目标

在 `server/gateway/front/front_app.py` 内实现两层守卫：① 素材侧段级静音 ASR 幻觉
过滤（主防线）；② 整理产物占位守卫（后防线，承重），让已冻结 SCRF-C1..C4 转绿；
并在 `server/tests/test_archive_guard_units.py` 纯 additive 扩测 + 8 条变异取证。

## 已完成段落（全部完成）

### 1. 新常量与纯函数（front_app.py，紧贴既有归档守卫区）

- `_FRONT_SILENT_ASR_HALLUCINATIONS`：段级整串集合，元素为 `_guard_compact` 形态
  （档 A 本仓真机 `okay`/`ok`；档 B 引擎同族多字条目；档 C 日语族，注释标注
  「家族模式推断、非本仓实录」）。不收中文应答词与单字。
- `_FRONT_ORGANIZE_PLACEHOLDERS`：整理产物占位整串集合（档 A 本轮实证 + 档 B
  变体），compact 形态（`na` / `nocontent` / `nothingtoorganize`）。
- `_ORGANIZE_BODY_MIN_CHARS = 2`、`_ORGANIZE_BRACKETED_MAX_CHARS = 16`、
  `_ORGANIZE_SILENT_RELEASE_REASONS`（empty_body / insignificant_body /
  placeholder_body）。
- `_guard_compact` / `_is_silent_asr_hallucination` / `_organize_body_reject_reason`
  （R1 空 / R4 括号剥壳 / R2 过短 / R3 占位表；引号族明确排除）。
- `_archive_material_reject_reason` 与 `_FRONT_ARCHIVE_HALLUCINATIONS` 逐字未动。

### 2. 素材侧段级过滤

`_story_feed_from_stt` 在引导播报窗判定之后、`now_dt` / `routing` 之前插入整串
命中 drop（`return False` + `story_feed_guard dropped=silent_asr_hallucination`
审计），stt 转发路径与 pending_stop 挂起态零改动。

### 3. 整理产物守卫与 reason 通道

`_organize_and_archive(raw, corr_id=None, device_id=None, *, reason_sink=None)`；
`_organize_and_archive_offloop` 透传。删多行分支的 `or reply.strip()`；单行未命中
占位逐字保留 `title=cand[:12]`、`body=cand`；主体守卫在 `archive_story` 之前；
每个 `return None` append reason（llm_unavailable / empty_reply / R1-R4 /
archive_valueerror；空素材直调分支归 empty_body）。

### 4. 收尾分档处置

`_story_finish_session` 构造 `reason_sink` 并 keyword 透传；`record is None` 分两档：
静默释放类（不安抚、逐腿 `_push_story_list` 页面退出帧、审计
`reason=organize_placeholder body_reason=<细分>`）；丢稿类逐字保留安抚 + 退出帧 +
`reason=organize_failed`（追加 body_reason）。三锚顺序与 `_issue_finish_downlink(session_id`
字面未动，新分支不写 `reason=empty_material`。

### 5. 单元测（纯 additive）

`_load_output_guard()` 第二 AST 抽取器 + `OrganizeOutputGuardUnits` 6 条：
R1/R2/R3/R4 八样本逐条 reason、R4 双层与 16 字边界、真实短故事/引号族/title==body
放行、段级命中与不命中、集合不变式（compact 形态且 >=2）、占位表 compact 且与
段级表不重叠。既有 `_load_guard()` 与四条 case 逐字未动。

## 实跑结果

| 面 | 命令 | 结果 |
|---|---|---|
| 目标件 | `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/test_session_channel_resident.acceptance.py server/tests/test_archive_guard_units.py` | **47 passed / 0 failed / 99 subtests passed** |
| server 全量 | `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance server/tests` | **799 passed / 0 failed / 9 deselected / 137 subtests passed** |

## 变异证据（8/8 实跑，整文件字节写回 + sha256 比对）

基线 sha256（front_app.py）：`ea09f0b505229ff8dc51a33597466dcf31fb0bdf1f5d41b73966910b43bd8bcc`
（工作树 CRLF 字节口径）。8/8 `sha256_restored_matches = true`。

| # | 变异 | 转红（含 co-red） |
|---|---|---|
| 1 | 删 R4 括号判据块 | C2 mechanical（【无】/（本轮素材不足））+ 单测 R1-R4、R4 双层边界 |
| 2 | MIN_CHARS 2 -> 0 | C2 mechanical（嗯）+ 单测 R2 |
| 3 | 删段级过滤调用 | C1、C4 hit family、C4 pending、C4 set invariant |
| 4 | 段级集合加入「嗯」 | **B10 case 2**、C4 hit family、C4 set invariant、单测命中/不变式 |
| 5 | 静默分支加回安抚 | C2 端到端 |
| 6 | 静默分支去掉页面退出帧 | C2 端到端 + C3 llm_unavailable |
| 7 | reason=organize_placeholder -> empty_material | C2 端到端（审计分档断言） |
| 8 | 单行前置 + 主体守卫一起移到 archive_story 之后 | C2 端到端 + C2 mechanical 八样本 |

## 偏差与如实申报

1. **R4 判序空间**：派发规格列举 R1→R2→R3→R4；实现为 R1→R4→R2→R3。原因 =
   冻结验收 C2 对「【无】」期望 `placeholder_body`，而其 compact=1 会先命中 R2
   `insignificant_body`；验收不可改，故让结构判据 R4 先于长度判据 R2（非括号单字
   「嗯」仍走 R2）。判据集合与语义未变，仅重叠样本的分档优先级调整。
2. **`_load_output_guard` wanted 增列 `_FRONT_GUARD_STRIP_RE`**：派发清单漏列，但
   `_guard_compact` 函数体直接引用它，不抽取会 exec 期 NameError（假红）。
3. **空素材直调分支 reason = `empty_body`**：派发 reason 枚举未单列该分支；收尾单源
   已在调用前短路，故仅直调形态可达，归静默释放类。
4. 工作树文件为 CRLF；变异脚本按字节读写并逐条比对，未用 `git checkout`。

## 行为面观察（申报节）

- 真机端到端未运行：GAP-C1（引擎收到 active=false 后不再产尾音 ASR）与 GAP-C2
  （设备 ORGANIZING 页真实退出）仍归 human 持板复测；host 只证 front 下发帧序与
  零归档/零 seq。
- 段级过滤/产物守卫均落 front host 可跑路径，无固件落点；本批未产生新的 host 与
  真机不等价接缝（EQ-C1/EQ-C2 已在冻结件申报）。

## 下一步

- 无实现段遗留；结构化回执交 planner（含 8 条变异全文、偏差申报、超读申报）。