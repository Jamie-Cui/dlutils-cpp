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

#pragma once

#include <dlfcn.h>
#include <string>

#include <functional>
#include <sstream>
#include <vector>

namespace dlutils {

namespace internal {

inline void MakeStringInternal(std::stringstream & /*ss*/) {}

template <typename T>
inline void MakeStringInternal(std::stringstream &ss, const T &t) {
  ss << t;
}

template <>
inline void MakeStringInternal(std::stringstream &ss,
                               const std::stringstream &t) {
  ss << t.str();
}

template <typename T, typename... Args>
inline void MakeStringInternal(std::stringstream &ss, const T &t,
                               const Args &...args) {
  MakeStringInternal(ss, t);
  MakeStringInternal(ss, args...);
}

template <typename... Args> std::string MakeString(const Args &...args) {
  std::stringstream ss;
  MakeStringInternal(ss, args...);
  return std::string(ss.str());
}

template <typename T>
std::string MakeString(const std::vector<T> &v,
                       const std::string &delim = " ") {
  std::stringstream ss;
  for (auto it = v.begin(); it < v.end(); it++) {
    if (it != v.begin()) {
      MakeStringInternal(ss, delim);
    }
    MakeStringInternal(ss, *it);
  }
  return std::string(ss.str());
}

// Specializations for already-a-string types.
template <> inline std::string MakeString(const std::string &str) {
  return str;
}
inline std::string MakeString(const char *cstr) { return std::string(cstr); }

} // namespace internal

template <class R, class... Args> class DlFun {
public:
  using FunTy = R (*)(Args...);
  DlFun() = default;
  DlFun(std::string_view name, FunTy funptr)
      : funName_(name), funptr_(funptr) {}

  std::function<R(Args...)> Get() const { return funptr_; }

  std::string GetName() const { return funName_; }

  R operator()(Args... args) {
    if (funptr_ == nullptr) {
      throw std::runtime_error(internal::MakeString(
          "[", __FILE__ ":", __LINE__, "] Fatal Error: function ", funName_,
          " is nullptr. This typically means dlsym failed to load the function. ",
          "Check that the library is properly loaded and the function name is correct."));
    }
    return funptr_(std::forward<Args>(args)...);
  }

private:
  std::string funName_ = "unknown";            // default to unknown
  std::function<R(Args...)> funptr_ = nullptr; // default to nullptr
};

// Dynamic-load Lib base class
class DlLibBase {
protected:
  explicit DlLibBase(std::string_view lib) : libName_(lib) {}

  ~DlLibBase() = default;

  bool SelfDlOpen() {
    // see: https://man7.org/linux/man-pages/man3/dlopen.3.html
    //      If filename is NULL, then the returned handle is for the main
    //      program.  If filename contains a slash ("/"), then it is
    //      interpreted as a (relative or absolute) pathname.  Otherwise, the
    //      dynamic linker searches for the object
    libptr_ = dlopen(libName_.data() /* search for LD_LIBRARY_PATH */,
                     RTLD_NOW | RTLD_GLOBAL);
    return libptr_ != nullptr;
  }

  // Call dlsym to find symbol
  // None thread-safe function
  // Q: Whay adding an unused arguments: templateHelper
  // A: When the caller calls this function with templateHelper argument, c++
  // compiler will automatically deduce the tempalte arguments, such that there
  // is no need for the caller to specify the exact template arguments.
  template <class R, class... Args>
  bool SelfDlSym(std::string_view funName, DlFun<R, Args...> &outFun) {
    if (libptr_ == nullptr || funName.empty()) {
      return false;
    }

    void *funPtr = dlsym(libptr_, funName.data());
    funCache_.push_back(funPtr); // the failed funPtr also get writed in
                                 // funCache_, we check nullptr later
    outFun =
        DlFun<R, Args...>(funName, reinterpret_cast<R (*)(Args...)>(funPtr));
    return true;
  }

  bool CheckFunCache() {
    for (auto *p : funCache_) {
      if (p == nullptr) {
        return false;
      }
    }
    return true;
  }

  size_t GetFunCacheSize() { return funCache_.size(); }

private:
  void *libptr_ = nullptr;
  const std::string libName_ = "unknown";

  // funCache_ stores all dlsym function raw pointers
  // WARNING: DO NOT manually manipulate those pointers
  std::vector<void *>
      funCache_; // cache DO NOT own pointers, those pointers should read only
};

// The second argument is the templateHelper which helps template deduction
#define DLUTILS_SELF_DLSYM(NAME) SelfDlSym(#NAME, NAME)

} // namespace dlutils
