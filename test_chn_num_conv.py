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
    
    print("--- Chinese Test Cases ---")
    for ch in chars:
        cc = ChineseNumberConvertor(ch, language=Language.Chinese)()
        print(cc)

    jp_chars = [
        "百一", # 101
        "千一", # 1001
        "一億二千万", # 120,000,000
        "負百", # -100
        "マイナス百", # -100
        "ゼロ", # 0
        "今日は百一円を使いました",
        "私の戦闘力は五十三万です" # 530,000
    ]

    print("\n--- Japanese Test Cases ---")
    for ch in jp_chars:
        cc = ChineseNumberConvertor(ch, language=Language.Japanese)()
        print(f"{ch} -> {cc}")
