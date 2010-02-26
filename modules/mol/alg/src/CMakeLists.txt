set(OST_MOL_ALG_HEADERS
  svd_superpose.hh
  module_config.hh
  sec_structure_segments.hh
)

set(OST_MOL_ALG_SOURCES
  svd_superpose.cc
  sec_structure_segments.cc
)

module(NAME mol_alg SOURCES ${OST_MOL_ALG_SOURCES}
       HEADERS ${OST_MOL_ALG_HEADERS}
       HEADER_OUTPUT_DIR ost/mol/alg
       DEPENDS_ON mol)

