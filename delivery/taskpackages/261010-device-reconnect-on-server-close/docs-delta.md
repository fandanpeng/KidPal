# docs-delta —— 261010-device-reconnect-on-server-close

> blocking closeout 制品。**arch 落笔 = architect 代写**（scope-map agents 段：integrator 无 `docs/architecture.md` 写权）；本文件提供逐点精确落笔文本，architect 转写后由 planner 派第二轮收口 commit（冻结 diff 须含 arch 四投影点）。
> 源：taskpackage `docsSyncReason` + plan.md「docs-sync 落档」节 + human 裁定原话「永远不休眠」（2026-10-10 触点①）。
> 行号锚 = 2026-10-11 本仓 `docs/architecture.md` 实况核对（:133 / :322 / :354 / §10 末条 28 在 :419）。

## 零改申报

- `docs/requirements.md` 零改——提醒必达语义已钉（requirements.md:67），本包 = 兑现，无新增语义（RC-E2）。
- `docs/contracts/**` 零改——WS wire 形状零变化，重连复用既有 hello 建连序列（RC-E2）。

## 投影点 1：§10 决策表 append 决策 29（锚：:419 决策 28 条目之后、:421 `---` 之前）

新增一行（编号核 = 29，27/28 已占用；建包时已修正 architect 原稿笔误并留痕于 RC-E3）：

```
29. **设备永不休眠、常驻在线（2026-10-10 human 触点①裁定，原话「永远不休眠」）**：设备常驻在线、永不休眠——提醒推送可达性优先于闲置功耗；会话收尾本地关后短延迟（T=5s，kResidentRebuildDelayS，已冻结进齿）常驻重建，Idle 态恒持活 WS；服务端/网络侧断自动重连复用既有 TryStartAutoConnect 守卫与退避、本地关 8 调用点分流零误重连；板载 18650 电池续航代价随裁定一并接受（使用形态转随用随充）。落地 = dev 包 261010-device-reconnect-on-server-close；§3.3 / §6.4 / §8.1 残余同步闭账（本条）。
```

advisory 注记（交 architect 落笔时裁量，integrator 不裁）：§10 头注（:390）写「2026-09-14 起新决策一律登记 ADR.md；本节以下为 MVP 时代决策历史档，冻结不再新增」——决策 29 append 与该头注存在措辞张力。建议 architect 落笔时在头注补一句指针（如「产品级休眠裁定（决策 29）经 human 触点①显式指示随 dev 包落本表，为既定例外」），或按其判断调整；不改裁定内容本体。

## 投影点 2：§3.3 假设表「设备常驻在线」行改写（锚：:133）

现文（逐字）：

```
| 设备常驻在线（提醒推送可达） | 引擎侧两道超时已修正为 24h 兜底（决策 21）；残余 = 闲置约 3 分钟休眠断连 → `alert_skip reason=no_online_device`【产品决策待裁，§8】 |
```

改为：

```
| 设备常驻在线（提醒推送可达） | 引擎侧两道超时已修正为 24h 兜底（决策 21）；已裁：永不休眠（决策 29）；重连 + 常驻重建已落（261010 包） |
```

留痕：plan「docs-sync 落档」节第 2 条源文本写「（260910 包）」，对照 changeId `261010-device-reconnect-on-server-close` 判为笔误，落笔取 261010（先例 = RC-E3 建包修正 architect 原稿决策编号笔误，留痕处置同款）。

## 投影点 3：§8.1 风险债表第 3 行闭账（锚：:354）

现文（逐字）：

```
| 3 | 设备闲置 ~3 分钟休眠断连 → `alert_skip` | 提醒不可达窗口 | 常驻连接与休眠的取舍 = 产品决策待裁 | human |
```

改为（闭账形态对齐表内第 7 行【已闭合】先例）：

```
| 3 | 设备闲置 ~3 分钟休眠断连 → `alert_skip` | 提醒不可达窗口 | **已闭账**：human 2026-10-10 触点①裁「永远不休眠」（决策 29）——服务端/网络侧断自动重连 + 会话收尾常驻重建随 261010 包根治，离线驻留窗口消除；电池续航代价随裁定接受 | 已闭（原 owner human） |
```

## 投影点 4：§6.4 已知残余句改写（锚：:322 段末句）

现文段末句（逐字）：

```
已知残余：设备闲置约 3 分钟休眠断 WS → `alert_skip reason=no_online_device`【产品决策待裁，§8】。
```

改为：

```
休眠残余闭账（2026-10-11）：human 2026-10-10 触点①裁「永远不休眠」（决策 29）——设备 Idle 态恒持活 WS 常驻在线，会话收尾本地关后 T=5s 常驻重建、服务端/网络侧断自动重连（261010 包落地），`alert_skip reason=no_online_device` 离线驻留窗口消除。常驻连接 modem 省电档（LOW_POWER/BALANCED）以 WS keepalive 长稳为准取低档，结论待真机观测冻结（RC-B6）。
```
