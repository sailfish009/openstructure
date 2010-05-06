set(OST_IO_PYMOD_SOURCES
  wrap_io.cc
  export_pdb_io.cc
)

if (ENABLE_IMG)
  set(OST_IO_PYMOD_SOURCES ${OST_IO_PYMOD_SOURCES} export_map_io.cc)
endif()

set(OST_IO_PYMOD_MODULES
  __init__.py
  hhsearch.py
  repository.py
)


pymod(NAME io CPP ${OST_IO_PYMOD_SOURCES} PY ${OST_IO_PYMOD_MODULES})
if (STATIC_LIBRARIES)
  message("adding zlib libraries")
  target_link_libraries(_io ${ZLIB_LIBRARIES})
endif()

