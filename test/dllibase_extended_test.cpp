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

  void OpenLib() { SelfDlOpen(); }

  template <class R, class... Args>
  void LoadSymbol(std::string_view funName, DlFun<R, Args...> &outFun) {
    SelfDlSym(funName, outFun);
  }
};

// Tests for DlLibBase class with invalid library
TEST(DlLibBaseExtendedTest, OpenInvalidLibrary) {
  ExtendedMockDlLib lib("libnonexistent.so");
  EXPECT_THROW(lib.OpenLib(), std::runtime_error);
}

// Tests for DlLibBase class with invalid function name
TEST(DlLibBaseExtendedTest, LoadInvalidFunction) {
  ExtendedMockDlLib lib("libnonexistent.so");
  DlFun<int, int, int> func;
  // This should throw because the library wasn't opened
  EXPECT_THROW(lib.LoadSymbol("nonexistent_function", func), std::runtime_error);
}

// Tests for DlLibBase class with null library pointer
TEST(DlLibBaseExtendedTest, LoadSymbolWithNullLibPtr) {
  ExtendedMockDlLib lib("libnonexistent.so");
  // Not calling OpenLib, so libptr_ remains nullptr
  DlFun<int, int, int> func;
  EXPECT_THROW(lib.LoadSymbol("some_function", func), std::runtime_error);
}

// Tests for DlLibBase class with empty function name
TEST(DlLibBaseExtendedTest, LoadSymbolWithEmptyName) {
  ExtendedMockDlLib lib("libnonexistent.so");
  DlFun<int, int, int> func;
  EXPECT_THROW(lib.LoadSymbol("", func), std::runtime_error);
}

// Additional extended tests for DlLibBase class
TEST(DlLibBaseExtendedTest, MultipleFunctionLoadsWithoutLibrary) {
  ExtendedMockDlLib lib("libnonexistent.so");
  DlFun<int, int, int> func1;
  DlFun<double, double> func2;
  DlFun<void> func3;
  
  // Try to load functions without opening library
  // These should throw exceptions since library is not loaded
  EXPECT_THROW(lib.LoadSymbol("func1", func1), std::runtime_error);
  EXPECT_THROW(lib.LoadSymbol("func2", func2), std::runtime_error);
  EXPECT_THROW(lib.LoadSymbol("func3", func3), std::runtime_error);
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
  EXPECT_THROW(lib.LoadSymbol(longFuncName, func), std::runtime_error);
}

} // namespace dlutils
