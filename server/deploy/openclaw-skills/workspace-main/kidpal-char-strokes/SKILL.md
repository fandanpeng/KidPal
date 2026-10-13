---
name: "kidpal-char-strokes"
description: "孩子问「字怎么写/笔顺」时 (KidPal): 查 chardata JSONL 数据出笔顺答案+display char 指令。8-12岁。"
---

# KidPal 查生字笔顺（字怎么写）

孩子问「某字怎么写」「这个字几画」「笔顺是什么」时，查 `kidpal/chardata/` 数据，按顺序讲清笔顺，末尾附 `display` 屏幕指令（view: char）。char 的 `data` 只接受查字工具的完整结果：键 = C1 契约 `char/pinyin/radical/strokeCount/strokes/medians/structure/meaning/words`，**整条原样照抄，禁自生成/复述任何 strokes/medians 坐标**。

## 步骤

1. **先按字取 char_cache.json（拼装好的完整结果，首选）**
   `kidpal/chardata/char_cache.json` 是查字工具按字（key）存好的完整结果。Python：`json.loads` 后 `cache["字"]`（如 `cache["勇"]`），命中即查字完成——把整条 entry **原样**放进 display 的 `data`，不改写、不删减、不重排任何键或坐标。自检：`strokeCount` = `len(strokes)` = `len(medians)`。
   注：已生成的 `<字>_stroke_order.svg` 只是视觉参考；C1 契约键里没有 `svg` 路径或笔画名列表，别把它们塞进 `data`。

2. **cache 未命中才回退 makemeahanzi 源文件**
   两个文件都是 JSONL、行首 `{"character":...`：不要 `grep '^字'` 按前缀匹配，用 Python `json.loads` 逐行解析、匹配 `character` 字段。
   - `dictionary.txt`：取 `pinyin`、`radical`（部首）、`decomposition`（如 `⿱甬力`）、`matches` 长度（=总笔画数）。
   - `graphics.txt`：取 `medians`，数组顺序即书写顺序，按 C1 键拼装 entry。坐标只出自这两个文件，绝不自己编。

3. **笔画名称只进口语部分，别从 medians 几何反推**
   makemeahanzi 不提供笔画名称（点/横/竖/撇/捺/钩/折…），且坐标原点在左下、y 向上，肉眼从点列定名极易错。拿不准就 `web_search` 查权威笔顺站（如 bishun.hao86.com、bishun.fjnews.com 的「笔画顺序为：横撇/横钩、点、…」列表），多来源一致再用。查到的名称只用于口语，**不进** display `data`。

4. **口语回答（8–12 岁，≤3 句，先念出来）**
   顺序：字 + 拼音 + 总笔画数 + 逐笔名称 + 一句结构小提示。例：「勇，yǒng，9 画：横撇、点、竖、横折、横、横、竖、横折钩、撇。上面甬下面力，力就是力气，勇敢的人力气大」。写完逐句计数，超 3 句删减。

5. **末尾附 display 代码块（view: char）**
   数据形状示例（坐标从查字结果原样来，此处省略）：
   ```json
   {"view": "char", "data": {"char": "勇", "pinyin": "yǒng", "radical": "力", "strokeCount": 9, "strokes": ["…9 条 SVG path…"], "medians": [[[…]], "…9 段骨架点…"], "structure": "⿱甬力", "meaning": "brave, courageous, fierce", "words": []}}
   ```
   view 白名单 char/schedule/story_list/story_text/text/home/control；schedule 用 `data.items` 逐条 `{time, name, location}`（见 AGENTS.md）。旧写法 `stroke_count`+笔画名数组+`svg` 路径与 C1 契约不符，勿用。

## 验证

- 一致性：`strokeCount` = strokes 条数 = medians 段数 = 口语报的笔画数。
- 口语 ≤ 3 句且念全笔顺；display 块在回复末尾、view 合法、data 是查字工具完整结果的原样拷贝（无自编坐标）。
