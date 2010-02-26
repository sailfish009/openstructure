# recursively process all subdirectories
file(GLOB ALL_FILES *)
foreach(fname ${ALL_FILES})
  if(NOT ${fname} MATCHES ".*.svn" 
     AND NOT ${fname} MATCHES ".*CMakeFiles" 
     AND NOT ${fname} MATCHES ".*main")
      if(IS_DIRECTORY ${fname})
        add_subdirectory(${fname})
      endif()
  endif()
endforeach()

