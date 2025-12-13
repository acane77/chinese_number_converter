from chn_num_conv import *
import time

def conv(*args, **kwargs):
    return ChineseNumberConvertor(*args, **kwargs)()


if __name__ == '__main__':
    sentenses = [
        "给我买两三个榴莲",
        "给我买二十三个榴莲",
    ]
    for s in sentenses:
        s = conv(s)
        print(s)
