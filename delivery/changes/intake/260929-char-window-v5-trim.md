---
changeId: 260929-char-window-v5-trim
sources:
  humanVerbatim:
    - "把这个装饰星 去掉，现在的实现里 没有组词，对应的是释义，所以把组词 改成释义"
    - "还有米子格 改成 正 圆角矩形了，实现 还是斜的，而且没有 米子，只是一个空斜框"
    - "我发当前的真机图片给你，而且 右侧的显示 有点被覆盖了"
  refs:
    - 真机照片（2026-09-28，樊字查字窗：右栏左缘被框压 / 空斜框无米字 / 底部星可见）
    - 设计母版 docs/ui/kidpal-ui-design-v5.html:990（rbox 圆角14px）+:254 注⑧
faceMap:
  requirements: 动——S-F3.2 AC1 五行去组词（requirements.md:505，story 标题:500+愿望句:502）；牵连同步 :68/:181/:460/:474/:556 + §8 已砍登记 + §9 v1.60；框形状无需改需求（:508 视觉归设计稿 v5，母版已先行）；装饰星/几何零需求面
  architecture: 不动——architecture.md 全文零镜像句（:211 只钉链路与 KEY 口径）
  contract: 不动（architect 四分类=无需改）——payload words 保留（interface-contract.md:59）；:63「可显示」容许性措辞 + v1.56 加释义行零契约改动先例；不触发逃生门
  implementation: 动——char_page.cc（①星 :486-524+:648-658 ②words join :676-718+:767-769 ③frm_236_236 斜框资产过时 + 引导线灰度阈值化不可见 :225-239 ④cardlabel x228 被框右缘 235 压）+ 验收件 char_window_overlay.acceptance.c（test 10 width 齿 / test 11 A3-A4 齿 / ACC-4 末端锚 :1915 依赖被删函数签名须换锚）
  governance: 不动——agent 定义/gate/settings 零命中
facePlan:
  - 顺序1 需求面（上游先）：requirement-change 档 1（architect 意见=档 1 足够；字面「牵连≥2 处升档」规则命中与否归 human 触点①裁）——story-author 起草 → requirement-reviewer 四查 → 主线程落稿 → 回填 anchors.req
  - 顺序2 实现面（下游后）：② 单包 Lite（riskTier=medium）→ delivery/taskpackages/260929-char-window-v5-trim/plan.md → planner /build-feature 完整 dev-loop
  - launchPrecondition: anchors.req 未回填前承载包不得 launch；实现包引用 story 定稿 v1.60
anchors:
  req: "requirements.md 定稿 v1.60（§9 v1.60 行 2026-09-29 = 定稿载体）：S-F3.2 标题/愿望句/AC1 收敛四行 + 五处枚举同步 + WF-F3-01 线框 + §8 已砍登记 + §9 v1.60 + 头部版本行；requirement-change 档 1 全程走毕（story-author 起草 → requirement-reviewer 四查全 PASS 放行）；spec-anchor-check ok:true（落稿后复跑 2026-09-29）"
  contract: "不动（architect 四分类=无需改，2026-09-29 分流 review）——payload words 保留、:63「可显示」容许性措辞 + v1.56 零契约改动先例"
  arch: "不动——architecture.md 零镜像句（分流 review 独立取证）"
  imp: "taskpackage 260929-char-window-v5-trim 建包（plan.md 落盘 2026-09-29，②单包 Lite riskTier=medium，reviewed plan 已按触点①裁定改写框方案=重烘资产案；taskpackage.json/origin 归 planner /build-feature intake 时建）"
openQ:
  - id: q1-米字引导线
    question: 圆角矩形框内是否保留米字（十字+双对角）引导线？
    ruling: human 2026-09-29 触点①裁——保留，改黑点线（点距/深浅归真机轮）
    status: resolved
  - id: q2-框方案取向
    question: 圆角框走程序化画框还是重烘 frm_236_236 资产？
    ruling: human 2026-09-29 触点①裁——重烘 frm_236_236 资产（不采 architect 推荐的程序化案；连带义务 = 资产管线轮 + app+art 双分区烧录，K4 教训入 plan）
    status: resolved
  - id: q3-设计母版协同
    question: 设计母版 v5 HTML 查字窗段仍画组词行(:1016)与星(:1018)——是否同批更新母版？
    ruling: 留收口阶段裁（非 blocking，实现包收口时随 ui-align 账面注记一并呈）
    status: deferred
  - id: q4-需求档位
    question: 需求面走档 1 还是档 2（牵连同步枚举 ≥2 处字面命中升档规则）？
    ruling: human 2026-09-29 触点①裁——档 1
    status: resolved
  - id: q5-立项ack路由
    question: 四档路由确认
    ruling: human 2026-09-29 触点①裁——确认 ② 单包 Lite（riskTier=medium）
    status: resolved
  - id: q6-真机烧录授权
    question: 真机轮 human 是否持板？
    ruling: human 2026-09-29 预授权离场——headless 开发转绿后由主线程代烧（spiffs 资产变更 = app+art 双分区烧录，flash-asset-partition-rule），目检五项（无星/四行/圆角框无锯齿/米字黑点线可见/右栏不被压）留 human 起床后真机确认，不构成 closeout 真机销账；若烧录异常（boot loop/黑屏）按 kidpal-flash-device 症状表处置并留痕。**代烧已执行（2026-09-29 凌晨，主线程）**：全量 idf_run.py flash（app @0x20000 + art @0x780000 + assets @0x800000 均写入且 Hash verified）；boot 观测 = 0 RE-ENUM / RX 50185B / uptime 16.9s→117.7s 平滑、HOME P1 渲染、RTC 回写正常——启动成功；目检五项待 human 确认
    status: resolved
  - id: q7-真机目检结果
    question: 真机轮目检五项结果（2026-09-29 human 确认）
    ruling: 四过一缺——无星 ✓ / 四行 ✓ / 圆角框无锯齿 ✓ / 右栏不被压 ✓；**米字黑点线仍不可见**。主线程根因定位 = charpage_blit 全幅逐像素覆写（ink?黑:白）吃掉先画的 canvas 域格点（char_page.cc :580 draw_grid → :584 render_frame→blit 顺序必然擦除；同雷 = PENDING 分支 :291 虚线点链 canvas 直绘同被擦）。修法方向 = 格点/点链挪 rast 1bpp 光栅域、blit 兜底一次成型。**修复已落地（q7 轮，headless 接棒 2026-09-29 晨）**：acceptance-author 先 re-anchor 齿（A4 负向齿 lv_canvas_set_px=0 + 正向齿 rast 引用）→ fixer 最小补丁 charpage_rast_ink helper + draw_grid/PENDING 点链入 rast 域（+35/-17）→ r4 十闸全绿 + host 107/0 → commit 02cb8bf；主线程复烧（全量 app+art Hash verified）+ boot 观测过（0 RE-ENUM / uptime 平滑 / RTC 正常）；真机米字线可见性待 human 再确认
    status: resolved
  - id: q8-跟着写帽标
    question: 框内左上「跟着写」黑帽标去留（human 2026-09-29：感觉大字显示位置偏下，帽标可去掉不要）
    ruling: human 2026-09-29 裁——摘除「跟着写」帽标（V5 对齐批加入项，human 为设计权威、推翻稿面）；主线程核实帽标不物理影响大字位置（独立叠层），「偏下」感若摘后仍在则下一刀 = 字在格内垂直居中调整（映射层，q9 预留）；设计母版 v5 同步随 q3 批。**修复已落地（q8 轮 2026-09-29 晨）**：验收件零齿钉 chip（全量 grep 核证）→ fixer 纯删 char_page.cc 21 行零新增 → host 107/0 零新红 + r5 八闸绿 → commit a730ee3；主线程复烧 boot 观测过（0 RE-ENUM / RTC 08:07 正常）；偏下感是否消解待 human 真机复看（仍在则开 q9）
    status: resolved
  - id: q9-大字垂直居中
    question: 大字在米字格内系统性偏下（q8 摘帽标后 human 复看仍在）——字位如何定/怎么修
    ruling: human 2026-09-29 先开修（bbox 平移居中方案已派单），**随即撤单（「等等，那你别做偏移了」）**——主线程即时停掉 headless 轮，探测阶段零改动落地（无 commit、固件源零 diff），HEAD 仍 90122c5。字位口径维持现状 = 权威字库 em 盒 1:1 直映、零拟合零平移（教学红线最忠实形态）；偏下为源数据墨迹分布使然，接受现状。后续若重提，从本条起案
    status: cancelled
  - id: q9-大字墨迹域映射
    question: 大字映射域假设 [0,1024] ≠ makemeahanzi 实测墨迹域（约 [-66,860]）——顶部恒 33~49px 留白 + 底部墨迹越界裁切（控制点包络上限 7~13px，实裁几像素，human 真机细看证实「一点点」）
    ruling: human 2026-09-29 裁「按真实墨迹域修」（先撤过一版纯平移居中案，本次重开改为域修正案）。方案 = 每字预解析期算墨迹 bbox（strokes 轮廓点 + medians 全点），**单一等比因子 s = min((size-1)/Δx, (size-1)/Δy)** fit 入格 + bbox 中心对齐格心（等比锁形不变胖瘦；bbox 含控制点天然留安全边）；替换现 charpage_map 的 0..1024 假设域直映；三态渲染路径与 medians 点链同用一套变换；越界 guard 保留。真机判据 = 勇底部钩不再削、顶部留白对称。**修复已落地（q9 轮 2026-09-29 上午）**：验收件零齿钉映射面（grep 核证，无需 re-anchor）→ fixer 单文件 char_page.cc（map_s 解析期算定 + bbox 居中 + 退化回退 + guard 保留；中间留平移案 ef87624、最终等比拟合 e70c63d）→ r7 八闸绿 + firmware-build 实编过 + host 107/0；主线程复烧 boot 观测过（0 RE-ENUM / RTC 09:02）；真机墨迹域效果待 human 确认。fixer 升级建议（不阻塞）：映射语义现无验收齿，是否补 guard case 待 human 裁
    status: resolved
  - id: q10-真机全项确认
    question: 查字窗收敛包真机全项销账（human 终确认）
    ruling: human 2026-09-29「可以了 commit」——q7 米字线 ✓ / q8 帽标摘除 ✓ / q9 墨迹域映射（底部不削、顶底对称）✓ + 此前四项（无星/四行/圆角框/右栏不被压）✓，真机轮全项过；closeout 真机销账成立；fixer 升级建议（映射语义补 guard 齿）human 未置评，留后续包通道不入本包
    status: resolved
  - id: q11-合并门处置
    question: 合并门 tier3（requirements.md 并行重叠消解）三条路如何走
    ruling: human 2026-09-29 裁「③ 暂缓」——不合并、不消解；包状态全部落盘可随时续；重开时从 planner 退出报告三选项（① architect 消解评审 / ② human 拍板并集 / ③ 暂缓）起案
    status: resolved
status: awaiting-human:merge-gate
---

# spine · 260929-char-window-v5-trim

查字窗视觉收敛四件：①去装饰星 ②去组词行（words 运行时恒空、实际呈现即释义，卡面收敛四行）③外框按设计母版 v5 改正圆角矩形（现真机=过时斜框资产+引导线阈值化不可见）④右栏让位（cardlabel x228 被框右缘 235 压）。

裁决来源：architect 分流 review（同会话 return，2026-09-29），faceMap/facePlan 取其报告 §1/§2 落盘。
