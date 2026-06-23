# Install script for directory: /mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/src

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosUtilities.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosUtilities.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosUtilities.so"
         RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Utilities/libResbosUtilities.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosUtilities.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosUtilities.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosUtilities.so"
         OLD_RPATH "/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/fmt-build:/mnt/home/fuyao3/ROOT_6_30_02/lib:::::::::::::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/software-current/2023.06/x86_64/generic/software/binutils/2.40-GCCcore-13.2.0/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosUtilities.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosBeam.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosBeam.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosBeam.so"
         RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/GCC13/LHAPDF/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Beam/libResbosBeam.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosBeam.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosBeam.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosBeam.so"
         OLD_RPATH "/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Utilities:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/fmt-build:/mnt/home/fuyao3/GCC13/LHAPDF/lib:"
         NEW_RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/GCC13/LHAPDF/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/software-current/2023.06/x86_64/generic/software/binutils/2.40-GCCcore-13.2.0/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosBeam.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosCalculation.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosCalculation.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosCalculation.so"
         RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Calculation/libResbosCalculation.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosCalculation.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosCalculation.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosCalculation.so"
         OLD_RPATH "/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Utilities:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/fmt-build:"
         NEW_RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/software-current/2023.06/x86_64/generic/software/binutils/2.40-GCCcore-13.2.0/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosCalculation.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosProcess.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosProcess.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosProcess.so"
         RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Process/libResbosProcess.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosProcess.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosProcess.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosProcess.so"
         OLD_RPATH "/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Utilities:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/fmt-build:"
         NEW_RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/software-current/2023.06/x86_64/generic/software/binutils/2.40-GCCcore-13.2.0/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosProcess.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosUser.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosUser.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosUser.so"
         RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/User/libResbosUser.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosUser.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosUser.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosUser.so"
         OLD_RPATH "/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/software-current/2023.06/x86_64/generic/software/binutils/2.40-GCCcore-13.2.0/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbosUser.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbos.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbos.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbos.so"
         RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/GCC13/LHAPDF/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/libResbos.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbos.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbos.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbos.so"
         OLD_RPATH "/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Beam:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Calculation:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Process:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/User:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/GCC13/LHAPDF/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Utilities:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/fmt-build:"
         NEW_RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/GCC13/LHAPDF/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/software-current/2023.06/x86_64/generic/software/binutils/2.40-GCCcore-13.2.0/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libResbos.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMCFMInterface.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMCFMInterface.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMCFMInterface.so"
         RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/GCC13/LHAPDF/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/libMCFMInterface.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMCFMInterface.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMCFMInterface.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMCFMInterface.so"
         OLD_RPATH "/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Beam:/mnt/home/fuyao3/GCC13/LHAPDF/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Calculation:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Process:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Utilities:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/User:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/fmt-build:"
         NEW_RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/GCC13/LHAPDF/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/software-current/2023.06/x86_64/generic/software/binutils/2.40-GCCcore-13.2.0/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMCFMInterface.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/resbos" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/resbos")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/resbos"
         RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/GCC13/LHAPDF/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/resbos")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/resbos" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/resbos")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/resbos"
         OLD_RPATH "/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Beam:/mnt/home/fuyao3/GCC13/LHAPDF/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Calculation:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Process:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Utilities:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/User:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/fmt-build:"
         NEW_RPATH "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib:/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/lib64:/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/ROOT_6_30_02/lib:/mnt/home/fuyao3/GCC13/LHAPDF/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/software-current/2023.06/x86_64/generic/software/binutils/2.40-GCCcore-13.2.0/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/resbos")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfmt.so.6.2.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfmt.so.6"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/fmt-build/libfmt.so.6.2.1"
    "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/fmt-build/libfmt.so.6"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfmt.so.6.2.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfmt.so.6"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/mnt/ffs24/home/fuyao3/ROOT_6_30_02/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/opt/software-current/2023.06/x86_64/generic/software/binutils/2.40-GCCcore-13.2.0/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/fmt-build/libfmt.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libyaml-cpp.so.0.6.3"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libyaml-cpp.so.0.6"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/yaml-cpp-build/libyaml-cpp.so.0.6.3"
    "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/yaml-cpp-build/libyaml-cpp.so.0.6"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libyaml-cpp.so.0.6.3"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libyaml-cpp.so.0.6"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/opt/software-current/2023.06/x86_64/generic/software/binutils/2.40-GCCcore-13.2.0/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/_deps/yaml-cpp-build/libyaml-cpp.so")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Utilities/cmake_install.cmake")
  include("/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Beam/cmake_install.cmake")
  include("/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Calculation/cmake_install.cmake")
  include("/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/Process/cmake_install.cmake")
  include("/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/User/cmake_install.cmake")
  include("/mnt/home/fuyao3/pku_resbos/SourceCode/resbos2/build/src/NonPertFit/cmake_install.cmake")

endif()

