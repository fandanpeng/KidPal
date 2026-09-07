"""server/acceptance conftest — live 验收不 auto-run 的机械闸.

ARQ-260901-g3-server-live-acceptance 纪律：@pytest.mark.live 的验收（SSH 到 ECS
/ 真机环境）在 merge 环境 **只做结构闸、不 auto-run**。此前该纪律靠运行命令自觉
（`-m "not live"`）维持；本 conftest 把它机械化——默认收集时给 live 项打 skip，
显式 `RUN_LIVE=1`（或 `VRT_RUN_LIVE=1`）环境变量时放行，`-m live` marker 过滤
不受影响（显式点名 = 有意执行）。

acceptance-author 2026-09-07（TaskPackage 260829-voice-roundtrip）：launch 输入
授权的「server/tests 收集通路最小调整（conftest/ini）」的落位形态；文件名两点
ModuleNotFoundError 另需 `--import-mode=importlib`（G 包先例）或
server/pytest.ini `addopts`。
"""

import os

import pytest


def pytest_collection_modifyitems(config, items):
    if os.environ.get("RUN_LIVE") == "1" or os.environ.get("VRT_RUN_LIVE") == "1":
        return
    skip_live = pytest.mark.skip(
        reason="live 不 auto-run (ARQ-260901-g3): 显式 RUN_LIVE=1 或 -m live 执行")
    for item in items:
        if "live" in item.keywords:
            item.add_marker(skip_live)
