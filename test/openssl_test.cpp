// Mit License
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

#include "openssl.hpp"
#include "gtest/gtest.h"

#include <iostream>

namespace dlutils {

TEST(OpenSSLTest, ShouldWork) {
  auto libcrypto = LibCrypto::GetInstance();
  EVP_MD_CTX *mdctx;
  // const EVP_MD *md;
  // unsigned char md_value[EVP_MAX_MD_SIZE];
  // unsigned int md_len;
  // const char *message = "Hello, OpenSSL Hashing!";

  // 1. Initialize the message digest context
  mdctx = libcrypto.EVP_MD_CTX_new();
  if (mdctx == NULL) {
    fprintf(stderr, "Error creating EVP_MD_CTX\n");
  }

  // 2. Select the hash algorithm (e.g., SHA256)
  // md = EVP_sha256();
  // if (md == NULL) {
  //   fprintf(stderr, "Error getting SHA256 method\n");
  //   EVP_MD_CTX_free(mdctx);
  //   return 1;
  // }

  // // 3. Initialize the digest operation
  // if (EVP_DigestInit_ex(mdctx, md, NULL) != 1) {
  //   fprintf(stderr, "Error initializing digest\n");
  //   EVP_MD_CTX_free(mdctx);
  //   return 1;
  // }

  // // 4. Update the digest with the data
  // if (EVP_DigestUpdate(mdctx, message, strlen(message)) != 1) {
  //   fprintf(stderr, "Error updating digest\n");
  //   EVP_MD_CTX_free(mdctx);
  //   return 1;
  // }

  // // 5. Finalize the digest and retrieve the hash value
  // if (EVP_DigestFinal_ex(mdctx, md_value, &md_len) != 1) {
  //   fprintf(stderr, "Error finalizing digest\n");
  //   EVP_MD_CTX_free(mdctx);
  //   return 1;
  // }

  // // 6. Free the message digest context
  // EVP_MD_CTX_free(mdctx);

  // // Print the hash value
  // printf("SHA256 Hash of \"%s\": ", message);
  // for (unsigned int i = 0; i < md_len; i++) {
  //   printf("%02x", md_value[i]);
  // }
  // printf("\n");
}

} // namespace dlutils
