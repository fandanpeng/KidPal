# kidpal-mvp · progress

> 执行日志（追加式）：改了什么、测了什么、失败/修复。

- 2026-08-29 portfolio-planner 完成拆包设计，PROPOSAL FROZEN v0.2（human 盖冻结），落 11 个 TaskPackage。
- 2026-09-01 功能支撑验证批（260831 三包）收口并入 main，主线程对照 validation-batch-conclusion.md review 本 workstream，human 拍板三项调整（待落 PROPOSAL v0.3）：
  1. **C1 char 查询 109s 修法 = 服务端静态字库＋目标字表预热**（human 2026-09-01 拍板）。瓶颈定位：延迟随输出体量放大（text 4.9s / story_list 17.9s / char 109.1s），大头是 DeepSeek 把 strokes/medians 坐标 JSON 逐 token 复述；makemeahanzi 是静态字库，应确定性查表、不该走生成路径。修法：「汉字→D3 块」查表拼装挂 OpenClaw 侧（不入 shim，守契约 §1.5 薄桥红线），LLM 只出意图＋字；落地前先做 usage/stream profiling 定死延迟三段比例。纳入包 4（260829-char-stroke）云侧范围。契约 §1.6 system prompt 措辞疑似随之改 → 包 4 launch 前走 architect-reviewer 四分类预审。
  2. **C2 美术落地归属 = 新增「美术资产集成包」**（human 选 (a)），依赖包 0，B 段合入；V3 设计稿是其前置输入，OQ-3(a) 三项目测 human 已看过、观感通过。
  3. **轻量事实更新一并入 v0.3**：契约待确认点 1（display 遵循度 12/12）/ 2（课表 MD 解析 OC-11）已实证 → 包 2、6 的 contractBlocked 解除；待确认点 3（提醒 cron）未验 → 包 7 维持；包 S 不再建 shim、直接复用验证批 `server/shim.py`，只做设备侧闭环；包 1 起点改为现有 shim，launch 前先处置 ARQ-260901-shim-api-contract-gap；9b 补验 16px 以下 CJK 真机可读性＋前光。
- 2026-09-01 portfolio-planner 出 PROPOSAL-v0.3-draft.md（12 包：新增 U `260901-ui-art-integration`，mergeOrder 11，关键路径 7 级），待 human 冻结。
- 2026-09-01 **char 数据「设备端热字包」候选方案缓议**（human 拍：「先不动设备了，真机测试后看看效果再说」）：评估结论为技术可行（低年级字集打包 ~0.5–1MB，assets 分区余量 ~4MB 装得下；渲染路径本就在板端；可换毫秒级首帧＋离线回看），代价是触 spec §7 决策 1「数据全在 OpenClaw」原则（需按「预取缓存」定性）、与美术/字体共享分区余量、字集增长无轻量通道。**复议触发 = 真机测试后看 char 查询实际体验**；届时两级形态（设备热包＋服务端全库）方案全文在本条上文讨论中。v0.3 维持纯服务端字库。
- 2026-09-01 **PROPOSAL FROZEN v0.3**（human 盖冻结）：architect 单轮 dev-side review GREEN（10 条建议级修正 F1–F10 全回灌，无回问项），三项拍板 C1/C2/C3 落地。**关键发现并纠错**：v0.2 文头「11 包」是 off-by-one（盘上实有 12 个 260829-* taskpackage），v0.3 加 U 后实为 **13 包**——PROPOSAL.md/review.md 计数引用已全量校正为 13。冻结 pin = `sha1:500df090a227865e`（raw-content sha1，LF 归一化），钉入 13 包 `origin.upstreamFrozen` + decision-coverage.json。
- 2026-09-01 **冻结后机械落地完成**：① PROPOSAL.md 正文替换为 v0.3（状态 FROZEN v0.3）；② 13 包 pin 全量钉入；③ 同步 7 个 taskpackage 内容——S 缩域（复用 server/shim.py）、包 1 起点前移（+闸 B）、包 2/6 解除 contractBlocked、包 4 云侧扩 C1（+CHR-A8/A9 +闸 A + testHost→hybrid）、9b 补 HWV-A7/A8 + dependsOn 扩 U + mergeOrder 12、9a mergeOrder 13；④ 新建 U 包 `260901-ui-art-integration`（mergeOrder 11）；⑤ portfolio-reconcile 初始化 13 Pod（9b/9a/U 已更新，board 13 pods）；⑥ decision-coverage.json 12→13（新增 S-UI美术层承载 U、contractBlockedPackages 只剩包 7）。

## 2026-09-02 Pod S（260829-smoke-e2e）首板冒烟进展（planner 记）

- 冻结验收落盘（锚点修复路线实证有效）→ 结构面红基线 7 RED/3 GREEN 实测吻合 → builder 设备侧实现（shim 协议/mp3 解码/配网承载，编译通过）→ mutation 战役 4 条全敏感（A2/A3 证据回填）。
- 过程两缺陷均已修复并真机复验：① acceptance-coverage 门 Python 断言识别缺口（human 批准一次性热修）；② server/shim.py require_device 门用 return JSONResponse 不短路、实测穿透（fixer 改 raise HTTPException，ECS 复验 401/400/无转发）。
- 真板冒烟（无麦/喇叭，human 拍板先验其余面）：烧录✓、NVS 注入（wifi+shim_base+token 整片 0x9000）✓、AP 配网✓、联网✓；首板发现启动序激活门（api.tenclass.net 重试）阻塞协议初始化 → builder 编译期旁路（镜像级死代码消除，tenclass/xiaozhi.me 字符串归零）→ 重烧后 `Shim protocol selected, base=http://8.137.91.16:8080`。
- BOOT 会话实证：3s 静音 WAV → 公网 POST /v1/asr（token 鉴权通过，服务端日志源 IP 111.18.42.121）→ 200 → 空文本 → 屏显 `shim: empty asr text`（显示管线实证）。**设备↔shim↔(asr) 公网链路串通**。
- 验收现态：8 GREEN / 2 RED。余两红同因：roundtrip.json 需真实声学往返（麦克风+喇叭），待硬件到位。
- 暴露面未回收（冒烟仍需）：shim 0.0.0.0:8080 + 安全组放行，待末次冒烟后回收。

### 补记（同日稍后）
- 冻结验收 live 类首跑暴露 3 个线束缺陷（2 处 `_ssh` 三元解包写成二元、`_remote_write` 内联大 payload 撞 Windows 命令行上限）——均为「从未执行过」的冻结件缺陷，acceptance-author 修复（断言语义零变更）。
- 修复后复跑：**结构面 8 GREEN / 2 RED（两红 = roundtrip.json，硬件段）+ live 5/5 全绿**（含 401 硬闸与真实 edge-tts→asr→chat→mp3 往返）。至此，无声学硬件可测的面已全部测毕。
