#-------------------------------------------------------------------------------
#  Author:  Andreas Schenk
#
#  This file contains the Linux specific CPack configuration.
#
#-------------------------------------------------------------------------------

include(CheckLSBTypes)

if(( ${LSB_DISTRIBUTOR_ID} MATCHES "centos" AND ${LSB_RELEASE} GREATER 5.9)
 OR (${LSB_DISTRIBUTOR_ID} MATCHES "redhat" AND ${LSB_RELEASE} GREATER 5.9)
 OR (${LSB_DISTRIBUTOR_ID} MATCHES "fedora"))
  set(CPACK_GENERATOR "RPM")
elseif(${LSB_DISTRIBUTOR_ID} MATCHES "debian")
  set(CPACK_GENERATOR "DEB")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "The OpenStructure developers")
else()
  install(CODE "
    if(NOT ${CMAKE_INSTALL_PREFIX} STREQUAL \${CMAKE_INSTALL_PREFIX})
      set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/ost/cmake_support)
      include(CopyDependencies)
      set(OST_BINARIES \"\${CMAKE_INSTALL_PREFIX}/bin/lddt\"
                       \"\${CMAKE_INSTALL_PREFIX}/bin/chemdict_tool\"
                       \"\${CMAKE_INSTALL_PREFIX}/bin/molck\"
                       \"\${CMAKE_INSTALL_PREFIX}/libexec/openstructure/gosty\")
      
      copy_python(${PYTHON_INCLUDE_PATH} ${PYTHON_VERSION} new_python_binary)
      list(APPEND OST_BINARIES \${new_python_binary})

      file(GLOB_RECURSE OST_PYMODS \"\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}/*.so\")
      list(APPEND  OST_BINARIES \${OST_PYMODS})
      
      #copy Qt stuff
      copy_qt(${QT_LIBRARY_DIR} ${QT_PLUGINS_DIR} qt_plugins)
      list(APPEND  OST_BINARIES \${qt_plugins})

      copy_dependencies(\"\${OST_BINARIES}\")
    endif(NOT ${CMAKE_INSTALL_PREFIX} STREQUAL \${CMAKE_INSTALL_PREFIX})
  ")
  set(CPACK_GENERATOR "TGZ")
  #set(CPACK_INSTALL_CMAKE_PROJECTS "")
endif()
