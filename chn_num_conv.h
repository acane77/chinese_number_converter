/*

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
 * */

#ifndef _SISI_CHN_NUM_CONV_H_
#define _SISI_CHN_NUM_CONV_H_

#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>

#if SISI_IS_BIG_ENDIAN
#define SISI_ENABLE_LITTLE_ENDIAN 0
#elif SISI_IS_LITTLE_ENDIAN
#define SISI_ENABLE_LITTLE_ENDIAN 1
#elif _WIN32
#define SISI_ENABLE_LITTLE_ENDIAN 1
#elif _DARWIN
#define SISI_ENABLE_LITTLE_ENDIAN 1
#else
  #if defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN
    #define SISI_ENABLE_LITTLE_ENDIAN 1
  #elif defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN
    #define SISI_ENABLE_LITTLE_ENDIAN 0
  #else
    #error "Unknown byte order"
  #endif
#endif

#define SISI_ENABLE_LOG 0
#ifdef SISI_ENABLE_LOG
#define SISI_LOGD(fmt, ...) printf("[%d][%s] " fmt "\n", __LINE__, __FUNCTION__, ## __VA_ARGS__)
#else
#define SISI_LOGD(fmt, ...)
#endif

/// ^_^ Sisi is my English name Sisi
namespace sisi {

class UTF8String {
public:
  typedef std::vector<uint32_t> UTF8Sequence;
  typedef std::vector<int> CharIndexMapping;  // UTF8_index -> CHAR idx mapping

  UTF8String(const std::string& str) {
    this->str_ = str;
  }

  size_t size() {
    InitString();
    return seq_.size();
  }

  const uint32_t& operator[] (size_t idx) {
    InitString();
    return seq_[idx];
  }

  UTF8String& operator=(const UTF8String r) = delete;
  UTF8String& operator=(const UTF8String& r) = delete;
  UTF8String& operator=(const UTF8String&& r) = delete;

private:
  bool             is_init_ = false;
  std::string      str_;
  UTF8Sequence     seq_;
  CharIndexMapping idx_;

  void InitString() {
    if (is_init_) {
      return;
    }
    is_init_ = true;
    size_t sz = str_.size();
    int ch_idx = 0;
    while (ch_idx < sz) {
      uint32_t ch;
      idx_.push_back(ch_idx);
      ch_idx += UTF8NextChar((uint8_t*)str_.c_str() + ch_idx, &ch);
      seq_.push_back(ch);
    }
  }

  static uint32_t UTF8NextChar(const uint8_t* start, uint32_t* out) {
#define SISI_ULS(a, nbits)  (((uint32_t)(a)) << (nbits))
#if SISI_ENABLE_LITTLE_ENDIAN
#define SISI_U4(a, b, c, d) (SISI_ULS(a, 0) | SISI_ULS(b, 8) | SISI_ULS(c, 16) | SISI_ULS(d, 24))
#define SISI_U3(a, b, c)    (SISI_ULS(a, 0) | SISI_ULS(b, 8) | SISI_ULS(c, 16))
#define SISI_U2(a, b)       (SISI_ULS(a, 0) | SISI_ULS(b, 8))
#define SISI_U1(a)          (SISI_ULS(a, 0))
#else
#define SISI_U4(a, b, c, d) (SISI_ULS(a, 24) | SISI_ULS(b, 16) | SISI_ULS(c, 8) | SISI_ULS(d, 0))
#define SISI_U3(a, b, c)    (SISI_ULS(a, 24) | SISI_ULS(b, 16) | SISI_ULS(c, 8))
#define SISI_U2(a, b)       (SISI_ULS(a, 24) | SISI_ULS(b, 16))
#define SISI_U1(a)          (SISI_ULS(a, 24))
#endif
    uint8_t b0 = *start;
    if ((b0 & 0x80) == 0) {
      *out = SISI_U1(b0);
      return 1;
    }
    uint8_t b1 = *(start + 1);
    if ((b0 & 0xe0) == 0xc0) {
      *out = SISI_U2(b0, b1);
      return 2;
    }
    if ((b0 & 0xf0) == 0xe0) {
      uint8_t b3 = *(start + 2);
      *out = SISI_U3(b0, b1, b3);
      return 3;
    }
    if ((b0 & 0xf8) == 0xf0) {
      uint8_t b3 = *(start + 2);
      uint8_t b4 = *(start + 3);
      *out = SISI_U4(b0, b1, b3, b4);
      return 4;
    }
    // Invalid UTF-8 char
    fprintf(stderr, "warning: cannot decode UTF-8 char: 0x%x\n", b0);
    *out = ' ';
    return 1;
  }
#undef SISI_ULS
#undef SISI_U4
#undef SISI_U3
#undef SISI_U2
#undef SISI_U1
};

/*
   The BNF Grammar is list as follows:

   Negative -> 负 Oku | Oku
   Oku -> Man 亿 Man | Man
   Man -> Sen 万 Sen | Sen
   Sen -> NonZero 千 Hyaku | NonZero 千 零 Juu | Hyaku
   Hyaku -> NonZero 百 Juu | NonZero 百 零 Num |
   Juu -> NonZero 十 Num | 十 Num | Num
   Num -> NonZero | 零
   NonZero -> 一 | 二 | 三 | 四 | 五 | 六 | 七 | 八 | 九
 * */
class ChineseNumberConvertor {
public:
  using U8Char = uint64_t;
  using U8CharMap = std::unordered_map<U8Char, int>;
  using NumberType = int64_t;

  enum NumberUnit {
    NUMBER_UNIT_TEN             = 1,
    NUMBER_UNIT_HUNDRED         = 2,
    NUMBER_UNIT_THOUSAND        = 3,
    NUMBER_UNIT_TEN_THOUSAND    = 4,
    NUMBER_UNIT_HUNDRED_MILLION = 5,

    NUMBER_UNIT_J               = NUMBER_UNIT_TEN,
    NUMBER_UNIT_H               = NUMBER_UNIT_HUNDRED,
    NUMBER_UNIT_S               = NUMBER_UNIT_THOUSAND,
    NUMBER_UNIT_M               = NUMBER_UNIT_TEN_THOUSAND,
    NUMBER_UNIT_O               = NUMBER_UNIT_HUNDRED_MILLION,
  };

  explicit ChineseNumberConvertor(const char* str) : str_(str) {
    InitializeNumDict();
  }

  const std::string& Evaluate() {
    if (!has_out_) return Start();
    return out_;
  }

  const std::string& operator()() {
    return Evaluate();
  }

private:
  UTF8String  str_;
  enum : U8Char {
    TOKEN_TYPE_EOF = ~((U8Char)0),
  };
  U8Char      char_ne_;
  U8Char      char_pt_;
  U8Char      char_zero_;
  U8CharMap   chn_dict_;
  std::unordered_map<NumberUnit, U8CharMap> unit_num_;
  size_t      peak_idx_       = -1;
  size_t      peak_idx_rec_   = -1;
  size_t      lookahead_;
  size_t      unit_factor_    = 1;
  bool        has_out_        = false;
  std::string out_;
  bool        has_error_      = false;

  U8CharMap MakeCharMapFromString(const char* str) {
    U8CharMap u8chmap;
    AssignCharMapFromString(str, u8chmap);
    return u8chmap;
  }

  void AssignCharMapFromString(const char* str, U8CharMap& u8chmap) {
    UTF8String u8str(str);
    size_t sz = u8str.size();
    for (int i=0; i<sz; i++) {
      u8chmap[u8str[i]] = i;
    }
  }

  bool In(const U8CharMap& list, int* num) {
    auto it = list.find(lookahead_);
    if (it == list.end()) {
      return false;
    }
    if (num) {
      *num = it->second;
    }
    return true;
  }

  bool In(const U8CharMap& list) {
    return In(list, nullptr);
  }

  void InitializeNumDict() {
    unit_num_.clear();
    unit_num_[NUMBER_UNIT_J] = MakeCharMapFromString("十拾");
    unit_num_[NUMBER_UNIT_H] = MakeCharMapFromString("百佰");
    unit_num_[NUMBER_UNIT_S] = MakeCharMapFromString("千仟");
    unit_num_[NUMBER_UNIT_M] = MakeCharMapFromString("万");
    unit_num_[NUMBER_UNIT_O] = MakeCharMapFromString("亿");

    chn_dict_.clear();
    AssignCharMapFromString("零一二三四五六七八九", chn_dict_);
    AssignCharMapFromString("零壹贰叁肆伍陆柒捌玖", chn_dict_);

    UTF8String special_words("零点两负");
    char_zero_ = special_words[0];
    char_pt_ = special_words[1];
    chn_dict_[special_words[2]] = 2; // Alias for 二
    char_ne_ = special_words[3];
  }

  U8Char Next() {
    peak_idx_++;
    if (peak_idx_ >= str_.size()) {
      lookahead_ = TOKEN_TYPE_EOF;
      return lookahead_;
    }
    lookahead_ = str_[peak_idx_];
    return lookahead_;
  }

  U8Char Retract() {
    peak_idx_--;
    lookahead_ = str_[peak_idx_];
    return lookahead_;
  }

  void SavePos() {
    peak_idx_rec_ = peak_idx_;
  }

  void RestorePos() {
    peak_idx_ = peak_idx_rec_;
    lookahead_ = str_[peak_idx_];
  }

#define SISI_IS_FIRST_O() (In(chn_dict_) || In(unit_num_[NUMBER_UNIT_J]))
#define SISI_IS_FIRST_NE() (SISI_IS_FIRST_O() || LOOKAHEAD == char_ne_)

#define LOOKAHEAD (lookahead_)
#define LOOKAHEAD_STR (reinterpret_cast<char*>(&LOOKAHEAD))

// #define SISI_PRINT_LOOKAHEAD SISI_LOGD("lookahead enter: %s", LOOKAHEAD_STR);
// #define SISI_ENTER SISI_LOGD("enter %s...", __FUNCTION__);
// #define SISI_EXIT SISI_LOGD("exit %s...", __FUNCTION__);
// #define SISI_RETURN(x) do { auto rr = (x); SISI_LOGD("exit %s..., ret value is %ld", __FUNCTION__, rr); return (rr); } while (0)
#define SISI_RETURN(x) return (x)

  NumberType N(bool use_f=false) {
    int n;
    if (In(chn_dict_, &n)) {
      Next(); SISI_RETURN(n);
    }
    SISI_RETURN(-1);
  }

  NumberType J() {
    NumberType n = N();
    if (In(unit_num_[NUMBER_UNIT_J])) {
      Next(); unit_factor_ = 10;
      NumberType m = N(true);
      SISI_RETURN(std::max<NumberType>(1, n) * 10 + std::max<NumberType>(0, m));
    }
    SISI_RETURN(std::max<NumberType>(0, n));
  }

  NumberType H() {
    NumberType n = N(), m;
    if (In(unit_num_[NUMBER_UNIT_H])) {
      Next();
      unit_factor_ = 100;
      if (LOOKAHEAD == char_zero_) {
        unit_factor_ = 10; Next(); m = N();
      }
      else { m = J(); }
      SISI_RETURN(std::max<NumberType>(1, n) * NumberType(100) + std::max<NumberType>(0, m));
    }
    if (n >= 0) { Retract(); }
    SISI_RETURN(J());
  }

  NumberType S() {
    NumberType n = N(), m;
    if (In(unit_num_[NUMBER_UNIT_S])) {
      Next();
      unit_factor_ = 1000;
      if (LOOKAHEAD == char_zero_) {
        unit_factor_ = 10; Next(); m = J();
      }
      else { m = H(); }
      SISI_RETURN(std::max<NumberType>(1, n) * NumberType(1000) + std::max<NumberType>(0, m));
    }
    if (n >= 0) { Retract(); }
    SISI_RETURN(H());
  }

  NumberType M() {
    NumberType n = S(), m;
    if (In(unit_num_[NUMBER_UNIT_M])) {
      Next(); unit_factor_ = 10000;
      if (LOOKAHEAD == char_zero_) { unit_factor_ = 10; Next(); }
      m = S();
      SISI_RETURN(std::max<NumberType>(0, n) * NumberType(10000) + std::max<NumberType>(0, m));
    }
    SISI_RETURN(n);
  }

  NumberType O() {
    NumberType n = M(), m;
    if (In(unit_num_[NUMBER_UNIT_O])) {
      Next(); unit_factor_ = 100000000;
      if (LOOKAHEAD == char_zero_) { unit_factor_ = 10; Next(); }
      m = M();
      SISI_RETURN(std::max<NumberType>(0, n) * NumberType(100000000) + std::max<NumberType>(0, m));
    }
    SISI_RETURN(n);
  }

  NumberType NE() {
    int factor = 1;
    if (LOOKAHEAD == char_ne_) {
      Next(); factor = -1;
    }
    if (!SISI_IS_FIRST_O()) {
      has_error_ = true;
      SISI_RETURN(0);
    }
    SISI_RETURN(factor * O());
  }

  NumberType ParseNumber() {
    unit_factor_ = 1;
    has_error_ = false;
    NumberType num = NE();
    if (unit_factor_ > 10 && unit_factor_ <= 10000) {
      // In oral Chinese, only tailing number less than 10 thousand can omit
      // the tailing numeric unit (千, 百, 十)
      auto tail_num = num % 10;
      num = num - tail_num + tail_num * unit_factor_ / 10;
    }
    return num;
  }

  const std::string& Start() {
    has_out_ = false;
    out_.clear();
    out_.reserve(str_.size());
    Next();
    bool last_is_num = false;
    while (LOOKAHEAD != TOKEN_TYPE_EOF) {
      if (SISI_IS_FIRST_NE()) {
        SavePos();
        NumberType num = ParseNumber();
        if (has_error_) {
          RestorePos();
#if SISI_IS_BIG_ENDIAN
          LOOKAHEAD <<= 32;
#endif
          out_ += LOOKAHEAD_STR;
          Next();
          continue;
        }
        // Do not add space for single number, Typically for phone numbers, years
        if (last_is_num && num > 10) {
          out_ += " ";
        }
        out_ += std::to_string(num);
        last_is_num = true;
      } else {
        if (LOOKAHEAD == char_pt_ && last_is_num) {
          out_ += ".";
        } else {
#if SISI_IS_BIG_ENDIAN
          LOOKAHEAD <<= 32;
#endif
          out_ += LOOKAHEAD_STR;
        }
        last_is_num = false;
        Next();
      }
    }
    has_out_ = true;
    return out_;
  }
};


#undef LOOKAHEAD
#undef LOOKAHEAD_STR
#undef SISI_LOGD
#undef SISI_RETURN
#undef SISI_EXIT
#undef SISI_ENTER
#undef SISI_ENABLE_LOG
#undef SISI_IS_FIRST_NE
#undef SISI_IS_FIRST_O

}

#endif