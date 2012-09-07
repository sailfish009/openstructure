#-------------------------------------------------------------------------------
#  Author:  Andreas Schenk
#
#  This file contains functions to determine, copy and verify the dependencies
#  of the application bundle.
#
#  Part of this code was taken from BundleUtilities.cmake, distributed
#  under the OSI-approved BSD License (see below), and adapted.
#-------------------------------------------------------------------------------

#  CMake - Cross Platform Makefile Generator
#  Copyright 2000-2011 Kitware, Inc., Insight Software Consortium
#  All rights reserved.
#  
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  
#  * Neither the names of Kitware, Inc., the Insight Software Consortium,
#    nor the names of their contributors may be used to endorse or promote
#    products derived from this software without specific prior written
#    permission.
#  
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#=============================================================================
# function copy dependencies
#=============================================================================
function(copy_dependencies binaries)
  #--------------------------------------------------------------------------
  message("copy dependencies: determining dependencies")
  #--------------------------------------------------------------------------
  message(STATUS "  binaries='${binaries}'")
  collect_dependencies("${binaries}" keys)

  #--------------------------------------------------------------------------
  message("copy dependencies: copying files")
  #--------------------------------------------------------------------------
  list(LENGTH keys n)
  math(EXPR n ${n}*2)

  set(i 0)
  foreach(key ${keys})
    math(EXPR i ${i}+1)
    if(${${key}_COPYFLAG})
      message(STATUS "${i}/${n}: copying '${${key}_RESOLVED_ITEM}' to '${${key}_RESOLVED_EMBEDDED_ITEM}'")
      set(item "${${key}_ITEM}")
      copy_resolved_item("${${key}_RESOLVED_ITEM}" "${${key}_RESOLVED_EMBEDDED_ITEM}")
    else(${${key}_COPYFLAG})
      message(STATUS "${i}/${n}: *NOT* copying '${${key}_RESOLVED_ITEM}'")
    endif(${${key}_COPYFLAG})
  endforeach(key)

  #--------------------------------------------------------------------------
  message("copy dependencies: changing install names")
  #--------------------------------------------------------------------------
  foreach(key ${keys})
    math(EXPR i ${i}+1)
    if(APPLE)
      if(NOT ${${key}_SYSTEMFLAG})
        message(STATUS "${i}/${n}: changing install names for '${${key}_RESOLVED_EMBEDDED_ITEM}'")
        change_install_names_for_item(keys "${key}")
      else(NOT ${${key}_SYSTEMFLAG})
        message(STATUS "${i}/${n}: skipping install name change for system library: '${${key}_RESOLVED_EMBEDDED_ITEM}'")
      endif(NOT ${${key}_SYSTEMFLAG})
    else(APPLE)
      message(STATUS "${i}/${n}: changing install names not required on this platform '${${key}_RESOLVED_EMBEDDED_ITEM}'")
    endif(APPLE)
  endforeach(key)


  #--------------------------------------------------------------------------
  message("copy dependencies: verifying binaries")
  #--------------------------------------------------------------------------
  verify(keys)

  #--------------------------------------------------------------------------
  message("copy dependencies: cleaning up")
  #--------------------------------------------------------------------------
  clear_keys(keys)

  #--------------------------------------------------------------------------
  message("copy dependencies: done")
  #--------------------------------------------------------------------------
endfunction(copy_dependencies)

#=============================================================================
# function collect_dependencies
#=============================================================================
function(collect_dependencies binaries keys_var)
  set(${keys_var} PARENT_SCOPE)

  # First add all binaries to the keys. 
  # This is done before dependency resolution because one of the binaries could
  # potentially be a dependency. 
  foreach(exe ${binaries})
    get_filename_component(exepath "${exe}" PATH)
    set_keys_for_item(${keys_var} "${exe}" "${exe}" 0 0)
  endforeach(exe)

  # Get dependencies and add them to the keys.
  foreach(exe ${binaries})
    get_filename_component(exepath "${exe}" PATH)
    add_dependencies_for_item(${keys_var}  "${exe}" "${exe}" "${exepath}")
  endforeach(exe)

  # Propagate values to caller's scope:
  set(${keys_var} ${${keys_var}} PARENT_SCOPE)
  foreach(key ${${keys_var}})
    set(${key}_ITEM "${${key}_ITEM}" PARENT_SCOPE)
    set(${key}_RESOLVED_ITEM "${${key}_RESOLVED_ITEM}" PARENT_SCOPE)
    set(${key}_RESOLVED_EMBEDDED_ITEM "${${key}_RESOLVED_EMBEDDED_ITEM}" PARENT_SCOPE)
    set(${key}_COPYFLAG "${${key}_COPYFLAG}" PARENT_SCOPE)
    set(${key}_SYSTEMFLAG "${${key}_SYSTEMFLAG}" PARENT_SCOPE)
  endforeach(key)

endfunction(collect_dependencies)




#=============================================================================
# function set_keys_for_item
#=============================================================================
function(set_keys_for_item keys_var context resolved_item copyflag systemflag)
  get_item_key("${resolved_item}" key)
  list(LENGTH ${keys_var} length_before)
  append_unique(${keys_var} "${key}")
  list(LENGTH ${keys_var} length_after)

  if(NOT length_before EQUAL length_after)
    if(${copyflag} AND (NOT ${systemflag}) )
      resolve_embedded_item("${resolved_item}" resolved_embedded_item)
      set(${key}_RESOLVED_EMBEDDED_ITEM "${resolved_embedded_item}" PARENT_SCOPE)
      if(${resolved_embedded_item} STREQUAL ${resolved_item})
        set(${key}_COPYFLAG 0 PARENT_SCOPE)
      else(${resolved_embedded_item} STREQUAL ${resolved_item})
        set(${key}_COPYFLAG 1 PARENT_SCOPE)
      endif(${resolved_embedded_item} STREQUAL ${resolved_item})
      set(${key}_SYSTEMFLAG 0 PARENT_SCOPE)
    else(${copyflag} AND (NOT ${systemflag}) )
      set(${key}_COPYFLAG ${copyflag} PARENT_SCOPE)
      set(${key}_SYSTEMFLAG ${systemflag} PARENT_SCOPE)
      set(${key}_RESOLVED_EMBEDDED_ITEM "${resolved_item}" PARENT_SCOPE)
    endif(${copyflag} AND (NOT ${systemflag}) )
    set(${keys_var} ${${keys_var}} PARENT_SCOPE)
    get_filename_component(item_name "${resolved_item}" NAME)
    set(${key}_ITEM "${item_name}" PARENT_SCOPE)
    set(${key}_RESOLVED_ITEM "${resolved_item}" PARENT_SCOPE)

  else(NOT length_before EQUAL length_after)
    message(FATAL_ERROR "Item key '${key}' already in the list. This should not happen.")
  endif(NOT length_before EQUAL length_after)
endfunction(set_keys_for_item)

#=============================================================================
# function get_item_key
#=============================================================================
function(get_item_key item key_var)
  get_filename_component(item_name "${item}" REALPATH)
  if(WIN32)
    string(TOLOWER "${item_name}" item_name)
  endif()
  string(REGEX REPLACE "\\." "_" item_name "${item_name}")
  string(REGEX REPLACE "/" "_"   item_name "${item_name}")
  string(REGEX REPLACE "\\\\" "_" ${key_var} "${item_name}")
  set(${key_var} ${${key_var}} PARENT_SCOPE)
endfunction(get_item_key)


#=============================================================================
# function clear_keys
#=============================================================================
function(clear_keys keys_var)
  foreach(key ${${keys_var}})
    set(${key}_ITEM PARENT_SCOPE)
    set(${key}_RESOLVED_ITEM PARENT_SCOPE)
    set(${key}_RESOLVED_EMBEDDED_ITEM PARENT_SCOPE)
    set(${key}_COPYFLAG PARENT_SCOPE)
    set(${key}_SYSTEMFLAG PARENT_SCOPE)
  endforeach(key)
  set(${keys_var} PARENT_SCOPE)
endfunction(clear_keys)



#=============================================================================
# function resolve_item
#=============================================================================
function(resolve_item context item exepath resolved_item_var)
  set(resolved_item "${item}")

  # @rpath references are relative to the paths built into the binaries with -rpath
  # rpath resolution comes first, as the rpath might contain a @loader_path or @executable_path reference
  if(resolved_item MATCHES "@rpath")
    get_rpath("${context}" rpath)
    string(REPLACE "@rpath/" "${rpath}/" resolved_item "${resolved_item}")
  endif(resolved_item MATCHES "@rpath")

  # @executable_path references are assumed relative to exepath
  if(resolved_item MATCHES "@executable_path")
    string(REPLACE "@executable_path" "${exepath}" resolved_item "${resolved_item}")
  endif(resolved_item MATCHES "@executable_path")

  # @loader_path references are assumed relative to the
  # PATH of the given "context" (presumably another library)
  if(resolved_item MATCHES "@loader_path")
    get_filename_component(contextpath "${context}" PATH)
    string(REPLACE "@loader_path" "${contextpath}" resolved_item "${resolved_item}")
  endif(resolved_item MATCHES "@loader_path")
  
  get_filename_component(resolved_item "${resolved_item}" REALPATH)

  if(IS_ABSOLUTE "${resolved_item}" AND EXISTS "${resolved_item}")
    set(${resolved_item_var} "${resolved_item}" PARENT_SCOPE)
    return()
  endif(IS_ABSOLUTE "${resolved_item}" AND EXISTS "${resolved_item}")

  set(ri "ri-NOTFOUND")
  find_file(ri "${item}" ${exepath}  NO_DEFAULT_PATH)
  find_file(ri "${item}" ${exepath}  /usr/lib)
  if(WIN32 AND NOT UNIX)
    find_program(ri "${item}" PATHS "${exepath}" NO_DEFAULT_PATH)
    find_program(ri "${item}" PATHS "${exepath}")
  endif(WIN32 AND NOT UNIX)
  if(ri)
    set(resolved_item "${ri}")
    set(ri "ri-NOTFOUND")
  endif(ri)

  if(IS_ABSOLUTE "${resolved_item}" AND EXISTS "${resolved_item}")
    set(${resolved_item_var} "${resolved_item}" PARENT_SCOPE)
  else(IS_ABSOLUTE "${resolved_item}" AND EXISTS "${resolved_item}")
    message(FATAL_ERROR "cannot resolve item '${item}' in context:'${context}'. Partially resolved path is: '${resolved_item}'")
  endif(IS_ABSOLUTE "${resolved_item}" AND EXISTS "${resolved_item}")
endfunction(resolve_item)


#=============================================================================
# function resolve_embedded_item
#=============================================================================
function(resolve_embedded_item item resolved_item_var)
  if(${item} MATCHES ${CMAKE_INSTALL_PREFIX})
    # item already embedded
    set(${resolved_item_var} ${item} PARENT_SCOPE)
  else(${item} MATCHES ${CMAKE_INSTALL_PREFIX})
    # only embed libraries, therefore put into lib dir
    get_filename_component(item_name "${item}" NAME)
    if ("${CMAKE_NATIVE_ARCH}" MATCHES "64")
      set(${resolved_item_var} "${CMAKE_INSTALL_PREFIX}/lib64/${item_name}" PARENT_SCOPE)
    else()
      set(${resolved_item_var} "${CMAKE_INSTALL_PREFIX}/lib/${item_name}" PARENT_SCOPE)
    endif()
  endif(${item} MATCHES ${CMAKE_INSTALL_PREFIX})
endfunction(resolve_embedded_item)



#=============================================================================
# function copy_resolved_item
#=============================================================================
function(copy_resolved_item resolved_item resolved_embedded_item)
  if(WIN32)
    # ignore case on Windows
    string(TOLOWER "${resolved_item}" resolved_item_compare)
    string(TOLOWER "${resolved_embedded_item}" resolved_embedded_item_compare)
  else()
    set(resolved_item_compare "${resolved_item}")
    set(resolved_embedded_item_compare "${resolved_embedded_item}")
  endif()

  if("${resolved_item_compare}" STREQUAL "${resolved_embedded_item_compare}")
    message(STATUS "warning: resolved_item == resolved_embedded_item - not copying...")
  else()
    message(STATUS "copying COMMAND ${CMAKE_COMMAND} -E copy ${resolved_item} ${resolved_embedded_item}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${resolved_item}" "${resolved_embedded_item}")
    if(UNIX AND NOT APPLE)
      file(RPATH_REMOVE FILE "${resolved_embedded_item}")
    endif(UNIX AND NOT APPLE)
  endif()

endfunction(copy_resolved_item)


#=============================================================================
# function get_rpath
#=============================================================================
function(get_rpath target rpath_var)
  if(APPLE)
    set(rpath_cmd "otool")
    set(rpath_cmd_args "-l")
    set(rpath_cmd_regex ".*LC_RPATH\n *cmdsize [0-9]*\n\ *path ([^\n ]*) \\(offset.*")
  else(APPLE)
    MESSAGE( FATAL_ERROR rpath extraction not implemented for non APPLE platform)
  endif(APPLE)
  execute_process(
    COMMAND ${rpath_cmd} ${rpath_cmd_args} ${target}
    OUTPUT_VARIABLE rpath_cmd_ov
    RESULT_VARIABLE retcode
  )
  if(retcode)
    MESSAGE(FATAL_ERROR "otool stopped with return code: '${retcode}'")
  endif(retcode)
  if(rpath_cmd_ov MATCHES ${rpath_cmd_regex})
    string(REGEX REPLACE ${rpath_cmd_regex} "\\1"  rpath ${rpath_cmd_ov})
    set(${rpath_var} ${rpath} PARENT_SCOPE)
  endif(rpath_cmd_ov MATCHES ${rpath_cmd_regex})
endfunction(get_rpath)


#=============================================================================
# function change_install_names_for_item
#=============================================================================
function(change_install_names_for_item keys_var key)

  # Ensure the item is within the install directory
  if(${${key}_RESOLVED_EMBEDDED_ITEM} MATCHES ${CMAKE_INSTALL_PREFIX})
    set(prereqs "")
    get_filename_component(exepath ${${key}_RESOLVED_EMBEDDED_ITEM} PATH)
    string(REPLACE ${CMAKE_INSTALL_PREFIX} "" rel_path ${exepath})
    string(REGEX REPLACE "/[^/]*" "/.." inv_rel_path ${rel_path})
    get_dependencies_for_item("${${key}_RESOLVED_EMBEDDED_ITEM}" prereqs)
    set(changes "")
    foreach(pr ${prereqs})
      # Each referenced item's key is "rkey" in the loop:
      get_filename_component(exepath ${${key}_RESOLVED_EMBEDDED_ITEM} PATH)
      resolve_item( ${${key}_RESOLVED_EMBEDDED_ITEM} ${pr} exepath resolved_pr)
      get_item_key("${resolved_pr}" rkey)
      list(FIND ${keys_var} ${rkey} r_found)
      if(r_found EQUAL -1)
        MESSAGE(FATAL_ERROR "Key: '${rkey}' not found in dependency list. Cannot change install name.")
      endif(r_found EQUAL -1)
      if(NOT ${${rkey}_SYSTEMFLAG})
        get_filename_component(ref_exepath ${${rkey}_RESOLVED_EMBEDDED_ITEM} PATH)
        string(REPLACE ${CMAKE_INSTALL_PREFIX} "" ref_rel_path ${ref_exepath})
        set(diff_path @loader_path${inv_rel_path}${ref_rel_path}/)
        set(changes ${changes} "-change" "${pr}" "${diff_path}${${rkey}_ITEM}")
      endif(NOT ${${rkey}_SYSTEMFLAG})
    endforeach(pr)
     
    execute_process(COMMAND chmod u+w "${${key}_RESOLVED_EMBEDDED_ITEM}")
    
    # Change this item's id and all of its references in one call
    # to install_name_tool:
    execute_process(COMMAND install_name_tool ${changes} -id "${${key}_ITEM}" "${${key}_RESOLVED_EMBEDDED_ITEM}")
  else( ${${key}_RESOLVED_EMBEDDED_ITEM} MATCHES ${CMAKE_INSTALL_PREFIX})
    MESSAGE(FATAL_ERROR "Trying to change install name for: ${${key}_RESOLVED_EMBEDDED_ITEM}, which is outside of install directory")
  endif(${${key}_RESOLVED_EMBEDDED_ITEM} MATCHES ${CMAKE_INSTALL_PREFIX})

endfunction(change_install_names_for_item)



#=============================================================================
# function verify
#=============================================================================
function(verify keys_var)
  foreach(key ${${keys_var}})
    get_dependencies_for_item("${${key}_RESOLVED_EMBEDDED_ITEM}" dependencies)
    get_filename_component(exepath ${${key}_RESOLVED_EMBEDDED_ITEM} PATH)
    foreach(dep ${dependencies})
      resolve_item( ${${key}_RESOLVED_EMBEDDED_ITEM} ${dep} exepath resolved_dep)
      is_system_lib(${resolved_dep} system_flag)
      if(NOT ${system_flag})
        if(NOT ${resolved_dep} MATCHES ${CMAKE_INSTALL_PREFIX})
          MESSAGE("Warning: item:'${${key}_RESOLVED_EMBEDDED_ITEM}' contains external dependency:'${resolved_dep}'")
        endif(NOT ${resolved_dep} MATCHES ${CMAKE_INSTALL_PREFIX})
      endif(NOT ${system_flag})
    endforeach(dep)
  endforeach(key)
endfunction(verify)


#=============================================================================
# macro get_dependencies_for_item
#=============================================================================
# Note: dispatcher written as macro to avoid having to copy results to parent scope
macro(get_dependencies_for_item item list_var)

  if(NOT IS_ABSOLUTE "${item}" AND EXISTS "${item}")
    message(FATAL_ERROR "Cannot find item: '${item}' to get dependencies.")
  endif(NOT IS_ABSOLUTE "${item}" AND EXISTS "${item}")

  if(APPLE)
    get_dependencies_for_item_apple(${item} ${list_var})
  elseif(WIN32 AND NOT UNIX)
    get_dependencies_for_item_win(${item} ${list_var})
  elseif(UNIX)
    get_dependencies_for_item_linux(${item} ${list_var})
  else(APPLE)
    MESSAGE(FATAL_ERROR "Cannot determine dependencies for unknown system.")
  endif(APPLE)
endmacro(get_dependencies_for_item)

#=============================================================================
# function get_dependencies_for_item_apple
#=============================================================================
function(get_dependencies_for_item_apple  item list_var)

  find_program(otool_cmd "otool" PATHS "/usr/local/bin" "/usr/bin")
  if(NOT otool_cmd)
    message(FATAL_ERROR "Could not find otool - cannot analyze dependencies.")
    return()
  endif(NOT otool_cmd)

  execute_process( COMMAND ${otool_cmd} -L ${item} OUTPUT_VARIABLE otool_cmd_ov RESULT_VARIABLE retcode )
  if(retcode)
    MESSAGE(FATAL_ERROR "otool stopped with return code: '${retcode}'")
  endif(retcode)
  # Convert to a list of lines:
  string(REGEX REPLACE ";" "\\\\;" candidates "${otool_cmd_ov}")
  set(eol_char "E")
  string(REGEX REPLACE "\n" "${eol_char};" candidates "${candidates}")

  # check for install id and remove it from list, since otool -L can include a reference to itself
  set(install_id)
  execute_process( COMMAND ${otool_cmd} -D ${item} OUTPUT_VARIABLE install_id_ov  RESULT_VARIABLE retcode)
  if(retcode)
    MESSAGE(FATAL_ERROR "otool stopped with return code: '${retcode}'")
  endif(retcode)
  # second line is install name
  string(REGEX REPLACE ".*:\n" "" install_id "${install_id_ov}")
  if(install_id)
    string(REGEX MATCH "[^\n ].*[^\n ]" install_id "${install_id}")
  endif(install_id)

  # Analyze each line for file names that match the regular expression:
  set(otool_regex "^\t([^\t]+) \\(compatibility version ([0-9]+.[0-9]+.[0-9]+), current version ([0-9]+.[0-9]+.[0-9]+)\\)${eol_char}$")
  set(dep_list)
  foreach(candidate ${candidates})
    if("${candidate}" MATCHES "${otool_regex}")
      # Extract information from each candidate:
      string(REGEX REPLACE "${otool_regex}" "\\1" dep "${candidate}")
      if(NOT "${dep}" STREQUAL "${install_id}")
        list(APPEND dep_list ${dep})
      endif(NOT "${dep}" STREQUAL "${install_id}")
    endif("${candidate}" MATCHES "${otool_regex}")
  endforeach(candidate)
  set(${list_var} ${dep_list} PARENT_SCOPE)
endfunction(get_dependencies_for_item_apple)

#=============================================================================
# function get_dependencies_for_item_linux
#=============================================================================
function(get_dependencies_for_item_linux  item list_var)

  find_program(ldd_cmd "ldd" PATHS "/usr/local/bin" "/usr/bin")
  if(NOT ldd_cmd)
    message(FATAL_ERROR "Could not find ldd - cannot analyze dependencies.")
    return()
  endif(NOT ldd_cmd)

  execute_process( COMMAND ${ldd_cmd}  ${item} OUTPUT_VARIABLE ldd_cmd_ov RESULT_VARIABLE retcode)
  if(retcode)
    MESSAGE(FATAL_ERROR "ldd stopped with return code: '${retcode}'")
  endif(retcode)
  # Convert to a list of lines:
  string(REGEX REPLACE ";" "\\\\;" candidates "${ldd_cmd_ov}")
  set(eol_char "E")
  string(REGEX REPLACE "\n" "${eol_char};" candidates "${candidates}")

  # Analyze each line for file names that match the regular expression:
  set(ldd_regex "^[\t ]*[^\t ]+ => ([^\t\(]+) .*${eol_char}$")
  set(ldd_regex_error "not found${eol_char}$")
  set(ldd_regex_fallback "^[\t ]*([^\t ]+) => ([^\t ]+).*${eol_char}$")
  set(dep_list)
  foreach(candidate ${candidates})
    if("${candidate}" MATCHES "${ldd_regex}")
      # Extract information from each candidate:
      if("${candidate}" MATCHES "${ldd_regex_error}")
        string(REGEX REPLACE "${ldd_regex_fallback}" "\\1" raw_item "${candidate}")
      else("${candidate}" MATCHES "${ldd_regex_error}")
        string(REGEX REPLACE "${ldd_regex}" "\\1" raw_item "${candidate}")
      endif("${candidate}" MATCHES "${ldd_regex_error}")
      list(APPEND depl_list ${dep})
    endif("${candidate}" MATCHES "${ldd_regex}")
  set(${list_var} ${dep_list} PARENT_SCOPE)
endfunction(get_dependencies_for_item_linux)

#=============================================================================
# function get_dependencies_for_item_win
#=============================================================================
function(get_dependencies_for_item_win  item list_var)

  find_program(dumpbin_cmd "dumpbin")
  if(NOT dumpbin_cmd)
    message(FATAL_ERROR "Could not find dumpbin - cannot analyze dependencies.")
    return()
  endif(NOT dumpbin_cmd)

  set(ENV{VS_UNICODE_OUTPUT} "") # Block extra output from inside VS IDE.
  execute_process( COMMAND ${dumpbin_cmd}  /dependents ${item} OUTPUT_VARIABLE dumpbin_cmd_ov RESULT_VARIABLE retcode)
  if(retcode)
    MESSAGE(FATAL_ERROR "dumpbin stopped with return code: '${retcode}'")
  endif(retcode)
  # Convert to a list of lines:
  string(REGEX REPLACE ";" "\\\\;" candidates "${dumpbin_cmd_ov}")
  set(eol_char "E")
  string(REGEX REPLACE "\n" "${eol_char};" candidates "${candidates}")

  # Analyze each line for file names that match the regular expression:
  set(dumpbin_regex "^    ([^ ].*[Dd][Ll][Ll])${eol_char}$")
  set(dep_list)
  foreach(candidate ${candidates})
    if("${candidate}" MATCHES "${dumpbin_regex}")
      # Extract information from each candidate:
      string(REGEX REPLACE "${dumpbin_regex}" "\\1" raw_item "${candidate}")
      list(APPEND depl_list ${dep})
    endif("${candidate}" MATCHES "${dumpbin_regex}")
  set(${list_var} ${dep_list} PARENT_SCOPE)
endfunction(get_dependencies_for_item_win)


#=============================================================================
# function add_dependencies_for_item
#=============================================================================
function(add_dependencies_for_item keys_var context item exepath)
  if(NOT IS_ABSOLUTE "${item}" AND EXISTS "${item}")
    message(FATAL_ERROR "Item '${item}' does not exist.")
  endif(NOT IS_ABSOLUTE "${item}" AND EXISTS "${item}")

  get_dependencies_for_item(${item} dependencies)
  foreach(dep ${dependencies})
    resolve_item("${context}" "${dep}" "${exepath}" resolved_dep)
    get_item_key("${resolved_dep}" dep_key)
    list(FIND ${keys_var} ${dep_key} dep_found)
    if(dep_found EQUAL -1)
      message(STATUS "Resolved '${dep}' to: '${resolved_dep}'")
      is_system_lib(${resolved_dep} system_flag)
      if(${system_flag})
        MESSAGE(STATUS "Skipping system library:'${resolved_dep}'")
        set_keys_for_item(${keys_var} ${item} ${resolved_dep} 0 1)
      else(${system_flag})
        set_keys_for_item(${keys_var} ${item} ${resolved_dep} 1   0)
        add_dependencies_for_item(${keys_var} ${item} ${resolved_dep} ${exepath})
      endif(${system_flag})
    endif(dep_found EQUAL -1)
  endforeach(dep ${dependencies})
  # Propagate values to caller's scope:
  set(${keys_var} ${${keys_var}} PARENT_SCOPE)
  foreach(key ${${keys_var}})
    set(${key}_ITEM "${${key}_ITEM}" PARENT_SCOPE)
    set(${key}_RESOLVED_ITEM "${${key}_RESOLVED_ITEM}" PARENT_SCOPE)
    set(${key}_RESOLVED_EMBEDDED_ITEM "${${key}_RESOLVED_EMBEDDED_ITEM}" PARENT_SCOPE)
    set(${key}_COPYFLAG "${${key}_COPYFLAG}" PARENT_SCOPE)
    set(${key}_SYSTEMFLAG "${${key}_SYSTEMFLAG}" PARENT_SCOPE)
  endforeach(key)
endfunction(add_dependencies_for_item)


#=============================================================================
# function is_system_lib
#=============================================================================
function(is_system_lib item system_var)
  if(APPLE)
    if(item MATCHES "^(/System/Library/|/usr/lib/)")
      set(${system_var}  1 PARENT_SCOPE)
    else()
      set(${system_var}  0 PARENT_SCOPE)
    endif()
    return()
  elseif(WIN32)
    string(TOLOWER "${item}" lower)
    string(TOLOWER "$ENV{SystemRoot}" sysroot)
    string(REGEX REPLACE "\\\\" "/" sysroot "${sysroot}")
    string(TOLOWER "$ENV{windir}" windir)
    string(REGEX REPLACE "\\\\" "/" windir "${windir}")
    if(lower MATCHES "^(${sysroot}/sys(tem|wow)|${windir}/sys(tem|wow)|(.*/)*msvc[^/]+dll)")
      set(${system_var}  1 PARENT_SCOPE)
    else()
      set(${system_var}  0 PARENT_SCOPE)
    endif()
    return()
  elseif(UNIX)
    if(item MATCHES "^(/lib/|/lib32/|/lib64/|/usr/lib/|/usr/lib32/|/usr/lib64/|/usr/X11R6/|/usr/bin/)")
      set(${system_var} 1 PARENT_SCOPE)
    else()
      set(${system_var}  0 PARENT_SCOPE)
    endif()
    return()
  else(APPLE)
    MESSAGE(FATAL_ERROR "Unknown OS. Cannot determine if item:'${item}' is a system lib.")
  endif(APPLE)
endfunction(is_system_lib)

#=============================================================================
# function append_unique
#=============================================================================
function(append_unique list_var value)
  set(contains 0)
  foreach(item ${${list_var}})
    if("${item}" STREQUAL "${value}")
      set(contains 1)
      break()
    endif("${item}" STREQUAL "${value}")
  endforeach(item)

  if(NOT contains)
    set(${list_var} ${${list_var}} "${value}" PARENT_SCOPE)
  endif(NOT contains)
endfunction(append_unique)
