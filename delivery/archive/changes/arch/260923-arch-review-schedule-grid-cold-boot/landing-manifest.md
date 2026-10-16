# Landing Manifest · 260923-arch-review-schedule-grid-cold-boot

> packet-bound landing 清单 = v3 草案（candidate-spec-draft.md，R1-R11 回灌定稿 + F-C5 human 裁定回灌）+ 契约门 human 批准（2026-09-23 AskUserQuestion「批准（C1 挂账）」）。同 PR（同 commit）原子落地；landing-verifier 逐文件 diff 核对「实际落地 == 本清单」。

## L1 · docs/requirements.md — S-F1.2 补充规则段末追加一句

位置：S-F1.2 补充规则行（L271）「埋点 = 无。」之后（同段内追加）。追加文本（R1 后口径）：

> 冷启动恢复 = WS 建连与每日 0:00 跨天时云端附带重推周网格（恢复性推送，只填缓存不抢前台，P2 在屏时刷新本页；录入 marker 同型附带，触发时机权威承载见 arch §5.2 / ADR-005）；断网或未收到推送时翻到本页显示友好空态提示（引导孩子用语音问课表），联网恢复后下次建连自动补齐。

## L2 · docs/requirements.md — NFR-OFF-1 行追加半句注记（R6）

位置：NFR-OFF-1 阈值行「…设备不缓存任何事项。」后追加：

> （RAM 渲染缓冲不禁、掉电即失——见 arch §6.2 澄清。）

## L3 · docs/architecture.md — §5.2 场景①段末追加（R2 后口径）

位置：「**① 开机与常显**」段（L207）末尾追加：

> P2 周网格缓存恢复 = 三触发（与「接下来」同源同机）的附带推送：WS 建连、每日 0:00 跨天与录入 marker 时，front 在推「接下来」后附带重推周网格**恢复性载荷**（契约 §1.3② `data.restore: true` 语义：设备只填 RAM 网格缓存、不接管前台，P2 在屏时仅重绘本页）。决策 26 三触发表述只覆盖 upcoming，网格恢复面由 ADR-005 补齐；语音「帮我显示课表」意图薄载荷展开仍为主动展示、无条件接管前台（M3-K3 冻结行为）。断网或推送未达期间翻到 P2 = 友好空态（决策 24 离线边界不变，网格缓存属 RAM 渲染缓冲、§5.4 板端状态行）。

## L4 · docs/architecture.md — §4 追踪表 NFR-OFF-1 行追加半句注记（R6）

位置：追踪表「NFR-OFF-1 离线边界」行「…断网仅本地时间/温湿度/电量」后追加：

> +RAM 渲染缓冲不禁（掉电即失，§6.2 澄清）

## L5 · docs/architecture.md — §6.2 追加澄清句（R5）

位置：「不缓存任何事项。」之后追加：

> （「不缓存」禁的是落非易失介质的持久层；板端 RAM 渲染缓冲——P2 网格缓存、P1 seam——掉电即失，不在禁列，见 §5.4 板端状态行。）

## L6 · docs/ADR.md — append ADR-005

全文 = v3 草案 §3.4（ADR-005 五节 + 残余风险记录段，F-C5 human 裁定 2026-09-23 采 defender 方案；含部署时序句 R4 / K1 验收锚与触发必要性分层 R8 / 泛化立场 R9 / 在屏判定括注 R10）。append-only，不覆盖既有条目。

## L7 · docs/contracts/interface-contract.md — §1.3② 四件（契约门已批，C1 挂账；git apply 通道落地）

1. **modifying L85 板子行为句**：在「…今天列整列高亮）」后插「；`data.restore: true` 时除外——只填网格缓存、不接管前台，P2 在屏时仅重绘本页」。
2. **新增 `data.restore` bullet**（「今天」列 bullet 之后）：

   > - **`data.restore`**（可选布尔，additive——缺省（absent）或显式 `false` 时设备行为完全不变）：`true` = 恢复性载荷，只填网格缓存、不接管前台（P2 在屏时仅重绘本页）。降级双面注记：restore 载荷**板端入板后**解析失败 = 板端静默降级（记日志、缓存零变化、不弹提示、不动前台）；**front 校验面 V27**（脏块拒绝/降级提示、不入板盲渲染）在入板前独立生效——两层降级互不替代。

3. **新增生产纪律句**（restore bullet 之后）：

   > - **生产纪律（谁可置 `data.restore`）**：仅 front 后台附带推送路径可置 `restore`（触发时机属行为策略，权威承载 = arch §5.2 / ADR-005，不进契约）；意图薄载荷 `{view:schedule,data:{}}` 的展开（`expand_schedule_intent_payload`）恒不置——展开产物 = 主动展示、接管前台（M3-K3 冻结行为）。

4. **契约变更记录行**（对齐 2026-09-21 记录行格式，追加在既有记录行之后）：

   > - （契约变更记录：2026-09-23 经契约门批准新增可选 `data.restore` 恢复性推送语义——additive 字段 + 既有板子行为条款 modifying；随 260923-m3-k7-schedule-grid-restore 同 PR 落地，见 [CONTRACT-CHANGE-PACKET.md](../../delivery/changes/arch/260923-arch-review-schedule-grid-cold-boot/CONTRACT-CHANGE-PACKET.md)。）

   实现包 id 假定 = `260923-m3-k7-schedule-grid-restore`（随本 landing 一并建包对齐；若 human 改名，本行连带改）。

## 不落地项（零触碰确认）

- docs/contracts/ 其它节（§1.3⑥/§1.5/§1.6/§2.3/§四）、requirements S-F1.2 正文与 AC1-AC4、arch §10 决策历史档（决策 26 原文不动，修订以 ADR-005 承载）。
- ECS 推送开关：保持默认关（C1 closeout 挂账，builder 实测复核后人工开闸）。

## 同 commit 附带

- reviewDir 制品（candidate-spec-draft.md / charter / findings×3 / review-packet / deadlocks / debate-log / 本 manifest）随同 commit 入库。
- `docs/ARCH-REVIEW-QUEUE.md` ARQ-260923-schedule-grid-restore-gate 行状态更新（F-C5 已裁 accept-as-risk + 契约门已批 → 随冻结转 watchlist / 销行注记）——append/行内状态更新按 queue 惯例。
