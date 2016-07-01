set(OST_MOL_MM_HEADERS
  forcefield.hh
  ff_reader.hh
  buildingblock.hh
  block_modifiers.hh
  gromacs_block_modifiers.hh
  heuristic_block_modifiers.hh
  interaction.hh
  settings.hh
  simulation.hh 
  modeller.hh
  observer.hh
  state_extractor.hh
  system_creator.hh
  topology_creator.hh
  index.hh
  topology.hh
  steep.hh
  openmm_plugins/density_plugin/openmmapi/density_force.hh
  openmm_plugins/density_plugin/openmmapi/density_kernels.hh
  openmm_plugins/density_plugin/openmmapi/density_force_impl.hh
  openmm_plugins/density_plugin/platforms/reference/reference_density_kernels.hh
  openmm_plugins/density_plugin/platforms/reference/reference_density_kernel_factory.hh
)

set(OST_MOL_MM_SOURCES
  forcefield.cc
  ff_reader.cc
  buildingblock.cc
  gromacs_block_modifiers.cc
  heuristic_block_modifiers.cc
  interaction.cc
  simulation.cc
  modeller.cc
  state_extractor.cc
  observer.cc
  system_creator.cc
  topology_creator.cc
  topology.cc
  steep.cc
  openmm_plugins/density_plugin/openmmapi/density_force.cc
  openmm_plugins/density_plugin/openmmapi/density_force_impl.cc
)

# create settings.hh as configurational header, needed to set the plugins path
set(config_hh_generator "CMake")
set(SETTINGS_HH_FILE "${CMAKE_CURRENT_SOURCE_DIR}/settings.hh")
configure_file(settings.hh.in ${SETTINGS_HH_FILE})


set(MOL_MM_DEPS ost_mol ost_io ost_img)


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

#The plugins that get loaded by OpenMM have to be built seperately

##########################
#build the density plugin#
##########################

#define the sources (headers are already handled above)
SET(DENSITY_PLUGIN_REFERENCE_SOURCES
  openmm_plugins/density_plugin/platforms/reference/reference_density_kernel_factory.cc
  openmm_plugins/density_plugin/platforms/reference/reference_density_kernels.cc
)

# Set the library name
SET(OPENMM_DENSITY_REFERENCE_LIBRARY_NAME density_plugin_reference)
SET(SHARED_TARGET_DENSITY_PLUGIN ${OPENMM_DENSITY_REFERENCE_LIBRARY_NAME})

ADD_LIBRARY(${SHARED_TARGET_DENSITY_PLUGIN} SHARED ${DENSITY_PLUGIN_REFERENCE_SOURCES})
ADD_DEPENDENCIES(${SHARED_TARGET_DENSITY_PLUGIN} ost_mol_mm ost_img ost_mol_mm)

SET_TARGET_PROPERTIES(${SHARED_TARGET_DENSITY_PLUGIN} PROPERTIES
                      LIBRARY_OUTPUT_DIRECTORY ${STAGE_DIR}/share/openstructure/openmm_plugins/
                      ARCHIVE_OUTPUT_DIRECTORY ${STAGE_DIR}/share/openstructure/openmm_plugins/
                      RUNTIME_OUTPUT_DIRECTORY ${STAGE_DIR}/share/openstructure/openmm_plugins/)

foreach(_DEPENDENCY ${_MOL_MM_DEPS})
  TARGET_LINK_LIBRARIES(${SHARED_TARGET_DENSITY_PLUGIN} ${_DEPENDENCY})
endforeach()
#also link against OpenMM and the mm module
TARGET_LINK_LIBRARIES(${SHARED_TARGET_DENSITY_PLUGIN} ${OPEN_MM_LIBRARIES})
TARGET_LINK_LIBRARIES(${SHARED_TARGET_DENSITY_PLUGIN} ost_mol_mm)

file(MAKE_DIRECTORY ${STAGE_DIR}/share/openstructure/openmm_plugins/)
INSTALL(TARGETS ${SHARED_TARGET_DENSITY_PLUGIN} LIBRARY DESTINATION "${STAGE_DIR}/share/openstructure/openmm_plugins/")




