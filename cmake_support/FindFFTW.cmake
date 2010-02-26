# Try to find FFTW
# adapted from FINDEIGEN.cmake of OpenStructure by Juergen Haas <juergen.haas@unibas.ch>
# license see FINDEIGEN.cmake 
#
# - Find FFTW
# Find the native FFTW includes and library
# 
#  FFTW_INCLUDE_PATH - where to find gsl/gsl_*.h, etc.
#  FFTW_LIBRARIES   - List of libraries when using FFTW.
#  FFTW_FOUND       - True if FFTW found.

if (FFTW_INCLUDE_PATH)
 set(FFTW_FOUND TRUE)
else (FFTW_INCLUDE_PATH)
  find_path (FFTW_INCLUDE_PATH fftw3.h)
  if (_DOUBLE_PREC)
    find_library (FFTW_LIBRARIES NAMES fftw3)
  elseif(NOT _DOUBLE_PREC)
    find_library (FFTW_LIBRARIES NAMES fftw3f)
  endif()
endif (FFTW_INCLUDE_PATH)

# handle the QUIETLY and REQUIRED arguments and set FFTW_FOUND to TRUE if 
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (FFTW DEFAULT_MSG FFTW_LIBRARIES FFTW_INCLUDE_PATH)

mark_as_advanced (FFTW_LIBRARIES FFTW_INCLUDE_PATH)
