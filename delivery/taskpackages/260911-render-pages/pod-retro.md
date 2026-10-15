---
changeId: 260911-render-pages
date: 2026-09-12
author: planner
label: 第一人称热记忆（不复述机器事实——tokens/gates/rounds/verdict/test results 以 machine artifacts / pod-events / verifier-evidence 为权威）
---

# pod-retro · 260911-render-pages

## 顺的地方

- intake 一次做对是这轮最大的体感收益：architect 预审把 counterparts 十一条、双分支陷阱、注册序约束全部前置钉死，acceptance-author 和 builder 全程没有为「对端在哪」返工过一轮。M1 的「纸面推演、rebase 校正」活口这轮直接消掉了。
- 红基线「不重跑、引在册」的口径（human 2026-09-12 指示）省了整整一轮全量验证挂钟，判读全程没有歧义。
- 字体方案实测先行是对的：如果没把 cbin 拆开实测，「先试 cbin」会变成 builder 中途发现缺字的返工。一轮 AskUserQuestion 前的实测直接把 human 决策缩成确认题。

## 卡点与绕轮

- 三轮 scoped 红全是「marker/字段类」而非实现类：coverage map 缺 5 个 id、mutationSkipReason 缺字段、docs/notes 缺锚点。每轮都是几行的事但都要一个完整 verify 周期——这类「扩包后元数据没跟着扩」的债，起包 checklist 或许该加一条「acceptance[] 扩包后 coverage/suiteGate/mutation 三字段同步自检」。
- 两条冻结齿自冲突（test 6 互斥、F5 strstr 越窗）说明 acceptance-author 单人冻结多文件齿时缺一个「新齿与全部既有冻结件交叉自洽」的自检步。好在两条都是「不可满足齿」方向（收紧方向错），不是放松——但下次最好在冻结前就抓住。
- docs/notes 写权坑（launch prompt 预警过）第三次应验：anchors 只能主线程落。这个通道每次都靠人肉记住，或许值得机判化。

## 帮倒忙/别扭的

- `firmware/.gitignore *.bin` 与新字体资产天然相撞，每个包都要靠交接提醒 git add -f。字体资产已经有独立 fonts_root 闭集了，这条 ignore 或许该收窄。
- offline 解析 cbin 的 20 字节对齐步长与设备 loader 的 19 字节步长读法不一致（设备上能跑、离线拆不开），我最后靠「改试自然对齐」绕开——记录在 font-decision-s01 实测方法段，后来者不用再撞。

## escalation themes（cross-ref pod-events）

- 无 agent-escalation 事件——全程未触发升级桥，human 触点只有 S0.1 字体决策一次（ AskUserQuestion 两轮，第一轮是概念澄清、第二轮实裁）。
- loop-retro 阈值触达（15 ≥ 3）advisory 在案，发起责任归主线程（非本 Pod 内闭环动作，勿忘）。

## 真机轮 RP-A5 教训（2026-09-12 追加 · 四轮 fixer 收敛后）

- **① host runner 不编译设备面文件，host 绿 ≠ 能开机。** lcd_display.cc / kidpal_render_node.cc 这类设备面代码不在 host_app target 里，host 全绿对它们是结构性失明——r3 的 boot 死锁（`Board::GetInstance()` 在板卡构造期重入 `__cxa_guard`）编译期零信号，是烧录后才现形的。开机冒烟（烧录后走到 activating→idle 的 serial 一瞥）已立为必经步，不再等真机轮全量才撞墙。
- **② 冻结齿被注释文本糊绿 = 齿强度自检缺一环。** v3_restore 齿 6 以文本出现为满足，`schedule_page.c` 一段注释里恰好含关键词，齿就绿了——三字体零运行时接线这件事，齿面全程无信号，最后靠 human 肉眼「界面还是老的」戳破。教训：文本匹配齿要防「注释即满足」（注释豁免或结构锚定），冻结多文件齿时加一道「新齿会不会被注释/字符串偶然满足」的自检。
- **③ `Board::GetInstance()` 在板卡构造期重入 = boot 死锁，设备面代码取 display 一律自注册。** 函数内 static 单例在首次调用方尚未返回时重入，`__cxa_guard` 永久阻塞，boot 无声挂死（serial 停在 show_home 前一行，无 panic 无看门狗吠）。定例：render_node 等板卡构造期就会跑到的代码，取 display 走 SetupUI 自注册的 C 桥（`kidpal_lcd_voice_chrome_front` 先例），绝不回调单例。
- 另记：human 决策拆包（RP-A4 字体运行时接线拆后续包）是真机轮暴露齿强度缺陷后的干净处置——本包先收口、接线 + 真机腿后续包一并兑现，比在本包尾巴上追加改动稳。
