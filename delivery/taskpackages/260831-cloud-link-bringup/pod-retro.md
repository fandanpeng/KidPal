# Pod Retro — 260831-cloud-link-bringup（第一人称热记忆）

> 写于 closeout 后、supervisor gate 前。非权威事实源——tokens/gates/rounds/verdict/test results 一律以机器件（verifier-evidence / review-packet / changes/dev 制品）为准；本文件只记我全程在场所感到的卡点与改进点。

## 卡点与绕圈体感

1. 唯一一次「红→追根」绕圈是验收自己的坑，不是实现错。转写那条验收老红，追下去是测试没给 faster-whisper 传 `initial_prompt`，tiny 档默认吐繁体，`assertIn("公园")` 当然不中。别扭处在于：一场 live-ECS 验收把「模型语言行为（简/繁）」当成了隐式前提，而 tiny 档在这点上很脆。实现侧（shim 的 /v1/asr）其实是对的，只是冻结测试没对齐它。
2. char 类查询冷启动慢（百秒级）踩爆了验收的 curl 超时，又造成一次「超时伪红」。体感是：给 live 外部系统写验收时，没把「慢但正确」和「真失败」分开，全堆在一句同步 curl 上。
3. ECS 环境处处是地雷，且全是 builder/integrator 现场才撞出来：huggingface 直连被墙（得绕 hf-mirror）、系统 Python 3.6.8（whisper 另建 3.11 venv）、Git Bash/MSYS2 把 `/opt/...` 路径吞成 `C:/Program Files/Git/opt/...`。这些该在起包时就录成「已知环境前置」，而不是每个 agent 各自再考古一遍。
4. closeout 机械闸是坏的（closeout-check.mjs 缺 delivery-status.mjs 依赖崩溃），fallback 直接跑承重闸，又撞出 decisionCoverage 的 landsIn 引用写成裸 ID、跟 acceptance 里带 `(OC-x)` 的 ID 对不上。这一串是治理机制本身的摩擦，跟业务无关，却吃最多回改时间。

## 帮倒忙的规则/钩子

- acceptance 写进「带括号的 ID」（`CLB-A3(OC-6)`）这件事，跟 decisionCoverage 的 `landsIn: acceptance:CLB-A3` 裸引用天然打架，机械闸就靠这个判错。要么 acceptance ID 保持裸 `CLB-A3`（说明放别处），要么 landsIn 校验做归一。

## 下次改进

- live-ECS 类 package 起包时就录「环境前置清单」（GFW/镜像、Python 版本、路径转义），别等 builder 现场考古。
- 给外部系统验收留「慢路径」分支，区分「慢但正确」和「真失败」，超时阈值单独放宽到慢查询档位。
- 契约字段信封（shim /v1/* 的 `{text}`→`{audio,display}`）这次靠「冻结决策 + env 覆盖」硬撑，没进契约——这个缺口的回填要排进契约门，否则下次谁碰 shim 都得重猜。