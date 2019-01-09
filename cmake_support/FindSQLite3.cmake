# Try to find SQLite3
#
# Once done this will define
# - Find SQLite3
# Find the native SQLite3 includes and library
# 
#  SQLITE3_INCLUDE_DIRS - SQLite3 include dirs.
#  SQLITE3_LIBRARIES    - List of libraries when using SQLite3.
#  SQLITE3_FOUND        - True if SQLite3 found.

find_path(SQLITE3_INCLUDE_DIR NAMES sqlite3.h)
find_library(SQLITE3_LIBRARY NAMES sqlite3)

# handle the QUIETLY and REQUIRED arguments and set SQLITE3_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SQLITE3 DEFAULT_MSG SQLITE3_LIBRARY SQLITE3_INCLUDE_DIR)
IF(SQLITE3_FOUND)
	SET(SQLITE3_INCLUDE_DIRS ${SQLITE3_INCLUDE_DIR})
	SET(SQLITE3_LIBRARIES ${SQLITE3_LIBRARY})
ELSE(SQLITE3_FOUND)
	SET(SQLITE3_INCLUDE_DIRS)
	SET(SQLITE3_LIBRARIES)
ENDIF(SQLITE3_FOUND)

mark_as_advanced(SQLITE3_LIBRARY SQLITE3_INCLUDE_DIR)

