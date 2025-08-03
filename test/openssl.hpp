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

#include "dlutils/dlutils.hpp"

namespace dlutils {

// Define the missing definition of ossl structures
typedef struct evp_md_ctx_st EVP_MD_CTX;
typedef struct evp_md_st EVP_MD;
typedef struct engine_st ENGINE;

#define EVP_MAX_MD_SIZE (16 + 20) /* The SSLv3 md5+sha1 type */

class LibCrypto : public DlLibBase {
public:
  ~LibCrypto() = default;

  // Singleton instance
  static LibCrypto &GetInstance() {
    static LibCrypto instance;
    return instance;
  }

  // Check wheter dlopen and dlsym has succeed
  bool CheckOk() { return CheckFunCache(); }

  // Reload all functions
  bool Reload() {
    LoadAll();
    return CheckFunCache();
  }

  // Size() function will not check if all functions are callable
  size_t Size() { return GetFunCacheSize(); }

  // Declare all functions that you need
  // NOTE Please make sure the class instance is inited before calling those
  // functions
  DlFun<EVP_MD_CTX *> EVP_MD_CTX_new;
  DlFun<const EVP_MD *> EVP_sha256;
  DlFun<int, EVP_MD_CTX *, const EVP_MD *, ENGINE *> EVP_DigestInit_ex;
  DlFun<int, EVP_MD_CTX *, const void *, size_t> EVP_DigestUpdate;
  DlFun<int, EVP_MD_CTX *, unsigned char *, unsigned int *> EVP_DigestFinal_ex;
  DlFun<void, EVP_MD_CTX *> EVP_MD_CTX_free;

private:
  void LoadAll() {
    // NOTE explicitly dlopen shared library
    SelfDlOpen();
    DLUTILS_SELF_DLSYM(EVP_MD_CTX_new);
    DLUTILS_SELF_DLSYM(EVP_sha256);
    DLUTILS_SELF_DLSYM(EVP_DigestInit_ex);
    DLUTILS_SELF_DLSYM(EVP_DigestUpdate);
    DLUTILS_SELF_DLSYM(EVP_DigestFinal_ex);
    DLUTILS_SELF_DLSYM(EVP_MD_CTX_free);
  }

  LibCrypto() : DlLibBase(LIB_NAME) { LoadAll(); }

  static constexpr std::string_view LIB_NAME = "libcrypto.so";
};

} // namespace dlutils
