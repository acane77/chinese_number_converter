from chn_num_conv import *
import time

def _time_stat(callable, *args, **kwargs):
    start_time = time.time()
    ret_val = callable(*args, **kwargs)
    end_time = time.time()
    print('[time] run time: {:.7f}s'.format(end_time - start_time))
    return ret_val

if __name__ == '__main__':
    chars = [
        "小米十三Pro十六加一百二十八G要三千九百九十九元，比红米K六零Pro贵",
        "中国有十四亿一千七十七万八千七百二十四人，二万亿GDP",  # 1,411,778,724
        "一百万五千九百九十五英镑",
        "《一千零一夜》只卖三十五元",
        "红米十二一百二十八GB多少钱？",
        "他的电话是一三八一二三四五六七八",
        "今天是二零二三年十月三十一日",
        "刚刚清点了今天的收入，总共九万八千五百二十一点一零元"
    ]
    for ch in chars:
        cc = _time_stat(ChineseNumberConvertor(ch))
        print(cc)