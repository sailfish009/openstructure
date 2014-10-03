set(OST_MOL_MM_HEADERS
  forcefield.hh
  gromacs_reader.hh
  buildingblock.hh
  block_modifiers.hh
  gromacs_block_modifiers.hh
  heuristic_block_modifiers.hh
  mm_interaction.hh
  mm_settings.hh
  simulation.hh 
  mm_modeller.hh
  mm_observer.hh
  state_extractor.hh
  system_creator.hh
  topology_creator.hh
  index.hh
  topology.hh
  steep.hh

)

set(OST_MOL_MM_SOURCES
  forcefield.cc
  gromacs_reader.cc
  buildingblock.cc
  gromacs_block_modifiers.cc
  heuristic_block_modifiers.cc
  mm_interaction.cc
  simulation.cc
  mm_modeller.cc
  state_extractor.cc
  mm_observer.cc
  system_creator.cc
  topology_creator.cc
  topology.cc
  steep.cc
)

set(MOL_MM_DEPS ost_mol ost_io)


module(NAME mol_mm SOURCES ${OST_MOL_MM_SOURCES}
       HEADERS ${OST_MOL_MM_HEADERS}
       HEADER_OUTPUT_DIR ost/mol/mm
       DEPENDS_ON ${MOL_MM_DEPS}
       LINK ${BOOST_PROGRAM_OPTIONS})

target_link_libraries(ost_mol_mm ${OPEN_MM_LIBRARIES})

copy_if_different("${CMAKE_CURRENT_SOURCE_DIR}" "${STAGE_DIR}/share/openstructure/forcefields"
                  "AMBER03.dat" "AMBER_03_FORCEFIELD"
                  "ost_mol_mm")
install(FILES "AMBER03.dat" DESTINATION "share/openstructure/forcefields/")

copy_if_different("${CMAKE_CURRENT_SOURCE_DIR}" "${STAGE_DIR}/share/openstructure/forcefields"
                  "CHARMM27.dat" "CHARMM_27_FORCEFIELD"
                  "ost_mol_mm")
install(FILES "CHARMM27.dat" DESTINATION "share/openstructure/forcefields/")
