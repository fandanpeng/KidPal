# red baseline · 260915-m1-k3-voice-round-ui（acceptance-author 现跑冻结记录，planner 转写）

## 命令（2026-09-16 实跑，仓根 cwd，git-bash）

```
MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf
```

## 结果 verbatim

`ninja: no work to do. … 18 Tests 0 Failures 0 Ignored` → `OK`，exit code 0。

- 与 intake 量测一致（n1b 期锚 13 + k2 新齿 5 = 18，本包有效基线；progress.md 基线锚段两面判停有差异 → 本面重跑，结果即本基线）。
- server 面：0 collected（`--ignore=server/acceptance/archive` 口径，现役面空态锚核一致）。
- **在册既存红清单 = 空**（零新增红基线，无豁免件）。k1a 期 4 条在册红已随归档批次移出全量跑面。

## 义务红（新齿接线后设计态红，非回归红）

- test 1 / test 2 / test 3 / test 5-tooth-1：接缝未落地时按设计红。
- test 4：对现状即绿（消费在世件的零回归守卫，非假绿）。

## C 类实编

`gcc -std=c11 -Wall -Wextra -fsyntax-only`（红基线 include 路径）→ error 0 / warning 0。

判读口径详见齿文件尾「VQ-A8 终跑判读」与「RED baseline 判读口径」段（firmware/main/test/voice_round_ui.acceptance.c）。
