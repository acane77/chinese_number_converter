# 中文数字转阿拉伯数字

## 产生式

``` 
   Oku -> Man 亿 Man | Man
   Man -> Sen 万 Sen | Sen
   Sen -> NonZero 千 Hyaku | NonZero 千 零 Juu | Hyaku
   Hyaku -> NonZero 百 Juu | NonZero 百 零 Num |
   Juu -> NonZero 十 Num | 十 Num | Num
   Num -> NonZero | 零
   NonZero -> 一 | 二 | 三 | 四 | 五 | 六 | 七 | 八 | 九 | 两
```

## 使用方法

```python 
from chn_num_conv import *


if __name__ == '__main__':
    chars = [
        "小米十三Pro十六加一百二十八G要三千九百九十九元，比红米K六零Pro贵",
        "中国有十四亿一千七十七万八千七百二十四人，二万亿GDP",  # 1,411,778,724
        "一百万五千九百九十五英镑",
        "《一千零一夜》只卖三十五元",
        "红米十二一百二十八GB多少钱？",
        "他的电话是一三八一二三四五六七八",
        "今天是二零二三年十月三十一日",
        "刚刚清点了今天的收入，总共九万八千五百二十一点一零元",
        "他一个月的工资是三万两千元"
    ]
    for ch in chars:
        cc = ChineseNumberConvertor(ch)()
        print(cc)
```

输出：
``` 
小米13Pro16加128G要3999元，比红米K60Pro贵
中国有1410778724人，2000000000000GDP
1005995英镑
《1001夜》只卖35元
红米12 128GB多少钱？
他的电话是13812345678
今天是2023年10月31日
刚刚清点了今天的收入，总共98521.10元
他1个月的工资是32000元
```
