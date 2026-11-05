# pod-retro · 261104-k1-points-ledger-core

> Pod 收尾复盘（planner 收尾相位制品，closeout advisory 指名）。Pod 形态 = VS Code 扩展
> 会话 planner 内联（human 指示「以 Planner 形式在本会话跑，不 headless」）；base ref
> 85cc3747；pod-events 25+ 事件贯穿。留档供 loop-retro 沉淀。

## 顺利面（保持）

- **冻结契约先行 + builder 双向独立验证**：acceptance-author 冻结的 10 case 把账本不变量
  逐条钉死（OQ-1 例子原样进断言），builder 实现后反而用「实现+探针」把冻结件三处内部
  矛盾逼出来——作者≠审者的对偶在这里是双向的，实现轮是冻结件的第二双眼。重冻最小修正
  （断言实质与 binding refs 不动）一轮收口。
- **机械闸链全程无人工绕行**：launch 门 → verify scoped 8/8 → final 10/10（含 device
  build 真编译），每处红都是门抓真问题（AC backref 集、live-manual 承载形态、变异簿记），
  无一处是环境噪音；修法全部有 K0 先例对表。
- **input-brief 行号级备料值回票价**：brief ② 的对端实读（K0 现状函数行号）让
  acceptance-author 与 builder 零考古起步；GAP-1 承接判定 + K0 rulings 转录避免了跨包
  事实丢失。

## 踩坑与教训（供 loop-retro 提共性）

1. **冻结验收也会自相矛盾**：三处（A4 revision 与 A8/契约互斥、chain helper ±1 步长
   假设、A9 期望字节/权威值算错）在红基线期不可见——它们全是「实现缺席型红」，只有实现
   在盘后才暴露。**共性**：验收件内部一致性核对（同一常量/契约条目跨 case 对表）应成为
   acceptance-author 交付前自检步；red 基线只证「红因正确」，不证「绿路径可满足」。
2. **mentionsId 全 id 子串匹配的簿记债**：perAcceptance 记录文本是 acceptance 串的
   复制——上游串一改（AC 引用形态修正），下游匹配静默失效。**共性**：门消费的派生文本
   字段要么引用 id（短 id）要么程序生成，人工复制全文本就是给未来的自己埋雷。
3. **AC backref 门的双向判据**：多引 = 少引 = 方向偏差信号，sourceRefs 标签必须纯形态
   （锚定正则）、注记只能住 acceptance 串文本——第一版注记进 sourceRefs 被锚定正则打回。
   **共性**：给机械门喂数据前先读门的正则判据，不猜格式。
4. **live-manual 承载形态是地方知识**：K0 先例（冻结件头 `coverage:` 标记承载、不进
   taskpackage acceptance 数组）没有成文在 task-intake/taskpackage-authoring 的显眼处，
   本次靠查 K0 文件才对上。**共性**：跨 pod 反复出现的形态先例应上收到 SSOT 一句话。
5. **扩展会话降级形态的收尾义务**：settings-presence 申报 + 收尾手工跑
   supervisor-gate-check.mjs + 手工 reconcile——三件套在 pod-events 有记，收尾相位照办
   无遗漏。

## 流程债与尾差（交主线程/human）

- K1L-L1/L2 真机轮（human 在场）：QQ 落账后不抢前台/原地刷新（GAP-1 活性接线）、真实
  消费端分页对账——设备轮证据 none，本包离线面全绿不冒充真机终验。
- 部署轮：manifest 新增 points.py 条目铺放 + `/opt/kidpal/points/` 布局迁移
  （KIDPAL_POINTS_STATE env 单源不变）——K0 live 旧位 `/opt/kidpal/points.json` 一次性
  迁移注记在 deploy.sh。
- manifest 扩单（第 9 项 test_points_spike_units.py）与 GAP-1 承接判定请 supervisor/
  human 复核（review-packet finding ②）。
