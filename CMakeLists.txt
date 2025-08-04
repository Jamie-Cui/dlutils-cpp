# MIT License
#
# Copyright (c) 2025 Jamie Cui
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.20)

project(dlutils-cpp LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17) # Enable C++17

# Additional compiler flags

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

# Coverage flags
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)
if(ENABLE_COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage")
endif()

include(GNUInstallDirs)
include(ExternalProject)
include(CTest)

enable_testing()

set(CMAKE_DEPS_PREFIX ${CMAKE_BINARY_DIR}/deps)
set(CMAKE_DEPS_LIBDIR ${CMAKE_DEPS_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_DEPS_BINDIR ${CMAKE_DEPS_PREFIX}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_DEPS_INCLUDEDIR ${CMAKE_DEPS_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
set(CMAKE_DEPS_SRCDIR ${CMAKE_DEPS_PREFIX}/src)

include(gtest)

add_executable(openssl_test ${CMAKE_CURRENT_LIST_DIR}/test/openssl_test.cpp)
target_link_libraries(openssl_test PRIVATE Deps::gtest)
target_include_directories(
  openssl_test
  PUBLIC # these dirs are only used when building inside the build tree
         "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
         "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
         # these dirs are only used when linking against a prebuilt package
         "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
         ${PROJECT_SOURCE_DIR}/include
         ${CMAKE_DEPS_INCLUDEDIR})

add_test(NAME openssl_test COMMAND openssl_test)

# Add dlutils core tests
add_executable(dlutils_test ${CMAKE_CURRENT_LIST_DIR}/test/dlutils_test.cpp ${CMAKE_CURRENT_LIST_DIR}/test/dllibase_extended_test.cpp)
target_link_libraries(dlutils_test PRIVATE Deps::gtest)
target_include_directories(
  dlutils_test
  PUBLIC # these dirs are only used when building inside the build tree
         "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
         "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
         # these dirs are only used when linking against a prebuilt package
         "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
         ${PROJECT_SOURCE_DIR}/include
         ${CMAKE_DEPS_INCLUDEDIR})

add_test(NAME dlutils_test COMMAND dlutils_test)

# Coverage reporting target
if(ENABLE_COVERAGE)
  find_program(LCOV_PATH lcov)
  find_program(GENHTML_PATH genhtml)
  
  if(LCOV_PATH AND GENHTML_PATH)
    add_custom_target(coverage
      COMMAND ${LCOV_PATH} --capture --directory . --output-file coverage.info --ignore-errors mismatch
      COMMAND ${GENHTML_PATH} coverage.info --output-directory ${CMAKE_BINARY_DIR}/coverage_report
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Generating code coverage report..."
      VERBATIM)
  else()
    # Fallback to simple gcov output
    add_custom_target(coverage
      COMMAND ${CMAKE_COMMAND} -E echo "lcov and/or genhtml not found. Generating gcov files instead."
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/gcov_report
      COMMAND find . -name "*.gcda" -exec gcov -o ${CMAKE_BINARY_DIR}/gcov_report {} +
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Generating gcov coverage report..."
      VERBATIM)
  endif()
endif()
