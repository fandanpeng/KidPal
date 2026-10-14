# Review Packet · 260917-m2-k2-retract-p4-three-page

> workstream m2-char-window ｜ M2-K2 ｜ mergeOrder 29 ｜ rigor full ｜ riskTier high ｜ subsystem firmware
> integrator 收拢制品（2026-09-18）；合并门判读归 supervisor-verdict-gate 机械闸，本包不 push、不 merge、不在 main 上 commit。

## Change Size

- **实现（commit `3109717`，firmware 面 9 文件 +65/-66）**：
  - `firmware/main/display/kidpal_render_node.h` +17/-12——`kidpal_page_id_t` 重排 `KP_PAGE_HOME=0 / KP_PAGE_SCHEDULE=1 / KP_PAGE_STORY=2 / KP_PAGE_COUNT=3`，`KP_PAGE_CHAR` 整值撤出；两处写侧死态 API（`cache_char` / `cache_char_data`）声明处加注。
  - `firmware/main/display/kidpal_render_node.cc` +6/-21——删 `rn_render_kidpal_page_locked` 的 `KP_PAGE_CHAR` 停靠 case；`cycle_next` 步进基数走 `KP_PAGE_COUNT`（未改，仍同源）；静态 `s_has_char_data` / `s_char_json` 对加写侧死态注记。
  - `firmware/main/key_gesture_parser.h` +6/-5——`page_id_t` 撤 `PAGE_P4`（`PAGE_COUNT` 自动 4→3）；`key_page_next` 回绕注记同步。
  - `firmware/main/key_gesture_parser.cc` +1/-1——`key_route_global(BOOT, CLICK)` 注释（语义零改动）。
  - `firmware/main/display/keybar.h` +7/-7 / `keybar.c` +5/-5——页槽表撤 `case PAGE_P4:` 兜底、`default:` 改 `return -1`（越界交防御列）；页点总数注记收回 deviation。
  - `firmware/main/display/lcd_display.cc` +4/-10——`kidpal_keybar_sync_locked` 的 `KP_PAGE_* → PAGE_n` 映射塌成恒等（越界回落 `PAGE_P1`，防御语义不变）。
  - `firmware/main/mcp_server.cc` +10/-5——case 3 查字路由注释随撤页对账（**路由语义与调用点零改动**）。
  - `firmware/main/test/host_app/main/CMakeLists.txt` +9/-0——新冻结验收 `../../retract_p4_three_page.acceptance.c` 登记进 `HOST_ACCEPTANCE_SRCS`（**置于 `host_infra_smoke.acceptance.c` 之前**；门 100 行段，同步注释）。
- **验收冻结（commit `2c89733`，firmware 面 3 文件 +1215/-32）**：
  - 新增 `firmware/main/test/retract_p4_three_page.acceptance.c`（**1116 行，7 个 TEST_CASE，17 处 coverage 标记**，+1116/-0）。
  - `firmware/main/test/key_layer_dispatch.acceptance.c` +35/-11（KD-E1 撤页守卫留痕修订）。
  - `firmware/main/test/keybar_three_slot.acceptance.c` +64/-21（KB-E1 deviation 对账收回两处同源化修订）。
- **Net diff**：firmware 面合计 **12 文件 +1280/-98**（实现 9 + 验收 3）。
- **簿记面（非实现改动，随收口 commit 冻结）**：`delivery/taskpackages/<id>/taskpackage.json`（红基线 + mutationEvidence 回填）、`delivery/taskpackages/current/taskpackage.json`（planner finalize 投影）、`input-brief-acceptance-author.md` / `input-brief-builder.md`、`pod-events.jsonl`（append）、`verify-runs.jsonl`、`verifier-evidence/`（immutable，只引路径不复制正文）。
- **包外 gov 提交（非本包可归因）**：`13cd811` `docs/ARCH-REVIEW-QUEUE.md` +1（ARQ-260918-pod-session-settings-identity-unobservable 登记，本 Pod intake 触发，纯 append、早于本包验收冻结）。
- **Scope**：板端 `firmware/` 单一子系统——① 本地浏览页循环 4→3 + 两页枚举对齐 spec 页序；② 键帽条中槽页点 4→3（收回 M1-K2 deviation）。云端 `server/` 零改动（`git diff 01f8b97..HEAD -- server/` 零命中），`docs/` 除包外 gov 提交外零触碰。
- **Split recommendation**：不拆——撤页为一个语义动作，两枚举 + 循环 + 页点必须同批对齐（半撤即页序/页点脱钩），验收件单文件冻结、一次 RED→GREEN 链承载。

## Verification Evidence

- **Structural gates**：final r2 fact 九闸全绿（`ok:true`、`observedGateExit:0`、invocationId `97a5acfc-ad34-45db-b9a3-8f2d2475e983`，2026-09-18T14:47:04Z–14:47:19Z，evidenceRef `delivery/taskpackages/260917-m2-k2-retract-p4-three-page/verifier-evidence/rounds/r2.json`）——taskpackage-required / **firmware-build（device build 真编译，`VERIFY_FW_BUILD_REQUIRED=1`，`xiaozhi.bin` 0x30b6f0 bytes、分区余量 11%，未降级）** / contract-lint / acceptance-coverage / mutation-check / docs-sync / security / unit（pytest 37 passed）/ acceptance（oracle-pass，firmwareAcceptanceFiles 21）。scoped r1 fact 七闸全绿（invocationId `37ab5573-c8e0-4f09-9ff8-ebf1bfe72d2b`，rounds/r1.json）。
- **机械校验**：`.claude/scripts/verifier-evidence-check.mjs --change-id 260917-m2-k2-retract-p4-three-page --phase final --round 2 --invocation-id 97a5acfc-…` **integrator 实跑 exit 0、`factOk:true`、`observedGateExit:0`**（2026-09-18）——final evidence 有效，可进 closeout。
- **host runner 全量（integrator 独立复跑，非闸境）**：`cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- bash ./espidf_wsl.sh build` 后直调 `./build/host_infra_test.elf`（**elf 相对 host_app/**）→ **`46 Tests / 0 Failures / 0 Ignored`、EXIT=0**（2026-09-18 integrator 实跑）。账实相符：既有基线 **39T/0F @ ca7ff81**（K2 launch 锚）+ 新件 7 齿 = 46；本包义务红 KD-E1（撤页前 39T/1F）转绿。
- **server 面（跨子系统覆盖边界）**：`server/` 本包零改动；pytest `37 passed`（integrator 独立复跑，2026-09-18）。本包无 server 侧事件流/契约面改动，故「跨子系统事件流面」无本包可归因的增量；覆盖边界 = 固件 host 逻辑面（host runner 全量）+ 设备编译面（device build 闸），真机渲染/手感面 deferred（见 demo 对账行）。
- **Mutation check**：TaskPackage `mutationEvidence.perAcceptance` 四条 record（M2K2-A1/A2/A3/A4）全部 `sensitive:true` + `failed as expected`，M2K2-A5 structural 豁免——A1（`PAGE_COUNT→PAGE_COUNT+1` 页循环多留一位 / `cycle_next` 步进写死 `% 3`）、A2（`total` 硬编码 4 / 复活 `PAGE_P4` / render 枚举漂移）、A3（`BOOT,CLICK→GLOBAL_ACT_NONE` 翻页动作丢失）、A4（`char_page_show` 调用点删 / `cache_char_data` 声明静默删）；各自还原后复绿，红→绿链完整。本 packet 引用 taskpackage 字段，不重跑。
- **Red baseline（acceptance-author 冻结相位实跑）**：新件 standalone `7T/6F`、ELF exit 6（六条红因全为本包缺失的真实行为，零环境/线束错、零 RED(infra)）；两修订件 standalone `5T/1F`（KD-E1 设计态红）+ `5T/0F`；真 host runner `39T/1F`（唯一红 = 本包义务红 KD-E1，基线锚 39T/0F 守恒）；scratch 目标态自证 `7T/0F`（无「永远绿不了的齿」）+ 八条单点变异逐齿翻红。
- **Docs sync**：`docsSyncRequired=false`；实现面零 docs/contracts/spec 触碰经 `git diff` 核对（详见同目录 `docs-delta.md`）。**独立核对发现一处 architecture 面 stale（Findings SHOULD 在案，交主线程 spec 批）**。
- **S2 exploratory**：TaskPackage 未声明 `s2ExploratoryRequired` → 无 S2 official scan（非遗漏）。

## Demo 对账行（workstream 包锚 3 · roadmap §③ K2 demo 证据形态核对）

roadmap 权威口径（`delivery/workstreams/m2-char-window/roadmap.md` §③ K2）：**「demo 证据形态：host runner（三页循环齿 + 页点 3 点齿 + 键路由回归靶）+ device build 闸；真机翻页手感归 K3 设备轮」**；同节「完成后人能看到什么」= ① BOOT 翻页 4 页变 3 页循环（首页→课表→故事）② 查字页不再作为常驻页被翻到 ③ 底部键帽条中槽页点 4 点变 3 点、翻页序与 spec P1→P2→P3 一致 ④ 查字能力本身还在（服务端下发仍能触发渲染），仅退出页循环等待改窗。逐项对账：

| roadmap 证据形态项 | 本包实际证据 | 对账 |
|---|---|---|
| host runner：三页循环齿 | test 1（M2K2-A1）`key_page_next` P1→P2→P3→P1 回绕 + 非法页回 P1 + 复合环（RED today 红名「key_page_next(P3)=3 期望回绕 PAGE_P1(0)」verbatim 在 redBaseline）→ 修复后 GREEN；test 2 结构半钉 `cycle_next` 区零 `KP_PAGE_CHAR` 旁路 + 渲染权威区 CHAR 停靠 case 拆除；MUT-M2K2-A1 两条变异红复现 | 一致 |
| host runner：页点 3 点齿 | test 4（M2K2-A2）`kidpal_keybar_dots` 三页 `total == PAGE_COUNT`（同源、不钉字面）+ 原第 4 页索引 `page==3` 越界返还 -1；test 3 钉 `PAGE_COUNT==3` 字面单点 + 两枚举逐项等值；MUT-M2K2-A2 三条变异红复现 | 一致 |
| host runner：键路由回归靶 | test 5（M2K2-A3）`key_route_global` 三值零偏移 + `key_route_layer` 覆盖层列/基层列等价 + 覆盖层 BOOT 隔离 + KEY 长按族零偏移 + 复合环；KD 套件除 KD-E1 一处留痕修订外零修改仍全绿（39T 基线守恒）；MUT-M2K2-A3 红复现 | 一致 |
| device build 闸 | final r2 `firmware-build` exit 0（真编译、`VERIFY_FW_BUILD_REQUIRED=1` 未降级，设备面 `render_node.cc` / `lcd_display.cc` / `keybar.c` 的改动均在编译内，E3 device build 面满足） | 一致 |
| 真机翻页手感 + 页点目击 deferred 归 K3 | M2K2-A5 structural deferred 条目 +「单真机不两轮撞车，挂 K3 设备轮」note 在册 | 一致 |

**一致性结论：一致。** demo 证据形态五项与 roadmap §③ K2 权威口径逐项吻合，无缺项、无超出声明的证据形态、无未声明的降级。效果段核对：①②③ 由 host 齿 + device build 闸落证；④「查字能力仍在」由 test 6（M2K2-A4）在册（`mcp_server.cc` case 3 路由 + `kidpal_char_page_show` 调用点 + `cache_char_data` 调用点 + 数值码 of `view_router` 零改动）；「真机翻页手感」一项按 roadmap 归 K3 设备轮。数字链自洽：39（既有基线）+ 7（新齿）= 46 = 全量实跑；红基线 6 Failures 与 A1/A2/A4 缺失行为红因构成一致。**无不一致项，无需就 demo 口径上报 planner。**

### UI design-review 证据（纯文本；本包触键帽条 chrome + 页面导航）

- 改动面：`comp-keybar` 中槽页点（4→3）与页导航（BOOT 翻页 3 页循环）为 UI 可视行为；`docs/requirements.md §2.2` 键帽条行 + 线框行 + `docs/ui` brand-spec「姿态规则」/ 设计稿 v4 §03 `comp-keybar` 为设计权威。
- design-drift summary（brief↔实现）：页槽表（`keybar.c`）与 §2.2 线框行逐字一致（keybar_three_slot 套件既有 KB-A3 齿复核，本包零改其语义）；中槽页点数与 §2.2「中 3 页点」现文一致（deviation 收回）；翻页序与 §2.4 P1→P2→P3 现文一致。**无设计漂移。**
- viewport coverage / a11y summary：本包不触 a11y 面；viewport = RLCD 单一物理屏（归一屏单屏不变式不动）；真机像素级目击 deferred 归 K3 设备轮（同 demo 对账行）。
- external artifact reference：无（bootstrap 期不在仓内存 screenshot/video/HTML 证据，重资产留仓外）。
- residual risk：真机 3 页翻页手感与 3 点目击未在本包，挂 K3（见 Residual Risk）。
- UI finding severity：本包无 UI BLOCKING / SHOULD（设计权威面零漂移）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （空） | changed diff 内无 `minimal:` 标记（`git diff 01f8b97..HEAD -- firmware/main \| grep minimal:` 实证零命中） | — | — |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | docs/spec | `docs/architecture.md` §5.1.1 渲染节点层（行 170）现文仍描述旧实现态「页循环枚举 `HOME→CHAR→SCHEDULE→STORY`（…撤 P4 后改 3 页【修复项】）」，本包落地后真实现已是 `HOME→SCHEDULE→STORY`（3 页）；同行 381 to-be ④「撤 P4 常驻页 → 3 页循环」的撤页半亦已达成 → 该文两处 stale，其【修复项】标记待收回 | changed-code hunk `kidpal_render_node.h` 枚举重排（+17/-12）+ `docs/architecture.md:170` 被行 404 显式指定为「现状」权威（行 404 自注「本条为历史口径，现状见 §5.1.1（渲染节点层）」）+ `git log -- docs/architecture.md` 实证该文全部改动均为主线程 spec/gov 批 commit（无 dev 包提交） | 交**主线程 spec 批** reconcile（须收回两处【修复项】标记）；本包 **E2 铁律明禁改 spec**（`docs/requirements.md` + `architecture.md`），**不就地改**。planner 裁定：登记（本 packet + docs-delta）或将本条升 ARQ |
| FYI | judgement-call | firmware/display | `kidpal_render_node.cc:44` 的 `#include "char_page.h"` 随 CHAR 停靠 case 删除成未用 include（`kidpal_char_page_show` 仅剩 mcp_server 一处调用方） | changed hunk `kidpal_render_node.cc` 删 CHAR case（+6/-21）+ 该头引用零残留（非 hard rule，属 cleanliness） | no-trigger——planner 已裁定保留（K3 改窗复用，与 E3 同向；见 `3109717` commit body 裁定①） |
| FYI | judgement-call | firmware/display | 撤页后两个公开 API `kidpal_render_node_cache_char` / `cache_char_data` 与静态 `s_has_char_data` / `s_char_json` 成**写侧死态**（仅写不读，无循环消费方）——本包按 **M2K2-E3** 保留不动 | changed hunk `kidpal_render_node.cc`（静默不删）+ mcp_server.cc:665 唯一写点在场 + E3 明文「保留不动、由 K3 改窗时统一收，在 landing summary 如实登记，禁静默删除」 | 登记完成（本 packet + docs-delta 承载）→ **K3 改窗时统一收**；本包不得越包删 |
| FYI | judgement-call | governance/tooling | 本 Pod 两条 friction 已落 pod-events：① acceptance-author over-scan 13 行（本批最多，`f204cd6a-…`）② `enforce-scope-canary.mjs` 实测 3/7，四条失败全在 architect 角色且属**期望侧过期**（与现行 scope-map architect allow 相悖，非钩子缺陷，`e1374a96-…`） | pod-events 事件 id 在场 + 该脚本不在 verify 闸集内 | no-trigger——非本包阻断项，候选并入下次治理批 |
| NIT | judgement-call | firmware/display | `keybar.c` 页槽表 `switch` 的 `default: return -1` 对合法页（0..PAGE_COUNT-1）不可达（越界已在函数入口防御列拦下并返 -1） | changed hunk `keybar.c:36-52`（入口 `page < 0 \|\| page >= PAGE_COUNT → -1` + 尾部 `default: return -1`）+ deletion test（删该分支仅失去 switch 完整性、复杂度不转移） | no-trigger——保留仅为 switch 完整性/无 default 警告，且 A2 结构齿要求 `PAGE_P4` 字面零残留 |

无 BLOCKING。结构闸 PASS/FAIL 不复述为 finding（verifier evidence 承载）；gate 全绿仍完成五轴语义 review（见下）。

## Five-Axis Review

1. **Correctness**：四条验收齿语义齐备——A1 三页回绕（P1→P2→P3→P1 + 非法页含 `PAGE_COUNT`/越界/`-1` 回 P1 + 复合环 + 步进越界不变量）；A2 两枚举逐项等值与字面 3 单点（`PAGE_P1=0/P2=1/P3=2/PAGE_COUNT=3` 与 `KP_PAGE_HOME=0/SCHEDULE=1/STORY=2/COUNT=3` 双向钉）+ 页点同源；A4 查字通路在场（路由 case 3 + 两调用点 + 数值码）。边界条件（`page==PAGE_COUNT` 越界、`-1`、`99`、`PAGE_COUNT+1`）在齿内覆盖。撤页动作三面同批落地（枚举 / 循环 / 页点），无半撤脱钩——`lcd_display` 映射塌恒等后，设备侧页索引与逻辑页序逐项同值（齿内以「两枚举逐项等值 = 塌恒等的形状无关前提」钉）。
2. **Test quality**：断言真实行为非实现镜像——A1/A2 行为半直接断言 host 可编实现（`key_page_next` / `kidpal_keybar_dots` / `kidpal_keybar_slots`）的返回值；结构半对设备专属文件（`render_node.cc/.h` / `lcd_display.cc` / `mcp_server.cc`）用稳定签名切区 + 「区内零字面」结构齿（KD-A2 先例形态），并以「三页 case 全在世」守卫防「删整个函数换绿」；字面 3 只单点钉一处（新件 test 3），两处修订件不另立第二字面源（防互相遮蔽）。四条功能齿全部 mutation 敏感（`sensitive:true × 4`），八条单点变异逐齿翻红 + 还原复绿。红基线由 acceptance-author 独立实跑落账，非 builder 自证；scratch 目标态自证 7T/0F 排除「永远绿不了的齿」。
3. **Contract/boundary safety**：`docs/contracts/**` 零触碰（`git diff` 零命中 + contract-lint 绿）；`view_router` 数值码零改动（A4 齿钉）；两处留痕修订走 **acceptance-author 通道（M2K2-E2）**、tighten-only、REVISION LOG append-only（历史实测段零改写），未放宽任何语义断言。**spec 面漂移**：requirements 面零漂移（本包向已冻结的需求对账）；architecture 面发现一处描述 stale（Findings SHOULD，非本包可改，交主线程 spec 批）——非契约/事件/tool/shared type 漂移。
4. **Operability/security**：无 secret / 权限 / correlationId 面（security 闸 `securityReviewRequired=false`，exit 0）。撤页为纯前台页归属变更，不触网络/存储/输入解析；`mcp_server` char 路由保持防御降级门（必需键六件不全→降级提示）不变；`lcd_display` 页索引映射保留越界回落 `PAGE_P1` 防御（不 UB）；`keybar` 两函数保留 `NULL`/越界防御列。无新增攻击面、无新增依赖（实现面零新增 `#include`，仅新验收件引入既有项目头 + 标准头）。
5. **Maintainability/simplicity**：改动局部、单源清晰——页循环页数以两枚举为单一事实源（`KP_PAGE_COUNT` / `PAGE_COUNT` 自动随枚举，齿以同源判别而非字面），消除了 M1-K2 遗留的 deviation 兜底 case 与「值钉定归 human」挂账；`lcd_display` 的换算表塌恒等是纯删除（复杂度消失，非转移），符合 deletion test；`keybar` 兜底 case 消解为 `return -1` 一行。**无过早抽象、无重复实现**：两枚举仍分属逻辑层（key_gesture_parser，host 可测）与渲染层（render_node，设备专属），未硬凑合一（避免第三方 host 链接污染）；`redBaselineCommand` 的 cwd 歧义已在 intake 修正。simplification tags 零命中（`minimal:` 零命中）。

## Residual Risk

- Known limitation:
  - **M2K2-A5 真机翻页确认 deferred 归 M2-K3 设备轮**——BOOT 翻页 3 页循环手感 + 页点 3 点真机目击在本包外；本包 device build 闸只担保编译，不担保实机行为。**K3 接手口径**：单真机不两轮撞车，K3 设备轮随 F3 真机确认轮（S-F3.1 AC 清单 + NFR-PERF-2 串口计时）一并目击 3 页循环手感与 3 点页点，沿用 RM-A8 取证形态。
  - **E3 死态 API 登记（交 K3 改窗统一收）**：`firmware/main/display/kidpal_render_node.{h,cc}` 的公开 API `kidpal_render_node_cache_char` / `kidpal_render_node_cache_char_data` 与静态 `s_has_char_data` / `s_char_json` 随撤页成写侧死态（定义处已加注）；`firmware/main/display/kidpal_render_node.cc:44` 的 `#include "char_page.h"` 成未用 include（planner 裁定保留，K3 复用）。**登记在册、禁静默删除**，K3 改窗时统一收或复用。
  - **architecture 面 stale（Findings SHOULD）**：`docs/architecture.md` 行 170 / 381 的【修复项】标记待主线程 spec 批收回；本包 E2 禁触 spec，故不在本包消解。
- Follow-up recommended：
  - 主线程 spec 批 reconcile `docs/architecture.md` §5.1.1 行 170 + to-be ④（撤 P4 半达成）+（可同批）requirements §2.3 行 166 ② 实现侧对账台账的消解标记。
  - M2-K3 设备轮执行 A5 目击 + 统一收 E3 死态 API / 未用 include。
  - 两条 friction（over-scan / enforce-scope-canary 期望侧过期）候选并入下次治理批。
- Human decision needed：**无**（无契约/spec 变更面由本包就地裁、无依赖新增、无安全残余）——architecture 面 reconcile 走主线程 spec 批（按 Track B / spec 批既有通道），不由本包升级人。回滚方式 = 本包两 commit 单点 revert（改动面收敛于 firmware 9 源/接线 + 3 测试件，无 schema/契约迁移；`retract_p4_three_page.acceptance.c` 新增件随 revert 移除，CMakeLists 登记行反向删除）。
- decisionCoverage：TaskPackage `decisionCoverage` 为空集（`[]`）——无 `landsIn: dropped && reason: risk-accepted` 条目，无 D<n> 风险接受挂账。

## 影响面核对（worktree 口径）

- **detect_changes: N/A (worktree)**——`KidPal` 图索引解析到主 checkout（`C:/AI/KidPal`），Pod worktree 内 diff 对索引不可见；以 `git diff --stat` 闭集 + 关键共享符号 `trace_path` 抽查替代（2026-09-18 integrator 实跑）。
- `git diff --stat 01f8b97..HEAD`：改动闭集 = firmware 12 文件（实现 9 + 验收 3）+ 簿记面 + 包外 gov 提交 `13cd811`；`server/`、`docs/contracts/`、`docs/requirements.md` 零命中。
- 共享符号 `trace_path` 抽查（均在 firmware/main 场内，无跨层新增调用方）：
  - `kidpal_render_node_cycle_next` 入站 callers = `InitializeButtons`（boards）+ `keya_dispatch_global_action`——翻页入口闭集在场内，未新增调用方。
  - `kidpal_render_node_get_page` 入站 callers = 7（`InitializeButtons` / `keya_overlay_consume` / `KidPalKeybarSync` / `KidPalVoiceChromeFront` / `SetupUI` / `kidpal_keybar_sync_locked` / `kidpal_voice_apply_look`）——本包只改其**消费侧**（`kidpal_keybar_sync_locked` 映射塌恒等），签名与返回语义不变，7 个调用方零受影响。
  - `key_page_next`：firmware 非测试面**无 live 调用方**（仅 `reminder_page.h` 注释引用）——纯逻辑页序原语，device 侧翻页恒走 `cycle_next`；本包改其回绕基数由 `PAGE_COUNT` 同源（结构不变）。
  - `kidpal_keybar_slots` / `kidpal_keybar_dots` 图内入站 callers = 0（调用点在 `lcd_display.cc` 经 qualified 形式，图未解析）；以 grep 佐证消费点唯一 = `lcd_display.cc` 的 `kidpal_keybar_sync_locked`（`kidpal_keybar_slots`/`kidpal_keybar_dots` 各一处），改动仅在其返回值的语义（页槽表行数 / 越界列）而非签名。
- 索引状态：工具图与工作树差异如实以本节手工抽查承载，未虚报；re-index 需求归主线程/human。

## Merge Packet / Supervisor 输入

- 待合并分支：`dev/260915-m1-k1a-key-layer-dispatch`（本 Pod 常驻分支，本包两 commit 冻结于其上，mergeOrder 29）。
- 合并路径：planner 主线程跑 `supervisor-verdict-gate.mjs` 机械闸 → APPROVE 后前置升级闸（`pre-merge-escalation-gate.mjs`）→ 放行 `merge-driver --pod` 本地合并；**本 integrator 不执行 merge、不 push、不在 main 上 commit**。
- 阻塞项：**无 BLOCKING finding**；`closeout-check` 两 blocking 制品（`docs-delta.md` / `review-packet.md`）已晋升本目录。
- 唯一需 planner 裁定的非阻塞项：Findings SHOULD（architecture 面 stale）——登记或升 ARQ、交主线程 spec 批，**不阻断**本包合并门。
- 回滚方式：单点 revert 两 commit（见 Residual Risk）。
- 遗留脏行说明：`delivery/taskpackages/current/verdict.json`（verify.mjs 脚本 fs-write 的 non-bearing 投影，指向 r2）、`pod-events.jsonl`（append 一条 builder dispatch-completed）为簿记面 M 态，随本收口 commit 一并冻结；`.codebase-memory/` 脏为工具再生面常态，不 stage、不还原。