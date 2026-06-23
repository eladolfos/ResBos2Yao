# Install script for directory: /mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/src/NonPertFit

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/opt/software-current/2023.06/x86_64/generic/software/binutils/2.40-GCCcore-13.2.0/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/NonPertFit" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/NonPertFit")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/NonPertFit"
         RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/GCC13/BAT/lib:/mnt/home/fuyao3/GCC13/LHAPDF/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/NonPertFit/NonPertFit")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/NonPertFit" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/NonPertFit")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/NonPertFit"
         OLD_RPATH "/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Beam:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Calculation:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Process:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Utilities:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/User:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/yaml-cpp-build:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/GCC13/BAT/lib:/mnt/home/fuyao3/GCC13/LHAPDF/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/fmt-build:"
         NEW_RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/GCC13/BAT/lib:/mnt/home/fuyao3/GCC13/LHAPDF/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/software-current/2023.06/x86_64/generic/software/binutils/2.40-GCCcore-13.2.0/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/NonPertFit")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/GetFixed" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/GetFixed")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/GetFixed"
         RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/GCC13/BAT/lib:/mnt/home/fuyao3/GCC13/LHAPDF/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/NonPertFit/GetFixed")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/GetFixed" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/GetFixed")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/GetFixed"
         OLD_RPATH "/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Beam:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Calculation:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Process:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Utilities:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/User:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/GCC13/BAT/lib:/mnt/home/fuyao3/GCC13/LHAPDF/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/fmt-build:"
         NEW_RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/GCC13/BAT/lib:/mnt/home/fuyao3/GCC13/LHAPDF/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/software-current/2023.06/x86_64/generic/software/binutils/2.40-GCCcore-13.2.0/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/GetFixed")
    endif()
  endif()
endif()

