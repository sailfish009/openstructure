# - Try to find Eigen2 lib
# Once done this will define
#
#  EIGEN2_FOUND - system has eigen lib
#  EIGEN2_INCLUDE_DIRS - the eigen include directories

find_path(EIGEN2_INCLUDE_DIR NAMES Eigen/Core
          PATH_SUFFIXES eigen2
          PATH_SUFFIXES eigen3
)

set(EIGEN2_INCLUDE_DIRS ${EIGEN2_INCLUDE_DIR}) 

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (EIGEN DEFAULT_MSG EIGEN2_INCLUDE_DIR)

mark_as_advanced (EIGEN2_INCLUDE_DIR)





