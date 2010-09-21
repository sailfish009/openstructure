set(OST_CONOP_HEADERS
builder.hh
builder_fw.hh
conop.hh
heuristic_builder.hh
compound.hh
compound_lib.hh
module_config.hh
rule_based_builder.hh
ring_finder.hh
)

set(OST_CONOP_SOURCES
builder.cc
conop.cc
heuristic_builder.cc
compound.cc
compound_lib.cc
rule_based_builder.cc
ring_finder.cc
)

module(NAME conop SOURCES ${OST_CONOP_SOURCES}
       HEADERS ${OST_CONOP_HEADERS} DEPENDS_ON mol geom db)

executable(NAME chemdict_tool SOURCES chemdict_tool.cc DEPENDS_ON io)

if (COMPOUND_LIB)
  if (EXISTS "${COMPOUND_LIB}")
    copy_if_different("./" "${STAGE_DIR}/share/openstructure" 
                      "${COMPOUND_LIB}" "COMPOUND_LIB" ost_conop)
    install(FILES ${INSPECTOR_ICONS} DESTINATION "share/openstructure")
  else()
    message(FATAL_ERROR "${COMPOUND_LIB} does not exist. \n"
            "Pass -DCOMPOUND_LIB=NONE if you would like to build "
            "OpenStructure without a compound library")
  endif()

endif()