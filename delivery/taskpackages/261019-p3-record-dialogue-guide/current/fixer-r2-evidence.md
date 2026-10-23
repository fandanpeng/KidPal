# fixer R2 真机轮（第三轮）· 证据存档（2026-10-20）

> 承接主线程已对 ECS 日志核定的 F1/F2/F4 诊断。本轮只做 5 处精确编辑 + 验证，不改任何冻结验收
> （server/acceptance/**、firmware/main/test/**）与 docs/contracts/**，不 commit、不部署 ECS。
> 本文件由 fixer 拟稿、planner 代存（fixer 写权不含 delivery/taskpackages/**，回执原文转存）。

## 1. 五处编辑清单（最终行号）

| # | 文件:行 | 内容 | 状态 |
|---|---|---|---|
| 1 | server/gateway/front/front_app.py:796-810 | pending-stop 下行分支（feed_rec / finish_rec）演出先行序统一为 downlink → story_text → story_list | 完成 |
| 2 | server/gateway/front/front_app.py:913-922 | KEY 收尾（_story_stop_on_listen_stop 立即归档分支）补三式收尾统一下发 _issue_finish_downlink | 完成 |
| 3 | server/deploy/engine-patch/connection.py:1250-1254 | chat() 首行「大模型收到用户消息」日志在记录语境抑制时不再预打 | 完成 |
| 4 | firmware/main/mcp_server.cc:109-115 | ScheduleStoryRender lambda 内缓存同步改取执行期 g_story_*，不再回写入队时旧快照 | 完成 |
| 5 | firmware/main/mcp_server.cc:1498-1535 | story_list 路由区：基层缓存同步上提为 gating 前唯一写点，覆盖 gated 与前推两路 | 完成（按冻结计数钉调整形状，见 §2） |

## 2. 编辑 5 的形状调整（如实申报）

派单原文要求在前推 else 分支的 app.Schedule lambda 内新增第二处
kidpal_render_node_cache_story(&g_story_view, ...)。实跑发现这违反冻结结构断言
SLP-SLA4（firmware/main/test/story_page_v2.acceptance.c:2230）：路由区
（VRT_ROUTE_STORY_LIST 到 VRT_ROUTE_STORY_TEXT 之间）cache_story(&g_story_view 调用形
计数恒须为 1（K7-A5「唯一缓存写点」纪律），新增后计数 = 2，测试转红。

处理：不碰冻结测试，按冻结纪律改形状——把 gated 分支原有的缓存同步上提到 if/else 之前，
成为两路共用的唯一写点；前推分支不再另起调用。效果与 F4 意图一致（前推路径同样同步基层缓存，
且取 g_story_list 在本拍 view_init 之后的最新值），同时守住计数钉。首版 lambda 内新增写法
已撤销，未留残余调用。

## 3. 验证命令与逐字结果

### 3.1 Python 冻结套件（repo 根）

```
python -m pytest -q --import-mode=importlib -m "not live" \
  server/acceptance/test_record_dialogue_guide.acceptance.py \
  server/acceptance/test_m4k1_story_cloud_link.acceptance.py \
  server/acceptance/test_m4k3_record_session_live.acceptance.py \
  server/acceptance/test_engine_stop_truncation_asr_filter.acceptance.py
```
结果：68 passed, 27 subtests passed in 6.92s

```
python -m pytest -q --import-mode=importlib -m "not live" server/tests/test_story_units.py
```
结果：40 passed in 0.31s

### 3.2 固件 host 结构齿

```
MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- \
  /mnt/c/AI/KidPal-pod-m2/firmware/main/test/host_app/build/host_infra_test.elf
```
结果：219 Tests 3 Failures 0 Ignored

- 3 失败 = 存量红，非本轮引入：p1_auxbar_style.acceptance.c 的 AB-A1 / AB-A2 / AB-A4
  （04f8409a 直修后冻结锚未回钉；涉案 home_p1.c 等文件本轮零触碰）。同一存量红已在
  fixer-r1-evidence.md §1 登记（当时同为 219 Tests / 3 Failures）。
- 本轮相关冻结齿全绿：SLP-SLA4、key_layer_dispatch、story_page_v2 其余结构齿均 PASS。
- host runner 不链接 mcp_server.cc（结构齿运行时扫源文件），本轮未重编 elf 即完成扫描验证。

### 3.3 改动面

```
git diff --stat
 firmware/main/mcp_server.cc                        | 23 +++++++++++----
 server/deploy/engine-patch/connection.py           |  5 +++-
 server/gateway/front/front_app.py                  | 14 +++++++--
 delivery/taskpackages/261019-p3-record-dialogue-guide/pod-events.jsonl | 3 ++
 delivery/taskpackages/INDEX.md                     | 3 +-
 delivery/taskpackages/current/verdict.json         | 34 +++++++++++-----------
 6 files changed, 55 insertions(+), 27 deletions(-)
```

实现面改动恰为 3 个文件（front_app.py / connection.py / mcp_server.cc）。另外 3 个
delivery/** 文件是会话开始前工作树已有的未提交改动，本轮未触碰。

## 4. guard 覆盖说明

- 编辑 1 的收尾词路径（finish_rec）：既有 RDG-A6
  test_rdg_a6_finish_word_path_issues_start_organizing_and_exit_signal 承载
  start_organizing 恰一帧 + 出语境信号，已随本轮实跑保绿。
- 编辑 2 的 KEY 路径：RDG 冻结件头注 GAP-4（test_record_dialogue_guide.acceptance.py:192-195）
  明确申报「KEY 收尾 wire 半观察归真机轮 S5」，offline 不造假齿；按派单要求本轮不改
  server/acceptance/**，故 KEY 路径无新增离线 guard。既有 M4K1-A3 时点②真机序链
  （listen-stop → stt）继续断言 story_text / story_list 恰一帧，实跑保绿。
- 编辑 4/5 的缓存同步：SLP-SLA4 计数钉守住「唯一缓存写点」结构纪律；mcp_server.cc 行为
  本体 host 不可达（runner 不链接该 TU），前推路径行为半边归真机轮。
- 编辑 3 为日志可观测性修正，RDG-A4 守卫源锚齿保绿。

## 5. 显式申报

收尾路径 organize LLM 未接线——冻结验收 SAO-A1 钉死「归档正文 == 转写」，给收尾路径接
organize LLM 属语义变更，与冻结断言冲突。本轮不接线，该事项留 acceptance-author 通道
裁决（由主线程/planner 派发），fixer 不在本轮自行触碰。

## 6. 诊断对账补记（主线程 ECS 日志实证，2026-10-20）

- F2 前提反转：引擎日志 12:11:10「kidpal record-context: 进入记录语境（抑制 LLM）」在役，
  13 句叙述每句「大模型收到用户消息」后紧跟「抑制 LLM 派发（记录语境闭嘴回显）」= 守卫拦截
  即 return，零真派发。现场包「引擎零 kidpal_record_context 帧」为字面 grep 误读（日志实际
  文本 = 「kidpal record-context」带空格）。真实缺口 = 出语境信号全程未发（无「退出记录
  语境」行）——由编辑 2 承载修复。
- F4 根因与板端日志逐点闭合：135885 story_list_dispatch items=3 → 135955 盖章旧快照渲染
  （旧列表回写基层缓存）→ 136515 前推渲染（无基层同步）→ 247885 页重进渲出 vcount=2。
- 部署面注记（R3 前置）：front_app.py 需 front 服务重载；engine-patch/connection.py 需
  容器内 bind-mount 副本更新 + 容器重启（生产写入，human 门）。
