# pod-retro · 261028-m6-k2b-p2-schedule-16px

> 主线程（planner）第一人称热记忆 · 2026-11-02 · dev-loop Pod（VS Code 扩展会话，同树接棒 P1 收尾后直启本包）。

**【R1】verify 全链绿 + integrator 收口后即写；supervisor 闸前落盘。**

**【R2】计量诚实**：子 agent tokens 有计量——acceptance-author ~293k（三段）；builder ~294k（四段）；integrator 本包收口估 ~50k（±40%，无精确计量）。主线程估 ~90k（±40%）。时长：P2 验收冻结 + 实现 + 收口约 5h 有效墙钟（含 dac2aa32 事故修复轮）。

## 1. 时间账（估）

| 相位 | 内容 | 时长（估） |
|---|---|---|
| launch 面 | 启动卡 + 基线重测锚（host 253/3 @ 6ce77123、server 395 强制重测） | ~30min |
| 验收冻结 | 新冻结 SG16-A1..A6 + 收紧型 supersede + 红基线 253/6 | ~1.5h |
| 实现 | builder step-0 + 实现 + 变异 9 条 + 冻结件缺陷对抗轮 | ~2h |
| 事故修复 | dac2aa32 变异值入库还原 + M3/M6 复测证据回填（18ff3a22） | ~30min |
| 收口 | verify 全链 + integrator 制品 + 本 retro | ~40min |

## 2. Token 账

花费最大处 = acceptance-author 与 builder 基本打平（~293k / ~294k）。可压缩：builder 四段中冻结件缺陷对抗轮（诊断副本定位 + 复测）占比最高；冻结件首版若把 A1 墨高窗按「实测先行」校准，可省约一段。

## 3. 摩擦与卡点

- **F1 · 冻结件 6 处逻辑缺陷由 builder 对抗轮咬出（本包最大摩擦）**：现象 = builder step-0 通道③现跑 258/7，红名单与义务红名单对不上——追出冻结件 6 处逻辑不可满足缺陷（A1 墨高窗 [15..18] 越证据域：16px 资产实测墨高恒 14；旧位全行白负锚与 7 条日列竖线互斥；TEST 3 三处截断帧对误比 f_a/f_b；tooth6 用 day0 x 带判 day2；helper 极值函数命中带底与竖线）→ 根因 = 「验收冻结件也会带缺陷进冻结」——acceptance-author 冻结时点预判窗未实测校准，负锚写进了与既有家具矛盾的形态 → 证据锚 = builder 不入库诊断副本（_sgx_oracle_tmp.c，只修五处缺陷即 263/7 全绿，证明实现满足冻结意图）+ acceptance-author 收紧型修订轮（REVISION LOG 留痕于冻结件文件尾）。**教训成立**：冻结 ≠ 无缺陷，冻结件自身要过「可满足性」实测；**正面证据**：跨角色对抗把缺陷钉在可诊断形态——builder 不改冻结件本体、用诊断副本定位，acceptance-author 凭实测重基线修订、零负锚削弱，缺陷没有溜进实现面歪修。
- **F2 · dac2aa32 提交事故**：现象 = 主线程在 builder 变异轮进行中提交实现轮 commit，把 M3 变异值 `ksg_slot_caps {4,4,4}` 带进 HEAD → 根因 = 提交时序与子 agent 挂着的临时变异态无机械互斥，主线程 commit 前未核工作树是否处于变异态 → 修复 = 18ff3a22 还原 {4,4,3} + M3/M6 修订后复测转红证据回填，host 终态 258/3 与干净基线吻合。**新铁律候选**：主线程 commit 前必须核 builder 是否挂着临时变异——变异态不得跨 commit 边界（建议后续落 verify 前置机械检查）。
- **F3 · M3/M6 MASKED→复测转正**：M3（容量回退）与 M6（下划线 2→3px）首测 MASKED——当时冻结件缺陷齿先中断、变异齿没跑到。冻结件修订轮修掉缺陷齿后复测，两条均转正 turnedRed（258/4 = 3 存量 + 1 新增）。**教训**：MASKED 不是终点——遮挡齿消除后必须复测，否则变异证据缺两条、判别力覆盖虚假齐全。
- **F4 · 基线判读按红名单不按红数**：本包红名单三度变化（冻结态 253/6 → 实现态 258/7 → 修订后 258/3），每次判读都按名单逐条对照而非总数，P1 retro F4 教训继续兑现，零误判。
- **near-miss**：openclaw 包 verify-runs.jsonl 被本包 verifier 首轮窗口期 append 1 行（changeId 解析仍指旧包）——launch 轮 current/taskpackage.json 切包与 verifier 首跑之间的时序窗口。台账 append 无害，但同型窗口若发生在 verdict 投影面会污染包归属。
- **保险丝判定：接住**——builder 对抗轮拦下冻结件缺陷（没让实现面迁就坏断言）；verify 8 闸与 mutation-check 拦格式与覆盖缺口；18ff3a22 把入库变异在收口前钉回。

## 4. 优化建议

1. **冻结件可满足性实测进冻结轮惯例**：治 F1 · 凡预判窗（墨高/盒宽/带位）先跑一次最小实测再定窗，不凭资产元数据推算。
2. **commit 前变异态核查**：治 F2 · 候选机械闸（verify 前置查工作树是否含已知变异标记），铁律先入 retro、机械闸后议。
3. **MASKED 复测义务写进变异证据格式**：治 F3 · mutationEvidence 对 MASKED 条目强制带「复测后终态」字段，缺则 mutation-check 红。
4. **verifier 切包时序窗**：near-miss 登记，launch 轮先切 current/taskpackage.json 再触发 verify（本包顺序已对，但窗口存在，留意派单时序）。

## 5. 一句总结

成本 ≈ 0.65M 子 agent tokens + ~5h，human 触点 0 次 · 最险一刻 = 冻结件 6 处不可满足缺陷若被 builder 就地歪修，负锚将整体失效、验收镜像报废 · **保险丝判定：接住**（诊断副本定位 + 收紧型修订 + 变异复测转正三段接力）· 最值得带走的一条 = **「验收冻结件也会带缺陷进冻结——冻结前实测校准预判窗，冻结后靠跨角色对抗兜底」**。

## 6. 本环遗留

- 真机轮 P2S-E5（pitch 19 紧凑观感 / 4-4-3 容量 / 静默截断 / 下划线★ / 长课截断）——收口后主线程/human 执行，结果供 P3 终端收口复证。
- ARQ-261029 上游 ※ 写入侧（真机轮下划线观测的样本前提）——待 human 确认派单方式。
- F2 新铁律候选（commit 前变异态核查）待主线程裁决是否立账。
- 远程发布待 human（release-pr.mjs --publish 后 human 合 PR）。
