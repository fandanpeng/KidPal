# pod-retro — 260831-ui-art-assets（第一人称热记忆）

> planner 收尾相位（integrator closeout 之后、supervisor 机械闸之前）趁热写。只记第一人称过程体感，不冠机器权威。

## 过程体感

这是功能支撑验证批的第三包，也是**首个把这套循环走到 bearing round + supervisor 机械闸**的包——A/C 两包当时的循环没踩到这个深度。所以本包替我踩出了几条 A/C 没暴露的闸面摩擦。

### 卡点 1：current/taskpackage.json 的散列一致性是无声雷
builder 交付后第一次真跑 verifier 直接「current TaskPackage missing」——planner 起包 checklist 里的「刷新 current/」我漏做了。补副本时又撞换行符不匹配：canonical 文件带 `\r`（Windows 写法），我 Write 出来的是 LF 且无尾换行，`taskpackage-required` 闸按 SHA256 字节比对于是红。根因 = 两个工具写出的字节规范没对齐（历史的 CRLF vs 我的 LF）。教训：起包当下就 refresh current/，用同一套字节写两处，别拖到收尾再回补。

### 卡点 2：验收文件「可识别断言」的格式期望没提前讲清
acceptance-author 按前包模板写的是 markdown prose（通过判据 + bash 命令），被 `acceptance-coverage` 判「无可识别断言」——闸要 `expect(`/`assert(` 这类代码式字样。回派补了 31 条逐条映射到机判脚本的机器断言才过。别扭点：验收作者的「沿前包惯性」和闸的「代码式断言期望」从没在同一条线上对齐过，纯靠红一次再回派才暴露。

### 卡点 3：带 `(B#)` 的 acceptance ID 破坏 `\b` 匹配
ID 写成「UAA-A2(B1)」、结尾 `)` 后是换行，coverage 闸的 `\b` 匹配不到，标记得强写成 `coverage: UAA-A2(B1)_B1` 这种尾随下划线的丑形态。这是我在 acceptance ID 里塞 parenthetical 的取舍代价——human 可读性和机闸可匹配性在上括号这件事上打架。下次要么 ID 不带括号，要么认这种丑标记。

### 卡点 4：mutation / docs-sync 两闸对「资产生成包」错位
mutation 闸要「删除源码→测试变红」的证据，docs-sync 闸要「文档提到 acceptance ID」；资产包两样都不天然成立（没有可 mutation 的源码语义、docs 用 B 编号而非 UAA 编号）。用闸自带的 `mutationRequired:false` / `docsSyncRequired:false` + 诚实 reason 声明化解。体会：这两个闸是给 delivery-story 代码包设计的，planning-workstream 资产包要**主动声明降级**，别硬扛到红。

## 人工纠偏体感

human 在起包前点名「先读 planner 定义、别再漏 pod-event 和 pod-retro」——这条纠对了：A/C 包我是收尾补记 retro，本包 pod-events 全程实时 append、pod-retro 在 closeout 相位写。这是对的节奏，下次照此默认。

## 下次改进

- 起包即 refresh current/，且字节规范（LF）一致。
- 派 acceptance-author / builder 前，把要喂给机械闸的格式期望（可识别断言、coverage 匹配形态）写死进派发 prompt，避免 prose-only 一次返工。
- 资产/规划类包对 mutation / docs-sync 的降级声明，在 Draft TaskPackage 阶段就写死字段，别等 verifier 红再补。