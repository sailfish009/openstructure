set(OST_MOL_ALG_HEADERS
  svd_superpose.hh
  module_config.hh
  sec_structure_segments.hh
)

set(OST_MOL_ALG_SOURCES
  svd_superpose.cc
  sec_structure_segments.cc
)

set(MOL_ALG_DEPS mol)

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

module(NAME mol_alg SOURCES ${OST_MOL_ALG_SOURCES}
       HEADERS ${OST_MOL_ALG_HEADERS}
       HEADER_OUTPUT_DIR ost/mol/alg
       DEPENDS_ON ${MOL_ALG_DEPS})

copy_if_different("." "${STAGE_DIR}/share/openstructure"
                  "atom_scattering_properties.txt" "ATOM_SCATTERING_PROPS"
                  "ost_mol_alg")
