# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/extern_gtest-src"
  "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/extern_gtest-build"
  "/mnt/ffs24/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/extern_gtest-subbuild/extern_gtest-populate-prefix"
  "/mnt/ffs24/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/extern_gtest-subbuild/extern_gtest-populate-prefix/tmp"
  "/mnt/ffs24/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/extern_gtest-subbuild/extern_gtest-populate-prefix/src/extern_gtest-populate-stamp"
  "/mnt/ffs24/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/extern_gtest-subbuild/extern_gtest-populate-prefix/src"
  "/mnt/ffs24/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/extern_gtest-subbuild/extern_gtest-populate-prefix/src/extern_gtest-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/mnt/ffs24/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/extern_gtest-subbuild/extern_gtest-populate-prefix/src/extern_gtest-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/mnt/ffs24/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/extern_gtest-subbuild/extern_gtest-populate-prefix/src/extern_gtest-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
