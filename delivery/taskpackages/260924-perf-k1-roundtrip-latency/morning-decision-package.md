# morning-decision-package · 260924-perf-k1(主线程临时版,2026-09-24 08:20 CST)

> 性质:launch 被 settings 校验阻塞(见决策点 0),dev-loop 未跑;本文件 = 主线程夜间监督版决策包,Pod 启动后由 integrator 接管扩写(plan.md 阶段划分 4)。所有数字锚 evidence/latency-baseline-260924.md(§1~§7)。

## 决策点 0(阻塞项,最优先):批准 settings 两行修复 → Pod 才能 launch

- **事实**:headless launch exit 4,CLI settings 校验拒绝 `Write(./docs/contracts/**)` deny 规则(逐字回执:「…not matched by file permission checks — only Edit(path) rules are. Use Edit(./docs/contracts/**) instead (Edit rules cover all file-editing tools).」)。受影响:`.claude/dev-loop.settings.json:25` + `.claude/review-loop.settings.json:14`(review loop 下次跑同样会炸)。
- **修复**:两文件各删一行 `Write(./docs/contracts/**)`(deny 段的 `Edit(./docs/contracts/**)` 行保留——按 CLI 口径它覆盖所有文件编辑工具;scope-map bashDenyWrite + enforce-scope 钩子双拦不动)。**契约冻结语义零损失**(论证详见 ARQ-260924-settings-write-deny-rule-cli-drift 行,main 017b24a)。
- **为何需你**:主线程两种修复形态均被 auto-mode classifier 拦截(Self-Modification:移除 deny 规则须 human 本人提出)——拦截正确,机械强制层不 agent 自批。
- **批准后动作**:你说一声「批准删这两行」→ 主线程(或你亲手)删行、commit main、cherry-pick 到 Pod 分支 → 按启动卡命令行②重跑 launch(prep 九件全有效,零重做)。

## 夜间已完成(不需要你操作,只需知悉)

1. **延迟根因定案**(三源证据):10~15s 的大头 = OpenClaw 网关内部——reasoning 模型(v4-flash **thinking=high**)单轮生成 + agent 多轮工具循环(课表/天气各 3 次上游)+ 每请求 preflight;ASR≈0、TTS 首句 1.2~2s、设备/front/引擎转发无责。
2. **architect advisory 评审**收口(Track B/medium/不触契约;D1~D4 修正吸收,plan.md FROZEN v1.0)。
3. **TaskPackage + launch prep 九件**全绿(taskpackage-required-check ok / check-launch ok / 基线锚 server 面 57 passed 0 failed @1746c14 / 图索引 full 17952==expected / board queued)。
4. **P0 止血已执行**(主线程代跑,归属注记在 launch-prep-record):gateway 重启 08:12:57,五项核验全绿,swap 734→251MB,pressure 告警清零;重启前/后/稳定性三批带 v6 探针(N=3 中位数)落账 evidence §7。

## 夜间新发现(改变部分晨间议题的权重)

- **A. 「David」人设渗漏已实证为 live 生产面问题**:带 v6 prompt 的探针里,闲聊轮双峰——快样本(4.4~9.3s)全部滑向 workspace「David」人设且丢 display 块,慢样本(9~17s)才守「小伴」纪律。**质量与延迟耦合:v6 遵循 = 慢路径**。k2「人设注入质检」升为高优先(儿童生产会话可能收到「David」称呼+无 display 块答复)。
- **B. thinking=high 是活配置**(startup 日志逐字在档)——B 案(v4-flash 关/降 thinking)有具体抓手,Pod 步⑤ timebox 内裁 A/B。
- **C. 天气题 display 块 0/6**(两批全缺;昨晚 live 轮有块)——display 纪律在天气路径现配置已不稳;A2 门(100%)对现状同样红,定案时带此认知。
- **D. ttfc==total(28/28 样本)**:OpenClaw run 完才吐流——引擎无法提前 TTS 首句;优化标的 = 首轮 agent run 总时长(k2 结构调研:缓冲点在 agent loop 还是 openai-http 层)。
- **E. session store 529 且生产固有速率增长**(每轮 ~25k token 行)——k2 retention 议题实证 +1;重启后 error|warn 11 行未逐条归因(留 Pod)。

## 晨间决策点(architect §7 六项,按夜间数据更新)

1. **A3 真机轮**(你持板,Pod 落地 P1 后跑,会话早段):保底闲聊≤8s/课表≤10s,目标 5s/7s;三源取证。
2. **共 agent 回归**:QQ 录入 1 条测试安排 + 提醒 semantic.json 形状抽查(qqbot 与 voice 共 main agent,模型切换必然波及)。
3. **模型定案拍板**(P1 数据出来后):A(deepseek-chat)/B(v4-flash thinking off)批准为生产常态 + 成本注记(deepseek-chat input 2x/output 1.5x)+ architecture.md §2 依赖表与决策 19 型号名 docsSync 或 ARQ 二选一。
4. **P3 拍板**:ECS 4GB 升级(停机 resize 窗口)vs 过渡项(每夜 gateway 重启 cron + sessions 维护定时)——夜间数据:2GB 下 swap 常态 250~790MB、pressure 告警 5min 一次、同题 2 倍方差。
5. **k2 立项**(携当晚数据):静态上下文瘦身(~25k tokens/请求)/ **David 人设渗漏治理(升优先)** / session store retention / 课表 3 轮工具循环收敛(prompt v7,槽位预约先行)/ ttfc==total 流式结构调研 / per-channel 模型隔离 / openclaw/default 路由语义。
6. **OpenClaw 2.0.4(npm)升级窗口**:过 runbook §0 版本漂移闸(char-stroke dist 锚点),可与 P3 停机窗口合并。

## 簿记漂移(顺手报)

- board K7 行 status=blocked 而其 pod-events 有 merge-completed(2026-09-23T12:50Z)——人工路径合并未回写 board,待治理批或顺手修正。
- 昨晚 catalog 更新(22:43「remote model catalog updated; restart the Gateway to apply it」)已被本次 P0 重启应用;startup 显示模型/thinking 档未变,无需动作,留档。
