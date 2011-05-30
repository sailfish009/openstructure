INCLUDE(InstallRequiredSystemLibraries)


set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OpenStructure Framework")
set(CPACK_PACKAGE_VENDOR "2008-2010 -- The OpenStructure Team")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/ReadMe.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
if (OST_REV)
  set(CPACK_PACKAGE_VERSION_PATCH "0a1_${OST_REV}")
else()
  set(CPACK_PACKAGE_VERSION_PATCH "0a1")
endif()
set(OST_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "openstructure ${OST_VERSION}" )

IF(WIN32 AND NOT UNIX)
# There is a bug in NSI that does not handle full unix paths properly. Make
# sure there is at least one set of four (4) backlashes.
  set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/graphics/\\\\icon_32.png")
  #set(CPACK_NSIS_INSTALLED_ICON_NAME "bin/\\\\gosty.exe")
  set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
  set(CPACK_NSIS_HELP_LINK "http:www.openstructure.org")
  #set(CPACK_NSIS_URL_INFO_ABOUT "http:www.my-personal-home-page.com")
  set(CPACK_NSIS_CONTACT "users@maillist.unibas.ch")
  set(CPACK_NSIS_MODIFY_PATH ON)
  set(CPACK_SOURCE_STRIP_FILES TRUE)
  set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/graphics/\\\\icon_32.png")
  set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "")
  set(CPACK_PACKAGE_EXECUTABLES "dng" "DNG")
  set(CPACK_PACKAGE_EXECUTABLES "gipltng" "gipltng")
  set(CPACK_INSTALL_DIRECTORIES "d:/progams/lib")
  # set(CPACK_STRIP_FILES "bin/gosty" 
                        # "bin/ost.bat"
                        # "bin/ost_cl.bat"
                        # "bin\gipltng.bat
  # )
ENDIF()

INCLUDE(CPack) #this must come last
