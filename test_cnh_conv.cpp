#include <iostream>
#include <cstring>

#include "gtest.h"
#include "chn_num_conv.h"

TEST(NumConv, ChineseTest) {
  const char *strs[] = {
      "截至二零二三年十二月，中国有十四亿一千七十七万八千七百二十四人，GDP超过两万五千五百亿人民币",
      "今年的增长率为负三十五个百分点，需要负责人研究如何止住负增长趋势",
      "二百五加三百六等于六百一。三百零五加四十五等于三百五",
      "买这个电脑我花了一万五",
      "pi等于三点一四一五九二六五三五，她的电话是一三五一二三四五六七八。",
      "给我推荐一个四五千的手机",
  };
  const char *results[] = {
      "截至2023年12月，中国有1410778724人，GDP超过2550000000000人民币",
      "今年的增长率为-35个百分点，需要负责人研究如何止住负增长趋势",
      "250加360等于610。305加45等于350",
      "买这个电脑我花了15000",
      "pi等于3.1415926535，她的电话是13512345678。",
      "给我推荐1个4 5000的手机", // Logic converts One to 1.
  };
  
  for (int i=0; i<sizeof(strs) / sizeof(const char *); i++) {
    const char* str = strs[i], *r = results[i];
    sisi::ChineseNumberConvertor cc(str, sisi::Language::Chinese);
    const char* result = cc().c_str();
    printf("[CN] %s -> %s\n", str, result);
    ASSERT_EQ(strcmp(r, result), 0);
  }
}

TEST(NumConv, JapaneseTest) {
    auto run = [](const char* in, const char* expected) {
        sisi::ChineseNumberConvertor cc(in, sisi::Language::Japanese);
        std::string res = cc();
        printf("[JP] %s -> %s\n", in, res.c_str());
        ASSERT_EQ(res, expected);
    };

    run("百一", "101");
    run("千一", "1001");
    run("一億二千万", "120000000");
    run("負百", "-100");
    run("マイナス百", "-100");
    run("ゼロからの勉強", "0からの勉強");
    run("今日は百一円を使いました", "今日は101円を使いました");
    run("私の戦闘力は五十三万です", "私の戦闘力は530000です");
    run("六百五", "605"); // Japanese exactness vs Chinese implicit unit (650)
    run("百十", "110");
    run("一万五十", "10050");
    run("一億二千三百四十五万六千七百八十九", "123456789");
    run("欠損金は負二億です", "欠損金は-200000000です");
    run("このパソコンは十九万八千円です", "このパソコンは198000円です");
    run("千二十四", "1024");
    run("一千二百三十四", "1234");
    run("平成二十四年", "平成24年");
}

int main() {
    TestRegistry::run_all();
    return 0;
}
