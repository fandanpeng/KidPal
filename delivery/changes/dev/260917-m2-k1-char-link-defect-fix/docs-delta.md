# Docs Delta · 260917-m2-k1-char-link-defect-fix

## 结论

**本包零 docs / contracts / spec 面改动（docsSyncRequired=false）。**

## 声明理由（引用 taskpackage docsSyncReason 逐字）

> 纯实现缺陷修复：契约 D3 无改动（SVG path 词汇表内解析补全）、spec 层需求行已冻结（S-F3.1 可行性注记已登记两缺陷修复路径），零 docs 面改动；修复细节随代码注释与 closeout 制品承载。

## 逐面核对

| 面 | 文件 | 本包动作 | 核对证据 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md`（D3 笔顺 JSON） | 零触碰（只读） | contract-lint 闸绿（final r3，exit 0）；git diff 零命中；E2 铁律在案——C 命令属 makemeahanzi SVG path 词汇表内解析补全、非契约改动（architect 快审确认归类，plan.md 裁定） |
| spec 面 | `docs/requirements.md` / `docs/architecture.md` | 零触碰 | docs-sync 闸绿（final r3，docsSyncRequired=false）；git diff 零命中；S-F3.1 可行性注记（两缺陷登记）在 2026-09-15 真机复验批已冻结在位 |
| docRefs | `docs/requirements.md` | 只读引用（S-F3.1 / NFR-PERF-2） | 无需更新——需求行语义未变，本包实现为既有缺陷的贴合性修复；live 复测（NFR-PERF-2 黄灯转绿判据）归 M2-K3 设备轮，届时由 K3 批面承载 |

## 核对方法

- `git diff --name-only | grep ^docs/` → 零命中（2026-09-18 integrator 收拢时实跑；diff 面 = firmware 4 文件 + 新验收件 + taskpackage/bookkeeping 面）。
- final r3（invocationId `5aaa1e91-2c09-4a93-8ef5-bb5f8ee5d9a6`）contract-lint / docs-sync / taskpackage-required 三闸 exit 0。

## 实现侧注记承载位置

- C 命令词汇表扩充依据与语义注释：`firmware/main/display/char_stroke.c`（cs_tokenize 接受集注释 + `cs_bezier_cubic_t` 三次贝塞尔公式注释）。
- 去栈机制注释：`firmware/main/display/char_page.cc`（`CHARPAGE_MAX_PTS` 定义处 + `charpage_parse_data` 预解析去栈注释 + `charpage_render_frame` 静态承载注释）。
- 栈定容收编依据注释：`firmware/main/boards/waveshare-s3-rlcd-4.2/custom_lcd_display.cc`（`KP_TASK_LVGL_STACK_SIZE` 定义处，替换 2026-09-15 一次性实验补丁注记）。
- 验收齿设计注记：`firmware/main/test/char_stroke.acceptance.c` 文件头 seam contract 块。
- 修复叙事与证据：本目录 `review-packet.md`。
