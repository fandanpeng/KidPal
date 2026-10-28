# Review Packet · 261028-m6-k1-font-assets-runtime-loading

- integrator 收口（2026-10-28）；base ref = `9f8b2170`（包首提交父）；分支 `dev/pod-m2`（feature 分支，未 push）。
- test-scope 清单落盘路径：`delivery/taskpackages/261028-m6-k1-font-assets-runtime-loading/test-scope.md`（范围 = 影响面圈定清单：本包冻结件 + p1_cjk_name 相邻锚 + host_infra_smoke 守卫 + host CMakeLists 验收接线面；detect_changes 正常圈定、非降级；安全余量 = host 全量 250 tests + 设备 build 真编译，已按该范围执行）。

## Change Size

- Files changed（代码面，`git diff 9f8b2170` + 新增未跟踪）：11 改 + 5 新 = 16 个实现面文件；另 6 个治理制品（taskpackage.json / current/ 投影 / pod-events.jsonl / verify-runs.jsonl / acceptance-wiring-patch.md / 本包收尾制品）。
- Net diff：约 +612/−167 行（代码面）；另两枚二进制资产 font_body_16.bin 376,568B、font_title_20.bin 306,860B（`.gitignore` 屏蔽 *.bin，已 force-add 保留跟踪）。
- Scope：固件字体资产产线（gen_fonts.sh）+ 设备 CMakeLists assets 打包接线（fonts_root extra_files）+ 开机挂载（assets.cc）+ 统一句柄 seam（kidpal_font.{h,cc} 新建，[K1]-[K5]）+ render_node 句柄透传（零排版）+ host 验收接线与 FAS 结构清单块 + 冻结验收件（FAS-A1..A8）+ ADR-017 实测注记回填。
- Split recommendation：不拆——P0 spike 单包交付，运行时底座不可再分。

## Verification Evidence

- Structural gates：verify.mjs 第 4 轮（2026-10-28T14:45:50Z）8 闸全绿（taskpackage-required / change-manifest〔pre-cutoff 存量放行〕/ firmware-build / contract-lint / acceptance-coverage / mutation-check / docs-sync / security），error:null——`delivery/taskpackages/current/verdict.json` + `verify-runs.jsonl` 第 4 行。contract-lint 仅存量 warning（历史批断链，非本包）。
- Acceptance/unit：红基线（builder step-0 现跑）= 250 Tests / 9 Failures（FAS-A1..A6 设计态红 + 存量 p1_auxbar AB-A1/A2/A4 三条；FAS-A7/A8 恒绿、零段错误）；绿跑 = 250 Tests / 3 Failures（仅存量 p1_auxbar 族，零新增红）。
- Security gate：securityReviewRequired=false，闸 PASS。
- Mutation check：taskpackage.json mutationEvidence.perAcceptance 8 条全 turned red（每条变异 build→run→还原，FINAL 干净跑 250/3 FAS 全绿）；外溢红（A3→A5、A8→p1_cjk_name[S1]）为目标验收确红前提下的可接受连带。
- Docs sync：docs-sync 闸 PASS；本包 docs 面 = ADR-017 append-only 实测注记（见 docs-delta.md），不改 F-5/F-6/F-7 决策语义，不触 docs/contracts/**，架构 §5.5.3 零改（docsNoChangeReason 与闸输出一致）。
- Device build：真编译绿——xiaozhi.bin 0x324950 bytes，app 分区 0x370000，余 0x4b6b0（9%）；generated_assets.bin 3,732,975 bytes，assets 8MiB 余 4,655,633 bytes（≈4.44 MiB）。
- 范围圈定终验：detect_changes（项目 C-AI-KidPal-pod-m2，base 9f8b2170，inbound depth 2）= changed_files 17 / impacted_total 14 符号，全部收敛 firmware/main，无 server 面、无清单外符号外溢。

### Scope 圈定结论（清单内外对照）

- 代码改动 ⊆ changeManifest，**零清单外实现面文件**。逐文件：
  - 清单内已改/新增：gen_fonts.sh、firmware/main/CMakeLists.txt、assets.cc、kidpal_font.{h,cc}（新）、kidpal_render_node.{h,cc}、host_app/main/CMakeLists.txt、font_assets_runtime.acceptance.c（新）、fonts_root/font_body_16.bin + font_title_20.bin（新）。
  - 清单内登记但未触（manifest 为上限，未触不违）：firmware/scripts/build_default_assets.py（DEFAULT_ASSETS_EXTRA_FILES 通路复用既有打包链，build 实证 generated_assets.bin 已含两枚 cbin）、firmware/main/assets.h、display/lvgl_display/lvgl_font.{cc,h}（设备装载复用既有 LvglCBinFont，零改）、display/font_size_test_page.cc（FAS-A7 只验存在性+编入，零改）、host_infra_seam.c（零改）。
- 清单外的全部改动均为治理制品（delivery/taskpackages/ 本包目录与 current/ 投影），不属实现面清单语义，如实列记：taskpackage.json（acceptance/mutationEvidence 回填）、current/{taskpackage.json,verdict.json}（指针投影）、pod-events.jsonl、verify-runs.jsonl、acceptance-wiring-patch.md。
- 跨子系统影响扫描：render_node 仅新增两个 extern 消费口函数（body/title 句柄透传），不改任何既有函数与页面字体选择行为——home/schedule/story 渲染行为零变（host 250/3 全量回归 + FAS-A8 KSG/KSTORY 常量零触佐证）；server 面零 diff（changeManifest 零 server 文件）。

## 决策裁定史（本包三段）

1. **验收运行时禁扫源码树**（human 定向裁定 2026-10-28，首例非仓库通用规则）：结构面断言改为 FAS-MANIFEST 机制——host CMakeLists configure 期采集结构事实写构建产物 fas_structure_manifest.txt，冻结件运行时只读构建产物，被扫源文件全登记 CMAKE_CONFIGURE_DEPENDS 保新鲜度。
2. **builder step-0 接线**：acceptance-author 被 enforce-scope 拦于 host_app/main/CMakeLists.txt，接线补丁逐字落 `acceptance-wiring-patch.md`，builder step-0 落线后立刻现跑红基线并逐字回报（250/9 == 设计态红）。
3. **双裁修冻结件基线**（step-0 阻塞裁定）：矛盾 A = cbin 格式只存在于 78 的 lv_font_conv fork，标准 1.5.2 不可达 FAR-E2 → provenance 换 78 fork（--format cbin，钉 commit db011302）+ 运行期守卫；矛盾 B = cbin_font_create 是 32-bit 内存镜像直调、64-bit host 必崩 → host 半预基线修正：去直调、改 kidpal_font 编译期宽度感知解码器（同一枚 .bin、F-7 同源意图不变），设备面仍 cbin_font_create，解码等价性归真机轮（FAR-E3）。F-5 核心语义（mount/降级/缺字回退）保住 host 行为级覆盖；裁定留痕 = taskpackage notes + retro，不重开 /arch-change。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （本包 changed 文件零 `minimal:` 标记；下条为收口新记）gen_fonts.sh provenance 段 | 78 fork 钉版以注释 + cbin writer 运行期守卫承载，未登记进 tools/asset-pipeline/package.json | 新环境复现产线须读注释装包；守卫（exit 3）已收敛「静默产错格式」风险 | 产线在新环境跑不出 cbin / fork 版本漂移时 → no-trigger（当前守卫足够） |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | tools/asset-pipeline/fonts/gen_fonts.sh | 78/lv_font_conv fork 依赖未入 package.json（`npm install --no-save`），可复现性靠脚本 provenance 注释 + cbin writer 存在性守卫（exit 3）承载 | changed hunk（provenance 注释段 + if 守卫）；heuristic：依赖钉版可复现性。风险已被运行期守卫实质收敛（缺 writer 即硬失败，不静默产旧格式） | 可选：登记进 package.json dependencies（钉 github:78/lv_font_conv）；不阻断本包 |
| NIT | judgement-call | firmware/main/test/font_assets_runtime.acceptance.c:628/887 | FAS-A2 注释把 0x732D 标为「猾」（实为 U+733E）；gen_fonts.sh 已以超集字表显式并入 0x733E 使断言成立（超集非子集化，覆盖语义未被弱化） | changed hunk（gen_fonts.sh mk_symbols canary 注释段自证）；冻结件不可改，本包不处置 | acceptance-author 复核归 retro（已知项，调度方指定留根） |

无 BLOCKING finding；结构闸 PASS/FAIL 不复述为 finding（gate 全绿仍已完整执行五轴语义 review，见下节）。

## Five-Axis Review

1. **Correctness**：[K1]-[K5] seam 语义齐——[K2] 双装载面按编译期指针宽度分派（设备 cbin_font_create / host 宽度感知解码器），NULL/短缓冲（<32B）回退 14px；[K3] reader==NULL 等价全缺、mount 返回 0（降级=合法完成态）；[K4] body()/title() 恒非 NULL；FAS-A2 line_height 档（16px→23 / 20px→28）与冻结标定逐值吻合。设备挂载点 = Assets::Apply 成功出口（assets 分区 + LVGL 均就绪），早退路径不调用 → 静态初始 14px 兜底。边界条件由 FAS-A4（兜底）/FAS-A5（缺字回退）行为钉。
2. **Test quality**：8 条验收全具设计态红基线 + 逐条变异证据（变异→build→run→还原，外溢红如实记）；负向探针（未知键）有钉（TEST 7 tooth 3）；断言消费构建产物与真实 .bin，不扫源码树（human 裁定落地）；「infra 红不冒充结构红」自诊断在案。
3. **Contract/boundary safety**：docs/contracts/** 零触（contract-lint 绿）；架构 §5.5.3 / requirements 零改；ADR-017 只追加实测注记；冻结锚（p1_cjk_name [S1] 签名、KSG_COURSE_CAP 84 / KSG_FONT_MIN_PX 14 / KSTORY 两常量）零触（FAS-A8 绿）。
4. **Operability/security**：securityReviewRequired=false（纯板端资产与装载通路，无网络/权限/secret 面）；缺资产降级不黑屏有行为钉（FAS-A4），深腐蚀数据崩溃域不做 host 钉点、强证据如实归真机缺 assets 降级开机轮（FAR-E3）——不写假行为断言。
5. **Maintainability/simplicity**：改动局部（16 文件，全部在清单内）；kidpal_font 为深模组（装载/降级/回退复杂度收进 seam，页面侧只剩两个取句柄函数，deletion test：删掉则 32/64-bit 解码复杂度泄漏回每个消费方）；零过早抽象——P1/P2/P3 消费切换未提前实现（crossPackageBehaviors owner/carriedBy 边界在案）。

## Residual Risk

- Known limitation：host 半走编译期宽度感知解码器，与设备 cbin_font_create 的解码等价性未在 host 侧证明（kern 不解码、letter_next=0 不触发）——按裁定归真机轮 FAR-E3 收口，本包 host 侧不冒充设备行为。
- Known limitation：真机探针轮（FAR-E3：看屏照片 + 串口日志证明 16px/20px 来自运行时资产 + 缺 assets 降级开机）尚未执行——spike-first 硬边界：该轮收口前 P1/P2/P3 不得 launch。
- Follow-up recommended：78 fork 登记进 package.json（见 Findings SHOULD 条）；acceptance-author 复核 FAS-A2 canary 注释笔误（NIT 条，归 retro）。
- decisionCoverage `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目：无（本包 decisionCoverage 九条全部 landsIn exitCriteria 或下游包，零 dropped）。
- Human decision needed：无——合并门交 supervisor 机械闸复核（主线程跑 supervisor-verdict-gate）。

## 可回滚路径

- 包级回滚 = `git reset --hard 9f8b2170`（base ref，含本包全部已提交改动；工作树未提交态在 commit 后不存在残留）。两枚 .bin 随 reset 一并消失（重新可由 gen_fonts.sh 产线再生成）。无 schema/数据迁移、无部署态耦合，回滚零副作用。
