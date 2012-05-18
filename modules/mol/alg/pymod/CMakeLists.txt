set(OST_MOL_ALG_PYMOD_SOURCES
  wrap_mol_alg.cc
  export_svd_superpose.cc
  export_clash.cc
  export_trajectory_analysis.cc
  export_structure_analysis.cc
)

set(OST_MOL_ALG_PYMOD_MODULES
  "__init__.py"
  views.py
  superpose.py
  trajectory_analysis.py
  structure_analysis.py
)

if (ENABLE_IMG)

  set(OST_MOL_ALG_PYMOD_SOURCES
    ${OST_MOL_ALG_PYMOD_SOURCES}
    export_entity_to_density.cc
  )

endif()
if (NOT ENABLE_STATIC)
  pymod(NAME mol_alg OUTPUT_DIR ost/mol/alg CPP ${OST_MOL_ALG_PYMOD_SOURCES}
        PY ${OST_MOL_ALG_PYMOD_MODULES})
endif()
