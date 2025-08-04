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

namespace dlutils {

// Mock class for testing DlLibBase with extended functionality
class ExtendedMockDlLib : public DlLibBase {
public:
  explicit ExtendedMockDlLib(std::string_view lib) : DlLibBase(lib) {}

  bool OpenLib() { return SelfDlOpen(); }

  template <class R, class... Args>
  bool LoadSymbol(std::string_view funName, DlFun<R, Args...> &outFun) {
    return SelfDlSym(funName, outFun);
  }

  bool CheckCache() { return CheckFunCache(); }

  size_t CacheSize() { return GetFunCacheSize(); }
};

// Tests for DlLibBase class with invalid library
TEST(DlLibBaseExtendedTest, OpenInvalidLibrary) {
  ExtendedMockDlLib lib("libnonexistent.so");
  EXPECT_FALSE(lib.OpenLib());
}

// Tests for DlLibBase class with invalid function name
TEST(DlLibBaseExtendedTest, LoadInvalidFunction) {
  ExtendedMockDlLib lib("libnonexistent.so");
  DlFun<int, int, int> func;
  // This should return false because the library wasn't opened
  EXPECT_FALSE(lib.LoadSymbol("nonexistent_function", func));
  // The function name should still be "unknown" because LoadSymbol didn't
  // succeed
  EXPECT_EQ(func.GetName(), "unknown");
}

// Tests for DlLibBase class with null library pointer
TEST(DlLibBaseExtendedTest, LoadSymbolWithNullLibPtr) {
  ExtendedMockDlLib lib("libnonexistent.so");
  // Not calling OpenLib, so libptr_ remains nullptr
  DlFun<int, int, int> func;
  EXPECT_FALSE(lib.LoadSymbol("some_function", func));
}

// Tests for DlLibBase class with empty function name
TEST(DlLibBaseExtendedTest, LoadSymbolWithEmptyName) {
  ExtendedMockDlLib lib("libnonexistent.so");
  DlFun<int, int, int> func;
  EXPECT_FALSE(lib.LoadSymbol("", func));
}

// Tests for DlLibBase class function cache size
TEST(DlLibBaseExtendedTest, FunCacheSizeInitiallyZero) {
  ExtendedMockDlLib lib("libnonexistent.so");
  EXPECT_EQ(lib.CacheSize(), 0u);
}

// Tests for DlLibBase class CheckFunCache initially true
TEST(DlLibBaseExtendedTest, CheckFunCacheInitiallyTrue) {
  ExtendedMockDlLib lib("libnonexistent.so");

  // Initially no functions loaded, so should return true
  EXPECT_TRUE(lib.CheckCache());
}

// Additional extended tests for DlLibBase class
TEST(DlLibBaseExtendedTest, FunCacheSizeAfterMultipleLoads) {
  ExtendedMockDlLib lib("libnonexistent.so");
  DlFun<int, int, int> func1;
  DlFun<double, double> func2;
  DlFun<void> func3;
  
  // Cache size should be 0 initially
  EXPECT_EQ(lib.CacheSize(), 0u);
  
  // Try to load functions without opening library
  // These should return false and not add to cache since preconditions aren't met
  EXPECT_FALSE(lib.LoadSymbol("func1", func1));
  EXPECT_FALSE(lib.LoadSymbol("func2", func2));
  EXPECT_FALSE(lib.LoadSymbol("func3", func3));
  
  // Cache size should still be 0 since SelfDlSym returns false when preconditions aren't met
  EXPECT_EQ(lib.CacheSize(), 0u);
  
  // CheckFunCache should return true since cache is empty
  EXPECT_TRUE(lib.CheckCache());
}

TEST(DlLibBaseExtendedTest, FunCacheSizeAfterMixedSuccessFailure) {
  ExtendedMockDlLib lib("libnonexistent.so");
  DlFun<int, int, int> func1;
  DlFun<double, double> func2;
  
  // Cache size should be 0 initially
  EXPECT_EQ(lib.CacheSize(), 0u);
  
  // Try to load one function without opening library (should fail)
  EXPECT_FALSE(lib.LoadSymbol("func1", func1));
  
  // Cache size should still be 0 since SelfDlSym returns false when preconditions aren't met
  EXPECT_EQ(lib.CacheSize(), 0u);
  
  // CheckFunCache should return true since cache is empty
  EXPECT_TRUE(lib.CheckCache());
}

TEST(DlLibBaseExtendedTest, ConstructorWithLongLibraryName) {
  std::string longName(1000, 'a');
  longName += ".so";
  ExtendedMockDlLib lib(longName);
  // Constructor test - should not throw
  SUCCEED();
}

TEST(DlLibBaseExtendedTest, LoadSymbolWithVeryLongFunctionName) {
  ExtendedMockDlLib lib("libnonexistent.so");
  DlFun<int, int, int> func;
  std::string longFuncName(1000, 'f');
  EXPECT_FALSE(lib.LoadSymbol(longFuncName, func));
  EXPECT_EQ(func.GetName(), "unknown");
}

} // namespace dlutils
