# CONTRACT-CHANGE-PACKET · story_list data.restore additive（261010-p3-storylist-cold-start 契约面）

> Gate: 261010-p3-storylist-cold-start（spine = delivery/changes/intake/261010-p3-storylist-cold-start.md）/ TECH-261010-p3-storylist-cold-start-empty
> Date: 2026-10-12
> Status: PRECHECKED（architect-reviewer 四分类已出：面 A = CONTRACT_CHANGE_REQUIRED；面 B = NO_CONTRACT_CHANGE；human 未裁）

## 1. Scope

单一文件 `docs/contracts/interface-contract.md`，四处改动（两处插入、两处 modifying）：

| # | 位置 | 类型 |
|---|---|---|
| D1 | §1.3③ :108 `items` 行之后、:109 变更记录行之前 | 插入两条 bullet（字段定义 + 生产纪律） |
| D2 | §1.3③ :105 板子行为句 | modifying（句尾追加 restore 例外子句） |
| D3 | §1.5 第 3 条 :177 story_list floor 枚举 | modifying（枚举追加 restore 可选布尔） |
| D4 | §1.3③ :110 变更记录行之后 | 插入变更记录 bullet |

明确 out of scope：§1.5 第 6 条 :180（面 B 裁 NO_CONTRACT_CHANGE，不动）；§1.3② schedule 面（:85-87 已定稿，不动）；§四 协议剖面（无新消息类型）；requirements/architecture（v1.63 与 :186/:214 已 frozen，本 packet 只读引）。

## 2. Proposed Diff（逐字）

### D1 · §1.3③ 插入（:108 之后）

插入两行（形态逐字复刻 §1.3② :85/:86 先例）：

> - **`data.restore`**（可选布尔，additive——缺省（absent）或显式 `false` 时设备行为完全不变）：`true` = 恢复性载荷（WS 建连附带重推），只填 RAM 列表缓存、不接管前台（P3 在屏时仅重绘本页）。降级双面注记：restore 载荷**板端入板后**解析失败 = 板端静默降级（记日志、缓存零变化、不弹提示、不动前台）；**front 校验面 V27**（脏块拒绝/降级提示、不入板盲渲染）在入板前独立生效——两层降级互不替代。
> - **生产纪律（谁可置 `data.restore`）**：仅 front 后台附带推送路径（WS 建连恢复推送）可置 `restore`；归档成功后 / 删除执行后的主动推送恒不置（缺省 = 主动展示、接管前台，K2 消费面既有语义）。触发时机属行为策略，权威承载 = arch §5.2 场景④，不进契约。story_list 引擎侧来源已被 relay 收紧拒绝（§1.5 第 3 条 Q7 单源），不存在 LLM 直产 restore 载荷路径（区别于 §1.3② 的 LLM 绕过残余风险面，ADR-010 记）。

### D2 · §1.3③ 板子行为句 modifying（:105）

原句（:105）：
> 板子行为：切到 P3 星星记录本，渲染列表（每条 = 星星图标 + 标题 + 日期、选中行带边框、超一屏分屏指示；手势族权威 = requirements §2.3 矩阵——契约不复制交互细节，只钉数据消费与降级：畸形/缺字段拒绝并友好降级、不盲渲染（V27））。

新句（句尾追加，复刻 :87「`data.restore: true` 时除外」句式）：
> 板子行为：切到 P3 星星记录本，渲染列表（每条 = 星星图标 + 标题 + 日期、选中行带边框、超一屏分屏指示；手势族权威 = requirements §2.3 矩阵——契约不复制交互细节，只钉数据消费与降级：畸形/缺字段拒绝并友好降级、不盲渲染（V27））；**`data.restore: true` 时除外——只填列表缓存、不接管前台，P3 在屏时仅重绘本页**。

### D3 · §1.5 第 3 条 floor 枚举随批联动（:177）

原句片段（:177）：
> story_list/story_text 轻量形状 floor（total 整数 ≥0 / items 三键 {id,title,date} / seq 可选整数——2026-09-24 契约门 Q8）

新句片段：
> story_list/story_text 轻量形状 floor（total 整数 ≥0 / items 三键 {id,title,date} / seq 可选整数 / story_list 另认 data.restore 可选布尔——2026-09-24 契约门 Q8 + 2026-10-12 契约门 restore additive）

（不联动则 §1.3③ 新字段与 §1.5 floor 枚举自相矛盾——floor 是 V27 机械校验的契约依据，display_schema.py 同批扩。）

### D4 · §1.3③ 变更记录 bullet（:110 之后）

> - （契约变更记录：2026-10-12 经契约门批准新增可选 `data.restore` 恢复性推送语义（additive——缺省/false 设备行为完全不变）+ 板子行为句随批 modifying——复刻 §1.3② data.restore / ADR-005 先例；requirements v1.63 S-F2.1 补充规则冷启动恢复赎回 260928-story-connect-push-defer 挂起的时点①重裁；随 261010-p3-storylist-cold-start-restore 同 PR 落地，见 CONTRACT-CHANGE-PACKET.md〔delivery/taskpackages/261010-p3-storylist-cold-start-restore/contract-gate/CONTRACT-CHANGE-PACKET.md〕。）

### 示例帧（packet 级说明，不入契约正文——§1.3② 先例未为 restore 另附示例帧，字段 bullet 即权威）

restore 帧（WS 建连恢复推送，新增）：
```json
{ "view": "story_list", "data": { "restore": true, "total": 3,
  "items": [ { "id": "2026-08-20-xiaotuzi", "title": "小兔子找朋友", "date": "08-20" },
             { "id": "2026-08-25-yonggan", "title": "勇敢的小明", "date": "08-25" } ] } }
```
非 restore 帧（现役 ②归档后/③删除后推送，wire 零偏移——data 无 restore 键即缺省 false）：
```json
{ "view": "story_list", "data": { "total": 3,
  "items": [ { "id": "2026-08-20-xiaotuzi", "title": "小兔子找朋友", "date": "08-20" } ] } }
```

## 3. Alternatives（含被否备选）

1. **固件本地页 gating、wire 零变化**（OQ-1 备选②）——放弃（human 已裁：spine OQ-1 账，触点①第三问答「确认」选 restore 标记）。否决理由：现有固件 K3 [S4] gating（mcp_server.cc :1088-1118）只覆盖「记录语境前台」半，wire 零变化方案须扩 gate 覆盖「非 P3 本地页前台」半 = 改 K2 消费面「其余页态照旧推前台」冻结语义（mcp_server.cc :1092-1093 红线原句）；且板端以「P3 是否在屏」自判接管与否 = ADR-005 备选③同型「wire 语义藏进时间推断」drift 温床。
2. **设备按需拉取（device RPC）**——放弃：ADR-005 :47 备选①同型否决（需在 §四新增首例设备发起请求-应答消息对，破 ADR-005「无 device RPC 先例」边界，契约面远重于一个布尔字段）。
3. **NVS 持久化列表缓存**——放弃：违反决策 24（板端 RAM 缓存重启即失是有意不变式，ADR-005 :47 备选④同型不进入备选）。

## 4. Impact（谁产 · 谁消 · 缺省语义三线）

- **谁产**：front——`server/gateway/front/front_app.py` `_push_story_list`（:179）加 restore 形参；建连处理段（:1005-1012 schedule connect push 同位）新增 restore=True 调用（try/except 同 :1009-1012 先例，失败不阻断建连）；现役②归档后（:573）/③删除后（:223）调用点缺省 False 零偏移。defer 时代遗留的 ctx(story) 条件推送（:216「①恢复 = 引导态会话建立同拍」）如何与新建连推送归并（保留/摘除/改标记）= 行为策略，owner = 261010-p3-storylist-cold-start-restore 包（plan.md :29 ARQ-260911 owner 登记），**契约不裁**——builder 落地时与 arch :186 三时点表述对齐即可。
- **谁消**：固件——`firmware/main/mcp_server.cc` VRT_ROUTE_STORY_LIST 分支（:1033-1119）按 `data.restore` 分流：restore=true → 解析装载缓存 + `kidpal_render_node_cache_story` 同步 + 仅 P3 在屏（s_local_page == KP_PAGE_STORY）重绘本页（形态复刻同文件 schedule restore 分支 :926-991：显式持 LVGL 锁写缓存、begin/end_page 不调）；restore 缺省/false → 现行为零偏移（K2 消费 + K3 [S4] gating 全保持）。布尔解析复用既有 helper `mcp_json_get_bool`（:444-446，absent/非 true 字面同判 false——与契约「缺省（absent）或显式 false」语义逐字对齐）。
- **缺省语义（非破坏论证）**：字段 additive 可选；缺省/false 时设备行为完全不变（:929 同款解析 + :947 分流先例在役实证）。旧 front（不带 restore）对新固件 = 现行为；新 front（restore=true）对**旧固件** = 旧固件无 restore 分支、确定性接管前台顶到 P3——与 ADR-005 :45 部署时序注记同型，处置同先例：**ECS 推送开关默认关（schedule 先例 `grid_restore_enabled` front_app.py :743 在役），restore 分支固件烧板验证后人工开闸**（gate evidence C1 类 closeout condition）。
- **其他消费方**：display_schema.py（V27 机械校验）story_list floor 同批认 restore 可选布尔（D3 联动）；relay 引擎侧 story_list 已收紧拒绝（:177 Q7）→ 无 LLM 直产路径，ADR-005 :48 残余风险（F-C5）在 story_list 面**不适用**，ADR-010 如实记此差异。
- **不破坏已跑通链路**：makemeahanzi 笔顺管道（char view）不涉；②③时点 wire 零偏移；items ≤16 窗口 / total 语义（:107-108）不动；删除对账隐式 ack（§四 kidpal_story_delete）不动。

## 5. Human Question（契约门要答的精确问题）

是否批准 `docs/contracts/interface-contract.md` §1.3③ story_list 的 data 块新增可选布尔字段 `data.restore`（additive——缺省/false 设备行为完全不变；true = 恢复性载荷只填 RAM 列表缓存不接管前台、P3 在屏仅重绘本页；复刻 §1.3② / ADR-005 先例），连同 §1.3③ 板子行为句 restoring 例外子句、§1.5 第 3 条 floor 枚举随批补「restore 可选布尔」、§1.3③ 变更记录行，与 261010-p3-storylist-cold-start-restore 实现同批原子落？

## 6. 验证建议（如实标注：本仓无 spec↔contract 机械门，ARQ-260827 口径 = 人工核对 + JSON 示例回归）

1. 契约 diff 逐字人工核对：D1-D4 四处与 §1.3② :85-87 先例形态逐句对读，落 diff inventory 进 architecture-gate-evidence.md §5。
2. JSON 示例回归（host）：restore 帧 / 非 restore 帧 / 畸形 restore 帧（restore 非 bool 字面）三例过 display_schema story_list floor——前两绿、第三例降级。
3. plan.md 验收齿 SL-A1~A6（:39-45）：SL-A1 建连恰一帧 restore（先红后绿）、SL-A2/A3 固件分流纯逻辑、SL-A4 gating 叠加、SL-A5 真机重启冷启动（human 在场）、SL-A6 推送失败不阻断建连。
4. Batch gatekeeper 同步核查：落批前 census 是否存在扫描 `docs/contracts/**` diff 的批级结构门（contract-change-evidence.md §5 要求如实记录 census 结果；本仓预审未发现现役门，主线程复核后落 §5/§8）。
5. 开闸时序：ECS 推送开关默认关 → 固件烧板验证（SL-A5）→ human 开闸（schedule 先例 C1 同型 closeout condition）。

## 7. ADR 建议

新增 **ADR-010**（`docs/ADR.md`，现最末 ADR-009 :76）：「story_list 冷启动恢复 = 『数据面更新 vs 前台接管分离』模式第三实例（home upcoming / schedule grid 之后）：data.restore additive 只填缓存不接管前台，触发时机属行为策略不进契约」——记录：本例与 §1.3② 的两点差异（relay 收紧使 LLM 绕过风险不适用；defer 沿革 260928 摘除→v1.63 赎回）、被否备选（wire 零变化改 K2 语义 / device RPC / NVS）、ADR-005 :46 泛化立场「逐案 review、不自动套用」的履行。
