#-------------------------------------------------------------------------------
# Check for OpenStructure Libraries
#
#    OPENSTRUCTURE_ROOT                 Prefix for OpenStructure libraries
#    OPENSTRUCTURE_MIN_VERSION          minimal OPenStructure version required
#  
# When OpenStructure is found, the result is placed in the following variables:
# 
#    OPENSTRUCTURE_LIBRARIES            is set to the library and linker flags used to
#                             link against python
#    OPENSTRUCTURE_VERSION              is set to the version of OpenStructure
#    OPENSTRUCTURE_INCLUDE_PATH         is set to the path that contains base.hh
#
# Author: Valerio Mariani,Marco Biasini
#-------------------------------------------------------------------------------

macro(find_OPENSTRUCTURE OPENSTRUCTURE_ROOT NAMES HEADER_NAMES)
  if(OPENSTRUCTURE_ROOT)
    find_library(OPENSTRUCTURE_LIBRARIES
      NAMES "${NAMES}"
      HINTS "${PYTHON_ROOT}"
      PATH_SUFFIXES lib
      NO_SYSTEM_ENVIRONMENT_PATH NO_DEFAULT_PATH
    )
    find_path(OPENSTRUCTURE_INCLUDE_PATH
      NAMES "${HEADER_NAMES}"
      HINTS "${OPENSTRUCTURE_ROOT}/include"
      NO_SYSTEM_ENVIRONMENT_PATH NO_DEFAULT_PATH
    )    
  else()
    find_library(OPENSTRUCTURE_LIBRARIES
      NAMES "${NAMES}"
      PATH_SUFFIXES lib
    )
    find_path(OPENSTRUCTURE_INCLUDE_PATH
      NAMES "${HEADER_NAMES}"
      PATH_SUFFIXES include      
    )
  endif()  
  if (OPENSTRUCTURE_INCLUDE_PATH)
    file(STRINGS ${OPENSTRUCTURE_INCLUDE_PATH}/boOPENSTRUCTURE/version.hh OPENSTRUCTURE_VERSIONSTR
         REGEX "#define[ ]+OPENSTRUCTURE_VERSION_STRING+[ ]+[0-9]\.[0-9]\.[0-9]")
    string(REGEX MATCH "[0-9].[0-9].[0-9]" OPENSTRUCTURE_VERSION ${OPENSTRUCTURE_VERSIONSTR})
  endif()  
endmacro()

#-------------------------------------------------------------------------------

find_OPENSTRUCTURE(OPENSTRUCTURE_ROOT "base" "OPENSTRUCTURE/config.hh")

mark_as_advanced(
  OPENSTRUCTURE_LIBRARIES
  OPENSTRUCTURE_INCLUDE_PATH
  OPENSTRUCTURE_VERSION
)

if (OPENSTRUCTURE_FOUND)
   if (NOT OPENSTRUCTURE_FIND_QUIETLY)
      message(STATUS "Found Openstructure: ${OPENSTRUCTURE_LIBRARY}")
   endif ()
else (FOO_FOUND)
   if (OPENSTRUCTURE_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find OpenStructure")
   endif ()
endif ()