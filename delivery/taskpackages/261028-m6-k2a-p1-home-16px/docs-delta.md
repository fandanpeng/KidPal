# docs-delta · 261028-m6-k2a-p1-home-16px

- 结论：**canonical docs 零 diff**（`docsSyncRequired=false`，docs-sync 闸 PASS 输出与本声明一致）。
- `docs/requirements.md`：v1.69 已冻结（S-F1.1-AC1/AC2、WF-F1-01），本包只追实现，零改。
- `docs/architecture.md`：§5.5.3 首页帧面已冻结，本包零触。
- `docs/contracts/interface-contract.md`：零触（恒只读；wire 形状不变是 F-9 裁定——[U1] 只消费 name 首字符 ※ 前缀，不扩字段、不改 §1.3⑥ {time,name} 语义）。
- `docs/decisions/ADR.md`：ADR-017（字体体系）P0 批已落，本包零新增 ADR（`adrRequired=false`，无新架构裁决）。
- `docs/ui/**`：V8 canvas-p1 布局权威只消费不重出（OQ2），零改。
- 本包新增 docs 面 = `docs/debt/tech/ARQ-261029-home-temp-marker-producer.md`（debt 账本条目，F-9 上游 ※ 写入缺口立账，human 裁上游补写入侧）+ `docs/debt/tech/INDEX.md` 登记——属 debt 账本，非 canonical spec/契约面。
