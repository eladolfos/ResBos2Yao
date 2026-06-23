# Try to find FFTW
# Defines:
#   FFTW_FOUND
#   FFTW_INCLUDE_DIR
#   FFTW_INCLUDE_DIRS (not cached)
#   FFTW_LIBRARY
#   FFTW_LIBRARIES (not cached)
#   FFTW_LIBRARY_DIR (not cached)

find_library(FFTW_LIBRARY NAMES fftw3
    HINTS $ENV{FFTW_ROOT_DIR}/lib ${FFTW_ROOT_DIR}/lib)

find_path(FFTW_INCLUDE_DIR fftw3.h
    HINTS $ENV{FFTW_ROOT_DIR}/include ${FFTW_ROOT_DIR}/include)

mark_as_advanced(FFTW_LIBRARY FFTW_INCLUDE_DIR)

# handle QUIETLY and REQUIRED arguments and set FFTW_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FFTW DEFAULT_MSG FFTW_INCLUDE_DIR FFTW_LIBRARY)

set(FFTW_LIBRARIES ${FFTW_LIBRARY})
get_filename_component(FFTW_LIBRARY_DIRS ${FFTW_LIBRARY} PATH)

set(FFTW_INCLUDE_DIRS ${FFTW_INCLUDE_DIR})

mark_as_advanced(FFTW_FOUND)

