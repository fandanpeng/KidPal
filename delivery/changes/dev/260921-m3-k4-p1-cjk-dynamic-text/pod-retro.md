# pod-retro · 260921-m3-k4-p1-cjk-dynamic-text（第一人称热记忆，planner）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts / verifier-evidence / pod-events 为权威，本文件不复述。escalation themes：本包零 agent-escalation 事件（pod-events 交叉引：human-decision `b396f1ff` 主线程代跑 launch、friction over-scan ×2、dispatch ×4）。

## 卡点与体感

1. **本棒最顺的一步是「判红依据先亲核再路由」**。builder 报唯一红并声明是冻结验收自身算术缺陷（D==3D 互斥）——我没有直接转发，先自己读了 `pcn_diff_stats` 的 out_total 语义和 TEST 2 求和段，确认互斥成立才派 acceptance-author 修冻齿。builder「不私改冻齿、升级 planner」的纪律和我的亲核形成了双保险，这条链路一次往返就闭合了，体感上比 K 期「verifier↔fixer 空转」的棒次干净得多。
2. **scoped r1/r2 两红全是 planner 侧元数据格式错，不是代码红**——contractRefs 空数组被判缺、mutationEvidence 用短 id 不匹配全 id 判据。两轮本可省：起包时 K3 的 taskpackage 里 contractRefs 就有非空先例可抄，mutation-check 的「全 id + 敏感信号」锐边我在 K2/K3 期也没踩过所以没有预案。**教训：finalize 时对着闸的判据（而非自己的直觉格式）回填，两轮预算就是净赚的。**
3. **acceptance-author 两棒都发挥稳**：第一棒红基线两通道实跑 + A3 单项零墨齿诚实补缺（对我派单里「不需要新齿」的判语做了有据修正——这种向下修正比照单全收更有价值）；第二棒最小补丁修冻齿 + REVISION LOG + 判别力两层保全自查，零越界。
4. **over-scan 申报纪律运转顺滑**：acceptance-author 7 行、builder 5 行，全部任务必需或纪律强制，无一行是漫读。派发时那句「超读申报表必须诚实列」继续是低成本的防线。
5. integrator 这棒的 final bearing 轮自主跑掉了（K3 先例同款），我 FS-EVIDENCE-3 逐项复核制品在场 + closeout-check + git log 后直接进机械闸——收尾相位的动作清单已经磨合到无卡点。
6. Review Packet 一条 NIT（mutation 执行面留痕粒度：登记 ≠ 执行证明）——我裁不阻塞：红基线 stub 跑 ≙ A1 变异态在案、A2 去 clip 变异复验在 REVISION LOG，其余条目的执行留痕若判官要，iterationBudget 容得下补跑轮。这是诚实登记面与执行面的已知粒度差，不是本包缺陷。

## 别扭的 acceptance / 帮倒忙的钩子

- mutation-check 的全 id 匹配判据对起包方是隐式的（报错只说 missingSensitive，不提示「要全文 + sensitive」）——对新人或新包形态是绊点，建议报错信息里带一句格式提示（机械闸改进建议，非本包阻塞）。
- schedule_grid 的 E2 canary 扫注释子串（`!contains(home_p1.c,"upcoming")`）让 builder 被迫改注释措辞——canary 识字串而非识语义的老问题，这次代价只是一句措辞，但值得记着。

## 人工纠偏体感

- 本棒 human 只出场一次（launch 指示主线程代跑，K3 先例复用），loop 中途零打断——分层出场纪律连续第三棒成立。
- loop-retro 阈值 18（15 pending + 3 waiting）继续挂着——发起责任在主线程/human，K2、K3、K4 三棒 retro 都提了，这笔账越滚越大，收尾报告继续提请。

## 下次改进

- finalize taskpackage 前把三个元数据闸（taskpackage-required / mutation-check / acceptance-coverage）的判据当 checklist 走一遍，别凭 K 期记忆填字段。
- 「子 agent 报告断言的缺陷根因，先花一次读代码的功夫亲核再路由」——本棒再次验证划算，升为习惯。

## 追加轮体感（2026-09-22 · live FAIL 修复 + 版式扩展，最后一轮为准）

1. **live 轮的失败本身是这包最有价值的时刻**：host 80 齿全绿、机械闸 APPROVE、真机一烧就乱码——「齿测不到的路径才是风险面」。根因（raw A1 连续位流 vs 字节对齐行）我在派单前用只读调查实锤到 LVGL 解码循环源码级，human 才能一次拍板 B 方案。**只读根因调查先于 fixer 派发，这个 human 介入点救了一轮盲修。**
2. **host 全绿 ≠ 设备正确的机制这次看清了**：host Source Han bpp4 + 偶宽字形让两种寻址碰巧等价——「双分支强符号同算法」的等价性验证齿对 bpp1 连续位流这类格式语义差异天然盲。方案 B 走 LVGL 官方解码后这个假设面整个消失，比修位寻址多值。
3. **两处冻齿缺陷（TEST 2 算术、TEST 9 tooth4 live/快照笔误）都是 builder 先红 + planner 亲核 + acceptance-author 修**——「验收测试的缺陷也走所有权链，不经 fixer」这条线三次都走顺了，每次一轮闭合。
4. HPB 跨包修订窗口是新模式：版式扩展震到别的包的冻结齿（tooth2/4/5），授权链 = human 拍板事件原文逐字核对。tooth 4 前半重钉超出登记面一点（同 tooth 语义随版式演进），acceptance-author 主动申报理由——**跨包修订要的就是这种「超出授权面就明说」**。
5. human 分层出场依旧：live FAIL 报告 → human 两个判断点（先调查后修 + B 方案 + 版式扩展），loop 内零打断。追加轮 r5/r6 verifier 一次全绿，与主轮的元数据红形成对比——判据 checklist 化起效了。

## 第 3 追加轮体感（2026-09-22 · A11 设备字体 host 覆盖 + 真根因，最后一轮为准）

1. **「设备字体上 host」这个 human 硬要求是本轮的胜负手**。读代码两轮没收敛（我先给了连续位流诊断、B 方案按官方语义重写了仍乱码）——把 font_puhui_14_1 编进 host runner 加独立 oracle 齿后，一次接线就复现：真根因是 `get_glyph_bitmap` 返回 `lv_draw_buf_t*` 结构指针被强转当像素读，读的是结构头垃圾。**连续两轮 live FAIL 的「不同乱码形态」同一瞬间得到解释（垃圾值随内存布局漂移）。教训：同一缺陷两次修复都没踩中时，停止读代码推理，把缺失的实证面（真实输入 × 同款代码路径）搬到可观测侧。**
2. **B 方案修复后我一度以为 host 全绿 = 语义正确**——实际上 host 从来没跑过设备字体，Source Han bpp4 齿的「绿」对 bpp1 路径是彻底的盲区。「双分支同算法」的验证面要有双分支输入才算数，这条该进 seam 类实现的验收常识。
3. r7 设备编译红（C++ const cast，host C TU 隐式转换合法所以只有设备侧暴露）是「双 TU 同构」的又一镜像——连编译器行为都两边不一致，fixer 一行补丁闭环。
4. 治理收益实打实：A11 五 tooth（含阈值判别面移 bpp4 子集的防免疫设计）永久入册，此后字形路径回归免烧板。acceptance-author 的 oracle 独立性设计（零调用官方解码入口、Python ASCII art 预验证）是齿可信的关键。
