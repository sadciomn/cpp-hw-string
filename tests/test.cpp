
#include "../string.hpp"

#include <gtest/gtest.h>

TEST(ExampleTest, Simple) {
  String s1("Please test your code");

  ASSERT_EQ(s1.Length(), 21);
  ASSERT_EQ(s1[0], 'P');
  ASSERT_EQ(s1[20], 'e');
  ASSERT_EQ(s1[5], 'e');

  String s2;
  ASSERT_TRUE(s2.Empty());
}

TEST(YourTest, Test) {
  String s;
}
#include <cassert>
#include <cstring>
#include <sstream>
#include <string>
#include <tuple>  // for std::ignore

#include <gtest/gtest.h>

TEST(String, Basic) {
  String s("abcdef");

  ASSERT_EQ(s.Length(), 6);
  ASSERT_EQ(s[0], 'a');
  ASSERT_EQ(s[3], 'd');
  ASSERT_EQ(s[5], 'f');
  ASSERT_EQ(s.Back(), 'f');
  ASSERT_EQ(s.Front(), 'a');
  ASSERT_EQ(s[6], '\0');

  s[1] = 'x';
  s.Back() = 'y';
  s.Front() = 'y';

  ASSERT_EQ(s[0], 'y');
  ASSERT_EQ(s[5], 'y');
  ASSERT_EQ(s[1], 'x');
}

TEST(String, Const) {
  const String s("abcdef");
  ASSERT_EQ(s.Length(), 6);
  ASSERT_EQ(s[0], 'a');
  ASSERT_EQ(s[3], 'd');
  ASSERT_EQ(s[5], 'f');
  ASSERT_EQ(s.Back(), 'f');
  ASSERT_EQ(s.Front(), 'a');
  ASSERT_EQ(s[6], '\0');

  ASSERT_TRUE(
      (!std::is_assignable_v<decltype(static_cast<const String&>(String())[0]),
                             char>));
  ASSERT_TRUE((!std::is_assignable_v<
               decltype(static_cast<const String&>(String()).Front()), char>));
  ASSERT_TRUE((!std::is_assignable_v<
               decltype(static_cast<const String&>(String()).Back()), char>));

  String s2("abc");
  const char& cfront = s2.Front();
  const char& cback = s2.Back();
  s2.Front() = 'c';
  s2.Back() = 'a';
  ASSERT_EQ(cfront, 'c') << "front() must not copy the character";
  ASSERT_EQ(cback, 'a') << "back() must not copy the character";

  const String& s3 = s2;
  char* data2 = s2.Data();
  ASSERT_EQ(strlen(data2), 3);
  const char* data3 = s3.Data();
  ASSERT_EQ(strlen(data3), 3);
  data2[0] = 'z';
  data2[2] = 'v';
  ASSERT_EQ(*data3, 'z');
  ASSERT_EQ(s2.Front(), 'z');
  ASSERT_EQ(s3.Front(), 'z');
  ASSERT_EQ(s2[0], 'z');
  ASSERT_EQ(s3[0], 'z');
  ASSERT_EQ(s2.Back(), 'v');
  ASSERT_EQ(s3.Back(), 'v');
}

TEST(String, LongStrings) {
  String s("aaa");
  for (int i = 0; i < 500'000; ++i) {
    s.PushBack('a');
  }
  {
    String ss = s;
    ASSERT_EQ(ss.Length(), 500'003);
    ASSERT_EQ(s[500'000], 'a');
    ss[500'000] = 'b';
  }
  ASSERT_EQ(s.Length(), 500'003);
  ASSERT_EQ(s[500'000], 'a');

  String ss = s;
  for (int i = 0; i < 500'000; ++i) {
    s.PopBack();
  }

  ASSERT_EQ(s.Length(), 3);
  ASSERT_EQ(ss.Length(), 500'003);
  ss = s;
  s = String("abc");
  ASSERT_EQ(ss.Length(), 3);
  ASSERT_EQ(ss[2], 'a');

  for (int i = 0; i < 500'000; ++i) {
    const String sss(1000, 'a');
    String sz = sss;
    s += sz.Back();
  }

  s = [](String& s) {
    return s;
  }(s);
  ASSERT_EQ(s.Length(), 500'003);
  ASSERT_EQ(s[500'002], 'a');
  ASSERT_EQ(s[266'532], 'a');

  (s += String(100, 'b')) += String(100, 'c');
  ASSERT_EQ(s.Length(), 500'203);
  ASSERT_EQ(s.Back(), 'c');

  for (int i = 0; i < 150; ++i) {
    s.PopBack();
  }
  ASSERT_EQ(s.Back(), 'b');

  s = ss = s;
  (ss = s) = ss;
}

TEST(String, Streams) {
  std::istringstream iss("abcdefg\nABCDE");

  String s;
  ASSERT_TRUE(s.Empty());

  iss >> s;
  ASSERT_EQ(s.Length(), 7);

  String ss;
  iss >> ss;
  ASSERT_FALSE(ss.Empty());
  ASSERT_EQ(ss.Length(), 5);
  ASSERT_EQ((s + ss).Length(), 12);

  auto sum = ss + s;
  sum.PopBack();
  ASSERT_EQ(sum.Length(), 11);

  sum[0] = 'x';
  ASSERT_EQ(ss.Length(), 5);
  ASSERT_EQ(s.Length(), 7);

  std::ostringstream oss;
  oss << ('!' + sum + '?');

  ASSERT_EQ(oss.str(), "!xBCDEabcdef?");
}

TEST(String, Assignments) {
  String s = "abcde";

  auto& ss = s += 'f';

  ss += 'g';

  std::ostringstream oss;
  oss << ss;

  ASSERT_EQ(oss.str(), "abcdefg");
  ASSERT_EQ(s.Length(), 7);

  {
    auto& ss = (s = [](String& s) {
      return s;
    }(s));
    ASSERT_EQ(ss[5], 'f');
    auto sss = ss;
    ss.Clear();
    ASSERT_EQ(sss.Length(), 7);
  }
  ASSERT_TRUE(ss.Empty());
}

TEST(String, FindSubstr) {
  const String s = "122333444455555";
  //  0123456789ABCDE

  ASSERT_EQ(s.Find("44"), 6);
  ASSERT_EQ(s.Rfind("44"), 8);

  ASSERT_EQ(
      s.Find("12345"),
      (std::is_same_v<String, std::string> ? std::string::npos : s.Length()));

  ASSERT_EQ(s.Substr(5, 4), "3444");

  decltype(s.Substr(1, 1)) ss = s;
  ss[0] = '0';
  ASSERT_EQ(s[0], '1');

  decltype(ss = ss) sss = ss;
  sss[0] = '2';
  ASSERT_EQ(ss[0], '2');

  decltype(ss += 'a') ssss = ss;
  ssss.Front() = '3';
  ASSERT_EQ(ss.Front(), '3');

  {
    String s = "###$#$#$#$$###***$*#######@@##???#$$";
    ASSERT_EQ(s.Find("#$$"), 8);
    ASSERT_EQ(s.Rfind("#$$"), s.Length() - 3);
  }
}

int number_of_new = 0;

void* operator new(std::size_t size) {
  ++number_of_new;
  void* p = malloc(size);
  if (p == nullptr) {
    throw std::bad_alloc();
  }
  return p;
}

void* operator new[](std::size_t size) {
  ++number_of_new;
  void* p = malloc(size);
  if (p == nullptr) {
    throw std::bad_alloc();
  }
  return p;
}

void* operator new[](std::size_t size, const std::nothrow_t&) noexcept {
  ++number_of_new;
  return malloc(size);
}

void* operator new(std::size_t size, const std::nothrow_t&) noexcept {
  ++number_of_new;
  return malloc(size);
}

void operator delete(void* ptr) noexcept {
  free(ptr);
}

void operator delete(void* ptr, size_t) noexcept {
  free(ptr);
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept {
  free(ptr);
}

void operator delete[](void* ptr) noexcept {
  free(ptr);
}

void operator delete[](void* ptr, size_t) noexcept {
  free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept {
  free(ptr);
}

TEST(String, Allocations) {
  number_of_new = 0;

  String s1("abc");
  ASSERT_LE(number_of_new, 1) << "String s(\"abc\")";
  number_of_new = 0;

  String s2(5, 'c');
  ASSERT_LE(number_of_new, 1) << "String s(5, \'c\')";
  number_of_new = 0;

  String s3(s2);
  ASSERT_LE(number_of_new, 1) << "String s(String other)";
  number_of_new = 0;

  s2 = s1;
  ASSERT_LE(number_of_new, 1) << "s2=s1";
  number_of_new = 0;

  s3.PushBack('q');
  ASSERT_LE(number_of_new, 1) << "push_back";
  number_of_new = 0;

  s3.PopBack();
  ASSERT_EQ(number_of_new, 0) << "pop_back";
  number_of_new = 0;

  String s4("a");
  number_of_new = 0;
  s4 += s2;
  ASSERT_LE(number_of_new, 1) << "s4+=s2";
  s4 += s2 += s2 += s2;
  s4 += "defe";
  String s5("def");
  number_of_new = 0;

  std::ignore = s4.Find(s5);
  ASSERT_EQ(number_of_new, 0) << "s4.find(s5)";
  number_of_new = 0;

  std::ignore = s4.Rfind(s2);
  ASSERT_EQ(number_of_new, 0) << "s4.rfind (s2)";
  number_of_new = 0;

  std::ignore = s4.Substr(2, 5);
  ASSERT_LE(number_of_new, 1) << "For substr you need only one allocation";
  number_of_new = 0;

  s4.Clear();
  ASSERT_EQ(number_of_new, 0) << "You shouldn't reallocate memory on clear()";
  number_of_new = 0;

  String s6("abcdefg");
  for (size_t i = 0; i < 15; ++i) {
    s6.PushBack('a');
  }
  number_of_new = 0;
  s6.ShrinkToFit();
  ASSERT_EQ(s6.Size(), s6.Capacity());
  ASSERT_LE(number_of_new, 1);

  number_of_new = 0;

  char* data = s6.Data();
  std::ignore = data;
  ASSERT_EQ(number_of_new, 0) << "You don't need allocations for data() call";
}

TEST(String, Compare) {
  {
    const String s = "aboba";
    const String t = "abiba";
    ASSERT_TRUE(t < s);
    ASSERT_TRUE(!(t > s));
    ASSERT_TRUE(!(t >= s));
    ASSERT_TRUE(t <= s);
    ASSERT_TRUE(!(t == s));
    ASSERT_TRUE(t != s);

    ASSERT_FALSE("abiba" < t);
    ASSERT_TRUE("abuba" > t);
    ASSERT_FALSE("aboba" == t);
  }

  {
    String s = String("abobabiba");
    String t = String("aboba");
    ASSERT_TRUE(t < s);
    ASSERT_TRUE(!(t > s));
    ASSERT_TRUE(!(t >= s));
    ASSERT_TRUE(t <= s);
    ASSERT_TRUE(!(t == s));
    ASSERT_TRUE(t != s);

    s = String("aboba");
    t = String("");
    ASSERT_TRUE(t < s);
    ASSERT_TRUE(!(t > s));
    ASSERT_TRUE(!(t >= s));
    ASSERT_TRUE(t <= s);
    ASSERT_TRUE(!(t == s));
    ASSERT_TRUE(t != s);
  }

  String s = String(10'000'000, 'a');
  String t = String(5'000'000, 'a');
  for (size_t i = 0; i < 1'000'000; ++i) {
    s[9'999'999 - i] = 'b';
    ASSERT_TRUE(s != t);
  }
}

namespace {

void CheckEqual(const String& string1, const String& string2) {
  ASSERT_EQ(string1.Size(), string2.Size());
  ASSERT_TRUE(string1 == string2);
  for (uint64_t i = 0; i < std::max(string2.Size(), string1.Size()); i++) {
    ASSERT_EQ(string1[i], string2[i]);
  }
}

void CheckEqual(const String& string1, const char* cstr2) {
  String string2(cstr2);
  CheckEqual(string1, string2);
}

void CheckEmpty(const String& string) {
  ASSERT_EQ(string.Size(), 0ul);
  ASSERT_EQ(string.Length(), 0ul);
  ASSERT_TRUE(string.Empty());
}

}  // namespace

TEST(String, ConstructorsDestructor) {
  {
    [[maybe_unused]] String string;
  }
  {
    [[maybe_unused]] String string("ayooo");
  }
  {
    String string("boooo");
    [[maybe_unused]] String string1(string);
  }
  {
    [[maybe_unused]] String string(10'000'000, 'x');
  }
}

TEST(String, IndexOperatorRead) {
  String string("test string");
  ASSERT_EQ(string[0], 't');
  ASSERT_EQ(string[1], 'e');
  ASSERT_EQ(string[2], 's');
  ASSERT_EQ(string[3], 't');
  ASSERT_EQ(string[4], ' ');
  ASSERT_EQ(string[5], 's');
  ASSERT_EQ(string[6], 't');
  ASSERT_EQ(string[7], 'r');
  ASSERT_EQ(string[8], 'i');
  ASSERT_EQ(string[9], 'n');
  ASSERT_EQ(string[10], 'g');
}

TEST(String, IndexOperatorWrite) {
  String string("test string");
  string[0] = 's';
  ASSERT_EQ(string[0], 's');
  ASSERT_EQ(string.Data()[0], 's');
}

TEST(String, Empty) {
  String string;
  ASSERT_EQ(string.Size(), 0ul);
  ASSERT_EQ(string.Length(), 0ul);
  ASSERT_TRUE(string.Empty());
  uint64_t inf = string.Size();  // for std::string string.size() -> -1
  ASSERT_EQ(string.Find("booya"), inf);
  ASSERT_EQ(string.Rfind("ayoob"), inf);

  String string2;
  ASSERT_TRUE(string2 == string);
  ASSERT_TRUE(string2 <= string);
  ASSERT_TRUE(string2 >= string);

  CheckEmpty(string + string2);
  CheckEmpty(string += string2);
  ASSERT_TRUE(string == string);
  string.ShrinkToFit();
  ASSERT_TRUE(string == string);
}

TEST(String, CStyleToString) {
  String string("test string");
  ASSERT_EQ(string.Size(), 11ul);
  const char* s = "test string";
  for (int i = 0; i < 11; i++) {
    ASSERT_EQ(string[i], s[i]);
  }
  CheckEqual(string, string);
}

TEST(String, Copy) {
  {
    String string("test string");
    String str_copy(string);

    ASSERT_EQ(str_copy.Size(), 11ul);

    CheckEqual(string, str_copy);
    CheckEqual(string, "test string");
    CheckEqual(string, string);
  }

  {
    String string("test string");
    string[4] = '\0';
    String str_copy(string);

    ASSERT_EQ(str_copy.Size(), 11ul);
    ASSERT_EQ(string.Size(), 11ul);

    CheckEqual(string, str_copy);
    CheckEqual(string, string);
  }

  {
    String string(
        "test string random letters svkslvpvsvnosdmpodbnvofsjvsojvsojbnsfojnv "
        "sfojnvw");
    String str_copy(string);

    CheckEqual(string, str_copy);
    CheckEqual(string,
               "test string random letters "
               "svkslvpvsvnosdmpodbnvofsjvsojvsojbnsfojnv sfojnvw");
    CheckEqual(string, string);
  }
}

TEST(String, CopyNotShallow) {
  {
    String string("test string");
    String str_copy(string);

    ASSERT_EQ(str_copy.Size(), 11ul);

    str_copy[0] = '0';
    string[1] = '1';

    ASSERT_EQ(string[0], 't') << "Copy-constructor is wrong";
    ASSERT_EQ(str_copy[0], '0') << "Copy-constructor is wrong";

    ASSERT_EQ(string[1], '1') << "Copy-constructor is wrong";
    ASSERT_EQ(str_copy[1], 'e') << "Copy-constructor is wrong";
  }

  {
    String string("test string");
    string[4] = '\0';
    String str_copy(string);

    ASSERT_EQ(str_copy.Size(), 11ul);
    ASSERT_EQ(string.Size(), 11ul);

    str_copy[8] = '8';
    string[9] = '9';

    ASSERT_EQ(string[8], 'i') << "Copy-constructor is wrong";
    ASSERT_EQ(str_copy[8], '8') << "Copy-constructor is wrong";

    ASSERT_EQ(string[9], '9') << "Copy-constructor is wrong";
    ASSERT_EQ(str_copy[9], 'n') << "Copy-constructor is wrong";
  }

  {
    String string("test string");
    {
      [[maybe_unused]] String str_copy(string);
    }
    CheckEqual(string, "test string");
  }

  {
    String string(
        "test string random letters ouhveafnovsnfvosfbvofbvfsbnfosnvfsnvjsfv");
    String str_copy(string);

    str_copy[0] = '0';
    string[1] = '1';

    ASSERT_EQ(string[0], 't') << "Copy-constructor is wrong";
    ASSERT_EQ(str_copy[0], '0') << "Copy-constructor is wrong";

    ASSERT_EQ(string[1], '1') << "Copy-constructor is wrong";
    ASSERT_EQ(str_copy[1], 'e') << "Copy-constructor is wrong";
  }
}

TEST(String, Assignment) {
  String string = "test string";
  String copy_string("totaly different string");
  copy_string = string;
  CheckEqual(string, copy_string);

  for (int i = 0; i < 10; i++) {
    String temp_string(copy_string);
    copy_string = temp_string;
  }

  CheckEqual(string, copy_string);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
  copy_string = copy_string;
#pragma clang diagnostic pop

  CheckEqual(string, copy_string);
  CheckEqual(string, string);
}

TEST(String, PushBack) {
  {
    String string = "test string";
    string.PushBack('a');
    String str_expected("test stringa");
    CheckEqual(string, str_expected);
  }

  {
    String string = "";
    string.PushBack('a');
    String str_expected("a");
    CheckEqual(string, str_expected);
  }
}

TEST(String, PopBack) {
  String string = "test string";
  string.PopBack();
  String str_expected("test strin");
  CheckEqual(string, str_expected);
}

TEST(String, Output) {
  {
    std::ostringstream out;
    const String string("test string");
    out << string;
    const char* str_exprected = "test string";
    ASSERT_EQ(std::strcmp(str_exprected, (out.str()).c_str()), 0);
  }
  {
    std::ostringstream out;
    String string("test string");
    string[4] = '\0';
    out << string;
    ASSERT_EQ(out.str()[4], '\0');
    ASSERT_EQ(out.str()[5], 's');
  }
}

TEST(String, Input) {
  String string;
  std::istringstream in("test_string\n");
  in >> string;
  const String str_expected("test_string");
  CheckEqual(string, str_expected);
}

TEST(String, Plus) {
  {
    String string1("test ");
    String string2 = "string";
    String exp_string("test string");
    CheckEqual(string1 + string2, exp_string);
  }

  {
    String string1 = "random sssymolssss eapijfpofddofndonfdonbodnbdonb";
    String string2;
    String exp_string("random sssymolssss eapijfpofddofndonfdonbodnbdonb");
    CheckEqual(string1 + string2, exp_string);
  }

  {
    String string = "ome string i gues";
    char ch = 's';
    String exp_string = "ome string i guess";
    CheckEqual(string + ch, exp_string);
    exp_string = "some string i gues";
    CheckEqual(ch + string, exp_string);
  }

  {
    String string = "halabalooo";
    String exp_string1 = "halabaloooyaa";
    String exp_string2 = "yaahalabalooo";
    CheckEqual(string + "yaa", exp_string1);
    CheckEqual("yaa" + string, exp_string2);
  }
}

TEST(String, PlusAssign) {
  {
    String string1("test ");
    String string2 = "string";
    String exp_string("test string");
    CheckEqual(string1 += string2, exp_string);
  }

  {
    String string1 = "random sssymolssss eapijfpofddofndonfdonbodnbdonb";
    String string2;
    String exp_string("random sssymolssss eapijfpofddofndonfdonbodnbdonb");
    CheckEqual(string1 += string2, exp_string);
  }

  {
    String string = "ome string i gues";
    char s = 's';
    String exp_string = "ome string i guess";
    CheckEqual(string += s, exp_string);
  }

  {
    String string = "halabalooo";
    String exp_string = "halabaloooyaa";
    CheckEqual(string += "yaa", exp_string);
  }
}

TEST(String, BackFront) {
  String string("test string");
  ASSERT_EQ(string.Front(), 't');
  ASSERT_EQ(string.Back(), 'g');
  string.PopBack();
  ASSERT_EQ(string.Back(), 'n');
  string.PopBack();
  ASSERT_EQ(string.Back(), 'i');
}

TEST(String, FindRfind) {
  {
    String string("test string");

    ASSERT_EQ(string.Find("t"), 0ul);
    ASSERT_EQ(string.Rfind("t"), 6ul);

    uint64_t inf = string.Size();  // for std::string string.size() -> -1
    ASSERT_EQ(string.Find("popopo"), inf);
    ASSERT_EQ(string.Rfind("popopo"), inf);
  }

  {
    String string("test string");
    string[4] = '\0';

    ASSERT_EQ(string.Find("t"), 0ul);
    ASSERT_EQ(string.Rfind("t"), 6ul);

    ASSERT_EQ(string.Find("n"), 9ul);
    ASSERT_EQ(string.Rfind("e"), 1ul);

    ASSERT_EQ(string.Find("popopo"), string.Length());
    ASSERT_EQ(string.Rfind("popopo"), string.Length());
  }
}

TEST(String, ShrinkToFit) {
  String string("test string");
  string.ShrinkToFit();
  ASSERT_EQ(string.Size(), 11ul);

  string.PopBack();
  string.PopBack();
  string.ShrinkToFit();
  ASSERT_EQ(string.Size(), 9ul);

  string.PopBack();
  string.PopBack();
  string.PopBack();
  string.PopBack();
  string.ShrinkToFit();
  ASSERT_EQ(string.Size(), 5ul);

  string.PopBack();
  string.PopBack();
  string.ShrinkToFit();
  ASSERT_EQ(string.Size(), 3ul);
}
