> 归位注记（acceptance-author, 2026-09-07）：本文件目标位置为
> `C:/AI/KidPal/delivery/taskpackages/260829-char-stroke/red-baseline-summary.md`
> （planner 已按包 2 先例授权产出该摘要），但 enforce-scope 拦截（acceptance-author
> 写边界 = firmware/server 的 acceptance 面 + tools）。故落边界内本路径（WSA 包
> red-baseline-summary.md 同目录先例；包专名防覆盖 WSA 同名件），请有写权方原样归位。

# CHR 红基线判定摘要（acceptance-author 2026-09-07 实测）

TaskPackage `260829-char-stroke`（查生字 P4 + 云侧 C1）冻结验收两文件的红基线执行记录：

- `firmware/main/test/char_stroke.acceptance.c`（C 侧: D3 消费/贝塞尔/变换/光栅/动画/速查卡/防御降级 + 结构齿）
- `server/acceptance/test_char_stroke.acceptance.py`（python 侧: 静态字库拼装 + LRU/预热 + prompt v2 + live）

## 归位注记（acceptance-author 被 enforce-scope 拦的三处，请有写权方落）

1. **host_app CMakeLists 接线**：`firmware/main/test/host_app/main/CMakeLists.txt` 被
   scope 拦（acceptance-author 只能写 `*.acceptance.c` / `acceptance/**`），接线属 builder
   （[C4.2]，WSA/包 2 同款先例）。builder 须向 `HOST_ACCEPTANCE_SRCS` 加两行：
   `"../../char_stroke.acceptance.c"` 与（实现落地后）`"../../../display/char_stroke.c"`，
   并加 include dir `target_include_directories(${COMPONENT_LIB} PRIVATE "../../../display")`
   （验收文件经 `__has_include("char_stroke.h")` 在实现头落地后自动切换到真实契约声明）。
   验收文件头 Run protocol 已写明。
2. **taskpackage 勘误**：`acceptanceTestsFile` 需补第二行
   `"server/acceptance/test_char_stroke.acceptance.py"`（python 侧新增承载；planner 指令
   授权本勘误但 scope 拦，零字段变更由 planner 落）。
3. **本摘要**：见顶部归位注记。

## C 侧红基线（命令与实跑）

闸上 runner = host_app linux-target ELF（[C4.4]），builder 接线后 verifier 复跑。本次按
包 2 先例用边界内脚手架（本目录 `unity.h` stub + `runner.c`，非冻结件）把**同一份冻结
验收文件**在 WSL Ubuntu-24.04 编成原生 Linux ELF 当场执行：

```
cd firmware/main/test/acceptance/red-baseline
gcc -std=c11 -Wall -Wextra -I. -o /tmp/csa_red_runner ../../char_stroke.acceptance.c runner.c
        # BUILD_CLEAN（error 0、warning 0）——T4 C 类冻结验收实编通道过
cd <repo root>（WSL cwd 继承仓库根）
/tmp/csa_red_runner            # 直取 argv 形态
```

实测：**10 Tests 9 Failures，ELF 退出码 = 9**（直取 argv 形态核实；`bash -c` 包装下
`echo $?` 恒 0 是 wsl.exe 参数重组伪影，VRT 摘要已记录同款）。

判读要点：行为齿经 weak-symbol frozen offline contract 真执行（9 个契约函数
`kidpal_char_stroke_parse / medians_parse / transform / raster_stroke / anim_state /
stroke_state / dash_dot_pitch / card_build / data_validate`，weak 默认哨兵 -2，断言先打
NOT-sentinel）——红 = 「frozen contract not implemented」缺行为红，非链接/编译红。

### 逐条判定

| TEST_CASE | 判定 | 红因（各自真实缺失行为, 2026-09-07 实测） |
|---|---|---|
| CHR-A1（查字页装配结构） | **RED** | firmware/main 无任何文件引用 `kidpal_char_stroke_parse(`——D3 消费/光栅面缺失；mcp_server.cc 无 case 3 接线（char view 仍落 default 降级）；齿 1-4 全缺。kidpal/display AddTool 到达面守卫绿（包 2 landed） |
| CHR-A2（D3 解析 M/Q/L/Z + medians） | **RED** | `kidpal_char_stroke_parse`/`kidpal_char_medians_parse` 未实现（weak 哨兵）——贝塞尔离散/骨架点消费缺失 |
| CHR-A3（坐标变换 1024→400×300） | **RED** | `kidpal_char_transform` 未实现——Y 翻转/偏上/屏内变换缺失 |
| CHR-A4（1bpp 光栅 + 逐笔叠加） | **RED** | `kidpal_char_raster_stroke` 未实现——scanline 填充缺失 |
| CHR-A5（动画状态机） | **RED** | `kidpal_char_anim_state` 等未实现——950ms 步进/歇 2s/三态/6px 点链缺失 |
| CHR-A5 structural（米字格 + 握笔小手） | **RED** | firmware/main 零「米字格」「握笔/penhand」标记——页面结构缺失 |
| CHR-A6（右栏速查卡） | **RED** | `kidpal_char_card_build` 未实现——共 N 画/第 X 笔·共 N 笔/写完啦装配缺失 |
| degrade（闸 A 包内裁定：缺字段降级） | **RED** | `kidpal_char_data_validate` 未实现——最小块防御降级门缺失 |
| CHR-A7（教学字体红线） | **RED** | `kidpal_char_raster_stroke` 0 文件引用（示范字光栅渲染路径不存在，需定义+调用 ≥2 文件）；tilt 负向齿空转、实现落地即咬合 |
| boundary/security + CHR-E3 | GREEN（守卫, 必须保持绿） | 扫描器四态自检 + 计数器通过；makemeahanzi/graphics.txt 零引用（数据集不入固件）；无 openclaw/18789 直连 |

## python 侧红基线（命令与实跑）

```
cd server && python -m pytest acceptance/test_char_stroke.acceptance.py -q --import-mode=importlib
```

实测（2026-09-07, Windows python + pytest）：**6 failed, 2 passed, 3 skipped**，收集零错误
（文件名两点须 importlib 模式，G 包先例）。**live 不 auto-run 已机械确认**：3 个 live 项被
`server/acceptance/conftest.py` skip（reason = 「live 不 auto-run (ARQ-260901-g3): 显式
RUN_LIVE=1 或 -m live 执行」），红基线未触 SSH/真 ECS。

### 逐条判定（offline 类）

| test | 判定 | 红因 |
|---|---|---|
| test_a8_char_lib_module_landed | **RED** | server/deploy/openclaw-patch/ 不存在——拼装契约模块（assemble_char / extract_minimal_char_block / replace_minimal_block）全缺 |
| test_a8_assembly_product_matches_source | **RED** | 同上（模块缺失 -> 缺行为红非 import 崩）；实现后断言逐字段对照 makemeahanzi 格式 fixture + 字符错行 ValueError + strokeCount 自洽 |
| test_a8_minimal_block_extract_and_replace | **RED** | 最小块提取/拼装替换语义缺失——「LLM 只出意图＋字」的机械承载不存在 |
| test_a9_lru_and_warmup_semantics | **RED** | CharLruCache/warmup_cache 缺失——LRU 命中/淘汰/晋升与预热 dedup 语义无处测 |
| test_e3_prompt_v2_landed_with_discipline | **RED** | openclaw-system-prompt.v2.md 不存在——禁复述 strokes/medians + 三句收紧措辞无处钉死；v1 共存守卫绿 |
| test_e3_prompt_v2_wired_into_deploy | **RED** | overlay 渲染面（apply-xiaozhi-overlay.sh）与声明面（provider.config.yaml）均无 v2 引用。**收紧记录**：首版齿扫全 deploy corpus 被声明文件 OPENCLAW_SYSTEM_PROMPT_CARRIER 的前瞻注记（「后缀演进：.v2 以 … 新落盘」）打出假绿，已收窄到渲染/声明两面（真接线 = overlay 渲染切换或声明面更新） |
| test_e3_gateway_thin_bridge_untouched | GREEN（守卫, 必须保持绿） | server/gateway/** 零 makemeahanzi/assemble_char/CharLruCache/数据集引用——薄桥红线（§1.5, 机制 A 否决理由）在世 |
| test_gate_a_display_schema_untouched | GREEN（守卫, 必须保持绿） | display_schema.py 可 import 且 VALID_VIEWS 七 view 白名单零漂移（闸 A 裁定：MVP 不动网关，缺字段降级归设备侧） |
| test_live_* ×3 | 不在红基线（@live, skip） | A8 = 拼装块对真 makemeahanzi 数据集逐字对照（ECS kidpal/chardata/，CHAR_DATA_DIR env 可覆盖）；A9 = char 端到端 5 秒档（CHAR_E2E_BUDGET_S 默认 8s 预算口径）；SSH 前置 |

## 红的构成一句话

**红 = C 侧 9 齿对当前 story 缺失行为成立（固件无 char 消费/光栅/动画/降级面、无 case 3
接线、无米字格结构）+ python 侧 6 齿成立（拼装契约模块、LRU/预热、prompt v2 落盘与
接线全缺）；C boundary 与 python 双守卫为 green。** 无一红因是 import/collection/编译
错误（C BUILD_CLEAN 退出码 9、pytest 收集零错误、断言全落在缺失行为上）。本包把它切到绿。

## 冻结契约给 builder 的输入（唯一被迫接受面）

- **C 侧**（建议落点 `firmware/main/display/char_stroke.(c|cc)` + `char_stroke.h`，声明须与
  验收文件 fallback 块逐字一致）：9 个函数签名 + 数值码（PHASE_WRITING=0/REST=1、
  STATE_WRITTEN=0/CURRENT=1/PENDING=2、屏幕 400×300、源 1024、950ms/2000ms/6px）+
  语义（贝塞尔 ≥2 内点/Q 段、Z 闭合、even-odd 填充、只置位叠加、Y 严格递减 + 域中心
  映射严格高于屏中点、progress 逐字格式「第 X 笔·共 N 笔」/「写完啦·共 N 笔」、validate
  必需键 = char/pinyin/radical/strokeCount/strokes/medians 六件）。全部写在
  `firmware/main/test/char_stroke.acceptance.c` 文件头。
- **python 侧**（建议落点 `server/deploy/openclaw-patch/`，模块名可自定、契约标记文本
  不得偏移）：`assemble_char / extract_minimal_char_block / replace_minimal_block /
  CharLruCache / warmup_cache` 语义见 `server/acceptance/test_char_stroke.acceptance.py`
  模块 docstring；prompt v2 落 `server/deploy/openclaw-system-prompt.v2.md` 并把
  overlay/声明面切到 v2。

## 降级与留痕记录（诚实边界）

1. **闸 runner 未接线**（scope 拦，见归位注记 1）：C 红以 stub runner 执行同一文件得出，
   断言行为与闸上一致；builder 接线 + 实现落地后 verifier 须在 host_app ELF 复跑
   （转绿期望 = 全 TEST_CASE PASS + EXIT=0 + 强符号覆盖 weak 哨兵）。
2. **LVGL 像素级观感殿后**（CHR-E2 口径）：本验收把像素断言钉在纯 1bpp 光栅位图层
   （scanline/叠加/挖洞），米字格线/三态/点链的屏上观感属包 9b device 验收；A5 structural
   齿只钉页面结构事实存在。
3. **live 面**：A8/A9 真连 ECS/OpenClaw + 真 makemeahanzi 数据集才有意义，标 @live 不
   auto-run；dataset 路径与预算经 env（CHAR_DATA_DIR / CHAR_E2E_BUDGET_S）可覆盖，live
   执行时若路径不符按红处置、不得以「环境不符」跳过对照。
