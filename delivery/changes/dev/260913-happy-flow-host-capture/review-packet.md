# Review Packet · 260913-happy-flow-host-capture

> 补制收口制品（2026-10-09）。本变更 = human 一次性授权的主线程直做（跳过 dev-loop，
> Hard Rules 例外条款如实记录于 README），本件按直做实况做一次轻量复核记录，
> 不冒充 dev-loop supervisor/integrator 制品。

## 变更概要

| 项 | 值 |
|---|---|
| 性质 | host 整帧截图存档轮（验证结果记录，非功能变更） |
| 执行方式 | 主线程直做，human 一次性授权（2026-09-13） |
| 落地 commit | 96378a7（当日并入 main） |
| 代码面 | host_app 测试基建 4 文件（seam 落盘函数 / 新截图 test.c / CMakeLists 警告豁免 / 方法文档节）+ 截图存档 10 枚 |
| 风险 | 低——additive 测试基建 + 记录面制品，业务实现面零触碰，契约/spec 零触碰 |

## 复核记录（2026-10-09 补记时点逐项核对）

1. **Correctness**：截图驱动为普通 `.test.c`（非冻结验收件），env 门控 `KIDPAL_CAPTURE_DIR`，
   不设该 env 时纯验证渲染契约返回值、不落盘——对既有 host 齿面零干扰。实跑
   129 Tests / 3 Failures，3 红均为既有证据文件齿（VRT-A2 live / WSA-A4 flash.log /
   WSA-A6 eg 归档），改动前即红、非本变更引入（README + runner-run.md 在案）。
2. **Test quality**：不新增冻结断言；截图测试按 `HOST_ACCEPTANCE_SRCS` 逆序执行序落位，
   与冻结验收同源同路径渲染（P1-A2/P2-A3/P3-A2·A4·A5·A6/CHR-A4/RMD-A5 同源表在 README）。
3. **Contract/boundary safety**：`docs/contracts/**` 零触碰；requirements/architecture 零改
   （本补记时点复核 git 历史确认，commit 96378a7 文件清单仅测试基建 + 存档 + 当日旧位 INDEX）。
4. **Operability/security**：落盘目标为 env 指定目录（配方钉 `/tmp`，WSL 内），无网络面、
   无 secret；CMakeLists 仅 `-Wno-error=comment`（冻结件 REVISION LOG 路径字面量豁免）。
5. **Maintainability/simplicity**：方法已固化 HOST_TEST_CONVENTIONS.md「HOST CAPTURE」节
   （模板六条 + WSL 单条配方 + 三坑 + 字体现实），后续 pod 可复用，README 仅作首用记录。

## 已知未收口项（如实登记，非本变更义务）

- host 字体 = Source Han Sans SC 14 CJK 部分子集，次常用字为豆腐块；真机走 V3 字体资产，
  截图字形观感不代表真机（根因与候选修法固化于 HOST_TEST_CONVENTIONS.md「字体现实」节）。
- P4 笔画数据为合法合成闭合轮廓（「十」两笔），验证的是 D3 光栅管线而非字形美学。
- 语音轮五态演出内容面（RP-A9）host 无渲染面，不在截图清单。

## 回滚路径

- revert commit 96378a7 即整体回滚：seam 落盘函数与截图 test.c 为新增文件，CMakeLists
  警告豁免与 sdkconfig.defaults 字体选择随 revert 还原；存档目录随 revert 移除，无残留。
- 无设备侧依赖（纯 host 面），不涉烧录回滚。

## 状态

- 无 supervisor 闸（直做操作不进合并门）；human 授权即放行，落地当日已并入 main。
- 本件与 docs-delta.md 为 2026-10-09 INDEX 收口补落制品，事实以 README / runner-run.md /
  commit 96378a7 为准，本件不新增裁定。
