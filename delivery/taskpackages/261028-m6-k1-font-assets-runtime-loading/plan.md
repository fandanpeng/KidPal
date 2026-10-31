---
changeId: 261028-m6-k1-font-assets-runtime-loading
title: 字体资产与运行时装载 spike——16px/20px 全量 CJK、assets/cbin、host 同源与 14px 降级
rigor: full
riskTier: high
sourceType: proposal
origin:
  ref: delivery/workstreams/261028-font-typography-scaleup/PROPOSAL.md
  workstream: 261028-font-typography-scaleup
reviewed-by: architect（/triage-change 分流 review 2026-10-28；门 1 待 human 派 architect）
date: 2026-10-28
gateProfile: ["evidence-integrity", "test-quality", "docs-sync", "device-round"]
dependsOn: []
mergeOrder: 60
---

# 背景摘录（新建型锚「现状与差距」，PROPOSAL §2）

- 现役全设备正文使用编译内 `font_puhui_14_1`；`fonts_root` 虽已有旧三枚资产，但全树没有本板生产消费接线。16px 正文与 20px 阅读标题两枚全量 CJK 资产尚不存在，assets→cbin→1bpp 真机字形通路也未打通。
- F-5 已否决编入固件：ota_1 余量小于 16px 全量估算，且还须保留 14px 兜底与 20px 标题。F-6 要求 20px 全量而非静态子集，F-7 要求 host 与设备读同一枚 `.bin`。
- 本包是 spike-first 底座，不替任何页面完成排版。业务目的 6 的架构换代已 landed；本包只补 ADR-017 预留的精确尺寸与实测证据。

# 可行性判据

- `lv_font_conv` 产线、assets 8MB mmap、`Assets::GetAssetData`、`LvglCBinFont/cbin_font_create` 与 host runner 均在位；资产余量足够，编译内 14px 可继续承担降级。
- 需求 v1.69、架构 §5.5.3/§12 与 ADR-017 已冻结，F-5/F-6/F-7 无待裁方向。host 可验证装载、同源与降级逻辑；真机探针可验证 1bpp 消费端结果，不能用生产者自证替代。
- PROPOSAL §6 三项旧工作树改动当前已不在工作树；launch prep 仍须复核并保证不混入本包。九档实验页和 `font_test_*` 按 OQ3 保留。

# 跨包事实与共享冻结面约束（含跨切片边角）

- **P0 阻塞 P1/P2/P3**：三个页面包只能消费本包统一 `kidpal_font` 句柄，不得各自实现资产加载、20px→16px 或缺资产→14px 的第二套分支。
- **P0→P1 串行边**：`kidpal_render_node.cc` 与 host 字体 seam 是共享文件面；P1 必须等本包合并后再切首页消费。P2/P3 在本包合并后可进入各自页面切片。
- **14px 保留域**：键帽条、辅助条、查字窗、提醒弹窗与语音轮四态不跟随放大；旧三枚 `fonts_root` 资产不得被追述为已完成生产接线。
- **相邻冻结锚扫查（拆包期非零命中）**：`p1_cjk_name.acceptance.c` 的 [S1] 钉 `font_puhui_14_1`、host seam 与 14px 文本。P0 只建立新通路并维持现有页面行为，不预批 P1 的 supersede；P1 后续由 acceptance-author 留痕重基线。schedule/story 的 pitch、行数和旧标记常量本包零触。
- **真机证据分工**：本包负责探针页与缺 assets 降级首轮；P2 负责 pitch/容量轮；P3 在四页合并后做终端并集轮。原可选 P4 已并入 P3，不另立包。

# 铁律约束

- 不改 `docs/contracts/**`，不改 requirements/architecture 的冻结语义；疑似契约或 spec 冲突立即停并走 architect-reviewer/`/arch-change`。
- 20px 必须全量 CJK；不删 14px 兜底，不退役九档实验页，不重排分区表，不做 OTA 字体下发或静态子集化。
- 冻结验收修订只能由 acceptance-author 做收紧型留痕 supersede；builder/fixer 不得撤断言、以注释充证据或静默改冻结件。
- ADR-017 只追加实测尺寸与证据注记，保持 append-only；精确值必须来自实产，不抄估算。
- 只做 changeManifest 内文件；需扩面时回 planner 评估，不自行吞并 P1/P2/P3 页面改动。

# demo 证据形态（与 roadmap 一致）

- host runner：两枚同源的 `.bin` 均可经 cbin 装载，16px/20px 句柄与 14px 降级分支有 RED→GREEN 和变异证据；既有套件零新增红。
- device build + assets 打包/烧写证据，ADR-017 精确尺寸与余量注记。
- 一轮真机探针：看屏照片 + 串口日志证明消费端使用新资产；移除/隔离 assets 后仍以 14px 正常开机、不黑屏。
