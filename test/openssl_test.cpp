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
#include <cstdio>
#include <cstring>

namespace dlutils {

TEST(OpenSSLTest, ShouldWork) {
  auto libcrypto = LibCrypto::GetInstance();
  EVP_MD_CTX *mdctx;
  const EVP_MD *md;
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len;
  const char *message = "Hello, OpenSSL Hashing!";

  // 1. Initialize the message digest context
  mdctx = libcrypto.EVP_MD_CTX_new();
  ASSERT_NE(mdctx, nullptr) << "Error creating EVP_MD_CTX";

  // 2. Select the hash algorithm (e.g., SHA256)
  md = libcrypto.EVP_sha256();
  ASSERT_NE(md, nullptr) << "Error getting SHA256 method";

  // 3. Initialize the digest operation
  ASSERT_EQ(libcrypto.EVP_DigestInit_ex(mdctx, md, nullptr), 1)
      << "Error initializing digest";

  // 4. Update the digest with the data
  ASSERT_EQ(libcrypto.EVP_DigestUpdate(mdctx, message, strlen(message)), 1)
      << "Error updating digest";

  // 5. Finalize the digest and retrieve the hash value
  ASSERT_EQ(libcrypto.EVP_DigestFinal_ex(mdctx, md_value, &md_len), 1)
      << "Error finalizing digest";

  // 6. Free the message digest context
  libcrypto.EVP_MD_CTX_free(mdctx);

  // Verify we got a hash of the expected length (SHA256 = 32 bytes)
  EXPECT_EQ(md_len, 32u);

  // Print the hash value
  printf("SHA256 Hash of \"%s\": ", message);
  for (unsigned int i = 0; i < md_len; i++) {
    printf("%02x", md_value[i]);
  }
  printf("\n");
}

} // namespace dlutils
