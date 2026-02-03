"""
Copyright 2023 Sisi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

"""
The BNF Grammar is list as follows:
   
   Negative -> 负 Oku | Oku
   Oku -> Man 亿 Man | Man
   Man -> Sen 万 Sen | Sen
   Sen -> NonZero 千 Hyaku | NonZero 千 零 Juu | Hyaku
   Hyaku -> NonZero 百 Juu | NonZero 百 零 Num |
   Juu -> NonZero 十 Num | 十 Num | Num
   Num -> NonZero | 零
   NonZero -> 一 | 二 | 三 | 四 | 五 | 六 | 七 | 八 | 九
"""

import re
from enum import Enum

class Language(Enum):
    Chinese = 0
    Japanese = 1

class ChineseNumberConvertor():
    # Constant for unit
    NUMBER_UNIT_TEN             = "Juu"
    NUMBER_UNIT_HUNDRED         = "Hyaku"
    NUMBER_UNIT_THOUSAND        = "Sen"
    NUMBER_UNIT_TEN_THOUSAND    = "Man"
    NUMBER_UNIT_HUNDRED_MILLION = "Oku"

    _NUMBER_UNIT_J = NUMBER_UNIT_TEN
    _NUMBER_UNIT_H = NUMBER_UNIT_HUNDRED
    _NUMBER_UNIT_S = NUMBER_UNIT_THOUSAND
    _NUMBER_UNIT_M = NUMBER_UNIT_TEN_THOUSAND
    _NUMBER_UNIT_O = NUMBER_UNIT_HUNDRED_MILLION

    CHAR_NEGATIVE = "负"

    def __init__(self, s, language=Language.Chinese):
        self.language = language
        self._init_num_dict()

        # Parser state
        self.i = -1
        self.s = s
        self.p = ""
        self.f = 0
        self._out = None

    def _init_num_dict(self):
        # Initialize number list
        self.chn_num = "零一二三四五六七八九"
        self.chn_num_upper = "零壹贰叁肆伍陆柒捌玖"
        self.chn_zero = [ "零" ]
        self.chn_pt = "点"
        
        if self.language == Language.Japanese:
             self.CHAR_NEGATIVE = "負"
             self.chn_zero.append("ゼーロ")
        else:
             self.CHAR_NEGATIVE = "负"

        self.chn_dict = {}
        for i, c in enumerate(self.chn_num):
            self.chn_dict[c] = i
        for i, c in enumerate(self.chn_num_upper):
            self.chn_dict[c] = i
        self.chn_num += "两"
        self.chn_dict["两"] = 2
        
        # Add Ze-Ro to dict
        if self.language == Language.Japanese:
             self.chn_dict["ゼーロ"] = 0

        self.unit_num = {}
        self.unit_num[self._NUMBER_UNIT_J] = ["十", "拾"]
        self.unit_num[self._NUMBER_UNIT_H] = ["百", "佰"]
        self.unit_num[self._NUMBER_UNIT_S] = ["千", "仟"]
        self.unit_num[self._NUMBER_UNIT_M] = ["万"]
        self.unit_num[self._NUMBER_UNIT_O] = ["亿"]
        
        if self.language == Language.Japanese:
             self.unit_num[self._NUMBER_UNIT_O] = ["億"]

    def _next(self):
        self.i += 1
        if self.i >= self.s.__len__():
            self.p = 'EOF'
            return 'EOF'
        self.p = self.s[self.i]
        
        
        # Handle multi-char token 'ゼーロ'
        if self.language == Language.Japanese and self.p == 'ゼ':
             if self.i + 2 < self.s.__len__() and self.s[self.i+1] == 'ー' and self.s[self.i+2] == 'ロ':
                  self.p = 'ゼーロ'
                  self.i += 2

        # Handle multi-char token 'マイナス'
        if self.language == Language.Japanese and self.p == 'マ':
             if self.i + 3 < self.s.__len__() and self.s[self.i+1] == 'イ' and self.s[self.i+2] == 'ナ' and self.s[self.i+3] == 'ス':
                  self.p = 'マイナス'
                  self.i += 3
        
        return self.p

    def _retract(self):
        if self.language == Language.Japanese and self.p == 'ゼーロ':
             self.i -= 3
        elif self.language == Language.Japanese and self.p == 'マイナス':
             self.i -= 4
        else:
             self.i -= 1
        self.p = self.s[self.i]
        # If we retracted back into 'ゼーロ' or 'マイナス', we need to restore p
        if self.language == Language.Japanese:
             if self.p == 'ロ' and self.i >= 2:
                  if self.s[self.i-1] == 'ー' and self.s[self.i-2] == 'ゼ':
                       self.p = 'ゼーロ'
             elif self.p == 'ス' and self.i >= 3:
                  if self.s[self.i-1] == 'ナ' and self.s[self.i-2] == 'イ' and self.s[self.i-3] == 'マ':
                       self.p = 'マイナス'
        return self.p

    def _save_pos(self):
        self.ii = self.i

    def _restore_pos(self):
        self.i = self.ii
        if self.i >= 0 and self.i < len(self.s):
             self.p = self.s[self.i]
             if self.language == Language.Japanese:
                  if self.p == 'ロ' and self.i >= 2:
                       if self.s[self.i-1] == 'ー' and self.s[self.i-2] == 'ゼ':
                            self.p = 'ゼーロ'
                  elif self.p == 'ス' and self.i >= 3:
                       if self.s[self.i-1] == 'ナ' and self.s[self.i-2] == 'イ' and self.s[self.i-3] == 'マ':
                            self.p = 'マイナス'
        else:
             self.p = 'EOF'

    def _N(self, use_f=False):
        if self.p in self.chn_num or (self.language == Language.Japanese and self.p == 'ゼーロ'):
            n = self.chn_dict[self.p]
            self._next()
            return n
        else:
            return -1

    def _J(self):
        n = self._N()
        if self.p in self.unit_num[self._NUMBER_UNIT_J]:
            self.f = 10
            self._next()
            m = self._N(use_f=True)
            return max(1, n) * 10 + max(0, m)
        return max(0, n)

    def _H(self):
        n = self._N()
        if self.p in self.unit_num[self._NUMBER_UNIT_H]:
            self.f = 100
            self._next()
            if self.language == Language.Chinese and self.p in self.chn_zero:
                self.f = 10
                self._next()
                m = self._N()
            else:
                m = self._J()
            return max(1, n) * 100 + max(0, m)
        if n >= 0:
            self._retract()
        return self._J()

    def _S(self):
        n = self._N()
        if self.p in self.unit_num[self._NUMBER_UNIT_S]:
            self.f = 1000
            self._next()
            if self.language == Language.Chinese and self.p in self.chn_zero:
                self.f = 10
                self._next()
                m = self._J()
            else:
                m = self._H()
            return max(1, n) * 1000 + m
        if n >= 0:
            self._retract()
        return self._H()

    def _M(self):
        n = self._S()
        if self.p in self.unit_num[self._NUMBER_UNIT_M]:
            self.f = 1e4
            self._next()
            if self.language == Language.Chinese and self.p in self.chn_zero:
                self.f = 10
                self._next()
            m = self._S()
            return n * 10000 + m
        return n

    def _O(self):
        n = self._M()
        if self.p in self.unit_num[self._NUMBER_UNIT_O]:
            self.f = 1e8
            self._next()
            if self.language == Language.Chinese and self.p in self.chn_zero:
                self.f = 10
                self._next()
            m = self._M()
            return n * 1e8 + m
        return n

    def _NE(self):
        factor = 1
        if self.p == self.CHAR_NEGATIVE or (self.language == Language.Japanese and self.p == 'マイナス'):
            self._next()
            factor = -1
        self._match(self._first_O())
        return factor * self._O()

    def _match(self, toks):
        if self.p in toks:
            return
        # Special check for Japanese Ze-Ro
        if self.language == Language.Japanese and self.p == 'ゼーロ' and 'ゼーロ' in toks:
            return
            
        raise ValueError(f"Invalid syntax: expected one of tokens: {toks}")

    def _first_O(self):
        keys = list(self.chn_dict.keys())
        first_tokens = keys + self.unit_num[self._NUMBER_UNIT_J]
        if self.language == Language.Japanese:
             first_tokens += self.unit_num[self._NUMBER_UNIT_H]
             first_tokens += self.unit_num[self._NUMBER_UNIT_S]
             first_tokens += self.unit_num[self._NUMBER_UNIT_M]
             first_tokens += self.unit_num[self._NUMBER_UNIT_O]
        return first_tokens

    def _first_NE(self):
        first_tokens = self._first_O() + [self.CHAR_NEGATIVE]
        if self.language == Language.Japanese:
             first_tokens.append('マイナス')
        return first_tokens

    def _init_for_next(self):
        self.f = 1

    def _parse_num(self):
        self._init_for_next()
        num = self._NE()
        if self.language == Language.Chinese:
            if self.f > 10 and self.f <= 10000:
                # In oral Chinese, only tailing number less than 10 thousand can omit
                # the tailing numeric unit (千, 百, 十)
                tail_num = num % 10
                num = num - tail_num + tail_num * self.f / 10
        return num

    def _start(self):
        out = ""
        self._next()
        last_is_num = 0
        while self.p != "EOF":
            # For _first_NE check, we need to be careful with list vs string containment
            first_ne_tokens = self._first_NE()
            if self.p in first_ne_tokens:
                self._save_pos()
                try:
                    num = int(self._parse_num())
                except ValueError:
                    self._restore_pos()
                    out += self.p
                    self._next()
                    continue
                # Do not add space for single number
                # Typically for phone numbers, years
                if last_is_num and num >= 10:
                    out += " "
                out += str(num)
                last_is_num = 1
            else:
                if self.p == self.chn_pt and last_is_num:
                    out += "."
                else:
                    out += self.p
                last_is_num = 0
                self._next()
        self._out = out
        return out

    def __call__(self):
        if self._out is not None:
            return self._out
        return self._start()



def replace_chinese_nums(s, ignore_quant=False, quant_unit :str=None, unit_dict :dict=None, chn_dict :dict=None, language=Language.Chinese):
    if quant_unit is None:
        quant_unit = "款年月日个台天部代元块"
    cnc = ChineseNumberConvertor(s, language=language)
    if unit_dict is not None:
        cnc.unit_dict = unit_dict
    if chn_dict is not None:
        cnc.chn_dict = chn_dict
        cnc.chn_zero = []
        for c, n in cnc.chn_dict.items():
            if n == 0:
                cnc.chn_zero.append(c)

    # Preprocess
    special_nums = []
    if ignore_quant:
        special_nums = re.findall('[' + cnc.chn_num + cnc.chn_num_upper + ']{2,}[' + quant_unit + ']', s)
        special_nums = list(set(special_nums))
        for i, sn in enumerate(special_nums):
            s = s.replace(sn, "<__chn_num_protect_group_({})>".format(i))
    
    # Perform conversion
    s = ChineseNumberConvertor(s, language=language)()
    
    # Postprocess
    if ignore_quant:
        for i, sn in enumerate(special_nums):
            s = s.replace("<__chn_num_protect_group_({})>".format(i), sn)
    
    return s


def main(s, **kwargs):
    print(">>", s)
    s = replace_chinese_nums(s, **kwargs)
    print(s)


"""
Sample output:
>> 给我买两三个榴莲，一百五十二块一个
给我买两三个榴莲，152块1个
>> 给我买二十三个榴莲，再买一两个西瓜
给我买23个榴莲，再买一两个西瓜
>> 截至二零二三年十二月，中国有十四亿一千七十七万八千七百二十四人，GDP超过两万五千五百亿人民币
截至二零二三年12月，中国有1410778724人，GDP超过2550000000000人民币
>> 今年的增长率为负三十五个百分点，需要负责人研究如何止住负增长趋势
今年的增长率为-35个百分点，需要负责人研究如何止住负增长趋势
>> 二百五加三百六等于六百一。三百零五加四十五等于三百五
250加360等于610。305加45等于350
>> 买这个电脑我花了一万五
买这个电脑我花了15000
>> 《一千零一夜》这本书很好看。一千一，一千零一十，一千五百五，一千五百零五
《1001夜》这本书很好看。1100，1010，1550，1505
"""
if __name__ == '__main__':
    sentenses = [
        "给我买两三个榴莲，一百五十二块一个",
        "给我买二十三个榴莲，再买一两个西瓜",
        "截至二零二三年十二月，中国有十四亿一千七十七万八千七百二十四人，GDP超过两万五千五百亿人民币",
        "今年的增长率为负三十五个百分点，需要负责人研究如何止住负增长趋势",
        "二百五加三百六等于六百一。三百零五加四十五等于三百五",
        "买这个电脑我花了一万五",
        "《一千零一夜》这本书很好看。一千一，一千零一十，一千五百五，一千五百零五"
    ]
    for s in sentenses:
        main(s, ignore_quant=True)
