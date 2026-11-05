# pod-retro · 261104-k0a-four-page-skeleton

> Pod 收尾复盘（planner 收尾相位制品，closeout advisory 指名）。Pod 形态 = 交互式 CLI
> planner（--settings --agent planner 全量在位）；base ref 22b30654；本地 main 合并 @3306abac
> （2026-11-05）。留档供 loop-retro 沉淀。

## 顺利面（保持）

- **先写验收 + 变异全链顺畅**：acceptance-author 冻结 15 条断言 9 文件、红基线两轮稳定
  273/21→义务红逐条对表；builder 首轮 16/18 转绿；变异取证 20 条 functional id 全
  turnedRed + md5 还原零残留——「红基线→落地绿→变异敏感」三段证据链一次成型。
- **接棒续跑纪律成立**：两次被停拉起后第三次会话凭交接说明 + 盘上制品核验续跑，intake
  半成品零重复劳动；pod-events 逐节点记账贯穿全程（本包 20+ 事件）。

## 踩坑与教训（供 loop-retro 提共性）

1. **孤儿双写手**（最重）：前两次被停的 headless planner 进程存活，其 acceptance-author
   与新会话派出者并发写同一冻结验收面——acceptance-author 停点上报 + planner 定点
   taskkill（按 CommandLine 鉴别）+ 已落盘产物在盘评审后接管，不推倒重来。
   **共性**：headless 拉起被停后必须进程面清场（查残留 claude -p 进程）再重拉。
2. **拆包预扫规则缺口**：相邻冻结锚反查只扫 `*.acceptance.c`，漏了三类——实现文件
   （log_semantics.c 页名表）、设备消费面注释（lcd_display.cc 邻接锚）、`*.test.c`
   数值镜像（RSR builder 单测）。**共性**：页集/枚举变更的相邻锚必扫清单应含
   「实现文件 + 设备注释锚 + *.test.c 镜像」三类；文档 canary 类 acceptance 起包期
   就标 `kind:'structural'`。
3. **真机轮三轮两缺陷**：host 全绿 ≠ 真机成立（F1 键帽 sync 链、F2 云端阅读路径双真相源），
   两处都是 host 不可见的接线/状态传播缺口。**共性**：设备 TU（render_node/mcp_server）
   的改动，host 齿只证纯函数，真机轮必须真跑——本轮两次救场。
4. **闸链热修簇**（human 两次亲签）：merge-driver 传闸 `--source-ref` 字面量 'HEAD' 不归一化 +
   闸 deviceOutcome sourceCommit 严格相等在「证据随合并提交」下自引用不可满足——261028 P8
   设备轮硬门立起后首个端到端走通该路径的包把两处缺口全部踩出。**共性**：新闸立起后应有
   一次「闸链演练」（dry-run 走通全链）再依赖；首包当小白鼠的成本是三轮真机 + 四轮闸拉锯。
5. **证据自引用与基线锚纪律**：device-round-evidence sourceCommit 在 rebase 重写下不断
   失效——最终以「线性化链上固件同源提交 + 祖先判据（binaryRef SHA-256 硬绑定内容）」收口；
   base ref 22b30654 口径贯穿全程未漂。

## 流程债与尾差（交主线程）

- merge-driver/闸热修簇 + F3 分屏指示 debt 的 **gov 登记**（ARQ 条目）未做——主线程
  以 /loop-retro 或 gov fast-path 补（pod-events human-decision 两条 + 本文件为凭）。
- origin/main 与本地 main 的 release PR 回同步：origin 走前（PR #52 已合），本地 main
  已 merge origin（258db475）；push 仍归 human（release-pr.mjs --publish）。
- K0 解锁：重取基线、追加共享 CMake 登记行（K0a merged 后重取基线 = 本地 main 3306abac）。
