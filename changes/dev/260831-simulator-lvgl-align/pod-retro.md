# Pod Retro — 260831-simulator-lvgl-align（第一人称热记忆）

> 写于 closeout 后、supervisor gate 前。非权威事实源——tokens/gates/rounds/verdict/test results 一律以机器件（verifier-evidence / review-packet / changes/dev 制品）为准；本文件只记我全程在场所感到的卡点与改进点。

## 卡点与绕圈体感

1. 最隐蔽的坑是 LVGL 树被 gitignore（`tools/.gitignore` 里 `simulator/LvglPlatform/`），所以「9.5→9.3 降级」这步根本进不了 git——到 integrator 收口才发现得靠手工复现（拷固件 managed_components + 跑 regenerate 脚本）。这类「本地 vendored 但不可复现」的环境，起包时就该当一等公民写清复现步骤，而不是收口才补。
2. 实验代码的债比想象深：阈值 128（固件 127）、snapshot_timer 用 PostQuitMessage 但主循环不泵消息根本退不出、kidpal_ui.c 里散着 C:/tmp 直写。这几处都是 260830 主线程一次性实验留下的漂移，正式化时一次性全撞出来——正好印证「实验代码必须 dev-loop 接管」那条规定是对的，但也说明一次性实验的债会涨利息。
3. 承重闸对 contractRefs 非空是硬要求，可这包明明不碰契约，作者（上一会话的我）就顺手留了空数组 → 收口红。兄弟包 TP-B 同样埋着这个雷，外加 landsIn 裸 ID 对不上带括号 ID 的老毛病。字段级的「默认惯例」没写进起包指引，纯靠撞。
4. 「脚本改了、数据没重跑」这个尾巴：builder 把阈值统一写进脚本（127），但 assets.c/png 还是老 128 产物，收口只能标 defer 给 TP-B 一起重生成。这是「最小实现」跟「数据一致性」的一次拉扯——脚本是单一源、数据是派生件，派生的更新成本（约一万五千行 churn）让人想躲，可躲了又留下 127/128 一档像素错位隐患。

## 帮倒忙的规则/钩子

- 冻结验收里 A5.1 的 grep 目标写成了 assets.h，可 C 语言里 extern 声明在 .h、定义体（含 .cf）在 .c；验收作者没读实现结构就钉错了 grep 目标，builder 只能反过来在 .h 加标注「喂」给验收入口，才能让既定断言命中。

## 下次改进

- 给「gitignored vendored 依赖」立个规矩：起包时必须在 taskpackage 里带复现步骤，别留到 closeout。
- 起包指引里写明白两件字段惯例：contractRefs 永远列 interface-contract.md（哪怕是只读引用）；acceptance ID 保持裸 ID、别带括号后缀（否则 decisionCoverage 的 landsIn 对不上）。
- 阈值这类「符号级单一源」应该从起包就是一个常量 + 一个来源；资产管线的阈值早在 128 就跟固件 127 漂了，一直没人验到。