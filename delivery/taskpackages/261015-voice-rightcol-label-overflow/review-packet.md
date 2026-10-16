# Review Packet —— 261015-voice-rightcol-label-overflow

> integrator 制品轮产出（2026-10-15）。状态：**scoped round 2 八闸全绿（bearing fact = `verifier-evidence/rounds/r2.json`，invocationId `44aad3de-b932-4cde-8c47-41572c5e18a5`）** + 红基线 172/4 设计态红全数转绿（172/0）+ mutation 三族取证齐（perAcceptance 四条 functional turnedRed=true）+ 范围圈定清单落盘（`verifier-scope.json`）+ closeout 制品齐（docs-delta.md / review-packet.md）+ feature 分支 commit 冻结 diff；**final 相位终验、supervisor 裁断与 merge-driver 归 planner 主线程，本 integrator 不判定通过、不 merge、不 push**。rigor=lite、sourceType=regression、riskTier=medium。

## 来源链

human 真机照片实证语音轮 TALK 态问答框行尾 1~2 字被裁 → 主线程 `/triage-change` 分流：architect review 产出 sibling `plan.md`（2026-10-15，1g 前置技术预评本体，frontmatter `reviewed-by` 为证；根因链含 git 证据 = ui-v5-align 阶段3 commit 9f4c82a 右列容器收窄 x=192/宽 196，label 宽 264 为 260915-m1-k3 时代 9cb0c07 遗留——容器改了、label 没跟着改）→ human 触点①已裁（②单包 Lite + 标头 :1187 一并收进）→ 本包单包自建 intake 形状（无 portfolio taskpackage、无 devloop/pods 条目、无 PROPOSAL；launch-card.md 缺失如实记录、不设门）。

## 根因与修法

`LV_LABEL_LONG_WRAP` 下 label 按 264px 换行、右列容器视口仅 196（x=192、pad_all=0）→ LVGL 默认裁剪溢出，每行右端约 68px（≈4~5 个 14px CJK 字形）不可见。修法 = 三处 `lv_obj_set_width` 字面宽度 264→196（`lcd_display.cc` :1187 标头 `g_kidpal_voice_header_label`〔human 触点①改判收进，x=124 位置零触碰〕/ :1209 回显 `g_kidpal_voice_echo_label` / :1229 问答 `g_kidpal_voice_answer_label`），与右列容器内容宽一致。同文件先例：THINK 引导行 label（:1886）同父同版式已写 196。接线 = `host_app/main/CMakeLists.txt` +2 行（新验收件入 HOST_ACCEPTANCE_SRCS、置于 host_infra_smoke 之前，C4.2/C4.3 纪律）。

## Change Size

- **实现面**：`firmware/main/display/lcd_display.cc`（+3/−3，三处字面常量）、`firmware/main/test/host_app/main/CMakeLists.txt`（+2 接线）。
- **验收面**：`firmware/main/test/voice_rightcol_label.acceptance.c` 新建冻结件 568 行（VR-A1~A4 四齿，MCD-A1 源锚形态：正向齿认注释剥离后可执行源字面整数、反证齿行级双针走裸源）。
- **环境态**：`firmware/sdkconfig`（+5，Image Effects Configuration 空节头自动再生，TECH-261014 处置口径已申报，非功能改动）。
- **执行 ledger**：taskpackage 目录（taskpackage.json / plan.md / 两份 input-brief / mutation-evidence 三件 / pod-events.jsonl / verify-runs.jsonl / verifier-evidence/rounds/{r1,r2}.json / verifier-scope.json）+ `current/` 投影 + `delivery/changes/dev/261015-voice-rightcol-label-overflow/`（本 closeout 目录）。
- **Scope**：板端 LVGL 显示版式修复——零 wire 形状变更、零契约面、零 docs 面、零新增依赖、零新增模块。
- **Split recommendation**：不拆——单主题（右列 label 宽收窄）单文件三行原子变。

## Verification Evidence

- **红基线（2026-10-15 本树实跑）**：全量构建 EXIT=0（新验收件入套件编译通过）+ host runner **172 Tests / 4 Failures**——4 红全部为新齿设计态红且红名与冻结申报逐字一致（VRL-A1/A2/A3 RED: W=264 超上界 196；VRL-A4 RED: 仍有 3 行 lv_obj_set_width 携带 264），零 RED(infra)；既有 168 齿全绿 = 相对上游基线（261014 合并后 168/0）零新增红。另有 acceptance-author 独立冒烟通道取证（gcc 实编 exit=0 + 实跑 4 Tests 4 Failures 同形状）。
- **修复后**：host 全量 **172 Tests / 0 Failures 全绿**（既有 168 零倒退 + 新四齿转绿）。
- **mutation 取证（三族回退变异，builder 第一方实跑，证据 = `mutation-evidence/m{1,2,3}-*-revert-264.txt`）**：m1（echo 回退 264）→ VRL-A1 + VRL-A4 同源双红、其余齿绿；m2（answer 回退 264）→ VRL-A2 + VRL-A4 双红；m3（标头回退 264）→ VRL-A3 + VRL-A4 双红；三态还原 196 均复绿 4/4 PASS。VR-A5（既有齿）/ VR-A6（真机轮）为取证豁免面（taskpackage mutationEvidence.reason 申报口径）。
- **verifier 轮次史**：r1 scoped 红（round=1，6 闸，mutation-check 元数据缺口——perAcceptance 缺 VR-A4/A5 条目，planner 补条目后转机，非测试红）→ **r2 scoped 全绿（round=2，八闸 PASS**：taskpackage-required / change-manifest〔pre-cutoff 存量放行〕/ firmware-build〔设备侧真编译，xiaozhi.bin 0x312710 bytes、app 分区余 11%〕/ contract-lint / acceptance-coverage〔functional = VR-A1~A4、structural = VR-A5/A6〕/ mutation-check / docs-sync〔docsSyncRequired=false〕/ security）。verify fact 程序 fs-write，integrator 只消费不重跑。
- **范围圈定终验（Step 4，三分法：per-Pod 专属图索引可用 → 正常圈定，无降级）**：圈定清单落盘路径（一行锚）= **`delivery/taskpackages/261015-voice-rightcol-label-overflow/verifier-scope.json`**（final verify 轮以 `--scope-file` 消费）。圈定 = 本包 acceptanceTestsFile（`voice_rightcol_label.acceptance.c`）∪ taskpackage impactedTestsSeed 三件（voice_round_ui / 新验收件 / CMakeLists 接线）∪ blast radius 映射 ∪ 共享 seam 相关 ∪ 安全余量（char_window_overlay〔MCD-A1 同文件先例回归面〕+ host_infra_smoke〔包 0 守卫〕）。**floor = 本包冻结验收件 voice_rightcol_label.acceptance.c + 既有冻结件 voice_round_ui.acceptance.c 零改动回归（VR-A5）**。final 相位终验归 planner（本 integrator 不重跑）。
- **影响面核对记账（Step 9b advisory）**：`detect_changes` 实跑（per-Pod 专属索引 `C-AI-KidPal-pod-m2`，base=main，merge_base=3fedba46，含工作树未 commit diff）→ changed_files=22（本包实现/验收/环境态四件 + delivery/ 簿记面 + 同分支先行非本包制品 ad30de5 debt 登记）；seed_symbols=0（字面常量改动无符号签名变化，blast radius 以文件面 + 源锚齿读文件关系判定）。`trace_path` 抽查：`LcdDisplay::SetupUI` 入向 callers=3（Mipi/Rgb/Spi 三个显示子类构造路径），无跨子系统、无 wire 面调用边——与 crossEnd.exempt 一致。改动只影响预期符号面，结果记入本 packet + `verifier-scope.json`。
- **diff 面核对**：`git status --porcelain` 实证改动面 == changeManifest（lcd_display.cc / voice_rightcol_label.acceptance.c / CMakeLists.txt）+ sdkconfig 环境态 + delivery/ 簿记；`docs/` ` .claude/` `tools/` 零命中。264 旧值销账：`grep 264 lcd_display.cc` 仅余 2 处注释提及（:1011/:1031 y262-264 设计注记，无关），可执行调用零残留（VR-A4 反证齿绿承载）。
- **架构/依赖卫生检查摘要**：零新增依赖、零循环依赖、零绕过 contracts 的本地类型、零跨层越权接线、零重复实现——三处字面常量收窄 + 一行测试接线，无结构性面。

## 未覆盖面申报（对照本包 acceptance[]（VR-A1..A6）逐条「性质 + 现状 + 去向」）

- **VR-A1 / VR-A2 / VR-A3（三 label 宽度收窄）**：**已覆盖**。性质 = host 源锚结构齿（W 在场且 ≤196 区间断言）+ 三族回退变异判别。现状 = 新四齿全绿 + m1/m2/m3 turnedRed + 172/0。去向 = n/a。
- **VR-A4（反证齿，264 销账）**：**已覆盖**。性质 = 反证结构齿（行级双针裸源扫描）。现状 = 三族变异各自同源双红 + 源文件可执行调用零 264 残留。去向 = n/a。
- **VR-A5（既有冻结件零改动回归）**：**已覆盖**。性质 = 零改动 canary（structural，mutation 语义不适用 M-4 同款）。现状 = voice_round_ui.acceptance.c 零 diff + 既有 168 齿全量 host 实跑零倒退。去向 = n/a。
- **VR-A6（真机轮 live-manual structural 豁免，五项持板目测）**：**本次未覆盖——设计性延后（plan 冻结口径）**。性质 = live-manual structural 豁免，不计入 runner 通过面。现状 = host 面转绿后停门等 human，不自行烧录。去向 = human 持板验收（三态长文右缘目测 / THINK 让位换行一致性 / 标头不回归 / 滚动条瞬态观感可接受 / app 分区单烧），判定标准权威 = taskpackage VR-A6 条目原文。
- **本包引用契约条目**：零变更、无验收断言增补义务——`contractRefs` 仅 canary 引用，契约面零 diff（contract-lint r2 PASS）。去向 = n/a。

**零缺口申报：none**——对照本包 acceptance[] 全 6 条 + 引用契约条目，无「漏跑」型缺口；唯一未覆盖项（VR-A6 真机轮）为 plan 冻结的设计性延后，逐条标性质 + 现状 + 去向，无静默跳过。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| firmware/main/display/lcd_display.cc / firmware/main/test/voice_rightcol_label.acceptance.c / firmware/main/test/host_app/main/CMakeLists.txt | 本轮改动面 `git diff` grep `minimal:` **零命中**——零新增简化债务 | n/a | n/a |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| FYI | judgement-call | 让位自由度（builder+真机轮终判） | 实现取区间上界 196（与容器内容宽严格一致）；plan 明载真机观感不佳可让位改 188~190（MCD-A1 W 终判先例，验收齿区间断言 [188,196] 两端兼容）——本值非终判，真机轮持板观感是终判 oracle | changed-code hunk（三处 lv_obj_set_width 实参 196）+ plan.md「滚动条让位」节（默认主题滚动条 ≈5px 瞬态遮盖、观感风险低）+ heuristic | 无（VR-A6 第 4 项承载终判；若 human 持板判观感不佳，让位改值在验收齿区间内、无需改冻结件） |
| FYI | judgement-call | 源锚齿为结构代理（非行为 oracle） | lcd_display.cc 设备装配面非 host 编——host 齿为源文本树扫代理（正向齿注释剥离防假绿、反证齿裸源行级双针防注释假红），「行尾不再被裁」的真行为断言只能在真机目测承载 | changed-code hunk（voice_rightcol_label.acceptance.c 文件头 Nature 节自申报）+ heuristic（host 不可编文件的固有代理强度上限） | 无（行为 oracle 归 VR-A6 真机轮第 1 项，已申报） |
| NIT | judgement-call | sdkconfig 环境态副作用入 diff | firmware/sdkconfig +5 行为 Image Effects Configuration 空节头自动再生（环境态归一），非本包功能改动但进入本包 commit 面——已知地雷 TECH-261014（TECH-261014-host-app-sdkconfig-env-drift）处置口径在案 | changed-code hunk（sdkconfig diff）+ taskpackage notes 已知地雷①申报 + heuristic（环境等价化镜像副作用） | 无（如实随包 commit 留痕；后续包同款漂移按 TECH-261014 口径处置） |

结构闸 PASS/FAIL 不复述为 finding（gate 全绿仍完整执行五轴语义 review，见下）。本轮 findings 面清洁：**无任何升级级（hard-violation 等级）finding（明写：BLOCKING = 0）**；亦无触及治理 spec 面（契约/需求/架构）的 Basis 引用——本轮 finding 全为 judgement-call，Basis 为 changed-code hunk + plan 冻结文本 + heuristic。

## Five-Axis Review

- **Correctness（是否满足声明的 S-Fx.y，边界缺失）**：兑现 S-F1.3 AC1/AC3 上屏显示完整性既有口径——label 宽对齐容器内容宽 196，行尾裁剪根因消除；换行点前移/滚动更早 = VQ-A2/A3 滚动语义预期行为非副作用。边界：标头 x=124 位置零触碰（WF-B 独立版式决策）、THINK 让位路径（:2040-2048 只 set_pos/set_size echo_box）零联动、既有 168 齿零倒退——三族变异 + 全量实跑实证。VR-A6 真机行为 oracle 未落（已申报，设计性延后）。
- **Test quality（断言真实行为，含错误路径）**：源锚齿带防假绿设计（正向齿注释剥离 + 反证齿裸源双针 + 扫描器自检两枚）；mutation 三族 turnedRed 实证敏化——非「没报错就算过」。精度提示：结构代理非行为 oracle（Findings FYI #2，已申报）。
- **Contract/boundary safety（是否漂移 contracts / events / spec）**：`docs/contracts/**` / `docs/requirements.md` / `docs/architecture.md` 零 diff（contract-lint + docs-sync r2 PASS）；wire 协议零变更（crossEnd exempt）；无影子实现。
- **Operability/security（ErrorEnvelope / correlationId / 权限 / 输入 / secret / 日志）**：零新增攻击面——纯显示版式常量改动，零新增出网/输入处理/secret；日志面零变化。`securityReviewRequired=false`（security 闸 PASS）。
- **Maintainability/simplicity（局部、易懂、无过早抽象/重复）**：单文件三处字面常量 + 一行接线，零抽象、零重复、零新增依赖；`minimal:` 零标记。

## Residual Risk

- **Known limitation**：VR-A6 真机轮五项持板验收未闭环——host 面全绿不构成真机目击（源锚结构齿是代理，非像素/真机 oracle）；host 面 closed、真机探真 open（host 转绿后停门等 human，不自行烧录）。
- **Follow-up recommended**：human 真机轮持板验收（VR-A6 五项；含让位自由度终判——观感不佳可改 188~190，验收齿区间兼容免改冻结件）。
- **Human decision needed**：真机轮组织（human 持板 + 烧录 app 分区单烧）。taskpackage **未启用 decisionCoverage 字段**——`landsIn: dropped && reason: risk-accepted` 条目 = **零**，本 Residual Risk 节按模板申报零条目。

## 执行边界留痕

1. **验收件归 acceptance-author**——integrator 零改 `*.acceptance.*`（voice_rightcol_label.acceptance.c 冻结件只读消费）。
2. **docs 面 integrator 无写权**——spec 面漂移检出 = 无，零自改；docsSyncRequired=false 裁定落本包 docs-delta.md（docsOwner = integrator）。
3. **不 push；不在 main/master commit；不执行 merge**——已遵守（本分支 dev/pod-m2 commit 冻结 diff；final verify + supervisor 机械闸 + merge-driver 归 planner）。
4. **当前分支 = `dev/pod-m2`（本 Pod 常驻分支）**——已核（`git rev-parse --abbrev-ref HEAD`）。
5. **真机轮不自行烧录**——已遵守（VR-A6 停门等 human）。

## 集成执行回报

本轮无 merge-driver 冲突消解（单包制品轮，Pod 分支无对端冲突，全部改动为工作树未 commit 态一次收口），集成复跑清单 n/a；圈定 floor 注（双方冻结件 ∪ 圈定清单）不适用——**单包无对端冻结件，floor = 本包冻结验收件 voice_rightcol_label.acceptance.c + 既有冻结件 voice_round_ui.acceptance.c（VR-A5 零改动回归）**。复跑面 = 影响面圈定清单（`verifier-scope.json` 五件 impactedTests；firmware host 单 ELF 套件级全量 172 齿实跑承载）。detect_changes 记账见 Verification Evidence（changed_files=22 为整分支 vs main 口径，本包实现 seed 全落 firmware/main/display + test 面，无降级、per-Pod 索引可用）。**全量回归 = 人工触发位，不在本次**（final 相位按 scope 圈定，归 planner）。
