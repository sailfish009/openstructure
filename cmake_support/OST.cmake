#-------------------------------------------------------------------------------
# Author: Marco Biasini, Juergen Haas
#
# This file contains a bunch of useful macros to facilitate the build-system
# configuration for the modules.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# check_architecture
#
# detect architecture based on void pointer size. the output is stored in the
# 3 variables OS_32_BITS, OS_64_BITS and CMAKE_NATIVE_ARCH. The former two are
# set to 0 and 1 accordingly and CMAKE_NATIVE_ARCH is set to the 32 or 64 bit.
#-------------------------------------------------------------------------------
macro(check_architecture)
  include(CheckTypeSize)
  check_type_size(void*  SIZEOF_VOID_PTR)
  if(${SIZEOF_VOID_PTR} MATCHES "^8$")
    set(OS_32_BITS 0)
    set(OS_64_BITS 1)
    set(CMAKE_NATIVE_ARCH 64)
  else()
    set(OS_32_BITS 1)
    set(OS_64_BITS 0)
    set(CMAKE_NATIVE_ARCH 32)
  endif()
endmacro()

#-------------------------------------------------------------------------------
# this macro has been adapted from
# http://www.cmake.org/Wiki/CMakeMacroParseArguments
#-------------------------------------------------------------------------------
macro(parse_argument_list PREFIX ARG_NAMES OPT_NAMES)
  set(_DEFAULT_ARGS)
  # reset variables
  foreach(${_ARG_NAME} ${ARG_NAMES})
    set(PREFIX_${_ARG_NAME})
  endforeach()
  foreach(_OPT_NAME ${OPT_NAMES})
    set(PREFIX_${_OPT_NAME} FALSE)
  endforeach()
  set(_CURR_ARG_NAME DEF_ARGS)
  set(_CURR_ARG_LIST)
  # loop over parameter list and split by ARG_NAMES
  foreach(_ARG ${ARGN})
    set(_LARG_NAMES ${ARG_NAMES})  
    list(FIND _LARG_NAMES ${_ARG} _IS_ARG_NAME)
    if (_IS_ARG_NAME GREATER -1)
      set(${PREFIX}_${_CURR_ARG_NAME} ${_CURR_ARG_LIST})
      set(_CURR_ARG_NAME "${_ARG}")
      set(_CURR_ARG_LIST)
    else()
    set(_LOPT_NAMES ${OPT_NAMES})  
      list(FIND _LOPT_NAMES ${_ARG} _IS_OPT_NAME)
      if (_IS_OPT_NAME GREATER -1)
        set(${PREFIX}_${_ARG} TRUE)
      else()
        list(APPEND _CURR_ARG_LIST "${_ARG}")
      endif()
    endif()
  endforeach()
  set(${PREFIX}_${_CURR_ARG_NAME} ${_CURR_ARG_LIST})
endmacro()

#-------------------------------------------------------------------------------
# copy_if_different
#
# copies file from source directory to destination directory, but only if its
# content changed.
#-------------------------------------------------------------------------------
macro(copy_if_different FROM_DIR TO_DIR FILES TARGETS TARGET)
  set(ADD_TARGETS "")
  foreach(SRC ${FILES})
      set(SRCFILE ${SRC})
      if("${FROM_DIR}" STREQUAL "" OR "${FROM_DIR}" STREQUAL "./")
          set(FROM ${SRC})
      else()
          set(FROM ${FROM_DIR}/${SRC})
      endif()
      if("${TO_DIR}" STREQUAL "")
          set(TO ${SRCFILE})
      else()
          get_filename_component(TOFILE ${SRC} NAME)      
          set(TO ${TO_DIR}/${TOFILE})
      endif()
      file(MAKE_DIRECTORY  ${TO_DIR})
      add_custom_command(TARGET "${TARGET}" PRE_BUILD
          DEPENDS ${FROM}
          COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FROM} ${TO}
          COMMENT ""
          )
      list(APPEND ADD_TARGETS ${TO})
  endforeach()
  if (TARGETS)
    set(${TARGETS} ${ADD_TARGETS})
  endif()
endmacro()

#-------------------------------------------------------------------------------
# stage_headers
#-------------------------------------------------------------------------------
macro(stage_headers HEADERS HEADER_INSTALL_DIR TARGET SUB)
  set(FROM_DIR "./")
  # introduce a helper target to make sure the headers are staged before
  # building the library
  string(REPLACE "/" "_" _SUB_NO_SLASH "${SUB}")
  string(REPLACE "${PREFIX}_" "" _TARGET "${TARGET}")
  #message("target before: ${TARGET} after: ${_TARGET}")
  set(_TARGET_NAME ${_TARGET}_${_SUB_NO_SLASH}_headers)
  set(_SUB ${SUB})
  if (NOT _SUB)
    set(_TARGET_NAME ${_TARGET}_headers)
  endif()
  add_custom_target("${_TARGET_NAME}" COMMENT "")
  set(HEADER_DIR "${HEADER_STAGE_PATH}/${HEADER_INSTALL_DIR}")
  copy_if_different("${FROM_DIR}" "${HEADER_DIR}"
                    "${HEADERS}" ""
                    "${_TARGET_NAME}")
  add_dependencies("${TARGET}" "${_TARGET_NAME}")
  set_target_properties("${_TARGET_NAME}" PROPERTIES
                        EchoString "Staging headers ${TARGET}")
endmacro()

#-------------------------------------------------------------------------------
# Synopsis:
#   module(NAME name SOURCES source1 source2 HEADERS header1 header2 
#          [IN_DIR dir] [header3 header4 [IN_DIR dir]] [DEPENDS_ON dep1 dep2]
#          [HEADER_OUTPUT_DIR dir]
#          [LINK link_cmds])
# Description:
#   Define an OpenStructure module.
#-------------------------------------------------------------------------------
macro(module)
  #-----------------------------------------------------------------------------
  # deal with arguments
  #-----------------------------------------------------------------------------
  set(_ARGS "NAME;SOURCES;HEADERS;DEPENDS_ON;LINK;HEADER_OUTPUT_DIR;PREFIX")
  set(_ARG_PREFIX ost)  
  parse_argument_list(_ARG "${_ARGS}" "NO_STATIC" ${ARGN})  
  if (NOT _ARG_NAME)
    message(FATAL_ERROR 
            "invalid use of module(): a module name must be provided")
  endif()


  if (_ARG_HEADER_OUTPUT_DIR)
    set(_HEADER_OUTPUT_DIR ${_ARG_HEADER_OUTPUT_DIR})
  else()
    if (_ARG_PREFIX)
      set(_HEADER_OUTPUT_DIR "${_ARG_PREFIX}/${_ARG_NAME}")
    else()
      set(_HEADER_OUTPUT_DIR "${_ARG_NAME}")
    endif()
  endif()
  if (_ARG_PREFIX)
    set(_LIB_NAME ${_ARG_PREFIX}_${_ARG_NAME})
  else()
    set(_LIB_NAME ${_ARG_NAME})
  endif()
  string(TOUPPER ${_LIB_NAME} _UPPER_LIB_NAME)  
  #-----------------------------------------------------------------------------
  # create library  
  #-----------------------------------------------------------------------------
  file(MAKE_DIRECTORY ${LIB_STAGE_PATH})
  if (WIN32)
    set(_ABS_FILE_PATTERN "^[A-Z]:/")
  else()
    set(_ABS_FILE_PATTERN "^/")
  endif()
  if (_ARG_SOURCES)
    # when there is at least one source file, we build a library
    set(_ABS_SOURCE_NAMES)
    foreach(_SOURCE ${_ARG_SOURCES})
      if (_SOURCE MATCHES ${_ABS_FILE_PATTERN})
        list(APPEND _ABS_SOURCE_NAMES "${_SOURCE}")
      else()
        list(APPEND _ABS_SOURCE_NAMES "${CMAKE_CURRENT_SOURCE_DIR}/${_SOURCE}")
      endif()
    endforeach()
    add_library(${_LIB_NAME} SHARED ${_ABS_SOURCE_NAMES})

    set_target_properties(${_LIB_NAME} 
                          PROPERTIES OUTPUT_NAME ${_LIB_NAME}
                                     PROJECT_LABEL ${_ARG_NAME}
                                     EchoString   ${_ARG_NAME}
                                     MODULE_DEPS "${_ARG_DEPENDS_ON}")
    get_target_property(_DEFS ${_LIB_NAME} COMPILE_DEFINITIONS)
    set_target_properties(${_LIB_NAME} PROPERTIES
                          COMPILE_DEFINITIONS OST_MODULE_${_UPPER_LIB_NAME})
    set_target_properties(${_LIB_NAME} PROPERTIES
                          VERSION ${OST_VERSION_STRING}
                          SOVERSION ${OST_VERSION_MAJOR}.${OST_VERSION_MINOR})    
    set_target_properties(${_LIB_NAME} PROPERTIES
                          LIBRARY_OUTPUT_DIRECTORY ${LIB_STAGE_PATH}
                          ARCHIVE_OUTPUT_DIRECTORY ${LIB_STAGE_PATH}
                          RUNTIME_OUTPUT_DIRECTORY ${LIB_STAGE_PATH})
    if (ENABLE_STATIC AND NOT _ARG_NO_STATIC)
      add_library(${_LIB_NAME}_static STATIC ${_ABS_SOURCE_NAMES})
      set_target_properties(${_LIB_NAME}_static
                            PROPERTIES OUTPUT_NAME ${_LIB_NAME}
                                       PROJECT_LABEL ${_ARG_NAME}
                                       EchoString   ${_ARG_NAME}
                                       MODULE_DEPS "${_ARG_DEPENDS_ON}")
      get_target_property(_DEFS ${_LIB_NAME}_static COMPILE_DEFINITIONS)
      set_target_properties(${_LIB_NAME}_static PROPERTIES
                            COMPILE_DEFINITIONS OST_MODULE_${_UPPER_LIB_NAME})
      set_target_properties(${_LIB_NAME}_static PROPERTIES
                            LIBRARY_OUTPUT_DIRECTORY ${LIB_STAGE_PATH}
                            ARCHIVE_OUTPUT_DIRECTORY ${LIB_STAGE_PATH}
                            RUNTIME_OUTPUT_DIRECTORY ${LIB_STAGE_PATH})
      foreach(_DEPENDENCY ${_ARG_DEPENDS_ON})
        target_link_libraries(${_LIB_NAME}_static ${_DEPENDENCY}_static)
      endforeach()
      target_link_libraries(${_LIB_NAME}_static ${ZLIB_LIBRARIES} ${_ARG_LINK})
    endif()
    if (APPLE)
      set_target_properties(${_LIB_NAME} PROPERTIES
                            LINK_FLAGS "-Wl,-rpath,@@loader_path"
                            INSTALL_NAME_DIR "@rpath")
    endif()
    if (WIN32)
      #set_target_properties(${_LIB_NAME} PROPERTIES PREFIX "../")
      install(TARGETS ${_LIB_NAME} ARCHIVE DESTINATION "${LIB_DIR}")
    else()
      install(TARGETS ${_LIB_NAME} LIBRARY DESTINATION "${LIB_DIR}")
    endif()                          
    if (_ARG_LINK)
      target_link_libraries(${_LIB_NAME} ${_ARG_LINK})
    endif()
    foreach(_DEPENDENCY ${_ARG_DEPENDS_ON})
      target_link_libraries(${_LIB_NAME} ${_DEPENDENCY})
    endforeach()
  else()
    add_custom_target("${_LIB_NAME}" ALL)
    set_target_properties("${_LIB_NAME}" PROPERTIES HEADER_ONLY 1 
                          MODULE_DEPS "${_ARG_DEPENDS_ON}")
  endif()
  #-----------------------------------------------------------------------------
  # stage headers  
  #-----------------------------------------------------------------------------
  if (_ARG_HEADERS)
    set(_HEADERS)
    set(_EXPECT_IN_DIR FALSE)
    foreach(_HEADER ${_ARG_HEADERS})
      if (_HEADER STREQUAL "IN_DIR")
        set(_EXPECT_IN_DIR TRUE)
      else()
        if (_EXPECT_IN_DIR)
          set(_EXPECT_IN_DIR FALSE)
          set(_DIR ${_HEADER})
          set(_ABS_HEADER_NAMES)
          set(_HDR_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${_DIR}")
          foreach(_HDR ${_HEADERS})
            list(APPEND _ABS_HEADER_NAMES "${_HDR_SOURCE_DIR}/${_HDR}")
          endforeach()
          install(FILES ${_ABS_HEADER_NAMES} DESTINATION
                  "include/${_HEADER_OUTPUT_DIR}/${_DIR}")
          set(_HDR_STAGE_DIR "${_HEADER_OUTPUT_DIR}/${_DIR}")
          stage_headers("${_ABS_HEADER_NAMES}" "${_HDR_STAGE_DIR}" 
                        "${_LIB_NAME}" "${_DIR}")
          set(_HEADERS)
        else()
          list(APPEND _HEADERS "${_HEADER}")
        endif()
      endif()
    endforeach()
    list(LENGTH _HEADERS _HEADER_LIST_LENGTH)
    if (_HEADER_LIST_LENGTH GREATER 0)    
      set(_ABS_HEADER_NAMES)   
      set(_HDR_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")       
      foreach(_HDR ${_HEADERS})
        list(APPEND _ABS_HEADER_NAMES "${_HDR_SOURCE_DIR}/${_HDR}")
      endforeach()      
      install(FILES ${_ABS_HEADER_NAMES} DESTINATION
              "include/${_HEADER_OUTPUT_DIR}")
      set(_HDR_STAGE_DIR "${_HEADER_OUTPUT_DIR}")
      stage_headers("${_ABS_HEADER_NAMES}" "${_HDR_STAGE_DIR}" 
                    "${_LIB_NAME}" "")
    endif()
  endif()
endmacro()


#-------------------------------------------------------------------------------
# Synopsis
#   executable(NAME exe_name SOURCES source1 source2 LINK link1 link2)
#
# Description:
#  Compile, link and stage a C++ executable
#-------------------------------------------------------------------------------
macro(executable)
  parse_argument_list(_ARG 
                      "NAME;SOURCES;LINK;DEPENDS_ON" "NO_RPATH;STATIC" ${ARGN})
  if (NOT _ARG_NAME)
    message(FATAL_ERROR "invalid use of executable(): a name must be provided")
  endif()
  add_executable(${_ARG_NAME} ${_ARG_SOURCES})
  if (APPLE AND NOT _ARG_NO_RPATH AND NOT _ARG_STATIC)
    set_target_properties(${_ARG_NAME} PROPERTIES
                          LINK_FLAGS "-Wl,-rpath,@loader_path/../lib")
  endif()
  if (_ARG_LINK)
    target_link_libraries(${_ARG_NAME} ${_ARG_LINK})
  endif()
  if (ENABLE_STATIC AND _ARG_STATIC)
    set(TARGET_SUFFIX _static)
    target_link_libraries(${_ARG_NAME} ${STATIC_LIBRARIES})
  endif()
  foreach(_DEP ${_ARG_DEPENDS_ON})
    target_link_libraries(${_ARG_NAME} ${_DEP}${TARGET_SUFFIX})
  endforeach()
  install(TARGETS ${_ARG_NAME} DESTINATION bin)
endmacro()


#-------------------------------------------------------------------------------
# Synopsis
#   executable_libexec(NAME exe_name SOURCES source1 source2 LINK link1 link2)
#
# Description:
#  Compile, link and stage a C++ executable into the libexec directory
#-------------------------------------------------------------------------------
macro(executable_libexec)
  parse_argument_list(_ARG 
                      "NAME;SOURCES;LINK;DEPENDS_ON" "NO_RPATH;STATIC" ${ARGN})
  if (NOT _ARG_NAME)
    message(FATAL_ERROR "invalid use of executable(): a name must be provided")
  endif()   
  add_executable(${_ARG_NAME} ${_ARG_SOURCES})
  set_target_properties(${_ARG_NAME}
                        PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                       "${LIBEXEC_STAGE_PATH}")  
  if (APPLE AND NOT _ARG_NO_RPATH AND NOT _ARG_STATIC)
    set_target_properties(${_ARG_NAME} PROPERTIES
                          LINK_FLAGS "-Wl,-rpath,@loader_path/../lib")
  endif()
  if (_ARG_LINK)
    target_link_libraries(${_ARG_NAME} ${_ARG_LINK})
  endif()
  if (ENABLE_STATIC AND _ARG_STATIC)
    set(TARGET_SUFFIX _static)
    target_link_libraries(${_ARG_NAME} ${STATIC_LIBRARIES})
  endif()
  foreach(_DEP ${_ARG_DEPENDS_ON})
    target_link_libraries(${_ARG_NAME} ${_DEP}${TARGET_SUFFIX})
  endforeach()
  install(TARGETS ${_ARG_NAME} DESTINATION ${LIBEXEC_PATH})
endmacro()

#-------------------------------------------------------------------------------
# Synopsis:
#   substitute(IN_FILE in_file OUT_FILE out_file DICT a=b c=d)
#
#-------------------------------------------------------------------------------
macro(substitute)
 parse_argument_list(_ARG 
                     "IN_FILE;OUT_FILE;DICT" "" ${ARGN})
  if (NOT _ARG_IN_FILE)
    message(FATAL_ERROR "invalid use of substitute(): no IN_FILE given")
  endif()
  if (NOT _ARG_OUT_FILE)
    message(FATAL_ERROR "invalid use of substitute(): no OUT_FILE given")
  endif()
  set(_SUBST_DICT -DINPUT_FILE=${_ARG_IN_FILE} -DOUT_FILE=${_ARG_OUT_FILE})
  foreach(_SUBST ${_ARG_DICT})
    list(APPEND _SUBST_DICT -D${_SUBST})
  endforeach()
  add_custom_target(subst_${_ARG_OUT_FILE} ALL COMMAND 
                    ${CMAKE_COMMAND} ${_SUBST_DICT}
                    -P ${CMAKE_SOURCE_DIR}/cmake_support/substitute.cmake)
endmacro()

#-------------------------------------------------------------------------------
# Synopsis:
#   script(NAME script_name INPUT input_name SUBSTITUTE key=val key=val
#          [OUTPUT_DIR dir] [TARGET target])
#-------------------------------------------------------------------------------
macro(script)
  set(_ARG_OUTPUT_DIR bin)
  parse_argument_list(_ARG 
                      "NAME;INPUT;SUBSTITUTE;TARGET;OUTPUT_DIR" "" ${ARGN})
  if (NOT _ARG_NAME)
    message(FATAL_ERROR "invalid use of executable(): a name must be provided")
  endif()
  set(_INPUT ${_ARG_NAME})
  if (_ARG_INPUT)
    set(_INPUT ${_ARG_INPUT})
  endif()
  if (_ARG_SUBSTITUTE)
    if (NOT _ARG_INPUT)
      message(FATAL_ERROR "script() can only substitute when INPUT is present.")    
    endif()
    substitute(IN_FILE ${_INPUT} OUT_FILE ${_ARG_NAME} 
               DICT ${_ARG_SUBSTITUTE})
  endif()
  install(FILES ${_ARG_NAME} DESTINATION ${_ARG_OUTPUT_DIR} 
          PERMISSIONS WORLD_EXECUTE GROUP_EXECUTE OWNER_EXECUTE 
                      WORLD_READ GROUP_READ OWNER_READ)
  copy_if_different("./" "${STAGE_DIR}/${_ARG_OUTPUT_DIR}" 
                    "${_ARG_NAME}" "TARGETS" ${_ARG_TARGET})
  add_dependencies(${_ARG_TARGET} subst_${_ARG_NAME})
endmacro()

#-------------------------------------------------------------------------------
# Synopsis:
#   ui_to_python(module out_files [input_file1 ...])
# Description:
#   Calls pyuic on every input file. The resulting python files are stored in
#   the variable with name out_files.
#-------------------------------------------------------------------------------
macro(ui_to_python module out_files)
  set(_input_files ${ARGN})
  find_program(_PYUIC_EXECUTABLE
    NAMES pyuic4-${PYTHON_VERSION} pyuic4 pyuic
    PATHS  ENV PATH 
  )  
  if(NOT _PYUIC_EXECUTABLE)
    message(FATAL_ERROR "Could not find pyuic command in " ${QT_BINARY_DIR} " for python version " ${PYTHON_VERSION})
  endif(NOT _PYUIC_EXECUTABLE)

  foreach(input_file ${_input_files})
    get_filename_component(_out_file ${input_file} NAME_WE)
    get_filename_component(_in_file ${input_file} ABSOLUTE)
    set(_out_file ${CMAKE_CURRENT_SOURCE_DIR}/${_out_file}_ui.py)
    add_custom_command(TARGET ${module}
                       COMMAND ${_PYUIC_EXECUTABLE} -o ${_out_file} ${_in_file}
                       VERBATIM DEPENDS ${input_file}
                       )
    list(APPEND ${out_files} ${_out_file})
  endforeach()
endmacro()

#-------------------------------------------------------------------------------
# Synopsis:
#   compile_py_files(module out_files [input_file1 ...])
# Description:
#   Calls pyuic on every input file. The resulting python files are stored in
#   the variable with name out_files.
#-------------------------------------------------------------------------------
macro(compile_py_files module out_dir)
  set(_input_files ${ARGN})
  foreach(input_file ${_input_files})
    get_filename_component(_out_file ${input_file} NAME_WE)
    get_filename_component(_in_file ${input_file} ABSOLUTE)
    set(_out_file ${out_dir}/${_out_file}.pyc)
    get_filename_component(_in_name ${input_file} NAME)
    file(MAKE_DIRECTORY  ${out_dir})
    add_custom_command(TARGET ${module}
                       COMMAND ${PYTHON_BINARY} -c "import py_compile;py_compile.compile(\"${_in_file}\",\"${_out_file}\",\"${_in_name}\",doraise=True)"
                       VERBATIM DEPENDS ${input_file}
                       )
  endforeach()
endmacro()

#-------------------------------------------------------------------------------
# Synopsis:
#   pymod(NAME name CPP source1 source2 PY source source2 [IN_DIR dir] 
#         source3 source4 [IN_DIR dir] [LINK link] [OUTPUT_DIR dir] [UI user_interface_files])
#
# Description:
#  Define a python module consisting of C++ type wrappers and/or code written in 
#  Python.
# NAME is the name of
#-------------------------------------------------------------------------------
macro(pymod)
  #-----------------------------------------------------------------------------
  # deal with arguments
  #-----------------------------------------------------------------------------
  set(_ARG_PREFIX ost)
  parse_argument_list(_ARG 
                      "NAME;CPP;PY;LINK;OUTPUT_DIR;UI;PREFIX" "" ${ARGN})
  if (NOT _ARG_NAME)
    message(FATAL_ERROR "invalid use of pymod(): a name must be provided")
  endif()
  if (_ARG_OUTPUT_DIR)
    set(PYMOD_DIR "openstructure/${_ARG_OUTPUT_DIR}")
  else()
    set(PYMOD_DIR "openstructure/${_ARG_PREFIX}/${_ARG_NAME}")
  endif()
  set(_LIB_NAME ${_ARG_PREFIX}_${_ARG_NAME})
  set(PYMOD_STAGE_DIR "${LIB_STAGE_PATH}/${PYMOD_DIR}")
  file(MAKE_DIRECTORY ${PYMOD_STAGE_DIR})
  #-----------------------------------------------------------------------------
  # compile and link C++ wrappers
  #-----------------------------------------------------------------------------
  if (_ARG_CPP)
    add_library("_${_LIB_NAME}" MODULE ${_ARG_CPP})
    set_target_properties("_${_LIB_NAME}"
                          PROPERTIES ECHO_STRING
                          "Building Python Module ${_ARG_NAME}")
    if (_ARG_PREFIX)
      set(_PARENT_NAME "${_ARG_PREFIX}_${_ARG_NAME}")
    else()
      set(_PARENT_NAME "${_ARG_NAME}")
    endif()
    get_target_property(_CUSTOM_CHECK "${_PARENT_NAME}" HEADER_ONLY)
    if (NOT _CUSTOM_CHECK)
      set(_PARENT_LIB_NAME "${_PARENT_NAME}")
    endif()
    target_link_libraries("_${_LIB_NAME}" ${_PARENT_LIB_NAME} 
                          ${PYTHON_LIBRARIES} ${BOOST_PYTHON_LIBRARIES})
    if (_USE_RPATH)
      set_target_properties("_${_LIB_NAME}"
                            PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PYMOD_STAGE_DIR}
                            INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LIB_DIR}")
    else()
      set_target_properties("_${_LIB_NAME}"
                            PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PYMOD_STAGE_DIR}
                            INSTALL_RPATH "")
    endif()
    if (APPLE)
      file(RELATIVE_PATH _REL_PATH "${PYMOD_STAGE_DIR}" "${LIB_STAGE_PATH}")
      set_target_properties("_${_LIB_NAME}" PROPERTIES
                            LINK_FLAGS "-Wl,-rpath,@loader_path/${_REL_PATH}"
                            INSTALL_NAME_DIR "@rpath")
    endif()                          
    if (NOT WIN32)
      set_target_properties("_${_LIB_NAME}"
                          PROPERTIES PREFIX "")
    else ()
      set_target_properties("_${_LIB_NAME}"
                          PROPERTIES PREFIX "../")

      set_target_properties("_${_LIB_NAME}"
                          PROPERTIES SUFFIX ".pyd")

    endif()
    install(TARGETS "_${_LIB_NAME}" LIBRARY DESTINATION
            "${LIB_DIR}/${PYMOD_DIR}")
  else()
    add_custom_target("_${_LIB_NAME}" ALL)
  endif()
  set(_PY_FILES)  
  if (_ARG_UI)
    add_custom_target("${_LIB_NAME}_ui")    
    ui_to_python("${_LIB_NAME}_ui" _PY_FILES ${_ARG_UI})
  endif()  
  if (_ARG_PY OR _PY_FILES)
    set(_EXPECT_IN_DIR FALSE)
    foreach(_PY_FILE ${_ARG_PY})
      if (_PY_FILE STREQUAL "IN_DIR")
        set(_EXPECT_IN_DIR TRUE)
      else()
        if (_EXPECT_IN_DIR)
          set(_EXPECT_IN_DIR FALSE)
          set(_DIR ${_PY_FILE})
          set(_ABS_PY_FILES)
          set(_PY_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${_DIR}")
          foreach(_PY ${_PY_FILES})
            list(APPEND _ABS_PY_FILES "${_PY_SOURCE_DIR}/${_PY}")
          endforeach()
          install(FILES ${_ABS_PY_FILES} DESTINATION
                  "${LIB_DIR}/${PYMOD_DIR}/${_DIR}")
          string(REPLACE "/" "_" _DIR_NO_SLASH "${_DIR}")
          add_custom_target("${_ARG_NAME}_${_DIR_NO_SLASH}_pymod" ALL)
          copy_if_different("./" "${PYMOD_STAGE_DIR}/${_DIR}"
                            "${_ABS_PY_FILES}" "TARGETS"
                            "${_ARG_NAME}_${_DIR_NO_SLASH}_pymod")
          compile_py_files(_${_LIB_NAME} ${PYMOD_STAGE_DIR}/${_DIR} ${_ABS_PY_FILES})
          set(_PY_FILES)
        else()
          list(APPEND _PY_FILES "${_PY_FILE}")
        endif()
      endif()
    endforeach()
    if (_PY_FILES)
      add_custom_target("${_LIB_NAME}_pymod" ALL)
      if (_ARG_UI)
        add_dependencies("${_LIB_NAME}_pymod" "${_LIB_NAME}_ui"})
      endif()
      copy_if_different("./" "${PYMOD_STAGE_DIR}" "${_PY_FILES}" "TARGETS"
                        "${_LIB_NAME}_pymod")
      add_dependencies("_${_LIB_NAME}" "${_LIB_NAME}_pymod")
      compile_py_files(_${_LIB_NAME} ${PYMOD_STAGE_DIR} ${_PY_FILES})
      include_directories(${PYTHON_INCLUDE_PATH})
      install(FILES ${_PY_FILES} DESTINATION "${LIB_DIR}/${PYMOD_DIR}")
      endif()
  endif()  
  get_target_property(_MOD_DEPS "${_PARENT_NAME}" MODULE_DEPS)
  if(_MOD_DEPS)
    foreach(dep ${_MOD_DEPS})
       add_dependencies("_${_LIB_NAME}" "_${dep}")
    endforeach()
  endif()

endmacro()

add_custom_target(check)
if (WIN32)
  set_target_properties(check PROPERTIES EXCLUDE_FROM_ALL "1")
endif()

#-------------------------------------------------------------------------------
# ost_unittest
#
# define a unit test
#-------------------------------------------------------------------------------
macro(ost_unittest)
  set(_ARG_PREFIX ost)
  parse_argument_list(_ARG 
                      "MODULE;PREFIX;SOURCES;LINK" "" ${ARGN})
    set(_SOURCES ${_ARG_SOURCES})
    set(CPP_TESTS)
    set(PY_TESTS)
    set(CMAKE_CURRENT_BINARY_DIR "${CMAKE_BINARY_DIR}/tests")
    foreach(src ${_SOURCES})
      if (${src} MATCHES "\\.py$")
       list(APPEND PY_TESTS "${src}")      
      else()
        list(APPEND CPP_TESTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
     endif()
    endforeach()
    set(_SOURCES ${CPP_TESTS})
    set(_test_name "${_ARG_PREFIX}_${_ARG_MODULE}_tests")
    if(DEFINED CPP_TESTS)
      if(COMPILE_TESTS)
        add_executable(${_test_name} ${_SOURCES})
      else()
        add_executable(${_test_name} EXCLUDE_FROM_ALL ${_SOURCES})
      endif()
      if (WIN32)
        target_link_libraries(${_test_name} ${BOOST_UNIT_TEST_LIBRARIES} "${_ARG_PREFIX}_${_ARG_MODULE}")  
        add_custom_target("${_test_name}_run":
                        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${_test_name}.exe || echo
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/..
                        COMMENT "running checks for module ${_ARG_MODULE}"
                        DEPENDS ${_test_name})
        add_test("${_test_name}" ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${_test_name}.exe)
      else()
        target_link_libraries(${_test_name} ${BOOST_UNIT_TEST_LIBRARIES}
                            "${_ARG_PREFIX}_${_ARG_MODULE}")
        add_custom_target("${_test_name}_run"
                        COMMAND OST_ROOT=${STAGE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${_test_name} || echo 
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        COMMENT "running checks for module ${_ARG_MODULE}"
                        DEPENDS ${_test_name})
        add_test("${_test_name}" ${CMAKE_CURRENT_BINARY_DIR}/${_test_name} )
      endif()
      
      if (_ARG_LINK)
        target_link_libraries("${_test_name}" ${_ARG_LINK})
      endif()

      add_dependencies(check "${_test_name}_run")
      set_target_properties(${_test_name}
                            PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                            "${CMAKE_CURRENT_BINARY_DIR}")
      if (WIN32)
        set_target_properties("${_test_name}_run" PROPERTIES EXCLUDE_FROM_ALL "1")
      endif()
    endif()
    foreach(py_test ${PY_TESTS})
      if(WIN32)
        set (PY_TESTS_CMD "${EXECUTABLE_OUTPUT_PATH}/ost.bat")
        add_custom_target("${py_test}_run"
                  CALL "${PY_TESTS_CMD} ${CMAKE_CURRENT_SOURCE_DIR}/${py_test} || echo"
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  COMMENT "running checks ${py_test}" VERBATIM)
      else()
        set (PY_TESTS_CMD "${EXECUTABLE_OUTPUT_PATH}/ost")
        add_custom_target("${py_test}_run"
                  sh -c "${PY_TESTS_CMD} ${CMAKE_CURRENT_SOURCE_DIR}/${py_test} || echo"
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  COMMENT "running checks ${py_test}" VERBATIM)
      endif()
      add_dependencies("${py_test}_run" ost_scripts "_${_ARG_PREFIX}_${_ARG_MODULE}")
      add_dependencies(check "${py_test}_run")
      if (WIN32)
        set_target_properties("${py_test}_run" PROPERTIES EXCLUDE_FROM_ALL "1")
      endif()
      
    endforeach()
    
    # get_target_property(OUTT ${_test_name} GENERATOR_FILE_NAME)
    # message("${OUTT}")
    
    # get_target_property(OUTT check GENERATOR_FILE_NAME)
    # message("${OUTT}")
endmacro()

#-------------------------------------------------------------------------------
# make sure the previously detected Python interpreter has the given module
#-------------------------------------------------------------------------------
macro(ost_find_python_module MODULE)
  if (NOT PYTHON_MODULE_${MODULE})
    execute_process(COMMAND ${PYTHON_BINARY} -c "import ${MODULE}"
                    OUTPUT_QUIET ERROR_QUIET
                    RESULT_VARIABLE _IMPORT_ERROR)
    if (_IMPORT_ERROR)
      message(FATAL_ERROR "Could not find python module ${MODULE}. Please install it")
    else()
      message(STATUS "Found python module ${MODULE}")
      set("PYTHON_MODULE_${MODULE}" FOUND CACHE STRING "" FORCE)
    endif()
  endif()
endmacro()


#-------------------------------------------------------------------------------
# this macro tries to detect a very common problem during configuration stage:
# it makes sure that boost python is linked against the same version of python 
# that we are linking against. 
#-------------------------------------------------------------------------------
macro(ost_match_boost_python_version)
  include(CheckCXXSourceRuns)
  # this variable may either be a simple library path or list that contains
  # different libraries for different build-options. For example:
  # optimized;<lib1>;debug;<lib2>
  set(_BOOST_PYTHON_LIBRARY ${Boost_PYTHON_LIBRARY})
  list(LENGTH _BOOST_PYTHON_LIBRARY _BP_LENGTH)
  if (_BP_LENGTH GREATER 1)
    list(FIND _BOOST_PYTHON_LIBRARY optimized _OPTIMIZED_INDEX)
    if (_OPTIMIZED_INDEX EQUAL -1)
      message(FATAL_ERROR 
              "Error while trying to get path of boost python library")
    endif()
    math(EXPR _LIB_INDEX 1+${_OPTIMIZED_INDEX})
    list(GET _BOOST_PYTHON_LIBRARY ${_LIB_INDEX} _BP_LIB_PATH)
    set(_BOOST_PYTHON_LIBRARY ${_BP_LIB_PATH})
  endif()
  set(CMAKE_REQUIRED_FLAGS "-I${PYTHON_INCLUDE_PATH} -I${Boost_INCLUDE_DIR} ${PYTHON_LIBRARIES} ${_BOOST_PYTHON_LIBRARY}")
  check_cxx_source_runs(
"#include <boost/python.hpp>

using namespace boost::python;
                           
int main( int argc, char ** argv ) {
  try {
    Py_Initialize();

    object main_module((
      handle<>(borrowed(PyImport_AddModule(\"__main__\")))));

    object main_namespace = main_module.attr(\"__dict__\");

    handle<> ignored(( PyRun_String( \"print 'Hello, World'\",
                                     Py_file_input,
                                     main_namespace.ptr(),
                                     main_namespace.ptr() ) ));
  } catch( error_already_set ) {
    PyErr_Print();
    return 1;
  }   
  return 0;
}" PYTHON_VERSION_FOUND_MATCHES_PYTHON_VERSION_BOOST_WAS_COMPILED_WITH)

  if(NOT PYTHON_VERSION_FOUND_MATCHES_PYTHON_VERSION_BOOST_WAS_COMPILED_WITH)
     message(FATAL_ERROR "Python versions mismatch!\n"
             "Make sure the python version for boost match the one you "
             "specified during configuration\n")
  endif()
endmacro()


#-------------------------------------------------------------------------------
# this macro sets up the stage directories
#-------------------------------------------------------------------------------
macro(setup_stage)
  set(STAGE_DIR "${CMAKE_BINARY_DIR}/stage")
  set(EXECUTABLE_OUTPUT_PATH ${STAGE_DIR}/bin  )
  set(HEADER_STAGE_PATH ${STAGE_DIR}/include )
  set(SHARED_DATA_PATH ${STAGE_DIR}/share/openstructure  )

  if (UNIX AND NOT APPLE)
    check_architecture()
  endif()
  set (ARCH ${CMAKE_NATIVE_ARCH})
  if ("${ARCH}" MATCHES "64")
    set(LIB_DIR lib64  )
    set(LIB_STAGE_PATH "${STAGE_DIR}/lib64"  )
  else()
    set(LIB_DIR lib  )
    set(LIB_STAGE_PATH "${STAGE_DIR}/lib"  )
  endif()
  if (_DEBIAN_STYLE_LIBEXEC)
    set(LIBEXEC_PATH ${LIB_DIR}/openstructure/libexec  )
  else()
    set(LIBEXEC_PATH libexec/openstructure  )
  endif()
  if (_DEBIAN_STYLE_LIBEXEC)
    set(LIBEXEC_STAGE_PATH ${LIB_STAGE_PATH}/openstructure/libexec  )
  else()
    set(LIBEXEC_STAGE_PATH ${STAGE_DIR}/libexec/openstructure  )
  endif()

  include_directories("${HEADER_STAGE_PATH}")
  link_directories(${LIB_STAGE_PATH})

endmacro()

#-------------------------------------------------------------------------------
# get compiler version
#-------------------------------------------------------------------------------
function(get_compiler_version _OUTPUT_VERSION)
  exec_program(${CMAKE_CXX_COMPILER}
               ARGS ${CMAKE_CXX_COMPILER_ARG1} -dumpversion
               OUTPUT_VARIABLE _COMPILER_VERSION
  )
  string(REGEX REPLACE "([0-9])\\.([0-9])(\\.[0-9])?" "\\1\\2"
    _COMPILER_VERSION ${_COMPILER_VERSION})

  set(${_OUTPUT_VERSION} ${_COMPILER_VERSION} PARENT_SCOPE)
endfunction()



macro(setup_compiler_flags)
  if (WIN32)
     # add_definitions(-DBOOST_TEST_INCLUDED)

     add_definitions(-D_USE_MATH_DEFINES -D_CRT_SECURE_NO_DEPRECATE
                     -D_SCL_SECURE_NO_DEPRECATE -DNOMINMAX)
     add_definitions(-Zc:wchar_t-)   #
    # add_definitions(-MDd -vmg -EHsc -GR)
    #GR:Uses the __fastcall calling convention (x86 only).
    #-EHsc to specify the synchronous exception handling mode/
    #-vmg Uses full generality for pointers to members.
    add_definitions(-DBOOST_ZLIB_BINARY=zdll)
    #add_definitions(-NODEFAULTLIB:LIBCMTD.lib)
  endif()


  if (CMAKE_COMPILER_IS_GNUCXX)
    get_compiler_version(_GCC_VERSION)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )
    if (_GCC_VERSION MATCHES "44")
      # gcc 4.4. is very strict about aliasing rules. the shared_count
      # implementation that is used boost's shared_ptr violates these rules. To
      # silence the warnings and prevent miscompiles, enable
      #  -fno-strict-aliasing
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing" )
    endif()
  endif()
endmacro()
set(_BOOST_MIN_VERSION 1.37)

macro(setup_boost)
  find_package(Boost ${_BOOST_MIN_VERSION} COMPONENTS python REQUIRED)
  set(BOOST_PYTHON_LIBRARIES ${Boost_LIBRARIES})
  set(Boost_LIBRARIES)
  find_package(Boost ${_BOOST_MIN_VERSION}
               COMPONENTS unit_test_framework REQUIRED)
  set(BOOST_UNIT_TEST_LIBRARIES ${Boost_LIBRARIES})
  set(Boost_LIBRARIES)
  if (ENABLE_STATIC)
    set(Boost_USE_STATIC_LIBS ON)
  endif()
  find_package(Boost ${_BOOST_MIN_VERSION}
               COMPONENTS filesystem system REQUIRED)
  set(BOOST_LIBRARIES ${Boost_LIBRARIES})
  set(Boost_LIBRARIES)
  find_package(Boost ${_BOOST_MIN_VERSION} COMPONENTS iostreams REQUIRED)
  set(BOOST_IOSTREAM_LIBRARIES ${Boost_LIBRARIES})
  set(Boost_LIBRARIES)
  find_package(Boost ${_BOOST_MIN_VERSION} COMPONENTS program_options REQUIRED)
  set(BOOST_PROGRAM_OPTIONS ${Boost_LIBRARIES})
  set(Boost_LIBRARIES)
  find_package(Boost ${_BOOST_MIN_VERSION} COMPONENTS regex REQUIRED)
  set(BOOST_REGEX_LIBRARIES ${Boost_LIBRARIES})
  set(Boost_LIBRARIES)
endmacro()
