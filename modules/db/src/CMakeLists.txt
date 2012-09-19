set(OST_DB_HEADERS
sqlite_wrap.hh
sqlite3.h
sqlite_conv.hh
module_config.hh
)

set(OST_DB_SOURCES
sqlite_wrap.cc
sqlite3.c
)

module(NAME db SOURCES ${OST_DB_SOURCES} HEADERS ${OST_DB_HEADERS} 
       DEPENDS_ON ost_base)
if(WIN32)
  set_target_properties(ost_db PROPERTIES LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.def")
  add_definitions(/DSQLITE_ENABLE_COLUMN_METADATA)
else(WIN32)
  add_definitions(-DSQLITE_OMIT_LOAD_EXTENSION)
endif(WIN32)
