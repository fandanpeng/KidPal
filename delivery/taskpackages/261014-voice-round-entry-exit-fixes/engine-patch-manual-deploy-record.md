# engine-patch 手工部署记录 · 261014（TECH-261014-kidpal-speak-await-nonetype）

> 执行者 = 主线程；human 指示「engine-patch 手工部署一下，测试没问题了再推远端正规部署」。记录时间 2026-10-14 13:23 +0800。

## 结论

手工部署完成并核验通过；**引擎侧动态验证（提醒轮无 ERROR）待下一次真实提醒轮**——今日下一轮约 18:25（书法课前 15 分钟，weekly|3），亦为 VR-A8 联测第 3 项的一部分，同轮闭账。

## 前置对账（部署前）

- 目标位（manifest 条目）：`server/deploy/engine-patch/connection.py` → `/opt/kidpal/xiaozhi-esp32-server/deploy/engine-patch/connection.py`（bind-mount 覆盖容器 core/，restart: engine）。
- 远端现值：md5(raw) `6974aa84…` / 85991 字节 / mtime 2026-09-12 12:08；**归一（`tr -d '\r'`）md5 = `60c9925b…` == 仓内基线（`1885156` 版）**，逐行 diff 零差异——md5 差为 CRLF 伪影，即「远端 == 本包改动前基线」，覆盖安全（无未回流 live 热修）。
- 备份：`connection.py.bak-261014-pre`（-p 保元数据的远端原位副本）。

## 上传与校验

- 命令：`python C:/tmp/ecs_put.py C:/tmp/conn-new.py <远端绝对路径>`（exec_command + stdin，绕 SFTP chroot；`MSYS_NO_PATHCONV=1`）。
- 结果：`local=45013aba… remote=45013aba… bytes=84485 rc=0`——双侧一致（`45013aba…` = 仓内 `HEAD` 归一 md5）。
- 改动落地核对：远端第 390 行 = `speak_txt(self, text.strip())`（同步直调，无 await）。

## 重启与启动观测

- 命令：`docker restart xiaozhi-esp32-server`（引擎重启 = 设备全断一次，随即自动重连）。
- 启动日志：`初始化组件: llm成功 OpenClawOpenAI / intent成功 nointent / memory成功 nomem / vad成功 SileroVAD / asr成功 Qwen3ASRFlash`——与在册 live 值一致；零 ERROR / 零 Traceback。
- 补丁生效标记：`[core.connection]-INFO-kidpal display-strip patch active`——确认容器加载的是补丁版 connection.py。
- 端口面：engine `127.0.0.1:8010`(ws) + `127.0.0.1:8003` 监听、front `0.0.0.0:8000` 监听、`kidpal-front.service` active。

## 未做 / 待办

- **动态验证未完成**：需一次真实提醒轮触发 `_kidpal_try_speak`，届时引擎日志应无 `kidpal_speak 播报注入失败` ERROR 且 `语音生成成功` / `发送第一段语音` 照常（对照部署前三轮逐字 ERROR 的基线）。
- **远端正规部署未做**（human 明示：测试通过后再推远端正规部署）——本记录仅为 ECS 手工热修；仓内 diff 待 human 测试通过后随分支推送、走 deploy.sh 正规铺放（届时 md5 对账应直接通过）。
