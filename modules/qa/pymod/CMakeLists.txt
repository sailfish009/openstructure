set(OST_QA_PYMOD_SOURCES
  export_interaction.cc
  export_torsion.cc
  export_packing.cc
  export_reduced.cc
  export_utilities.cc
  wrap_qa.cc
  export_clash.cc
)
set(OST_QA_PYMOD_MODULES
__init__.py
)
pymod(NAME qa CPP ${OST_QA_PYMOD_SOURCES} PY ${OST_QA_PYMOD_MODULES})
