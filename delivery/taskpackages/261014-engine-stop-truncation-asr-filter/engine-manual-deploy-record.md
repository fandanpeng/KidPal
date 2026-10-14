# engine 手工部署记录 · 261014-engine-stop-truncation-asr-filter

> 执行者 = 主线程；human 口径沿用「先手工部署测试，通过后再推远端正规部署」。记录时间 2026-10-14 16:36 +0800。

## 结论

手工部署完成并核验通过；**真机复验已过**（human 2026-10-14 ~16:40 联测：查字轮「不客气」追加段消失、故事录音归档正常〔manual 豁免判定面〕、正常多句对话回归正常）。引擎侧动态判定面（引擎日志 stop 后无 `识别文本:`/无 `发送第一段语音`）由 18:25 提醒轮自动核验兜底（另有定时任务）。

## 前置对账

- 目标位：`server/deploy/engine-patch/connection.py` → `/opt/kidpal/xiaozhi-esp32-server/deploy/engine-patch/connection.py`（manifest 在册条目，restart: engine）。
- 部署前远端 = 误 await 修版（归一 md5 `45013aba…`）＝预期基线 ✓。
- 备份：`connection.py.bak-261014-pre2`（勿与 `.bak-261014-pre`〔误 await 修前回滚点〕混淆）。

## 上传与校验

- `ecs_put.py`（exec_command+stdin，`MSYS_NO_PATHCONV=1`）：`local=1cb1d131… remote=1cb1d131… bytes=90073 rc=0`（1cb1d131 = 合并后 main HEAD 的仓内版本归一 md5）。
- 落地核对：live 文件含 `_kidpal_on_listen_message` / `_kidpal_is_asr_hallucination` / stop-truncation 标识共 7 处命中。

## 重启与启动观测

- `docker restart xiaozhi-esp32-server`：组件初始化全绿（llm=OpenClawOpenAI / intent=nointent / memory=nomem / vad=SileroVAD / asr=Qwen3ASRFlash）、零 ERROR / 零 Traceback、8010+8003 双口监听。
- 设备侧：重启断连后未自愈重连（已知行为 TECH-261014-autoconnect-gives-up-silently），human 按 KEY 重建连后联测。

## 真机复验结果（human 持板，串口窗 live-261014-round3-enginefix.log）

1. 查字轮：介绍播完无追加寒暄、查字窗驻留 ✓（defect of record 消失）。
2. 故事录音回归：录音 → 讲述 → KEY 收尾 → 归档正常 ✓（manual 豁免承重判定面）。
3. 正常多句对话 + 连续追问 ✓。

## 回滚路径

还原 `.bak-261014-pre2` + restart + md5 复核；已知良好基线 = 部署前 live（误 await 修版）。
