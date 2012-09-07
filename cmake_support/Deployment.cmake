#-------------------------------------------------------------------------------
#  Author:  Andreas Schenk
#
#  This file contains the configuration for the bundle/installer creation
#  with CPack.
#
#-------------------------------------------------------------------------------


INCLUDE(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OpenStructure Framework")
set(CPACK_PACKAGE_VENDOR "2008-2010 -- The OpenStructure Team")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/ReadMe.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR ${OST_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${OST_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${OST_VERSION_PATCH})
if(APPLE)
  set(CPACK_GENERATOR Bundle)
  set(CPACK_BUNDLE_NAME DNG)
  set(CPACK_BUNDLE_PLIST ${PROJECT_SOURCE_DIR}/deployment/macos/Info.plist)
  set (CPACK_PACKAGE_ICON ${PROJECT_SOURCE_DIR}/graphics/icon.icns)
  set (CPACK_BUNDLE_ICON ${PROJECT_SOURCE_DIR}/graphics/icon.icns)
  install(CODE "if(NOT ${CMAKE_INSTALL_PREFIX} STREQUAL \${CMAKE_INSTALL_PREFIX})
                  #read ost_config to apply modifications
                  file(READ \"\${CMAKE_INSTALL_PREFIX}/libexec/openstructure/ost_config\" ost_config)
                  
                  set(OST_BINARIES \"\${CMAKE_INSTALL_PREFIX}/bin/lddt\"
                                   \"\${CMAKE_INSTALL_PREFIX}/bin/chemdict_tool\"
                                   \"\${CMAKE_INSTALL_PREFIX}/libexec/openstructure/gosty\")
                  
                  #copy python into bundle if a version different from the system version is used
                  if(NOT ${PYTHON_BINARY} MATCHES /usr/bin/)
                    get_filename_component(real_python_include_path ${PYTHON_INCLUDE_PATH} REALPATH)
                    get_filename_component(python_root_dir \${real_python_include_path}/../.. REALPATH) 
                    file(COPY \${python_root_dir}/${LIB_DIR}/python${PYTHON_VERSION}/
                         DESTINATION \${CMAKE_INSTALL_PREFIX}/${LIB_DIR}/python${PYTHON_VERSION})
                    file(COPY \${python_root_dir}/Resources/Python.app/Contents/MacOS/Python
                         DESTINATION \${CMAKE_INSTALL_PREFIX}/bin)
                    file(COPY ${PYTHON_INCLUDE_PATH}/pyconfig.h
                         DESTINATION \${CMAKE_INSTALL_PREFIX}/include/python${PYTHON_VERSION})

                    list(APPEND OST_BINARIES \"\${CMAKE_INSTALL_PREFIX}/bin/Python\")
                    string(REGEX REPLACE \"pyexec=\\\"[^\n\$]*\\\"\"
                                         \"pyexec=\$DNG_BINDIR/Python\"
                                         ost_config \"\${ost_config}\")
                  endif(NOT ${PYTHON_BINARY} MATCHES /usr/bin/)  
  
                  file(GLOB_RECURSE OST_PYMODS \"\${CMAKE_INSTALL_PREFIX}/${LIB_DIR}/*.so\")
                  list(APPEND  OST_BINARIES \${OST_PYMODS})
                  
                  #copy Qt stuff
                  file(COPY \"${QT_LIBRARY_DIR}/Resources/qt_menu.nib\"
                       DESTINATION \"\${CMAKE_INSTALL_PREFIX}/libexec/openstructure/\"
                       FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
                  file(COPY ${QT_PLUGINS_DIR}
                       DESTINATION \${CMAKE_INSTALL_PREFIX}/libexec/openstructure) 
                  file(GLOB_RECURSE QT_PLUGINS \"\${CMAKE_INSTALL_PREFIX}/libexec/openstructure/*.dylib\")
                  list(APPEND  OST_BINARIES \${QT_PLUGINS})
                  file(WRITE \"\${CMAKE_INSTALL_PREFIX}/libexec/openstructure/qt.conf\" \"[Paths]\nPlugins=../plugins\n\")

                  file(WRITE \"\${CMAKE_INSTALL_PREFIX}/../MacOS/dng\" \"#!/bin/sh\nexec \\\"\\\${0%/*}/../Resources/bin/dng\\\"\n\")
                  execute_process(COMMAND chmod ugo+x \"\${CMAKE_INSTALL_PREFIX}/../MacOS/dng\")
                  file(WRITE \"\${CMAKE_INSTALL_PREFIX}/libexec/openstructure/ost_config\" \"\${ost_config}\")
  
                  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake_support)
                  include(CopyDependencies)
                  copy_dependencies(\"\${OST_BINARIES}\") 
                 endif(NOT ${CMAKE_INSTALL_PREFIX} STREQUAL \${CMAKE_INSTALL_PREFIX})")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "openstructure ${OST_VERSION_STRING}" )
endif(APPLE)


if(${PROJECT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
  INCLUDE(CPack) #only include CPack for the main project and not for subprojects
endif(${PROJECT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
