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

ExternalProject_Add(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
  URL_HASH
    SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
  CMAKE_ARGS -DCMAKE_POSITION_INDEPENDENT_CODE=On #
             -DCMAKE_CXX_STANDARD=17 #
             -DCMAKE_C_STANDARD_REQUIRED=Yes #
             -DCMAKE_INSTALL_PREFIX=${CMAKE_DEPS_PREFIX} #
             -DBUILD_GMOCK=On #
  PREFIX ${CMAKE_DEPS_PREFIX}
  EXCLUDE_FROM_ALL true
  DOWNLOAD_EXTRACT_TIMESTAMP On
  LOG_DOWNLOAD On
  LOG_CONFIGURE On
  LOG_BUILD On
  LOG_INSTALL On)

macro(import_static_lib_from LIBNAME LIB)
  add_library(${LIBNAME} STATIC IMPORTED)
  set_target_properties(
    ${LIBNAME}
    PROPERTIES IMPORTED_LOCATION
               ${CMAKE_DEPS_LIBDIR}/${LIBNAME}${CMAKE_STATIC_LIBRARY_SUFFIX})
  add_dependencies(${LIBNAME} ${LIB})
endmacro()

import_static_lib_from(libgtest googletest)
import_static_lib_from(libgtest_main googletest)
import_static_lib_from(libgmock_main googletest)
import_static_lib_from(libgmock googletest)

target_link_libraries(libgtest_main INTERFACE libgtest)
target_link_libraries(libgmock_main INTERFACE libgmock)

# -----------------------------
# Alias Target for External Use
# -----------------------------
add_library(Deps::gtest ALIAS libgtest_main)
add_library(Deps::gmock ALIAS libgmock_main)
