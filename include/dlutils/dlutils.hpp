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

/// @brief A header-only C++ library for dynamic loading utilities
///
/// This library provides safe and convenient wrappers around dlopen/dlsym
/// for dynamically loading shared libraries at runtime.
namespace dlutils {

/// @brief Internal utilities for string creation
///
/// These functions are used internally to create strings from various types
/// and combinations of arguments.
namespace internal {

/// @brief Base case for recursive MakeStringInternal template
inline void MakeStringInternal(std::stringstream & /*ss*/) {}

/// @brief Appends a single value to a stringstream
/// @tparam T The type of the value to append
/// @param ss The stringstream to append to
/// @param t The value to append
template <typename T>
inline void MakeStringInternal(std::stringstream &ss, const T &t) {
  ss << t;
}

/// @brief Specialization for stringstream objects
/// @param ss The stringstream to append to
/// @param t The stringstream whose content will be appended
template <>
inline void MakeStringInternal(std::stringstream &ss,
                               const std::stringstream &t) {
  ss << t.str();
}

/// @brief Recursively appends multiple values to a stringstream
/// @tparam T The type of the first value to append
/// @tparam Args The types of the remaining values to append
/// @param ss The stringstream to append to
/// @param t The first value to append
/// @param args The remaining values to append
template <typename T, typename... Args>
inline void MakeStringInternal(std::stringstream &ss, const T &t,
                               const Args &...args) {
  MakeStringInternal(ss, t);
  MakeStringInternal(ss, args...);
}

/// @brief Creates a string from multiple arguments of various types
/// @tparam Args The types of the arguments
/// @param args The arguments to convert to string
/// @return A string containing all arguments converted to string form
template <typename... Args> std::string MakeString(const Args &...args) {
  std::stringstream ss;
  MakeStringInternal(ss, args...);
  return std::string(ss.str());
}

/// @brief Creates a string from vector elements with optional delimiter
/// @tparam T The type of elements in the vector
/// @param v The vector of elements to convert to string
/// @param delim The delimiter to use between elements (default: space)
/// @return A string containing all vector elements separated by the delimiter
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
/// @brief Specialization for std::string objects
/// @param str The string to return
/// @return The input string
template <> inline std::string MakeString(const std::string &str) {
  return str;
}

/// @brief Specialization for C-style strings
/// @param cstr The C-style string to convert
/// @return A std::string containing the same content
inline std::string MakeString(const char *cstr) { return std::string(cstr); }

} // namespace internal

/// @brief A wrapper class for dynamically loaded functions
///
/// This template class wraps function pointers obtained from dynamically loaded
/// libraries. It provides safe function calling with nullptr checking and
/// meaningful error messages.
///
/// @tparam R The return type of the function
/// @tparam Args The parameter types of the function
template <class R, class... Args> class DlFun {
public:
  /// @brief The function pointer type
  using FunTy = R (*)(Args...);

  /// @brief Default constructor
  DlFun() = default;

  /// @brief Constructor with function name and pointer
  /// @param name The name of the function
  /// @param funptr The function pointer
  DlFun(std::string_view name, FunTy funptr)
      : funName_(name), funptr_(funptr) {}

  /// @brief Get the underlying function pointer
  /// @return A std::function wrapper around the function pointer
  std::function<R(Args...)> Get() const { return funptr_; }

  /// @brief Get the function name
  /// @return The name of the function
  std::string GetName() const { return funName_; }

  /// @brief Call the function with the given arguments
  ///
  /// This operator checks if the function pointer is valid before calling it.
  /// If the pointer is nullptr, it throws a runtime_error with diagnostic
  /// information.
  ///
  /// @param args The arguments to pass to the function
  /// @return The result of calling the function
  /// @throws std::runtime_error if the function pointer is nullptr
  R operator()(Args... args) {
    if (funptr_ == nullptr) {
      throw std::runtime_error(internal::MakeString(
          "[", __FILE__, ":", __LINE__, "] Fatal Error: function ", funName_,
          " is nullptr. This typically means dlsym failed to load the "
          "function. ",
          "Check that the library is properly loaded and the function name is "
          "correct."));
    }
    return funptr_(std::forward<Args>(args)...);
  }

private:
  std::string funName_ =
      "unknown"; ///< The name of the function (default: "unknown")
  std::function<R(Args...)> funptr_ =
      nullptr; ///< The function pointer (default: nullptr)
};

/// @brief Base class for dynamic library loading
///
/// This class provides a foundation for loading dynamic libraries and their
/// functions. It wraps dlopen/dlsym operations and provides utilities for
/// managing loaded functions.
class DlLibBase {
protected:
  /// @brief Constructor
  /// @param lib The name of the library to load
  explicit DlLibBase(std::string_view lib) : libName_(lib) {}

  /// @brief Destructor (default)
  ~DlLibBase() = default;

  /// @brief Open the dynamic library
  ///
  /// This function uses dlopen to load the library specified in the
  /// constructor. See: https://man7.org/linux/man-pages/man3/dlopen.3.html If
  /// filename contains a slash ("/"), then it is interpreted as a (relative or
  /// absolute) pathname. Otherwise, the dynamic linker searches for the object.
  ///
  /// @return true if the library was successfully loaded, false otherwise
  bool SelfDlOpen() {
    libptr_ = dlopen(libName_.data(), RTLD_NOW | RTLD_GLOBAL);
    return libptr_ != nullptr;
  }

  /// @brief Load a symbol from the dynamic library
  ///
  /// This function uses dlsym to find a symbol in the loaded library and wraps
  /// it in a DlFun object. NOTE: This function is not thread-safe.
  ///
  /// Template argument deduction helper:
  /// When the caller calls this function with the outFun argument, the C++
  /// compiler will automatically deduce the template arguments, eliminating the
  /// need for the caller to specify the exact template arguments.
  ///
  /// @tparam R The return type of the function
  /// @tparam Args The parameter types of the function
  /// @param funName The name of the function to load
  /// @param outFun The DlFun object to populate with the function
  /// @return true if the symbol lookup was attempted (even if it failed), false
  /// if preconditions were not met
  template <class R, class... Args>
  bool SelfDlSym(std::string_view funName, DlFun<R, Args...> &outFun) {
    if (libptr_ == nullptr || funName.empty()) {
      return false;
    }

    void *funPtr = dlsym(libptr_, funName.data());
    funCache_.push_back(
        funPtr); // Store all function pointers, including failed ones
    outFun =
        DlFun<R, Args...>(funName, reinterpret_cast<R (*)(Args...)>(funPtr));
    return true;
  }

  /// @brief Check if all functions in the cache were successfully loaded
  /// @return true if all functions were successfully loaded, false otherwise
  bool CheckFunCache() const {
    for (const auto *p : funCache_) {
      if (p == nullptr) {
        return false;
      }
    }
    return true;
  }

  /// @brief Get the number of functions in the cache
  /// @return The number of functions in the cache
  size_t GetFunCacheSize() const { return funCache_.size(); }

private:
  void *libptr_ = nullptr; ///< Handle to the loaded library (default: nullptr)
  const std::string libName_ =
      "unknown"; ///< The name of the library to load (default: "unknown")

  /// @brief Cache of function pointers obtained through dlsym
  ///
  /// WARNING: DO NOT manually manipulate these pointers.
  /// The cache does NOT own these pointers; they should be read-only.
  std::vector<void *> funCache_;
};

/// @brief Macro to simplify loading symbols from dynamic libraries
///
/// This macro simplifies the process of loading symbols by automatically
/// converting the function name to a string and passing it to SelfDlSym.
/// The second argument acts as a template helper, allowing the compiler
/// to automatically deduce the template arguments for SelfDlSym.
///
/// Usage: DLUTILS_SELF_DLSYM(function_name)
/// Expands to: SelfDlSym("function_name", function_name)
#define DLUTILS_SELF_DLSYM(NAME) SelfDlSym(#NAME, NAME)

} // namespace dlutils
