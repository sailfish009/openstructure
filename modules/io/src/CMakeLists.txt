add_subdirectory(mol)
add_subdirectory(seq)
add_subdirectory(iplt)

set(OST_IO_HEADERS
binary_data_sink.hh		
io_manager.hh
binary_data_source.hh		
module_config.hh
container_serialization.hh	
swap_util.hh
io_exception.hh
convert.hh
converting_streams.hh
)

set(OST_IO_SOURCES
io_manager.cc
convert.cc
)


foreach(fname ${OST_IO_MOL_SOURCES})
  set(OST_IO_SOURCES ${OST_IO_SOURCES} mol/${fname})
endforeach(fname ${OST_IO_MOL_SOURCES})

foreach(fname ${OST_IO_SEQ_SOURCES})
  set(OST_IO_SOURCES ${OST_IO_SOURCES} seq/${fname})
endforeach(fname ${OST_IO_SEQ_SOURCES})

if (ENABLE_IPLT)
  foreach(fname ${OST_IO_IPLT_SOURCES})
    set(OST_IO_SOURCES ${OST_IO_SOURCES} iplt/${fname})
  endforeach(fname ${OST_IO_IPLT_SOURCES})

  foreach(fname ${OST_IO_IPLT_HEADERS})
    set(OST_IO_HEADERS ${OST_IO_HEADERS} iplt/${fname})
  endforeach(fname ${OST_IO_IPLT_HEADERS})
endif()

foreach(fname ${OST_IO_MOL_HEADERS})
  set(OST_IO_HEADERS ${OST_IO_HEADERS} mol/${fname})
endforeach(fname ${OST_IO_MOL_HEADERS})

foreach(fname ${OST_IO_SEQ_HEADERS})
  set(OST_IO_HEADERS ${OST_IO_HEADERS} seq/${fname})
endforeach(fname ${OST_IO_SEQ_HEADERS})



set(OST_IO_DEPENDENCIES base;conop;seq)
if (ENABLE_IPLT)
  set(OST_IO_DEPENDENCIES ${OST_IO_DEPENDENCIES};iplt;iplt_alg)
endif()
module(NAME io SOURCES "${OST_IO_SOURCES}" 
       HEADERS ${OST_IO_MOL_HEADERS} IN_DIR mol
               ${OST_IO_IPLT_HEADERS} IN_DIR iplt
               ${OST_IO_SEQ_HEADERS} IN_DIR seq
               ${OST_IO_HEADERS}
       DEPENDS_ON ${OST_IO_DEPENDENCIES})
if (NOT WIN32)
  # see note in modules/gui/src/CMakeLists.txt
  target_link_libraries(ost_io ${BOOST_IOSTREAM_LIBRARIES})
endif()
if (ENABLE_IPLT)
  target_link_libraries(ost_io ${TIFF_LIBRARIES} ${PNG_LIBRARIES})
endif()