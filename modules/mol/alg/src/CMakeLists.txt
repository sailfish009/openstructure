set(OST_MOL_ALG_HEADERS
  svd_superpose.hh
  module_config.hh
  sec_structure_segments.hh
  local_dist_test.hh
  superpose_frames.hh
  filter_clashes.hh
  construct_cbeta.hh
  clash_score.hh
)

set(OST_MOL_ALG_SOURCES
  svd_superpose.cc
  clash_score.cc
  sec_structure_segments.cc
  local_dist_test.cc
  superpose_frames.cc
  filter_clashes.cc
  construct_cbeta.cc
)

set(MOL_ALG_DEPS mol seq)

if (ENABLE_IMG)
  set(OST_MOL_ALG_HEADERS
    ${OST_MOL_ALG_HEADERS}
    entity_to_density.hh
  )

  set(OST_MOL_ALG_SOURCES
    ${OST_MOL_ALG_SOURCES}
    entity_to_density.cc
  )

  set(MOL_ALG_DEPS ${MOL_ALG_DEPS} img img_alg)
endif()

executable(NAME ldt SOURCES ldt.cc 
           DEPENDS_ON io mol_alg STATIC)

module(NAME mol_alg SOURCES ${OST_MOL_ALG_SOURCES}
       HEADERS ${OST_MOL_ALG_HEADERS}
       HEADER_OUTPUT_DIR ost/mol/alg
       DEPENDS_ON ${MOL_ALG_DEPS}
       LINK ${BOOST_PROGRAM_OPTIONS})

copy_if_different("." "${STAGE_DIR}/share/openstructure"
                  "atom_scattering_properties.txt" "ATOM_SCATTERING_PROPS"
                  "ost_mol_alg")
