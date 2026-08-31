# PROPOSAL · 260830-arch-review-ui-impl-strategy

> **状态：FROZEN · 2026-08-31 · R3 GREEN + human 立项 ack（2026-08-31「先landing吧」）；packet-binding 生效，landing 范围锁定为下方 landing-manifest 三路径**
> **承载 ARQ**：无（新起批次；踩坑记录 `docs/notes/ui-to-lvgl-pitfalls.md` 为输入材料而非根 ARQ）
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。本批不触 `docs/contracts/**` 直改，升级判据①②均不命中。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T4）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（spec 新增一节实现策略 + 一份策略文档，不改既有 story 行为语义；代码事实核验进 §5 验证集，不动实现面）。
> **review profile**：G2（触面 = `docs/spec.md` 一处新增节 + `docs/notes/` 新增一件 + `docs/ARCH-REVIEW-QUEUE.md` 追加登记；G1/契约面 = 零，契约仅只读引用）。

```landing-manifest
docs/spec.md
docs/notes/ui-implementation-strategy.md
docs/ARCH-REVIEW-QUEUE.md
```

---

## 0. 想实现的效果与差距

**目标效果**：把「KidPal 设备 UI 怎么实现」从一笔带过的一句话（spec §7 决策 17「沿用小智自带 LVGL，笔顺用 lv_canvas」）升级为一份经过设计稿逐元素核对、有事实依据、可执行的实现策略，并锚进 spec——后续 M4 开发按图索骥，不再凭印象踩坑。

**现状差距**：设计稿 v2（`docs/ui/kidpal-ui-design-v2.html`，已与 spec §4 story 锚点互链）定的是「手绘可爱风」：不规则圆角、倾斜、网点纹理、复杂吉祥物 SVG 姿态、110px 大字体。kimi for code 用「LVGL 原生控件手搓」路线在模拟器上做了 P1，human 实测判定「很烂、和设计差别很大」。模拟器现状（`tools/simulator/LvglWindowsSimulator/kidpal_ui.c`，431 行）与踩坑记录（`docs/notes/ui-to-lvgl-pitfalls.md` §5.2/§8）证实：该路线在吉祥物、手绘圆角、网点三处硬撞 LVGL 原生能力边界，且模拟器（LVGL 9.5）与固件（LVGL 9.3）版本错位，模拟器验证结论不能直接当真机结论。

**非目标**（本批不做）：不改任何 UI story 的功能语义与验收锚点；不改契约（笔顺 JSON D3 / display 块 E1 / 课表 MD B1 均原样）；不评估「换掉 LVGL / 换掉小智基座」这类推翻既有决策的方向（spec §7 决策 2/17 已定）。~~不写实现代码~~（按 human 2026-08-31 指示做了例外：在模拟器上以本方案实战开发 P1 验证可行性——成果与发现见 §8，模拟器实验代码属 `tools/` 面的一次性验证、landing 清单不含任何 `tools/` 文件）。

## 1. 问题与根因（地面真值）

### 1.1 设计稿的四个核心视觉语言，LVGL 原生控件三个半做不到

逐元素核对设计稿 v2 后对 LVGL 9.x 原生能力分类（依据 = 踩坑记录实测 + LVGL 9.3 源码在场核验，见下）：

| 设计元素 | 设计稿锚点 | LVGL 原生可行性 | 结论 |
|---|---|---|---|
| 大字体排版（时间 112px／标题 30／正文） | `canvas-p1` 等 | 位图字体子集可做到任意尺寸，能力满足；真机缺字体管线 | **可实现**（路线已定：小智同款 `lv_font_conv` → SPIFFS `.bin`，踩坑记录 §7.5） |
| 图标家族（48 盒 14 枚） | `sec-icons` | 小智同款 FontAwesome 式图标字，或 `LV_USE_IMAGEFONT`（本仓 LVGL 9.3 `src/others/imgfont/` 目录在场） | **可实现** |
| 三档网点纹理 d50/d25/d12 | `.d50/.d25/.d12`，4px 格 | 原生无图案填充；4px 格规则点阵做成 4×4 平铺贴图资产 | **需资产化**（同框族走资产管线，与 OQ-2(a) 口径一致） |
| 反白强调块（inv） | `.inv` | 原生 fill 直接做 | **可实现** |
| 布局、绝对定位、页点、键帽条 | `pgbuts`/`pgdots` 等 | 原生控件正常发挥区 | **可实现** |
| 米字格笔顺动画 | `canvas-p4` | 契约 D3 已定板端用 `lv_canvas` 折线逼近贝塞尔（spec §3.0.2） | **可实现**（契约已定，不在本批范围） |
| 吉祥物七姿态（Hello Kitty 风 SVG path，贝塞尔+旋转+组合） | `pose-daily` 等 7 个 symbol | 原生**不能**：v9 旋转的是整个矩形对象（踩坑记录 §5.2「旋转小耳朵会变成菱形」）；`lv_line` 不支持贝塞尔 | **需资产化** |
| 手绘不规则圆角（`border-radius: 255px 15px 225px 15px/...` 八值不对称） | `.frm/.frm-sm`，全站框族 | 原生**不能**：LVGL radius 是单值对称圆角，无不对称半径 | **需资产化** |
| 元素倾斜 ±1–2°（`tilt1/tilt2` 每屏 ≤3 处） | `.tilt*`，红线清单要求 | 原生**基本不能**：v9 支持旋转但位图旋转无抗锯齿，1bpp 屏边缘必成锯齿 | **需取舍**（OQ-1） |

**根因**：设计稿的表达介质是 CSS/SVG（任意贝塞尔、八值圆角、rotate transform），LVGL 原生控件的表达介质是「矩形 + 单值圆角 + 实心填充 + 标签」。两者之间差一个**栅格化层**。kimi 路线试图用手搓控件跨过这个层，结果只能产出「规整矩形堆叠」——这正是 human 看到「很烂」的直接原因，也是设计稿 §01 作废清单里「整齐划一的卡片阵列——像后台管理系统」所反对的东西本身。

### 1.2 模拟器与固件 LVGL 大版本错位，模拟器验证结论失真

- 模拟器：`lv_port_pc_visual_studio`，LVGL **v9.5.0**（踩坑记录 §1.1）。
- 固件：`firmware/main/idf_component.yml:43` 锁定 `lvgl/lvgl: ~9.3.0`，`firmware/dependencies.lock:701` 实测 `version: 9.3.0`（2026-08-30 起草期亲核）。

**根因**：模拟器工程是独立拷贝，未与固件组件锁对齐。9.3→9.5 之间样式/绘制 API 有演进，在 9.5 上验证的样式代码可能用了 9.3 没有的 API，或表现不一致。「模拟器很烂」里有多少是路线问题、多少是版本错位的假信号，现状不可分辨。

### 1.3 真机渲染是软件全屏绘制，RAM 与刷新预算约束资产方案的形状

- 真机驱动 `firmware/main/boards/waveshare-s3-rlcd-4.2/custom_lcd_display.cc:16-34`：flush 回调逐像素把 RGB565 阈值化成 1bpp 再 `RLCD_Display()` 推全屏——**没有硬件加速**，所有绘制都是 CPU 软件渲染。
- 同文件实测 PSRAM 分配（2026-08-31 复核 `:85-86`）：LVGL draw buffer 240,000 字节（单缓冲 `LV_DISPLAY_RENDER_MODE_PARTIAL` 整块 PSRAM 驻留）+ RLCD 帧缓冲 15,000 字节 + `PixelIndexLUT`（uint16_t）240,000 字节 + `PixelBitLUT`（uint8_t）120,000 字节，两 LUT 合计 360,000 字节。
- 固件 sdkconfig 实测：`CONFIG_LV_USE_VECTOR_GRAPHIC is not set`、`CONFIG_LV_USE_THORVG is not set`（`firmware/sdkconfig:2854,3087`）——ThorVG 矢量渲染真机默认关闭。

**根因**：在这块板上，「画复杂图形」的唯一低成本方式是**预栅格化成 1bpp 位图资产**（离线制作，运行时只是 blit），而不是运行时矢量绘制。这决定了吉祥物/手绘框的落地方向必然是资产管线，而不是「换一个运行时画法」。

### 1.4 既有资产先例：GIF 解码器已在树内

小智基座已带 GIF 支持：`firmware/main/display/lvgl_display/gif/gifdec.c`（otto-emoji-gif 组件链路，dependencies.lock 有 `txp666/otto-emoji-gif-component` 记录）。**根因推论**：「离线制作图资产 → SPIFFS → 运行时渲染」这条管道在基座里已经存在（字体 `.bin` + emoji GIF 都走它），UI 美术资产复用同一管道是最小新增复杂度路径，不需要新立运行时绘制子系统。

## 2. 改法（T1…Tn）

### T1 · 新增 `docs/notes/ui-implementation-strategy.md`（策略主体文档）

新增一件策略文档，承载本提案的完整分析，结构如下：

1. **设计稿元素可行性矩阵**（§1.1 表的完整版，逐元素带设计稿 `data-od-id` 锚点）。
2. **实现方案定论**：
   - **页面骨架走 LVGL 原生控件**：布局、标签、页点、键帽提示条、列表、反白块——这是 LVGL 的正常发挥区，保留 widget 语义（动态文本更新、页面切换、焦点）比全屏贴图省 RAM 也好维护。
   - **美术元素走 1bpp 资产管线**：吉祥物七姿态、手绘框族、三档网点（4×4 平铺贴图）、涂鸦点缀（星/云/波浪线）、印章，全部离线从设计稿 SVG 导出为 1bpp 位图资产，运行时以 `lv_image` 渲染（帧动画用多帧 `lv_image` 换帧或复用基座 GIF 链路，落地时二选一，倾向静态帧换图——GIF 解码在 1bpp 屏是浪费）。
   - **资产管线**：设计稿 HTML 内嵌 SVG 是唯一母版 → 脚本（`tools/` 下新建，进 dev-loop 包）抽取 symbol → 按目标尺寸栅格化 → **直出 RGB565 位图资产**（`.bin` 经 SPIFFS 加载；§8.2-V1 实战证明 I1 索引格式在 LVGL 9.5 软件渲染路径下调色板被绕过，RGB565 是双端行为一致的最稳格式，I1 仅作为真机空间优化项留待执行期在 9.3 上再验）→ SPIFFS 资源分区（与字体 `.bin`、笔顺 JSON、课表 MD 同分区策略，沿用踩坑记录 §7.5 定论）。模拟器侧同一批资产文件直接文件路径加载，**同资产双端渲染**，消灭「模拟器好看真机变样」的第三类失真。
   - **字体**：维持踩坑记录 §7.5 定论（模拟器 FreeType 快速迭代 / 真机 `lv_font_conv` PuHui 子集 `.bin`），补三条增量：数字大号字体（Baloo 2 ExtraBold，设计稿 §04 指定）按同管线单独出一个大尺寸子集；**大号数字与图案化文字（如首页大时间）首选 imgfont 图像字体或直接预渲染位图资产**（§8.2-V4：原生标签的行距/基线与相邻图形对齐是实战确认的高频痛点）；标题字体（站酷快乐体）需确认授权与栅格化效果，列入管线待验项。
3. **模拟器对齐措施**：模拟器 LVGL 降级/锁定到 9.3.x 与固件同版；`lv_conf.h` 以固件 sdkconfig 的 LVGL 配置为基准对齐（ThorVG 关、单色阈值一致）；**加一套「1bpp 滤镜」验证手段**——模拟器是 32bit 真彩渲染，与真机 1bpp 纯黑白观感有系统性差异（§8.2-V6：灰度抗锯齿让细线/小字在模拟器上显得比真机可用），至少把「画布快照 + 阈值化 PNG」做成一键自查；模拟器窗口固定 400×300（已做，踩坑记录 §1.2）。
4. **真机性能预算**：基于 §1.3 事实立预算线——动画区（眨眼冒号、声波条、笔顺步进）局部重绘，吉祥物/框族静态资产不参与高频 invalidation；资产总尺寸预算与 SPIFFS 分区表核对进 dev-loop 验收；RGB565 资产 Flash 占用按 2 字节/像素估算（实测 P1 全套 8 资产约 96KB，P1 大图时间字块占 55KB——大图资产须盯总量，I1 化或缩小栅格尺寸是执行期优化位）。
5. **分阶段路线**：①模拟器对齐 9.3 + 资产管线脚本跑通（吉祥物日常姿态一枚出图）→ ②P1 用「骨架+资产」重做，human 模拟器目测 → ③P2–P4 铺开 + 全局状态演出 → ④真机联调（SPIFFS 打包、前光下目测）。

目标文件：`docs/notes/ui-implementation-strategy.md`（新增）

### T2 · `docs/spec.md` §5 增订实现策略小节 + §7 决策 17 补注

在 spec.md §5（交互与 UI 设计）新增子节「§5.7 UI 实现策略」，内容为一段索引性定论 + 指向 T1 文档的链接：

> UI 实现采用「LVGL 原生骨架 + 预栅格化 1bpp 美术资产」双层方案：页面布局与动态文本走 LVGL 控件，吉祥物/手绘框/涂鸦等美术元素离线从设计稿 SVG 栅格化为 1bpp 资产经 SPIFFS 加载（与字体 .bin 同管道）。模拟器与固件 LVGL 版本必须对齐（当前锁 9.3.x）。完整可行性矩阵与资产管线见 [ui-implementation-strategy.md](notes/ui-implementation-strategy.md)。

§7 决策 17（spec.md:540 区域）追加半句补注：「——美术元素（吉祥物/手绘框/涂鸦）不走运行时绘制，走预栅格化 1bpp 资产（§5.7）。」

目标文件：`docs/spec.md`（改）

### T3 · `docs/ARCH-REVIEW-QUEUE.md` 追加登记留根项

以 append 方式登记两条（不硬治、只登记）：

- 模拟器 LVGL 版本与固件错位（9.5 vs 9.3）是常驻风险：每次固件升 LVGL 组件锁，模拟器须同步——建议在未来 simulator 相关包落地时把版本对齐做成构建期校验。**实战增量证据（§8.2-V1）**：9.5 软件渲染的 I1 索引图像路径与调色板语义和直觉/文档预期不符（`lv_draw_sw_blend_to_argb8888.c` 的 `i1_image_blend` 把位图当 alpha/灰度直接混色，调色板仅经 bin-decoder 对文件源走 `decode_indexed_line` 生效）——这类渲染行为差异只能按「模拟器结论不直接当真机结论」处理，双端同资产验证不可省。
- 模拟器（真彩灰度）与真机（1bpp 阈值化）的**显色差异**是第二类失真：同一张 UI 在模拟器上细线/小字可读，阈值化后可能糊掉（§8.2-V6）。建议 simulator 包顺手做「画布快照阈值化 PNG」一键自查，验收图一律以阈值化版本为准。
- P4 笔顺「未写笔画的虚线点链」设计稿用 dasharray 描边实现，与契约 D3 的 canvas 折线渲染之间需要一次视觉对齐核验（设计稿允许「自然淡网格」观感，契约未定虚线渲染细则），进 P4 dev-loop 包的验收注意项。

目标文件：`docs/ARCH-REVIEW-QUEUE.md`（追加）

### T4 · PROPOSAL 评审与 retro 制品

按 Track B 七步闭环：architect 评审至 GREEN → human 立项 ack + FROZEN → 主线程落地 T1–T3 → landing-verifier 独立核验 → retro。

目标文件：无新增仓内目标（制品 = 本目录下 landing-summary / landing-verdict / retro，随流程产生）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| spec 面 | `docs/spec.md` | G2 | 新增 §5.7 一节 + 决策 17 补注半句；不改任何 story 语义与验收锚点（锚点引用关系不变） |
| 文档面 | `docs/notes/ui-implementation-strategy.md` | 新增 | 策略主体文档，notes 层非 canonical 治理面，spec 以链接引用 |
| 队列面 | `docs/ARCH-REVIEW-QUEUE.md` | G3 | append-only 登记两条留根，不改既有行 |
| 契约面 | `docs/contracts/**` | 未触 | 仅只读引用 D3/E1/B1 现状；笔顺虚线渲染细则若将来要进契约，走独立契约门（T3 登记提示） |
| 实现面 | `firmware/**`、`server/**` | 未触 | 本批零代码改动；资产管线脚本与 P1 重做是后续 dev-loop 包 |
| 工具面 | `tools/simulator/**` | 未触 | 模拟器对齐措施写进策略文档，执行归后续 dev-loop 包 |
| 参考面 | `ESP32-S3-RLCD-4.2-Demo/**` | 未触 | 保持只读 |

## 4. human 取舍点（OQ）

> **human 裁定（2026-08-30）**：OQ-1/OQ-2/OQ-3 三项均按推荐位通过——OQ-1=(a) 倾斜烘进资产、OQ-2=(a) 框族 9 宫格资产化、OQ-3=(a) 结构+风格保真不追求像素级。

### OQ-1 · 倾斜（tilt ±1–2°）在真机的取舍【已裁 (a)】

设计稿把「微倾」列为可爱五抓手之一（每屏 ≤3 处），但 1bpp 屏旋转位图无抗锯齿，边缘必出锯齿。

- (a) **资产离线倾斜**（推荐）：栅格化时直接把倾斜烘进资产——离线渲染器旋转后阈值化，可以人工挑选观感最好的角度（如只取 1°/2° 两档），锯齿在离线阶段可见可控；运行时零成本。
- (b) 运行时 `lv_image_set_rotation`：灵活但锯齿不可控，且每次旋转重算，软件渲染下有 CPU 开销。
- (c) 砍掉倾斜：最稳但丢一个设计抓手。

推荐 (a)：倾斜是「风格资产」不是「动态行为」，烘进资产符合整个管线的形状。

### OQ-2 · 手绘不规则框的实现粒度【已裁 (a)】

- (a) **框族资产化**（推荐）：设计稿的框族（`.frm/.frm-sm` 圆角组合）离线渲染成 9 宫格可拉伸 1bpp 框图（draw_buf 切片），全站统一 3–4 种框型；运行时按尺寸拉伸，角部不变形。
- (b) 接受 LVGL 对称圆角 + 用网点/倾斜制造「手绘感」替代：零资产成本，但框就是规整圆角，「手绘框」这一语言降级。
- (c) 全屏背景图方案（每页一张 400×300 静态背景，动态元素叠上）：视觉保真度上限最高，但每页占 15KB 帧内存/Flash，且文本位置与背景耦合、改文案要重出图——维护代价大，仅作为 OQ-3 的对比项存在，不推荐。

推荐 (a)，框型数量有限（设计稿实际只用 2 种圆角组合），9 宫格资产约 4–6 张。

### OQ-3 · 保真度目标的校准基线【已裁 (a)】

「跟设计图比哪些能实现」需要一个 human 认可的判定基线：

- (a) **结构保真 + 风格保真，像素级不追求**（推荐）：布局、字号阶梯、黑白层次、吉祥物造型、网点观感 1:1；倾斜角、手绘抖动的具体形状允许资产化过程中的合理近似。验收方式 = human 对模拟器/真机目测过「可爱检验题」（设计稿 §01 两句验收题）。
- (b) 像素级对齐设计稿：需要全屏背景图路线（OQ-2c），RAM/Flash/维护代价换取浏览器渲染的完全复刻，不推荐。
- (c) 以「信息正确 + 不丑」为线，风格放飞：回到 kimi 路线的实际产出，human 已否。

## 5. 验证集（landing 前机械前置 + landing 后核验）

1. **锚点完整性自检**：`grep -c "data-od-id" docs/ui/kidpal-ui-design-v2.html` 起草本稿实测 = 设计稿锚点在场（起草期已逐一核对 §1.1 表内锚点存在）；landing 后 T1 文档中引用的每个 `data-od-id` 锚点须能在设计稿中 grep 到（机械命令：`for id in <T1引用锚点>; do grep -q "$id" docs/ui/kidpal-ui-design-v2.html; done`，预期全过）。
2. **事实断言复核**（landing 后 landing-verifier 重放）：
   - `grep -n "lvgl/lvgl" firmware/main/idf_component.yml` → 预期见 `~9.3.0`；
   - `grep -n "version: 9.3.0" firmware/dependencies.lock` → 预期在场；
   - `grep -n "CONFIG_LV_USE_THORVG is not set" firmware/sdkconfig` → 预期在场（行号 3087，行号漂移可接受、行须在）；
   - `ls firmware/managed_components/lvgl__lvgl/src/others/imgfont/` → 预期目录在场；
   - `ls firmware/main/display/lvgl_display/gif/gifdec.c` → 预期在场。
3. **spec 链接核验**：landing 后 `grep -n "ui-implementation-strategy.md" docs/spec.md` → 预期至少 1 处（§5.7）；`grep -n "§5.7\|5.7" docs/spec.md` → 预期新增节在场。
4. **真空绿规约**：以上均为「重放先断言输入非空」适用项——每条 grep 预期非空命中， verifier 记录原始输出。

## 6. 留根清单

- 模拟器↔固件 LVGL 版本对齐的**机械校验**（构建期比对）不进本批，登记 T3，待 simulator 相关 dev-loop 包顺手做。
- 资产管线脚本（SVG → 1bpp → .bin → SPIFFS）不进本批（本批只定策略），归后续 dev-loop TaskPackage，验收须含「同一资产模拟器/真机双端渲染」。
- 标题字体（站酷快乐体）授权确认与子集化效果验证，留执行期核验（设计稿 §04 注「免费商用」，落地前须核授权原文）。
- 本批不定「模拟器是否降级 9.3 的具体操作步骤」——策略里定为方向，操作细节归执行包。
- P4 笔顺虚线点链的视觉对齐核验登记 T3，若执行期确认需要契约字段则单独走契约门，本批不预批。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · architect 评审 · 2026-08-31 · 打回（1 项事实修正 + 2 项澄清，均不触目标效果）

**评审范围**：Track B 步骤 2  architect 评审循环第 1 轮。只读 canonical、只写本区。逐条复核 §1 地面真值机械断言、§1.1 可行性矩阵、§2 锚清单一致性、§3 sweep、§5 验证集、§4 OQ 裁定记录。

**A. 地面真值断言复核（抽查 10+ 条，要求 ≥3）——除下述 1 条外全部命中**

| 断言 | 复核结果 |
|---|---|
| `idf_component.yml:43` 锁 `lvgl/lvgl: ~9.3.0` | ✅ 逐字命中（`:43`） |
| `dependencies.lock` `version: 9.3.0` | ✅ 命中；实际行号 `:702`（PROPOSAL 写 `:701`），§5.2 已声明行号漂移可接受、行须在——成立 |
| sdkconfig `CONFIG_LV_USE_THORVG is not set` | ✅ 命中 `:3087` |
| sdkconfig `CONFIG_LV_USE_VECTOR_GRAPHIC is not set` | ✅ 命中 `:2854` |
| `src/others/imgfont/` 目录在场 | ✅ 在场（含 `lv_imgfont.c/.h`） |
| `gifdec.c` 在场 | ✅ 命中 |
| 模拟器 `kidpal_ui.c` 431 行 | ✅ `wc -l` = 431 |
| `custom_lcd_display.cc:16-34` flush 软件逐像素阈值化推全屏、无硬件加速 | ✅ Read 证实（`Lvgl_flush_cb` 逐像素 `RLCD_SetPixel` + `RLCD_Display()`） |
| LVGL draw buffer 240,000 字节 + RLCD 帧缓冲 15,000 字节 | ✅ 反算命中：`transfer=400*300=120,000`；draw buffer=2×120,000=240,000；`DisplayLen=transfer>>3=15,000` |
| otto-emoji-gif-component 链路（§1.4） | ✅ `dependencies.lock:710,802` 在场 |
| §1.1 设计稿锚点列（`canvas-p1`/`sec-icons`/`canvas-p4`/`pose-daily`，`.frm/.frm-sm`/`tilt1/tilt2`/`.d50/.d25/.d12`/`.inv`） | ✅ 全部 grep 到场；`data-od-id` 计 61 |

**B. §1.1 两条技术判断复核——均成立**

- 「手绘不规则圆角 LVGL 原生不能」：✅ 成立。grep LVGL 9.3 树内仅单值 `LV_STYLE_RADIUS`，无任何 per-corner / 非对称圆角 API（无 `radius_top`/`corner_radius` 等）。踩坑记录 §8 把「手绘不规则圆角（border-radius 非对称）实现策略」明确列为待补（未解决），佐证该判断有实测依据。
- 「倾斜运行时 1bpp 锯齿 / v9 旋转的是整个矩形对象」：✅ 成立。踩坑记录 §5.2 逐字证实「旋转的是整个带边框的矩形对象，不是三角形」。

**C. §2/§3/§5/§4 复核——一致**

- §2 T2：spec.md 现状 §5 最高编号为 `### 5.6`（`:488`），新增 §5.7 无编号冲突；决策 17 在 `:540` 与 PROPOSAL 引用一致。新增节内容（§5.7 引文 + 决策 17 补注半句 + T1 全文结构）已具体到可直接 packet-bound 落地。landing-manifest 三路径与 §2 T1–T3 目标文件一一对应，硬项 2a 成立。
- §3 sweep：7 面全分类，UNCLASSIFIED=0。契约面「未触」声明成立（本批只读引用 D3/E1/B1，不改契约；笔顺虚线细则显式留独立契约门，不预批——符合契约面纪律）。
- §5 验证集：每条均为机械可判 grep/ls 且附预期；§5.4 真空绿规约声明「重放先断言输入非空」适用——成立。
- §4 OQ：头部裁定行 + OQ-1/2/3 标题【已裁 (a)】标注齐全，与背景所述 human 裁定一致。

**打回项（均不触目标效果，无需升级 human 拍板，按 Track B 由主线程修订后复审）**

- **R1-1（事实修正，必修）**：§1.3「两张 LUT 各 120,000 字节」**不成立**。Read `custom_lcd_display.cc:85-86` 实测：`PixelIndexLUT` 类型为 `uint16_t(*)[300]`，分配 `transfer*sizeof(uint16_t)=120,000*2=`**240,000** 字节；`PixelBitLUT` 类型为 `uint8_t(*)[300]`，分配 `transfer*sizeof(uint8_t)=120,000*1=`**120,000** 字节。两 LUT 一为 240KB 一为 120KB（合计 360KB），并非「各 120KB」（合计 240KB）。这是 §1.3 标明「起草期亲核」的机械事实断言，且 T1 文档「真机性能预算」将继承这张 RAM 预算表——须改为「PixelIndexLUT 240,000 字节 + PixelBitLUT 120,000 字节（合计 360KB，较文中多占 120KB PSRAM）」。§1.3 的根因结论（预栅格化资产方向）不依赖 LUT 精确值，故不触目标效果；但预算基数须如实，否则后续 SPIFFS/RAM 预算线建立在偏低的数字上。
- **R1-2（澄清，建议）**：§1.3 断言「240,000 字节 LVGL draw buffer」对应 `LV_DISPLAY_RENDER_MODE_PARTIAL`（`:109`）单缓冲全屏尺寸。建议补半句说明「单 buffer、PARTIAL 模式、整块 PSRAM 驻留」——避免读者误以为双缓冲或按脏区分块。非强制，采纳与否主线程定。
- **R1-3（澄清，建议）**：§1.1 网点纹理判「近似可实现」，与吉祥物/手绘框判「需资产化」的口径略不一致（网点同样「原生无图案填充」）。建议在 T1 文档矩阵里统一表述：网点本质也走「贴图/canvas 资产」路径，与框族同一管线，避免读者把「近似可实现」误读为「原生控件直接做」。非强制。

**结论**：1 项事实修正（R1-1）+ 2 项可选澄清。R1-1 修订后本批可达 GREEN。三项均不触目标效果、不触契约面、不触 G1/G3，按 Track B 由主线程修订 PROPOSAL 后复审，无需升级 human。

### R2 · architect 复审 · 2026-08-31 · **GREEN**

**复审范围**：R1 三项打回项修订核验。只读 canonical、只写本区。

| 打回项 | 修订核验 | 结论 |
|---|---|---|
| R1-1（必修）LUT 字节数 | §1.3 `:56` 已改为「`PixelIndexLUT`（uint16_t）240,000 字节 + `PixelBitLUT`（uint8_t）120,000 字节，两 LUT 合计 360,000 字节」，依据标注 `:85-86` + 2026-08-31 复核。与 R1 实测（`transfer=120,000`，分别乘 `sizeof(uint16_t)=2` / `sizeof(uint8_t)=1`）逐字一致。 | ✅ 满足 |
| R1-2（可选）单缓冲说明 | §1.3 `:56` 已补「单缓冲 `LV_DISPLAY_RENDER_MODE_PARTIAL` 整块 PSRAM 驻留」。 | ✅ 满足 |
| R1-3（可选）网点口径 | §1.1 `:36` 网点行结论由「近似可实现」改为「**需资产化**（同框族走资产管线，与 OQ-2(a) 口径一致）」，可行性列同步改为「4×4 平铺贴图资产」；§2 T1 `:74` 资产管线枚举同步补入「三档网点（4×4 平铺贴图）」。两处口径一致。 | ✅ 满足 |

**一致性回扫**：状态行已更新 DRAFT v0.3（2026-08-31）；§3 sweep 分类、§5 验证集、§4 OQ 裁定记录、landing-manifest 三路径在修订后无联动破坏（网点仅措辞与枚举调整，未改目标文件锚清单与验证命令）。

**R2 结论：GREEN。** R1 全部打回项已满足，无新增打回项。本批达成 Track B 步骤 2「评审循环至 GREEN」。后续：主线程凭「human 立项 ack + 本 GREEN」标 FROZEN（步骤 3），再 packet-bound 落地 T1–T3（步骤 4）。

### R3 · architect 复审 · 2026-08-31 · **GREEN**

**复审范围**：R2 GREEN 后按 human 指示新增的实战验证节 §8 及其引发的本体回填（v0.3 → v0.4，见 §8.3 回填表）。只读 canonical 与 `tools/` 实验代码、只写本区。重点：①§8 可验证断言的机械复核（≥3 条）；②§8.3 回填是否越出「R2 GREEN 稿 + 实战增量」边界；③§8 性质声明是否合规。

**A. §8 可验证断言机械复核（抽查 5 条，要求 ≥3）——全部命中**

| 断言 | 复核结果 |
|---|---|
| `tools/asset-pipeline/gen_assets.mjs` 在场，含 flatten 修复（V2）与 time_10_24 文字资产 job（V4） | ✅ 在场（7134 B）。`:47` `.flatten({background:'#ffffff'})` 先于 `.greyscale()` 阈值化（V2 修法逐字命中，`:50` 注释解释「透明像素 RGB=0 会被阈值误判为黑」）；`:90` `jobs.push(['time_10_24', ...]` 文字资产 job 在场（V4）；`:124` 直出 `LV_COLOR_FORMAT_RGB565`（V1 落点） |
| `tools/simulator/LvglWindowsSimulator/assets/assets.c` 在场且 CF=RGB565（V1 结论落点） | ✅ 在场（484291 B ≈ 473KB）。8 个 `lv_image_dsc_t` 描述符全部 `.cf = LV_COLOR_FORMAT_RGB565`（`:1608/:1729/:2188/:2654/:2934/:7534/:7681/:7718`），含大时间字块 246×112（`:7534`，stride=492=2×246 与 2 字节/像素一致）。96KB 估算为「8 资产中 P1 使用的核心位图数据」量级口径，与 8 描述符 stride×h 反算相容；assets.c 总字节含 C 文本膨胀，不矛盾 |
| `kidpal_ui.c` 中 time_10_24 以 lv_image 渲染、眨眼冒号定时器在场 | ✅ 逐字命中。`:146` `img_at(parent, &time_10_24, 14, 42)`（`:57-58` `lv_image_create` + `lv_image_set_src`）；`:14/:25` `colon_blink_timer` + `:163` `lv_timer_create(colon_blink_timer, 30, NULL)`；`:269` `snapshot_timer`（V6/§8.4 阈值化快照自查）亦在场 |
| 本目录三件对比图 `p1-design-ref.png` / `p1-design-1bpp.png` / `p1-sim-canvas.png` 在场 | ✅ 三件均在场（26890 / 6152 / 8865 字节，2026-08-31 时间戳） |
| §8.2-V1 源码断言：`lv_draw_sw_blend_to_argb8888.c` 内 `i1_image_blend` 把位图当 alpha/灰度直接混色、调色板被绕过 | ✅ 逐字证实。`:402-491` `i1_image_blend` 全部分支均 `get_bit(src_buf_i1, src_x) * 255`（`:423/:438/:450/:463/:479`）写入 RGB 三通道或经 `lv_color_8_32_mix` 混色，**函数体内零 palette 引用**；`get_bit` 定义于 `:1310`。调色板查表仅存在于 `src/libs/bin_decoder/lv_bin_decoder.c`（`decode_indexed_line` `:72/:491`，`decode_indexed` `:578` 读 palette `:600`）——证实 V1「调色板仅 bin-decoder 对文件源走 `decode_indexed_line` 才生效、SW blend 直渲路径绕过」的源码级结论 |

**B. §8.3 回填表边界核验——未越界**

- **§8.3「未变」清单逐条核对在场且未被实战节破坏**：可行性矩阵（§1.1 `:30-42`）、OQ 三项裁定（§4 `:123` 裁定行 + `:125/:135/:143`【已裁 (a)】）、landing-manifest 三路径（`:10-14` 仅 `docs/spec.md` / `docs/notes/ui-implementation-strategy.md` / `docs/ARCH-REVIEW-QUEUE.md`，**不含任何 `tools/` 文件**）、验证集（§5 `:151-161`）、R2 GREEN 结论（§7 `:227`）——五者均在 v0.4 全文中保持完整。
- **T1/T3 修订仅为口径收紧 / 增量登记，未扩大 landing 范围**：T1 资产管线由「1bpp / lv_img_conv」改为「直出 RGB565 为基准、I1 留真机优化项」（格式口径收紧，目标文件未变）；T1 字体增 imgfont/预渲染资产一条（V4 增量）；T1 模拟器对齐增「1bpp 滤镜/阈值化快照」（V6 增量）；T1 性能预算增 RGB565 体积估算（V1 增量）；T3 增 I1 调色板行为差异、显色差异两条 append 登记（V1/V6）。全部落在既有三个目标文件的既有结构内，**未新增 landing 文件、未改 §2 锚清单路径、未改 §5 验证命令**。
- **§3 sweep 一致性保持**：实现面 `:117`、工具面 `:118` 仍标「未触」——与 §8 声明「`tools/` 实验代码属一次性验证、landing 清单不含任何 `tools/` 文件」自洽，无「实验代码偷渡进 landing」的矛盾。

**C. §8 性质声明合规性核验——合规**

- **一次性例外如实记录**：§8 引言 `:231` 明示「跳过 dev-loop 的例外操作，如实记录」，§0 非目标 `:24` 同步以删除线+改写记录「按 human 2026-08-31 指示做了例外」——符合 Hard Rules「唯一例外 = human 明确说跳过流程的一次性操作，且须诚实记录『跳过流程』」。两处记录一致、可归因（human 指示）。
- **tools/ 实验代码不进 landing-manifest**：landing-manifest `:10-14` 经 grep 核验确无 `tools/` 路径；§8 引言 `:231` 与 §8.4 `:269` 双重声明实验代码不进本批 landing。
- **不留「实验代码变事实标准」暗坑**：§8.4 `:269` 显式声明「landing 后建议由 simulator 包接管重写或正式化，避免实验代码沉淀为事实标准」——接管路径已点名（后续 simulator dev-loop 包），符合任务要求。

**结论：GREEN。** §8 五条抽查断言全部机械命中（含 V1 源码级断言逐字证实——调色板绕过论有 LVGL 9.5 树内源码兜底）；§8.3 回填未越界（五件「未变」项保持完整，T1/T3 仅口径收紧/增量登记，landing 三路径与验证集零破坏）；§8 性质声明合规（一次性例外如实归因、tools/ 隔离出 landing、simulator 包接管路径已点名）。无新增打回项。实战验证把 §1.1「需资产化」从推断变成已操作一遍的实证，根因分析（双层方案可交付设计稿风格）获正向证据——本批在 Track B 步骤 2 层面维持 GREEN，可进步骤 3（human 立项 ack + FROZEN）。

**附注（非打回项，主线程落地时参考）**：§8.2-V1 已正确把「I1 在真机 9.3 + 文件源路径下是否可用」标为执行期验证项——与本复核一致：V1 源码结论针对的是 9.5 SW blend 直渲路径，9.3 的 bin-decoder 文件源路径（SPIFFS `.bin`）行为须另行在真机验证，§2 T1 已按此把 I1 降级为「真机空间优化项留待执行期再验」。此项已在 PROPOSAL 内自洽闭环，无需修订。

## 8. 实战验证（2026-08-31，按 human 指示「开发 home 页看效果，问题总结进 proposal，实战后再 landing」）

> 性质说明：本节为 R2 GREEN 之后、human 立项 ack 之前，按 human 明确指示做的一次性实战验证——跳过 dev-loop 的例外操作，如实记录。验证产物（`tools/asset-pipeline/gen_assets.mjs`、`tools/simulator/LvglWindowsSimulator/kidpal_ui.c` 等 `tools/` 面改动）是实验代码，**不在本批 landing-manifest 内**；本节结论只用于修订策略文档口径（T1/T3 已按此回填）。

### 8.1 做了什么与效果判定

用本提案「LVGL 骨架 + 预栅格化资产」双层方案在模拟器上把 P1 首页从零重做了一遍，全链路跑通：

1. **资产管线**：`tools/asset-pipeline/gen_assets.mjs`（Node + sharp）从设计稿 HTML 抽取 SVG symbol/内联 SVG，按设计稿像素尺寸栅格化、倾斜角烘入（OQ-1a）、阈值化、直出 RGB565 LVGL C 数组——8 枚资产一次出齐：吉祥物日常姿态 98×98、涂鸦星/云、温度计小人、水滴宝宝、电池、铃铛、大时间数字块 246×112。
2. **骨架**：`kidpal_ui.c` 重写——状态栏、日期、接下来卡、反白胶囊、倒计时贴纸、虚线分隔、键帽条、页点全部原生控件；眨眼冒号用原生定时器动画（4.4s 周期眯眼 150ms，对齐设计稿 `blinkEye` 关键帧）。
3. **效果**（同尺寸对比图存于本目录 `p1-design-ref.png` / `p1-design-1bpp.png` / `p1-sim-canvas.png`）：布局结构、黑白层次、字号阶梯、吉祥物造型与设计稿基本一致，按 OQ-3(a)「结构+风格保真、像素级不追求」基线判定**达标**；与 kimi 全原生路线产出（human 判「很烂」）相比是质变——差异几乎全部来自资产化元素（吉祥物、大时间倾斜字块、涂鸦）。

### 8.2 实战发现的问题（全部为新事实，R2 评审时未知）

- **V1 · LVGL 9.5 软件渲染的 I1 索引图像绕过调色板（已解决，方案级影响）**：管线最初按 I1（1bpp 索引+内嵌调色板）出资产，结果全部反色/糊版。读源码定位：9.5 SW blend 路径 `lv_draw_sw_blend_to_argb8888.c` 的 `i1_image_blend` 把位图当 alpha/灰度直接 `get_bit*255` 混色，调色板只有 bin-decoder 对**文件源**走 `decode_indexed_line` 时才生效。**规避 = 管线直出 RGB565**（2 字节/像素、双端行为一致、无需任何转换代码）。代价是 Flash 占用 16 倍于 I1（P1 全套约 96KB，其中大时间字块 55KB）；I1 是否可在真机 9.3 + 文件源（SPIFFS `.bin`）路径下安全使用，留执行期验证——本提案策略口径已改为「RGB565 为基准，I1 仅作真机空间优化项」。
- **V2 · 透明背景 SVG 必须 flatten 再阈值化（管线必修项）**：libvips 对无底色 SVG 输出 RGBA 且透明像素 RGB=0，直接 `greyscale()` 阈值化会把所有透明区域判成黑——首版资产整版全黑。修法一行：`.flatten({background:'#ffffff'})` 先于阈值化。此坑对任何「SVG→单色位图」管线通用，已写进管线脚本注释。
- **V3 · libvips 不解析多层 `<use href>` 间接链**：设计稿的吉祥物是 `<use>`→`<symbol>`→`<use>`→`<defs>` 三层间接，libvips 渲染为空白。管线对策：抽取 symbol 主体直接内联（不嵌套 use），defs 随文档注入。同理 `<text>` 元素 libvips 可渲染（大时间字块即用此路径），但字体依赖运行环境——设计稿指定的 Baloo 2 在 PC 上以 Segoe UI Bold 替身出图，真机须以实际 Baloo 2 子集重烘，**同资产双端一致性的前提是管线环境装齐设计稿字体**。
- **V4 · 原生大字号标签与相邻图形的对齐是高频痛点**：大时间初版用 84px FreeType 原生标签 + 原生冒号块，行距/基线/字距三者叠加导致冒号始终对不成「眼睛」；最终按资产思路把整个「10  24」字块（含 -1° 倾斜）烘成一张位图，冒号保留原生动画——一次对齐、永久稳定。**推论进 T1**：大号数字/图案化文字首选 imgfont 或预渲染资产，原生标签只用于正文级动态文本。
- **V5 · 400×300 上「绝对定位 + 资产混排」的碰撞调优占了约一半工时**：涂鸦与电池、预告文字与吉祥物、贴纸与湿度读数三轮碰撞，全靠「快照 PNG → 人眼 → 改坐标」循环收敛。这不怪方案（设计稿本身就是绝对定位密排），但说明 P2–P4 铺开后需要把「设计稿坐标 → C 坐标」的转写做成管线的一部分（从 HTML style 直接抽坐标生成布局表），而不是手抄。
- **V6 · 模拟器真彩渲染与真机 1bpp 存在显色差异（验收口径影响）**：对比图制作时发现设计稿截图经阈值化后，「钢琴课/琴房 2」级别的 12–16px 文字明显糊边——模拟器（FreeType 灰度抗锯齿）上好看的细字，真机 1bpp 未必成立。已登记 T3；真机字体子集化时字号下限需实测（16px 以下 CJK 在 1bpp 的可读性）。

### 8.3 对 PROPOSAL 本体的回填（v0.4 相对 v0.3 的全部改动）

| 位置 | 改动 | 依据 |
|---|---|---|
| 状态行 | DRAFT v0.3 → v0.4，补实战验证状态 | 本节 |
| §0 非目标 | 「不写实现代码」改为如实记录的一次性例外 | human 指示 |
| §2 T1 资产管线 | 「1bpp / lv_img_conv」口径改为「直出 RGB565 为基准，I1 留真机优化项」 | V1 |
| §2 T1 字体 | 增「大号数字/图案化文字首选 imgfont/预渲染资产」 | V4 |
| §2 T1 模拟器对齐 | 增「1bpp 滤镜/阈值化快照」验证手段 | V6 |
| §2 T1 性能预算 | 增 RGB565 资产体积估算与总量盯防 | V1 |
| §2 T3 | 增 I1 调色板行为差异登记、显色差异登记两条 | V1/V6 |
| 本目录附件 | `p1-design-ref.png` / `p1-design-1bpp.png` / `p1-sim-canvas.png` 三件对比图 | §8.1 |

**未变**：可行性矩阵（§1.1）、OQ 三项裁定（§4）、landing-manifest 三路径、验证集（§5）、R2 GREEN 结论。实战验证**证实**了根因分析（双层方案可交付设计稿风格），并把「需资产化」从推断变成已操作一遍的实证。

### 8.4 已知残留（不阻塞 landing，进执行期注意项）

- 模拟器跑的是 LVGL 9.5，固件 9.3——本次用到的 API（`lv_image`、`lv_timer`、样式、FreeType、snapshot）在 9.3 全部在场，但 V1 这类渲染行为差异证明「9.5 验证通过 ≠ 9.3 无坑」，模拟器降级对齐（T1-3）在执行包里的优先级应提高。
- 大时间字块是「10:24」硬编码内容，动态时间须走 imgfont（数字 0–9 十枚图像字 + 冒号动画），执行期需验证 imgfont 在 9.3 的渲染质量（等宽数字表格线）。
- 模拟器实验代码（`kidpal_ui.c` / `gen_assets.mjs` / vcxproj 加 assets.c）未走 dev-loop，landing 后建议由 simulator 包接管重写或正式化，避免实验代码沉淀为事实标准。
