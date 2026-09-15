# Runner 实跑记录（2026-09-13）

## 命令与环境

- 构建：`wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build`（linux target，WSL Ubuntu-24.04，ESP-IDF v5.5.1）
- 运行：`cd firmware/main/test/host_app && env KIDPAL_CAPTURE_DIR=/tmp/hfc ./build/host_infra_test.elf`（WSL 内）
- 转换：WSL 内 python3 PIL，BMP → PNG，拷回 `changes/dev/260913-happy-flow-host-capture/capture/`

## 结果

```
129 Tests 3 Failures 0 Ignored
```

- **5 个截图 TEST_CASE 全 PASS**，10 枚 BMP 全落盘并转 PNG 归档（01–10）。
- **3 个 Failure 均为既有证据文件齿**（工作树本地证据文件缺失，gitignore 的本地产物不在本 pod
  worktree，与本次改动无关，改动前即红）：
  1. VRT-A2/E2 live 冒烟证据（voice_roundtrip 真机 live 证据文件）
  2. WSA-A4 device+live 证据（flash.log）
  3. WSA-A6 eg 归档（ecs-teardown-evidence.md）
- 进程退出码观测为 0（unity 汇总行 FAIL 但 ELF exit 0，与 [C4.4]「退出码=失败数」约定不符，
  疑为 runner 包装层行为，未追查——如实记录备查）。
- 末行 PASS：`schedule p2: P2 课表页消费面装配结构 (P2-A1)`。

## 截图轮定位

截图测试按 `HOST_ACCEPTANCE_SRCS` 逆序执行序跑在包 0 smoke（boundary 齿）之后、其余包之前
（接线于 smoke 条目之前），对既有齿无扰动；capture 文件自身 env 门控，默认全量跑不依赖截图目录。
