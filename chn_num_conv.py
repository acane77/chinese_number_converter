

'''
   Oku -> Man 亿 Man | Man
   Man -> Sen 万 Sen | Sen
   Sen -> NonZero 千 Hyaku | NonZero 千 零 Juu | Hyaku
   Hyaku -> NonZero 百 Juu | NonZero 百 零 Num |
   Juu -> NonZero 十 Num | 十 Num | Num
   Num -> NonZero | 零
   NonZero -> 一 | 二 | 三 | 四 | 五 | 六 | 七 | 八 | 九
'''

class ChineseNumberConvertor():
    def __init__(self, s):
        self.chn_num = "零一二三四五六七八九"
        self.chn_dict = {}
        for i, c in enumerate(self.chn_num):
            self.chn_dict[c] = i
        self.chn_num += "两"
        self.chn_dict["两"] = 2
        self.i = -1
        self.s = s
        self.p = ""
        self._out = None

    def _next(self):
        self.i += 1
        if self.i >= self.s.__len__():
            self.p = 'EOF'
            return 'EOF'
        self.p = self.s[self.i]
        return self.p

    def _retract(self):
        self.i -= 1
        self.p = self.s[self.i]
        return self.p

    def _N(self):
        if self.p in self.chn_num:
            n = self.chn_dict[self.p]
            self._next()
            return n
        else:
            return -1

    def _J(self):
        n = self._N()
        if self.p == '十':
            self._next()
            m = self._N()
            return max(1, n) * 10 + max(0, m)
        return max(0, n)

    def _H(self):
        n = self._N()
        if self.p == '百':
            self._next()
            if self.p == '零':
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
        if self.p == '千':
            self._next()
            if self.p == '零':
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
        if self.p == '万':
            self._next()
            if self.p == '零':
                self._next()
            m = self._S()
            return n * 10000 + m
        return n

    def _O(self):
        n = self._M()
        if self.p == '亿':
            self._next()
            if self.p == '零':
                self._next()
            m = self._M()
            return n * 1e8 + m
        return n

    def _start(self):
        out = ""
        self._next()
        last_is_num = 0
        while self.p != "EOF":
            if self.p in self.chn_num or self.p == '十':
                num = int(self._O())
                # 单个的数字就不需要加空格了
                if last_is_num and num > 10:
                    out += " "
                out += str(num)
                last_is_num = 1
            else:
                if self.p == '点' and last_is_num:
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


