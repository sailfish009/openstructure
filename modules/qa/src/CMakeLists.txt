set(OST_QA_HEADERS
histogram.hh
index.hh
multi_classifier.hh
amino_acids.hh
torsion_statistics.hh
torsion_potential.hh
solis_torsion_statistics.hh
solis_torsion_potential.hh
interaction_statistics.hh
all_atom_potential.hh
packing_statistics.hh
atom_types.hh
packing_potential.hh
clash_score.hh
reduced_statistics.hh
reduced_potential.hh
module_config.hh
)

set(OST_QA_SOURCES
reduced_statistics.cc
reduced_potential.cc
torsion_statistics.cc
torsion_potential.cc
solis_torsion_statistics.cc
solis_torsion_potential.cc
interaction_statistics.cc
packing_statistics.cc
amino_acids.cc
atom_types.cc
all_atom_potential.cc
packing_potential.cc
clash_score.cc
impl/reduced_impl.cc
)


module(NAME qa SOURCES ${OST_QA_SOURCES}
       HEADERS reduced_impl.hh IN_DIR impl ${OST_QA_HEADERS} DEPENDS_ON io)

