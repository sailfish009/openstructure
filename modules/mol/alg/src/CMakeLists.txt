set(OST_MOL_ALG_HEADERS
  svd_superpose.hh
  module_config.hh
  sec_structure_segments.hh
  local_dist_diff_test.hh
  distance_test_common.hh
  distance_rmsd_test.hh
  superpose_frames.hh
  filter_clashes.hh
  construct_cbeta.hh
  clash_score.hh
  trajectory_analysis.hh
  structure_analysis.hh
  consistency_checks.hh
  pdbize.hh
)

set(OST_MOL_ALG_SOURCES
  svd_superpose.cc
  clash_score.cc
  sec_structure_segments.cc
  local_dist_diff_test.cc
  distance_test_common.cc
  distance_rmsd_test.cc
  superpose_frames.cc
  filter_clashes.cc
  construct_cbeta.cc
  trajectory_analysis.cc
  structure_analysis.cc
  consistency_checks.cc
  pdbize.cc
)

set(MOL_ALG_DEPS ost_mol ost_seq)

if (ENABLE_IMG)
  set(OST_MOL_ALG_HEADERS
    ${OST_MOL_ALG_HEADERS}
    entity_to_density.hh
  )

  set(OST_MOL_ALG_SOURCES
    ${OST_MOL_ALG_SOURCES}
    entity_to_density.cc
  )

  set(MOL_ALG_DEPS ${MOL_ALG_DEPS} ost_img ost_img_alg)
endif()

executable(NAME lddt SOURCES lddt.cc 
           DEPENDS_ON  ost_mol ost_mol_alg ost_io STATIC)

module(NAME mol_alg SOURCES ${OST_MOL_ALG_SOURCES}
       HEADERS ${OST_MOL_ALG_HEADERS}
       HEADER_OUTPUT_DIR ost/mol/alg
       DEPENDS_ON ${MOL_ALG_DEPS}
       LINK ${BOOST_PROGRAM_OPTIONS})

copy_if_different("${CMAKE_CURRENT_SOURCE_DIR}" "${STAGE_DIR}/share/openstructure"
                  "atom_scattering_properties.txt" "ATOM_SCATTERING_PROPS"
                  "ost_mol_alg")
install(FILES "atom_scattering_properties.txt" DESTINATION "share/openstructure/")

copy_if_different("${CMAKE_CURRENT_SOURCE_DIR}" "${STAGE_DIR}/share/openstructure"
                  "stereo_chemical_props.txt" "STEREO_CHEMICAL_PROPS"
                  "ost_mol_alg")
install(FILES "stereo_chemical_props.txt" DESTINATION "share/openstructure/")
