# Pod Retro · 260829-voice-roundtrip（方向 B 语音回环 + display 消费链路）

> planner 收尾相位补写 · 2026-09-07 · 先于合并（Track B 步骤 6 对偶 · closeout advisory 消费）
> 包结论：verify.mjs 九闸全绿（含 final bearing round r1）、六条 mutation 全数「变异→红→还原」、live 三轮真机实证、commit 727286c。

## 做得好的（保持）

1. **weak-symbol frozen offline contract（C 对偶 G 包 python 先例）**：验收头注钉死三契约函数签名 + weak 哨兵，builder 落强定义即真逻辑被测——mutation 探针（路由错置/降级直通）直接红在行为上，无假绿空间。此形态可直接复用到包 4/5/6/8。
2. **证据齿三态探针先例**（WSA-A4 口径）顺利复用：证据件「缺失即红/满足即绿/破坏载体即红」三态闭环。
3. **intake 用代码/契约事实裁决 launch 输入的矛盾断言**（tools 依赖）：fence 路径不依赖 tools 的裁定被后续冻结 harness 与 live 全程印证，避免了不必要的升级等待。
4. **live harness 的机械纪律**：conftest 把「live 不 auto-run」从自觉变机械闸；harness 内断言「请求不带 tools」把 intake 裁定固化进验收本体。

## 踩坑与摩擦（含可推广教训）

1. **gitignore 机器本地文件的 worktree 镜像陷阱（本包最大摩擦）**：`firmware/sdkconfig` 不入库，launch 镜像取自主 checkout 的过期副本（WS 包板型/证书 pinning 改动只在其 worktree 本地）→ 首烧错板型黑屏。修复 = 从 WS worktree 取正确副本。**教训：跨 worktree 的 gitignore 构建配置需要显式同步清单或 .gitattributes 化**；主 checkout 本地 sdkconfig 至今过期，后续包会重踩（已留根 workstream findings + friction fb2c70c9）。
2. **两套构建通路易混**：`idf_run.py build` = 设备 target（firmware/build）；host 齿只认 `firmware/main/test/host_app` 独立工程（espidf_wsl.sh）。本包 mutation 首探即因误用设备 build 跑了陈旧 ELF 假绿。**教训：host 齿运行命令应写死 host_app 通路并在 WSA/文档醒目处互斥标注**（HOST_TEST_CONVENTIONS.md 已载，但「名字像」的 idf_run.py 仍诱人踩）。
3. **WSL 执行三连坑实操解法**：CRLF（`tr -d '\r' < script | bash -s` stdin 通路）、MSYS 路径改写（`MSYS_NO_PATHCONV=1`）、退出码取 argv 直取形（`bash -c` 包装下 wsl.exe 恒 0 伪影）。三者每次都耗真时间，建议治理批把 stdin 通路写进 HOST_TEST_CONVENTIONS.md 作标准跑法。
4. **计数齿的注释不敏感**：VRT-A6 `AbortSpeaking(` 计数齿为纯文本子串扫描，注释式变异不红（注释行仍含 needle）、删除式才红。齿能红但粒度局限如实入 mutationEvidence notes。**教训：写阈值计数齿时注明「注释不豁免」语义或改 AST/结构化扫描**。
5. **live 探针的凭据依赖静默失败**：首跑 a4 统计 0/8 全空回复（loopback 401），harness 判「统计性失败 + 处置」而绿灯——探针通路问题被统计口径吸收。**教训：live 探针应先做一次 sanity 探针（如固定单字请求）再进统计循环**，通路红与遵循度红要分层。
6. **live 配置面路径陷阱再验证**：apply-xiaozhi-overlay.sh 默认渲染到 `main/xiaozhi-server/data/` 陈旧诱饵（真 mount = `deploy/xiaozhi-docker-data/`）——ops skill 已载此 trap，但 repo 脚本仍写错默认值。fixer 已修并 live 注入 prompt（16→87 行）。「所测 = 生产所用」自此闭环。
7. **verify 承重件缺位**：包内只跑过 scoped 轮，final bearing round fact 缺失，integrator 收口时补跑 r1 才过 verifier-evidence-check。**教训：planner 在 verify 全绿后应立即跑 `verify.mjs --evidence-mode=round --phase=final`，不等收口补**。

## 发现留根（跨包）

- **G 包 live relay 缺陷**（SHOULD #1）：引擎 display 切分 fence 未剥离混入播报文本 + kidpal/display tools/call 中转未发起——display text 视图面 live 未被触发（用户「翻不回上一页」即此）。归 `260903-gateway-directionB` live 集成修复；workstream findings + review packet 已载，建议 portfolio 决策处置形态（ARQ / 回包 / 缩圈包）。
- 本包是包 4/5/6/8 共享底座：view_router additive 注册表 + `server/deploy/openclaw-system-prompt.v1.md` 版本化扩展区已就位，后续包只 append。

## 流程观察

- AskUserQuestion 真机协作（烧录确认 / 冒烟三步 / KEY 复测 / display 轮）高效；human 在场是 hybrid 包的天然优势，四轮交互零等待浪费。
- 多 Pod 并行下的证据工件共享（从 WS worktree 复制 gitignore 工件）是安全的——同机真实产物、哈希可溯，比重跑烧录证据便宜且无伪造风险。
