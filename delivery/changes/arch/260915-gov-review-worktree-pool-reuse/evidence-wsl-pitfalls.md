# evidence-wsl-pitfalls · T5 两坑实测 verbatim（硬项 1a 留痕，2026-09-15）

> 产生于 260915 m1-home-qa W1 launch-prep 基线轮（主 checkout 仓根、git bash、WSL Ubuntu-24.04、ESP-IDF v5.5.1）。命令与输出逐字如下。

## 坑 1：`espidf_wsl.sh` 头注释 `-T main` 示例已失效（idf.py 无 `-T` 选项）

命令（照抄脚本头注释用法示例）：

```
wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal/firmware/main/test/host_app/espidf_wsl.sh -T main build
```

输出（verbatim）：

```
Try 'idf.py --help' for help.

Error: No such option: -T
```

修正用法 = 去掉 `-T main`：`espidf_wsl.sh build`（实测通过，见坑 2 成功形态）。

## 坑 2：git-bash 调 WSL 须 `MSYS_NO_PATHCONV=1`（否则 /mnt/c 被 MSYS 改写）

命令（未加前缀时的失败形态）：

```
wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal/firmware/main/test/host_app/espidf_wsl.sh build
```

输出（verbatim）：

```
bash: C:/Program Files/Git/mnt/c/AI/KidPal/firmware/main/test/host_app/espidf_wsl.sh: No such file or directory
/bin/bash: line 1: C:/Program Files/Git/mnt/c/AI/KidPal/firmware/main/test/host_app/build/host_infra_test.elf: No such file or directory
```

成功形态（verbatim，加 `MSYS_NO_PATHCONV=1` 后同命令）：

```
[33/34] Linking C static library esp-idf/main/libmain.a
[34/34] Linking CXX executable host_infra_test.elf
Done
```

ELF 全量跑 summary（同轮）：`129 Tests 4 Failures 0 Ignored`。
