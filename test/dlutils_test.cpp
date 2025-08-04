// MIT License
//
// Copyright (c) 2025 Jamie Cui
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "dlutils/dlutils.hpp"
#include "gtest/gtest.h"

#include <string>
#include <vector>

namespace dlutils {
namespace internal {

// Tests for MakeString function with various arguments
TEST(MakeStringTest, WithSingleString) {
  std::string result = internal::MakeString("Hello");
  EXPECT_EQ(result, "Hello");
}

TEST(MakeStringTest, WithCString) {
  const char *cstr = "World";
  std::string result = internal::MakeString(cstr);
  EXPECT_EQ(result, "World");
}

TEST(MakeStringTest, WithMultipleArguments) {
  std::string result = internal::MakeString("Hello", " ", "World");
  EXPECT_EQ(result, "Hello World");
}

TEST(MakeStringTest, WithNumbers) {
  std::string result = internal::MakeString("Value: ", 42, " and ", 3.14);
  EXPECT_EQ(result, "Value: 42 and 3.14");
}

TEST(MakeStringTest, WithVector) {
  std::vector<int> vec = {1, 2, 3, 4, 5};
  std::string result = internal::MakeString(vec);
  EXPECT_EQ(result, "1 2 3 4 5");
}

TEST(MakeStringTest, WithVectorAndCustomDelimiter) {
  std::vector<int> vec = {1, 2, 3, 4, 5};
  std::string result = internal::MakeString(vec, std::string(","));
  EXPECT_EQ(result, "1,2,3,4,5");
}

TEST(MakeStringTest, WithEmptyVector) {
  std::vector<int> vec = {};
  std::string result = internal::MakeString(vec);
  EXPECT_EQ(result, "");
}

} // namespace internal

// Tests for DlFun class
TEST(DlFunTest, DefaultConstructor) {
  DlFun<int, int, int> func;
  EXPECT_EQ(func.GetName(), "unknown");
  EXPECT_THROW(func(1, 2), std::runtime_error);
}

TEST(DlFunTest, ConstructorWithNameAndFunctionPointer) {
  auto lambda = [](int a, int b) { return a + b; };
  DlFun<int, int, int> func("add", lambda);
  EXPECT_EQ(func.GetName(), "add");
}

TEST(DlFunTest, GetFunctionPointer) {
  auto lambda = [](int a, int b) { return a + b; };
  DlFun<int, int, int> func("add", lambda);
  auto funcPtr = func.Get();
  EXPECT_NE(funcPtr, nullptr);
}

TEST(DlFunTest, CallOperatorWithValidFunction) {
  auto lambda = [](int a, int b) { return a + b; };
  DlFun<int, int, int> func("add", lambda);
  int result = func(3, 4);
  EXPECT_EQ(result, 7);
}

TEST(DlFunTest, CallOperatorWithNullptr) {
  DlFun<int, int, int> func;
  EXPECT_THROW(func(1, 2), std::runtime_error);
}

// Mock class for testing DlLibBase
class MockDlLib : public DlLibBase {
public:
  explicit MockDlLib(std::string_view lib) : DlLibBase(lib) {}

  bool OpenLib() { return SelfDlOpen(); }

  template <class R, class... Args>
  bool LoadSymbol(std::string_view funName, DlFun<R, Args...> &outFun) {
    return SelfDlSym(funName, outFun);
  }

  bool CheckCache() { return CheckFunCache(); }

  size_t CacheSize() { return GetFunCacheSize(); }
};

// Tests for DlLibBase class
TEST(DlLibBaseTest, Constructor) {
  MockDlLib lib("libtest.so");
  // Constructor test - should not throw
  SUCCEED();
}

TEST(DlLibBaseTest, GetFunCacheSizeInitiallyZero) {
  MockDlLib lib("libtest.so");
  EXPECT_EQ(lib.CacheSize(), 0u);
}

TEST(DlLibBaseTest, CheckFunCacheInitiallyTrue) {
  MockDlLib lib("libtest.so");
  // Initially no functions loaded, so should return true
  EXPECT_TRUE(lib.CheckCache());
}

} // namespace dlutils
