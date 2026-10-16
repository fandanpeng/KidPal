# mutation 取证 · 261016-sd-card-mount（builder 第一方实跑，2026-10-16）

> 执行者 = builder（agentId a2d04f5b767e1c014，第一方实跑）；planner 代存（builder 无 delivery/taskpackages/** 写权，派单预指派）。
> 每形：回退 → 独立冒烟通道确认对应齿红且靶位正确 → 还原复绿。

## m1 · 单宏值回退 D0 39→40（config.h L45）
- 红名 verbatim：`SD-A1 RED: config.h 未取到 #define SD_MMC_D0_GPIO 与 GPIO_NUM_39 同行宏定义——引脚宏面不完整（修复 = 补宏）`
- 靶位 = test 1 / SD-A1，与骨架 affectedTest 预判一致；SD-A2/A3/A4 保持 PASS；`4 Tests 1 Failures`
- 日志：`/c/tmp/sdm_mutation_m1_261016.log`；还原后复绿 ✓

## m2 · format_if_mount_failed→true（板 .cc L847）
- 红名 verbatim：`SD-A2 RED: InitializeSdCard 函数窗未取到 format_if_mount_failed = false——坏卡/异格式不销毁数据、只报错降级义务被破坏（对偶 InitializeArt 既有范式）`
- 靶位 = test 2 / SD-A2，与预判一致；`4 Tests 1 Failures`
- 日志：`/c/tmp/sdm_mutation_m2_261016.log`；还原后复绿 ✓

## m3 · WITHOUT_ABORT→裸 ESP_ERROR_CHECK（板 .cc L855）
- 红名 verbatim：`SD-A3 RED: InitializeSdCard 函数窗未取到 ESP_ERROR_CHECK_WITHOUT_ABORT——失败路径未走降级形态（修复 = 挂载/总线初始化错误路径用 WITHOUT_ABORT 承接, 不 panic）`
- 靶位 = test 3 / SD-A3（正确）；`4 Tests 1 Failures`
- **偏离注**：骨架举例预测 panic 计数齿红名，实际首发红 = WITHOUT_ABORT presence 齿——unity 短路，presence 齿（tooth 2）先于 panic 计数齿（tooth 3）触发；红仍落在正确 test 3/SD-A3 靶位
- 日志：`/c/tmp/sdm_mutation_m3_261016.log`；还原后复绿 ✓

## m4 · accessor 改常量 stub return true（板 .cc L870）
- 红名 verbatim：`SD-A4 RED: 板级查询入口 SdCardMounted() 未落地（waveshare-s3-rlcd-4.2.cc 无定义体读 s_sd_mounted 的 accessor）——防死代码误删观察锚缺席（本包零消费方, plan §2: 暴露板级查询入口供将来消费方判标志; 修复 = 定义 bool SdCardMounted() 读 s_sd_mounted 返回）`
- 靶位 = test 4 / SD-A4，与预判一致；`4 Tests 1 Failures`
- 日志：`/c/tmp/sdm_mutation_m4_261016.log`；还原后复绿 ✓

## 终态
- 全形还原后终复绿：`4 Tests 0 Failures`（`/c/tmp/sdm_builder_regreen_261016.log`）
- `grep -rn MUTATION` 于两实现文件 = NONE（无残留变异标记）
- 独立冒烟全绿 + host 套件 182 Tests 3 Failures（3 红 = p1_auxbar_style 上游漂移，与净 HEAD 同值，零新增红）
- A3 设备编译闸：`VERIFY_FW_BUILD_REQUIRED=1 node .claude/scripts/firmware-build-check.mjs` → `{"ok": true, "gate": "firmware-build", "phase": "build-pass"}`，exit 0（`/c/tmp/sdm_builder_fwbuild_261016.log`）
