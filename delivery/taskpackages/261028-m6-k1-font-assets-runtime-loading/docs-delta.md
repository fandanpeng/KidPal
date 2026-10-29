# docs-delta · 261028-m6-k1-font-assets-runtime-loading

- integrator docs-sync（2026-10-28）；docsOwner=integrator；docsSyncRequired=true。

## 实际 docs 变化

1. `docs/decisions/ADR.md` — ADR-017 尾部追加一条实测注记（append-only，只追加不改既有行）：
   - 标题行：`- **2026-10-28 P0 实产复测注记（FAR-E1 append-only 回填，只追加实测、不改上列决策语义）**`
   - 内容摘要：font_body_16.bin = 376,568 bytes（1bpp，line_height 23）/ font_title_20.bin = 306,860 bytes（1bpp，line_height 28），line_height 逐值吻合冻结 FAS-A2 标定（16px→23 / 20px→28）；generated_assets.bin = 3,732,975 bytes，assets 8MiB 分区余量 4,655,633 bytes（≈4.44 MiB），较 ADR 立项时余量净减 942,112 bytes；生产 provenance = 78/lv_font_conv fork @ db011302d4f448027cbf1af6d0d47a27d59d22c1（--format cbin）+ 运行期守卫；证据锚 = 本包 verify 第 4 轮 8 闸全绿 + mutationEvidence 8 条。
   - 未改 F-5/F-6/F-7 决策语义；未触 ADR-017 决策/理由/备选/关联四节任何既有文字。

## 零变化声明

- `docs/requirements.md` / `docs/architecture.md`：零改（架构 §5.5.3 无需再改，与 taskpackage docsSyncReason 及 docs-sync 闸输出一致）。
- `docs/contracts/**`：零触（契约冻结纪律；contract-lint 绿，唯一 warning 为历史批断链存量、非本包）。
- 其余 docRefs（workstream PROPOSAL / task_plan、本包 plan.md、ARQ 归档件、HOST_TEST_CONVENTIONS.md）：零改——均为只读输入，本包无措辞级漂移。
