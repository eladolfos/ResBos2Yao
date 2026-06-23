# Try to find Hoppet
# Defines:
#   Hoppet_FOUND
#   Hoppet_INCLUDE_DIR
#   Hoppet_INCLUDE_DIRS (not cached)
#   Hoppet_LIBRARY
#   Hoppet_LIBRARIES (not cached)
#   Hoppet_LIBRARY_DIR (not cached)

find_library(Hoppet_LIBRARY NAMES hoppet_v1
    HINTS $ENV{Hoppet_ROOT_DIR}/lib ${Hoppet_ROOT_DIR}/lib)

IF(${Hoppet_LIBRARY} MATCHES "Hoppet_LIBRARY-NOTFOUND")
    FIND_PROGRAM(Hoppet_CONFIG_EXECUTABLE NAMES hoppet-config
        HINTS $ENV{Hoppet_ROOT_DIR}/bin ${Hoppet_ROOT_DIR}/bin)
    IF(${Hoppet_CONFIG_EXECUTABLE} MATCHES "Hoppet_CONFIG_EXECUTABLE-NOTFOUND")
        MESSAGE(STATUS "Looking for Hoppet... hoppet-config executable not found")
    ELSE(${Hoppet_CONFIG_EXECUTABLE} MATCHES "Hoppet_CONFIG_EXECUTABLE-NOTFOUND")
        MESSAGE(STATUS "Looking for Hoppet... using hoppet-config executable")
        EXEC_PROGRAM(${Hoppet_CONFIG_EXECUTABLE} ARGS "--prefix" OUTPUT_VARIABLE Hoppet_PREFIX)
        find_library(Hoppet_LIBRARY NAMES hoppet_v1 PATHS ${Hoppet_PREFIX}/lib)
    ENDIF(${Hoppet_CONFIG_EXECUTABLE} MATCHES "Hoppet_CONFIG_EXECUTABLE-NOTFOUND")
ENDIF(${Hoppet_LIBRARY} MATCHES "Hoppet_LIBRARY-NOTFOUND")

find_path(Hoppet_INCLUDE_DIR hoppet_v1.h
    HINTS $ENV{Hoppet_ROOT_DIR}/include ${Hoppet_ROOT_DIR}/include ${Hoppet_PREFIX}/include)

mark_as_advanced(Hoppet_LIBRARY Hoppet_INCLUDE_DIR)

# handle QUIETLY and REQUIRED arguments and set hoppet_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Hoppet DEFAULT_MSG Hoppet_INCLUDE_DIR Hoppet_LIBRARY)

set(Hoppet_LIBRARIES ${hoppet_LIBRARY})
get_filename_component(Hoppet_LIBRARY_DIRS ${Hoppet_LIBRARY} PATH)

set(Hoppet_INCLUDE_DIRS ${Hoppet_INCLUDE_DIR})

mark_as_advanced(Hoppet_FOUND)

