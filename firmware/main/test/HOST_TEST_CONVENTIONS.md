# Host 测试目录约定（ESP-IDF linux target 单测）

> 本文件是 TaskPackage `260829-host-test-infra` 的 host 测试目录约定文档，承载 INF-A4
> 的 [C4.1]-[C4.5] 核验点，以及 INF-E2 的「host 可编译 vs device only」兼容矩阵。后续所有
> 纯逻辑包（2/3/4/5/6/8/U）在电脑上跑单元测试时，都照本文件的放置、命名、运行命令约定挂测试。

## 目录结构总览

```
firmware/main/test/
├── HOST_TEST_CONVENTIONS.md          # 本文件：约定 + [C4.1]-[C4.5] + INF-E2 矩阵
├── host_infra_smoke.acceptance.c     # 冻结验收文件（FROZEN，builder/verifier 只读）
└── host_app/                         # 独立的 host 单测 ESP-IDF 工程（linux target）
    ├── CMakeLists.txt                # 独立工程根：set(COMPONENTS main) 隔离设备组件树
    ├── sdkconfig.defaults            # 仅 linux/host 配置（CONFIG_IDF_TARGET_LINUX）
    ├── espidf_wsl.sh                 # WSL 运行入口（受支持 shell + 可跑 linux ELF）
    ├── espidf.ps1                    # Windows 本地探索用入口（不跑 ELF，仅配置/编译）
    └── main/
        ├── CMakeLists.txt            # 注册 runner + seam + 每个 *.acceptance.c
        ├── host_app_main.c           # linux 入口 app_main：自动跑全部 TEST_CASE，退出码=失败数
        ├── host_infra_seam.c         # 渲染缝 host_infra_render_frame / host_infra_read_pixel
        └── idf_component.yml         # 仅依赖 lvgl/lvgl（不依赖设备组件树）
```

[ C4.1 ]：本文件就是 host 测试目录约定文档，位于 `firmware/main/test/` 下；INF-E2 的
host-vs-device 编译矩阵落在「INF-E2 兼容矩阵」一节（同一文档）。

## [C4.2] 测试放置约定（TEST-CASE PLACEMENT）

- 所有 host 单测的**摩擦面测试文件**（含冻结验收 `*.acceptance.c`）放在 `firmware/main/test/`
  （本包即如此）。后续纯逻辑包若有多模块，可在 `firmware/main/test/` 下开 `/<subsystem>/`
  子目录（如 `schedule/`、`char/`），每个 `*.acceptance.c` 就近放。
- 每个 `*.acceptance.c` 必须被编译进 host 单测 runner：在 `firmware/main/test/host_app/main/
  CMakeLists.txt` 的 `HOST_ACCEPTANCE_SRCS` 列表里加一行相对路径，例如 `../../my_pkg.acceptance.c`。
- 一个普通 C 单测文件若只含 `TEST_CASE` 而无真实 `assert(...)`，会被 acceptance-coverage-check
  门误判「无可识别断言」转红；因此**任何新增 C 单测文件都必须带至少一个真实 `assert()` 调用**
  作为逻辑守卫（例如反射到被测逻辑的不变式），而不是可有可无的摆设。

## 冻结验收归档纪律（2026-09-15 起）

- MVP/验证期冻结验收与普通单测完成其交付使命后**归档不删除**：统一 `git mv` 进
  `firmware/main/test/acceptance/archive/`（逐件来源包与归档时点事实见该目录
  `README.md`），并从 `host_app/main/CMakeLists.txt` 的 `HOST_ACCEPTANCE_SRCS` 摘除，
  不再编入 host runner 每轮全量跑。
- 2026-09-15 两批归档（human 裁定，M1 阶段一已进入）：首批 13 件冻结验收（260829–
  260911 批），随归档自然消红 4 条在册既存红（SU-E3 / KR-A6 / WSA-A4 / WSA-A6，见
  `delivery/changes/dev/260915-m1-k1a-key-layer-dispatch/existing-red-inventory.md`
  R3–R6）；第二批 6 件普通 .test.c/.cpp 单测 + 仅被它们消费的 6 个被测实现。归档后
  host runner 现役 = key_layer_dispatch（M1-K1a 冻结验收）+ host_infra_smoke（包 0
  守卫）两件。
- 后续纪律：归档件不得重新接线；M1+ 各包新写冻结验收/单测照 [C4.2]/[C4.3] 放置接线，
  置于 smoke 之前；被改动行为需要回归守卫时由改动包 acceptance-author 新写钉点，不复活
  整份归档件；仅被归档测试消费的被测实现一并从 host 构建摘除（新包需要时随新验收接线
  回来）。

## [C4.3] 命名约定（NAMING CONVENTION）

- 后缀：冻结验收文件统一用 `*.acceptance.c`；普通单测文件用 `*.test.c`。本文件首个冻结样例即
  `host_infra_smoke.acceptance.c`。
- 标签：每个 `TEST_CASE` 用方括号标签 `[INF-x]`（宿主包验收编号）加功能标签，例如
  `TEST_CASE("host infra: ... (INF-A3)", "[host_infra][INF-A3][linux]")`。后续包沿用
  `[PKG-<id>]` 为其验收编号，功能标签（如 `[schedule]`）自定。
- 命名即文档：TEST_CASE 名称第一段中文/英文描述行为，括号内挂验收 id，让 `--test-filter`
  能按 id 精准跑。

## [C4.4] 运行命令约定（RUN COMMAND，最终形态）

linux target 产出的是**原生 Linux ELF，必须在 Linux 上运行**；本机 Git Bash(MSys/Mingw) 既非
ESP-IDF 受支持 shell、也无法运行 Linux ELF。因此 host 单测统一走已安装的 **WSL Ubuntu-24.04**
运行（受支持 shell + 能跑 ELF）。入口脚本 `firmware/main/test/host_app/espidf_wsl.sh` 会：切到
host_app 工程目录、用 WSL 侧 `~/.espressif/python_env/idf5.5_py3.12_env` 的 python 调 `idf.py`、
不 source export.sh（linux target 用 WSL 系统 gcc/cmake/ninja）。从仓库根执行：

```bash
# 0) 首次：配置 linux target（一次性；host_app 是独立工程，不碰设备芯片 sdkconfig）
wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh --preview set-target linux

# 1) 编译 host 单测 runner
wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build

# 2) 运行原生 runner；绿色 = exit 0 且报告全部 TEST_CASE pass
cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
```

期望输出（绿色，验收文件 3 个 TEST_CASE + seam_bounds.test.c 补充 1 个 = 4 Tests，2026-09-05 integrator 在 WSL 实测）：

```
Running host infra: seam read_pixel returns sentinel on out-of-range coords (supp)... PASS
Running host infra: render seam must distinguish nothing-rendered from a real frame (boundary)... PASS
Running host infra: LVGL SDL renders a real frame to virtual screen (INF-A3)... PASS
Running host infra: host unity runner executes sample logic (INF-A2)... PASS
-----------------------
4 Tests 0 Failures 0 Ignored
OK
（EXIT=0）
```

其中第 1 个 `seam_bounds.test.c`（普通 `.test.c`，后缀符合 [C4.3]）是 runner 挂载的补充单测，
不算冻结验收；验收文件 `host_infra_smoke.acceptance.c` 提供 INF-A2 / INF-A3 / boundary 三个
TEST_CASE。若后续包向 `HOST_ACCEPTANCE_SRCS` 新增测试，期望行数会相应增加——真正的绿判据是
「OK + EXIT=0 + 全部 TEST_CASE 均为 PASS 且至少含冻结验收三个样例断言」，不要按某种「固定 4
个测试」硬编码校验。

说明：`set-target linux` 只写 `host_app/sdkconfig`（独立工程），设备芯片工程的
`sdkconfig.defaults*` 完全不受影响。host_app 的 `sdkconfig.defaults` 只含 linux/host 相关配置。

### 关键 build 配置的机制说明（新包不改）

`firmware/main/test/host_app/main/CMakeLists.txt` 里这三点是让 host runner 非空洞的关键：

- `WHOLE_ARCHIVE`：unity 的 `TEST_CASE` 靠 constructor 自注册；若归属 .o 是静态库中未被引用的
  归档成员，链接器会丢弃它，`unity_run_all_tests()` 会报「0 Tests」。`WHOLE_ARCHIVE` 强制把
  每个 `*.acceptance.c.o` 拉进链接，runner 才真正注册并运行每个冻结 TEST_CASE。
- `REQUIRES unity lvgl`：把 IDF unity 帧和 LVGL 软件渲染器接入 runner。
- 渲染缝 `host_infra_render_frame()` / `host_infra_read_pixel()` 由 `host_infra_seam.c` 提供，
  是纯 LVGL 软件离屏渲染（无窗口/X11），headless 可跑；未渲染时探针返回哨兵 -1。

`sdkconfig.defaults` 两个关键项：

- `CONFIG_UNITY_ENABLE_IDF_TEST_RUNNER=y`：启用 IDF unity runner。
- `CONFIG_LV_COLOR_DEPTH_32=y`（LVGL 颜色深度是**布尔 choice**，不能写 `CONFIG_LV_COLOR_DEPTH=32`
  赋值，否则回退 RGB565）+ `CONFIG_LV_USE_CLIB_MALLOC/y`(+CLIB_STRING/SPRINTF)：32 位 ARGB8888
  让探针能精确比对 0xRRGGBB；CLIB 分配器让 LVGL 从 OS 堆分配约 480KB 的一帧缓冲（内置
  LV_MEM_SIZE 默认池不够，会 NULL 导致渲染缝 abort）。

## [C4.5] / INF-E2 兼容矩阵：host 可编译 vs device only

> 判定依据 = **实测**，不预判结论。host 侧由本次实际构建（linux target 独立工程）的
> component 集与 runner 运行结果佐证；device-only 侧由设备工程 `firmware/main/idf_component.yml`
> 的 target 规则（manifest 自身声明哪些依赖只对芯片 target 生效，linux 不在其中）+ host 构建
> component 集实测未见这些模块双重佐证。矩阵如实记录，若某模块后续有 linux port 则更新为 host。

### Host 可编译（实测通过，单测 runner 已链接并跑绿）

下面组件在 linux target 独立工程构建中被实际编译链接（见 host build `Components:` 行）并跑出
0 失败；纯逻辑与 LVGL 软渲染已在会话中实测：

| 模块/组件 | 类型 | 实测证据 |
|---|---|---|
| 纯逻辑样例（位反转/缓冲变换） | 自有代码 `*.acceptance.c` 内 | INF-A2 `TEST_CASE` 实际运行断言通过（3+ 断言） |
| LVGL 软渲染缝 `host_infra_seam.c` | 自有代码 | INF-A3 探针 6 像素 == 0x336699 通过 |
| runner `host_app_main.c` | 自有代码 | 全部 TEST_CASE 注册并跑、exit 0 |
| IDF core：esp_common esp_hw_support esp_rom esp_system esp_timer freertos hal heap soc spi_flash log linux | ESP-IDF 框架 | host build 组件集实测编译链接 |
| unity、lvgl/lvgl (9.3.0) | 组件 | host build 组件集实测编译链接 |

### 只 device（host 构建不包含 / manifest 无 linux 规则）

下列模块在设备工程 `firmware/main/idf_component.yml` 中通过芯片 target 规则排除 linux，且 host
单测工程显式不把它们纳入 `COMPONENTS`（`set(COMPONENTS main)` 隔离），因此**不能**在 linux
target 编译，只能上设备验证。矩阵如实反映「这些依赖声明的是芯片 target，linux 不在内」这一
manifest 事实，不臆测。

| 模块/依赖 | 硬绑定原因（manifest 规则/本质） |
|---|---|
| ESP-SR AFE 唤醒词（`esp-sr ~2.2.0` + `audio/wake_words/afe_wake_word.cc`） | esp-sr 预编译二进制仅芯片 target，无 linux rule；AFE 依赖具体芯片外设 |
| 各 LCD 面板驱动（sh8601/ili9341/gc9a01/st77916/axs15231b/st7701/...） | 多数带 `if: target in [esp32*, esp32p4,...]`；宿主渲染缝用 LVGL 软渲染替代真实面板 |
| LCD 触摸（ft5x06/gt911/cst9217/...） | 芯片 I2C 触摸芯片，无 linux rule |
| 音频编解码（es8311/es8374/es8388/es8389 + esp_codec_dev） | 依赖 I2C/I2S 硬件寄存器 |
| ML307 蜂窝模组（`78/esp-ml307`） | 芯片 UART 模组驱动，无 linux rule |
| 板级 BSP（`boards/<board>/`、esp_lcd_*_ev_board） | 绑定具体板卡引脚/触摸/屏 |
| Wi-Fi/蓝牙（esp_wifi、blufi provisioning） | 芯片 RF 外设，linux topic 无 |
| 设备版 OTA / NVS / 分区 | 芯片 flash/分区，host 无 flash 面 |

补充说明（诚实记录测量边界，不夸大）：host 侧每一列为**实测**（本次真实构建通过）；device 侧
列以组件 manifest 的 target 规则 + host 构建组件集实测双证据记录。包 0 未逐一对每个 device 模块
强行在 linux 下单独编译（那等于重搭设备树，超出本包范围），但「host 构建组件集里没有它们」这一
点已由本次 build 的 `Components:` 实测直接证明。

## Host 整帧截图办法（HOST CAPTURE，2026-09-13 起各 pod 通用）

> 动机与首用：human 一次性授权把 Happy Flow 各页在 host 真渲染后整帧落盘存档
> （`delivery/changes/dev/260913-happy-flow-host-capture/`）。本节把该办法固化成可复用配方：
> 任何 pod 想给「host 可渲染的页面/控件」留截图证据，照此办理，不必重新趟坑。
> 参考实现 = `firmware/main/test/acceptance/archive/happy_flow_capture.test.c`
> （2026-09-15 归档移位，配方仍有效；需要截图时按本节配方接线复活或照抄写新测试文件）。

### 能力面：渲染缝的落盘入口

- `int host_infra_dump_bmp(const char *path)`（`host_infra_seam.c`，additive）：把当前帧缓冲
  整帧写成 24-bit BMP（400×300，底行在前，BGR 字节序）。返回 0 成功；`-1` = 无渲染帧或
  path 为 NULL；`-2` = 落盘失败（fopen/写失败，errno 有值可供诊断）。
- 置 `rendered` 态的途径：`host_infra_render_frame()`（0x336699 整帧，兼做缝初始化）、
  `host_infra_render_p1_frame(argb,w,h)`、`art_render_blit_frame(argb,w,h)`（整帧覆盖）。
  **注意**：页面自己调 `lv_refr_now(NULL)` 触发的 flush **不**置 rendered 态（flush 回调不写
  该标志）——所以截图测试起手必须先调一次 `host_infra_render_frame()`，否则 dump 返回 -1。

### 写截图测试的模板（六条）

1. 整个文件包 `#if defined(CONFIG_IDF_TARGET_LINUX)` 守卫。
2. env 门控：`getenv("KIDPAL_CAPTURE_DIR")`，未设时只验证渲染契约返回值、不落盘——host runner
   的全量默认跑不被截图依赖绑架，CI 不挂。
3. 起手：`host_infra_render_frame()` 初始化缝并置 rendered → 把 CJK 字体设到 active screen
   （子对象继承，见「字体现实」节）→ 调页面契约渲染函数 → `lv_refr_now(NULL)` → dump。
4. 自建控件/大缓冲用 `static`（栈纪律）；布局类像素合成用
   `lv_draw_buf_create` + `lv_canvas_set_draw_buf`（LVGL 管对齐）。
5. 执行序纪律：runner 执行序 = `HOST_ACCEPTANCE_SRCS` 列表**逆序**（后注册先跑），smoke 保持
   列表末位（最先执行）。你的用例若跟在别的页面用例之后，起手 `lv_obj_clean(lv_screen_active())`
   + `lv_refr_now(NULL)` 对齐「页面渲染自带先清屏」的纪律，避免前一页残留混入截图。
6. 零残留齿避让：不要在测试文件里出现 `s_chat_overlay` / `s_page_pinned` /
   `s_current_page` / `s_chat_scr` / `s_voice_ui_scr` / `remind_before_min` /
   `port 18789` 等被树级扫描禁用的字符串；不要在测试文件里调用
   `kidpal_story_page_input` / `kidpal_story_on_saved` / `kidpal_story_rec_tick`（驱动
   view 状态用直接写字段）。`FILE_COUNT` 类齿是 min_n 下界，新增消费文件安全。

### 接线

在 `firmware/main/test/host_app/main/CMakeLists.txt` 的 `HOST_ACCEPTANCE_SRCS` 里加一行相对
路径（如 `"../../my_capture.test.c"`），置于 smoke 条目之前即可；执行序影响见上条 5。

### 跑起来：WSL 单条命令配方（含三个坑的解法）

```bash
# 编译（从仓库根）
wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build

# 跑 + 转换 + 归档，必须在**一条** wsl 调用里做完（坑 2）
wsl -d Ubuntu-24.04 -- bash -c 'rm -rf /tmp/cap && mkdir -p /tmp/cap \
  && cd /mnt/c/<repo>/firmware/main/test/host_app \
  && env KIDPAL_CAPTURE_DIR=/tmp/cap ./build/host_infra_test.elf \
  && python3 -c "from PIL import Image; import glob; \
       [Image.open(f).save(f[:-4]+chr(46)+chr(112)+chr(110)+chr(103)) \
        for f in sorted(glob.glob(chr(47)+chr(116)+chr(109)+chr(112)+chr(47)+chr(99)+chr(97)+chr(112)+chr(47)+chr(42)+chr(46)+chr(98)+chr(109)+chr(112)))]" \
  && cp /tmp/cap/*.png /mnt/c/<changes 目标目录>/'
```

三个坑（均已实测踩过）：

1. **ELF 的 fopen 不认 `/mnt/c/...` 绝对路径**（ESP-IDF linux target 的 libc/VFS 行为，ENOENT），
   相对路径与 WSL 原生路径正常 → 捕获目录用 `/tmp/...`，转换完 `cp` 回 `/mnt/c`（cp 是 WSL
   shell 做的，不受此限）。
2. **`/tmp` 在多条自动化 shell 调用间不保留**（每次调用独立命名空间）→ 跑、转换、拷贝必须在
   同一条命令里完成。
3. **某些自动化 shell 会吞掉 `$变量`/`*.bmp` 展开** → 配方里全用字面路径；python 侧用
   `chr()` 拼 `/*.bmp`、`.png` 是防展开被吞的保险写法。人在普通终端里可直接写 glob 字面量。
   （WSL 侧另需 `python3-pil`：`sudo apt install python3-pil`，或用 ffmpeg 逐张转。）

### 字体现实（读图须知，勿再踩坑）

- LVGL 9.3 的 Kconfig `LV_FONT_DEFAULT_*` choice 在 `lv_conf_internal.h` **无映射**（死配置），
  设了不生效。正确做法：运行时 `lv_obj_set_style_text_font(lv_screen_active(), &font, 0)`，
  子对象继承。
- 内置 SimSun 16 / Source Han Sans SC 14 CJK 都是**部分子集**（约 1000–3000 字）；琴/勇/钢/
  刺猬 等次常用字缺字 → 截图里是豆腐块。host_app `sdkconfig.defaults` 已选覆盖更大的
  Source Han 14（改后需删掉生成的 `sdkconfig` 才会重配生效）。这是**已知未收口项**：host 截图
  的字形永远不等于真机（Kconfig 无 `LV_USE_BINFONT`，设备 font_body.bin 的 GB2312 全量
  SPIFFS 加载器 host 不可复现）。
- 因此 host 截图的证据边界 = 版面几何、控件在位、资产落点、两态差异**真实可信**；字形观感
  **不代表真机**，真机字形终审归设备/模拟器轮。

### 已知边界与排错

- 整帧覆盖式 `art_render_blit_frame` 会擦掉 LVGL 控件——要「像素合成 + LVGL 控件」同帧，用
  `lv_canvas` + `lv_draw_buf_create`（实测可行）；裸 `lv_image_dsc_t` 指向自建 static 缓冲
  在 host 9.3.0 实测**不绘制**（无报错日志），勿用。
- 依赖 art 资产的页面先 `kidpal_art_mount("<repo>/firmware/main/assets/spiffs_root")`。
- 若 clean build 报冻结验收件 `-Werror=comment`（REVISION LOG 注释携带路径字面量触发）：
  已知历史问题，豁免已落在 `host_app/main/CMakeLists.txt`（2026-09-13，`-Wno-error=comment`，
  冻结件不动）。
- BMP 需转 PNG 存档（PIL 或 ffmpeg）；BMP 本身可直接被多数看图工具打开。