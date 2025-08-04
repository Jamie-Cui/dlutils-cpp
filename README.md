# dlutils-cpp

[![Build and Coverage](https://github.com/Jamie-Cui/dlutils-cpp/actions/workflows/build-test-coverage.yml/badge.svg)](https://github.com/Jamie-Cui/dlutils-cpp/actions/workflows/build-test-coverage.yml)

A header-only C++ library for dynamic loading utilities that provides safe and convenient wrappers around `dlopen`/`dlsym` for dynamically loading shared libraries at runtime.

## Features

- **Header-only**: Easy integration with no linking required
- **Type-safe**: Template-based design ensures compile-time type checking
- **Exception-safe**: Automatic error handling with meaningful error messages
- **RAII-compliant**: Automatic resource management
- **Lightweight**: Minimal overhead and dependencies
- **Thread-aware**: Designed with threading considerations (note: `dlopen`/`dlsym` operations are not thread-safe)

## Installation

### Prerequisites

- C++17 compatible compiler
- CMake 3.20 or higher
- POSIX-compliant system with `dlfcn.h` (Linux, macOS, etc.)

### Using as a Header-Only Library

Simply copy the `include/dlutils` directory into your project and include the header:

```cpp
#include "dlutils/dlutils.hpp"
```

### Using with CMake

Add this repository as a submodule or download the source, then in your `CMakeLists.txt`:

```cmake
add_subdirectory(dlutils-cpp)
target_link_libraries(your_target PRIVATE dlutils-cpp)
```

## Usage

Here's a simple example showing how to load OpenSSL functions dynamically:

```cpp
#include "dlutils/dlutils.hpp"

class LibCrypto : public dlutils::DlLibBase {
public:
  // Declare the functions you want to load
  dlutils::DlFun<EVP_MD_CTX*> EVP_MD_CTX_new;
  dlutils::DlFun<const EVP_MD*> EVP_sha256;
  
  static LibCrypto& GetInstance() {
    static LibCrypto instance("libcrypto.so");
    return instance;
  }
  
  bool LoadFunctions() {
    SelfDlOpen();  // Load the library
    DLUTILS_SELF_DLSYM(EVP_MD_CTX_new);  // Load function
    DLUTILS_SELF_DLSYM(EVP_sha256);      // Load function
    return CheckFunCache();  // Verify all functions loaded successfully
  }
  
private:
  LibCrypto(const std::string& libName) : DlLibBase(libName) {}
};

// Usage
auto& libcrypto = LibCrypto::GetInstance();
if (libcrypto.LoadFunctions()) {
  // Use the functions safely
  EVP_MD_CTX* ctx = libcrypto.EVP_MD_CTX_new();
  const EVP_MD* md = libcrypto.EVP_sha256();
  // ... use ctx and md ...
}
```

For a complete working example, see [test/openssl.hpp](test/openssl.hpp) and [test/openssl_test.cpp](test/openssl_test.cpp).

## Building and Testing

### Building

```bash
mkdir build && cd build
cmake ..
make
```

### Running Tests

```bash
mkdir build && cd build
cmake ..
make
ctest
# Or run specific test
./openssl_test
```

### Generating Coverage Reports

```bash
mkdir build && cd build
cmake -DENABLE_COVERAGE=ON ..
make
ctest  # Run tests to generate coverage data
make coverage  # Generate coverage report
```

If `lcov` is installed, open `coverage_report/index.html` in your browser to view the report.
Otherwise, `gcov` files will be generated in the `gcov_report` directory.

### Continuous Integration

This project uses GitHub Actions for continuous integration. Code coverage reports are automatically generated for each pull request and push to the main branch. You can view the reports by downloading the artifacts from the workflow runs.

[![Build and Coverage](https://github.com/Jamie-Cui/dlutils-cpp/actions/workflows/coverage.yml/badge.svg)](https://github.com/Jamie-Cui/dlutils-cpp/actions/workflows/coverage.yml)

## Adding New Functions

1. Add a new `DlFun<>` member to your library class
2. Add the function name to the `LoadAll()` method using `DLUTILS_SELF_DLSYM(NAME)`
3. The template arguments for `DlFun` should match the function signature

## Adding New Libraries

1. Create a new class inheriting from `DlLibBase`
2. Define `DlFun` members for each function you want to load
3. Implement `LoadAll()` method to load all functions
4. Set the correct library name in the constructor

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
