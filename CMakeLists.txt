#-------------------------------------------------------------------------------
# Author: Marco Biasini
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)
project(OpenStructure CXX C)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake_support)
include(OST)


option(USE_SHADER "whether to compile with shader support"
       OFF)
option(SET_RPATH "embed rpath upon make install"
       OFF)
option(COMPILE_TMTOOLS "whether to compile the tmalign and tmscore programs"
       OFF)
option(PROFILE "whether to compile with profiling support"
       OFF)       
option(ENABLE_GUI "whether the graphical user interface should be enabled"
       ON)
option(ENABLE_GFX "whether graphics support should be enabled"
      ON)
option(ENABLE_IMG "whether the image processing module should be compiled"
       ON)
option(USE_DOUBLE_PRECISION "whether to compile in double precision" 
       OFF)
option(ENABLE_SPNAV "whether 3DConnexion devices should be supported"
      OFF)
option(STATIC_PROPERTY_WORKAROUND "workaround for static property bug with some boost/boost_python combinations" OFF)
option(DEPLOYMENT "switch on deployment settings" OFF)
option(COMPILE_TESTS "wheter unit tests should be compiled by default" OFF)
option(ENABLE_STATIC "whether static libraries should be compiled" OFF)
if (CXX)
  set(CMAKE_CXX_COMPILER ${CXX})
endif()

if (CC)
  set(CMAKE_C_COMPILER ${CC})
endif()

if (FORTRAN_COMPILER)
  set(CMAKE_FORTRAN_COMPILER ${FORTRAN_COMPILER})
endif()


if (PREFIX)
  set(CMAKE_INSTALL_PREFIX ${PREFIX})
endif()

if (SYS_ROOT)
  set(CMAKE_PREFIX_PATH ${SYS_ROOT})
endif()

if (OPTIMIZE)
  set(CMAKE_BUILD_TYPE Release)
  set(_OPT ON)
else()
   set(CMAKE_BUILD_TYPE Debug)
    set(_OPT OFF)
endif()

if (USE_DOUBLE_PRECISION)
  set(_DOUBLE_PREC ON)
else()
  set(_DOUBLE_PREC OFF)
  set(_DOUBLE_PREC OFF)
endif()

if (NOT ENABLE_GFX)
  set(ENABLE_GUI OFF)
endif()

if (ENABLE_GUI)
  set(_UI ON)
else()
  set(_UI OFF)
endif()

if (ENABLE_GFX)
  set(_OPENGL ON)
else()
  set(_OPENGL OFF)
endif()

if (USE_SHADER)
  set(_SHADER ON)
else()
  set(_SHADER OFF)
endif()
if (COMPILE_TMTOOLS)
  set(_TM_TOOLS ON)
else()
  set(_TM_TOOLS OFF)
endif()
if (ENABLE_IMG)
  set(_IMG ON)
else()
  set(_IMG OFF)
endif()
if (PROFILE)
  set(_PROFILE ON)
else()
  set(_PROFILE OFF)
endif()

add_definitions(-DEIGEN2_SUPPORT)

if (COMPOUND_LIB)
  set(_COMP_LIB "${COMPOUND_LIB}")
  if (NOT IS_ABSOLUTE "${COMPOUND_LIB}")
    set(COMPOUND_LIB "${CMAKE_CURRENT_SOURCE_DIR}/${COMPOUND_LIB}")
  endif()
else()
  set(_COMP_LIB "NONE")
endif()

if (STATIC_PROPERTY_WORKAROUND)
  set(_STATICPROPS ON)
else()
  set(_STATICPROPS OFF)
endif()

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

setup_stage()
file(MAKE_DIRECTORY ${STAGE_DIR}
     ${EXECUTABLE_OUTPUT_PATH}
     ${HEADER_STAGE_PATH}
     ${LIB_STAGE_PATH}
     ${LIBEXEC_STAGE_PATH})

setup_compiler_flags()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY)

setup_boost()
        
find_package(Qt4 4.5.0 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)
find_package(Eigen 2.0.0 REQUIRED)
find_package(Python 2.4 REQUIRED)

if (ENABLE_IMG)
  find_package(FFTW REQUIRED)
  find_package(TIFF REQUIRED)
endif()


if (ENABLE_STATIC)
  set(Boost_LIBRARIES)
  set(Boost_USE_STATIC_LIBS ON)
  find_package(Boost ${_BOOST_MIN_VERSION} 
               COMPONENTS filesystem system iostreams regex REQUIRED)
  message(${Boost_LIBRARIES})
  find_package(ZLIB REQUIRED)
  set(STATIC_LIBRARIES ${Boost_LIBRARIES} ${FFTW_LIBRARIES} ${TIFF_LIBRARIES} 
      ${PNG_LIBRARIES} ${ZLIB_LIBRARIES})
endif()

if (ENABLE_GUI)
  ost_find_python_module(sip)
  ost_find_python_module(PyQt4)
endif()

if (ENABLE_SPNAV)
  find_package(SpNav REQUIRED)
endif()

# basic environment
include_directories(${Boost_INCLUDE_DIRS} 
                    ${FFTW_INCLUDE_PATH} 
                    ${EIGEN2_INCLUDE_DIR}
                    ${TIFF_INCLUDE_DIR}
                    ${SPNAV_INCLUDE_DIR}
                    ${PNG_INCLUDE_DIR}
                    )
if (UNIX)
  SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  if(USE_RPATH)
    set(_USE_RPATH ON)
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LIB_DIR}")
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  else()
    set(_USE_RPATH OFF)
   SET(CMAKE_INSTALL_RPATH "")
   SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
  endif() 
endif()


add_subdirectory(modules)
add_subdirectory(scripts)
add_subdirectory(deployment)
set(FILES_TO_BE_REMOVED CMakeFiles stage tests)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
                         "${FILES_TO_BE_REMOVED}")

#ost_match_boost_python_version(${PYTHON_LIBRARIES})

message(STATUS 
        "OpenStructure will be built with the following options:\n"
        "   Install Prefix                 (-DPREFIX) : ${CMAKE_INSTALL_PREFIX}\n"
        "   RPath in install            (-DUSE_RPATH) : ${_USE_RPATH}\n"
        "   Graphical interface        (-DENABLE_GUI) : ${_UI}\n"
        "   OpenGL support             (-DENABLE_GFX) : ${_OPENGL}\n"
        "   Image Processing support   (-DENABLE_IMG) : ${_IMG}\n"
        "   Shader support             (-DUSE_SHADER) : ${_SHADER}\n"
        "   Optimize                     (-DOPTIMIZE) : ${_OPT}\n"
        "   Profiling support             (-DPROFILE) : ${_PROFILE}\n"
        "   Double Precision (-DUSE_DOUBLE_PRECISION) : ${_DOUBLE_PREC}\n"
        "   Compound Lib             (-DCOMPOUND_LIB) : ${_COMP_LIB}\n"
        "   TMAlign and TMScore   (-DCOMPILE_TMTOOLS) : ${_TM_TOOLS}\n"
        "   Static Libraries       (-DENABLE_STATIC)  : ${ENABLE_STATIC}")
