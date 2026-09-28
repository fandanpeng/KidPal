# Mutation Replay Evidence · 260928-story-connect-push-defer

> 落盘责任人 = planner（派发书硬约定③代落）。**第一方原始输出 = builder 源级重放报告（2026-09-28）**，原样收录；冻结期三段运行期实测 = acceptance-author（notes §10.3，不在此重复）。
> 冻结件基线：`server/acceptance/test_m4k1_story_cloud_link.acceptance.py` sha256 = `78c4e1eac267678c6b51e76550839ba70a0a637198e56c710c6fb1ae79658457`（交付形态，commit `73b5d3d`）。

## 1. 绿定义复跑（builder，仓根逐字）

```
193 passed, 4 deselected, 2 subtests passed in 2.38s
```

= FIX-A1 转绿 + 既有面零新增红（192→193 即 FIX-A1 一齿）。planner 独立复跑同结果（193 passed / 0 failed，2.25s）。

## 2. 源级 mutation 重放（builder 第一方，转绿后闭环）

变异 = 在 `ws_gateway` 连接处理段把建连即推调用加回（临时改动，逐字恢复原 try/except + log.warning，变异标记 `# MUT`）。

变异后实跑（目标口径 `-k "fix_a1 or a3_push"`）：

```
FAILED server\acceptance\test_m4k1_story_cloud_link.acceptance.py::M4K1ConnectPushDeferAcceptance::test_fix_a1_connect_path_emits_zero_story_list
1 failed, 1 passed, 25 deselected in 0.20s
```

全文件口径：

```
FAILED server\acceptance\test_m4k1_story_cloud_link.acceptance.py::M4K1ConnectPushDeferAcceptance::test_fix_a1_connect_path_emits_zero_story_list
1 failed, 26 passed in 0.44s
```

红名断言逐字：

```
E   AssertionError: 1 != 0 : FIX-A1 RED: 建连路径产生 1 帧 story_list 下行 —— 建连即推未摘除（human 方向锚 b036f554；①时机归 M4-K3 session_ctx 承接）。
server\acceptance\test_m4k1_story_cloud_link.acceptance.py:1002: AssertionError
```

变异仅 FIX-A1 转红；修订后 a3 齿（`test_a3_push_seam_and_wire_timepoints`）保绿——负锚与时机语义修订互不干扰，符合 FIX-A2 设计。

还原（删除临时加回块）→ 复绿：

```
193 passed, 4 deselected, 2 subtests passed in 2.38s
```

## 3. 冻结期三段实测（acceptance-author 第一方，notes §10.3 摘要）

(1) 摘除态模拟（caller==ws_gateway 跳过）→ GREEN（可满足性）；(2) 原变异（连接段同位注入恢复建连即推）→ RED；(3) 还原现树 → 设计态红（= counterEvidence）。源级重放（§2）为 builder 摘除后的真实闭环。

## 4. 终态自检（builder 三条 + planner 复核）

builder：`git status --short` 仅 `server/gateway/front/front_app.py`（+7/−6）；冻结件 sha256 == `78c4e1ea…58457`；全仓 grep 无 MUT/临时变异残余。另如实申报 5 条非本任务脏件（他会话残余），收口排除在外。

planner 复核（代落时点）：diff --stat = 1 file（+7/−6）✓；sha 一致 ✓；全套复跑 193 passed / 0 failed ✓。

## 5. 责任链

变异设计 + 冻结期实测：acceptance-author；摘除实现 + 源级重放：builder；代落与复核：planner。权威 = 各自第一方记录 + verifier 机械闸。
